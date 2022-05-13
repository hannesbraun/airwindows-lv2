#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE6BUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/console6buss"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* inputGain;

	uint32_t fpdL;
	uint32_t fpdR;
} Console6Buss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console6Buss* console6Buss = (Console6Buss*) calloc(1, sizeof(Console6Buss));
	return (LV2_Handle) console6Buss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console6Buss* console6Buss = (Console6Buss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console6Buss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console6Buss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console6Buss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console6Buss->output[1] = (float*) data;
			break;
		case INPUT:
			console6Buss->inputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console6Buss* console6Buss = (Console6Buss*) instance;
	console6Buss->fpdL = 1.0;
	while (console6Buss->fpdL < 16386) console6Buss->fpdL = rand() * UINT32_MAX;
	console6Buss->fpdR = 1.0;
	while (console6Buss->fpdR < 16386) console6Buss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console6Buss* console6Buss = (Console6Buss*) instance;

	const float* in1 = console6Buss->input[0];
	const float* in2 = console6Buss->input[1];
	float* out1 = console6Buss->output[0];
	float* out2 = console6Buss->output[1];

	const float gain = *console6Buss->inputGain;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console6Buss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console6Buss->fpdR * 1.18e-17;


		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		//encode/decode courtesy of torridgristle under the MIT license
		//Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		else if (inputSampleL > 0.0) inputSampleL = 1.0 - pow(1.0 - inputSampleL, 0.5);

		if (inputSampleL < -1.0) inputSampleL = -1.0;
		else if (inputSampleL < 0.0) inputSampleL = -1.0 + pow(1.0 + inputSampleL, 0.5);

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		else if (inputSampleR > 0.0) inputSampleR = 1.0 - pow(1.0 - inputSampleR, 0.5);

		if (inputSampleR < -1.0) inputSampleR = -1.0;
		else if (inputSampleR < 0.0) inputSampleR = -1.0 + pow(1.0 + inputSampleR, 0.5);


		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		console6Buss->fpdL ^= console6Buss->fpdL << 13;
		console6Buss->fpdL ^= console6Buss->fpdL >> 17;
		console6Buss->fpdL ^= console6Buss->fpdL << 5;
		inputSampleL += (((double)console6Buss->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		console6Buss->fpdR ^= console6Buss->fpdR << 13;
		console6Buss->fpdR ^= console6Buss->fpdR >> 17;
		console6Buss->fpdR ^= console6Buss->fpdR << 5;
		inputSampleR += (((double)console6Buss->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	CONSOLE6BUSS_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
