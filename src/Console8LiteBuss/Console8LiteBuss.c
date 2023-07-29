#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define CONSOLE8LITEBUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/console8litebuss"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FADER = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* fader;

	double iirAL;
	double iirBL;
	double iirAR;
	double iirBR;
	bool hsr;
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
	double fix[fix_total];
	double fixB[fix_total];
	double softL[11];
	double softR[11];
	int cycleEnd;

	double inTrimA;
	double inTrimB;
	double lastSampleL;
	double intermediateL[18];
	bool wasPosClipL;
	bool wasNegClipL; // ClipOnly2
	double lastSampleR;
	double intermediateR[18];
	bool wasPosClipR;
	bool wasNegClipR;
	int spacing;

	uint32_t fpdL;
	uint32_t fpdR;
} Console8LiteBuss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8LiteBuss* console8LiteBuss = (Console8LiteBuss*) calloc(1, sizeof(Console8LiteBuss));
	console8LiteBuss->sampleRate = rate;
	return (LV2_Handle) console8LiteBuss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8LiteBuss* console8LiteBuss = (Console8LiteBuss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8LiteBuss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8LiteBuss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8LiteBuss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8LiteBuss->output[1] = (float*) data;
			break;
		case FADER:
			console8LiteBuss->fader = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8LiteBuss* console8LiteBuss = (Console8LiteBuss*) instance;

	console8LiteBuss->iirAL = 0.0;
	console8LiteBuss->iirBL = 0.0;
	console8LiteBuss->iirAR = 0.0;
	console8LiteBuss->iirBR = 0.0;
	for (int x = 0; x < fix_total; x++) {
		console8LiteBuss->fix[x] = 0.0;
		console8LiteBuss->fixB[x] = 0.0;
	}
	for (int x = 0; x < 10; x++) {
		console8LiteBuss->softL[x] = 0.0;
		console8LiteBuss->softR[x] = 0.0;
	}

	console8LiteBuss->inTrimA = 0.5;
	console8LiteBuss->inTrimB = 0.5;
	console8LiteBuss->lastSampleL = 0.0;
	console8LiteBuss->wasPosClipL = false;
	console8LiteBuss->wasNegClipL = false;
	console8LiteBuss->lastSampleR = 0.0;
	console8LiteBuss->wasPosClipR = false;
	console8LiteBuss->wasNegClipR = false;
	for (int x = 0; x < 17; x++) {
		console8LiteBuss->intermediateL[x] = 0.0;
		console8LiteBuss->intermediateR[x] = 0.0;
	} // ADClip2

	console8LiteBuss->fpdL = 1.0;
	while (console8LiteBuss->fpdL < 16386) console8LiteBuss->fpdL = rand() * UINT32_MAX;
	console8LiteBuss->fpdR = 1.0;
	while (console8LiteBuss->fpdR < 16386) console8LiteBuss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8LiteBuss* console8LiteBuss = (Console8LiteBuss*) instance;

	const float* in1 = console8LiteBuss->input[0];
	const float* in2 = console8LiteBuss->input[1];
	float* out1 = console8LiteBuss->output[0];
	float* out2 = console8LiteBuss->output[1];

	const double sampleRate = console8LiteBuss->sampleRate;

	double iirAmountA = 12.66 / sampleRate;
	// this is our distributed unusual highpass, which is
	// adding subtle harmonics to the really deep stuff to define it
	if (fabs(console8LiteBuss->iirAL) < 1.18e-37) console8LiteBuss->iirAL = 0.0;
	if (fabs(console8LiteBuss->iirBL) < 1.18e-37) console8LiteBuss->iirBL = 0.0;
	if (fabs(console8LiteBuss->iirAR) < 1.18e-37) console8LiteBuss->iirAR = 0.0;
	if (fabs(console8LiteBuss->iirBR) < 1.18e-37) console8LiteBuss->iirBR = 0.0;
	// catch denormals early and only check once per buffer
	if (sampleRate > 49000.0) console8LiteBuss->hsr = true;
	else console8LiteBuss->hsr = false;
	console8LiteBuss->fix[fix_freq] = 24000.0 / sampleRate;
	console8LiteBuss->fix[fix_reso] = 0.55495813;
	double K = tan(M_PI * console8LiteBuss->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8LiteBuss->fix[fix_reso] + K * K);
	console8LiteBuss->fix[fix_a0] = K * K * norm;
	console8LiteBuss->fix[fix_a1] = 2.0 * console8LiteBuss->fix[fix_a0];
	console8LiteBuss->fix[fix_a2] = console8LiteBuss->fix[fix_a0];
	console8LiteBuss->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8LiteBuss->fix[fix_b2] = (1.0 - K / console8LiteBuss->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	console8LiteBuss->cycleEnd = floor(overallscale);
	if (console8LiteBuss->cycleEnd < 1) console8LiteBuss->cycleEnd = 1;
	if (console8LiteBuss->cycleEnd == 3) console8LiteBuss->cycleEnd = 4;
	if (console8LiteBuss->cycleEnd > 4) console8LiteBuss->cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k

	uint32_t inFramesToProcess = sampleFrames;
	console8LiteBuss->spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (console8LiteBuss->spacing < 1) console8LiteBuss->spacing = 1;
	if (console8LiteBuss->spacing > 16) console8LiteBuss->spacing = 16;

	console8LiteBuss->inTrimA = console8LiteBuss->inTrimB;
	console8LiteBuss->inTrimB = *console8LiteBuss->fader * 2.0;
	// 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
	// into softclip and ADClip in case we need intense loudness bursts on transients.

	console8LiteBuss->fixB[fix_freq] = 24000.0 / sampleRate;
	console8LiteBuss->fixB[fix_reso] = 0.5;
	K = tan(M_PI * console8LiteBuss->fixB[fix_freq]); // lowpass
	norm = 1.0 / (1.0 + K / console8LiteBuss->fixB[fix_reso] + K * K);
	console8LiteBuss->fixB[fix_a0] = K * K * norm;
	console8LiteBuss->fixB[fix_a1] = 2.0 * console8LiteBuss->fixB[fix_a0];
	console8LiteBuss->fixB[fix_a2] = console8LiteBuss->fixB[fix_a0];
	console8LiteBuss->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8LiteBuss->fixB[fix_b2] = (1.0 - K / console8LiteBuss->fixB[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8LiteBuss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8LiteBuss->fpdR * 1.18e-17;

		double position = (double) sampleFrames / inFramesToProcess;
		double inTrim = (console8LiteBuss->inTrimA * position) + (console8LiteBuss->inTrimB * (1.0 - position));
		// input trim smoothed to cut out zipper noise

		console8LiteBuss->iirAL = (console8LiteBuss->iirAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		double iirAmountBL = fabs(console8LiteBuss->iirAL) + 0.00001;
		console8LiteBuss->iirBL = (console8LiteBuss->iirBL * (1.0 - iirAmountBL)) + (console8LiteBuss->iirAL * iirAmountBL);
		inputSampleL -= console8LiteBuss->iirBL;
		console8LiteBuss->iirAR = (console8LiteBuss->iirAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		double iirAmountBR = fabs(console8LiteBuss->iirAR) + 0.00001;
		console8LiteBuss->iirBR = (console8LiteBuss->iirBR * (1.0 - iirAmountBR)) + (console8LiteBuss->iirAR * iirAmountBR);
		inputSampleR -= console8LiteBuss->iirBR;
		// Console8 highpass

		if (console8LiteBuss->cycleEnd == 4) {
			console8LiteBuss->softL[8] = console8LiteBuss->softL[7];
			console8LiteBuss->softL[7] = console8LiteBuss->softL[6];
			console8LiteBuss->softL[6] = console8LiteBuss->softL[5];
			console8LiteBuss->softL[5] = console8LiteBuss->softL[4];
			console8LiteBuss->softL[4] = console8LiteBuss->softL[3];
			console8LiteBuss->softL[3] = console8LiteBuss->softL[2];
			console8LiteBuss->softL[2] = console8LiteBuss->softL[1];
			console8LiteBuss->softL[1] = console8LiteBuss->softL[0];
			console8LiteBuss->softL[0] = inputSampleL;
			console8LiteBuss->softR[8] = console8LiteBuss->softR[7];
			console8LiteBuss->softR[7] = console8LiteBuss->softR[6];
			console8LiteBuss->softR[6] = console8LiteBuss->softR[5];
			console8LiteBuss->softR[5] = console8LiteBuss->softR[4];
			console8LiteBuss->softR[4] = console8LiteBuss->softR[3];
			console8LiteBuss->softR[3] = console8LiteBuss->softR[2];
			console8LiteBuss->softR[2] = console8LiteBuss->softR[1];
			console8LiteBuss->softR[1] = console8LiteBuss->softR[0];
			console8LiteBuss->softR[0] = inputSampleR;
		}
		if (console8LiteBuss->cycleEnd == 2) {
			console8LiteBuss->softL[8] = console8LiteBuss->softL[6];
			console8LiteBuss->softL[6] = console8LiteBuss->softL[4];
			console8LiteBuss->softL[4] = console8LiteBuss->softL[2];
			console8LiteBuss->softL[2] = console8LiteBuss->softL[0];
			console8LiteBuss->softL[0] = inputSampleL;
			console8LiteBuss->softR[8] = console8LiteBuss->softR[6];
			console8LiteBuss->softR[6] = console8LiteBuss->softR[4];
			console8LiteBuss->softR[4] = console8LiteBuss->softR[2];
			console8LiteBuss->softR[2] = console8LiteBuss->softR[0];
			console8LiteBuss->softR[0] = inputSampleR;
		}
		if (console8LiteBuss->cycleEnd == 1) {
			console8LiteBuss->softL[8] = console8LiteBuss->softL[4];
			console8LiteBuss->softL[4] = console8LiteBuss->softL[0];
			console8LiteBuss->softL[0] = inputSampleL;
			console8LiteBuss->softR[8] = console8LiteBuss->softR[4];
			console8LiteBuss->softR[4] = console8LiteBuss->softR[0];
			console8LiteBuss->softR[0] = inputSampleR;
		}
		console8LiteBuss->softL[9] = ((console8LiteBuss->softL[0] - console8LiteBuss->softL[4]) - (console8LiteBuss->softL[4] - console8LiteBuss->softL[8]));
		if (console8LiteBuss->softL[9] > 0.56852180) inputSampleL = console8LiteBuss->softL[4] + (fabs(console8LiteBuss->softL[4]) * sin(console8LiteBuss->softL[9] - 0.56852180) * 0.4314782);
		if (-console8LiteBuss->softL[9] > 0.56852180) inputSampleL = console8LiteBuss->softL[4] - (fabs(console8LiteBuss->softL[4]) * sin(-console8LiteBuss->softL[9] - 0.56852180) * 0.4314782);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		console8LiteBuss->softR[9] = ((console8LiteBuss->softR[0] - console8LiteBuss->softR[4]) - (console8LiteBuss->softR[4] - console8LiteBuss->softR[8]));
		if (console8LiteBuss->softR[9] > 0.56852180) inputSampleR = console8LiteBuss->softR[4] + (fabs(console8LiteBuss->softR[4]) * sin(console8LiteBuss->softR[9] - 0.56852180) * 0.4314782);
		if (-console8LiteBuss->softR[9] > 0.56852180) inputSampleR = console8LiteBuss->softR[4] - (fabs(console8LiteBuss->softR[4]) * sin(-console8LiteBuss->softR[9] - 0.56852180) * 0.4314782);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system

		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		if (console8LiteBuss->hsr) {
			double outSample = (inputSampleL * console8LiteBuss->fix[fix_a0]) + console8LiteBuss->fix[fix_sL1];
			console8LiteBuss->fix[fix_sL1] = (inputSampleL * console8LiteBuss->fix[fix_a1]) - (outSample * console8LiteBuss->fix[fix_b1]) + console8LiteBuss->fix[fix_sL2];
			console8LiteBuss->fix[fix_sL2] = (inputSampleL * console8LiteBuss->fix[fix_a2]) - (outSample * console8LiteBuss->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8LiteBuss->fix[fix_a0]) + console8LiteBuss->fix[fix_sR1];
			console8LiteBuss->fix[fix_sR1] = (inputSampleR * console8LiteBuss->fix[fix_a1]) - (outSample * console8LiteBuss->fix[fix_b1]) + console8LiteBuss->fix[fix_sR2];
			console8LiteBuss->fix[fix_sR2] = (inputSampleR * console8LiteBuss->fix[fix_a2]) - (outSample * console8LiteBuss->fix[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		inputSampleL = asin(inputSampleL); // Console8 decode
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		inputSampleR = asin(inputSampleR); // Console8 decode

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
		if (console8LiteBuss->hsr) {
			double outSample = (inputSampleL * console8LiteBuss->fixB[fix_a0]) + console8LiteBuss->fixB[fix_sL1];
			console8LiteBuss->fixB[fix_sL1] = (inputSampleL * console8LiteBuss->fixB[fix_a1]) - (outSample * console8LiteBuss->fixB[fix_b1]) + console8LiteBuss->fixB[fix_sL2];
			console8LiteBuss->fixB[fix_sL2] = (inputSampleL * console8LiteBuss->fixB[fix_a2]) - (outSample * console8LiteBuss->fixB[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8LiteBuss->fixB[fix_a0]) + console8LiteBuss->fixB[fix_sR1];
			console8LiteBuss->fixB[fix_sR1] = (inputSampleR * console8LiteBuss->fixB[fix_a1]) - (outSample * console8LiteBuss->fixB[fix_b1]) + console8LiteBuss->fixB[fix_sR2];
			console8LiteBuss->fixB[fix_sR2] = (inputSampleR * console8LiteBuss->fixB[fix_a2]) - (outSample * console8LiteBuss->fixB[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		inputSampleL *= inTrim;
		inputSampleR *= inTrim;
		// the final output fader, before ClipOnly2

		// begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (console8LiteBuss->wasPosClipL == true) { // current will be over
			if (inputSampleL < console8LiteBuss->lastSampleL) console8LiteBuss->lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
			else console8LiteBuss->lastSampleL = 0.2491717 + (console8LiteBuss->lastSampleL * 0.7390851);
		}
		console8LiteBuss->wasPosClipL = false;
		if (inputSampleL > 0.9549925859) {
			console8LiteBuss->wasPosClipL = true;
			inputSampleL = 0.7058208 + (console8LiteBuss->lastSampleL * 0.2609148);
		}
		if (console8LiteBuss->wasNegClipL == true) { // current will be -over
			if (inputSampleL > console8LiteBuss->lastSampleL) console8LiteBuss->lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
			else console8LiteBuss->lastSampleL = -0.2491717 + (console8LiteBuss->lastSampleL * 0.7390851);
		}
		console8LiteBuss->wasNegClipL = false;
		if (inputSampleL < -0.9549925859) {
			console8LiteBuss->wasNegClipL = true;
			inputSampleL = -0.7058208 + (console8LiteBuss->lastSampleL * 0.2609148);
		}
		console8LiteBuss->intermediateL[console8LiteBuss->spacing] = inputSampleL;
		inputSampleL = console8LiteBuss->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = console8LiteBuss->spacing; x > 0; x--) console8LiteBuss->intermediateL[x - 1] = console8LiteBuss->intermediateL[x];
		console8LiteBuss->lastSampleL = console8LiteBuss->intermediateL[0]; // run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (console8LiteBuss->wasPosClipR == true) { // current will be over
			if (inputSampleR < console8LiteBuss->lastSampleR) console8LiteBuss->lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
			else console8LiteBuss->lastSampleR = 0.2491717 + (console8LiteBuss->lastSampleR * 0.7390851);
		}
		console8LiteBuss->wasPosClipR = false;
		if (inputSampleR > 0.9549925859) {
			console8LiteBuss->wasPosClipR = true;
			inputSampleR = 0.7058208 + (console8LiteBuss->lastSampleR * 0.2609148);
		}
		if (console8LiteBuss->wasNegClipR == true) { // current will be -over
			if (inputSampleR > console8LiteBuss->lastSampleR) console8LiteBuss->lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
			else console8LiteBuss->lastSampleR = -0.2491717 + (console8LiteBuss->lastSampleR * 0.7390851);
		}
		console8LiteBuss->wasNegClipR = false;
		if (inputSampleR < -0.9549925859) {
			console8LiteBuss->wasNegClipR = true;
			inputSampleR = -0.7058208 + (console8LiteBuss->lastSampleR * 0.2609148);
		}
		console8LiteBuss->intermediateR[console8LiteBuss->spacing] = inputSampleR;
		inputSampleR = console8LiteBuss->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = console8LiteBuss->spacing; x > 0; x--) console8LiteBuss->intermediateR[x - 1] = console8LiteBuss->intermediateR[x];
		console8LiteBuss->lastSampleR = console8LiteBuss->intermediateR[0]; // run a little buffer to handle this
		// end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console8LiteBuss->fpdL ^= console8LiteBuss->fpdL << 13;
		console8LiteBuss->fpdL ^= console8LiteBuss->fpdL >> 17;
		console8LiteBuss->fpdL ^= console8LiteBuss->fpdL << 5;
		inputSampleL += (((double) console8LiteBuss->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8LiteBuss->fpdR ^= console8LiteBuss->fpdR << 13;
		console8LiteBuss->fpdR ^= console8LiteBuss->fpdR >> 17;
		console8LiteBuss->fpdR ^= console8LiteBuss->fpdR << 5;
		inputSampleR += (((double) console8LiteBuss->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8LITEBUSS_URI,
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
