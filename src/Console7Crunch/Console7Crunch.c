#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE7CRUNCH_URI "https://hannesbraun.net/ns/lv2/airwindows/console7crunch"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CRUNCH = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* crunch;

	double gainchase;
	double chasespeed;

	double biquadA[15];
	double biquadB[15];
	double biquadC[15];
	double biquadD[15];
	double biquadE[15];

	uint32_t fpdL;
	uint32_t fpdR;
} Console7Crunch;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console7Crunch* console7Crunch = (Console7Crunch*) calloc(1, sizeof(Console7Crunch));
	console7Crunch->sampleRate = rate;
	return (LV2_Handle) console7Crunch;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console7Crunch* console7Crunch = (Console7Crunch*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console7Crunch->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console7Crunch->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console7Crunch->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console7Crunch->output[1] = (float*) data;
			break;
		case CRUNCH:
			console7Crunch->crunch = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console7Crunch* console7Crunch = (Console7Crunch*) instance;

	console7Crunch->gainchase = -1.0;
	console7Crunch->chasespeed = 64.0;
	for (int x = 0; x < 15; x++) {
		console7Crunch->biquadA[x] = 0.0;
		console7Crunch->biquadB[x] = 0.0;
		console7Crunch->biquadC[x] = 0.0;
		console7Crunch->biquadD[x] = 0.0;
		console7Crunch->biquadE[x] = 0.0;
	}
	console7Crunch->fpdL = 1.0;
	while (console7Crunch->fpdL < 16386) console7Crunch->fpdL = rand() * UINT32_MAX;
	console7Crunch->fpdR = 1.0;
	while (console7Crunch->fpdR < 16386) console7Crunch->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console7Crunch* console7Crunch = (Console7Crunch*) instance;

	const float* in1 = console7Crunch->input[0];
	const float* in2 = console7Crunch->input[1];
	float* out1 = console7Crunch->output[0];
	float* out2 = console7Crunch->output[1];

	double inputgain = 1.0 - (pow(1.0 - *console7Crunch->crunch, 2));
	if (console7Crunch->gainchase != inputgain) console7Crunch->chasespeed *= 2.0;
	if (console7Crunch->chasespeed > sampleFrames) console7Crunch->chasespeed = sampleFrames;
	if (console7Crunch->gainchase < 0.0) console7Crunch->gainchase = inputgain;

	console7Crunch->biquadE[0] = console7Crunch->biquadD[0] = console7Crunch->biquadC[0] = console7Crunch->biquadB[0] = console7Crunch->biquadA[0] = 20000.0 / console7Crunch->sampleRate;
	console7Crunch->biquadA[1] = 0.50623256;
	console7Crunch->biquadB[1] = 0.56116312;
	console7Crunch->biquadC[1] = 0.70710678;
	console7Crunch->biquadD[1] = 1.10134463;
	console7Crunch->biquadE[1] = 3.19622661; //tenth order Butterworth out of five biquads

	double K = tan(M_PI * console7Crunch->biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / console7Crunch->biquadA[1] + K * K);
	console7Crunch->biquadA[2] = K * K * norm;
	console7Crunch->biquadA[3] = 2.0 * console7Crunch->biquadA[2];
	console7Crunch->biquadA[4] = console7Crunch->biquadA[2];
	console7Crunch->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	console7Crunch->biquadA[6] = (1.0 - K / console7Crunch->biquadA[1] + K * K) * norm;

	K = tan(M_PI * console7Crunch->biquadA[0]);
	norm = 1.0 / (1.0 + K / console7Crunch->biquadB[1] + K * K);
	console7Crunch->biquadB[2] = K * K * norm;
	console7Crunch->biquadB[3] = 2.0 * console7Crunch->biquadB[2];
	console7Crunch->biquadB[4] = console7Crunch->biquadB[2];
	console7Crunch->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	console7Crunch->biquadB[6] = (1.0 - K / console7Crunch->biquadB[1] + K * K) * norm;

	K = tan(M_PI * console7Crunch->biquadC[0]);
	norm = 1.0 / (1.0 + K / console7Crunch->biquadC[1] + K * K);
	console7Crunch->biquadC[2] = K * K * norm;
	console7Crunch->biquadC[3] = 2.0 * console7Crunch->biquadC[2];
	console7Crunch->biquadC[4] = console7Crunch->biquadC[2];
	console7Crunch->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	console7Crunch->biquadC[6] = (1.0 - K / console7Crunch->biquadC[1] + K * K) * norm;

	K = tan(M_PI * console7Crunch->biquadD[0]);
	norm = 1.0 / (1.0 + K / console7Crunch->biquadD[1] + K * K);
	console7Crunch->biquadD[2] = K * K * norm;
	console7Crunch->biquadD[3] = 2.0 * console7Crunch->biquadD[2];
	console7Crunch->biquadD[4] = console7Crunch->biquadD[2];
	console7Crunch->biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	console7Crunch->biquadD[6] = (1.0 - K / console7Crunch->biquadD[1] + K * K) * norm;

	K = tan(M_PI * console7Crunch->biquadE[0]);
	norm = 1.0 / (1.0 + K / console7Crunch->biquadE[1] + K * K);
	console7Crunch->biquadE[2] = K * K * norm;
	console7Crunch->biquadE[3] = 2.0 * console7Crunch->biquadE[2];
	console7Crunch->biquadE[4] = console7Crunch->biquadE[2];
	console7Crunch->biquadE[5] = 2.0 * (K * K - 1.0) * norm;
	console7Crunch->biquadE[6] = (1.0 - K / console7Crunch->biquadE[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console7Crunch->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console7Crunch->fpdR * 1.18e-17;

		double outSampleL = console7Crunch->biquadA[2] * inputSampleL + console7Crunch->biquadA[3] * console7Crunch->biquadA[7] + console7Crunch->biquadA[4] * console7Crunch->biquadA[8] - console7Crunch->biquadA[5] * console7Crunch->biquadA[9] - console7Crunch->biquadA[6] * console7Crunch->biquadA[10];
		console7Crunch->biquadA[8] = console7Crunch->biquadA[7];
		console7Crunch->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Crunch->biquadA[10] = console7Crunch->biquadA[9];
		console7Crunch->biquadA[9] = inputSampleL; //DF1 left
		double outSampleR = console7Crunch->biquadA[2] * inputSampleR + console7Crunch->biquadA[3] * console7Crunch->biquadA[11] + console7Crunch->biquadA[4] * console7Crunch->biquadA[12] - console7Crunch->biquadA[5] * console7Crunch->biquadA[13] - console7Crunch->biquadA[6] * console7Crunch->biquadA[14];
		console7Crunch->biquadA[12] = console7Crunch->biquadA[11];
		console7Crunch->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Crunch->biquadA[14] = console7Crunch->biquadA[13];
		console7Crunch->biquadA[13] = inputSampleR; //DF1 right

		console7Crunch->chasespeed *= 0.9999;
		console7Crunch->chasespeed -= 0.01;
		if (console7Crunch->chasespeed < 64.0) console7Crunch->chasespeed = 64.0;
		//we have our chase speed compensated for recent fader activity
		console7Crunch->gainchase = (((console7Crunch->gainchase * console7Crunch->chasespeed) + inputgain) / (console7Crunch->chasespeed + 1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		double cascade = console7Crunch->gainchase + (console7Crunch->gainchase * 0.62);

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

		outSampleL = console7Crunch->biquadB[2] * inputSampleL + console7Crunch->biquadB[3] * console7Crunch->biquadB[7] + console7Crunch->biquadB[4] * console7Crunch->biquadB[8] - console7Crunch->biquadB[5] * console7Crunch->biquadB[9] - console7Crunch->biquadB[6] * console7Crunch->biquadB[10];
		console7Crunch->biquadB[8] = console7Crunch->biquadB[7];
		console7Crunch->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Crunch->biquadB[10] = console7Crunch->biquadB[9];
		console7Crunch->biquadB[9] = inputSampleL; //DF1 left
		outSampleR = console7Crunch->biquadB[2] * inputSampleR + console7Crunch->biquadB[3] * console7Crunch->biquadB[11] + console7Crunch->biquadB[4] * console7Crunch->biquadB[12] - console7Crunch->biquadB[5] * console7Crunch->biquadB[13] - console7Crunch->biquadB[6] * console7Crunch->biquadB[14];
		console7Crunch->biquadB[12] = console7Crunch->biquadB[11];
		console7Crunch->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Crunch->biquadB[14] = console7Crunch->biquadB[13];
		console7Crunch->biquadB[13] = inputSampleR; //DF1 right

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

		outSampleL = console7Crunch->biquadC[2] * inputSampleL + console7Crunch->biquadC[3] * console7Crunch->biquadC[7] + console7Crunch->biquadC[4] * console7Crunch->biquadC[8] - console7Crunch->biquadC[5] * console7Crunch->biquadC[9] - console7Crunch->biquadC[6] * console7Crunch->biquadC[10];
		console7Crunch->biquadC[8] = console7Crunch->biquadC[7];
		console7Crunch->biquadC[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Crunch->biquadC[10] = console7Crunch->biquadC[9];
		console7Crunch->biquadC[9] = inputSampleL; //DF1 left
		outSampleR = console7Crunch->biquadC[2] * inputSampleR + console7Crunch->biquadC[3] * console7Crunch->biquadC[11] + console7Crunch->biquadC[4] * console7Crunch->biquadC[12] - console7Crunch->biquadC[5] * console7Crunch->biquadC[13] - console7Crunch->biquadC[6] * console7Crunch->biquadC[14];
		console7Crunch->biquadC[12] = console7Crunch->biquadC[11];
		console7Crunch->biquadC[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Crunch->biquadC[14] = console7Crunch->biquadC[13];
		console7Crunch->biquadC[13] = inputSampleR; //DF1 right

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

		outSampleL = console7Crunch->biquadD[2] * inputSampleL + console7Crunch->biquadD[3] * console7Crunch->biquadD[7] + console7Crunch->biquadD[4] * console7Crunch->biquadD[8] - console7Crunch->biquadD[5] * console7Crunch->biquadD[9] - console7Crunch->biquadD[6] * console7Crunch->biquadD[10];
		console7Crunch->biquadD[8] = console7Crunch->biquadD[7];
		console7Crunch->biquadD[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Crunch->biquadD[10] = console7Crunch->biquadD[9];
		console7Crunch->biquadD[9] = inputSampleL; //DF1 left
		outSampleR = console7Crunch->biquadD[2] * inputSampleR + console7Crunch->biquadD[3] * console7Crunch->biquadD[11] + console7Crunch->biquadD[4] * console7Crunch->biquadD[12] - console7Crunch->biquadD[5] * console7Crunch->biquadD[13] - console7Crunch->biquadD[6] * console7Crunch->biquadD[14];
		console7Crunch->biquadD[12] = console7Crunch->biquadD[11];
		console7Crunch->biquadD[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Crunch->biquadD[14] = console7Crunch->biquadD[13];
		console7Crunch->biquadD[13] = inputSampleR; //DF1 right

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

		outSampleL = console7Crunch->biquadE[2] * inputSampleL + console7Crunch->biquadE[3] * console7Crunch->biquadE[7] + console7Crunch->biquadE[4] * console7Crunch->biquadE[8] - console7Crunch->biquadE[5] * console7Crunch->biquadE[9] - console7Crunch->biquadE[6] * console7Crunch->biquadE[10];
		console7Crunch->biquadE[8] = console7Crunch->biquadE[7];
		console7Crunch->biquadE[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Crunch->biquadE[10] = console7Crunch->biquadE[9];
		console7Crunch->biquadE[9] = inputSampleL; //DF1 left
		outSampleR = console7Crunch->biquadE[2] * inputSampleR + console7Crunch->biquadE[3] * console7Crunch->biquadE[11] + console7Crunch->biquadE[4] * console7Crunch->biquadE[12] - console7Crunch->biquadE[5] * console7Crunch->biquadE[13] - console7Crunch->biquadE[6] * console7Crunch->biquadE[14];
		console7Crunch->biquadE[12] = console7Crunch->biquadE[11];
		console7Crunch->biquadE[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Crunch->biquadE[14] = console7Crunch->biquadE[13];
		console7Crunch->biquadE[13] = inputSampleR; //DF1 right

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
		console7Crunch->fpdL ^= console7Crunch->fpdL << 13;
		console7Crunch->fpdL ^= console7Crunch->fpdL >> 17;
		console7Crunch->fpdL ^= console7Crunch->fpdL << 5;
		inputSampleL += (((double)console7Crunch->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		console7Crunch->fpdR ^= console7Crunch->fpdR << 13;
		console7Crunch->fpdR ^= console7Crunch->fpdR >> 17;
		console7Crunch->fpdR ^= console7Crunch->fpdR << 5;
		inputSampleR += (((double)console7Crunch->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE7CRUNCH_URI,
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
