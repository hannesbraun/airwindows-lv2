#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define XREGION_URI "https://hannesbraun.net/ns/lv2/airwindows/xregion"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GAIN = 4,
	FIRST = 5,
	LAST = 6,
	NUKE = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* gain;
	const float* first;
	const float* last;
	const float* nuke;
	const float* dryWet;

	double biquad[15];
	double biquadA[15];
	double biquadB[15];
	double biquadC[15];
	double biquadD[15];
	uint32_t fpdL;
	uint32_t fpdR;
} XRegion;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	XRegion* xRegion = (XRegion*) calloc(1, sizeof(XRegion));
	xRegion->sampleRate = rate;
	return (LV2_Handle) xRegion;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	XRegion* xRegion = (XRegion*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			xRegion->input[0] = (const float*) data;
			break;
		case INPUT_R:
			xRegion->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			xRegion->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			xRegion->output[1] = (float*) data;
			break;
		case GAIN:
			xRegion->gain = (const float*) data;
			break;
		case FIRST:
			xRegion->first = (const float*) data;
			break;
		case LAST:
			xRegion->last = (const float*) data;
			break;
		case NUKE:
			xRegion->nuke = (const float*) data;
			break;
		case DRY_WET:
			xRegion->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	XRegion* xRegion = (XRegion*) instance;

	for (int x = 0; x < 15; x++) {
		xRegion->biquad[x] = 0.0;
		xRegion->biquadA[x] = 0.0;
		xRegion->biquadB[x] = 0.0;
		xRegion->biquadC[x] = 0.0;
		xRegion->biquadD[x] = 0.0;
	}
	xRegion->fpdL = 1.0;
	while (xRegion->fpdL < 16386) xRegion->fpdL = rand() * UINT32_MAX;
	xRegion->fpdR = 1.0;
	while (xRegion->fpdR < 16386) xRegion->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	XRegion* xRegion = (XRegion*) instance;

	const float* in1 = xRegion->input[0];
	const float* in2 = xRegion->input[1];
	float* out1 = xRegion->output[0];
	float* out2 = xRegion->output[1];

	double gain = pow(*xRegion->gain + 0.5, 4);

	double high = *xRegion->first;
	double low = *xRegion->last;
	double mid = (high + low) * 0.5;
	double spread = 1.001 - fabs(high - low);

	const double sampleRate = xRegion->sampleRate;
	const float nuke = *xRegion->nuke;

	xRegion->biquad[0] = (pow(high, 3) * 20000.0) / sampleRate;
	if (xRegion->biquad[0] < 0.00009) xRegion->biquad[0] = 0.00009;
	double compensation = sqrt(xRegion->biquad[0]) * 6.4 * spread;
	double clipFactor = 0.75 + (xRegion->biquad[0] * nuke * 37.0);

	xRegion->biquadA[0] = (pow((high + mid) * 0.5, 3) * 20000.0) / sampleRate;
	if (xRegion->biquadA[0] < 0.00009) xRegion->biquadA[0] = 0.00009;
	double compensationA = sqrt(xRegion->biquadA[0]) * 6.4 * spread;
	double clipFactorA = 0.75 + (xRegion->biquadA[0] * nuke * 37.0);

	xRegion->biquadB[0] = (pow(mid, 3) * 20000.0) / sampleRate;
	if (xRegion->biquadB[0] < 0.00009) xRegion->biquadB[0] = 0.00009;
	double compensationB = sqrt(xRegion->biquadB[0]) * 6.4 * spread;
	double clipFactorB = 0.75 + (xRegion->biquadB[0] * nuke * 37.0);

	xRegion->biquadC[0] = (pow((mid + low) * 0.5, 3) * 20000.0) / sampleRate;
	if (xRegion->biquadC[0] < 0.00009) xRegion->biquadC[0] = 0.00009;
	double compensationC = sqrt(xRegion->biquadC[0]) * 6.4 * spread;
	double clipFactorC = 0.75 + (xRegion->biquadC[0] * nuke * 37.0);

	xRegion->biquadD[0] = (pow(low, 3) * 20000.0) / sampleRate;
	if (xRegion->biquadD[0] < 0.00009) xRegion->biquadD[0] = 0.00009;
	double compensationD = sqrt(xRegion->biquadD[0]) * 6.4 * spread;
	double clipFactorD = 0.75 + (xRegion->biquadD[0] * nuke * 37.0);

	double K = tan(M_PI * xRegion->biquad[0]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xRegion->biquad[2] = K / 0.7071 * norm;
	xRegion->biquad[4] = -xRegion->biquad[2];
	xRegion->biquad[5] = 2.0 * (K * K - 1.0) * norm;
	xRegion->biquad[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * xRegion->biquadA[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xRegion->biquadA[2] = K / 0.7071 * norm;
	xRegion->biquadA[4] = -xRegion->biquadA[2];
	xRegion->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	xRegion->biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * xRegion->biquadB[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xRegion->biquadB[2] = K / 0.7071 * norm;
	xRegion->biquadB[4] = -xRegion->biquadB[2];
	xRegion->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	xRegion->biquadB[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * xRegion->biquadC[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xRegion->biquadC[2] = K / 0.7071 * norm;
	xRegion->biquadC[4] = -xRegion->biquadC[2];
	xRegion->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	xRegion->biquadC[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * xRegion->biquadD[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	xRegion->biquadD[2] = K / 0.7071 * norm;
	xRegion->biquadD[4] = -xRegion->biquadD[2];
	xRegion->biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	xRegion->biquadD[6] = (1.0 - K / 0.7071 + K * K) * norm;

	double aWet = 1.0;
	double bWet = 1.0;
	double cWet = 1.0;
	double dWet = nuke * 4.0;
	double wet = *xRegion->dryWet;

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
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = xRegion->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = xRegion->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		double nukeLevelL = inputSampleL;
		double nukeLevelR = inputSampleR;

		inputSampleL *= clipFactor;
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		inputSampleL = sin(inputSampleL);
		outSample = xRegion->biquad[2] * inputSampleL + xRegion->biquad[4] * xRegion->biquad[8] - xRegion->biquad[5] * xRegion->biquad[9] - xRegion->biquad[6] * xRegion->biquad[10];
		xRegion->biquad[8] = xRegion->biquad[7];
		xRegion->biquad[7] = inputSampleL;
		xRegion->biquad[10] = xRegion->biquad[9];
		xRegion->biquad[9] = outSample; // DF1 left
		inputSampleL = outSample / compensation;
		nukeLevelL = inputSampleL;

		inputSampleR *= clipFactor;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleR = sin(inputSampleR);
		outSample = xRegion->biquad[2] * inputSampleR + xRegion->biquad[4] * xRegion->biquad[12] - xRegion->biquad[5] * xRegion->biquad[13] - xRegion->biquad[6] * xRegion->biquad[14];
		xRegion->biquad[12] = xRegion->biquad[11];
		xRegion->biquad[11] = inputSampleR;
		xRegion->biquad[14] = xRegion->biquad[13];
		xRegion->biquad[13] = outSample; // DF1 right
		inputSampleR = outSample / compensation;
		nukeLevelR = inputSampleR;

		if (aWet > 0.0) {
			inputSampleL *= clipFactorA;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xRegion->biquadA[2] * inputSampleL + xRegion->biquadA[4] * xRegion->biquadA[8] - xRegion->biquadA[5] * xRegion->biquadA[9] - xRegion->biquadA[6] * xRegion->biquadA[10];
			xRegion->biquadA[8] = xRegion->biquadA[7];
			xRegion->biquadA[7] = inputSampleL;
			xRegion->biquadA[10] = xRegion->biquadA[9];
			xRegion->biquadA[9] = outSample; // DF1 left
			inputSampleL = outSample / compensationA;
			inputSampleL = (inputSampleL * aWet) + (nukeLevelL * (1.0 - aWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactorA;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xRegion->biquadA[2] * inputSampleR + xRegion->biquadA[4] * xRegion->biquadA[12] - xRegion->biquadA[5] * xRegion->biquadA[13] - xRegion->biquadA[6] * xRegion->biquadA[14];
			xRegion->biquadA[12] = xRegion->biquadA[11];
			xRegion->biquadA[11] = inputSampleR;
			xRegion->biquadA[14] = xRegion->biquadA[13];
			xRegion->biquadA[13] = outSample; // DF1 right
			inputSampleR = outSample / compensationA;
			inputSampleR = (inputSampleR * aWet) + (nukeLevelR * (1.0 - aWet));
			nukeLevelR = inputSampleR;
		}
		if (bWet > 0.0) {
			inputSampleL *= clipFactorB;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xRegion->biquadB[2] * inputSampleL + xRegion->biquadB[4] * xRegion->biquadB[8] - xRegion->biquadB[5] * xRegion->biquadB[9] - xRegion->biquadB[6] * xRegion->biquadB[10];
			xRegion->biquadB[8] = xRegion->biquadB[7];
			xRegion->biquadB[7] = inputSampleL;
			xRegion->biquadB[10] = xRegion->biquadB[9];
			xRegion->biquadB[9] = outSample; // DF1 left
			inputSampleL = outSample / compensationB;
			inputSampleL = (inputSampleL * bWet) + (nukeLevelL * (1.0 - bWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactorB;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xRegion->biquadB[2] * inputSampleR + xRegion->biquadB[4] * xRegion->biquadB[12] - xRegion->biquadB[5] * xRegion->biquadB[13] - xRegion->biquadB[6] * xRegion->biquadB[14];
			xRegion->biquadB[12] = xRegion->biquadB[11];
			xRegion->biquadB[11] = inputSampleR;
			xRegion->biquadB[14] = xRegion->biquadB[13];
			xRegion->biquadB[13] = outSample; // DF1 right
			inputSampleR = outSample / compensationB;
			inputSampleR = (inputSampleR * bWet) + (nukeLevelR * (1.0 - bWet));
			nukeLevelR = inputSampleR;
		}
		if (cWet > 0.0) {
			inputSampleL *= clipFactorC;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xRegion->biquadC[2] * inputSampleL + xRegion->biquadC[4] * xRegion->biquadC[8] - xRegion->biquadC[5] * xRegion->biquadC[9] - xRegion->biquadC[6] * xRegion->biquadC[10];
			xRegion->biquadC[8] = xRegion->biquadC[7];
			xRegion->biquadC[7] = inputSampleL;
			xRegion->biquadC[10] = xRegion->biquadC[9];
			xRegion->biquadC[9] = outSample; // DF1 left
			inputSampleL = outSample / compensationC;
			inputSampleL = (inputSampleL * cWet) + (nukeLevelL * (1.0 - cWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactorC;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xRegion->biquadC[2] * inputSampleR + xRegion->biquadC[4] * xRegion->biquadC[12] - xRegion->biquadC[5] * xRegion->biquadC[13] - xRegion->biquadC[6] * xRegion->biquadC[14];
			xRegion->biquadC[12] = xRegion->biquadC[11];
			xRegion->biquadC[11] = inputSampleR;
			xRegion->biquadC[14] = xRegion->biquadC[13];
			xRegion->biquadC[13] = outSample; // DF1 right
			inputSampleR = outSample / compensationC;
			inputSampleR = (inputSampleR * cWet) + (nukeLevelR * (1.0 - cWet));
			nukeLevelR = inputSampleR;
		}
		if (dWet > 0.0) {
			inputSampleL *= clipFactorD;
			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			inputSampleL = sin(inputSampleL);
			outSample = xRegion->biquadD[2] * inputSampleL + xRegion->biquadD[4] * xRegion->biquadD[8] - xRegion->biquadD[5] * xRegion->biquadD[9] - xRegion->biquadD[6] * xRegion->biquadD[10];
			xRegion->biquadD[8] = xRegion->biquadD[7];
			xRegion->biquadD[7] = inputSampleL;
			xRegion->biquadD[10] = xRegion->biquadD[9];
			xRegion->biquadD[9] = outSample; // DF1 left
			inputSampleL = outSample / compensationD;
			inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0 - dWet));
			nukeLevelL = inputSampleL;

			inputSampleR *= clipFactorD;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleR = sin(inputSampleR);
			outSample = xRegion->biquadD[2] * inputSampleR + xRegion->biquadD[4] * xRegion->biquadD[12] - xRegion->biquadD[5] * xRegion->biquadD[13] - xRegion->biquadD[6] * xRegion->biquadD[14];
			xRegion->biquadD[12] = xRegion->biquadD[11];
			xRegion->biquadD[11] = inputSampleR;
			xRegion->biquadD[14] = xRegion->biquadD[13];
			xRegion->biquadD[13] = outSample; // DF1 right
			inputSampleR = outSample / compensationD;
			inputSampleR = (inputSampleR * dWet) + (nukeLevelR * (1.0 - dWet));
			nukeLevelR = inputSampleR;
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
		xRegion->fpdL ^= xRegion->fpdL << 13;
		xRegion->fpdL ^= xRegion->fpdL >> 17;
		xRegion->fpdL ^= xRegion->fpdL << 5;
		inputSampleL += (((double) xRegion->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		xRegion->fpdR ^= xRegion->fpdR << 13;
		xRegion->fpdR ^= xRegion->fpdR >> 17;
		xRegion->fpdR ^= xRegion->fpdR << 5;
		inputSampleR += (((double) xRegion->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	XREGION_URI,
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
