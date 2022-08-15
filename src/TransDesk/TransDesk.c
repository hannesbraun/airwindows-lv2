#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TRANSDESK_URI "https://hannesbraun.net/ns/lv2/airwindows/transdesk"

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
	// default stuff
	int gcount;

	double dL[20];
	double controlL;
	double lastSampleL;
	double lastOutSampleL;
	double lastSlewL;

	double dR[20];
	double controlR;
	double lastSampleR;
	double lastOutSampleR;
	double lastSlewR;
} TransDesk;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	TransDesk* transDesk = (TransDesk*) calloc(1, sizeof(TransDesk));
	transDesk->sampleRate = rate;
	return (LV2_Handle) transDesk;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	TransDesk* transDesk = (TransDesk*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			transDesk->input[0] = (const float*) data;
			break;
		case INPUT_R:
			transDesk->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			transDesk->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			transDesk->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	TransDesk* transDesk = (TransDesk*) instance;

	for (int count = 0; count < 19; count++) {
		transDesk->dL[count] = 0;
		transDesk->dR[count] = 0;
	}
	transDesk->gcount = 0;

	transDesk->controlL = 0;
	transDesk->lastSampleL = 0.0;
	transDesk->lastOutSampleL = 0.0;
	transDesk->lastSlewL = 0.0;

	transDesk->controlR = 0;
	transDesk->lastSampleR = 0.0;
	transDesk->lastOutSampleR = 0.0;
	transDesk->lastSlewR = 0.0;

	transDesk->fpdL = 1.0;
	while (transDesk->fpdL < 16386) transDesk->fpdL = rand() * UINT32_MAX;
	transDesk->fpdR = 1.0;
	while (transDesk->fpdR < 16386) transDesk->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	TransDesk* transDesk = (TransDesk*) instance;

	const float* in1 = transDesk->input[0];
	const float* in2 = transDesk->input[1];
	float* out1 = transDesk->output[0];
	float* out2 = transDesk->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= transDesk->sampleRate;

	double intensity = 0.02198359;
	double depthA = 3.0;
	int offsetA = (int) (depthA * overallscale);
	if (offsetA < 1) offsetA = 1;
	if (offsetA > 8) offsetA = 8;

	double clamp;
	double thickness;
	double out;

	double gain = 0.130;
	double slewgain = 0.197;
	double prevslew = 0.255;
	double balanceB = 0.0001;
	slewgain *= overallscale;
	prevslew *= overallscale;
	balanceB /= overallscale;
	double balanceA = 1.0 - balanceB;
	double slew;
	double bridgerectifier;
	double combSample;

	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = transDesk->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = transDesk->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		if (transDesk->gcount < 0 || transDesk->gcount > 9) {
			transDesk->gcount = 9;
		}

		// begin L
		transDesk->dL[transDesk->gcount + 9] = transDesk->dL[transDesk->gcount] = fabs(inputSampleL) * intensity;
		transDesk->controlL += (transDesk->dL[transDesk->gcount] / offsetA);
		transDesk->controlL -= (transDesk->dL[transDesk->gcount + offsetA] / offsetA);
		transDesk->controlL -= 0.000001;
		clamp = 1;
		if (transDesk->controlL < 0) {
			transDesk->controlL = 0;
		}
		if (transDesk->controlL > 1) {
			clamp -= (transDesk->controlL - 1);
			transDesk->controlL = 1;
		}
		if (clamp < 0.5) {
			clamp = 0.5;
		}
		// control = 0 to 1
		thickness = ((1.0 - transDesk->controlL) * 2.0) - 1.0;
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
		slew = inputSampleL - transDesk->lastSampleL;
		transDesk->lastSampleL = inputSampleL;
		// Set up direct reference for slew
		bridgerectifier = fabs(slew * slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier / slewgain;
		else slew = -(bridgerectifier / slewgain);
		inputSampleL = (transDesk->lastOutSampleL * balanceA) + (transDesk->lastSampleL * balanceB) + slew;
		// go from last slewed, but include some raw values
		transDesk->lastOutSampleL = inputSampleL;
		// Set up slewed reference
		combSample = fabs(drySampleL * transDesk->lastSampleL);
		if (combSample > 1.0) combSample = 1.0;
		// bailout for very high input gains
		inputSampleL -= (transDesk->lastSlewL * combSample * prevslew);
		transDesk->lastSlewL = slew;
		// slew interaction with previous slew
		inputSampleL *= gain;
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		// drive section
		inputSampleL /= gain;
		// end of Desk section
		// end L

		// begin R
		transDesk->dR[transDesk->gcount + 9] = transDesk->dR[transDesk->gcount] = fabs(inputSampleR) * intensity;
		transDesk->controlR += (transDesk->dR[transDesk->gcount] / offsetA);
		transDesk->controlR -= (transDesk->dR[transDesk->gcount + offsetA] / offsetA);
		transDesk->controlR -= 0.000001;
		clamp = 1;
		if (transDesk->controlR < 0) {
			transDesk->controlR = 0;
		}
		if (transDesk->controlR > 1) {
			clamp -= (transDesk->controlR - 1);
			transDesk->controlR = 1;
		}
		if (clamp < 0.5) {
			clamp = 0.5;
		}
		// control = 0 to 1
		thickness = ((1.0 - transDesk->controlR) * 2.0) - 1.0;
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
		slew = inputSampleR - transDesk->lastSampleR;
		transDesk->lastSampleR = inputSampleR;
		// Set up direct reference for slew
		bridgerectifier = fabs(slew * slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier / slewgain;
		else slew = -(bridgerectifier / slewgain);
		inputSampleR = (transDesk->lastOutSampleR * balanceA) + (transDesk->lastSampleR * balanceB) + slew;
		// go from last slewed, but include some raw values
		transDesk->lastOutSampleR = inputSampleR;
		// Set up slewed reference
		combSample = fabs(drySampleR * transDesk->lastSampleR);
		if (combSample > 1.0) combSample = 1.0;
		// bailout for very high input gains
		inputSampleR -= (transDesk->lastSlewR * combSample * prevslew);
		transDesk->lastSlewR = slew;
		// slew interaction with previous slew
		inputSampleR *= gain;
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		// drive section
		inputSampleR /= gain;
		// end of Desk section
		// end R

		transDesk->gcount--;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		transDesk->fpdL ^= transDesk->fpdL << 13;
		transDesk->fpdL ^= transDesk->fpdL >> 17;
		transDesk->fpdL ^= transDesk->fpdL << 5;
		inputSampleL += (((double) transDesk->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		transDesk->fpdR ^= transDesk->fpdR << 13;
		transDesk->fpdR ^= transDesk->fpdR >> 17;
		transDesk->fpdR ^= transDesk->fpdR << 5;
		inputSampleR += (((double) transDesk->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TRANSDESK_URI,
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
