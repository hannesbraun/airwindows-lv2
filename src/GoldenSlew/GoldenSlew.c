#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define GOLDENSLEW_URI "https://hannesbraun.net/ns/lv2/airwindows/goldenslew"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	SLEW = 4
} PortIndex;

enum {
	prevSampL1,
	prevSampR1,
	threshold1,
	prevSampL2,
	prevSampR2,
	threshold2,
	prevSampL3,
	prevSampR3,
	threshold3,
	prevSampL4,
	prevSampR4,
	threshold4,
	prevSampL5,
	prevSampR5,
	threshold5,
	prevSampL6,
	prevSampR6,
	threshold6,
	prevSampL7,
	prevSampR7,
	threshold7,
	prevSampL8,
	prevSampR8,
	threshold8,
	prevSampL9,
	prevSampR9,
	threshold9,
	prevSampL10,
	prevSampR10,
	threshold10,
	gslew_total
}; // fixed frequency pear filter for ultrasonics, stereo

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* slew;

	double gslew[gslew_total];

	uint32_t fpdL;
	uint32_t fpdR;
} GoldenSlew;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	GoldenSlew* goldenSlew = (GoldenSlew*) calloc(1, sizeof(GoldenSlew));
	goldenSlew->sampleRate = rate;
	return (LV2_Handle) goldenSlew;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	GoldenSlew* goldenSlew = (GoldenSlew*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			goldenSlew->input[0] = (const float*) data;
			break;
		case INPUT_R:
			goldenSlew->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			goldenSlew->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			goldenSlew->output[1] = (float*) data;
			break;
		case SLEW:
			goldenSlew->slew = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	GoldenSlew* goldenSlew = (GoldenSlew*) instance;
	for (int x = 0; x < gslew_total; x++) goldenSlew->gslew[x] = 0.0;
	goldenSlew->fpdL = 1.0;
	while (goldenSlew->fpdL < 16386) goldenSlew->fpdL = rand() * UINT32_MAX;
	goldenSlew->fpdR = 1.0;
	while (goldenSlew->fpdR < 16386) goldenSlew->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	GoldenSlew* goldenSlew = (GoldenSlew*) instance;

	const float* in1 = goldenSlew->input[0];
	const float* in2 = goldenSlew->input[1];
	float* out1 = goldenSlew->output[0];
	float* out2 = goldenSlew->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= goldenSlew->sampleRate;

	double source = pow(1 - *goldenSlew->slew, 4) / overallscale;

	goldenSlew->gslew[threshold10] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold9] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold8] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold7] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold6] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold5] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold4] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold3] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold2] = source;
	source *= 1.618033988749894848204586;
	goldenSlew->gslew[threshold1] = source;
	source *= 1.618033988749894848204586;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = goldenSlew->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = goldenSlew->fpdR * 1.18e-17;

		for (int x = 0; x < gslew_total; x += 3) {
			if ((inputSampleL - goldenSlew->gslew[x]) > goldenSlew->gslew[x + 2])
				inputSampleL = goldenSlew->gslew[x] + goldenSlew->gslew[x + 2];
			if (-(inputSampleL - goldenSlew->gslew[x]) > goldenSlew->gslew[x + 2])
				inputSampleL = goldenSlew->gslew[x] - goldenSlew->gslew[x + 2];
			goldenSlew->gslew[x] = inputSampleL;

			if ((inputSampleR - goldenSlew->gslew[x + 1]) > goldenSlew->gslew[x + 2])
				inputSampleR = goldenSlew->gslew[x + 1] + goldenSlew->gslew[x + 2];
			if (-(inputSampleR - goldenSlew->gslew[x + 1]) > goldenSlew->gslew[x + 2])
				inputSampleR = goldenSlew->gslew[x + 1] - goldenSlew->gslew[x + 2];
			goldenSlew->gslew[x + 1] = inputSampleR;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		goldenSlew->fpdL ^= goldenSlew->fpdL << 13;
		goldenSlew->fpdL ^= goldenSlew->fpdL >> 17;
		goldenSlew->fpdL ^= goldenSlew->fpdL << 5;
		inputSampleL += (((double) goldenSlew->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		goldenSlew->fpdR ^= goldenSlew->fpdR << 13;
		goldenSlew->fpdR ^= goldenSlew->fpdR >> 17;
		goldenSlew->fpdR ^= goldenSlew->fpdR << 5;
		inputSampleR += (((double) goldenSlew->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	GOLDENSLEW_URI,
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
