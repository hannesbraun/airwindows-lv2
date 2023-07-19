#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CLIPSOFTLY_URI "https://hannesbraun.net/ns/lv2/airwindows/clipsoftly"

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

	double lastSampleL;
	double intermediateL[16];
	double lastSampleR;
	double intermediateR[16];
	uint32_t fpdL;
	uint32_t fpdR;
} ClipSoftly;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ClipSoftly* clipSoftly = (ClipSoftly*) calloc(1, sizeof(ClipSoftly));
	clipSoftly->sampleRate = rate;
	return (LV2_Handle) clipSoftly;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ClipSoftly* clipSoftly = (ClipSoftly*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			clipSoftly->input[0] = (const float*) data;
			break;
		case INPUT_R:
			clipSoftly->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			clipSoftly->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			clipSoftly->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ClipSoftly* clipSoftly = (ClipSoftly*) instance;

	clipSoftly->lastSampleL = 0.0;
	clipSoftly->lastSampleR = 0.0;
	for (int x = 0; x < 16; x++) {
		clipSoftly->intermediateL[x] = 0.0;
		clipSoftly->intermediateR[x] = 0.0;
	}
	clipSoftly->fpdL = 1.0;
	while (clipSoftly->fpdL < 16386) clipSoftly->fpdL = rand() * UINT32_MAX;
	clipSoftly->fpdR = 1.0;
	while (clipSoftly->fpdR < 16386) clipSoftly->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ClipSoftly* clipSoftly = (ClipSoftly*) instance;

	const float* in1 = clipSoftly->input[0];
	const float* in2 = clipSoftly->input[1];
	float* out1 = clipSoftly->output[0];
	float* out2 = clipSoftly->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= clipSoftly->sampleRate;
	int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = clipSoftly->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = clipSoftly->fpdR * 1.18e-17;

		double softSpeed = fabs(inputSampleL);
		if (softSpeed < 1.0) softSpeed = 1.0;
		else softSpeed = 1.0 / softSpeed;
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL) * 0.9549925859; // scale to what cliponly uses
		inputSampleL = (inputSampleL * softSpeed) + (clipSoftly->lastSampleL * (1.0 - softSpeed));

		softSpeed = fabs(inputSampleR);
		if (softSpeed < 1.0) softSpeed = 1.0;
		else softSpeed = 1.0 / softSpeed;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR) * 0.9549925859; // scale to what cliponly uses
		inputSampleR = (inputSampleR * softSpeed) + (clipSoftly->lastSampleR * (1.0 - softSpeed));

		clipSoftly->intermediateL[spacing] = inputSampleL;
		inputSampleL = clipSoftly->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) clipSoftly->intermediateL[x - 1] = clipSoftly->intermediateL[x];
		clipSoftly->lastSampleL = clipSoftly->intermediateL[0]; // run a little buffer to handle this

		clipSoftly->intermediateR[spacing] = inputSampleR;
		inputSampleR = clipSoftly->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) clipSoftly->intermediateR[x - 1] = clipSoftly->intermediateR[x];
		clipSoftly->lastSampleR = clipSoftly->intermediateR[0]; // run a little buffer to handle this

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		clipSoftly->fpdL ^= clipSoftly->fpdL << 13;
		clipSoftly->fpdL ^= clipSoftly->fpdL >> 17;
		clipSoftly->fpdL ^= clipSoftly->fpdL << 5;
		inputSampleL += (((double) clipSoftly->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		clipSoftly->fpdR ^= clipSoftly->fpdR << 13;
		clipSoftly->fpdR ^= clipSoftly->fpdR >> 17;
		clipSoftly->fpdR ^= clipSoftly->fpdR << 5;
		inputSampleR += (((double) clipSoftly->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CLIPSOFTLY_URI,
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
