#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ADT_URI "https://hannesbraun.net/ns/lv2/airwindows/adt"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	HEADROOM = 4,
	A_DELAY = 5,
	A_LEVEL = 6,
	B_DELAY = 7,
	B_LEVEL = 8,
	OUTPUT = 9
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* headroom;
	const float* aDelay;
	const float* aLevel;
	const float* bDelay;
	const float* bLevel;
	const float* outputGain;

	double pL[10000];
	double pR[10000];
	int gcount;
	double offsetA;
	double offsetB;

	uint32_t fpdL;
	uint32_t fpdR;
} ADT;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ADT* adt = (ADT*) calloc(1, sizeof(ADT));
	return (LV2_Handle) adt;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ADT* adt = (ADT*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			adt->input[0] = (const float*) data;
			break;
		case INPUT_R:
			adt->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			adt->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			adt->output[1] = (float*) data;
			break;
		case HEADROOM:
			adt->headroom = (const float*) data;
			break;
		case A_DELAY:
			adt->aDelay = (const float*) data;
			break;
		case A_LEVEL:
			adt->aLevel = (const float*) data;
			break;
		case B_DELAY:
			adt->bDelay = (const float*) data;
			break;
		case B_LEVEL:
			adt->bLevel = (const float*) data;
			break;
		case OUTPUT:
			adt->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ADT* adt = (ADT*) instance;
	for (int count = 0; count < 9999; count++) {
		adt->pL[count] = 0;
		adt->pR[count] = 0;
	}
	adt->offsetA = 9001;
	adt->offsetB = 9001; //  :D
	adt->gcount = 0;

	adt->fpdL = 1.0;
	while (adt->fpdL < 16386) adt->fpdL = rand() * UINT32_MAX;
	adt->fpdR = 1.0;
	while (adt->fpdR < 16386) adt->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ADT* adt = (ADT*) instance;

	const float* in1 = adt->input[0];
	const float* in2 = adt->input[1];
	float* out1 = adt->output[0];
	float* out2 = adt->output[1];

	double gain = (*adt->headroom * 0.5) * 1.272;
	double targetA = pow(*adt->aDelay, 4) * 4790.0;
	double fractionA;
	double minusA;
	double intensityA = *adt->aLevel * 0.5;
	// first delay
	double targetB = (pow(*adt->bDelay, 4) * 4790.0);
	double fractionB;
	double minusB;
	double intensityB = *adt->bLevel * 0.5;
	// second delay
	double output = *adt->outputGain;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = adt->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = adt->fpdR * 1.18e-17;

		if (fabs(adt->offsetA - targetA) > 1000) adt->offsetA = targetA;
		adt->offsetA = ((adt->offsetA * 999.0) + targetA) / 1000.0;
		fractionA = adt->offsetA - floor(adt->offsetA);
		minusA = 1.0 - fractionA;

		if (fabs(adt->offsetB - targetB) > 1000) adt->offsetB = targetB;
		adt->offsetB = ((adt->offsetB * 999.0) + targetB) / 1000.0;
		fractionB = adt->offsetB - floor(adt->offsetB);
		minusB = 1.0 - fractionB;
		// chase delay taps for smooth action

		if (gain > 0) {
			inputSampleL /= gain;
			inputSampleR /= gain;
		}

		if (inputSampleL > 1.2533141373155) inputSampleL = 1.2533141373155;
		if (inputSampleL < -1.2533141373155) inputSampleL = -1.2533141373155;
		if (inputSampleR > 1.2533141373155) inputSampleR = 1.2533141373155;
		if (inputSampleR < -1.2533141373155) inputSampleR = -1.2533141373155;

		inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
		inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
		// Spiral: lean out the sound a little when decoded by ConsoleBuss

		if (adt->gcount < 1 || adt->gcount > 4800) {
			adt->gcount = 4800;
		}
		int count = adt->gcount;
		double totalL = 0.0;
		double totalR = 0.0;
		double tempL;
		double tempR;
		adt->pL[count + 4800] = adt->pL[count] = inputSampleL;
		adt->pR[count + 4800] = adt->pR[count] = inputSampleR;
		// double buffer

		if (intensityA != 0.0) {
			count = (int) (adt->gcount + floor(adt->offsetA));

			tempL = (adt->pL[count] * minusA); // less as value moves away from .0
			tempL += adt->pL[count + 1]; // we can assume always using this in one way or another?
			tempL += (adt->pL[count + 2] * fractionA); // greater as value moves away from .0
			tempL -= (((adt->pL[count] - adt->pL[count + 1]) - (adt->pL[count + 1] - adt->pL[count + 2])) / 50); // interpolation hacks 'r us
			totalL += (tempL * intensityA);

			tempR = (adt->pR[count] * minusA); // less as value moves away from .0
			tempR += adt->pR[count + 1]; // we can assume always using this in one way or another?
			tempR += (adt->pR[count + 2] * fractionA); // greater as value moves away from .0
			tempR -= (((adt->pR[count] - adt->pR[count + 1]) - (adt->pR[count + 1] - adt->pR[count + 2])) / 50); // interpolation hacks 'r us
			totalR += (tempR * intensityA);
		}

		if (intensityB != 0.0) {
			count = (int) (adt->gcount + floor(adt->offsetB));

			tempL = (adt->pL[count] * minusB); // less as value moves away from .0
			tempL += adt->pL[count + 1]; // we can assume always using this in one way or another?
			tempL += (adt->pL[count + 2] * fractionB); // greater as value moves away from .0
			tempL -= (((adt->pL[count] - adt->pL[count + 1]) - (adt->pL[count + 1] - adt->pL[count + 2])) / 50); // interpolation hacks 'r us
			totalL += (tempL * intensityB);

			tempR = (adt->pR[count] * minusB); // less as value moves away from .0
			tempR += adt->pR[count + 1]; // we can assume always using this in one way or another?
			tempR += (adt->pR[count + 2] * fractionB); // greater as value moves away from .0
			tempR -= (((adt->pR[count] - adt->pR[count + 1]) - (adt->pR[count + 1] - adt->pR[count + 2])) / 50); // interpolation hacks 'r us
			totalR += (tempR * intensityB);
		}

		adt->gcount--;
		// still scrolling through the samples, remember

		inputSampleL += totalL;
		inputSampleR += totalR;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		// amplitude aspect

		inputSampleL *= gain;
		inputSampleR *= gain;

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		adt->fpdL ^= adt->fpdL << 13;
		adt->fpdL ^= adt->fpdL >> 17;
		adt->fpdL ^= adt->fpdL << 5;
		inputSampleL += (((double) adt->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		adt->fpdR ^= adt->fpdR << 13;
		adt->fpdR ^= adt->fpdR >> 17;
		adt->fpdR ^= adt->fpdR << 5;
		inputSampleR += (((double) adt->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ADT_URI,
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
