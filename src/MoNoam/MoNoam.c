#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define MONOAM_URI "https://hannesbraun.net/ns/lv2/airwindows/monoam"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	MONOTOR = 4
} PortIndex;

const int kBYPASS = 0;
const int kMONO = 1;
const int kMONOR = 2;
const int kMONOL = 3;
const int kSIDE = 4;
const int kSIDEM = 5;
const int kSIDER = 6;
const int kSIDEL = 7;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* monotor;

	uint32_t fpdL;
	uint32_t fpdR;
} MoNoam;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	MoNoam* moNoam = (MoNoam*) calloc(1, sizeof(MoNoam));
	return (LV2_Handle) moNoam;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	MoNoam* moNoam = (MoNoam*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			moNoam->input[0] = (const float*) data;
			break;
		case INPUT_R:
			moNoam->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			moNoam->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			moNoam->output[1] = (float*) data;
			break;
		case MONOTOR:
			moNoam->monotor = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	MoNoam* moNoam = (MoNoam*) instance;
	moNoam->fpdL = 1.0;
	while (moNoam->fpdL < 16386) moNoam->fpdL = rand() * UINT32_MAX;
	moNoam->fpdR = 1.0;
	while (moNoam->fpdR < 16386) moNoam->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	MoNoam* moNoam = (MoNoam*) instance;

	const float* in1 = moNoam->input[0];
	const float* in2 = moNoam->input[1];
	float* out1 = moNoam->output[0];
	float* out2 = moNoam->output[1];

	int processing = (int) *moNoam->monotor;
	if (processing > 7) {
		processing = 7;
	} else if (processing < 0) {
		processing = 0;
	}

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = moNoam->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = moNoam->fpdR * 1.18e-17;

		double mid;
		mid = inputSampleL + inputSampleR;
		double side;
		side = inputSampleL - inputSampleR;

		if (processing == kMONO || processing == kMONOR || processing == kMONOL) side = 0.0;
		if (processing == kSIDE || processing == kSIDEM || processing == kSIDER || processing == kSIDEL) mid = 0.0;

		inputSampleL = (mid + side) / 2.0;
		inputSampleR = (mid - side) / 2.0;

		if (processing == kSIDEM || processing == kSIDER || processing == kSIDEL) inputSampleL = -inputSampleL;

		if (processing == kMONOR || processing == kSIDER) inputSampleL = 0.0;
		if (processing == kMONOL || processing == kSIDEL) inputSampleR = 0.0;

		if (processing == kBYPASS) {
			inputSampleL = *in1;
			inputSampleR = *in2;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		moNoam->fpdL ^= moNoam->fpdL << 13;
		moNoam->fpdL ^= moNoam->fpdL >> 17;
		moNoam->fpdL ^= moNoam->fpdL << 5;
		inputSampleL += (((double) moNoam->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		moNoam->fpdR ^= moNoam->fpdR << 13;
		moNoam->fpdR ^= moNoam->fpdR >> 17;
		moNoam->fpdR ^= moNoam->fpdR << 5;
		inputSampleR += (((double) moNoam->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MONOAM_URI,
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
