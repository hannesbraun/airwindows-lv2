#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SHORTBUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/shortbuss"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	SHORTBUSS = 4,
	DRY_WET = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* shortBuss;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double sbSampleL;
	double sbSampleR;
} ShortBuss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ShortBuss* shortBuss = (ShortBuss*) calloc(1, sizeof(ShortBuss));
	shortBuss->sampleRate = rate;
	return (LV2_Handle) shortBuss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ShortBuss* shortBuss = (ShortBuss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			shortBuss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			shortBuss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			shortBuss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			shortBuss->output[1] = (float*) data;
			break;
		case SHORTBUSS:
			shortBuss->shortBuss = (const float*) data;
			break;
		case DRY_WET:
			shortBuss->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ShortBuss* shortBuss = (ShortBuss*) instance;

	shortBuss->sbSampleL = 0.0;
	shortBuss->sbSampleR = 0.0;

	shortBuss->fpdL = 1.0;
	while (shortBuss->fpdL < 16386) shortBuss->fpdL = rand() * UINT32_MAX;
	shortBuss->fpdR = 1.0;
	while (shortBuss->fpdR < 16386) shortBuss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ShortBuss* shortBuss = (ShortBuss*) instance;

	const float* in1 = shortBuss->input[0];
	const float* in2 = shortBuss->input[1];
	float* out1 = shortBuss->output[0];
	float* out2 = shortBuss->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= shortBuss->sampleRate;

	double sbScale = (pow(*shortBuss->shortBuss, 3) * 0.001) / sqrt(overallscale);
	double wet = *shortBuss->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = shortBuss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = shortBuss->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL += (2.0 * inputSampleL * inputSampleL) * shortBuss->sbSampleL;
		shortBuss->sbSampleL += ((inputSampleL - sin(inputSampleL)) * sbScale);
		shortBuss->sbSampleL = sin(shortBuss->sbSampleL * 0.015625) * 64.0;

		inputSampleR += (2.0 * inputSampleR * inputSampleR) * shortBuss->sbSampleR;
		shortBuss->sbSampleR += ((inputSampleR - sin(inputSampleR)) * sbScale);
		shortBuss->sbSampleR = sin(shortBuss->sbSampleR * 0.015625) * 64.0;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		shortBuss->fpdL ^= shortBuss->fpdL << 13;
		shortBuss->fpdL ^= shortBuss->fpdL >> 17;
		shortBuss->fpdL ^= shortBuss->fpdL << 5;
		inputSampleL += (((double) shortBuss->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		shortBuss->fpdR ^= shortBuss->fpdR << 13;
		shortBuss->fpdR ^= shortBuss->fpdR >> 17;
		shortBuss->fpdR ^= shortBuss->fpdR << 5;
		inputSampleR += (((double) shortBuss->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SHORTBUSS_URI,
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
