#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define AIR4_URI "https://hannesbraun.net/ns/lv2/airwindows/air4"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	AIR = 4,
	GND = 5,
	DARK_F = 6,
	RATIO = 7
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
	lastSL,
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
	lastSR,
	air_total
};

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* airParam;
	const float* gnd;
	const float* darkF;
	const float* ratio;

	double air[air_total];

	uint32_t fpdL;
	uint32_t fpdR;
} Air4;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Air4* air4 = (Air4*) calloc(1, sizeof(Air4));
	air4->sampleRate = rate;
	return (LV2_Handle) air4;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Air4* air4 = (Air4*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			air4->input[0] = (const float*) data;
			break;
		case INPUT_R:
			air4->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			air4->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			air4->output[1] = (float*) data;
			break;
		case AIR:
			air4->airParam = (const float*) data;
			break;
		case GND:
			air4->gnd = (const float*) data;
			break;
		case DARK_F:
			air4->darkF = (const float*) data;
			break;
		case RATIO:
			air4->ratio = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Air4* air4 = (Air4*) instance;

	for (int x = 0; x < air_total; x++) air4->air[x] = 0.0;

	air4->fpdL = 1.0;
	while (air4->fpdL < 16386) air4->fpdL = rand() * UINT32_MAX;
	air4->fpdR = 1.0;
	while (air4->fpdR < 16386) air4->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Air4* air4 = (Air4*) instance;

	const float* in1 = air4->input[0];
	const float* in2 = air4->input[1];
	float* out1 = air4->output[0];
	float* out2 = air4->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= air4->sampleRate;

	double airGain = *air4->airParam * 2.0;
	if (airGain > 1.0) airGain = pow(airGain, 3.0 + sqrt(overallscale));
	double gndGain = *air4->gnd * 2.0;
	double threshSinew = pow(*air4->darkF, 2) / overallscale;
	double depthSinew = *air4->ratio;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = air4->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = air4->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		air4->air[pvSL4] = air4->air[pvAL4] - air4->air[pvAL3];
		air4->air[pvSL3] = air4->air[pvAL3] - air4->air[pvAL2];
		air4->air[pvSL2] = air4->air[pvAL2] - air4->air[pvAL1];
		air4->air[pvSL1] = air4->air[pvAL1] - inputSampleL;

		air4->air[accSL3] = air4->air[pvSL4] - air4->air[pvSL3];
		air4->air[accSL2] = air4->air[pvSL3] - air4->air[pvSL2];
		air4->air[accSL1] = air4->air[pvSL2] - air4->air[pvSL1];

		air4->air[acc2SL2] = air4->air[accSL3] - air4->air[accSL2];
		air4->air[acc2SL1] = air4->air[accSL2] - air4->air[accSL1];

		air4->air[outAL] = -(air4->air[pvAL1] + air4->air[pvSL3] + air4->air[acc2SL2] - ((air4->air[acc2SL2] + air4->air[acc2SL1]) * 0.5));

		air4->air[gainAL] *= 0.5;
		air4->air[gainAL] += fabs(drySampleL - air4->air[outAL]) * 0.5;
		if (air4->air[gainAL] > 0.3 * sqrt(overallscale)) air4->air[gainAL] = 0.3 * sqrt(overallscale);
		air4->air[pvAL4] = air4->air[pvAL3];
		air4->air[pvAL3] = air4->air[pvAL2];
		air4->air[pvAL2] = air4->air[pvAL1];
		air4->air[pvAL1] = (air4->air[gainAL] * air4->air[outAL]) + drySampleL;

		double gnd = drySampleL - ((air4->air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
		double temp = (gnd + air4->air[gndavgL]) * 0.5;
		air4->air[gndavgL] = gnd;
		gnd = temp;
		inputSampleL = ((drySampleL - gnd) * airGain) + (gnd * gndGain);

		air4->air[pvSR4] = air4->air[pvAR4] - air4->air[pvAR3];
		air4->air[pvSR3] = air4->air[pvAR3] - air4->air[pvAR2];
		air4->air[pvSR2] = air4->air[pvAR2] - air4->air[pvAR1];
		air4->air[pvSR1] = air4->air[pvAR1] - inputSampleR;

		air4->air[accSR3] = air4->air[pvSR4] - air4->air[pvSR3];
		air4->air[accSR2] = air4->air[pvSR3] - air4->air[pvSR2];
		air4->air[accSR1] = air4->air[pvSR2] - air4->air[pvSR1];

		air4->air[acc2SR2] = air4->air[accSR3] - air4->air[accSR2];
		air4->air[acc2SR1] = air4->air[accSR2] - air4->air[accSR1];

		air4->air[outAR] = -(air4->air[pvAR1] + air4->air[pvSR3] + air4->air[acc2SR2] - ((air4->air[acc2SR2] + air4->air[acc2SR1]) * 0.5));

		air4->air[gainAR] *= 0.5;
		air4->air[gainAR] += fabs(drySampleR - air4->air[outAR]) * 0.5;
		if (air4->air[gainAR] > 0.3 * sqrt(overallscale)) air4->air[gainAR] = 0.3 * sqrt(overallscale);
		air4->air[pvAR4] = air4->air[pvAR3];
		air4->air[pvAR3] = air4->air[pvAR2];
		air4->air[pvAR2] = air4->air[pvAR1];
		air4->air[pvAR1] = (air4->air[gainAR] * air4->air[outAR]) + drySampleR;

		gnd = drySampleR - ((air4->air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
		temp = (gnd + air4->air[gndavgR]) * 0.5;
		air4->air[gndavgR] = gnd;
		gnd = temp;
		inputSampleR = ((drySampleR - gnd) * airGain) + (gnd * gndGain);

		temp = inputSampleL;
		if (temp > 1.0) temp = 1.0;
		if (temp < -1.0) temp = -1.0;
		double sinew = threshSinew * cos(air4->air[lastSL] * air4->air[lastSL]);
		if (temp - air4->air[lastSL] > sinew) temp = air4->air[lastSL] + sinew;
		if (-(temp - air4->air[lastSL]) > sinew) temp = air4->air[lastSL] - sinew;
		air4->air[lastSL] = temp;
		if (air4->air[lastSL] > 1.0) air4->air[lastSL] = 1.0;
		if (air4->air[lastSL] < -1.0) air4->air[lastSL] = -1.0;
		inputSampleL = (inputSampleL * (1.0 - depthSinew)) + (air4->air[lastSL] * depthSinew);
		temp = inputSampleR;
		if (temp > 1.0) temp = 1.0;
		if (temp < -1.0) temp = -1.0;
		sinew = threshSinew * cos(air4->air[lastSR] * air4->air[lastSR]);
		if (temp - air4->air[lastSR] > sinew) temp = air4->air[lastSR] + sinew;
		if (-(temp - air4->air[lastSR]) > sinew) temp = air4->air[lastSR] - sinew;
		air4->air[lastSR] = temp;
		if (air4->air[lastSR] > 1.0) air4->air[lastSR] = 1.0;
		if (air4->air[lastSR] < -1.0) air4->air[lastSR] = -1.0;
		inputSampleR = (inputSampleR * (1.0 - depthSinew)) + (air4->air[lastSR] * depthSinew);
		// run Sinew to stop excess slews, but run a dry/wet to allow a range of brights

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		air4->fpdL ^= air4->fpdL << 13;
		air4->fpdL ^= air4->fpdL >> 17;
		air4->fpdL ^= air4->fpdL << 5;
		inputSampleL += (((double) air4->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		air4->fpdR ^= air4->fpdR << 13;
		air4->fpdR ^= air4->fpdR >> 17;
		air4->fpdR ^= air4->fpdR << 5;
		inputSampleR += (((double) air4->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	AIR4_URI,
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
