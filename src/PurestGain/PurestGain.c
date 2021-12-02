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

	long double fpNShapeL;
	long double fpNShapeR;
	//default stuff
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
	purestGain->fpNShapeL = 0.0;
	purestGain->fpNShapeR = 0.0;
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
		//increment the slowness for each fader movement
		//continuous alteration makes it react smoother
		//sudden jump to setting, not so much
	}
	if (purestGain->chasespeed > 2500.0) purestGain->chasespeed = 2500.0;
	//bail out if it's too extreme
	if (purestGain->gainchase < -60.0) {
		purestGain->gainchase = pow(10.0, inputgain / 20.0);
		//shouldn't even be a negative number
		//this is about starting at whatever's set, when
		//plugin is instantiated.
		//Otherwise it's the target, in dB.
	}
	double targetgain;
	//done with top controller
	double targetBgain = *purestGain->slowfade;
	if (purestGain->gainBchase < 0.0) purestGain->gainBchase = targetBgain;
	//this one is not a dB value, but straight multiplication
	//done with slow fade controller
	double outputgain;


	long double inputSampleL;
	long double inputSampleR;

	while (sampleFrames-- > 0) {
		targetgain = pow(10.0, purestGain->settingchase / 20.0);
		//now we have the target in our temp variable

		purestGain->chasespeed *= 0.9999;
		purestGain->chasespeed -= 0.01;
		if (purestGain->chasespeed < 350.0) purestGain->chasespeed = 350.0;
		//we have our chase speed compensated for recent fader activity

		purestGain->gainchase = (((purestGain->gainchase * purestGain->chasespeed) + targetgain) / (purestGain->chasespeed + 1.0));
		//gainchase is chasing the target, as a simple multiply gain factor

		purestGain->gainBchase = (((purestGain->gainBchase * 4000) + targetBgain) / 4001);
		//gainchase is chasing the target, as a simple multiply gain factor

		outputgain = purestGain->gainchase * purestGain->gainBchase;
		//directly multiply the dB gain by the straight multiply gain

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

		if (1.0 == outputgain) {
			*out1 = *in1;
			*out2 = *in2;
		} else {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
			//stereo 32 bit dither, made small and tidy.
			int expon;
			frexpf((float)inputSampleL, &expon);
			long double dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
			inputSampleL += (dither - purestGain->fpNShapeL);
			purestGain->fpNShapeL = dither;
			frexpf((float)inputSampleR, &expon);
			dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
			inputSampleR += (dither - purestGain->fpNShapeR);
			purestGain->fpNShapeR = dither;
			//end 32 bit dither
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
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
