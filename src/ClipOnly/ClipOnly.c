#include <lv2/core/lv2.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CLIPONLY_URI "https://hannesbraun.net/ns/lv2/airwindows/cliponly"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];

	double lastSampleL;
	bool wasPosClipL;
	bool wasNegClipL;

	double lastSampleR;
	bool wasPosClipR;
	bool wasNegClipR;
} ClipOnly;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ClipOnly* clipOnly = (ClipOnly*) calloc(1, sizeof(ClipOnly));
	return (LV2_Handle) clipOnly;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ClipOnly* clipOnly = (ClipOnly*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			clipOnly->input[0] = (const float*) data;
			break;
		case INPUT_R:
			clipOnly->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			clipOnly->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			clipOnly->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ClipOnly* clipOnly = (ClipOnly*) instance;
	clipOnly->lastSampleL = 0.0;
	clipOnly->wasPosClipL = false;
	clipOnly->wasNegClipL = false;

	clipOnly->lastSampleR = 0.0;
	clipOnly->wasPosClipR = false;
	clipOnly->wasNegClipR = false;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ClipOnly* clipOnly = (ClipOnly*) instance;

	const float* in1 = clipOnly->input[0];
	const float* in2 = clipOnly->input[1];
	float* out1 = clipOnly->output[0];
	float* out2 = clipOnly->output[1];

	double hardness = 0.7390851332151606; // x == cos(x)
	double softness = 1.0 - hardness;
	double refclip = 0.9549925859; // -0.2dB

	float inputSampleL;
	float inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;

		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;

		if (clipOnly->wasPosClipL == true) { //current will be over
			if (inputSampleL < clipOnly->lastSampleL) { //next one will NOT be over
				clipOnly->lastSampleL = ((refclip * hardness) + (inputSampleL * softness));
			} else { //still clipping, still chasing the target
				clipOnly->lastSampleL = ((clipOnly->lastSampleL * hardness) + (refclip * softness));
			}
		}
		clipOnly->wasPosClipL = false;
		if (inputSampleL > refclip) { //next will be over the true clip level. otherwise we directly use it
			clipOnly->wasPosClipL = true; //set the clip flag
			inputSampleL = ((refclip * hardness) + (clipOnly->lastSampleL * softness));
		}

		if (clipOnly->wasNegClipL == true) { //current will be -over
			if (inputSampleL > clipOnly->lastSampleL) { //next one will NOT be -over
				clipOnly->lastSampleL = ((-refclip * hardness) + (inputSampleL * softness));
			} else { //still clipping, still chasing the target
				clipOnly->lastSampleL = ((clipOnly->lastSampleL * hardness) + (-refclip * softness));
			}
		}
		clipOnly->wasNegClipL = false;
		if (inputSampleL < -refclip) { //next will be -refclip.  otherwise we directly use it
			clipOnly->wasNegClipL = true; //set the clip flag
			inputSampleL = ((-refclip * hardness) + (clipOnly->lastSampleL * softness));
		}

		if (clipOnly->wasPosClipR == true) { //current will be over
			if (inputSampleR < clipOnly->lastSampleR) { //next one will NOT be over
				clipOnly->lastSampleR = ((refclip * hardness) + (inputSampleR * softness));
			} else { //still clipping, still chasing the target
				clipOnly->lastSampleR = ((clipOnly->lastSampleR * hardness) + (refclip * softness));
			}
		}
		clipOnly->wasPosClipR = false;
		if (inputSampleR > refclip) { //next will be over the true clip level. otherwise we directly use it
			clipOnly->wasPosClipR = true; //set the clip flag
			inputSampleR = ((refclip * hardness) + (clipOnly->lastSampleR * softness));
		}

		if (clipOnly->wasNegClipR == true) { //current will be -over
			if (inputSampleR > clipOnly->lastSampleR) { //next one will NOT be -over
				clipOnly->lastSampleR = ((-refclip * hardness) + (inputSampleR * softness));
			} else { //still clipping, still chasing the target
				clipOnly->lastSampleR = ((clipOnly->lastSampleR * hardness) + (-refclip * softness));
			}
		}
		clipOnly->wasNegClipR = false;
		if (inputSampleR < -refclip) { //next will be -refclip.  otherwise we directly use it
			clipOnly->wasNegClipR = true; //set the clip flag
			inputSampleR = ((-refclip * hardness) + (clipOnly->lastSampleR * softness));
		}

		*out1 = (float) clipOnly->lastSampleL;
		*out2 = (float) clipOnly->lastSampleR;
		clipOnly->lastSampleL = inputSampleL;
		clipOnly->lastSampleR = inputSampleR;

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
	CLIPONLY_URI,
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
