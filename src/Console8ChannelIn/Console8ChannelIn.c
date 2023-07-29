#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define CONSOLE8CHANNELIN_URI "https://hannesbraun.net/ns/lv2/airwindows/console8channelin"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

enum {
	fix_freq,
	fix_reso,
	fix_a0,
	fix_a1,
	fix_a2,
	fix_b1,
	fix_b2,
	fix_sL1,
	fix_sL2,
	fix_sR1,
	fix_sR2,
	fix_total
}; // fixed frequency biquad filter for ultrasonics, stereo

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];

	double iirAL;
	double iirBL;
	double iirAR;
	double iirBR;
	bool hsr;
	double fix[fix_total];
	double softL[11];
	double softR[11];
	int cycleEnd;
	uint32_t fpdL;
	uint32_t fpdR;
} Console8ChannelIn;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8ChannelIn* console8ChannelIn = (Console8ChannelIn*) calloc(1, sizeof(Console8ChannelIn));
	console8ChannelIn->sampleRate = rate;
	return (LV2_Handle) console8ChannelIn;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8ChannelIn* console8ChannelIn = (Console8ChannelIn*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8ChannelIn->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8ChannelIn->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8ChannelIn->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8ChannelIn->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8ChannelIn* console8ChannelIn = (Console8ChannelIn*) instance;

	console8ChannelIn->iirAL = 0.0;
	console8ChannelIn->iirBL = 0.0;
	console8ChannelIn->iirAR = 0.0;
	console8ChannelIn->iirBR = 0.0;
	for (int x = 0; x < fix_total; x++) console8ChannelIn->fix[x] = 0.0;
	for (int x = 0; x < 10; x++) {
		console8ChannelIn->softL[x] = 0.0;
		console8ChannelIn->softR[x] = 0.0;
	}
	console8ChannelIn->fpdL = 1.0;
	while (console8ChannelIn->fpdL < 16386) console8ChannelIn->fpdL = rand() * UINT32_MAX;
	console8ChannelIn->fpdR = 1.0;
	while (console8ChannelIn->fpdR < 16386) console8ChannelIn->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8ChannelIn* console8ChannelIn = (Console8ChannelIn*) instance;

	const float* in1 = console8ChannelIn->input[0];
	const float* in2 = console8ChannelIn->input[1];
	float* out1 = console8ChannelIn->output[0];
	float* out2 = console8ChannelIn->output[1];

	const double sampleRate = console8ChannelIn->sampleRate;

	if (sampleRate > 49000.0) console8ChannelIn->hsr = true;
	else console8ChannelIn->hsr = false;
	console8ChannelIn->fix[fix_freq] = 24000.0 / sampleRate;
	console8ChannelIn->fix[fix_reso] = 0.76352112;
	double K = tan(M_PI * console8ChannelIn->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8ChannelIn->fix[fix_reso] + K * K);
	console8ChannelIn->fix[fix_a0] = K * K * norm;
	console8ChannelIn->fix[fix_a1] = 2.0 * console8ChannelIn->fix[fix_a0];
	console8ChannelIn->fix[fix_a2] = console8ChannelIn->fix[fix_a0];
	console8ChannelIn->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8ChannelIn->fix[fix_b2] = (1.0 - K / console8ChannelIn->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	console8ChannelIn->cycleEnd = floor(overallscale);
	if (console8ChannelIn->cycleEnd < 1) console8ChannelIn->cycleEnd = 1;
	if (console8ChannelIn->cycleEnd == 3) console8ChannelIn->cycleEnd = 4;
	if (console8ChannelIn->cycleEnd > 4) console8ChannelIn->cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k
	double iirAmountA = 12.66 / sampleRate;
	// this is our distributed unusual highpass, which is
	// adding subtle harmonics to the really deep stuff to define it
	if (fabs(console8ChannelIn->iirAL) < 1.18e-37) console8ChannelIn->iirAL = 0.0;
	if (fabs(console8ChannelIn->iirBL) < 1.18e-37) console8ChannelIn->iirBL = 0.0;
	if (fabs(console8ChannelIn->iirAR) < 1.18e-37) console8ChannelIn->iirAR = 0.0;
	if (fabs(console8ChannelIn->iirBR) < 1.18e-37) console8ChannelIn->iirBR = 0.0;
	// catch denormals early and only check once per buffer

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8ChannelIn->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8ChannelIn->fpdR * 1.18e-17;

		console8ChannelIn->iirAL = (console8ChannelIn->iirAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		double iirAmountBL = fabs(console8ChannelIn->iirAL) + 0.00001;
		console8ChannelIn->iirBL = (console8ChannelIn->iirBL * (1.0 - iirAmountBL)) + (console8ChannelIn->iirAL * iirAmountBL);
		inputSampleL -= console8ChannelIn->iirBL;
		console8ChannelIn->iirAR = (console8ChannelIn->iirAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		double iirAmountBR = fabs(console8ChannelIn->iirAR) + 0.00001;
		console8ChannelIn->iirBR = (console8ChannelIn->iirBR * (1.0 - iirAmountBR)) + (console8ChannelIn->iirAR * iirAmountBR);
		inputSampleR -= console8ChannelIn->iirBR;
		// Console8 highpass
		if (console8ChannelIn->cycleEnd == 4) {
			console8ChannelIn->softL[8] = console8ChannelIn->softL[7];
			console8ChannelIn->softL[7] = console8ChannelIn->softL[6];
			console8ChannelIn->softL[6] = console8ChannelIn->softL[5];
			console8ChannelIn->softL[5] = console8ChannelIn->softL[4];
			console8ChannelIn->softL[4] = console8ChannelIn->softL[3];
			console8ChannelIn->softL[3] = console8ChannelIn->softL[2];
			console8ChannelIn->softL[2] = console8ChannelIn->softL[1];
			console8ChannelIn->softL[1] = console8ChannelIn->softL[0];
			console8ChannelIn->softL[0] = inputSampleL;
			console8ChannelIn->softR[8] = console8ChannelIn->softR[7];
			console8ChannelIn->softR[7] = console8ChannelIn->softR[6];
			console8ChannelIn->softR[6] = console8ChannelIn->softR[5];
			console8ChannelIn->softR[5] = console8ChannelIn->softR[4];
			console8ChannelIn->softR[4] = console8ChannelIn->softR[3];
			console8ChannelIn->softR[3] = console8ChannelIn->softR[2];
			console8ChannelIn->softR[2] = console8ChannelIn->softR[1];
			console8ChannelIn->softR[1] = console8ChannelIn->softR[0];
			console8ChannelIn->softR[0] = inputSampleR;
		}
		if (console8ChannelIn->cycleEnd == 2) {
			console8ChannelIn->softL[8] = console8ChannelIn->softL[6];
			console8ChannelIn->softL[6] = console8ChannelIn->softL[4];
			console8ChannelIn->softL[4] = console8ChannelIn->softL[2];
			console8ChannelIn->softL[2] = console8ChannelIn->softL[0];
			console8ChannelIn->softL[0] = inputSampleL;
			console8ChannelIn->softR[8] = console8ChannelIn->softR[6];
			console8ChannelIn->softR[6] = console8ChannelIn->softR[4];
			console8ChannelIn->softR[4] = console8ChannelIn->softR[2];
			console8ChannelIn->softR[2] = console8ChannelIn->softR[0];
			console8ChannelIn->softR[0] = inputSampleR;
		}
		if (console8ChannelIn->cycleEnd == 1) {
			console8ChannelIn->softL[8] = console8ChannelIn->softL[4];
			console8ChannelIn->softL[4] = console8ChannelIn->softL[0];
			console8ChannelIn->softL[0] = inputSampleL;
			console8ChannelIn->softR[8] = console8ChannelIn->softR[4];
			console8ChannelIn->softR[4] = console8ChannelIn->softR[0];
			console8ChannelIn->softR[0] = inputSampleR;
		}
		console8ChannelIn->softL[9] = ((console8ChannelIn->softL[0] - console8ChannelIn->softL[4]) - (console8ChannelIn->softL[4] - console8ChannelIn->softL[8]));
		if (console8ChannelIn->softL[9] > 0.91416342) inputSampleL = console8ChannelIn->softL[4] + (fabs(console8ChannelIn->softL[4]) * sin(console8ChannelIn->softL[9] - 0.91416342) * 0.08583658);
		if (-console8ChannelIn->softL[9] > 0.91416342) inputSampleL = console8ChannelIn->softL[4] - (fabs(console8ChannelIn->softL[4]) * sin(-console8ChannelIn->softL[9] - 0.91416342) * 0.08583658);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		console8ChannelIn->softR[9] = ((console8ChannelIn->softR[0] - console8ChannelIn->softR[4]) - (console8ChannelIn->softR[4] - console8ChannelIn->softR[8]));
		if (console8ChannelIn->softR[9] > 0.91416342) inputSampleR = console8ChannelIn->softR[4] + (fabs(console8ChannelIn->softR[4]) * sin(console8ChannelIn->softR[9] - 0.91416342) * 0.08583658);
		if (-console8ChannelIn->softR[9] > 0.91416342) inputSampleR = console8ChannelIn->softR[4] - (fabs(console8ChannelIn->softR[4]) * sin(-console8ChannelIn->softR[9] - 0.91416342) * 0.08583658);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		if (console8ChannelIn->hsr) {
			double outSample = (inputSampleL * console8ChannelIn->fix[fix_a0]) + console8ChannelIn->fix[fix_sL1];
			console8ChannelIn->fix[fix_sL1] = (inputSampleL * console8ChannelIn->fix[fix_a1]) - (outSample * console8ChannelIn->fix[fix_b1]) + console8ChannelIn->fix[fix_sL2];
			console8ChannelIn->fix[fix_sL2] = (inputSampleL * console8ChannelIn->fix[fix_a2]) - (outSample * console8ChannelIn->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8ChannelIn->fix[fix_a0]) + console8ChannelIn->fix[fix_sR1];
			console8ChannelIn->fix[fix_sR1] = (inputSampleR * console8ChannelIn->fix[fix_a1]) - (outSample * console8ChannelIn->fix[fix_b1]) + console8ChannelIn->fix[fix_sR2];
			console8ChannelIn->fix[fix_sR2] = (inputSampleR * console8ChannelIn->fix[fix_a2]) - (outSample * console8ChannelIn->fix[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		// we can go directly into the first distortion stage of ChannelOut
		// with a filtered signal, so its biquad is between stages
		// on the input channel we have direct signal, not Console8 decode

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console8ChannelIn->fpdL ^= console8ChannelIn->fpdL << 13;
		console8ChannelIn->fpdL ^= console8ChannelIn->fpdL >> 17;
		console8ChannelIn->fpdL ^= console8ChannelIn->fpdL << 5;
		inputSampleL += (((double) console8ChannelIn->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8ChannelIn->fpdR ^= console8ChannelIn->fpdR << 13;
		console8ChannelIn->fpdR ^= console8ChannelIn->fpdR >> 17;
		console8ChannelIn->fpdR ^= console8ChannelIn->fpdR << 5;
		inputSampleR += (((double) console8ChannelIn->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8CHANNELIN_URI,
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
