#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define HYPERSONIC_URI "https://hannesbraun.net/ns/lv2/airwindows/hypersonic"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
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

	double fixA[fix_total];
	double fixB[fix_total];
	double fixC[fix_total];
	double fixD[fix_total];
	double fixE[fix_total];
	double fixF[fix_total];
	double fixG[fix_total];

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
} Hypersonic;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Hypersonic* hypersonic = (Hypersonic*) calloc(1, sizeof(Hypersonic));
	hypersonic->sampleRate = rate;
	return (LV2_Handle) hypersonic;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Hypersonic* hypersonic = (Hypersonic*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			hypersonic->input[0] = (const float*) data;
			break;
		case INPUT_R:
			hypersonic->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			hypersonic->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			hypersonic->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Hypersonic* hypersonic = (Hypersonic*) instance;
	for (int x = 0; x < fix_total; x++) {
		hypersonic->fixA[x] = 0.0;
		hypersonic->fixB[x] = 0.0;
		hypersonic->fixC[x] = 0.0;
		hypersonic->fixD[x] = 0.0;
		hypersonic->fixE[x] = 0.0;
		hypersonic->fixF[x] = 0.0;
		hypersonic->fixG[x] = 0.0;
	}
	hypersonic->fpdL = 1.0;
	while (hypersonic->fpdL < 16386) hypersonic->fpdL = rand() * UINT32_MAX;
	hypersonic->fpdR = 1.0;
	while (hypersonic->fpdR < 16386) hypersonic->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Hypersonic* hypersonic = (Hypersonic*) instance;

	const float* in1 = hypersonic->input[0];
	const float* in2 = hypersonic->input[1];
	float* out1 = hypersonic->output[0];
	float* out2 = hypersonic->output[1];

	double cutoff = 25000.0 / hypersonic->sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k

	hypersonic->fixA[fix_freq] = cutoff;
	hypersonic->fixB[fix_freq] = cutoff;
	hypersonic->fixC[fix_freq] = cutoff;
	hypersonic->fixD[fix_freq] = cutoff;
	hypersonic->fixE[fix_freq] = cutoff;
	hypersonic->fixF[fix_freq] = cutoff;
	hypersonic->fixG[fix_freq] = cutoff;

	hypersonic->fixA[fix_reso] = 4.46570214;
	hypersonic->fixB[fix_reso] = 1.51387132;
	hypersonic->fixC[fix_reso] = 0.93979296;
	hypersonic->fixD[fix_reso] = 0.70710678;
	hypersonic->fixE[fix_reso] = 0.59051105;
	hypersonic->fixF[fix_reso] = 0.52972649;
	hypersonic->fixG[fix_reso] = 0.50316379;

	double K = tan(M_PI * hypersonic->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / hypersonic->fixA[fix_reso] + K * K);
	hypersonic->fixA[fix_a0] = K * K * norm;
	hypersonic->fixA[fix_a1] = 2.0 * hypersonic->fixA[fix_a0];
	hypersonic->fixA[fix_a2] = hypersonic->fixA[fix_a0];
	hypersonic->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixA[fix_b2] = (1.0 - K / hypersonic->fixA[fix_reso] + K * K) * norm;

	K = tan(M_PI * hypersonic->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / hypersonic->fixB[fix_reso] + K * K);
	hypersonic->fixB[fix_a0] = K * K * norm;
	hypersonic->fixB[fix_a1] = 2.0 * hypersonic->fixB[fix_a0];
	hypersonic->fixB[fix_a2] = hypersonic->fixB[fix_a0];
	hypersonic->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixB[fix_b2] = (1.0 - K / hypersonic->fixB[fix_reso] + K * K) * norm;

	K = tan(M_PI * hypersonic->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / hypersonic->fixC[fix_reso] + K * K);
	hypersonic->fixC[fix_a0] = K * K * norm;
	hypersonic->fixC[fix_a1] = 2.0 * hypersonic->fixC[fix_a0];
	hypersonic->fixC[fix_a2] = hypersonic->fixC[fix_a0];
	hypersonic->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixC[fix_b2] = (1.0 - K / hypersonic->fixC[fix_reso] + K * K) * norm;

	K = tan(M_PI * hypersonic->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / hypersonic->fixD[fix_reso] + K * K);
	hypersonic->fixD[fix_a0] = K * K * norm;
	hypersonic->fixD[fix_a1] = 2.0 * hypersonic->fixD[fix_a0];
	hypersonic->fixD[fix_a2] = hypersonic->fixD[fix_a0];
	hypersonic->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixD[fix_b2] = (1.0 - K / hypersonic->fixD[fix_reso] + K * K) * norm;

	K = tan(M_PI * hypersonic->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / hypersonic->fixE[fix_reso] + K * K);
	hypersonic->fixE[fix_a0] = K * K * norm;
	hypersonic->fixE[fix_a1] = 2.0 * hypersonic->fixE[fix_a0];
	hypersonic->fixE[fix_a2] = hypersonic->fixE[fix_a0];
	hypersonic->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixE[fix_b2] = (1.0 - K / hypersonic->fixE[fix_reso] + K * K) * norm;

	K = tan(M_PI * hypersonic->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / hypersonic->fixF[fix_reso] + K * K);
	hypersonic->fixF[fix_a0] = K * K * norm;
	hypersonic->fixF[fix_a1] = 2.0 * hypersonic->fixF[fix_a0];
	hypersonic->fixF[fix_a2] = hypersonic->fixF[fix_a0];
	hypersonic->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixF[fix_b2] = (1.0 - K / hypersonic->fixF[fix_reso] + K * K) * norm;

	K = tan(M_PI * hypersonic->fixG[fix_freq]);
	norm = 1.0 / (1.0 + K / hypersonic->fixG[fix_reso] + K * K);
	hypersonic->fixG[fix_a0] = K * K * norm;
	hypersonic->fixG[fix_a1] = 2.0 * hypersonic->fixG[fix_a0];
	hypersonic->fixG[fix_a2] = hypersonic->fixG[fix_a0];
	hypersonic->fixG[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	hypersonic->fixG[fix_b2] = (1.0 - K / hypersonic->fixG[fix_reso] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = hypersonic->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = hypersonic->fpdR * 1.18e-17;

		double outSample = (inputSampleL * hypersonic->fixA[fix_a0]) + hypersonic->fixA[fix_sL1];
		hypersonic->fixA[fix_sL1] = (inputSampleL * hypersonic->fixA[fix_a1]) - (outSample * hypersonic->fixA[fix_b1]) + hypersonic->fixA[fix_sL2];
		hypersonic->fixA[fix_sL2] = (inputSampleL * hypersonic->fixA[fix_a2]) - (outSample * hypersonic->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixA[fix_a0]) + hypersonic->fixA[fix_sR1];
		hypersonic->fixA[fix_sR1] = (inputSampleR * hypersonic->fixA[fix_a1]) - (outSample * hypersonic->fixA[fix_b1]) + hypersonic->fixA[fix_sR2];
		hypersonic->fixA[fix_sR2] = (inputSampleR * hypersonic->fixA[fix_a2]) - (outSample * hypersonic->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		outSample = (inputSampleL * hypersonic->fixB[fix_a0]) + hypersonic->fixB[fix_sL1];
		hypersonic->fixB[fix_sL1] = (inputSampleL * hypersonic->fixB[fix_a1]) - (outSample * hypersonic->fixB[fix_b1]) + hypersonic->fixB[fix_sL2];
		hypersonic->fixB[fix_sL2] = (inputSampleL * hypersonic->fixB[fix_a2]) - (outSample * hypersonic->fixB[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixB[fix_a0]) + hypersonic->fixB[fix_sR1];
		hypersonic->fixB[fix_sR1] = (inputSampleR * hypersonic->fixB[fix_a1]) - (outSample * hypersonic->fixB[fix_b1]) + hypersonic->fixB[fix_sR2];
		hypersonic->fixB[fix_sR2] = (inputSampleR * hypersonic->fixB[fix_a2]) - (outSample * hypersonic->fixB[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		outSample = (inputSampleL * hypersonic->fixC[fix_a0]) + hypersonic->fixC[fix_sL1];
		hypersonic->fixC[fix_sL1] = (inputSampleL * hypersonic->fixC[fix_a1]) - (outSample * hypersonic->fixC[fix_b1]) + hypersonic->fixC[fix_sL2];
		hypersonic->fixC[fix_sL2] = (inputSampleL * hypersonic->fixC[fix_a2]) - (outSample * hypersonic->fixC[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixC[fix_a0]) + hypersonic->fixC[fix_sR1];
		hypersonic->fixC[fix_sR1] = (inputSampleR * hypersonic->fixC[fix_a1]) - (outSample * hypersonic->fixC[fix_b1]) + hypersonic->fixC[fix_sR2];
		hypersonic->fixC[fix_sR2] = (inputSampleR * hypersonic->fixC[fix_a2]) - (outSample * hypersonic->fixC[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		outSample = (inputSampleL * hypersonic->fixD[fix_a0]) + hypersonic->fixD[fix_sL1];
		hypersonic->fixD[fix_sL1] = (inputSampleL * hypersonic->fixD[fix_a1]) - (outSample * hypersonic->fixD[fix_b1]) + hypersonic->fixD[fix_sL2];
		hypersonic->fixD[fix_sL2] = (inputSampleL * hypersonic->fixD[fix_a2]) - (outSample * hypersonic->fixD[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixD[fix_a0]) + hypersonic->fixD[fix_sR1];
		hypersonic->fixD[fix_sR1] = (inputSampleR * hypersonic->fixD[fix_a1]) - (outSample * hypersonic->fixD[fix_b1]) + hypersonic->fixD[fix_sR2];
		hypersonic->fixD[fix_sR2] = (inputSampleR * hypersonic->fixD[fix_a2]) - (outSample * hypersonic->fixD[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		outSample = (inputSampleL * hypersonic->fixE[fix_a0]) + hypersonic->fixE[fix_sL1];
		hypersonic->fixE[fix_sL1] = (inputSampleL * hypersonic->fixE[fix_a1]) - (outSample * hypersonic->fixE[fix_b1]) + hypersonic->fixE[fix_sL2];
		hypersonic->fixE[fix_sL2] = (inputSampleL * hypersonic->fixE[fix_a2]) - (outSample * hypersonic->fixE[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixE[fix_a0]) + hypersonic->fixE[fix_sR1];
		hypersonic->fixE[fix_sR1] = (inputSampleR * hypersonic->fixE[fix_a1]) - (outSample * hypersonic->fixE[fix_b1]) + hypersonic->fixE[fix_sR2];
		hypersonic->fixE[fix_sR2] = (inputSampleR * hypersonic->fixE[fix_a2]) - (outSample * hypersonic->fixE[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		outSample = (inputSampleL * hypersonic->fixF[fix_a0]) + hypersonic->fixF[fix_sL1];
		hypersonic->fixF[fix_sL1] = (inputSampleL * hypersonic->fixF[fix_a1]) - (outSample * hypersonic->fixF[fix_b1]) + hypersonic->fixF[fix_sL2];
		hypersonic->fixF[fix_sL2] = (inputSampleL * hypersonic->fixF[fix_a2]) - (outSample * hypersonic->fixF[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixF[fix_a0]) + hypersonic->fixF[fix_sR1];
		hypersonic->fixF[fix_sR1] = (inputSampleR * hypersonic->fixF[fix_a1]) - (outSample * hypersonic->fixF[fix_b1]) + hypersonic->fixF[fix_sR2];
		hypersonic->fixF[fix_sR2] = (inputSampleR * hypersonic->fixF[fix_a2]) - (outSample * hypersonic->fixF[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		outSample = (inputSampleL * hypersonic->fixG[fix_a0]) + hypersonic->fixG[fix_sL1];
		hypersonic->fixG[fix_sL1] = (inputSampleL * hypersonic->fixG[fix_a1]) - (outSample * hypersonic->fixG[fix_b1]) + hypersonic->fixG[fix_sL2];
		hypersonic->fixG[fix_sL2] = (inputSampleL * hypersonic->fixG[fix_a2]) - (outSample * hypersonic->fixG[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics
		outSample = (inputSampleR * hypersonic->fixG[fix_a0]) + hypersonic->fixG[fix_sR1];
		hypersonic->fixG[fix_sR1] = (inputSampleR * hypersonic->fixG[fix_a1]) - (outSample * hypersonic->fixG[fix_b1]) + hypersonic->fixG[fix_sR2];
		hypersonic->fixG[fix_sR2] = (inputSampleR * hypersonic->fixG[fix_a2]) - (outSample * hypersonic->fixG[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		hypersonic->fpdL ^= hypersonic->fpdL << 13;
		hypersonic->fpdL ^= hypersonic->fpdL >> 17;
		hypersonic->fpdL ^= hypersonic->fpdL << 5;
		inputSampleL += (((double) hypersonic->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		hypersonic->fpdR ^= hypersonic->fpdR << 13;
		hypersonic->fpdR ^= hypersonic->fpdR >> 17;
		hypersonic->fpdR ^= hypersonic->fpdR << 5;
		inputSampleR += (((double) hypersonic->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	HYPERSONIC_URI,
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
