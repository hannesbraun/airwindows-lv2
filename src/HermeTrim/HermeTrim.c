#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define HERMETRIM_URI "https://hannesbraun.net/ns/lv2/airwindows/hermetrim"

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
} HermeTrim;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	HermeTrim* hermetrim = (HermeTrim*) calloc(1, sizeof(HermeTrim));
	return (LV2_Handle) hermetrim;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	HermeTrim* hermetrim = (HermeTrim*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			hermetrim->input[0] = (const float*) data;
			break;
		case INPUT_R:
			hermetrim->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			hermetrim->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			hermetrim->output[1] = (float*) data;
			break;
		case LEFT:
			hermetrim->left = (const float*) data;
			break;
		case RIGHT:
			hermetrim->right = (const float*) data;
			break;
		case MID:
			hermetrim->mid = (const float*) data;
			break;
		case SIDE:
			hermetrim->side = (const float*) data;
			break;
		case MASTER:
			hermetrim->master = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	HermeTrim* hermetrim = (HermeTrim*) instance;
	hermetrim->fpdL = 1.0;
	while (hermetrim->fpdL < 16386) hermetrim->fpdL = rand() * UINT32_MAX;
	hermetrim->fpdR = 1.0;
	while (hermetrim->fpdR < 16386) hermetrim->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	HermeTrim* hermetrim = (HermeTrim*) instance;

	const float* in1 = hermetrim->input[0];
	const float* in2 = hermetrim->input[1];
	float* out1 = hermetrim->output[0];
	float* out2 = hermetrim->output[1];

	double leftgain = pow(10.0, *hermetrim->left / 20.0);
	double rightgain = pow(10.0, *hermetrim->right / 20.0);
	double midgain = pow(10.0, *hermetrim->mid / 20.0);
	double sidegain = pow(10.0, *hermetrim->side / 20.0);
	double mastergain = pow(10.0, *hermetrim->master / 20.0) * 0.5;

	double inputSampleL;
	double inputSampleR;
	double mid;
	double side;

	leftgain *= mastergain;
	rightgain *= mastergain;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = hermetrim->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = hermetrim->fpdR * 1.18e-17;

		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;
		mid *= midgain;
		side *= sidegain;
		inputSampleL = (mid + side) * leftgain;
		inputSampleR = (mid - side) * rightgain;
		//contains mastergain and the gain trim fixing the mid/side

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		hermetrim->fpdL ^= hermetrim->fpdL << 13;
		hermetrim->fpdL ^= hermetrim->fpdL >> 17;
		hermetrim->fpdL ^= hermetrim->fpdL << 5;
		inputSampleL += (((double)hermetrim->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		hermetrim->fpdR ^= hermetrim->fpdR << 13;
		hermetrim->fpdR ^= hermetrim->fpdR >> 17;
		hermetrim->fpdR ^= hermetrim->fpdR << 5;
		inputSampleR += (((double)hermetrim->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	HERMETRIM_URI,
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
