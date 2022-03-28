#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define INFRASONIC_URI "https://hannesbraun.net/ns/lv2/airwindows/infrasonic"

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
	double biquadB[15];
	double biquadC[15];
	double biquadD[15];
	double biquadE[15];
	uint32_t fpdL;
	uint32_t fpdR;
} Infrasonic;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Infrasonic* infrasonic = (Infrasonic*) calloc(1, sizeof(Infrasonic));
	infrasonic->sampleRate = rate;
	return (LV2_Handle) infrasonic;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Infrasonic* infrasonic = (Infrasonic*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			infrasonic->input[0] = (const float*) data;
			break;
		case INPUT_R:
			infrasonic->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			infrasonic->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			infrasonic->output[1] = (float*) data;
	}
}

static void activate(LV2_Handle instance)
{
	Infrasonic* infrasonic = (Infrasonic*) instance;
	for (int x = 0; x < 15; x++) {
		infrasonic->biquadA[x] = 0.0;
		infrasonic->biquadB[x] = 0.0;
		infrasonic->biquadC[x] = 0.0;
		infrasonic->biquadD[x] = 0.0;
		infrasonic->biquadE[x] = 0.0;
	}
	infrasonic->fpdL = 1.0;
	while (infrasonic->fpdL < 16386) infrasonic->fpdL = rand() * UINT32_MAX;
	infrasonic->fpdR = 1.0;
	while (infrasonic->fpdR < 16386) infrasonic->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Infrasonic* infrasonic = (Infrasonic*) instance;

	const float* in1 = infrasonic->input[0];
	const float* in2 = infrasonic->input[1];
	float* out1 = infrasonic->output[0];
	float* out2 = infrasonic->output[1];

	const double init = 20.0 / infrasonic->sampleRate;
	infrasonic->biquadA[0] = init;
	infrasonic->biquadB[0] = init;
	infrasonic->biquadC[0] = init;
	infrasonic->biquadD[0] = init;
	infrasonic->biquadE[0] = init;
	infrasonic->biquadA[1] = 0.50623256;
	infrasonic->biquadB[1] = 0.56116312;
	infrasonic->biquadC[1] = 0.70710678;
	infrasonic->biquadD[1] = 1.10134463;
	infrasonic->biquadE[1] = 3.19622661; //tenth order Butterworth out of five biquads

	double K = tan(M_PI * infrasonic->biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / infrasonic->biquadA[1] + K * K);
	infrasonic->biquadA[2] = norm;
	infrasonic->biquadA[3] = -2.0 * infrasonic->biquadA[2];
	infrasonic->biquadA[4] = infrasonic->biquadA[2];
	infrasonic->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	infrasonic->biquadA[6] = (1.0 - K / infrasonic->biquadA[1] + K * K) * norm;

	K = tan(M_PI * infrasonic->biquadA[0]);
	norm = 1.0 / (1.0 + K / infrasonic->biquadB[1] + K * K);
	infrasonic->biquadB[2] = norm;
	infrasonic->biquadB[3] = -2.0 * infrasonic->biquadB[2];
	infrasonic->biquadB[4] = infrasonic->biquadB[2];
	infrasonic->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	infrasonic->biquadB[6] = (1.0 - K / infrasonic->biquadB[1] + K * K) * norm;

	K = tan(M_PI * infrasonic->biquadC[0]);
	norm = 1.0 / (1.0 + K / infrasonic->biquadC[1] + K * K);
	infrasonic->biquadC[2] = norm;
	infrasonic->biquadC[3] = -2.0 * infrasonic->biquadC[2];
	infrasonic->biquadC[4] = infrasonic->biquadC[2];
	infrasonic->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	infrasonic->biquadC[6] = (1.0 - K / infrasonic->biquadC[1] + K * K) * norm;

	K = tan(M_PI * infrasonic->biquadD[0]);
	norm = 1.0 / (1.0 + K / infrasonic->biquadD[1] + K * K);
	infrasonic->biquadD[2] = norm;
	infrasonic->biquadD[3] = -2.0 * infrasonic->biquadD[2];
	infrasonic->biquadD[4] = infrasonic->biquadD[2];
	infrasonic->biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	infrasonic->biquadD[6] = (1.0 - K / infrasonic->biquadD[1] + K * K) * norm;

	K = tan(M_PI * infrasonic->biquadE[0]);
	norm = 1.0 / (1.0 + K / infrasonic->biquadE[1] + K * K);
	infrasonic->biquadE[2] = norm;
	infrasonic->biquadE[3] = -2.0 * infrasonic->biquadE[2];
	infrasonic->biquadE[4] = infrasonic->biquadE[2];
	infrasonic->biquadE[5] = 2.0 * (K * K - 1.0) * norm;
	infrasonic->biquadE[6] = (1.0 - K / infrasonic->biquadE[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = infrasonic->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = infrasonic->fpdR * 1.18e-17;

		double outSampleL = infrasonic->biquadA[2] * inputSampleL + infrasonic->biquadA[3] * infrasonic->biquadA[7] + infrasonic->biquadA[4] * infrasonic->biquadA[8] - infrasonic->biquadA[5] * infrasonic->biquadA[9] - infrasonic->biquadA[6] * infrasonic->biquadA[10];
		infrasonic->biquadA[8] = infrasonic->biquadA[7];
		infrasonic->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		infrasonic->biquadA[10] = infrasonic->biquadA[9];
		infrasonic->biquadA[9] = inputSampleL; //DF1 left

		outSampleL = infrasonic->biquadB[2] * inputSampleL + infrasonic->biquadB[3] * infrasonic->biquadB[7] + infrasonic->biquadB[4] * infrasonic->biquadB[8] - infrasonic->biquadB[5] * infrasonic->biquadB[9] - infrasonic->biquadB[6] * infrasonic->biquadB[10];
		infrasonic->biquadB[8] = infrasonic->biquadB[7];
		infrasonic->biquadB[7] = inputSampleL;
		inputSampleL = outSampleL;
		infrasonic->biquadB[10] = infrasonic->biquadB[9];
		infrasonic->biquadB[9] = inputSampleL; //DF1 left

		outSampleL = infrasonic->biquadC[2] * inputSampleL + infrasonic->biquadC[3] * infrasonic->biquadC[7] + infrasonic->biquadC[4] * infrasonic->biquadC[8] - infrasonic->biquadC[5] * infrasonic->biquadC[9] - infrasonic->biquadC[6] * infrasonic->biquadC[10];
		infrasonic->biquadC[8] = infrasonic->biquadC[7];
		infrasonic->biquadC[7] = inputSampleL;
		inputSampleL = outSampleL;
		infrasonic->biquadC[10] = infrasonic->biquadC[9];
		infrasonic->biquadC[9] = inputSampleL; //DF1 left

		outSampleL = infrasonic->biquadD[2] * inputSampleL + infrasonic->biquadD[3] * infrasonic->biquadD[7] + infrasonic->biquadD[4] * infrasonic->biquadD[8] - infrasonic->biquadD[5] * infrasonic->biquadD[9] - infrasonic->biquadD[6] * infrasonic->biquadD[10];
		infrasonic->biquadD[8] = infrasonic->biquadD[7];
		infrasonic->biquadD[7] = inputSampleL;
		inputSampleL = outSampleL;
		infrasonic->biquadD[10] = infrasonic->biquadD[9];
		infrasonic->biquadD[9] = inputSampleL; //DF1 left

		outSampleL = infrasonic->biquadE[2] * inputSampleL + infrasonic->biquadE[3] * infrasonic->biquadE[7] + infrasonic->biquadE[4] * infrasonic->biquadE[8] - infrasonic->biquadE[5] * infrasonic->biquadE[9] - infrasonic->biquadE[6] * infrasonic->biquadE[10];
		infrasonic->biquadE[8] = infrasonic->biquadE[7];
		infrasonic->biquadE[7] = inputSampleL;
		inputSampleL = outSampleL;
		infrasonic->biquadE[10] = infrasonic->biquadE[9];
		infrasonic->biquadE[9] = inputSampleL; //DF1 left

		double outSampleR = infrasonic->biquadA[2] * inputSampleR + infrasonic->biquadA[3] * infrasonic->biquadA[11] + infrasonic->biquadA[4] * infrasonic->biquadA[12] - infrasonic->biquadA[5] * infrasonic->biquadA[13] - infrasonic->biquadA[6] * infrasonic->biquadA[14];
		infrasonic->biquadA[12] = infrasonic->biquadA[11];
		infrasonic->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		infrasonic->biquadA[14] = infrasonic->biquadA[13];
		infrasonic->biquadA[13] = inputSampleR; //DF1 right

		outSampleR = infrasonic->biquadB[2] * inputSampleR + infrasonic->biquadB[3] * infrasonic->biquadB[11] + infrasonic->biquadB[4] * infrasonic->biquadB[12] - infrasonic->biquadB[5] * infrasonic->biquadB[13] - infrasonic->biquadB[6] * infrasonic->biquadB[14];
		infrasonic->biquadB[12] = infrasonic->biquadB[11];
		infrasonic->biquadB[11] = inputSampleR;
		inputSampleR = outSampleR;
		infrasonic->biquadB[14] = infrasonic->biquadB[13];
		infrasonic->biquadB[13] = inputSampleR; //DF1 right

		outSampleR = infrasonic->biquadC[2] * inputSampleR + infrasonic->biquadC[3] * infrasonic->biquadC[11] + infrasonic->biquadC[4] * infrasonic->biquadC[12] - infrasonic->biquadC[5] * infrasonic->biquadC[13] - infrasonic->biquadC[6] * infrasonic->biquadC[14];
		infrasonic->biquadC[12] = infrasonic->biquadC[11];
		infrasonic->biquadC[11] = inputSampleR;
		inputSampleR = outSampleR;
		infrasonic->biquadC[14] = infrasonic->biquadC[13];
		infrasonic->biquadC[13] = inputSampleR; //DF1 right

		outSampleR = infrasonic->biquadD[2] * inputSampleR + infrasonic->biquadD[3] * infrasonic->biquadD[11] + infrasonic->biquadD[4] * infrasonic->biquadD[12] - infrasonic->biquadD[5] * infrasonic->biquadD[13] - infrasonic->biquadD[6] * infrasonic->biquadD[14];
		infrasonic->biquadD[12] = infrasonic->biquadD[11];
		infrasonic->biquadD[11] = inputSampleR;
		inputSampleR = outSampleR;
		infrasonic->biquadD[14] = infrasonic->biquadD[13];
		infrasonic->biquadD[13] = inputSampleR; //DF1 right

		outSampleR = infrasonic->biquadE[2] * inputSampleR + infrasonic->biquadE[3] * infrasonic->biquadE[11] + infrasonic->biquadE[4] * infrasonic->biquadE[12] - infrasonic->biquadE[5] * infrasonic->biquadE[13] - infrasonic->biquadE[6] * infrasonic->biquadE[14];
		infrasonic->biquadE[12] = infrasonic->biquadE[11];
		infrasonic->biquadE[11] = inputSampleR;
		inputSampleR = outSampleR;
		infrasonic->biquadE[14] = infrasonic->biquadE[13];
		infrasonic->biquadE[13] = inputSampleR; //DF1 right

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		infrasonic->fpdL ^= infrasonic->fpdL << 13;
		infrasonic->fpdL ^= infrasonic->fpdL >> 17;
		infrasonic->fpdL ^= infrasonic->fpdL << 5;
		inputSampleL += (((double)infrasonic->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		infrasonic->fpdR ^= infrasonic->fpdR << 13;
		infrasonic->fpdR ^= infrasonic->fpdR >> 17;
		infrasonic->fpdR ^= infrasonic->fpdR << 5;
		inputSampleR += (((double)infrasonic->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	INFRASONIC_URI,
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
