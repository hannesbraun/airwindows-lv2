#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define HIGHGLOSSDITHER_URI "https://hannesbraun.net/ns/lv2/airwindows/highglossdither"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];

	int position;
	bool flip;
	uint32_t fpdL;
	uint32_t fpdR;
} HighGlossDither;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	HighGlossDither* highGlossDither = (HighGlossDither*) calloc(1, sizeof(HighGlossDither));
	return (LV2_Handle) highGlossDither;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	HighGlossDither* highGlossDither = (HighGlossDither*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			highGlossDither->input[0] = (const float*) data;
			break;
		case INPUT_R:
			highGlossDither->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			highGlossDither->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			highGlossDither->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	HighGlossDither* highGlossDither = (HighGlossDither*) instance;

	highGlossDither->position = 99999999;
	highGlossDither->flip = false;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	HighGlossDither* highGlossDither = (HighGlossDither*) instance;

	const float* in1 = highGlossDither->input[0];
	const float* in2 = highGlossDither->input[1];
	float* out1 = highGlossDither->output[0];
	float* out2 = highGlossDither->output[1];

	int hotbinA;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = highGlossDither->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = highGlossDither->fpdR * 1.18e-17;

		inputSampleL *= 8388608.0;
		inputSampleR *= 8388608.0;
		// 0-1 is now one bit, now we dither

		highGlossDither->position += 1;
		// Note- uses integer overflow as a 'mod' operator
		hotbinA = highGlossDither->position * highGlossDither->position;
		hotbinA = hotbinA % 170003;
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 17011;
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 1709;
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 173;
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 17;
		hotbinA *= 0.0635;
		if (highGlossDither->flip) hotbinA = -hotbinA;

		inputSampleL += hotbinA;
		inputSampleR += hotbinA;
		inputSampleL = floor(inputSampleL);
		inputSampleR = floor(inputSampleR);
		// Quadratic dither

		highGlossDither->flip = !highGlossDither->flip;

		inputSampleL /= 8388608.0;
		inputSampleR /= 8388608.0;

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
	HIGHGLOSSDITHER_URI,
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
