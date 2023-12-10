#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define RAWGLITTERS_URI "https://hannesbraun.net/ns/lv2/airwindows/rawglitters"

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
} RawGlitters;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	RawGlitters* rawGlitters = (RawGlitters*) calloc(1, sizeof(RawGlitters));
	return (LV2_Handle) rawGlitters;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	RawGlitters* rawGlitters = (RawGlitters*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			rawGlitters->input[0] = (const float*) data;
			break;
		case INPUT_R:
			rawGlitters->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			rawGlitters->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			rawGlitters->output[1] = (float*) data;
			break;
		case QUANTIZER:
			rawGlitters->quantizer = (const float*) data;
			break;
		case DEREZ:
			rawGlitters->deRez = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	RawGlitters* rawGlitters = (RawGlitters*) instance;

	rawGlitters->fpdL = 1.0;
	while (rawGlitters->fpdL < 16386) rawGlitters->fpdL = rand() * UINT32_MAX;
	rawGlitters->fpdR = 1.0;
	while (rawGlitters->fpdR < 16386) rawGlitters->fpdR = rand() * UINT32_MAX;
	rawGlitters->lastSampleL = 0.0;
	rawGlitters->lastSample2L = 0.0;
	rawGlitters->lastSampleR = 0.0;
	rawGlitters->lastSample2R = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	RawGlitters* rawGlitters = (RawGlitters*) instance;

	const float* in1 = rawGlitters->input[0];
	const float* in2 = rawGlitters->input[1];
	float* out1 = rawGlitters->output[0];
	float* out2 = rawGlitters->output[1];

	int processing = (int) *rawGlitters->quantizer;
	if (processing >= 2) {
		processing = 1;
	}
	bool highres = false;
	if (processing == 1) highres = true;
	float scaleFactor;
	if (highres) scaleFactor = 8388608.0;
	else scaleFactor = 32768.0;
	float derez = *rawGlitters->deRez;
	if (derez > 0.0) scaleFactor *= pow(1.0 - derez, 6);
	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	float outScale = scaleFactor;
	if (outScale < 8.0) outScale = 8.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = rawGlitters->fpdL * 1.18e-17;
		rawGlitters->fpdL ^= rawGlitters->fpdL << 13;
		rawGlitters->fpdL ^= rawGlitters->fpdL >> 17;
		rawGlitters->fpdL ^= rawGlitters->fpdL << 5;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = rawGlitters->fpdR * 1.18e-17;
		rawGlitters->fpdR ^= rawGlitters->fpdR << 13;
		rawGlitters->fpdR ^= rawGlitters->fpdR >> 17;
		rawGlitters->fpdR ^= rawGlitters->fpdR << 5;

		inputSampleL *= scaleFactor;
		inputSampleR *= scaleFactor;
		double outputSampleL;
		double outputSampleR;

		inputSampleL += 0.381966011250105;
		inputSampleR += 0.381966011250105;

		if ((rawGlitters->lastSampleL + rawGlitters->lastSampleL) <= (inputSampleL + rawGlitters->lastSample2L)) outputSampleL = floor(rawGlitters->lastSampleL);
		else outputSampleL = floor(rawGlitters->lastSampleL + 1.0); // round down or up based on whether it softens treble angles

		if ((rawGlitters->lastSampleR + rawGlitters->lastSampleR) <= (inputSampleR + rawGlitters->lastSample2R)) outputSampleR = floor(rawGlitters->lastSampleR);
		else outputSampleR = floor(rawGlitters->lastSampleR + 1.0); // round down or up based on whether it softens treble angles

		rawGlitters->lastSample2L = rawGlitters->lastSampleL;
		rawGlitters->lastSampleL = inputSampleL; // we retain three samples in a row

		rawGlitters->lastSample2R = rawGlitters->lastSampleR;
		rawGlitters->lastSampleR = inputSampleR; // we retain three samples in a row

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
	RAWGLITTERS_URI,
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
