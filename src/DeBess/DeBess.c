#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define DEBESS_URI "https://hannesbraun.net/ns/lv2/airwindows/debess"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INTENSE = 4,
	SHARP = 5,
	DEPTH = 6,
	FILTER = 7,
	SENSE = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* intense;
	const float* sharp;
	const float* depth;
	const float* filter;
	const float* sense;

	long double sL[41], mL[41]/*, cL[41]*/;
	double ratioAL;
	double ratioBL;
	double iirSampleAL;
	double iirSampleBL;

	long double sR[41], mR[41]/*, cR[41]*/;
	double ratioAR;
	double ratioBR;
	double iirSampleAR;
	double iirSampleBR;


	bool flip;
	uint32_t fpd;
} DeBess;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	DeBess* debess = (DeBess*) calloc(1, sizeof(DeBess));
	debess->sampleRate = rate;
	return (LV2_Handle) debess;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	DeBess* debess = (DeBess*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			debess->input[0] = (const float*) data;
			break;
		case INPUT_R:
			debess->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			debess->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			debess->output[1] = (float*) data;
			break;
		case INTENSE:
			debess->intense = (const float*) data;
			break;
		case SHARP:
			debess->sharp = (const float*) data;
			break;
		case DEPTH:
			debess->depth = (const float*) data;
			break;
		case FILTER:
			debess->filter = (const float*) data;
			break;
		case SENSE:
			debess->sense = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	DeBess* debess = (DeBess*) instance;

	for (int x = 0; x < 41; x++) {
		debess->sL[x] = 0.0;
		debess->mL[x] = 0.0;
		//debess->cL[x] = 0.0;
		debess->sR[x] = 0.0;
		debess->mR[x] = 0.0;
		//debess->cR[x] = 0.0;
	}
	debess->ratioAL = 1.0;
	debess->ratioBL = 1.0;
	debess->iirSampleAL = 0.0;
	debess->iirSampleBL = 0.0;
	debess->ratioAR = 1.0;
	debess->ratioBR = 1.0;
	debess->iirSampleAR = 0.0;
	debess->iirSampleBR = 0.0;

	debess->flip = false;
	debess->fpd = 17;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	DeBess* debess = (DeBess*) instance;

	const float* in1 = debess->input[0];
	const float* in2 = debess->input[1];
	float* out1 = debess->output[0];
	float* out2 = debess->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= debess->sampleRate;

	double intensity = pow(*debess->intense, 5) * (8192 / overallscale);
	double sharpness = *debess->sharp * 40.0;
	if (sharpness < 2) sharpness = 2;
	double speed = 0.1 / sharpness;
	double depth = 1.0 / (*debess->depth + 0.0001);
	double iirAmount = *debess->filter;
	float monitoring = *debess->sense;

	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabsl(inputSampleL) < 1.18e-37) inputSampleL = debess->fpd * 1.18e-37;
		if (fabsl(inputSampleR) < 1.18e-37) inputSampleR = debess->fpd * 1.18e-37;

		debess->sL[0] = inputSampleL; //set up so both [0] and [1] will be input sample
		debess->sR[0] = inputSampleR; //set up so both [0] and [1] will be input sample
		//we only use the [1] so this is just where samples come in
		for (int x = sharpness; x > 0; x--) {
			debess->sL[x] = debess->sL[x - 1];
			debess->sR[x] = debess->sR[x - 1];
		} //building up a set of slews

		debess->mL[1] = (debess->sL[1] - debess->sL[2]) * ((debess->sL[1] - debess->sL[2]) / 1.3);
		debess->mR[1] = (debess->sR[1] - debess->sR[2]) * ((debess->sR[1] - debess->sR[2]) / 1.3);
		for (int x = sharpness - 1; x > 1; x--) {
			debess->mL[x] = (debess->sL[x] - debess->sL[x + 1]) * ((debess->sL[x - 1] - debess->sL[x]) / 1.3);
			debess->mR[x] = (debess->sR[x] - debess->sR[x + 1]) * ((debess->sR[x - 1] - debess->sR[x]) / 1.3);
		} //building up a set of slews of slews

		double senseL = fabsl(debess->mL[1] - debess->mL[2]) * sharpness * sharpness;
		double senseR = fabsl(debess->mR[1] - debess->mR[2]) * sharpness * sharpness;
		for (int x = sharpness - 1; x > 0; x--) {
			double multL = fabsl(debess->mL[x] - debess->mL[x + 1]) * sharpness * sharpness;
			if (multL < 1.0) senseL *= multL;
			double multR = fabsl(debess->mR[x] - debess->mR[x + 1]) * sharpness * sharpness;
			if (multR < 1.0) senseR *= multR;
		} //sense is slews of slews times each other

		senseL = 1.0 + (intensity * intensity * senseL);
		if (senseL > intensity) {
			senseL = intensity;
		}
		senseR = 1.0 + (intensity * intensity * senseR);
		if (senseR > intensity) {
			senseR = intensity;
		}

		if (debess->flip) {
			debess->iirSampleAL = (debess->iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			debess->iirSampleAR = (debess->iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			debess->ratioAL = (debess->ratioAL * (1.0 - speed)) + (senseL * speed);
			debess->ratioAR = (debess->ratioAR * (1.0 - speed)) + (senseR * speed);
			if (debess->ratioAL > depth) debess->ratioAL = depth;
			if (debess->ratioAR > depth) debess->ratioAR = depth;
			if (debess->ratioAL > 1.0) inputSampleL = debess->iirSampleAL + ((inputSampleL - debess->iirSampleAL) / debess->ratioAL);
			if (debess->ratioAR > 1.0) inputSampleR = debess->iirSampleAR + ((inputSampleR - debess->iirSampleAR) / debess->ratioAR);
		} else {
			debess->iirSampleBL = (debess->iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			debess->iirSampleBR = (debess->iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			debess->ratioBL = (debess->ratioBL * (1.0 - speed)) + (senseL * speed);
			debess->ratioBR = (debess->ratioBR * (1.0 - speed)) + (senseR * speed);
			if (debess->ratioBL > depth) debess->ratioBL = depth;
			if (debess->ratioBR > depth) debess->ratioBR = depth;
			if (debess->ratioAL > 1.0) inputSampleL = debess->iirSampleBL + ((inputSampleL - debess->iirSampleBL) / debess->ratioBL);
			if (debess->ratioAR > 1.0) inputSampleR = debess->iirSampleBR + ((inputSampleR - debess->iirSampleBR) / debess->ratioBR);
		}
		debess->flip = !debess->flip;

		if (monitoring > 0.49999) {
			inputSampleL = *in1 - inputSampleL;
			inputSampleR = *in2 - inputSampleR;
		}
		//sense monitoring

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		debess->fpd ^= debess->fpd << 13;
		debess->fpd ^= debess->fpd >> 17;
		debess->fpd ^= debess->fpd << 5;
		inputSampleL += (((double)debess->fpd - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		debess->fpd ^= debess->fpd << 13;
		debess->fpd ^= debess->fpd >> 17;
		debess->fpd ^= debess->fpd << 5;
		inputSampleR += (((double)debess->fpd - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	DEBESS_URI,
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
