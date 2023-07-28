#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LILAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/lilamp"

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
	double OddAL;
	double OddBL;
	double OddCL;
	double OddDL;
	double OddEL;
	double EvenAL;
	double EvenBL;
	double EvenCL;
	double EvenDL;
	double EvenEL; // amp

	double lastSampleR;
	double storeSampleR;
	double lastSlewR;
	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double OddAR;
	double OddBR;
	double OddCR;
	double OddDR;
	double OddER;
	double EvenAR;
	double EvenBR;
	double EvenCR;
	double EvenDR;
	double EvenER;

	bool flip; // amp

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
	// default stuff
} LilAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	LilAmp* lilAmp = (LilAmp*) calloc(1, sizeof(LilAmp));
	lilAmp->sampleRate = rate;
	return (LV2_Handle) lilAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	LilAmp* lilAmp = (LilAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			lilAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			lilAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			lilAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			lilAmp->output[1] = (float*) data;
			break;
		case GAIN:
			lilAmp->gain = (const float*) data;
			break;
		case TONE:
			lilAmp->tone = (const float*) data;
			break;
		case OUTPUT:
			lilAmp->outputGain = (const float*) data;
			break;
		case DRY_WET:
			lilAmp->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	LilAmp* lilAmp = (LilAmp*) instance;

	lilAmp->lastSampleL = 0.0;
	lilAmp->storeSampleL = 0.0;
	lilAmp->lastSlewL = 0.0;
	lilAmp->iirSampleAL = 0.0;
	lilAmp->iirSampleBL = 0.0;
	lilAmp->iirSampleCL = 0.0;
	lilAmp->iirSampleDL = 0.0;
	lilAmp->iirSampleEL = 0.0;
	lilAmp->OddAL = 0.0;
	lilAmp->OddBL = 0.0;
	lilAmp->OddCL = 0.0;
	lilAmp->OddDL = 0.0;
	lilAmp->OddEL = 0.0;
	lilAmp->EvenAL = 0.0;
	lilAmp->EvenBL = 0.0;
	lilAmp->EvenCL = 0.0;
	lilAmp->EvenDL = 0.0;
	lilAmp->EvenEL = 0.0; // amp

	lilAmp->lastSampleR = 0.0;
	lilAmp->storeSampleR = 0.0;
	lilAmp->lastSlewR = 0.0;
	lilAmp->iirSampleAR = 0.0;
	lilAmp->iirSampleBR = 0.0;
	lilAmp->iirSampleCR = 0.0;
	lilAmp->iirSampleDR = 0.0;
	lilAmp->iirSampleER = 0.0;
	lilAmp->OddAR = 0.0;
	lilAmp->OddBR = 0.0;
	lilAmp->OddCR = 0.0;
	lilAmp->OddDR = 0.0;
	lilAmp->OddER = 0.0;
	lilAmp->EvenAR = 0.0;
	lilAmp->EvenBR = 0.0;
	lilAmp->EvenCR = 0.0;
	lilAmp->EvenDR = 0.0;
	lilAmp->EvenER = 0.0;

	lilAmp->flip = false; // amp

	for (int fcount = 0; fcount < 90; fcount++) {
		lilAmp->bL[fcount] = 0;
		lilAmp->bR[fcount] = 0;
	}
	lilAmp->smoothCabAL = 0.0;
	lilAmp->smoothCabBL = 0.0;
	lilAmp->lastCabSampleL = 0.0; // cab
	lilAmp->smoothCabAR = 0.0;
	lilAmp->smoothCabBR = 0.0;
	lilAmp->lastCabSampleR = 0.0; // cab

	for (int fcount = 0; fcount < 9; fcount++) {
		lilAmp->lastRefL[fcount] = 0.0;
		lilAmp->lastRefR[fcount] = 0.0;
	}
	lilAmp->cycle = 0; // undersampling

	for (int x = 0; x < fix_total; x++) {
		lilAmp->fixA[x] = 0.0;
		lilAmp->fixB[x] = 0.0;
		lilAmp->fixC[x] = 0.0;
		lilAmp->fixD[x] = 0.0;
		lilAmp->fixE[x] = 0.0;
		lilAmp->fixF[x] = 0.0;
	} // filtering

	lilAmp->fpdL = 1.0;
	while (lilAmp->fpdL < 16386) lilAmp->fpdL = rand() * UINT32_MAX;
	lilAmp->fpdR = 1.0;
	while (lilAmp->fpdR < 16386) lilAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	LilAmp* lilAmp = (LilAmp*) instance;

	const float* in1 = lilAmp->input[0];
	const float* in2 = lilAmp->input[1];
	float* out1 = lilAmp->output[0];
	float* out2 = lilAmp->output[1];

	const double sampleRate = lilAmp->sampleRate;
	const float tone = *lilAmp->tone;

	double inputlevel = *lilAmp->gain * 6.0;
	double EQ = (tone / sampleRate) * 22050;
	double basstrim = tone;
	double outputlevel = *lilAmp->outputGain;
	double wet = *lilAmp->dryWet;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (lilAmp->cycle > cycleEnd - 1) lilAmp->cycle = cycleEnd - 1; // sanity check

	double skewlevel = pow(basstrim, 2) * outputlevel;

	double cutoff = (15000.0 + (tone * 10000.0)) / sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low

	lilAmp->fixF[fix_freq] = lilAmp->fixE[fix_freq] = lilAmp->fixD[fix_freq] = lilAmp->fixC[fix_freq] = lilAmp->fixB[fix_freq] = lilAmp->fixA[fix_freq] = cutoff;

	lilAmp->fixA[fix_reso] = 4.46570214;
	lilAmp->fixB[fix_reso] = 1.51387132;
	lilAmp->fixC[fix_reso] = 0.93979296;
	lilAmp->fixD[fix_reso] = 0.70710678;
	lilAmp->fixE[fix_reso] = 0.52972649;
	lilAmp->fixF[fix_reso] = 0.50316379;

	double K = tan(M_PI * lilAmp->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / lilAmp->fixA[fix_reso] + K * K);
	lilAmp->fixA[fix_a0] = K * K * norm;
	lilAmp->fixA[fix_a1] = 2.0 * lilAmp->fixA[fix_a0];
	lilAmp->fixA[fix_a2] = lilAmp->fixA[fix_a0];
	lilAmp->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	lilAmp->fixA[fix_b2] = (1.0 - K / lilAmp->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * lilAmp->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / lilAmp->fixB[fix_reso] + K * K);
	lilAmp->fixB[fix_a0] = K * K * norm;
	lilAmp->fixB[fix_a1] = 2.0 * lilAmp->fixB[fix_a0];
	lilAmp->fixB[fix_a2] = lilAmp->fixB[fix_a0];
	lilAmp->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	lilAmp->fixB[fix_b2] = (1.0 - K / lilAmp->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * lilAmp->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / lilAmp->fixC[fix_reso] + K * K);
	lilAmp->fixC[fix_a0] = K * K * norm;
	lilAmp->fixC[fix_a1] = 2.0 * lilAmp->fixC[fix_a0];
	lilAmp->fixC[fix_a2] = lilAmp->fixC[fix_a0];
	lilAmp->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	lilAmp->fixC[fix_b2] = (1.0 - K / lilAmp->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * lilAmp->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / lilAmp->fixD[fix_reso] + K * K);
	lilAmp->fixD[fix_a0] = K * K * norm;
	lilAmp->fixD[fix_a1] = 2.0 * lilAmp->fixD[fix_a0];
	lilAmp->fixD[fix_a2] = lilAmp->fixD[fix_a0];
	lilAmp->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	lilAmp->fixD[fix_b2] = (1.0 - K / lilAmp->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * lilAmp->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / lilAmp->fixE[fix_reso] + K * K);
	lilAmp->fixE[fix_a0] = K * K * norm;
	lilAmp->fixE[fix_a1] = 2.0 * lilAmp->fixE[fix_a0];
	lilAmp->fixE[fix_a2] = lilAmp->fixE[fix_a0];
	lilAmp->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	lilAmp->fixE[fix_b2] = (1.0 - K / lilAmp->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * lilAmp->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / lilAmp->fixF[fix_reso] + K * K);
	lilAmp->fixF[fix_a0] = K * K * norm;
	lilAmp->fixF[fix_a1] = 2.0 * lilAmp->fixF[fix_a0];
	lilAmp->fixF[fix_a2] = lilAmp->fixF[fix_a0];
	lilAmp->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	lilAmp->fixF[fix_b2] = (1.0 - K / lilAmp->fixF[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = lilAmp->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = lilAmp->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double outSample = (inputSampleL * lilAmp->fixA[fix_a0]) + lilAmp->fixA[fix_sL1];
		lilAmp->fixA[fix_sL1] = (inputSampleL * lilAmp->fixA[fix_a1]) - (outSample * lilAmp->fixA[fix_b1]) + lilAmp->fixA[fix_sL2];
		lilAmp->fixA[fix_sL2] = (inputSampleL * lilAmp->fixA[fix_a2]) - (outSample * lilAmp->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * lilAmp->fixA[fix_a0]) + lilAmp->fixA[fix_sR1];
		lilAmp->fixA[fix_sR1] = (inputSampleR * lilAmp->fixA[fix_a1]) - (outSample * lilAmp->fixA[fix_b1]) + lilAmp->fixA[fix_sR2];
		lilAmp->fixA[fix_sR2] = (inputSampleR * lilAmp->fixA[fix_a2]) - (outSample * lilAmp->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		double skewL = (inputSampleL - lilAmp->lastSampleL);
		lilAmp->lastSampleL = inputSampleL;
		// skew will be direction/angle
		double bridgerectifier = fabs(skewL);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewL > 0.0) skewL = bridgerectifier;
		else skewL = -bridgerectifier;
		// skew is now sined and clamped and then re-amplified again
		skewL *= inputSampleL;
		skewL *= skewlevel;
		inputSampleL *= basstrim;
		inputSampleL *= inputlevel;
		double offsetL = (1.0 - EQ) + (fabs(inputSampleL) * EQ);
		if (offsetL < 0.0) offsetL = 0.0;
		if (offsetL > 1.0) offsetL = 1.0;
		lilAmp->iirSampleAL = (lilAmp->iirSampleAL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - lilAmp->iirSampleAL;
		// highpass
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		lilAmp->iirSampleCL = (lilAmp->iirSampleCL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = lilAmp->iirSampleCL;
		// lowpass. Use offset from before gain stage
		// finished first gain stage

		double skewR = (inputSampleR - lilAmp->lastSampleR);
		lilAmp->lastSampleR = inputSampleR;
		// skew will be direction/angle
		bridgerectifier = fabs(skewR);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewR > 0.0) skewR = bridgerectifier;
		else skewR = -bridgerectifier;
		// skew is now sined and clamped and then re-amplified again
		skewR *= inputSampleR;
		skewR *= skewlevel;
		inputSampleR *= basstrim;
		inputSampleR *= inputlevel;
		double offsetR = (1.0 - EQ) + (fabs(inputSampleR) * EQ);
		if (offsetR < 0.0) offsetR = 0.0;
		if (offsetR > 1.0) offsetR = 1.0;
		lilAmp->iirSampleAR = (lilAmp->iirSampleAR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - lilAmp->iirSampleAR;
		// highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (inputSampleR > 0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// overdrive
		lilAmp->iirSampleCR = (lilAmp->iirSampleCR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = lilAmp->iirSampleCR;
		// lowpass. Use offset from before gain stage
		// finished first gain stage

		outSample = (inputSampleL * lilAmp->fixB[fix_a0]) + lilAmp->fixB[fix_sL1];
		lilAmp->fixB[fix_sL1] = (inputSampleL * lilAmp->fixB[fix_a1]) - (outSample * lilAmp->fixB[fix_b1]) + lilAmp->fixB[fix_sL2];
		lilAmp->fixB[fix_sL2] = (inputSampleL * lilAmp->fixB[fix_a2]) - (outSample * lilAmp->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * lilAmp->fixB[fix_a0]) + lilAmp->fixB[fix_sR1];
		lilAmp->fixB[fix_sR1] = (inputSampleR * lilAmp->fixB[fix_a1]) - (outSample * lilAmp->fixB[fix_b1]) + lilAmp->fixB[fix_sR2];
		lilAmp->fixB[fix_sR2] = (inputSampleR * lilAmp->fixB[fix_a2]) - (outSample * lilAmp->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		offsetL = (1.0 + offsetL) / 2.0;
		lilAmp->iirSampleBL = (lilAmp->iirSampleBL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = inputSampleL - lilAmp->iirSampleBL;
		// highpass
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * (-0.57079633 + skewL)) + (bridgerectifier * (1.57079633 + skewL));
		else inputSampleL = (inputSampleL * (-0.57079633 + skewL)) - (bridgerectifier * (1.57079633 + skewL));
		// overdrive
		lilAmp->iirSampleDL = (lilAmp->iirSampleDL * (1.0 - (offsetL * EQ))) + (inputSampleL * (offsetL * EQ));
		inputSampleL = lilAmp->iirSampleDL;
		// lowpass. Use offset from before gain stage

		inputSampleR *= inputlevel;
		offsetR = (1.0 + offsetR) / 2.0;
		lilAmp->iirSampleBR = (lilAmp->iirSampleBR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = inputSampleR - lilAmp->iirSampleBR;
		// highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * (-0.57079633 + skewR)) + (bridgerectifier * (1.57079633 + skewR));
		else inputSampleR = (inputSampleR * (-0.57079633 + skewR)) - (bridgerectifier * (1.57079633 + skewR));
		// overdrive
		lilAmp->iirSampleDR = (lilAmp->iirSampleDR * (1.0 - (offsetR * EQ))) + (inputSampleR * (offsetR * EQ));
		inputSampleR = lilAmp->iirSampleDR;
		// lowpass. Use offset from before gain stage

		if (lilAmp->flip) {
			lilAmp->OddDL = lilAmp->OddCL;
			lilAmp->OddCL = lilAmp->OddBL;
			lilAmp->OddBL = lilAmp->OddAL;
			lilAmp->OddAL = inputSampleL;
			inputSampleL = (lilAmp->OddAL + lilAmp->OddBL + lilAmp->OddCL + lilAmp->OddDL) / 4.0;
			lilAmp->OddDR = lilAmp->OddCR;
			lilAmp->OddCR = lilAmp->OddBR;
			lilAmp->OddBR = lilAmp->OddAR;
			lilAmp->OddAR = inputSampleR;
			inputSampleR = (lilAmp->OddAR + lilAmp->OddBR + lilAmp->OddCR + lilAmp->OddDR) / 4.0;
		} else {
			lilAmp->EvenDL = lilAmp->EvenCL;
			lilAmp->EvenCL = lilAmp->EvenBL;
			lilAmp->EvenBL = lilAmp->EvenAL;
			lilAmp->EvenAL = inputSampleL;
			inputSampleL = (lilAmp->EvenAL + lilAmp->EvenBL + lilAmp->EvenCL + lilAmp->EvenDL) / 4.0;
			lilAmp->EvenDR = lilAmp->EvenCR;
			lilAmp->EvenCR = lilAmp->EvenBR;
			lilAmp->EvenBR = lilAmp->EvenAR;
			lilAmp->EvenAR = inputSampleR;
			inputSampleR = (lilAmp->EvenAR + lilAmp->EvenBR + lilAmp->EvenCR + lilAmp->EvenDR) / 4.0;
		}

		outSample = (inputSampleL * lilAmp->fixC[fix_a0]) + lilAmp->fixC[fix_sL1];
		lilAmp->fixC[fix_sL1] = (inputSampleL * lilAmp->fixC[fix_a1]) - (outSample * lilAmp->fixC[fix_b1]) + lilAmp->fixC[fix_sL2];
		lilAmp->fixC[fix_sL2] = (inputSampleL * lilAmp->fixC[fix_a2]) - (outSample * lilAmp->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * lilAmp->fixC[fix_a0]) + lilAmp->fixC[fix_sR1];
		lilAmp->fixC[fix_sR1] = (inputSampleR * lilAmp->fixC[fix_a1]) - (outSample * lilAmp->fixC[fix_b1]) + lilAmp->fixC[fix_sR2];
		lilAmp->fixC[fix_sR2] = (inputSampleR * lilAmp->fixC[fix_a2]) - (outSample * lilAmp->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * -0.57079633) + (bridgerectifier * 1.57079633);
		else inputSampleL = (inputSampleL * -0.57079633) - (bridgerectifier * 1.57079633);
		// output stage has less gain, no highpass, straight lowpass
		lilAmp->iirSampleEL = (lilAmp->iirSampleEL * (1.0 - EQ)) + (inputSampleL * EQ);
		inputSampleL = lilAmp->iirSampleEL;
		// lowpass. Use offset from before gain stage

		inputSampleR *= inputlevel;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * -0.57079633) + (bridgerectifier * 1.57079633);
		else inputSampleR = (inputSampleR * -0.57079633) - (bridgerectifier * 1.57079633);
		// output stage has less gain, no highpass, straight lowpass
		lilAmp->iirSampleER = (lilAmp->iirSampleER * (1.0 - EQ)) + (inputSampleR * EQ);
		inputSampleR = lilAmp->iirSampleER;
		// lowpass. Use offset from before gain stage

		outSample = (inputSampleL * lilAmp->fixD[fix_a0]) + lilAmp->fixD[fix_sL1];
		lilAmp->fixD[fix_sL1] = (inputSampleL * lilAmp->fixD[fix_a1]) - (outSample * lilAmp->fixD[fix_b1]) + lilAmp->fixD[fix_sL2];
		lilAmp->fixD[fix_sL2] = (inputSampleL * lilAmp->fixD[fix_a2]) - (outSample * lilAmp->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * lilAmp->fixD[fix_a0]) + lilAmp->fixD[fix_sR1];
		lilAmp->fixD[fix_sR1] = (inputSampleR * lilAmp->fixD[fix_a1]) - (outSample * lilAmp->fixD[fix_b1]) + lilAmp->fixD[fix_sR2];
		lilAmp->fixD[fix_sR2] = (inputSampleR * lilAmp->fixD[fix_a2]) - (outSample * lilAmp->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL = sin(inputSampleL * outputlevel);
		inputSampleR = sin(inputSampleR * outputlevel);

		outSample = (inputSampleL * lilAmp->fixE[fix_a0]) + lilAmp->fixE[fix_sL1];
		lilAmp->fixE[fix_sL1] = (inputSampleL * lilAmp->fixE[fix_a1]) - (outSample * lilAmp->fixE[fix_b1]) + lilAmp->fixE[fix_sL2];
		lilAmp->fixE[fix_sL2] = (inputSampleL * lilAmp->fixE[fix_a2]) - (outSample * lilAmp->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * lilAmp->fixE[fix_a0]) + lilAmp->fixE[fix_sR1];
		lilAmp->fixE[fix_sR1] = (inputSampleR * lilAmp->fixE[fix_a1]) - (outSample * lilAmp->fixE[fix_b1]) + lilAmp->fixE[fix_sR2];
		lilAmp->fixE[fix_sR2] = (inputSampleR * lilAmp->fixE[fix_a2]) - (outSample * lilAmp->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		double randy = (((double) lilAmp->fpdL / UINT32_MAX) * 0.034);
		inputSampleL = ((inputSampleL * (1.0 - randy)) + (lilAmp->storeSampleL * randy)) * outputlevel;
		lilAmp->storeSampleL = inputSampleL;

		randy = (((double) lilAmp->fpdR / UINT32_MAX) * 0.034);
		inputSampleR = ((inputSampleR * (1.0 - randy)) + (lilAmp->storeSampleR * randy)) * outputlevel;
		lilAmp->storeSampleR = inputSampleR;

		outSample = (inputSampleL * lilAmp->fixF[fix_a0]) + lilAmp->fixF[fix_sL1];
		lilAmp->fixF[fix_sL1] = (inputSampleL * lilAmp->fixF[fix_a1]) - (outSample * lilAmp->fixF[fix_b1]) + lilAmp->fixF[fix_sL2];
		lilAmp->fixF[fix_sL2] = (inputSampleL * lilAmp->fixF[fix_a2]) - (outSample * lilAmp->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * lilAmp->fixF[fix_a0]) + lilAmp->fixF[fix_sR1];
		lilAmp->fixF[fix_sR1] = (inputSampleR * lilAmp->fixF[fix_a1]) - (outSample * lilAmp->fixF[fix_b1]) + lilAmp->fixF[fix_sR2];
		lilAmp->fixF[fix_sR2] = (inputSampleR * lilAmp->fixF[fix_a2]) - (outSample * lilAmp->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		lilAmp->flip = !lilAmp->flip;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider
		// amp

		lilAmp->cycle++;
		if (lilAmp->cycle == cycleEnd) {

			double temp = (inputSampleL + lilAmp->smoothCabAL) / 3.0;
			lilAmp->smoothCabAL = inputSampleL;
			inputSampleL = temp;

			lilAmp->bL[82] = lilAmp->bL[81];
			lilAmp->bL[81] = lilAmp->bL[80];
			lilAmp->bL[80] = lilAmp->bL[79];
			lilAmp->bL[79] = lilAmp->bL[78];
			lilAmp->bL[78] = lilAmp->bL[77];
			lilAmp->bL[77] = lilAmp->bL[76];
			lilAmp->bL[76] = lilAmp->bL[75];
			lilAmp->bL[75] = lilAmp->bL[74];
			lilAmp->bL[74] = lilAmp->bL[73];
			lilAmp->bL[73] = lilAmp->bL[72];
			lilAmp->bL[72] = lilAmp->bL[71];
			lilAmp->bL[71] = lilAmp->bL[70];
			lilAmp->bL[70] = lilAmp->bL[69];
			lilAmp->bL[69] = lilAmp->bL[68];
			lilAmp->bL[68] = lilAmp->bL[67];
			lilAmp->bL[67] = lilAmp->bL[66];
			lilAmp->bL[66] = lilAmp->bL[65];
			lilAmp->bL[65] = lilAmp->bL[64];
			lilAmp->bL[64] = lilAmp->bL[63];
			lilAmp->bL[63] = lilAmp->bL[62];
			lilAmp->bL[62] = lilAmp->bL[61];
			lilAmp->bL[61] = lilAmp->bL[60];
			lilAmp->bL[60] = lilAmp->bL[59];
			lilAmp->bL[59] = lilAmp->bL[58];
			lilAmp->bL[58] = lilAmp->bL[57];
			lilAmp->bL[57] = lilAmp->bL[56];
			lilAmp->bL[56] = lilAmp->bL[55];
			lilAmp->bL[55] = lilAmp->bL[54];
			lilAmp->bL[54] = lilAmp->bL[53];
			lilAmp->bL[53] = lilAmp->bL[52];
			lilAmp->bL[52] = lilAmp->bL[51];
			lilAmp->bL[51] = lilAmp->bL[50];
			lilAmp->bL[50] = lilAmp->bL[49];
			lilAmp->bL[49] = lilAmp->bL[48];
			lilAmp->bL[48] = lilAmp->bL[47];
			lilAmp->bL[47] = lilAmp->bL[46];
			lilAmp->bL[46] = lilAmp->bL[45];
			lilAmp->bL[45] = lilAmp->bL[44];
			lilAmp->bL[44] = lilAmp->bL[43];
			lilAmp->bL[43] = lilAmp->bL[42];
			lilAmp->bL[42] = lilAmp->bL[41];
			lilAmp->bL[41] = lilAmp->bL[40];
			lilAmp->bL[40] = lilAmp->bL[39];
			lilAmp->bL[39] = lilAmp->bL[38];
			lilAmp->bL[38] = lilAmp->bL[37];
			lilAmp->bL[37] = lilAmp->bL[36];
			lilAmp->bL[36] = lilAmp->bL[35];
			lilAmp->bL[35] = lilAmp->bL[34];
			lilAmp->bL[34] = lilAmp->bL[33];
			lilAmp->bL[33] = lilAmp->bL[32];
			lilAmp->bL[32] = lilAmp->bL[31];
			lilAmp->bL[31] = lilAmp->bL[30];
			lilAmp->bL[30] = lilAmp->bL[29];
			lilAmp->bL[29] = lilAmp->bL[28];
			lilAmp->bL[28] = lilAmp->bL[27];
			lilAmp->bL[27] = lilAmp->bL[26];
			lilAmp->bL[26] = lilAmp->bL[25];
			lilAmp->bL[25] = lilAmp->bL[24];
			lilAmp->bL[24] = lilAmp->bL[23];
			lilAmp->bL[23] = lilAmp->bL[22];
			lilAmp->bL[22] = lilAmp->bL[21];
			lilAmp->bL[21] = lilAmp->bL[20];
			lilAmp->bL[20] = lilAmp->bL[19];
			lilAmp->bL[19] = lilAmp->bL[18];
			lilAmp->bL[18] = lilAmp->bL[17];
			lilAmp->bL[17] = lilAmp->bL[16];
			lilAmp->bL[16] = lilAmp->bL[15];
			lilAmp->bL[15] = lilAmp->bL[14];
			lilAmp->bL[14] = lilAmp->bL[13];
			lilAmp->bL[13] = lilAmp->bL[12];
			lilAmp->bL[12] = lilAmp->bL[11];
			lilAmp->bL[11] = lilAmp->bL[10];
			lilAmp->bL[10] = lilAmp->bL[9];
			lilAmp->bL[9] = lilAmp->bL[8];
			lilAmp->bL[8] = lilAmp->bL[7];
			lilAmp->bL[7] = lilAmp->bL[6];
			lilAmp->bL[6] = lilAmp->bL[5];
			lilAmp->bL[5] = lilAmp->bL[4];
			lilAmp->bL[4] = lilAmp->bL[3];
			lilAmp->bL[3] = lilAmp->bL[2];
			lilAmp->bL[2] = lilAmp->bL[1];
			lilAmp->bL[1] = lilAmp->bL[0];
			lilAmp->bL[0] = inputSampleL;
			inputSampleL += (lilAmp->bL[1] * (1.42133070619855229 - (0.18270903813104500 * fabs(lilAmp->bL[1]))));
			inputSampleL += (lilAmp->bL[2] * (1.47209686171873821 - (0.27954009590498585 * fabs(lilAmp->bL[2]))));
			inputSampleL += (lilAmp->bL[3] * (1.34648011331265294 - (0.47178343556301960 * fabs(lilAmp->bL[3]))));
			inputSampleL += (lilAmp->bL[4] * (0.82133804036124580 - (0.41060189990353935 * fabs(lilAmp->bL[4]))));
			inputSampleL += (lilAmp->bL[5] * (0.21628057120466901 - (0.26062442734317454 * fabs(lilAmp->bL[5]))));
			inputSampleL -= (lilAmp->bL[6] * (0.30306716082877883 + (0.10067648425439185 * fabs(lilAmp->bL[6]))));
			inputSampleL -= (lilAmp->bL[7] * (0.69484313178531876 - (0.09655574841702286 * fabs(lilAmp->bL[7]))));
			inputSampleL -= (lilAmp->bL[8] * (0.88320822356980833 - (0.26501644327144314 * fabs(lilAmp->bL[8]))));
			inputSampleL -= (lilAmp->bL[9] * (0.81326147029423723 - (0.31115926837054075 * fabs(lilAmp->bL[9]))));
			inputSampleL -= (lilAmp->bL[10] * (0.56728759049069222 - (0.23304233545561287 * fabs(lilAmp->bL[10]))));
			inputSampleL -= (lilAmp->bL[11] * (0.33340326645198737 - (0.12361361388240180 * fabs(lilAmp->bL[11]))));
			inputSampleL -= (lilAmp->bL[12] * (0.20280263733605616 - (0.03531960962500105 * fabs(lilAmp->bL[12]))));
			inputSampleL -= (lilAmp->bL[13] * (0.15864533788751345 + (0.00355160825317868 * fabs(lilAmp->bL[13]))));
			inputSampleL -= (lilAmp->bL[14] * (0.12544767480555119 + (0.01979010423176500 * fabs(lilAmp->bL[14]))));
			inputSampleL -= (lilAmp->bL[15] * (0.06666788902658917 + (0.00188830739903378 * fabs(lilAmp->bL[15]))));
			inputSampleL += (lilAmp->bL[16] * (0.02977793355081072 + (0.02304216615605394 * fabs(lilAmp->bL[16]))));
			inputSampleL += (lilAmp->bL[17] * (0.12821526330916558 + (0.02636238376777800 * fabs(lilAmp->bL[17]))));
			inputSampleL += (lilAmp->bL[18] * (0.19933812710210136 - (0.02932657234709721 * fabs(lilAmp->bL[18]))));
			inputSampleL += (lilAmp->bL[19] * (0.18346460191225772 - (0.12859581955080629 * fabs(lilAmp->bL[19]))));
			inputSampleL -= (lilAmp->bL[20] * (0.00088697526755385 + (0.15855257539277415 * fabs(lilAmp->bL[20]))));
			inputSampleL -= (lilAmp->bL[21] * (0.28904286712096761 + (0.06226286786982616 * fabs(lilAmp->bL[21]))));
			inputSampleL -= (lilAmp->bL[22] * (0.49133546282552537 - (0.06512851581813534 * fabs(lilAmp->bL[22]))));
			inputSampleL -= (lilAmp->bL[23] * (0.52908013030763046 - (0.13606992188523465 * fabs(lilAmp->bL[23]))));
			inputSampleL -= (lilAmp->bL[24] * (0.45897241332311706 - (0.15527194946346906 * fabs(lilAmp->bL[24]))));
			inputSampleL -= (lilAmp->bL[25] * (0.35535938629924352 - (0.13634771941703441 * fabs(lilAmp->bL[25]))));
			inputSampleL -= (lilAmp->bL[26] * (0.26185269405237693 - (0.08736651482771546 * fabs(lilAmp->bL[26]))));
			inputSampleL -= (lilAmp->bL[27] * (0.19997351944186473 - (0.01714565029656306 * fabs(lilAmp->bL[27]))));
			inputSampleL -= (lilAmp->bL[28] * (0.18894054145105646 + (0.04557612705740050 * fabs(lilAmp->bL[28]))));
			inputSampleL -= (lilAmp->bL[29] * (0.24043993691153928 + (0.05267500387081067 * fabs(lilAmp->bL[29]))));
			inputSampleL -= (lilAmp->bL[30] * (0.29191852873822671 + (0.01922151122971644 * fabs(lilAmp->bL[30]))));
			inputSampleL -= (lilAmp->bL[31] * (0.29399783430587761 - (0.02238952856106585 * fabs(lilAmp->bL[31]))));
			inputSampleL -= (lilAmp->bL[32] * (0.26662219155294159 - (0.07760819463416335 * fabs(lilAmp->bL[32]))));
			inputSampleL -= (lilAmp->bL[33] * (0.20881206667122221 - (0.11930017354479640 * fabs(lilAmp->bL[33]))));
			inputSampleL -= (lilAmp->bL[34] * (0.12916658879944876 - (0.11798638949823513 * fabs(lilAmp->bL[34]))));
			inputSampleL -= (lilAmp->bL[35] * (0.07678815166012012 - (0.06826864734598684 * fabs(lilAmp->bL[35]))));
			inputSampleL -= (lilAmp->bL[36] * (0.08568505484529348 - (0.00510459741104792 * fabs(lilAmp->bL[36]))));
			inputSampleL -= (lilAmp->bL[37] * (0.13613615872486634 + (0.02288223583971244 * fabs(lilAmp->bL[37]))));
			inputSampleL -= (lilAmp->bL[38] * (0.17426657494209266 + (0.02723737220296440 * fabs(lilAmp->bL[38]))));
			inputSampleL -= (lilAmp->bL[39] * (0.17343619261009030 + (0.01412920547179825 * fabs(lilAmp->bL[39]))));
			inputSampleL -= (lilAmp->bL[40] * (0.14548368977428555 - (0.02640418940455951 * fabs(lilAmp->bL[40]))));
			inputSampleL -= (lilAmp->bL[41] * (0.10485295885802372 - (0.06334665781931498 * fabs(lilAmp->bL[41]))));
			inputSampleL -= (lilAmp->bL[42] * (0.06632268974717079 - (0.05960343688612868 * fabs(lilAmp->bL[42]))));
			inputSampleL -= (lilAmp->bL[43] * (0.06915692039882040 - (0.03541337869596061 * fabs(lilAmp->bL[43]))));
			inputSampleL -= (lilAmp->bL[44] * (0.11889611687783583 - (0.02250608307287119 * fabs(lilAmp->bL[44]))));
			inputSampleL -= (lilAmp->bL[45] * (0.14598456370320673 + (0.00280345943128246 * fabs(lilAmp->bL[45]))));
			inputSampleL -= (lilAmp->bL[46] * (0.12312084125613143 + (0.04947283933434576 * fabs(lilAmp->bL[46]))));
			inputSampleL -= (lilAmp->bL[47] * (0.11379940289994711 + (0.06590080966570636 * fabs(lilAmp->bL[47]))));
			inputSampleL -= (lilAmp->bL[48] * (0.12963290754003182 + (0.02597647654256477 * fabs(lilAmp->bL[48]))));
			inputSampleL -= (lilAmp->bL[49] * (0.12723837402978638 - (0.04942071966927938 * fabs(lilAmp->bL[49]))));
			inputSampleL -= (lilAmp->bL[50] * (0.09185015882996231 - (0.10420810015956679 * fabs(lilAmp->bL[50]))));
			inputSampleL -= (lilAmp->bL[51] * (0.04011592913036545 - (0.10234174227772008 * fabs(lilAmp->bL[51]))));
			inputSampleL += (lilAmp->bL[52] * (0.00992597785057113 + (0.05674042373836896 * fabs(lilAmp->bL[52]))));
			inputSampleL += (lilAmp->bL[53] * (0.04921452178306781 - (0.00222698867111080 * fabs(lilAmp->bL[53]))));
			inputSampleL += (lilAmp->bL[54] * (0.06096504883783566 - (0.04040426549982253 * fabs(lilAmp->bL[54]))));
			inputSampleL += (lilAmp->bL[55] * (0.04113530718724200 - (0.04190143593049960 * fabs(lilAmp->bL[55]))));
			inputSampleL += (lilAmp->bL[56] * (0.01292699017654650 - (0.01121994018532499 * fabs(lilAmp->bL[56]))));
			inputSampleL -= (lilAmp->bL[57] * (0.00437123132431870 - (0.02482497612289103 * fabs(lilAmp->bL[57]))));
			inputSampleL -= (lilAmp->bL[58] * (0.02090571264211918 - (0.03732746039260295 * fabs(lilAmp->bL[58]))));
			inputSampleL -= (lilAmp->bL[59] * (0.04749751678612051 - (0.02960060937328099 * fabs(lilAmp->bL[59]))));
			inputSampleL -= (lilAmp->bL[60] * (0.07675095194206227 - (0.02241927084099648 * fabs(lilAmp->bL[60]))));
			inputSampleL -= (lilAmp->bL[61] * (0.08879414028581609 - (0.01144281133042115 * fabs(lilAmp->bL[61]))));
			inputSampleL -= (lilAmp->bL[62] * (0.07378854974999530 + (0.02518742701599147 * fabs(lilAmp->bL[62]))));
			inputSampleL -= (lilAmp->bL[63] * (0.04677309194488959 + (0.08984657372223502 * fabs(lilAmp->bL[63]))));
			inputSampleL -= (lilAmp->bL[64] * (0.02911874044176449 + (0.14202665940555093 * fabs(lilAmp->bL[64]))));
			inputSampleL -= (lilAmp->bL[65] * (0.02103564720234969 + (0.14640411976171003 * fabs(lilAmp->bL[65]))));
			inputSampleL -= (lilAmp->bL[66] * (0.01940626429101940 + (0.10867274382865903 * fabs(lilAmp->bL[66]))));
			inputSampleL -= (lilAmp->bL[67] * (0.03965401793931531 + (0.04775225375522835 * fabs(lilAmp->bL[67]))));
			inputSampleL -= (lilAmp->bL[68] * (0.08102486457314527 - (0.03204447425666343 * fabs(lilAmp->bL[68]))));
			inputSampleL -= (lilAmp->bL[69] * (0.11794849372825778 - (0.12755667382696789 * fabs(lilAmp->bL[69]))));
			inputSampleL -= (lilAmp->bL[70] * (0.11946469076758266 - (0.20151394599125422 * fabs(lilAmp->bL[70]))));
			inputSampleL -= (lilAmp->bL[71] * (0.07404630324668053 - (0.21300634351769704 * fabs(lilAmp->bL[71]))));
			inputSampleL -= (lilAmp->bL[72] * (0.00477584437144086 - (0.16864707684978708 * fabs(lilAmp->bL[72]))));
			inputSampleL += (lilAmp->bL[73] * (0.05924822014377220 + (0.09394651445109450 * fabs(lilAmp->bL[73]))));
			inputSampleL += (lilAmp->bL[74] * (0.10060989907457370 + (0.00419196431884887 * fabs(lilAmp->bL[74]))));
			inputSampleL += (lilAmp->bL[75] * (0.10817907203844988 - (0.07459664480796091 * fabs(lilAmp->bL[75]))));
			inputSampleL += (lilAmp->bL[76] * (0.08701102204768002 - (0.11129477437630560 * fabs(lilAmp->bL[76]))));
			inputSampleL += (lilAmp->bL[77] * (0.05673785623180162 - (0.10638640242375266 * fabs(lilAmp->bL[77]))));
			inputSampleL += (lilAmp->bL[78] * (0.02944190197442081 - (0.08499792583420167 * fabs(lilAmp->bL[78]))));
			inputSampleL += (lilAmp->bL[79] * (0.01570145445652971 - (0.06190456843465320 * fabs(lilAmp->bL[79]))));
			inputSampleL += (lilAmp->bL[80] * (0.02770233032476748 - (0.04573713136865480 * fabs(lilAmp->bL[80]))));
			inputSampleL += (lilAmp->bL[81] * (0.05417160459175360 - (0.03965651064634598 * fabs(lilAmp->bL[81]))));
			inputSampleL += (lilAmp->bL[82] * (0.06080831637644498 - (0.02909500789113911 * fabs(lilAmp->bL[82]))));

			temp = (inputSampleL + lilAmp->smoothCabBL) / 3.0;
			lilAmp->smoothCabBL = inputSampleL;
			inputSampleL = temp / 4.0;

			randy = (((double) lilAmp->fpdL / UINT32_MAX) * 0.085);
			drySampleL = ((((inputSampleL * (1.0 - randy)) + (lilAmp->lastCabSampleL * randy)) * wet) + (drySampleL * (1.0 - wet))) * outputlevel;
			lilAmp->lastCabSampleL = inputSampleL;
			inputSampleL = drySampleL; // cab L

			temp = (inputSampleR + lilAmp->smoothCabAR) / 3.0;
			lilAmp->smoothCabAR = inputSampleR;
			inputSampleR = temp;

			lilAmp->bR[82] = lilAmp->bR[81];
			lilAmp->bR[81] = lilAmp->bR[80];
			lilAmp->bR[80] = lilAmp->bR[79];
			lilAmp->bR[79] = lilAmp->bR[78];
			lilAmp->bR[78] = lilAmp->bR[77];
			lilAmp->bR[77] = lilAmp->bR[76];
			lilAmp->bR[76] = lilAmp->bR[75];
			lilAmp->bR[75] = lilAmp->bR[74];
			lilAmp->bR[74] = lilAmp->bR[73];
			lilAmp->bR[73] = lilAmp->bR[72];
			lilAmp->bR[72] = lilAmp->bR[71];
			lilAmp->bR[71] = lilAmp->bR[70];
			lilAmp->bR[70] = lilAmp->bR[69];
			lilAmp->bR[69] = lilAmp->bR[68];
			lilAmp->bR[68] = lilAmp->bR[67];
			lilAmp->bR[67] = lilAmp->bR[66];
			lilAmp->bR[66] = lilAmp->bR[65];
			lilAmp->bR[65] = lilAmp->bR[64];
			lilAmp->bR[64] = lilAmp->bR[63];
			lilAmp->bR[63] = lilAmp->bR[62];
			lilAmp->bR[62] = lilAmp->bR[61];
			lilAmp->bR[61] = lilAmp->bR[60];
			lilAmp->bR[60] = lilAmp->bR[59];
			lilAmp->bR[59] = lilAmp->bR[58];
			lilAmp->bR[58] = lilAmp->bR[57];
			lilAmp->bR[57] = lilAmp->bR[56];
			lilAmp->bR[56] = lilAmp->bR[55];
			lilAmp->bR[55] = lilAmp->bR[54];
			lilAmp->bR[54] = lilAmp->bR[53];
			lilAmp->bR[53] = lilAmp->bR[52];
			lilAmp->bR[52] = lilAmp->bR[51];
			lilAmp->bR[51] = lilAmp->bR[50];
			lilAmp->bR[50] = lilAmp->bR[49];
			lilAmp->bR[49] = lilAmp->bR[48];
			lilAmp->bR[48] = lilAmp->bR[47];
			lilAmp->bR[47] = lilAmp->bR[46];
			lilAmp->bR[46] = lilAmp->bR[45];
			lilAmp->bR[45] = lilAmp->bR[44];
			lilAmp->bR[44] = lilAmp->bR[43];
			lilAmp->bR[43] = lilAmp->bR[42];
			lilAmp->bR[42] = lilAmp->bR[41];
			lilAmp->bR[41] = lilAmp->bR[40];
			lilAmp->bR[40] = lilAmp->bR[39];
			lilAmp->bR[39] = lilAmp->bR[38];
			lilAmp->bR[38] = lilAmp->bR[37];
			lilAmp->bR[37] = lilAmp->bR[36];
			lilAmp->bR[36] = lilAmp->bR[35];
			lilAmp->bR[35] = lilAmp->bR[34];
			lilAmp->bR[34] = lilAmp->bR[33];
			lilAmp->bR[33] = lilAmp->bR[32];
			lilAmp->bR[32] = lilAmp->bR[31];
			lilAmp->bR[31] = lilAmp->bR[30];
			lilAmp->bR[30] = lilAmp->bR[29];
			lilAmp->bR[29] = lilAmp->bR[28];
			lilAmp->bR[28] = lilAmp->bR[27];
			lilAmp->bR[27] = lilAmp->bR[26];
			lilAmp->bR[26] = lilAmp->bR[25];
			lilAmp->bR[25] = lilAmp->bR[24];
			lilAmp->bR[24] = lilAmp->bR[23];
			lilAmp->bR[23] = lilAmp->bR[22];
			lilAmp->bR[22] = lilAmp->bR[21];
			lilAmp->bR[21] = lilAmp->bR[20];
			lilAmp->bR[20] = lilAmp->bR[19];
			lilAmp->bR[19] = lilAmp->bR[18];
			lilAmp->bR[18] = lilAmp->bR[17];
			lilAmp->bR[17] = lilAmp->bR[16];
			lilAmp->bR[16] = lilAmp->bR[15];
			lilAmp->bR[15] = lilAmp->bR[14];
			lilAmp->bR[14] = lilAmp->bR[13];
			lilAmp->bR[13] = lilAmp->bR[12];
			lilAmp->bR[12] = lilAmp->bR[11];
			lilAmp->bR[11] = lilAmp->bR[10];
			lilAmp->bR[10] = lilAmp->bR[9];
			lilAmp->bR[9] = lilAmp->bR[8];
			lilAmp->bR[8] = lilAmp->bR[7];
			lilAmp->bR[7] = lilAmp->bR[6];
			lilAmp->bR[6] = lilAmp->bR[5];
			lilAmp->bR[5] = lilAmp->bR[4];
			lilAmp->bR[4] = lilAmp->bR[3];
			lilAmp->bR[3] = lilAmp->bR[2];
			lilAmp->bR[2] = lilAmp->bR[1];
			lilAmp->bR[1] = lilAmp->bR[0];
			lilAmp->bR[0] = inputSampleR;
			inputSampleR += (lilAmp->bR[1] * (1.42133070619855229 - (0.18270903813104500 * fabs(lilAmp->bR[1]))));
			inputSampleR += (lilAmp->bR[2] * (1.47209686171873821 - (0.27954009590498585 * fabs(lilAmp->bR[2]))));
			inputSampleR += (lilAmp->bR[3] * (1.34648011331265294 - (0.47178343556301960 * fabs(lilAmp->bR[3]))));
			inputSampleR += (lilAmp->bR[4] * (0.82133804036124580 - (0.41060189990353935 * fabs(lilAmp->bR[4]))));
			inputSampleR += (lilAmp->bR[5] * (0.21628057120466901 - (0.26062442734317454 * fabs(lilAmp->bR[5]))));
			inputSampleR -= (lilAmp->bR[6] * (0.30306716082877883 + (0.10067648425439185 * fabs(lilAmp->bR[6]))));
			inputSampleR -= (lilAmp->bR[7] * (0.69484313178531876 - (0.09655574841702286 * fabs(lilAmp->bR[7]))));
			inputSampleR -= (lilAmp->bR[8] * (0.88320822356980833 - (0.26501644327144314 * fabs(lilAmp->bR[8]))));
			inputSampleR -= (lilAmp->bR[9] * (0.81326147029423723 - (0.31115926837054075 * fabs(lilAmp->bR[9]))));
			inputSampleR -= (lilAmp->bR[10] * (0.56728759049069222 - (0.23304233545561287 * fabs(lilAmp->bR[10]))));
			inputSampleR -= (lilAmp->bR[11] * (0.33340326645198737 - (0.12361361388240180 * fabs(lilAmp->bR[11]))));
			inputSampleR -= (lilAmp->bR[12] * (0.20280263733605616 - (0.03531960962500105 * fabs(lilAmp->bR[12]))));
			inputSampleR -= (lilAmp->bR[13] * (0.15864533788751345 + (0.00355160825317868 * fabs(lilAmp->bR[13]))));
			inputSampleR -= (lilAmp->bR[14] * (0.12544767480555119 + (0.01979010423176500 * fabs(lilAmp->bR[14]))));
			inputSampleR -= (lilAmp->bR[15] * (0.06666788902658917 + (0.00188830739903378 * fabs(lilAmp->bR[15]))));
			inputSampleR += (lilAmp->bR[16] * (0.02977793355081072 + (0.02304216615605394 * fabs(lilAmp->bR[16]))));
			inputSampleR += (lilAmp->bR[17] * (0.12821526330916558 + (0.02636238376777800 * fabs(lilAmp->bR[17]))));
			inputSampleR += (lilAmp->bR[18] * (0.19933812710210136 - (0.02932657234709721 * fabs(lilAmp->bR[18]))));
			inputSampleR += (lilAmp->bR[19] * (0.18346460191225772 - (0.12859581955080629 * fabs(lilAmp->bR[19]))));
			inputSampleR -= (lilAmp->bR[20] * (0.00088697526755385 + (0.15855257539277415 * fabs(lilAmp->bR[20]))));
			inputSampleR -= (lilAmp->bR[21] * (0.28904286712096761 + (0.06226286786982616 * fabs(lilAmp->bR[21]))));
			inputSampleR -= (lilAmp->bR[22] * (0.49133546282552537 - (0.06512851581813534 * fabs(lilAmp->bR[22]))));
			inputSampleR -= (lilAmp->bR[23] * (0.52908013030763046 - (0.13606992188523465 * fabs(lilAmp->bR[23]))));
			inputSampleR -= (lilAmp->bR[24] * (0.45897241332311706 - (0.15527194946346906 * fabs(lilAmp->bR[24]))));
			inputSampleR -= (lilAmp->bR[25] * (0.35535938629924352 - (0.13634771941703441 * fabs(lilAmp->bR[25]))));
			inputSampleR -= (lilAmp->bR[26] * (0.26185269405237693 - (0.08736651482771546 * fabs(lilAmp->bR[26]))));
			inputSampleR -= (lilAmp->bR[27] * (0.19997351944186473 - (0.01714565029656306 * fabs(lilAmp->bR[27]))));
			inputSampleR -= (lilAmp->bR[28] * (0.18894054145105646 + (0.04557612705740050 * fabs(lilAmp->bR[28]))));
			inputSampleR -= (lilAmp->bR[29] * (0.24043993691153928 + (0.05267500387081067 * fabs(lilAmp->bR[29]))));
			inputSampleR -= (lilAmp->bR[30] * (0.29191852873822671 + (0.01922151122971644 * fabs(lilAmp->bR[30]))));
			inputSampleR -= (lilAmp->bR[31] * (0.29399783430587761 - (0.02238952856106585 * fabs(lilAmp->bR[31]))));
			inputSampleR -= (lilAmp->bR[32] * (0.26662219155294159 - (0.07760819463416335 * fabs(lilAmp->bR[32]))));
			inputSampleR -= (lilAmp->bR[33] * (0.20881206667122221 - (0.11930017354479640 * fabs(lilAmp->bR[33]))));
			inputSampleR -= (lilAmp->bR[34] * (0.12916658879944876 - (0.11798638949823513 * fabs(lilAmp->bR[34]))));
			inputSampleR -= (lilAmp->bR[35] * (0.07678815166012012 - (0.06826864734598684 * fabs(lilAmp->bR[35]))));
			inputSampleR -= (lilAmp->bR[36] * (0.08568505484529348 - (0.00510459741104792 * fabs(lilAmp->bR[36]))));
			inputSampleR -= (lilAmp->bR[37] * (0.13613615872486634 + (0.02288223583971244 * fabs(lilAmp->bR[37]))));
			inputSampleR -= (lilAmp->bR[38] * (0.17426657494209266 + (0.02723737220296440 * fabs(lilAmp->bR[38]))));
			inputSampleR -= (lilAmp->bR[39] * (0.17343619261009030 + (0.01412920547179825 * fabs(lilAmp->bR[39]))));
			inputSampleR -= (lilAmp->bR[40] * (0.14548368977428555 - (0.02640418940455951 * fabs(lilAmp->bR[40]))));
			inputSampleR -= (lilAmp->bR[41] * (0.10485295885802372 - (0.06334665781931498 * fabs(lilAmp->bR[41]))));
			inputSampleR -= (lilAmp->bR[42] * (0.06632268974717079 - (0.05960343688612868 * fabs(lilAmp->bR[42]))));
			inputSampleR -= (lilAmp->bR[43] * (0.06915692039882040 - (0.03541337869596061 * fabs(lilAmp->bR[43]))));
			inputSampleR -= (lilAmp->bR[44] * (0.11889611687783583 - (0.02250608307287119 * fabs(lilAmp->bR[44]))));
			inputSampleR -= (lilAmp->bR[45] * (0.14598456370320673 + (0.00280345943128246 * fabs(lilAmp->bR[45]))));
			inputSampleR -= (lilAmp->bR[46] * (0.12312084125613143 + (0.04947283933434576 * fabs(lilAmp->bR[46]))));
			inputSampleR -= (lilAmp->bR[47] * (0.11379940289994711 + (0.06590080966570636 * fabs(lilAmp->bR[47]))));
			inputSampleR -= (lilAmp->bR[48] * (0.12963290754003182 + (0.02597647654256477 * fabs(lilAmp->bR[48]))));
			inputSampleR -= (lilAmp->bR[49] * (0.12723837402978638 - (0.04942071966927938 * fabs(lilAmp->bR[49]))));
			inputSampleR -= (lilAmp->bR[50] * (0.09185015882996231 - (0.10420810015956679 * fabs(lilAmp->bR[50]))));
			inputSampleR -= (lilAmp->bR[51] * (0.04011592913036545 - (0.10234174227772008 * fabs(lilAmp->bR[51]))));
			inputSampleR += (lilAmp->bR[52] * (0.00992597785057113 + (0.05674042373836896 * fabs(lilAmp->bR[52]))));
			inputSampleR += (lilAmp->bR[53] * (0.04921452178306781 - (0.00222698867111080 * fabs(lilAmp->bR[53]))));
			inputSampleR += (lilAmp->bR[54] * (0.06096504883783566 - (0.04040426549982253 * fabs(lilAmp->bR[54]))));
			inputSampleR += (lilAmp->bR[55] * (0.04113530718724200 - (0.04190143593049960 * fabs(lilAmp->bR[55]))));
			inputSampleR += (lilAmp->bR[56] * (0.01292699017654650 - (0.01121994018532499 * fabs(lilAmp->bR[56]))));
			inputSampleR -= (lilAmp->bR[57] * (0.00437123132431870 - (0.02482497612289103 * fabs(lilAmp->bR[57]))));
			inputSampleR -= (lilAmp->bR[58] * (0.02090571264211918 - (0.03732746039260295 * fabs(lilAmp->bR[58]))));
			inputSampleR -= (lilAmp->bR[59] * (0.04749751678612051 - (0.02960060937328099 * fabs(lilAmp->bR[59]))));
			inputSampleR -= (lilAmp->bR[60] * (0.07675095194206227 - (0.02241927084099648 * fabs(lilAmp->bR[60]))));
			inputSampleR -= (lilAmp->bR[61] * (0.08879414028581609 - (0.01144281133042115 * fabs(lilAmp->bR[61]))));
			inputSampleR -= (lilAmp->bR[62] * (0.07378854974999530 + (0.02518742701599147 * fabs(lilAmp->bR[62]))));
			inputSampleR -= (lilAmp->bR[63] * (0.04677309194488959 + (0.08984657372223502 * fabs(lilAmp->bR[63]))));
			inputSampleR -= (lilAmp->bR[64] * (0.02911874044176449 + (0.14202665940555093 * fabs(lilAmp->bR[64]))));
			inputSampleR -= (lilAmp->bR[65] * (0.02103564720234969 + (0.14640411976171003 * fabs(lilAmp->bR[65]))));
			inputSampleR -= (lilAmp->bR[66] * (0.01940626429101940 + (0.10867274382865903 * fabs(lilAmp->bR[66]))));
			inputSampleR -= (lilAmp->bR[67] * (0.03965401793931531 + (0.04775225375522835 * fabs(lilAmp->bR[67]))));
			inputSampleR -= (lilAmp->bR[68] * (0.08102486457314527 - (0.03204447425666343 * fabs(lilAmp->bR[68]))));
			inputSampleR -= (lilAmp->bR[69] * (0.11794849372825778 - (0.12755667382696789 * fabs(lilAmp->bR[69]))));
			inputSampleR -= (lilAmp->bR[70] * (0.11946469076758266 - (0.20151394599125422 * fabs(lilAmp->bR[70]))));
			inputSampleR -= (lilAmp->bR[71] * (0.07404630324668053 - (0.21300634351769704 * fabs(lilAmp->bR[71]))));
			inputSampleR -= (lilAmp->bR[72] * (0.00477584437144086 - (0.16864707684978708 * fabs(lilAmp->bR[72]))));
			inputSampleR += (lilAmp->bR[73] * (0.05924822014377220 + (0.09394651445109450 * fabs(lilAmp->bR[73]))));
			inputSampleR += (lilAmp->bR[74] * (0.10060989907457370 + (0.00419196431884887 * fabs(lilAmp->bR[74]))));
			inputSampleR += (lilAmp->bR[75] * (0.10817907203844988 - (0.07459664480796091 * fabs(lilAmp->bR[75]))));
			inputSampleR += (lilAmp->bR[76] * (0.08701102204768002 - (0.11129477437630560 * fabs(lilAmp->bR[76]))));
			inputSampleR += (lilAmp->bR[77] * (0.05673785623180162 - (0.10638640242375266 * fabs(lilAmp->bR[77]))));
			inputSampleR += (lilAmp->bR[78] * (0.02944190197442081 - (0.08499792583420167 * fabs(lilAmp->bR[78]))));
			inputSampleR += (lilAmp->bR[79] * (0.01570145445652971 - (0.06190456843465320 * fabs(lilAmp->bR[79]))));
			inputSampleR += (lilAmp->bR[80] * (0.02770233032476748 - (0.04573713136865480 * fabs(lilAmp->bR[80]))));
			inputSampleR += (lilAmp->bR[81] * (0.05417160459175360 - (0.03965651064634598 * fabs(lilAmp->bR[81]))));
			inputSampleR += (lilAmp->bR[82] * (0.06080831637644498 - (0.02909500789113911 * fabs(lilAmp->bR[82]))));

			temp = (inputSampleR + lilAmp->smoothCabBR) / 3.0;
			lilAmp->smoothCabBR = inputSampleR;
			inputSampleR = temp / 4.0;

			randy = (((double) lilAmp->fpdR / UINT32_MAX) * 0.085);
			drySampleR = ((((inputSampleR * (1.0 - randy)) + (lilAmp->lastCabSampleR * randy)) * wet) + (drySampleR * (1.0 - wet))) * outputlevel;
			lilAmp->lastCabSampleR = inputSampleR;
			inputSampleR = drySampleR; // cab

			if (cycleEnd == 4) {
				lilAmp->lastRefL[0] = lilAmp->lastRefL[4]; // start from previous last
				lilAmp->lastRefL[2] = (lilAmp->lastRefL[0] + inputSampleL) / 2; // half
				lilAmp->lastRefL[1] = (lilAmp->lastRefL[0] + lilAmp->lastRefL[2]) / 2; // one quarter
				lilAmp->lastRefL[3] = (lilAmp->lastRefL[2] + inputSampleL) / 2; // three quarters
				lilAmp->lastRefL[4] = inputSampleL; // full
				lilAmp->lastRefR[0] = lilAmp->lastRefR[4]; // start from previous last
				lilAmp->lastRefR[2] = (lilAmp->lastRefR[0] + inputSampleR) / 2; // half
				lilAmp->lastRefR[1] = (lilAmp->lastRefR[0] + lilAmp->lastRefR[2]) / 2; // one quarter
				lilAmp->lastRefR[3] = (lilAmp->lastRefR[2] + inputSampleR) / 2; // three quarters
				lilAmp->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				lilAmp->lastRefL[0] = lilAmp->lastRefL[3]; // start from previous last
				lilAmp->lastRefL[2] = (lilAmp->lastRefL[0] + lilAmp->lastRefL[0] + inputSampleL) / 3; // third
				lilAmp->lastRefL[1] = (lilAmp->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				lilAmp->lastRefL[3] = inputSampleL; // full
				lilAmp->lastRefR[0] = lilAmp->lastRefR[3]; // start from previous last
				lilAmp->lastRefR[2] = (lilAmp->lastRefR[0] + lilAmp->lastRefR[0] + inputSampleR) / 3; // third
				lilAmp->lastRefR[1] = (lilAmp->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				lilAmp->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				lilAmp->lastRefL[0] = lilAmp->lastRefL[2]; // start from previous last
				lilAmp->lastRefL[1] = (lilAmp->lastRefL[0] + inputSampleL) / 2; // half
				lilAmp->lastRefL[2] = inputSampleL; // full
				lilAmp->lastRefR[0] = lilAmp->lastRefR[2]; // start from previous last
				lilAmp->lastRefR[1] = (lilAmp->lastRefR[0] + inputSampleR) / 2; // half
				lilAmp->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				lilAmp->lastRefL[0] = inputSampleL;
				lilAmp->lastRefR[0] = inputSampleR;
			}
			lilAmp->cycle = 0; // reset
			inputSampleL = lilAmp->lastRefL[lilAmp->cycle];
			inputSampleR = lilAmp->lastRefR[lilAmp->cycle];
		} else {
			inputSampleL = lilAmp->lastRefL[lilAmp->cycle];
			inputSampleR = lilAmp->lastRefR[lilAmp->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				lilAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + lilAmp->lastRefL[7]) * 0.5;
				lilAmp->lastRefL[7] = lilAmp->lastRefL[8]; // continue, do not break
				lilAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + lilAmp->lastRefR[7]) * 0.5;
				lilAmp->lastRefR[7] = lilAmp->lastRefR[8]; // continue, do not break
			case 3:
				lilAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + lilAmp->lastRefL[6]) * 0.5;
				lilAmp->lastRefL[6] = lilAmp->lastRefL[8]; // continue, do not break
				lilAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + lilAmp->lastRefR[6]) * 0.5;
				lilAmp->lastRefR[6] = lilAmp->lastRefR[8]; // continue, do not break
			case 2:
				lilAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + lilAmp->lastRefL[5]) * 0.5;
				lilAmp->lastRefL[5] = lilAmp->lastRefL[8]; // continue, do not break
				lilAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + lilAmp->lastRefR[5]) * 0.5;
				lilAmp->lastRefR[5] = lilAmp->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		lilAmp->fpdL ^= lilAmp->fpdL << 13;
		lilAmp->fpdL ^= lilAmp->fpdL >> 17;
		lilAmp->fpdL ^= lilAmp->fpdL << 5;
		inputSampleL += (((double) lilAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		lilAmp->fpdR ^= lilAmp->fpdR << 13;
		lilAmp->fpdR ^= lilAmp->fpdR >> 17;
		lilAmp->fpdR ^= lilAmp->fpdR << 5;
		inputSampleR += (((double) lilAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	LILAMP_URI,
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
