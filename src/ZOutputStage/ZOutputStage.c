#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ZOUTPUTSTAGE_URI "https://hannesbraun.net/ns/lv2/airwindows/zoutputstage"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4,
	OUTPUT = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputGain;
	const float* outputGain;

	double biquadE[15];
	double biquadF[15];
	double iirSampleAL;
	double iirSampleAR;

	uint32_t fpdL;
	uint32_t fpdR;
} ZOutputStage;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ZOutputStage* zOutputStage = (ZOutputStage*) calloc(1, sizeof(ZOutputStage));
	zOutputStage->sampleRate = rate;
	return (LV2_Handle) zOutputStage;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ZOutputStage* zOutputStage = (ZOutputStage*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			zOutputStage->input[0] = (const float*) data;
			break;
		case INPUT_R:
			zOutputStage->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			zOutputStage->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			zOutputStage->output[1] = (float*) data;
			break;
		case INPUT:
			zOutputStage->inputGain = (const float*) data;
			break;
		case OUTPUT:
			zOutputStage->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ZOutputStage* zOutputStage = (ZOutputStage*) instance;

	for (int x = 0; x < 15; x++) {
		zOutputStage->biquadE[x] = 0.0;
		zOutputStage->biquadF[x] = 0.0;
	}
	zOutputStage->iirSampleAL = 0.0;
	zOutputStage->iirSampleAR = 0.0;

	zOutputStage->fpdL = 1.0;
	while (zOutputStage->fpdL < 16386) zOutputStage->fpdL = rand() * UINT32_MAX;
	zOutputStage->fpdR = 1.0;
	while (zOutputStage->fpdR < 16386) zOutputStage->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ZOutputStage* zOutputStage = (ZOutputStage*) instance;

	const float* in1 = zOutputStage->input[0];
	const float* in2 = zOutputStage->input[1];
	float* out1 = zOutputStage->output[0];
	float* out2 = zOutputStage->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= zOutputStage->sampleRate;

	// opamp stuff
	double inTrim = *zOutputStage->inputGain * 10.0;
	inTrim *= inTrim;
	inTrim *= inTrim;
	double outPad = *zOutputStage->outputGain;
	double iirAmountA = 0.00069 / overallscale;
	zOutputStage->biquadF[0] = zOutputStage->biquadE[0] = 15500.0 / zOutputStage->sampleRate;
	zOutputStage->biquadF[1] = zOutputStage->biquadE[1] = 0.935;
	double K = tan(M_PI * zOutputStage->biquadE[0]); // lowpass
	double norm = 1.0 / (1.0 + K / zOutputStage->biquadE[1] + K * K);
	zOutputStage->biquadE[2] = K * K * norm;
	zOutputStage->biquadE[3] = 2.0 * zOutputStage->biquadE[2];
	zOutputStage->biquadE[4] = zOutputStage->biquadE[2];
	zOutputStage->biquadE[5] = 2.0 * (K * K - 1.0) * norm;
	zOutputStage->biquadE[6] = (1.0 - K / zOutputStage->biquadE[1] + K * K) * norm;
	for (int x = 0; x < 7; x++) zOutputStage->biquadF[x] = zOutputStage->biquadE[x];
	// end opamp stuff

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = zOutputStage->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = zOutputStage->fpdR * 1.18e-17;

		if (inTrim != 1.0) {
			inputSampleL *= inTrim;
			inputSampleR *= inTrim;
		}

		// opamp stage
		if (fabs(zOutputStage->iirSampleAL) < 1.18e-37) zOutputStage->iirSampleAL = 0.0;
		zOutputStage->iirSampleAL = (zOutputStage->iirSampleAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
		inputSampleL -= zOutputStage->iirSampleAL;
		if (fabs(zOutputStage->iirSampleAR) < 1.18e-37) zOutputStage->iirSampleAR = 0.0;
		zOutputStage->iirSampleAR = (zOutputStage->iirSampleAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
		inputSampleR -= zOutputStage->iirSampleAR;

		double outSample = zOutputStage->biquadE[2] * inputSampleL + zOutputStage->biquadE[3] * zOutputStage->biquadE[7] + zOutputStage->biquadE[4] * zOutputStage->biquadE[8] - zOutputStage->biquadE[5] * zOutputStage->biquadE[9] - zOutputStage->biquadE[6] * zOutputStage->biquadE[10];
		zOutputStage->biquadE[8] = zOutputStage->biquadE[7];
		zOutputStage->biquadE[7] = inputSampleL;
		inputSampleL = outSample;
		zOutputStage->biquadE[10] = zOutputStage->biquadE[9];
		zOutputStage->biquadE[9] = inputSampleL; // DF1 left

		outSample = zOutputStage->biquadE[2] * inputSampleR + zOutputStage->biquadE[3] * zOutputStage->biquadE[11] + zOutputStage->biquadE[4] * zOutputStage->biquadE[12] - zOutputStage->biquadE[5] * zOutputStage->biquadE[13] - zOutputStage->biquadE[6] * zOutputStage->biquadE[14];
		zOutputStage->biquadE[12] = zOutputStage->biquadE[11];
		zOutputStage->biquadE[11] = inputSampleR;
		inputSampleR = outSample;
		zOutputStage->biquadE[14] = zOutputStage->biquadE[13];
		zOutputStage->biquadE[13] = inputSampleR; // DF1 right

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		inputSampleL -= (inputSampleL * inputSampleL * inputSampleL * inputSampleL * inputSampleL * 0.1768);
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		inputSampleR -= (inputSampleR * inputSampleR * inputSampleR * inputSampleR * inputSampleR * 0.1768);

		outSample = zOutputStage->biquadF[2] * inputSampleL + zOutputStage->biquadF[3] * zOutputStage->biquadF[7] + zOutputStage->biquadF[4] * zOutputStage->biquadF[8] - zOutputStage->biquadF[5] * zOutputStage->biquadF[9] - zOutputStage->biquadF[6] * zOutputStage->biquadF[10];
		zOutputStage->biquadF[8] = zOutputStage->biquadF[7];
		zOutputStage->biquadF[7] = inputSampleL;
		inputSampleL = outSample;
		zOutputStage->biquadF[10] = zOutputStage->biquadF[9];
		zOutputStage->biquadF[9] = inputSampleL; // DF1 left

		outSample = zOutputStage->biquadF[2] * inputSampleR + zOutputStage->biquadF[3] * zOutputStage->biquadF[11] + zOutputStage->biquadF[4] * zOutputStage->biquadF[12] - zOutputStage->biquadF[5] * zOutputStage->biquadF[13] - zOutputStage->biquadF[6] * zOutputStage->biquadF[14];
		zOutputStage->biquadF[12] = zOutputStage->biquadF[11];
		zOutputStage->biquadF[11] = inputSampleR;
		inputSampleR = outSample;
		zOutputStage->biquadF[14] = zOutputStage->biquadF[13];
		zOutputStage->biquadF[13] = inputSampleR; // DF1 right

		if (outPad != 1.0) {
			inputSampleL *= outPad;
			inputSampleR *= outPad;
		}
		// end opamp stage

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		zOutputStage->fpdL ^= zOutputStage->fpdL << 13;
		zOutputStage->fpdL ^= zOutputStage->fpdL >> 17;
		zOutputStage->fpdL ^= zOutputStage->fpdL << 5;
		inputSampleL += (((double) zOutputStage->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		zOutputStage->fpdR ^= zOutputStage->fpdR << 13;
		zOutputStage->fpdR ^= zOutputStage->fpdR >> 17;
		zOutputStage->fpdR ^= zOutputStage->fpdR << 5;
		inputSampleR += (((double) zOutputStage->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ZOUTPUTSTAGE_URI,
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
