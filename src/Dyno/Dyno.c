#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define DYNO_URI "https://hannesbraun.net/ns/lv2/airwindows/dyno"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* gain;

	uint32_t fpdL;
	uint32_t fpdR;
} Dyno;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Dyno* dyno = (Dyno*) calloc(1, sizeof(Dyno));
	return (LV2_Handle) dyno;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Dyno* dyno = (Dyno*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			dyno->input[0] = (const float*) data;
			break;
		case INPUT_R:
			dyno->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			dyno->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			dyno->output[1] = (float*) data;
			break;
		case GAIN:
			dyno->gain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Dyno* dyno = (Dyno*) instance;
	dyno->fpdL = 1.0;
	while (dyno->fpdL < 16386) dyno->fpdL = rand() * UINT32_MAX;
	dyno->fpdR = 1.0;
	while (dyno->fpdR < 16386) dyno->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Dyno* dyno_instance = (Dyno*) instance;

	const float* in1 = dyno_instance->input[0];
	const float* in2 = dyno_instance->input[1];
	float* out1 = dyno_instance->output[0];
	float* out2 = dyno_instance->output[1];

	double gain = pow(10.0, *dyno_instance->gain / 20.0);

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = dyno_instance->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = dyno_instance->fpdR * 1.18e-17;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		double dyno = pow(fabs(inputSampleL), 4);
		if (dyno > 0.0) inputSampleL = (sin(inputSampleL * dyno) / dyno) * 1.1654321;
		//dyno is the one that tries to raise peak energy
		dyno = pow(fabs(inputSampleR), 4);
		if (dyno > 0.0) inputSampleR = (sin(inputSampleR * dyno) / dyno) * 1.1654321;
		//dyno is the one that tries to raise peak energy

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		dyno_instance->fpdL ^= dyno_instance->fpdL << 13;
		dyno_instance->fpdL ^= dyno_instance->fpdL >> 17;
		dyno_instance->fpdL ^= dyno_instance->fpdL << 5;
		inputSampleL += (((double)dyno_instance->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		dyno_instance->fpdR ^= dyno_instance->fpdR << 13;
		dyno_instance->fpdR ^= dyno_instance->fpdR >> 17;
		dyno_instance->fpdR ^= dyno_instance->fpdR << 5;
		inputSampleR += (((double)dyno_instance->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	DYNO_URI,
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
