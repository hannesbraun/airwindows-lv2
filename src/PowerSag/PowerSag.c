#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define POWERSAG_URI "https://hannesbraun.net/ns/lv2/airwindows/powersag"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DEPTH = 4,
	SPEED = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* depth;
	const float* speed;

	// default stuff
	uint32_t fpdL;
	uint32_t fpdR;

	double dL[9000];
	double dR[9000];
	double controlL;
	double controlR;
	int gcount;
} PowerSag;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PowerSag* powerSag = (PowerSag*) calloc(1, sizeof(PowerSag));
	return (LV2_Handle) powerSag;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PowerSag* powerSag = (PowerSag*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			powerSag->input[0] = (const float*) data;
			break;
		case INPUT_R:
			powerSag->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			powerSag->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			powerSag->output[1] = (float*) data;
			break;
		case DEPTH:
			powerSag->depth = (const float*) data;
			break;
		case SPEED:
			powerSag->speed = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PowerSag* powerSag = (PowerSag*) instance;
	for (int count = 0; count < 8999; count++) {
		powerSag->dL[count] = 0;
		powerSag->dR[count] = 0;
	}
	powerSag->controlL = 0;
	powerSag->controlR = 0;
	powerSag->gcount = 0;
	powerSag->fpdL = 1.0;
	while (powerSag->fpdL < 16386) powerSag->fpdL = rand() * UINT32_MAX;
	powerSag->fpdR = 1.0;
	while (powerSag->fpdR < 16386) powerSag->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PowerSag* powerSag = (PowerSag*) instance;

	const float* in1 = powerSag->input[0];
	const float* in2 = powerSag->input[1];
	float* out1 = powerSag->output[0];
	float* out2 = powerSag->output[1];

	double intensity = pow(*powerSag->depth, 5) * 80.0;
	double depthA = pow(*powerSag->speed, 2);
	int offsetA = (int) (depthA * 3900) + 1;
	double clamp;
	double thickness;
	double out;
	double bridgerectifier;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = powerSag->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = powerSag->fpdR * 1.18e-17;

		if (powerSag->gcount < 0 || powerSag->gcount > 4000) {
			powerSag->gcount = 4000;
		}

		// doing L
		powerSag->dL[powerSag->gcount + 4000] = powerSag->dL[powerSag->gcount] = fabs(inputSampleL) * intensity;
		powerSag->controlL += (powerSag->dL[powerSag->gcount] / offsetA);
		powerSag->controlL -= (powerSag->dL[powerSag->gcount + offsetA] / offsetA);
		powerSag->controlL -= 0.000001;
		clamp = 1;
		if (powerSag->controlL < 0) {
			powerSag->controlL = 0;
		}
		if (powerSag->controlL > 1) {
			clamp -= (powerSag->controlL - 1);
			powerSag->controlL = 1;
		}
		if (clamp < 0.5) {
			clamp = 0.5;
		}
		thickness = ((1.0 - powerSag->controlL) * 2.0) - 1.0;
		out = fabs(thickness);
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		// max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1 - cos(bridgerectifier);
		// produce either boosted or starved version
		if (inputSampleL > 0) inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
		else inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
		// blend according to density control
		inputSampleL *= clamp;
		// end L

		// doing R
		powerSag->dR[powerSag->gcount + 4000] = powerSag->dR[powerSag->gcount] = fabs(inputSampleR) * intensity;
		powerSag->controlR += (powerSag->dR[powerSag->gcount] / offsetA);
		powerSag->controlR -= (powerSag->dR[powerSag->gcount + offsetA] / offsetA);
		powerSag->controlR -= 0.000001;
		clamp = 1;
		if (powerSag->controlR < 0) {
			powerSag->controlR = 0;
		}
		if (powerSag->controlR > 1) {
			clamp -= (powerSag->controlR - 1);
			powerSag->controlR = 1;
		}
		if (clamp < 0.5) {
			clamp = 0.5;
		}
		thickness = ((1.0 - powerSag->controlR) * 2.0) - 1.0;
		out = fabs(thickness);
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		// max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1 - cos(bridgerectifier);
		// produce either boosted or starved version
		if (inputSampleR > 0) inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
		else inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
		// blend according to density control
		inputSampleR *= clamp;
		// end R

		powerSag->gcount--;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		powerSag->fpdL ^= powerSag->fpdL << 13;
		powerSag->fpdL ^= powerSag->fpdL >> 17;
		powerSag->fpdL ^= powerSag->fpdL << 5;
		inputSampleL += (((double) powerSag->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		powerSag->fpdR ^= powerSag->fpdR << 13;
		powerSag->fpdR ^= powerSag->fpdR >> 17;
		powerSag->fpdR ^= powerSag->fpdR << 5;
		inputSampleR += (((double) powerSag->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POWERSAG_URI,
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
