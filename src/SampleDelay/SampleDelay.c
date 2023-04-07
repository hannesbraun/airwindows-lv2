#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SAMPLEDELAY_URI "https://hannesbraun.net/ns/lv2/airwindows/sampledelay"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	MS = 4,
	SAMPLES = 5,
	SUBSAMPLE = 6,
	INV_WET = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* ms;
	const float* samples;
	const float* subsample;
	const float* invWet;

	uint32_t fpdL;
	uint32_t fpdR;

	double pL[16386];
	double pR[16386];
	int gcount;
} SampleDelay;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	SampleDelay* sampleDelay = (SampleDelay*) calloc(1, sizeof(SampleDelay));
	sampleDelay->sampleRate = rate;
	return (LV2_Handle) sampleDelay;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	SampleDelay* sampleDelay = (SampleDelay*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			sampleDelay->input[0] = (const float*) data;
			break;
		case INPUT_R:
			sampleDelay->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			sampleDelay->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			sampleDelay->output[1] = (float*) data;
			break;
		case MS:
			sampleDelay->ms = (const float*) data;
			break;
		case SAMPLES:
			sampleDelay->samples = (const float*) data;
			break;
		case SUBSAMPLE:
			sampleDelay->subsample = (const float*) data;
			break;
		case INV_WET:
			sampleDelay->invWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	SampleDelay* sampleDelay = (SampleDelay*) instance;
	for (int count = 0; count < 16385; count++) {
		sampleDelay->pL[count] = 0.0;
		sampleDelay->pR[count] = 0.0;
	}
	sampleDelay->gcount = 0;

	sampleDelay->fpdL = 1.0;
	while (sampleDelay->fpdL < 16386) sampleDelay->fpdL = rand() * UINT32_MAX;
	sampleDelay->fpdR = 1.0;
	while (sampleDelay->fpdR < 16386) sampleDelay->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	SampleDelay* sampleDelay = (SampleDelay*) instance;

	const float* in1 = sampleDelay->input[0];
	const float* in2 = sampleDelay->input[1];
	float* out1 = sampleDelay->output[0];
	float* out2 = sampleDelay->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleDelay->sampleRate;

	double coarse = (*sampleDelay->ms / 100.0) * (4410.0 * overallscale);
	double fine = *sampleDelay->samples;
	double subsample = *sampleDelay->subsample;
	int offset = floor(coarse + fine);
	if (offset > 16380) offset = 16380; // insanity check
	int maxtime = 16384;
	double phase = (*sampleDelay->invWet * 2.0) - 1.0;
	double dryLevel = 1.0 - fabs(phase);

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = sampleDelay->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = sampleDelay->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (sampleDelay->gcount < 0 || sampleDelay->gcount > maxtime) {
			sampleDelay->gcount = maxtime;
		}
		int count = sampleDelay->gcount;
		sampleDelay->pL[count] = inputSampleL;
		sampleDelay->pR[count] = inputSampleR;
		count += offset;
		inputSampleL = sampleDelay->pL[count - ((count > maxtime) ? maxtime + 1 : 0)] * (1.0 - subsample);
		inputSampleR = sampleDelay->pR[count - ((count > maxtime) ? maxtime + 1 : 0)] * (1.0 - subsample);
		inputSampleL += sampleDelay->pL[count + 1 - ((count + 1 > maxtime) ? maxtime + 1 : 0)] * subsample;
		inputSampleR += sampleDelay->pR[count + 1 - ((count + 1 > maxtime) ? maxtime + 1 : 0)] * subsample;
		sampleDelay->gcount--;

		if (phase < 1.0) {
			inputSampleL *= phase;
			inputSampleR *= phase;
		}

		if (dryLevel > 0.0) {
			inputSampleL += (drySampleL * dryLevel);
			inputSampleR += (drySampleR * dryLevel);
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		sampleDelay->fpdL ^= sampleDelay->fpdL << 13;
		sampleDelay->fpdL ^= sampleDelay->fpdL >> 17;
		sampleDelay->fpdL ^= sampleDelay->fpdL << 5;
		inputSampleL += (((double) sampleDelay->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		sampleDelay->fpdR ^= sampleDelay->fpdR << 13;
		sampleDelay->fpdR ^= sampleDelay->fpdR >> 17;
		sampleDelay->fpdR ^= sampleDelay->fpdR << 5;
		inputSampleR += (((double) sampleDelay->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SAMPLEDELAY_URI,
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
