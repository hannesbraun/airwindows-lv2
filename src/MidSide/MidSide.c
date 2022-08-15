#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define MIDSIDE_URI "https://hannesbraun.net/ns/lv2/airwindows/midside"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	MIDSIDE = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* midside;

	uint32_t fpdL;
	uint32_t fpdR;
} MidSide;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	MidSide* midside = (MidSide*) calloc(1, sizeof(MidSide));
	return (LV2_Handle) midside;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	MidSide* midside = (MidSide*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			midside->input[0] = (const float*) data;
			break;
		case INPUT_R:
			midside->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			midside->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			midside->output[1] = (float*) data;
			break;
		case MIDSIDE:
			midside->midside = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	MidSide* midside = (MidSide*) instance;
	midside->fpdL = 1.0;
	while (midside->fpdL < 16386) midside->fpdL = rand() * UINT32_MAX;
	midside->fpdR = 1.0;
	while (midside->fpdR < 16386) midside->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	MidSide* midside = (MidSide*) instance;

	const float* in1 = midside->input[0];
	const float* in2 = midside->input[1];
	float* out1 = midside->output[0];
	float* out2 = midside->output[1];

	double inputSampleL;
	double inputSampleR;

	double mid;
	double side;

	double sidegain = *midside->midside * 2.0;
	double midgain = 2.0 - sidegain;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = midside->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = midside->fpdR * 1.18e-17;

		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;

		mid *= midgain;
		side *= sidegain;

		// begin 32 bit floating point dither
		int expon;
		frexpf((float) mid, &expon);
		midside->fpdL ^= midside->fpdL << 13;
		midside->fpdL ^= midside->fpdL >> 17;
		midside->fpdL ^= midside->fpdL << 5;
		mid += (((double) midside->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		// end 32 bit floating point dither
		frexpf((float) side, &expon);
		midside->fpdR ^= midside->fpdR << 13;
		midside->fpdR ^= midside->fpdR >> 17;
		midside->fpdR ^= midside->fpdR << 5;
		side += (((double) midside->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MIDSIDE_URI,
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
