#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PURESTCONSOLEBUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/purestconsolebuss"

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
} PurestConsoleBuss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) calloc(1, sizeof(PurestConsoleBuss));
	return (LV2_Handle) purestConsoleBuss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestConsoleBuss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestConsoleBuss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestConsoleBuss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestConsoleBuss->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) instance;
	purestConsoleBuss->fpdL = 1.0;
	while (purestConsoleBuss->fpdL < 16386) purestConsoleBuss->fpdL = rand() * UINT32_MAX;
	purestConsoleBuss->fpdR = 1.0;
	while (purestConsoleBuss->fpdR < 16386) purestConsoleBuss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) instance;

	const float* in1 = purestConsoleBuss->input[0];
	const float* in2 = purestConsoleBuss->input[1];
	float* out1 = purestConsoleBuss->output[0];
	float* out2 = purestConsoleBuss->output[1];

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestConsoleBuss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestConsoleBuss->fpdR * 1.18e-17;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		// amplitude aspect

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		purestConsoleBuss->fpdL ^= purestConsoleBuss->fpdL << 13;
		purestConsoleBuss->fpdL ^= purestConsoleBuss->fpdL >> 17;
		purestConsoleBuss->fpdL ^= purestConsoleBuss->fpdL << 5;
		inputSampleL += (((double) purestConsoleBuss->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		purestConsoleBuss->fpdR ^= purestConsoleBuss->fpdR << 13;
		purestConsoleBuss->fpdR ^= purestConsoleBuss->fpdR >> 17;
		purestConsoleBuss->fpdR ^= purestConsoleBuss->fpdR << 5;
		inputSampleR += (((double) purestConsoleBuss->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PURESTCONSOLEBUSS_URI,
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
