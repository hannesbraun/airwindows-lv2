#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CURVE_URI "https://hannesbraun.net/ns/lv2/airwindows/curve"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];

	double gain;
	uint32_t fpdL;
	uint32_t fpdR;
} Curve;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Curve* curve = (Curve*) calloc(1, sizeof(Curve));
	return (LV2_Handle) curve;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Curve* curve = (Curve*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			curve->input[0] = (const float*) data;
			break;
		case INPUT_R:
			curve->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			curve->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			curve->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Curve* curve = (Curve*) instance;
	curve->gain = 1.0;
	curve->fpdL = 1.0;
	while (curve->fpdL < 16386) curve->fpdL = rand() * UINT32_MAX;
	curve->fpdR = 1.0;
	while (curve->fpdR < 16386) curve->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Curve* curve = (Curve*) instance;

	const float* in1 = curve->input[0];
	const float* in2 = curve->input[1];
	float* out1 = curve->output[0];
	float* out2 = curve->output[1];

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = curve->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = curve->fpdR * 1.18e-17;

		inputSampleL *= 0.5;
		inputSampleR *= 0.5;

		if (curve->gain < 0.0078125) curve->gain = 0.0078125;
		if (curve->gain > 1.0) curve->gain = 1.0;
		// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
		// smaller number is maximum clamping, if too small it'll take a while to bounce back
		inputSampleL *= curve->gain;
		inputSampleR *= curve->gain;

		curve->gain += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
		curve->gain += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
		// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed

		inputSampleL *= 2.0;
		inputSampleR *= 2.0;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		curve->fpdL ^= curve->fpdL << 13;
		curve->fpdL ^= curve->fpdL >> 17;
		curve->fpdL ^= curve->fpdL << 5;
		inputSampleL += (((double) curve->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		curve->fpdR ^= curve->fpdR << 13;
		curve->fpdR ^= curve->fpdR >> 17;
		curve->fpdR ^= curve->fpdR << 5;
		inputSampleR += (((double) curve->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CURVE_URI,
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
