#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PURESTDRIVE_URI "https://hannesbraun.net/ns/lv2/airwindows/purestdrive"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* drive;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double previousSampleL;
	double previousSampleR;
} PurestDrive;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestDrive* purestDrive = (PurestDrive*) calloc(1, sizeof(PurestDrive));
	return (LV2_Handle) purestDrive;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestDrive* purestDrive = (PurestDrive*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestDrive->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestDrive->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestDrive->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestDrive->output[1] = (float*) data;
			break;
		case DRIVE:
			purestDrive->drive = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestDrive* purestDrive = (PurestDrive*) instance;
	purestDrive->previousSampleL = 0.0;
	purestDrive->previousSampleR = 0.0;

	purestDrive->fpdL = 1.0;
	while (purestDrive->fpdL < 16386) purestDrive->fpdL = rand() * UINT32_MAX;
	purestDrive->fpdR = 1.0;
	while (purestDrive->fpdR < 16386) purestDrive->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestDrive* purestDrive = (PurestDrive*) instance;

	const float* in1 = purestDrive->input[0];
	const float* in2 = purestDrive->input[1];
	float* out1 = purestDrive->output[0];
	float* out2 = purestDrive->output[1];

	double intensity = *purestDrive->drive;
	double drySampleL;
	double drySampleR;
	double inputSampleL;
	double inputSampleR;
	double apply;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestDrive->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestDrive->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		inputSampleL = sin(inputSampleL);
		// basic distortion factor
		apply = (fabs(purestDrive->previousSampleL + inputSampleL) / 2.0) * intensity;
		// saturate less if previous sample was undistorted and low level, or if it was
		// inverse polarity. Lets through highs and brightness more.
		inputSampleL = (drySampleL * (1.0 - apply)) + (inputSampleL * apply);
		// dry-wet control for intensity also has FM modulation to clean up highs
		purestDrive->previousSampleL = sin(drySampleL);
		// apply the sine while storing previous sample

		inputSampleR = sin(inputSampleR);
		// basic distortion factor
		apply = (fabs(purestDrive->previousSampleR + inputSampleR) / 2.0) * intensity;
		// saturate less if previous sample was undistorted and low level, or if it was
		// inverse polarity. Lets through highs and brightness more.
		inputSampleR = (drySampleR * (1.0 - apply)) + (inputSampleR * apply);
		// dry-wet control for intensity also has FM modulation to clean up highs
		purestDrive->previousSampleR = sin(drySampleR);
		// apply the sine while storing previous sample

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		purestDrive->fpdL ^= purestDrive->fpdL << 13;
		purestDrive->fpdL ^= purestDrive->fpdL >> 17;
		purestDrive->fpdL ^= purestDrive->fpdL << 5;
		inputSampleL += (((double) purestDrive->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		purestDrive->fpdR ^= purestDrive->fpdR << 13;
		purestDrive->fpdR ^= purestDrive->fpdR >> 17;
		purestDrive->fpdR ^= purestDrive->fpdR << 5;
		inputSampleR += (((double) purestDrive->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PURESTDRIVE_URI,
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
