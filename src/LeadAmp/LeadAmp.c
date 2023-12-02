#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define LEADAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/leadamp"

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
	double iirSampleKL;
	double iirLowpassL;
	double iirSpkAL;
	double iirSpkBL;
	double iirSubL;
	double OddL[257];
	double EvenL[257];

	double lastSampleR;
	double storeSampleR;
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
	double iirSampleKR;
	double iirLowpassR;
	double iirSpkAR;
	double iirSpkBR;
	double iirSubR;
	double OddR[257];
	double EvenR[257];

	bool flip;
	int count;

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
} LeadAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	LeadAmp* leadAmp = (LeadAmp*) calloc(1, sizeof(LeadAmp));
	leadAmp->sampleRate = rate;
	return (LV2_Handle) leadAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	LeadAmp* leadAmp = (LeadAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			leadAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			leadAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			leadAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			leadAmp->output[1] = (float*) data;
			break;
		case GAIN:
			leadAmp->gain = (const float*) data;
			break;
		case TONE:
			leadAmp->tone = (const float*) data;
			break;
		case OUTPUT:
			leadAmp->outputGain = (const float*) data;
			break;
		case DRY_WET:
			leadAmp->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	LeadAmp* leadAmp = (LeadAmp*) instance;

	leadAmp->lastSampleL = 0.0;
	leadAmp->storeSampleL = 0.0;
	leadAmp->smoothAL = 0.0;
	leadAmp->smoothBL = 0.0;
	leadAmp->smoothCL = 0.0;
	leadAmp->smoothDL = 0.0;
	leadAmp->smoothEL = 0.0;
	leadAmp->smoothFL = 0.0;
	leadAmp->smoothGL = 0.0;
	leadAmp->smoothHL = 0.0;
	leadAmp->smoothIL = 0.0;
	leadAmp->smoothJL = 0.0;
	leadAmp->smoothKL = 0.0;
	leadAmp->iirSampleAL = 0.0;
	leadAmp->iirSampleBL = 0.0;
	leadAmp->iirSampleCL = 0.0;
	leadAmp->iirSampleDL = 0.0;
	leadAmp->iirSampleEL = 0.0;
	leadAmp->iirSampleFL = 0.0;
	leadAmp->iirSampleGL = 0.0;
	leadAmp->iirSampleHL = 0.0;
	leadAmp->iirSampleIL = 0.0;
	leadAmp->iirSampleJL = 0.0;
	leadAmp->iirSampleKL = 0.0;
	leadAmp->iirLowpassL = 0.0;
	leadAmp->iirSpkAL = 0.0;
	leadAmp->iirSpkBL = 0.0;
	leadAmp->iirSubL = 0.0;

	leadAmp->lastSampleR = 0.0;
	leadAmp->storeSampleR = 0.0;
	leadAmp->smoothAR = 0.0;
	leadAmp->smoothBR = 0.0;
	leadAmp->smoothCR = 0.0;
	leadAmp->smoothDR = 0.0;
	leadAmp->smoothER = 0.0;
	leadAmp->smoothFR = 0.0;
	leadAmp->smoothGR = 0.0;
	leadAmp->smoothHR = 0.0;
	leadAmp->smoothIR = 0.0;
	leadAmp->smoothJR = 0.0;
	leadAmp->smoothKR = 0.0;
	leadAmp->iirSampleAR = 0.0;
	leadAmp->iirSampleBR = 0.0;
	leadAmp->iirSampleCR = 0.0;
	leadAmp->iirSampleDR = 0.0;
	leadAmp->iirSampleER = 0.0;
	leadAmp->iirSampleFR = 0.0;
	leadAmp->iirSampleGR = 0.0;
	leadAmp->iirSampleHR = 0.0;
	leadAmp->iirSampleIR = 0.0;
	leadAmp->iirSampleJR = 0.0;
	leadAmp->iirSampleKR = 0.0;
	leadAmp->iirLowpassR = 0.0;
	leadAmp->iirSpkAR = 0.0;
	leadAmp->iirSpkBR = 0.0;
	leadAmp->iirSubR = 0.0;

	for (int fcount = 0; fcount < 257; fcount++) {
		leadAmp->OddL[fcount] = 0.0;
		leadAmp->EvenL[fcount] = 0.0;
		leadAmp->OddR[fcount] = 0.0;
		leadAmp->EvenR[fcount] = 0.0;
	}
	leadAmp->count = 0;
	leadAmp->flip = false; // amp

	for (int fcount = 0; fcount < 90; fcount++) {
		leadAmp->bL[fcount] = 0;
		leadAmp->bR[fcount] = 0;
	}
	leadAmp->smoothCabAL = 0.0;
	leadAmp->smoothCabBL = 0.0;
	leadAmp->lastCabSampleL = 0.0; // cab
	leadAmp->smoothCabAR = 0.0;
	leadAmp->smoothCabBR = 0.0;
	leadAmp->lastCabSampleR = 0.0; // cab

	for (int fcount = 0; fcount < 9; fcount++) {
		leadAmp->lastRefL[fcount] = 0.0;
		leadAmp->lastRefR[fcount] = 0.0;
	}
	leadAmp->cycle = 0; // undersampling

	for (int x = 0; x < fix_total; x++) {
		leadAmp->fixA[x] = 0.0;
		leadAmp->fixB[x] = 0.0;
		leadAmp->fixC[x] = 0.0;
		leadAmp->fixD[x] = 0.0;
		leadAmp->fixE[x] = 0.0;
		leadAmp->fixF[x] = 0.0;
	} // filtering

	leadAmp->fpdL = 1.0;
	while (leadAmp->fpdL < 16386) leadAmp->fpdL = rand() * UINT32_MAX;
	leadAmp->fpdR = 1.0;
	while (leadAmp->fpdR < 16386) leadAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	LeadAmp* leadAmp = (LeadAmp*) instance;

	const float* in1 = leadAmp->input[0];
	const float* in2 = leadAmp->input[1];
	float* out1 = leadAmp->output[0];
	float* out2 = leadAmp->output[1];

	const double sampleRate = leadAmp->sampleRate;
	const float tone = *leadAmp->tone;

	double bassfill = *leadAmp->gain;
	double startlevel = bassfill;
	double samplerate = sampleRate;
	double basstrim = bassfill / 10.0;
	double toneEQ = (tone / samplerate) * 22050.0;
	double EQ = (basstrim / samplerate) * 22050.0;
	double outputlevel = *leadAmp->outputGain;
	double wet = *leadAmp->dryWet;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (leadAmp->cycle > cycleEnd - 1) leadAmp->cycle = cycleEnd - 1; // sanity check

	double bleed = outputlevel / 16.0;
	double bassfactor = 1.0 - (basstrim * basstrim);
	double BEQ = (bleed / samplerate) * 22050.0;
	int diagonal = (int) (0.000861678 * samplerate);
	if (diagonal > 127) diagonal = 127;
	int side = (int) (diagonal / 1.4142135623730951);
	int down = (side + diagonal) / 2;
	// now we've got down, side and diagonal as offsets and we also use three successive samples upfront

	double cutoff = (15000.0 + (tone * 10000.0)) / leadAmp->sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low

	leadAmp->fixF[fix_freq] = leadAmp->fixE[fix_freq] = leadAmp->fixD[fix_freq] = leadAmp->fixC[fix_freq] = leadAmp->fixB[fix_freq] = leadAmp->fixA[fix_freq] = cutoff;

	leadAmp->fixA[fix_reso] = 4.46570214;
	leadAmp->fixB[fix_reso] = 1.51387132;
	leadAmp->fixC[fix_reso] = 0.93979296;
	leadAmp->fixD[fix_reso] = 0.70710678;
	leadAmp->fixE[fix_reso] = 0.52972649;
	leadAmp->fixF[fix_reso] = 0.50316379;

	double K = tan(M_PI * leadAmp->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / leadAmp->fixA[fix_reso] + K * K);
	leadAmp->fixA[fix_a0] = K * K * norm;
	leadAmp->fixA[fix_a1] = 2.0 * leadAmp->fixA[fix_a0];
	leadAmp->fixA[fix_a2] = leadAmp->fixA[fix_a0];
	leadAmp->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	leadAmp->fixA[fix_b2] = (1.0 - K / leadAmp->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * leadAmp->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / leadAmp->fixB[fix_reso] + K * K);
	leadAmp->fixB[fix_a0] = K * K * norm;
	leadAmp->fixB[fix_a1] = 2.0 * leadAmp->fixB[fix_a0];
	leadAmp->fixB[fix_a2] = leadAmp->fixB[fix_a0];
	leadAmp->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	leadAmp->fixB[fix_b2] = (1.0 - K / leadAmp->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * leadAmp->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / leadAmp->fixC[fix_reso] + K * K);
	leadAmp->fixC[fix_a0] = K * K * norm;
	leadAmp->fixC[fix_a1] = 2.0 * leadAmp->fixC[fix_a0];
	leadAmp->fixC[fix_a2] = leadAmp->fixC[fix_a0];
	leadAmp->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	leadAmp->fixC[fix_b2] = (1.0 - K / leadAmp->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * leadAmp->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / leadAmp->fixD[fix_reso] + K * K);
	leadAmp->fixD[fix_a0] = K * K * norm;
	leadAmp->fixD[fix_a1] = 2.0 * leadAmp->fixD[fix_a0];
	leadAmp->fixD[fix_a2] = leadAmp->fixD[fix_a0];
	leadAmp->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	leadAmp->fixD[fix_b2] = (1.0 - K / leadAmp->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * leadAmp->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / leadAmp->fixE[fix_reso] + K * K);
	leadAmp->fixE[fix_a0] = K * K * norm;
	leadAmp->fixE[fix_a1] = 2.0 * leadAmp->fixE[fix_a0];
	leadAmp->fixE[fix_a2] = leadAmp->fixE[fix_a0];
	leadAmp->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	leadAmp->fixE[fix_b2] = (1.0 - K / leadAmp->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * leadAmp->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / leadAmp->fixF[fix_reso] + K * K);
	leadAmp->fixF[fix_a0] = K * K * norm;
	leadAmp->fixF[fix_a1] = 2.0 * leadAmp->fixF[fix_a0];
	leadAmp->fixF[fix_a2] = leadAmp->fixF[fix_a0];
	leadAmp->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	leadAmp->fixF[fix_b2] = (1.0 - K / leadAmp->fixF[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = leadAmp->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = leadAmp->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double outSample = (inputSampleL * leadAmp->fixA[fix_a0]) + leadAmp->fixA[fix_sL1];
		leadAmp->fixA[fix_sL1] = (inputSampleL * leadAmp->fixA[fix_a1]) - (outSample * leadAmp->fixA[fix_b1]) + leadAmp->fixA[fix_sL2];
		leadAmp->fixA[fix_sL2] = (inputSampleL * leadAmp->fixA[fix_a2]) - (outSample * leadAmp->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * leadAmp->fixA[fix_a0]) + leadAmp->fixA[fix_sR1];
		leadAmp->fixA[fix_sR1] = (inputSampleR * leadAmp->fixA[fix_a1]) - (outSample * leadAmp->fixA[fix_b1]) + leadAmp->fixA[fix_sR2];
		leadAmp->fixA[fix_sR2] = (inputSampleR * leadAmp->fixA[fix_a2]) - (outSample * leadAmp->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		double basscutL = 0.99;
		// we're going to be shifting this as the stages progress
		double inputlevelL = startlevel;
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleAL = (leadAmp->iirSampleAL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleAL * basscutL);
		// highpass
		double bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		// overdrive
		bridgerectifier = (leadAmp->smoothAL + inputSampleL);
		leadAmp->smoothAL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		double basscutR = 0.99;
		// we're going to be shifting this as the stages progress
		double inputlevelR = startlevel;
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleAR = (leadAmp->iirSampleAR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleAR * basscutR);
		// highpass
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		// overdrive
		bridgerectifier = (leadAmp->smoothAR + inputSampleR);
		leadAmp->smoothAR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * leadAmp->fixB[fix_a0]) + leadAmp->fixB[fix_sL1];
		leadAmp->fixB[fix_sL1] = (inputSampleL * leadAmp->fixB[fix_a1]) - (outSample * leadAmp->fixB[fix_b1]) + leadAmp->fixB[fix_sL2];
		leadAmp->fixB[fix_sL2] = (inputSampleL * leadAmp->fixB[fix_a2]) - (outSample * leadAmp->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * leadAmp->fixB[fix_a0]) + leadAmp->fixB[fix_sR1];
		leadAmp->fixB[fix_sR1] = (inputSampleR * leadAmp->fixB[fix_a1]) - (outSample * leadAmp->fixB[fix_b1]) + leadAmp->fixB[fix_sR2];
		leadAmp->fixB[fix_sR2] = (inputSampleR * leadAmp->fixB[fix_a2]) - (outSample * leadAmp->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleBL = (leadAmp->iirSampleBL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleBL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothBL + inputSampleL);
		leadAmp->smoothBL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleBR = (leadAmp->iirSampleBR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleBR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothBR + inputSampleR);
		leadAmp->smoothBR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * leadAmp->fixC[fix_a0]) + leadAmp->fixC[fix_sL1];
		leadAmp->fixC[fix_sL1] = (inputSampleL * leadAmp->fixC[fix_a1]) - (outSample * leadAmp->fixC[fix_b1]) + leadAmp->fixC[fix_sL2];
		leadAmp->fixC[fix_sL2] = (inputSampleL * leadAmp->fixC[fix_a2]) - (outSample * leadAmp->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * leadAmp->fixC[fix_a0]) + leadAmp->fixC[fix_sR1];
		leadAmp->fixC[fix_sR1] = (inputSampleR * leadAmp->fixC[fix_a1]) - (outSample * leadAmp->fixC[fix_b1]) + leadAmp->fixC[fix_sR2];
		leadAmp->fixC[fix_sR2] = (inputSampleR * leadAmp->fixC[fix_a2]) - (outSample * leadAmp->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleCL = (leadAmp->iirSampleCL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleCL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothCL + inputSampleL);
		leadAmp->smoothCL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleCR = (leadAmp->iirSampleCR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleCR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothCR + inputSampleR);
		leadAmp->smoothCR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleDL = (leadAmp->iirSampleDL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleDL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothDL + inputSampleL);
		leadAmp->smoothDL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleDR = (leadAmp->iirSampleDR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleDR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothDR + inputSampleR);
		leadAmp->smoothDR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * leadAmp->fixD[fix_a0]) + leadAmp->fixD[fix_sL1];
		leadAmp->fixD[fix_sL1] = (inputSampleL * leadAmp->fixD[fix_a1]) - (outSample * leadAmp->fixD[fix_b1]) + leadAmp->fixD[fix_sL2];
		leadAmp->fixD[fix_sL2] = (inputSampleL * leadAmp->fixD[fix_a2]) - (outSample * leadAmp->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * leadAmp->fixD[fix_a0]) + leadAmp->fixD[fix_sR1];
		leadAmp->fixD[fix_sR1] = (inputSampleR * leadAmp->fixD[fix_a1]) - (outSample * leadAmp->fixD[fix_b1]) + leadAmp->fixD[fix_sR2];
		leadAmp->fixD[fix_sR2] = (inputSampleR * leadAmp->fixD[fix_a2]) - (outSample * leadAmp->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleEL = (leadAmp->iirSampleEL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleEL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothEL + inputSampleL);
		leadAmp->smoothEL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleER = (leadAmp->iirSampleER * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleER * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothER + inputSampleR);
		leadAmp->smoothER = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleFL = (leadAmp->iirSampleFL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleFL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothFL + inputSampleL);
		leadAmp->smoothFL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleFR = (leadAmp->iirSampleFR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleFR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothFR + inputSampleR);
		leadAmp->smoothFR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * leadAmp->fixE[fix_a0]) + leadAmp->fixE[fix_sL1];
		leadAmp->fixE[fix_sL1] = (inputSampleL * leadAmp->fixE[fix_a1]) - (outSample * leadAmp->fixE[fix_b1]) + leadAmp->fixE[fix_sL2];
		leadAmp->fixE[fix_sL2] = (inputSampleL * leadAmp->fixE[fix_a2]) - (outSample * leadAmp->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * leadAmp->fixE[fix_a0]) + leadAmp->fixE[fix_sR1];
		leadAmp->fixE[fix_sR1] = (inputSampleR * leadAmp->fixE[fix_a1]) - (outSample * leadAmp->fixE[fix_b1]) + leadAmp->fixE[fix_sR2];
		leadAmp->fixE[fix_sR2] = (inputSampleR * leadAmp->fixE[fix_a2]) - (outSample * leadAmp->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleGL = (leadAmp->iirSampleGL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleGL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothGL + inputSampleL);
		leadAmp->smoothGL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleGR = (leadAmp->iirSampleGR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleGR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothGR + inputSampleR);
		leadAmp->smoothGR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleHL = (leadAmp->iirSampleHL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleHL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothHL + inputSampleL);
		leadAmp->smoothHL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleHR = (leadAmp->iirSampleHR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleHR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothHR + inputSampleR);
		leadAmp->smoothHR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * leadAmp->fixF[fix_a0]) + leadAmp->fixF[fix_sL1];
		leadAmp->fixF[fix_sL1] = (inputSampleL * leadAmp->fixF[fix_a1]) - (outSample * leadAmp->fixF[fix_b1]) + leadAmp->fixF[fix_sL2];
		leadAmp->fixF[fix_sL2] = (inputSampleL * leadAmp->fixF[fix_a2]) - (outSample * leadAmp->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * leadAmp->fixF[fix_a0]) + leadAmp->fixF[fix_sR1];
		leadAmp->fixF[fix_sR1] = (inputSampleR * leadAmp->fixF[fix_a1]) - (outSample * leadAmp->fixF[fix_b1]) + leadAmp->fixF[fix_sR2];
		leadAmp->fixF[fix_sR2] = (inputSampleR * leadAmp->fixF[fix_a2]) - (outSample * leadAmp->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleIL = (leadAmp->iirSampleIL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleIL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothIL + inputSampleL);
		leadAmp->smoothIL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleIR = (leadAmp->iirSampleIR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleIR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothIR + inputSampleR);
		leadAmp->smoothIR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleJL = (leadAmp->iirSampleJL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleJL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothJL + inputSampleL);
		leadAmp->smoothJL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleJR = (leadAmp->iirSampleJR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleJR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothJR + inputSampleR);
		leadAmp->smoothJR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleKL = (leadAmp->iirSampleKL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (leadAmp->iirSampleKL * basscutL);
		// highpass
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// overdrive
		bridgerectifier = (leadAmp->smoothKL + inputSampleL);
		leadAmp->smoothKL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		leadAmp->iirSampleKR = (leadAmp->iirSampleKR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (leadAmp->iirSampleKR * basscutR);
		// highpass
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// overdrive
		bridgerectifier = (leadAmp->smoothKR + inputSampleR);
		leadAmp->smoothKR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		leadAmp->iirLowpassL = (leadAmp->iirLowpassL * (1.0 - toneEQ)) + (inputSampleL * toneEQ);
		inputSampleL = leadAmp->iirLowpassL;
		// lowpass. The only one of this type.

		leadAmp->iirLowpassR = (leadAmp->iirLowpassR * (1.0 - toneEQ)) + (inputSampleR * toneEQ);
		inputSampleR = leadAmp->iirLowpassR;
		// lowpass. The only one of this type.

		leadAmp->iirSpkAL = (leadAmp->iirSpkAL * (1.0 - BEQ)) + (inputSampleL * BEQ);
		// extra lowpass for 4*12" speakers

		leadAmp->iirSpkAR = (leadAmp->iirSpkAR * (1.0 - BEQ)) + (inputSampleR * BEQ);
		// extra lowpass for 4*12" speakers

		if (leadAmp->count < 0 || leadAmp->count > 128) {
			leadAmp->count = 128;
		}
		double resultBL = 0.0;
		double resultBR = 0.0;
		if (leadAmp->flip) {
			leadAmp->OddL[leadAmp->count + 128] = leadAmp->OddL[leadAmp->count] = leadAmp->iirSpkAL;
			resultBL = (leadAmp->OddL[leadAmp->count + down] + leadAmp->OddL[leadAmp->count + side] + leadAmp->OddL[leadAmp->count + diagonal]);
			leadAmp->OddR[leadAmp->count + 128] = leadAmp->OddR[leadAmp->count] = leadAmp->iirSpkAR;
			resultBR = (leadAmp->OddR[leadAmp->count + down] + leadAmp->OddR[leadAmp->count + side] + leadAmp->OddR[leadAmp->count + diagonal]);
		} else {
			leadAmp->EvenL[leadAmp->count + 128] = leadAmp->EvenL[leadAmp->count] = leadAmp->iirSpkAL;
			resultBL = (leadAmp->EvenL[leadAmp->count + down] + leadAmp->EvenL[leadAmp->count + side] + leadAmp->EvenL[leadAmp->count + diagonal]);
			leadAmp->EvenR[leadAmp->count + 128] = leadAmp->EvenR[leadAmp->count] = leadAmp->iirSpkAR;
			resultBR = (leadAmp->EvenR[leadAmp->count + down] + leadAmp->EvenR[leadAmp->count + side] + leadAmp->EvenR[leadAmp->count + diagonal]);
		}
		leadAmp->count--;

		leadAmp->iirSpkBL = (leadAmp->iirSpkBL * (1.0 - BEQ)) + (resultBL * BEQ);
		inputSampleL += (leadAmp->iirSpkBL * bleed);
		// extra lowpass for 4*12" speakers

		leadAmp->iirSpkBR = (leadAmp->iirSpkBR * (1.0 - BEQ)) + (resultBR * BEQ);
		inputSampleR += (leadAmp->iirSpkBR * bleed);
		// extra lowpass for 4*12" speakers

		leadAmp->iirSubL = (leadAmp->iirSubL * (1.0 - BEQ)) + (inputSampleL * BEQ);
		inputSampleL += (leadAmp->iirSubL * bassfill);

		leadAmp->iirSubR = (leadAmp->iirSubR * (1.0 - BEQ)) + (inputSampleR * BEQ);
		inputSampleR += (leadAmp->iirSubR * bassfill);

		bridgerectifier = fabs(inputSampleL * outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;

		bridgerectifier = fabs(inputSampleR * outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;

		double randy = (((double) leadAmp->fpdL / UINT32_MAX) * 0.084);
		inputSampleL = ((inputSampleL * (1.0 - randy)) + (leadAmp->storeSampleL * randy)) * outputlevel;
		leadAmp->storeSampleL = inputSampleL;

		randy = (((double) leadAmp->fpdR / UINT32_MAX) * 0.084);
		inputSampleR = ((inputSampleR * (1.0 - randy)) + (leadAmp->storeSampleR * randy)) * outputlevel;
		leadAmp->storeSampleR = inputSampleR;

		leadAmp->flip = !leadAmp->flip;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider
		// amp

		leadAmp->cycle++;
		if (leadAmp->cycle == cycleEnd) {

			double temp = (inputSampleL + leadAmp->smoothCabAL) / 3.0;
			leadAmp->smoothCabAL = inputSampleL;
			inputSampleL = temp;

			leadAmp->bL[85] = leadAmp->bL[84];
			leadAmp->bL[84] = leadAmp->bL[83];
			leadAmp->bL[83] = leadAmp->bL[82];
			leadAmp->bL[82] = leadAmp->bL[81];
			leadAmp->bL[81] = leadAmp->bL[80];
			leadAmp->bL[80] = leadAmp->bL[79];
			leadAmp->bL[79] = leadAmp->bL[78];
			leadAmp->bL[78] = leadAmp->bL[77];
			leadAmp->bL[77] = leadAmp->bL[76];
			leadAmp->bL[76] = leadAmp->bL[75];
			leadAmp->bL[75] = leadAmp->bL[74];
			leadAmp->bL[74] = leadAmp->bL[73];
			leadAmp->bL[73] = leadAmp->bL[72];
			leadAmp->bL[72] = leadAmp->bL[71];
			leadAmp->bL[71] = leadAmp->bL[70];
			leadAmp->bL[70] = leadAmp->bL[69];
			leadAmp->bL[69] = leadAmp->bL[68];
			leadAmp->bL[68] = leadAmp->bL[67];
			leadAmp->bL[67] = leadAmp->bL[66];
			leadAmp->bL[66] = leadAmp->bL[65];
			leadAmp->bL[65] = leadAmp->bL[64];
			leadAmp->bL[64] = leadAmp->bL[63];
			leadAmp->bL[63] = leadAmp->bL[62];
			leadAmp->bL[62] = leadAmp->bL[61];
			leadAmp->bL[61] = leadAmp->bL[60];
			leadAmp->bL[60] = leadAmp->bL[59];
			leadAmp->bL[59] = leadAmp->bL[58];
			leadAmp->bL[58] = leadAmp->bL[57];
			leadAmp->bL[57] = leadAmp->bL[56];
			leadAmp->bL[56] = leadAmp->bL[55];
			leadAmp->bL[55] = leadAmp->bL[54];
			leadAmp->bL[54] = leadAmp->bL[53];
			leadAmp->bL[53] = leadAmp->bL[52];
			leadAmp->bL[52] = leadAmp->bL[51];
			leadAmp->bL[51] = leadAmp->bL[50];
			leadAmp->bL[50] = leadAmp->bL[49];
			leadAmp->bL[49] = leadAmp->bL[48];
			leadAmp->bL[48] = leadAmp->bL[47];
			leadAmp->bL[47] = leadAmp->bL[46];
			leadAmp->bL[46] = leadAmp->bL[45];
			leadAmp->bL[45] = leadAmp->bL[44];
			leadAmp->bL[44] = leadAmp->bL[43];
			leadAmp->bL[43] = leadAmp->bL[42];
			leadAmp->bL[42] = leadAmp->bL[41];
			leadAmp->bL[41] = leadAmp->bL[40];
			leadAmp->bL[40] = leadAmp->bL[39];
			leadAmp->bL[39] = leadAmp->bL[38];
			leadAmp->bL[38] = leadAmp->bL[37];
			leadAmp->bL[37] = leadAmp->bL[36];
			leadAmp->bL[36] = leadAmp->bL[35];
			leadAmp->bL[35] = leadAmp->bL[34];
			leadAmp->bL[34] = leadAmp->bL[33];
			leadAmp->bL[33] = leadAmp->bL[32];
			leadAmp->bL[32] = leadAmp->bL[31];
			leadAmp->bL[31] = leadAmp->bL[30];
			leadAmp->bL[30] = leadAmp->bL[29];
			leadAmp->bL[29] = leadAmp->bL[28];
			leadAmp->bL[28] = leadAmp->bL[27];
			leadAmp->bL[27] = leadAmp->bL[26];
			leadAmp->bL[26] = leadAmp->bL[25];
			leadAmp->bL[25] = leadAmp->bL[24];
			leadAmp->bL[24] = leadAmp->bL[23];
			leadAmp->bL[23] = leadAmp->bL[22];
			leadAmp->bL[22] = leadAmp->bL[21];
			leadAmp->bL[21] = leadAmp->bL[20];
			leadAmp->bL[20] = leadAmp->bL[19];
			leadAmp->bL[19] = leadAmp->bL[18];
			leadAmp->bL[18] = leadAmp->bL[17];
			leadAmp->bL[17] = leadAmp->bL[16];
			leadAmp->bL[16] = leadAmp->bL[15];
			leadAmp->bL[15] = leadAmp->bL[14];
			leadAmp->bL[14] = leadAmp->bL[13];
			leadAmp->bL[13] = leadAmp->bL[12];
			leadAmp->bL[12] = leadAmp->bL[11];
			leadAmp->bL[11] = leadAmp->bL[10];
			leadAmp->bL[10] = leadAmp->bL[9];
			leadAmp->bL[9] = leadAmp->bL[8];
			leadAmp->bL[8] = leadAmp->bL[7];
			leadAmp->bL[7] = leadAmp->bL[6];
			leadAmp->bL[6] = leadAmp->bL[5];
			leadAmp->bL[5] = leadAmp->bL[4];
			leadAmp->bL[4] = leadAmp->bL[3];
			leadAmp->bL[3] = leadAmp->bL[2];
			leadAmp->bL[2] = leadAmp->bL[1];
			leadAmp->bL[1] = leadAmp->bL[0];
			leadAmp->bL[0] = inputSampleL;
			inputSampleL += (leadAmp->bL[1] * (1.30406584776167445 - (0.01410622186823351 * fabs(leadAmp->bL[1]))));
			inputSampleL += (leadAmp->bL[2] * (1.09350974154373559 + (0.34478044709202327 * fabs(leadAmp->bL[2]))));
			inputSampleL += (leadAmp->bL[3] * (0.52285510059938256 + (0.84225842837363574 * fabs(leadAmp->bL[3]))));
			inputSampleL -= (leadAmp->bL[4] * (0.00018126260714707 - (1.02446537989058117 * fabs(leadAmp->bL[4]))));
			inputSampleL -= (leadAmp->bL[5] * (0.34943699771860115 - (0.84094709567790016 * fabs(leadAmp->bL[5]))));
			inputSampleL -= (leadAmp->bL[6] * (0.53068048407937285 - (0.49231169327705593 * fabs(leadAmp->bL[6]))));
			inputSampleL -= (leadAmp->bL[7] * (0.48631669406792399 - (0.08965111766223610 * fabs(leadAmp->bL[7]))));
			inputSampleL -= (leadAmp->bL[8] * (0.28099201947014130 + (0.23921137841068607 * fabs(leadAmp->bL[8]))));
			inputSampleL -= (leadAmp->bL[9] * (0.10333290012666248 + (0.35058962687321482 * fabs(leadAmp->bL[9]))));
			inputSampleL -= (leadAmp->bL[10] * (0.06605032198166226 + (0.23447405567823365 * fabs(leadAmp->bL[10]))));
			inputSampleL -= (leadAmp->bL[11] * (0.10485808661261729 + (0.05025985449763527 * fabs(leadAmp->bL[11]))));
			inputSampleL -= (leadAmp->bL[12] * (0.13231190973014911 - (0.05484648240248013 * fabs(leadAmp->bL[12]))));
			inputSampleL -= (leadAmp->bL[13] * (0.12926184767180304 - (0.04054223744746116 * fabs(leadAmp->bL[13]))));
			inputSampleL -= (leadAmp->bL[14] * (0.13802696739839460 + (0.01876754906568237 * fabs(leadAmp->bL[14]))));
			inputSampleL -= (leadAmp->bL[15] * (0.16548980700926913 + (0.06772130758771169 * fabs(leadAmp->bL[15]))));
			inputSampleL -= (leadAmp->bL[16] * (0.14469310965751475 + (0.10590928840978781 * fabs(leadAmp->bL[16]))));
			inputSampleL -= (leadAmp->bL[17] * (0.07838457396093310 + (0.13120101199677947 * fabs(leadAmp->bL[17]))));
			inputSampleL -= (leadAmp->bL[18] * (0.05123031606187391 + (0.13883400806512292 * fabs(leadAmp->bL[18]))));
			inputSampleL -= (leadAmp->bL[19] * (0.08906103481939850 + (0.07840461228402337 * fabs(leadAmp->bL[19]))));
			inputSampleL -= (leadAmp->bL[20] * (0.13939265522625241 + (0.01194366471800457 * fabs(leadAmp->bL[20]))));
			inputSampleL -= (leadAmp->bL[21] * (0.14957600717294034 + (0.07687598594361914 * fabs(leadAmp->bL[21]))));
			inputSampleL -= (leadAmp->bL[22] * (0.14112708654047090 + (0.20118461131186977 * fabs(leadAmp->bL[22]))));
			inputSampleL -= (leadAmp->bL[23] * (0.14961020766492997 + (0.30005716443826147 * fabs(leadAmp->bL[23]))));
			inputSampleL -= (leadAmp->bL[24] * (0.16130382224652270 + (0.40459872030013055 * fabs(leadAmp->bL[24]))));
			inputSampleL -= (leadAmp->bL[25] * (0.15679868471080052 + (0.47292767226083465 * fabs(leadAmp->bL[25]))));
			inputSampleL -= (leadAmp->bL[26] * (0.16456530552807727 + (0.45182121471666481 * fabs(leadAmp->bL[26]))));
			inputSampleL -= (leadAmp->bL[27] * (0.16852385701909278 + (0.38272684270752266 * fabs(leadAmp->bL[27]))));
			inputSampleL -= (leadAmp->bL[28] * (0.13317562760966850 + (0.28829580273670768 * fabs(leadAmp->bL[28]))));
			inputSampleL -= (leadAmp->bL[29] * (0.09396196532150952 + (0.18886898332071317 * fabs(leadAmp->bL[29]))));
			inputSampleL -= (leadAmp->bL[30] * (0.10133496956734221 + (0.11158788414137354 * fabs(leadAmp->bL[30]))));
			inputSampleL -= (leadAmp->bL[31] * (0.16097596389376778 + (0.02621299102374547 * fabs(leadAmp->bL[31]))));
			inputSampleL -= (leadAmp->bL[32] * (0.21419006394821866 - (0.03585678078834797 * fabs(leadAmp->bL[32]))));
			inputSampleL -= (leadAmp->bL[33] * (0.21273234570555244 - (0.02574469802924526 * fabs(leadAmp->bL[33]))));
			inputSampleL -= (leadAmp->bL[34] * (0.16934948798707830 + (0.01354331184333835 * fabs(leadAmp->bL[34]))));
			inputSampleL -= (leadAmp->bL[35] * (0.11970436472852493 + (0.04242183865883427 * fabs(leadAmp->bL[35]))));
			inputSampleL -= (leadAmp->bL[36] * (0.09329023656747724 + (0.06890873292358397 * fabs(leadAmp->bL[36]))));
			inputSampleL -= (leadAmp->bL[37] * (0.10255328436608116 + (0.11482972519137427 * fabs(leadAmp->bL[37]))));
			inputSampleL -= (leadAmp->bL[38] * (0.13883223352796811 + (0.18016014431438840 * fabs(leadAmp->bL[38]))));
			inputSampleL -= (leadAmp->bL[39] * (0.16532844286979087 + (0.24521957638633446 * fabs(leadAmp->bL[39]))));
			inputSampleL -= (leadAmp->bL[40] * (0.16254607738965438 + (0.25669472097572482 * fabs(leadAmp->bL[40]))));
			inputSampleL -= (leadAmp->bL[41] * (0.15353207135544752 + (0.15048064682912729 * fabs(leadAmp->bL[41]))));
			inputSampleL -= (leadAmp->bL[42] * (0.13039046390746015 - (0.00200335414623601 * fabs(leadAmp->bL[42]))));
			inputSampleL -= (leadAmp->bL[43] * (0.06707245032180627 - (0.06498125592578702 * fabs(leadAmp->bL[43]))));
			inputSampleL += (leadAmp->bL[44] * (0.01427326441869788 + (0.01940451360783622 * fabs(leadAmp->bL[44]))));
			inputSampleL += (leadAmp->bL[45] * (0.06151238306578224 - (0.07335755969763329 * fabs(leadAmp->bL[45]))));
			inputSampleL += (leadAmp->bL[46] * (0.04685840498892526 - (0.14258849371688248 * fabs(leadAmp->bL[46]))));
			inputSampleL -= (leadAmp->bL[47] * (0.00950136304466093 + (0.14379354707665129 * fabs(leadAmp->bL[47]))));
			inputSampleL -= (leadAmp->bL[48] * (0.06245771575493557 + (0.07639718586346110 * fabs(leadAmp->bL[48]))));
			inputSampleL -= (leadAmp->bL[49] * (0.07159593175777741 - (0.00595536565276915 * fabs(leadAmp->bL[49]))));
			inputSampleL -= (leadAmp->bL[50] * (0.03167929390245019 - (0.03856769526301793 * fabs(leadAmp->bL[50]))));
			inputSampleL += (leadAmp->bL[51] * (0.01890898565110766 + (0.00760539424271147 * fabs(leadAmp->bL[51]))));
			inputSampleL += (leadAmp->bL[52] * (0.04926161137832240 - (0.06411014430053390 * fabs(leadAmp->bL[52]))));
			inputSampleL += (leadAmp->bL[53] * (0.05768814623421683 - (0.15068618173358578 * fabs(leadAmp->bL[53]))));
			inputSampleL += (leadAmp->bL[54] * (0.06144258297076708 - (0.21200636329120301 * fabs(leadAmp->bL[54]))));
			inputSampleL += (leadAmp->bL[55] * (0.06348341960185613 - (0.19620269813094307 * fabs(leadAmp->bL[55]))));
			inputSampleL += (leadAmp->bL[56] * (0.04877736350310589 - (0.11864999881200111 * fabs(leadAmp->bL[56]))));
			inputSampleL += (leadAmp->bL[57] * (0.01010950997574472 - (0.02630070679113791 * fabs(leadAmp->bL[57]))));
			inputSampleL -= (leadAmp->bL[58] * (0.02929178864801191 - (0.04439260202207482 * fabs(leadAmp->bL[58]))));
			inputSampleL -= (leadAmp->bL[59] * (0.03484517126321562 - (0.04508635396034735 * fabs(leadAmp->bL[59]))));
			inputSampleL -= (leadAmp->bL[60] * (0.00547176780437610 - (0.00205637806941426 * fabs(leadAmp->bL[60]))));
			inputSampleL += (leadAmp->bL[61] * (0.02278296865283977 - (0.00063732526427685 * fabs(leadAmp->bL[61]))));
			inputSampleL += (leadAmp->bL[62] * (0.02688982591366477 + (0.05333738901586284 * fabs(leadAmp->bL[62]))));
			inputSampleL += (leadAmp->bL[63] * (0.01942012754957055 + (0.10942832669749143 * fabs(leadAmp->bL[63]))));
			inputSampleL += (leadAmp->bL[64] * (0.01572585258756565 + (0.11189204189054594 * fabs(leadAmp->bL[64]))));
			inputSampleL += (leadAmp->bL[65] * (0.01490550715016034 + (0.04449822818925343 * fabs(leadAmp->bL[65]))));
			inputSampleL += (leadAmp->bL[66] * (0.01715683226376727 - (0.06944648050933899 * fabs(leadAmp->bL[66]))));
			inputSampleL += (leadAmp->bL[67] * (0.02822659878011318 - (0.17843652160132820 * fabs(leadAmp->bL[67]))));
			inputSampleL += (leadAmp->bL[68] * (0.03758307610456144 - (0.21986013433664692 * fabs(leadAmp->bL[68]))));
			inputSampleL += (leadAmp->bL[69] * (0.03275008021608433 - (0.15869878676112170 * fabs(leadAmp->bL[69]))));
			inputSampleL += (leadAmp->bL[70] * (0.01855749786752354 - (0.02337224995718105 * fabs(leadAmp->bL[70]))));
			inputSampleL += (leadAmp->bL[71] * (0.00217095395782931 + (0.10971764224593601 * fabs(leadAmp->bL[71]))));
			inputSampleL -= (leadAmp->bL[72] * (0.01851381451105007 - (0.17214910008793413 * fabs(leadAmp->bL[72]))));
			inputSampleL -= (leadAmp->bL[73] * (0.04722574936345419 - (0.14341588977845254 * fabs(leadAmp->bL[73]))));
			inputSampleL -= (leadAmp->bL[74] * (0.07151540514482006 - (0.04684695724814321 * fabs(leadAmp->bL[74]))));
			inputSampleL -= (leadAmp->bL[75] * (0.06827195484995092 + (0.07022207121861397 * fabs(leadAmp->bL[75]))));
			inputSampleL -= (leadAmp->bL[76] * (0.03290227240464227 + (0.16328400808152735 * fabs(leadAmp->bL[76]))));
			inputSampleL += (leadAmp->bL[77] * (0.01043861198275382 - (0.20184486126076279 * fabs(leadAmp->bL[77]))));
			inputSampleL += (leadAmp->bL[78] * (0.03236563559476477 - (0.17125821306380920 * fabs(leadAmp->bL[78]))));
			inputSampleL += (leadAmp->bL[79] * (0.02040121529932702 - (0.09103660189829657 * fabs(leadAmp->bL[79]))));
			inputSampleL -= (leadAmp->bL[80] * (0.00509649513318102 + (0.01170360991547489 * fabs(leadAmp->bL[80]))));
			inputSampleL -= (leadAmp->bL[81] * (0.01388353426600228 - (0.03588955538451771 * fabs(leadAmp->bL[81]))));
			inputSampleL -= (leadAmp->bL[82] * (0.00523671715033842 - (0.07068798057534148 * fabs(leadAmp->bL[82]))));
			inputSampleL += (leadAmp->bL[83] * (0.00665852487721137 + (0.11666210640054926 * fabs(leadAmp->bL[83]))));
			inputSampleL += (leadAmp->bL[84] * (0.01593540832939290 + (0.15844892856402149 * fabs(leadAmp->bL[84]))));
			inputSampleL += (leadAmp->bL[85] * (0.02080509201836796 + (0.17186274420065850 * fabs(leadAmp->bL[85]))));

			temp = (inputSampleL + leadAmp->smoothCabBL) / 3.0;
			leadAmp->smoothCabBL = inputSampleL;
			inputSampleL = temp / 4.0;

			randy = (((double) leadAmp->fpdL / UINT32_MAX) * 0.079);
			drySampleL = ((((inputSampleL * (1.0 - randy)) + (leadAmp->lastCabSampleL * randy)) * wet) + (drySampleL * (1.0 - wet))) * outputlevel;
			leadAmp->lastCabSampleL = inputSampleL;
			inputSampleL = drySampleL; // cab L

			temp = (inputSampleR + leadAmp->smoothCabAR) / 3.0;
			leadAmp->smoothCabAR = inputSampleR;
			inputSampleR = temp;

			leadAmp->bR[85] = leadAmp->bR[84];
			leadAmp->bR[84] = leadAmp->bR[83];
			leadAmp->bR[83] = leadAmp->bR[82];
			leadAmp->bR[82] = leadAmp->bR[81];
			leadAmp->bR[81] = leadAmp->bR[80];
			leadAmp->bR[80] = leadAmp->bR[79];
			leadAmp->bR[79] = leadAmp->bR[78];
			leadAmp->bR[78] = leadAmp->bR[77];
			leadAmp->bR[77] = leadAmp->bR[76];
			leadAmp->bR[76] = leadAmp->bR[75];
			leadAmp->bR[75] = leadAmp->bR[74];
			leadAmp->bR[74] = leadAmp->bR[73];
			leadAmp->bR[73] = leadAmp->bR[72];
			leadAmp->bR[72] = leadAmp->bR[71];
			leadAmp->bR[71] = leadAmp->bR[70];
			leadAmp->bR[70] = leadAmp->bR[69];
			leadAmp->bR[69] = leadAmp->bR[68];
			leadAmp->bR[68] = leadAmp->bR[67];
			leadAmp->bR[67] = leadAmp->bR[66];
			leadAmp->bR[66] = leadAmp->bR[65];
			leadAmp->bR[65] = leadAmp->bR[64];
			leadAmp->bR[64] = leadAmp->bR[63];
			leadAmp->bR[63] = leadAmp->bR[62];
			leadAmp->bR[62] = leadAmp->bR[61];
			leadAmp->bR[61] = leadAmp->bR[60];
			leadAmp->bR[60] = leadAmp->bR[59];
			leadAmp->bR[59] = leadAmp->bR[58];
			leadAmp->bR[58] = leadAmp->bR[57];
			leadAmp->bR[57] = leadAmp->bR[56];
			leadAmp->bR[56] = leadAmp->bR[55];
			leadAmp->bR[55] = leadAmp->bR[54];
			leadAmp->bR[54] = leadAmp->bR[53];
			leadAmp->bR[53] = leadAmp->bR[52];
			leadAmp->bR[52] = leadAmp->bR[51];
			leadAmp->bR[51] = leadAmp->bR[50];
			leadAmp->bR[50] = leadAmp->bR[49];
			leadAmp->bR[49] = leadAmp->bR[48];
			leadAmp->bR[48] = leadAmp->bR[47];
			leadAmp->bR[47] = leadAmp->bR[46];
			leadAmp->bR[46] = leadAmp->bR[45];
			leadAmp->bR[45] = leadAmp->bR[44];
			leadAmp->bR[44] = leadAmp->bR[43];
			leadAmp->bR[43] = leadAmp->bR[42];
			leadAmp->bR[42] = leadAmp->bR[41];
			leadAmp->bR[41] = leadAmp->bR[40];
			leadAmp->bR[40] = leadAmp->bR[39];
			leadAmp->bR[39] = leadAmp->bR[38];
			leadAmp->bR[38] = leadAmp->bR[37];
			leadAmp->bR[37] = leadAmp->bR[36];
			leadAmp->bR[36] = leadAmp->bR[35];
			leadAmp->bR[35] = leadAmp->bR[34];
			leadAmp->bR[34] = leadAmp->bR[33];
			leadAmp->bR[33] = leadAmp->bR[32];
			leadAmp->bR[32] = leadAmp->bR[31];
			leadAmp->bR[31] = leadAmp->bR[30];
			leadAmp->bR[30] = leadAmp->bR[29];
			leadAmp->bR[29] = leadAmp->bR[28];
			leadAmp->bR[28] = leadAmp->bR[27];
			leadAmp->bR[27] = leadAmp->bR[26];
			leadAmp->bR[26] = leadAmp->bR[25];
			leadAmp->bR[25] = leadAmp->bR[24];
			leadAmp->bR[24] = leadAmp->bR[23];
			leadAmp->bR[23] = leadAmp->bR[22];
			leadAmp->bR[22] = leadAmp->bR[21];
			leadAmp->bR[21] = leadAmp->bR[20];
			leadAmp->bR[20] = leadAmp->bR[19];
			leadAmp->bR[19] = leadAmp->bR[18];
			leadAmp->bR[18] = leadAmp->bR[17];
			leadAmp->bR[17] = leadAmp->bR[16];
			leadAmp->bR[16] = leadAmp->bR[15];
			leadAmp->bR[15] = leadAmp->bR[14];
			leadAmp->bR[14] = leadAmp->bR[13];
			leadAmp->bR[13] = leadAmp->bR[12];
			leadAmp->bR[12] = leadAmp->bR[11];
			leadAmp->bR[11] = leadAmp->bR[10];
			leadAmp->bR[10] = leadAmp->bR[9];
			leadAmp->bR[9] = leadAmp->bR[8];
			leadAmp->bR[8] = leadAmp->bR[7];
			leadAmp->bR[7] = leadAmp->bR[6];
			leadAmp->bR[6] = leadAmp->bR[5];
			leadAmp->bR[5] = leadAmp->bR[4];
			leadAmp->bR[4] = leadAmp->bR[3];
			leadAmp->bR[3] = leadAmp->bR[2];
			leadAmp->bR[2] = leadAmp->bR[1];
			leadAmp->bR[1] = leadAmp->bR[0];
			leadAmp->bR[0] = inputSampleR;
			inputSampleR += (leadAmp->bR[1] * (1.30406584776167445 - (0.01410622186823351 * fabs(leadAmp->bR[1]))));
			inputSampleR += (leadAmp->bR[2] * (1.09350974154373559 + (0.34478044709202327 * fabs(leadAmp->bR[2]))));
			inputSampleR += (leadAmp->bR[3] * (0.52285510059938256 + (0.84225842837363574 * fabs(leadAmp->bR[3]))));
			inputSampleR -= (leadAmp->bR[4] * (0.00018126260714707 - (1.02446537989058117 * fabs(leadAmp->bR[4]))));
			inputSampleR -= (leadAmp->bR[5] * (0.34943699771860115 - (0.84094709567790016 * fabs(leadAmp->bR[5]))));
			inputSampleR -= (leadAmp->bR[6] * (0.53068048407937285 - (0.49231169327705593 * fabs(leadAmp->bR[6]))));
			inputSampleR -= (leadAmp->bR[7] * (0.48631669406792399 - (0.08965111766223610 * fabs(leadAmp->bR[7]))));
			inputSampleR -= (leadAmp->bR[8] * (0.28099201947014130 + (0.23921137841068607 * fabs(leadAmp->bR[8]))));
			inputSampleR -= (leadAmp->bR[9] * (0.10333290012666248 + (0.35058962687321482 * fabs(leadAmp->bR[9]))));
			inputSampleR -= (leadAmp->bR[10] * (0.06605032198166226 + (0.23447405567823365 * fabs(leadAmp->bR[10]))));
			inputSampleR -= (leadAmp->bR[11] * (0.10485808661261729 + (0.05025985449763527 * fabs(leadAmp->bR[11]))));
			inputSampleR -= (leadAmp->bR[12] * (0.13231190973014911 - (0.05484648240248013 * fabs(leadAmp->bR[12]))));
			inputSampleR -= (leadAmp->bR[13] * (0.12926184767180304 - (0.04054223744746116 * fabs(leadAmp->bR[13]))));
			inputSampleR -= (leadAmp->bR[14] * (0.13802696739839460 + (0.01876754906568237 * fabs(leadAmp->bR[14]))));
			inputSampleR -= (leadAmp->bR[15] * (0.16548980700926913 + (0.06772130758771169 * fabs(leadAmp->bR[15]))));
			inputSampleR -= (leadAmp->bR[16] * (0.14469310965751475 + (0.10590928840978781 * fabs(leadAmp->bR[16]))));
			inputSampleR -= (leadAmp->bR[17] * (0.07838457396093310 + (0.13120101199677947 * fabs(leadAmp->bR[17]))));
			inputSampleR -= (leadAmp->bR[18] * (0.05123031606187391 + (0.13883400806512292 * fabs(leadAmp->bR[18]))));
			inputSampleR -= (leadAmp->bR[19] * (0.08906103481939850 + (0.07840461228402337 * fabs(leadAmp->bR[19]))));
			inputSampleR -= (leadAmp->bR[20] * (0.13939265522625241 + (0.01194366471800457 * fabs(leadAmp->bR[20]))));
			inputSampleR -= (leadAmp->bR[21] * (0.14957600717294034 + (0.07687598594361914 * fabs(leadAmp->bR[21]))));
			inputSampleR -= (leadAmp->bR[22] * (0.14112708654047090 + (0.20118461131186977 * fabs(leadAmp->bR[22]))));
			inputSampleR -= (leadAmp->bR[23] * (0.14961020766492997 + (0.30005716443826147 * fabs(leadAmp->bR[23]))));
			inputSampleR -= (leadAmp->bR[24] * (0.16130382224652270 + (0.40459872030013055 * fabs(leadAmp->bR[24]))));
			inputSampleR -= (leadAmp->bR[25] * (0.15679868471080052 + (0.47292767226083465 * fabs(leadAmp->bR[25]))));
			inputSampleR -= (leadAmp->bR[26] * (0.16456530552807727 + (0.45182121471666481 * fabs(leadAmp->bR[26]))));
			inputSampleR -= (leadAmp->bR[27] * (0.16852385701909278 + (0.38272684270752266 * fabs(leadAmp->bR[27]))));
			inputSampleR -= (leadAmp->bR[28] * (0.13317562760966850 + (0.28829580273670768 * fabs(leadAmp->bR[28]))));
			inputSampleR -= (leadAmp->bR[29] * (0.09396196532150952 + (0.18886898332071317 * fabs(leadAmp->bR[29]))));
			inputSampleR -= (leadAmp->bR[30] * (0.10133496956734221 + (0.11158788414137354 * fabs(leadAmp->bR[30]))));
			inputSampleR -= (leadAmp->bR[31] * (0.16097596389376778 + (0.02621299102374547 * fabs(leadAmp->bR[31]))));
			inputSampleR -= (leadAmp->bR[32] * (0.21419006394821866 - (0.03585678078834797 * fabs(leadAmp->bR[32]))));
			inputSampleR -= (leadAmp->bR[33] * (0.21273234570555244 - (0.02574469802924526 * fabs(leadAmp->bR[33]))));
			inputSampleR -= (leadAmp->bR[34] * (0.16934948798707830 + (0.01354331184333835 * fabs(leadAmp->bR[34]))));
			inputSampleR -= (leadAmp->bR[35] * (0.11970436472852493 + (0.04242183865883427 * fabs(leadAmp->bR[35]))));
			inputSampleR -= (leadAmp->bR[36] * (0.09329023656747724 + (0.06890873292358397 * fabs(leadAmp->bR[36]))));
			inputSampleR -= (leadAmp->bR[37] * (0.10255328436608116 + (0.11482972519137427 * fabs(leadAmp->bR[37]))));
			inputSampleR -= (leadAmp->bR[38] * (0.13883223352796811 + (0.18016014431438840 * fabs(leadAmp->bR[38]))));
			inputSampleR -= (leadAmp->bR[39] * (0.16532844286979087 + (0.24521957638633446 * fabs(leadAmp->bR[39]))));
			inputSampleR -= (leadAmp->bR[40] * (0.16254607738965438 + (0.25669472097572482 * fabs(leadAmp->bR[40]))));
			inputSampleR -= (leadAmp->bR[41] * (0.15353207135544752 + (0.15048064682912729 * fabs(leadAmp->bR[41]))));
			inputSampleR -= (leadAmp->bR[42] * (0.13039046390746015 - (0.00200335414623601 * fabs(leadAmp->bR[42]))));
			inputSampleR -= (leadAmp->bR[43] * (0.06707245032180627 - (0.06498125592578702 * fabs(leadAmp->bR[43]))));
			inputSampleR += (leadAmp->bR[44] * (0.01427326441869788 + (0.01940451360783622 * fabs(leadAmp->bR[44]))));
			inputSampleR += (leadAmp->bR[45] * (0.06151238306578224 - (0.07335755969763329 * fabs(leadAmp->bR[45]))));
			inputSampleR += (leadAmp->bR[46] * (0.04685840498892526 - (0.14258849371688248 * fabs(leadAmp->bR[46]))));
			inputSampleR -= (leadAmp->bR[47] * (0.00950136304466093 + (0.14379354707665129 * fabs(leadAmp->bR[47]))));
			inputSampleR -= (leadAmp->bR[48] * (0.06245771575493557 + (0.07639718586346110 * fabs(leadAmp->bR[48]))));
			inputSampleR -= (leadAmp->bR[49] * (0.07159593175777741 - (0.00595536565276915 * fabs(leadAmp->bR[49]))));
			inputSampleR -= (leadAmp->bR[50] * (0.03167929390245019 - (0.03856769526301793 * fabs(leadAmp->bR[50]))));
			inputSampleR += (leadAmp->bR[51] * (0.01890898565110766 + (0.00760539424271147 * fabs(leadAmp->bR[51]))));
			inputSampleR += (leadAmp->bR[52] * (0.04926161137832240 - (0.06411014430053390 * fabs(leadAmp->bR[52]))));
			inputSampleR += (leadAmp->bR[53] * (0.05768814623421683 - (0.15068618173358578 * fabs(leadAmp->bR[53]))));
			inputSampleR += (leadAmp->bR[54] * (0.06144258297076708 - (0.21200636329120301 * fabs(leadAmp->bR[54]))));
			inputSampleR += (leadAmp->bR[55] * (0.06348341960185613 - (0.19620269813094307 * fabs(leadAmp->bR[55]))));
			inputSampleR += (leadAmp->bR[56] * (0.04877736350310589 - (0.11864999881200111 * fabs(leadAmp->bR[56]))));
			inputSampleR += (leadAmp->bR[57] * (0.01010950997574472 - (0.02630070679113791 * fabs(leadAmp->bR[57]))));
			inputSampleR -= (leadAmp->bR[58] * (0.02929178864801191 - (0.04439260202207482 * fabs(leadAmp->bR[58]))));
			inputSampleR -= (leadAmp->bR[59] * (0.03484517126321562 - (0.04508635396034735 * fabs(leadAmp->bR[59]))));
			inputSampleR -= (leadAmp->bR[60] * (0.00547176780437610 - (0.00205637806941426 * fabs(leadAmp->bR[60]))));
			inputSampleR += (leadAmp->bR[61] * (0.02278296865283977 - (0.00063732526427685 * fabs(leadAmp->bR[61]))));
			inputSampleR += (leadAmp->bR[62] * (0.02688982591366477 + (0.05333738901586284 * fabs(leadAmp->bR[62]))));
			inputSampleR += (leadAmp->bR[63] * (0.01942012754957055 + (0.10942832669749143 * fabs(leadAmp->bR[63]))));
			inputSampleR += (leadAmp->bR[64] * (0.01572585258756565 + (0.11189204189054594 * fabs(leadAmp->bR[64]))));
			inputSampleR += (leadAmp->bR[65] * (0.01490550715016034 + (0.04449822818925343 * fabs(leadAmp->bR[65]))));
			inputSampleR += (leadAmp->bR[66] * (0.01715683226376727 - (0.06944648050933899 * fabs(leadAmp->bR[66]))));
			inputSampleR += (leadAmp->bR[67] * (0.02822659878011318 - (0.17843652160132820 * fabs(leadAmp->bR[67]))));
			inputSampleR += (leadAmp->bR[68] * (0.03758307610456144 - (0.21986013433664692 * fabs(leadAmp->bR[68]))));
			inputSampleR += (leadAmp->bR[69] * (0.03275008021608433 - (0.15869878676112170 * fabs(leadAmp->bR[69]))));
			inputSampleR += (leadAmp->bR[70] * (0.01855749786752354 - (0.02337224995718105 * fabs(leadAmp->bR[70]))));
			inputSampleR += (leadAmp->bR[71] * (0.00217095395782931 + (0.10971764224593601 * fabs(leadAmp->bR[71]))));
			inputSampleR -= (leadAmp->bR[72] * (0.01851381451105007 - (0.17214910008793413 * fabs(leadAmp->bR[72]))));
			inputSampleR -= (leadAmp->bR[73] * (0.04722574936345419 - (0.14341588977845254 * fabs(leadAmp->bR[73]))));
			inputSampleR -= (leadAmp->bR[74] * (0.07151540514482006 - (0.04684695724814321 * fabs(leadAmp->bR[74]))));
			inputSampleR -= (leadAmp->bR[75] * (0.06827195484995092 + (0.07022207121861397 * fabs(leadAmp->bR[75]))));
			inputSampleR -= (leadAmp->bR[76] * (0.03290227240464227 + (0.16328400808152735 * fabs(leadAmp->bR[76]))));
			inputSampleR += (leadAmp->bR[77] * (0.01043861198275382 - (0.20184486126076279 * fabs(leadAmp->bR[77]))));
			inputSampleR += (leadAmp->bR[78] * (0.03236563559476477 - (0.17125821306380920 * fabs(leadAmp->bR[78]))));
			inputSampleR += (leadAmp->bR[79] * (0.02040121529932702 - (0.09103660189829657 * fabs(leadAmp->bR[79]))));
			inputSampleR -= (leadAmp->bR[80] * (0.00509649513318102 + (0.01170360991547489 * fabs(leadAmp->bR[80]))));
			inputSampleR -= (leadAmp->bR[81] * (0.01388353426600228 - (0.03588955538451771 * fabs(leadAmp->bR[81]))));
			inputSampleR -= (leadAmp->bR[82] * (0.00523671715033842 - (0.07068798057534148 * fabs(leadAmp->bR[82]))));
			inputSampleR += (leadAmp->bR[83] * (0.00665852487721137 + (0.11666210640054926 * fabs(leadAmp->bR[83]))));
			inputSampleR += (leadAmp->bR[84] * (0.01593540832939290 + (0.15844892856402149 * fabs(leadAmp->bR[84]))));
			inputSampleR += (leadAmp->bR[85] * (0.02080509201836796 + (0.17186274420065850 * fabs(leadAmp->bR[85]))));

			temp = (inputSampleR + leadAmp->smoothCabBR) / 3.0;
			leadAmp->smoothCabBR = inputSampleR;
			inputSampleR = temp / 4.0;

			randy = (((double) leadAmp->fpdR / UINT32_MAX) * 0.079);
			drySampleR = ((((inputSampleR * (1.0 - randy)) + (leadAmp->lastCabSampleR * randy)) * wet) + (drySampleR * (1.0 - wet))) * outputlevel;
			leadAmp->lastCabSampleR = inputSampleR;
			inputSampleR = drySampleR; // cab

			if (cycleEnd == 4) {
				leadAmp->lastRefL[0] = leadAmp->lastRefL[4]; // start from previous last
				leadAmp->lastRefL[2] = (leadAmp->lastRefL[0] + inputSampleL) / 2; // half
				leadAmp->lastRefL[1] = (leadAmp->lastRefL[0] + leadAmp->lastRefL[2]) / 2; // one quarter
				leadAmp->lastRefL[3] = (leadAmp->lastRefL[2] + inputSampleL) / 2; // three quarters
				leadAmp->lastRefL[4] = inputSampleL; // full
				leadAmp->lastRefR[0] = leadAmp->lastRefR[4]; // start from previous last
				leadAmp->lastRefR[2] = (leadAmp->lastRefR[0] + inputSampleR) / 2; // half
				leadAmp->lastRefR[1] = (leadAmp->lastRefR[0] + leadAmp->lastRefR[2]) / 2; // one quarter
				leadAmp->lastRefR[3] = (leadAmp->lastRefR[2] + inputSampleR) / 2; // three quarters
				leadAmp->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				leadAmp->lastRefL[0] = leadAmp->lastRefL[3]; // start from previous last
				leadAmp->lastRefL[2] = (leadAmp->lastRefL[0] + leadAmp->lastRefL[0] + inputSampleL) / 3; // third
				leadAmp->lastRefL[1] = (leadAmp->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				leadAmp->lastRefL[3] = inputSampleL; // full
				leadAmp->lastRefR[0] = leadAmp->lastRefR[3]; // start from previous last
				leadAmp->lastRefR[2] = (leadAmp->lastRefR[0] + leadAmp->lastRefR[0] + inputSampleR) / 3; // third
				leadAmp->lastRefR[1] = (leadAmp->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				leadAmp->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				leadAmp->lastRefL[0] = leadAmp->lastRefL[2]; // start from previous last
				leadAmp->lastRefL[1] = (leadAmp->lastRefL[0] + inputSampleL) / 2; // half
				leadAmp->lastRefL[2] = inputSampleL; // full
				leadAmp->lastRefR[0] = leadAmp->lastRefR[2]; // start from previous last
				leadAmp->lastRefR[1] = (leadAmp->lastRefR[0] + inputSampleR) / 2; // half
				leadAmp->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				leadAmp->lastRefL[0] = inputSampleL;
				leadAmp->lastRefR[0] = inputSampleR;
			}
			leadAmp->cycle = 0; // reset
			inputSampleL = leadAmp->lastRefL[leadAmp->cycle];
			inputSampleR = leadAmp->lastRefR[leadAmp->cycle];
		} else {
			inputSampleL = leadAmp->lastRefL[leadAmp->cycle];
			inputSampleR = leadAmp->lastRefR[leadAmp->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				leadAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + leadAmp->lastRefL[7]) * 0.5;
				leadAmp->lastRefL[7] = leadAmp->lastRefL[8]; // continue, do not break
				leadAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + leadAmp->lastRefR[7]) * 0.5;
				leadAmp->lastRefR[7] = leadAmp->lastRefR[8]; // continue, do not break
			case 3:
				leadAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + leadAmp->lastRefL[6]) * 0.5;
				leadAmp->lastRefL[6] = leadAmp->lastRefL[8]; // continue, do not break
				leadAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + leadAmp->lastRefR[6]) * 0.5;
				leadAmp->lastRefR[6] = leadAmp->lastRefR[8]; // continue, do not break
			case 2:
				leadAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + leadAmp->lastRefL[5]) * 0.5;
				leadAmp->lastRefL[5] = leadAmp->lastRefL[8]; // continue, do not break
				leadAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + leadAmp->lastRefR[5]) * 0.5;
				leadAmp->lastRefR[5] = leadAmp->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		leadAmp->fpdL ^= leadAmp->fpdL << 13;
		leadAmp->fpdL ^= leadAmp->fpdL >> 17;
		leadAmp->fpdL ^= leadAmp->fpdL << 5;
		inputSampleL += (((double) leadAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		leadAmp->fpdR ^= leadAmp->fpdR << 13;
		leadAmp->fpdR ^= leadAmp->fpdR >> 17;
		leadAmp->fpdR ^= leadAmp->fpdR << 5;
		inputSampleR += (((double) leadAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	LEADAMP_URI,
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
