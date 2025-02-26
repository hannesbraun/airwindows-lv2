#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define LRCONVOLVE_URI "https://hannesbraun.net/ns/lv2/airwindows/lrconvolve"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];

	uint32_t fpdL;
	uint32_t fpdR;
} LRConvolve;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	LRConvolve* lrConvolve = (LRConvolve*) calloc(1, sizeof(LRConvolve));
	return (LV2_Handle) lrConvolve;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	LRConvolve* lrConvolve = (LRConvolve*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			lrConvolve->input[0] = (const float*) data;
			break;
		case INPUT_R:
			lrConvolve->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			lrConvolve->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			lrConvolve->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	LRConvolve* lrConvolve = (LRConvolve*) instance;

	lrConvolve->fpdL = 1.0;
	while (lrConvolve->fpdL < 16386) lrConvolve->fpdL = rand() * UINT32_MAX;
	lrConvolve->fpdR = 1.0;
	while (lrConvolve->fpdR < 16386) lrConvolve->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	LRConvolve* lrConvolve = (LRConvolve*) instance;

	const float* in1 = lrConvolve->input[0];
	const float* in2 = lrConvolve->input[1];
	float* out1 = lrConvolve->output[0];
	float* out2 = lrConvolve->output[1];

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = lrConvolve->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = lrConvolve->fpdR * 1.18e-17;

		// blame Jannik Asfaig (BoyXx76) for this (and me (Chris)) :D
		double out = 0.0;
		if (inputSampleL > 0.0 && inputSampleR > 0.0) out = sqrt(fabs(inputSampleL) * fabs(inputSampleR));
		if (inputSampleL < 0.0 && inputSampleR > 0.0) out = -sqrt(fabs(inputSampleL) * fabs(inputSampleR));
		if (inputSampleL > 0.0 && inputSampleR < 0.0) out = -sqrt(fabs(inputSampleL) * fabs(inputSampleR));
		if (inputSampleL < 0.0 && inputSampleR < 0.0) out = sqrt(fabs(inputSampleL) * fabs(inputSampleR));
		inputSampleL = inputSampleR = out;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		lrConvolve->fpdL ^= lrConvolve->fpdL << 13;
		lrConvolve->fpdL ^= lrConvolve->fpdL >> 17;
		lrConvolve->fpdL ^= lrConvolve->fpdL << 5;
		inputSampleL += (((double) lrConvolve->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		lrConvolve->fpdR ^= lrConvolve->fpdR << 13;
		lrConvolve->fpdR ^= lrConvolve->fpdR >> 17;
		lrConvolve->fpdR ^= lrConvolve->fpdR << 5;
		inputSampleR += (((double) lrConvolve->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	LRCONVOLVE_URI,
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
