#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define CONSOLE8SUBHYPE_URI "https://hannesbraun.net/ns/lv2/airwindows/console8subhype"

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
	const float* console8SubHypeparam;

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
} Console8SubHype;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8SubHype* console8SubHype = (Console8SubHype*) calloc(1, sizeof(Console8SubHype));
	console8SubHype->sampleRate = rate;
	return (LV2_Handle) console8SubHype;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8SubHype* console8SubHype = (Console8SubHype*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8SubHype->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8SubHype->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8SubHype->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8SubHype->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8SubHype* console8SubHype = (Console8SubHype*) instance;

	console8SubHype->iirAL = 0.0;
	console8SubHype->iirBL = 0.0;
	console8SubHype->iirAR = 0.0;
	console8SubHype->iirBR = 0.0;
	for (int x = 0; x < fix_total; x++) console8SubHype->fix[x] = 0.0;
	for (int x = 0; x < 10; x++) {
		console8SubHype->softL[x] = 0.0;
		console8SubHype->softR[x] = 0.0;
	}
	console8SubHype->fpdL = 1.0;
	while (console8SubHype->fpdL < 16386) console8SubHype->fpdL = rand() * UINT32_MAX;
	console8SubHype->fpdR = 1.0;
	while (console8SubHype->fpdR < 16386) console8SubHype->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8SubHype* console8SubHype = (Console8SubHype*) instance;

	const float* in1 = console8SubHype->input[0];
	const float* in2 = console8SubHype->input[1];
	float* out1 = console8SubHype->output[0];
	float* out2 = console8SubHype->output[1];

	const double sampleRate = console8SubHype->sampleRate;

	double iirAmountA = 12.66 / sampleRate;
	// this is our distributed unusual highpass, which is
	// adding subtle harmonics to the really deep stuff to define it
	if (fabs(console8SubHype->iirAL) < 1.18e-37) console8SubHype->iirAL = 0.0;
	if (fabs(console8SubHype->iirBL) < 1.18e-37) console8SubHype->iirBL = 0.0;
	if (fabs(console8SubHype->iirAR) < 1.18e-37) console8SubHype->iirAR = 0.0;
	if (fabs(console8SubHype->iirBR) < 1.18e-37) console8SubHype->iirBR = 0.0;
	// catch denormals early and only check once per buffer
	if (sampleRate > 49000.0) console8SubHype->hsr = true;
	else console8SubHype->hsr = false;
	console8SubHype->fix[fix_freq] = 24000.0 / sampleRate;
	console8SubHype->fix[fix_reso] = 0.59435114;
	double K = tan(M_PI * console8SubHype->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8SubHype->fix[fix_reso] + K * K);
	console8SubHype->fix[fix_a0] = K * K * norm;
	console8SubHype->fix[fix_a1] = 2.0 * console8SubHype->fix[fix_a0];
	console8SubHype->fix[fix_a2] = console8SubHype->fix[fix_a0];
	console8SubHype->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8SubHype->fix[fix_b2] = (1.0 - K / console8SubHype->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	console8SubHype->cycleEnd = floor(overallscale);
	if (console8SubHype->cycleEnd < 1) console8SubHype->cycleEnd = 1;
	if (console8SubHype->cycleEnd == 3) console8SubHype->cycleEnd = 4;
	if (console8SubHype->cycleEnd > 4) console8SubHype->cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8SubHype->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8SubHype->fpdR * 1.18e-17;

		console8SubHype->iirAL = (console8SubHype->iirAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		double iirAmountBL = fabs(console8SubHype->iirAL) + 0.00001;
		console8SubHype->iirBL = (console8SubHype->iirBL * (1.0 - iirAmountBL)) + (console8SubHype->iirAL * iirAmountBL);
		inputSampleL -= console8SubHype->iirBL;
		console8SubHype->iirAR = (console8SubHype->iirAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		double iirAmountBR = fabs(console8SubHype->iirAR) + 0.00001;
		console8SubHype->iirBR = (console8SubHype->iirBR * (1.0 - iirAmountBR)) + (console8SubHype->iirAR * iirAmountBR);
		inputSampleR -= console8SubHype->iirBR;
		// Console8 highpass
		if (console8SubHype->cycleEnd == 4) {
			console8SubHype->softL[8] = console8SubHype->softL[7];
			console8SubHype->softL[7] = console8SubHype->softL[6];
			console8SubHype->softL[6] = console8SubHype->softL[5];
			console8SubHype->softL[5] = console8SubHype->softL[4];
			console8SubHype->softL[4] = console8SubHype->softL[3];
			console8SubHype->softL[3] = console8SubHype->softL[2];
			console8SubHype->softL[2] = console8SubHype->softL[1];
			console8SubHype->softL[1] = console8SubHype->softL[0];
			console8SubHype->softL[0] = inputSampleL;
			console8SubHype->softR[8] = console8SubHype->softR[7];
			console8SubHype->softR[7] = console8SubHype->softR[6];
			console8SubHype->softR[6] = console8SubHype->softR[5];
			console8SubHype->softR[5] = console8SubHype->softR[4];
			console8SubHype->softR[4] = console8SubHype->softR[3];
			console8SubHype->softR[3] = console8SubHype->softR[2];
			console8SubHype->softR[2] = console8SubHype->softR[1];
			console8SubHype->softR[1] = console8SubHype->softR[0];
			console8SubHype->softR[0] = inputSampleR;
		}
		if (console8SubHype->cycleEnd == 2) {
			console8SubHype->softL[8] = console8SubHype->softL[6];
			console8SubHype->softL[6] = console8SubHype->softL[4];
			console8SubHype->softL[4] = console8SubHype->softL[2];
			console8SubHype->softL[2] = console8SubHype->softL[0];
			console8SubHype->softL[0] = inputSampleL;
			console8SubHype->softR[8] = console8SubHype->softR[6];
			console8SubHype->softR[6] = console8SubHype->softR[4];
			console8SubHype->softR[4] = console8SubHype->softR[2];
			console8SubHype->softR[2] = console8SubHype->softR[0];
			console8SubHype->softR[0] = inputSampleR;
		}
		if (console8SubHype->cycleEnd == 1) {
			console8SubHype->softL[8] = console8SubHype->softL[4];
			console8SubHype->softL[4] = console8SubHype->softL[0];
			console8SubHype->softL[0] = inputSampleL;
			console8SubHype->softR[8] = console8SubHype->softR[4];
			console8SubHype->softR[4] = console8SubHype->softR[0];
			console8SubHype->softR[0] = inputSampleR;
		}
		console8SubHype->softL[9] = ((console8SubHype->softL[0] - console8SubHype->softL[4]) - (console8SubHype->softL[4] - console8SubHype->softL[8]));
		if (console8SubHype->softL[9] < -1.57079633) console8SubHype->softL[9] = -1.57079633;
		if (console8SubHype->softL[9] > 1.57079633) console8SubHype->softL[9] = 1.57079633;
		inputSampleL = console8SubHype->softL[8] + (sin(console8SubHype->softL[9]) * 0.61803398);
		console8SubHype->softR[9] = ((console8SubHype->softR[0] - console8SubHype->softR[4]) - (console8SubHype->softR[4] - console8SubHype->softR[8]));
		if (console8SubHype->softR[9] < -1.57079633) console8SubHype->softR[9] = -1.57079633;
		if (console8SubHype->softR[9] > 1.57079633) console8SubHype->softR[9] = 1.57079633;
		inputSampleR = console8SubHype->softR[8] + (sin(console8SubHype->softR[9]) * 0.61803398);
		// treble softening effect ended up being an aural exciter

		if (console8SubHype->hsr) {
			double outSample = (inputSampleL * console8SubHype->fix[fix_a0]) + console8SubHype->fix[fix_sL1];
			console8SubHype->fix[fix_sL1] = (inputSampleL * console8SubHype->fix[fix_a1]) - (outSample * console8SubHype->fix[fix_b1]) + console8SubHype->fix[fix_sL2];
			console8SubHype->fix[fix_sL2] = (inputSampleL * console8SubHype->fix[fix_a2]) - (outSample * console8SubHype->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8SubHype->fix[fix_a0]) + console8SubHype->fix[fix_sR1];
			console8SubHype->fix[fix_sR1] = (inputSampleR * console8SubHype->fix[fix_a1]) - (outSample * console8SubHype->fix[fix_b1]) + console8SubHype->fix[fix_sR2];
			console8SubHype->fix[fix_sR2] = (inputSampleR * console8SubHype->fix[fix_a2]) - (outSample * console8SubHype->fix[fix_b2]);
			inputSampleR = outSample;
		} // fixed biquad filtering ultrasonics
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		inputSampleL = asin(inputSampleL); // Console8 decode
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		inputSampleR = asin(inputSampleR); // Console8 decode

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console8SubHype->fpdL ^= console8SubHype->fpdL << 13;
		console8SubHype->fpdL ^= console8SubHype->fpdL >> 17;
		console8SubHype->fpdL ^= console8SubHype->fpdL << 5;
		inputSampleL += (((double) console8SubHype->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8SubHype->fpdR ^= console8SubHype->fpdR << 13;
		console8SubHype->fpdR ^= console8SubHype->fpdR >> 17;
		console8SubHype->fpdR ^= console8SubHype->fpdR << 5;
		inputSampleR += (((double) console8SubHype->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8SUBHYPE_URI,
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
