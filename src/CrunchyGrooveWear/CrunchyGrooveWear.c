#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CRUNCHYGROOVEWEAR_URI "https://hannesbraun.net/ns/lv2/airwindows/crunchygroovewear"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FREQ = 4,
	APPLY = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* freq;
	const float* apply;

	uint32_t fpdL;
	uint32_t fpdR;

	double aMidL[21];
	double aMidPrevL;
	double bMidL[21];
	double bMidPrevL;
	double cMidL[21];
	double cMidPrevL;
	double dMidL[21];
	double dMidPrevL;

	double aMidR[21];
	double aMidPrevR;
	double bMidR[21];
	double bMidPrevR;
	double cMidR[21];
	double cMidPrevR;
	double dMidR[21];
	double dMidPrevR;

	double fMid[21];
} CrunchyGrooveWear;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	CrunchyGrooveWear* crunchyGrooveWear = (CrunchyGrooveWear*) calloc(1, sizeof(CrunchyGrooveWear));
	return (LV2_Handle) crunchyGrooveWear;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	CrunchyGrooveWear* crunchyGrooveWear = (CrunchyGrooveWear*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			crunchyGrooveWear->input[0] = (const float*) data;
			break;
		case INPUT_R:
			crunchyGrooveWear->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			crunchyGrooveWear->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			crunchyGrooveWear->output[1] = (float*) data;
			break;
		case FREQ:
			crunchyGrooveWear->freq = (const float*) data;
			break;
		case APPLY:
			crunchyGrooveWear->apply = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	CrunchyGrooveWear* crunchyGrooveWear = (CrunchyGrooveWear*) instance;

	for (int count = 0; count < 21; count++) {
		crunchyGrooveWear->aMidL[count] = 0.0;
		crunchyGrooveWear->bMidL[count] = 0.0;
		crunchyGrooveWear->cMidL[count] = 0.0;
		crunchyGrooveWear->dMidL[count] = 0.0;
		crunchyGrooveWear->aMidR[count] = 0.0;
		crunchyGrooveWear->bMidR[count] = 0.0;
		crunchyGrooveWear->cMidR[count] = 0.0;
		crunchyGrooveWear->dMidR[count] = 0.0;
		crunchyGrooveWear->fMid[count] = 0.0;
	}
	crunchyGrooveWear->aMidPrevL = 0.0;
	crunchyGrooveWear->bMidPrevL = 0.0;
	crunchyGrooveWear->cMidPrevL = 0.0;
	crunchyGrooveWear->dMidPrevL = 0.0;

	crunchyGrooveWear->aMidPrevR = 0.0;
	crunchyGrooveWear->bMidPrevR = 0.0;
	crunchyGrooveWear->cMidPrevR = 0.0;
	crunchyGrooveWear->dMidPrevR = 0.0;

	crunchyGrooveWear->fpdL = 1.0;
	while (crunchyGrooveWear->fpdL < 16386) crunchyGrooveWear->fpdL = rand() * UINT32_MAX;
	crunchyGrooveWear->fpdR = 1.0;
	while (crunchyGrooveWear->fpdR < 16386) crunchyGrooveWear->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	CrunchyGrooveWear* crunchyGrooveWear = (CrunchyGrooveWear*) instance;

	const float* in1 = crunchyGrooveWear->input[0];
	const float* in2 = crunchyGrooveWear->input[1];
	float* out1 = crunchyGrooveWear->output[0];
	float* out2 = crunchyGrooveWear->output[1];

	double overallscale = (pow(*crunchyGrooveWear->freq, 2) * 19.0) + 1.0;
	double gain = overallscale;
	// mid groove wear
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[0] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[0] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[1] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[1] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[2] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[2] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[3] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[3] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[4] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[4] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[5] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[5] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[6] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[6] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[7] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[7] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[8] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[8] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[9] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[9] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[10] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[10] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[11] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[11] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[12] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[12] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[13] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[13] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[14] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[14] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[15] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[15] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[16] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[16] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[17] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[17] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[18] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[18] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		crunchyGrooveWear->fMid[19] = 1.0;
		gain -= 1.0;
	} else {
		crunchyGrooveWear->fMid[19] = gain;
		gain = 0.0;
	}
	// there, now we have a neat little moving average with remainders, in stereo

	if (overallscale < 1.0) overallscale = 1.0;
	crunchyGrooveWear->fMid[0] /= overallscale;
	crunchyGrooveWear->fMid[1] /= overallscale;
	crunchyGrooveWear->fMid[2] /= overallscale;
	crunchyGrooveWear->fMid[3] /= overallscale;
	crunchyGrooveWear->fMid[4] /= overallscale;
	crunchyGrooveWear->fMid[5] /= overallscale;
	crunchyGrooveWear->fMid[6] /= overallscale;
	crunchyGrooveWear->fMid[7] /= overallscale;
	crunchyGrooveWear->fMid[8] /= overallscale;
	crunchyGrooveWear->fMid[9] /= overallscale;
	crunchyGrooveWear->fMid[10] /= overallscale;
	crunchyGrooveWear->fMid[11] /= overallscale;
	crunchyGrooveWear->fMid[12] /= overallscale;
	crunchyGrooveWear->fMid[13] /= overallscale;
	crunchyGrooveWear->fMid[14] /= overallscale;
	crunchyGrooveWear->fMid[15] /= overallscale;
	crunchyGrooveWear->fMid[16] /= overallscale;
	crunchyGrooveWear->fMid[17] /= overallscale;
	crunchyGrooveWear->fMid[18] /= overallscale;
	crunchyGrooveWear->fMid[19] /= overallscale;
	// and now it's neatly scaled, too

	double accumulatorSampleL;
	double correctionL;
	double accumulatorSampleR;
	double correctionR;

	double aWet = 1.0;
	double bWet = 1.0;
	double cWet = 1.0;
	double dWet = *crunchyGrooveWear->apply * 4.0;
	// four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0) {
		aWet = dWet;
		bWet = 0.0;
		cWet = 0.0;
		dWet = 0.0;
	} else if (dWet < 2.0) {
		bWet = dWet - 1.0;
		cWet = 0.0;
		dWet = 0.0;
	} else if (dWet < 3.0) {
		cWet = dWet - 2.0;
		dWet = 0.0;
	} else {
		dWet -= 3.0;
	}
	// this is one way to make a little set of dry/wet stages that are successively added to the
	// output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	// beyond that point: this is a way to progressively add a 'black box' sound processing
	// which lets you fall through to simpler processing at lower settings.

	// now we set them up so each full intensity one is blended evenly with dry for each stage.
	// That's because the GrooveWear algorithm works best combined with dry.
	// aWet *= 0.5;
	// bWet *= 0.5; This was the tweak which caused GrooveWear to be dark instead of distorty
	// cWet *= 0.5; Disabling this causes engaged stages to take on an edge, but 0.5 settings
	// dWet *= 0.5; for any stage will still produce a darker tone.
	//  This will make the behavior of the plugin more complex
	// if you are using a more typical algorithm (like a sin() or something) you won't use this part

	double aDry = 1.0 - aWet;
	double bDry = 1.0 - bWet;
	double cDry = 1.0 - cWet;
	double dDry = 1.0 - dWet;

	double drySampleL;
	double drySampleR;
	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = crunchyGrooveWear->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = crunchyGrooveWear->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		if (aWet > 0.0) {
			crunchyGrooveWear->aMidL[19] = crunchyGrooveWear->aMidL[18];
			crunchyGrooveWear->aMidL[18] = crunchyGrooveWear->aMidL[17];
			crunchyGrooveWear->aMidL[17] = crunchyGrooveWear->aMidL[16];
			crunchyGrooveWear->aMidL[16] = crunchyGrooveWear->aMidL[15];
			crunchyGrooveWear->aMidL[15] = crunchyGrooveWear->aMidL[14];
			crunchyGrooveWear->aMidL[14] = crunchyGrooveWear->aMidL[13];
			crunchyGrooveWear->aMidL[13] = crunchyGrooveWear->aMidL[12];
			crunchyGrooveWear->aMidL[12] = crunchyGrooveWear->aMidL[11];
			crunchyGrooveWear->aMidL[11] = crunchyGrooveWear->aMidL[10];
			crunchyGrooveWear->aMidL[10] = crunchyGrooveWear->aMidL[9];
			crunchyGrooveWear->aMidL[9] = crunchyGrooveWear->aMidL[8];
			crunchyGrooveWear->aMidL[8] = crunchyGrooveWear->aMidL[7];
			crunchyGrooveWear->aMidL[7] = crunchyGrooveWear->aMidL[6];
			crunchyGrooveWear->aMidL[6] = crunchyGrooveWear->aMidL[5];
			crunchyGrooveWear->aMidL[5] = crunchyGrooveWear->aMidL[4];
			crunchyGrooveWear->aMidL[4] = crunchyGrooveWear->aMidL[3];
			crunchyGrooveWear->aMidL[3] = crunchyGrooveWear->aMidL[2];
			crunchyGrooveWear->aMidL[2] = crunchyGrooveWear->aMidL[1];
			crunchyGrooveWear->aMidL[1] = crunchyGrooveWear->aMidL[0];
			crunchyGrooveWear->aMidL[0] = accumulatorSampleL = (inputSampleL - crunchyGrooveWear->aMidPrevL);
			// this is different from Aura because that is accumulating rates of change OF the rate of change
			// this is just averaging slews directly, and we have two stages of it.

			accumulatorSampleL *= crunchyGrooveWear->fMid[0];
			accumulatorSampleL += (crunchyGrooveWear->aMidL[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleL += (crunchyGrooveWear->aMidL[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionL = (inputSampleL - crunchyGrooveWear->aMidPrevL) - accumulatorSampleL;
			crunchyGrooveWear->aMidPrevL = inputSampleL;
			inputSampleL -= correctionL;
			inputSampleL = (inputSampleL * aWet) + (drySampleL * aDry);
			drySampleL = inputSampleL;

			crunchyGrooveWear->aMidR[19] = crunchyGrooveWear->aMidR[18];
			crunchyGrooveWear->aMidR[18] = crunchyGrooveWear->aMidR[17];
			crunchyGrooveWear->aMidR[17] = crunchyGrooveWear->aMidR[16];
			crunchyGrooveWear->aMidR[16] = crunchyGrooveWear->aMidR[15];
			crunchyGrooveWear->aMidR[15] = crunchyGrooveWear->aMidR[14];
			crunchyGrooveWear->aMidR[14] = crunchyGrooveWear->aMidR[13];
			crunchyGrooveWear->aMidR[13] = crunchyGrooveWear->aMidR[12];
			crunchyGrooveWear->aMidR[12] = crunchyGrooveWear->aMidR[11];
			crunchyGrooveWear->aMidR[11] = crunchyGrooveWear->aMidR[10];
			crunchyGrooveWear->aMidR[10] = crunchyGrooveWear->aMidR[9];
			crunchyGrooveWear->aMidR[9] = crunchyGrooveWear->aMidR[8];
			crunchyGrooveWear->aMidR[8] = crunchyGrooveWear->aMidR[7];
			crunchyGrooveWear->aMidR[7] = crunchyGrooveWear->aMidR[6];
			crunchyGrooveWear->aMidR[6] = crunchyGrooveWear->aMidR[5];
			crunchyGrooveWear->aMidR[5] = crunchyGrooveWear->aMidR[4];
			crunchyGrooveWear->aMidR[4] = crunchyGrooveWear->aMidR[3];
			crunchyGrooveWear->aMidR[3] = crunchyGrooveWear->aMidR[2];
			crunchyGrooveWear->aMidR[2] = crunchyGrooveWear->aMidR[1];
			crunchyGrooveWear->aMidR[1] = crunchyGrooveWear->aMidR[0];
			crunchyGrooveWear->aMidR[0] = accumulatorSampleR = (inputSampleR - crunchyGrooveWear->aMidPrevR);
			// this is different from Aura because that is accumulating rates of change OF the rate of change
			// this is just averaging slews directly, and we have two stages of it.

			accumulatorSampleR *= crunchyGrooveWear->fMid[0];
			accumulatorSampleR += (crunchyGrooveWear->aMidR[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleR += (crunchyGrooveWear->aMidR[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionR = (inputSampleR - crunchyGrooveWear->aMidPrevR) - accumulatorSampleR;
			crunchyGrooveWear->aMidPrevR = inputSampleR;
			inputSampleR -= correctionR;
			inputSampleR = (inputSampleR * aWet) + (drySampleR * aDry);
			drySampleR = inputSampleR;
		}

		if (bWet > 0.0) {
			crunchyGrooveWear->bMidL[19] = crunchyGrooveWear->bMidL[18];
			crunchyGrooveWear->bMidL[18] = crunchyGrooveWear->bMidL[17];
			crunchyGrooveWear->bMidL[17] = crunchyGrooveWear->bMidL[16];
			crunchyGrooveWear->bMidL[16] = crunchyGrooveWear->bMidL[15];
			crunchyGrooveWear->bMidL[15] = crunchyGrooveWear->bMidL[14];
			crunchyGrooveWear->bMidL[14] = crunchyGrooveWear->bMidL[13];
			crunchyGrooveWear->bMidL[13] = crunchyGrooveWear->bMidL[12];
			crunchyGrooveWear->bMidL[12] = crunchyGrooveWear->bMidL[11];
			crunchyGrooveWear->bMidL[11] = crunchyGrooveWear->bMidL[10];
			crunchyGrooveWear->bMidL[10] = crunchyGrooveWear->bMidL[9];
			crunchyGrooveWear->bMidL[9] = crunchyGrooveWear->bMidL[8];
			crunchyGrooveWear->bMidL[8] = crunchyGrooveWear->bMidL[7];
			crunchyGrooveWear->bMidL[7] = crunchyGrooveWear->bMidL[6];
			crunchyGrooveWear->bMidL[6] = crunchyGrooveWear->bMidL[5];
			crunchyGrooveWear->bMidL[5] = crunchyGrooveWear->bMidL[4];
			crunchyGrooveWear->bMidL[4] = crunchyGrooveWear->bMidL[3];
			crunchyGrooveWear->bMidL[3] = crunchyGrooveWear->bMidL[2];
			crunchyGrooveWear->bMidL[2] = crunchyGrooveWear->bMidL[1];
			crunchyGrooveWear->bMidL[1] = crunchyGrooveWear->bMidL[0];
			crunchyGrooveWear->bMidL[0] = accumulatorSampleL = (inputSampleL - crunchyGrooveWear->bMidPrevL);

			accumulatorSampleL *= crunchyGrooveWear->fMid[0];
			accumulatorSampleL += (crunchyGrooveWear->bMidL[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleL += (crunchyGrooveWear->bMidL[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionL = (inputSampleL - crunchyGrooveWear->bMidPrevL) - accumulatorSampleL;
			crunchyGrooveWear->bMidPrevL = inputSampleL;
			inputSampleL -= correctionL;
			inputSampleL = (inputSampleL * bWet) + (drySampleL * bDry);
			drySampleL = inputSampleL;

			crunchyGrooveWear->bMidR[19] = crunchyGrooveWear->bMidR[18];
			crunchyGrooveWear->bMidR[18] = crunchyGrooveWear->bMidR[17];
			crunchyGrooveWear->bMidR[17] = crunchyGrooveWear->bMidR[16];
			crunchyGrooveWear->bMidR[16] = crunchyGrooveWear->bMidR[15];
			crunchyGrooveWear->bMidR[15] = crunchyGrooveWear->bMidR[14];
			crunchyGrooveWear->bMidR[14] = crunchyGrooveWear->bMidR[13];
			crunchyGrooveWear->bMidR[13] = crunchyGrooveWear->bMidR[12];
			crunchyGrooveWear->bMidR[12] = crunchyGrooveWear->bMidR[11];
			crunchyGrooveWear->bMidR[11] = crunchyGrooveWear->bMidR[10];
			crunchyGrooveWear->bMidR[10] = crunchyGrooveWear->bMidR[9];
			crunchyGrooveWear->bMidR[9] = crunchyGrooveWear->bMidR[8];
			crunchyGrooveWear->bMidR[8] = crunchyGrooveWear->bMidR[7];
			crunchyGrooveWear->bMidR[7] = crunchyGrooveWear->bMidR[6];
			crunchyGrooveWear->bMidR[6] = crunchyGrooveWear->bMidR[5];
			crunchyGrooveWear->bMidR[5] = crunchyGrooveWear->bMidR[4];
			crunchyGrooveWear->bMidR[4] = crunchyGrooveWear->bMidR[3];
			crunchyGrooveWear->bMidR[3] = crunchyGrooveWear->bMidR[2];
			crunchyGrooveWear->bMidR[2] = crunchyGrooveWear->bMidR[1];
			crunchyGrooveWear->bMidR[1] = crunchyGrooveWear->bMidR[0];
			crunchyGrooveWear->bMidR[0] = accumulatorSampleR = (inputSampleR - crunchyGrooveWear->bMidPrevR);

			accumulatorSampleR *= crunchyGrooveWear->fMid[0];
			accumulatorSampleR += (crunchyGrooveWear->bMidR[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleR += (crunchyGrooveWear->bMidR[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionR = (inputSampleR - crunchyGrooveWear->bMidPrevR) - accumulatorSampleR;
			crunchyGrooveWear->bMidPrevR = inputSampleR;
			inputSampleR -= correctionR;
			inputSampleR = (inputSampleR * bWet) + (drySampleR * bDry);
			drySampleR = inputSampleR;
		}

		if (cWet > 0.0) {
			crunchyGrooveWear->cMidL[19] = crunchyGrooveWear->cMidL[18];
			crunchyGrooveWear->cMidL[18] = crunchyGrooveWear->cMidL[17];
			crunchyGrooveWear->cMidL[17] = crunchyGrooveWear->cMidL[16];
			crunchyGrooveWear->cMidL[16] = crunchyGrooveWear->cMidL[15];
			crunchyGrooveWear->cMidL[15] = crunchyGrooveWear->cMidL[14];
			crunchyGrooveWear->cMidL[14] = crunchyGrooveWear->cMidL[13];
			crunchyGrooveWear->cMidL[13] = crunchyGrooveWear->cMidL[12];
			crunchyGrooveWear->cMidL[12] = crunchyGrooveWear->cMidL[11];
			crunchyGrooveWear->cMidL[11] = crunchyGrooveWear->cMidL[10];
			crunchyGrooveWear->cMidL[10] = crunchyGrooveWear->cMidL[9];
			crunchyGrooveWear->cMidL[9] = crunchyGrooveWear->cMidL[8];
			crunchyGrooveWear->cMidL[8] = crunchyGrooveWear->cMidL[7];
			crunchyGrooveWear->cMidL[7] = crunchyGrooveWear->cMidL[6];
			crunchyGrooveWear->cMidL[6] = crunchyGrooveWear->cMidL[5];
			crunchyGrooveWear->cMidL[5] = crunchyGrooveWear->cMidL[4];
			crunchyGrooveWear->cMidL[4] = crunchyGrooveWear->cMidL[3];
			crunchyGrooveWear->cMidL[3] = crunchyGrooveWear->cMidL[2];
			crunchyGrooveWear->cMidL[2] = crunchyGrooveWear->cMidL[1];
			crunchyGrooveWear->cMidL[1] = crunchyGrooveWear->cMidL[0];
			crunchyGrooveWear->cMidL[0] = accumulatorSampleL = (inputSampleL - crunchyGrooveWear->cMidPrevL);

			accumulatorSampleL *= crunchyGrooveWear->fMid[0];
			accumulatorSampleL += (crunchyGrooveWear->cMidL[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleL += (crunchyGrooveWear->cMidL[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionL = (inputSampleL - crunchyGrooveWear->cMidPrevL) - accumulatorSampleL;
			crunchyGrooveWear->cMidPrevL = inputSampleL;
			inputSampleL -= correctionL;
			inputSampleL = (inputSampleL * cWet) + (drySampleL * cDry);
			drySampleL = inputSampleL;

			crunchyGrooveWear->cMidR[19] = crunchyGrooveWear->cMidR[18];
			crunchyGrooveWear->cMidR[18] = crunchyGrooveWear->cMidR[17];
			crunchyGrooveWear->cMidR[17] = crunchyGrooveWear->cMidR[16];
			crunchyGrooveWear->cMidR[16] = crunchyGrooveWear->cMidR[15];
			crunchyGrooveWear->cMidR[15] = crunchyGrooveWear->cMidR[14];
			crunchyGrooveWear->cMidR[14] = crunchyGrooveWear->cMidR[13];
			crunchyGrooveWear->cMidR[13] = crunchyGrooveWear->cMidR[12];
			crunchyGrooveWear->cMidR[12] = crunchyGrooveWear->cMidR[11];
			crunchyGrooveWear->cMidR[11] = crunchyGrooveWear->cMidR[10];
			crunchyGrooveWear->cMidR[10] = crunchyGrooveWear->cMidR[9];
			crunchyGrooveWear->cMidR[9] = crunchyGrooveWear->cMidR[8];
			crunchyGrooveWear->cMidR[8] = crunchyGrooveWear->cMidR[7];
			crunchyGrooveWear->cMidR[7] = crunchyGrooveWear->cMidR[6];
			crunchyGrooveWear->cMidR[6] = crunchyGrooveWear->cMidR[5];
			crunchyGrooveWear->cMidR[5] = crunchyGrooveWear->cMidR[4];
			crunchyGrooveWear->cMidR[4] = crunchyGrooveWear->cMidR[3];
			crunchyGrooveWear->cMidR[3] = crunchyGrooveWear->cMidR[2];
			crunchyGrooveWear->cMidR[2] = crunchyGrooveWear->cMidR[1];
			crunchyGrooveWear->cMidR[1] = crunchyGrooveWear->cMidR[0];
			crunchyGrooveWear->cMidR[0] = accumulatorSampleR = (inputSampleR - crunchyGrooveWear->cMidPrevR);

			accumulatorSampleR *= crunchyGrooveWear->fMid[0];
			accumulatorSampleR += (crunchyGrooveWear->cMidR[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleR += (crunchyGrooveWear->cMidR[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionR = (inputSampleR - crunchyGrooveWear->cMidPrevR) - accumulatorSampleR;
			crunchyGrooveWear->cMidPrevR = inputSampleR;
			inputSampleR -= correctionR;
			inputSampleR = (inputSampleR * cWet) + (drySampleR * cDry);
			drySampleR = inputSampleR;
		}

		if (dWet > 0.0) {
			crunchyGrooveWear->dMidL[19] = crunchyGrooveWear->dMidL[18];
			crunchyGrooveWear->dMidL[18] = crunchyGrooveWear->dMidL[17];
			crunchyGrooveWear->dMidL[17] = crunchyGrooveWear->dMidL[16];
			crunchyGrooveWear->dMidL[16] = crunchyGrooveWear->dMidL[15];
			crunchyGrooveWear->dMidL[15] = crunchyGrooveWear->dMidL[14];
			crunchyGrooveWear->dMidL[14] = crunchyGrooveWear->dMidL[13];
			crunchyGrooveWear->dMidL[13] = crunchyGrooveWear->dMidL[12];
			crunchyGrooveWear->dMidL[12] = crunchyGrooveWear->dMidL[11];
			crunchyGrooveWear->dMidL[11] = crunchyGrooveWear->dMidL[10];
			crunchyGrooveWear->dMidL[10] = crunchyGrooveWear->dMidL[9];
			crunchyGrooveWear->dMidL[9] = crunchyGrooveWear->dMidL[8];
			crunchyGrooveWear->dMidL[8] = crunchyGrooveWear->dMidL[7];
			crunchyGrooveWear->dMidL[7] = crunchyGrooveWear->dMidL[6];
			crunchyGrooveWear->dMidL[6] = crunchyGrooveWear->dMidL[5];
			crunchyGrooveWear->dMidL[5] = crunchyGrooveWear->dMidL[4];
			crunchyGrooveWear->dMidL[4] = crunchyGrooveWear->dMidL[3];
			crunchyGrooveWear->dMidL[3] = crunchyGrooveWear->dMidL[2];
			crunchyGrooveWear->dMidL[2] = crunchyGrooveWear->dMidL[1];
			crunchyGrooveWear->dMidL[1] = crunchyGrooveWear->dMidL[0];
			crunchyGrooveWear->dMidL[0] = accumulatorSampleL = (inputSampleL - crunchyGrooveWear->dMidPrevL);

			accumulatorSampleL *= crunchyGrooveWear->fMid[0];
			accumulatorSampleL += (crunchyGrooveWear->dMidL[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleL += (crunchyGrooveWear->dMidL[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionL = (inputSampleL - crunchyGrooveWear->dMidPrevL) - accumulatorSampleL;
			crunchyGrooveWear->dMidPrevL = inputSampleL;
			inputSampleL -= correctionL;
			inputSampleL = (inputSampleL * dWet) + (drySampleL * dDry);
			drySampleL = inputSampleL;

			crunchyGrooveWear->dMidR[19] = crunchyGrooveWear->dMidR[18];
			crunchyGrooveWear->dMidR[18] = crunchyGrooveWear->dMidR[17];
			crunchyGrooveWear->dMidR[17] = crunchyGrooveWear->dMidR[16];
			crunchyGrooveWear->dMidR[16] = crunchyGrooveWear->dMidR[15];
			crunchyGrooveWear->dMidR[15] = crunchyGrooveWear->dMidR[14];
			crunchyGrooveWear->dMidR[14] = crunchyGrooveWear->dMidR[13];
			crunchyGrooveWear->dMidR[13] = crunchyGrooveWear->dMidR[12];
			crunchyGrooveWear->dMidR[12] = crunchyGrooveWear->dMidR[11];
			crunchyGrooveWear->dMidR[11] = crunchyGrooveWear->dMidR[10];
			crunchyGrooveWear->dMidR[10] = crunchyGrooveWear->dMidR[9];
			crunchyGrooveWear->dMidR[9] = crunchyGrooveWear->dMidR[8];
			crunchyGrooveWear->dMidR[8] = crunchyGrooveWear->dMidR[7];
			crunchyGrooveWear->dMidR[7] = crunchyGrooveWear->dMidR[6];
			crunchyGrooveWear->dMidR[6] = crunchyGrooveWear->dMidR[5];
			crunchyGrooveWear->dMidR[5] = crunchyGrooveWear->dMidR[4];
			crunchyGrooveWear->dMidR[4] = crunchyGrooveWear->dMidR[3];
			crunchyGrooveWear->dMidR[3] = crunchyGrooveWear->dMidR[2];
			crunchyGrooveWear->dMidR[2] = crunchyGrooveWear->dMidR[1];
			crunchyGrooveWear->dMidR[1] = crunchyGrooveWear->dMidR[0];
			crunchyGrooveWear->dMidR[0] = accumulatorSampleR = (inputSampleR - crunchyGrooveWear->dMidPrevR);

			accumulatorSampleR *= crunchyGrooveWear->fMid[0];
			accumulatorSampleR += (crunchyGrooveWear->dMidR[1] * crunchyGrooveWear->fMid[1]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[2] * crunchyGrooveWear->fMid[2]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[3] * crunchyGrooveWear->fMid[3]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[4] * crunchyGrooveWear->fMid[4]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[5] * crunchyGrooveWear->fMid[5]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[6] * crunchyGrooveWear->fMid[6]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[7] * crunchyGrooveWear->fMid[7]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[8] * crunchyGrooveWear->fMid[8]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[9] * crunchyGrooveWear->fMid[9]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[10] * crunchyGrooveWear->fMid[10]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[11] * crunchyGrooveWear->fMid[11]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[12] * crunchyGrooveWear->fMid[12]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[13] * crunchyGrooveWear->fMid[13]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[14] * crunchyGrooveWear->fMid[14]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[15] * crunchyGrooveWear->fMid[15]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[16] * crunchyGrooveWear->fMid[16]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[17] * crunchyGrooveWear->fMid[17]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[18] * crunchyGrooveWear->fMid[18]);
			accumulatorSampleR += (crunchyGrooveWear->dMidR[19] * crunchyGrooveWear->fMid[19]);
			// we are doing our repetitive calculations on a separate value
			correctionR = (inputSampleR - crunchyGrooveWear->dMidPrevR) - accumulatorSampleR;
			crunchyGrooveWear->dMidPrevR = inputSampleR;
			inputSampleR -= correctionR;
			inputSampleR = (inputSampleR * dWet) + (drySampleR * dDry);
			drySampleR = inputSampleR;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		crunchyGrooveWear->fpdL ^= crunchyGrooveWear->fpdL << 13;
		crunchyGrooveWear->fpdL ^= crunchyGrooveWear->fpdL >> 17;
		crunchyGrooveWear->fpdL ^= crunchyGrooveWear->fpdL << 5;
		inputSampleL += (((double) crunchyGrooveWear->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		crunchyGrooveWear->fpdR ^= crunchyGrooveWear->fpdR << 13;
		crunchyGrooveWear->fpdR ^= crunchyGrooveWear->fpdR >> 17;
		crunchyGrooveWear->fpdR ^= crunchyGrooveWear->fpdR << 5;
		inputSampleR += (((double) crunchyGrooveWear->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CRUNCHYGROOVEWEAR_URI,
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
