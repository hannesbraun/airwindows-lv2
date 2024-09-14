#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define POP_URI "https://hannesbraun.net/ns/lv2/airwindows/pop"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INTENSITY = 4,
	OUTPUT = 5,
	DRY_WET = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* intensity;
	const float* outputGain;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;

	double muVaryL;
	double muAttackL;
	double muNewSpeedL;
	double muSpeedAL;
	double muSpeedBL;
	double muCoefficientAL;
	double muCoefficientBL;
	double thickenL;
	double previousL;
	double previous2L;
	double previous3L;
	double previous4L;
	double previous5L;
	double dL[10001];

	double muVaryR;
	double muAttackR;
	double muNewSpeedR;
	double muSpeedAR;
	double muSpeedBR;
	double muCoefficientAR;
	double muCoefficientBR;
	double thickenR;
	double previousR;
	double previous2R;
	double previous3R;
	double previous4R;
	double previous5R;
	double dR[10001];

	int delay;
	bool flip;
} Pop;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Pop* pop = (Pop*) calloc(1, sizeof(Pop));
	pop->sampleRate = rate;
	return (LV2_Handle) pop;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Pop* pop = (Pop*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pop->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pop->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pop->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pop->output[1] = (float*) data;
			break;
		case INTENSITY:
			pop->intensity = (const float*) data;
			break;
		case OUTPUT:
			pop->outputGain = (const float*) data;
			break;
		case DRY_WET:
			pop->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Pop* pop = (Pop*) instance;

	pop->fpdL = 1.0;
	while (pop->fpdL < 16386) pop->fpdL = rand() * UINT32_MAX;
	pop->fpdR = 1.0;
	while (pop->fpdR < 16386) pop->fpdR = rand() * UINT32_MAX;

	for (int count = 0; count < 10000; count++) {
		pop->dL[count] = 0;
		pop->dR[count] = 0;
	}
	pop->delay = 0;
	pop->flip = false;

	pop->muSpeedAL = 10000;
	pop->muSpeedBL = 10000;
	pop->muCoefficientAL = 1;
	pop->muCoefficientBL = 1;
	pop->thickenL = 1;
	pop->muVaryL = 1;
	pop->previousL = 0.0;
	pop->previous2L = 0.0;
	pop->previous3L = 0.0;
	pop->previous4L = 0.0;
	pop->previous5L = 0.0;

	pop->muSpeedAR = 10000;
	pop->muSpeedBR = 10000;
	pop->muCoefficientAR = 1;
	pop->muCoefficientBR = 1;
	pop->thickenR = 1;
	pop->muVaryR = 1;
	pop->previousR = 0.0;
	pop->previous2R = 0.0;
	pop->previous3R = 0.0;
	pop->previous4R = 0.0;
	pop->previous5R = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Pop* pop = (Pop*) instance;

	const float* in1 = pop->input[0];
	const float* in2 = pop->input[1];
	float* out1 = pop->output[0];
	float* out2 = pop->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= pop->sampleRate;

	const float intensity = *pop->intensity;
	double highGainOffset = pow(intensity, 2) * 0.023;
	double threshold = 1.001 - (1.0 - pow(1.0 - intensity, 5));
	double muMakeupGain = sqrt(1.0 / threshold);
	// gain settings around threshold
	double release = (intensity * 100000.0) + 300000.0;
	int maxdelay = (int) (1450.0 * overallscale);
	if (maxdelay > 9999) maxdelay = 9999;
	release /= overallscale;
	double fastest = sqrt(release);
	// speed settings around release
	double output = *pop->outputGain;
	double wet = *pop->dryWet;
	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pop->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pop->fpdR * 1.18e-17;

		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		pop->dL[pop->delay] = inputSampleL;
		pop->dR[pop->delay] = inputSampleR;
		pop->delay--;
		if (pop->delay < 0 || pop->delay > maxdelay) {
			pop->delay = maxdelay;
		}
		// yes this is a second bounds check. it's cheap, check EVERY time
		inputSampleL = (inputSampleL * pop->thickenL) + (pop->dL[pop->delay] * (1.0 - pop->thickenL));
		inputSampleR = (inputSampleR * pop->thickenR) + (pop->dR[pop->delay] * (1.0 - pop->thickenR));

		double lowestSampleL = inputSampleL;
		if (fabs(inputSampleL) > fabs(pop->previousL)) lowestSampleL = pop->previousL;
		if (fabs(lowestSampleL) > fabs(pop->previous2L)) lowestSampleL = (lowestSampleL + pop->previous2L) / 1.99;
		if (fabs(lowestSampleL) > fabs(pop->previous3L)) lowestSampleL = (lowestSampleL + pop->previous3L) / 1.98;
		if (fabs(lowestSampleL) > fabs(pop->previous4L)) lowestSampleL = (lowestSampleL + pop->previous4L) / 1.97;
		if (fabs(lowestSampleL) > fabs(pop->previous5L)) lowestSampleL = (lowestSampleL + pop->previous5L) / 1.96;
		pop->previous5L = pop->previous4L;
		pop->previous4L = pop->previous3L;
		pop->previous3L = pop->previous2L;
		pop->previous2L = pop->previousL;
		pop->previousL = inputSampleL;
		inputSampleL *= muMakeupGain;
		double punchinessL = 0.95 - fabs(inputSampleL * 0.08);
		if (punchinessL < 0.65) punchinessL = 0.65;

		double lowestSampleR = inputSampleR;
		if (fabs(inputSampleR) > fabs(pop->previousR)) lowestSampleR = pop->previousR;
		if (fabs(lowestSampleR) > fabs(pop->previous2R)) lowestSampleR = (lowestSampleR + pop->previous2R) / 1.99;
		if (fabs(lowestSampleR) > fabs(pop->previous3R)) lowestSampleR = (lowestSampleR + pop->previous3R) / 1.98;
		if (fabs(lowestSampleR) > fabs(pop->previous4R)) lowestSampleR = (lowestSampleR + pop->previous4R) / 1.97;
		if (fabs(lowestSampleR) > fabs(pop->previous5R)) lowestSampleR = (lowestSampleR + pop->previous5R) / 1.96;
		pop->previous5R = pop->previous4R;
		pop->previous4R = pop->previous3R;
		pop->previous3R = pop->previous2R;
		pop->previous2R = pop->previousR;
		pop->previousR = inputSampleR;
		inputSampleR *= muMakeupGain;
		double punchinessR = 0.95 - fabs(inputSampleR * 0.08);
		if (punchinessR < 0.65) punchinessR = 0.65;

		// adjust coefficients for L
		if (pop->flip) {
			if (fabs(lowestSampleL) > threshold) {
				pop->muVaryL = threshold / fabs(lowestSampleL);
				pop->muAttackL = sqrt(fabs(pop->muSpeedAL));
				pop->muCoefficientAL = pop->muCoefficientAL * (pop->muAttackL - 1.0);
				if (pop->muVaryL < threshold) {
					pop->muCoefficientAL = pop->muCoefficientAL + threshold;
				} else {
					pop->muCoefficientAL = pop->muCoefficientAL + pop->muVaryL;
				}
				pop->muCoefficientAL = pop->muCoefficientAL / pop->muAttackL;
			} else {
				pop->muCoefficientAL = pop->muCoefficientAL * ((pop->muSpeedAL * pop->muSpeedAL) - 1.0);
				pop->muCoefficientAL = pop->muCoefficientAL + 1.0;
				pop->muCoefficientAL = pop->muCoefficientAL / (pop->muSpeedAL * pop->muSpeedAL);
			}
			pop->muNewSpeedL = pop->muSpeedAL * (pop->muSpeedAL - 1);
			pop->muNewSpeedL = pop->muNewSpeedL + fabs(lowestSampleL * release) + fastest;
			pop->muSpeedAL = pop->muNewSpeedL / pop->muSpeedAL;
		} else {
			if (fabs(lowestSampleL) > threshold) {
				pop->muVaryL = threshold / fabs(lowestSampleL);
				pop->muAttackL = sqrt(fabs(pop->muSpeedBL));
				pop->muCoefficientBL = pop->muCoefficientBL * (pop->muAttackL - 1);
				if (pop->muVaryL < threshold) {
					pop->muCoefficientBL = pop->muCoefficientBL + threshold;
				} else {
					pop->muCoefficientBL = pop->muCoefficientBL + pop->muVaryL;
				}
				pop->muCoefficientBL = pop->muCoefficientBL / pop->muAttackL;
			} else {
				pop->muCoefficientBL = pop->muCoefficientBL * ((pop->muSpeedBL * pop->muSpeedBL) - 1.0);
				pop->muCoefficientBL = pop->muCoefficientBL + 1.0;
				pop->muCoefficientBL = pop->muCoefficientBL / (pop->muSpeedBL * pop->muSpeedBL);
			}
			pop->muNewSpeedL = pop->muSpeedBL * (pop->muSpeedBL - 1);
			pop->muNewSpeedL = pop->muNewSpeedL + fabs(lowestSampleL * release) + fastest;
			pop->muSpeedBL = pop->muNewSpeedL / pop->muSpeedBL;
		}
		// got coefficients, adjusted speeds for L

		// adjust coefficients for R
		if (pop->flip) {
			if (fabs(lowestSampleR) > threshold) {
				pop->muVaryR = threshold / fabs(lowestSampleR);
				pop->muAttackR = sqrt(fabs(pop->muSpeedAR));
				pop->muCoefficientAR = pop->muCoefficientAR * (pop->muAttackR - 1.0);
				if (pop->muVaryR < threshold) {
					pop->muCoefficientAR = pop->muCoefficientAR + threshold;
				} else {
					pop->muCoefficientAR = pop->muCoefficientAR + pop->muVaryR;
				}
				pop->muCoefficientAR = pop->muCoefficientAR / pop->muAttackR;
			} else {
				pop->muCoefficientAR = pop->muCoefficientAR * ((pop->muSpeedAR * pop->muSpeedAR) - 1.0);
				pop->muCoefficientAR = pop->muCoefficientAR + 1.0;
				pop->muCoefficientAR = pop->muCoefficientAR / (pop->muSpeedAR * pop->muSpeedAR);
			}
			pop->muNewSpeedR = pop->muSpeedAR * (pop->muSpeedAR - 1);
			pop->muNewSpeedR = pop->muNewSpeedR + fabs(lowestSampleR * release) + fastest;
			pop->muSpeedAR = pop->muNewSpeedR / pop->muSpeedAR;
		} else {
			if (fabs(lowestSampleR) > threshold) {
				pop->muVaryR = threshold / fabs(lowestSampleR);
				pop->muAttackR = sqrt(fabs(pop->muSpeedBR));
				pop->muCoefficientBR = pop->muCoefficientBR * (pop->muAttackR - 1);
				if (pop->muVaryR < threshold) {
					pop->muCoefficientBR = pop->muCoefficientBR + threshold;
				} else {
					pop->muCoefficientBR = pop->muCoefficientBR + pop->muVaryR;
				}
				pop->muCoefficientBR = pop->muCoefficientBR / pop->muAttackR;
			} else {
				pop->muCoefficientBR = pop->muCoefficientBR * ((pop->muSpeedBR * pop->muSpeedBR) - 1.0);
				pop->muCoefficientBR = pop->muCoefficientBR + 1.0;
				pop->muCoefficientBR = pop->muCoefficientBR / (pop->muSpeedBR * pop->muSpeedBR);
			}
			pop->muNewSpeedR = pop->muSpeedBR * (pop->muSpeedBR - 1);
			pop->muNewSpeedR = pop->muNewSpeedR + fabs(lowestSampleR * release) + fastest;
			pop->muSpeedBR = pop->muNewSpeedR / pop->muSpeedBR;
		}
		// got coefficients, adjusted speeds for R

		double coefficientL = highGainOffset;
		if (pop->flip) coefficientL += pow(pop->muCoefficientAL, 2);
		else coefficientL += pow(pop->muCoefficientBL, 2);
		inputSampleL *= coefficientL;
		pop->thickenL = (coefficientL / 5) + punchinessL; // 0.80;
		pop->thickenL = (1.0 - wet) + (wet * pop->thickenL);

		double coefficientR = highGainOffset;
		if (pop->flip) coefficientR += pow(pop->muCoefficientAR, 2);
		else coefficientR += pow(pop->muCoefficientBR, 2);
		inputSampleR *= coefficientR;
		pop->thickenR = (coefficientR / 5) + punchinessR; // 0.80;
		pop->thickenR = (1.0 - wet) + (wet * pop->thickenR);
		// applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		// applied gain correction to control output level- tends to constrain sound rather than inflate it

		double bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.2533141373155) bridgerectifier = 1.2533141373155;
		bridgerectifier = sin(bridgerectifier * fabs(bridgerectifier)) / ((fabs(bridgerectifier) == 0.0) ? 1 : fabs(bridgerectifier));
		// using Spiral instead of Density algorithm
		if (inputSampleL > 0) inputSampleL = (inputSampleL * coefficientL) + (bridgerectifier * (1 - coefficientL));
		else inputSampleL = (inputSampleL * coefficientL) - (bridgerectifier * (1 - coefficientL));
		// second stage of overdrive to prevent overs and allow bloody loud extremeness

		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.2533141373155) bridgerectifier = 1.2533141373155;
		bridgerectifier = sin(bridgerectifier * fabs(bridgerectifier)) / ((fabs(bridgerectifier) == 0.0) ? 1 : fabs(bridgerectifier));
		// using Spiral instead of Density algorithm
		if (inputSampleR > 0) inputSampleR = (inputSampleR * coefficientR) + (bridgerectifier * (1 - coefficientR));
		else inputSampleR = (inputSampleR * coefficientR) - (bridgerectifier * (1 - coefficientR));
		// second stage of overdrive to prevent overs and allow bloody loud extremeness

		pop->flip = !pop->flip;

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}
		if (wet < 1.0) {
			inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pop->fpdL ^= pop->fpdL << 13;
		pop->fpdL ^= pop->fpdL >> 17;
		pop->fpdL ^= pop->fpdL << 5;
		inputSampleL += (((double) pop->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pop->fpdR ^= pop->fpdR << 13;
		pop->fpdR ^= pop->fpdR >> 17;
		pop->fpdR ^= pop->fpdR << 5;
		inputSampleR += (((double) pop->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POP_URI,
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
