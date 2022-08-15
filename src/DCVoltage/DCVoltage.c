#include <lv2/core/lv2.h>

#include <stdint.h>
#include <stdlib.h>

#define DCVOLTAGE_URI "https://hannesbraun.net/ns/lv2/airwindows/dcvoltage"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	VOLTAGE = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* voltage;
} DCVoltage;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	DCVoltage* dcvoltage = (DCVoltage*) calloc(1, sizeof(DCVoltage));
	return (LV2_Handle) dcvoltage;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	DCVoltage* dcvoltage = (DCVoltage*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			dcvoltage->input[0] = (const float*) data;
			break;
		case INPUT_R:
			dcvoltage->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			dcvoltage->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			dcvoltage->output[1] = (float*) data;
			break;
		case VOLTAGE:
			dcvoltage->voltage = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance) {}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	DCVoltage* dcvoltage = (DCVoltage*) instance;

	const float* in1 = dcvoltage->input[0];
	const float* in2 = dcvoltage->input[1];
	float* out1 = dcvoltage->output[0];
	float* out2 = dcvoltage->output[1];
	const float voltage = *dcvoltage->voltage;

	while (sampleFrames-- > 0) {
		*out1 = *in1 + voltage;
		*out2 = *in2 + voltage;

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
	DCVOLTAGE_URI,
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
