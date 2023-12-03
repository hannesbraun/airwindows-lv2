#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define FIREAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/fireamp"

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
	double smoothLL;
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
	double iirSampleLL;
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
	double smoothLR;
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
	double iirSampleLR;
	double iirLowpassR;
	double iirSpkAR;
	double iirSpkBR;
	double iirSubR;
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
} FireAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	FireAmp* fireAmp = (FireAmp*) calloc(1, sizeof(FireAmp));
	fireAmp->sampleRate = rate;
	return (LV2_Handle) fireAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	FireAmp* fireAmp = (FireAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			fireAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			fireAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			fireAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			fireAmp->output[1] = (float*) data;
			break;
		case GAIN:
			fireAmp->gain = (const float*) data;
			break;
		case TONE:
			fireAmp->tone = (const float*) data;
			break;
		case OUTPUT:
			fireAmp->outputGain = (const float*) data;
			break;
		case DRY_WET:
			fireAmp->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	FireAmp* fireAmp = (FireAmp*) instance;

	fireAmp->lastSampleL = 0.0;
	fireAmp->storeSampleL = 0.0;
	fireAmp->smoothAL = 0.0;
	fireAmp->smoothBL = 0.0;
	fireAmp->smoothCL = 0.0;
	fireAmp->smoothDL = 0.0;
	fireAmp->smoothEL = 0.0;
	fireAmp->smoothFL = 0.0;
	fireAmp->smoothGL = 0.0;
	fireAmp->smoothHL = 0.0;
	fireAmp->smoothIL = 0.0;
	fireAmp->smoothJL = 0.0;
	fireAmp->smoothKL = 0.0;
	fireAmp->smoothLL = 0.0;
	fireAmp->iirSampleAL = 0.0;
	fireAmp->iirSampleBL = 0.0;
	fireAmp->iirSampleCL = 0.0;
	fireAmp->iirSampleDL = 0.0;
	fireAmp->iirSampleEL = 0.0;
	fireAmp->iirSampleFL = 0.0;
	fireAmp->iirSampleGL = 0.0;
	fireAmp->iirSampleHL = 0.0;
	fireAmp->iirSampleIL = 0.0;
	fireAmp->iirSampleJL = 0.0;
	fireAmp->iirSampleKL = 0.0;
	fireAmp->iirSampleLL = 0.0;
	fireAmp->iirLowpassL = 0.0;
	fireAmp->iirSpkAL = 0.0;
	fireAmp->iirSpkBL = 0.0;
	fireAmp->iirSubL = 0.0;

	fireAmp->lastSampleR = 0.0;
	fireAmp->storeSampleR = 0.0;
	fireAmp->smoothAR = 0.0;
	fireAmp->smoothBR = 0.0;
	fireAmp->smoothCR = 0.0;
	fireAmp->smoothDR = 0.0;
	fireAmp->smoothER = 0.0;
	fireAmp->smoothFR = 0.0;
	fireAmp->smoothGR = 0.0;
	fireAmp->smoothHR = 0.0;
	fireAmp->smoothIR = 0.0;
	fireAmp->smoothJR = 0.0;
	fireAmp->smoothKR = 0.0;
	fireAmp->smoothLR = 0.0;
	fireAmp->iirSampleAR = 0.0;
	fireAmp->iirSampleBR = 0.0;
	fireAmp->iirSampleCR = 0.0;
	fireAmp->iirSampleDR = 0.0;
	fireAmp->iirSampleER = 0.0;
	fireAmp->iirSampleFR = 0.0;
	fireAmp->iirSampleGR = 0.0;
	fireAmp->iirSampleHR = 0.0;
	fireAmp->iirSampleIR = 0.0;
	fireAmp->iirSampleJR = 0.0;
	fireAmp->iirSampleKR = 0.0;
	fireAmp->iirSampleLR = 0.0;
	fireAmp->iirLowpassR = 0.0;
	fireAmp->iirSpkAR = 0.0;
	fireAmp->iirSpkBR = 0.0;
	fireAmp->iirSubR = 0.0;

	for (int fcount = 0; fcount < 257; fcount++) {
		fireAmp->OddL[fcount] = 0.0;
		fireAmp->EvenL[fcount] = 0.0;
		fireAmp->OddR[fcount] = 0.0;
		fireAmp->EvenR[fcount] = 0.0;
	}

	fireAmp->count = 0;
	fireAmp->flip = false; // amp

	for (int fcount = 0; fcount < 90; fcount++) {
		fireAmp->bL[fcount] = 0;
		fireAmp->bR[fcount] = 0;
	}
	fireAmp->smoothCabAL = 0.0;
	fireAmp->smoothCabBL = 0.0;
	fireAmp->lastCabSampleL = 0.0; // cab
	fireAmp->smoothCabAR = 0.0;
	fireAmp->smoothCabBR = 0.0;
	fireAmp->lastCabSampleR = 0.0; // cab

	for (int fcount = 0; fcount < 9; fcount++) {
		fireAmp->lastRefL[fcount] = 0.0;
		fireAmp->lastRefR[fcount] = 0.0;
	}
	fireAmp->cycle = 0; // undersampling

	for (int x = 0; x < fix_total; x++) {
		fireAmp->fixA[x] = 0.0;
		fireAmp->fixB[x] = 0.0;
		fireAmp->fixC[x] = 0.0;
		fireAmp->fixD[x] = 0.0;
		fireAmp->fixE[x] = 0.0;
		fireAmp->fixF[x] = 0.0;
	} // filtering

	fireAmp->fpdL = 1.0;
	while (fireAmp->fpdL < 16386) fireAmp->fpdL = rand() * UINT32_MAX;
	fireAmp->fpdR = 1.0;
	while (fireAmp->fpdR < 16386) fireAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	FireAmp* fireAmp = (FireAmp*) instance;

	const float* in1 = fireAmp->input[0];
	const float* in2 = fireAmp->input[1];
	float* out1 = fireAmp->output[0];
	float* out2 = fireAmp->output[1];

	const double sampleRate = fireAmp->sampleRate;
	const float tone = *fireAmp->tone;

	double bassfill = *fireAmp->gain;
	double outputlevel = *fireAmp->outputGain;
	double wet = *fireAmp->dryWet;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (fireAmp->cycle > cycleEnd - 1) fireAmp->cycle = cycleEnd - 1; // sanity check

	double startlevel = bassfill;
	double samplerate = sampleRate;
	double basstrim = bassfill / 16.0;
	double toneEQ = (tone / samplerate) * 22050.0;
	double EQ = (basstrim / samplerate) * 22050.0;
	double bleed = outputlevel / 16.0;
	double bassfactor = 1.0 - (basstrim * basstrim);
	double BEQ = (bleed / samplerate) * 22050.0;
	int diagonal = (int) (0.000861678 * samplerate);
	if (diagonal > 127) diagonal = 127;
	int side = (int) (diagonal / 1.4142135623730951);
	int down = (side + diagonal) / 2;
	// now we've got down, side and diagonal as offsets and we also use three successive samples upfront

	double cutoff = (15000.0 + (tone * 10000.0)) / sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low

	fireAmp->fixF[fix_freq] = fireAmp->fixE[fix_freq] = fireAmp->fixD[fix_freq] = fireAmp->fixC[fix_freq] = fireAmp->fixB[fix_freq] = fireAmp->fixA[fix_freq] = cutoff;

	fireAmp->fixA[fix_reso] = 4.46570214;
	fireAmp->fixB[fix_reso] = 1.51387132;
	fireAmp->fixC[fix_reso] = 0.93979296;
	fireAmp->fixD[fix_reso] = 0.70710678;
	fireAmp->fixE[fix_reso] = 0.52972649;
	fireAmp->fixF[fix_reso] = 0.50316379;

	double K = tan(M_PI * fireAmp->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / fireAmp->fixA[fix_reso] + K * K);
	fireAmp->fixA[fix_a0] = K * K * norm;
	fireAmp->fixA[fix_a1] = 2.0 * fireAmp->fixA[fix_a0];
	fireAmp->fixA[fix_a2] = fireAmp->fixA[fix_a0];
	fireAmp->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fireAmp->fixA[fix_b2] = (1.0 - K / fireAmp->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * fireAmp->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / fireAmp->fixB[fix_reso] + K * K);
	fireAmp->fixB[fix_a0] = K * K * norm;
	fireAmp->fixB[fix_a1] = 2.0 * fireAmp->fixB[fix_a0];
	fireAmp->fixB[fix_a2] = fireAmp->fixB[fix_a0];
	fireAmp->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fireAmp->fixB[fix_b2] = (1.0 - K / fireAmp->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * fireAmp->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / fireAmp->fixC[fix_reso] + K * K);
	fireAmp->fixC[fix_a0] = K * K * norm;
	fireAmp->fixC[fix_a1] = 2.0 * fireAmp->fixC[fix_a0];
	fireAmp->fixC[fix_a2] = fireAmp->fixC[fix_a0];
	fireAmp->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fireAmp->fixC[fix_b2] = (1.0 - K / fireAmp->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * fireAmp->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / fireAmp->fixD[fix_reso] + K * K);
	fireAmp->fixD[fix_a0] = K * K * norm;
	fireAmp->fixD[fix_a1] = 2.0 * fireAmp->fixD[fix_a0];
	fireAmp->fixD[fix_a2] = fireAmp->fixD[fix_a0];
	fireAmp->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fireAmp->fixD[fix_b2] = (1.0 - K / fireAmp->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * fireAmp->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / fireAmp->fixE[fix_reso] + K * K);
	fireAmp->fixE[fix_a0] = K * K * norm;
	fireAmp->fixE[fix_a1] = 2.0 * fireAmp->fixE[fix_a0];
	fireAmp->fixE[fix_a2] = fireAmp->fixE[fix_a0];
	fireAmp->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fireAmp->fixE[fix_b2] = (1.0 - K / fireAmp->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * fireAmp->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / fireAmp->fixF[fix_reso] + K * K);
	fireAmp->fixF[fix_a0] = K * K * norm;
	fireAmp->fixF[fix_a1] = 2.0 * fireAmp->fixF[fix_a0];
	fireAmp->fixF[fix_a2] = fireAmp->fixF[fix_a0];
	fireAmp->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fireAmp->fixF[fix_b2] = (1.0 - K / fireAmp->fixF[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = fireAmp->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = fireAmp->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double outSample = (inputSampleL * fireAmp->fixA[fix_a0]) + fireAmp->fixA[fix_sL1];
		fireAmp->fixA[fix_sL1] = (inputSampleL * fireAmp->fixA[fix_a1]) - (outSample * fireAmp->fixA[fix_b1]) + fireAmp->fixA[fix_sL2];
		fireAmp->fixA[fix_sL2] = (inputSampleL * fireAmp->fixA[fix_a2]) - (outSample * fireAmp->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fireAmp->fixA[fix_a0]) + fireAmp->fixA[fix_sR1];
		fireAmp->fixA[fix_sR1] = (inputSampleR * fireAmp->fixA[fix_a1]) - (outSample * fireAmp->fixA[fix_b1]) + fireAmp->fixA[fix_sR2];
		fireAmp->fixA[fix_sR2] = (inputSampleR * fireAmp->fixA[fix_a2]) - (outSample * fireAmp->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		double basscutL = 0.98;
		// we're going to be shifting this as the stages progress
		double inputlevelL = startlevel;
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleAL = (fireAmp->iirSampleAL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleAL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		double bridgerectifier = (fireAmp->smoothAL + inputSampleL);
		fireAmp->smoothAL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleBL = (fireAmp->iirSampleBL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleBL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothBL + inputSampleL);
		fireAmp->smoothBL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		double basscutR = 0.98;
		// we're going to be shifting this as the stages progress
		double inputlevelR = startlevel;
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleAR = (fireAmp->iirSampleAR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleAR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothAR + inputSampleR);
		fireAmp->smoothAR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleBR = (fireAmp->iirSampleBR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleBR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothBR + inputSampleR);
		fireAmp->smoothBR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * fireAmp->fixB[fix_a0]) + fireAmp->fixB[fix_sL1];
		fireAmp->fixB[fix_sL1] = (inputSampleL * fireAmp->fixB[fix_a1]) - (outSample * fireAmp->fixB[fix_b1]) + fireAmp->fixB[fix_sL2];
		fireAmp->fixB[fix_sL2] = (inputSampleL * fireAmp->fixB[fix_a2]) - (outSample * fireAmp->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fireAmp->fixB[fix_a0]) + fireAmp->fixB[fix_sR1];
		fireAmp->fixB[fix_sR1] = (inputSampleR * fireAmp->fixB[fix_a1]) - (outSample * fireAmp->fixB[fix_b1]) + fireAmp->fixB[fix_sR2];
		fireAmp->fixB[fix_sR2] = (inputSampleR * fireAmp->fixB[fix_a2]) - (outSample * fireAmp->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleCL = (fireAmp->iirSampleCL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleCL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothCL + inputSampleL);
		fireAmp->smoothCL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleDL = (fireAmp->iirSampleDL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleDL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothDL + inputSampleL);
		fireAmp->smoothDL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleCR = (fireAmp->iirSampleCR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleCR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothCR + inputSampleR);
		fireAmp->smoothCR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleDR = (fireAmp->iirSampleDR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleDR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothDR + inputSampleR);
		fireAmp->smoothDR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * fireAmp->fixC[fix_a0]) + fireAmp->fixC[fix_sL1];
		fireAmp->fixC[fix_sL1] = (inputSampleL * fireAmp->fixC[fix_a1]) - (outSample * fireAmp->fixC[fix_b1]) + fireAmp->fixC[fix_sL2];
		fireAmp->fixC[fix_sL2] = (inputSampleL * fireAmp->fixC[fix_a2]) - (outSample * fireAmp->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fireAmp->fixC[fix_a0]) + fireAmp->fixC[fix_sR1];
		fireAmp->fixC[fix_sR1] = (inputSampleR * fireAmp->fixC[fix_a1]) - (outSample * fireAmp->fixC[fix_b1]) + fireAmp->fixC[fix_sR2];
		fireAmp->fixC[fix_sR2] = (inputSampleR * fireAmp->fixC[fix_a2]) - (outSample * fireAmp->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleEL = (fireAmp->iirSampleEL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleEL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothEL + inputSampleL);
		fireAmp->smoothEL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleFL = (fireAmp->iirSampleFL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleFL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothFL + inputSampleL);
		fireAmp->smoothFL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleER = (fireAmp->iirSampleER * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleER * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothER + inputSampleR);
		fireAmp->smoothER = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleFR = (fireAmp->iirSampleFR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleFR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothFR + inputSampleR);
		fireAmp->smoothFR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * fireAmp->fixD[fix_a0]) + fireAmp->fixD[fix_sL1];
		fireAmp->fixD[fix_sL1] = (inputSampleL * fireAmp->fixD[fix_a1]) - (outSample * fireAmp->fixD[fix_b1]) + fireAmp->fixD[fix_sL2];
		fireAmp->fixD[fix_sL2] = (inputSampleL * fireAmp->fixD[fix_a2]) - (outSample * fireAmp->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fireAmp->fixD[fix_a0]) + fireAmp->fixD[fix_sR1];
		fireAmp->fixD[fix_sR1] = (inputSampleR * fireAmp->fixD[fix_a1]) - (outSample * fireAmp->fixD[fix_b1]) + fireAmp->fixD[fix_sR2];
		fireAmp->fixD[fix_sR2] = (inputSampleR * fireAmp->fixD[fix_a2]) - (outSample * fireAmp->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleGL = (fireAmp->iirSampleGL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleGL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothGL + inputSampleL);
		fireAmp->smoothGL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleHL = (fireAmp->iirSampleHL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleHL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothHL + inputSampleL);
		fireAmp->smoothHL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleGR = (fireAmp->iirSampleGR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleGR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothGR + inputSampleR);
		fireAmp->smoothGR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleHR = (fireAmp->iirSampleHR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleHR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothHR + inputSampleR);
		fireAmp->smoothHR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * fireAmp->fixE[fix_a0]) + fireAmp->fixE[fix_sL1];
		fireAmp->fixE[fix_sL1] = (inputSampleL * fireAmp->fixE[fix_a1]) - (outSample * fireAmp->fixE[fix_b1]) + fireAmp->fixE[fix_sL2];
		fireAmp->fixE[fix_sL2] = (inputSampleL * fireAmp->fixE[fix_a2]) - (outSample * fireAmp->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fireAmp->fixE[fix_a0]) + fireAmp->fixE[fix_sR1];
		fireAmp->fixE[fix_sR1] = (inputSampleR * fireAmp->fixE[fix_a1]) - (outSample * fireAmp->fixE[fix_b1]) + fireAmp->fixE[fix_sR2];
		fireAmp->fixE[fix_sR2] = (inputSampleR * fireAmp->fixE[fix_a2]) - (outSample * fireAmp->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleIL = (fireAmp->iirSampleIL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleIL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothIL + inputSampleL);
		fireAmp->smoothIL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleJL = (fireAmp->iirSampleJL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleJL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothJL + inputSampleL);
		fireAmp->smoothJL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleIR = (fireAmp->iirSampleIR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleIR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothIR + inputSampleR);
		fireAmp->smoothIR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleJR = (fireAmp->iirSampleJR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleJR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothJR + inputSampleR);
		fireAmp->smoothJR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		outSample = (inputSampleL * fireAmp->fixF[fix_a0]) + fireAmp->fixF[fix_sL1];
		fireAmp->fixF[fix_sL1] = (inputSampleL * fireAmp->fixF[fix_a1]) - (outSample * fireAmp->fixF[fix_b1]) + fireAmp->fixF[fix_sL2];
		fireAmp->fixF[fix_sL2] = (inputSampleL * fireAmp->fixF[fix_a2]) - (outSample * fireAmp->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fireAmp->fixF[fix_a0]) + fireAmp->fixF[fix_sR1];
		fireAmp->fixF[fix_sR1] = (inputSampleR * fireAmp->fixF[fix_a1]) - (outSample * fireAmp->fixF[fix_b1]) + fireAmp->fixF[fix_sR2];
		fireAmp->fixF[fix_sR2] = (inputSampleR * fireAmp->fixF[fix_a2]) - (outSample * fireAmp->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleKL = (fireAmp->iirSampleKL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleKL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothKL + inputSampleL);
		fireAmp->smoothKL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleL *= inputlevelL;
		inputlevelL = ((inputlevelL * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleLL = (fireAmp->iirSampleLL * (1.0 - EQ)) + (inputSampleL * EQ);
		basscutL *= bassfactor;
		inputSampleL = inputSampleL - (fireAmp->iirSampleLL * basscutL);
		// highpass
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothLL + inputSampleL);
		fireAmp->smoothLL = inputSampleL;
		inputSampleL = bridgerectifier;
		// two-sample averaging lowpass

		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleKR = (fireAmp->iirSampleKR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleKR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothKR + inputSampleR);
		fireAmp->smoothKR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass
		inputSampleR *= inputlevelR;
		inputlevelR = ((inputlevelR * 7.0) + 1.0) / 8.0;
		fireAmp->iirSampleLR = (fireAmp->iirSampleLR * (1.0 - EQ)) + (inputSampleR * EQ);
		basscutR *= bassfactor;
		inputSampleR = inputSampleR - (fireAmp->iirSampleLR * basscutR);
		// highpass
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654));
		// overdrive
		bridgerectifier = (fireAmp->smoothLR + inputSampleR);
		fireAmp->smoothLR = inputSampleR;
		inputSampleR = bridgerectifier;
		// two-sample averaging lowpass

		fireAmp->iirLowpassL = (fireAmp->iirLowpassL * (1.0 - toneEQ)) + (inputSampleL * toneEQ);
		inputSampleL = fireAmp->iirLowpassL;
		// lowpass. The only one of this type.
		fireAmp->iirLowpassR = (fireAmp->iirLowpassR * (1.0 - toneEQ)) + (inputSampleR * toneEQ);
		inputSampleR = fireAmp->iirLowpassR;
		// lowpass. The only one of this type.

		fireAmp->iirSpkAL = (fireAmp->iirSpkAL * (1.0 - BEQ)) + (inputSampleL * BEQ);
		// extra lowpass for 4*12" speakers
		fireAmp->iirSpkAR = (fireAmp->iirSpkAR * (1.0 - BEQ)) + (inputSampleR * BEQ);
		// extra lowpass for 4*12" speakers

		if (fireAmp->count < 0 || fireAmp->count > 128) {
			fireAmp->count = 128;
		}
		double resultBL = 0.0;
		double resultBR = 0.0;
		if (fireAmp->flip) {
			fireAmp->OddL[fireAmp->count + 128] = fireAmp->OddL[fireAmp->count] = fireAmp->iirSpkAL;
			resultBL = (fireAmp->OddL[fireAmp->count + down] + fireAmp->OddL[fireAmp->count + side] + fireAmp->OddL[fireAmp->count + diagonal]);
			fireAmp->OddR[fireAmp->count + 128] = fireAmp->OddR[fireAmp->count] = fireAmp->iirSpkAR;
			resultBR = (fireAmp->OddR[fireAmp->count + down] + fireAmp->OddR[fireAmp->count + side] + fireAmp->OddR[fireAmp->count + diagonal]);
		} else {
			fireAmp->EvenL[fireAmp->count + 128] = fireAmp->EvenL[fireAmp->count] = fireAmp->iirSpkAL;
			resultBL = (fireAmp->EvenL[fireAmp->count + down] + fireAmp->EvenL[fireAmp->count + side] + fireAmp->EvenL[fireAmp->count + diagonal]);
			fireAmp->EvenR[fireAmp->count + 128] = fireAmp->EvenR[fireAmp->count] = fireAmp->iirSpkAR;
			resultBR = (fireAmp->EvenR[fireAmp->count + down] + fireAmp->EvenR[fireAmp->count + side] + fireAmp->EvenR[fireAmp->count + diagonal]);
		}
		fireAmp->count--;
		fireAmp->iirSpkBL = (fireAmp->iirSpkBL * (1.0 - BEQ)) + (resultBL * BEQ);
		inputSampleL += (fireAmp->iirSpkBL * bleed);
		// extra lowpass for 4*12" speakers
		fireAmp->iirSpkBR = (fireAmp->iirSpkBR * (1.0 - BEQ)) + (resultBR * BEQ);
		inputSampleR += (fireAmp->iirSpkBR * bleed);
		// extra lowpass for 4*12" speakers

		bridgerectifier = fabs(inputSampleL * outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;

		bridgerectifier = fabs(inputSampleR * outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;

		fireAmp->iirSubL = (fireAmp->iirSubL * (1.0 - BEQ)) + (inputSampleL * BEQ);
		inputSampleL += (fireAmp->iirSubL * bassfill * outputlevel);

		fireAmp->iirSubR = (fireAmp->iirSubR * (1.0 - BEQ)) + (inputSampleR * BEQ);
		inputSampleR += (fireAmp->iirSubR * bassfill * outputlevel);

		double randy = (((double) fireAmp->fpdL / UINT32_MAX) * 0.053);
		inputSampleL = ((inputSampleL * (1.0 - randy)) + (fireAmp->storeSampleL * randy)) * outputlevel;
		fireAmp->storeSampleL = inputSampleL;

		randy = (((double) fireAmp->fpdR / UINT32_MAX) * 0.053);
		inputSampleR = ((inputSampleR * (1.0 - randy)) + (fireAmp->storeSampleR * randy)) * outputlevel;
		fireAmp->storeSampleR = inputSampleR;

		fireAmp->flip = !fireAmp->flip;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider
		// amp

		fireAmp->cycle++;
		if (fireAmp->cycle == cycleEnd) {
			double temp = (inputSampleL + fireAmp->smoothCabAL) / 3.0;
			fireAmp->smoothCabAL = inputSampleL;
			inputSampleL = temp;

			fireAmp->bL[84] = fireAmp->bL[83];
			fireAmp->bL[83] = fireAmp->bL[82];
			fireAmp->bL[82] = fireAmp->bL[81];
			fireAmp->bL[81] = fireAmp->bL[80];
			fireAmp->bL[80] = fireAmp->bL[79];
			fireAmp->bL[79] = fireAmp->bL[78];
			fireAmp->bL[78] = fireAmp->bL[77];
			fireAmp->bL[77] = fireAmp->bL[76];
			fireAmp->bL[76] = fireAmp->bL[75];
			fireAmp->bL[75] = fireAmp->bL[74];
			fireAmp->bL[74] = fireAmp->bL[73];
			fireAmp->bL[73] = fireAmp->bL[72];
			fireAmp->bL[72] = fireAmp->bL[71];
			fireAmp->bL[71] = fireAmp->bL[70];
			fireAmp->bL[70] = fireAmp->bL[69];
			fireAmp->bL[69] = fireAmp->bL[68];
			fireAmp->bL[68] = fireAmp->bL[67];
			fireAmp->bL[67] = fireAmp->bL[66];
			fireAmp->bL[66] = fireAmp->bL[65];
			fireAmp->bL[65] = fireAmp->bL[64];
			fireAmp->bL[64] = fireAmp->bL[63];
			fireAmp->bL[63] = fireAmp->bL[62];
			fireAmp->bL[62] = fireAmp->bL[61];
			fireAmp->bL[61] = fireAmp->bL[60];
			fireAmp->bL[60] = fireAmp->bL[59];
			fireAmp->bL[59] = fireAmp->bL[58];
			fireAmp->bL[58] = fireAmp->bL[57];
			fireAmp->bL[57] = fireAmp->bL[56];
			fireAmp->bL[56] = fireAmp->bL[55];
			fireAmp->bL[55] = fireAmp->bL[54];
			fireAmp->bL[54] = fireAmp->bL[53];
			fireAmp->bL[53] = fireAmp->bL[52];
			fireAmp->bL[52] = fireAmp->bL[51];
			fireAmp->bL[51] = fireAmp->bL[50];
			fireAmp->bL[50] = fireAmp->bL[49];
			fireAmp->bL[49] = fireAmp->bL[48];
			fireAmp->bL[48] = fireAmp->bL[47];
			fireAmp->bL[47] = fireAmp->bL[46];
			fireAmp->bL[46] = fireAmp->bL[45];
			fireAmp->bL[45] = fireAmp->bL[44];
			fireAmp->bL[44] = fireAmp->bL[43];
			fireAmp->bL[43] = fireAmp->bL[42];
			fireAmp->bL[42] = fireAmp->bL[41];
			fireAmp->bL[41] = fireAmp->bL[40];
			fireAmp->bL[40] = fireAmp->bL[39];
			fireAmp->bL[39] = fireAmp->bL[38];
			fireAmp->bL[38] = fireAmp->bL[37];
			fireAmp->bL[37] = fireAmp->bL[36];
			fireAmp->bL[36] = fireAmp->bL[35];
			fireAmp->bL[35] = fireAmp->bL[34];
			fireAmp->bL[34] = fireAmp->bL[33];
			fireAmp->bL[33] = fireAmp->bL[32];
			fireAmp->bL[32] = fireAmp->bL[31];
			fireAmp->bL[31] = fireAmp->bL[30];
			fireAmp->bL[30] = fireAmp->bL[29];
			fireAmp->bL[29] = fireAmp->bL[28];
			fireAmp->bL[28] = fireAmp->bL[27];
			fireAmp->bL[27] = fireAmp->bL[26];
			fireAmp->bL[26] = fireAmp->bL[25];
			fireAmp->bL[25] = fireAmp->bL[24];
			fireAmp->bL[24] = fireAmp->bL[23];
			fireAmp->bL[23] = fireAmp->bL[22];
			fireAmp->bL[22] = fireAmp->bL[21];
			fireAmp->bL[21] = fireAmp->bL[20];
			fireAmp->bL[20] = fireAmp->bL[19];
			fireAmp->bL[19] = fireAmp->bL[18];
			fireAmp->bL[18] = fireAmp->bL[17];
			fireAmp->bL[17] = fireAmp->bL[16];
			fireAmp->bL[16] = fireAmp->bL[15];
			fireAmp->bL[15] = fireAmp->bL[14];
			fireAmp->bL[14] = fireAmp->bL[13];
			fireAmp->bL[13] = fireAmp->bL[12];
			fireAmp->bL[12] = fireAmp->bL[11];
			fireAmp->bL[11] = fireAmp->bL[10];
			fireAmp->bL[10] = fireAmp->bL[9];
			fireAmp->bL[9] = fireAmp->bL[8];
			fireAmp->bL[8] = fireAmp->bL[7];
			fireAmp->bL[7] = fireAmp->bL[6];
			fireAmp->bL[6] = fireAmp->bL[5];
			fireAmp->bL[5] = fireAmp->bL[4];
			fireAmp->bL[4] = fireAmp->bL[3];
			fireAmp->bL[3] = fireAmp->bL[2];
			fireAmp->bL[2] = fireAmp->bL[1];
			fireAmp->bL[1] = fireAmp->bL[0];
			fireAmp->bL[0] = inputSampleL;
			inputSampleL += (fireAmp->bL[1] * (1.31698250313308396 - (0.08140616497621633 * fabs(fireAmp->bL[1]))));
			inputSampleL += (fireAmp->bL[2] * (1.47229016949915326 - (0.27680278993637253 * fabs(fireAmp->bL[2]))));
			inputSampleL += (fireAmp->bL[3] * (1.30410109086044956 - (0.35629113432046489 * fabs(fireAmp->bL[3]))));
			inputSampleL += (fireAmp->bL[4] * (0.81766210474551260 - (0.26808782337659753 * fabs(fireAmp->bL[4]))));
			inputSampleL += (fireAmp->bL[5] * (0.19868872545506663 - (0.11105517193919669 * fabs(fireAmp->bL[5]))));
			inputSampleL -= (fireAmp->bL[6] * (0.39115909132567039 - (0.12630622002682679 * fabs(fireAmp->bL[6]))));
			inputSampleL -= (fireAmp->bL[7] * (0.76881891559343574 - (0.40879849500403143 * fabs(fireAmp->bL[7]))));
			inputSampleL -= (fireAmp->bL[8] * (0.87146861782680340 - (0.59529560488000599 * fabs(fireAmp->bL[8]))));
			inputSampleL -= (fireAmp->bL[9] * (0.79504575932563670 - (0.60877047551611796 * fabs(fireAmp->bL[9]))));
			inputSampleL -= (fireAmp->bL[10] * (0.61653017622406314 - (0.47662851438557335 * fabs(fireAmp->bL[10]))));
			inputSampleL -= (fireAmp->bL[11] * (0.40718195794382067 - (0.24955839378539713 * fabs(fireAmp->bL[11]))));
			inputSampleL -= (fireAmp->bL[12] * (0.31794900040616203 - (0.04169792259600613 * fabs(fireAmp->bL[12]))));
			inputSampleL -= (fireAmp->bL[13] * (0.41075032540217843 + (0.00368483996076280 * fabs(fireAmp->bL[13]))));
			inputSampleL -= (fireAmp->bL[14] * (0.56901352922170667 - (0.11027360805893105 * fabs(fireAmp->bL[14]))));
			inputSampleL -= (fireAmp->bL[15] * (0.62443222391889264 - (0.22198075154245228 * fabs(fireAmp->bL[15]))));
			inputSampleL -= (fireAmp->bL[16] * (0.53462856723129204 - (0.22933544545324852 * fabs(fireAmp->bL[16]))));
			inputSampleL -= (fireAmp->bL[17] * (0.34441703361995046 - (0.12956809502269492 * fabs(fireAmp->bL[17]))));
			inputSampleL -= (fireAmp->bL[18] * (0.13947052337867882 + (0.00339775055962799 * fabs(fireAmp->bL[18]))));
			inputSampleL += (fireAmp->bL[19] * (0.03771252648928484 - (0.10863931549251718 * fabs(fireAmp->bL[19]))));
			inputSampleL += (fireAmp->bL[20] * (0.18280210770271693 - (0.17413646599296417 * fabs(fireAmp->bL[20]))));
			inputSampleL += (fireAmp->bL[21] * (0.24621986701761467 - (0.14547053270435095 * fabs(fireAmp->bL[21]))));
			inputSampleL += (fireAmp->bL[22] * (0.22347075142737360 - (0.02493869490104031 * fabs(fireAmp->bL[22]))));
			inputSampleL += (fireAmp->bL[23] * (0.14346348482123716 + (0.11284054747963246 * fabs(fireAmp->bL[23]))));
			inputSampleL += (fireAmp->bL[24] * (0.00834364862916028 + (0.24284684053733926 * fabs(fireAmp->bL[24]))));
			inputSampleL -= (fireAmp->bL[25] * (0.11559740296078347 - (0.32623054435304538 * fabs(fireAmp->bL[25]))));
			inputSampleL -= (fireAmp->bL[26] * (0.18067604561283060 - (0.32311481551122478 * fabs(fireAmp->bL[26]))));
			inputSampleL -= (fireAmp->bL[27] * (0.22927997789035612 - (0.26991539052832925 * fabs(fireAmp->bL[27]))));
			inputSampleL -= (fireAmp->bL[28] * (0.28487666578669446 - (0.22437227250279349 * fabs(fireAmp->bL[28]))));
			inputSampleL -= (fireAmp->bL[29] * (0.31992973037153838 - (0.15289876100963865 * fabs(fireAmp->bL[29]))));
			inputSampleL -= (fireAmp->bL[30] * (0.35174606303520733 - (0.05656293023086628 * fabs(fireAmp->bL[30]))));
			inputSampleL -= (fireAmp->bL[31] * (0.36894898011375254 + (0.04333925421463558 * fabs(fireAmp->bL[31]))));
			inputSampleL -= (fireAmp->bL[32] * (0.32567576055307507 + (0.14594589410921388 * fabs(fireAmp->bL[32]))));
			inputSampleL -= (fireAmp->bL[33] * (0.27440135050585784 + (0.15529667398122521 * fabs(fireAmp->bL[33]))));
			inputSampleL -= (fireAmp->bL[34] * (0.21998973785078091 + (0.05083553737157104 * fabs(fireAmp->bL[34]))));
			inputSampleL -= (fireAmp->bL[35] * (0.10323624876862457 - (0.04651829594199963 * fabs(fireAmp->bL[35]))));
			inputSampleL += (fireAmp->bL[36] * (0.02091603687851074 + (0.12000046818439322 * fabs(fireAmp->bL[36]))));
			inputSampleL += (fireAmp->bL[37] * (0.11344930914138468 + (0.17697142512225839 * fabs(fireAmp->bL[37]))));
			inputSampleL += (fireAmp->bL[38] * (0.22766779627643968 + (0.13645102964003858 * fabs(fireAmp->bL[38]))));
			inputSampleL += (fireAmp->bL[39] * (0.38378309953638229 - (0.01997653307333791 * fabs(fireAmp->bL[39]))));
			inputSampleL += (fireAmp->bL[40] * (0.52789400804568076 - (0.21409137428422448 * fabs(fireAmp->bL[40]))));
			inputSampleL += (fireAmp->bL[41] * (0.55444630296938280 - (0.32331980931576626 * fabs(fireAmp->bL[41]))));
			inputSampleL += (fireAmp->bL[42] * (0.42333237669264601 - (0.26855847463044280 * fabs(fireAmp->bL[42]))));
			inputSampleL += (fireAmp->bL[43] * (0.21942831522035078 - (0.12051365248820624 * fabs(fireAmp->bL[43]))));
			inputSampleL -= (fireAmp->bL[44] * (0.00584169427830633 - (0.03706970171280329 * fabs(fireAmp->bL[44]))));
			inputSampleL -= (fireAmp->bL[45] * (0.24279799124660351 - (0.17296440491477982 * fabs(fireAmp->bL[45]))));
			inputSampleL -= (fireAmp->bL[46] * (0.40173760787507085 - (0.21717989835163351 * fabs(fireAmp->bL[46]))));
			inputSampleL -= (fireAmp->bL[47] * (0.43930035724188155 - (0.16425928481378199 * fabs(fireAmp->bL[47]))));
			inputSampleL -= (fireAmp->bL[48] * (0.41067765934041811 - (0.10390115786636855 * fabs(fireAmp->bL[48]))));
			inputSampleL -= (fireAmp->bL[49] * (0.34409235547165967 - (0.07268159377411920 * fabs(fireAmp->bL[49]))));
			inputSampleL -= (fireAmp->bL[50] * (0.26542883122568151 - (0.05483457497365785 * fabs(fireAmp->bL[50]))));
			inputSampleL -= (fireAmp->bL[51] * (0.22024754776138800 - (0.06484897950087598 * fabs(fireAmp->bL[51]))));
			inputSampleL -= (fireAmp->bL[52] * (0.20394367993632415 - (0.08746309731952180 * fabs(fireAmp->bL[52]))));
			inputSampleL -= (fireAmp->bL[53] * (0.17565242431124092 - (0.07611309538078760 * fabs(fireAmp->bL[53]))));
			inputSampleL -= (fireAmp->bL[54] * (0.10116623231246825 - (0.00642818706295112 * fabs(fireAmp->bL[54]))));
			inputSampleL -= (fireAmp->bL[55] * (0.00782648272053632 + (0.08004141267685004 * fabs(fireAmp->bL[55]))));
			inputSampleL += (fireAmp->bL[56] * (0.05059046006747323 - (0.12436676387548490 * fabs(fireAmp->bL[56]))));
			inputSampleL += (fireAmp->bL[57] * (0.06241531553254467 - (0.11530779547021434 * fabs(fireAmp->bL[57]))));
			inputSampleL += (fireAmp->bL[58] * (0.04952694587101836 - (0.08340945324333944 * fabs(fireAmp->bL[58]))));
			inputSampleL += (fireAmp->bL[59] * (0.00843873294401687 - (0.03279659052562903 * fabs(fireAmp->bL[59]))));
			inputSampleL -= (fireAmp->bL[60] * (0.05161338949440241 - (0.03428181149163798 * fabs(fireAmp->bL[60]))));
			inputSampleL -= (fireAmp->bL[61] * (0.08165520146902012 - (0.08196746092283110 * fabs(fireAmp->bL[61]))));
			inputSampleL -= (fireAmp->bL[62] * (0.06639532849935320 - (0.09797462781896329 * fabs(fireAmp->bL[62]))));
			inputSampleL -= (fireAmp->bL[63] * (0.02953430910661621 - (0.09175612938515763 * fabs(fireAmp->bL[63]))));
			inputSampleL += (fireAmp->bL[64] * (0.00741058547442938 + (0.05442091048731967 * fabs(fireAmp->bL[64]))));
			inputSampleL += (fireAmp->bL[65] * (0.01832866125391727 + (0.00306243693643687 * fabs(fireAmp->bL[65]))));
			inputSampleL += (fireAmp->bL[66] * (0.00526964230373573 - (0.04364102661136410 * fabs(fireAmp->bL[66]))));
			inputSampleL -= (fireAmp->bL[67] * (0.00300984373848200 + (0.09742737841278880 * fabs(fireAmp->bL[67]))));
			inputSampleL -= (fireAmp->bL[68] * (0.00413616769576694 + (0.14380661694523073 * fabs(fireAmp->bL[68]))));
			inputSampleL -= (fireAmp->bL[69] * (0.00588769034931419 + (0.16012843578892538 * fabs(fireAmp->bL[69]))));
			inputSampleL -= (fireAmp->bL[70] * (0.00688588239450581 + (0.14074464279305798 * fabs(fireAmp->bL[70]))));
			inputSampleL -= (fireAmp->bL[71] * (0.02277307992926315 + (0.07914752191801366 * fabs(fireAmp->bL[71]))));
			inputSampleL -= (fireAmp->bL[72] * (0.04627166091180877 - (0.00192787268067208 * fabs(fireAmp->bL[72]))));
			inputSampleL -= (fireAmp->bL[73] * (0.05562045897455786 - (0.05932868727665747 * fabs(fireAmp->bL[73]))));
			inputSampleL -= (fireAmp->bL[74] * (0.05134243784922165 - (0.08245334798868090 * fabs(fireAmp->bL[74]))));
			inputSampleL -= (fireAmp->bL[75] * (0.04719409472239919 - (0.07498680629253825 * fabs(fireAmp->bL[75]))));
			inputSampleL -= (fireAmp->bL[76] * (0.05889738914266415 - (0.06116127018043697 * fabs(fireAmp->bL[76]))));
			inputSampleL -= (fireAmp->bL[77] * (0.09428363535111127 - (0.06535868867863834 * fabs(fireAmp->bL[77]))));
			inputSampleL -= (fireAmp->bL[78] * (0.15181756953225126 - (0.08982979655234427 * fabs(fireAmp->bL[78]))));
			inputSampleL -= (fireAmp->bL[79] * (0.20878969456036670 - (0.10761070891499538 * fabs(fireAmp->bL[79]))));
			inputSampleL -= (fireAmp->bL[80] * (0.22647885581813790 - (0.08462542510349125 * fabs(fireAmp->bL[80]))));
			inputSampleL -= (fireAmp->bL[81] * (0.19723482443646323 - (0.02665160920736287 * fabs(fireAmp->bL[81]))));
			inputSampleL -= (fireAmp->bL[82] * (0.16441643451155163 + (0.02314691954338197 * fabs(fireAmp->bL[82]))));
			inputSampleL -= (fireAmp->bL[83] * (0.15201914054931515 + (0.04424903493886839 * fabs(fireAmp->bL[83]))));
			inputSampleL -= (fireAmp->bL[84] * (0.15454370641307855 + (0.04223203797913008 * fabs(fireAmp->bL[84]))));

			temp = (inputSampleL + fireAmp->smoothCabBL) / 3.0;
			fireAmp->smoothCabBL = inputSampleL;
			inputSampleL = temp / 4.0;

			randy = (((double) fireAmp->fpdL / UINT32_MAX) * 0.057);
			drySampleL = ((((inputSampleL * (1 - randy)) + (fireAmp->lastCabSampleL * randy)) * wet) + (drySampleL * (1.0 - wet))) * outputlevel;
			fireAmp->lastCabSampleL = inputSampleL;
			inputSampleL = drySampleL; // cab L

			temp = (inputSampleR + fireAmp->smoothCabAR) / 3.0;
			fireAmp->smoothCabAR = inputSampleR;
			inputSampleR = temp;

			fireAmp->bR[84] = fireAmp->bR[83];
			fireAmp->bR[83] = fireAmp->bR[82];
			fireAmp->bR[82] = fireAmp->bR[81];
			fireAmp->bR[81] = fireAmp->bR[80];
			fireAmp->bR[80] = fireAmp->bR[79];
			fireAmp->bR[79] = fireAmp->bR[78];
			fireAmp->bR[78] = fireAmp->bR[77];
			fireAmp->bR[77] = fireAmp->bR[76];
			fireAmp->bR[76] = fireAmp->bR[75];
			fireAmp->bR[75] = fireAmp->bR[74];
			fireAmp->bR[74] = fireAmp->bR[73];
			fireAmp->bR[73] = fireAmp->bR[72];
			fireAmp->bR[72] = fireAmp->bR[71];
			fireAmp->bR[71] = fireAmp->bR[70];
			fireAmp->bR[70] = fireAmp->bR[69];
			fireAmp->bR[69] = fireAmp->bR[68];
			fireAmp->bR[68] = fireAmp->bR[67];
			fireAmp->bR[67] = fireAmp->bR[66];
			fireAmp->bR[66] = fireAmp->bR[65];
			fireAmp->bR[65] = fireAmp->bR[64];
			fireAmp->bR[64] = fireAmp->bR[63];
			fireAmp->bR[63] = fireAmp->bR[62];
			fireAmp->bR[62] = fireAmp->bR[61];
			fireAmp->bR[61] = fireAmp->bR[60];
			fireAmp->bR[60] = fireAmp->bR[59];
			fireAmp->bR[59] = fireAmp->bR[58];
			fireAmp->bR[58] = fireAmp->bR[57];
			fireAmp->bR[57] = fireAmp->bR[56];
			fireAmp->bR[56] = fireAmp->bR[55];
			fireAmp->bR[55] = fireAmp->bR[54];
			fireAmp->bR[54] = fireAmp->bR[53];
			fireAmp->bR[53] = fireAmp->bR[52];
			fireAmp->bR[52] = fireAmp->bR[51];
			fireAmp->bR[51] = fireAmp->bR[50];
			fireAmp->bR[50] = fireAmp->bR[49];
			fireAmp->bR[49] = fireAmp->bR[48];
			fireAmp->bR[48] = fireAmp->bR[47];
			fireAmp->bR[47] = fireAmp->bR[46];
			fireAmp->bR[46] = fireAmp->bR[45];
			fireAmp->bR[45] = fireAmp->bR[44];
			fireAmp->bR[44] = fireAmp->bR[43];
			fireAmp->bR[43] = fireAmp->bR[42];
			fireAmp->bR[42] = fireAmp->bR[41];
			fireAmp->bR[41] = fireAmp->bR[40];
			fireAmp->bR[40] = fireAmp->bR[39];
			fireAmp->bR[39] = fireAmp->bR[38];
			fireAmp->bR[38] = fireAmp->bR[37];
			fireAmp->bR[37] = fireAmp->bR[36];
			fireAmp->bR[36] = fireAmp->bR[35];
			fireAmp->bR[35] = fireAmp->bR[34];
			fireAmp->bR[34] = fireAmp->bR[33];
			fireAmp->bR[33] = fireAmp->bR[32];
			fireAmp->bR[32] = fireAmp->bR[31];
			fireAmp->bR[31] = fireAmp->bR[30];
			fireAmp->bR[30] = fireAmp->bR[29];
			fireAmp->bR[29] = fireAmp->bR[28];
			fireAmp->bR[28] = fireAmp->bR[27];
			fireAmp->bR[27] = fireAmp->bR[26];
			fireAmp->bR[26] = fireAmp->bR[25];
			fireAmp->bR[25] = fireAmp->bR[24];
			fireAmp->bR[24] = fireAmp->bR[23];
			fireAmp->bR[23] = fireAmp->bR[22];
			fireAmp->bR[22] = fireAmp->bR[21];
			fireAmp->bR[21] = fireAmp->bR[20];
			fireAmp->bR[20] = fireAmp->bR[19];
			fireAmp->bR[19] = fireAmp->bR[18];
			fireAmp->bR[18] = fireAmp->bR[17];
			fireAmp->bR[17] = fireAmp->bR[16];
			fireAmp->bR[16] = fireAmp->bR[15];
			fireAmp->bR[15] = fireAmp->bR[14];
			fireAmp->bR[14] = fireAmp->bR[13];
			fireAmp->bR[13] = fireAmp->bR[12];
			fireAmp->bR[12] = fireAmp->bR[11];
			fireAmp->bR[11] = fireAmp->bR[10];
			fireAmp->bR[10] = fireAmp->bR[9];
			fireAmp->bR[9] = fireAmp->bR[8];
			fireAmp->bR[8] = fireAmp->bR[7];
			fireAmp->bR[7] = fireAmp->bR[6];
			fireAmp->bR[6] = fireAmp->bR[5];
			fireAmp->bR[5] = fireAmp->bR[4];
			fireAmp->bR[4] = fireAmp->bR[3];
			fireAmp->bR[3] = fireAmp->bR[2];
			fireAmp->bR[2] = fireAmp->bR[1];
			fireAmp->bR[1] = fireAmp->bR[0];
			fireAmp->bR[0] = inputSampleR;
			inputSampleR += (fireAmp->bR[1] * (1.31698250313308396 - (0.08140616497621633 * fabs(fireAmp->bR[1]))));
			inputSampleR += (fireAmp->bR[2] * (1.47229016949915326 - (0.27680278993637253 * fabs(fireAmp->bR[2]))));
			inputSampleR += (fireAmp->bR[3] * (1.30410109086044956 - (0.35629113432046489 * fabs(fireAmp->bR[3]))));
			inputSampleR += (fireAmp->bR[4] * (0.81766210474551260 - (0.26808782337659753 * fabs(fireAmp->bR[4]))));
			inputSampleR += (fireAmp->bR[5] * (0.19868872545506663 - (0.11105517193919669 * fabs(fireAmp->bR[5]))));
			inputSampleR -= (fireAmp->bR[6] * (0.39115909132567039 - (0.12630622002682679 * fabs(fireAmp->bR[6]))));
			inputSampleR -= (fireAmp->bR[7] * (0.76881891559343574 - (0.40879849500403143 * fabs(fireAmp->bR[7]))));
			inputSampleR -= (fireAmp->bR[8] * (0.87146861782680340 - (0.59529560488000599 * fabs(fireAmp->bR[8]))));
			inputSampleR -= (fireAmp->bR[9] * (0.79504575932563670 - (0.60877047551611796 * fabs(fireAmp->bR[9]))));
			inputSampleR -= (fireAmp->bR[10] * (0.61653017622406314 - (0.47662851438557335 * fabs(fireAmp->bR[10]))));
			inputSampleR -= (fireAmp->bR[11] * (0.40718195794382067 - (0.24955839378539713 * fabs(fireAmp->bR[11]))));
			inputSampleR -= (fireAmp->bR[12] * (0.31794900040616203 - (0.04169792259600613 * fabs(fireAmp->bR[12]))));
			inputSampleR -= (fireAmp->bR[13] * (0.41075032540217843 + (0.00368483996076280 * fabs(fireAmp->bR[13]))));
			inputSampleR -= (fireAmp->bR[14] * (0.56901352922170667 - (0.11027360805893105 * fabs(fireAmp->bR[14]))));
			inputSampleR -= (fireAmp->bR[15] * (0.62443222391889264 - (0.22198075154245228 * fabs(fireAmp->bR[15]))));
			inputSampleR -= (fireAmp->bR[16] * (0.53462856723129204 - (0.22933544545324852 * fabs(fireAmp->bR[16]))));
			inputSampleR -= (fireAmp->bR[17] * (0.34441703361995046 - (0.12956809502269492 * fabs(fireAmp->bR[17]))));
			inputSampleR -= (fireAmp->bR[18] * (0.13947052337867882 + (0.00339775055962799 * fabs(fireAmp->bR[18]))));
			inputSampleR += (fireAmp->bR[19] * (0.03771252648928484 - (0.10863931549251718 * fabs(fireAmp->bR[19]))));
			inputSampleR += (fireAmp->bR[20] * (0.18280210770271693 - (0.17413646599296417 * fabs(fireAmp->bR[20]))));
			inputSampleR += (fireAmp->bR[21] * (0.24621986701761467 - (0.14547053270435095 * fabs(fireAmp->bR[21]))));
			inputSampleR += (fireAmp->bR[22] * (0.22347075142737360 - (0.02493869490104031 * fabs(fireAmp->bR[22]))));
			inputSampleR += (fireAmp->bR[23] * (0.14346348482123716 + (0.11284054747963246 * fabs(fireAmp->bR[23]))));
			inputSampleR += (fireAmp->bR[24] * (0.00834364862916028 + (0.24284684053733926 * fabs(fireAmp->bR[24]))));
			inputSampleR -= (fireAmp->bR[25] * (0.11559740296078347 - (0.32623054435304538 * fabs(fireAmp->bR[25]))));
			inputSampleR -= (fireAmp->bR[26] * (0.18067604561283060 - (0.32311481551122478 * fabs(fireAmp->bR[26]))));
			inputSampleR -= (fireAmp->bR[27] * (0.22927997789035612 - (0.26991539052832925 * fabs(fireAmp->bR[27]))));
			inputSampleR -= (fireAmp->bR[28] * (0.28487666578669446 - (0.22437227250279349 * fabs(fireAmp->bR[28]))));
			inputSampleR -= (fireAmp->bR[29] * (0.31992973037153838 - (0.15289876100963865 * fabs(fireAmp->bR[29]))));
			inputSampleR -= (fireAmp->bR[30] * (0.35174606303520733 - (0.05656293023086628 * fabs(fireAmp->bR[30]))));
			inputSampleR -= (fireAmp->bR[31] * (0.36894898011375254 + (0.04333925421463558 * fabs(fireAmp->bR[31]))));
			inputSampleR -= (fireAmp->bR[32] * (0.32567576055307507 + (0.14594589410921388 * fabs(fireAmp->bR[32]))));
			inputSampleR -= (fireAmp->bR[33] * (0.27440135050585784 + (0.15529667398122521 * fabs(fireAmp->bR[33]))));
			inputSampleR -= (fireAmp->bR[34] * (0.21998973785078091 + (0.05083553737157104 * fabs(fireAmp->bR[34]))));
			inputSampleR -= (fireAmp->bR[35] * (0.10323624876862457 - (0.04651829594199963 * fabs(fireAmp->bR[35]))));
			inputSampleR += (fireAmp->bR[36] * (0.02091603687851074 + (0.12000046818439322 * fabs(fireAmp->bR[36]))));
			inputSampleR += (fireAmp->bR[37] * (0.11344930914138468 + (0.17697142512225839 * fabs(fireAmp->bR[37]))));
			inputSampleR += (fireAmp->bR[38] * (0.22766779627643968 + (0.13645102964003858 * fabs(fireAmp->bR[38]))));
			inputSampleR += (fireAmp->bR[39] * (0.38378309953638229 - (0.01997653307333791 * fabs(fireAmp->bR[39]))));
			inputSampleR += (fireAmp->bR[40] * (0.52789400804568076 - (0.21409137428422448 * fabs(fireAmp->bR[40]))));
			inputSampleR += (fireAmp->bR[41] * (0.55444630296938280 - (0.32331980931576626 * fabs(fireAmp->bR[41]))));
			inputSampleR += (fireAmp->bR[42] * (0.42333237669264601 - (0.26855847463044280 * fabs(fireAmp->bR[42]))));
			inputSampleR += (fireAmp->bR[43] * (0.21942831522035078 - (0.12051365248820624 * fabs(fireAmp->bR[43]))));
			inputSampleR -= (fireAmp->bR[44] * (0.00584169427830633 - (0.03706970171280329 * fabs(fireAmp->bR[44]))));
			inputSampleR -= (fireAmp->bR[45] * (0.24279799124660351 - (0.17296440491477982 * fabs(fireAmp->bR[45]))));
			inputSampleR -= (fireAmp->bR[46] * (0.40173760787507085 - (0.21717989835163351 * fabs(fireAmp->bR[46]))));
			inputSampleR -= (fireAmp->bR[47] * (0.43930035724188155 - (0.16425928481378199 * fabs(fireAmp->bR[47]))));
			inputSampleR -= (fireAmp->bR[48] * (0.41067765934041811 - (0.10390115786636855 * fabs(fireAmp->bR[48]))));
			inputSampleR -= (fireAmp->bR[49] * (0.34409235547165967 - (0.07268159377411920 * fabs(fireAmp->bR[49]))));
			inputSampleR -= (fireAmp->bR[50] * (0.26542883122568151 - (0.05483457497365785 * fabs(fireAmp->bR[50]))));
			inputSampleR -= (fireAmp->bR[51] * (0.22024754776138800 - (0.06484897950087598 * fabs(fireAmp->bR[51]))));
			inputSampleR -= (fireAmp->bR[52] * (0.20394367993632415 - (0.08746309731952180 * fabs(fireAmp->bR[52]))));
			inputSampleR -= (fireAmp->bR[53] * (0.17565242431124092 - (0.07611309538078760 * fabs(fireAmp->bR[53]))));
			inputSampleR -= (fireAmp->bR[54] * (0.10116623231246825 - (0.00642818706295112 * fabs(fireAmp->bR[54]))));
			inputSampleR -= (fireAmp->bR[55] * (0.00782648272053632 + (0.08004141267685004 * fabs(fireAmp->bR[55]))));
			inputSampleR += (fireAmp->bR[56] * (0.05059046006747323 - (0.12436676387548490 * fabs(fireAmp->bR[56]))));
			inputSampleR += (fireAmp->bR[57] * (0.06241531553254467 - (0.11530779547021434 * fabs(fireAmp->bR[57]))));
			inputSampleR += (fireAmp->bR[58] * (0.04952694587101836 - (0.08340945324333944 * fabs(fireAmp->bR[58]))));
			inputSampleR += (fireAmp->bR[59] * (0.00843873294401687 - (0.03279659052562903 * fabs(fireAmp->bR[59]))));
			inputSampleR -= (fireAmp->bR[60] * (0.05161338949440241 - (0.03428181149163798 * fabs(fireAmp->bR[60]))));
			inputSampleR -= (fireAmp->bR[61] * (0.08165520146902012 - (0.08196746092283110 * fabs(fireAmp->bR[61]))));
			inputSampleR -= (fireAmp->bR[62] * (0.06639532849935320 - (0.09797462781896329 * fabs(fireAmp->bR[62]))));
			inputSampleR -= (fireAmp->bR[63] * (0.02953430910661621 - (0.09175612938515763 * fabs(fireAmp->bR[63]))));
			inputSampleR += (fireAmp->bR[64] * (0.00741058547442938 + (0.05442091048731967 * fabs(fireAmp->bR[64]))));
			inputSampleR += (fireAmp->bR[65] * (0.01832866125391727 + (0.00306243693643687 * fabs(fireAmp->bR[65]))));
			inputSampleR += (fireAmp->bR[66] * (0.00526964230373573 - (0.04364102661136410 * fabs(fireAmp->bR[66]))));
			inputSampleR -= (fireAmp->bR[67] * (0.00300984373848200 + (0.09742737841278880 * fabs(fireAmp->bR[67]))));
			inputSampleR -= (fireAmp->bR[68] * (0.00413616769576694 + (0.14380661694523073 * fabs(fireAmp->bR[68]))));
			inputSampleR -= (fireAmp->bR[69] * (0.00588769034931419 + (0.16012843578892538 * fabs(fireAmp->bR[69]))));
			inputSampleR -= (fireAmp->bR[70] * (0.00688588239450581 + (0.14074464279305798 * fabs(fireAmp->bR[70]))));
			inputSampleR -= (fireAmp->bR[71] * (0.02277307992926315 + (0.07914752191801366 * fabs(fireAmp->bR[71]))));
			inputSampleR -= (fireAmp->bR[72] * (0.04627166091180877 - (0.00192787268067208 * fabs(fireAmp->bR[72]))));
			inputSampleR -= (fireAmp->bR[73] * (0.05562045897455786 - (0.05932868727665747 * fabs(fireAmp->bR[73]))));
			inputSampleR -= (fireAmp->bR[74] * (0.05134243784922165 - (0.08245334798868090 * fabs(fireAmp->bR[74]))));
			inputSampleR -= (fireAmp->bR[75] * (0.04719409472239919 - (0.07498680629253825 * fabs(fireAmp->bR[75]))));
			inputSampleR -= (fireAmp->bR[76] * (0.05889738914266415 - (0.06116127018043697 * fabs(fireAmp->bR[76]))));
			inputSampleR -= (fireAmp->bR[77] * (0.09428363535111127 - (0.06535868867863834 * fabs(fireAmp->bR[77]))));
			inputSampleR -= (fireAmp->bR[78] * (0.15181756953225126 - (0.08982979655234427 * fabs(fireAmp->bR[78]))));
			inputSampleR -= (fireAmp->bR[79] * (0.20878969456036670 - (0.10761070891499538 * fabs(fireAmp->bR[79]))));
			inputSampleR -= (fireAmp->bR[80] * (0.22647885581813790 - (0.08462542510349125 * fabs(fireAmp->bR[80]))));
			inputSampleR -= (fireAmp->bR[81] * (0.19723482443646323 - (0.02665160920736287 * fabs(fireAmp->bR[81]))));
			inputSampleR -= (fireAmp->bR[82] * (0.16441643451155163 + (0.02314691954338197 * fabs(fireAmp->bR[82]))));
			inputSampleR -= (fireAmp->bR[83] * (0.15201914054931515 + (0.04424903493886839 * fabs(fireAmp->bR[83]))));
			inputSampleR -= (fireAmp->bR[84] * (0.15454370641307855 + (0.04223203797913008 * fabs(fireAmp->bR[84]))));

			temp = (inputSampleR + fireAmp->smoothCabBR) / 3.0;
			fireAmp->smoothCabBR = inputSampleR;
			inputSampleR = temp / 4.0;

			randy = (((double) fireAmp->fpdR / UINT32_MAX) * 0.057);
			drySampleR = ((((inputSampleR * (1.0 - randy)) + (fireAmp->lastCabSampleR * randy)) * wet) + (drySampleR * (1.0 - wet))) * outputlevel;
			fireAmp->lastCabSampleR = inputSampleR;
			inputSampleR = drySampleR; // cab

			if (cycleEnd == 4) {
				fireAmp->lastRefL[0] = fireAmp->lastRefL[4]; // start from previous last
				fireAmp->lastRefL[2] = (fireAmp->lastRefL[0] + inputSampleL) / 2; // half
				fireAmp->lastRefL[1] = (fireAmp->lastRefL[0] + fireAmp->lastRefL[2]) / 2; // one quarter
				fireAmp->lastRefL[3] = (fireAmp->lastRefL[2] + inputSampleL) / 2; // three quarters
				fireAmp->lastRefL[4] = inputSampleL; // full
				fireAmp->lastRefR[0] = fireAmp->lastRefR[4]; // start from previous last
				fireAmp->lastRefR[2] = (fireAmp->lastRefR[0] + inputSampleR) / 2; // half
				fireAmp->lastRefR[1] = (fireAmp->lastRefR[0] + fireAmp->lastRefR[2]) / 2; // one quarter
				fireAmp->lastRefR[3] = (fireAmp->lastRefR[2] + inputSampleR) / 2; // three quarters
				fireAmp->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				fireAmp->lastRefL[0] = fireAmp->lastRefL[3]; // start from previous last
				fireAmp->lastRefL[2] = (fireAmp->lastRefL[0] + fireAmp->lastRefL[0] + inputSampleL) / 3; // third
				fireAmp->lastRefL[1] = (fireAmp->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				fireAmp->lastRefL[3] = inputSampleL; // full
				fireAmp->lastRefR[0] = fireAmp->lastRefR[3]; // start from previous last
				fireAmp->lastRefR[2] = (fireAmp->lastRefR[0] + fireAmp->lastRefR[0] + inputSampleR) / 3; // third
				fireAmp->lastRefR[1] = (fireAmp->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				fireAmp->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				fireAmp->lastRefL[0] = fireAmp->lastRefL[2]; // start from previous last
				fireAmp->lastRefL[1] = (fireAmp->lastRefL[0] + inputSampleL) / 2; // half
				fireAmp->lastRefL[2] = inputSampleL; // full
				fireAmp->lastRefR[0] = fireAmp->lastRefR[2]; // start from previous last
				fireAmp->lastRefR[1] = (fireAmp->lastRefR[0] + inputSampleR) / 2; // half
				fireAmp->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				fireAmp->lastRefL[0] = inputSampleL;
				fireAmp->lastRefR[0] = inputSampleR;
			}
			fireAmp->cycle = 0; // reset
			inputSampleL = fireAmp->lastRefL[fireAmp->cycle];
			inputSampleR = fireAmp->lastRefR[fireAmp->cycle];
		} else {
			inputSampleL = fireAmp->lastRefL[fireAmp->cycle];
			inputSampleR = fireAmp->lastRefR[fireAmp->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				fireAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + fireAmp->lastRefL[7]) * 0.5;
				fireAmp->lastRefL[7] = fireAmp->lastRefL[8]; // continue, do not break
				fireAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + fireAmp->lastRefR[7]) * 0.5;
				fireAmp->lastRefR[7] = fireAmp->lastRefR[8]; // continue, do not break
			case 3:
				fireAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + fireAmp->lastRefL[6]) * 0.5;
				fireAmp->lastRefL[6] = fireAmp->lastRefL[8]; // continue, do not break
				fireAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + fireAmp->lastRefR[6]) * 0.5;
				fireAmp->lastRefR[6] = fireAmp->lastRefR[8]; // continue, do not break
			case 2:
				fireAmp->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + fireAmp->lastRefL[5]) * 0.5;
				fireAmp->lastRefL[5] = fireAmp->lastRefL[8]; // continue, do not break
				fireAmp->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + fireAmp->lastRefR[5]) * 0.5;
				fireAmp->lastRefR[5] = fireAmp->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		fireAmp->fpdL ^= fireAmp->fpdL << 13;
		fireAmp->fpdL ^= fireAmp->fpdL >> 17;
		fireAmp->fpdL ^= fireAmp->fpdL << 5;
		inputSampleL += (((double) fireAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		fireAmp->fpdR ^= fireAmp->fpdR << 13;
		fireAmp->fpdR ^= fireAmp->fpdR >> 17;
		fireAmp->fpdR ^= fireAmp->fpdR << 5;
		inputSampleR += (((double) fireAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	FIREAMP_URI,
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
