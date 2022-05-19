#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define TOTAPE6_URI "https://hannesbraun.net/ns/lv2/airwindows/totape6"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4,
	SOFTEN = 5,
	HEAD_BUMP = 6,
	FLUTTER = 7,
	OUTPUT = 8,
	DRY_WET = 9
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputGain;
	const float* soften;
	const float* headBump;
	const float* flutter;
	const float* outputGain;
	const float* dryWet;

	double dL[502];
	double dR[502];
	int gcount;
	double rateof;
	double sweep;
	double nextmax;

	double iirMidRollerAL;
	double iirMidRollerBL;
	double iirHeadBumpAL;
	double iirHeadBumpBL;

	double iirMidRollerAR;
	double iirMidRollerBR;
	double iirHeadBumpAR;
	double iirHeadBumpBR;
	double biquadAL[9];
	double biquadBL[9];
	double biquadCL[9];
	double biquadDL[9];

	double biquadAR[9];
	double biquadBR[9];
	double biquadCR[9];
	double biquadDR[9];
	bool flip;

	double lastSampleL;
	double lastSampleR;

	uint32_t fpdL;
	uint32_t fpdR;
} ToTape6;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ToTape6* toTape6 = (ToTape6*) calloc(1, sizeof(ToTape6));
	toTape6->sampleRate = rate;
	return (LV2_Handle) toTape6;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ToTape6* toTape6 = (ToTape6*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			toTape6->input[0] = (const float*) data;
			break;
		case INPUT_R:
			toTape6->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			toTape6->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			toTape6->output[1] = (float*) data;
			break;
		case INPUT:
			toTape6->inputGain = (const float*) data;
			break;
		case SOFTEN:
			toTape6->soften = (const float*) data;
			break;
		case HEAD_BUMP:
			toTape6->headBump = (const float*) data;
			break;
		case FLUTTER:
			toTape6->flutter = (const float*) data;
			break;
		case OUTPUT:
			toTape6->outputGain = (const float*) data;
			break;
		case DRY_WET:
			toTape6->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ToTape6* toTape6 = (ToTape6*) instance;

	toTape6->iirMidRollerAL = 0.0;
	toTape6->iirMidRollerBL = 0.0;
	toTape6->iirHeadBumpAL = 0.0;
	toTape6->iirHeadBumpBL = 0.0;

	toTape6->iirMidRollerAR = 0.0;
	toTape6->iirMidRollerBR = 0.0;
	toTape6->iirHeadBumpAR = 0.0;
	toTape6->iirHeadBumpBR = 0.0;

	for (int x = 0; x < 9; x++) {
		toTape6->biquadAL[x] = 0.0;
		toTape6->biquadBL[x] = 0.0;
		toTape6->biquadCL[x] = 0.0;
		toTape6->biquadDL[x] = 0.0;
		toTape6->biquadAR[x] = 0.0;
		toTape6->biquadBR[x] = 0.0;
		toTape6->biquadCR[x] = 0.0;
		toTape6->biquadDR[x] = 0.0;
	}
	toTape6->flip = false;
	for (int temp = 0; temp < 501; temp++) {
		toTape6->dL[temp] = 0.0;
		toTape6->dR[temp] = 0.0;
	}

	toTape6->gcount = 0;
	toTape6->rateof = 0.5;
	toTape6->sweep = M_PI;
	toTape6->nextmax = 0.5;
	toTape6->lastSampleL = 0.0;
	toTape6->lastSampleR = 0.0;
	toTape6->flip = 0;

	toTape6->fpdL = 1.0;
	while (toTape6->fpdL < 16386) toTape6->fpdL = rand() * UINT32_MAX;
	toTape6->fpdR = 1.0;
	while (toTape6->fpdR < 16386) toTape6->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ToTape6* toTape6 = (ToTape6*) instance;

	const float* in1 = toTape6->input[0];
	const float* in2 = toTape6->input[1];
	float* out1 = toTape6->output[0];
	float* out2 = toTape6->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= toTape6->sampleRate;

	double inputgain = pow(10.0, *toTape6->inputGain / 20.0);
	double SoftenControl = pow(*toTape6->soften, 2);
	double RollAmount = (1.0 - (SoftenControl * 0.45)) / overallscale;
	double HeadBumpControl = *toTape6->headBump * 0.25 * inputgain;
	double HeadBumpFreq = 0.12 / overallscale;
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	toTape6->biquadAL[0] = 0.007 / overallscale;
	toTape6->biquadBL[0] = 0.007 / overallscale;
	toTape6->biquadAR[0] = 0.007 / overallscale;
	toTape6->biquadBR[0] = 0.007 / overallscale;
	toTape6->biquadAL[1] = 0.0009;
	toTape6->biquadBL[1] = 0.0009;
	toTape6->biquadAR[1] = 0.0009;
	toTape6->biquadBR[1] = 0.0009;
	double K = tan(M_PI * toTape6->biquadBR[0]);
	double norm = 1.0 / (1.0 + K / toTape6->biquadBR[1] + K * K);
	toTape6->biquadAL[2] = K / toTape6->biquadBR[1] * norm;
	toTape6->biquadBL[2] = toTape6->biquadAL[2];
	toTape6->biquadAR[2] = toTape6->biquadAL[2];
	toTape6->biquadBR[2] = toTape6->biquadAL[2];
	toTape6->biquadAL[4] = -toTape6->biquadBR[2];
	toTape6->biquadBL[4] = -toTape6->biquadBR[2];
	toTape6->biquadAR[4] = -toTape6->biquadBR[2];
	toTape6->biquadBR[4] = -toTape6->biquadBR[2];
	toTape6->biquadAL[5] = 2.0 * (K * K - 1.0) * norm;
	toTape6->biquadBL[5] = toTape6->biquadAL[5];
	toTape6->biquadAR[5] = toTape6->biquadAL[5];
	toTape6->biquadBR[5] = toTape6->biquadAL[5];
	toTape6->biquadAL[6] = (1.0 - K / toTape6->biquadBR[1] + K * K) * norm;
	toTape6->biquadBL[6] = toTape6->biquadAL[6];
	toTape6->biquadAR[6] = toTape6->biquadAL[6];
	toTape6->biquadBR[6] = toTape6->biquadAL[6];

	toTape6->biquadCL[0] = 0.032 / overallscale;
	toTape6->biquadDL[0] = 0.032 / overallscale;
	toTape6->biquadCR[0] = 0.032 / overallscale;
	toTape6->biquadDR[0] = 0.032 / overallscale;
	toTape6->biquadCL[1] = 0.0007;
	toTape6->biquadDL[1] = 0.0007;
	toTape6->biquadCR[1] = 0.0007;
	toTape6->biquadDR[1] = 0.0007;
	K = tan(M_PI * toTape6->biquadDR[0]);
	norm = 1.0 / (1.0 + K / toTape6->biquadDR[1] + K * K);
	toTape6->biquadCL[2] = K / toTape6->biquadDR[1] * norm;
	toTape6->biquadDL[2] = toTape6->biquadCL[2];
	toTape6->biquadCR[2] = toTape6->biquadCL[2];
	toTape6->biquadDR[2] = toTape6->biquadCL[2];
	toTape6->biquadCL[4] = -toTape6->biquadDR[2];
	toTape6->biquadDL[4] = -toTape6->biquadDR[2];
	toTape6->biquadCR[4] = -toTape6->biquadDR[2];
	toTape6->biquadDR[4] = -toTape6->biquadDR[2];
	toTape6->biquadCL[5] = 2.0 * (K * K - 1.0) * norm;
	toTape6->biquadDL[5] = toTape6->biquadCL[5];
	toTape6->biquadCR[5] = toTape6->biquadCL[5];
	toTape6->biquadDR[5] = toTape6->biquadCL[5];
	toTape6->biquadCL[6] = (1.0 - K / toTape6->biquadDR[1] + K * K) * norm;
	toTape6->biquadDL[6] = toTape6->biquadCL[6];
	toTape6->biquadCR[6] = toTape6->biquadCL[6];
	toTape6->biquadDR[6] = toTape6->biquadCL[6];

	const float flutter = *toTape6->flutter;
	double depth = pow(flutter, 2) * overallscale * 70;
	double fluttertrim = (0.0024 * pow(flutter, 2)) / overallscale;
	double outputgain = pow(10.0, *toTape6->outputGain / 20.0);

	double refclip = 0.99;
	double softness = 0.618033988749894848204586;

	double wet = *toTape6->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = toTape6->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = toTape6->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (inputgain < 1.0) {
			inputSampleL *= inputgain;
			inputSampleR *= inputgain;
		} //gain cut before plugin

		double flutterrandy = (double)toTape6->fpdL / UINT32_MAX;
		//now we've got a random flutter, so we're messing with the pitch before tape effects go on
		if (toTape6->gcount < 0 || toTape6->gcount > 499) {
			toTape6->gcount = 499;
		}
		toTape6->dL[toTape6->gcount] = inputSampleL;
		toTape6->dR[toTape6->gcount] = inputSampleR;
		int count = toTape6->gcount;
		if (depth != 0.0) {

			double offset = depth + (depth * pow(toTape6->rateof, 2) * sin(toTape6->sweep));

			count += (int)floor(offset);
			inputSampleL = (toTape6->dL[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
			inputSampleR = (toTape6->dR[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
			inputSampleL += (toTape6->dL[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));
			inputSampleR += (toTape6->dR[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));

			toTape6->rateof = (toTape6->rateof * (1.0 - fluttertrim)) + (toTape6->nextmax * fluttertrim);
			toTape6->sweep += toTape6->rateof * fluttertrim;

			if (toTape6->sweep >= (M_PI * 2.0)) {
				toTape6->sweep -= M_PI;
				toTape6->nextmax = 0.24 + (flutterrandy * 0.74);
			}
			//apply to input signal only when flutter is present, interpolate samples
		}
		toTape6->gcount--;

		double vibDrySampleL = inputSampleL;
		double vibDrySampleR = inputSampleR;
		double HighsSampleL = 0.0;
		double HighsSampleR = 0.0;
		double NonHighsSampleL = 0.0;
		double NonHighsSampleR = 0.0;
		double tempSample;

		if (toTape6->flip) {
			toTape6->iirMidRollerAL = (toTape6->iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
			toTape6->iirMidRollerAR = (toTape6->iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
			HighsSampleL = inputSampleL - toTape6->iirMidRollerAL;
			HighsSampleR = inputSampleR - toTape6->iirMidRollerAR;
			NonHighsSampleL = toTape6->iirMidRollerAL;
			NonHighsSampleR = toTape6->iirMidRollerAR;

			toTape6->iirHeadBumpAL += (inputSampleL * 0.05);
			toTape6->iirHeadBumpAR += (inputSampleR * 0.05);
			toTape6->iirHeadBumpAL -= (toTape6->iirHeadBumpAL * toTape6->iirHeadBumpAL * toTape6->iirHeadBumpAL * HeadBumpFreq);
			toTape6->iirHeadBumpAR -= (toTape6->iirHeadBumpAR * toTape6->iirHeadBumpAR * toTape6->iirHeadBumpAR * HeadBumpFreq);
			toTape6->iirHeadBumpAL = sin(toTape6->iirHeadBumpAL);
			toTape6->iirHeadBumpAR = sin(toTape6->iirHeadBumpAR);

			tempSample = (toTape6->iirHeadBumpAL * toTape6->biquadAL[2]) + toTape6->biquadAL[7];
			toTape6->biquadAL[7] = (toTape6->iirHeadBumpAL * toTape6->biquadAL[3]) - (tempSample * toTape6->biquadAL[5]) + toTape6->biquadAL[8];
			toTape6->biquadAL[8] = (toTape6->iirHeadBumpAL * toTape6->biquadAL[4]) - (tempSample * toTape6->biquadAL[6]);
			toTape6->iirHeadBumpAL = tempSample; //interleaved biquad
			if (toTape6->iirHeadBumpAL > 1.0) toTape6->iirHeadBumpAL = 1.0;
			if (toTape6->iirHeadBumpAL < -1.0) toTape6->iirHeadBumpAL = -1.0;
			toTape6->iirHeadBumpAL = asin(toTape6->iirHeadBumpAL);

			tempSample = (toTape6->iirHeadBumpAR * toTape6->biquadAR[2]) + toTape6->biquadAR[7];
			toTape6->biquadAR[7] = (toTape6->iirHeadBumpAR * toTape6->biquadAR[3]) - (tempSample * toTape6->biquadAR[5]) + toTape6->biquadAR[8];
			toTape6->biquadAR[8] = (toTape6->iirHeadBumpAR * toTape6->biquadAR[4]) - (tempSample * toTape6->biquadAR[6]);
			toTape6->iirHeadBumpAR = tempSample; //interleaved biquad
			if (toTape6->iirHeadBumpAR > 1.0) toTape6->iirHeadBumpAR = 1.0;
			if (toTape6->iirHeadBumpAR < -1.0) toTape6->iirHeadBumpAR = -1.0;
			toTape6->iirHeadBumpAR = asin(toTape6->iirHeadBumpAR);

			inputSampleL = sin(inputSampleL);
			tempSample = (inputSampleL * toTape6->biquadCL[2]) + toTape6->biquadCL[7];
			toTape6->biquadCL[7] = (inputSampleL * toTape6->biquadCL[3]) - (tempSample * toTape6->biquadCL[5]) + toTape6->biquadCL[8];
			toTape6->biquadCL[8] = (inputSampleL * toTape6->biquadCL[4]) - (tempSample * toTape6->biquadCL[6]);
			inputSampleL = tempSample; //interleaved biquad
			if (inputSampleL > 1.0) inputSampleL = 1.0;
			if (inputSampleL < -1.0) inputSampleL = -1.0;
			inputSampleL = asin(inputSampleL);

			inputSampleR = sin(inputSampleR);
			tempSample = (inputSampleR * toTape6->biquadCR[2]) + toTape6->biquadCR[7];
			toTape6->biquadCR[7] = (inputSampleR * toTape6->biquadCR[3]) - (tempSample * toTape6->biquadCR[5]) + toTape6->biquadCR[8];
			toTape6->biquadCR[8] = (inputSampleR * toTape6->biquadCR[4]) - (tempSample * toTape6->biquadCR[6]);
			inputSampleR = tempSample; //interleaved biquad
			if (inputSampleR > 1.0) inputSampleR = 1.0;
			if (inputSampleR < -1.0) inputSampleR = -1.0;
			inputSampleR = asin(inputSampleR);
		} else {
			toTape6->iirMidRollerBL = (toTape6->iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
			toTape6->iirMidRollerBR = (toTape6->iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
			HighsSampleL = inputSampleL - toTape6->iirMidRollerBL;
			HighsSampleR = inputSampleR - toTape6->iirMidRollerBR;
			NonHighsSampleL = toTape6->iirMidRollerBL;
			NonHighsSampleR = toTape6->iirMidRollerBR;

			toTape6->iirHeadBumpBL += (inputSampleL * 0.05);
			toTape6->iirHeadBumpBR += (inputSampleR * 0.05);
			toTape6->iirHeadBumpBL -= (toTape6->iirHeadBumpBL * toTape6->iirHeadBumpBL * toTape6->iirHeadBumpBL * HeadBumpFreq);
			toTape6->iirHeadBumpBR -= (toTape6->iirHeadBumpBR * toTape6->iirHeadBumpBR * toTape6->iirHeadBumpBR * HeadBumpFreq);
			toTape6->iirHeadBumpBL = sin(toTape6->iirHeadBumpBL);
			toTape6->iirHeadBumpBR = sin(toTape6->iirHeadBumpBR);

			tempSample = (toTape6->iirHeadBumpBL * toTape6->biquadBL[2]) + toTape6->biquadBL[7];
			toTape6->biquadBL[7] = (toTape6->iirHeadBumpBL * toTape6->biquadBL[3]) - (tempSample * toTape6->biquadBL[5]) + toTape6->biquadBL[8];
			toTape6->biquadBL[8] = (toTape6->iirHeadBumpBL * toTape6->biquadBL[4]) - (tempSample * toTape6->biquadBL[6]);
			toTape6->iirHeadBumpBL = tempSample; //interleaved biquad
			if (toTape6->iirHeadBumpBL > 1.0) toTape6->iirHeadBumpBL = 1.0;
			if (toTape6->iirHeadBumpBL < -1.0) toTape6->iirHeadBumpBL = -1.0;
			toTape6->iirHeadBumpBL = asin(toTape6->iirHeadBumpBL);

			tempSample = (toTape6->iirHeadBumpBR * toTape6->biquadBR[2]) + toTape6->biquadBR[7];
			toTape6->biquadBR[7] = (toTape6->iirHeadBumpBR * toTape6->biquadBR[3]) - (tempSample * toTape6->biquadBR[5]) + toTape6->biquadBR[8];
			toTape6->biquadBR[8] = (toTape6->iirHeadBumpBR * toTape6->biquadBR[4]) - (tempSample * toTape6->biquadBR[6]);
			toTape6->iirHeadBumpBR = tempSample; //interleaved biquad
			if (toTape6->iirHeadBumpBR > 1.0) toTape6->iirHeadBumpBR = 1.0;
			if (toTape6->iirHeadBumpBR < -1.0) toTape6->iirHeadBumpBR = -1.0;
			toTape6->iirHeadBumpBR = asin(toTape6->iirHeadBumpBR);

			inputSampleL = sin(inputSampleL);
			tempSample = (inputSampleL * toTape6->biquadDL[2]) + toTape6->biquadDL[7];
			toTape6->biquadDL[7] = (inputSampleL * toTape6->biquadDL[3]) - (tempSample * toTape6->biquadDL[5]) + toTape6->biquadDL[8];
			toTape6->biquadDL[8] = (inputSampleL * toTape6->biquadDL[4]) - (tempSample * toTape6->biquadDL[6]);
			inputSampleL = tempSample; //interleaved biquad
			if (inputSampleL > 1.0) inputSampleL = 1.0;
			if (inputSampleL < -1.0) inputSampleL = -1.0;
			inputSampleL = asin(inputSampleL);

			inputSampleR = sin(inputSampleR);
			tempSample = (inputSampleR * toTape6->biquadDR[2]) + toTape6->biquadDR[7];
			toTape6->biquadDR[7] = (inputSampleR * toTape6->biquadDR[3]) - (tempSample * toTape6->biquadDR[5]) + toTape6->biquadDR[8];
			toTape6->biquadDR[8] = (inputSampleR * toTape6->biquadDR[4]) - (tempSample * toTape6->biquadDR[6]);
			inputSampleR = tempSample; //interleaved biquad
			if (inputSampleR > 1.0) inputSampleR = 1.0;
			if (inputSampleR < -1.0) inputSampleR = -1.0;
			inputSampleR = asin(inputSampleR);
		}
		toTape6->flip = !toTape6->flip;

		double groundSampleL = vibDrySampleL - inputSampleL; //set up UnBox on fluttered audio
		double groundSampleR = vibDrySampleR - inputSampleR; //set up UnBox on fluttered audio

		if (inputgain > 1.0) {
			inputSampleL *= inputgain;
			inputSampleR *= inputgain;
		}

		double applySoften = fabs(HighsSampleL) * 1.57079633;
		if (applySoften > 1.57079633) applySoften = 1.57079633;
		applySoften = 1 - cos(applySoften);
		if (HighsSampleL > 0) inputSampleL -= applySoften;
		if (HighsSampleL < 0) inputSampleL += applySoften;
		//apply Soften depending on polarity
		applySoften = fabs(HighsSampleR) * 1.57079633;
		if (applySoften > 1.57079633) applySoften = 1.57079633;
		applySoften = 1 - cos(applySoften);
		if (HighsSampleR > 0) inputSampleR -= applySoften;
		if (HighsSampleR < 0) inputSampleR += applySoften;
		//apply Soften depending on polarity

		double suppress = (1.0 - fabs(inputSampleL)) * 0.00013;
		if (toTape6->iirHeadBumpAL > suppress) toTape6->iirHeadBumpAL -= suppress;
		if (toTape6->iirHeadBumpAL < -suppress) toTape6->iirHeadBumpAL += suppress;
		if (toTape6->iirHeadBumpBL > suppress) toTape6->iirHeadBumpBL -= suppress;
		if (toTape6->iirHeadBumpBL < -suppress) toTape6->iirHeadBumpBL += suppress;
		//restrain resonant quality of head bump algorithm
		suppress = (1.0 - fabs(inputSampleR)) * 0.00013;
		if (toTape6->iirHeadBumpAR > suppress) toTape6->iirHeadBumpAR -= suppress;
		if (toTape6->iirHeadBumpAR < -suppress) toTape6->iirHeadBumpAR += suppress;
		if (toTape6->iirHeadBumpBR > suppress) toTape6->iirHeadBumpBR -= suppress;
		if (toTape6->iirHeadBumpBR < -suppress) toTape6->iirHeadBumpBR += suppress;
		//restrain resonant quality of head bump algorithm

		inputSampleL += ((toTape6->iirHeadBumpAL + toTape6->iirHeadBumpBL) * HeadBumpControl);
		inputSampleR += ((toTape6->iirHeadBumpAR + toTape6->iirHeadBumpBR) * HeadBumpControl);
		//apply Fatten.

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		double mojo;
		mojo = pow(fabs(inputSampleL), 0.25);
		if (mojo > 0.0) inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo);
		//mojo is the one that flattens WAAAAY out very softly before wavefolding

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		mojo = pow(fabs(inputSampleR), 0.25);
		if (mojo > 0.0) inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo);
		//mojo is the one that flattens WAAAAY out very softly before wavefolding

		inputSampleL += groundSampleL; //apply UnBox processing
		inputSampleR += groundSampleR; //apply UnBox processing

		if (outputgain != 1.0) {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
		}

		if (toTape6->lastSampleL >= refclip) {
			if (inputSampleL < refclip) toTape6->lastSampleL = ((refclip * softness) + (inputSampleL * (1.0 - softness)));
			else toTape6->lastSampleL = refclip;
		}

		if (toTape6->lastSampleL <= -refclip) {
			if (inputSampleL > -refclip) toTape6->lastSampleL = ((-refclip * softness) + (inputSampleL * (1.0 - softness)));
			else toTape6->lastSampleL = -refclip;
		}

		if (inputSampleL > refclip) {
			if (toTape6->lastSampleL < refclip) inputSampleL = ((refclip * softness) + (toTape6->lastSampleL * (1.0 - softness)));
			else inputSampleL = refclip;
		}

		if (inputSampleL < -refclip) {
			if (toTape6->lastSampleL > -refclip) inputSampleL = ((-refclip * softness) + (toTape6->lastSampleL * (1.0 - softness)));
			else inputSampleL = -refclip;
		}
		toTape6->lastSampleL = inputSampleL; //end ADClip L


		if (toTape6->lastSampleR >= refclip) {
			if (inputSampleR < refclip) toTape6->lastSampleR = ((refclip * softness) + (inputSampleR * (1.0 - softness)));
			else toTape6->lastSampleR = refclip;
		}

		if (toTape6->lastSampleR <= -refclip) {
			if (inputSampleR > -refclip) toTape6->lastSampleR = ((-refclip * softness) + (inputSampleR * (1.0 - softness)));
			else toTape6->lastSampleR = -refclip;
		}

		if (inputSampleR > refclip) {
			if (toTape6->lastSampleR < refclip) inputSampleR = ((refclip * softness) + (toTape6->lastSampleR * (1.0 - softness)));
			else inputSampleR = refclip;
		}

		if (inputSampleR < -refclip) {
			if (toTape6->lastSampleR > -refclip) inputSampleR = ((-refclip * softness) + (toTape6->lastSampleR * (1.0 - softness)));
			else inputSampleR = -refclip;
		}
		toTape6->lastSampleR = inputSampleR; //end ADClip R

		if (inputSampleL > refclip) inputSampleL = refclip;
		if (inputSampleL < -refclip) inputSampleL = -refclip;
		//final iron bar
		if (inputSampleR > refclip) inputSampleR = refclip;
		if (inputSampleR < -refclip) inputSampleR = -refclip;
		//final iron bar

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		toTape6->fpdL ^= toTape6->fpdL << 13;
		toTape6->fpdL ^= toTape6->fpdL >> 17;
		toTape6->fpdL ^= toTape6->fpdL << 5;
		inputSampleL += (((double)toTape6->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		toTape6->fpdR ^= toTape6->fpdR << 13;
		toTape6->fpdR ^= toTape6->fpdR >> 17;
		toTape6->fpdR ^= toTape6->fpdR << 5;
		inputSampleR += (((double)toTape6->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TOTAPE6_URI,
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
