#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CREATURE_URI "https://hannesbraun.net/ns/lv2/airwindows/creature"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	DEPTH = 5,
	INV_WET = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* depth;
	const float* invWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double slewL[102]; // probably worth just using a number here
	double slewR[102]; // probably worth just using a number here
} Creature;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Creature* creature = (Creature*) calloc(1, sizeof(Creature));
	creature->sampleRate = rate;
	return (LV2_Handle) creature;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Creature* creature = (Creature*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			creature->input[0] = (const float*) data;
			break;
		case INPUT_R:
			creature->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			creature->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			creature->output[1] = (float*) data;
			break;
		case DRIVE:
			creature->drive = (const float*) data;
			break;
		case DEPTH:
			creature->depth = (const float*) data;
			break;
		case INV_WET:
			creature->invWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Creature* creature = (Creature*) instance;

	for (int x = 0; x < 101; x++) {
		creature->slewL[x] = 0.0;
		creature->slewR[x] = 0.0;
	}

	creature->fpdL = 1.0;
	while (creature->fpdL < 16386) creature->fpdL = rand() * UINT32_MAX;
	creature->fpdR = 1.0;
	while (creature->fpdR < 16386) creature->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Creature* creature = (Creature*) instance;

	const float* in1 = creature->input[0];
	const float* in2 = creature->input[1];
	float* out1 = creature->output[0];
	float* out2 = creature->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= creature->sampleRate;

	double source = 1.0 - pow(1.0 - *creature->drive, 5);
	int stages = (pow(*creature->depth, 2) * 32.0 * sqrt(overallscale)) + 1;
	const float invWet = *creature->invWet;
	double wet = (invWet * 2.0) - 1.0; // inv-dry-wet for highpass
	double dry = 2.0 - (invWet * 2.0);
	if (dry > 1.0) dry = 1.0; // full dry for use with inv, to 0.0 at full wet

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = creature->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = creature->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		for (int x = 0; x < stages; x++) {
			inputSampleL = (creature->slewL[x] + (sin(creature->slewL[x] - inputSampleL) * 0.5)) * source;
			creature->slewL[x] = inputSampleL * 0.5;
			inputSampleR = (creature->slewR[x] + (sin(creature->slewR[x] - inputSampleR) * 0.5)) * source;
			creature->slewR[x] = inputSampleR * 0.5;
		}
		if (stages % 2 > 0) {
			inputSampleL = -inputSampleL;
			inputSampleR = -inputSampleR;
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
		creature->fpdL ^= creature->fpdL << 13;
		creature->fpdL ^= creature->fpdL >> 17;
		creature->fpdL ^= creature->fpdL << 5;
		inputSampleL += (((double) creature->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		creature->fpdR ^= creature->fpdR << 13;
		creature->fpdR ^= creature->fpdR >> 17;
		creature->fpdR ^= creature->fpdR << 5;
		inputSampleR += (((double) creature->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CREATURE_URI,
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
