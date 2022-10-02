#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define XNOTCH_URI "https://hannesbraun.net/ns/lv2/airwindows/xnotch"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	FREQ = 5,
	NUKE = 6,
	DRY_WET = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* gain;
	const float* freq;
	const float* nuke;
	const float* dryWet;

	double biquad[15];
	double biquadA[15];
	double biquadB[15];
	double biquadC[15];
	double biquadD[15];
	uint32_t fpdL;
	uint32_t fpdR;
} XNotch;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	XNotch* xNotch = (XNotch*) calloc(1, sizeof(XNotch));
	xNotch->sampleRate = rate;
	return (LV2_Handle) xNotch;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	XNotch* xNotch = (XNotch*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			xNotch->input[0] = (const float*) data;
			break;
		case INPUT_R:
			xNotch->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			xNotch->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			xNotch->output[1] = (float*) data;
			break;
		case GAIN:
			xNotch->gain = (const float*) data;
			break;
		case FREQ:
			xNotch->freq = (const float*) data;
			break;
		case NUKE:
			xNotch->nuke = (const float*) data;
			break;
		case DRY_WET:
			xNotch->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	XNotch* xNotch = (XNotch*) instance;

	for (int x = 0; x < 15; x++) {
		xNotch->biquad[x] = 0.0;
		xNotch->biquadA[x] = 0.0;
		xNotch->biquadB[x] = 0.0;
		xNotch->biquadC[x] = 0.0;
		xNotch->biquadD[x] = 0.0;
	}
	xNotch->fpdL = 1.0;
	while (xNotch->fpdL < 16386) xNotch->fpdL = rand() * UINT32_MAX;
	xNotch->fpdR = 1.0;
	while (xNotch->fpdR < 16386) xNotch->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	XNotch* xNotch = (XNotch*) instance;

	const float* in1 = xNotch->input[0];
	const float* in2 = xNotch->input[1];
	float* out1 = xNotch->output[0];
	float* out2 = xNotch->output[1];

	const float nuke = *xNotch->nuke;
	double gain = pow(*xNotch->gain + 0.5, 4);
	xNotch->biquadA[0] = (pow(*xNotch->freq, 3) * 20000.0) / xNotch->sampleRate;
	if (xNotch->biquadA[0] < 0.0003) xNotch->biquadA[0] = 0.0003;
	double clipFactor = 1.0 + (pow(nuke, 3) * 2.0);

	double K = tan(M_PI * xNotch->biquadA[0]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xNotch->biquadA[2] = (1.0 + K * K) * norm;
	xNotch->biquadA[3] = 2.0 * (K * K - 1) * norm;
	xNotch->biquadA[4] = xNotch->biquadA[2];
	xNotch->biquadA[5] = xNotch->biquadA[3];
	xNotch->biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;

	for (int x = 0; x < 7; x++) {
		xNotch->biquad[x] = xNotch->biquadD[x] = xNotch->biquadC[x] = xNotch->biquadB[x] = xNotch->biquadA[x];
	}

	double aWet = 1.0;
	double bWet = 1.0;
	double cWet = 1.0;
	double dWet = nuke * 4.0;
	double wet = *xNotch->dryWet;

	// four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0) {
		aWet = dWet;
		bWet = 0.0;
		cWet = 0.0;
		dWet = 0.0;
	} else if (dWet < 2.0) {
		bWet = dWet - 1.0;
		cWet = 0.0;
		dWet = 0.0;
	} else if (dWet < 3.0) {
		cWet = dWet - 2.0;
		dWet = 0.0;
	} else {
		dWet -= 3.0;
	}
	// this is one way to make a little set of dry/wet stages that are successively added to the
	// output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	// beyond that point: this is a way to progressively add a 'black box' sound processing
	// which lets you fall through to simpler processing at lower settings.
	double outSample = 0.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = xNotch->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = xNotch->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		double nukeLevelL = inputSampleL;
		double nukeLevelR = inputSampleR;

		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		outSample = xNotch->biquad[2] * inputSampleL + xNotch->biquad[3] * xNotch->biquad[7] + xNotch->biquad[4] * xNotch->biquad[8] - xNotch->biquad[5] * xNotch->biquad[9] - xNotch->biquad[6] * xNotch->biquad[10];
		xNotch->biquad[8] = xNotch->biquad[7];
		xNotch->biquad[7] = inputSampleL;
		xNotch->biquad[10] = xNotch->biquad[9];
		nukeLevelL = inputSampleL = xNotch->biquad[9] = outSample; // DF1 left

		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		outSample = xNotch->biquad[2] * inputSampleR + xNotch->biquad[3] * xNotch->biquad[11] + xNotch->biquad[4] * xNotch->biquad[12] - xNotch->biquad[5] * xNotch->biquad[13] - xNotch->biquad[6] * xNotch->biquad[14];
		xNotch->biquad[12] = xNotch->biquad[11];
		xNotch->biquad[11] = inputSampleR;
		xNotch->biquad[14] = xNotch->biquad[13];
		nukeLevelR = inputSampleR = xNotch->biquad[13] = outSample; // DF1 right

		if (aWet > 0.0) {
			inputSampleL *= clipFactor;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xNotch->biquadA[2] * inputSampleL + xNotch->biquadA[3] * xNotch->biquadA[7] + xNotch->biquadA[4] * xNotch->biquadA[8] - xNotch->biquadA[5] * xNotch->biquadA[9] - xNotch->biquadA[6] * xNotch->biquadA[10];
			xNotch->biquadA[8] = xNotch->biquadA[7];
			xNotch->biquadA[7] = inputSampleL;
			xNotch->biquadA[10] = xNotch->biquadA[9];
			inputSampleL = xNotch->biquadA[9] = outSample; // DF1 left
			nukeLevelL = inputSampleL = (inputSampleL * aWet) + (nukeLevelL * (1.0 - aWet));

			inputSampleR *= clipFactor;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xNotch->biquadA[2] * inputSampleR + xNotch->biquadA[3] * xNotch->biquadA[11] + xNotch->biquadA[4] * xNotch->biquadA[12] - xNotch->biquadA[5] * xNotch->biquadA[13] - xNotch->biquadA[6] * xNotch->biquadA[14];
			xNotch->biquadA[12] = xNotch->biquadA[11];
			xNotch->biquadA[11] = inputSampleR;
			xNotch->biquadA[14] = xNotch->biquadA[13];
			inputSampleR = xNotch->biquadA[13] = outSample; // DF1 right
			nukeLevelR = inputSampleR = (inputSampleR * aWet) + (nukeLevelR * (1.0 - aWet));
		}

		if (bWet > 0.0) {
			inputSampleL *= clipFactor;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xNotch->biquadB[2] * inputSampleL + xNotch->biquadB[3] * xNotch->biquadB[7] + xNotch->biquadB[4] * xNotch->biquadB[8] - xNotch->biquadB[5] * xNotch->biquadB[9] - xNotch->biquadB[6] * xNotch->biquadB[10];
			xNotch->biquadB[8] = xNotch->biquadB[7];
			xNotch->biquadB[7] = inputSampleL;
			xNotch->biquadB[10] = xNotch->biquadB[9];
			inputSampleL = xNotch->biquadB[9] = outSample; // DF1 left
			nukeLevelL = inputSampleL = (inputSampleL * bWet) + (nukeLevelL * (1.0 - bWet));

			inputSampleR *= clipFactor;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xNotch->biquadB[2] * inputSampleR + xNotch->biquadB[3] * xNotch->biquadB[11] + xNotch->biquadB[4] * xNotch->biquadB[12] - xNotch->biquadB[5] * xNotch->biquadB[13] - xNotch->biquadB[6] * xNotch->biquadB[14];
			xNotch->biquadB[12] = xNotch->biquadB[11];
			xNotch->biquadB[11] = inputSampleR;
			xNotch->biquadB[14] = xNotch->biquadB[13];
			inputSampleR = xNotch->biquadB[13] = outSample; // DF1 right
			nukeLevelR = inputSampleR = (inputSampleR * bWet) + (nukeLevelR * (1.0 - bWet));
		}
		if (cWet > 0.0) {
			inputSampleL *= clipFactor;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xNotch->biquadC[2] * inputSampleL + xNotch->biquadC[3] * xNotch->biquadC[7] + xNotch->biquadC[4] * xNotch->biquadC[8] - xNotch->biquadC[5] * xNotch->biquadC[9] - xNotch->biquadC[6] * xNotch->biquadC[10];
			xNotch->biquadC[8] = xNotch->biquadC[7];
			xNotch->biquadC[7] = inputSampleL;
			xNotch->biquadC[10] = xNotch->biquadC[9];
			inputSampleL = xNotch->biquadC[9] = outSample; // DF1 left
			nukeLevelL = inputSampleL = (inputSampleL * cWet) + (nukeLevelL * (1.0 - cWet));

			inputSampleR *= clipFactor;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xNotch->biquadC[2] * inputSampleR + xNotch->biquadC[3] * xNotch->biquadC[11] + xNotch->biquadC[4] * xNotch->biquadC[12] - xNotch->biquadC[5] * xNotch->biquadC[13] - xNotch->biquadC[6] * xNotch->biquadC[14];
			xNotch->biquadC[12] = xNotch->biquadC[11];
			xNotch->biquadC[11] = inputSampleR;
			xNotch->biquadC[14] = xNotch->biquadC[13];
			inputSampleR = xNotch->biquadC[13] = outSample; // DF1 right
			nukeLevelR = inputSampleR = (inputSampleR * cWet) + (nukeLevelR * (1.0 - cWet));
		}
		if (dWet > 0.0) {
			inputSampleL *= clipFactor;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xNotch->biquadD[2] * inputSampleL + xNotch->biquadD[3] * xNotch->biquadD[7] + xNotch->biquadD[4] * xNotch->biquadD[8] - xNotch->biquadD[5] * xNotch->biquadD[9] - xNotch->biquadD[6] * xNotch->biquadD[10];
			xNotch->biquadD[8] = xNotch->biquadD[7];
			xNotch->biquadD[7] = inputSampleL;
			xNotch->biquadD[10] = xNotch->biquadD[9];
			inputSampleL = xNotch->biquadD[9] = outSample; // DF1 left
			nukeLevelL = inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0 - dWet));

			inputSampleR *= clipFactor;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xNotch->biquadD[2] * inputSampleR + xNotch->biquadD[3] * xNotch->biquadD[11] + xNotch->biquadD[4] * xNotch->biquadD[12] - xNotch->biquadD[5] * xNotch->biquadD[13] - xNotch->biquadD[6] * xNotch->biquadD[14];
			xNotch->biquadD[12] = xNotch->biquadD[11];
			xNotch->biquadD[11] = inputSampleR;
			xNotch->biquadD[14] = xNotch->biquadD[13];
			inputSampleR = xNotch->biquadD[13] = outSample; // DF1 right
			nukeLevelR = inputSampleR = (inputSampleR * dWet) + (nukeLevelR * (1.0 - dWet));
		}

		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);

		if (wet < 1.0) {
			inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		xNotch->fpdL ^= xNotch->fpdL << 13;
		xNotch->fpdL ^= xNotch->fpdL >> 17;
		xNotch->fpdL ^= xNotch->fpdL << 5;
		inputSampleL += (((double) xNotch->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		xNotch->fpdR ^= xNotch->fpdR << 13;
		xNotch->fpdR ^= xNotch->fpdR >> 17;
		xNotch->fpdR ^= xNotch->fpdR << 5;
		inputSampleR += (((double) xNotch->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	XNOTCH_URI,
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
