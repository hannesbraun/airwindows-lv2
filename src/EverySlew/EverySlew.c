#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define EVERYSLEW_URI "https://hannesbraun.net/ns/lv2/airwindows/everyslew"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	SLEW = 4,
	DEPTH = 5,
	HALO = 6,
	INV_WET = 7
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
	const float* depth;
	const float* halo;
	const float* invWet;

	double gslew[gslew_total]; // probably worth just using a number here

	uint32_t fpdL;
	uint32_t fpdR;
} EverySlew;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	EverySlew* everySlew = (EverySlew*) calloc(1, sizeof(EverySlew));
	everySlew->sampleRate = rate;
	return (LV2_Handle) everySlew;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	EverySlew* everySlew = (EverySlew*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			everySlew->input[0] = (const float*) data;
			break;
		case INPUT_R:
			everySlew->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			everySlew->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			everySlew->output[1] = (float*) data;
			break;
		case SLEW:
			everySlew->slew = (const float*) data;
			break;
		case DEPTH:
			everySlew->depth = (const float*) data;
			break;
		case HALO:
			everySlew->halo = (const float*) data;
			break;
		case INV_WET:
			everySlew->invWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	EverySlew* everySlew = (EverySlew*) instance;

	for (int x = 0; x < gslew_total; x++) everySlew->gslew[x] = 0.0;
	everySlew->fpdL = 1.0;
	while (everySlew->fpdL < 16386) everySlew->fpdL = rand() * UINT32_MAX;
	everySlew->fpdR = 1.0;
	while (everySlew->fpdR < 16386) everySlew->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	EverySlew* everySlew = (EverySlew*) instance;

	const float* in1 = everySlew->input[0];
	const float* in2 = everySlew->input[1];
	float* out1 = everySlew->output[0];
	float* out2 = everySlew->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= everySlew->sampleRate;

	double source = pow(1 - *everySlew->slew, 4) / overallscale;
	int stages = (1.0 - *everySlew->depth) * 9.99;
	stages *= 5;
	double halo = *everySlew->halo;
	const float invWet = *everySlew->invWet;
	double wet = (invWet * 2.0) - 1.0; // inv-dry-wet for highpass
	double dry = 2.0 - (invWet * 2.0);
	if (dry > 1.0) dry = 1.0; // full dry for use with inv, to 0.0 at full wet

	everySlew->gslew[threshold10] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold9] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold8] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold7] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold6] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold5] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold4] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold3] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold2] = source;
	source *= 1.618033988749894848204586;
	everySlew->gslew[threshold1] = source;
	source *= 1.618033988749894848204586;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = everySlew->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = everySlew->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		for (int x = stages; x < gslew_total; x += 5) {
			if (((inputSampleL - everySlew->gslew[x]) - ((everySlew->gslew[x] - everySlew->gslew[x + 2]) * 0.618033988749894848204586)) > everySlew->gslew[x + 4])
				inputSampleL = (everySlew->gslew[x] - ((everySlew->gslew[x] - everySlew->gslew[x + 2]) * halo)) + (everySlew->gslew[x + 4] * (1.0 - halo));
			if (-((inputSampleL - everySlew->gslew[x]) - ((everySlew->gslew[x] - everySlew->gslew[x + 2]) * 0.618033988749894848204586)) > everySlew->gslew[x + 4])
				inputSampleL = (everySlew->gslew[x] - ((everySlew->gslew[x] - everySlew->gslew[x + 2]) * halo * 0.78)) - (everySlew->gslew[x + 4] * (1.0 - (halo * 0.78)));
			everySlew->gslew[x + 2] = everySlew->gslew[x] * (1.0 - halo);
			everySlew->gslew[x] = inputSampleL;

			if (((inputSampleR - everySlew->gslew[x + 1]) - ((everySlew->gslew[x + 1] - everySlew->gslew[x + 3]) * 0.618033988749894848204586)) > everySlew->gslew[x + 4])
				inputSampleR = (everySlew->gslew[x + 1] - ((everySlew->gslew[x + 1] - everySlew->gslew[x + 3]) * halo)) + (everySlew->gslew[x + 4] * (1.0 - halo));
			if (-((inputSampleR - everySlew->gslew[x + 1]) - ((everySlew->gslew[x + 1] - everySlew->gslew[x + 3]) * 0.618033988749894848204586)) > everySlew->gslew[x + 4])
				inputSampleR = (everySlew->gslew[x + 1] - ((everySlew->gslew[x + 1] - everySlew->gslew[x + 3]) * halo * 0.78)) - (everySlew->gslew[x + 4] * (1.0 - (halo * 0.78)));
			everySlew->gslew[x + 3] = everySlew->gslew[x + 1] * (1.0 - halo);
			everySlew->gslew[x + 1] = inputSampleR;
		}

		inputSampleL *= wet;
		inputSampleR *= wet;
		drySampleL *= dry;
		drySampleR *= dry;
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		everySlew->fpdL ^= everySlew->fpdL << 13;
		everySlew->fpdL ^= everySlew->fpdL >> 17;
		everySlew->fpdL ^= everySlew->fpdL << 5;
		inputSampleL += (((double) everySlew->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		everySlew->fpdR ^= everySlew->fpdR << 13;
		everySlew->fpdR ^= everySlew->fpdR >> 17;
		everySlew->fpdR ^= everySlew->fpdR << 5;
		inputSampleR += (((double) everySlew->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	EVERYSLEW_URI,
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
