#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE7BUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/console7buss"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	MASTER = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* master;

	double gainchase;
	double chasespeed;

	double biquadA[15];
	double biquadB[15];

	uint32_t fpdL;
	uint32_t fpdR;
} Console7Buss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console7Buss* console7Buss = (Console7Buss*) calloc(1, sizeof(Console7Buss));
	console7Buss->sampleRate = rate;
	return (LV2_Handle) console7Buss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console7Buss* console7Buss = (Console7Buss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console7Buss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console7Buss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console7Buss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console7Buss->output[1] = (float*) data;
			break;
		case MASTER:
			console7Buss->master = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console7Buss* console7Buss = (Console7Buss*) instance;

	console7Buss->gainchase = -1.0;
	console7Buss->chasespeed = 64.0;
	for (int x = 0; x < 15; x++) {
		console7Buss->biquadA[x] = 0.0;
		console7Buss->biquadB[x] = 0.0;
	}
	console7Buss->fpdL = 1.0;
	while (console7Buss->fpdL < 16386) console7Buss->fpdL = rand() * UINT32_MAX;
	console7Buss->fpdR = 1.0;
	while (console7Buss->fpdR < 16386) console7Buss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console7Buss* console7Buss = (Console7Buss*) instance;

	const float* in1 = console7Buss->input[0];
	const float* in2 = console7Buss->input[1];
	float* out1 = console7Buss->output[0];
	float* out2 = console7Buss->output[1];

	double inputgain = *console7Buss->master * 1.03;

	if (console7Buss->gainchase != inputgain) console7Buss->chasespeed *= 2.0;
	if (console7Buss->chasespeed > sampleFrames) console7Buss->chasespeed = sampleFrames;
	if (console7Buss->gainchase < 0.0) console7Buss->gainchase = inputgain;

	console7Buss->biquadB[0] = console7Buss->biquadA[0] = 20000.0 / console7Buss->sampleRate;
	console7Buss->biquadA[1] = 0.618033988749894848204586;
	console7Buss->biquadB[1] = 0.5;

	double K = tan(M_PI * console7Buss->biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / console7Buss->biquadA[1] + K * K);
	console7Buss->biquadA[2] = K * K * norm;
	console7Buss->biquadA[3] = 2.0 * console7Buss->biquadA[2];
	console7Buss->biquadA[4] = console7Buss->biquadA[2];
	console7Buss->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	console7Buss->biquadA[6] = (1.0 - K / console7Buss->biquadA[1] + K * K) * norm;

	K = tan(M_PI * console7Buss->biquadA[0]);
	norm = 1.0 / (1.0 + K / console7Buss->biquadB[1] + K * K);
	console7Buss->biquadB[2] = K * K * norm;
	console7Buss->biquadB[3] = 2.0 * console7Buss->biquadB[2];
	console7Buss->biquadB[4] = console7Buss->biquadB[2];
	console7Buss->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	console7Buss->biquadB[6] = (1.0 - K / console7Buss->biquadB[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console7Buss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console7Buss->fpdR * 1.18e-17;

		double outSampleL = console7Buss->biquadA[2] * inputSampleL + console7Buss->biquadA[3] * console7Buss->biquadA[7] + console7Buss->biquadA[4] * console7Buss->biquadA[8] - console7Buss->biquadA[5] * console7Buss->biquadA[9] - console7Buss->biquadA[6] * console7Buss->biquadA[10];
		console7Buss->biquadA[8] = console7Buss->biquadA[7];
		console7Buss->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Buss->biquadA[10] = console7Buss->biquadA[9];
		console7Buss->biquadA[9] = inputSampleL; //DF1 left

		double outSampleR = console7Buss->biquadA[2] * inputSampleR + console7Buss->biquadA[3] * console7Buss->biquadA[11] + console7Buss->biquadA[4] * console7Buss->biquadA[12] - console7Buss->biquadA[5] * console7Buss->biquadA[13] - console7Buss->biquadA[6] * console7Buss->biquadA[14];
		console7Buss->biquadA[12] = console7Buss->biquadA[11];
		console7Buss->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Buss->biquadA[14] = console7Buss->biquadA[13];
		console7Buss->biquadA[13] = inputSampleR; //DF1 right

		console7Buss->chasespeed *= 0.9999;
		console7Buss->chasespeed -= 0.01;
		if (console7Buss->chasespeed < 64.0) console7Buss->chasespeed = 64.0;
		//we have our chase speed compensated for recent fader activity
		console7Buss->gainchase = (((console7Buss->gainchase * console7Buss->chasespeed) + inputgain) / (console7Buss->chasespeed + 1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		if (1.0 != console7Buss->gainchase) {
			inputSampleL *= sqrt(console7Buss->gainchase);
			inputSampleR *= sqrt(console7Buss->gainchase);
		}
		//done with trim control

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		inputSampleL = ((asin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.618033988749894848204586) + (asin(inputSampleL) * 0.381966011250105);
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		inputSampleR = ((asin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.618033988749894848204586) + (asin(inputSampleR) * 0.381966011250105);
		//this is an asin version of Spiral blended with regular asin ConsoleBuss.
		//It's blending between two different harmonics in the overtones of the algorithm

		outSampleL = console7Buss->biquadB[2] * inputSampleL + console7Buss->biquadB[3] * console7Buss->biquadB[7] + console7Buss->biquadB[4] * console7Buss->biquadB[8] - console7Buss->biquadB[5] * console7Buss->biquadB[9] - console7Buss->biquadB[6] * console7Buss->biquadB[10];
		console7Buss->biquadB[8] = console7Buss->biquadB[7];
		console7Buss->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Buss->biquadB[10] = console7Buss->biquadB[9];
		console7Buss->biquadB[9] = inputSampleL; //DF1 left

		outSampleR = console7Buss->biquadB[2] * inputSampleR + console7Buss->biquadB[3] * console7Buss->biquadB[11] + console7Buss->biquadB[4] * console7Buss->biquadB[12] - console7Buss->biquadB[5] * console7Buss->biquadB[13] - console7Buss->biquadB[6] * console7Buss->biquadB[14];
		console7Buss->biquadB[12] = console7Buss->biquadB[11];
		console7Buss->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Buss->biquadB[14] = console7Buss->biquadB[13];
		console7Buss->biquadB[13] = inputSampleR; //DF1 right

		if (1.0 != console7Buss->gainchase) {
			inputSampleL *= sqrt(console7Buss->gainchase);
			inputSampleR *= sqrt(console7Buss->gainchase);
		}
		//we re-amplify after the distortion relative to how much we cut back previously.

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		console7Buss->fpdL ^= console7Buss->fpdL << 13;
		console7Buss->fpdL ^= console7Buss->fpdL >> 17;
		console7Buss->fpdL ^= console7Buss->fpdL << 5;
		inputSampleL += (((double)console7Buss->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		console7Buss->fpdR ^= console7Buss->fpdR << 13;
		console7Buss->fpdR ^= console7Buss->fpdR >> 17;
		console7Buss->fpdR ^= console7Buss->fpdR << 5;
		inputSampleR += (((double)console7Buss->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE7BUSS_URI,
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
