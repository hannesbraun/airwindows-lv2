#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MIDAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/midamp"

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
	double OddL[257];
	double EvenL[257];

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
} MidAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	MidAmp* midAmp = (MidAmp*) calloc(1, sizeof(MidAmp));
	midAmp->sampleRate = rate;
	return (LV2_Handle) midAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	MidAmp* midAmp = (MidAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			midAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			midAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			midAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			midAmp->output[1] = (float*) data;
			break;
		case GAIN:
			midAmp->gain = (const float*) data;
			break;
		case TONE:
			midAmp->tone = (const float*) data;
			break;
		case OUTPUT:
			midAmp->outputGain = (const float*) data;
			break;
		case DRY_WET:
			midAmp->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	MidAmp* midAmp = (MidAmp*) instance;

	midAmp->lastSampleL = 0.0;
	midAmp->storeSampleL = 0.0;
	midAmp->lastSlewL = 0.0;
	midAmp->iirSampleAL = 0.0;
	midAmp->iirSampleBL = 0.0;
	midAmp->iirSampleCL = 0.0;
	midAmp->iirSampleDL = 0.0;
	midAmp->iirSampleEL = 0.0;
	midAmp->iirSampleFL = 0.0;
	midAmp->iirSampleGL = 0.0;
	midAmp->iirSampleHL = 0.0;

	midAmp->lastSampleR = 0.0;
	midAmp->storeSampleR = 0.0;
	midAmp->lastSlewR = 0.0;
	midAmp->iirSampleAR = 0.0;
	midAmp->iirSampleBR = 0.0;
	midAmp->iirSampleCR = 0.0;
	midAmp->iirSampleDR = 0.0;
	midAmp->iirSampleER = 0.0;
	midAmp->iirSampleFR = 0.0;
	midAmp->iirSampleGR = 0.0;
	midAmp->iirSampleHR = 0.0;

	for (int fcount = 0; fcount < 257; fcount++) {
		midAmp->OddL[fcount] = 0.0;
		midAmp->EvenL[fcount] = 0.0;
		midAmp->OddR[fcount] = 0.0;
		midAmp->EvenR[fcount] = 0.0;
	}
	midAmp->count = 0;
	midAmp->flip = false; // amp

	for (int fcount = 0; fcount < 90; fcount++) {
		midAmp->bL[fcount] = 0;
		midAmp->bR[fcount] = 0;
	}
	midAmp->smoothCabAL = 0.0;
	midAmp->smoothCabBL = 0.0;
	midAmp->lastCabSampleL = 0.0; // cab
	midAmp->smoothCabAR = 0.0;
	midAmp->smoothCabBR = 0.0;
	midAmp->lastCabSampleR = 0.0; // cab

	for (int fcount = 0; fcount < 9; fcount++) {
		midAmp->lastRefL[fcount] = 0.0;
		midAmp->lastRefR[fcount] = 0.0;
	}
	midAmp->cycle = 0; // undersampling

	for (int x = 0; x < fix_total; x++) {
		midAmp->fixA[x] = 0.0;
		midAmp->fixB[x] = 0.0;
		midAmp->fixC[x] = 0.0;
		midAmp->fixD[x] = 0.0;
		midAmp->fixE[x] = 0.0;
		midAmp->fixF[x] = 0.0;
	} // filtering

	midAmp->fpdL = 1.0;
	while (midAmp->fpdL < 16386) midAmp->fpdL = rand() * UINT32_MAX;
	midAmp->fpdR = 1.0;
	while (midAmp->fpdR < 16386) midAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	MidAmp* midAmp = (MidAmp*) instance;

	const float* in1 = midAmp->input[0];
	const float* in2 = midAmp->input[1];
	float* out1 = midAmp->output[0];
	float* out2 = midAmp->output[1];

	const double sampleRate = midAmp->sampleRate;
	const float tone = *midAmp->tone;

	double inputlevel = *midAmp->gain * 3.0;
	double samplerate = sampleRate;
	double EQ = (tone / samplerate) * 22050.0;
	double basstrim = tone;
	double outputlevel = *midAmp->outputGain;
	double wet = *midAmp->dryWet;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= samplerate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (midAmp->cycle > cycleEnd - 1) midAmp->cycle = cycleEnd - 1; // sanity check

	double bleed = outputlevel / 6.0;
	double BEQ = (bleed / samplerate) * 44100.0;
	int side = (int) (0.0006092985 * samplerate);
	if (side > 126) side = 126;
	int down = side + 1;
	inputlevel = (inputlevel + (inputlevel * basstrim)) / 2.0;

	double cutoff = (15000.0 + (tone * 10000.0)) / sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low

	midAmp->fixF[fix_freq] = midAmp->fixE[fix_freq] = midAmp->fixD[fix_freq] = midAmp->fixC[fix_freq] = midAmp->fixB[fix_freq] = midAmp->fixA[fix_freq] = cutoff;

	midAmp->fixA[fix_reso] = 4.46570214;
	midAmp->fixB[fix_reso] = 1.51387132;
	midAmp->fixC[fix_reso] = 0.93979296;
	midAmp->fixD[fix_reso] = 0.70710678;
	midAmp->fixE[fix_reso] = 0.52972649;
	midAmp->fixF[fix_reso] = 0.50316379;

	double K = tan(M_PI * midAmp->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / midAmp->fixA[fix_reso] + K * K);
	midAmp->fixA[fix_a0] = K * K * norm;
	midAmp->fixA[fix_a1] = 2.0 * midAmp->fixA[fix_a0];
	midAmp->fixA[fix_a2] = midAmp->fixA[fix_a0];
	midAmp->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	midAmp->fixA[fix_b2] = (1.0 - K / midAmp->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * midAmp->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / midAmp->fixB[fix_reso] + K * K);
	midAmp->fixB[fix_a0] = K * K * norm;
	midAmp->fixB[fix_a1] = 2.0 * midAmp->fixB[fix_a0];
	midAmp->fixB[fix_a2] = midAmp->fixB[fix_a0];
	midAmp->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	midAmp->fixB[fix_b2] = (1.0 - K / midAmp->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * midAmp->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / midAmp->fixC[fix_reso] + K * K);
	midAmp->fixC[fix_a0] = K * K * norm;
	midAmp->fixC[fix_a1] = 2.0 * midAmp->fixC[fix_a0];
	midAmp->fixC[fix_a2] = midAmp->fixC[fix_a0];
	midAmp->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	midAmp->fixC[fix_b2] = (1.0 - K / midAmp->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * midAmp->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / midAmp->fixD[fix_reso] + K * K);
	midAmp->fixD[fix_a0] = K * K * norm;
	midAmp->fixD[fix_a1] = 2.0 * midAmp->fixD[fix_a0];
	midAmp->fixD[fix_a2] = midAmp->fixD[fix_a0];
	midAmp->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	midAmp->fixD[fix_b2] = (1.0 - K / midAmp->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * midAmp->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / midAmp->fixE[fix_reso] + K * K);
	midAmp->fixE[fix_a0] = K * K * norm;
	midAmp->fixE[fix_a1] = 2.0 * midAmp->fixE[fix_a0];
	midAmp->fixE[fix_a2] = midAmp->fixE[fix_a0];
	midAmp->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	midAmp->fixE[fix_b2] = (1.0 - K / midAmp->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * midAmp->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / midAmp->fixF[fix_reso] + K * K);
	midAmp->fixF[fix_a0] = K * K * norm;
	midAmp->fixF[fix_a1] = 2.0 * midAmp->fixF[fix_a0];
	midAmp->fixF[fix_a2] = midAmp->fixF[fix_a0];
	midAmp->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	midAmp->fixF[fix_b2] = (1.0 - K / midAmp->fixF[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = midAmp->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = midAmp->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double outSample = (inputSampleL * midAmp->fixA[fix_a0]) + midAmp->fixA[fix_sL1];
		midAmp->fixA[fix_sL1] = (inputSampleL * midAmp->fixA[fix_a1]) - (outSample * midAmp->fixA[fix_b1]) + midAmp->fixA[fix_sL2];
		midAmp->fixA[fix_sL2] = (inputSampleL * midAmp->fixA[fix_a2]) - (outSample * midAmp->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * midAmp->fixA[fix_a0]) + midAmp->fixA[fix_sR1];
		midAmp->fixA[fix_sR1] = (inputSampleR * midAmp->fixA[fix_a1]) - (outSample * midAmp->fixA[fix_b1]) + midAmp->fixA[fix_sR2];
		midAmp->fixA[fix_sR2] = (inputSampleR * midAmp->fixA[fix_a2]) - (outSample * midAmp->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		double skewL = (inputSampleL - midAmp->lastSampleL);
		midAmp->lastSampleL = inputSampleL;
		// skew will be direction/angle
		double bridgerectifier = fabs(skewL);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewL > 0.0) skewL = bridgerectifier;
		else skewL = -bridgerectifier;
		// skew is now sined and clamped and then re-amplified again
		skewL *= inputSampleL;

		double skewR = (inputSampleR - midAmp->lastSampleR);
		midAmp->lastSampleR = inputSampleR;
		// skew will be direction/angle
		bridgerectifier = fabs(skewR);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewR > 0.0) skewR = bridgerectifier;
		else skewR = -bridgerectifier;
		// skew is now sined and clamped and then re-amplified again
		skewR *= inputSampleR;

		outSample = (inputSampleL * midAmp->fixB[fix_a0]) + midAmp->fixB[fix_sL1];
		midAmp->fixB[fix_sL1] = (inputSampleL * midAmp->fixB[fix_a1]) - (outSample * midAmp->fixB[fix_b1]) + midAmp->fixB[fix_sL2];
		midAmp->fixB[fix_sL2] = (inputSampleL * midAmp->fixB[fix_a2]) - (outSample * midAmp->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * midAmp->fixB[fix_a0]) + midAmp->fixB[fix_sR1];
		midAmp->fixB[fix_sR1] = (inputSampleR * midAmp->fixB[fix_a1]) - (outSample * midAmp->fixB[fix_b1]) + midAmp->fixB[fix_sR2];
		midAmp->fixB[fix_sR2] = (inputSampleR * midAmp->fixB[fix_a2]) - (outSample * midAmp->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		double offsetL = (1.0 - EQ) + (fabs(inputSampleL) * EQ);
		if (offsetL < 0.0) offsetL = 0.0;
		if (offsetL > 1.0) offsetL = 1.0;
		midAmp->iirSampleAL = (midAmp->iirSampleAL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - (midAmp->iirSampleAL * 0.8);
		// highpass
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		midAmp->iirSampleCL = (midAmp->iirSampleCL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - (midAmp->iirSampleCL * 0.6);
		// highpass
		skewL /= 1.57079633;
		// finished first gain stage

		inputSampleR *= inputlevel;
		double offsetR = (1.0 - EQ) + (fabs(inputSampleR) * EQ);
		if (offsetR < 0.0) offsetR = 0.0;
		if (offsetR > 1.0) offsetR = 1.0;
		midAmp->iirSampleAR = (midAmp->iirSampleAR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - (midAmp->iirSampleAR * 0.8);
		// highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// overdrive
		midAmp->iirSampleCR = (midAmp->iirSampleCR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - (midAmp->iirSampleCR * 0.6);
		// highpass
		skewR /= 1.57079633;
		// finished first gain stage

		outSample = (inputSampleL * midAmp->fixC[fix_a0]) + midAmp->fixC[fix_sL1];
		midAmp->fixC[fix_sL1] = (inputSampleL * midAmp->fixC[fix_a1]) - (outSample * midAmp->fixC[fix_b1]) + midAmp->fixC[fix_sL2];
		midAmp->fixC[fix_sL2] = (inputSampleL * midAmp->fixC[fix_a2]) - (outSample * midAmp->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * midAmp->fixC[fix_a0]) + midAmp->fixC[fix_sR1];
		midAmp->fixC[fix_sR1] = (inputSampleR * midAmp->fixC[fix_a1]) - (outSample * midAmp->fixC[fix_b1]) + midAmp->fixC[fix_sR2];
		midAmp->fixC[fix_sR2] = (inputSampleR * midAmp->fixC[fix_a2]) - (outSample * midAmp->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		offsetL = (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		midAmp->iirSampleGL = (midAmp->iirSampleGL * (1.0 - EQ)) + (inputSampleL * EQ);
		inputSampleL = inputSampleL - (midAmp->iirSampleGL * 0.4);
		// highpass. no further basscut, no more highpasses
		midAmp->iirSampleDL = (midAmp->iirSampleDL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = midAmp->iirSampleDL;
		skewL /= 1.57079633;
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
		midAmp->iirSampleGR = (midAmp->iirSampleGR * (1.0 - EQ)) + (inputSampleR * EQ);
		inputSampleR = inputSampleR - (midAmp->iirSampleGR * 0.4);
		// highpass. no further basscut, no more highpasses
		midAmp->iirSampleDR = (midAmp->iirSampleDR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = midAmp->iirSampleDR;
		skewR /= 1.57079633;
		// lowpass. Use offset from before gain stage

		outSample = (inputSampleL * midAmp->fixD[fix_a0]) + midAmp->fixD[fix_sL1];
		midAmp->fixD[fix_sL1] = (inputSampleL * midAmp->fixD[fix_a1]) - (outSample * midAmp->fixD[fix_b1]) + midAmp->fixD[fix_sL2];
		midAmp->fixD[fix_sL2] = (inputSampleL * midAmp->fixD[fix_a2]) - (outSample * midAmp->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * midAmp->fixD[fix_a0]) + midAmp->fixD[fix_sR1];
		midAmp->fixD[fix_sR1] = (inputSampleR * midAmp->fixD[fix_a1]) - (outSample * midAmp->fixD[fix_b1]) + midAmp->fixD[fix_sR2];
		midAmp->fixD[fix_sR2] = (inputSampleR * midAmp->fixD[fix_a2]) - (outSample * midAmp->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// output stage has less gain, no highpass, straight lowpass
		midAmp->iirSampleEL = (midAmp->iirSampleEL * (1.0 - EQ)) + (inputSampleL * EQ);
		inputSampleL = midAmp->iirSampleEL;
		// lowpass. Use offset from before gain stage

		inputSampleR *= inputlevel;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// output stage has less gain, no highpass, straight lowpass
		midAmp->iirSampleER = (midAmp->iirSampleER * (1.0 - EQ)) + (inputSampleR * EQ);
		inputSampleR = midAmp->iirSampleER;
		// lowpass. Use offset from before gain stage

		outSample = (inputSampleL * midAmp->fixE[fix_a0]) + midAmp->fixE[fix_sL1];
		midAmp->fixE[fix_sL1] = (inputSampleL * midAmp->fixE[fix_a1]) - (outSample * midAmp->fixE[fix_b1]) + midAmp->fixE[fix_sL2];
		midAmp->fixE[fix_sL2] = (inputSampleL * midAmp->fixE[fix_a2]) - (outSample * midAmp->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * midAmp->fixE[fix_a0]) + midAmp->fixE[fix_sR1];
		midAmp->fixE[fix_sR1] = (inputSampleR * midAmp->fixE[fix_a1]) - (outSample * midAmp->fixE[fix_b1]) + midAmp->fixE[fix_sR2];
		midAmp->fixE[fix_sR2] = (inputSampleR * midAmp->fixE[fix_a2]) - (outSample * midAmp->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		midAmp->iirSampleFL = (midAmp->iirSampleFL * (1.0 - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		// extra lowpass for 4*12" speakers
		midAmp->iirSampleFR = (midAmp->iirSampleFR * (1.0 - (offsetR * BEQ))) + (inputSampleR * (offsetR * BEQ));
		// extra lowpass for 4*12" speakers

		if (midAmp->count < 0 || midAmp->count > 128) {
			midAmp->count = 128;
		}
		double resultBL = 0.0;
		double resultBR = 0.0;
		if (midAmp->flip) {
			midAmp->OddL[midAmp->count + 128] = midAmp->OddL[midAmp->count] = midAmp->iirSampleFL;
			resultBL = (midAmp->OddL[midAmp->count + down] + midAmp->OddL[midAmp->count + side]);
			midAmp->OddR[midAmp->count + 128] = midAmp->OddR[midAmp->count] = midAmp->iirSampleFR;
			resultBR = (midAmp->OddR[midAmp->count + down] + midAmp->OddR[midAmp->count + side]);
		} else {
			midAmp->EvenL[midAmp->count + 128] = midAmp->EvenL[midAmp->count] = midAmp->iirSampleFL;
			resultBL = (midAmp->EvenL[midAmp->count + down] + midAmp->EvenL[midAmp->count + side]);
			midAmp->EvenR[midAmp->count + 128] = midAmp->EvenR[midAmp->count] = midAmp->iirSampleFR;
			resultBR = (midAmp->EvenR[midAmp->count + down] + midAmp->EvenR[midAmp->count + side]);
		}
		midAmp->count--;

		midAmp->iirSampleBL = (midAmp->iirSampleBL * (1.0 - (offsetL * BEQ))) + (resultBL * (offsetL * BEQ));
		inputSampleL += (midAmp->iirSampleBL * bleed);
		// extra lowpass for 4*12" speakers

		midAmp->iirSampleBR = (midAmp->iirSampleBR * (1.0 - (offsetR * BEQ))) + (resultBR * (offsetR * BEQ));
		inputSampleR += (midAmp->iirSampleBR * bleed);
		// extra lowpass for 4*12" speakers

		midAmp->iirSampleHL = (midAmp->iirSampleHL * (1.0 - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		inputSampleL += midAmp->iirSampleHL;

		midAmp->iirSampleHR = (midAmp->iirSampleHR * (1.0 - (offsetR * BEQ))) + (inputSampleR * (offsetR * BEQ));
		inputSampleR += midAmp->iirSampleHR;

		inputSampleL = sin(inputSampleL * outputlevel);
		double randy = (((double) midAmp->fpdL / UINT32_MAX) * 0.047);
		inputSampleL = ((inputSampleL * (1.0 - randy)) + (midAmp->storeSampleL * randy)) * outputlevel;
		midAmp->storeSampleL = inputSampleL;

		inputSampleR = sin(inputSampleR * outputlevel);
		randy = (((double) midAmp->fpdR / UINT32_MAX) * 0.047);
		inputSampleR = ((inputSampleR * (1.0 - randy)) + (midAmp->storeSampleR * randy)) * outputlevel;
		midAmp->storeSampleR = inputSampleR;

		outSample = (inputSampleL * midAmp->fixF[fix_a0]) + midAmp->fixF[fix_sL1];
		midAmp->fixF[fix_sL1] = (inputSampleL * midAmp->fixF[fix_a1]) - (outSample * midAmp->fixF[fix_b1]) + midAmp->fixF[fix_sL2];
		midAmp->fixF[fix_sL2] = (inputSampleL * midAmp->fixF[fix_a2]) - (outSample * midAmp->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * midAmp->fixF[fix_a0]) + midAmp->fixF[fix_sR1];
		midAmp->fixF[fix_sR1] = (inputSampleR * midAmp->fixF[fix_a1]) - (outSample * midAmp->fixF[fix_b1]) + midAmp->fixF[fix_sR2];
		midAmp->fixF[fix_sR2] = (inputSampleR * midAmp->fixF[fix_a2]) - (outSample * midAmp->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		midAmp->flip = !midAmp->flip;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider
		// amp

		midAmp->cycle++;
		if (midAmp->cycle == cycleEnd) {
			double temp = (inputSampleL + midAmp->smoothCabAL) / 3.0;
			midAmp->smoothCabAL = inputSampleL;
			inputSampleL = temp;

			midAmp->bL[87] = midAmp->bL[86];
			midAmp->bL[86] = midAmp->bL[85];
			midAmp->bL[85] = midAmp->bL[84];
			midAmp->bL[84] = midAmp->bL[83];
			midAmp->bL[83] = midAmp->bL[82];
			midAmp->bL[82] = midAmp->bL[81];
			midAmp->bL[81] = midAmp->bL[80];
			midAmp->bL[80] = midAmp->bL[79];
			midAmp->bL[79] = midAmp->bL[78];
			midAmp->bL[78] = midAmp->bL[77];
			midAmp->bL[77] = midAmp->bL[76];
			midAmp->bL[76] = midAmp->bL[75];
			midAmp->bL[75] = midAmp->bL[74];
			midAmp->bL[74] = midAmp->bL[73];
			midAmp->bL[73] = midAmp->bL[72];
			midAmp->bL[72] = midAmp->bL[71];
			midAmp->bL[71] = midAmp->bL[70];
			midAmp->bL[70] = midAmp->bL[69];
			midAmp->bL[69] = midAmp->bL[68];
			midAmp->bL[68] = midAmp->bL[67];
			midAmp->bL[67] = midAmp->bL[66];
			midAmp->bL[66] = midAmp->bL[65];
			midAmp->bL[65] = midAmp->bL[64];
			midAmp->bL[64] = midAmp->bL[63];
			midAmp->bL[63] = midAmp->bL[62];
			midAmp->bL[62] = midAmp->bL[61];
			midAmp->bL[61] = midAmp->bL[60];
			midAmp->bL[60] = midAmp->bL[59];
			midAmp->bL[59] = midAmp->bL[58];
			midAmp->bL[58] = midAmp->bL[57];
			midAmp->bL[57] = midAmp->bL[56];
			midAmp->bL[56] = midAmp->bL[55];
			midAmp->bL[55] = midAmp->bL[54];
			midAmp->bL[54] = midAmp->bL[53];
			midAmp->bL[53] = midAmp->bL[52];
			midAmp->bL[52] = midAmp->bL[51];
			midAmp->bL[51] = midAmp->bL[50];
			midAmp->bL[50] = midAmp->bL[49];
			midAmp->bL[49] = midAmp->bL[48];
			midAmp->bL[48] = midAmp->bL[47];
			midAmp->bL[47] = midAmp->bL[46];
			midAmp->bL[46] = midAmp->bL[45];
			midAmp->bL[45] = midAmp->bL[44];
			midAmp->bL[44] = midAmp->bL[43];
			midAmp->bL[43] = midAmp->bL[42];
			midAmp->bL[42] = midAmp->bL[41];
			midAmp->bL[41] = midAmp->bL[40];
			midAmp->bL[40] = midAmp->bL[39];
			midAmp->bL[39] = midAmp->bL[38];
			midAmp->bL[38] = midAmp->bL[37];
			midAmp->bL[37] = midAmp->bL[36];
			midAmp->bL[36] = midAmp->bL[35];
			midAmp->bL[35] = midAmp->bL[34];
			midAmp->bL[34] = midAmp->bL[33];
			midAmp->bL[33] = midAmp->bL[32];
			midAmp->bL[32] = midAmp->bL[31];
			midAmp->bL[31] = midAmp->bL[30];
			midAmp->bL[30] = midAmp->bL[29];
			midAmp->bL[29] = midAmp->bL[28];
			midAmp->bL[28] = midAmp->bL[27];
			midAmp->bL[27] = midAmp->bL[26];
			midAmp->bL[26] = midAmp->bL[25];
			midAmp->bL[25] = midAmp->bL[24];
			midAmp->bL[24] = midAmp->bL[23];
			midAmp->bL[23] = midAmp->bL[22];
			midAmp->bL[22] = midAmp->bL[21];
			midAmp->bL[21] = midAmp->bL[20];
			midAmp->bL[20] = midAmp->bL[19];
			midAmp->bL[19] = midAmp->bL[18];
			midAmp->bL[18] = midAmp->bL[17];
			midAmp->bL[17] = midAmp->bL[16];
			midAmp->bL[16] = midAmp->bL[15];
			midAmp->bL[15] = midAmp->bL[14];
			midAmp->bL[14] = midAmp->bL[13];
			midAmp->bL[13] = midAmp->bL[12];
			midAmp->bL[12] = midAmp->bL[11];
			midAmp->bL[11] = midAmp->bL[10];
			midAmp->bL[10] = midAmp->bL[9];
			midAmp->bL[9] = midAmp->bL[8];
			midAmp->bL[8] = midAmp->bL[7];
			midAmp->bL[7] = midAmp->bL[6];
			midAmp->bL[6] = midAmp->bL[5];
			midAmp->bL[5] = midAmp->bL[4];
			midAmp->bL[4] = midAmp->bL[3];
			midAmp->bL[3] = midAmp->bL[2];
			midAmp->bL[2] = midAmp->bL[1];
			midAmp->bL[1] = midAmp->bL[0];
			midAmp->bL[0] = inputSampleL;
			inputSampleL += (midAmp->bL[1] * (1.31819680801404560 + (0.00362521700518292 * fabs(midAmp->bL[1]))));
			inputSampleL += (midAmp->bL[2] * (1.37738284126127919 + (0.14134596126256205 * fabs(midAmp->bL[2]))));
			inputSampleL += (midAmp->bL[3] * (1.09957637225311622 + (0.33199581815501555 * fabs(midAmp->bL[3]))));
			inputSampleL += (midAmp->bL[4] * (0.62025358899656258 + (0.37476042042088142 * fabs(midAmp->bL[4]))));
			inputSampleL += (midAmp->bL[5] * (0.12926194402137478 + (0.24702655568406759 * fabs(midAmp->bL[5]))));
			inputSampleL -= (midAmp->bL[6] * (0.28927985011367602 - (0.13289168298307708 * fabs(midAmp->bL[6]))));
			inputSampleL -= (midAmp->bL[7] * (0.56518146339033448 - (0.11026641793526121 * fabs(midAmp->bL[7]))));
			inputSampleL -= (midAmp->bL[8] * (0.59843200696815069 - (0.10139909232154271 * fabs(midAmp->bL[8]))));
			inputSampleL -= (midAmp->bL[9] * (0.45219971861789204 - (0.13313355255903159 * fabs(midAmp->bL[9]))));
			inputSampleL -= (midAmp->bL[10] * (0.32520490032331351 - (0.29009061730364216 * fabs(midAmp->bL[10]))));
			inputSampleL -= (midAmp->bL[11] * (0.29773131872442909 - (0.45307495356996669 * fabs(midAmp->bL[11]))));
			inputSampleL -= (midAmp->bL[12] * (0.31738895975218867 - (0.43198591958928922 * fabs(midAmp->bL[12]))));
			inputSampleL -= (midAmp->bL[13] * (0.33336150604703757 - (0.24240412850274029 * fabs(midAmp->bL[13]))));
			inputSampleL -= (midAmp->bL[14] * (0.32461638442042151 - (0.02779297492397464 * fabs(midAmp->bL[14]))));
			inputSampleL -= (midAmp->bL[15] * (0.27812829473787770 + (0.15565718905032455 * fabs(midAmp->bL[15]))));
			inputSampleL -= (midAmp->bL[16] * (0.19413454458668097 + (0.32087693535188599 * fabs(midAmp->bL[16]))));
			inputSampleL -= (midAmp->bL[17] * (0.12378036344480114 + (0.37736575956794161 * fabs(midAmp->bL[17]))));
			inputSampleL -= (midAmp->bL[18] * (0.12550494837257106 + (0.25593811142722300 * fabs(midAmp->bL[18]))));
			inputSampleL -= (midAmp->bL[19] * (0.17725736033713696 + (0.07708896413593636 * fabs(midAmp->bL[19]))));
			inputSampleL -= (midAmp->bL[20] * (0.22023699647700670 - (0.01600371273599124 * fabs(midAmp->bL[20]))));
			inputSampleL -= (midAmp->bL[21] * (0.21987645486953747 + (0.00973336938352798 * fabs(midAmp->bL[21]))));
			inputSampleL -= (midAmp->bL[22] * (0.15014276479707978 + (0.11602269600138954 * fabs(midAmp->bL[22]))));
			inputSampleL -= (midAmp->bL[23] * (0.05176520203073560 + (0.20383164255692698 * fabs(midAmp->bL[23]))));
			inputSampleL -= (midAmp->bL[24] * (0.04276687165294867 + (0.17785002166834518 * fabs(midAmp->bL[24]))));
			inputSampleL -= (midAmp->bL[25] * (0.15951546388137597 + (0.06748854885822464 * fabs(midAmp->bL[25]))));
			inputSampleL -= (midAmp->bL[26] * (0.30211952144352616 - (0.03440494237025149 * fabs(midAmp->bL[26]))));
			inputSampleL -= (midAmp->bL[27] * (0.36462803375134506 - (0.05874284362202409 * fabs(midAmp->bL[27]))));
			inputSampleL -= (midAmp->bL[28] * (0.32283960219377539 + (0.01189623197958362 * fabs(midAmp->bL[28]))));
			inputSampleL -= (midAmp->bL[29] * (0.19245178663350720 + (0.11088858383712991 * fabs(midAmp->bL[29]))));
			inputSampleL += (midAmp->bL[30] * (0.00681589563349590 - (0.16314250963457660 * fabs(midAmp->bL[30]))));
			inputSampleL += (midAmp->bL[31] * (0.20927798345622584 - (0.16952981620487462 * fabs(midAmp->bL[31]))));
			inputSampleL += (midAmp->bL[32] * (0.25638846543430976 - (0.11462562122281153 * fabs(midAmp->bL[32]))));
			inputSampleL += (midAmp->bL[33] * (0.04584495673888605 + (0.04669671229804190 * fabs(midAmp->bL[33]))));
			inputSampleL -= (midAmp->bL[34] * (0.25221561978187662 - (0.19250758741703761 * fabs(midAmp->bL[34]))));
			inputSampleL -= (midAmp->bL[35] * (0.35662801992424953 - (0.12244680002787561 * fabs(midAmp->bL[35]))));
			inputSampleL -= (midAmp->bL[36] * (0.21498114329314663 + (0.12152120956991189 * fabs(midAmp->bL[36]))));
			inputSampleL += (midAmp->bL[37] * (0.00968605571673376 - (0.30597812512858558 * fabs(midAmp->bL[37]))));
			inputSampleL += (midAmp->bL[38] * (0.18029119870614621 - (0.31569386468576782 * fabs(midAmp->bL[38]))));
			inputSampleL += (midAmp->bL[39] * (0.22744437185251629 - (0.18028438460422197 * fabs(midAmp->bL[39]))));
			inputSampleL += (midAmp->bL[40] * (0.09725687638959078 + (0.05479918522830433 * fabs(midAmp->bL[40]))));
			inputSampleL -= (midAmp->bL[41] * (0.17970389267353537 - (0.29222750363124067 * fabs(midAmp->bL[41]))));
			inputSampleL -= (midAmp->bL[42] * (0.42371969704763018 - (0.34924957781842314 * fabs(midAmp->bL[42]))));
			inputSampleL -= (midAmp->bL[43] * (0.43313266755788055 - (0.11503731970288061 * fabs(midAmp->bL[43]))));
			inputSampleL -= (midAmp->bL[44] * (0.22178165627851801 + (0.25002925550036226 * fabs(midAmp->bL[44]))));
			inputSampleL -= (midAmp->bL[45] * (0.00410198176852576 + (0.43283281457037676 * fabs(midAmp->bL[45]))));
			inputSampleL += (midAmp->bL[46] * (0.09072426344812032 - (0.35318250460706446 * fabs(midAmp->bL[46]))));
			inputSampleL += (midAmp->bL[47] * (0.08405839183965140 - (0.16936391987143717 * fabs(midAmp->bL[47]))));
			inputSampleL -= (midAmp->bL[48] * (0.01110419756114383 - (0.01247164991313877 * fabs(midAmp->bL[48]))));
			inputSampleL -= (midAmp->bL[49] * (0.18593334646855278 - (0.14513260199423966 * fabs(midAmp->bL[49]))));
			inputSampleL -= (midAmp->bL[50] * (0.33665010871497486 - (0.14456206192169668 * fabs(midAmp->bL[50]))));
			inputSampleL -= (midAmp->bL[51] * (0.32644968491439380 + (0.01594380759082303 * fabs(midAmp->bL[51]))));
			inputSampleL -= (midAmp->bL[52] * (0.14855437679485431 + (0.23555511219002742 * fabs(midAmp->bL[52]))));
			inputSampleL += (midAmp->bL[53] * (0.05113019250820622 - (0.35556617126595202 * fabs(midAmp->bL[53]))));
			inputSampleL += (midAmp->bL[54] * (0.12915754942362243 - (0.28571671825750300 * fabs(midAmp->bL[54]))));
			inputSampleL += (midAmp->bL[55] * (0.07406865846069306 - (0.10543886479975995 * fabs(midAmp->bL[55]))));
			inputSampleL -= (midAmp->bL[56] * (0.03669573814193980 - (0.03194267657582078 * fabs(midAmp->bL[56]))));
			inputSampleL -= (midAmp->bL[57] * (0.13429103278009327 - (0.06145796486786051 * fabs(midAmp->bL[57]))));
			inputSampleL -= (midAmp->bL[58] * (0.17884021749974641 - (0.00156626902982124 * fabs(midAmp->bL[58]))));
			inputSampleL -= (midAmp->bL[59] * (0.16138212225178239 + (0.09402070836837134 * fabs(midAmp->bL[59]))));
			inputSampleL -= (midAmp->bL[60] * (0.10867028245257521 + (0.15407963447815898 * fabs(midAmp->bL[60]))));
			inputSampleL -= (midAmp->bL[61] * (0.06312416389213464 + (0.11241095544179526 * fabs(midAmp->bL[61]))));
			inputSampleL -= (midAmp->bL[62] * (0.05826376574081994 - (0.03635253545701986 * fabs(midAmp->bL[62]))));
			inputSampleL -= (midAmp->bL[63] * (0.07991631148258237 - (0.18041947557579863 * fabs(midAmp->bL[63]))));
			inputSampleL -= (midAmp->bL[64] * (0.07777397532506500 - (0.20817156738202205 * fabs(midAmp->bL[64]))));
			inputSampleL -= (midAmp->bL[65] * (0.03812528734394271 - (0.13679963125162486 * fabs(midAmp->bL[65]))));
			inputSampleL += (midAmp->bL[66] * (0.00204900323943951 + (0.04009000730101046 * fabs(midAmp->bL[66]))));
			inputSampleL += (midAmp->bL[67] * (0.01779599498119764 - (0.04218637577942354 * fabs(midAmp->bL[67]))));
			inputSampleL += (midAmp->bL[68] * (0.00950301949319113 - (0.07908911305044238 * fabs(midAmp->bL[68]))));
			inputSampleL -= (midAmp->bL[69] * (0.04283600714814891 + (0.02716262334097985 * fabs(midAmp->bL[69]))));
			inputSampleL -= (midAmp->bL[70] * (0.14478320837041933 - (0.08823515277628832 * fabs(midAmp->bL[70]))));
			inputSampleL -= (midAmp->bL[71] * (0.23250267035795688 - (0.15334197814956568 * fabs(midAmp->bL[71]))));
			inputSampleL -= (midAmp->bL[72] * (0.22369031446225857 - (0.08550989980799503 * fabs(midAmp->bL[72]))));
			inputSampleL -= (midAmp->bL[73] * (0.11142757883989868 + (0.08321482928259660 * fabs(midAmp->bL[73]))));
			inputSampleL += (midAmp->bL[74] * (0.02752318631713307 - (0.25252906099212968 * fabs(midAmp->bL[74]))));
			inputSampleL += (midAmp->bL[75] * (0.11940028414727490 - (0.34358127205009553 * fabs(midAmp->bL[75]))));
			inputSampleL += (midAmp->bL[76] * (0.12702057126698307 - (0.31808560130583663 * fabs(midAmp->bL[76]))));
			inputSampleL += (midAmp->bL[77] * (0.03639067777025356 - (0.17970282734717846 * fabs(midAmp->bL[77]))));
			inputSampleL -= (midAmp->bL[78] * (0.11389848143835518 + (0.00470616711331971 * fabs(midAmp->bL[78]))));
			inputSampleL -= (midAmp->bL[79] * (0.23024072979374310 - (0.09772245468884058 * fabs(midAmp->bL[79]))));
			inputSampleL -= (midAmp->bL[80] * (0.24389015061112601 - (0.09600959885615798 * fabs(midAmp->bL[80]))));
			inputSampleL -= (midAmp->bL[81] * (0.16680269075295703 - (0.05194978963662898 * fabs(midAmp->bL[81]))));
			inputSampleL -= (midAmp->bL[82] * (0.05108041495077725 - (0.01796071525570735 * fabs(midAmp->bL[82]))));
			inputSampleL += (midAmp->bL[83] * (0.06489835353859555 - (0.00808013770331126 * fabs(midAmp->bL[83]))));
			inputSampleL += (midAmp->bL[84] * (0.15481511440233464 - (0.02674063848284838 * fabs(midAmp->bL[84]))));
			inputSampleL += (midAmp->bL[85] * (0.18620867857907253 - (0.01786423699465214 * fabs(midAmp->bL[85]))));
			inputSampleL += (midAmp->bL[86] * (0.13879832139055756 + (0.01584446839973597 * fabs(midAmp->bL[86]))));
			inputSampleL += (midAmp->bL[87] * (0.04878235109120615 + (0.02962866516075816 * fabs(midAmp->bL[87]))));

			temp = (inputSampleL + midAmp->smoothCabBL) / 3.0;
			midAmp->smoothCabBL = inputSampleL;
			inputSampleL = temp / 4.0;

			randy = (((double) midAmp->fpdL / UINT32_MAX) * 0.05);
			drySampleL = ((((inputSampleL * (1 - randy)) + (midAmp->lastCabSampleL * randy)) * wet) + (drySampleL * (1.0 - wet))) * outputlevel;
			midAmp->lastCabSampleL = inputSampleL;
			inputSampleL = drySampleL; // cab L

			temp = (inputSampleR + midAmp->smoothCabAR) / 3.0;
			midAmp->smoothCabAR = inputSampleR;
			inputSampleR = temp;

			midAmp->bR[87] = midAmp->bR[86];
			midAmp->bR[86] = midAmp->bR[85];
			midAmp->bR[85] = midAmp->bR[84];
			midAmp->bR[84] = midAmp->bR[83];
			midAmp->bR[83] = midAmp->bR[82];
			midAmp->bR[82] = midAmp->bR[81];
			midAmp->bR[81] = midAmp->bR[80];
			midAmp->bR[80] = midAmp->bR[79];
			midAmp->bR[79] = midAmp->bR[78];
			midAmp->bR[78] = midAmp->bR[77];
			midAmp->bR[77] = midAmp->bR[76];
			midAmp->bR[76] = midAmp->bR[75];
			midAmp->bR[75] = midAmp->bR[74];
			midAmp->bR[74] = midAmp->bR[73];
			midAmp->bR[73] = midAmp->bR[72];
			midAmp->bR[72] = midAmp->bR[71];
			midAmp->bR[71] = midAmp->bR[70];
			midAmp->bR[70] = midAmp->bR[69];
			midAmp->bR[69] = midAmp->bR[68];
			midAmp->bR[68] = midAmp->bR[67];
			midAmp->bR[67] = midAmp->bR[66];
			midAmp->bR[66] = midAmp->bR[65];
			midAmp->bR[65] = midAmp->bR[64];
			midAmp->bR[64] = midAmp->bR[63];
			midAmp->bR[63] = midAmp->bR[62];
			midAmp->bR[62] = midAmp->bR[61];
			midAmp->bR[61] = midAmp->bR[60];
			midAmp->bR[60] = midAmp->bR[59];
			midAmp->bR[59] = midAmp->bR[58];
			midAmp->bR[58] = midAmp->bR[57];
			midAmp->bR[57] = midAmp->bR[56];
			midAmp->bR[56] = midAmp->bR[55];
			midAmp->bR[55] = midAmp->bR[54];
			midAmp->bR[54] = midAmp->bR[53];
			midAmp->bR[53] = midAmp->bR[52];
			midAmp->bR[52] = midAmp->bR[51];
			midAmp->bR[51] = midAmp->bR[50];
			midAmp->bR[50] = midAmp->bR[49];
			midAmp->bR[49] = midAmp->bR[48];
			midAmp->bR[48] = midAmp->bR[47];
			midAmp->bR[47] = midAmp->bR[46];
			midAmp->bR[46] = midAmp->bR[45];
			midAmp->bR[45] = midAmp->bR[44];
			midAmp->bR[44] = midAmp->bR[43];
			midAmp->bR[43] = midAmp->bR[42];
			midAmp->bR[42] = midAmp->bR[41];
			midAmp->bR[41] = midAmp->bR[40];
			midAmp->bR[40] = midAmp->bR[39];
			midAmp->bR[39] = midAmp->bR[38];
			midAmp->bR[38] = midAmp->bR[37];
			midAmp->bR[37] = midAmp->bR[36];
			midAmp->bR[36] = midAmp->bR[35];
			midAmp->bR[35] = midAmp->bR[34];
			midAmp->bR[34] = midAmp->bR[33];
			midAmp->bR[33] = midAmp->bR[32];
			midAmp->bR[32] = midAmp->bR[31];
			midAmp->bR[31] = midAmp->bR[30];
			midAmp->bR[30] = midAmp->bR[29];
			midAmp->bR[29] = midAmp->bR[28];
			midAmp->bR[28] = midAmp->bR[27];
			midAmp->bR[27] = midAmp->bR[26];
			midAmp->bR[26] = midAmp->bR[25];
			midAmp->bR[25] = midAmp->bR[24];
			midAmp->bR[24] = midAmp->bR[23];
			midAmp->bR[23] = midAmp->bR[22];
			midAmp->bR[22] = midAmp->bR[21];
			midAmp->bR[21] = midAmp->bR[20];
			midAmp->bR[20] = midAmp->bR[19];
			midAmp->bR[19] = midAmp->bR[18];
			midAmp->bR[18] = midAmp->bR[17];
			midAmp->bR[17] = midAmp->bR[16];
			midAmp->bR[16] = midAmp->bR[15];
			midAmp->bR[15] = midAmp->bR[14];
			midAmp->bR[14] = midAmp->bR[13];
			midAmp->bR[13] = midAmp->bR[12];
			midAmp->bR[12] = midAmp->bR[11];
			midAmp->bR[11] = midAmp->bR[10];
			midAmp->bR[10] = midAmp->bR[9];
			midAmp->bR[9] = midAmp->bR[8];
			midAmp->bR[8] = midAmp->bR[7];
			midAmp->bR[7] = midAmp->bR[6];
			midAmp->bR[6] = midAmp->bR[5];
			midAmp->bR[5] = midAmp->bR[4];
			midAmp->bR[4] = midAmp->bR[3];
			midAmp->bR[3] = midAmp->bR[2];
			midAmp->bR[2] = midAmp->bR[1];
			midAmp->bR[1] = midAmp->bR[0];
			midAmp->bR[0] = inputSampleR;
			inputSampleR += (midAmp->bR[1] * (1.31819680801404560 + (0.00362521700518292 * fabs(midAmp->bR[1]))));
			inputSampleR += (midAmp->bR[2] * (1.37738284126127919 + (0.14134596126256205 * fabs(midAmp->bR[2]))));
			inputSampleR += (midAmp->bR[3] * (1.09957637225311622 + (0.33199581815501555 * fabs(midAmp->bR[3]))));
			inputSampleR += (midAmp->bR[4] * (0.62025358899656258 + (0.37476042042088142 * fabs(midAmp->bR[4]))));
			inputSampleR += (midAmp->bR[5] * (0.12926194402137478 + (0.24702655568406759 * fabs(midAmp->bR[5]))));
			inputSampleR -= (midAmp->bR[6] * (0.28927985011367602 - (0.13289168298307708 * fabs(midAmp->bR[6]))));
			inputSampleR -= (midAmp->bR[7] * (0.56518146339033448 - (0.11026641793526121 * fabs(midAmp->bR[7]))));
			inputSampleR -= (midAmp->bR[8] * (0.59843200696815069 - (0.10139909232154271 * fabs(midAmp->bR[8]))));
			inputSampleR -= (midAmp->bR[9] * (0.45219971861789204 - (0.13313355255903159 * fabs(midAmp->bR[9]))));
			inputSampleR -= (midAmp->bR[10] * (0.32520490032331351 - (0.29009061730364216 * fabs(midAmp->bR[10]))));
			inputSampleR -= (midAmp->bR[11] * (0.29773131872442909 - (0.45307495356996669 * fabs(midAmp->bR[11]))));
			inputSampleR -= (midAmp->bR[12] * (0.31738895975218867 - (0.43198591958928922 * fabs(midAmp->bR[12]))));
			inputSampleR -= (midAmp->bR[13] * (0.33336150604703757 - (0.24240412850274029 * fabs(midAmp->bR[13]))));
			inputSampleR -= (midAmp->bR[14] * (0.32461638442042151 - (0.02779297492397464 * fabs(midAmp->bR[14]))));
			inputSampleR -= (midAmp->bR[15] * (0.27812829473787770 + (0.15565718905032455 * fabs(midAmp->bR[15]))));
			inputSampleR -= (midAmp->bR[16] * (0.19413454458668097 + (0.32087693535188599 * fabs(midAmp->bR[16]))));
			inputSampleR -= (midAmp->bR[17] * (0.12378036344480114 + (0.37736575956794161 * fabs(midAmp->bR[17]))));
			inputSampleR -= (midAmp->bR[18] * (0.12550494837257106 + (0.25593811142722300 * fabs(midAmp->bR[18]))));
			inputSampleR -= (midAmp->bR[19] * (0.17725736033713696 + (0.07708896413593636 * fabs(midAmp->bR[19]))));
			inputSampleR -= (midAmp->bR[20] * (0.22023699647700670 - (0.01600371273599124 * fabs(midAmp->bR[20]))));
			inputSampleR -= (midAmp->bR[21] * (0.21987645486953747 + (0.00973336938352798 * fabs(midAmp->bR[21]))));
			inputSampleR -= (midAmp->bR[22] * (0.15014276479707978 + (0.11602269600138954 * fabs(midAmp->bR[22]))));
			inputSampleR -= (midAmp->bR[23] * (0.05176520203073560 + (0.20383164255692698 * fabs(midAmp->bR[23]))));
			inputSampleR -= (midAmp->bR[24] * (0.04276687165294867 + (0.17785002166834518 * fabs(midAmp->bR[24]))));
			inputSampleR -= (midAmp->bR[25] * (0.15951546388137597 + (0.06748854885822464 * fabs(midAmp->bR[25]))));
			inputSampleR -= (midAmp->bR[26] * (0.30211952144352616 - (0.03440494237025149 * fabs(midAmp->bR[26]))));
			inputSampleR -= (midAmp->bR[27] * (0.36462803375134506 - (0.05874284362202409 * fabs(midAmp->bR[27]))));
			inputSampleR -= (midAmp->bR[28] * (0.32283960219377539 + (0.01189623197958362 * fabs(midAmp->bR[28]))));
			inputSampleR -= (midAmp->bR[29] * (0.19245178663350720 + (0.11088858383712991 * fabs(midAmp->bR[29]))));
			inputSampleR += (midAmp->bR[30] * (0.00681589563349590 - (0.16314250963457660 * fabs(midAmp->bR[30]))));
			inputSampleR += (midAmp->bR[31] * (0.20927798345622584 - (0.16952981620487462 * fabs(midAmp->bR[31]))));
			inputSampleR += (midAmp->bR[32] * (0.25638846543430976 - (0.11462562122281153 * fabs(midAmp->bR[32]))));
			inputSampleR += (midAmp->bR[33] * (0.04584495673888605 + (0.04669671229804190 * fabs(midAmp->bR[33]))));
			inputSampleR -= (midAmp->bR[34] * (0.25221561978187662 - (0.19250758741703761 * fabs(midAmp->bR[34]))));
			inputSampleR -= (midAmp->bR[35] * (0.35662801992424953 - (0.12244680002787561 * fabs(midAmp->bR[35]))));
			inputSampleR -= (midAmp->bR[36] * (0.21498114329314663 + (0.12152120956991189 * fabs(midAmp->bR[36]))));
			inputSampleR += (midAmp->bR[37] * (0.00968605571673376 - (0.30597812512858558 * fabs(midAmp->bR[37]))));
			inputSampleR += (midAmp->bR[38] * (0.18029119870614621 - (0.31569386468576782 * fabs(midAmp->bR[38]))));
			inputSampleR += (midAmp->bR[39] * (0.22744437185251629 - (0.18028438460422197 * fabs(midAmp->bR[39]))));
			inputSampleR += (midAmp->bR[40] * (0.09725687638959078 + (0.05479918522830433 * fabs(midAmp->bR[40]))));
			inputSampleR -= (midAmp->bR[41] * (0.17970389267353537 - (0.29222750363124067 * fabs(midAmp->bR[41]))));
			inputSampleR -= (midAmp->bR[42] * (0.42371969704763018 - (0.34924957781842314 * fabs(midAmp->bR[42]))));
			inputSampleR -= (midAmp->bR[43] * (0.43313266755788055 - (0.11503731970288061 * fabs(midAmp->bR[43]))));
			inputSampleR -= (midAmp->bR[44] * (0.22178165627851801 + (0.25002925550036226 * fabs(midAmp->bR[44]))));
			inputSampleR -= (midAmp->bR[45] * (0.00410198176852576 + (0.43283281457037676 * fabs(midAmp->bR[45]))));
			inputSampleR += (midAmp->bR[46] * (0.09072426344812032 - (0.35318250460706446 * fabs(midAmp->bR[46]))));
			inputSampleR += (midAmp->bR[47] * (0.08405839183965140 - (0.16936391987143717 * fabs(midAmp->bR[47]))));
			inputSampleR -= (midAmp->bR[48] * (0.01110419756114383 - (0.01247164991313877 * fabs(midAmp->bR[48]))));
			inputSampleR -= (midAmp->bR[49] * (0.18593334646855278 - (0.14513260199423966 * fabs(midAmp->bR[49]))));
			inputSampleR -= (midAmp->bR[50] * (0.33665010871497486 - (0.14456206192169668 * fabs(midAmp->bR[50]))));
			inputSampleR -= (midAmp->bR[51] * (0.32644968491439380 + (0.01594380759082303 * fabs(midAmp->bR[51]))));
			inputSampleR -= (midAmp->bR[52] * (0.14855437679485431 + (0.23555511219002742 * fabs(midAmp->bR[52]))));
			inputSampleR += (midAmp->bR[53] * (0.05113019250820622 - (0.35556617126595202 * fabs(midAmp->bR[53]))));
			inputSampleR += (midAmp->bR[54] * (0.12915754942362243 - (0.28571671825750300 * fabs(midAmp->bR[54]))));
			inputSampleR += (midAmp->bR[55] * (0.07406865846069306 - (0.10543886479975995 * fabs(midAmp->bR[55]))));
			inputSampleR -= (midAmp->bR[56] * (0.03669573814193980 - (0.03194267657582078 * fabs(midAmp->bR[56]))));
			inputSampleR -= (midAmp->bR[57] * (0.13429103278009327 - (0.06145796486786051 * fabs(midAmp->bR[57]))));
			inputSampleR -= (midAmp->bR[58] * (0.17884021749974641 - (0.00156626902982124 * fabs(midAmp->bR[58]))));
			inputSampleR -= (midAmp->bR[59] * (0.16138212225178239 + (0.09402070836837134 * fabs(midAmp->bR[59]))));
			inputSampleR -= (midAmp->bR[60] * (0.10867028245257521 + (0.15407963447815898 * fabs(midAmp->bR[60]))));
			inputSampleR -= (midAmp->bR[61] * (0.06312416389213464 + (0.11241095544179526 * fabs(midAmp->bR[61]))));
			inputSampleR -= (midAmp->bR[62] * (0.05826376574081994 - (0.03635253545701986 * fabs(midAmp->bR[62]))));
			inputSampleR -= (midAmp->bR[63] * (0.07991631148258237 - (0.18041947557579863 * fabs(midAmp->bR[63]))));
			inputSampleR -= (midAmp->bR[64] * (0.07777397532506500 - (0.20817156738202205 * fabs(midAmp->bR[64]))));
			inputSampleR -= (midAmp->bR[65] * (0.03812528734394271 - (0.13679963125162486 * fabs(midAmp->bR[65]))));
			inputSampleR += (midAmp->bR[66] * (0.00204900323943951 + (0.04009000730101046 * fabs(midAmp->bR[66]))));
			inputSampleR += (midAmp->bR[67] * (0.01779599498119764 - (0.04218637577942354 * fabs(midAmp->bR[67]))));
			inputSampleR += (midAmp->bR[68] * (0.00950301949319113 - (0.07908911305044238 * fabs(midAmp->bR[68]))));
			inputSampleR -= (midAmp->bR[69] * (0.04283600714814891 + (0.02716262334097985 * fabs(midAmp->bR[69]))));
			inputSampleR -= (midAmp->bR[70] * (0.14478320837041933 - (0.08823515277628832 * fabs(midAmp->bR[70]))));
			inputSampleR -= (midAmp->bR[71] * (0.23250267035795688 - (0.15334197814956568 * fabs(midAmp->bR[71]))));
			inputSampleR -= (midAmp->bR[72] * (0.22369031446225857 - (0.08550989980799503 * fabs(midAmp->bR[72]))));
			inputSampleR -= (midAmp->bR[73] * (0.11142757883989868 + (0.08321482928259660 * fabs(midAmp->bR[73]))));
			inputSampleR += (midAmp->bR[74] * (0.02752318631713307 - (0.25252906099212968 * fabs(midAmp->bR[74]))));
			inputSampleR += (midAmp->bR[75] * (0.11940028414727490 - (0.34358127205009553 * fabs(midAmp->bR[75]))));
			inputSampleR += (midAmp->bR[76] * (0.12702057126698307 - (0.31808560130583663 * fabs(midAmp->bR[76]))));
			inputSampleR += (midAmp->bR[77] * (0.03639067777025356 - (0.17970282734717846 * fabs(midAmp->bR[77]))));
			inputSampleR -= (midAmp->bR[78] * (0.11389848143835518 + (0.00470616711331971 * fabs(midAmp->bR[78]))));
			inputSampleR -= (midAmp->bR[79] * (0.23024072979374310 - (0.09772245468884058 * fabs(midAmp->bR[79]))));
			inputSampleR -= (midAmp->bR[80] * (0.24389015061112601 - (0.09600959885615798 * fabs(midAmp->bR[80]))));
			inputSampleR -= (midAmp->bR[81] * (0.16680269075295703 - (0.05194978963662898 * fabs(midAmp->bR[81]))));
			inputSampleR -= (midAmp->bR[82] * (0.05108041495077725 - (0.01796071525570735 * fabs(midAmp->bR[82]))));
			inputSampleR += (midAmp->bR[83] * (0.06489835353859555 - (0.00808013770331126 * fabs(midAmp->bR[83]))));
			inputSampleR += (midAmp->bR[84] * (0.15481511440233464 - (0.02674063848284838 * fabs(midAmp->bR[84]))));
			inputSampleR += (midAmp->bR[85] * (0.18620867857907253 - (0.01786423699465214 * fabs(midAmp->bR[85]))));
			inputSampleR += (midAmp->bR[86] * (0.13879832139055756 + (0.01584446839973597 * fabs(midAmp->bR[86]))));
			inputSampleR += (midAmp->bR[87] * (0.04878235109120615 + (0.02962866516075816 * fabs(midAmp->bR[87]))));

			temp = (inputSampleR + midAmp->smoothCabBR) / 3.0;
			midAmp->smoothCabBR = inputSampleR;
			inputSampleR = temp / 4.0;

			randy = (((double) midAmp->fpdR / UINT32_MAX) * 0.05);
			drySampleR = ((((inputSampleR * (1 - randy)) + (midAmp->lastCabSampleR * randy)) * wet) + (drySampleR * (1.0 - wet))) * outputlevel;
			midAmp->lastCabSampleR = inputSampleR;
			inputSampleR = drySampleR; // cab

			if (cycleEnd == 4) {
				midAmp->lastRefL[0] = midAmp->lastRefL[4]; // start from previous last
				midAmp->lastRefL[2] = (midAmp->lastRefL[0] + inputSampleL) / 2; // half
				midAmp->lastRefL[1] = (midAmp->lastRefL[0] + midAmp->lastRefL[2]) / 2; // one quarter
				midAmp->lastRefL[3] = (midAmp->lastRefL[2] + inputSampleL) / 2; // three quarters
				midAmp->lastRefL[4] = inputSampleL; // full
				midAmp->lastRefR[0] = midAmp->lastRefR[4]; // start from previous last
				midAmp->lastRefR[2] = (midAmp->lastRefR[0] + inputSampleR) / 2; // half
				midAmp->lastRefR[1] = (midAmp->lastRefR[0] + midAmp->lastRefR[2]) / 2; // one quarter
				midAmp->lastRefR[3] = (midAmp->lastRefR[2] + inputSampleR) / 2; // three quarters
				midAmp->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				midAmp->lastRefL[0] = midAmp->lastRefL[3]; // start from previous last
				midAmp->lastRefL[2] = (midAmp->lastRefL[0] + midAmp->lastRefL[0] + inputSampleL) / 3; // third
				midAmp->lastRefL[1] = (midAmp->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				midAmp->lastRefL[3] = inputSampleL; // full
				midAmp->lastRefR[0] = midAmp->lastRefR[3]; // start from previous last
				midAmp->lastRefR[2] = (midAmp->lastRefR[0] + midAmp->lastRefR[0] + inputSampleR) / 3; // third
				midAmp->lastRefR[1] = (midAmp->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				midAmp->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				midAmp->lastRefL[0] = midAmp->lastRefL[2]; // start from previous last
				midAmp->lastRefL[1] = (midAmp->lastRefL[0] + inputSampleL) / 2; // half
				midAmp->lastRefL[2] = inputSampleL; // full
				midAmp->lastRefR[0] = midAmp->lastRefR[2]; // start from previous last
				midAmp->lastRefR[1] = (midAmp->lastRefR[0] + inputSampleR) / 2; // half
				midAmp->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				midAmp->lastRefL[0] = inputSampleL;
				midAmp->lastRefR[0] = inputSampleR;
			}
			midAmp->cycle = 0; // reset
			inputSampleL = midAmp->lastRefL[midAmp->cycle];
			inputSampleR = midAmp->lastRefR[midAmp->cycle];
		} else {
			inputSampleL = midAmp->lastRefL[midAmp->cycle];
			inputSampleR = midAmp->lastRefR[midAmp->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				midAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + midAmp->lastRefL[7]) * 0.5;
				midAmp->lastRefL[7] = midAmp->lastRefL[8]; // continue, do not break
				midAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + midAmp->lastRefR[7]) * 0.5;
				midAmp->lastRefR[7] = midAmp->lastRefR[8]; // continue, do not break
			case 3:
				midAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + midAmp->lastRefL[6]) * 0.5;
				midAmp->lastRefL[6] = midAmp->lastRefL[8]; // continue, do not break
				midAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + midAmp->lastRefR[6]) * 0.5;
				midAmp->lastRefR[6] = midAmp->lastRefR[8]; // continue, do not break
			case 2:
				midAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + midAmp->lastRefL[5]) * 0.5;
				midAmp->lastRefL[5] = midAmp->lastRefL[8]; // continue, do not break
				midAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + midAmp->lastRefR[5]) * 0.5;
				midAmp->lastRefR[5] = midAmp->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		midAmp->fpdL ^= midAmp->fpdL << 13;
		midAmp->fpdL ^= midAmp->fpdL >> 17;
		midAmp->fpdL ^= midAmp->fpdL << 5;
		inputSampleL += (((double) midAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		midAmp->fpdR ^= midAmp->fpdR << 13;
		midAmp->fpdR ^= midAmp->fpdR >> 17;
		midAmp->fpdR ^= midAmp->fpdR << 5;
		inputSampleR += (((double) midAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MIDAMP_URI,
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
