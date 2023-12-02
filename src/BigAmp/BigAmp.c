#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define BIGAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/bigamp"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	TONE = 5,
	OUTPUT = 6,
	DRY_WET = 7
} PortIndex;

enum {
	fix_freq,
	fix_reso,
	fix_a0,
	fix_a1,
	fix_a2,
	fix_b1,
	fix_b2,
	fix_sL1,
	fix_sL2,
	fix_sR1,
	fix_sR2,
	fix_total
}; // fixed frequency biquad filter for ultrasonics, stereo

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* gain;
	const float* tone;
	const float* outputGain;
	const float* dryWet;

	double lastSampleL;
	double storeSampleL;
	double lastSlewL;
	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double iirSampleIL;
	double iirSampleJL;
	double OddL[257];
	double EvenL[257]; // amp

	double lastSampleR;
	double storeSampleR;
	double lastSlewR;
	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double iirSampleGR;
	double iirSampleHR;
	double iirSampleIR;
	double iirSampleJR;
	double OddR[257];
	double EvenR[257];

	bool flip;
	int count; // amp

	double bL[90];
	double lastCabSampleL;
	double smoothCabAL;
	double smoothCabBL; // cab

	double bR[90];
	double lastCabSampleR;
	double smoothCabAR;
	double smoothCabBR; // cab

	double lastRefL[10];
	double lastRefR[10];
	int cycle; // undersampling

	double fixA[fix_total];
	double fixB[fix_total];
	double fixC[fix_total];
	double fixD[fix_total];
	double fixE[fix_total];
	double fixF[fix_total]; // filtering

	uint32_t fpdL;
	uint32_t fpdR;
} BigAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BigAmp* bigAmp = (BigAmp*) calloc(1, sizeof(BigAmp));
	bigAmp->sampleRate = rate;
	return (LV2_Handle) bigAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BigAmp* bigAmp = (BigAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			bigAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			bigAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			bigAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			bigAmp->output[1] = (float*) data;
			break;
		case GAIN:
			bigAmp->gain = (const float*) data;
			break;
		case TONE:
			bigAmp->tone = (const float*) data;
			break;
		case OUTPUT:
			bigAmp->outputGain = (const float*) data;
			break;
		case DRY_WET:
			bigAmp->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	BigAmp* bigAmp = (BigAmp*) instance;

	bigAmp->lastSampleL = 0.0;
	bigAmp->storeSampleL = 0.0;
	bigAmp->lastSlewL = 0.0;
	bigAmp->iirSampleAL = 0.0;
	bigAmp->iirSampleBL = 0.0;
	bigAmp->iirSampleCL = 0.0;
	bigAmp->iirSampleDL = 0.0;
	bigAmp->iirSampleEL = 0.0;
	bigAmp->iirSampleFL = 0.0;
	bigAmp->iirSampleGL = 0.0;
	bigAmp->iirSampleHL = 0.0;
	bigAmp->iirSampleIL = 0.0;
	bigAmp->iirSampleJL = 0.0;

	bigAmp->lastSampleR = 0.0;
	bigAmp->storeSampleR = 0.0;
	bigAmp->lastSlewR = 0.0;
	bigAmp->iirSampleAR = 0.0;
	bigAmp->iirSampleBR = 0.0;
	bigAmp->iirSampleCR = 0.0;
	bigAmp->iirSampleDR = 0.0;
	bigAmp->iirSampleER = 0.0;
	bigAmp->iirSampleFR = 0.0;
	bigAmp->iirSampleGR = 0.0;
	bigAmp->iirSampleHR = 0.0;
	bigAmp->iirSampleIR = 0.0;
	bigAmp->iirSampleJR = 0.0;

	for (int fcount = 0; fcount < 257; fcount++) {
		bigAmp->OddL[fcount] = 0.0;
		bigAmp->EvenL[fcount] = 0.0;
		bigAmp->OddR[fcount] = 0.0;
		bigAmp->EvenR[fcount] = 0.0;
	}

	bigAmp->count = 0;
	bigAmp->flip = false; // amp

	for (int fcount = 0; fcount < 90; fcount++) {
		bigAmp->bL[fcount] = 0;
		bigAmp->bR[fcount] = 0;
	}
	bigAmp->smoothCabAL = 0.0;
	bigAmp->smoothCabBL = 0.0;
	bigAmp->lastCabSampleL = 0.0; // cab
	bigAmp->smoothCabAR = 0.0;
	bigAmp->smoothCabBR = 0.0;
	bigAmp->lastCabSampleR = 0.0; // cab

	for (int fcount = 0; fcount < 9; fcount++) {
		bigAmp->lastRefL[fcount] = 0.0;
		bigAmp->lastRefR[fcount] = 0.0;
	}
	bigAmp->cycle = 0; // undersampling

	for (int x = 0; x < fix_total; x++) {
		bigAmp->fixA[x] = 0.0;
		bigAmp->fixB[x] = 0.0;
		bigAmp->fixC[x] = 0.0;
		bigAmp->fixD[x] = 0.0;
		bigAmp->fixE[x] = 0.0;
		bigAmp->fixF[x] = 0.0;
	} // filtering

	bigAmp->fpdL = 1.0;
	while (bigAmp->fpdL < 16386) bigAmp->fpdL = rand() * UINT32_MAX;
	bigAmp->fpdR = 1.0;
	while (bigAmp->fpdR < 16386) bigAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BigAmp* bigAmp = (BigAmp*) instance;

	const float* in1 = bigAmp->input[0];
	const float* in2 = bigAmp->input[1];
	float* out1 = bigAmp->output[0];
	float* out2 = bigAmp->output[1];

	const double sampleRate = bigAmp->sampleRate;
	const float tone = *bigAmp->tone;

	double bassfill = *bigAmp->gain;
	double basstrim = tone;
	double outputlevel = *bigAmp->outputGain;
	double wet = *bigAmp->dryWet;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (bigAmp->cycle > cycleEnd - 1) bigAmp->cycle = cycleEnd - 1; // sanity check

	double bleed = outputlevel / 16.0;
	double inputlevel = bassfill * 3.0;

	double samplerate = sampleRate;
	double EQ = (tone / samplerate) * 22050.0;
	double BEQ = (bleed / samplerate) * 22050.0;

	int diagonal = (int) (0.000861678 * samplerate);
	if (diagonal > 127) diagonal = 127;
	int side = (int) (diagonal / 1.4142135623730951);
	int down = (side + diagonal) / 2;
	// now we've got down, side and diagonal as offsets and we also use three successive samples upfront

	double cutoff = (15000.0 + (tone * 10000.0)) / sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low

	bigAmp->fixF[fix_freq] = bigAmp->fixE[fix_freq] = bigAmp->fixD[fix_freq] = bigAmp->fixC[fix_freq] = bigAmp->fixB[fix_freq] = bigAmp->fixA[fix_freq] = cutoff;

	bigAmp->fixA[fix_reso] = 4.46570214;
	bigAmp->fixB[fix_reso] = 1.51387132;
	bigAmp->fixC[fix_reso] = 0.93979296;
	bigAmp->fixD[fix_reso] = 0.70710678;
	bigAmp->fixE[fix_reso] = 0.52972649;
	bigAmp->fixF[fix_reso] = 0.50316379;

	double K = tan(M_PI * bigAmp->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / bigAmp->fixA[fix_reso] + K * K);
	bigAmp->fixA[fix_a0] = K * K * norm;
	bigAmp->fixA[fix_a1] = 2.0 * bigAmp->fixA[fix_a0];
	bigAmp->fixA[fix_a2] = bigAmp->fixA[fix_a0];
	bigAmp->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	bigAmp->fixA[fix_b2] = (1.0 - K / bigAmp->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * bigAmp->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / bigAmp->fixB[fix_reso] + K * K);
	bigAmp->fixB[fix_a0] = K * K * norm;
	bigAmp->fixB[fix_a1] = 2.0 * bigAmp->fixB[fix_a0];
	bigAmp->fixB[fix_a2] = bigAmp->fixB[fix_a0];
	bigAmp->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	bigAmp->fixB[fix_b2] = (1.0 - K / bigAmp->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * bigAmp->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / bigAmp->fixC[fix_reso] + K * K);
	bigAmp->fixC[fix_a0] = K * K * norm;
	bigAmp->fixC[fix_a1] = 2.0 * bigAmp->fixC[fix_a0];
	bigAmp->fixC[fix_a2] = bigAmp->fixC[fix_a0];
	bigAmp->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	bigAmp->fixC[fix_b2] = (1.0 - K / bigAmp->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * bigAmp->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / bigAmp->fixD[fix_reso] + K * K);
	bigAmp->fixD[fix_a0] = K * K * norm;
	bigAmp->fixD[fix_a1] = 2.0 * bigAmp->fixD[fix_a0];
	bigAmp->fixD[fix_a2] = bigAmp->fixD[fix_a0];
	bigAmp->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	bigAmp->fixD[fix_b2] = (1.0 - K / bigAmp->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * bigAmp->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / bigAmp->fixE[fix_reso] + K * K);
	bigAmp->fixE[fix_a0] = K * K * norm;
	bigAmp->fixE[fix_a1] = 2.0 * bigAmp->fixE[fix_a0];
	bigAmp->fixE[fix_a2] = bigAmp->fixE[fix_a0];
	bigAmp->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	bigAmp->fixE[fix_b2] = (1.0 - K / bigAmp->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * bigAmp->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / bigAmp->fixF[fix_reso] + K * K);
	bigAmp->fixF[fix_a0] = K * K * norm;
	bigAmp->fixF[fix_a1] = 2.0 * bigAmp->fixF[fix_a0];
	bigAmp->fixF[fix_a2] = bigAmp->fixF[fix_a0];
	bigAmp->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	bigAmp->fixF[fix_b2] = (1.0 - K / bigAmp->fixF[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = bigAmp->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = bigAmp->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double outSample = (inputSampleL * bigAmp->fixA[fix_a0]) + bigAmp->fixA[fix_sL1];
		bigAmp->fixA[fix_sL1] = (inputSampleL * bigAmp->fixA[fix_a1]) - (outSample * bigAmp->fixA[fix_b1]) + bigAmp->fixA[fix_sL2];
		bigAmp->fixA[fix_sL2] = (inputSampleL * bigAmp->fixA[fix_a2]) - (outSample * bigAmp->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * bigAmp->fixA[fix_a0]) + bigAmp->fixA[fix_sR1];
		bigAmp->fixA[fix_sR1] = (inputSampleR * bigAmp->fixA[fix_a1]) - (outSample * bigAmp->fixA[fix_b1]) + bigAmp->fixA[fix_sR2];
		bigAmp->fixA[fix_sR2] = (inputSampleR * bigAmp->fixA[fix_a2]) - (outSample * bigAmp->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		double skewL = (inputSampleL - bigAmp->lastSampleL);
		bigAmp->lastSampleL = inputSampleL;
		// skew will be direction/angle
		double bridgerectifier = fabs(skewL);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewL > 0) skewL = bridgerectifier;
		else skewL = -bridgerectifier;
		// skew is now sined and clamped and then re-amplified again
		skewL *= inputSampleL;
		skewL = (skewL + (skewL * basstrim)) / 2.0;
		inputSampleL *= basstrim;

		double skewR = (inputSampleR - bigAmp->lastSampleR);
		bigAmp->lastSampleR = inputSampleR;
		// skew will be direction/angle
		bridgerectifier = fabs(skewR);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewR > 0) skewR = bridgerectifier;
		else skewR = -bridgerectifier;
		// skew is now sined and clamped and then re-amplified again
		skewR *= inputSampleR;
		skewR = (skewR + (skewR * basstrim)) / 2.0;
		inputSampleR *= basstrim;
		double basscut = basstrim;
		// we're going to be shifting this as the stages progress

		outSample = (inputSampleL * bigAmp->fixB[fix_a0]) + bigAmp->fixB[fix_sL1];
		bigAmp->fixB[fix_sL1] = (inputSampleL * bigAmp->fixB[fix_a1]) - (outSample * bigAmp->fixB[fix_b1]) + bigAmp->fixB[fix_sL2];
		bigAmp->fixB[fix_sL2] = (inputSampleL * bigAmp->fixB[fix_a2]) - (outSample * bigAmp->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * bigAmp->fixB[fix_a0]) + bigAmp->fixB[fix_sR1];
		bigAmp->fixB[fix_sR1] = (inputSampleR * bigAmp->fixB[fix_a1]) - (outSample * bigAmp->fixB[fix_b1]) + bigAmp->fixB[fix_sR2];
		bigAmp->fixB[fix_sR2] = (inputSampleR * bigAmp->fixB[fix_a2]) - (outSample * bigAmp->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		double offsetL = (1.0 - EQ) + (fabs(inputSampleL) * EQ);
		if (offsetL < 0.0) offsetL = 0.0;
		if (offsetL > 1.0) offsetL = 1.0;
		bigAmp->iirSampleAL = (bigAmp->iirSampleAL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - (bigAmp->iirSampleAL * basscut);
		// highpass
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		bigAmp->iirSampleBL = (bigAmp->iirSampleBL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - (bigAmp->iirSampleBL * basscut);
		// highpass. Use offset from before gain stage
		// finished first gain stage

		inputSampleR *= inputlevel;
		double offsetR = (1.0 - EQ) + (fabs(inputSampleR) * EQ);
		if (offsetR < 0.0) offsetR = 0.0;
		if (offsetR > 1.0) offsetR = 1.0;
		bigAmp->iirSampleAR = (bigAmp->iirSampleAR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - (bigAmp->iirSampleAR * basscut);
		// highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// overdrive
		bigAmp->iirSampleBR = (bigAmp->iirSampleBR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - (bigAmp->iirSampleBR * basscut);
		basscut /= 2.0;
		// highpass. Use offset from before gain stage
		// finished first gain stage

		outSample = (inputSampleL * bigAmp->fixC[fix_a0]) + bigAmp->fixC[fix_sL1];
		bigAmp->fixC[fix_sL1] = (inputSampleL * bigAmp->fixC[fix_a1]) - (outSample * bigAmp->fixC[fix_b1]) + bigAmp->fixC[fix_sL2];
		bigAmp->fixC[fix_sL2] = (inputSampleL * bigAmp->fixC[fix_a2]) - (outSample * bigAmp->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * bigAmp->fixC[fix_a0]) + bigAmp->fixC[fix_sR1];
		bigAmp->fixC[fix_sR1] = (inputSampleR * bigAmp->fixC[fix_a1]) - (outSample * bigAmp->fixC[fix_b1]) + bigAmp->fixC[fix_sR2];
		bigAmp->fixC[fix_sR2] = (inputSampleR * bigAmp->fixC[fix_a2]) - (outSample * bigAmp->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		skewL /= 2.0;
		offsetL = (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		bigAmp->iirSampleCL = (bigAmp->iirSampleCL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - (bigAmp->iirSampleCL * basscut);
		// highpass.
		bigAmp->iirSampleDL = (bigAmp->iirSampleDL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = bigAmp->iirSampleDL;
		// lowpass. Use offset from before gain stage

		inputSampleR *= inputlevel;
		offsetR = (1.0 + offsetR) / 2.0;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// overdrive
		bigAmp->iirSampleCR = (bigAmp->iirSampleCR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - (bigAmp->iirSampleCR * basscut);
		basscut /= 2.0;
		// highpass.
		bigAmp->iirSampleDR = (bigAmp->iirSampleDR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = bigAmp->iirSampleDR;
		// lowpass. Use offset from before gain stage

		outSample = (inputSampleL * bigAmp->fixD[fix_a0]) + bigAmp->fixD[fix_sL1];
		bigAmp->fixD[fix_sL1] = (inputSampleL * bigAmp->fixD[fix_a1]) - (outSample * bigAmp->fixD[fix_b1]) + bigAmp->fixD[fix_sL2];
		bigAmp->fixD[fix_sL2] = (inputSampleL * bigAmp->fixD[fix_a2]) - (outSample * bigAmp->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * bigAmp->fixD[fix_a0]) + bigAmp->fixD[fix_sR1];
		bigAmp->fixD[fix_sR1] = (inputSampleR * bigAmp->fixD[fix_a1]) - (outSample * bigAmp->fixD[fix_b1]) + bigAmp->fixD[fix_sR2];
		bigAmp->fixD[fix_sR2] = (inputSampleR * bigAmp->fixD[fix_a2]) - (outSample * bigAmp->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		skewL /= 2.0;
		offsetL = (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		bigAmp->iirSampleEL = (bigAmp->iirSampleEL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - (bigAmp->iirSampleEL * basscut);
		// we don't need to do basscut again, that was the last one
		// highpass.
		bigAmp->iirSampleFL = (bigAmp->iirSampleFL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = bigAmp->iirSampleFL;
		// lowpass. Use offset from before gain stage

		inputSampleR *= inputlevel;
		skewR /= 2.0;
		offsetR = (1.0 + offsetR) / 2.0;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// overdrive
		bigAmp->iirSampleER = (bigAmp->iirSampleER * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - (bigAmp->iirSampleER * basscut);
		// we don't need to do basscut again, that was the last one
		// highpass.
		bigAmp->iirSampleFR = (bigAmp->iirSampleFR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = bigAmp->iirSampleFR;
		// lowpass. Use offset from before gain stage

		outSample = (inputSampleL * bigAmp->fixE[fix_a0]) + bigAmp->fixE[fix_sL1];
		bigAmp->fixE[fix_sL1] = (inputSampleL * bigAmp->fixE[fix_a1]) - (outSample * bigAmp->fixE[fix_b1]) + bigAmp->fixE[fix_sL2];
		bigAmp->fixE[fix_sL2] = (inputSampleL * bigAmp->fixE[fix_a2]) - (outSample * bigAmp->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * bigAmp->fixE[fix_a0]) + bigAmp->fixE[fix_sR1];
		bigAmp->fixE[fix_sR1] = (inputSampleR * bigAmp->fixE[fix_a1]) - (outSample * bigAmp->fixE[fix_b1]) + bigAmp->fixE[fix_sR2];
		bigAmp->fixE[fix_sR2] = (inputSampleR * bigAmp->fixE[fix_a2]) - (outSample * bigAmp->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		skewL /= 2.0;
		offsetL = (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// output stage has less gain, no highpass, straight lowpass
		bigAmp->iirSampleGL = (bigAmp->iirSampleGL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = bigAmp->iirSampleGL;
		// lowpass. Use offset from before gain stage
		bigAmp->iirSampleHL = (bigAmp->iirSampleHL * (1.0 - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		// extra lowpass for 4*12" speakers

		inputSampleR *= inputlevel;
		skewR /= 2.0;
		offsetR = (1.0 + offsetR) / 2.0;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// output stage has less gain, no highpass, straight lowpass
		bigAmp->iirSampleGR = (bigAmp->iirSampleGR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = bigAmp->iirSampleGR;
		// lowpass. Use offset from before gain stage
		bigAmp->iirSampleHR = (bigAmp->iirSampleHR * (1.0 - (offsetR * BEQ))) + (inputSampleR * (offsetR * BEQ));
		// extra lowpass for 4*12" speakers

		if (bigAmp->count < 0 || bigAmp->count > 128) bigAmp->count = 128;
		double resultBL = 0.0;
		double resultBR = 0.0;
		if (bigAmp->flip) {
			bigAmp->OddL[bigAmp->count + 128] = bigAmp->OddL[bigAmp->count] = bigAmp->iirSampleHL;
			resultBL = (bigAmp->OddL[bigAmp->count + down] + bigAmp->OddL[bigAmp->count + side] + bigAmp->OddL[bigAmp->count + diagonal]);
			bigAmp->OddR[bigAmp->count + 128] = bigAmp->OddR[bigAmp->count] = bigAmp->iirSampleHR;
			resultBR = (bigAmp->OddR[bigAmp->count + down] + bigAmp->OddR[bigAmp->count + side] + bigAmp->OddR[bigAmp->count + diagonal]);
		} else {
			bigAmp->EvenL[bigAmp->count + 128] = bigAmp->EvenL[bigAmp->count] = bigAmp->iirSampleHL;
			resultBL = (bigAmp->EvenL[bigAmp->count + down] + bigAmp->EvenL[bigAmp->count + side] + bigAmp->EvenL[bigAmp->count + diagonal]);
			bigAmp->EvenR[bigAmp->count + 128] = bigAmp->EvenR[bigAmp->count] = bigAmp->iirSampleHR;
			resultBR = (bigAmp->EvenR[bigAmp->count + down] + bigAmp->EvenR[bigAmp->count + side] + bigAmp->EvenR[bigAmp->count + diagonal]);
		}
		bigAmp->count--;

		bigAmp->iirSampleIL = (bigAmp->iirSampleIL * (1.0 - (offsetL * BEQ))) + (resultBL * (offsetL * BEQ));
		inputSampleL += (bigAmp->iirSampleIL * bleed);
		// extra lowpass for 4*12" speakers
		bigAmp->iirSampleJL = (bigAmp->iirSampleJL * (1.0 - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		inputSampleL += (bigAmp->iirSampleJL * bassfill);
		inputSampleL = sin(inputSampleL * outputlevel);
		double randy = (((double) bigAmp->fpdL / UINT32_MAX) * 0.04);
		inputSampleL = ((inputSampleL * (1 - randy)) + (bigAmp->storeSampleL * randy)) * outputlevel;
		bigAmp->storeSampleL = inputSampleL;

		bigAmp->iirSampleIR = (bigAmp->iirSampleIR * (1.0 - (offsetR * BEQ))) + (resultBR * (offsetR * BEQ));
		inputSampleR += (bigAmp->iirSampleIR * bleed);
		// extra lowpass for 4*12" speakers
		bigAmp->iirSampleJR = (bigAmp->iirSampleJR * (1.0 - (offsetR * BEQ))) + (inputSampleR * (offsetR * BEQ));
		inputSampleR += (bigAmp->iirSampleJR * bassfill);
		inputSampleR = sin(inputSampleR * outputlevel);
		randy = (((double) bigAmp->fpdR / UINT32_MAX) * 0.04);
		inputSampleR = ((inputSampleR * (1 - randy)) + (bigAmp->storeSampleR * randy)) * outputlevel;
		bigAmp->storeSampleR = inputSampleR;

		outSample = (inputSampleL * bigAmp->fixF[fix_a0]) + bigAmp->fixF[fix_sL1];
		bigAmp->fixF[fix_sL1] = (inputSampleL * bigAmp->fixF[fix_a1]) - (outSample * bigAmp->fixF[fix_b1]) + bigAmp->fixF[fix_sL2];
		bigAmp->fixF[fix_sL2] = (inputSampleL * bigAmp->fixF[fix_a2]) - (outSample * bigAmp->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * bigAmp->fixF[fix_a0]) + bigAmp->fixF[fix_sR1];
		bigAmp->fixF[fix_sR1] = (inputSampleR * bigAmp->fixF[fix_a1]) - (outSample * bigAmp->fixF[fix_b1]) + bigAmp->fixF[fix_sR2];
		bigAmp->fixF[fix_sR2] = (inputSampleR * bigAmp->fixF[fix_a2]) - (outSample * bigAmp->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		bigAmp->flip = !bigAmp->flip;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider
		// amp

		bigAmp->cycle++;
		if (bigAmp->cycle == cycleEnd) {

			double temp = (inputSampleL + bigAmp->smoothCabAL) / 3.0;
			bigAmp->smoothCabAL = inputSampleL;
			inputSampleL = temp;

			bigAmp->bL[81] = bigAmp->bL[80];
			bigAmp->bL[80] = bigAmp->bL[79];
			bigAmp->bL[79] = bigAmp->bL[78];
			bigAmp->bL[78] = bigAmp->bL[77];
			bigAmp->bL[77] = bigAmp->bL[76];
			bigAmp->bL[76] = bigAmp->bL[75];
			bigAmp->bL[75] = bigAmp->bL[74];
			bigAmp->bL[74] = bigAmp->bL[73];
			bigAmp->bL[73] = bigAmp->bL[72];
			bigAmp->bL[72] = bigAmp->bL[71];
			bigAmp->bL[71] = bigAmp->bL[70];
			bigAmp->bL[70] = bigAmp->bL[69];
			bigAmp->bL[69] = bigAmp->bL[68];
			bigAmp->bL[68] = bigAmp->bL[67];
			bigAmp->bL[67] = bigAmp->bL[66];
			bigAmp->bL[66] = bigAmp->bL[65];
			bigAmp->bL[65] = bigAmp->bL[64];
			bigAmp->bL[64] = bigAmp->bL[63];
			bigAmp->bL[63] = bigAmp->bL[62];
			bigAmp->bL[62] = bigAmp->bL[61];
			bigAmp->bL[61] = bigAmp->bL[60];
			bigAmp->bL[60] = bigAmp->bL[59];
			bigAmp->bL[59] = bigAmp->bL[58];
			bigAmp->bL[58] = bigAmp->bL[57];
			bigAmp->bL[57] = bigAmp->bL[56];
			bigAmp->bL[56] = bigAmp->bL[55];
			bigAmp->bL[55] = bigAmp->bL[54];
			bigAmp->bL[54] = bigAmp->bL[53];
			bigAmp->bL[53] = bigAmp->bL[52];
			bigAmp->bL[52] = bigAmp->bL[51];
			bigAmp->bL[51] = bigAmp->bL[50];
			bigAmp->bL[50] = bigAmp->bL[49];
			bigAmp->bL[49] = bigAmp->bL[48];
			bigAmp->bL[48] = bigAmp->bL[47];
			bigAmp->bL[47] = bigAmp->bL[46];
			bigAmp->bL[46] = bigAmp->bL[45];
			bigAmp->bL[45] = bigAmp->bL[44];
			bigAmp->bL[44] = bigAmp->bL[43];
			bigAmp->bL[43] = bigAmp->bL[42];
			bigAmp->bL[42] = bigAmp->bL[41];
			bigAmp->bL[41] = bigAmp->bL[40];
			bigAmp->bL[40] = bigAmp->bL[39];
			bigAmp->bL[39] = bigAmp->bL[38];
			bigAmp->bL[38] = bigAmp->bL[37];
			bigAmp->bL[37] = bigAmp->bL[36];
			bigAmp->bL[36] = bigAmp->bL[35];
			bigAmp->bL[35] = bigAmp->bL[34];
			bigAmp->bL[34] = bigAmp->bL[33];
			bigAmp->bL[33] = bigAmp->bL[32];
			bigAmp->bL[32] = bigAmp->bL[31];
			bigAmp->bL[31] = bigAmp->bL[30];
			bigAmp->bL[30] = bigAmp->bL[29];
			bigAmp->bL[29] = bigAmp->bL[28];
			bigAmp->bL[28] = bigAmp->bL[27];
			bigAmp->bL[27] = bigAmp->bL[26];
			bigAmp->bL[26] = bigAmp->bL[25];
			bigAmp->bL[25] = bigAmp->bL[24];
			bigAmp->bL[24] = bigAmp->bL[23];
			bigAmp->bL[23] = bigAmp->bL[22];
			bigAmp->bL[22] = bigAmp->bL[21];
			bigAmp->bL[21] = bigAmp->bL[20];
			bigAmp->bL[20] = bigAmp->bL[19];
			bigAmp->bL[19] = bigAmp->bL[18];
			bigAmp->bL[18] = bigAmp->bL[17];
			bigAmp->bL[17] = bigAmp->bL[16];
			bigAmp->bL[16] = bigAmp->bL[15];
			bigAmp->bL[15] = bigAmp->bL[14];
			bigAmp->bL[14] = bigAmp->bL[13];
			bigAmp->bL[13] = bigAmp->bL[12];
			bigAmp->bL[12] = bigAmp->bL[11];
			bigAmp->bL[11] = bigAmp->bL[10];
			bigAmp->bL[10] = bigAmp->bL[9];
			bigAmp->bL[9] = bigAmp->bL[8];
			bigAmp->bL[8] = bigAmp->bL[7];
			bigAmp->bL[7] = bigAmp->bL[6];
			bigAmp->bL[6] = bigAmp->bL[5];
			bigAmp->bL[5] = bigAmp->bL[4];
			bigAmp->bL[4] = bigAmp->bL[3];
			bigAmp->bL[3] = bigAmp->bL[2];
			bigAmp->bL[2] = bigAmp->bL[1];
			bigAmp->bL[1] = bigAmp->bL[0];
			bigAmp->bL[0] = inputSampleL;

			inputSampleL += (bigAmp->bL[1] * (1.35472031405494242 + (0.00220914099195157 * fabs(bigAmp->bL[1]))));
			inputSampleL += (bigAmp->bL[2] * (1.63534207755253003 - (0.11406232654509685 * fabs(bigAmp->bL[2]))));
			inputSampleL += (bigAmp->bL[3] * (1.82334575691525869 - (0.42647194712964054 * fabs(bigAmp->bL[3]))));
			inputSampleL += (bigAmp->bL[4] * (1.86156386235405868 - (0.76744187887586590 * fabs(bigAmp->bL[4]))));
			inputSampleL += (bigAmp->bL[5] * (1.67332739338852599 - (0.95161997324293013 * fabs(bigAmp->bL[5]))));
			inputSampleL += (bigAmp->bL[6] * (1.25054130794899021 - (0.98410433514572859 * fabs(bigAmp->bL[6]))));
			inputSampleL += (bigAmp->bL[7] * (0.70049121047281737 - (0.87375612110718992 * fabs(bigAmp->bL[7]))));
			inputSampleL += (bigAmp->bL[8] * (0.15291791448081560 - (0.61195266024519046 * fabs(bigAmp->bL[8]))));
			inputSampleL -= (bigAmp->bL[9] * (0.37301992914152693 + (0.16755422915252094 * fabs(bigAmp->bL[9]))));
			inputSampleL -= (bigAmp->bL[10] * (0.76568539228498433 - (0.28554435228965386 * fabs(bigAmp->bL[10]))));
			inputSampleL -= (bigAmp->bL[11] * (0.95726568749937369 - (0.61659719162806048 * fabs(bigAmp->bL[11]))));
			inputSampleL -= (bigAmp->bL[12] * (1.01273552193911032 - (0.81827288407943954 * fabs(bigAmp->bL[12]))));
			inputSampleL -= (bigAmp->bL[13] * (0.93920108117234447 - (0.80077111864205874 * fabs(bigAmp->bL[13]))));
			inputSampleL -= (bigAmp->bL[14] * (0.79831898832953974 - (0.65814750339694406 * fabs(bigAmp->bL[14]))));
			inputSampleL -= (bigAmp->bL[15] * (0.64200088100452313 - (0.46135833001232618 * fabs(bigAmp->bL[15]))));
			inputSampleL -= (bigAmp->bL[16] * (0.48807302802822128 - (0.15506178974799034 * fabs(bigAmp->bL[16]))));
			inputSampleL -= (bigAmp->bL[17] * (0.36545171501947982 + (0.16126103769376721 * fabs(bigAmp->bL[17]))));
			inputSampleL -= (bigAmp->bL[18] * (0.31469581455759105 + (0.32250870039053953 * fabs(bigAmp->bL[18]))));
			inputSampleL -= (bigAmp->bL[19] * (0.36893534817945800 + (0.25409418897237473 * fabs(bigAmp->bL[19]))));
			inputSampleL -= (bigAmp->bL[20] * (0.41092557722975687 + (0.13114730488878301 * fabs(bigAmp->bL[20]))));
			inputSampleL -= (bigAmp->bL[21] * (0.38584044480710594 + (0.06825323739722661 * fabs(bigAmp->bL[21]))));
			inputSampleL -= (bigAmp->bL[22] * (0.33378434007178670 + (0.04144255489164217 * fabs(bigAmp->bL[22]))));
			inputSampleL -= (bigAmp->bL[23] * (0.26144203061699706 + (0.06031313105098152 * fabs(bigAmp->bL[23]))));
			inputSampleL -= (bigAmp->bL[24] * (0.25818342000920502 + (0.03642289242586355 * fabs(bigAmp->bL[24]))));
			inputSampleL -= (bigAmp->bL[25] * (0.28096018498822661 + (0.00976973667327174 * fabs(bigAmp->bL[25]))));
			inputSampleL -= (bigAmp->bL[26] * (0.25845682019095384 + (0.02749015358080831 * fabs(bigAmp->bL[26]))));
			inputSampleL -= (bigAmp->bL[27] * (0.26655607865953096 - (0.00329839675455690 * fabs(bigAmp->bL[27]))));
			inputSampleL -= (bigAmp->bL[28] * (0.30590085026938518 - (0.07375043215328811 * fabs(bigAmp->bL[28]))));
			inputSampleL -= (bigAmp->bL[29] * (0.32875683916470899 - (0.12454134857516502 * fabs(bigAmp->bL[29]))));
			inputSampleL -= (bigAmp->bL[30] * (0.38166643180506560 - (0.19973911428609989 * fabs(bigAmp->bL[30]))));
			inputSampleL -= (bigAmp->bL[31] * (0.49068186937289598 - (0.34785166842136384 * fabs(bigAmp->bL[31]))));
			inputSampleL -= (bigAmp->bL[32] * (0.60274753867622777 - (0.48685038872711034 * fabs(bigAmp->bL[32]))));
			inputSampleL -= (bigAmp->bL[33] * (0.65944678627090636 - (0.49844657885975518 * fabs(bigAmp->bL[33]))));
			inputSampleL -= (bigAmp->bL[34] * (0.64488955808717285 - (0.40514406499806987 * fabs(bigAmp->bL[34]))));
			inputSampleL -= (bigAmp->bL[35] * (0.55818730353434354 - (0.28029870614987346 * fabs(bigAmp->bL[35]))));
			inputSampleL -= (bigAmp->bL[36] * (0.43110859113387556 - (0.15373504582939335 * fabs(bigAmp->bL[36]))));
			inputSampleL -= (bigAmp->bL[37] * (0.37726894966096269 - (0.11570983506028532 * fabs(bigAmp->bL[37]))));
			inputSampleL -= (bigAmp->bL[38] * (0.39953242355200935 - (0.17879231130484088 * fabs(bigAmp->bL[38]))));
			inputSampleL -= (bigAmp->bL[39] * (0.36726676379100875 - (0.22013553023983223 * fabs(bigAmp->bL[39]))));
			inputSampleL -= (bigAmp->bL[40] * (0.27187029469227386 - (0.18461171768478427 * fabs(bigAmp->bL[40]))));
			inputSampleL -= (bigAmp->bL[41] * (0.21109334552321635 - (0.14497481318083569 * fabs(bigAmp->bL[41]))));
			inputSampleL -= (bigAmp->bL[42] * (0.19808797405293213 - (0.14916579928186940 * fabs(bigAmp->bL[42]))));
			inputSampleL -= (bigAmp->bL[43] * (0.16287926785495671 - (0.15146098461120627 * fabs(bigAmp->bL[43]))));
			inputSampleL -= (bigAmp->bL[44] * (0.11086621477163359 - (0.13182973443924018 * fabs(bigAmp->bL[44]))));
			inputSampleL -= (bigAmp->bL[45] * (0.07531043236890560 - (0.08062172796472888 * fabs(bigAmp->bL[45]))));
			inputSampleL -= (bigAmp->bL[46] * (0.01747364473230771 + (0.02201865873632456 * fabs(bigAmp->bL[46]))));
			inputSampleL += (bigAmp->bL[47] * (0.03080279125662693 - (0.08721756240972101 * fabs(bigAmp->bL[47]))));
			inputSampleL += (bigAmp->bL[48] * (0.02354148659185142 - (0.06376361763053796 * fabs(bigAmp->bL[48]))));
			inputSampleL -= (bigAmp->bL[49] * (0.02835772372098715 + (0.00589978513642627 * fabs(bigAmp->bL[49]))));
			inputSampleL -= (bigAmp->bL[50] * (0.08983370744565244 - (0.02350960427706536 * fabs(bigAmp->bL[50]))));
			inputSampleL -= (bigAmp->bL[51] * (0.14148947620055380 - (0.03329826628693369 * fabs(bigAmp->bL[51]))));
			inputSampleL -= (bigAmp->bL[52] * (0.17576502674572581 - (0.06507546651241880 * fabs(bigAmp->bL[52]))));
			inputSampleL -= (bigAmp->bL[53] * (0.17168865666573860 - (0.07734801128437317 * fabs(bigAmp->bL[53]))));
			inputSampleL -= (bigAmp->bL[54] * (0.14107027738292105 - (0.03136459344220402 * fabs(bigAmp->bL[54]))));
			inputSampleL -= (bigAmp->bL[55] * (0.12287163395380074 + (0.01933408169185258 * fabs(bigAmp->bL[55]))));
			inputSampleL -= (bigAmp->bL[56] * (0.12276622398112971 + (0.01983508766241737 * fabs(bigAmp->bL[56]))));
			inputSampleL -= (bigAmp->bL[57] * (0.12349721440213673 - (0.01111031415304768 * fabs(bigAmp->bL[57]))));
			inputSampleL -= (bigAmp->bL[58] * (0.08649454142716655 + (0.02252815645513927 * fabs(bigAmp->bL[58]))));
			inputSampleL -= (bigAmp->bL[59] * (0.00953083685474757 + (0.13778878548343007 * fabs(bigAmp->bL[59]))));
			inputSampleL += (bigAmp->bL[60] * (0.06045983158868478 - (0.23966318224935096 * fabs(bigAmp->bL[60]))));
			inputSampleL += (bigAmp->bL[61] * (0.09053229817093242 - (0.27190119941572544 * fabs(bigAmp->bL[61]))));
			inputSampleL += (bigAmp->bL[62] * (0.08112662178843048 - (0.22456862606452327 * fabs(bigAmp->bL[62]))));
			inputSampleL += (bigAmp->bL[63] * (0.07503525686243730 - (0.14330154410548213 * fabs(bigAmp->bL[63]))));
			inputSampleL += (bigAmp->bL[64] * (0.07372595404399729 - (0.06185193766408734 * fabs(bigAmp->bL[64]))));
			inputSampleL += (bigAmp->bL[65] * (0.06073789200080433 + (0.01261857435786178 * fabs(bigAmp->bL[65]))));
			inputSampleL += (bigAmp->bL[66] * (0.04616712695742254 + (0.05851771967084609 * fabs(bigAmp->bL[66]))));
			inputSampleL += (bigAmp->bL[67] * (0.01036235510345900 + (0.08286534414423796 * fabs(bigAmp->bL[67]))));
			inputSampleL -= (bigAmp->bL[68] * (0.03708389413229191 - (0.06695282381039531 * fabs(bigAmp->bL[68]))));
			inputSampleL -= (bigAmp->bL[69] * (0.07092204876981217 - (0.01915829199112784 * fabs(bigAmp->bL[69]))));
			inputSampleL -= (bigAmp->bL[70] * (0.09443579589460312 + (0.01210082455316246 * fabs(bigAmp->bL[70]))));
			inputSampleL -= (bigAmp->bL[71] * (0.07824038577769601 + (0.06121988546065113 * fabs(bigAmp->bL[71]))));
			inputSampleL -= (bigAmp->bL[72] * (0.00854730633079399 + (0.14468518752295506 * fabs(bigAmp->bL[72]))));
			inputSampleL += (bigAmp->bL[73] * (0.06845589924191028 - (0.18902431382592944 * fabs(bigAmp->bL[73]))));
			inputSampleL += (bigAmp->bL[74] * (0.10351569998375465 - (0.13204443060279647 * fabs(bigAmp->bL[74]))));
			inputSampleL += (bigAmp->bL[75] * (0.10513368758532179 - (0.02993199294485649 * fabs(bigAmp->bL[75]))));
			inputSampleL += (bigAmp->bL[76] * (0.08896978950235003 + (0.04074499273825906 * fabs(bigAmp->bL[76]))));
			inputSampleL += (bigAmp->bL[77] * (0.03697537734050980 + (0.09217751130846838 * fabs(bigAmp->bL[77]))));
			inputSampleL -= (bigAmp->bL[78] * (0.04014322441280276 - (0.14062297149365666 * fabs(bigAmp->bL[78]))));
			inputSampleL -= (bigAmp->bL[79] * (0.10505934581398618 - (0.16988861157275814 * fabs(bigAmp->bL[79]))));
			inputSampleL -= (bigAmp->bL[80] * (0.13937661651676272 - (0.15083294570551492 * fabs(bigAmp->bL[80]))));
			inputSampleL -= (bigAmp->bL[81] * (0.13183458845108439 - (0.06657454442471208 * fabs(bigAmp->bL[81]))));

			temp = (inputSampleL + bigAmp->smoothCabBL) / 3.0;
			bigAmp->smoothCabBL = inputSampleL;
			inputSampleL = temp / 4.0;

			randy = (((double) bigAmp->fpdL / UINT32_MAX) * 0.05);
			drySampleL = ((((inputSampleL * (1 - randy)) + (bigAmp->lastCabSampleL * randy)) * wet) + (drySampleL * (1.0 - wet))) * outputlevel;
			bigAmp->lastCabSampleL = inputSampleL;
			inputSampleL = drySampleL; // cab L

			temp = (inputSampleR + bigAmp->smoothCabAR) / 3.0;
			bigAmp->smoothCabAR = inputSampleR;
			inputSampleR = temp;

			bigAmp->bR[81] = bigAmp->bR[80];
			bigAmp->bR[80] = bigAmp->bR[79];
			bigAmp->bR[79] = bigAmp->bR[78];
			bigAmp->bR[78] = bigAmp->bR[77];
			bigAmp->bR[77] = bigAmp->bR[76];
			bigAmp->bR[76] = bigAmp->bR[75];
			bigAmp->bR[75] = bigAmp->bR[74];
			bigAmp->bR[74] = bigAmp->bR[73];
			bigAmp->bR[73] = bigAmp->bR[72];
			bigAmp->bR[72] = bigAmp->bR[71];
			bigAmp->bR[71] = bigAmp->bR[70];
			bigAmp->bR[70] = bigAmp->bR[69];
			bigAmp->bR[69] = bigAmp->bR[68];
			bigAmp->bR[68] = bigAmp->bR[67];
			bigAmp->bR[67] = bigAmp->bR[66];
			bigAmp->bR[66] = bigAmp->bR[65];
			bigAmp->bR[65] = bigAmp->bR[64];
			bigAmp->bR[64] = bigAmp->bR[63];
			bigAmp->bR[63] = bigAmp->bR[62];
			bigAmp->bR[62] = bigAmp->bR[61];
			bigAmp->bR[61] = bigAmp->bR[60];
			bigAmp->bR[60] = bigAmp->bR[59];
			bigAmp->bR[59] = bigAmp->bR[58];
			bigAmp->bR[58] = bigAmp->bR[57];
			bigAmp->bR[57] = bigAmp->bR[56];
			bigAmp->bR[56] = bigAmp->bR[55];
			bigAmp->bR[55] = bigAmp->bR[54];
			bigAmp->bR[54] = bigAmp->bR[53];
			bigAmp->bR[53] = bigAmp->bR[52];
			bigAmp->bR[52] = bigAmp->bR[51];
			bigAmp->bR[51] = bigAmp->bR[50];
			bigAmp->bR[50] = bigAmp->bR[49];
			bigAmp->bR[49] = bigAmp->bR[48];
			bigAmp->bR[48] = bigAmp->bR[47];
			bigAmp->bR[47] = bigAmp->bR[46];
			bigAmp->bR[46] = bigAmp->bR[45];
			bigAmp->bR[45] = bigAmp->bR[44];
			bigAmp->bR[44] = bigAmp->bR[43];
			bigAmp->bR[43] = bigAmp->bR[42];
			bigAmp->bR[42] = bigAmp->bR[41];
			bigAmp->bR[41] = bigAmp->bR[40];
			bigAmp->bR[40] = bigAmp->bR[39];
			bigAmp->bR[39] = bigAmp->bR[38];
			bigAmp->bR[38] = bigAmp->bR[37];
			bigAmp->bR[37] = bigAmp->bR[36];
			bigAmp->bR[36] = bigAmp->bR[35];
			bigAmp->bR[35] = bigAmp->bR[34];
			bigAmp->bR[34] = bigAmp->bR[33];
			bigAmp->bR[33] = bigAmp->bR[32];
			bigAmp->bR[32] = bigAmp->bR[31];
			bigAmp->bR[31] = bigAmp->bR[30];
			bigAmp->bR[30] = bigAmp->bR[29];
			bigAmp->bR[29] = bigAmp->bR[28];
			bigAmp->bR[28] = bigAmp->bR[27];
			bigAmp->bR[27] = bigAmp->bR[26];
			bigAmp->bR[26] = bigAmp->bR[25];
			bigAmp->bR[25] = bigAmp->bR[24];
			bigAmp->bR[24] = bigAmp->bR[23];
			bigAmp->bR[23] = bigAmp->bR[22];
			bigAmp->bR[22] = bigAmp->bR[21];
			bigAmp->bR[21] = bigAmp->bR[20];
			bigAmp->bR[20] = bigAmp->bR[19];
			bigAmp->bR[19] = bigAmp->bR[18];
			bigAmp->bR[18] = bigAmp->bR[17];
			bigAmp->bR[17] = bigAmp->bR[16];
			bigAmp->bR[16] = bigAmp->bR[15];
			bigAmp->bR[15] = bigAmp->bR[14];
			bigAmp->bR[14] = bigAmp->bR[13];
			bigAmp->bR[13] = bigAmp->bR[12];
			bigAmp->bR[12] = bigAmp->bR[11];
			bigAmp->bR[11] = bigAmp->bR[10];
			bigAmp->bR[10] = bigAmp->bR[9];
			bigAmp->bR[9] = bigAmp->bR[8];
			bigAmp->bR[8] = bigAmp->bR[7];
			bigAmp->bR[7] = bigAmp->bR[6];
			bigAmp->bR[6] = bigAmp->bR[5];
			bigAmp->bR[5] = bigAmp->bR[4];
			bigAmp->bR[4] = bigAmp->bR[3];
			bigAmp->bR[3] = bigAmp->bR[2];
			bigAmp->bR[2] = bigAmp->bR[1];
			bigAmp->bR[1] = bigAmp->bR[0];
			bigAmp->bR[0] = inputSampleR;

			inputSampleR += (bigAmp->bR[1] * (1.35472031405494242 + (0.00220914099195157 * fabs(bigAmp->bR[1]))));
			inputSampleR += (bigAmp->bR[2] * (1.63534207755253003 - (0.11406232654509685 * fabs(bigAmp->bR[2]))));
			inputSampleR += (bigAmp->bR[3] * (1.82334575691525869 - (0.42647194712964054 * fabs(bigAmp->bR[3]))));
			inputSampleR += (bigAmp->bR[4] * (1.86156386235405868 - (0.76744187887586590 * fabs(bigAmp->bR[4]))));
			inputSampleR += (bigAmp->bR[5] * (1.67332739338852599 - (0.95161997324293013 * fabs(bigAmp->bR[5]))));
			inputSampleR += (bigAmp->bR[6] * (1.25054130794899021 - (0.98410433514572859 * fabs(bigAmp->bR[6]))));
			inputSampleR += (bigAmp->bR[7] * (0.70049121047281737 - (0.87375612110718992 * fabs(bigAmp->bR[7]))));
			inputSampleR += (bigAmp->bR[8] * (0.15291791448081560 - (0.61195266024519046 * fabs(bigAmp->bR[8]))));
			inputSampleR -= (bigAmp->bR[9] * (0.37301992914152693 + (0.16755422915252094 * fabs(bigAmp->bR[9]))));
			inputSampleR -= (bigAmp->bR[10] * (0.76568539228498433 - (0.28554435228965386 * fabs(bigAmp->bR[10]))));
			inputSampleR -= (bigAmp->bR[11] * (0.95726568749937369 - (0.61659719162806048 * fabs(bigAmp->bR[11]))));
			inputSampleR -= (bigAmp->bR[12] * (1.01273552193911032 - (0.81827288407943954 * fabs(bigAmp->bR[12]))));
			inputSampleR -= (bigAmp->bR[13] * (0.93920108117234447 - (0.80077111864205874 * fabs(bigAmp->bR[13]))));
			inputSampleR -= (bigAmp->bR[14] * (0.79831898832953974 - (0.65814750339694406 * fabs(bigAmp->bR[14]))));
			inputSampleR -= (bigAmp->bR[15] * (0.64200088100452313 - (0.46135833001232618 * fabs(bigAmp->bR[15]))));
			inputSampleR -= (bigAmp->bR[16] * (0.48807302802822128 - (0.15506178974799034 * fabs(bigAmp->bR[16]))));
			inputSampleR -= (bigAmp->bR[17] * (0.36545171501947982 + (0.16126103769376721 * fabs(bigAmp->bR[17]))));
			inputSampleR -= (bigAmp->bR[18] * (0.31469581455759105 + (0.32250870039053953 * fabs(bigAmp->bR[18]))));
			inputSampleR -= (bigAmp->bR[19] * (0.36893534817945800 + (0.25409418897237473 * fabs(bigAmp->bR[19]))));
			inputSampleR -= (bigAmp->bR[20] * (0.41092557722975687 + (0.13114730488878301 * fabs(bigAmp->bR[20]))));
			inputSampleR -= (bigAmp->bR[21] * (0.38584044480710594 + (0.06825323739722661 * fabs(bigAmp->bR[21]))));
			inputSampleR -= (bigAmp->bR[22] * (0.33378434007178670 + (0.04144255489164217 * fabs(bigAmp->bR[22]))));
			inputSampleR -= (bigAmp->bR[23] * (0.26144203061699706 + (0.06031313105098152 * fabs(bigAmp->bR[23]))));
			inputSampleR -= (bigAmp->bR[24] * (0.25818342000920502 + (0.03642289242586355 * fabs(bigAmp->bR[24]))));
			inputSampleR -= (bigAmp->bR[25] * (0.28096018498822661 + (0.00976973667327174 * fabs(bigAmp->bR[25]))));
			inputSampleR -= (bigAmp->bR[26] * (0.25845682019095384 + (0.02749015358080831 * fabs(bigAmp->bR[26]))));
			inputSampleR -= (bigAmp->bR[27] * (0.26655607865953096 - (0.00329839675455690 * fabs(bigAmp->bR[27]))));
			inputSampleR -= (bigAmp->bR[28] * (0.30590085026938518 - (0.07375043215328811 * fabs(bigAmp->bR[28]))));
			inputSampleR -= (bigAmp->bR[29] * (0.32875683916470899 - (0.12454134857516502 * fabs(bigAmp->bR[29]))));
			inputSampleR -= (bigAmp->bR[30] * (0.38166643180506560 - (0.19973911428609989 * fabs(bigAmp->bR[30]))));
			inputSampleR -= (bigAmp->bR[31] * (0.49068186937289598 - (0.34785166842136384 * fabs(bigAmp->bR[31]))));
			inputSampleR -= (bigAmp->bR[32] * (0.60274753867622777 - (0.48685038872711034 * fabs(bigAmp->bR[32]))));
			inputSampleR -= (bigAmp->bR[33] * (0.65944678627090636 - (0.49844657885975518 * fabs(bigAmp->bR[33]))));
			inputSampleR -= (bigAmp->bR[34] * (0.64488955808717285 - (0.40514406499806987 * fabs(bigAmp->bR[34]))));
			inputSampleR -= (bigAmp->bR[35] * (0.55818730353434354 - (0.28029870614987346 * fabs(bigAmp->bR[35]))));
			inputSampleR -= (bigAmp->bR[36] * (0.43110859113387556 - (0.15373504582939335 * fabs(bigAmp->bR[36]))));
			inputSampleR -= (bigAmp->bR[37] * (0.37726894966096269 - (0.11570983506028532 * fabs(bigAmp->bR[37]))));
			inputSampleR -= (bigAmp->bR[38] * (0.39953242355200935 - (0.17879231130484088 * fabs(bigAmp->bR[38]))));
			inputSampleR -= (bigAmp->bR[39] * (0.36726676379100875 - (0.22013553023983223 * fabs(bigAmp->bR[39]))));
			inputSampleR -= (bigAmp->bR[40] * (0.27187029469227386 - (0.18461171768478427 * fabs(bigAmp->bR[40]))));
			inputSampleR -= (bigAmp->bR[41] * (0.21109334552321635 - (0.14497481318083569 * fabs(bigAmp->bR[41]))));
			inputSampleR -= (bigAmp->bR[42] * (0.19808797405293213 - (0.14916579928186940 * fabs(bigAmp->bR[42]))));
			inputSampleR -= (bigAmp->bR[43] * (0.16287926785495671 - (0.15146098461120627 * fabs(bigAmp->bR[43]))));
			inputSampleR -= (bigAmp->bR[44] * (0.11086621477163359 - (0.13182973443924018 * fabs(bigAmp->bR[44]))));
			inputSampleR -= (bigAmp->bR[45] * (0.07531043236890560 - (0.08062172796472888 * fabs(bigAmp->bR[45]))));
			inputSampleR -= (bigAmp->bR[46] * (0.01747364473230771 + (0.02201865873632456 * fabs(bigAmp->bR[46]))));
			inputSampleR += (bigAmp->bR[47] * (0.03080279125662693 - (0.08721756240972101 * fabs(bigAmp->bR[47]))));
			inputSampleR += (bigAmp->bR[48] * (0.02354148659185142 - (0.06376361763053796 * fabs(bigAmp->bR[48]))));
			inputSampleR -= (bigAmp->bR[49] * (0.02835772372098715 + (0.00589978513642627 * fabs(bigAmp->bR[49]))));
			inputSampleR -= (bigAmp->bR[50] * (0.08983370744565244 - (0.02350960427706536 * fabs(bigAmp->bR[50]))));
			inputSampleR -= (bigAmp->bR[51] * (0.14148947620055380 - (0.03329826628693369 * fabs(bigAmp->bR[51]))));
			inputSampleR -= (bigAmp->bR[52] * (0.17576502674572581 - (0.06507546651241880 * fabs(bigAmp->bR[52]))));
			inputSampleR -= (bigAmp->bR[53] * (0.17168865666573860 - (0.07734801128437317 * fabs(bigAmp->bR[53]))));
			inputSampleR -= (bigAmp->bR[54] * (0.14107027738292105 - (0.03136459344220402 * fabs(bigAmp->bR[54]))));
			inputSampleR -= (bigAmp->bR[55] * (0.12287163395380074 + (0.01933408169185258 * fabs(bigAmp->bR[55]))));
			inputSampleR -= (bigAmp->bR[56] * (0.12276622398112971 + (0.01983508766241737 * fabs(bigAmp->bR[56]))));
			inputSampleR -= (bigAmp->bR[57] * (0.12349721440213673 - (0.01111031415304768 * fabs(bigAmp->bR[57]))));
			inputSampleR -= (bigAmp->bR[58] * (0.08649454142716655 + (0.02252815645513927 * fabs(bigAmp->bR[58]))));
			inputSampleR -= (bigAmp->bR[59] * (0.00953083685474757 + (0.13778878548343007 * fabs(bigAmp->bR[59]))));
			inputSampleR += (bigAmp->bR[60] * (0.06045983158868478 - (0.23966318224935096 * fabs(bigAmp->bR[60]))));
			inputSampleR += (bigAmp->bR[61] * (0.09053229817093242 - (0.27190119941572544 * fabs(bigAmp->bR[61]))));
			inputSampleR += (bigAmp->bR[62] * (0.08112662178843048 - (0.22456862606452327 * fabs(bigAmp->bR[62]))));
			inputSampleR += (bigAmp->bR[63] * (0.07503525686243730 - (0.14330154410548213 * fabs(bigAmp->bR[63]))));
			inputSampleR += (bigAmp->bR[64] * (0.07372595404399729 - (0.06185193766408734 * fabs(bigAmp->bR[64]))));
			inputSampleR += (bigAmp->bR[65] * (0.06073789200080433 + (0.01261857435786178 * fabs(bigAmp->bR[65]))));
			inputSampleR += (bigAmp->bR[66] * (0.04616712695742254 + (0.05851771967084609 * fabs(bigAmp->bR[66]))));
			inputSampleR += (bigAmp->bR[67] * (0.01036235510345900 + (0.08286534414423796 * fabs(bigAmp->bR[67]))));
			inputSampleR -= (bigAmp->bR[68] * (0.03708389413229191 - (0.06695282381039531 * fabs(bigAmp->bR[68]))));
			inputSampleR -= (bigAmp->bR[69] * (0.07092204876981217 - (0.01915829199112784 * fabs(bigAmp->bR[69]))));
			inputSampleR -= (bigAmp->bR[70] * (0.09443579589460312 + (0.01210082455316246 * fabs(bigAmp->bR[70]))));
			inputSampleR -= (bigAmp->bR[71] * (0.07824038577769601 + (0.06121988546065113 * fabs(bigAmp->bR[71]))));
			inputSampleR -= (bigAmp->bR[72] * (0.00854730633079399 + (0.14468518752295506 * fabs(bigAmp->bR[72]))));
			inputSampleR += (bigAmp->bR[73] * (0.06845589924191028 - (0.18902431382592944 * fabs(bigAmp->bR[73]))));
			inputSampleR += (bigAmp->bR[74] * (0.10351569998375465 - (0.13204443060279647 * fabs(bigAmp->bR[74]))));
			inputSampleR += (bigAmp->bR[75] * (0.10513368758532179 - (0.02993199294485649 * fabs(bigAmp->bR[75]))));
			inputSampleR += (bigAmp->bR[76] * (0.08896978950235003 + (0.04074499273825906 * fabs(bigAmp->bR[76]))));
			inputSampleR += (bigAmp->bR[77] * (0.03697537734050980 + (0.09217751130846838 * fabs(bigAmp->bR[77]))));
			inputSampleR -= (bigAmp->bR[78] * (0.04014322441280276 - (0.14062297149365666 * fabs(bigAmp->bR[78]))));
			inputSampleR -= (bigAmp->bR[79] * (0.10505934581398618 - (0.16988861157275814 * fabs(bigAmp->bR[79]))));
			inputSampleR -= (bigAmp->bR[80] * (0.13937661651676272 - (0.15083294570551492 * fabs(bigAmp->bR[80]))));
			inputSampleR -= (bigAmp->bR[81] * (0.13183458845108439 - (0.06657454442471208 * fabs(bigAmp->bR[81]))));

			temp = (inputSampleR + bigAmp->smoothCabBR) / 3.0;
			bigAmp->smoothCabBR = inputSampleR;
			inputSampleR = temp / 4.0;

			randy = (((double) bigAmp->fpdR / UINT32_MAX) * 0.05);
			drySampleR = ((((inputSampleR * (1 - randy)) + (bigAmp->lastCabSampleR * randy)) * wet) + (drySampleR * (1.0 - wet))) * outputlevel;
			bigAmp->lastCabSampleR = inputSampleR;
			inputSampleR = drySampleR; // cab

			if (cycleEnd == 4) {
				bigAmp->lastRefL[0] = bigAmp->lastRefL[4]; // start from previous last
				bigAmp->lastRefL[2] = (bigAmp->lastRefL[0] + inputSampleL) / 2; // half
				bigAmp->lastRefL[1] = (bigAmp->lastRefL[0] + bigAmp->lastRefL[2]) / 2; // one quarter
				bigAmp->lastRefL[3] = (bigAmp->lastRefL[2] + inputSampleL) / 2; // three quarters
				bigAmp->lastRefL[4] = inputSampleL; // full
				bigAmp->lastRefR[0] = bigAmp->lastRefR[4]; // start from previous last
				bigAmp->lastRefR[2] = (bigAmp->lastRefR[0] + inputSampleR) / 2; // half
				bigAmp->lastRefR[1] = (bigAmp->lastRefR[0] + bigAmp->lastRefR[2]) / 2; // one quarter
				bigAmp->lastRefR[3] = (bigAmp->lastRefR[2] + inputSampleR) / 2; // three quarters
				bigAmp->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				bigAmp->lastRefL[0] = bigAmp->lastRefL[3]; // start from previous last
				bigAmp->lastRefL[2] = (bigAmp->lastRefL[0] + bigAmp->lastRefL[0] + inputSampleL) / 3; // third
				bigAmp->lastRefL[1] = (bigAmp->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				bigAmp->lastRefL[3] = inputSampleL; // full
				bigAmp->lastRefR[0] = bigAmp->lastRefR[3]; // start from previous last
				bigAmp->lastRefR[2] = (bigAmp->lastRefR[0] + bigAmp->lastRefR[0] + inputSampleR) / 3; // third
				bigAmp->lastRefR[1] = (bigAmp->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				bigAmp->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				bigAmp->lastRefL[0] = bigAmp->lastRefL[2]; // start from previous last
				bigAmp->lastRefL[1] = (bigAmp->lastRefL[0] + inputSampleL) / 2; // half
				bigAmp->lastRefL[2] = inputSampleL; // full
				bigAmp->lastRefR[0] = bigAmp->lastRefR[2]; // start from previous last
				bigAmp->lastRefR[1] = (bigAmp->lastRefR[0] + inputSampleR) / 2; // half
				bigAmp->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				bigAmp->lastRefL[0] = inputSampleL;
				bigAmp->lastRefR[0] = inputSampleR;
			}
			bigAmp->cycle = 0; // reset
			inputSampleL = bigAmp->lastRefL[bigAmp->cycle];
			inputSampleR = bigAmp->lastRefR[bigAmp->cycle];
		} else {
			inputSampleL = bigAmp->lastRefL[bigAmp->cycle];
			inputSampleR = bigAmp->lastRefR[bigAmp->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				bigAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + bigAmp->lastRefL[7]) * 0.5;
				bigAmp->lastRefL[7] = bigAmp->lastRefL[8]; // continue, do not break
				bigAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + bigAmp->lastRefR[7]) * 0.5;
				bigAmp->lastRefR[7] = bigAmp->lastRefR[8]; // continue, do not break
			case 3:
				bigAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + bigAmp->lastRefL[6]) * 0.5;
				bigAmp->lastRefL[6] = bigAmp->lastRefL[8]; // continue, do not break
				bigAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + bigAmp->lastRefR[6]) * 0.5;
				bigAmp->lastRefR[6] = bigAmp->lastRefR[8]; // continue, do not break
			case 2:
				bigAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + bigAmp->lastRefL[5]) * 0.5;
				bigAmp->lastRefL[5] = bigAmp->lastRefL[8]; // continue, do not break
				bigAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + bigAmp->lastRefR[5]) * 0.5;
				bigAmp->lastRefR[5] = bigAmp->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		bigAmp->fpdL ^= bigAmp->fpdL << 13;
		bigAmp->fpdL ^= bigAmp->fpdL >> 17;
		bigAmp->fpdL ^= bigAmp->fpdL << 5;
		inputSampleL += (((double) bigAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		bigAmp->fpdR ^= bigAmp->fpdR << 13;
		bigAmp->fpdR ^= bigAmp->fpdR >> 17;
		bigAmp->fpdR ^= bigAmp->fpdR << 5;
		inputSampleR += (((double) bigAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		// end 32 bit stereo floating point dither

		*out1 = (float) inputSampleL;
		*out2 = (float) inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
	}
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance)
{
	free(instance);
}

static const void* extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	BIGAMP_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
