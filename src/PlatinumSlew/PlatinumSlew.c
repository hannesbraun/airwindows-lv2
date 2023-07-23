#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PLATINUMSLEW_URI "https://hannesbraun.net/ns/lv2/airwindows/platinumslew"

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
	invSampL1,
	invSampR1,
	threshold1,
	prevSampL2,
	prevSampR2,
	invSampL2,
	invSampR2,
	threshold2,
	prevSampL3,
	prevSampR3,
	invSampL3,
	invSampR3,
	threshold3,
	prevSampL4,
	prevSampR4,
	invSampL4,
	invSampR4,
	threshold4,
	prevSampL5,
	prevSampR5,
	invSampL5,
	invSampR5,
	threshold5,
	prevSampL6,
	prevSampR6,
	invSampL6,
	invSampR6,
	threshold6,
	prevSampL7,
	prevSampR7,
	invSampL7,
	invSampR7,
	threshold7,
	prevSampL8,
	prevSampR8,
	invSampL8,
	invSampR8,
	threshold8,
	prevSampL9,
	prevSampR9,
	invSampL9,
	invSampR9,
	threshold9,
	prevSampL10,
	prevSampR10,
	invSampL10,
	invSampR10,
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
} PlatinumSlew;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PlatinumSlew* platinumSlew = (PlatinumSlew*) calloc(1, sizeof(PlatinumSlew));
	platinumSlew->sampleRate = rate;
	return (LV2_Handle) platinumSlew;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PlatinumSlew* platinumSlew = (PlatinumSlew*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			platinumSlew->input[0] = (const float*) data;
			break;
		case INPUT_R:
			platinumSlew->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			platinumSlew->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			platinumSlew->output[1] = (float*) data;
			break;
		case SLEW:
			platinumSlew->slew = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PlatinumSlew* platinumSlew = (PlatinumSlew*) instance;

	for (int x = 0; x < gslew_total; x++) platinumSlew->gslew[x] = 0.0;
	platinumSlew->fpdL = 1.0;
	while (platinumSlew->fpdL < 16386) platinumSlew->fpdL = rand() * UINT32_MAX;
	platinumSlew->fpdR = 1.0;
	while (platinumSlew->fpdR < 16386) platinumSlew->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PlatinumSlew* platinumSlew = (PlatinumSlew*) instance;

	const float* in1 = platinumSlew->input[0];
	const float* in2 = platinumSlew->input[1];
	float* out1 = platinumSlew->output[0];
	float* out2 = platinumSlew->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= platinumSlew->sampleRate;

	double source = pow(1 - *platinumSlew->slew, 4) / overallscale;

	platinumSlew->gslew[threshold10] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold9] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold8] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold7] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold6] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold5] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold4] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold3] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold2] = source;
	source *= 1.618033988749894848204586;
	platinumSlew->gslew[threshold1] = source;
	source *= 1.618033988749894848204586;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = platinumSlew->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = platinumSlew->fpdR * 1.18e-17;

		for (int x = 0; x < gslew_total; x += 5) {
			if (((inputSampleL - platinumSlew->gslew[x]) - ((platinumSlew->gslew[x] - platinumSlew->gslew[x + 2]) * 0.618033988749894848204586)) > platinumSlew->gslew[x + 4])
				inputSampleL = (platinumSlew->gslew[x] - ((platinumSlew->gslew[x] - platinumSlew->gslew[x + 2]) * 0.156)) + (platinumSlew->gslew[x + 4] * 0.844);
			if (-((inputSampleL - platinumSlew->gslew[x]) - ((platinumSlew->gslew[x] - platinumSlew->gslew[x + 2]) * 0.618033988749894848204586)) > platinumSlew->gslew[x + 4])
				inputSampleL = (platinumSlew->gslew[x] - ((platinumSlew->gslew[x] - platinumSlew->gslew[x + 2]) * 0.2)) - (platinumSlew->gslew[x + 4] * 0.8);
			platinumSlew->gslew[x + 2] = platinumSlew->gslew[x] * 0.844;
			platinumSlew->gslew[x] = inputSampleL;

			if (((inputSampleR - platinumSlew->gslew[x + 1]) - ((platinumSlew->gslew[x + 1] - platinumSlew->gslew[x + 3]) * 0.618033988749894848204586)) > platinumSlew->gslew[x + 4])
				inputSampleR = (platinumSlew->gslew[x + 1] - ((platinumSlew->gslew[x + 1] - platinumSlew->gslew[x + 3]) * 0.156)) + (platinumSlew->gslew[x + 4] * 0.844);
			if (-((inputSampleR - platinumSlew->gslew[x + 1]) - ((platinumSlew->gslew[x + 1] - platinumSlew->gslew[x + 3]) * 0.618033988749894848204586)) > platinumSlew->gslew[x + 4])
				inputSampleR = (platinumSlew->gslew[x + 1] - ((platinumSlew->gslew[x + 1] - platinumSlew->gslew[x + 3]) * 0.2)) - (platinumSlew->gslew[x + 4] * 0.8);
			platinumSlew->gslew[x + 3] = platinumSlew->gslew[x + 1] * 0.844;
			platinumSlew->gslew[x + 1] = inputSampleR;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		platinumSlew->fpdL ^= platinumSlew->fpdL << 13;
		platinumSlew->fpdL ^= platinumSlew->fpdL >> 17;
		platinumSlew->fpdL ^= platinumSlew->fpdL << 5;
		inputSampleL += (((double) platinumSlew->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		platinumSlew->fpdR ^= platinumSlew->fpdR << 13;
		platinumSlew->fpdR ^= platinumSlew->fpdR >> 17;
		platinumSlew->fpdR ^= platinumSlew->fpdR << 5;
		inputSampleR += (((double) platinumSlew->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PLATINUMSLEW_URI,
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
