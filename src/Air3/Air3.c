#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define AIR3_URI "https://hannesbraun.net/ns/lv2/airwindows/air3"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	AIR = 4,
	GND = 5
} PortIndex;

enum {
	pvAL1,
	pvSL1,
	accSL1,
	acc2SL1,
	pvAL2,
	pvSL2,
	accSL2,
	acc2SL2,
	pvAL3,
	pvSL3,
	accSL3,
	pvAL4,
	pvSL4,
	gndavgL,
	outAL,
	gainAL,
	pvAR1,
	pvSR1,
	accSR1,
	acc2SR1,
	pvAR2,
	pvSR2,
	accSR2,
	acc2SR2,
	pvAR3,
	pvSR3,
	accSR3,
	pvAR4,
	pvSR4,
	gndavgR,
	outAR,
	gainAR,
	air_total
};

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* airParam;
	const float* gnd;

	double air[air_total];

	uint32_t fpdL;
	uint32_t fpdR;
} Air3;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Air3* air3 = (Air3*) calloc(1, sizeof(Air3));
	air3->sampleRate = rate;
	return (LV2_Handle) air3;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Air3* air3 = (Air3*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			air3->input[0] = (const float*) data;
			break;
		case INPUT_R:
			air3->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			air3->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			air3->output[1] = (float*) data;
			break;
		case AIR:
			air3->airParam = (const float*) data;
			break;
		case GND:
			air3->gnd = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Air3* air3 = (Air3*) instance;
	for (int x = 0; x < air_total; x++) air3->air[x] = 0.0;

	air3->fpdL = 1.0;
	while (air3->fpdL < 16386) air3->fpdL = rand() * UINT32_MAX;
	air3->fpdR = 1.0;
	while (air3->fpdR < 16386) air3->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Air3* air3 = (Air3*) instance;

	const float* in1 = air3->input[0];
	const float* in2 = air3->input[1];
	float* out1 = air3->output[0];
	float* out2 = air3->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= air3->sampleRate;

	double airGain = *air3->airParam * 2.0;
	if (airGain > 1.0) airGain = pow(airGain, 3.0 + sqrt(overallscale));
	double gndGain = *air3->gnd * 2.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = air3->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = air3->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		air3->air[pvSL4] = air3->air[pvAL4] - air3->air[pvAL3];
		air3->air[pvSL3] = air3->air[pvAL3] - air3->air[pvAL2];
		air3->air[pvSL2] = air3->air[pvAL2] - air3->air[pvAL1];
		air3->air[pvSL1] = air3->air[pvAL1] - inputSampleL;

		air3->air[accSL3] = air3->air[pvSL4] - air3->air[pvSL3];
		air3->air[accSL2] = air3->air[pvSL3] - air3->air[pvSL2];
		air3->air[accSL1] = air3->air[pvSL2] - air3->air[pvSL1];

		air3->air[acc2SL2] = air3->air[accSL3] - air3->air[accSL2];
		air3->air[acc2SL1] = air3->air[accSL2] - air3->air[accSL1];

		air3->air[outAL] = -(air3->air[pvAL1] + air3->air[pvSL3] + air3->air[acc2SL2] - ((air3->air[acc2SL2] + air3->air[acc2SL1]) * 0.5));

		air3->air[gainAL] *= 0.5;
		air3->air[gainAL] += fabs(drySampleL - air3->air[outAL]) * 0.5;
		if (air3->air[gainAL] > 0.3 * sqrt(overallscale)) air3->air[gainAL] = 0.3 * sqrt(overallscale);
		air3->air[pvAL4] = air3->air[pvAL3];
		air3->air[pvAL3] = air3->air[pvAL2];
		air3->air[pvAL2] = air3->air[pvAL1];
		air3->air[pvAL1] = (air3->air[gainAL] * air3->air[outAL]) + drySampleL;

		double gnd = drySampleL - ((air3->air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
		double temp = (gnd + air3->air[gndavgL]) * 0.5;
		air3->air[gndavgL] = gnd;
		gnd = temp;

		inputSampleL = ((drySampleL - gnd) * airGain) + (gnd * gndGain);

		air3->air[pvSR4] = air3->air[pvAR4] - air3->air[pvAR3];
		air3->air[pvSR3] = air3->air[pvAR3] - air3->air[pvAR2];
		air3->air[pvSR2] = air3->air[pvAR2] - air3->air[pvAR1];
		air3->air[pvSR1] = air3->air[pvAR1] - inputSampleR;

		air3->air[accSR3] = air3->air[pvSR4] - air3->air[pvSR3];
		air3->air[accSR2] = air3->air[pvSR3] - air3->air[pvSR2];
		air3->air[accSR1] = air3->air[pvSR2] - air3->air[pvSR1];

		air3->air[acc2SR2] = air3->air[accSR3] - air3->air[accSR2];
		air3->air[acc2SR1] = air3->air[accSR2] - air3->air[accSR1];

		air3->air[outAR] = -(air3->air[pvAR1] + air3->air[pvSR3] + air3->air[acc2SR2] - ((air3->air[acc2SR2] + air3->air[acc2SR1]) * 0.5));

		air3->air[gainAR] *= 0.5;
		air3->air[gainAR] += fabs(drySampleR - air3->air[outAR]) * 0.5;
		if (air3->air[gainAR] > 0.3 * sqrt(overallscale)) air3->air[gainAR] = 0.3 * sqrt(overallscale);
		air3->air[pvAR4] = air3->air[pvAR3];
		air3->air[pvAR3] = air3->air[pvAR2];
		air3->air[pvAR2] = air3->air[pvAR1];
		air3->air[pvAR1] = (air3->air[gainAR] * air3->air[outAR]) + drySampleR;

		gnd = drySampleR - ((air3->air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
		temp = (gnd + air3->air[gndavgR]) * 0.5;
		air3->air[gndavgR] = gnd;
		gnd = temp;

		inputSampleR = ((drySampleR - gnd) * airGain) + (gnd * gndGain);

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		air3->fpdL ^= air3->fpdL << 13;
		air3->fpdL ^= air3->fpdL >> 17;
		air3->fpdL ^= air3->fpdL << 5;
		inputSampleL += (((double) air3->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		air3->fpdR ^= air3->fpdR << 13;
		air3->fpdR ^= air3->fpdR >> 17;
		air3->fpdR ^= air3->fpdR << 5;
		inputSampleR += (((double) air3->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	AIR3_URI,
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
