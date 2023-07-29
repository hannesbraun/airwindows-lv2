#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE8BUSSIN_URI "https://hannesbraun.net/ns/lv2/airwindows/console8bussin"

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
} Console8BussIn;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console8BussIn* console8BussIn = (Console8BussIn*) calloc(1, sizeof(Console8BussIn));
	console8BussIn->sampleRate = rate;
	return (LV2_Handle) console8BussIn;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console8BussIn* console8BussIn = (Console8BussIn*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console8BussIn->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console8BussIn->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console8BussIn->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console8BussIn->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console8BussIn* console8BussIn = (Console8BussIn*) instance;

	console8BussIn->iirAL = 0.0;
	console8BussIn->iirBL = 0.0;
	console8BussIn->iirAR = 0.0;
	console8BussIn->iirBR = 0.0;
	for (int x = 0; x < fix_total; x++) console8BussIn->fix[x] = 0.0;
	for (int x = 0; x < 10; x++) {
		console8BussIn->softL[x] = 0.0;
		console8BussIn->softR[x] = 0.0;
	}
	console8BussIn->fpdL = 1.0;
	while (console8BussIn->fpdL < 16386) console8BussIn->fpdL = rand() * UINT32_MAX;
	console8BussIn->fpdR = 1.0;
	while (console8BussIn->fpdR < 16386) console8BussIn->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console8BussIn* console8BussIn = (Console8BussIn*) instance;

	const float* in1 = console8BussIn->input[0];
	const float* in2 = console8BussIn->input[1];
	float* out1 = console8BussIn->output[0];
	float* out2 = console8BussIn->output[1];

	const double sampleRate = console8BussIn->sampleRate;

	double iirAmountA = 12.66 / sampleRate;
	// this is our distributed unusual highpass, which is
	// adding subtle harmonics to the really deep stuff to define it
	if (fabs(console8BussIn->iirAL) < 1.18e-37) console8BussIn->iirAL = 0.0;
	if (fabs(console8BussIn->iirBL) < 1.18e-37) console8BussIn->iirBL = 0.0;
	if (fabs(console8BussIn->iirAR) < 1.18e-37) console8BussIn->iirAR = 0.0;
	if (fabs(console8BussIn->iirBR) < 1.18e-37) console8BussIn->iirBR = 0.0;
	// catch denormals early and only check once per buffer
	if (sampleRate > 49000.0) console8BussIn->hsr = true;
	else console8BussIn->hsr = false;
	console8BussIn->fix[fix_freq] = 24000.0 / sampleRate;
	console8BussIn->fix[fix_reso] = 0.5;
	double K = tan(M_PI * console8BussIn->fix[fix_freq]); // lowpass
	double norm = 1.0 / (1.0 + K / console8BussIn->fix[fix_reso] + K * K);
	console8BussIn->fix[fix_a0] = K * K * norm;
	console8BussIn->fix[fix_a1] = 2.0 * console8BussIn->fix[fix_a0];
	console8BussIn->fix[fix_a2] = console8BussIn->fix[fix_a0];
	console8BussIn->fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	console8BussIn->fix[fix_b2] = (1.0 - K / console8BussIn->fix[fix_reso] + K * K) * norm;
	// this is the fixed biquad distributed anti-aliasing filter
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sampleRate;
	console8BussIn->cycleEnd = floor(overallscale);
	if (console8BussIn->cycleEnd < 1) console8BussIn->cycleEnd = 1;
	if (console8BussIn->cycleEnd == 3) console8BussIn->cycleEnd = 4;
	if (console8BussIn->cycleEnd > 4) console8BussIn->cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console8BussIn->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console8BussIn->fpdR * 1.18e-17;

		console8BussIn->iirAL = (console8BussIn->iirAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		double iirAmountBL = fabs(console8BussIn->iirAL) + 0.00001;
		console8BussIn->iirBL = (console8BussIn->iirBL * (1.0 - iirAmountBL)) + (console8BussIn->iirAL * iirAmountBL);
		inputSampleL -= console8BussIn->iirBL;
		console8BussIn->iirAR = (console8BussIn->iirAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		double iirAmountBR = fabs(console8BussIn->iirAR) + 0.00001;
		console8BussIn->iirBR = (console8BussIn->iirBR * (1.0 - iirAmountBR)) + (console8BussIn->iirAR * iirAmountBR);
		inputSampleR -= console8BussIn->iirBR;
		// Console8 highpass
		if (console8BussIn->cycleEnd == 4) {
			console8BussIn->softL[8] = console8BussIn->softL[7];
			console8BussIn->softL[7] = console8BussIn->softL[6];
			console8BussIn->softL[6] = console8BussIn->softL[5];
			console8BussIn->softL[5] = console8BussIn->softL[4];
			console8BussIn->softL[4] = console8BussIn->softL[3];
			console8BussIn->softL[3] = console8BussIn->softL[2];
			console8BussIn->softL[2] = console8BussIn->softL[1];
			console8BussIn->softL[1] = console8BussIn->softL[0];
			console8BussIn->softL[0] = inputSampleL;
			console8BussIn->softR[8] = console8BussIn->softR[7];
			console8BussIn->softR[7] = console8BussIn->softR[6];
			console8BussIn->softR[6] = console8BussIn->softR[5];
			console8BussIn->softR[5] = console8BussIn->softR[4];
			console8BussIn->softR[4] = console8BussIn->softR[3];
			console8BussIn->softR[3] = console8BussIn->softR[2];
			console8BussIn->softR[2] = console8BussIn->softR[1];
			console8BussIn->softR[1] = console8BussIn->softR[0];
			console8BussIn->softR[0] = inputSampleR;
		}
		if (console8BussIn->cycleEnd == 2) {
			console8BussIn->softL[8] = console8BussIn->softL[6];
			console8BussIn->softL[6] = console8BussIn->softL[4];
			console8BussIn->softL[4] = console8BussIn->softL[2];
			console8BussIn->softL[2] = console8BussIn->softL[0];
			console8BussIn->softL[0] = inputSampleL;
			console8BussIn->softR[8] = console8BussIn->softR[6];
			console8BussIn->softR[6] = console8BussIn->softR[4];
			console8BussIn->softR[4] = console8BussIn->softR[2];
			console8BussIn->softR[2] = console8BussIn->softR[0];
			console8BussIn->softR[0] = inputSampleR;
		}
		if (console8BussIn->cycleEnd == 1) {
			console8BussIn->softL[8] = console8BussIn->softL[4];
			console8BussIn->softL[4] = console8BussIn->softL[0];
			console8BussIn->softL[0] = inputSampleL;
			console8BussIn->softR[8] = console8BussIn->softR[4];
			console8BussIn->softR[4] = console8BussIn->softR[0];
			console8BussIn->softR[0] = inputSampleR;
		}
		console8BussIn->softL[9] = ((console8BussIn->softL[0] - console8BussIn->softL[4]) - (console8BussIn->softL[4] - console8BussIn->softL[8]));
		if (console8BussIn->softL[9] > 0.56852180) inputSampleL = console8BussIn->softL[4] + (fabs(console8BussIn->softL[4]) * sin(console8BussIn->softL[9] - 0.56852180) * 0.4314782);
		if (-console8BussIn->softL[9] > 0.56852180) inputSampleL = console8BussIn->softL[4] - (fabs(console8BussIn->softL[4]) * sin(-console8BussIn->softL[9] - 0.56852180) * 0.4314782);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		console8BussIn->softR[9] = ((console8BussIn->softR[0] - console8BussIn->softR[4]) - (console8BussIn->softR[4] - console8BussIn->softR[8]));
		if (console8BussIn->softR[9] > 0.56852180) inputSampleR = console8BussIn->softR[4] + (fabs(console8BussIn->softR[4]) * sin(console8BussIn->softR[9] - 0.56852180) * 0.4314782);
		if (-console8BussIn->softR[9] > 0.56852180) inputSampleR = console8BussIn->softR[4] - (fabs(console8BussIn->softR[4]) * sin(-console8BussIn->softR[9] - 0.56852180) * 0.4314782);
		// Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		if (console8BussIn->hsr) {
			double outSample = (inputSampleL * console8BussIn->fix[fix_a0]) + console8BussIn->fix[fix_sL1];
			console8BussIn->fix[fix_sL1] = (inputSampleL * console8BussIn->fix[fix_a1]) - (outSample * console8BussIn->fix[fix_b1]) + console8BussIn->fix[fix_sL2];
			console8BussIn->fix[fix_sL2] = (inputSampleL * console8BussIn->fix[fix_a2]) - (outSample * console8BussIn->fix[fix_b2]);
			inputSampleL = outSample;
			outSample = (inputSampleR * console8BussIn->fix[fix_a0]) + console8BussIn->fix[fix_sR1];
			console8BussIn->fix[fix_sR1] = (inputSampleR * console8BussIn->fix[fix_a1]) - (outSample * console8BussIn->fix[fix_b1]) + console8BussIn->fix[fix_sR2];
			console8BussIn->fix[fix_sR2] = (inputSampleR * console8BussIn->fix[fix_a2]) - (outSample * console8BussIn->fix[fix_b2]);
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
		console8BussIn->fpdL ^= console8BussIn->fpdL << 13;
		console8BussIn->fpdL ^= console8BussIn->fpdL >> 17;
		console8BussIn->fpdL ^= console8BussIn->fpdL << 5;
		inputSampleL += (((double) console8BussIn->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console8BussIn->fpdR ^= console8BussIn->fpdR << 13;
		console8BussIn->fpdR ^= console8BussIn->fpdR >> 17;
		console8BussIn->fpdR ^= console8BussIn->fpdR << 5;
		inputSampleR += (((double) console8BussIn->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE8BUSSIN_URI,
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
