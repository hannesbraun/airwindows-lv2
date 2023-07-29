#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define CONSOLE8SUBOUT_URI "https://hannesbraun.net/ns/lv2/airwindows/console8subout"

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
	uint32_t fpdL;
	uint32_t fpdR;
} Console8SubOut;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8SubOut* console8SubOut = (Console8SubOut*) calloc(1, sizeof(Console8SubOut));
	console8SubOut->sampleRate = rate;
	return (LV2_Handle) console8SubOut;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8SubOut* console8SubOut = (Console8SubOut*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8SubOut->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8SubOut->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8SubOut->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8SubOut->output[1] = (float*) data;
			break;
		case FADER:
			console8SubOut->fader = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8SubOut* console8SubOut = (Console8SubOut*) instance;

	console8SubOut->inTrimA = 0.5;
	console8SubOut->inTrimB = 0.5;
	for (int x = 0; x < fix_total; x++) console8SubOut->fix[x] = 0.0;
	console8SubOut->fpdL = 1.0;
	while (console8SubOut->fpdL < 16386) console8SubOut->fpdL = rand() * UINT32_MAX;
	console8SubOut->fpdR = 1.0;
	while (console8SubOut->fpdR < 16386) console8SubOut->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8SubOut* console8SubOut = (Console8SubOut*) instance;

	const float* in1 = console8SubOut->input[0];
	const float* in2 = console8SubOut->input[1];
	float* out1 = console8SubOut->output[0];
	float* out2 = console8SubOut->output[1];

	const double sampleRate = console8SubOut->sampleRate;

	uint32_t inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
	console8SubOut->inTrimA = console8SubOut->inTrimB;
	console8SubOut->inTrimB = *console8SubOut->fader * 2.0;
	// 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
	// into softclipping overdrive.
	if (sampleRate > 49000.0) console8SubOut->hsr = true;
	else console8SubOut->hsr = false;
	console8SubOut->fix[fix_freq] = 24000.0 / sampleRate;
	console8SubOut->fix[fix_reso] = 1.20361562;
	double K = tan(M_PI * console8SubOut->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8SubOut->fix[fix_reso] + K * K);
	console8SubOut->fix[fix_a0] = K * K * norm;
	console8SubOut->fix[fix_a1] = 2.0 * console8SubOut->fix[fix_a0];
	console8SubOut->fix[fix_a2] = console8SubOut->fix[fix_a0];
	console8SubOut->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8SubOut->fix[fix_b2] = (1.0 - K / console8SubOut->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8SubOut->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8SubOut->fpdR * 1.18e-17;

		double position = (double) sampleFrames / inFramesToProcess;
		double inTrim = (console8SubOut->inTrimA * position) + (console8SubOut->inTrimB * (1.0 - position));
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
		if (console8SubOut->hsr) {
			double outSample = (inputSampleL * console8SubOut->fix[fix_a0]) + console8SubOut->fix[fix_sL1];
			console8SubOut->fix[fix_sL1] = (inputSampleL * console8SubOut->fix[fix_a1]) - (outSample * console8SubOut->fix[fix_b1]) + console8SubOut->fix[fix_sL2];
			console8SubOut->fix[fix_sL2] = (inputSampleL * console8SubOut->fix[fix_a2]) - (outSample * console8SubOut->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8SubOut->fix[fix_a0]) + console8SubOut->fix[fix_sR1];
			console8SubOut->fix[fix_sR1] = (inputSampleR * console8SubOut->fix[fix_a1]) - (outSample * console8SubOut->fix[fix_b1]) + console8SubOut->fix[fix_sR2];
			console8SubOut->fix[fix_sR2] = (inputSampleR * console8SubOut->fix[fix_a2]) - (outSample * console8SubOut->fix[fix_b2]);
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
		console8SubOut->fpdL ^= console8SubOut->fpdL << 13;
		console8SubOut->fpdL ^= console8SubOut->fpdL >> 17;
		console8SubOut->fpdL ^= console8SubOut->fpdL << 5;
		inputSampleL += (((double) console8SubOut->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8SubOut->fpdR ^= console8SubOut->fpdR << 13;
		console8SubOut->fpdR ^= console8SubOut->fpdR >> 17;
		console8SubOut->fpdR ^= console8SubOut->fpdR << 5;
		inputSampleR += (((double) console8SubOut->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8SUBOUT_URI,
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
