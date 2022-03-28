#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define SIDEPASS_URI "https://hannesbraun.net/ns/lv2/airwindows/sidepass"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CUTOFF = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* cutoff;

	double iirSampleA;
	double iirSampleB;
	bool flip;
	uint32_t fpdL;
	uint32_t fpdR;
} Sidepass;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Sidepass* sidepass = (Sidepass*) calloc(1, sizeof(Sidepass));
	sidepass->sampleRate = rate;
	return (LV2_Handle) sidepass;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Sidepass* sidepass = (Sidepass*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			sidepass->input[0] = (const float*) data;
			break;
		case INPUT_R:
			sidepass->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			sidepass->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			sidepass->output[1] = (float*) data;
			break;
		case CUTOFF:
			sidepass->cutoff = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Sidepass* sidepass = (Sidepass*) instance;
	sidepass->iirSampleA = 0.0;
	sidepass->iirSampleB = 0.0;
	sidepass->flip = true;
	sidepass->fpdL = 1.0;
	while (sidepass->fpdL < 16386) sidepass->fpdL = rand() * UINT32_MAX;
	sidepass->fpdR = 1.0;
	while (sidepass->fpdR < 16386) sidepass->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Sidepass* sidepass = (Sidepass*) instance;

	const float* in1 = sidepass->input[0];
	const float* in2 = sidepass->input[1];
	float* out1 = sidepass->output[0];
	float* out2 = sidepass->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sidepass->sampleRate;
	double iirAmount = pow(*sidepass->cutoff, 3) / overallscale;
	double inputSampleL;
	double inputSampleR;
	double mid;
	double side;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = sidepass->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = sidepass->fpdR * 1.18e-17;

		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;

		if (sidepass->flip) {
			sidepass->iirSampleA = (sidepass->iirSampleA * (1 - iirAmount)) + (side * iirAmount);
			side -= sidepass->iirSampleA;
		} else {
			sidepass->iirSampleB = (sidepass->iirSampleB * (1 - iirAmount)) + (side * iirAmount);
			side -= sidepass->iirSampleB;
		}
		//highpass section

		inputSampleL = (mid + side) / 2.0;
		inputSampleR = (mid - side) / 2.0;

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		sidepass->fpdL ^= sidepass->fpdL << 13;
		sidepass->fpdL ^= sidepass->fpdL >> 17;
		sidepass->fpdL ^= sidepass->fpdL << 5;
		inputSampleL += (((double)sidepass->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		sidepass->fpdR ^= sidepass->fpdR << 13;
		sidepass->fpdR ^= sidepass->fpdR >> 17;
		sidepass->fpdR ^= sidepass->fpdR << 5;
		inputSampleR += (((double)sidepass->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SIDEPASS_URI,
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
