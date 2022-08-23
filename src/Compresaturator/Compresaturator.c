#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define COMPRESATURATOR_URI "https://hannesbraun.net/ns/lv2/airwindows/compresaturator"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	CLAMP = 5,
	EXPAND = 6,
	OUTPUT = 7,
	DRYWET = 8
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* clamp;
	const float* expand;
	const float* outputGain;
	const float* drywet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	int dCount;

	float dL[11000];
	int lastWidthL;
	double padFactorL;

	float dR[11000];
	int lastWidthR;
	double padFactorR;
} Compresaturator;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Compresaturator* compresaturator = (Compresaturator*) calloc(1, sizeof(Compresaturator));
	return (LV2_Handle) compresaturator;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Compresaturator* compresaturator = (Compresaturator*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			compresaturator->input[0] = (const float*) data;
			break;
		case INPUT_R:
			compresaturator->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			compresaturator->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			compresaturator->output[1] = (float*) data;
			break;
		case DRIVE:
			compresaturator->drive = (const float*) data;
			break;
		case CLAMP:
			compresaturator->clamp = (const float*) data;
			break;
		case EXPAND:
			compresaturator->expand = (const float*) data;
			break;
		case OUTPUT:
			compresaturator->outputGain = (const float*) data;
			break;
		case DRYWET:
			compresaturator->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Compresaturator* compresaturator = (Compresaturator*) instance;
	for (int count = 0; count < 10990; count++) {
		compresaturator->dL[count] = 0.0;
		compresaturator->dR[count] = 0.0;
	}
	compresaturator->dCount = 0;
	compresaturator->lastWidthL = 500;
	compresaturator->padFactorL = 0;
	compresaturator->lastWidthR = 500;
	compresaturator->padFactorR = 0;

	compresaturator->fpdL = 1.0;
	while (compresaturator->fpdL < 16386) compresaturator->fpdL = rand() * UINT32_MAX;
	compresaturator->fpdR = 1.0;
	while (compresaturator->fpdR < 16386) compresaturator->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Compresaturator* compresaturator = (Compresaturator*) instance;

	const float* in1 = compresaturator->input[0];
	const float* in2 = compresaturator->input[1];
	float* out1 = compresaturator->output[0];
	float* out2 = compresaturator->output[1];

	double inputgain = pow(10.0, *compresaturator->drive / 20.0);
	double satComp = *compresaturator->clamp * 0.01 * 2.0;
	int widestRange = *compresaturator->expand * sqrt(*compresaturator->expand / 5000.0);
	if (widestRange < 50) widestRange = 50;
	satComp += (((double) widestRange / 3000.0) * satComp);
	// set the max wideness of comp zone, minimum range boosted (too much?)
	double output = *compresaturator->outputGain;
	double wet = *compresaturator->drywet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = compresaturator->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = compresaturator->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (compresaturator->dCount < 1 || compresaturator->dCount > 5000) {
			compresaturator->dCount = 5000;
		}

		// begin drive L
		double temp = inputSampleL;
		double variSpeed = 1.0 + ((compresaturator->padFactorL / compresaturator->lastWidthL) * satComp);
		if (variSpeed < 1.0) variSpeed = 1.0;
		double totalgain = inputgain / variSpeed;
		if (totalgain != 1.0) {
			inputSampleL *= totalgain;
			if (totalgain < 1.0) {
				temp *= totalgain;
				// no boosting beyond unity please
			}
		}

		double bridgerectifier = fabs(inputSampleL);
		double overspill = 0;
		int targetWidth = widestRange;
		// we now have defaults and an absolute input value to work with
		if (bridgerectifier < 0.01) compresaturator->padFactorL *= 0.9999;
		// in silences we bring back padFactor if it got out of hand
		if (bridgerectifier > 1.57079633) {
			bridgerectifier = 1.57079633;
			targetWidth = 8;
		}
		// if our output's gone beyond saturating to distorting, we begin chasing the
		// buffer size smaller. Anytime we don't have that, we expand (smoothest sound, only adding to an increasingly subdivided buffer)

		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) {
			inputSampleL = bridgerectifier;
			overspill = temp - bridgerectifier;
		}

		if (inputSampleL < 0) {
			inputSampleL = -bridgerectifier;
			overspill = (-temp) - bridgerectifier;
		}
		// drive section L

		// begin drive R
		temp = inputSampleR;
		variSpeed = 1.0 + ((compresaturator->padFactorR / compresaturator->lastWidthR) * satComp);
		if (variSpeed < 1.0) variSpeed = 1.0;
		totalgain = inputgain / variSpeed;
		if (totalgain != 1.0) {
			inputSampleR *= totalgain;
			if (totalgain < 1.0) {
				temp *= totalgain;
				// no boosting beyond unity please
			}
		}

		bridgerectifier = fabs(inputSampleR);
		overspill = 0;
		targetWidth = widestRange;
		// we now have defaults and an absolute input value to work with
		if (bridgerectifier < 0.01) compresaturator->padFactorR *= 0.9999;
		// in silences we bring back padFactor if it got out of hand
		if (bridgerectifier > 1.57079633) {
			bridgerectifier = 1.57079633;
			targetWidth = 8;
		}
		// if our output's gone beyond saturating to distorting, we begin chasing the
		// buffer size smaller. Anytime we don't have that, we expand (smoothest sound, only adding to an increasingly subdivided buffer)

		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) {
			inputSampleR = bridgerectifier;
			overspill = temp - bridgerectifier;
		}

		if (inputSampleR < 0) {
			inputSampleR = -bridgerectifier;
			overspill = (-temp) - bridgerectifier;
		}
		// drive section R

		compresaturator->dL[compresaturator->dCount + 5000] = compresaturator->dL[compresaturator->dCount] = overspill * satComp;
		compresaturator->dR[compresaturator->dCount + 5000] = compresaturator->dR[compresaturator->dCount] = overspill * satComp;
		compresaturator->dCount--;
		// we now have a big buffer to draw from, which is always positive amount of overspill

		// begin pad L
		compresaturator->padFactorL += compresaturator->dL[compresaturator->dCount];
		double randy = ((double) compresaturator->fpdL / UINT32_MAX);
		if ((targetWidth * randy) > compresaturator->lastWidthL) {
			// we are expanding the buffer so we don't remove this trailing sample
			compresaturator->lastWidthL += 1;
		} else {
			compresaturator->padFactorL -= compresaturator->dL[compresaturator->dCount + compresaturator->lastWidthL];
			// zero change, or target is smaller and we are shrinking
			if (targetWidth < compresaturator->lastWidthL) {
				compresaturator->lastWidthL -= 1;
				if (compresaturator->lastWidthL < 2) compresaturator->lastWidthL = 2;
				// sanity check as randy can give us target zero
				compresaturator->padFactorL -= compresaturator->dL[compresaturator->dCount + compresaturator->lastWidthL];
			}
		}
		// variable attack/release speed more rapid as comp intensity increases
		// implemented in a way where we're repeatedly not altering the buffer as it expands, which makes the comp artifacts smoother
		if (compresaturator->padFactorL < 0) compresaturator->padFactorL = 0;
		// end pad L

		// begin pad R
		compresaturator->padFactorR += compresaturator->dR[compresaturator->dCount];
		randy = ((double) compresaturator->fpdR / UINT32_MAX);
		if ((targetWidth * randy) > compresaturator->lastWidthR) {
			// we are expanding the buffer so we don't remove this trailing sample
			compresaturator->lastWidthR += 1;
		} else {
			compresaturator->padFactorR -= compresaturator->dR[compresaturator->dCount + compresaturator->lastWidthR];
			// zero change, or target is smaller and we are shrinking
			if (targetWidth < compresaturator->lastWidthR) {
				compresaturator->lastWidthR -= 1;
				if (compresaturator->lastWidthR < 2) compresaturator->lastWidthR = 2;
				// sanity check as randy can give us target zero
				compresaturator->padFactorR -= compresaturator->dR[compresaturator->dCount + compresaturator->lastWidthR];
			}
		}
		// variable attack/release speed more rapid as comp intensity increases
		// implemented in a way where we're repeatedly not altering the buffer as it expands, which makes the comp artifacts smoother
		if (compresaturator->padFactorR < 0) compresaturator->padFactorR = 0;
		// end pad R

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}

		if (wet < 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		compresaturator->fpdL ^= compresaturator->fpdL << 13;
		compresaturator->fpdL ^= compresaturator->fpdL >> 17;
		compresaturator->fpdL ^= compresaturator->fpdL << 5;
		inputSampleL += (int32_t) compresaturator->fpdL * 5.960464655174751e-36L * pow(2, expon + 62);
		frexpf((float) inputSampleR, &expon);
		compresaturator->fpdR ^= compresaturator->fpdR << 13;
		compresaturator->fpdR ^= compresaturator->fpdR >> 17;
		compresaturator->fpdR ^= compresaturator->fpdR << 5;
		inputSampleR += (int32_t) compresaturator->fpdR * 5.960464655174751e-36L * pow(2, expon + 62);
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
	COMPRESATURATOR_URI,
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
