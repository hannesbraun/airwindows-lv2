#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define OVERHEADS_URI "https://hannesbraun.net/ns/lv2/airwindows/overheads"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	COMPR = 4,
	SHARP = 5,
	DRY_WET = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* compr;
	const float* sharp;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double ovhGain;
	double ovhL[130];
	double ovhR[130];
	int ovhCount;
} Overheads;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Overheads* overheads = (Overheads*) calloc(1, sizeof(Overheads));
	overheads->sampleRate = rate;
	return (LV2_Handle) overheads;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Overheads* overheads = (Overheads*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			overheads->input[0] = (const float*) data;
			break;
		case INPUT_R:
			overheads->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			overheads->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			overheads->output[1] = (float*) data;
			break;
		case COMPR:
			overheads->compr = (const float*) data;
			break;
		case SHARP:
			overheads->sharp = (const float*) data;
			break;
		case DRY_WET:
			overheads->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Overheads* overheads = (Overheads*) instance;

	overheads->ovhGain = 1.0;
	for (int count = 0; count < 129; count++) {
		overheads->ovhL[count] = 0.0;
		overheads->ovhR[count] = 0.0;
	}
	overheads->ovhCount = 0;

	overheads->fpdL = 1.0;
	while (overheads->fpdL < 16386) overheads->fpdL = rand() * UINT32_MAX;
	overheads->fpdR = 1.0;
	while (overheads->fpdR < 16386) overheads->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Overheads* overheads = (Overheads*) instance;

	const float* in1 = overheads->input[0];
	const float* in2 = overheads->input[1];
	float* out1 = overheads->output[0];
	float* out2 = overheads->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= overheads->sampleRate;

	double ovhTrim = pow(*overheads->compr, 3);
	ovhTrim += 1.0;
	ovhTrim *= ovhTrim;
	int offset = (pow(*overheads->sharp, 7) * 16.0 * overallscale) + 1;
	double wet = *overheads->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = overheads->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = overheads->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		// begin Overheads compressor
		inputSampleL *= ovhTrim;
		inputSampleR *= ovhTrim;
		overheads->ovhCount--;
		if (overheads->ovhCount < 0 || overheads->ovhCount > 128) overheads->ovhCount = 128;
		overheads->ovhL[overheads->ovhCount] = inputSampleL;
		overheads->ovhR[overheads->ovhCount] = inputSampleR;
		double ovhClamp = sin(fabs(inputSampleL - overheads->ovhL[(overheads->ovhCount + offset) - (((overheads->ovhCount + offset) > 128) ? 129 : 0)]) * (ovhTrim - 1.0) * 64.0);
		overheads->ovhGain *= (1.0 - ovhClamp);
		overheads->ovhGain += ((1.0 - ovhClamp) * ovhClamp);
		ovhClamp = sin(fabs(inputSampleR - overheads->ovhR[(overheads->ovhCount + offset) - (((overheads->ovhCount + offset) > 128) ? 129 : 0)]) * (ovhTrim - 1.0) * 64.0);
		overheads->ovhGain *= (1.0 - ovhClamp);
		overheads->ovhGain += ((1.0 - ovhClamp) * ovhClamp);
		if (overheads->ovhGain > 1.0) overheads->ovhGain = 1.0;
		if (overheads->ovhGain < 0.0) overheads->ovhGain = 0.0;
		inputSampleL *= overheads->ovhGain;
		inputSampleR *= overheads->ovhGain;
		// end Overheads compressor

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		overheads->fpdL ^= overheads->fpdL << 13;
		overheads->fpdL ^= overheads->fpdL >> 17;
		overheads->fpdL ^= overheads->fpdL << 5;
		inputSampleL += (((double) overheads->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		overheads->fpdR ^= overheads->fpdR << 13;
		overheads->fpdR ^= overheads->fpdR >> 17;
		overheads->fpdR ^= overheads->fpdR << 5;
		inputSampleR += (((double) overheads->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	OVERHEADS_URI,
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
