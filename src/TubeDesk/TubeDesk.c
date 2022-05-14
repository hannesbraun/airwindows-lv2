#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TUBEDESK_URI "https://hannesbraun.net/ns/lv2/airwindows/tubedesk"

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
	int gcount;

	double dL[5000];
	double controlL;
	double lastSampleL;
	double lastOutSampleL;
	double lastSlewL;

	double dR[5000];
	double controlR;
	double lastSampleR;
	double lastOutSampleR;
	double lastSlewR;
} TubeDesk;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	TubeDesk* tubeDesk = (TubeDesk*) calloc(1, sizeof(TubeDesk));
	tubeDesk->sampleRate = rate;
	return (LV2_Handle) tubeDesk;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	TubeDesk* tubeDesk = (TubeDesk*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			tubeDesk->input[0] = (const float*) data;
			break;
		case INPUT_R:
			tubeDesk->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			tubeDesk->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			tubeDesk->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	TubeDesk* tubeDesk = (TubeDesk*) instance;

	for (int count = 0; count < 4999; count++) {
		tubeDesk->dL[count] = 0;
		tubeDesk->dR[count] = 0;
	}
	tubeDesk->gcount = 0;

	tubeDesk->controlL = 0;
	tubeDesk->lastSampleL = 0.0;
	tubeDesk->lastOutSampleL = 0.0;
	tubeDesk->lastSlewL = 0.0;

	tubeDesk->controlR = 0;
	tubeDesk->lastSampleR = 0.0;
	tubeDesk->lastOutSampleR = 0.0;
	tubeDesk->lastSlewR = 0.0;

	tubeDesk->fpdL = 1.0;
	while (tubeDesk->fpdL < 16386) tubeDesk->fpdL = rand() * UINT32_MAX;
	tubeDesk->fpdR = 1.0;
	while (tubeDesk->fpdR < 16386) tubeDesk->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	TubeDesk* tubeDesk = (TubeDesk*) instance;

	const float* in1 = tubeDesk->input[0];
	const float* in2 = tubeDesk->input[1];
	float* out1 = tubeDesk->output[0];
	float* out2 = tubeDesk->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= tubeDesk->sampleRate;

	double intensity = 0.4384938;
	double depthA = 549.0;
	int offsetA = (int)(depthA * overallscale);
	if (offsetA < 1) offsetA = 1;
	if (offsetA > 2440) offsetA = 2440;

	double clamp;
	double thickness;
	double out;
	double gain = 0.5;
	double slewgain = 0.128;
	double prevslew = 0.105;
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
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = tubeDesk->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = tubeDesk->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;


		if (tubeDesk->gcount < 0 || tubeDesk->gcount > 2450) {
			tubeDesk->gcount = 2450;
		}

		//begin L
		tubeDesk->dL[tubeDesk->gcount + 2450] = tubeDesk->dL[tubeDesk->gcount] = fabs(inputSampleL) * intensity;
		tubeDesk->controlL += (tubeDesk->dL[tubeDesk->gcount] / offsetA);
		tubeDesk->controlL -= (tubeDesk->dL[tubeDesk->gcount + offsetA] / offsetA);
		tubeDesk->controlL -= 0.000001;
		clamp = 1;
		if (tubeDesk->controlL < 0) {
			tubeDesk->controlL = 0;
		}
		if (tubeDesk->controlL > 1) {
			clamp -= (tubeDesk->controlL - 1);
			tubeDesk->controlL = 1;
		}
		if (clamp < 0.5) {
			clamp = 0.5;
		}
		//control = 0 to 1
		thickness = ((1.0 - tubeDesk->controlL) * 2.0) - 1.0;
		out = fabs(thickness);
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1 - cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleL > 0) inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
		else inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
		//blend according to density control
		inputSampleL *= clamp;
		slew = inputSampleL - tubeDesk->lastSampleL;
		tubeDesk->lastSampleL = inputSampleL;
		//Set up direct reference for slew
		bridgerectifier = fabs(slew * slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier / slewgain;
		else slew = -(bridgerectifier / slewgain);
		inputSampleL = (tubeDesk->lastOutSampleL * balanceA) + (tubeDesk->lastSampleL * balanceB) + slew;
		//go from last slewed, but include some raw values
		tubeDesk->lastOutSampleL = inputSampleL;
		//Set up slewed reference
		combSample = fabs(drySampleL * tubeDesk->lastSampleL);
		if (combSample > 1.0) combSample = 1.0;
		//bailout for very high input gains
		inputSampleL -= (tubeDesk->lastSlewL * combSample * prevslew);
		tubeDesk->lastSlewL = slew;
		//slew interaction with previous slew
		inputSampleL *= gain;
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		//drive section
		inputSampleL /= gain;
		//end of Desk section
		//end L

		//begin R
		tubeDesk->dR[tubeDesk->gcount + 2450] = tubeDesk->dR[tubeDesk->gcount] = fabs(inputSampleR) * intensity;
		tubeDesk->controlR += (tubeDesk->dR[tubeDesk->gcount] / offsetA);
		tubeDesk->controlR -= (tubeDesk->dR[tubeDesk->gcount + offsetA] / offsetA);
		tubeDesk->controlR -= 0.000001;
		clamp = 1;
		if (tubeDesk->controlR < 0) {
			tubeDesk->controlR = 0;
		}
		if (tubeDesk->controlR > 1) {
			clamp -= (tubeDesk->controlR - 1);
			tubeDesk->controlR = 1;
		}
		if (clamp < 0.5) {
			clamp = 0.5;
		}
		//control = 0 to 1
		thickness = ((1.0 - tubeDesk->controlR) * 2.0) - 1.0;
		out = fabs(thickness);
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1 - cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleR > 0) inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
		else inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
		//blend according to density control
		inputSampleR *= clamp;
		slew = inputSampleR - tubeDesk->lastSampleR;
		tubeDesk->lastSampleR = inputSampleR;
		//Set up direct reference for slew
		bridgerectifier = fabs(slew * slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier / slewgain;
		else slew = -(bridgerectifier / slewgain);
		inputSampleR = (tubeDesk->lastOutSampleR * balanceA) + (tubeDesk->lastSampleR * balanceB) + slew;
		//go from last slewed, but include some raw values
		tubeDesk->lastOutSampleR = inputSampleR;
		//Set up slewed reference
		combSample = fabs(drySampleR * tubeDesk->lastSampleR);
		if (combSample > 1.0) combSample = 1.0;
		//bailout for very high input gains
		inputSampleR -= (tubeDesk->lastSlewR * combSample * prevslew);
		tubeDesk->lastSlewR = slew;
		//slew interaction with previous slew
		inputSampleR *= gain;
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		//drive section
		inputSampleR /= gain;
		//end of Desk section
		//end R

		tubeDesk->gcount--;

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		tubeDesk->fpdL ^= tubeDesk->fpdL << 13;
		tubeDesk->fpdL ^= tubeDesk->fpdL >> 17;
		tubeDesk->fpdL ^= tubeDesk->fpdL << 5;
		inputSampleL += (((double)tubeDesk->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		tubeDesk->fpdR ^= tubeDesk->fpdR << 13;
		tubeDesk->fpdR ^= tubeDesk->fpdR >> 17;
		tubeDesk->fpdR ^= tubeDesk->fpdR << 5;
		inputSampleR += (((double)tubeDesk->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TUBEDESK_URI,
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
