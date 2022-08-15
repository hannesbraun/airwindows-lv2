#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SPIRAL_URI "https://hannesbraun.net/ns/lv2/airwindows/spiral"

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
} Spiral;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Spiral* spiral = (Spiral*) calloc(1, sizeof(Spiral));
	return (LV2_Handle) spiral;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Spiral* spiral = (Spiral*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			spiral->input[0] = (const float*) data;
			break;
		case INPUT_R:
			spiral->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			spiral->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			spiral->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Spiral* spiral = (Spiral*) instance;
	spiral->fpdL = 1.0;
	while (spiral->fpdL < 16386) spiral->fpdL = rand() * UINT32_MAX;
	spiral->fpdR = 1.0;
	while (spiral->fpdR < 16386) spiral->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Spiral* spiral = (Spiral*) instance;

	const float* in1 = spiral->input[0];
	const float* in2 = spiral->input[1];
	float* out1 = spiral->output[0];
	float* out2 = spiral->output[1];

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = spiral->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = spiral->fpdR * 1.18e-17;

		// clip to 1.2533141373155 to reach maximum output
		inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
		inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		spiral->fpdL ^= spiral->fpdL << 13;
		spiral->fpdL ^= spiral->fpdL >> 17;
		spiral->fpdL ^= spiral->fpdL << 5;
		inputSampleL += (((double) spiral->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		spiral->fpdR ^= spiral->fpdR << 13;
		spiral->fpdR ^= spiral->fpdR >> 17;
		spiral->fpdR ^= spiral->fpdR << 5;
		inputSampleR += (((double) spiral->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SPIRAL_URI,
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
