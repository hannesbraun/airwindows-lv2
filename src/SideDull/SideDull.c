#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define SIDEDULL_URI "https://hannesbraun.net/ns/lv2/airwindows/sidedull"

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
} SideDull;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	SideDull* sideDull = (SideDull*) calloc(1, sizeof(SideDull));
	sideDull->sampleRate = rate;
	return (LV2_Handle) sideDull;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	SideDull* sideDull = (SideDull*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			sideDull->input[0] = (const float*) data;
			break;
		case INPUT_R:
			sideDull->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			sideDull->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			sideDull->output[1] = (float*) data;
			break;
		case CUTOFF:
			sideDull->cutoff = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	SideDull* sideDull = (SideDull*) instance;
	sideDull->iirSampleA = 0.0;
	sideDull->iirSampleB = 0.0;
	sideDull->flip = true;
	sideDull->fpdL = 1.0;
	while (sideDull->fpdL < 16386) sideDull->fpdL = rand() * UINT32_MAX;
	sideDull->fpdR = 1.0;
	while (sideDull->fpdR < 16386) sideDull->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	SideDull* sideDull = (SideDull*) instance;

	const float* in1 = sideDull->input[0];
	const float* in2 = sideDull->input[1];
	float* out1 = sideDull->output[0];
	float* out2 = sideDull->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sideDull->sampleRate;

	double inputSampleL;
	double inputSampleR;
	double mid;
	double side;
	double iirAmount = pow(*sideDull->cutoff, 3) / overallscale;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = sideDull->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = sideDull->fpdR * 1.18e-17;

		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;

		if (sideDull->flip) {
			sideDull->iirSampleA = (sideDull->iirSampleA * (1 - iirAmount)) + (side * iirAmount);
			side = sideDull->iirSampleA;
		} else {
			sideDull->iirSampleB = (sideDull->iirSampleB * (1 - iirAmount)) + (side * iirAmount);
			side = sideDull->iirSampleB;
		}
		// highpass section

		inputSampleL = (mid + side) / 2.0;
		inputSampleR = (mid - side) / 2.0;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		sideDull->fpdL ^= sideDull->fpdL << 13;
		sideDull->fpdL ^= sideDull->fpdL >> 17;
		sideDull->fpdL ^= sideDull->fpdL << 5;
		inputSampleL += (((double) sideDull->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		sideDull->fpdR ^= sideDull->fpdR << 13;
		sideDull->fpdR ^= sideDull->fpdR >> 17;
		sideDull->fpdR ^= sideDull->fpdR << 5;
		inputSampleR += (((double) sideDull->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SIDEDULL_URI,
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
