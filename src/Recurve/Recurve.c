#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define RECURVE_URI "https://hannesbraun.net/ns/lv2/airwindows/recurve"

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
} Recurve;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Recurve* recurve = (Recurve*) calloc(1, sizeof(Recurve));
	return (LV2_Handle) recurve;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Recurve* recurve = (Recurve*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			recurve->input[0] = (const float*) data;
			break;
		case INPUT_R:
			recurve->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			recurve->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			recurve->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Recurve* recurve = (Recurve*) instance;
	recurve->gain = 2.0;
	recurve->fpdL = 1.0;
	while (recurve->fpdL < 16386) recurve->fpdL = rand() * UINT32_MAX;
	recurve->fpdR = 1.0;
	while (recurve->fpdR < 16386) recurve->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Recurve* recurve = (Recurve*) instance;

	const float* in1 = recurve->input[0];
	const float* in2 = recurve->input[1];
	float* out1 = recurve->output[0];
	float* out2 = recurve->output[1];

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = recurve->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = recurve->fpdR * 1.18e-17;

		inputSampleL *= 0.5;
		inputSampleR *= 0.5;

		if (recurve->gain < 0.0078125) recurve->gain = 0.0078125;
		if (recurve->gain > 2.0) recurve->gain = 2.0;
		// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
		// smaller number is maximum clamping, if too small it'll take a while to bounce back
		inputSampleL *= recurve->gain;
		inputSampleR *= recurve->gain;

		recurve->gain += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
		recurve->gain += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
		// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed

		inputSampleL *= 2.0;
		inputSampleR *= 2.0;

		if (inputSampleL > 0.9999) inputSampleL = 0.9999;
		if (inputSampleR > 0.9999) inputSampleR = 0.9999;
		if (inputSampleL < -0.9999) inputSampleL = -0.9999;
		if (inputSampleR < -0.9999) inputSampleR = -0.9999;
		// iron bar so people can play with this as a loudenator

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		recurve->fpdL ^= recurve->fpdL << 13;
		recurve->fpdL ^= recurve->fpdL >> 17;
		recurve->fpdL ^= recurve->fpdL << 5;
		inputSampleL += (((double) recurve->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		recurve->fpdR ^= recurve->fpdR << 13;
		recurve->fpdR ^= recurve->fpdR >> 17;
		recurve->fpdR ^= recurve->fpdR << 5;
		inputSampleR += (((double) recurve->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	RECURVE_URI,
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
