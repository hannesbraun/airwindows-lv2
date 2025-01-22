#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define HYPERSOFT_URI "https://hannesbraun.net/ns/lv2/airwindows/hypersoft"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4,
	DEPTH = 5,
	BRIGHT = 6,
	OUTPUT = 7
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* inputGain;
	const float* depth;
	const float* bright;
	const float* outputGain;

	double lastSampleL;
	double lastSampleR;

	uint32_t fpdL;
	uint32_t fpdR;
} Hypersoft;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Hypersoft* hypersoft = (Hypersoft*) calloc(1, sizeof(Hypersoft));
	return (LV2_Handle) hypersoft;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Hypersoft* hypersoft = (Hypersoft*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			hypersoft->input[0] = (const float*) data;
			break;
		case INPUT_R:
			hypersoft->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			hypersoft->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			hypersoft->output[1] = (float*) data;
			break;
		case INPUT:
			hypersoft->inputGain = (const float*) data;
			break;
		case DEPTH:
			hypersoft->depth = (const float*) data;
			break;
		case BRIGHT:
			hypersoft->bright = (const float*) data;
			break;
		case OUTPUT:
			hypersoft->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Hypersoft* hypersoft = (Hypersoft*) instance;

	hypersoft->lastSampleL = 0.0;
	hypersoft->lastSampleR = 0.0;

	hypersoft->fpdL = 1.0;
	while (hypersoft->fpdL < 16386) hypersoft->fpdL = rand() * UINT32_MAX;
	hypersoft->fpdR = 1.0;
	while (hypersoft->fpdR < 16386) hypersoft->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Hypersoft* hypersoft = (Hypersoft*) instance;

	const float* in1 = hypersoft->input[0];
	const float* in2 = hypersoft->input[1];
	float* out1 = hypersoft->output[0];
	float* out2 = hypersoft->output[1];

	double inputGain = *hypersoft->inputGain * 2.0;
	if (inputGain > 1.0) inputGain *= inputGain;
	else inputGain = 1.0 - pow(1.0 - inputGain, 2);
	// this is the fader curve from ConsoleX with 0.5 being unity gain
	int stages = (int) (*hypersoft->depth * 12.0) + 2;
	// each stage brings in an additional layer of harmonics on the waveshaping
	double bright = (1.0 - *hypersoft->bright) * 0.15;
	// higher slews suppress these higher harmonics when they are sure to just alias
	double outputGain = *hypersoft->outputGain * 2.0;
	if (outputGain > 1.0) outputGain *= outputGain;
	else outputGain = 1.0 - pow(1.0 - outputGain, 2);
	outputGain *= 0.68;
	// this is the fader curve from ConsoleX, rescaled to work with Hypersoft

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = hypersoft->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = hypersoft->fpdR * 1.18e-17;

		inputSampleL *= inputGain;
		inputSampleR *= inputGain;

		inputSampleL = sin(inputSampleL);
		inputSampleL += (sin(inputSampleL * 2.0) / 2.0);
		inputSampleR = sin(inputSampleR);
		inputSampleR += (sin(inputSampleR * 2.0) / 2.0);
		for (int count = 2; count < stages; count++) {
			inputSampleL += ((sin(inputSampleL * (double) count) / (double) pow(count, 3)) * fmax(0.0, 1.0 - fabs((inputSampleL - hypersoft->lastSampleL) * bright * (double) (count * count))));
			inputSampleR += ((sin(inputSampleR * (double) count) / (double) pow(count, 3)) * fmax(0.0, 1.0 - fabs((inputSampleR - hypersoft->lastSampleR) * bright * (double) (count * count))));
		}
		hypersoft->lastSampleL = inputSampleL;
		hypersoft->lastSampleR = inputSampleR;

		inputSampleL *= outputGain;
		inputSampleR *= outputGain;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		hypersoft->fpdL ^= hypersoft->fpdL << 13;
		hypersoft->fpdL ^= hypersoft->fpdL >> 17;
		hypersoft->fpdL ^= hypersoft->fpdL << 5;
		inputSampleL += (((double) hypersoft->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		hypersoft->fpdR ^= hypersoft->fpdR << 13;
		hypersoft->fpdR ^= hypersoft->fpdR >> 17;
		hypersoft->fpdR ^= hypersoft->fpdR << 5;
		inputSampleR += (((double) hypersoft->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	HYPERSOFT_URI,
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
