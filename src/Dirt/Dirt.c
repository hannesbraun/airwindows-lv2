#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define DIRT_URI "https://hannesbraun.net/ns/lv2/airwindows/dirt"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	LOWPASS = 5,
	HIGHPASS = 6,
	OUTPUT = 7,
	DRY_WET = 8
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
	const float* lowpass;
	const float* highpass;
	const float* outputGain;
	const float* dryWet;

	double fixA[fix_total];
	double fixB[fix_total];
	double fixC[fix_total];
	double fixD[fix_total];
	double fixE[fix_total];
	double fixF[fix_total];
	double fixG[fix_total];
	double iirSampleL;
	double iirSampleR;

	uint32_t fpdL;
	uint32_t fpdR;

} Dirt;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Dirt* dirt = (Dirt*) calloc(1, sizeof(Dirt));
	dirt->sampleRate = rate;
	return (LV2_Handle) dirt;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Dirt* dirt = (Dirt*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			dirt->input[0] = (const float*) data;
			break;
		case INPUT_R:
			dirt->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			dirt->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			dirt->output[1] = (float*) data;
			break;
		case GAIN:
			dirt->gain = (const float*) data;
			break;
		case LOWPASS:
			dirt->lowpass = (const float*) data;
			break;
		case HIGHPASS:
			dirt->highpass = (const float*) data;
			break;
		case OUTPUT:
			dirt->outputGain = (const float*) data;
			break;
		case DRY_WET:
			dirt->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Dirt* dirt = (Dirt*) instance;
	for (int x = 0; x < fix_total; x++) {
		dirt->fixA[x] = 0.0;
		dirt->fixB[x] = 0.0;
		dirt->fixC[x] = 0.0;
		dirt->fixD[x] = 0.0;
		dirt->fixE[x] = 0.0;
		dirt->fixF[x] = 0.0;
		dirt->fixG[x] = 0.0;
	}
	dirt->iirSampleL = 0.0;
	dirt->iirSampleR = 0.0;

	dirt->fpdL = 1.0;
	while (dirt->fpdL < 16386) dirt->fpdL = rand() * UINT32_MAX;
	dirt->fpdR = 1.0;
	while (dirt->fpdR < 16386) dirt->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Dirt* dirt = (Dirt*) instance;

	const float* in1 = dirt->input[0];
	const float* in2 = dirt->input[1];
	float* out1 = dirt->output[0];
	float* out2 = dirt->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= dirt->sampleRate;

	double inTrim = ((1.0 - pow(1.0 - *dirt->gain, 2)) * 1.0) + 1.0;
	double cutoff = (*dirt->lowpass * 25000.0) / dirt->sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; // or if cutoff's too low
	double iirAmount = pow(*dirt->highpass, 3) * 0.5;
	if (iirAmount < 0.00000001) iirAmount = 0.00000001; // or if cutoff's too low
	iirAmount /= overallscale; // highpass is very gentle
	double outPad = *dirt->outputGain;
	double wet = *dirt->dryWet;

	dirt->fixG[fix_freq] = dirt->fixF[fix_freq] = dirt->fixE[fix_freq] = dirt->fixD[fix_freq] = dirt->fixC[fix_freq] = dirt->fixB[fix_freq] = dirt->fixA[fix_freq] = cutoff;

	dirt->fixA[fix_reso] = 4.46570214;
	dirt->fixB[fix_reso] = 1.51387132;
	dirt->fixC[fix_reso] = 0.93979296;
	dirt->fixD[fix_reso] = 0.70710678;
	dirt->fixE[fix_reso] = 0.59051105;
	dirt->fixF[fix_reso] = 0.52972649;
	dirt->fixG[fix_reso] = 0.50316379;

	double K = tan(M_PI * dirt->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / dirt->fixA[fix_reso] + K * K);
	dirt->fixA[fix_a0] = K * K * norm;
	dirt->fixA[fix_a1] = 2.0 * dirt->fixA[fix_a0];
	dirt->fixA[fix_a2] = dirt->fixA[fix_a0];
	dirt->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixA[fix_b2] = (1.0 - K / dirt->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * dirt->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / dirt->fixB[fix_reso] + K * K);
	dirt->fixB[fix_a0] = K * K * norm;
	dirt->fixB[fix_a1] = 2.0 * dirt->fixB[fix_a0];
	dirt->fixB[fix_a2] = dirt->fixB[fix_a0];
	dirt->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixB[fix_b2] = (1.0 - K / dirt->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * dirt->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / dirt->fixC[fix_reso] + K * K);
	dirt->fixC[fix_a0] = K * K * norm;
	dirt->fixC[fix_a1] = 2.0 * dirt->fixC[fix_a0];
	dirt->fixC[fix_a2] = dirt->fixC[fix_a0];
	dirt->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixC[fix_b2] = (1.0 - K / dirt->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * dirt->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / dirt->fixD[fix_reso] + K * K);
	dirt->fixD[fix_a0] = K * K * norm;
	dirt->fixD[fix_a1] = 2.0 * dirt->fixD[fix_a0];
	dirt->fixD[fix_a2] = dirt->fixD[fix_a0];
	dirt->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixD[fix_b2] = (1.0 - K / dirt->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * dirt->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / dirt->fixE[fix_reso] + K * K);
	dirt->fixE[fix_a0] = K * K * norm;
	dirt->fixE[fix_a1] = 2.0 * dirt->fixE[fix_a0];
	dirt->fixE[fix_a2] = dirt->fixE[fix_a0];
	dirt->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixE[fix_b2] = (1.0 - K / dirt->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * dirt->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / dirt->fixF[fix_reso] + K * K);
	dirt->fixF[fix_a0] = K * K * norm;
	dirt->fixF[fix_a1] = 2.0 * dirt->fixF[fix_a0];
	dirt->fixF[fix_a2] = dirt->fixF[fix_a0];
	dirt->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixF[fix_b2] = (1.0 - K / dirt->fixF[fix_reso] + K * K) * norm;

	K = tan(M_PI * dirt->fixG[fix_freq]);
	norm = 1.0 / (1.0 + K / dirt->fixG[fix_reso] + K * K);
	dirt->fixG[fix_a0] = K * K * norm;
	dirt->fixG[fix_a1] = 2.0 * dirt->fixG[fix_a0];
	dirt->fixG[fix_a2] = dirt->fixG[fix_a0];
	dirt->fixG[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dirt->fixG[fix_b2] = (1.0 - K / dirt->fixG[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = dirt->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = dirt->fpdR * 1.18e-17;

		if (fabs(dirt->iirSampleL) < 1.18e-33) dirt->iirSampleL = 0.0;
		dirt->iirSampleL = (dirt->iirSampleL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
		inputSampleL = inputSampleL - dirt->iirSampleL;

		if (fabs(dirt->iirSampleR) < 1.18e-33) dirt->iirSampleR = 0.0;
		dirt->iirSampleR = (dirt->iirSampleR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
		inputSampleR = inputSampleR - dirt->iirSampleR;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;

		double outSample = (inputSampleL * dirt->fixA[fix_a0]) + dirt->fixA[fix_sL1];
		dirt->fixA[fix_sL1] = (inputSampleL * dirt->fixA[fix_a1]) - (outSample * dirt->fixA[fix_b1]) + dirt->fixA[fix_sL2];
		dirt->fixA[fix_sL2] = (inputSampleL * dirt->fixA[fix_a2]) - (outSample * dirt->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixA[fix_a0]) + dirt->fixA[fix_sR1];
		dirt->fixA[fix_sR1] = (inputSampleR * dirt->fixA[fix_a1]) - (outSample * dirt->fixA[fix_b1]) + dirt->fixA[fix_sR2];
		dirt->fixA[fix_sR2] = (inputSampleR * dirt->fixA[fix_a2]) - (outSample * dirt->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
		inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;

		outSample = (inputSampleL * dirt->fixB[fix_a0]) + dirt->fixB[fix_sL1];
		dirt->fixB[fix_sL1] = (inputSampleL * dirt->fixB[fix_a1]) - (outSample * dirt->fixB[fix_b1]) + dirt->fixB[fix_sL2];
		dirt->fixB[fix_sL2] = (inputSampleL * dirt->fixB[fix_a2]) - (outSample * dirt->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixB[fix_a0]) + dirt->fixB[fix_sR1];
		dirt->fixB[fix_sR1] = (inputSampleR * dirt->fixB[fix_a1]) - (outSample * dirt->fixB[fix_b1]) + dirt->fixB[fix_sR2];
		dirt->fixB[fix_sR2] = (inputSampleR * dirt->fixB[fix_a2]) - (outSample * dirt->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
		inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;

		outSample = (inputSampleL * dirt->fixC[fix_a0]) + dirt->fixC[fix_sL1];
		dirt->fixC[fix_sL1] = (inputSampleL * dirt->fixC[fix_a1]) - (outSample * dirt->fixC[fix_b1]) + dirt->fixC[fix_sL2];
		dirt->fixC[fix_sL2] = (inputSampleL * dirt->fixC[fix_a2]) - (outSample * dirt->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixC[fix_a0]) + dirt->fixC[fix_sR1];
		dirt->fixC[fix_sR1] = (inputSampleR * dirt->fixC[fix_a1]) - (outSample * dirt->fixC[fix_b1]) + dirt->fixC[fix_sR2];
		dirt->fixC[fix_sR2] = (inputSampleR * dirt->fixC[fix_a2]) - (outSample * dirt->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
		inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;

		outSample = (inputSampleL * dirt->fixD[fix_a0]) + dirt->fixD[fix_sL1];
		dirt->fixD[fix_sL1] = (inputSampleL * dirt->fixD[fix_a1]) - (outSample * dirt->fixD[fix_b1]) + dirt->fixD[fix_sL2];
		dirt->fixD[fix_sL2] = (inputSampleL * dirt->fixD[fix_a2]) - (outSample * dirt->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixD[fix_a0]) + dirt->fixD[fix_sR1];
		dirt->fixD[fix_sR1] = (inputSampleR * dirt->fixD[fix_a1]) - (outSample * dirt->fixD[fix_b1]) + dirt->fixD[fix_sR2];
		dirt->fixD[fix_sR2] = (inputSampleR * dirt->fixD[fix_a2]) - (outSample * dirt->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
		inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;

		outSample = (inputSampleL * dirt->fixE[fix_a0]) + dirt->fixE[fix_sL1];
		dirt->fixE[fix_sL1] = (inputSampleL * dirt->fixE[fix_a1]) - (outSample * dirt->fixE[fix_b1]) + dirt->fixE[fix_sL2];
		dirt->fixE[fix_sL2] = (inputSampleL * dirt->fixE[fix_a2]) - (outSample * dirt->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixE[fix_a0]) + dirt->fixE[fix_sR1];
		dirt->fixE[fix_sR1] = (inputSampleR * dirt->fixE[fix_a1]) - (outSample * dirt->fixE[fix_b1]) + dirt->fixE[fix_sR2];
		dirt->fixE[fix_sR2] = (inputSampleR * dirt->fixE[fix_a2]) - (outSample * dirt->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
		inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;

		outSample = (inputSampleL * dirt->fixF[fix_a0]) + dirt->fixF[fix_sL1];
		dirt->fixF[fix_sL1] = (inputSampleL * dirt->fixF[fix_a1]) - (outSample * dirt->fixF[fix_b1]) + dirt->fixF[fix_sL2];
		dirt->fixF[fix_sL2] = (inputSampleL * dirt->fixF[fix_a2]) - (outSample * dirt->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixF[fix_a0]) + dirt->fixF[fix_sR1];
		dirt->fixF[fix_sR1] = (inputSampleR * dirt->fixF[fix_a1]) - (outSample * dirt->fixF[fix_b1]) + dirt->fixF[fix_sR2];
		dirt->fixF[fix_sR2] = (inputSampleR * dirt->fixF[fix_a2]) - (outSample * dirt->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
		inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;

		outSample = (inputSampleL * dirt->fixG[fix_a0]) + dirt->fixG[fix_sL1];
		dirt->fixG[fix_sL1] = (inputSampleL * dirt->fixG[fix_a1]) - (outSample * dirt->fixG[fix_b1]) + dirt->fixG[fix_sL2];
		dirt->fixG[fix_sL2] = (inputSampleL * dirt->fixG[fix_a2]) - (outSample * dirt->fixG[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics L
		outSample = (inputSampleR * dirt->fixG[fix_a0]) + dirt->fixG[fix_sR1];
		dirt->fixG[fix_sR1] = (inputSampleR * dirt->fixG[fix_a1]) - (outSample * dirt->fixG[fix_b1]) + dirt->fixG[fix_sR2];
		dirt->fixG[fix_sR2] = (inputSampleR * dirt->fixG[fix_a2]) - (outSample * dirt->fixG[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics R

		inputSampleL = (inputSampleL * wet * outPad) + (drySampleL * (1.0 - wet));
		inputSampleR = (inputSampleR * wet * outPad) + (drySampleR * (1.0 - wet));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		dirt->fpdL ^= dirt->fpdL << 13;
		dirt->fpdL ^= dirt->fpdL >> 17;
		dirt->fpdL ^= dirt->fpdL << 5;
		inputSampleL += (((double) dirt->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		dirt->fpdR ^= dirt->fpdR << 13;
		dirt->fpdR ^= dirt->fpdR >> 17;
		dirt->fpdR ^= dirt->fpdR << 5;
		inputSampleR += (((double) dirt->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	DIRT_URI,
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
