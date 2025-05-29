#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define AIR_URI "https://hannesbraun.net/ns/lv2/airwindows/air"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	K22_TAP = 4,
	K15_TAP = 5,
	K11_TAP = 6,
	FILTERS_Q = 7,
	OUTPUT_LEVEL = 8,
	DRY_WET = 9
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* k22Tap;
	const float* k15Tap;
	const float* k11Tap;
	const float* filtersQ;
	const float* outputLevel;
	const float* dryWet;

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

	uint32_t fpdL;
	uint32_t fpdR;
} Air;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Air* air = (Air*) calloc(1, sizeof(Air));
	return (LV2_Handle) air;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Air* air = (Air*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			air->input[0] = (const float*) data;
			break;
		case INPUT_R:
			air->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			air->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			air->output[1] = (float*) data;
			break;
		case K22_TAP:
			air->k22Tap = (const float*) data;
			break;
		case K15_TAP:
			air->k15Tap = (const float*) data;
			break;
		case K11_TAP:
			air->k11Tap = (const float*) data;
			break;
		case FILTERS_Q:
			air->filtersQ = (const float*) data;
			break;
		case OUTPUT_LEVEL:
			air->outputLevel = (const float*) data;
			break;
		case DRY_WET:
			air->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Air* air = (Air*) instance;

	air->airPrevAL = 0.0;
	air->airEvenAL = 0.0;
	air->airOddAL = 0.0;
	air->airFactorAL = 0.0;
	air->airPrevBL = 0.0;
	air->airEvenBL = 0.0;
	air->airOddBL = 0.0;
	air->airFactorBL = 0.0;
	air->airPrevCL = 0.0;
	air->airEvenCL = 0.0;
	air->airOddCL = 0.0;
	air->airFactorCL = 0.0;
	air->tripletPrevL = 0.0;
	air->tripletMidL = 0.0;
	air->tripletAL = 0.0;
	air->tripletBL = 0.0;
	air->tripletCL = 0.0;
	air->tripletFactorL = 0.0;

	air->airPrevAR = 0.0;
	air->airEvenAR = 0.0;
	air->airOddAR = 0.0;
	air->airFactorAR = 0.0;
	air->airPrevBR = 0.0;
	air->airEvenBR = 0.0;
	air->airOddBR = 0.0;
	air->airFactorBR = 0.0;
	air->airPrevCR = 0.0;
	air->airEvenCR = 0.0;
	air->airOddCR = 0.0;
	air->airFactorCR = 0.0;
	air->tripletPrevR = 0.0;
	air->tripletMidR = 0.0;
	air->tripletAR = 0.0;
	air->tripletBR = 0.0;
	air->tripletCR = 0.0;
	air->tripletFactorR = 0.0;

	air->flipA = false;
	air->flipB = false;
	air->flop = false;
	air->count = 1;

	air->fpdL = 1.0;
	while (air->fpdL < 16386) air->fpdL = rand() * UINT32_MAX;
	air->fpdR = 1.0;
	while (air->fpdR < 16386) air->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Air* air = (Air*) instance;

	const float* in1 = air->input[0];
	const float* in2 = air->input[1];
	float* out1 = air->output[0];
	float* out2 = air->output[1];

	double hiIntensity = -pow(*air->k22Tap, 3) * 2;
	double tripletintensity = -pow(*air->k15Tap, 3);
	double airIntensity = -pow(*air->k11Tap, 3) / 2;
	double filterQ = 2.1 - *air->filtersQ;
	double output = *air->outputLevel;
	double wet = *air->dryWet;
	double dry = 1.0 - wet;

	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;
	double correctionL;
	double correctionR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = air->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = air->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		correctionL = 0.0;
		correctionR = 0.0; // from here on down, please add L and R to the code

		if (air->count < 1 || air->count > 3) air->count = 1;
		air->tripletFactorL = air->tripletPrevL - inputSampleL;
		air->tripletFactorR = air->tripletPrevR - inputSampleR;
		switch (air->count) {
			case 1:
				air->tripletAL += air->tripletFactorL;
				air->tripletCL -= air->tripletFactorL;
				air->tripletFactorL = air->tripletAL * tripletintensity;
				air->tripletPrevL = air->tripletMidL;
				air->tripletMidL = inputSampleL;

				air->tripletAR += air->tripletFactorR;
				air->tripletCR -= air->tripletFactorR;
				air->tripletFactorR = air->tripletAR * tripletintensity;
				air->tripletPrevR = air->tripletMidR;
				air->tripletMidR = inputSampleR;
				break;
			case 2:
				air->tripletBL += air->tripletFactorL;
				air->tripletAL -= air->tripletFactorL;
				air->tripletFactorL = air->tripletBL * tripletintensity;
				air->tripletPrevL = air->tripletMidL;
				air->tripletMidL = inputSampleL;

				air->tripletBR += air->tripletFactorR;
				air->tripletAR -= air->tripletFactorR;
				air->tripletFactorR = air->tripletBR * tripletintensity;
				air->tripletPrevR = air->tripletMidR;
				air->tripletMidR = inputSampleR;
				break;
			case 3:
				air->tripletCL += air->tripletFactorL;
				air->tripletBL -= air->tripletFactorL;
				air->tripletFactorL = air->tripletCL * tripletintensity;
				air->tripletPrevL = air->tripletMidL;
				air->tripletMidL = inputSampleL;

				air->tripletCR += air->tripletFactorR;
				air->tripletBR -= air->tripletFactorR;
				air->tripletFactorR = air->tripletCR * tripletintensity;
				air->tripletPrevR = air->tripletMidR;
				air->tripletMidR = inputSampleR;
				break;
		}
		air->tripletAL /= filterQ;
		air->tripletBL /= filterQ;
		air->tripletCL /= filterQ;
		correctionL = correctionL + air->tripletFactorL;

		air->tripletAR /= filterQ;
		air->tripletBR /= filterQ;
		air->tripletCR /= filterQ;
		correctionR = correctionR + air->tripletFactorR;

		air->count++;
		// finished Triplet section- 15K

		if (air->flop) {
			air->airFactorAL = air->airPrevAL - inputSampleL;
			air->airFactorAR = air->airPrevAR - inputSampleR;
			if (air->flipA) {
				air->airEvenAL += air->airFactorAL;
				air->airOddAL -= air->airFactorAL;
				air->airFactorAL = air->airEvenAL * airIntensity;

				air->airEvenAR += air->airFactorAR;
				air->airOddAR -= air->airFactorAR;
				air->airFactorAR = air->airEvenAR * airIntensity;
			} else {
				air->airOddAL += air->airFactorAL;
				air->airEvenAL -= air->airFactorAL;
				air->airFactorAL = air->airOddAL * airIntensity;

				air->airOddAR += air->airFactorAR;
				air->airEvenAR -= air->airFactorAR;
				air->airFactorAR = air->airOddAR * airIntensity;
			}
			air->airOddAL = (air->airOddAL - ((air->airOddAL - air->airEvenAL) / 256.0)) / filterQ;
			air->airEvenAL = (air->airEvenAL - ((air->airEvenAL - air->airOddAL) / 256.0)) / filterQ;
			air->airPrevAL = inputSampleL;
			correctionL = correctionL + air->airFactorAL;

			air->airOddAR = (air->airOddAR - ((air->airOddAR - air->airEvenAR) / 256.0)) / filterQ;
			air->airEvenAR = (air->airEvenAR - ((air->airEvenAR - air->airOddAR) / 256.0)) / filterQ;
			air->airPrevAR = inputSampleR;
			correctionR = correctionR + air->airFactorAR;

			air->flipA = !air->flipA;
		} else {
			air->airFactorBL = air->airPrevBL - inputSampleL;
			air->airFactorBR = air->airPrevBR - inputSampleR;
			if (air->flipB) {
				air->airEvenBL += air->airFactorBL;
				air->airOddBL -= air->airFactorBL;
				air->airFactorBL = air->airEvenBL * airIntensity;

				air->airEvenBR += air->airFactorBR;
				air->airOddBR -= air->airFactorBR;
				air->airFactorBR = air->airEvenBR * airIntensity;
			} else {
				air->airOddBL += air->airFactorBL;
				air->airEvenBL -= air->airFactorBL;
				air->airFactorBL = air->airOddBL * airIntensity;

				air->airOddBR += air->airFactorBR;
				air->airEvenBR -= air->airFactorBR;
				air->airFactorBR = air->airOddBR * airIntensity;
			}
			air->airOddBL = (air->airOddBL - ((air->airOddBL - air->airEvenBL) / 256.0)) / filterQ;
			air->airEvenBL = (air->airEvenBL - ((air->airEvenBL - air->airOddBL) / 256.0)) / filterQ;
			air->airPrevBL = inputSampleL;
			correctionL = correctionL + air->airFactorBL;

			air->airOddBR = (air->airOddBR - ((air->airOddBR - air->airEvenBR) / 256.0)) / filterQ;
			air->airEvenBR = (air->airEvenBR - ((air->airEvenBR - air->airOddBR) / 256.0)) / filterQ;
			air->airPrevBR = inputSampleR;
			correctionR = correctionR + air->airFactorBR;

			air->flipB = !air->flipB;
		}
		// 11K one
		air->airFactorCL = air->airPrevCL - inputSampleL;
		air->airFactorCR = air->airPrevCR - inputSampleR;
		if (air->flop) {
			air->airEvenCL += air->airFactorCL;
			air->airOddCL -= air->airFactorCL;
			air->airFactorCL = air->airEvenCL * hiIntensity;

			air->airEvenCR += air->airFactorCR;
			air->airOddCR -= air->airFactorCR;
			air->airFactorCR = air->airEvenCR * hiIntensity;
		} else {
			air->airOddCL += air->airFactorCL;
			air->airEvenCL -= air->airFactorCL;
			air->airFactorCL = air->airOddCL * hiIntensity;

			air->airOddCR += air->airFactorCR;
			air->airEvenCR -= air->airFactorCR;
			air->airFactorCR = air->airOddCR * hiIntensity;
		}
		air->airOddCL = (air->airOddCL - ((air->airOddCL - air->airEvenCL) / 256.0)) / filterQ;
		air->airEvenCL = (air->airEvenCL - ((air->airEvenCL - air->airOddCL) / 256.0)) / filterQ;
		air->airPrevCL = inputSampleL;
		correctionL = correctionL + air->airFactorCL;

		air->airOddCR = (air->airOddCR - ((air->airOddCR - air->airEvenCR) / 256.0)) / filterQ;
		air->airEvenCR = (air->airEvenCR - ((air->airEvenCR - air->airOddCR) / 256.0)) / filterQ;
		air->airPrevCR = inputSampleR;
		correctionR = correctionR + air->airFactorCR;

		air->flop = !air->flop;

		inputSampleL += correctionL;
		inputSampleR += correctionR;

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}
		if (wet < 1.0) {
			inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
			inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
		}
		// nice little output stage template: if we have another scale of floating point
		// number, we really don't want to meaninglessly multiply that by 1.0.

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		air->fpdL ^= air->fpdL << 13;
		air->fpdL ^= air->fpdL >> 17;
		air->fpdL ^= air->fpdL << 5;
		inputSampleL += (((double) air->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		air->fpdR ^= air->fpdR << 13;
		air->fpdR ^= air->fpdR >> 17;
		air->fpdR ^= air->fpdR << 5;
		inputSampleR += (((double) air->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	AIR_URI,
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
