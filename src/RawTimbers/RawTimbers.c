#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define RAWTIMBERS_URI "https://hannesbraun.net/ns/lv2/airwindows/rawtimbers"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	QUANTIZER = 4,
	DEREZ = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* quantizer;
	const float* deRez;

	double lastSampleL;
	double lastSample2L;
	double lastSampleR;
	double lastSample2R;
	uint32_t fpdL;
	uint32_t fpdR;
} RawTimbers;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	RawTimbers* rawTimbers = (RawTimbers*) calloc(1, sizeof(RawTimbers));
	return (LV2_Handle) rawTimbers;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	RawTimbers* rawTimbers = (RawTimbers*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			rawTimbers->input[0] = (const float*) data;
			break;
		case INPUT_R:
			rawTimbers->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			rawTimbers->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			rawTimbers->output[1] = (float*) data;
			break;
		case QUANTIZER:
			rawTimbers->quantizer = (const float*) data;
			break;
		case DEREZ:
			rawTimbers->deRez = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	RawTimbers* rawTimbers = (RawTimbers*) instance;

	rawTimbers->fpdL = 1.0;
	while (rawTimbers->fpdL < 16386) rawTimbers->fpdL = rand() * UINT32_MAX;
	rawTimbers->fpdR = 1.0;
	while (rawTimbers->fpdR < 16386) rawTimbers->fpdR = rand() * UINT32_MAX;
	rawTimbers->lastSampleL = 0.0;
	rawTimbers->lastSample2L = 0.0;
	rawTimbers->lastSampleR = 0.0;
	rawTimbers->lastSample2R = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	RawTimbers* rawTimbers = (RawTimbers*) instance;

	const float* in1 = rawTimbers->input[0];
	const float* in2 = rawTimbers->input[1];
	float* out1 = rawTimbers->output[0];
	float* out2 = rawTimbers->output[1];

	int processing = (int) *rawTimbers->quantizer;
	if (processing >= 2) {
		processing = 1;
	}
	bool highres = false;
	if (processing == 1) highres = true;
	float scaleFactor;
	if (highres) scaleFactor = 8388608.0;
	else scaleFactor = 32768.0;
	float derez = *rawTimbers->deRez;
	if (derez > 0.0) scaleFactor *= pow(1.0 - derez, 6);
	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	float outScale = scaleFactor;
	if (outScale < 8.0) outScale = 8.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = rawTimbers->fpdL * 1.18e-17;
		rawTimbers->fpdL ^= rawTimbers->fpdL << 13;
		rawTimbers->fpdL ^= rawTimbers->fpdL >> 17;
		rawTimbers->fpdL ^= rawTimbers->fpdL << 5;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = rawTimbers->fpdR * 1.18e-17;
		rawTimbers->fpdR ^= rawTimbers->fpdR << 13;
		rawTimbers->fpdR ^= rawTimbers->fpdR >> 17;
		rawTimbers->fpdR ^= rawTimbers->fpdR << 5;

		inputSampleL *= scaleFactor;
		inputSampleR *= scaleFactor;
		double outputSampleL;
		double outputSampleR;

		inputSampleL += 0.381966011250105;
		inputSampleR += 0.381966011250105;

		if ((rawTimbers->lastSampleL + rawTimbers->lastSampleL) >= (inputSampleL + rawTimbers->lastSample2L)) outputSampleL = floor(rawTimbers->lastSampleL);
		else outputSampleL = floor(rawTimbers->lastSampleL + 1.0); // round down or up based on whether it softens treble angles

		if ((rawTimbers->lastSampleR + rawTimbers->lastSampleR) >= (inputSampleR + rawTimbers->lastSample2R)) outputSampleR = floor(rawTimbers->lastSampleR);
		else outputSampleR = floor(rawTimbers->lastSampleR + 1.0); // round down or up based on whether it softens treble angles

		rawTimbers->lastSample2L = rawTimbers->lastSampleL;
		rawTimbers->lastSampleL = inputSampleL; // we retain three samples in a row

		rawTimbers->lastSample2R = rawTimbers->lastSampleR;
		rawTimbers->lastSampleR = inputSampleR; // we retain three samples in a row

		*out1 = (float) outputSampleL / outScale;
		*out2 = (float) outputSampleR / outScale;

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
	RAWTIMBERS_URI,
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
