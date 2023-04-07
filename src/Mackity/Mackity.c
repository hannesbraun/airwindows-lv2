#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define MACKITY_URI "https://hannesbraun.net/ns/lv2/airwindows/mackity"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	IN_TRIM = 4,
	OUT_PAD = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* intrim;
	const float* outpad;

	double iirSampleAL;
	double iirSampleBL;
	double iirSampleAR;
	double iirSampleBR;
	double biquadA[15];
	double biquadB[15];

	uint32_t fpdL;
	uint32_t fpdR;
} Mackity;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Mackity* mackity = (Mackity*) calloc(1, sizeof(Mackity));
	mackity->sampleRate = rate;
	return (LV2_Handle) mackity;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Mackity* mackity = (Mackity*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			mackity->input[0] = (const float*) data;
			break;
		case INPUT_R:
			mackity->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			mackity->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			mackity->output[1] = (float*) data;
			break;
		case IN_TRIM:
			mackity->intrim = (const float*) data;
			break;
		case OUT_PAD:
			mackity->outpad = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Mackity* mackity = (Mackity*) instance;

	mackity->iirSampleAL = 0.0;
	mackity->iirSampleBL = 0.0;
	mackity->iirSampleAR = 0.0;
	mackity->iirSampleBR = 0.0;
	for (int x = 0; x < 15; x++) {
		mackity->biquadA[x] = 0.0;
		mackity->biquadB[x] = 0.0;
	}

	mackity->fpdL = 1.0;
	while (mackity->fpdL < 16386) mackity->fpdL = rand() * UINT32_MAX;
	mackity->fpdR = 1.0;
	while (mackity->fpdR < 16386) mackity->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Mackity* mackity = (Mackity*) instance;

	const float* in1 = mackity->input[0];
	const float* in2 = mackity->input[1];
	float* out1 = mackity->output[0];
	float* out2 = mackity->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= mackity->sampleRate;

	double inTrim = *mackity->intrim * 10.0;
	double outPad = *mackity->outpad;
	inTrim *= inTrim;

	double iirAmountA = 0.001860867 / overallscale;
	double iirAmountB = 0.000287496 / overallscale;

	mackity->biquadB[0] = mackity->biquadA[0] = 19160.0 / mackity->sampleRate;
	mackity->biquadA[1] = 0.431684981684982;
	mackity->biquadB[1] = 1.1582298;

	double K = tan(M_PI * mackity->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / mackity->biquadA[1] + K * K);
	mackity->biquadA[2] = K * K * norm;
	mackity->biquadA[3] = 2.0 * mackity->biquadA[2];
	mackity->biquadA[4] = mackity->biquadA[2];
	mackity->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	mackity->biquadA[6] = (1.0 - K / mackity->biquadA[1] + K * K) * norm;

	K = tan(M_PI * mackity->biquadB[0]);
	norm = 1.0 / (1.0 + K / mackity->biquadB[1] + K * K);
	mackity->biquadB[2] = K * K * norm;
	mackity->biquadB[3] = 2.0 * mackity->biquadB[2];
	mackity->biquadB[4] = mackity->biquadB[2];
	mackity->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	mackity->biquadB[6] = (1.0 - K / mackity->biquadB[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = mackity->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = mackity->fpdR * 1.18e-17;

		if (fabs(mackity->iirSampleAL) < 1.18e-37) mackity->iirSampleAL = 0.0;
		mackity->iirSampleAL = (mackity->iirSampleAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		inputSampleL -= mackity->iirSampleAL;
		if (fabs(mackity->iirSampleAR) < 1.18e-37) mackity->iirSampleAR = 0.0;
		mackity->iirSampleAR = (mackity->iirSampleAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		inputSampleR -= mackity->iirSampleAR;

		if (inTrim != 1.0) {
			inputSampleL *= inTrim;
			inputSampleR *= inTrim;
		}

		double outSampleL = mackity->biquadA[2] * inputSampleL + mackity->biquadA[3] * mackity->biquadA[7] + mackity->biquadA[4] * mackity->biquadA[8] - mackity->biquadA[5] * mackity->biquadA[9] - mackity->biquadA[6] * mackity->biquadA[10];
		mackity->biquadA[8] = mackity->biquadA[7];
		mackity->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		mackity->biquadA[10] = mackity->biquadA[9];
		mackity->biquadA[9] = inputSampleL; // DF1 left

		double outSampleR = mackity->biquadA[2] * inputSampleR + mackity->biquadA[3] * mackity->biquadA[11] + mackity->biquadA[4] * mackity->biquadA[12] - mackity->biquadA[5] * mackity->biquadA[13] - mackity->biquadA[6] * mackity->biquadA[14];
		mackity->biquadA[12] = mackity->biquadA[11];
		mackity->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		mackity->biquadA[14] = mackity->biquadA[13];
		mackity->biquadA[13] = inputSampleR; // DF1 right

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		inputSampleL -= pow(inputSampleL, 5) * 0.1768;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		inputSampleR -= pow(inputSampleR, 5) * 0.1768;

		outSampleL = mackity->biquadB[2] * inputSampleL + mackity->biquadB[3] * mackity->biquadB[7] + mackity->biquadB[4] * mackity->biquadB[8] - mackity->biquadB[5] * mackity->biquadB[9] - mackity->biquadB[6] * mackity->biquadB[10];
		mackity->biquadB[8] = mackity->biquadB[7];
		mackity->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		mackity->biquadB[10] = mackity->biquadB[9];
		mackity->biquadB[9] = inputSampleL; // DF1 left

		outSampleR = mackity->biquadB[2] * inputSampleR + mackity->biquadB[3] * mackity->biquadB[11] + mackity->biquadB[4] * mackity->biquadB[12] - mackity->biquadB[5] * mackity->biquadB[13] - mackity->biquadB[6] * mackity->biquadB[14];
		mackity->biquadB[12] = mackity->biquadB[11];
		mackity->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		mackity->biquadB[14] = mackity->biquadB[13];
		mackity->biquadB[13] = inputSampleR; // DF1 right

		if (fabs(mackity->iirSampleBL) < 1.18e-37) mackity->iirSampleBL = 0.0;
		mackity->iirSampleBL = (mackity->iirSampleBL * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
		inputSampleL -= mackity->iirSampleBL;
		if (fabs(mackity->iirSampleBR) < 1.18e-37) mackity->iirSampleBR = 0.0;
		mackity->iirSampleBR = (mackity->iirSampleBR * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
		inputSampleR -= mackity->iirSampleBR;

		if (outPad != 1.0) {
			inputSampleL *= outPad;
			inputSampleR *= outPad;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		mackity->fpdL ^= mackity->fpdL << 13;
		mackity->fpdL ^= mackity->fpdL >> 17;
		mackity->fpdL ^= mackity->fpdL << 5;
		inputSampleL += (((double) mackity->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		mackity->fpdR ^= mackity->fpdR << 13;
		mackity->fpdR ^= mackity->fpdR >> 17;
		mackity->fpdR ^= mackity->fpdR << 5;
		inputSampleR += (((double) mackity->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MACKITY_URI,
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
