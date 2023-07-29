#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define CONSOLE8LITECHANNEL_URI "https://hannesbraun.net/ns/lv2/airwindows/console8litechannel"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FADER = 4
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
	const float* fader;

	double iirAL;
	double iirBL;
	double iirAR;
	double iirBR;
	double inTrimA;
	double inTrimB;
	bool hsr;
	double fix[fix_total];
	double fixB[fix_total];
	double softL[11];
	double softR[11];
	int cycleEnd;

	uint32_t fpdL;
	uint32_t fpdR;
} Console8LiteChannel;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8LiteChannel* console8LiteChannel = (Console8LiteChannel*) calloc(1, sizeof(Console8LiteChannel));
	console8LiteChannel->sampleRate = rate;
	return (LV2_Handle) console8LiteChannel;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8LiteChannel* console8LiteChannel = (Console8LiteChannel*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8LiteChannel->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8LiteChannel->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8LiteChannel->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8LiteChannel->output[1] = (float*) data;
			break;
		case FADER:
			console8LiteChannel->fader = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8LiteChannel* console8LiteChannel = (Console8LiteChannel*) instance;

	console8LiteChannel->inTrimA = 0.5;
	console8LiteChannel->inTrimB = 0.5;
	console8LiteChannel->iirAL = 0.0;
	console8LiteChannel->iirBL = 0.0;
	console8LiteChannel->iirAR = 0.0;
	console8LiteChannel->iirBR = 0.0;
	for (int x = 0; x < fix_total; x++) {
		console8LiteChannel->fix[x] = 0.0;
		console8LiteChannel->fixB[x] = 0.0;
	}
	for (int x = 0; x < 10; x++) {
		console8LiteChannel->softL[x] = 0.0;
		console8LiteChannel->softR[x] = 0.0;
	}

	console8LiteChannel->fpdL = 1.0;
	while (console8LiteChannel->fpdL < 16386) console8LiteChannel->fpdL = rand() * UINT32_MAX;
	console8LiteChannel->fpdR = 1.0;
	while (console8LiteChannel->fpdR < 16386) console8LiteChannel->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8LiteChannel* console8LiteChannel = (Console8LiteChannel*) instance;

	const float* in1 = console8LiteChannel->input[0];
	const float* in2 = console8LiteChannel->input[1];
	float* out1 = console8LiteChannel->output[0];
	float* out2 = console8LiteChannel->output[1];

	const double sampleRate = console8LiteChannel->sampleRate;

	uint32_t inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
	double iirAmountA = 12.66 / sampleRate;
	// this is our distributed unusual highpass, which is
	// adding subtle harmonics to the really deep stuff to define it
	if (fabs(console8LiteChannel->iirAL) < 1.18e-37) console8LiteChannel->iirAL = 0.0;
	if (fabs(console8LiteChannel->iirBL) < 1.18e-37) console8LiteChannel->iirBL = 0.0;
	if (fabs(console8LiteChannel->iirAR) < 1.18e-37) console8LiteChannel->iirAR = 0.0;
	if (fabs(console8LiteChannel->iirBR) < 1.18e-37) console8LiteChannel->iirBR = 0.0;
	// catch denormals early and only check once per buffer
	if (sampleRate > 49000.0) console8LiteChannel->hsr = true;
	else console8LiteChannel->hsr = false;
	console8LiteChannel->fix[fix_freq] = 24000.0 / sampleRate;
	console8LiteChannel->fix[fix_reso] = 2.24697960;
	double K = tan(M_PI * console8LiteChannel->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8LiteChannel->fix[fix_reso] + K * K);
	console8LiteChannel->fix[fix_a0] = K * K * norm;
	console8LiteChannel->fix[fix_a1] = 2.0 * console8LiteChannel->fix[fix_a0];
	console8LiteChannel->fix[fix_a2] = console8LiteChannel->fix[fix_a0];
	console8LiteChannel->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8LiteChannel->fix[fix_b2] = (1.0 - K / console8LiteChannel->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter

	console8LiteChannel->inTrimA = console8LiteChannel->inTrimB;
	console8LiteChannel->inTrimB = *console8LiteChannel->fader * 2.0;
	// 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
	// into softclipping overdrive.
	console8LiteChannel->fixB[fix_freq] = 24000.0 / sampleRate;
	console8LiteChannel->fixB[fix_reso] = 0.80193774;
	K = tan(M_PI * console8LiteChannel->fixB[fix_freq]); // lowpass
	norm = 1.0 / (1.0 + K / console8LiteChannel->fixB[fix_reso] + K * K);
	console8LiteChannel->fixB[fix_a0] = K * K * norm;
	console8LiteChannel->fixB[fix_a1] = 2.0 * console8LiteChannel->fixB[fix_a0];
	console8LiteChannel->fixB[fix_a2] = console8LiteChannel->fixB[fix_a0];
	console8LiteChannel->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8LiteChannel->fixB[fix_b2] = (1.0 - K / console8LiteChannel->fixB[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	console8LiteChannel->cycleEnd = floor(overallscale);
	if (console8LiteChannel->cycleEnd < 1) console8LiteChannel->cycleEnd = 1;
	if (console8LiteChannel->cycleEnd == 3) console8LiteChannel->cycleEnd = 4;
	if (console8LiteChannel->cycleEnd > 4) console8LiteChannel->cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8LiteChannel->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8LiteChannel->fpdR * 1.18e-17;

		double position = (double) sampleFrames / inFramesToProcess;
		double inTrim = (console8LiteChannel->inTrimA * position) + (console8LiteChannel->inTrimB * (1.0 - position));
		// input trim smoothed to cut out zipper noise

		console8LiteChannel->iirAL = (console8LiteChannel->iirAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		double iirAmountBL = fabs(console8LiteChannel->iirAL) + 0.00001;
		console8LiteChannel->iirBL = (console8LiteChannel->iirBL * (1.0 - iirAmountBL)) + (console8LiteChannel->iirAL * iirAmountBL);
		inputSampleL -= console8LiteChannel->iirBL;
		console8LiteChannel->iirAR = (console8LiteChannel->iirAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		double iirAmountBR = fabs(console8LiteChannel->iirAR) + 0.00001;
		console8LiteChannel->iirBR = (console8LiteChannel->iirBR * (1.0 - iirAmountBR)) + (console8LiteChannel->iirAR * iirAmountBR);
		inputSampleR -= console8LiteChannel->iirBR;
		// Console8 highpass
		if (console8LiteChannel->cycleEnd == 4) {
			console8LiteChannel->softL[8] = console8LiteChannel->softL[7];
			console8LiteChannel->softL[7] = console8LiteChannel->softL[6];
			console8LiteChannel->softL[6] = console8LiteChannel->softL[5];
			console8LiteChannel->softL[5] = console8LiteChannel->softL[4];
			console8LiteChannel->softL[4] = console8LiteChannel->softL[3];
			console8LiteChannel->softL[3] = console8LiteChannel->softL[2];
			console8LiteChannel->softL[2] = console8LiteChannel->softL[1];
			console8LiteChannel->softL[1] = console8LiteChannel->softL[0];
			console8LiteChannel->softL[0] = inputSampleL;
			console8LiteChannel->softR[8] = console8LiteChannel->softR[7];
			console8LiteChannel->softR[7] = console8LiteChannel->softR[6];
			console8LiteChannel->softR[6] = console8LiteChannel->softR[5];
			console8LiteChannel->softR[5] = console8LiteChannel->softR[4];
			console8LiteChannel->softR[4] = console8LiteChannel->softR[3];
			console8LiteChannel->softR[3] = console8LiteChannel->softR[2];
			console8LiteChannel->softR[2] = console8LiteChannel->softR[1];
			console8LiteChannel->softR[1] = console8LiteChannel->softR[0];
			console8LiteChannel->softR[0] = inputSampleR;
		}
		if (console8LiteChannel->cycleEnd == 2) {
			console8LiteChannel->softL[8] = console8LiteChannel->softL[6];
			console8LiteChannel->softL[6] = console8LiteChannel->softL[4];
			console8LiteChannel->softL[4] = console8LiteChannel->softL[2];
			console8LiteChannel->softL[2] = console8LiteChannel->softL[0];
			console8LiteChannel->softL[0] = inputSampleL;
			console8LiteChannel->softR[8] = console8LiteChannel->softR[6];
			console8LiteChannel->softR[6] = console8LiteChannel->softR[4];
			console8LiteChannel->softR[4] = console8LiteChannel->softR[2];
			console8LiteChannel->softR[2] = console8LiteChannel->softR[0];
			console8LiteChannel->softR[0] = inputSampleR;
		}
		if (console8LiteChannel->cycleEnd == 1) {
			console8LiteChannel->softL[8] = console8LiteChannel->softL[4];
			console8LiteChannel->softL[4] = console8LiteChannel->softL[0];
			console8LiteChannel->softL[0] = inputSampleL;
			console8LiteChannel->softR[8] = console8LiteChannel->softR[4];
			console8LiteChannel->softR[4] = console8LiteChannel->softR[0];
			console8LiteChannel->softR[0] = inputSampleR;
		}
		console8LiteChannel->softL[9] = ((console8LiteChannel->softL[0] - console8LiteChannel->softL[4]) - (console8LiteChannel->softL[4] - console8LiteChannel->softL[8]));
		if (console8LiteChannel->softL[9] > 0.91416342) inputSampleL = console8LiteChannel->softL[4] + (fabs(console8LiteChannel->softL[4]) * sin(console8LiteChannel->softL[9] - 0.91416342) * 0.08583658);
		if (-console8LiteChannel->softL[9] > 0.91416342) inputSampleL = console8LiteChannel->softL[4] - (fabs(console8LiteChannel->softL[4]) * sin(-console8LiteChannel->softL[9] - 0.91416342) * 0.08583658);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		console8LiteChannel->softR[9] = ((console8LiteChannel->softR[0] - console8LiteChannel->softR[4]) - (console8LiteChannel->softR[4] - console8LiteChannel->softR[8]));
		if (console8LiteChannel->softR[9] > 0.91416342) inputSampleR = console8LiteChannel->softR[4] + (fabs(console8LiteChannel->softR[4]) * sin(console8LiteChannel->softR[9] - 0.91416342) * 0.08583658);
		if (-console8LiteChannel->softR[9] > 0.91416342) inputSampleR = console8LiteChannel->softR[4] - (fabs(console8LiteChannel->softR[4]) * sin(-console8LiteChannel->softR[9] - 0.91416342) * 0.08583658);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		if (console8LiteChannel->hsr) {
			double outSample = (inputSampleL * console8LiteChannel->fix[fix_a0]) + console8LiteChannel->fix[fix_sL1];
			console8LiteChannel->fix[fix_sL1] = (inputSampleL * console8LiteChannel->fix[fix_a1]) - (outSample * console8LiteChannel->fix[fix_b1]) + console8LiteChannel->fix[fix_sL2];
			console8LiteChannel->fix[fix_sL2] = (inputSampleL * console8LiteChannel->fix[fix_a2]) - (outSample * console8LiteChannel->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8LiteChannel->fix[fix_a0]) + console8LiteChannel->fix[fix_sR1];
			console8LiteChannel->fix[fix_sR1] = (inputSampleR * console8LiteChannel->fix[fix_a1]) - (outSample * console8LiteChannel->fix[fix_b1]) + console8LiteChannel->fix[fix_sR2];
			console8LiteChannel->fix[fix_sR2] = (inputSampleR * console8LiteChannel->fix[fix_a2]) - (outSample * console8LiteChannel->fix[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		// we can go directly into the first distortion stage of ChannelOut
		// with a filtered signal, so its biquad is between stages
		// on the input channel we have direct signal, not Console8 decode

		inputSampleL *= inTrim;
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// Console8 gain stage clips at exactly 1.0 post-sin()
		inputSampleR *= inTrim;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// Console8 gain stage clips at exactly 1.0 post-sin()
		if (console8LiteChannel->hsr) {
			double outSample = (inputSampleL * console8LiteChannel->fixB[fix_a0]) + console8LiteChannel->fixB[fix_sL1];
			console8LiteChannel->fixB[fix_sL1] = (inputSampleL * console8LiteChannel->fixB[fix_a1]) - (outSample * console8LiteChannel->fixB[fix_b1]) + console8LiteChannel->fixB[fix_sL2];
			console8LiteChannel->fixB[fix_sL2] = (inputSampleL * console8LiteChannel->fixB[fix_a2]) - (outSample * console8LiteChannel->fixB[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8LiteChannel->fixB[fix_a0]) + console8LiteChannel->fixB[fix_sR1];
			console8LiteChannel->fixB[fix_sR1] = (inputSampleR * console8LiteChannel->fixB[fix_a1]) - (outSample * console8LiteChannel->fixB[fix_b1]) + console8LiteChannel->fixB[fix_sR2];
			console8LiteChannel->fixB[fix_sR2] = (inputSampleR * console8LiteChannel->fixB[fix_a2]) - (outSample * console8LiteChannel->fixB[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		inputSampleL *= inTrim;
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		// Console8 gain stage clips at exactly 1.0 post-sin()
		inputSampleR *= inTrim;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		// Console8 gain stage clips at exactly 1.0 post-sin()

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console8LiteChannel->fpdL ^= console8LiteChannel->fpdL << 13;
		console8LiteChannel->fpdL ^= console8LiteChannel->fpdL >> 17;
		console8LiteChannel->fpdL ^= console8LiteChannel->fpdL << 5;
		inputSampleL += (((double) console8LiteChannel->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8LiteChannel->fpdR ^= console8LiteChannel->fpdR << 13;
		console8LiteChannel->fpdR ^= console8LiteChannel->fpdR >> 17;
		console8LiteChannel->fpdR ^= console8LiteChannel->fpdR << 5;
		inputSampleR += (((double) console8LiteChannel->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8LITECHANNEL_URI,
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
