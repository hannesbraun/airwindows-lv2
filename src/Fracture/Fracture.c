#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define FRACTURE_URI "https://hannesbraun.net/ns/lv2/airwindows/fracture"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	FRACTURE = 5,
	OUTPUT_LVL = 6,
	DRY_WET = 7
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* fracture;
	const float* outputLvl;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
} Fracture;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Fracture* fracture = (Fracture*) calloc(1, sizeof(Fracture));
	return (LV2_Handle) fracture;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Fracture* fracture = (Fracture*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			fracture->input[0] = (const float*) data;
			break;
		case INPUT_R:
			fracture->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			fracture->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			fracture->output[1] = (float*) data;
			break;
		case DRIVE:
			fracture->drive = (const float*) data;
			break;
		case FRACTURE:
			fracture->fracture = (const float*) data;
			break;
		case OUTPUT_LVL:
			fracture->outputLvl = (const float*) data;
			break;
		case DRY_WET:
			fracture->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Fracture* fracture = (Fracture*) instance;
	fracture->fpdL = 1.0;
	while (fracture->fpdL < 16386) fracture->fpdL = rand() * UINT32_MAX;
	fracture->fpdR = 1.0;
	while (fracture->fpdR < 16386) fracture->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Fracture* fractureInstance = (Fracture*) instance;

	const float* in1 = fractureInstance->input[0];
	const float* in2 = fractureInstance->input[1];
	float* out1 = fractureInstance->output[0];
	float* out2 = fractureInstance->output[1];

	double density = *fractureInstance->drive;
	double fracture = (*fractureInstance->fracture * 3.14159265358979);
	double output = *fractureInstance->outputLvl;
	double wet = *fractureInstance->dryWet;
	double dry = 1.0 - wet;
	double bridgerectifier;
	density = density * fabs(density);

	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = fractureInstance->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = fractureInstance->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		inputSampleL *= density;
		inputSampleR *= density;

		bridgerectifier = fabs(inputSampleL) * fracture;
		if (bridgerectifier > fracture) bridgerectifier = fracture;
		// max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		// blend according to density control

		bridgerectifier = fabs(inputSampleR) * fracture;
		if (bridgerectifier > fracture) bridgerectifier = fracture;
		// max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		// blend according to density control

		inputSampleL *= output;
		inputSampleR *= output;

		inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
		inputSampleR = (drySampleR * dry) + (inputSampleR * wet);

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		fractureInstance->fpdL ^= fractureInstance->fpdL << 13;
		fractureInstance->fpdL ^= fractureInstance->fpdL >> 17;
		fractureInstance->fpdL ^= fractureInstance->fpdL << 5;
		inputSampleL += (((double) fractureInstance->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		fractureInstance->fpdR ^= fractureInstance->fpdR << 13;
		fractureInstance->fpdR ^= fractureInstance->fpdR >> 17;
		fractureInstance->fpdR ^= fractureInstance->fpdR << 5;
		inputSampleR += (((double) fractureInstance->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	FRACTURE_URI,
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
