#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PHASENUDGE_URI "https://hannesbraun.net/ns/lv2/airwindows/phasenudge"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	PHASE_NUDGE = 4,
	DRYWET = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* phaseNudge;
	const float* dryWet;

	double dL[1503];
	double dR[1503];
	int one, maxdelay;

	uint32_t fpdL;
	uint32_t fpdR;
} PhaseNudge;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PhaseNudge* phaseNudge = (PhaseNudge*) calloc(1, sizeof(PhaseNudge));
	return (LV2_Handle) phaseNudge;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PhaseNudge* phaseNudge = (PhaseNudge*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			phaseNudge->input[0] = (const float*) data;
			break;
		case INPUT_R:
			phaseNudge->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			phaseNudge->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			phaseNudge->output[1] = (float*) data;
			break;
		case PHASE_NUDGE:
			phaseNudge->phaseNudge = (const float*) data;
			break;
		case DRYWET:
			phaseNudge->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PhaseNudge* phaseNudge = (PhaseNudge*) instance;
	for (int count = 0; count < 1502; count++) {
		phaseNudge->dL[count] = 0.0;
		phaseNudge->dR[count] = 0.0;
	}
	phaseNudge->one = 1;
	phaseNudge->maxdelay = 9001;
	phaseNudge->fpdL = 1.0;
	while (phaseNudge->fpdL < 16386) phaseNudge->fpdL = rand() * UINT32_MAX;
	phaseNudge->fpdR = 1.0;
	while (phaseNudge->fpdR < 16386) phaseNudge->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PhaseNudge* phaseNudge = (PhaseNudge*) instance;

	const float* in1 = phaseNudge->input[0];
	const float* in2 = phaseNudge->input[1];
	float* out1 = phaseNudge->output[0];
	float* out2 = phaseNudge->output[1];

	int allpasstemp;
	double outallpass = 0.618033988749894848204586; // golden ratio!
	// if you see 0.6180 it's not a wild stretch to wonder whether you are working with a constant
	int maxdelayTarget = (int) (pow(*phaseNudge->phaseNudge, 3) * 1501.0);
	double wet = *phaseNudge->dryWet;
	// removed extra dry variable
	double bridgerectifier;

	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = phaseNudge->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = phaseNudge->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		inputSampleL /= 4.0;
		inputSampleR /= 4.0;

		bridgerectifier = fabs(inputSampleL);
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;

		bridgerectifier = fabs(inputSampleR);
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;

		if (abs(phaseNudge->maxdelay - maxdelayTarget) > 1500) phaseNudge->maxdelay = maxdelayTarget;

		if (phaseNudge->maxdelay < maxdelayTarget) {
			phaseNudge->maxdelay++;
			phaseNudge->dL[phaseNudge->maxdelay] = (phaseNudge->dL[0] + phaseNudge->dL[phaseNudge->maxdelay - 1]) / 2.0;
			phaseNudge->dR[phaseNudge->maxdelay] = (phaseNudge->dR[0] + phaseNudge->dR[phaseNudge->maxdelay - 1]) / 2.0;
		}

		if (phaseNudge->maxdelay > maxdelayTarget) {
			phaseNudge->maxdelay--;
			phaseNudge->dL[phaseNudge->maxdelay] = (phaseNudge->dL[0] + phaseNudge->dL[phaseNudge->maxdelay]) / 2.0;
			phaseNudge->dR[phaseNudge->maxdelay] = (phaseNudge->dR[0] + phaseNudge->dR[phaseNudge->maxdelay]) / 2.0;
		}

		allpasstemp = phaseNudge->one - 1;

		if (allpasstemp < 0 || allpasstemp > phaseNudge->maxdelay) allpasstemp = phaseNudge->maxdelay;

		inputSampleL -= phaseNudge->dL[allpasstemp] * outallpass;
		inputSampleR -= phaseNudge->dR[allpasstemp] * outallpass;
		phaseNudge->dL[phaseNudge->one] = inputSampleL;
		phaseNudge->dR[phaseNudge->one] = inputSampleR;
		inputSampleL *= outallpass;
		inputSampleR *= outallpass;
		phaseNudge->one--;
		if (phaseNudge->one < 0 || phaseNudge->one > phaseNudge->maxdelay) {
			phaseNudge->one = phaseNudge->maxdelay;
		}
		inputSampleL += (phaseNudge->dL[phaseNudge->one]);
		inputSampleR += (phaseNudge->dR[phaseNudge->one]);

		bridgerectifier = fabs(inputSampleL);
		bridgerectifier = 1.0 - cos(bridgerectifier);
		if (inputSampleL > 0) inputSampleL -= bridgerectifier;
		else inputSampleL += bridgerectifier;

		bridgerectifier = fabs(inputSampleR);
		bridgerectifier = 1.0 - cos(bridgerectifier);
		if (inputSampleR > 0) inputSampleR -= bridgerectifier;
		else inputSampleR += bridgerectifier;

		inputSampleL *= 4.0;
		inputSampleR *= 4.0;

		if (wet < 1.0) {
			inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		phaseNudge->fpdL ^= phaseNudge->fpdL << 13;
		phaseNudge->fpdL ^= phaseNudge->fpdL >> 17;
		phaseNudge->fpdL ^= phaseNudge->fpdL << 5;
		inputSampleL += (((double) phaseNudge->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		phaseNudge->fpdR ^= phaseNudge->fpdR << 13;
		phaseNudge->fpdR ^= phaseNudge->fpdR >> 17;
		phaseNudge->fpdR ^= phaseNudge->fpdR << 5;
		inputSampleR += (((double) phaseNudge->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PHASENUDGE_URI,
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
