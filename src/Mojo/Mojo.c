#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define MOJO_URI "https://hannesbraun.net/ns/lv2/airwindows/mojo"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* gain;

	uint32_t fpdL;
	uint32_t fpdR;
} Mojo;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Mojo* mojo = (Mojo*) calloc(1, sizeof(Mojo));
	return (LV2_Handle) mojo;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Mojo* mojo = (Mojo*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			mojo->input[0] = (const float*) data;
			break;
		case INPUT_R:
			mojo->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			mojo->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			mojo->output[1] = (float*) data;
			break;
		case GAIN:
			mojo->gain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Mojo* mojo = (Mojo*) instance;
	mojo->fpdL = 1.0;
	while (mojo->fpdL < 16386) mojo->fpdL = rand() * UINT32_MAX;
	mojo->fpdR = 1.0;
	while (mojo->fpdR < 16386) mojo->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Mojo* mojoInstance = (Mojo*) instance;

	const float* in1 = mojoInstance->input[0];
	const float* in2 = mojoInstance->input[1];
	float* out1 = mojoInstance->output[0];
	float* out2 = mojoInstance->output[1];

	double gain = pow(10.0, *mojoInstance->gain / 20.0);

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = mojoInstance->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = mojoInstance->fpdR * 1.18e-17;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		double mojo = pow(fabs(inputSampleL), 0.25);
		if (mojo > 0.0) inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo) * 0.987654321;
		// mojo is the one that flattens WAAAAY out very softly before wavefolding
		mojo = pow(fabs(inputSampleR), 0.25);
		if (mojo > 0.0) inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo) * 0.987654321;
		// mojo is the one that flattens WAAAAY out very softly before wavefolding

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		mojoInstance->fpdL ^= mojoInstance->fpdL << 13;
		mojoInstance->fpdL ^= mojoInstance->fpdL >> 17;
		mojoInstance->fpdL ^= mojoInstance->fpdL << 5;
		inputSampleL += (((double) mojoInstance->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		mojoInstance->fpdR ^= mojoInstance->fpdR << 13;
		mojoInstance->fpdR ^= mojoInstance->fpdR >> 17;
		mojoInstance->fpdR ^= mojoInstance->fpdR << 5;
		inputSampleR += (((double) mojoInstance->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MOJO_URI,
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
