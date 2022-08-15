#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PURESTGAIN_URI "https://hannesbraun.net/ns/lv2/airwindows/purestgain"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	SLOW_FADE = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* gain;
	const float* slowfade;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double gainchase;
	double settingchase;
	double gainBchase;
	double chasespeed;
} PurestGain;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestGain* purestGain = (PurestGain*) calloc(1, sizeof(PurestGain));
	purestGain->sampleRate = rate;
	return (LV2_Handle) purestGain;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestGain* purestGain = (PurestGain*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestGain->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestGain->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestGain->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestGain->output[1] = (float*) data;
			break;
		case GAIN:
			purestGain->gain = (const float*) data;
			break;
		case SLOW_FADE:
			purestGain->slowfade = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestGain* purestGain = (PurestGain*) instance;
	purestGain->gainchase = -90.0;
	purestGain->settingchase = -90.0;
	purestGain->gainBchase = -90.0;
	purestGain->chasespeed = 350.0;
	purestGain->fpdL = 1.0;
	while (purestGain->fpdL < 16386) purestGain->fpdL = rand() * UINT32_MAX;
	purestGain->fpdR = 1.0;
	while (purestGain->fpdR < 16386) purestGain->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestGain* purestGain = (PurestGain*) instance;

	const float* in1 = purestGain->input[0];
	const float* in2 = purestGain->input[1];
	float* out1 = purestGain->output[0];
	float* out2 = purestGain->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= purestGain->sampleRate;

	double inputgain = *purestGain->gain;
	if (purestGain->settingchase != inputgain) {
		purestGain->chasespeed *= 2.0;
		purestGain->settingchase = inputgain;
		// increment the slowness for each fader movement
		// continuous alteration makes it react smoother
		// sudden jump to setting, not so much
	}
	if (purestGain->chasespeed > 2500.0) purestGain->chasespeed = 2500.0;
	// bail out if it's too extreme
	if (purestGain->gainchase < -60.0) {
		purestGain->gainchase = pow(10.0, inputgain / 20.0);
		// shouldn't even be a negative number
		// this is about starting at whatever's set, when
		// plugin is instantiated.
		// Otherwise it's the target, in dB.
	}
	double targetgain;
	// done with top controller
	double targetBgain = *purestGain->slowfade;
	if (purestGain->gainBchase < 0.0) purestGain->gainBchase = targetBgain;
	// this one is not a dB value, but straight multiplication
	// done with slow fade controller
	double outputgain;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		targetgain = pow(10.0, purestGain->settingchase / 20.0);
		// now we have the target in our temp variable

		purestGain->chasespeed *= 0.9999;
		purestGain->chasespeed -= 0.01;
		if (purestGain->chasespeed < 350.0) purestGain->chasespeed = 350.0;
		// we have our chase speed compensated for recent fader activity

		purestGain->gainchase = (((purestGain->gainchase * purestGain->chasespeed) + targetgain) / (purestGain->chasespeed + 1.0));
		// gainchase is chasing the target, as a simple multiply gain factor

		purestGain->gainBchase = (((purestGain->gainBchase * 4000) + targetBgain) / 4001);
		// gainchase is chasing the target, as a simple multiply gain factor

		outputgain = purestGain->gainchase * purestGain->gainBchase;
		// directly multiply the dB gain by the straight multiply gain

		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestGain->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestGain->fpdR * 1.18e-17;

		if (1.0 == outputgain) {
			*out1 = *in1;
			*out2 = *in2;
		} else {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
			// begin 32 bit stereo floating point dither
			int expon;
			frexpf((float) inputSampleL, &expon);
			purestGain->fpdL ^= purestGain->fpdL << 13;
			purestGain->fpdL ^= purestGain->fpdL >> 17;
			purestGain->fpdL ^= purestGain->fpdL << 5;
			inputSampleL += (((double) purestGain->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
			frexpf((float) inputSampleR, &expon);
			purestGain->fpdR ^= purestGain->fpdR << 13;
			purestGain->fpdR ^= purestGain->fpdR >> 17;
			purestGain->fpdR ^= purestGain->fpdR << 5;
			inputSampleR += (((double) purestGain->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
			// end 32 bit stereo floating point dither
			*out1 = (float) inputSampleL;
			*out2 = (float) inputSampleR;
		}

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
	PURESTGAIN_URI,
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
