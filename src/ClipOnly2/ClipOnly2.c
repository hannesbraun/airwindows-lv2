#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CLIPONLY2_URI "https://hannesbraun.net/ns/lv2/airwindows/cliponly2"

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
	bool wasPosClipL;
	bool wasNegClipL;
	double lastSampleR;
	double intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly2
} ClipOnly2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ClipOnly2* clipOnly2 = (ClipOnly2*) calloc(1, sizeof(ClipOnly2));
	clipOnly2->sampleRate = rate;
	return (LV2_Handle) clipOnly2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ClipOnly2* clipOnly2 = (ClipOnly2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			clipOnly2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			clipOnly2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			clipOnly2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			clipOnly2->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ClipOnly2* clipOnly2 = (ClipOnly2*) instance;
	clipOnly2->lastSampleL = 0.0;
	clipOnly2->wasPosClipL = false;
	clipOnly2->wasNegClipL = false;
	clipOnly2->lastSampleR = 0.0;
	clipOnly2->wasPosClipR = false;
	clipOnly2->wasNegClipR = false;
	for (int x = 0; x < 16; x++) {
		clipOnly2->intermediateL[x] = 0.0;
		clipOnly2->intermediateR[x] = 0.0;
	}
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ClipOnly2* clipOnly2 = (ClipOnly2*) instance;

	const float* in1 = clipOnly2->input[0];
	const float* in2 = clipOnly2->input[1];
	float* out1 = clipOnly2->output[0];
	float* out2 = clipOnly2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= clipOnly2->sampleRate;

	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;

	while (sampleFrames-- > 0) {
		float inputSampleL = *in1;
		float inputSampleR = *in2;

		//begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (clipOnly2->wasPosClipL == true) { //current will be over
			if (inputSampleL < clipOnly2->lastSampleL) clipOnly2->lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
			else clipOnly2->lastSampleL = 0.2491717 + (clipOnly2->lastSampleL * 0.7390851);
		}
		clipOnly2->wasPosClipL = false;
		if (inputSampleL > 0.9549925859) {
			clipOnly2->wasPosClipL = true;
			inputSampleL = 0.7058208 + (clipOnly2->lastSampleL * 0.2609148);
		}
		if (clipOnly2->wasNegClipL == true) { //current will be -over
			if (inputSampleL > clipOnly2->lastSampleL) clipOnly2->lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
			else clipOnly2->lastSampleL = -0.2491717 + (clipOnly2->lastSampleL * 0.7390851);
		}
		clipOnly2->wasNegClipL = false;
		if (inputSampleL < -0.9549925859) {
			clipOnly2->wasNegClipL = true;
			inputSampleL = -0.7058208 + (clipOnly2->lastSampleL * 0.2609148);
		}
		clipOnly2->intermediateL[spacing] = inputSampleL;
		inputSampleL = clipOnly2->lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) clipOnly2->intermediateL[x - 1] = clipOnly2->intermediateL[x];
		clipOnly2->lastSampleL = clipOnly2->intermediateL[0]; //run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (clipOnly2->wasPosClipR == true) { //current will be over
			if (inputSampleR < clipOnly2->lastSampleR) clipOnly2->lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
			else clipOnly2->lastSampleR = 0.2491717 + (clipOnly2->lastSampleR * 0.7390851);
		}
		clipOnly2->wasPosClipR = false;
		if (inputSampleR > 0.9549925859) {
			clipOnly2->wasPosClipR = true;
			inputSampleR = 0.7058208 + (clipOnly2->lastSampleR * 0.2609148);
		}
		if (clipOnly2->wasNegClipR == true) { //current will be -over
			if (inputSampleR > clipOnly2->lastSampleR) clipOnly2->lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
			else clipOnly2->lastSampleR = -0.2491717 + (clipOnly2->lastSampleR * 0.7390851);
		}
		clipOnly2->wasNegClipR = false;
		if (inputSampleR < -0.9549925859) {
			clipOnly2->wasNegClipR = true;
			inputSampleR = -0.7058208 + (clipOnly2->lastSampleR * 0.2609148);
		}
		clipOnly2->intermediateR[spacing] = inputSampleR;
		inputSampleR = clipOnly2->lastSampleR; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) clipOnly2->intermediateR[x - 1] = clipOnly2->intermediateR[x];
		clipOnly2->lastSampleR = clipOnly2->intermediateR[0]; //run a little buffer to handle this

		*out1 = inputSampleL;
		*out2 = inputSampleR;

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
	CLIPONLY2_URI,
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
