#include <lv2/core/lv2.h>

#include <stdint.h>
#include <stdlib.h>

#define RIGHTOMONO_URI "https://hannesbraun.net/ns/lv2/airwindows/rightomono"

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
} RightoMono;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	RightoMono* rightoMono = (RightoMono*) calloc(1, sizeof(RightoMono));
	return (LV2_Handle) rightoMono;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	RightoMono* rightoMono = (RightoMono*) instance;

	switch ((PortIndex) port) {
	case INPUT_L:
		rightoMono->inputL = (const float*) data;
		break;
	case INPUT_R:
		rightoMono->inputR = (const float*) data;
		break;
	case OUTPUT_L:
		rightoMono->outputL = (float*) data;
		break;
	case OUTPUT_R:
		rightoMono->outputR = (float*) data;
	}
}

static void activate(LV2_Handle instance) {
	RightoMono* rightoMono = (RightoMono*) instance;
}

static void run(LV2_Handle instance, uint32_t nSampleFrames) {
	RightoMono* rightoMono = (RightoMono*) instance;

	const float* inputL = rightoMono->inputL;
	const float* inputR = rightoMono->inputR;
	float* outputL = rightoMono->outputL;
	float* outputR = rightoMono->outputR;

	while (nSampleFrames-- > 0) {
		*outputL = *inputR;
		*outputR = *inputR;

		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) {
	free(instance);
}

static const void* extension_data(const char* uri) {
	return NULL;
}

static const LV2_Descriptor descriptor = {
	RIGHTOMONO_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
	return index == 0 ? &descriptor : NULL;
}
