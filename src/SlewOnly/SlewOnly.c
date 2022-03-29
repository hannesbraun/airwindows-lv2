#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SLEWONLY_URI "https://hannesbraun.net/ns/lv2/airwindows/slewonly"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];

	double lastSampleL;
	double lastSampleR;
	uint32_t fpdL;
	uint32_t fpdR;
} SlewOnly;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	SlewOnly* slewonly = (SlewOnly*) calloc(1, sizeof(SlewOnly));
	return (LV2_Handle) slewonly;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	SlewOnly* slewonly = (SlewOnly*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			slewonly->input[0] = (const float*) data;
			break;
		case INPUT_R:
			slewonly->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			slewonly->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			slewonly->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	SlewOnly* slewonly = (SlewOnly*) instance;
	slewonly->lastSampleL = 0.0;
	slewonly->lastSampleR = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	SlewOnly* slewonly = (SlewOnly*) instance;

	const float* in1 = slewonly->input[0];
	const float* in2 = slewonly->input[1];
	float* out1 = slewonly->output[0];
	float* out2 = slewonly->output[1];

	double inputSampleL;
	double inputSampleR;
	double outputSampleL;
	double outputSampleR;
	double trim = 2.302585092994045684017991; //natural logarithm of 10

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = slewonly->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = slewonly->fpdR * 1.18e-17;

		outputSampleL = (inputSampleL - slewonly->lastSampleL) * trim;
		outputSampleR = (inputSampleR - slewonly->lastSampleR) * trim;
		slewonly->lastSampleL = inputSampleL;
		slewonly->lastSampleR = inputSampleR;
		if (outputSampleL > 1.0) outputSampleL = 1.0;
		if (outputSampleR > 1.0) outputSampleR = 1.0;
		if (outputSampleL < -1.0) outputSampleL = -1.0;
		if (outputSampleR < -1.0) outputSampleR = -1.0;

		*out1 = (float) outputSampleL;
		*out2 = (float) outputSampleR;

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
	SLEWONLY_URI,
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
