#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TAPEHACK_URI "https://hannesbraun.net/ns/lv2/airwindows/tapehack"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4,
	OUTPUT = 5,
	DRY_WET = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputGain;
	const float* outputGain;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
} TapeHack;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	TapeHack* tapeHack = (TapeHack*) calloc(1, sizeof(TapeHack));
	return (LV2_Handle) tapeHack;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	TapeHack* tapeHack = (TapeHack*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			tapeHack->input[0] = (const float*) data;
			break;
		case INPUT_R:
			tapeHack->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			tapeHack->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			tapeHack->output[1] = (float*) data;
			break;
		case INPUT:
			tapeHack->inputGain = (const float*) data;
			break;
		case OUTPUT:
			tapeHack->outputGain = (const float*) data;
			break;
		case DRY_WET:
			tapeHack->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	TapeHack* tapeHack = (TapeHack*) instance;

	tapeHack->fpdL = 1.0;
	while (tapeHack->fpdL < 16386) tapeHack->fpdL = rand() * UINT32_MAX;
	tapeHack->fpdR = 1.0;
	while (tapeHack->fpdR < 16386) tapeHack->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	TapeHack* tapeHack = (TapeHack*) instance;

	const float* in1 = tapeHack->input[0];
	const float* in2 = tapeHack->input[1];
	float* out1 = tapeHack->output[0];
	float* out2 = tapeHack->output[1];

	double inputGain = *tapeHack->inputGain * 10.0;
	double outputGain = *tapeHack->outputGain * 0.9239;
	double wet = *tapeHack->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = tapeHack->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = tapeHack->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL = fmax(fmin(inputSampleL * inputGain, 2.305929007734908), -2.305929007734908);
		double addtwo = inputSampleL * inputSampleL;
		double empower = inputSampleL * addtwo; // inputSample to the third power
		inputSampleL -= (empower / 6.0);
		empower *= addtwo; // to the fifth power
		inputSampleL += (empower / 69.0);
		empower *= addtwo; // seventh
		inputSampleL -= (empower / 2530.08);
		empower *= addtwo; // ninth
		inputSampleL += (empower / 224985.6);
		empower *= addtwo; // eleventh
		inputSampleL -= (empower / 9979200.0f);
		// this is a degenerate form of a Taylor Series to approximate sin()

		inputSampleR = fmax(fmin(inputSampleR * inputGain, 2.305929007734908), -2.305929007734908);
		addtwo = inputSampleR * inputSampleR;
		empower = inputSampleR * addtwo; // inputSample to the third power
		inputSampleR -= (empower / 6.0);
		empower *= addtwo; // to the fifth power
		inputSampleR += (empower / 69.0);
		empower *= addtwo; // seventh
		inputSampleR -= (empower / 2530.08);
		empower *= addtwo; // ninth
		inputSampleR += (empower / 224985.6);
		empower *= addtwo; // eleventh
		inputSampleR -= (empower / 9979200.0f);
		// this is a degenerate form of a Taylor Series to approximate sin()

		inputSampleL = (inputSampleL * outputGain * wet) + (drySampleL * (1.0 - wet));
		inputSampleR = (inputSampleR * outputGain * wet) + (drySampleR * (1.0 - wet));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		tapeHack->fpdL ^= tapeHack->fpdL << 13;
		tapeHack->fpdL ^= tapeHack->fpdL >> 17;
		tapeHack->fpdL ^= tapeHack->fpdL << 5;
		inputSampleL += (((double) tapeHack->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		tapeHack->fpdR ^= tapeHack->fpdR << 13;
		tapeHack->fpdR ^= tapeHack->fpdR >> 17;
		tapeHack->fpdR ^= tapeHack->fpdR << 5;
		inputSampleR += (((double) tapeHack->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TAPEHACK_URI,
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
