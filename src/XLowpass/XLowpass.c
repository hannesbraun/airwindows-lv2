#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define XLOWPASS_URI "https://hannesbraun.net/ns/lv2/airwindows/xlowpass"

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
} XLowpass;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	XLowpass* xLowpass = (XLowpass*) calloc(1, sizeof(XLowpass));
	xLowpass->sampleRate = rate;
	return (LV2_Handle) xLowpass;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	XLowpass* xLowpass = (XLowpass*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			xLowpass->input[0] = (const float*) data;
			break;
		case INPUT_R:
			xLowpass->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			xLowpass->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			xLowpass->output[1] = (float*) data;
			break;
		case GAIN:
			xLowpass->gain = (const float*) data;
			break;
		case FREQ:
			xLowpass->freq = (const float*) data;
			break;
		case NUKE:
			xLowpass->nuke = (const float*) data;
			break;
		case DRY_WET:
			xLowpass->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	XLowpass* xLowpass = (XLowpass*) instance;

	for (int x = 0; x < 15; x++) {
		xLowpass->biquad[x] = 0.0;
		xLowpass->biquadA[x] = 0.0;
		xLowpass->biquadB[x] = 0.0;
		xLowpass->biquadC[x] = 0.0;
		xLowpass->biquadD[x] = 0.0;
	}
	xLowpass->fpdL = 1.0;
	while (xLowpass->fpdL < 16386) xLowpass->fpdL = rand() * UINT32_MAX;
	xLowpass->fpdR = 1.0;
	while (xLowpass->fpdR < 16386) xLowpass->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	XLowpass* xLowpass = (XLowpass*) instance;

	const float* in1 = xLowpass->input[0];
	const float* in2 = xLowpass->input[1];
	float* out1 = xLowpass->output[0];
	float* out2 = xLowpass->output[1];

	double gain = pow(*xLowpass->gain + 0.5, 4);
	xLowpass->biquadA[0] = (pow(*xLowpass->freq, 2) * 20000.0) / xLowpass->sampleRate;
	if (xLowpass->biquadA[0] < 0.001) xLowpass->biquadA[0] = 0.001;

	double compensation = sqrt(xLowpass->biquadA[0]) * 6.4;

	const float nuke = *xLowpass->nuke;
	double clipFactor = 1.0 + (xLowpass->biquadA[0] * xLowpass->biquadA[0] * nuke * 32.0);

	double K = tan(M_PI * xLowpass->biquadA[0]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xLowpass->biquadA[2] = K * K * norm;
	xLowpass->biquadA[3] = 2.0 * xLowpass->biquadA[2];
	xLowpass->biquadA[4] = xLowpass->biquadA[2];
	xLowpass->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	xLowpass->biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;

	for (int x = 0; x < 7; x++) {
		xLowpass->biquad[x] = xLowpass->biquadD[x] = xLowpass->biquadC[x] = xLowpass->biquadB[x] = xLowpass->biquadA[x];
	}

	double aWet = 1.0;
	double bWet = 1.0;
	double cWet = 1.0;
	double dWet = nuke * 4.0;
	double wet = *xLowpass->dryWet;

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
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = xLowpass->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = xLowpass->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		double nukeLevelL = inputSampleL;
		double nukeLevelR = inputSampleR;

		outSample = xLowpass->biquad[2] * inputSampleL + xLowpass->biquad[3] * xLowpass->biquad[7] + xLowpass->biquad[4] * xLowpass->biquad[8] - xLowpass->biquad[5] * xLowpass->biquad[9] - xLowpass->biquad[6] * xLowpass->biquad[10];
		xLowpass->biquad[8] = xLowpass->biquad[7];
		xLowpass->biquad[7] = inputSampleL;
		xLowpass->biquad[10] = xLowpass->biquad[9];
		outSample *= clipFactor;
		if (outSample > 1.57079633) outSample = 1.57079633;
		if (outSample < -1.57079633) outSample = -1.57079633;
		xLowpass->biquad[9] = sin(outSample); // DF1 left
		inputSampleL = outSample / compensation;
		nukeLevelL = inputSampleL;

		outSample = xLowpass->biquad[2] * inputSampleR + xLowpass->biquad[3] * xLowpass->biquad[11] + xLowpass->biquad[4] * xLowpass->biquad[12] - xLowpass->biquad[5] * xLowpass->biquad[13] - xLowpass->biquad[6] * xLowpass->biquad[14];
		xLowpass->biquad[12] = xLowpass->biquad[11];
		xLowpass->biquad[11] = inputSampleR;
		xLowpass->biquad[14] = xLowpass->biquad[13];
		outSample *= clipFactor;
		if (outSample > 1.57079633) outSample = 1.57079633;
		if (outSample < -1.57079633) outSample = -1.57079633;
		xLowpass->biquad[13] = sin(outSample); // DF1 right
		inputSampleR = outSample / compensation;
		nukeLevelR = inputSampleR;

		if (aWet > 0.0) {
			outSample = xLowpass->biquadA[2] * inputSampleL + xLowpass->biquadA[3] * xLowpass->biquadA[7] + xLowpass->biquadA[4] * xLowpass->biquadA[8] - xLowpass->biquadA[5] * xLowpass->biquadA[9] - xLowpass->biquadA[6] * xLowpass->biquadA[10];
			xLowpass->biquadA[8] = xLowpass->biquadA[7];
			xLowpass->biquadA[7] = inputSampleL;
			xLowpass->biquadA[10] = xLowpass->biquadA[9];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadA[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * aWet) + (nukeLevelL * (1.0 - aWet));
			nukeLevelL = inputSampleL;

			outSample = xLowpass->biquadA[2] * inputSampleR + xLowpass->biquadA[3] * xLowpass->biquadA[11] + xLowpass->biquadA[4] * xLowpass->biquadA[12] - xLowpass->biquadA[5] * xLowpass->biquadA[13] - xLowpass->biquadA[6] * xLowpass->biquadA[14];
			xLowpass->biquadA[12] = xLowpass->biquadA[11];
			xLowpass->biquadA[11] = inputSampleR;
			xLowpass->biquadA[14] = xLowpass->biquadA[13];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadA[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * aWet) + (nukeLevelR * (1.0 - aWet));
			nukeLevelR = inputSampleR;
		}
		if (bWet > 0.0) {
			outSample = xLowpass->biquadB[2] * inputSampleL + xLowpass->biquadB[3] * xLowpass->biquadB[7] + xLowpass->biquadB[4] * xLowpass->biquadB[8] - xLowpass->biquadB[5] * xLowpass->biquadB[9] - xLowpass->biquadB[6] * xLowpass->biquadB[10];
			xLowpass->biquadB[8] = xLowpass->biquadB[7];
			xLowpass->biquadB[7] = inputSampleL;
			xLowpass->biquadB[10] = xLowpass->biquadB[9];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadB[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * bWet) + (nukeLevelL * (1.0 - bWet));
			nukeLevelL = inputSampleL;

			outSample = xLowpass->biquadB[2] * inputSampleR + xLowpass->biquadB[3] * xLowpass->biquadB[11] + xLowpass->biquadB[4] * xLowpass->biquadB[12] - xLowpass->biquadB[5] * xLowpass->biquadB[13] - xLowpass->biquadB[6] * xLowpass->biquadB[14];
			xLowpass->biquadB[12] = xLowpass->biquadB[11];
			xLowpass->biquadB[11] = inputSampleR;
			xLowpass->biquadB[14] = xLowpass->biquadB[13];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadB[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * bWet) + (nukeLevelR * (1.0 - bWet));
			nukeLevelR = inputSampleR;
		}
		if (cWet > 0.0) {
			outSample = xLowpass->biquadC[2] * inputSampleL + xLowpass->biquadC[3] * xLowpass->biquadC[7] + xLowpass->biquadC[4] * xLowpass->biquadC[8] - xLowpass->biquadC[5] * xLowpass->biquadC[9] - xLowpass->biquadC[6] * xLowpass->biquadC[10];
			xLowpass->biquadC[8] = xLowpass->biquadC[7];
			xLowpass->biquadC[7] = inputSampleL;
			xLowpass->biquadC[10] = xLowpass->biquadC[9];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadC[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * cWet) + (nukeLevelL * (1.0 - cWet));
			nukeLevelL = inputSampleL;

			outSample = xLowpass->biquadC[2] * inputSampleR + xLowpass->biquadC[3] * xLowpass->biquadC[11] + xLowpass->biquadC[4] * xLowpass->biquadC[12] - xLowpass->biquadC[5] * xLowpass->biquadC[13] - xLowpass->biquadC[6] * xLowpass->biquadC[14];
			xLowpass->biquadC[12] = xLowpass->biquadC[11];
			xLowpass->biquadC[11] = inputSampleR;
			xLowpass->biquadC[14] = xLowpass->biquadC[13];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadC[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * cWet) + (nukeLevelR * (1.0 - cWet));
			nukeLevelR = inputSampleR;
		}
		if (dWet > 0.0) {
			outSample = xLowpass->biquadD[2] * inputSampleL + xLowpass->biquadD[3] * xLowpass->biquadD[7] + xLowpass->biquadD[4] * xLowpass->biquadD[8] - xLowpass->biquadD[5] * xLowpass->biquadD[9] - xLowpass->biquadD[6] * xLowpass->biquadD[10];
			xLowpass->biquadD[8] = xLowpass->biquadD[7];
			xLowpass->biquadD[7] = inputSampleL;
			xLowpass->biquadD[10] = xLowpass->biquadD[9];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadD[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0 - dWet));
			nukeLevelL = inputSampleL;

			outSample = xLowpass->biquadD[2] * inputSampleR + xLowpass->biquadD[3] * xLowpass->biquadD[11] + xLowpass->biquadD[4] * xLowpass->biquadD[12] - xLowpass->biquadD[5] * xLowpass->biquadD[13] - xLowpass->biquadD[6] * xLowpass->biquadD[14];
			xLowpass->biquadD[12] = xLowpass->biquadD[11];
			xLowpass->biquadD[11] = inputSampleR;
			xLowpass->biquadD[14] = xLowpass->biquadD[13];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xLowpass->biquadD[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * dWet) + (nukeLevelR * (1.0 - dWet));
			nukeLevelR = inputSampleR;
		}

		if (wet < 1.0) {
			inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		xLowpass->fpdL ^= xLowpass->fpdL << 13;
		xLowpass->fpdL ^= xLowpass->fpdL >> 17;
		xLowpass->fpdL ^= xLowpass->fpdL << 5;
		inputSampleL += (((double) xLowpass->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		xLowpass->fpdR ^= xLowpass->fpdR << 13;
		xLowpass->fpdR ^= xLowpass->fpdR >> 17;
		xLowpass->fpdR ^= xLowpass->fpdR << 5;
		inputSampleR += (((double) xLowpass->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	XLOWPASS_URI,
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
