#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLELABUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/consolelabuss"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	MASTER = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* master;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double lastSinewL;
	double lastSinewR;

	double subAL;
	double subAR;
	double subBL;
	double subBR;
	double subCL;
	double subCR;

	double gainA;
	double gainB; // smoothed master fader for channel, from Z2 series filter code
} ConsoleLABuss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ConsoleLABuss* consoleLABuss = (ConsoleLABuss*) calloc(1, sizeof(ConsoleLABuss));
	consoleLABuss->sampleRate = rate;
	return (LV2_Handle) consoleLABuss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ConsoleLABuss* consoleLABuss = (ConsoleLABuss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			consoleLABuss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			consoleLABuss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			consoleLABuss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			consoleLABuss->output[1] = (float*) data;
			break;
		case MASTER:
			consoleLABuss->master = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ConsoleLABuss* consoleLABuss = (ConsoleLABuss*) instance;

	consoleLABuss->lastSinewL = consoleLABuss->lastSinewR = 0.0;
	consoleLABuss->subAL = consoleLABuss->subAR = consoleLABuss->subBL = consoleLABuss->subBR = consoleLABuss->subCL = consoleLABuss->subCR = 0.0;
	consoleLABuss->gainA = consoleLABuss->gainB = 1.0;

	consoleLABuss->fpdL = 1.0;
	while (consoleLABuss->fpdL < 16386) consoleLABuss->fpdL = rand() * UINT32_MAX;
	consoleLABuss->fpdR = 1.0;
	while (consoleLABuss->fpdR < 16386) consoleLABuss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ConsoleLABuss* consoleLABuss = (ConsoleLABuss*) instance;

	const float* in1 = consoleLABuss->input[0];
	const float* in2 = consoleLABuss->input[1];
	float* out1 = consoleLABuss->output[0];
	float* out2 = consoleLABuss->output[1];

	uint32_t inFramesToProcess = sampleFrames;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= consoleLABuss->sampleRate;

	consoleLABuss->gainA = consoleLABuss->gainB;
	consoleLABuss->gainB = sqrt(*consoleLABuss->master); // smoothed master fader from Z2 filters
	// this will be applied three times: this is to make the various tone alterations
	// hit differently at different master fader drive levels.
	// in particular, backing off the master fader tightens the super lows
	// but opens up the modified Sinew, because more of the attentuation happens before
	// you even get to slew clipping :) and if the fader is not active, it bypasses completely.

	double threshSinew = 0.718 / overallscale;
	double subTrim = 0.0011 / overallscale;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = consoleLABuss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = consoleLABuss->fpdR * 1.18e-17;

		double temp = (double) sampleFrames / inFramesToProcess;
		double gain = (consoleLABuss->gainA * temp) + (consoleLABuss->gainB * (1.0 - temp));
		// setting up smoothed master fader

		// begin SubTight section
		double subSampleL = inputSampleL * subTrim;
		double subSampleR = inputSampleR * subTrim;

		double scale = 0.5 + fabs(subSampleL * 0.5);
		subSampleL = (consoleLABuss->subAL + (sin(consoleLABuss->subAL - subSampleL) * scale));
		consoleLABuss->subAL = subSampleL * scale;
		scale = 0.5 + fabs(subSampleR * 0.5);
		subSampleR = (consoleLABuss->subAR + (sin(consoleLABuss->subAR - subSampleR) * scale));
		consoleLABuss->subAR = subSampleR * scale;
		scale = 0.5 + fabs(subSampleL * 0.5);
		subSampleL = (consoleLABuss->subBL + (sin(consoleLABuss->subBL - subSampleL) * scale));
		consoleLABuss->subBL = subSampleL * scale;
		scale = 0.5 + fabs(subSampleR * 0.5);
		subSampleR = (consoleLABuss->subBR + (sin(consoleLABuss->subBR - subSampleR) * scale));
		consoleLABuss->subBR = subSampleR * scale;
		scale = 0.5 + fabs(subSampleL * 0.5);
		subSampleL = (consoleLABuss->subCL + (sin(consoleLABuss->subCL - subSampleL) * scale));
		consoleLABuss->subCL = subSampleL * scale;
		scale = 0.5 + fabs(subSampleR * 0.5);
		subSampleR = (consoleLABuss->subCR + (sin(consoleLABuss->subCR - subSampleR) * scale));
		consoleLABuss->subCR = subSampleR * scale;
		if (subSampleL > 0.25) subSampleL = 0.25;
		if (subSampleL < -0.25) subSampleL = -0.25;
		if (subSampleR > 0.25) subSampleR = 0.25;
		if (subSampleR < -0.25) subSampleR = -0.25;
		inputSampleL -= (subSampleL * 16.0);
		inputSampleR -= (subSampleR * 16.0);
		// end SubTight section

		if (gain < 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} // if using the master fader, we are going to attenuate three places.
		// subtight is always fully engaged: tighten response when restraining full console

		// begin ConsoleZeroBuss which is the one we choose for ConsoleLA
		if (inputSampleL > 2.8) inputSampleL = 2.8;
		if (inputSampleL < -2.8) inputSampleL = -2.8;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * 2.0) / (3.0 - inputSampleL);
		else inputSampleL = -(inputSampleL * -2.0) / (3.0 + inputSampleL);

		if (inputSampleR > 2.8) inputSampleR = 2.8;
		if (inputSampleR < -2.8) inputSampleR = -2.8;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * 2.0) / (3.0 - inputSampleR);
		else inputSampleR = -(inputSampleR * -2.0) / (3.0 + inputSampleR);
		// ConsoleZero Buss

		if (gain < 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} // if using the master fader, we are going to attenuate three places.
		// after C0Buss but before EverySlew: allow highs to come out a bit more
		// when pulling back master fader. Less drive equals more open

		temp = inputSampleL;
		double clamp = inputSampleL - consoleLABuss->lastSinewL;
		double sinew = threshSinew * cos(consoleLABuss->lastSinewL);
		if (clamp > sinew) temp = consoleLABuss->lastSinewL + sinew;
		if (-clamp > sinew) temp = consoleLABuss->lastSinewL - sinew;
		inputSampleL = consoleLABuss->lastSinewL = temp;
		if (consoleLABuss->lastSinewL > 1.0) consoleLABuss->lastSinewL = 1.0;
		if (consoleLABuss->lastSinewL < -1.0) consoleLABuss->lastSinewL = -1.0;
		temp = inputSampleR;
		clamp = inputSampleR - consoleLABuss->lastSinewR;
		sinew = threshSinew * cos(consoleLABuss->lastSinewR);
		if (clamp > sinew) temp = consoleLABuss->lastSinewR + sinew;
		if (-clamp > sinew) temp = consoleLABuss->lastSinewR - sinew;
		inputSampleR = consoleLABuss->lastSinewR = temp;
		if (consoleLABuss->lastSinewR > 1.0) consoleLABuss->lastSinewR = 1.0;
		if (consoleLABuss->lastSinewR < -1.0) consoleLABuss->lastSinewR = -1.0;

		if (gain < 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} // if using the master fader, we are going to attenuate three places.
		// after EverySlew fades the total output sound: least change in tone here.

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		consoleLABuss->fpdL ^= consoleLABuss->fpdL << 13;
		consoleLABuss->fpdL ^= consoleLABuss->fpdL >> 17;
		consoleLABuss->fpdL ^= consoleLABuss->fpdL << 5;
		inputSampleL += (((double) consoleLABuss->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		consoleLABuss->fpdR ^= consoleLABuss->fpdR << 13;
		consoleLABuss->fpdR ^= consoleLABuss->fpdR >> 17;
		consoleLABuss->fpdR ^= consoleLABuss->fpdR << 5;
		inputSampleR += (((double) consoleLABuss->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLELABUSS_URI,
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
