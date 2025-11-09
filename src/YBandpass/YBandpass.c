#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define YBANDPASS_URI "https://hannesbraun.net/ns/lv2/airwindows/ybandpass"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	FREQ = 5,
	RESON8 = 6,
	RES_EDGE = 7,
	OUTPUT = 8,
	DRY_WET = 9
} PortIndex;

enum {
	biq_freq,
	biq_reso,
	biq_a0,
	biq_a1,
	biq_a2,
	biq_b1,
	biq_b2,
	biq_aA0,
	biq_aA1,
	biq_aA2,
	biq_bA1,
	biq_bA2,
	biq_aB0,
	biq_aB1,
	biq_aB2,
	biq_bB1,
	biq_bB2,
	biq_sL1,
	biq_sL2,
	biq_sR1,
	biq_sR2,
	biq_total
}; // coefficient interpolating biquad filter, stereo

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
	const float* gain;
	const float* freq;
	const float* resEdge;
	const float* reson8;
	const float* outputGain;
	const float* dryWet;

	double biquad[biq_total];

	double powFactorA;
	double powFactorB;
	double inTrimA;
	double inTrimB;
	double outTrimA;
	double outTrimB;

	double fixA[fix_total];
	double fixB[fix_total];

	uint32_t fpdL;
	uint32_t fpdR;
} YBandpass;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	YBandpass* yBandpass = (YBandpass*) calloc(1, sizeof(YBandpass));
	yBandpass->sampleRate = rate;
	return (LV2_Handle) yBandpass;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	YBandpass* yBandpass = (YBandpass*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			yBandpass->input[0] = (const float*) data;
			break;
		case INPUT_R:
			yBandpass->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			yBandpass->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			yBandpass->output[1] = (float*) data;
			break;
		case GAIN:
			yBandpass->gain = (const float*) data;
			break;
		case FREQ:
			yBandpass->freq = (const float*) data;
			break;
		case RESON8:
			yBandpass->reson8 = (const float*) data;
			break;
		case RES_EDGE:
			yBandpass->resEdge = (const float*) data;
			break;
		case OUTPUT:
			yBandpass->outputGain = (const float*) data;
			break;
		case DRY_WET:
			yBandpass->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	YBandpass* yBandpass = (YBandpass*) instance;

	for (int x = 0; x < biq_total; x++) {
		yBandpass->biquad[x] = 0.0;
	}
	yBandpass->powFactorA = 1.0;
	yBandpass->powFactorB = 1.0;
	yBandpass->inTrimA = 0.1;
	yBandpass->inTrimB = 0.1;
	yBandpass->outTrimA = 1.0;
	yBandpass->outTrimB = 1.0;
	for (int x = 0; x < fix_total; x++) {
		yBandpass->fixA[x] = 0.0;
		yBandpass->fixB[x] = 0.0;
	}

	yBandpass->fpdL = 1.0;
	while (yBandpass->fpdL < 16386) yBandpass->fpdL = rand() * UINT32_MAX;
	yBandpass->fpdR = 1.0;
	while (yBandpass->fpdR < 16386) yBandpass->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	YBandpass* yBandpass = (YBandpass*) instance;

	const float* in1 = yBandpass->input[0];
	const float* in2 = yBandpass->input[1];
	float* out1 = yBandpass->output[0];
	float* out2 = yBandpass->output[1];

	const uint32_t inFramesToProcess = sampleFrames;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= yBandpass->sampleRate;

	yBandpass->inTrimA = yBandpass->inTrimB;
	yBandpass->inTrimB = *yBandpass->gain * 10.0;

	yBandpass->biquad[biq_freq] = pow(*yBandpass->freq, 3) * 20000.0;
	if (yBandpass->biquad[biq_freq] < 15.0) yBandpass->biquad[biq_freq] = 15.0;
	yBandpass->biquad[biq_freq] /= yBandpass->sampleRate;
	yBandpass->biquad[biq_reso] = (pow(*yBandpass->reson8, 2) * 15.0) + 0.5571;
	yBandpass->biquad[biq_aA0] = yBandpass->biquad[biq_aB0];
	// biquad[biq_aA1] = biquad[biq_aB1];
	yBandpass->biquad[biq_aA2] = yBandpass->biquad[biq_aB2];
	yBandpass->biquad[biq_bA1] = yBandpass->biquad[biq_bB1];
	yBandpass->biquad[biq_bA2] = yBandpass->biquad[biq_bB2];
	// previous run through the buffer is still in the filter, so we move it
	// to the A section and now it's the new starting point.
	double K = tan(M_PI * yBandpass->biquad[biq_freq]);
	double norm = 1.0 / (1.0 + K / yBandpass->biquad[biq_reso] + K * K);
	yBandpass->biquad[biq_aB0] = K / yBandpass->biquad[biq_reso] * norm;
	// biquad[biq_aB1] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
	yBandpass->biquad[biq_aB2] = -yBandpass->biquad[biq_aB0];
	yBandpass->biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	yBandpass->biquad[biq_bB2] = (1.0 - K / yBandpass->biquad[biq_reso] + K * K) * norm;
	// for the coefficient-interpolated biquad filter

	yBandpass->powFactorA = yBandpass->powFactorB;
	yBandpass->powFactorB = pow(*yBandpass->resEdge + 0.9, 4);

	// 1.0 == target neutral

	yBandpass->outTrimA = yBandpass->outTrimB;
	yBandpass->outTrimB = *yBandpass->outputGain;

	double wet = *yBandpass->dryWet;

	yBandpass->fixA[fix_freq] = yBandpass->fixB[fix_freq] = 20000.0 / yBandpass->sampleRate;
	yBandpass->fixA[fix_reso] = yBandpass->fixB[fix_reso] = 0.7071; // butterworth Q

	K = tan(M_PI * yBandpass->fixA[fix_freq]);
	norm = 1.0 / (1.0 + K / yBandpass->fixA[fix_reso] + K * K);
	yBandpass->fixA[fix_a0] = yBandpass->fixB[fix_a0] = K * K * norm;
	yBandpass->fixA[fix_a1] = yBandpass->fixB[fix_a1] = 2.0 * yBandpass->fixA[fix_a0];
	yBandpass->fixA[fix_a2] = yBandpass->fixB[fix_a2] = yBandpass->fixA[fix_a0];
	yBandpass->fixA[fix_b1] = yBandpass->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	yBandpass->fixA[fix_b2] = yBandpass->fixB[fix_b2] = (1.0 - K / yBandpass->fixA[fix_reso] + K * K) * norm;
	// for the fixed-position biquad filter

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = yBandpass->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = yBandpass->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double temp = (double) sampleFrames / inFramesToProcess;
		yBandpass->biquad[biq_a0] = (yBandpass->biquad[biq_aA0] * temp) + (yBandpass->biquad[biq_aB0] * (1.0 - temp));
		// biquad[biq_a1] = (biquad[biq_aA1]*temp)+(biquad[biq_aB1]*(1.0-temp));
		yBandpass->biquad[biq_a2] = (yBandpass->biquad[biq_aA2] * temp) + (yBandpass->biquad[biq_aB2] * (1.0 - temp));
		yBandpass->biquad[biq_b1] = (yBandpass->biquad[biq_bA1] * temp) + (yBandpass->biquad[biq_bB1] * (1.0 - temp));
		yBandpass->biquad[biq_b2] = (yBandpass->biquad[biq_bA2] * temp) + (yBandpass->biquad[biq_bB2] * (1.0 - temp));
		// this is the interpolation code for the biquad
		double powFactor = (yBandpass->powFactorA * temp) + (yBandpass->powFactorB * (1.0 - temp));
		double inTrim = (yBandpass->inTrimA * temp) + (yBandpass->inTrimB * (1.0 - temp));
		double outTrim = (yBandpass->outTrimA * temp) + (yBandpass->outTrimB * (1.0 - temp));

		inputSampleL *= inTrim;
		inputSampleR *= inTrim;

		temp = (inputSampleL * yBandpass->fixA[fix_a0]) + yBandpass->fixA[fix_sL1];
		yBandpass->fixA[fix_sL1] = (inputSampleL * yBandpass->fixA[fix_a1]) - (temp * yBandpass->fixA[fix_b1]) + yBandpass->fixA[fix_sL2];
		yBandpass->fixA[fix_sL2] = (inputSampleL * yBandpass->fixA[fix_a2]) - (temp * yBandpass->fixA[fix_b2]);
		inputSampleL = temp; // fixed biquad filtering ultrasonics
		temp = (inputSampleR * yBandpass->fixA[fix_a0]) + yBandpass->fixA[fix_sR1];
		yBandpass->fixA[fix_sR1] = (inputSampleR * yBandpass->fixA[fix_a1]) - (temp * yBandpass->fixA[fix_b1]) + yBandpass->fixA[fix_sR2];
		yBandpass->fixA[fix_sR2] = (inputSampleR * yBandpass->fixA[fix_a2]) - (temp * yBandpass->fixA[fix_b2]);
		inputSampleR = temp; // fixed biquad filtering ultrasonics

		// encode/decode courtesy of torridgristle under the MIT license
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		else if (inputSampleL > 0.0) inputSampleL = 1.0 - pow(1.0 - inputSampleL, powFactor);
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		else if (inputSampleL < 0.0) inputSampleL = -1.0 + pow(1.0 + inputSampleL, powFactor);
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		else if (inputSampleR > 0.0) inputSampleR = 1.0 - pow(1.0 - inputSampleR, powFactor);
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		else if (inputSampleR < 0.0) inputSampleR = -1.0 + pow(1.0 + inputSampleR, powFactor);

		temp = (inputSampleL * yBandpass->biquad[biq_a0]) + yBandpass->biquad[biq_sL1];
		yBandpass->biquad[biq_sL1] = -(temp * yBandpass->biquad[biq_b1]) + yBandpass->biquad[biq_sL2];
		yBandpass->biquad[biq_sL2] = (inputSampleL * yBandpass->biquad[biq_a2]) - (temp * yBandpass->biquad[biq_b2]);
		inputSampleL = temp; // coefficient interpolating biquad filter
		temp = (inputSampleR * yBandpass->biquad[biq_a0]) + yBandpass->biquad[biq_sR1];
		yBandpass->biquad[biq_sR1] = -(temp * yBandpass->biquad[biq_b1]) + yBandpass->biquad[biq_sR2];
		yBandpass->biquad[biq_sR2] = (inputSampleR * yBandpass->biquad[biq_a2]) - (temp * yBandpass->biquad[biq_b2]);
		inputSampleR = temp; // coefficient interpolating biquad filter

		// encode/decode courtesy of torridgristle under the MIT license
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		else if (inputSampleL > 0.0) inputSampleL = 1.0 - pow(1.0 - inputSampleL, (1.0 / powFactor));
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		else if (inputSampleL < 0.0) inputSampleL = -1.0 + pow(1.0 + inputSampleL, (1.0 / powFactor));
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		else if (inputSampleR > 0.0) inputSampleR = 1.0 - pow(1.0 - inputSampleR, (1.0 / powFactor));
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		else if (inputSampleR < 0.0) inputSampleR = -1.0 + pow(1.0 + inputSampleR, (1.0 / powFactor));

		inputSampleL *= outTrim;
		inputSampleR *= outTrim;

		temp = (inputSampleL * yBandpass->fixB[fix_a0]) + yBandpass->fixB[fix_sL1];
		yBandpass->fixB[fix_sL1] = (inputSampleL * yBandpass->fixB[fix_a1]) - (temp * yBandpass->fixB[fix_b1]) + yBandpass->fixB[fix_sL2];
		yBandpass->fixB[fix_sL2] = (inputSampleL * yBandpass->fixB[fix_a2]) - (temp * yBandpass->fixB[fix_b2]);
		inputSampleL = temp; // fixed biquad filtering ultrasonics
		temp = (inputSampleR * yBandpass->fixB[fix_a0]) + yBandpass->fixB[fix_sR1];
		yBandpass->fixB[fix_sR1] = (inputSampleR * yBandpass->fixB[fix_a1]) - (temp * yBandpass->fixB[fix_b1]) + yBandpass->fixB[fix_sR2];
		yBandpass->fixB[fix_sR2] = (inputSampleR * yBandpass->fixB[fix_a2]) - (temp * yBandpass->fixB[fix_b2]);
		inputSampleR = temp; // fixed biquad filtering ultrasonics

		if (wet < 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		yBandpass->fpdL ^= yBandpass->fpdL << 13;
		yBandpass->fpdL ^= yBandpass->fpdL >> 17;
		yBandpass->fpdL ^= yBandpass->fpdL << 5;
		inputSampleL += (((double) yBandpass->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		yBandpass->fpdR ^= yBandpass->fpdR << 13;
		yBandpass->fpdR ^= yBandpass->fpdR >> 17;
		yBandpass->fpdR ^= yBandpass->fpdR << 5;
		inputSampleR += (((double) yBandpass->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	YBANDPASS_URI,
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
