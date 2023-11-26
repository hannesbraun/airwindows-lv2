#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define GRINDAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/grindamp"

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

	double smoothAL;
	double smoothBL;
	double smoothCL;
	double smoothDL;
	double smoothEL;
	double smoothFL;
	double smoothGL;
	double smoothHL;
	double smoothIL;
	double smoothJL;
	double smoothKL;
	double secondAL;
	double secondBL;
	double secondCL;
	double secondDL;
	double secondEL;
	double secondFL;
	double secondGL;
	double secondHL;
	double secondIL;
	double secondJL;
	double secondKL;
	double thirdAL;
	double thirdBL;
	double thirdCL;
	double thirdDL;
	double thirdEL;
	double thirdFL;
	double thirdGL;
	double thirdHL;
	double thirdIL;
	double thirdJL;
	double thirdKL;
	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double iirSampleIL;
	double iirLowpassL;
	double iirSubL;
	double storeSampleL; // amp

	double smoothAR;
	double smoothBR;
	double smoothCR;
	double smoothDR;
	double smoothER;
	double smoothFR;
	double smoothGR;
	double smoothHR;
	double smoothIR;
	double smoothJR;
	double smoothKR;
	double secondAR;
	double secondBR;
	double secondCR;
	double secondDR;
	double secondER;
	double secondFR;
	double secondGR;
	double secondHR;
	double secondIR;
	double secondJR;
	double secondKR;
	double thirdAR;
	double thirdBR;
	double thirdCR;
	double thirdDR;
	double thirdER;
	double thirdFR;
	double thirdGR;
	double thirdHR;
	double thirdIR;
	double thirdJR;
	double thirdKR;
	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double iirSampleGR;
	double iirSampleHR;
	double iirSampleIR;
	double iirLowpassR;
	double iirSubR;
	double storeSampleR; // amp

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
} GrindAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	GrindAmp* grindAmp = (GrindAmp*) calloc(1, sizeof(GrindAmp));
	grindAmp->sampleRate = rate;
	return (LV2_Handle) grindAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	GrindAmp* grindAmp = (GrindAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			grindAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			grindAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			grindAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			grindAmp->output[1] = (float*) data;
			break;
		case GAIN:
			grindAmp->gain = (const float*) data;
			break;
		case TONE:
			grindAmp->tone = (const float*) data;
			break;
		case OUTPUT:
			grindAmp->outputGain = (const float*) data;
			break;
		case DRY_WET:
			grindAmp->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	GrindAmp* grindAmp = (GrindAmp*) instance;

	grindAmp->smoothAL = 0.0;
	grindAmp->smoothBL = 0.0;
	grindAmp->smoothCL = 0.0;
	grindAmp->smoothDL = 0.0;
	grindAmp->smoothEL = 0.0;
	grindAmp->smoothFL = 0.0;
	grindAmp->smoothGL = 0.0;
	grindAmp->smoothHL = 0.0;
	grindAmp->smoothIL = 0.0;
	grindAmp->smoothJL = 0.0;
	grindAmp->smoothKL = 0.0;
	grindAmp->secondAL = 0.0;
	grindAmp->secondBL = 0.0;
	grindAmp->secondCL = 0.0;
	grindAmp->secondDL = 0.0;
	grindAmp->secondEL = 0.0;
	grindAmp->secondFL = 0.0;
	grindAmp->secondGL = 0.0;
	grindAmp->secondHL = 0.0;
	grindAmp->secondIL = 0.0;
	grindAmp->secondJL = 0.0;
	grindAmp->secondKL = 0.0;
	grindAmp->thirdAL = 0.0;
	grindAmp->thirdBL = 0.0;
	grindAmp->thirdCL = 0.0;
	grindAmp->thirdDL = 0.0;
	grindAmp->thirdEL = 0.0;
	grindAmp->thirdFL = 0.0;
	grindAmp->thirdGL = 0.0;
	grindAmp->thirdHL = 0.0;
	grindAmp->thirdIL = 0.0;
	grindAmp->thirdJL = 0.0;
	grindAmp->thirdKL = 0.0;
	grindAmp->iirSampleAL = 0.0;
	grindAmp->iirSampleBL = 0.0;
	grindAmp->iirSampleCL = 0.0;
	grindAmp->iirSampleDL = 0.0;
	grindAmp->iirSampleEL = 0.0;
	grindAmp->iirSampleFL = 0.0;
	grindAmp->iirSampleGL = 0.0;
	grindAmp->iirSampleHL = 0.0;
	grindAmp->iirSampleIL = 0.0;
	grindAmp->iirLowpassL = 0.0;
	grindAmp->iirSubL = 0.0;
	grindAmp->storeSampleL = 0.0; // amp

	grindAmp->smoothAR = 0.0;
	grindAmp->smoothBR = 0.0;
	grindAmp->smoothCR = 0.0;
	grindAmp->smoothDR = 0.0;
	grindAmp->smoothER = 0.0;
	grindAmp->smoothFR = 0.0;
	grindAmp->smoothGR = 0.0;
	grindAmp->smoothHR = 0.0;
	grindAmp->smoothIR = 0.0;
	grindAmp->smoothJR = 0.0;
	grindAmp->smoothKR = 0.0;
	grindAmp->secondAR = 0.0;
	grindAmp->secondBR = 0.0;
	grindAmp->secondCR = 0.0;
	grindAmp->secondDR = 0.0;
	grindAmp->secondER = 0.0;
	grindAmp->secondFR = 0.0;
	grindAmp->secondGR = 0.0;
	grindAmp->secondHR = 0.0;
	grindAmp->secondIR = 0.0;
	grindAmp->secondJR = 0.0;
	grindAmp->secondKR = 0.0;
	grindAmp->thirdAR = 0.0;
	grindAmp->thirdBR = 0.0;
	grindAmp->thirdCR = 0.0;
	grindAmp->thirdDR = 0.0;
	grindAmp->thirdER = 0.0;
	grindAmp->thirdFR = 0.0;
	grindAmp->thirdGR = 0.0;
	grindAmp->thirdHR = 0.0;
	grindAmp->thirdIR = 0.0;
	grindAmp->thirdJR = 0.0;
	grindAmp->thirdKR = 0.0;
	grindAmp->iirSampleAR = 0.0;
	grindAmp->iirSampleBR = 0.0;
	grindAmp->iirSampleCR = 0.0;
	grindAmp->iirSampleDR = 0.0;
	grindAmp->iirSampleER = 0.0;
	grindAmp->iirSampleFR = 0.0;
	grindAmp->iirSampleGR = 0.0;
	grindAmp->iirSampleHR = 0.0;
	grindAmp->iirSampleIR = 0.0;
	grindAmp->iirLowpassR = 0.0;
	grindAmp->iirSubR = 0.0;
	grindAmp->storeSampleR = 0.0; // amp

	for (int fcount = 0; fcount < 90; fcount++) {
		grindAmp->bL[fcount] = 0;
		grindAmp->bR[fcount] = 0;
	}
	grindAmp->smoothCabAL = 0.0;
	grindAmp->smoothCabBL = 0.0;
	grindAmp->lastCabSampleL = 0.0; // cab
	grindAmp->smoothCabAR = 0.0;
	grindAmp->smoothCabBR = 0.0;
	grindAmp->lastCabSampleR = 0.0; // cab

	for (int fcount = 0; fcount < 9; fcount++) {
		grindAmp->lastRefL[fcount] = 0.0;
		grindAmp->lastRefR[fcount] = 0.0;
	}
	grindAmp->cycle = 0; // undersampling

	for (int x = 0; x < fix_total; x++) {
		grindAmp->fixA[x] = 0.0;
		grindAmp->fixB[x] = 0.0;
		grindAmp->fixC[x] = 0.0;
		grindAmp->fixD[x] = 0.0;
		grindAmp->fixE[x] = 0.0;
		grindAmp->fixF[x] = 0.0;
	} // filtering

	grindAmp->fpdL = 1.0;
	while (grindAmp->fpdL < 16386) grindAmp->fpdL = rand() * UINT32_MAX;
	grindAmp->fpdR = 1.0;
	while (grindAmp->fpdR < 16386) grindAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	GrindAmp* grindAmp = (GrindAmp*) instance;

	const float* in1 = grindAmp->input[0];
	const float* in2 = grindAmp->input[1];
	float* out1 = grindAmp->output[0];
	float* out2 = grindAmp->output[1];

	const double sampleRate = grindAmp->sampleRate;
	const float tone = *grindAmp->tone;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (grindAmp->cycle > cycleEnd - 1) grindAmp->cycle = cycleEnd - 1; // sanity check

	double inputlevel = pow(*grindAmp->gain, 2);
	double samplerate = sampleRate;
	double trimEQ = 1.1 - tone;
	double toneEQ = trimEQ / 1.2;
	trimEQ /= 50.0;
	trimEQ += 0.165;
	double EQ = ((trimEQ - (toneEQ / 6.1)) / samplerate) * 22050.0;
	double BEQ = ((trimEQ + (toneEQ / 2.1)) / samplerate) * 22050.0;
	double outputlevel = *grindAmp->outputGain;
	double wet = *grindAmp->dryWet;
	double bassdrive = 1.57079633 * (2.5 - toneEQ);

	double cutoff = (18000.0 + (tone * 1000.0)) / sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low

	grindAmp->fixF[fix_freq] = grindAmp->fixE[fix_freq] = grindAmp->fixD[fix_freq] = grindAmp->fixC[fix_freq] = grindAmp->fixB[fix_freq] = grindAmp->fixA[fix_freq] = cutoff;

	grindAmp->fixA[fix_reso] = 4.46570214;
	grindAmp->fixB[fix_reso] = 1.51387132;
	grindAmp->fixC[fix_reso] = 0.93979296;
	grindAmp->fixD[fix_reso] = 0.70710678;
	grindAmp->fixE[fix_reso] = 0.52972649;
	grindAmp->fixF[fix_reso] = 0.50316379;

	double K = tan(M_PI * grindAmp->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / grindAmp->fixA[fix_reso] + K * K);
	grindAmp->fixA[fix_a0] = K * K * norm;
	grindAmp->fixA[fix_a1] = 2.0 * grindAmp->fixA[fix_a0];
	grindAmp->fixA[fix_a2] = grindAmp->fixA[fix_a0];
	grindAmp->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	grindAmp->fixA[fix_b2] = (1.0 - K / grindAmp->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * grindAmp->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / grindAmp->fixB[fix_reso] + K * K);
	grindAmp->fixB[fix_a0] = K * K * norm;
	grindAmp->fixB[fix_a1] = 2.0 * grindAmp->fixB[fix_a0];
	grindAmp->fixB[fix_a2] = grindAmp->fixB[fix_a0];
	grindAmp->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	grindAmp->fixB[fix_b2] = (1.0 - K / grindAmp->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * grindAmp->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / grindAmp->fixC[fix_reso] + K * K);
	grindAmp->fixC[fix_a0] = K * K * norm;
	grindAmp->fixC[fix_a1] = 2.0 * grindAmp->fixC[fix_a0];
	grindAmp->fixC[fix_a2] = grindAmp->fixC[fix_a0];
	grindAmp->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	grindAmp->fixC[fix_b2] = (1.0 - K / grindAmp->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * grindAmp->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / grindAmp->fixD[fix_reso] + K * K);
	grindAmp->fixD[fix_a0] = K * K * norm;
	grindAmp->fixD[fix_a1] = 2.0 * grindAmp->fixD[fix_a0];
	grindAmp->fixD[fix_a2] = grindAmp->fixD[fix_a0];
	grindAmp->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	grindAmp->fixD[fix_b2] = (1.0 - K / grindAmp->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * grindAmp->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / grindAmp->fixE[fix_reso] + K * K);
	grindAmp->fixE[fix_a0] = K * K * norm;
	grindAmp->fixE[fix_a1] = 2.0 * grindAmp->fixE[fix_a0];
	grindAmp->fixE[fix_a2] = grindAmp->fixE[fix_a0];
	grindAmp->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	grindAmp->fixE[fix_b2] = (1.0 - K / grindAmp->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * grindAmp->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / grindAmp->fixF[fix_reso] + K * K);
	grindAmp->fixF[fix_a0] = K * K * norm;
	grindAmp->fixF[fix_a1] = 2.0 * grindAmp->fixF[fix_a0];
	grindAmp->fixF[fix_a2] = grindAmp->fixF[fix_a0];
	grindAmp->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	grindAmp->fixF[fix_b2] = (1.0 - K / grindAmp->fixF[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = grindAmp->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = grindAmp->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double outSample = (inputSampleL * grindAmp->fixA[fix_a0]) + grindAmp->fixA[fix_sL1];
		grindAmp->fixA[fix_sL1] = (inputSampleL * grindAmp->fixA[fix_a1]) - (outSample * grindAmp->fixA[fix_b1]) + grindAmp->fixA[fix_sL2];
		grindAmp->fixA[fix_sL2] = (inputSampleL * grindAmp->fixA[fix_a2]) - (outSample * grindAmp->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * grindAmp->fixA[fix_a0]) + grindAmp->fixA[fix_sR1];
		grindAmp->fixA[fix_sR1] = (inputSampleR * grindAmp->fixA[fix_a1]) - (outSample * grindAmp->fixA[fix_b1]) + grindAmp->fixA[fix_sR2];
		grindAmp->fixA[fix_sR2] = (inputSampleR * grindAmp->fixA[fix_a2]) - (outSample * grindAmp->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		grindAmp->iirSampleAL = (grindAmp->iirSampleAL * (1.0 - EQ)) + (inputSampleL * EQ);
		inputSampleL = inputSampleL - (grindAmp->iirSampleAL * 0.92);
		// highpass
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		double bridgerectifier = fabs(inputSampleL);
		double inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothAL + (grindAmp->secondAL * inverse) + (grindAmp->thirdAL * bridgerectifier) + inputSampleL);
		grindAmp->thirdAL = grindAmp->secondAL;
		grindAmp->secondAL = grindAmp->smoothAL;
		grindAmp->smoothAL = inputSampleL;
		double basscatchL = inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		inputSampleR *= inputlevel;
		grindAmp->iirSampleAR = (grindAmp->iirSampleAR * (1.0 - EQ)) + (inputSampleR * EQ);
		inputSampleR = inputSampleR - (grindAmp->iirSampleAR * 0.92);
		// highpass
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		bridgerectifier = fabs(inputSampleR);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothAR + (grindAmp->secondAR * inverse) + (grindAmp->thirdAR * bridgerectifier) + inputSampleR);
		grindAmp->thirdAR = grindAmp->secondAR;
		grindAmp->secondAR = grindAmp->smoothAR;
		grindAmp->smoothAR = inputSampleR;
		double basscatchR = inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		outSample = (inputSampleL * grindAmp->fixB[fix_a0]) + grindAmp->fixB[fix_sL1];
		grindAmp->fixB[fix_sL1] = (inputSampleL * grindAmp->fixB[fix_a1]) - (outSample * grindAmp->fixB[fix_b1]) + grindAmp->fixB[fix_sL2];
		grindAmp->fixB[fix_sL2] = (inputSampleL * grindAmp->fixB[fix_a2]) - (outSample * grindAmp->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * grindAmp->fixB[fix_a0]) + grindAmp->fixB[fix_sR1];
		grindAmp->fixB[fix_sR1] = (inputSampleR * grindAmp->fixB[fix_a1]) - (outSample * grindAmp->fixB[fix_b1]) + grindAmp->fixB[fix_sR2];
		grindAmp->fixB[fix_sR2] = (inputSampleR * grindAmp->fixB[fix_a2]) - (outSample * grindAmp->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevel;
		grindAmp->iirSampleBL = (grindAmp->iirSampleBL * (1.0 - EQ)) + (inputSampleL * EQ);
		inputSampleL = inputSampleL - (grindAmp->iirSampleBL * 0.79);
		// highpass
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		bridgerectifier = fabs(inputSampleL);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothBL + (grindAmp->secondBL * inverse) + (grindAmp->thirdBL * bridgerectifier) + inputSampleL);
		grindAmp->thirdBL = grindAmp->secondBL;
		grindAmp->secondBL = grindAmp->smoothBL;
		grindAmp->smoothBL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		inputSampleR *= inputlevel;
		grindAmp->iirSampleBR = (grindAmp->iirSampleBR * (1.0 - EQ)) + (inputSampleR * EQ);
		inputSampleR = inputSampleR - (grindAmp->iirSampleBR * 0.79);
		// highpass
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		bridgerectifier = fabs(inputSampleR);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothBR + (grindAmp->secondBR * inverse) + (grindAmp->thirdBR * bridgerectifier) + inputSampleR);
		grindAmp->thirdBR = grindAmp->secondBR;
		grindAmp->secondBR = grindAmp->smoothBR;
		grindAmp->smoothBR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleCL = (grindAmp->iirSampleCL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleCL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothCL + (grindAmp->secondCL * inverse) + (grindAmp->thirdCL * bridgerectifier) + inputSampleL);
		grindAmp->thirdCL = grindAmp->secondCL;
		grindAmp->secondCL = grindAmp->smoothCL;
		grindAmp->smoothCL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleCR = (grindAmp->iirSampleCR * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleCR * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothCR + (grindAmp->secondCR * inverse) + (grindAmp->thirdCR * bridgerectifier) + inputSampleR);
		grindAmp->thirdCR = grindAmp->secondCR;
		grindAmp->secondCR = grindAmp->smoothCR;
		grindAmp->smoothCR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		outSample = (inputSampleL * grindAmp->fixC[fix_a0]) + grindAmp->fixC[fix_sL1];
		grindAmp->fixC[fix_sL1] = (inputSampleL * grindAmp->fixC[fix_a1]) - (outSample * grindAmp->fixC[fix_b1]) + grindAmp->fixC[fix_sL2];
		grindAmp->fixC[fix_sL2] = (inputSampleL * grindAmp->fixC[fix_a2]) - (outSample * grindAmp->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * grindAmp->fixC[fix_a0]) + grindAmp->fixC[fix_sR1];
		grindAmp->fixC[fix_sR1] = (inputSampleR * grindAmp->fixC[fix_a1]) - (outSample * grindAmp->fixC[fix_b1]) + grindAmp->fixC[fix_sR2];
		grindAmp->fixC[fix_sR2] = (inputSampleR * grindAmp->fixC[fix_a2]) - (outSample * grindAmp->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		grindAmp->iirSampleDL = (grindAmp->iirSampleDL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleDL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothDL + (grindAmp->secondDL * inverse) + (grindAmp->thirdDL * bridgerectifier) + inputSampleL);
		grindAmp->thirdDL = grindAmp->secondDL;
		grindAmp->secondDL = grindAmp->smoothDL;
		grindAmp->smoothDL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleDR = (grindAmp->iirSampleDR * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleDR * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothDR + (grindAmp->secondDR * inverse) + (grindAmp->thirdDR * bridgerectifier) + inputSampleR);
		grindAmp->thirdDR = grindAmp->secondDR;
		grindAmp->secondDR = grindAmp->smoothDR;
		grindAmp->smoothDR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		outSample = (inputSampleL * grindAmp->fixD[fix_a0]) + grindAmp->fixD[fix_sL1];
		grindAmp->fixD[fix_sL1] = (inputSampleL * grindAmp->fixD[fix_a1]) - (outSample * grindAmp->fixD[fix_b1]) + grindAmp->fixD[fix_sL2];
		grindAmp->fixD[fix_sL2] = (inputSampleL * grindAmp->fixD[fix_a2]) - (outSample * grindAmp->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * grindAmp->fixD[fix_a0]) + grindAmp->fixD[fix_sR1];
		grindAmp->fixD[fix_sR1] = (inputSampleR * grindAmp->fixD[fix_a1]) - (outSample * grindAmp->fixD[fix_b1]) + grindAmp->fixD[fix_sR2];
		grindAmp->fixD[fix_sR2] = (inputSampleR * grindAmp->fixD[fix_a2]) - (outSample * grindAmp->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		grindAmp->iirSampleEL = (grindAmp->iirSampleEL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleEL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothEL + (grindAmp->secondEL * inverse) + (grindAmp->thirdEL * bridgerectifier) + inputSampleL);
		grindAmp->thirdEL = grindAmp->secondEL;
		grindAmp->secondEL = grindAmp->smoothEL;
		grindAmp->smoothEL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleER = (grindAmp->iirSampleER * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleER * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothER + (grindAmp->secondER * inverse) + (grindAmp->thirdER * bridgerectifier) + inputSampleR);
		grindAmp->thirdER = grindAmp->secondER;
		grindAmp->secondER = grindAmp->smoothER;
		grindAmp->smoothER = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleFL = (grindAmp->iirSampleFL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleFL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothFL + (grindAmp->secondFL * inverse) + (grindAmp->thirdFL * bridgerectifier) + inputSampleL);
		grindAmp->thirdFL = grindAmp->secondFL;
		grindAmp->secondFL = grindAmp->smoothFL;
		grindAmp->smoothFL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleFR = (grindAmp->iirSampleFR * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleFR * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothFR + (grindAmp->secondFR * inverse) + (grindAmp->thirdFR * bridgerectifier) + inputSampleR);
		grindAmp->thirdFR = grindAmp->secondFR;
		grindAmp->secondFR = grindAmp->smoothFR;
		grindAmp->smoothFR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		outSample = (inputSampleL * grindAmp->fixE[fix_a0]) + grindAmp->fixE[fix_sL1];
		grindAmp->fixE[fix_sL1] = (inputSampleL * grindAmp->fixE[fix_a1]) - (outSample * grindAmp->fixE[fix_b1]) + grindAmp->fixE[fix_sL2];
		grindAmp->fixE[fix_sL2] = (inputSampleL * grindAmp->fixE[fix_a2]) - (outSample * grindAmp->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * grindAmp->fixE[fix_a0]) + grindAmp->fixE[fix_sR1];
		grindAmp->fixE[fix_sR1] = (inputSampleR * grindAmp->fixE[fix_a1]) - (outSample * grindAmp->fixE[fix_b1]) + grindAmp->fixE[fix_sR2];
		grindAmp->fixE[fix_sR2] = (inputSampleR * grindAmp->fixE[fix_a2]) - (outSample * grindAmp->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		grindAmp->iirSampleGL = (grindAmp->iirSampleGL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleGL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothGL + (grindAmp->secondGL * inverse) + (grindAmp->thirdGL * bridgerectifier) + inputSampleL);
		grindAmp->thirdGL = grindAmp->secondGL;
		grindAmp->secondGL = grindAmp->smoothGL;
		grindAmp->smoothGL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleGR = (grindAmp->iirSampleGR * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleGR * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothGR + (grindAmp->secondGR * inverse) + (grindAmp->thirdGR * bridgerectifier) + inputSampleR);
		grindAmp->thirdGR = grindAmp->secondGR;
		grindAmp->secondGR = grindAmp->smoothGR;
		grindAmp->smoothGR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleHL = (grindAmp->iirSampleHL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleHL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothHL + (grindAmp->secondHL * inverse) + (grindAmp->thirdHL * bridgerectifier) + inputSampleL);
		grindAmp->thirdHL = grindAmp->secondHL;
		grindAmp->secondHL = grindAmp->smoothHL;
		grindAmp->smoothHL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleHR = (grindAmp->iirSampleHR * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleHR * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothHR + (grindAmp->secondHR * inverse) + (grindAmp->thirdHR * bridgerectifier) + inputSampleR);
		grindAmp->thirdHR = grindAmp->secondHR;
		grindAmp->secondHR = grindAmp->smoothHR;
		grindAmp->smoothHR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		outSample = (inputSampleL * grindAmp->fixF[fix_a0]) + grindAmp->fixF[fix_sL1];
		grindAmp->fixF[fix_sL1] = (inputSampleL * grindAmp->fixF[fix_a1]) - (outSample * grindAmp->fixF[fix_b1]) + grindAmp->fixF[fix_sL2];
		grindAmp->fixF[fix_sL2] = (inputSampleL * grindAmp->fixF[fix_a2]) - (outSample * grindAmp->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * grindAmp->fixF[fix_a0]) + grindAmp->fixF[fix_sR1];
		grindAmp->fixF[fix_sR1] = (inputSampleR * grindAmp->fixF[fix_a1]) - (outSample * grindAmp->fixF[fix_b1]) + grindAmp->fixF[fix_sR2];
		grindAmp->fixF[fix_sR2] = (inputSampleR * grindAmp->fixF[fix_a2]) - (outSample * grindAmp->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		grindAmp->iirSampleIL = (grindAmp->iirSampleIL * (1.0 - BEQ)) + (basscatchL * BEQ);
		basscatchL = grindAmp->iirSampleIL * bassdrive;
		bridgerectifier = fabs(basscatchL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchL > 0.0) basscatchL = bridgerectifier;
		else basscatchL = -bridgerectifier;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothIL + (grindAmp->secondIL * inverse) + (grindAmp->thirdIL * bridgerectifier) + inputSampleL);
		grindAmp->thirdIL = grindAmp->secondIL;
		grindAmp->secondIL = grindAmp->smoothIL;
		grindAmp->smoothIL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass
		bridgerectifier = fabs(inputSampleL);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothJL + (grindAmp->secondJL * inverse) + (grindAmp->thirdJL * bridgerectifier) + inputSampleL);
		grindAmp->thirdJL = grindAmp->secondJL;
		grindAmp->secondJL = grindAmp->smoothJL;
		grindAmp->smoothJL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass
		bridgerectifier = fabs(inputSampleL);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothKL + (grindAmp->secondKL * inverse) + (grindAmp->thirdKL * bridgerectifier) + inputSampleL);
		grindAmp->thirdKL = grindAmp->secondKL;
		grindAmp->secondKL = grindAmp->smoothKL;
		grindAmp->smoothKL = inputSampleL;
		inputSampleL = bridgerectifier;
		// three-sample averaging lowpass

		grindAmp->iirSampleIR = (grindAmp->iirSampleIR * (1.0 - BEQ)) + (basscatchR * BEQ);
		basscatchR = grindAmp->iirSampleIR * bassdrive;
		bridgerectifier = fabs(basscatchR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (basscatchR > 0.0) basscatchR = bridgerectifier;
		else basscatchR = -bridgerectifier;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// overdrive
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothIR + (grindAmp->secondIR * inverse) + (grindAmp->thirdIR * bridgerectifier) + inputSampleR);
		grindAmp->thirdIR = grindAmp->secondIR;
		grindAmp->secondIR = grindAmp->smoothIR;
		grindAmp->smoothIR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass
		bridgerectifier = fabs(inputSampleR);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothJR + (grindAmp->secondJR * inverse) + (grindAmp->thirdJR * bridgerectifier) + inputSampleR);
		grindAmp->thirdJR = grindAmp->secondJR;
		grindAmp->secondJR = grindAmp->smoothJR;
		grindAmp->smoothJR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass
		bridgerectifier = fabs(inputSampleR);
		inverse = (bridgerectifier + 1.0) / 2.0;
		bridgerectifier = (grindAmp->smoothKR + (grindAmp->secondKR * inverse) + (grindAmp->thirdKR * bridgerectifier) + inputSampleR);
		grindAmp->thirdKR = grindAmp->secondKR;
		grindAmp->secondKR = grindAmp->smoothKR;
		grindAmp->smoothKR = inputSampleR;
		inputSampleR = bridgerectifier;
		// three-sample averaging lowpass

		basscatchL /= 2.0;
		inputSampleL = (inputSampleL * toneEQ) + basscatchL;
		// extra lowpass for 4*12" speakers
		basscatchR /= 2.0;
		inputSampleR = (inputSampleR * toneEQ) + basscatchR;
		// extra lowpass for 4*12" speakers

		bridgerectifier = fabs(inputSampleL * outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		inputSampleL += basscatchL;
		// split bass between overdrive and clean
		inputSampleL /= (1.0 + toneEQ);

		bridgerectifier = fabs(inputSampleR * outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		inputSampleR += basscatchR;
		// split bass between overdrive and clean
		inputSampleR /= (1.0 + toneEQ);

		double randy = (((double) grindAmp->fpdL / UINT32_MAX) * 0.061);
		inputSampleL = ((inputSampleL * (1 - randy)) + (grindAmp->storeSampleL * randy)) * outputlevel;
		grindAmp->storeSampleL = inputSampleL;

		randy = (((double) grindAmp->fpdR / UINT32_MAX) * 0.061);
		inputSampleR = ((inputSampleR * (1 - randy)) + (grindAmp->storeSampleR * randy)) * outputlevel;
		grindAmp->storeSampleR = inputSampleR;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider
		// amp

		grindAmp->cycle++;
		if (grindAmp->cycle == cycleEnd) {

			double temp = (inputSampleL + grindAmp->smoothCabAL) / 3.0;
			grindAmp->smoothCabAL = inputSampleL;
			inputSampleL = temp;

			grindAmp->bL[83] = grindAmp->bL[82];
			grindAmp->bL[82] = grindAmp->bL[81];
			grindAmp->bL[81] = grindAmp->bL[80];
			grindAmp->bL[80] = grindAmp->bL[79];
			grindAmp->bL[79] = grindAmp->bL[78];
			grindAmp->bL[78] = grindAmp->bL[77];
			grindAmp->bL[77] = grindAmp->bL[76];
			grindAmp->bL[76] = grindAmp->bL[75];
			grindAmp->bL[75] = grindAmp->bL[74];
			grindAmp->bL[74] = grindAmp->bL[73];
			grindAmp->bL[73] = grindAmp->bL[72];
			grindAmp->bL[72] = grindAmp->bL[71];
			grindAmp->bL[71] = grindAmp->bL[70];
			grindAmp->bL[70] = grindAmp->bL[69];
			grindAmp->bL[69] = grindAmp->bL[68];
			grindAmp->bL[68] = grindAmp->bL[67];
			grindAmp->bL[67] = grindAmp->bL[66];
			grindAmp->bL[66] = grindAmp->bL[65];
			grindAmp->bL[65] = grindAmp->bL[64];
			grindAmp->bL[64] = grindAmp->bL[63];
			grindAmp->bL[63] = grindAmp->bL[62];
			grindAmp->bL[62] = grindAmp->bL[61];
			grindAmp->bL[61] = grindAmp->bL[60];
			grindAmp->bL[60] = grindAmp->bL[59];
			grindAmp->bL[59] = grindAmp->bL[58];
			grindAmp->bL[58] = grindAmp->bL[57];
			grindAmp->bL[57] = grindAmp->bL[56];
			grindAmp->bL[56] = grindAmp->bL[55];
			grindAmp->bL[55] = grindAmp->bL[54];
			grindAmp->bL[54] = grindAmp->bL[53];
			grindAmp->bL[53] = grindAmp->bL[52];
			grindAmp->bL[52] = grindAmp->bL[51];
			grindAmp->bL[51] = grindAmp->bL[50];
			grindAmp->bL[50] = grindAmp->bL[49];
			grindAmp->bL[49] = grindAmp->bL[48];
			grindAmp->bL[48] = grindAmp->bL[47];
			grindAmp->bL[47] = grindAmp->bL[46];
			grindAmp->bL[46] = grindAmp->bL[45];
			grindAmp->bL[45] = grindAmp->bL[44];
			grindAmp->bL[44] = grindAmp->bL[43];
			grindAmp->bL[43] = grindAmp->bL[42];
			grindAmp->bL[42] = grindAmp->bL[41];
			grindAmp->bL[41] = grindAmp->bL[40];
			grindAmp->bL[40] = grindAmp->bL[39];
			grindAmp->bL[39] = grindAmp->bL[38];
			grindAmp->bL[38] = grindAmp->bL[37];
			grindAmp->bL[37] = grindAmp->bL[36];
			grindAmp->bL[36] = grindAmp->bL[35];
			grindAmp->bL[35] = grindAmp->bL[34];
			grindAmp->bL[34] = grindAmp->bL[33];
			grindAmp->bL[33] = grindAmp->bL[32];
			grindAmp->bL[32] = grindAmp->bL[31];
			grindAmp->bL[31] = grindAmp->bL[30];
			grindAmp->bL[30] = grindAmp->bL[29];
			grindAmp->bL[29] = grindAmp->bL[28];
			grindAmp->bL[28] = grindAmp->bL[27];
			grindAmp->bL[27] = grindAmp->bL[26];
			grindAmp->bL[26] = grindAmp->bL[25];
			grindAmp->bL[25] = grindAmp->bL[24];
			grindAmp->bL[24] = grindAmp->bL[23];
			grindAmp->bL[23] = grindAmp->bL[22];
			grindAmp->bL[22] = grindAmp->bL[21];
			grindAmp->bL[21] = grindAmp->bL[20];
			grindAmp->bL[20] = grindAmp->bL[19];
			grindAmp->bL[19] = grindAmp->bL[18];
			grindAmp->bL[18] = grindAmp->bL[17];
			grindAmp->bL[17] = grindAmp->bL[16];
			grindAmp->bL[16] = grindAmp->bL[15];
			grindAmp->bL[15] = grindAmp->bL[14];
			grindAmp->bL[14] = grindAmp->bL[13];
			grindAmp->bL[13] = grindAmp->bL[12];
			grindAmp->bL[12] = grindAmp->bL[11];
			grindAmp->bL[11] = grindAmp->bL[10];
			grindAmp->bL[10] = grindAmp->bL[9];
			grindAmp->bL[9] = grindAmp->bL[8];
			grindAmp->bL[8] = grindAmp->bL[7];
			grindAmp->bL[7] = grindAmp->bL[6];
			grindAmp->bL[6] = grindAmp->bL[5];
			grindAmp->bL[5] = grindAmp->bL[4];
			grindAmp->bL[4] = grindAmp->bL[3];
			grindAmp->bL[3] = grindAmp->bL[2];
			grindAmp->bL[2] = grindAmp->bL[1];
			grindAmp->bL[1] = grindAmp->bL[0];
			grindAmp->bL[0] = inputSampleL;
			inputSampleL += (grindAmp->bL[1] * (1.29550481610475132 + (0.19713872057074355 * fabs(grindAmp->bL[1]))));
			inputSampleL += (grindAmp->bL[2] * (1.42302569895462616 + (0.30599505521284787 * fabs(grindAmp->bL[2]))));
			inputSampleL += (grindAmp->bL[3] * (1.28728195804197565 + (0.23168333460446133 * fabs(grindAmp->bL[3]))));
			inputSampleL += (grindAmp->bL[4] * (0.88553784290822690 + (0.14263256172918892 * fabs(grindAmp->bL[4]))));
			inputSampleL += (grindAmp->bL[5] * (0.37129054918432319 + (0.00150040944205920 * fabs(grindAmp->bL[5]))));
			inputSampleL -= (grindAmp->bL[6] * (0.12150959412556320 + (0.32776273620569107 * fabs(grindAmp->bL[6]))));
			inputSampleL -= (grindAmp->bL[7] * (0.44900065463203775 + (0.74101214925298819 * fabs(grindAmp->bL[7]))));
			inputSampleL -= (grindAmp->bL[8] * (0.54058781908186482 + (1.07821707459008387 * fabs(grindAmp->bL[8]))));
			inputSampleL -= (grindAmp->bL[9] * (0.49361966401791391 + (1.23540109014850508 * fabs(grindAmp->bL[9]))));
			inputSampleL -= (grindAmp->bL[10] * (0.39819495093078133 + (1.11247213730917749 * fabs(grindAmp->bL[10]))));
			inputSampleL -= (grindAmp->bL[11] * (0.31379279985435521 + (0.80330360359638298 * fabs(grindAmp->bL[11]))));
			inputSampleL -= (grindAmp->bL[12] * (0.30744359242808555 + (0.42132528876858205 * fabs(grindAmp->bL[12]))));
			inputSampleL -= (grindAmp->bL[13] * (0.33943170284673974 + (0.09183418349389982 * fabs(grindAmp->bL[13]))));
			inputSampleL -= (grindAmp->bL[14] * (0.33838775119286391 - (0.06453051658561271 * fabs(grindAmp->bL[14]))));
			inputSampleL -= (grindAmp->bL[15] * (0.30682305697961665 - (0.09549380253249232 * fabs(grindAmp->bL[15]))));
			inputSampleL -= (grindAmp->bL[16] * (0.23408741339295336 - (0.08083404732361277 * fabs(grindAmp->bL[16]))));
			inputSampleL -= (grindAmp->bL[17] * (0.10411746814025019 + (0.00253651281245780 * fabs(grindAmp->bL[17]))));
			inputSampleL += (grindAmp->bL[18] * (0.00133623776084696 - (0.04447267870865820 * fabs(grindAmp->bL[18]))));
			inputSampleL += (grindAmp->bL[19] * (0.02461903992114161 + (0.07530671732655550 * fabs(grindAmp->bL[19]))));
			inputSampleL += (grindAmp->bL[20] * (0.02086715842475373 + (0.22795860236804899 * fabs(grindAmp->bL[20]))));
			inputSampleL += (grindAmp->bL[21] * (0.02761433637100917 + (0.26108320417844094 * fabs(grindAmp->bL[21]))));
			inputSampleL += (grindAmp->bL[22] * (0.04475285369162533 + (0.19160705011061663 * fabs(grindAmp->bL[22]))));
			inputSampleL += (grindAmp->bL[23] * (0.09447338372862381 + (0.03681550508743799 * fabs(grindAmp->bL[23]))));
			inputSampleL += (grindAmp->bL[24] * (0.13445890343722280 - (0.13713036462146147 * fabs(grindAmp->bL[24]))));
			inputSampleL += (grindAmp->bL[25] * (0.13872868945088121 - (0.22401242373298191 * fabs(grindAmp->bL[25]))));
			inputSampleL += (grindAmp->bL[26] * (0.14915650097434549 - (0.26718804981526367 * fabs(grindAmp->bL[26]))));
			inputSampleL += (grindAmp->bL[27] * (0.12766643217091783 - (0.27745664795660430 * fabs(grindAmp->bL[27]))));
			inputSampleL += (grindAmp->bL[28] * (0.03675849788393101 - (0.18338278173550679 * fabs(grindAmp->bL[28]))));
			inputSampleL -= (grindAmp->bL[29] * (0.06307306864232835 + (0.06089480869040766 * fabs(grindAmp->bL[29]))));
			inputSampleL -= (grindAmp->bL[30] * (0.14947389348962944 + (0.04642103054798480 * fabs(grindAmp->bL[30]))));
			inputSampleL -= (grindAmp->bL[31] * (0.25235266566401526 + (0.08423062596460507 * fabs(grindAmp->bL[31]))));
			inputSampleL -= (grindAmp->bL[32] * (0.33496344048679683 + (0.09808328256677995 * fabs(grindAmp->bL[32]))));
			inputSampleL -= (grindAmp->bL[33] * (0.36590030482175445 + (0.10622650888958179 * fabs(grindAmp->bL[33]))));
			inputSampleL -= (grindAmp->bL[34] * (0.35015197011464372 + (0.08982043516016047 * fabs(grindAmp->bL[34]))));
			inputSampleL -= (grindAmp->bL[35] * (0.26808437585665090 + (0.00735561860229533 * fabs(grindAmp->bL[35]))));
			inputSampleL -= (grindAmp->bL[36] * (0.11624318543291220 - (0.07142484314510467 * fabs(grindAmp->bL[36]))));
			inputSampleL += (grindAmp->bL[37] * (0.05617084165377551 + (0.11785854050350089 * fabs(grindAmp->bL[37]))));
			inputSampleL += (grindAmp->bL[38] * (0.20540028692589385 + (0.20479174663329586 * fabs(grindAmp->bL[38]))));
			inputSampleL += (grindAmp->bL[39] * (0.30455415003043818 + (0.29074864580096849 * fabs(grindAmp->bL[39]))));
			inputSampleL += (grindAmp->bL[40] * (0.33810750937829476 + (0.29182307921316802 * fabs(grindAmp->bL[40]))));
			inputSampleL += (grindAmp->bL[41] * (0.31936133365277430 + (0.26535537727394987 * fabs(grindAmp->bL[41]))));
			inputSampleL += (grindAmp->bL[42] * (0.27388548321981876 + (0.19735049990538350 * fabs(grindAmp->bL[42]))));
			inputSampleL += (grindAmp->bL[43] * (0.21454597517994098 + (0.06415909270247236 * fabs(grindAmp->bL[43]))));
			inputSampleL += (grindAmp->bL[44] * (0.15001045817707717 - (0.03831118543404573 * fabs(grindAmp->bL[44]))));
			inputSampleL += (grindAmp->bL[45] * (0.07283437284653138 - (0.09281952429543777 * fabs(grindAmp->bL[45]))));
			inputSampleL -= (grindAmp->bL[46] * (0.03917872184241358 + (0.14306291461398810 * fabs(grindAmp->bL[46]))));
			inputSampleL -= (grindAmp->bL[47] * (0.16695932032148642 + (0.19138995946950504 * fabs(grindAmp->bL[47]))));
			inputSampleL -= (grindAmp->bL[48] * (0.27055854466909462 + (0.22531296466343192 * fabs(grindAmp->bL[48]))));
			inputSampleL -= (grindAmp->bL[49] * (0.33256357307578271 + (0.23305840475692102 * fabs(grindAmp->bL[49]))));
			inputSampleL -= (grindAmp->bL[50] * (0.33459770116834442 + (0.24091822618917569 * fabs(grindAmp->bL[50]))));
			inputSampleL -= (grindAmp->bL[51] * (0.27156687236338090 + (0.24062938573512443 * fabs(grindAmp->bL[51]))));
			inputSampleL -= (grindAmp->bL[52] * (0.17197093288412094 + (0.19083085091993421 * fabs(grindAmp->bL[52]))));
			inputSampleL -= (grindAmp->bL[53] * (0.06738628195910543 + (0.10268609751019808 * fabs(grindAmp->bL[53]))));
			inputSampleL += (grindAmp->bL[54] * (0.00222429218204290 + (0.01439664435720548 * fabs(grindAmp->bL[54]))));
			inputSampleL += (grindAmp->bL[55] * (0.01346992803494091 + (0.15947137113534526 * fabs(grindAmp->bL[55]))));
			inputSampleL -= (grindAmp->bL[56] * (0.02038911881377448 - (0.26763170752416160 * fabs(grindAmp->bL[56]))));
			inputSampleL -= (grindAmp->bL[57] * (0.08233579178189687 - (0.29415931086406055 * fabs(grindAmp->bL[57]))));
			inputSampleL -= (grindAmp->bL[58] * (0.15447855089824883 - (0.26489186990840807 * fabs(grindAmp->bL[58]))));
			inputSampleL -= (grindAmp->bL[59] * (0.20518281113362655 - (0.16135382257522859 * fabs(grindAmp->bL[59]))));
			inputSampleL -= (grindAmp->bL[60] * (0.22244686050232007 + (0.00847180390247432 * fabs(grindAmp->bL[60]))));
			inputSampleL -= (grindAmp->bL[61] * (0.21849243134998034 + (0.14460595245753741 * fabs(grindAmp->bL[61]))));
			inputSampleL -= (grindAmp->bL[62] * (0.20256105734574054 + (0.18932793221831667 * fabs(grindAmp->bL[62]))));
			inputSampleL -= (grindAmp->bL[63] * (0.18604070054295399 + (0.17250665610927965 * fabs(grindAmp->bL[63]))));
			inputSampleL -= (grindAmp->bL[64] * (0.17222844322058231 + (0.12992472027850357 * fabs(grindAmp->bL[64]))));
			inputSampleL -= (grindAmp->bL[65] * (0.14447856616566443 + (0.09089219002147308 * fabs(grindAmp->bL[65]))));
			inputSampleL -= (grindAmp->bL[66] * (0.10385520794251019 + (0.08600465834570559 * fabs(grindAmp->bL[66]))));
			inputSampleL -= (grindAmp->bL[67] * (0.07124435678265063 + (0.09071532210549428 * fabs(grindAmp->bL[67]))));
			inputSampleL -= (grindAmp->bL[68] * (0.05216857461197572 + (0.06794061706070262 * fabs(grindAmp->bL[68]))));
			inputSampleL -= (grindAmp->bL[69] * (0.05235381920184123 + (0.02818101717909346 * fabs(grindAmp->bL[69]))));
			inputSampleL -= (grindAmp->bL[70] * (0.07569701245553526 - (0.00634228544764946 * fabs(grindAmp->bL[70]))));
			inputSampleL -= (grindAmp->bL[71] * (0.10320125382718826 - (0.02751486906644141 * fabs(grindAmp->bL[71]))));
			inputSampleL -= (grindAmp->bL[72] * (0.12122120969079088 - (0.05434007312178933 * fabs(grindAmp->bL[72]))));
			inputSampleL -= (grindAmp->bL[73] * (0.13438969117200902 - (0.09135218559713874 * fabs(grindAmp->bL[73]))));
			inputSampleL -= (grindAmp->bL[74] * (0.13534390437529981 - (0.10437672041458675 * fabs(grindAmp->bL[74]))));
			inputSampleL -= (grindAmp->bL[75] * (0.11424128854188388 - (0.08693450726462598 * fabs(grindAmp->bL[75]))));
			inputSampleL -= (grindAmp->bL[76] * (0.08166894518596159 - (0.06949989431475120 * fabs(grindAmp->bL[76]))));
			inputSampleL -= (grindAmp->bL[77] * (0.04293976378555305 - (0.05718625137421843 * fabs(grindAmp->bL[77]))));
			inputSampleL += (grindAmp->bL[78] * (0.00933076320644409 + (0.01728285211520138 * fabs(grindAmp->bL[78]))));
			inputSampleL += (grindAmp->bL[79] * (0.06450430362918153 - (0.02492994833691022 * fabs(grindAmp->bL[79]))));
			inputSampleL += (grindAmp->bL[80] * (0.10187400687649277 - (0.03578455940532403 * fabs(grindAmp->bL[80]))));
			inputSampleL += (grindAmp->bL[81] * (0.11039763294094571 - (0.03995523517573508 * fabs(grindAmp->bL[81]))));
			inputSampleL += (grindAmp->bL[82] * (0.08557960776024547 - (0.03482514309492527 * fabs(grindAmp->bL[82]))));
			inputSampleL += (grindAmp->bL[83] * (0.02730881850805332 - (0.00514750108411127 * fabs(grindAmp->bL[83]))));

			temp = (inputSampleL + grindAmp->smoothCabBL) / 3.0;
			grindAmp->smoothCabBL = inputSampleL;
			inputSampleL = temp / 4.0;

			randy = (((double) grindAmp->fpdL / UINT32_MAX) * 0.044);
			drySampleL = ((((inputSampleL * (1 - randy)) + (grindAmp->lastCabSampleL * randy)) * wet) + (drySampleL * (1.0 - wet))) * outputlevel;
			grindAmp->lastCabSampleL = inputSampleL;
			inputSampleL = drySampleL; // cab L

			temp = (inputSampleR + grindAmp->smoothCabAR) / 3.0;
			grindAmp->smoothCabAR = inputSampleR;
			inputSampleR = temp;

			grindAmp->bR[83] = grindAmp->bR[82];
			grindAmp->bR[82] = grindAmp->bR[81];
			grindAmp->bR[81] = grindAmp->bR[80];
			grindAmp->bR[80] = grindAmp->bR[79];
			grindAmp->bR[79] = grindAmp->bR[78];
			grindAmp->bR[78] = grindAmp->bR[77];
			grindAmp->bR[77] = grindAmp->bR[76];
			grindAmp->bR[76] = grindAmp->bR[75];
			grindAmp->bR[75] = grindAmp->bR[74];
			grindAmp->bR[74] = grindAmp->bR[73];
			grindAmp->bR[73] = grindAmp->bR[72];
			grindAmp->bR[72] = grindAmp->bR[71];
			grindAmp->bR[71] = grindAmp->bR[70];
			grindAmp->bR[70] = grindAmp->bR[69];
			grindAmp->bR[69] = grindAmp->bR[68];
			grindAmp->bR[68] = grindAmp->bR[67];
			grindAmp->bR[67] = grindAmp->bR[66];
			grindAmp->bR[66] = grindAmp->bR[65];
			grindAmp->bR[65] = grindAmp->bR[64];
			grindAmp->bR[64] = grindAmp->bR[63];
			grindAmp->bR[63] = grindAmp->bR[62];
			grindAmp->bR[62] = grindAmp->bR[61];
			grindAmp->bR[61] = grindAmp->bR[60];
			grindAmp->bR[60] = grindAmp->bR[59];
			grindAmp->bR[59] = grindAmp->bR[58];
			grindAmp->bR[58] = grindAmp->bR[57];
			grindAmp->bR[57] = grindAmp->bR[56];
			grindAmp->bR[56] = grindAmp->bR[55];
			grindAmp->bR[55] = grindAmp->bR[54];
			grindAmp->bR[54] = grindAmp->bR[53];
			grindAmp->bR[53] = grindAmp->bR[52];
			grindAmp->bR[52] = grindAmp->bR[51];
			grindAmp->bR[51] = grindAmp->bR[50];
			grindAmp->bR[50] = grindAmp->bR[49];
			grindAmp->bR[49] = grindAmp->bR[48];
			grindAmp->bR[48] = grindAmp->bR[47];
			grindAmp->bR[47] = grindAmp->bR[46];
			grindAmp->bR[46] = grindAmp->bR[45];
			grindAmp->bR[45] = grindAmp->bR[44];
			grindAmp->bR[44] = grindAmp->bR[43];
			grindAmp->bR[43] = grindAmp->bR[42];
			grindAmp->bR[42] = grindAmp->bR[41];
			grindAmp->bR[41] = grindAmp->bR[40];
			grindAmp->bR[40] = grindAmp->bR[39];
			grindAmp->bR[39] = grindAmp->bR[38];
			grindAmp->bR[38] = grindAmp->bR[37];
			grindAmp->bR[37] = grindAmp->bR[36];
			grindAmp->bR[36] = grindAmp->bR[35];
			grindAmp->bR[35] = grindAmp->bR[34];
			grindAmp->bR[34] = grindAmp->bR[33];
			grindAmp->bR[33] = grindAmp->bR[32];
			grindAmp->bR[32] = grindAmp->bR[31];
			grindAmp->bR[31] = grindAmp->bR[30];
			grindAmp->bR[30] = grindAmp->bR[29];
			grindAmp->bR[29] = grindAmp->bR[28];
			grindAmp->bR[28] = grindAmp->bR[27];
			grindAmp->bR[27] = grindAmp->bR[26];
			grindAmp->bR[26] = grindAmp->bR[25];
			grindAmp->bR[25] = grindAmp->bR[24];
			grindAmp->bR[24] = grindAmp->bR[23];
			grindAmp->bR[23] = grindAmp->bR[22];
			grindAmp->bR[22] = grindAmp->bR[21];
			grindAmp->bR[21] = grindAmp->bR[20];
			grindAmp->bR[20] = grindAmp->bR[19];
			grindAmp->bR[19] = grindAmp->bR[18];
			grindAmp->bR[18] = grindAmp->bR[17];
			grindAmp->bR[17] = grindAmp->bR[16];
			grindAmp->bR[16] = grindAmp->bR[15];
			grindAmp->bR[15] = grindAmp->bR[14];
			grindAmp->bR[14] = grindAmp->bR[13];
			grindAmp->bR[13] = grindAmp->bR[12];
			grindAmp->bR[12] = grindAmp->bR[11];
			grindAmp->bR[11] = grindAmp->bR[10];
			grindAmp->bR[10] = grindAmp->bR[9];
			grindAmp->bR[9] = grindAmp->bR[8];
			grindAmp->bR[8] = grindAmp->bR[7];
			grindAmp->bR[7] = grindAmp->bR[6];
			grindAmp->bR[6] = grindAmp->bR[5];
			grindAmp->bR[5] = grindAmp->bR[4];
			grindAmp->bR[4] = grindAmp->bR[3];
			grindAmp->bR[3] = grindAmp->bR[2];
			grindAmp->bR[2] = grindAmp->bR[1];
			grindAmp->bR[1] = grindAmp->bR[0];
			grindAmp->bR[0] = inputSampleR;
			inputSampleR += (grindAmp->bR[1] * (1.29550481610475132 + (0.19713872057074355 * fabs(grindAmp->bR[1]))));
			inputSampleR += (grindAmp->bR[2] * (1.42302569895462616 + (0.30599505521284787 * fabs(grindAmp->bR[2]))));
			inputSampleR += (grindAmp->bR[3] * (1.28728195804197565 + (0.23168333460446133 * fabs(grindAmp->bR[3]))));
			inputSampleR += (grindAmp->bR[4] * (0.88553784290822690 + (0.14263256172918892 * fabs(grindAmp->bR[4]))));
			inputSampleR += (grindAmp->bR[5] * (0.37129054918432319 + (0.00150040944205920 * fabs(grindAmp->bR[5]))));
			inputSampleR -= (grindAmp->bR[6] * (0.12150959412556320 + (0.32776273620569107 * fabs(grindAmp->bR[6]))));
			inputSampleR -= (grindAmp->bR[7] * (0.44900065463203775 + (0.74101214925298819 * fabs(grindAmp->bR[7]))));
			inputSampleR -= (grindAmp->bR[8] * (0.54058781908186482 + (1.07821707459008387 * fabs(grindAmp->bR[8]))));
			inputSampleR -= (grindAmp->bR[9] * (0.49361966401791391 + (1.23540109014850508 * fabs(grindAmp->bR[9]))));
			inputSampleR -= (grindAmp->bR[10] * (0.39819495093078133 + (1.11247213730917749 * fabs(grindAmp->bR[10]))));
			inputSampleR -= (grindAmp->bR[11] * (0.31379279985435521 + (0.80330360359638298 * fabs(grindAmp->bR[11]))));
			inputSampleR -= (grindAmp->bR[12] * (0.30744359242808555 + (0.42132528876858205 * fabs(grindAmp->bR[12]))));
			inputSampleR -= (grindAmp->bR[13] * (0.33943170284673974 + (0.09183418349389982 * fabs(grindAmp->bR[13]))));
			inputSampleR -= (grindAmp->bR[14] * (0.33838775119286391 - (0.06453051658561271 * fabs(grindAmp->bR[14]))));
			inputSampleR -= (grindAmp->bR[15] * (0.30682305697961665 - (0.09549380253249232 * fabs(grindAmp->bR[15]))));
			inputSampleR -= (grindAmp->bR[16] * (0.23408741339295336 - (0.08083404732361277 * fabs(grindAmp->bR[16]))));
			inputSampleR -= (grindAmp->bR[17] * (0.10411746814025019 + (0.00253651281245780 * fabs(grindAmp->bR[17]))));
			inputSampleR += (grindAmp->bR[18] * (0.00133623776084696 - (0.04447267870865820 * fabs(grindAmp->bR[18]))));
			inputSampleR += (grindAmp->bR[19] * (0.02461903992114161 + (0.07530671732655550 * fabs(grindAmp->bR[19]))));
			inputSampleR += (grindAmp->bR[20] * (0.02086715842475373 + (0.22795860236804899 * fabs(grindAmp->bR[20]))));
			inputSampleR += (grindAmp->bR[21] * (0.02761433637100917 + (0.26108320417844094 * fabs(grindAmp->bR[21]))));
			inputSampleR += (grindAmp->bR[22] * (0.04475285369162533 + (0.19160705011061663 * fabs(grindAmp->bR[22]))));
			inputSampleR += (grindAmp->bR[23] * (0.09447338372862381 + (0.03681550508743799 * fabs(grindAmp->bR[23]))));
			inputSampleR += (grindAmp->bR[24] * (0.13445890343722280 - (0.13713036462146147 * fabs(grindAmp->bR[24]))));
			inputSampleR += (grindAmp->bR[25] * (0.13872868945088121 - (0.22401242373298191 * fabs(grindAmp->bR[25]))));
			inputSampleR += (grindAmp->bR[26] * (0.14915650097434549 - (0.26718804981526367 * fabs(grindAmp->bR[26]))));
			inputSampleR += (grindAmp->bR[27] * (0.12766643217091783 - (0.27745664795660430 * fabs(grindAmp->bR[27]))));
			inputSampleR += (grindAmp->bR[28] * (0.03675849788393101 - (0.18338278173550679 * fabs(grindAmp->bR[28]))));
			inputSampleR -= (grindAmp->bR[29] * (0.06307306864232835 + (0.06089480869040766 * fabs(grindAmp->bR[29]))));
			inputSampleR -= (grindAmp->bR[30] * (0.14947389348962944 + (0.04642103054798480 * fabs(grindAmp->bR[30]))));
			inputSampleR -= (grindAmp->bR[31] * (0.25235266566401526 + (0.08423062596460507 * fabs(grindAmp->bR[31]))));
			inputSampleR -= (grindAmp->bR[32] * (0.33496344048679683 + (0.09808328256677995 * fabs(grindAmp->bR[32]))));
			inputSampleR -= (grindAmp->bR[33] * (0.36590030482175445 + (0.10622650888958179 * fabs(grindAmp->bR[33]))));
			inputSampleR -= (grindAmp->bR[34] * (0.35015197011464372 + (0.08982043516016047 * fabs(grindAmp->bR[34]))));
			inputSampleR -= (grindAmp->bR[35] * (0.26808437585665090 + (0.00735561860229533 * fabs(grindAmp->bR[35]))));
			inputSampleR -= (grindAmp->bR[36] * (0.11624318543291220 - (0.07142484314510467 * fabs(grindAmp->bR[36]))));
			inputSampleR += (grindAmp->bR[37] * (0.05617084165377551 + (0.11785854050350089 * fabs(grindAmp->bR[37]))));
			inputSampleR += (grindAmp->bR[38] * (0.20540028692589385 + (0.20479174663329586 * fabs(grindAmp->bR[38]))));
			inputSampleR += (grindAmp->bR[39] * (0.30455415003043818 + (0.29074864580096849 * fabs(grindAmp->bR[39]))));
			inputSampleR += (grindAmp->bR[40] * (0.33810750937829476 + (0.29182307921316802 * fabs(grindAmp->bR[40]))));
			inputSampleR += (grindAmp->bR[41] * (0.31936133365277430 + (0.26535537727394987 * fabs(grindAmp->bR[41]))));
			inputSampleR += (grindAmp->bR[42] * (0.27388548321981876 + (0.19735049990538350 * fabs(grindAmp->bR[42]))));
			inputSampleR += (grindAmp->bR[43] * (0.21454597517994098 + (0.06415909270247236 * fabs(grindAmp->bR[43]))));
			inputSampleR += (grindAmp->bR[44] * (0.15001045817707717 - (0.03831118543404573 * fabs(grindAmp->bR[44]))));
			inputSampleR += (grindAmp->bR[45] * (0.07283437284653138 - (0.09281952429543777 * fabs(grindAmp->bR[45]))));
			inputSampleR -= (grindAmp->bR[46] * (0.03917872184241358 + (0.14306291461398810 * fabs(grindAmp->bR[46]))));
			inputSampleR -= (grindAmp->bR[47] * (0.16695932032148642 + (0.19138995946950504 * fabs(grindAmp->bR[47]))));
			inputSampleR -= (grindAmp->bR[48] * (0.27055854466909462 + (0.22531296466343192 * fabs(grindAmp->bR[48]))));
			inputSampleR -= (grindAmp->bR[49] * (0.33256357307578271 + (0.23305840475692102 * fabs(grindAmp->bR[49]))));
			inputSampleR -= (grindAmp->bR[50] * (0.33459770116834442 + (0.24091822618917569 * fabs(grindAmp->bR[50]))));
			inputSampleR -= (grindAmp->bR[51] * (0.27156687236338090 + (0.24062938573512443 * fabs(grindAmp->bR[51]))));
			inputSampleR -= (grindAmp->bR[52] * (0.17197093288412094 + (0.19083085091993421 * fabs(grindAmp->bR[52]))));
			inputSampleR -= (grindAmp->bR[53] * (0.06738628195910543 + (0.10268609751019808 * fabs(grindAmp->bR[53]))));
			inputSampleR += (grindAmp->bR[54] * (0.00222429218204290 + (0.01439664435720548 * fabs(grindAmp->bR[54]))));
			inputSampleR += (grindAmp->bR[55] * (0.01346992803494091 + (0.15947137113534526 * fabs(grindAmp->bR[55]))));
			inputSampleR -= (grindAmp->bR[56] * (0.02038911881377448 - (0.26763170752416160 * fabs(grindAmp->bR[56]))));
			inputSampleR -= (grindAmp->bR[57] * (0.08233579178189687 - (0.29415931086406055 * fabs(grindAmp->bR[57]))));
			inputSampleR -= (grindAmp->bR[58] * (0.15447855089824883 - (0.26489186990840807 * fabs(grindAmp->bR[58]))));
			inputSampleR -= (grindAmp->bR[59] * (0.20518281113362655 - (0.16135382257522859 * fabs(grindAmp->bR[59]))));
			inputSampleR -= (grindAmp->bR[60] * (0.22244686050232007 + (0.00847180390247432 * fabs(grindAmp->bR[60]))));
			inputSampleR -= (grindAmp->bR[61] * (0.21849243134998034 + (0.14460595245753741 * fabs(grindAmp->bR[61]))));
			inputSampleR -= (grindAmp->bR[62] * (0.20256105734574054 + (0.18932793221831667 * fabs(grindAmp->bR[62]))));
			inputSampleR -= (grindAmp->bR[63] * (0.18604070054295399 + (0.17250665610927965 * fabs(grindAmp->bR[63]))));
			inputSampleR -= (grindAmp->bR[64] * (0.17222844322058231 + (0.12992472027850357 * fabs(grindAmp->bR[64]))));
			inputSampleR -= (grindAmp->bR[65] * (0.14447856616566443 + (0.09089219002147308 * fabs(grindAmp->bR[65]))));
			inputSampleR -= (grindAmp->bR[66] * (0.10385520794251019 + (0.08600465834570559 * fabs(grindAmp->bR[66]))));
			inputSampleR -= (grindAmp->bR[67] * (0.07124435678265063 + (0.09071532210549428 * fabs(grindAmp->bR[67]))));
			inputSampleR -= (grindAmp->bR[68] * (0.05216857461197572 + (0.06794061706070262 * fabs(grindAmp->bR[68]))));
			inputSampleR -= (grindAmp->bR[69] * (0.05235381920184123 + (0.02818101717909346 * fabs(grindAmp->bR[69]))));
			inputSampleR -= (grindAmp->bR[70] * (0.07569701245553526 - (0.00634228544764946 * fabs(grindAmp->bR[70]))));
			inputSampleR -= (grindAmp->bR[71] * (0.10320125382718826 - (0.02751486906644141 * fabs(grindAmp->bR[71]))));
			inputSampleR -= (grindAmp->bR[72] * (0.12122120969079088 - (0.05434007312178933 * fabs(grindAmp->bR[72]))));
			inputSampleR -= (grindAmp->bR[73] * (0.13438969117200902 - (0.09135218559713874 * fabs(grindAmp->bR[73]))));
			inputSampleR -= (grindAmp->bR[74] * (0.13534390437529981 - (0.10437672041458675 * fabs(grindAmp->bR[74]))));
			inputSampleR -= (grindAmp->bR[75] * (0.11424128854188388 - (0.08693450726462598 * fabs(grindAmp->bR[75]))));
			inputSampleR -= (grindAmp->bR[76] * (0.08166894518596159 - (0.06949989431475120 * fabs(grindAmp->bR[76]))));
			inputSampleR -= (grindAmp->bR[77] * (0.04293976378555305 - (0.05718625137421843 * fabs(grindAmp->bR[77]))));
			inputSampleR += (grindAmp->bR[78] * (0.00933076320644409 + (0.01728285211520138 * fabs(grindAmp->bR[78]))));
			inputSampleR += (grindAmp->bR[79] * (0.06450430362918153 - (0.02492994833691022 * fabs(grindAmp->bR[79]))));
			inputSampleR += (grindAmp->bR[80] * (0.10187400687649277 - (0.03578455940532403 * fabs(grindAmp->bR[80]))));
			inputSampleR += (grindAmp->bR[81] * (0.11039763294094571 - (0.03995523517573508 * fabs(grindAmp->bR[81]))));
			inputSampleR += (grindAmp->bR[82] * (0.08557960776024547 - (0.03482514309492527 * fabs(grindAmp->bR[82]))));
			inputSampleR += (grindAmp->bR[83] * (0.02730881850805332 - (0.00514750108411127 * fabs(grindAmp->bR[83]))));

			temp = (inputSampleR + grindAmp->smoothCabBR) / 3.0;
			grindAmp->smoothCabBR = inputSampleR;
			inputSampleR = temp / 4.0;

			randy = (((double) grindAmp->fpdR / UINT32_MAX) * 0.04);
			drySampleR = ((((inputSampleR * (1 - randy)) + (grindAmp->lastCabSampleR * randy)) * wet) + (drySampleR * (1.0 - wet))) * outputlevel;
			grindAmp->lastCabSampleR = inputSampleR;
			inputSampleR = drySampleR; // cab

			if (cycleEnd == 4) {
				grindAmp->lastRefL[0] = grindAmp->lastRefL[4]; // start from previous last
				grindAmp->lastRefL[2] = (grindAmp->lastRefL[0] + inputSampleL) / 2; // half
				grindAmp->lastRefL[1] = (grindAmp->lastRefL[0] + grindAmp->lastRefL[2]) / 2; // one quarter
				grindAmp->lastRefL[3] = (grindAmp->lastRefL[2] + inputSampleL) / 2; // three quarters
				grindAmp->lastRefL[4] = inputSampleL; // full
				grindAmp->lastRefR[0] = grindAmp->lastRefR[4]; // start from previous last
				grindAmp->lastRefR[2] = (grindAmp->lastRefR[0] + inputSampleR) / 2; // half
				grindAmp->lastRefR[1] = (grindAmp->lastRefR[0] + grindAmp->lastRefR[2]) / 2; // one quarter
				grindAmp->lastRefR[3] = (grindAmp->lastRefR[2] + inputSampleR) / 2; // three quarters
				grindAmp->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				grindAmp->lastRefL[0] = grindAmp->lastRefL[3]; // start from previous last
				grindAmp->lastRefL[2] = (grindAmp->lastRefL[0] + grindAmp->lastRefL[0] + inputSampleL) / 3; // third
				grindAmp->lastRefL[1] = (grindAmp->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				grindAmp->lastRefL[3] = inputSampleL; // full
				grindAmp->lastRefR[0] = grindAmp->lastRefR[3]; // start from previous last
				grindAmp->lastRefR[2] = (grindAmp->lastRefR[0] + grindAmp->lastRefR[0] + inputSampleR) / 3; // third
				grindAmp->lastRefR[1] = (grindAmp->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				grindAmp->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				grindAmp->lastRefL[0] = grindAmp->lastRefL[2]; // start from previous last
				grindAmp->lastRefL[1] = (grindAmp->lastRefL[0] + inputSampleL) / 2; // half
				grindAmp->lastRefL[2] = inputSampleL; // full
				grindAmp->lastRefR[0] = grindAmp->lastRefR[2]; // start from previous last
				grindAmp->lastRefR[1] = (grindAmp->lastRefR[0] + inputSampleR) / 2; // half
				grindAmp->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				grindAmp->lastRefL[0] = inputSampleL;
				grindAmp->lastRefR[0] = inputSampleR;
			}
			grindAmp->cycle = 0; // reset
			inputSampleL = grindAmp->lastRefL[grindAmp->cycle];
			inputSampleR = grindAmp->lastRefR[grindAmp->cycle];
		} else {
			inputSampleL = grindAmp->lastRefL[grindAmp->cycle];
			inputSampleR = grindAmp->lastRefR[grindAmp->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				grindAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + grindAmp->lastRefL[7]) * 0.5;
				grindAmp->lastRefL[7] = grindAmp->lastRefL[8]; // continue, do not break
				grindAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + grindAmp->lastRefR[7]) * 0.5;
				grindAmp->lastRefR[7] = grindAmp->lastRefR[8]; // continue, do not break
			case 3:
				grindAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + grindAmp->lastRefL[6]) * 0.5;
				grindAmp->lastRefL[6] = grindAmp->lastRefL[8]; // continue, do not break
				grindAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + grindAmp->lastRefR[6]) * 0.5;
				grindAmp->lastRefR[6] = grindAmp->lastRefR[8]; // continue, do not break
			case 2:
				grindAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + grindAmp->lastRefL[5]) * 0.5;
				grindAmp->lastRefL[5] = grindAmp->lastRefL[8]; // continue, do not break
				grindAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + grindAmp->lastRefR[5]) * 0.5;
				grindAmp->lastRefR[5] = grindAmp->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		grindAmp->fpdL ^= grindAmp->fpdL << 13;
		grindAmp->fpdL ^= grindAmp->fpdL >> 17;
		grindAmp->fpdL ^= grindAmp->fpdL << 5;
		inputSampleL += (((double) grindAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		grindAmp->fpdR ^= grindAmp->fpdR << 13;
		grindAmp->fpdR ^= grindAmp->fpdR >> 17;
		grindAmp->fpdR ^= grindAmp->fpdR << 5;
		inputSampleR += (((double) grindAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	GRINDAMP_URI,
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
