#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define PURESTFADE_URI "https://hannesbraun.net/ns/lv2/airwindows/purestfade"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	FADE = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* gain;
	const float* fade;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double gainchase;
	double settingchase;
	double gainBchase;
	double chasespeed;
} PurestFade;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PurestFade* purestFade = (PurestFade*) calloc(1, sizeof(PurestFade));
	purestFade->sampleRate = rate;
	return (LV2_Handle) purestFade;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PurestFade* purestFade = (PurestFade*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			purestFade->input[0] = (const float*) data;
			break;
		case INPUT_R:
			purestFade->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			purestFade->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			purestFade->output[1] = (float*) data;
			break;
		case GAIN:
			purestFade->gain = (const float*) data;
			break;
		case FADE:
			purestFade->fade = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PurestFade* purestFade = (PurestFade*) instance;
	purestFade->gainchase = -90.0;
	purestFade->settingchase = -90.0;
	purestFade->gainBchase = -90.0;
	purestFade->chasespeed = 350.0;
	purestFade->fpdL = 1.0;
	while (purestFade->fpdL < 16386) purestFade->fpdL = rand() * UINT32_MAX;
	purestFade->fpdR = 1.0;
	while (purestFade->fpdR < 16386) purestFade->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PurestFade* purestFade = (PurestFade*) instance;

	const float* in1 = purestFade->input[0];
	const float* in2 = purestFade->input[1];
	float* out1 = purestFade->output[0];
	float* out2 = purestFade->output[1];

	double inputgain = *purestFade->gain;
	if (purestFade->settingchase != inputgain) {
		purestFade->chasespeed *= 2.0;
		purestFade->settingchase = inputgain;
		// increment the slowness for each fader movement
		// continuous alteration makes it react smoother
		// sudden jump to setting, not so much
	}
	if (purestFade->chasespeed > 2500.0) purestFade->chasespeed = 2500.0;
	// bail out if it's too extreme
	if (purestFade->gainchase < -60.0) {
		purestFade->gainchase = pow(10.0, inputgain / 20.0);
		// shouldn't even be a negative number
		// this is about starting at whatever's set, when
		// plugin is instantiated.
		// Otherwise it's the target, in dB.
	}
	double targetgain;
	// done with top controller
	double targetBgain = *purestFade->fade;
	if (purestFade->gainBchase < 0.0) purestFade->gainBchase = targetBgain;
	// this one is not a dB value, but straight multiplication
	// done with slow fade controller

	double fadeSpeed = targetBgain;
	if (fadeSpeed < 0.0027) fadeSpeed = 0.0027; // minimum fade speed
	fadeSpeed = ((purestFade->sampleRate * 0.004) / fadeSpeed);
	// this will tend to be much slower than PurestGain, and also adapt to sample rates

	double outputgain;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = purestFade->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = purestFade->fpdR * 1.18e-17;

		targetgain = pow(10.0, purestFade->settingchase / 20.0);
		// now we have the target in our temp variable

		purestFade->chasespeed *= 0.9999;
		purestFade->chasespeed -= 0.01;
		if (purestFade->chasespeed < 350.0) purestFade->chasespeed = 350.0;
		// we have our chase speed compensated for recent fader activity

		purestFade->gainchase = (((purestFade->gainchase * purestFade->chasespeed) + targetgain) / (purestFade->chasespeed + 1.0));
		// gainchase is chasing the target, as a simple multiply gain factor

		purestFade->gainBchase = (((purestFade->gainBchase * fadeSpeed) + targetBgain) / (fadeSpeed + 1.0));
		// gainchase is chasing the target, as a simple multiply gain factor

		outputgain = purestFade->gainchase * purestFade->gainBchase;
		// directly multiply the dB gain by the straight multiply gain

		if (1.0 == outputgain) {
			*out1 = *in1;
			*out2 = *in2;
		} else {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;

			// begin 32 bit stereo floating point dither
			int expon;
			frexpf((float) inputSampleL, &expon);
			purestFade->fpdL ^= purestFade->fpdL << 13;
			purestFade->fpdL ^= purestFade->fpdL >> 17;
			purestFade->fpdL ^= purestFade->fpdL << 5;
			inputSampleL += (((double) purestFade->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
			frexpf((float) inputSampleR, &expon);
			purestFade->fpdR ^= purestFade->fpdR << 13;
			purestFade->fpdR ^= purestFade->fpdR >> 17;
			purestFade->fpdR ^= purestFade->fpdR << 5;
			inputSampleR += (((double) purestFade->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PURESTFADE_URI,
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
