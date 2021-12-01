#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ULTRASONIC_URI "https://hannesbraun.net/ns/lv2/airwindows/ultrasonic"

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
	long double biquadA[15];
	long double biquadB[15];
	long double biquadC[15];
	long double biquadD[15];
	long double biquadE[15];
	uint32_t fpd;
} Ultrasonic;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Ultrasonic* ultrasonic = (Ultrasonic*) calloc(1, sizeof(Ultrasonic));
	ultrasonic->sampleRate = rate;
	return (LV2_Handle) ultrasonic;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Ultrasonic* ultrasonic = (Ultrasonic*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			ultrasonic->input[0] = (const float*) data;
			break;
		case INPUT_R:
			ultrasonic->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			ultrasonic->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			ultrasonic->output[1] = (float*) data;
	}
}

static void activate(LV2_Handle instance)
{
	Ultrasonic* ultrasonic = (Ultrasonic*) instance;
	for (int x = 0; x < 15; x++) {
		ultrasonic->biquadA[x] = 0.0;
		ultrasonic->biquadB[x] = 0.0;
		ultrasonic->biquadC[x] = 0.0;
		ultrasonic->biquadD[x] = 0.0;
		ultrasonic->biquadE[x] = 0.0;
	}
	ultrasonic->fpd = 17;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Ultrasonic* ultrasonic = (Ultrasonic*) instance;

	const float* in1 = ultrasonic->input[0];
	const float* in2 = ultrasonic->input[1];
	float* out1 = ultrasonic->output[0];
	float* out2 = ultrasonic->output[1];

	const long double init = 20000.0 / ultrasonic->sampleRate;
	ultrasonic->biquadA[0] = init;
	ultrasonic->biquadB[0] = init;
	ultrasonic->biquadC[0] = init;
	ultrasonic->biquadD[0] = init;
	ultrasonic->biquadE[0] = init;
	ultrasonic->biquadA[1] = 0.50623256;
	ultrasonic->biquadB[1] = 0.56116312;
	ultrasonic->biquadC[1] = 0.70710678;
	ultrasonic->biquadD[1] = 1.10134463;
	ultrasonic->biquadE[1] = 3.19622661; // tenth order Butterworth out of five biquads

	double K = tan(M_PI * ultrasonic->biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / ultrasonic->biquadA[1] + K * K);
	ultrasonic->biquadA[2] = K * K * norm;
	ultrasonic->biquadA[3] = 2.0 * ultrasonic->biquadA[2];
	ultrasonic->biquadA[4] = ultrasonic->biquadA[2];
	ultrasonic->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonic->biquadA[6] = (1.0 - K / ultrasonic->biquadA[1] + K * K) * norm;

	K = tan(M_PI * ultrasonic->biquadA[0]);
	norm = 1.0 / (1.0 + K / ultrasonic->biquadB[1] + K * K);
	ultrasonic->biquadB[2] = K * K * norm;
	ultrasonic->biquadB[3] = 2.0 * ultrasonic->biquadB[2];
	ultrasonic->biquadB[4] = ultrasonic->biquadB[2];
	ultrasonic->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonic->biquadB[6] = (1.0 - K / ultrasonic->biquadB[1] + K * K) * norm;

	K = tan(M_PI * ultrasonic->biquadC[0]);
	norm = 1.0 / (1.0 + K / ultrasonic->biquadC[1] + K * K);
	ultrasonic->biquadC[2] = K * K * norm;
	ultrasonic->biquadC[3] = 2.0 * ultrasonic->biquadC[2];
	ultrasonic->biquadC[4] = ultrasonic->biquadC[2];
	ultrasonic->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonic->biquadC[6] = (1.0 - K / ultrasonic->biquadC[1] + K * K) * norm;

	K = tan(M_PI * ultrasonic->biquadD[0]);
	norm = 1.0 / (1.0 + K / ultrasonic->biquadD[1] + K * K);
	ultrasonic->biquadD[2] = K * K * norm;
	ultrasonic->biquadD[3] = 2.0 * ultrasonic->biquadD[2];
	ultrasonic->biquadD[4] = ultrasonic->biquadD[2];
	ultrasonic->biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonic->biquadD[6] = (1.0 - K / ultrasonic->biquadD[1] + K * K) * norm;

	K = tan(M_PI * ultrasonic->biquadE[0]);
	norm = 1.0 / (1.0 + K / ultrasonic->biquadE[1] + K * K);
	ultrasonic->biquadE[2] = K * K * norm;
	ultrasonic->biquadE[3] = 2.0 * ultrasonic->biquadE[2];
	ultrasonic->biquadE[4] = ultrasonic->biquadE[2];
	ultrasonic->biquadE[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonic->biquadE[6] = (1.0 - K / ultrasonic->biquadE[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		long double inputSampleL = (long double) * in1;
		long double inputSampleR = (long double) * in2;
		if (fabsl(inputSampleL) < 1.18e-37) inputSampleL = ultrasonic->fpd * 1.18e-37;
		if (fabsl(inputSampleR) < 1.18e-37) inputSampleR = ultrasonic->fpd * 1.18e-37;

		long double outSampleL = ultrasonic->biquadA[2] * inputSampleL + ultrasonic->biquadA[3] * ultrasonic->biquadA[7] + ultrasonic->biquadA[4] * ultrasonic->biquadA[8] - ultrasonic->biquadA[5] * ultrasonic->biquadA[9] - ultrasonic->biquadA[6] * ultrasonic->biquadA[10];
		ultrasonic->biquadA[8] = ultrasonic->biquadA[7];
		ultrasonic->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonic->biquadA[10] = ultrasonic->biquadA[9];
		ultrasonic->biquadA[9] = inputSampleL; // DF1 left

		outSampleL = ultrasonic->biquadB[2] * inputSampleL + ultrasonic->biquadB[3] * ultrasonic->biquadB[7] + ultrasonic->biquadB[4] * ultrasonic->biquadB[8] - ultrasonic->biquadB[5] * ultrasonic->biquadB[9] - ultrasonic->biquadB[6] * ultrasonic->biquadB[10];
		ultrasonic->biquadB[8] = ultrasonic->biquadB[7];
		ultrasonic->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonic->biquadB[10] = ultrasonic->biquadB[9];
		ultrasonic->biquadB[9] = inputSampleL; // DF1 left

		outSampleL = ultrasonic->biquadC[2] * inputSampleL + ultrasonic->biquadC[3] * ultrasonic->biquadC[7] + ultrasonic->biquadC[4] * ultrasonic->biquadC[8] - ultrasonic->biquadC[5] * ultrasonic->biquadC[9] - ultrasonic->biquadC[6] * ultrasonic->biquadC[10];
		ultrasonic->biquadC[8] = ultrasonic->biquadC[7];
		ultrasonic->biquadC[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonic->biquadC[10] = ultrasonic->biquadC[9];
		ultrasonic->biquadC[9] = inputSampleL; // DF1 left

		outSampleL = ultrasonic->biquadD[2] * inputSampleL + ultrasonic->biquadD[3] * ultrasonic->biquadD[7] + ultrasonic->biquadD[4] * ultrasonic->biquadD[8] - ultrasonic->biquadD[5] * ultrasonic->biquadD[9] - ultrasonic->biquadD[6] * ultrasonic->biquadD[10];
		ultrasonic->biquadD[8] = ultrasonic->biquadD[7];
		ultrasonic->biquadD[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonic->biquadD[10] = ultrasonic->biquadD[9];
		ultrasonic->biquadD[9] = inputSampleL; // DF1 left

		outSampleL = ultrasonic->biquadE[2] * inputSampleL + ultrasonic->biquadE[3] * ultrasonic->biquadE[7] + ultrasonic->biquadE[4] * ultrasonic->biquadE[8] - ultrasonic->biquadE[5] * ultrasonic->biquadE[9] - ultrasonic->biquadE[6] * ultrasonic->biquadE[10];
		ultrasonic->biquadE[8] = ultrasonic->biquadE[7];
		ultrasonic->biquadE[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonic->biquadE[10] = ultrasonic->biquadE[9];
		ultrasonic->biquadE[9] = inputSampleL; // DF1 left

		long double outSampleR = ultrasonic->biquadA[2] * inputSampleR + ultrasonic->biquadA[3] * ultrasonic->biquadA[11] + ultrasonic->biquadA[4] * ultrasonic->biquadA[12] - ultrasonic->biquadA[5] * ultrasonic->biquadA[13] - ultrasonic->biquadA[6] * ultrasonic->biquadA[14];
		ultrasonic->biquadA[12] = ultrasonic->biquadA[11];
		ultrasonic->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonic->biquadA[14] = ultrasonic->biquadA[13];
		ultrasonic->biquadA[13] = inputSampleR; // DF1 right

		outSampleR = ultrasonic->biquadB[2] * inputSampleR + ultrasonic->biquadB[3] * ultrasonic->biquadB[11] + ultrasonic->biquadB[4] * ultrasonic->biquadB[12] - ultrasonic->biquadB[5] * ultrasonic->biquadB[13] - ultrasonic->biquadB[6] * ultrasonic->biquadB[14];
		ultrasonic->biquadB[12] = ultrasonic->biquadB[11];
		ultrasonic->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonic->biquadB[14] = ultrasonic->biquadB[13];
		ultrasonic->biquadB[13] = inputSampleR; // DF1 right

		outSampleR = ultrasonic->biquadC[2] * inputSampleR + ultrasonic->biquadC[3] * ultrasonic->biquadC[11] + ultrasonic->biquadC[4] * ultrasonic->biquadC[12] - ultrasonic->biquadC[5] * ultrasonic->biquadC[13] - ultrasonic->biquadC[6] * ultrasonic->biquadC[14];
		ultrasonic->biquadC[12] = ultrasonic->biquadC[11];
		ultrasonic->biquadC[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonic->biquadC[14] = ultrasonic->biquadC[13];
		ultrasonic->biquadC[13] = inputSampleR; // DF1 right

		outSampleR = ultrasonic->biquadD[2] * inputSampleR + ultrasonic->biquadD[3] * ultrasonic->biquadD[11] + ultrasonic->biquadD[4] * ultrasonic->biquadD[12] - ultrasonic->biquadD[5] * ultrasonic->biquadD[13] - ultrasonic->biquadD[6] * ultrasonic->biquadD[14];
		ultrasonic->biquadD[12] = ultrasonic->biquadD[11];
		ultrasonic->biquadD[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonic->biquadD[14] = ultrasonic->biquadD[13];
		ultrasonic->biquadD[13] = inputSampleR; // DF1 right

		outSampleR = ultrasonic->biquadE[2] * inputSampleR + ultrasonic->biquadE[3] * ultrasonic->biquadE[11] + ultrasonic->biquadE[4] * ultrasonic->biquadE[12] - ultrasonic->biquadE[5] * ultrasonic->biquadE[13] - ultrasonic->biquadE[6] * ultrasonic->biquadE[14];
		ultrasonic->biquadE[12] = ultrasonic->biquadE[11];
		ultrasonic->biquadE[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonic->biquadE[14] = ultrasonic->biquadE[13];
		ultrasonic->biquadE[13] = inputSampleR; // DF1 right

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		ultrasonic->fpd ^= ultrasonic->fpd << 13;
		ultrasonic->fpd ^= ultrasonic->fpd >> 17;
		ultrasonic->fpd ^= ultrasonic->fpd << 5;
		inputSampleL += (((double)(ultrasonic->fpd) - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		ultrasonic->fpd ^= ultrasonic->fpd << 13;
		ultrasonic->fpd ^= ultrasonic->fpd >> 17;
		ultrasonic->fpd ^= ultrasonic->fpd << 5;
		inputSampleR += (((double)(ultrasonic->fpd) - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ULTRASONIC_URI,
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
