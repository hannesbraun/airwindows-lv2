#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define PURESTWARM2_URI "https://hannesbraun.net/ns/lv2/airwindows/purestwarm2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRY_POS = 4,
	DRY_NEG = 5
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
	const float* dryPos;
	const float* dryNeg;

	double fixA[fix_total];

	uint32_t fpdL;
	uint32_t fpdR;
} PurestWarm2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestWarm2* purestWarm2 = (PurestWarm2*) calloc(1, sizeof(PurestWarm2));
	purestWarm2->sampleRate = rate;
	return (LV2_Handle) purestWarm2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestWarm2* purestWarm2 = (PurestWarm2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestWarm2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestWarm2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestWarm2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestWarm2->output[1] = (float*) data;
			break;
		case DRY_POS:
			purestWarm2->dryPos = (const float*) data;
			break;
		case DRY_NEG:
			purestWarm2->dryNeg = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestWarm2* purestWarm2 = (PurestWarm2*) instance;

	purestWarm2->fpdL = 1.0;
	while (purestWarm2->fpdL < 16386) purestWarm2->fpdL = rand() * UINT32_MAX;
	purestWarm2->fpdR = 1.0;
	while (purestWarm2->fpdR < 16386) purestWarm2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestWarm2* purestWarm2 = (PurestWarm2*) instance;

	const float* in1 = purestWarm2->input[0];
	const float* in2 = purestWarm2->input[1];
	float* out1 = purestWarm2->output[0];
	float* out2 = purestWarm2->output[1];

	double pos = *purestWarm2->dryPos;
	double neg = *purestWarm2->dryNeg;
	double cutoff = 25000.0 / purestWarm2->sampleRate;
	if (cutoff > 0.49) cutoff = 0.49; // don't crash if run at 44.1k
	purestWarm2->fixA[fix_freq] = cutoff;
	purestWarm2->fixA[fix_reso] = 0.70710678; // butterworth Q
	double K = tan(M_PI * purestWarm2->fixA[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / purestWarm2->fixA[fix_reso] + K * K);
	purestWarm2->fixA[fix_a0] = K * K * norm;
	purestWarm2->fixA[fix_a1] = 2.0 * purestWarm2->fixA[fix_a0];
	purestWarm2->fixA[fix_a2] = purestWarm2->fixA[fix_a0];
	purestWarm2->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	purestWarm2->fixA[fix_b2] = (1.0 - K / purestWarm2->fixA[fix_reso] + K * K) * norm;
	purestWarm2->fixA[fix_sL1] = 0.0;
	purestWarm2->fixA[fix_sL2] = 0.0;
	purestWarm2->fixA[fix_sR1] = 0.0;
	purestWarm2->fixA[fix_sR2] = 0.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestWarm2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestWarm2->fpdR * 1.18e-17;

		double outSample = (inputSampleL * purestWarm2->fixA[fix_a0]) + purestWarm2->fixA[fix_sL1];
		purestWarm2->fixA[fix_sL1] = (inputSampleL * purestWarm2->fixA[fix_a1]) - (outSample * purestWarm2->fixA[fix_b1]) + purestWarm2->fixA[fix_sL2];
		purestWarm2->fixA[fix_sL2] = (inputSampleL * purestWarm2->fixA[fix_a2]) - (outSample * purestWarm2->fixA[fix_b2]);
		inputSampleL = outSample; // fixed biquad filtering ultrasonics

		if (inputSampleL > 0) inputSampleL = (sin(inputSampleL * 1.57079634 * pos) / 1.57079634) + (inputSampleL * (1.0 - pos));
		if (inputSampleL < 0) inputSampleL = (sin(inputSampleL * 1.57079634 * neg) / 1.57079634) + (inputSampleL * (1.0 - neg));

		outSample = (inputSampleR * purestWarm2->fixA[fix_a0]) + purestWarm2->fixA[fix_sR1];
		purestWarm2->fixA[fix_sR1] = (inputSampleR * purestWarm2->fixA[fix_a1]) - (outSample * purestWarm2->fixA[fix_b1]) + purestWarm2->fixA[fix_sR2];
		purestWarm2->fixA[fix_sR2] = (inputSampleR * purestWarm2->fixA[fix_a2]) - (outSample * purestWarm2->fixA[fix_b2]);
		inputSampleR = outSample; // fixed biquad filtering ultrasonics

		if (inputSampleR > 0) inputSampleR = (sin(inputSampleR * 1.57079634 * pos) / 1.57079634) + (inputSampleR * (1.0 - pos));
		if (inputSampleR < 0) inputSampleR = (sin(inputSampleR * 1.57079634 * neg) / 1.57079634) + (inputSampleR * (1.0 - neg));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		purestWarm2->fpdL ^= purestWarm2->fpdL << 13;
		purestWarm2->fpdL ^= purestWarm2->fpdL >> 17;
		purestWarm2->fpdL ^= purestWarm2->fpdL << 5;
		inputSampleL += (((double) purestWarm2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		purestWarm2->fpdR ^= purestWarm2->fpdR << 13;
		purestWarm2->fpdR ^= purestWarm2->fpdR >> 17;
		purestWarm2->fpdR ^= purestWarm2->fpdR << 5;
		inputSampleR += (((double) purestWarm2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PURESTWARM2_URI,
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
