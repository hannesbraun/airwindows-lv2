#include <lv2/core/lv2.h>

#include <stdint.h>
#include <stdlib.h>

#define FLIPITY_URI "https://hannesbraun.net/ns/lv2/airwindows/flipity"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FLIPITY = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* flipity;

	// uint32_t fpdL;
	// uint32_t fpdR;
} Flipity;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Flipity* flipity = (Flipity*) calloc(1, sizeof(Flipity));
	return (LV2_Handle) flipity;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Flipity* flipity = (Flipity*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			flipity->input[0] = (const float*) data;
			break;
		case INPUT_R:
			flipity->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			flipity->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			flipity->output[1] = (float*) data;
			break;
		case FLIPITY:
			flipity->flipity = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	// Flipity* flipity = (Flipity*) instance;
	// flipity->fpdL = 1.0;
	// while (flipity->fpdL < 16386) flipity->fpdL = rand() * UINT32_MAX;
	// flipity->fpdR = 1.0;
	// while (flipity->fpdR < 16386) flipity->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Flipity* flipityInstance = (Flipity*) instance;

	const float* in1 = flipityInstance->input[0];
	const float* in2 = flipityInstance->input[1];
	float* out1 = flipityInstance->output[0];
	float* out2 = flipityInstance->output[1];

	int flipity = (int) (*flipityInstance->flipity);
	static const int kDry = 0;
	static const int kFlipL = 1;
	static const int kFlipR = 2;
	static const int kFlipLR = 3;
	static const int kSwap = 4;
	static const int kSwipL = 5;
	static const int kSwipR = 6;
	static const int kSwipLR = 7;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		double temp;

		switch (flipity) {
			case kDry:
				break;
			case kFlipL:
				inputSampleL = -inputSampleL;
				break;
			case kFlipR:
				inputSampleR = -inputSampleR;
				break;
			case kFlipLR:
				inputSampleL = -inputSampleL;
				inputSampleR = -inputSampleR;
				break;
			case kSwap:
				temp = inputSampleL;
				inputSampleL = inputSampleR;
				inputSampleR = temp;
				break;
			case kSwipL:
				temp = inputSampleL;
				inputSampleL = -inputSampleR;
				inputSampleR = temp;
				break;
			case kSwipR:
				temp = inputSampleL;
				inputSampleL = inputSampleR;
				inputSampleR = -temp;
				break;
			case kSwipLR:
			default:
				temp = inputSampleL;
				inputSampleL = -inputSampleR;
				inputSampleR = -temp;
				break;
		}

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
	FLIPITY_URI,
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
