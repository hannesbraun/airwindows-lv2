#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define XBANDPASS_URI "https://hannesbraun.net/ns/lv2/airwindows/xbandpass"

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
} XBandpass;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	XBandpass* xBandpass = (XBandpass*) calloc(1, sizeof(XBandpass));
	xBandpass->sampleRate = rate;
	return (LV2_Handle) xBandpass;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	XBandpass* xBandpass = (XBandpass*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			xBandpass->input[0] = (const float*) data;
			break;
		case INPUT_R:
			xBandpass->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			xBandpass->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			xBandpass->output[1] = (float*) data;
			break;
		case GAIN:
			xBandpass->gain = (const float*) data;
			break;
		case FREQ:
			xBandpass->freq = (const float*) data;
			break;
		case NUKE:
			xBandpass->nuke = (const float*) data;
			break;
		case DRY_WET:
			xBandpass->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	XBandpass* xBandpass = (XBandpass*) instance;

	for (int x = 0; x < 15; x++) {
		xBandpass->biquad[x] = 0.0;
		xBandpass->biquadA[x] = 0.0;
		xBandpass->biquadB[x] = 0.0;
		xBandpass->biquadC[x] = 0.0;
		xBandpass->biquadD[x] = 0.0;
	}
	xBandpass->fpdL = 1.0;
	while (xBandpass->fpdL < 16386) xBandpass->fpdL = rand() * UINT32_MAX;
	xBandpass->fpdR = 1.0;
	while (xBandpass->fpdR < 16386) xBandpass->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	XBandpass* xBandpass = (XBandpass*) instance;

	const float* in1 = xBandpass->input[0];
	const float* in2 = xBandpass->input[1];
	float* out1 = xBandpass->output[0];
	float* out2 = xBandpass->output[1];

	double gain = pow(*xBandpass->gain + 0.5, 4);
	xBandpass->biquadA[0] = (pow(*xBandpass->freq, 4) * 20000.0) / xBandpass->sampleRate;
	if (xBandpass->biquadA[0] < 0.00005) xBandpass->biquadA[0] = 0.00005;

	const float nuke = *xBandpass->nuke;
	double compensation = sqrt(xBandpass->biquadA[0]) * 6.4;
	double clipFactor = 0.768 + (xBandpass->biquadA[0] * nuke * 37.0);

	double K = tan(M_PI * xBandpass->biquadA[0]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xBandpass->biquadA[2] = K / 0.7071 * norm;
	// xBandpass->biquadA[3] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
	xBandpass->biquadA[4] = -xBandpass->biquadA[2];
	xBandpass->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	xBandpass->biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;

	for (int x = 0; x < 7; x++) {
		xBandpass->biquad[x] = xBandpass->biquadD[x] = xBandpass->biquadC[x] = xBandpass->biquadB[x] = xBandpass->biquadA[x];
	}

	double aWet = 1.0;
	double bWet = 1.0;
	double cWet = 1.0;
	double dWet = nuke * 4.0;
	double wet = *xBandpass->dryWet;

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
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = xBandpass->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = xBandpass->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		double nukeLevelL = inputSampleL;
		double nukeLevelR = inputSampleR;

		inputSampleL *= clipFactor;
		outSample = xBandpass->biquad[2] * inputSampleL + xBandpass->biquad[4] * xBandpass->biquad[8] - xBandpass->biquad[5] * xBandpass->biquad[9] - xBandpass->biquad[6] * xBandpass->biquad[10];
		xBandpass->biquad[8] = xBandpass->biquad[7];
		xBandpass->biquad[7] = inputSampleL;
		xBandpass->biquad[10] = xBandpass->biquad[9];
		if (outSample > 1.57079633) outSample = 1.57079633;
		if (outSample < -1.57079633) outSample = -1.57079633;
		xBandpass->biquad[9] = sin(outSample); // DF1 left
		inputSampleL = outSample / compensation;
		nukeLevelL = inputSampleL;

		inputSampleR *= clipFactor;
		outSample = xBandpass->biquad[2] * inputSampleR + xBandpass->biquad[4] * xBandpass->biquad[12] - xBandpass->biquad[5] * xBandpass->biquad[13] - xBandpass->biquad[6] * xBandpass->biquad[14];
		xBandpass->biquad[12] = xBandpass->biquad[11];
		xBandpass->biquad[11] = inputSampleR;
		xBandpass->biquad[14] = xBandpass->biquad[13];
		if (outSample > 1.57079633) outSample = 1.57079633;
		if (outSample < -1.57079633) outSample = -1.57079633;
		xBandpass->biquad[13] = sin(outSample); // DF1 right
		inputSampleR = outSample / compensation;
		nukeLevelR = inputSampleR;

		if (aWet > 0.0) {
			inputSampleL *= clipFactor;
			outSample = xBandpass->biquadA[2] * inputSampleL + xBandpass->biquadA[4] * xBandpass->biquadA[8] - xBandpass->biquadA[5] * xBandpass->biquadA[9] - xBandpass->biquadA[6] * xBandpass->biquadA[10];
			xBandpass->biquadA[8] = xBandpass->biquadA[7];
			xBandpass->biquadA[7] = inputSampleL;
			xBandpass->biquadA[10] = xBandpass->biquadA[9];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadA[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * aWet) + (nukeLevelL * (1.0 - aWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactor;
			outSample = xBandpass->biquadA[2] * inputSampleR + xBandpass->biquadA[4] * xBandpass->biquadA[12] - xBandpass->biquadA[5] * xBandpass->biquadA[13] - xBandpass->biquadA[6] * xBandpass->biquadA[14];
			xBandpass->biquadA[12] = xBandpass->biquadA[11];
			xBandpass->biquadA[11] = inputSampleR;
			xBandpass->biquadA[14] = xBandpass->biquadA[13];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadA[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * aWet) + (nukeLevelR * (1.0 - aWet));
			nukeLevelR = inputSampleR;
		}
		if (bWet > 0.0) {
			inputSampleL *= clipFactor;
			outSample = xBandpass->biquadB[2] * inputSampleL + xBandpass->biquadB[4] * xBandpass->biquadB[8] - xBandpass->biquadB[5] * xBandpass->biquadB[9] - xBandpass->biquadB[6] * xBandpass->biquadB[10];
			xBandpass->biquadB[8] = xBandpass->biquadB[7];
			xBandpass->biquadB[7] = inputSampleL;
			xBandpass->biquadB[10] = xBandpass->biquadB[9];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadB[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * bWet) + (nukeLevelL * (1.0 - bWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactor;
			outSample = xBandpass->biquadB[2] * inputSampleR + xBandpass->biquadB[4] * xBandpass->biquadB[12] - xBandpass->biquadB[5] * xBandpass->biquadB[13] - xBandpass->biquadB[6] * xBandpass->biquadB[14];
			xBandpass->biquadB[12] = xBandpass->biquadB[11];
			xBandpass->biquadB[11] = inputSampleR;
			xBandpass->biquadB[14] = xBandpass->biquadB[13];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadB[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * bWet) + (nukeLevelR * (1.0 - bWet));
			nukeLevelR = inputSampleR;
		}
		if (cWet > 0.0) {
			inputSampleL *= clipFactor;
			outSample = xBandpass->biquadC[2] * inputSampleL + xBandpass->biquadC[4] * xBandpass->biquadC[8] - xBandpass->biquadC[5] * xBandpass->biquadC[9] - xBandpass->biquadC[6] * xBandpass->biquadC[10];
			xBandpass->biquadC[8] = xBandpass->biquadC[7];
			xBandpass->biquadC[7] = inputSampleL;
			xBandpass->biquadC[10] = xBandpass->biquadC[9];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadC[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * cWet) + (nukeLevelL * (1.0 - cWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactor;
			outSample = xBandpass->biquadC[2] * inputSampleR + xBandpass->biquadC[4] * xBandpass->biquadC[12] - xBandpass->biquadC[5] * xBandpass->biquadC[13] - xBandpass->biquadC[6] * xBandpass->biquadC[14];
			xBandpass->biquadC[12] = xBandpass->biquadC[11];
			xBandpass->biquadC[11] = inputSampleR;
			xBandpass->biquadC[14] = xBandpass->biquadC[13];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadC[13] = sin(outSample); // DF1 right
			inputSampleR = outSample / compensation;
			inputSampleR = (inputSampleR * cWet) + (nukeLevelR * (1.0 - cWet));
			nukeLevelR = inputSampleR;
		}
		if (dWet > 0.0) {
			inputSampleL *= clipFactor;
			outSample = xBandpass->biquadD[2] * inputSampleL + xBandpass->biquadD[4] * xBandpass->biquadD[8] - xBandpass->biquadD[5] * xBandpass->biquadD[9] - xBandpass->biquadD[6] * xBandpass->biquadD[10];
			xBandpass->biquadD[8] = xBandpass->biquadD[7];
			xBandpass->biquadD[7] = inputSampleL;
			xBandpass->biquadD[10] = xBandpass->biquadD[9];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadD[9] = sin(outSample); // DF1 left
			inputSampleL = outSample / compensation;
			inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0 - dWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactor;
			outSample = xBandpass->biquadD[2] * inputSampleR + xBandpass->biquadD[4] * xBandpass->biquadD[12] - xBandpass->biquadD[5] * xBandpass->biquadD[13] - xBandpass->biquadD[6] * xBandpass->biquadD[14];
			xBandpass->biquadD[12] = xBandpass->biquadD[11];
			xBandpass->biquadD[11] = inputSampleR;
			xBandpass->biquadD[14] = xBandpass->biquadD[13];
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			xBandpass->biquadD[13] = sin(outSample); // DF1 right
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
		xBandpass->fpdL ^= xBandpass->fpdL << 13;
		xBandpass->fpdL ^= xBandpass->fpdL >> 17;
		xBandpass->fpdL ^= xBandpass->fpdL << 5;
		inputSampleL += (((double) xBandpass->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		xBandpass->fpdR ^= xBandpass->fpdR << 13;
		xBandpass->fpdR ^= xBandpass->fpdR >> 17;
		xBandpass->fpdR ^= xBandpass->fpdR << 5;
		inputSampleR += (((double) xBandpass->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	XBANDPASS_URI,
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
