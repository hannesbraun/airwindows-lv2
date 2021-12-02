#include <lv2/core/lv2.h>

#include <stdint.h>
#include <stdlib.h>

#define LEFTOMONO_URI "https://hannesbraun.net/ns/lv2/airwindows/leftomono"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	// Port buffers
	const float* inputL;
	const float* inputR;
	float* outputL;
	float* outputR;
} LeftoMono;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	LeftoMono* leftoMono = (LeftoMono*) calloc(1, sizeof(LeftoMono));
	return (LV2_Handle) leftoMono;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	LeftoMono* leftoMono = (LeftoMono*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			leftoMono->inputL = (const float*) data;
			break;
		case INPUT_R:
			leftoMono->inputR = (const float*) data;
			break;
		case OUTPUT_L:
			leftoMono->outputL = (float*) data;
			break;
		case OUTPUT_R:
			leftoMono->outputR = (float*) data;
	}
}

static void activate(LV2_Handle instance) {}

static void run(LV2_Handle instance, uint32_t nSampleFrames)
{
	LeftoMono* leftoMono = (LeftoMono*) instance;

	const float* inputL = leftoMono->inputL;
	const float* inputR = leftoMono->inputR;
	float* outputL = leftoMono->outputL;
	float* outputR = leftoMono->outputR;

	while (nSampleFrames-- > 0) {
		*outputL = *inputL;
		*outputR = *inputL;

		inputL++;
		inputR++;
		outputL++;
		outputR++;
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
	LEFTOMONO_URI,
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
