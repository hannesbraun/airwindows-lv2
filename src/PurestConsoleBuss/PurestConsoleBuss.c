#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PURESTCONSOLEBUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/purestconsolebuss"

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

	long double fpNShapeL;
	long double fpNShapeR;
} PurestConsoleBuss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) calloc(1, sizeof(PurestConsoleBuss));
	purestConsoleBuss->sampleRate = rate;
	return (LV2_Handle) purestConsoleBuss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestConsoleBuss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestConsoleBuss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestConsoleBuss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestConsoleBuss->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) instance;
	purestConsoleBuss->fpNShapeL = 0.0;
	purestConsoleBuss->fpNShapeR = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestConsoleBuss* purestConsoleBuss = (PurestConsoleBuss*) instance;

	const float* in1 = purestConsoleBuss->input[0];
	const float* in2 = purestConsoleBuss->input[1];
	float* out1 = purestConsoleBuss->output[0];
	float* out2 = purestConsoleBuss->output[1];

	long double inputSampleL;
	long double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
			static int noisesource = 0;
			//this declares a variable before anything else is compiled. It won't keep assigning
			//it to 0 for every sample, it's as if the declaration doesn't exist in this context,
			//but it lets me add this denormalization fix in a single place rather than updating
			//it in three different locations. The variable isn't thread-safe but this is only
			//a random seed and we can share it with whatever.
			noisesource = noisesource % 1700021;
			noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003;
			residue *= residue;
			residue = residue % 17011;
			residue *= residue;
			residue = residue % 1709;
			residue *= residue;
			residue = residue % 173;
			residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleL = applyresidue;
		}
		if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
			static int noisesource = 0;
			noisesource = noisesource % 1700021;
			noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003;
			residue *= residue;
			residue = residue % 17011;
			residue *= residue;
			residue = residue % 1709;
			residue *= residue;
			residue = residue % 173;
			residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleR = applyresidue;
			//this denormalization routine produces a white noise at -300 dB which the noise
			//shaping will interact with to produce a bipolar output, but the noise is actually
			//all positive. That should stop any variables from going denormal, and the routine
			//only kicks in if digital black is input. As a final touch, if you save to 24-bit
			//the silence will return to being digital black again.
		}

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		//amplitude aspect

		//stereo 32 bit dither, made small and tidy.
		int expon;
		frexpf((float)inputSampleL, &expon);
		long double dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
		inputSampleL += (dither - purestConsoleBuss->fpNShapeL);
		purestConsoleBuss->fpNShapeL = dither;
		frexpf((float)inputSampleR, &expon);
		dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
		inputSampleR += (dither - purestConsoleBuss->fpNShapeR);
		purestConsoleBuss->fpNShapeR = dither;
		//end 32 bit dither

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
	PURESTCONSOLEBUSS_URI,
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