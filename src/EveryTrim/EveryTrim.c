#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define EVERYTRIM_URI "https://hannesbraun.net/ns/lv2/airwindows/everytrim"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	LEFT = 4,
	RIGHT = 5,
	MID = 6,
	SIDE = 7,
	MASTER = 8
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* left;
	const float* right;
	const float* mid;
	const float* side;
	const float* master;

	uint32_t fpdL;
	uint32_t fpdR;
} EveryTrim;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	EveryTrim* everytrim = (EveryTrim*) calloc(1, sizeof(EveryTrim));
	return (LV2_Handle) everytrim;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	EveryTrim* everytrim = (EveryTrim*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			everytrim->input[0] = (const float*) data;
			break;
		case INPUT_R:
			everytrim->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			everytrim->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			everytrim->output[1] = (float*) data;
			break;
		case LEFT:
			everytrim->left = (const float*) data;
			break;
		case RIGHT:
			everytrim->right = (const float*) data;
			break;
		case MID:
			everytrim->mid = (const float*) data;
			break;
		case SIDE:
			everytrim->side = (const float*) data;
			break;
		case MASTER:
			everytrim->master = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	EveryTrim* everytrim = (EveryTrim*) instance;
	everytrim->fpdL = 1.0;
	while (everytrim->fpdL < 16386) everytrim->fpdL = rand() * UINT32_MAX;
	everytrim->fpdR = 1.0;
	while (everytrim->fpdR < 16386) everytrim->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	EveryTrim* everytrim = (EveryTrim*) instance;

	const float* in1 = everytrim->input[0];
	const float* in2 = everytrim->input[1];
	float* out1 = everytrim->output[0];
	float* out2 = everytrim->output[1];

	double leftgain = pow(10.0, *everytrim->left / 20.0);
	double rightgain = pow(10.0, *everytrim->right / 20.0);
	double midgain = pow(10.0, *everytrim->mid / 20.0);
	double sidegain = pow(10.0, *everytrim->side / 20.0);
	double mastergain = pow(10.0, *everytrim->master / 20.0) * 0.5;

	double inputSampleL;
	double inputSampleR;
	double mid;
	double side;

	leftgain *= mastergain;
	rightgain *= mastergain;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = everytrim->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = everytrim->fpdR * 1.18e-17;

		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;
		mid *= midgain;
		side *= sidegain;
		inputSampleL = (mid + side) * leftgain;
		inputSampleR = (mid - side) * rightgain;
		// contains mastergain and the gain trim fixing the mid/side

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		everytrim->fpdL ^= everytrim->fpdL << 13;
		everytrim->fpdL ^= everytrim->fpdL >> 17;
		everytrim->fpdL ^= everytrim->fpdL << 5;
		inputSampleL += (((double) everytrim->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		everytrim->fpdR ^= everytrim->fpdR << 13;
		everytrim->fpdR ^= everytrim->fpdR >> 17;
		everytrim->fpdR ^= everytrim->fpdR << 5;
		inputSampleR += (((double) everytrim->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		// end 32 bit stereo floating point dither

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
	EVERYTRIM_URI,
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
