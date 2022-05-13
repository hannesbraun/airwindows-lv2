#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE6CHANNEL_URI "https://hannesbraun.net/ns/lv2/airwindows/console6channel"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* inputGain;

	uint32_t fpdL;
	uint32_t fpdR;
} Console6Channel;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console6Channel* console6Channel = (Console6Channel*) calloc(1, sizeof(Console6Channel));
	return (LV2_Handle) console6Channel;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console6Channel* console6Channel = (Console6Channel*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console6Channel->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console6Channel->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console6Channel->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console6Channel->output[1] = (float*) data;
			break;
		case INPUT:
			console6Channel->inputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console6Channel* console6Channel = (Console6Channel*) instance;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console6Channel* console6Channel = (Console6Channel*) instance;

	const float* in1 = console6Channel->input[0];
	const float* in2 = console6Channel->input[1];
	float* out1 = console6Channel->output[0];
	float* out2 = console6Channel->output[1];

	const float gain = *console6Channel->inputGain;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = console6Channel->fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = console6Channel->fpdR * 1.18e-17;


		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		//encode/decode courtesy of torridgristle under the MIT license
		//Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5

		if (inputSampleL > 1.0) inputSampleL= 1.0;
		else if (inputSampleL > 0.0) inputSampleL = 1.0 - pow(1.0-inputSampleL,2.0);

		if (inputSampleL < -1.0) inputSampleL = -1.0;
		else if (inputSampleL < 0.0) inputSampleL = -1.0 + pow(1.0+inputSampleL,2.0);

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		else if (inputSampleR > 0.0) inputSampleR = 1.0 - pow(1.0-inputSampleR,2.0);

		if (inputSampleR < -1.0) inputSampleR = -1.0;
		else if (inputSampleR < 0.0) inputSampleR = -1.0 + pow(1.0+inputSampleR,2.0);


		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		console6Channel->fpdL ^= console6Channel->fpdL << 13; console6Channel->fpdL ^= console6Channel->fpdL >> 17; console6Channel->fpdL ^= console6Channel->fpdL << 5;
		inputSampleL += (((double)console6Channel->fpdL-(uint32_t)0x7fffffff) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		console6Channel->fpdR ^= console6Channel->fpdR << 13; console6Channel->fpdR ^= console6Channel->fpdR >> 17; console6Channel->fpdR ^= console6Channel->fpdR << 5;
		inputSampleR += (((double)console6Channel->fpdR-(uint32_t)0x7fffffff) * 5.5e-36l * pow(2,expon+62));
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
	CONSOLE6CHANNEL_URI,
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
