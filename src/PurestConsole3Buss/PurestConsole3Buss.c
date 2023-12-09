#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PURESTCONSOLE3BUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/purestconsole3buss"

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
} PurestConsole3Buss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestConsole3Buss* purestConsole3Buss = (PurestConsole3Buss*) calloc(1, sizeof(PurestConsole3Buss));
	return (LV2_Handle) purestConsole3Buss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestConsole3Buss* purestConsole3Buss = (PurestConsole3Buss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestConsole3Buss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestConsole3Buss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestConsole3Buss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestConsole3Buss->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestConsole3Buss* purestConsole3Buss = (PurestConsole3Buss*) instance;
	purestConsole3Buss->fpdL = 1.0;
	while (purestConsole3Buss->fpdL < 16386) purestConsole3Buss->fpdL = rand() * UINT32_MAX;
	purestConsole3Buss->fpdR = 1.0;
	while (purestConsole3Buss->fpdR < 16386) purestConsole3Buss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestConsole3Buss* purestConsole3Buss = (PurestConsole3Buss*) instance;

	const float* in1 = purestConsole3Buss->input[0];
	const float* in2 = purestConsole3Buss->input[1];
	float* out1 = purestConsole3Buss->output[0];
	float* out2 = purestConsole3Buss->output[1];

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestConsole3Buss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestConsole3Buss->fpdR * 1.18e-17;

		inputSampleL += (pow(inputSampleL, 3) / 4.0) + (pow(inputSampleL, 5) / 8.0) + (pow(inputSampleL, 7) / 16.0) + (pow(inputSampleL, 9) / 32.0);
		inputSampleR += (pow(inputSampleR, 3) / 4.0) + (pow(inputSampleR, 5) / 8.0) + (pow(inputSampleR, 7) / 16.0) + (pow(inputSampleR, 9) / 32.0);
		// crude arcsine. Note that because modern processors love math more than extra variables, this is optimized

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		purestConsole3Buss->fpdL ^= purestConsole3Buss->fpdL << 13;
		purestConsole3Buss->fpdL ^= purestConsole3Buss->fpdL >> 17;
		purestConsole3Buss->fpdL ^= purestConsole3Buss->fpdL << 5;
		inputSampleL += (((double) purestConsole3Buss->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		purestConsole3Buss->fpdR ^= purestConsole3Buss->fpdR << 13;
		purestConsole3Buss->fpdR ^= purestConsole3Buss->fpdR >> 17;
		purestConsole3Buss->fpdR ^= purestConsole3Buss->fpdR << 5;
		inputSampleR += (((double) purestConsole3Buss->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PURESTCONSOLE3BUSS_URI,
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
