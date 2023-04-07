#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define PURESTCONSOLE2CHANNEL_URI "https://hannesbraun.net/ns/lv2/airwindows/purestconsole2channel"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];

	double biquadA[15];
	uint32_t fpdL;
	uint32_t fpdR;
} PurestConsole2Channel;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestConsole2Channel* purestConsole2Channel = (PurestConsole2Channel*) calloc(1, sizeof(PurestConsole2Channel));
	purestConsole2Channel->sampleRate = rate;
	return (LV2_Handle) purestConsole2Channel;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestConsole2Channel* purestConsole2Channel = (PurestConsole2Channel*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestConsole2Channel->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestConsole2Channel->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestConsole2Channel->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestConsole2Channel->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestConsole2Channel* purestConsole2Channel = (PurestConsole2Channel*) instance;

	for (int x = 0; x < 15; x++) {
		purestConsole2Channel->biquadA[x] = 0.0;
	}
	purestConsole2Channel->fpdL = 1.0;
	while (purestConsole2Channel->fpdL < 16386) purestConsole2Channel->fpdL = rand() * UINT32_MAX;
	purestConsole2Channel->fpdR = 1.0;
	while (purestConsole2Channel->fpdR < 16386) purestConsole2Channel->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestConsole2Channel* purestConsole2Channel = (PurestConsole2Channel*) instance;

	const float* in1 = purestConsole2Channel->input[0];
	const float* in2 = purestConsole2Channel->input[1];
	float* out1 = purestConsole2Channel->output[0];
	float* out2 = purestConsole2Channel->output[1];

	purestConsole2Channel->biquadA[0] = 30000.0 / purestConsole2Channel->sampleRate;
	purestConsole2Channel->biquadA[1] = 0.618033988749894848204586;

	double K = tan(M_PI * purestConsole2Channel->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / purestConsole2Channel->biquadA[1] + K * K);
	purestConsole2Channel->biquadA[2] = K * K * norm;
	purestConsole2Channel->biquadA[3] = 2.0 * purestConsole2Channel->biquadA[2];
	purestConsole2Channel->biquadA[4] = purestConsole2Channel->biquadA[2];
	purestConsole2Channel->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	purestConsole2Channel->biquadA[6] = (1.0 - K / purestConsole2Channel->biquadA[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestConsole2Channel->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestConsole2Channel->fpdR * 1.18e-17;

		if (purestConsole2Channel->biquadA[0] < 0.49999) {
			double tempSample = purestConsole2Channel->biquadA[2] * inputSampleL + purestConsole2Channel->biquadA[3] * purestConsole2Channel->biquadA[7] + purestConsole2Channel->biquadA[4] * purestConsole2Channel->biquadA[8] - purestConsole2Channel->biquadA[5] * purestConsole2Channel->biquadA[9] - purestConsole2Channel->biquadA[6] * purestConsole2Channel->biquadA[10];
			purestConsole2Channel->biquadA[8] = purestConsole2Channel->biquadA[7];
			purestConsole2Channel->biquadA[7] = inputSampleL;
			inputSampleL = tempSample;
			purestConsole2Channel->biquadA[10] = purestConsole2Channel->biquadA[9];
			purestConsole2Channel->biquadA[9] = inputSampleL; // DF1 left
			tempSample = purestConsole2Channel->biquadA[2] * inputSampleR + purestConsole2Channel->biquadA[3] * purestConsole2Channel->biquadA[11] + purestConsole2Channel->biquadA[4] * purestConsole2Channel->biquadA[12] - purestConsole2Channel->biquadA[5] * purestConsole2Channel->biquadA[13] - purestConsole2Channel->biquadA[6] * purestConsole2Channel->biquadA[14];
			purestConsole2Channel->biquadA[12] = purestConsole2Channel->biquadA[11];
			purestConsole2Channel->biquadA[11] = inputSampleR;
			inputSampleR = tempSample;
			purestConsole2Channel->biquadA[14] = purestConsole2Channel->biquadA[13];
			purestConsole2Channel->biquadA[13] = inputSampleR; // DF1 right
		}

		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;

		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		// amplitude aspect

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		purestConsole2Channel->fpdL ^= purestConsole2Channel->fpdL << 13;
		purestConsole2Channel->fpdL ^= purestConsole2Channel->fpdL >> 17;
		purestConsole2Channel->fpdL ^= purestConsole2Channel->fpdL << 5;
		inputSampleL += (((double) purestConsole2Channel->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		purestConsole2Channel->fpdR ^= purestConsole2Channel->fpdR << 13;
		purestConsole2Channel->fpdR ^= purestConsole2Channel->fpdR >> 17;
		purestConsole2Channel->fpdR ^= purestConsole2Channel->fpdR << 5;
		inputSampleR += (((double) purestConsole2Channel->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PURESTCONSOLE2CHANNEL_URI,
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
