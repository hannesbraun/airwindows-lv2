#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define DESK_URI "https://hannesbraun.net/ns/lv2/airwindows/desk"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff

	double lastSampleL;
	double lastOutSampleL;
	double lastSlewL;
	double lastSampleR;
	double lastOutSampleR;
	double lastSlewR;
} Desk;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Desk* desk = (Desk*) calloc(1, sizeof(Desk));
	desk->sampleRate = rate;
	return (LV2_Handle) desk;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Desk* desk = (Desk*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			desk->input[0] = (const float*) data;
			break;
		case INPUT_R:
			desk->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			desk->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			desk->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Desk* desk = (Desk*) instance;

	desk->lastSampleL = 0.0;
	desk->lastOutSampleL = 0.0;
	desk->lastSlewL = 0.0;
	desk->lastSampleR = 0.0;
	desk->lastOutSampleR = 0.0;
	desk->lastSlewR = 0.0;
	desk->fpdL = 1.0;
	while (desk->fpdL < 16386) desk->fpdL = rand() * UINT32_MAX;
	desk->fpdR = 1.0;
	while (desk->fpdR < 16386) desk->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Desk* desk = (Desk*) instance;

	const float* in1 = desk->input[0];
	const float* in2 = desk->input[1];
	float* out1 = desk->output[0];
	float* out2 = desk->output[1];

	double gain = 0.135;
	double slewgain = 0.208;
	double prevslew = 0.333;
	double balanceB = 0.0001;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= desk->sampleRate;
	slewgain *= overallscale;
	prevslew *= overallscale;
	balanceB /= overallscale;
	double balanceA = 1.0 - balanceB;
	double slew;
	double bridgerectifier;
	double combsample;


	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = desk->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = desk->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		//begin L
		slew = inputSampleL - desk->lastSampleL;
		desk->lastSampleL = inputSampleL;
		//Set up direct reference for slew

		bridgerectifier = fabs(slew * slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier / slewgain;
		else slew = -(bridgerectifier / slewgain);

		inputSampleL = (desk->lastOutSampleL * balanceA) + (desk->lastSampleL * balanceB) + slew;
		//go from last slewed, but include some raw values
		desk->lastOutSampleL = inputSampleL;
		//Set up slewed reference

		combsample = fabs(drySampleL * desk->lastSampleL);
		if (combsample > 1.0) combsample = 1.0;
		//bailout for very high input gains
		inputSampleL -= (desk->lastSlewL * combsample * prevslew);
		desk->lastSlewL = slew;
		//slew interaction with previous slew

		inputSampleL *= gain;
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);

		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		//drive section
		inputSampleL /= gain;
		//end L

		//begin R
		slew = inputSampleR - desk->lastSampleR;
		desk->lastSampleR = inputSampleR;
		//Set up direct reference for slew

		bridgerectifier = fabs(slew * slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier / slewgain;
		else slew = -(bridgerectifier / slewgain);

		inputSampleR = (desk->lastOutSampleR * balanceA) + (desk->lastSampleR * balanceB) + slew;
		//go from last slewed, but include some raw values
		desk->lastOutSampleR = inputSampleR;
		//Set up slewed reference

		combsample = fabs(drySampleR * desk->lastSampleR);
		if (combsample > 1.0) combsample = 1.0;
		//bailout for very high input gains
		inputSampleR -= (desk->lastSlewR * combsample * prevslew);
		desk->lastSlewR = slew;
		//slew interaction with previous slew

		inputSampleR *= gain;
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);

		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		//drive section
		inputSampleR /= gain;
		//end R

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		desk->fpdL ^= desk->fpdL << 13;
		desk->fpdL ^= desk->fpdL >> 17;
		desk->fpdL ^= desk->fpdL << 5;
		inputSampleL += (((double)desk->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		desk->fpdR ^= desk->fpdR << 13;
		desk->fpdR ^= desk->fpdR >> 17;
		desk->fpdR ^= desk->fpdR << 5;
		inputSampleR += (((double)desk->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	DESK_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
