#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define ULTRASONICLITE_URI "https://hannesbraun.net/ns/lv2/airwindows/ultrasoniclite"

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
} UltrasonicLite;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	UltrasonicLite* ultrasoniclite = (UltrasonicLite*) calloc(1, sizeof(UltrasonicLite));
	ultrasoniclite->sampleRate = rate;
	return (LV2_Handle) ultrasoniclite;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	UltrasonicLite* ultrasoniclite = (UltrasonicLite*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			ultrasoniclite->input[0] = (const float*) data;
			break;
		case INPUT_R:
			ultrasoniclite->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			ultrasoniclite->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			ultrasoniclite->output[1] = (float*) data;
	}
}

static void activate(LV2_Handle instance)
{
	UltrasonicLite* ultrasoniclite = (UltrasonicLite*) instance;
	for (int x = 0; x < 15; x++) {
		ultrasoniclite->biquadA[x] = 0.0;
	}
	ultrasoniclite->fpdL = 1.0;
	while (ultrasoniclite->fpdL < 16386) ultrasoniclite->fpdL = rand() * UINT32_MAX;
	ultrasoniclite->fpdR = 1.0;
	while (ultrasoniclite->fpdR < 16386) ultrasoniclite->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	UltrasonicLite* ultrasoniclite = (UltrasonicLite*) instance;

	const float* in1 = ultrasoniclite->input[0];
	const float* in2 = ultrasoniclite->input[1];
	float* out1 = ultrasoniclite->output[0];
	float* out2 = ultrasoniclite->output[1];

	ultrasoniclite->biquadA[0] = 24000.0 / ultrasoniclite->sampleRate;
	if (ultrasoniclite->sampleRate < 88000.0) {
		ultrasoniclite->biquadA[0] = 21000.0 / ultrasoniclite->sampleRate;
	}
	ultrasoniclite->biquadA[1] = 0.70710678;

	double K = tan(M_PI * ultrasoniclite->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / ultrasoniclite->biquadA[1] + K * K);
	ultrasoniclite->biquadA[2] = K * K * norm;
	ultrasoniclite->biquadA[3] = 2.0 * ultrasoniclite->biquadA[2];
	ultrasoniclite->biquadA[4] = ultrasoniclite->biquadA[2];
	ultrasoniclite->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasoniclite->biquadA[6] = (1.0 - K / ultrasoniclite->biquadA[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = ultrasoniclite->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = ultrasoniclite->fpdR * 1.18e-17;

		double outSampleL = ultrasoniclite->biquadA[2] * inputSampleL + ultrasoniclite->biquadA[3] * ultrasoniclite->biquadA[7] + ultrasoniclite->biquadA[4] * ultrasoniclite->biquadA[8] - ultrasoniclite->biquadA[5] * ultrasoniclite->biquadA[9] - ultrasoniclite->biquadA[6] * ultrasoniclite->biquadA[10];
		ultrasoniclite->biquadA[8] = ultrasoniclite->biquadA[7];
		ultrasoniclite->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasoniclite->biquadA[10] = ultrasoniclite->biquadA[9];
		ultrasoniclite->biquadA[9] = inputSampleL; // DF1 left

		double outSampleR = ultrasoniclite->biquadA[2] * inputSampleR + ultrasoniclite->biquadA[3] * ultrasoniclite->biquadA[11] + ultrasoniclite->biquadA[4] * ultrasoniclite->biquadA[12] - ultrasoniclite->biquadA[5] * ultrasoniclite->biquadA[13] - ultrasoniclite->biquadA[6] * ultrasoniclite->biquadA[14];
		ultrasoniclite->biquadA[12] = ultrasoniclite->biquadA[11];
		ultrasoniclite->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasoniclite->biquadA[14] = ultrasoniclite->biquadA[13];
		ultrasoniclite->biquadA[13] = inputSampleR; // DF1 right

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		ultrasoniclite->fpdL ^= ultrasoniclite->fpdL << 13;
		ultrasoniclite->fpdL ^= ultrasoniclite->fpdL >> 17;
		ultrasoniclite->fpdL ^= ultrasoniclite->fpdL << 5;
		inputSampleL += (((double) ultrasoniclite->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		ultrasoniclite->fpdR ^= ultrasoniclite->fpdR << 13;
		ultrasoniclite->fpdR ^= ultrasoniclite->fpdR >> 17;
		ultrasoniclite->fpdR ^= ultrasoniclite->fpdR << 5;
		inputSampleR += (((double) ultrasoniclite->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ULTRASONICLITE_URI,
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
