#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define AIR2_URI "https://hannesbraun.net/ns/lv2/airwindows/air2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	HISS = 4,
	GLITTER = 5,
	AIR = 6,
	SILK = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* hiss;
	const float* glitter;
	const float* air;
	const float* silk;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double lastRefL[10];
	double lastRefR[10];
	int cycle;

	double airPrevAL;
	double airEvenAL;
	double airOddAL;
	double airFactorAL;
	double airPrevBL;
	double airEvenBL;
	double airOddBL;
	double airFactorBL;
	double airPrevCL;
	double airEvenCL;
	double airOddCL;
	double airFactorCL;
	double tripletPrevL;
	double tripletMidL;
	double tripletAL;
	double tripletBL;
	double tripletCL;
	double tripletFactorL;

	double airPrevAR;
	double airEvenAR;
	double airOddAR;
	double airFactorAR;
	double airPrevBR;
	double airEvenBR;
	double airOddBR;
	double airFactorBR;
	double airPrevCR;
	double airEvenCR;
	double airOddCR;
	double airFactorCR;
	double tripletPrevR;
	double tripletMidR;
	double tripletAR;
	double tripletBR;
	double tripletCR;
	double tripletFactorR;

	bool flipA;
	bool flipB;
	bool flop;
	int count;
	double postsine;
} Air2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Air2* air2 = (Air2*) calloc(1, sizeof(Air2));
	air2->sampleRate = rate;
	return (LV2_Handle) air2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Air2* air2 = (Air2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			air2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			air2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			air2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			air2->output[1] = (float*) data;
			break;
		case HISS:
			air2->hiss = (const float*) data;
			break;
		case GLITTER:
			air2->glitter = (const float*) data;
			break;
		case AIR:
			air2->air = (const float*) data;
			break;
		case SILK:
			air2->silk = (const float*) data;
			break;
		case DRY_WET:
			air2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Air2* air2 = (Air2*) instance;

	for (int c = 0; c < 9; c++) {
		air2->lastRefL[c] = 0.0;
		air2->lastRefR[c] = 0.0;
	}
	air2->cycle = 0;

	air2->airPrevAL = 0.0;
	air2->airEvenAL = 0.0;
	air2->airOddAL = 0.0;
	air2->airFactorAL = 0.0;
	air2->airPrevBL = 0.0;
	air2->airEvenBL = 0.0;
	air2->airOddBL = 0.0;
	air2->airFactorBL = 0.0;
	air2->airPrevCL = 0.0;
	air2->airEvenCL = 0.0;
	air2->airOddCL = 0.0;
	air2->airFactorCL = 0.0;
	air2->tripletPrevL = 0.0;
	air2->tripletMidL = 0.0;
	air2->tripletAL = 0.0;
	air2->tripletBL = 0.0;
	air2->tripletCL = 0.0;
	air2->tripletFactorL = 0.0;

	air2->airPrevAR = 0.0;
	air2->airEvenAR = 0.0;
	air2->airOddAR = 0.0;
	air2->airFactorAR = 0.0;
	air2->airPrevBR = 0.0;
	air2->airEvenBR = 0.0;
	air2->airOddBR = 0.0;
	air2->airFactorBR = 0.0;
	air2->airPrevCR = 0.0;
	air2->airEvenCR = 0.0;
	air2->airOddCR = 0.0;
	air2->airFactorCR = 0.0;
	air2->tripletPrevR = 0.0;
	air2->tripletMidR = 0.0;
	air2->tripletAR = 0.0;
	air2->tripletBR = 0.0;
	air2->tripletCR = 0.0;
	air2->tripletFactorR = 0.0;

	air2->flipA = false;
	air2->flipB = false;
	air2->flop = false;
	air2->count = 1;
	air2->postsine = sin(1.0);

	air2->fpdL = 1.0;
	while (air2->fpdL < 16386) air2->fpdL = rand() * UINT32_MAX;
	air2->fpdR = 1.0;
	while (air2->fpdR < 16386) air2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Air2* air2 = (Air2*) instance;

	const float* in1 = air2->input[0];
	const float* in2 = air2->input[1];
	float* out1 = air2->output[0];
	float* out2 = air2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= air2->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (air2->cycle > cycleEnd - 1) air2->cycle = cycleEnd - 1; // sanity check

	double hiIntensity = *air2->hiss;
	double tripletIntensity = *air2->glitter;
	double airIntensity = *air2->air;
	if (hiIntensity < 0.0) hiIntensity *= 0.57525;
	if (tripletIntensity < 0.0) tripletIntensity *= 0.71325;
	if (airIntensity < 0.0) airIntensity *= 0.5712;
	hiIntensity = -pow(hiIntensity, 3);
	tripletIntensity = -pow(tripletIntensity, 3);
	airIntensity = -pow(airIntensity, 3);
	double hiQ = 1.5 + fabs(hiIntensity * 0.5);
	double tripletQ = 1.5 + fabs(tripletIntensity * 0.5);
	double airQ = 1.5 + fabs(airIntensity * 0.5);
	double intensity = (pow(*air2->silk, 3) * 4.0) + 0.0001;
	double mix = *air2->dryWet;
	double drymix = (1.0 - mix) * 4.0;
	if (drymix > 1.0) drymix = 1.0;

	// all types of air band are running in parallel, not series

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = air2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = air2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		air2->cycle++;
		if (air2->cycle == cycleEnd) { // hit the end point and we do an Air sample
			double correctionL = 0.0;
			double correctionR = 0.0;
			if (fabs(hiIntensity) > 0.0001) {
				air2->airFactorCL = air2->airPrevCL - inputSampleL;
				air2->airFactorCR = air2->airPrevCR - inputSampleR;
				if (air2->flop) {
					air2->airEvenCL += air2->airFactorCL;
					air2->airOddCL -= air2->airFactorCL;
					air2->airFactorCL = air2->airEvenCL * hiIntensity;

					air2->airEvenCR += air2->airFactorCR;
					air2->airOddCR -= air2->airFactorCR;
					air2->airFactorCR = air2->airEvenCR * hiIntensity;
				} else {
					air2->airOddCL += air2->airFactorCL;
					air2->airEvenCL -= air2->airFactorCL;
					air2->airFactorCL = air2->airOddCL * hiIntensity;

					air2->airOddCR += air2->airFactorCR;
					air2->airEvenCR -= air2->airFactorCR;
					air2->airFactorCR = air2->airOddCR * hiIntensity;
				}
				air2->airOddCL = (air2->airOddCL - ((air2->airOddCL - air2->airEvenCL) / 256.0)) / hiQ;
				air2->airEvenCL = (air2->airEvenCL - ((air2->airEvenCL - air2->airOddCL) / 256.0)) / hiQ;
				air2->airPrevCL = inputSampleL;
				correctionL = correctionL + air2->airFactorCL;

				air2->airOddCR = (air2->airOddCR - ((air2->airOddCR - air2->airEvenCR) / 256.0)) / hiQ;
				air2->airEvenCR = (air2->airEvenCR - ((air2->airEvenCR - air2->airOddCR) / 256.0)) / hiQ;
				air2->airPrevCR = inputSampleR;
				correctionR = correctionR + air2->airFactorCR;
				air2->flop = !air2->flop;
			} // 22k

			if (fabs(tripletIntensity) > 0.0001) {
				air2->tripletFactorL = air2->tripletPrevL - inputSampleL;
				air2->tripletFactorR = air2->tripletPrevR - inputSampleR;
				if (air2->count < 1 || air2->count > 3) air2->count = 1;
				switch (air2->count) {
					case 1:
						air2->tripletAL += air2->tripletFactorL;
						air2->tripletCL -= air2->tripletFactorL;
						air2->tripletFactorL = air2->tripletAL * tripletIntensity;
						air2->tripletPrevL = air2->tripletMidL;
						air2->tripletMidL = inputSampleL;

						air2->tripletAR += air2->tripletFactorR;
						air2->tripletCR -= air2->tripletFactorR;
						air2->tripletFactorR = air2->tripletAR * tripletIntensity;
						air2->tripletPrevR = air2->tripletMidR;
						air2->tripletMidR = inputSampleR;
						break;
					case 2:
						air2->tripletBL += air2->tripletFactorL;
						air2->tripletAL -= air2->tripletFactorL;
						air2->tripletFactorL = air2->tripletBL * tripletIntensity;
						air2->tripletPrevL = air2->tripletMidL;
						air2->tripletMidL = inputSampleL;

						air2->tripletBR += air2->tripletFactorR;
						air2->tripletAR -= air2->tripletFactorR;
						air2->tripletFactorR = air2->tripletBR * tripletIntensity;
						air2->tripletPrevR = air2->tripletMidR;
						air2->tripletMidR = inputSampleR;
						break;
					case 3:
						air2->tripletCL += air2->tripletFactorL;
						air2->tripletBL -= air2->tripletFactorL;
						air2->tripletFactorL = air2->tripletCL * tripletIntensity;
						air2->tripletPrevL = air2->tripletMidL;
						air2->tripletMidL = inputSampleL;

						air2->tripletCR += air2->tripletFactorR;
						air2->tripletBR -= air2->tripletFactorR;
						air2->tripletFactorR = air2->tripletCR * tripletIntensity;
						air2->tripletPrevR = air2->tripletMidR;
						air2->tripletMidR = inputSampleR;
						break;
				}
				air2->tripletAL /= tripletQ;
				air2->tripletBL /= tripletQ;
				air2->tripletCL /= tripletQ;
				air2->tripletAR /= tripletQ;
				air2->tripletBR /= tripletQ;
				air2->tripletCR /= tripletQ;
				correctionL = correctionL + air2->tripletFactorL;
				correctionR = correctionR + air2->tripletFactorR;
				air2->count++;
			} // 15K

			if (fabs(airIntensity) > 0.0001) {
				if (air2->flop) {
					air2->airFactorAL = air2->airPrevAL - inputSampleL;
					air2->airFactorAR = air2->airPrevAR - inputSampleR;
					if (air2->flipA) {
						air2->airEvenAL += air2->airFactorAL;
						air2->airOddAL -= air2->airFactorAL;
						air2->airFactorAL = air2->airEvenAL * airIntensity;
						air2->airEvenAR += air2->airFactorAR;
						air2->airOddAR -= air2->airFactorAR;
						air2->airFactorAR = air2->airEvenAR * airIntensity;
					} else {
						air2->airOddAL += air2->airFactorAL;
						air2->airEvenAL -= air2->airFactorAL;
						air2->airFactorAL = air2->airOddAL * airIntensity;
						air2->airOddAR += air2->airFactorAR;
						air2->airEvenAR -= air2->airFactorAR;
						air2->airFactorAR = air2->airOddAR * airIntensity;
					}
					air2->airOddAL = (air2->airOddAL - ((air2->airOddAL - air2->airEvenAL) / 256.0)) / airQ;
					air2->airEvenAL = (air2->airEvenAL - ((air2->airEvenAL - air2->airOddAL) / 256.0)) / airQ;
					air2->airPrevAL = inputSampleL;
					correctionL = correctionL + air2->airFactorAL;

					air2->airOddAR = (air2->airOddAR - ((air2->airOddAR - air2->airEvenAR) / 256.0)) / airQ;
					air2->airEvenAR = (air2->airEvenAR - ((air2->airEvenAR - air2->airOddAR) / 256.0)) / airQ;
					air2->airPrevAR = inputSampleR;
					correctionR = correctionR + air2->airFactorAR;
					air2->flipA = !air2->flipA;
				} else {
					air2->airFactorBL = air2->airPrevBL - inputSampleL;
					air2->airFactorBR = air2->airPrevBR - inputSampleR;
					if (air2->flipB) {
						air2->airEvenBL += air2->airFactorBL;
						air2->airOddBL -= air2->airFactorBL;
						air2->airFactorBL = air2->airEvenBL * airIntensity;
						air2->airEvenBR += air2->airFactorBR;
						air2->airOddBR -= air2->airFactorBR;
						air2->airFactorBR = air2->airEvenBR * airIntensity;
					} else {
						air2->airOddBL += air2->airFactorBL;
						air2->airEvenBL -= air2->airFactorBL;
						air2->airFactorBL = air2->airOddBL * airIntensity;
						air2->airOddBR += air2->airFactorBR;
						air2->airEvenBR -= air2->airFactorBR;
						air2->airFactorBR = air2->airOddBR * airIntensity;
					}
					air2->airOddBL = (air2->airOddBL - ((air2->airOddBL - air2->airEvenBL) / 256.0)) / airQ;
					air2->airEvenBL = (air2->airEvenBL - ((air2->airEvenBL - air2->airOddBL) / 256.0)) / airQ;
					air2->airPrevBL = inputSampleL;
					correctionL = correctionL + air2->airFactorBL;

					air2->airOddBR = (air2->airOddBR - ((air2->airOddBR - air2->airEvenBR) / 256.0)) / airQ;
					air2->airEvenBR = (air2->airEvenBR - ((air2->airEvenBR - air2->airOddBR) / 256.0)) / airQ;
					air2->airPrevBR = inputSampleR;
					correctionR = correctionR + air2->airFactorBR;
					air2->flipB = !air2->flipB;
				}
			} // 11K one

			correctionL *= intensity;
			correctionL -= 1.0;
			double bridgerectifier = fabs(correctionL);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (correctionL > 0) correctionL = bridgerectifier;
			else correctionL = -bridgerectifier;
			correctionL += air2->postsine;
			correctionL /= intensity;

			correctionR *= intensity;
			correctionR -= 1.0;
			bridgerectifier = fabs(correctionR);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (correctionR > 0) correctionR = bridgerectifier;
			else correctionR = -bridgerectifier;
			correctionR += air2->postsine;
			correctionR /= intensity;

			inputSampleL = correctionL * 4.0 * mix;
			inputSampleR = correctionR * 4.0 * mix;

			if (cycleEnd == 4) {
				air2->lastRefL[0] = air2->lastRefL[4]; // start from previous last
				air2->lastRefL[2] = (air2->lastRefL[0] + inputSampleL) / 2; // half
				air2->lastRefL[1] = (air2->lastRefL[0] + air2->lastRefL[2]) / 2; // one quarter
				air2->lastRefL[3] = (air2->lastRefL[2] + inputSampleL) / 2; // three quarters
				air2->lastRefL[4] = inputSampleL; // full
				air2->lastRefR[0] = air2->lastRefR[4]; // start from previous last
				air2->lastRefR[2] = (air2->lastRefR[0] + inputSampleR) / 2; // half
				air2->lastRefR[1] = (air2->lastRefR[0] + air2->lastRefR[2]) / 2; // one quarter
				air2->lastRefR[3] = (air2->lastRefR[2] + inputSampleR) / 2; // three quarters
				air2->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				air2->lastRefL[0] = air2->lastRefL[3]; // start from previous last
				air2->lastRefL[2] = (air2->lastRefL[0] + air2->lastRefL[0] + inputSampleL) / 3; // third
				air2->lastRefL[1] = (air2->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				air2->lastRefL[3] = inputSampleL; // full
				air2->lastRefR[0] = air2->lastRefR[3]; // start from previous last
				air2->lastRefR[2] = (air2->lastRefR[0] + air2->lastRefR[0] + inputSampleR) / 3; // third
				air2->lastRefR[1] = (air2->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				air2->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				air2->lastRefL[0] = air2->lastRefL[2]; // start from previous last
				air2->lastRefL[1] = (air2->lastRefL[0] + inputSampleL) / 2; // half
				air2->lastRefL[2] = inputSampleL; // full
				air2->lastRefR[0] = air2->lastRefR[2]; // start from previous last
				air2->lastRefR[1] = (air2->lastRefR[0] + inputSampleR) / 2; // half
				air2->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				air2->lastRefL[0] = inputSampleL;
				air2->lastRefR[0] = inputSampleR;
			}
			air2->cycle = 0; // reset
			inputSampleL = air2->lastRefL[air2->cycle];
			inputSampleR = air2->lastRefR[air2->cycle];
		} else {
			inputSampleL = air2->lastRefL[air2->cycle];
			inputSampleR = air2->lastRefR[air2->cycle];
			// we are going through our references now
		}
		switch (cycleEnd) // multi-pole average using lastRef[] variables
		{
			case 4:
				air2->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + air2->lastRefL[7]) * 0.5;
				air2->lastRefL[7] = air2->lastRefL[8]; // continue, do not break
				air2->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + air2->lastRefR[7]) * 0.5;
				air2->lastRefR[7] = air2->lastRefR[8]; // continue, do not break
			case 3:
				air2->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + air2->lastRefL[6]) * 0.5;
				air2->lastRefL[6] = air2->lastRefL[8]; // continue, do not break
				air2->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + air2->lastRefR[6]) * 0.5;
				air2->lastRefR[6] = air2->lastRefR[8]; // continue, do not break
			case 2:
				air2->lastRefL[8] = inputSampleL;
				inputSampleL = (inputSampleL + air2->lastRefL[5]) * 0.5;
				air2->lastRefL[5] = air2->lastRefL[8]; // continue, do not break
				air2->lastRefR[8] = inputSampleR;
				inputSampleR = (inputSampleR + air2->lastRefR[5]) * 0.5;
				air2->lastRefR[5] = air2->lastRefR[8]; // continue, do not break
			case 1:
				break; // no further averaging
		}

		if (drymix < 1.0) {
			drySampleL *= drymix;
			drySampleR *= drymix;
		}
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		air2->fpdL ^= air2->fpdL << 13;
		air2->fpdL ^= air2->fpdL >> 17;
		air2->fpdL ^= air2->fpdL << 5;
		inputSampleL += (((double) air2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		air2->fpdR ^= air2->fpdR << 13;
		air2->fpdR ^= air2->fpdR >> 17;
		air2->fpdR ^= air2->fpdR << 5;
		inputSampleR += (((double) air2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	AIR2_URI,
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
