#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define FINALCLIP_URI "https://hannesbraun.net/ns/lv2/airwindows/finalclip"

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
	bool wasNegClipR; // Stereo ClipOnly2
} FinalClip;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	FinalClip* finalClip = (FinalClip*) calloc(1, sizeof(FinalClip));
	finalClip->sampleRate = rate;
	return (LV2_Handle) finalClip;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	FinalClip* finalClip = (FinalClip*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			finalClip->input[0] = (const float*) data;
			break;
		case INPUT_R:
			finalClip->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			finalClip->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			finalClip->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	FinalClip* finalClip = (FinalClip*) instance;
	finalClip->lastSampleL = 0.0;
	finalClip->wasPosClipL = false;
	finalClip->wasNegClipL = false;
	finalClip->lastSampleR = 0.0;
	finalClip->wasPosClipR = false;
	finalClip->wasNegClipR = false;
	for (int x = 0; x < 16; x++) {
		finalClip->intermediateL[x] = 0.0;
		finalClip->intermediateR[x] = 0.0;
	}
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	FinalClip* finalClip = (FinalClip*) instance;

	const float* in1 = finalClip->input[0];
	const float* in2 = finalClip->input[1];
	float* out1 = finalClip->output[0];
	float* out2 = finalClip->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= finalClip->sampleRate;

	int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;
	// double hardness = 0.7390851332151606; // x == cos(x)
	// double softness = 0.260914866784839; // 1.0 - hardness
	// double refclip = 0.9549925859; // -0.2dB we're making all this pure raw code
	// refclip*hardness = 0.705820822569392  to use ClipOnly as a prefab code-chunk.
	// refclip*softness = 0.249171763330607	Seven decimal places is plenty as it's
	// not related to the original sound much: it's an arbitrary position in softening.
	// This is where the numbers come from, referencing the code of the original ClipOnly

	// double hardness = 0.618033988749894; // golden ratio
	// double softness = 0.381966011250105; // 1.0 - hardness
	// double refclip = 1.618033988749894; // -0.2dB we're making all this pure raw code
	// refclip*hardness = 1.0  to use ClipOnly as a prefab code-chunk.
	// refclip*softness = 0.618033988749894	Seven decimal places is plenty as it's
	// not related to the original sound much: it's an arbitrary position in softening.

	while (sampleFrames-- > 0) {
		float inputSampleL = *in1;
		float inputSampleR = *in2;

		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (inputSampleL - finalClip->lastSampleL > 0.618033988749894) inputSampleL = finalClip->lastSampleL + 0.618033988749894;
		if (inputSampleL - finalClip->lastSampleL < -0.618033988749894) inputSampleL = finalClip->lastSampleL - 0.618033988749894;
		// same as slew clippage

		// begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (finalClip->wasPosClipL == true) { // current will be over
			if (inputSampleL < finalClip->lastSampleL) finalClip->lastSampleL = 1.0 + (inputSampleL * 0.381966011250105);
			else finalClip->lastSampleL = 0.618033988749894 + (finalClip->lastSampleL * 0.618033988749894);
		}
		finalClip->wasPosClipL = false;
		if (inputSampleL > 1.618033988749894) {
			finalClip->wasPosClipL = true;
			inputSampleL = 1.0 + (finalClip->lastSampleL * 0.381966011250105);
		}
		if (finalClip->wasNegClipL == true) { // current will be -over
			if (inputSampleL > finalClip->lastSampleL) finalClip->lastSampleL = -1.0 + (inputSampleL * 0.381966011250105);
			else finalClip->lastSampleL = -0.618033988749894 + (finalClip->lastSampleL * 0.618033988749894);
		}
		finalClip->wasNegClipL = false;
		if (inputSampleL < -1.618033988749894) {
			finalClip->wasNegClipL = true;
			inputSampleL = -1.0 + (finalClip->lastSampleL * 0.381966011250105);
		}
		finalClip->intermediateL[spacing] = inputSampleL;
		inputSampleL = finalClip->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) finalClip->intermediateL[x - 1] = finalClip->intermediateL[x];
		finalClip->lastSampleL = finalClip->intermediateL[0]; // run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (inputSampleR - finalClip->lastSampleR > 0.618033988749894) inputSampleR = finalClip->lastSampleR + 0.618033988749894;
		if (inputSampleR - finalClip->lastSampleR < -0.618033988749894) inputSampleR = finalClip->lastSampleR - 0.618033988749894;
		// same as slew clippage

		if (finalClip->wasPosClipR == true) { // current will be over
			if (inputSampleR < finalClip->lastSampleR) finalClip->lastSampleR = 1.0 + (inputSampleR * 0.381966011250105);
			else finalClip->lastSampleR = 0.618033988749894 + (finalClip->lastSampleR * 0.618033988749894);
		}
		finalClip->wasPosClipR = false;
		if (inputSampleR > 1.618033988749894) {
			finalClip->wasPosClipR = true;
			inputSampleR = 1.0 + (finalClip->lastSampleR * 0.381966011250105);
		}
		if (finalClip->wasNegClipR == true) { // current will be -over
			if (inputSampleR > finalClip->lastSampleR) finalClip->lastSampleR = -1.0 + (inputSampleR * 0.381966011250105);
			else finalClip->lastSampleR = -0.618033988749894 + (finalClip->lastSampleR * 0.618033988749894);
		}
		finalClip->wasNegClipR = false;
		if (inputSampleR < -1.618033988749894) {
			finalClip->wasNegClipR = true;
			inputSampleR = -1.0 + (finalClip->lastSampleR * 0.381966011250105);
		}
		finalClip->intermediateR[spacing] = inputSampleR;
		inputSampleR = finalClip->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) finalClip->intermediateR[x - 1] = finalClip->intermediateR[x];
		finalClip->lastSampleR = finalClip->intermediateR[0]; // run a little buffer to handle this
		// end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code

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
	FINALCLIP_URI,
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
