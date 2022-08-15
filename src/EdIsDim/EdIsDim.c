#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define EDISDIM_URI "https://hannesbraun.net/ns/lv2/airwindows/edisdim"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	EDISDIM = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* edisdim;

	uint32_t fpdL;
	uint32_t fpdR;
} EdIsDim;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	EdIsDim* edisdim = (EdIsDim*) calloc(1, sizeof(EdIsDim));
	return (LV2_Handle) edisdim;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	EdIsDim* edisdim = (EdIsDim*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			edisdim->input[0] = (const float*) data;
			break;
		case INPUT_R:
			edisdim->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			edisdim->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			edisdim->output[1] = (float*) data;
			break;
		case EDISDIM:
			edisdim->edisdim = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	EdIsDim* edisdim = (EdIsDim*) instance;
	edisdim->fpdL = 1.0;
	while (edisdim->fpdL < 16386) edisdim->fpdL = rand() * UINT32_MAX;
	edisdim->fpdR = 1.0;
	while (edisdim->fpdR < 16386) edisdim->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	EdIsDim* edisdim = (EdIsDim*) instance;

	const float* in1 = edisdim->input[0];
	const float* in2 = edisdim->input[1];
	float* out1 = edisdim->output[0];
	float* out2 = edisdim->output[1];

	double inputSampleL;
	double inputSampleR;
	double mid;
	double side;

	double midgain = *edisdim->edisdim * 2.0;
	double sidegain = 2.0 - midgain;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = edisdim->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = edisdim->fpdR * 1.18e-17;

		inputSampleL *= midgain;
		inputSampleR *= sidegain;

		mid = (inputSampleL + inputSampleR) / 2.0;
		side = (inputSampleL - inputSampleR) / 2.0;

		// begin 32 bit floating point dither
		int expon;
		frexpf((float) mid, &expon);
		edisdim->fpdL ^= edisdim->fpdL << 13;
		edisdim->fpdL ^= edisdim->fpdL >> 17;
		edisdim->fpdL ^= edisdim->fpdL << 5;
		mid += (((double) edisdim->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		// end 32 bit floating point dither
		// begin 32 bit floating point dither
		frexpf((float) side, &expon);
		edisdim->fpdR ^= edisdim->fpdR << 13;
		edisdim->fpdR ^= edisdim->fpdR >> 17;
		edisdim->fpdR ^= edisdim->fpdR << 5;
		side += (((double) edisdim->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		// end 32 bit floating point dither

		*out1 = (float) mid;
		*out2 = (float) side;

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
	EDISDIM_URI,
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
