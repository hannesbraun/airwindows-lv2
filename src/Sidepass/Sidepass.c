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
	long double fpNShapeL;
	long double fpNShapeR;
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
	sidepass->fpNShapeL = 0.0;
	sidepass->fpNShapeR = 0.0;
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
	long double inputSampleL;
	long double inputSampleR;
	long double mid;
	double side;

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

		//stereo 32 bit dither, made small and tidy.
		int expon;
		frexpf((float)inputSampleL, &expon);
		long double dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
		inputSampleL += (dither - sidepass->fpNShapeL);
		sidepass->fpNShapeL = dither;
		frexpf((float)inputSampleR, &expon);
		dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
		inputSampleR += (dither - sidepass->fpNShapeR);
		sidepass->fpNShapeR = dither;
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
