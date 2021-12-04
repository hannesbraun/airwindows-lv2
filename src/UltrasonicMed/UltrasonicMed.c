#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ULTRASONICMED_URI "https://hannesbraun.net/ns/lv2/airwindows/ultrasonicmed"

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
	uint32_t fpd;
} UltrasonicMed;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	UltrasonicMed* ultrasonicMed = (UltrasonicMed*) calloc(1, sizeof(UltrasonicMed));
	ultrasonicMed->sampleRate = rate;
	return (LV2_Handle) ultrasonicMed;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	UltrasonicMed* ultrasonicMed = (UltrasonicMed*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			ultrasonicMed->input[0] = (const float*) data;
			break;
		case INPUT_R:
			ultrasonicMed->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			ultrasonicMed->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			ultrasonicMed->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	UltrasonicMed* ultrasonicMed = (UltrasonicMed*) instance;
	for (int x = 0; x < 15; x++) {
		ultrasonicMed->biquadA[x] = 0.0;
		ultrasonicMed->biquadB[x] = 0.0;
	}
	ultrasonicMed->fpd = 1.0;
	while (ultrasonicMed->fpd < 16386) {
		ultrasonicMed->fpd = rand() * UINT32_MAX;
	}
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	UltrasonicMed* ultrasonicMed = (UltrasonicMed*) instance;

	const float* in1 = ultrasonicMed->input[0];
	const float* in2 = ultrasonicMed->input[1];
	float* out1 = ultrasonicMed->output[0];
	float* out2 = ultrasonicMed->output[1];

	ultrasonicMed->biquadA[0] = 24000.0 / ultrasonicMed->sampleRate;
	ultrasonicMed->biquadB[0] = 22000.0 / ultrasonicMed->sampleRate;
	if (ultrasonicMed->sampleRate < 88000.0) {
		ultrasonicMed->biquadA[0] = 21000.0 / ultrasonicMed->sampleRate;
		ultrasonicMed->biquadB[0] = 20000.0 / ultrasonicMed->sampleRate;
	}

	ultrasonicMed->biquadA[1] = 0.70710678;
	ultrasonicMed->biquadB[1] = 0.70710678;

	long double K = tan(M_PI * ultrasonicMed->biquadA[0]); //lowpass
	long double norm = 1.0 / (1.0 + K / ultrasonicMed->biquadA[1] + K * K);
	ultrasonicMed->biquadA[2] = K * K * norm;
	ultrasonicMed->biquadA[3] = 2.0 * ultrasonicMed->biquadA[2];
	ultrasonicMed->biquadA[4] = ultrasonicMed->biquadA[2];
	ultrasonicMed->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonicMed->biquadA[6] = (1.0 - K / ultrasonicMed->biquadA[1] + K * K) * norm;

	K = tan(M_PI * ultrasonicMed->biquadB[0]);
	norm = 1.0 / (1.0 + K / ultrasonicMed->biquadB[1] + K * K);
	ultrasonicMed->biquadB[2] = K * K * norm;
	ultrasonicMed->biquadB[3] = 2.0 * ultrasonicMed->biquadB[2];
	ultrasonicMed->biquadB[4] = ultrasonicMed->biquadB[2];
	ultrasonicMed->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	ultrasonicMed->biquadB[6] = (1.0 - K / ultrasonicMed->biquadB[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabsl(inputSampleL) < 1.18e-37) inputSampleL = ultrasonicMed->fpd * 1.18e-37;
		if (fabsl(inputSampleR) < 1.18e-37) inputSampleR = ultrasonicMed->fpd * 1.18e-37;

		long double outSampleL = ultrasonicMed->biquadA[2] * inputSampleL + ultrasonicMed->biquadA[3] * ultrasonicMed->biquadA[7] + ultrasonicMed->biquadA[4] * ultrasonicMed->biquadA[8] - ultrasonicMed->biquadA[5] * ultrasonicMed->biquadA[9] - ultrasonicMed->biquadA[6] * ultrasonicMed->biquadA[10];
		ultrasonicMed->biquadA[8] = ultrasonicMed->biquadA[7];
		ultrasonicMed->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonicMed->biquadA[10] = ultrasonicMed->biquadA[9];
		ultrasonicMed->biquadA[9] = inputSampleL; //DF1 left

		outSampleL = ultrasonicMed->biquadB[2] * inputSampleL + ultrasonicMed->biquadB[3] * ultrasonicMed->biquadB[7] + ultrasonicMed->biquadB[4] * ultrasonicMed->biquadB[8] - ultrasonicMed->biquadB[5] * ultrasonicMed->biquadB[9] - ultrasonicMed->biquadB[6] * ultrasonicMed->biquadB[10];
		ultrasonicMed->biquadB[8] = ultrasonicMed->biquadB[7];
		ultrasonicMed->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		ultrasonicMed->biquadB[10] = ultrasonicMed->biquadB[9];
		ultrasonicMed->biquadB[9] = inputSampleL; //DF1 left

		long double outSampleR = ultrasonicMed->biquadA[2] * inputSampleR + ultrasonicMed->biquadA[3] * ultrasonicMed->biquadA[11] + ultrasonicMed->biquadA[4] * ultrasonicMed->biquadA[12] - ultrasonicMed->biquadA[5] * ultrasonicMed->biquadA[13] - ultrasonicMed->biquadA[6] * ultrasonicMed->biquadA[14];
		ultrasonicMed->biquadA[12] = ultrasonicMed->biquadA[11];
		ultrasonicMed->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonicMed->biquadA[14] = ultrasonicMed->biquadA[13];
		ultrasonicMed->biquadA[13] = inputSampleR; //DF1 right

		outSampleR = ultrasonicMed->biquadB[2] * inputSampleR + ultrasonicMed->biquadB[3] * ultrasonicMed->biquadB[11] + ultrasonicMed->biquadB[4] * ultrasonicMed->biquadB[12] - ultrasonicMed->biquadB[5] * ultrasonicMed->biquadB[13] - ultrasonicMed->biquadB[6] * ultrasonicMed->biquadB[14];
		ultrasonicMed->biquadB[12] = ultrasonicMed->biquadB[11];
		ultrasonicMed->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		ultrasonicMed->biquadB[14] = ultrasonicMed->biquadB[13];
		ultrasonicMed->biquadB[13] = inputSampleR; //DF1 right

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		ultrasonicMed->fpd ^= ultrasonicMed->fpd << 13;
		ultrasonicMed->fpd ^= ultrasonicMed->fpd >> 17;
		ultrasonicMed->fpd ^= ultrasonicMed->fpd << 5;
		inputSampleL += (((double) ultrasonicMed->fpd - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		ultrasonicMed->fpd ^= ultrasonicMed->fpd << 13;
		ultrasonicMed->fpd ^= ultrasonicMed->fpd >> 17;
		ultrasonicMed->fpd ^= ultrasonicMed->fpd << 5;
		inputSampleR += (((double) ultrasonicMed->fpd - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ULTRASONICMED_URI,
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
