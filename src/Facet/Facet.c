#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define FACET_URI "https://hannesbraun.net/ns/lv2/airwindows/facet"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FACET = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* facet;

	uint32_t fpdL;
	uint32_t fpdR;
} Facet;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Facet* facet = (Facet*) calloc(1, sizeof(Facet));
	return (LV2_Handle) facet;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Facet* facet = (Facet*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			facet->input[0] = (const float*) data;
			break;
		case INPUT_R:
			facet->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			facet->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			facet->output[1] = (float*) data;
			break;
		case FACET:
			facet->facet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Facet* facet = (Facet*) instance;
	facet->fpdL = 1.0;
	while (facet->fpdL < 16386) facet->fpdL = rand() * UINT32_MAX;
	facet->fpdR = 1.0;
	while (facet->fpdR < 16386) facet->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Facet* facet = (Facet*) instance;

	const float* in1 = facet->input[0];
	const float* in2 = facet->input[1];
	float* out1 = facet->output[0];
	float* out2 = facet->output[1];

	double pos = *facet->facet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = facet->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = facet->fpdR * 1.18e-17;

		if (inputSampleL > pos) {
			inputSampleL = ((inputSampleL - pos) * pos) + pos;
		} // facet algorithm: put a corner distortion in producing an artifact
		if (inputSampleL < -pos) {
			inputSampleL = ((inputSampleL + pos) * pos) - pos;
		} // we increasingly produce a sharp 'angle' in the transfer function

		if (inputSampleR > pos) {
			inputSampleR = ((inputSampleR - pos) * pos) + pos;
		} // facet algorithm: put a corner distortion in producing an artifact
		if (inputSampleR < -pos) {
			inputSampleR = ((inputSampleR + pos) * pos) - pos;
		} // we increasingly produce a sharp 'angle' in the transfer function

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		facet->fpdL ^= facet->fpdL << 13;
		facet->fpdL ^= facet->fpdL >> 17;
		facet->fpdL ^= facet->fpdL << 5;
		inputSampleL += (((double) facet->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		facet->fpdR ^= facet->fpdR << 13;
		facet->fpdR ^= facet->fpdR >> 17;
		facet->fpdR ^= facet->fpdR << 5;
		inputSampleR += (((double) facet->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	FACET_URI,
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
