#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TEMPLATE_URI "https://hannesbraun.net/ns/lv2/airwindows/template"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TEMPLATE_PARAM = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* templateparam;
} Template;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Template* template = (Template*) calloc(1, sizeof(Template));
	template->sampleRate = rate;
	return (LV2_Handle) template;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Template* template = (Template*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			template->input[0] = (const float*) data;
			break;
		case INPUT_R:
			template->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			template->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			template->output[1] = (float*) data;
			break;
		case TEMPLATE_PARAM:
			template->templateparam = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Template* template = (Template*) instance;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Template* template = (Template*) instance;

	const float* in1 = template->input[0];
	const float* in2 = template->input[1];
	float* out1 = template->output[0];
	float* out2 = template->output[1];

	while (sampleFrames-- > 0) {
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
	TEMPLATE_URI,
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
