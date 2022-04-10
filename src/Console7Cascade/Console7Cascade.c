#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE7CASCADE_URI "https://hannesbraun.net/ns/lv2/airwindows/console7cascade"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CASCADE = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* cascade;

	double gainchase;
	double chasespeed;

	double biquadA[15];
	double biquadB[15];
	double biquadC[15];
	double biquadD[15];
	double biquadE[15];

	uint32_t fpdL;
	uint32_t fpdR;
} Console7Cascade;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console7Cascade* console7Cascade = (Console7Cascade*) calloc(1, sizeof(Console7Cascade));
	console7Cascade->sampleRate = rate;
	return (LV2_Handle) console7Cascade;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console7Cascade* console7Cascade = (Console7Cascade*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console7Cascade->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console7Cascade->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console7Cascade->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console7Cascade->output[1] = (float*) data;
			break;
		case CASCADE:
			console7Cascade->cascade = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console7Cascade* console7Cascade = (Console7Cascade*) instance;

	console7Cascade->gainchase = -1.0;
	console7Cascade->chasespeed = 64.0;
	for (int x = 0; x < 15; x++) {
		console7Cascade->biquadA[x] = 0.0;
		console7Cascade->biquadB[x] = 0.0;
		console7Cascade->biquadC[x] = 0.0;
		console7Cascade->biquadD[x] = 0.0;
		console7Cascade->biquadE[x] = 0.0;
	}
	console7Cascade->fpdL = 1.0;
	while (console7Cascade->fpdL < 16386) console7Cascade->fpdL = rand() * UINT32_MAX;
	console7Cascade->fpdR = 1.0;
	while (console7Cascade->fpdR < 16386) console7Cascade->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console7Cascade* console7Cascade = (Console7Cascade*) instance;

	const float* in1 = console7Cascade->input[0];
	const float* in2 = console7Cascade->input[1];
	float* out1 = console7Cascade->output[0];
	float* out2 = console7Cascade->output[1];

	double inputgain = 1.0 - (pow(1.0 - *console7Cascade->cascade, 2));
	if (console7Cascade->gainchase != inputgain) console7Cascade->chasespeed *= 2.0;
	if (console7Cascade->chasespeed > sampleFrames) console7Cascade->chasespeed = sampleFrames;
	if (console7Cascade->gainchase < 0.0) console7Cascade->gainchase = inputgain;

	console7Cascade->biquadE[0] = console7Cascade->biquadD[0] = console7Cascade->biquadC[0] = console7Cascade->biquadB[0] = console7Cascade->biquadA[0] = 20000.0 / console7Cascade->sampleRate;
	console7Cascade->biquadA[1] = 3.19622661; //tenth order Butterworth out of five biquads
	console7Cascade->biquadB[1] = 1.10134463;
	console7Cascade->biquadC[1] = 0.70710678;
	console7Cascade->biquadD[1] = 0.56116312;
	console7Cascade->biquadE[1] = 0.50623256;

	double K = tan(M_PI * console7Cascade->biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / console7Cascade->biquadA[1] + K * K);
	console7Cascade->biquadA[2] = K * K * norm;
	console7Cascade->biquadA[3] = 2.0 * console7Cascade->biquadA[2];
	console7Cascade->biquadA[4] = console7Cascade->biquadA[2];
	console7Cascade->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	console7Cascade->biquadA[6] = (1.0 - K / console7Cascade->biquadA[1] + K * K) * norm;

	K = tan(M_PI * console7Cascade->biquadA[0]);
	norm = 1.0 / (1.0 + K / console7Cascade->biquadB[1] + K * K);
	console7Cascade->biquadB[2] = K * K * norm;
	console7Cascade->biquadB[3] = 2.0 * console7Cascade->biquadB[2];
	console7Cascade->biquadB[4] = console7Cascade->biquadB[2];
	console7Cascade->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	console7Cascade->biquadB[6] = (1.0 - K / console7Cascade->biquadB[1] + K * K) * norm;

	K = tan(M_PI * console7Cascade->biquadC[0]);
	norm = 1.0 / (1.0 + K / console7Cascade->biquadC[1] + K * K);
	console7Cascade->biquadC[2] = K * K * norm;
	console7Cascade->biquadC[3] = 2.0 * console7Cascade->biquadC[2];
	console7Cascade->biquadC[4] = console7Cascade->biquadC[2];
	console7Cascade->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	console7Cascade->biquadC[6] = (1.0 - K / console7Cascade->biquadC[1] + K * K) * norm;

	K = tan(M_PI * console7Cascade->biquadD[0]);
	norm = 1.0 / (1.0 + K / console7Cascade->biquadD[1] + K * K);
	console7Cascade->biquadD[2] = K * K * norm;
	console7Cascade->biquadD[3] = 2.0 * console7Cascade->biquadD[2];
	console7Cascade->biquadD[4] = console7Cascade->biquadD[2];
	console7Cascade->biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	console7Cascade->biquadD[6] = (1.0 - K / console7Cascade->biquadD[1] + K * K) * norm;

	K = tan(M_PI * console7Cascade->biquadE[0]);
	norm = 1.0 / (1.0 + K / console7Cascade->biquadE[1] + K * K);
	console7Cascade->biquadE[2] = K * K * norm;
	console7Cascade->biquadE[3] = 2.0 * console7Cascade->biquadE[2];
	console7Cascade->biquadE[4] = console7Cascade->biquadE[2];
	console7Cascade->biquadE[5] = 2.0 * (K * K - 1.0) * norm;
	console7Cascade->biquadE[6] = (1.0 - K / console7Cascade->biquadE[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console7Cascade->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console7Cascade->fpdR * 1.18e-17;

		double outSampleL = console7Cascade->biquadA[2] * inputSampleL + console7Cascade->biquadA[3] * console7Cascade->biquadA[7] + console7Cascade->biquadA[4] * console7Cascade->biquadA[8] - console7Cascade->biquadA[5] * console7Cascade->biquadA[9] - console7Cascade->biquadA[6] * console7Cascade->biquadA[10];
		console7Cascade->biquadA[8] = console7Cascade->biquadA[7];
		console7Cascade->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Cascade->biquadA[10] = console7Cascade->biquadA[9];
		console7Cascade->biquadA[9] = inputSampleL; //DF1 left
		double outSampleR = console7Cascade->biquadA[2] * inputSampleR + console7Cascade->biquadA[3] * console7Cascade->biquadA[11] + console7Cascade->biquadA[4] * console7Cascade->biquadA[12] - console7Cascade->biquadA[5] * console7Cascade->biquadA[13] - console7Cascade->biquadA[6] * console7Cascade->biquadA[14];
		console7Cascade->biquadA[12] = console7Cascade->biquadA[11];
		console7Cascade->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Cascade->biquadA[14] = console7Cascade->biquadA[13];
		console7Cascade->biquadA[13] = inputSampleR; //DF1 right

		console7Cascade->chasespeed *= 0.9999;
		console7Cascade->chasespeed -= 0.01;
		if (console7Cascade->chasespeed < 64.0) console7Cascade->chasespeed = 64.0;
		//we have our chase speed compensated for recent fader activity
		console7Cascade->gainchase = (((console7Cascade->gainchase * console7Cascade->chasespeed) + inputgain) / (console7Cascade->chasespeed + 1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		double cascade = console7Cascade->gainchase + (console7Cascade->gainchase * 0.62);

		if (1.0 != cascade) {
			inputSampleL *= cascade;
			inputSampleR *= cascade;
		}
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		//Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x

		outSampleL = console7Cascade->biquadB[2] * inputSampleL + console7Cascade->biquadB[3] * console7Cascade->biquadB[7] + console7Cascade->biquadB[4] * console7Cascade->biquadB[8] - console7Cascade->biquadB[5] * console7Cascade->biquadB[9] - console7Cascade->biquadB[6] * console7Cascade->biquadB[10];
		console7Cascade->biquadB[8] = console7Cascade->biquadB[7];
		console7Cascade->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Cascade->biquadB[10] = console7Cascade->biquadB[9];
		console7Cascade->biquadB[9] = inputSampleL; //DF1 left
		outSampleR = console7Cascade->biquadB[2] * inputSampleR + console7Cascade->biquadB[3] * console7Cascade->biquadB[11] + console7Cascade->biquadB[4] * console7Cascade->biquadB[12] - console7Cascade->biquadB[5] * console7Cascade->biquadB[13] - console7Cascade->biquadB[6] * console7Cascade->biquadB[14];
		console7Cascade->biquadB[12] = console7Cascade->biquadB[11];
		console7Cascade->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Cascade->biquadB[14] = console7Cascade->biquadB[13];
		console7Cascade->biquadB[13] = inputSampleR; //DF1 right

		if (1.0 != cascade) {
			inputSampleL *= cascade;
			inputSampleR *= cascade;
		}
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		//Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x

		outSampleL = console7Cascade->biquadC[2] * inputSampleL + console7Cascade->biquadC[3] * console7Cascade->biquadC[7] + console7Cascade->biquadC[4] * console7Cascade->biquadC[8] - console7Cascade->biquadC[5] * console7Cascade->biquadC[9] - console7Cascade->biquadC[6] * console7Cascade->biquadC[10];
		console7Cascade->biquadC[8] = console7Cascade->biquadC[7];
		console7Cascade->biquadC[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Cascade->biquadC[10] = console7Cascade->biquadC[9];
		console7Cascade->biquadC[9] = inputSampleL; //DF1 left
		outSampleR = console7Cascade->biquadC[2] * inputSampleR + console7Cascade->biquadC[3] * console7Cascade->biquadC[11] + console7Cascade->biquadC[4] * console7Cascade->biquadC[12] - console7Cascade->biquadC[5] * console7Cascade->biquadC[13] - console7Cascade->biquadC[6] * console7Cascade->biquadC[14];
		console7Cascade->biquadC[12] = console7Cascade->biquadC[11];
		console7Cascade->biquadC[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Cascade->biquadC[14] = console7Cascade->biquadC[13];
		console7Cascade->biquadC[13] = inputSampleR; //DF1 right

		if (1.0 != cascade) {
			inputSampleL *= cascade;
			inputSampleR *= cascade;
		}
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		//Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x

		outSampleL = console7Cascade->biquadD[2] * inputSampleL + console7Cascade->biquadD[3] * console7Cascade->biquadD[7] + console7Cascade->biquadD[4] * console7Cascade->biquadD[8] - console7Cascade->biquadD[5] * console7Cascade->biquadD[9] - console7Cascade->biquadD[6] * console7Cascade->biquadD[10];
		console7Cascade->biquadD[8] = console7Cascade->biquadD[7];
		console7Cascade->biquadD[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Cascade->biquadD[10] = console7Cascade->biquadD[9];
		console7Cascade->biquadD[9] = inputSampleL; //DF1 left
		outSampleR = console7Cascade->biquadD[2] * inputSampleR + console7Cascade->biquadD[3] * console7Cascade->biquadD[11] + console7Cascade->biquadD[4] * console7Cascade->biquadD[12] - console7Cascade->biquadD[5] * console7Cascade->biquadD[13] - console7Cascade->biquadD[6] * console7Cascade->biquadD[14];
		console7Cascade->biquadD[12] = console7Cascade->biquadD[11];
		console7Cascade->biquadD[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Cascade->biquadD[14] = console7Cascade->biquadD[13];
		console7Cascade->biquadD[13] = inputSampleR; //DF1 right

		if (1.0 != cascade) {
			inputSampleL *= cascade;
			inputSampleR *= cascade;
		}
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		//Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x

		outSampleL = console7Cascade->biquadE[2] * inputSampleL + console7Cascade->biquadE[3] * console7Cascade->biquadE[7] + console7Cascade->biquadE[4] * console7Cascade->biquadE[8] - console7Cascade->biquadE[5] * console7Cascade->biquadE[9] - console7Cascade->biquadE[6] * console7Cascade->biquadE[10];
		console7Cascade->biquadE[8] = console7Cascade->biquadE[7];
		console7Cascade->biquadE[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Cascade->biquadE[10] = console7Cascade->biquadE[9];
		console7Cascade->biquadE[9] = inputSampleL; //DF1 left
		outSampleR = console7Cascade->biquadE[2] * inputSampleR + console7Cascade->biquadE[3] * console7Cascade->biquadE[11] + console7Cascade->biquadE[4] * console7Cascade->biquadE[12] - console7Cascade->biquadE[5] * console7Cascade->biquadE[13] - console7Cascade->biquadE[6] * console7Cascade->biquadE[14];
		console7Cascade->biquadE[12] = console7Cascade->biquadE[11];
		console7Cascade->biquadE[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Cascade->biquadE[14] = console7Cascade->biquadE[13];
		console7Cascade->biquadE[13] = inputSampleR; //DF1 right

		if (1.0 != cascade) {
			inputSampleL *= cascade;
			inputSampleR *= cascade;
		}
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		//Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x

		if (cascade > 1.0) {
			inputSampleL /= cascade;
			inputSampleR /= cascade;
		}
		//we re-amplify after the distortion relative to how much we cut back previously.

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		console7Cascade->fpdL ^= console7Cascade->fpdL << 13;
		console7Cascade->fpdL ^= console7Cascade->fpdL >> 17;
		console7Cascade->fpdL ^= console7Cascade->fpdL << 5;
		inputSampleL += (((double)console7Cascade->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		console7Cascade->fpdR ^= console7Cascade->fpdR << 13;
		console7Cascade->fpdR ^= console7Cascade->fpdR >> 17;
		console7Cascade->fpdR ^= console7Cascade->fpdR << 5;
		inputSampleR += (((double)console7Cascade->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE7CASCADE_URI,
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
