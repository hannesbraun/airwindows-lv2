#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define CONSOLE8BUSSOUT_URI "https://hannesbraun.net/ns/lv2/airwindows/console8bussout"

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

	double inTrimA;
	double inTrimB;
	bool hsr;
	double fix[fix_total];

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
} Console8BussOut;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8BussOut* console8BussOut = (Console8BussOut*) calloc(1, sizeof(Console8BussOut));
	console8BussOut->sampleRate = rate;
	return (LV2_Handle) console8BussOut;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8BussOut* console8BussOut = (Console8BussOut*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8BussOut->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8BussOut->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8BussOut->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8BussOut->output[1] = (float*) data;
			break;
		case FADER:
			console8BussOut->fader = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8BussOut* console8BussOut = (Console8BussOut*) instance;

	console8BussOut->inTrimA = 0.5;
	console8BussOut->inTrimB = 0.5;
	for (int x = 0; x < fix_total; x++) console8BussOut->fix[x] = 0.0;
	console8BussOut->lastSampleL = 0.0;
	console8BussOut->wasPosClipL = false;
	console8BussOut->wasNegClipL = false;
	console8BussOut->lastSampleR = 0.0;
	console8BussOut->wasPosClipR = false;
	console8BussOut->wasNegClipR = false;
	for (int x = 0; x < 17; x++) {
		console8BussOut->intermediateL[x] = 0.0;
		console8BussOut->intermediateR[x] = 0.0;
	} // ADClip2

	console8BussOut->fpdL = 1.0;
	while (console8BussOut->fpdL < 16386) console8BussOut->fpdL = rand() * UINT32_MAX;
	console8BussOut->fpdR = 1.0;
	while (console8BussOut->fpdR < 16386) console8BussOut->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8BussOut* console8BussOut = (Console8BussOut*) instance;

	const float* in1 = console8BussOut->input[0];
	const float* in2 = console8BussOut->input[1];
	float* out1 = console8BussOut->output[0];
	float* out2 = console8BussOut->output[1];

	const double sampleRate = console8BussOut->sampleRate;

	uint32_t inFramesToProcess = sampleFrames;
	console8BussOut->inTrimA = console8BussOut->inTrimB;
	console8BussOut->inTrimB = *console8BussOut->fader * 2.0;
	// 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
	// into softclipping overdrive.
	if (sampleRate > 49000.0) console8BussOut->hsr = true;
	else console8BussOut->hsr = false;
	console8BussOut->fix[fix_freq] = 24000.0 / sampleRate;
	console8BussOut->fix[fix_reso] = 0.52110856;
	double K = tan(M_PI * console8BussOut->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8BussOut->fix[fix_reso] + K * K);
	console8BussOut->fix[fix_a0] = K * K * norm;
	console8BussOut->fix[fix_a1] = 2.0 * console8BussOut->fix[fix_a0];
	console8BussOut->fix[fix_a2] = console8BussOut->fix[fix_a0];
	console8BussOut->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8BussOut->fix[fix_b2] = (1.0 - K / console8BussOut->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	console8BussOut->spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (console8BussOut->spacing < 1) console8BussOut->spacing = 1;
	if (console8BussOut->spacing > 16) console8BussOut->spacing = 16;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8BussOut->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8BussOut->fpdR * 1.18e-17;

		double position = (double) sampleFrames / inFramesToProcess;
		double inTrim = (console8BussOut->inTrimA * position) + (console8BussOut->inTrimB * (1.0 - position));
		// input trim smoothed to cut out zipper noise
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
		if (console8BussOut->hsr) {
			double outSample = (inputSampleL * console8BussOut->fix[fix_a0]) + console8BussOut->fix[fix_sL1];
			console8BussOut->fix[fix_sL1] = (inputSampleL * console8BussOut->fix[fix_a1]) - (outSample * console8BussOut->fix[fix_b1]) + console8BussOut->fix[fix_sL2];
			console8BussOut->fix[fix_sL2] = (inputSampleL * console8BussOut->fix[fix_a2]) - (outSample * console8BussOut->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8BussOut->fix[fix_a0]) + console8BussOut->fix[fix_sR1];
			console8BussOut->fix[fix_sR1] = (inputSampleR * console8BussOut->fix[fix_a1]) - (outSample * console8BussOut->fix[fix_b1]) + console8BussOut->fix[fix_sR2];
			console8BussOut->fix[fix_sR2] = (inputSampleR * console8BussOut->fix[fix_a2]) - (outSample * console8BussOut->fix[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		inputSampleL *= inTrim;
		inputSampleR *= inTrim;
		// the final output fader, before ClipOnly2 and dithering

		// begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (console8BussOut->wasPosClipL == true) { // current will be over
			if (inputSampleL < console8BussOut->lastSampleL) console8BussOut->lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
			else console8BussOut->lastSampleL = 0.2491717 + (console8BussOut->lastSampleL * 0.7390851);
		}
		console8BussOut->wasPosClipL = false;
		if (inputSampleL > 0.9549925859) {
			console8BussOut->wasPosClipL = true;
			inputSampleL = 0.7058208 + (console8BussOut->lastSampleL * 0.2609148);
		}
		if (console8BussOut->wasNegClipL == true) { // current will be -over
			if (inputSampleL > console8BussOut->lastSampleL) console8BussOut->lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
			else console8BussOut->lastSampleL = -0.2491717 + (console8BussOut->lastSampleL * 0.7390851);
		}
		console8BussOut->wasNegClipL = false;
		if (inputSampleL < -0.9549925859) {
			console8BussOut->wasNegClipL = true;
			inputSampleL = -0.7058208 + (console8BussOut->lastSampleL * 0.2609148);
		}
		console8BussOut->intermediateL[console8BussOut->spacing] = inputSampleL;
		inputSampleL = console8BussOut->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = console8BussOut->spacing; x > 0; x--) console8BussOut->intermediateL[x - 1] = console8BussOut->intermediateL[x];
		console8BussOut->lastSampleL = console8BussOut->intermediateL[0]; // run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (console8BussOut->wasPosClipR == true) { // current will be over
			if (inputSampleR < console8BussOut->lastSampleR) console8BussOut->lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
			else console8BussOut->lastSampleR = 0.2491717 + (console8BussOut->lastSampleR * 0.7390851);
		}
		console8BussOut->wasPosClipR = false;
		if (inputSampleR > 0.9549925859) {
			console8BussOut->wasPosClipR = true;
			inputSampleR = 0.7058208 + (console8BussOut->lastSampleR * 0.2609148);
		}
		if (console8BussOut->wasNegClipR == true) { // current will be -over
			if (inputSampleR > console8BussOut->lastSampleR) console8BussOut->lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
			else console8BussOut->lastSampleR = -0.2491717 + (console8BussOut->lastSampleR * 0.7390851);
		}
		console8BussOut->wasNegClipR = false;
		if (inputSampleR < -0.9549925859) {
			console8BussOut->wasNegClipR = true;
			inputSampleR = -0.7058208 + (console8BussOut->lastSampleR * 0.2609148);
		}
		console8BussOut->intermediateR[console8BussOut->spacing] = inputSampleR;
		inputSampleR = console8BussOut->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = console8BussOut->spacing; x > 0; x--) console8BussOut->intermediateR[x - 1] = console8BussOut->intermediateR[x];
		console8BussOut->lastSampleR = console8BussOut->intermediateR[0]; // run a little buffer to handle this
		// end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console8BussOut->fpdL ^= console8BussOut->fpdL << 13;
		console8BussOut->fpdL ^= console8BussOut->fpdL >> 17;
		console8BussOut->fpdL ^= console8BussOut->fpdL << 5;
		inputSampleL += (((double) console8BussOut->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8BussOut->fpdR ^= console8BussOut->fpdR << 13;
		console8BussOut->fpdR ^= console8BussOut->fpdR >> 17;
		console8BussOut->fpdR ^= console8BussOut->fpdR << 5;
		inputSampleR += (((double) console8BussOut->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8BUSSOUT_URI,
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
