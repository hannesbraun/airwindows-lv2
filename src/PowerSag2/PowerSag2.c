#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define POWERSAG2_URI "https://hannesbraun.net/ns/lv2/airwindows/powersag2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	RANGE = 4,
	INV_WET = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* range;
	const float* invWet;

	double dL[16386];
	double dR[16386];
	double controlL;
	double controlR;
	int gcount;

	uint32_t fpdL;
	uint32_t fpdR;
} PowerSag2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PowerSag2* powerSag2 = (PowerSag2*) calloc(1, sizeof(PowerSag2));
	return (LV2_Handle) powerSag2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PowerSag2* powerSag2 = (PowerSag2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			powerSag2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			powerSag2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			powerSag2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			powerSag2->output[1] = (float*) data;
			break;
		case RANGE:
			powerSag2->range = (const float*) data;
			break;
		case INV_WET:
			powerSag2->invWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PowerSag2* powerSag2 = (PowerSag2*) instance;
	for (int count = 0; count < 16385; count++) {
		powerSag2->dL[count] = 0;
		powerSag2->dR[count] = 0;
	}
	powerSag2->controlL = 0;
	powerSag2->controlR = 0;
	powerSag2->gcount = 0;

	powerSag2->fpdL = 1.0;
	while (powerSag2->fpdL < 16386) powerSag2->fpdL = rand() * UINT32_MAX;
	powerSag2->fpdR = 1.0;
	while (powerSag2->fpdR < 16386) powerSag2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PowerSag2* powerSag2 = (PowerSag2*) instance;

	const float* in1 = powerSag2->input[0];
	const float* in2 = powerSag2->input[1];
	float* out1 = powerSag2->output[0];
	float* out2 = powerSag2->output[1];

	double depth = pow(*powerSag2->range, 4);
	int offset = (int) (depth * 16383) + 1;
	double wet = *powerSag2->invWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = powerSag2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = powerSag2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (powerSag2->gcount < 0 || powerSag2->gcount > 16384) {
			powerSag2->gcount = 16384;
		}
		powerSag2->dL[powerSag2->gcount] = fabs(inputSampleL);
		powerSag2->dR[powerSag2->gcount] = fabs(inputSampleR);
		powerSag2->controlL += powerSag2->dL[powerSag2->gcount];
		powerSag2->controlR += powerSag2->dR[powerSag2->gcount];
		powerSag2->controlL -= powerSag2->dL[powerSag2->gcount + offset - ((powerSag2->gcount + offset > 16384) ? 16384 : 0)];
		powerSag2->controlR -= powerSag2->dR[powerSag2->gcount + offset - ((powerSag2->gcount + offset > 16384) ? 16384 : 0)];
		powerSag2->gcount--;

		if (powerSag2->controlL > offset) powerSag2->controlL = offset;
		if (powerSag2->controlL < 0.0) powerSag2->controlL = 0.0;
		if (powerSag2->controlR > offset) powerSag2->controlR = offset;
		if (powerSag2->controlR < 0.0) powerSag2->controlR = 0.0;

		double burst = inputSampleL * (powerSag2->controlL / sqrt(offset));
		double clamp = inputSampleL / ((burst == 0.0) ? 1.0 : burst);

		if (clamp > 1.0) clamp = 1.0;
		if (clamp < 0.0) clamp = 0.0;
		inputSampleL *= clamp;
		double difference = drySampleL - inputSampleL;
		if (wet < 0.0) drySampleL *= (wet + 1.0);
		inputSampleL = drySampleL - (difference * wet);

		burst = inputSampleR * (powerSag2->controlR / sqrt(offset));
		clamp = inputSampleR / ((burst == 0.0) ? 1.0 : burst);

		if (clamp > 1.0) clamp = 1.0;
		if (clamp < 0.0) clamp = 0.0;
		inputSampleR *= clamp;
		difference = drySampleR - inputSampleR;
		if (wet < 0.0) drySampleR *= (wet + 1.0);
		inputSampleR = drySampleR - (difference * wet);

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		powerSag2->fpdL ^= powerSag2->fpdL << 13;
		powerSag2->fpdL ^= powerSag2->fpdL >> 17;
		powerSag2->fpdL ^= powerSag2->fpdL << 5;
		inputSampleL += (((double) powerSag2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		powerSag2->fpdR ^= powerSag2->fpdR << 13;
		powerSag2->fpdR ^= powerSag2->fpdR >> 17;
		powerSag2->fpdR ^= powerSag2->fpdR << 5;
		inputSampleR += (((double) powerSag2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POWERSAG2_URI,
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
