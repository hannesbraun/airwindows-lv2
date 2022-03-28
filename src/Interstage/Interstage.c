#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define INTERSTAGE_URI "https://hannesbraun.net/ns/lv2/airwindows/interstage"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];

	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double lastSampleL;
	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double lastSampleR;
	uint32_t fpdL;
	uint32_t fpdR;
	bool flip;
} Interstage;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Interstage* interstage = (Interstage*) calloc(1, sizeof(Interstage));
	interstage->sampleRate = rate;
	return (LV2_Handle) interstage;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Interstage* interstage = (Interstage*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			interstage->input[0] = (const float*) data;
			break;
		case INPUT_R:
			interstage->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			interstage->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			interstage->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Interstage* interstage = (Interstage*) instance;
	interstage->iirSampleAL = 0.0;
	interstage->iirSampleBL = 0.0;
	interstage->iirSampleCL = 0.0;
	interstage->iirSampleDL = 0.0;
	interstage->iirSampleEL = 0.0;
	interstage->iirSampleFL = 0.0;
	interstage->lastSampleL = 0.0;
	interstage->iirSampleAR = 0.0;
	interstage->iirSampleBR = 0.0;
	interstage->iirSampleCR = 0.0;
	interstage->iirSampleDR = 0.0;
	interstage->iirSampleER = 0.0;
	interstage->iirSampleFR = 0.0;
	interstage->lastSampleR = 0.0;
	interstage->fpdL = 1.0;
	while (interstage->fpdL < 16386) interstage->fpdL = rand() * UINT32_MAX;
	interstage->fpdR = 1.0;
	while (interstage->fpdR < 16386) interstage->fpdR = rand() * UINT32_MAX;
	interstage->flip = true;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Interstage* interstage = (Interstage*) instance;

	const float* in1 = interstage->input[0];
	const float* in2 = interstage->input[1];
	float* out1 = interstage->output[0];
	float* out2 = interstage->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= interstage->sampleRate;

	const double firstStage = 0.381966011250105 / overallscale;
	const double iirAmount = 0.00295 / overallscale;
	const double threshold = 0.381966011250105;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = interstage->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = interstage->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL = (inputSampleL + interstage->lastSampleL) * 0.5;
		inputSampleR = (inputSampleR + interstage->lastSampleR) * 0.5; //start the lowpassing with an average

		if (interstage->flip) {
			interstage->iirSampleAL = (interstage->iirSampleAL * (1 - firstStage)) + (inputSampleL * firstStage);
			inputSampleL = interstage->iirSampleAL;
			interstage->iirSampleCL = (interstage->iirSampleCL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = interstage->iirSampleCL;
			interstage->iirSampleEL = (interstage->iirSampleEL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = interstage->iirSampleEL;
			inputSampleL = drySampleL - inputSampleL;
			//make highpass
			if (inputSampleL - interstage->iirSampleAL > threshold) inputSampleL = interstage->iirSampleAL + threshold;
			if (inputSampleL - interstage->iirSampleAL < -threshold) inputSampleL = interstage->iirSampleAL - threshold;
			//slew limit against lowpassed reference point

			interstage->iirSampleAR = (interstage->iirSampleAR * (1 - firstStage)) + (inputSampleR * firstStage);
			inputSampleR = interstage->iirSampleAR;
			interstage->iirSampleCR = (interstage->iirSampleCR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = interstage->iirSampleCR;
			interstage->iirSampleER = (interstage->iirSampleER * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = interstage->iirSampleER;
			inputSampleR = drySampleR - inputSampleR;
			//make highpass
			if (inputSampleR - interstage->iirSampleAR > threshold) inputSampleR = interstage->iirSampleAR + threshold;
			if (inputSampleR - interstage->iirSampleAR < -threshold) inputSampleR = interstage->iirSampleAR - threshold;
			//slew limit against lowpassed reference point
		} else {
			interstage->iirSampleBL = (interstage->iirSampleBL * (1 - firstStage)) + (inputSampleL * firstStage);
			inputSampleL = interstage->iirSampleBL;
			interstage->iirSampleDL = (interstage->iirSampleDL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = interstage->iirSampleDL;
			interstage->iirSampleFL = (interstage->iirSampleFL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = interstage->iirSampleFL;
			inputSampleL = drySampleL - inputSampleL;
			//make highpass
			if (inputSampleL - interstage->iirSampleBL > threshold) inputSampleL = interstage->iirSampleBL + threshold;
			if (inputSampleL - interstage->iirSampleBL < -threshold) inputSampleL = interstage->iirSampleBL - threshold;
			//slew limit against lowpassed reference point

			interstage->iirSampleBR = (interstage->iirSampleBR * (1 - firstStage)) + (inputSampleR * firstStage);
			inputSampleR = interstage->iirSampleBR;
			interstage->iirSampleDR = (interstage->iirSampleDR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = interstage->iirSampleDR;
			interstage->iirSampleFR = (interstage->iirSampleFR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = interstage->iirSampleFR;
			inputSampleR = drySampleR - inputSampleR;
			//make highpass
			if (inputSampleR - interstage->iirSampleBR > threshold) inputSampleR = interstage->iirSampleBR + threshold;
			if (inputSampleR - interstage->iirSampleBR < -threshold) inputSampleR = interstage->iirSampleBR - threshold;
			//slew limit against lowpassed reference point
		}
		interstage->flip = !interstage->flip;
		interstage->lastSampleL = inputSampleL;
		interstage->lastSampleR = inputSampleR;

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		interstage->fpdL ^= interstage->fpdL << 13;
		interstage->fpdL ^= interstage->fpdL >> 17;
		interstage->fpdL ^= interstage->fpdL << 5;
		inputSampleL += (((double)interstage->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		interstage->fpdR ^= interstage->fpdR << 13;
		interstage->fpdR ^= interstage->fpdR >> 17;
		interstage->fpdR ^= interstage->fpdR << 5;
		inputSampleR += (((double)interstage->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	INTERSTAGE_URI,
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
