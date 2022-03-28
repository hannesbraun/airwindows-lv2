#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SLEW_URI "https://hannesbraun.net/ns/lv2/airwindows/slew"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CLAMPING = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* gain;

	double lastSampleL;
	double lastSampleR;
} Slew;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Slew* slew = (Slew*) calloc(1, sizeof(Slew));
	slew->sampleRate = rate;
	return (LV2_Handle) slew;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Slew* slew = (Slew*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			slew->input[0] = (const float*) data;
			break;
		case INPUT_R:
			slew->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			slew->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			slew->output[1] = (float*) data;
			break;
		case CLAMPING:
			slew->gain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Slew* slew = (Slew*) instance;
	slew->lastSampleL = 0.0;
	slew->lastSampleR = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Slew* slew = (Slew*) instance;

	const float* in1 = slew->input[0];
	const float* in2 = slew->input[1];
	float* out1 = slew->output[0];
	float* out2 = slew->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= slew->sampleRate;

	double inputSampleL;
	double inputSampleR;
	double outputSampleL;
	double outputSampleR;

	double clamp;
	double threshold = pow((1 - *slew->gain), 4) / overallscale;

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

		clamp = inputSampleL - slew->lastSampleL;
		outputSampleL = inputSampleL;
		if (clamp > threshold)
			outputSampleL = slew->lastSampleL + threshold;
		if (-clamp > threshold)
			outputSampleL = slew->lastSampleL - threshold;
		slew->lastSampleL = outputSampleL;

		clamp = inputSampleR - slew->lastSampleR;
		outputSampleR = inputSampleR;
		if (clamp > threshold)
			outputSampleR = slew->lastSampleR + threshold;
		if (-clamp > threshold)
			outputSampleR = slew->lastSampleR - threshold;
		slew->lastSampleR = outputSampleR;

		*out1 = (float) outputSampleL;
		*out2 = (float) outputSampleR;

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
	SLEW_URI,
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