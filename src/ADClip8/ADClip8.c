#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define ADCLIP8_URI "https://hannesbraun.net/ns/lv2/airwindows/adclip8"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BOOST = 4,
	CEILING = 5,
	MODE = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* boost;
	const float* ceiling;
	const float* mode;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double lastSampleL[8];
	double intermediateL[16][8];
	bool wasPosClipL[8];
	bool wasNegClipL[8];
	double lastSampleR[8];
	double intermediateR[16][8];
	bool wasPosClipR[8];
	bool wasNegClipR[8];
} ADClip8;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ADClip8* adclip8 = (ADClip8*) calloc(1, sizeof(ADClip8));
	adclip8->sampleRate = rate;
	return (LV2_Handle) adclip8;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ADClip8* adclip8 = (ADClip8*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			adclip8->input[0] = (const float*) data;
			break;
		case INPUT_R:
			adclip8->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			adclip8->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			adclip8->output[1] = (float*) data;
			break;
		case BOOST:
			adclip8->boost = (const float*) data;
			break;
		case CEILING:
			adclip8->ceiling = (const float*) data;
			break;
		case MODE:
			adclip8->mode = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ADClip8* adclip8 = (ADClip8*) instance;

	for (int stage = 0; stage < 8; stage++) {
		adclip8->lastSampleL[stage] = 0.0;
		adclip8->lastSampleR[stage] = 0.0;
		adclip8->wasPosClipL[stage] = false;
		adclip8->wasPosClipR[stage] = false;
		adclip8->wasNegClipL[stage] = false;
		adclip8->wasNegClipR[stage] = false;
		for (int x = 0; x < 16; x++) {
			adclip8->intermediateL[x][stage] = 0.0;
			adclip8->intermediateR[x][stage] = 0.0;
		}
	}

	adclip8->fpdL = 1.0;
	while (adclip8->fpdL < 16386) adclip8->fpdL = rand() * UINT32_MAX;
	adclip8->fpdR = 1.0;
	while (adclip8->fpdR < 16386) adclip8->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ADClip8* adclip8 = (ADClip8*) instance;

	const float* in1 = adclip8->input[0];
	const float* in2 = adclip8->input[1];
	float* out1 = adclip8->output[0];
	float* out2 = adclip8->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= adclip8->sampleRate;

	int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;
	// double hardness = 0.618033988749894; // golden ratio
	// double softness = 0.381966011250105; // 1.0 - hardness
	// double refclip = 1.618033988749894; // -0.2dB we're making all this pure raw code
	// refclip*hardness = 1.0  to use ClipOnly as a prefab code-chunk.
	// refclip*softness = 0.618033988749894	Seven decimal places is plenty as it's
	// not related to the original sound much: it's an arbitrary position in softening.
	double inputGain = pow(10.0, *adclip8->boost / 20.0);

	double ceiling = (1.0 + (*adclip8->ceiling * 0.23594733)) * 0.5;
	int mode = (int) *adclip8->mode + 1;
	if (mode > 8) {
		mode = 8;
	} else if (mode < 0) {
		mode = 0;
	}
	int stageSetting = mode - 2;
	if (stageSetting < 1) stageSetting = 1;
	inputGain = ((inputGain - 1.0) / stageSetting) + 1.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = adclip8->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = adclip8->fpdR * 1.18e-17;
		double overshootL = 0.0;
		double overshootR = 0.0;
		inputSampleL *= 1.618033988749894;
		inputSampleR *= 1.618033988749894;

		for (int stage = 0; stage < stageSetting; stage++) {
			if (inputGain != 1.0) {
				inputSampleL *= inputGain;
				inputSampleR *= inputGain;
			}
			if (stage == 0) {
				overshootL = fabs(inputSampleL) - 1.618033988749894;
				if (overshootL < 0.0) overshootL = 0.0;
				overshootR = fabs(inputSampleR) - 1.618033988749894;
				if (overshootR < 0.0) overshootR = 0.0;
			}
			if (inputSampleL > 4.0) inputSampleL = 4.0;
			if (inputSampleL < -4.0) inputSampleL = -4.0;
			if (inputSampleL - adclip8->lastSampleL[stage] > 0.618033988749894) inputSampleL = adclip8->lastSampleL[stage] + 0.618033988749894;
			if (inputSampleL - adclip8->lastSampleL[stage] < -0.618033988749894) inputSampleL = adclip8->lastSampleL[stage] - 0.618033988749894;
			if (inputSampleR > 4.0) inputSampleR = 4.0;
			if (inputSampleR < -4.0) inputSampleR = -4.0;
			if (inputSampleR - adclip8->lastSampleR[stage] > 0.618033988749894) inputSampleR = adclip8->lastSampleR[stage] + 0.618033988749894;
			if (inputSampleR - adclip8->lastSampleR[stage] < -0.618033988749894) inputSampleR = adclip8->lastSampleR[stage] - 0.618033988749894;
			// same as slew clippage

			// begin ClipOnly2 as a little, compressed chunk that can be dropped into code
			if (adclip8->wasPosClipL[stage] == true) { // current will be over
				if (inputSampleL < adclip8->lastSampleL[stage]) adclip8->lastSampleL[stage] = 1.0 + (inputSampleL * 0.381966011250105);
				else adclip8->lastSampleL[stage] = 0.618033988749894 + (adclip8->lastSampleL[stage] * 0.618033988749894);
			}
			adclip8->wasPosClipL[stage] = false;
			if (inputSampleL > 1.618033988749894) {
				adclip8->wasPosClipL[stage] = true;
				inputSampleL = 1.0 + (adclip8->lastSampleL[stage] * 0.381966011250105);
			}
			if (adclip8->wasNegClipL[stage] == true) { // current will be -over
				if (inputSampleL > adclip8->lastSampleL[stage]) adclip8->lastSampleL[stage] = -1.0 + (inputSampleL * 0.381966011250105);
				else adclip8->lastSampleL[stage] = -0.618033988749894 + (adclip8->lastSampleL[stage] * 0.618033988749894);
			}
			adclip8->wasNegClipL[stage] = false;
			if (inputSampleL < -1.618033988749894) {
				adclip8->wasNegClipL[stage] = true;
				inputSampleL = -1.0 + (adclip8->lastSampleL[stage] * 0.381966011250105);
			}
			adclip8->intermediateL[spacing][stage] = inputSampleL;
			inputSampleL = adclip8->lastSampleL[stage]; // Latency is however many samples equals one 44.1k sample
			for (int x = spacing; x > 0; x--) adclip8->intermediateL[x - 1][stage] = adclip8->intermediateL[x][stage];
			adclip8->lastSampleL[stage] = adclip8->intermediateL[0][stage]; // run a little buffer to handle this
			// end ClipOnly2 as a little, compressed chunk that can be dropped into code

			// begin ClipOnly2 as a little, compressed chunk that can be dropped into code
			if (adclip8->wasPosClipR[stage] == true) { // current will be over
				if (inputSampleR < adclip8->lastSampleR[stage]) adclip8->lastSampleR[stage] = 1.0 + (inputSampleR * 0.381966011250105);
				else adclip8->lastSampleR[stage] = 0.618033988749894 + (adclip8->lastSampleR[stage] * 0.618033988749894);
			}
			adclip8->wasPosClipR[stage] = false;
			if (inputSampleR > 1.618033988749894) {
				adclip8->wasPosClipR[stage] = true;
				inputSampleR = 1.0 + (adclip8->lastSampleR[stage] * 0.381966011250105);
			}
			if (adclip8->wasNegClipR[stage] == true) { // current will be -over
				if (inputSampleR > adclip8->lastSampleR[stage]) adclip8->lastSampleR[stage] = -1.0 + (inputSampleR * 0.381966011250105);
				else adclip8->lastSampleR[stage] = -0.618033988749894 + (adclip8->lastSampleR[stage] * 0.618033988749894);
			}
			adclip8->wasNegClipR[stage] = false;
			if (inputSampleR < -1.618033988749894) {
				adclip8->wasNegClipR[stage] = true;
				inputSampleR = -1.0 + (adclip8->lastSampleR[stage] * 0.381966011250105);
			}
			adclip8->intermediateR[spacing][stage] = inputSampleR;
			inputSampleR = adclip8->lastSampleR[stage]; // Latency is however many samples equals one 44.1k sample
			for (int x = spacing; x > 0; x--) adclip8->intermediateR[x - 1][stage] = adclip8->intermediateR[x][stage];
			adclip8->lastSampleR[stage] = adclip8->intermediateR[0][stage]; // run a little buffer to handle this
			// end ClipOnly2 as a little, compressed chunk that can be dropped into code
		}

		switch (mode) {
			case 1:
				break; // Normal
			case 2:
				inputSampleL /= inputGain;
				inputSampleR /= inputGain;
				break; // Gain Match
			case 3:
				inputSampleL = overshootL;
				inputSampleR = overshootR;
				break; // Clip Only
			case 4:
				break; // Afterburner
			case 5:
				break; // Explode
			case 6:
				break; // Nuke
			case 7:
				break; // Apocalypse
			case 8:
				break; // Apotheosis
		}
		// this is our output mode switch, showing the effects
		inputSampleL *= ceiling;
		inputSampleR *= ceiling;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		adclip8->fpdL ^= adclip8->fpdL << 13;
		adclip8->fpdL ^= adclip8->fpdL >> 17;
		adclip8->fpdL ^= adclip8->fpdL << 5;
		inputSampleL += (((double) adclip8->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		adclip8->fpdR ^= adclip8->fpdR << 13;
		adclip8->fpdR ^= adclip8->fpdR >> 17;
		adclip8->fpdR ^= adclip8->fpdR << 5;
		inputSampleR += (((double) adclip8->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ADCLIP8_URI,
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
