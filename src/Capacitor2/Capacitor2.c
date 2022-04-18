#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CAPACITOR2_URI "https://hannesbraun.net/ns/lv2/airwindows/capacitor2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	LOWPASS = 4,
	HIGHPASS = 5,
	NONLIN = 6,
	DRYWET = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* lowpass;
	const float* highpass;
	const float* nonlin;
	const float* drywet;

	double iirHighpassAL;
	double iirHighpassBL;
	double iirHighpassCL;
	double iirHighpassDL;
	double iirHighpassEL;
	double iirHighpassFL;
	double iirLowpassAL;
	double iirLowpassBL;
	double iirLowpassCL;
	double iirLowpassDL;
	double iirLowpassEL;
	double iirLowpassFL;

	double iirHighpassAR;
	double iirHighpassBR;
	double iirHighpassCR;
	double iirHighpassDR;
	double iirHighpassER;
	double iirHighpassFR;
	double iirLowpassAR;
	double iirLowpassBR;
	double iirLowpassCR;
	double iirLowpassDR;
	double iirLowpassER;
	double iirLowpassFR;

	int count;

	double lowpassChase;
	double highpassChase;
	double wetChase;

	double lowpassBaseAmount;
	double highpassBaseAmount;
	double wet;

	double lastLowpass;
	double lastHighpass;
	double lastWet;

	uint32_t fpdL;
	uint32_t fpdR;
} Capacitor2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Capacitor2* capacitor2 = (Capacitor2*) calloc(1, sizeof(Capacitor2));
	capacitor2->sampleRate = rate;
	return (LV2_Handle) capacitor2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Capacitor2* capacitor2 = (Capacitor2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			capacitor2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			capacitor2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			capacitor2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			capacitor2->output[1] = (float*) data;
			break;
		case LOWPASS:
			capacitor2->lowpass = (const float*) data;
			break;
		case HIGHPASS:
			capacitor2->highpass = (const float*) data;
			break;
		case NONLIN:
			capacitor2->nonlin = (const float*) data;
			break;
		case DRYWET:
			capacitor2->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Capacitor2* capacitor2 = (Capacitor2*) instance;

	capacitor2->iirHighpassAL = 0.0;
	capacitor2->iirHighpassBL = 0.0;
	capacitor2->iirHighpassCL = 0.0;
	capacitor2->iirHighpassDL = 0.0;
	capacitor2->iirHighpassEL = 0.0;
	capacitor2->iirHighpassFL = 0.0;
	capacitor2->iirLowpassAL = 0.0;
	capacitor2->iirLowpassBL = 0.0;
	capacitor2->iirLowpassCL = 0.0;
	capacitor2->iirLowpassDL = 0.0;
	capacitor2->iirLowpassEL = 0.0;
	capacitor2->iirLowpassFL = 0.0;

	capacitor2->iirHighpassAR = 0.0;
	capacitor2->iirHighpassBR = 0.0;
	capacitor2->iirHighpassCR = 0.0;
	capacitor2->iirHighpassDR = 0.0;
	capacitor2->iirHighpassER = 0.0;
	capacitor2->iirHighpassFR = 0.0;
	capacitor2->iirLowpassAR = 0.0;
	capacitor2->iirLowpassBR = 0.0;
	capacitor2->iirLowpassCR = 0.0;
	capacitor2->iirLowpassDR = 0.0;
	capacitor2->iirLowpassER = 0.0;
	capacitor2->iirLowpassFR = 0.0;
	capacitor2->count = 0;
	capacitor2->lowpassChase = 0.0;
	capacitor2->highpassChase = 0.0;
	capacitor2->wetChase = 0.0;
	capacitor2->lowpassBaseAmount = 1.0;
	capacitor2->highpassBaseAmount = 0.0;
	capacitor2->wet = 1.0;
	capacitor2->lastLowpass = 1000.0;
	capacitor2->lastHighpass = 1000.0;
	capacitor2->lastWet = 1000.0;

	capacitor2->fpdL = 1.0;
	while (capacitor2->fpdL < 16386) capacitor2->fpdL = rand() * UINT32_MAX;
	capacitor2->fpdR = 1.0;
	while (capacitor2->fpdR < 16386) capacitor2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Capacitor2* capacitor2 = (Capacitor2*) instance;

	const float* in1 = capacitor2->input[0];
	const float* in2 = capacitor2->input[1];
	float* out1 = capacitor2->output[0];
	float* out2 = capacitor2->output[1];

	capacitor2->lowpassChase = pow(*capacitor2->lowpass, 2);
	capacitor2->highpassChase = pow(*capacitor2->highpass, 2);
	double nonLin = 1.0 + ((1.0 - *capacitor2->nonlin) * 6.0);
	double nonLinTrim = 1.5 / cbrt(nonLin);
	capacitor2->wetChase = *capacitor2->drywet;
	//should not scale with sample rate, because values reaching 1 are important
	//to its ability to bypass when set to max
	double lowpassSpeed = 300 / (fabs(capacitor2->lastLowpass - capacitor2->lowpassChase) + 1.0);
	double highpassSpeed = 300 / (fabs(capacitor2->lastHighpass - capacitor2->highpassChase) + 1.0);
	double wetSpeed = 300 / (fabs(capacitor2->lastWet - capacitor2->wetChase) + 1.0);
	capacitor2->lastLowpass = capacitor2->lowpassChase;
	capacitor2->lastHighpass = capacitor2->highpassChase;
	capacitor2->lastWet = capacitor2->wetChase;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = capacitor2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = capacitor2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double dielectricScaleL = fabs(2.0 - ((inputSampleL + nonLin) / nonLin));
		double dielectricScaleR = fabs(2.0 - ((inputSampleR + nonLin) / nonLin));

		capacitor2->lowpassBaseAmount = (((capacitor2->lowpassBaseAmount * lowpassSpeed) + capacitor2->lowpassChase) / (lowpassSpeed + 1.0));
		//positive voltage will mean lower capacitance when capacitor is barium titanate
		//on the lowpass, higher pressure means positive swings/smaller cap/larger value for lowpassAmount
		double lowpassAmountL = capacitor2->lowpassBaseAmount * dielectricScaleL;
		double invLowpassL = 1.0 - lowpassAmountL;
		double lowpassAmountR = capacitor2->lowpassBaseAmount * dielectricScaleR;
		double invLowpassR = 1.0 - lowpassAmountR;

		capacitor2->highpassBaseAmount = (((capacitor2->highpassBaseAmount * highpassSpeed) + capacitor2->highpassChase) / (highpassSpeed + 1.0));
		//positive voltage will mean lower capacitance when capacitor is barium titanate
		//on the highpass, higher pressure means positive swings/smaller cap/larger value for highpassAmount
		double highpassAmountL = capacitor2->highpassBaseAmount * dielectricScaleL;
		double invHighpassL = 1.0 - highpassAmountL;
		double highpassAmountR = capacitor2->highpassBaseAmount * dielectricScaleR;
		double invHighpassR = 1.0 - highpassAmountR;

		capacitor2->wet = (((capacitor2->wet * wetSpeed) + capacitor2->wetChase) / (wetSpeed + 1.0));

		capacitor2->count++;
		if (capacitor2->count > 5) capacitor2->count = 0;
		switch (capacitor2->count) {
			case 0:
				capacitor2->iirHighpassAL = (capacitor2->iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassAL;
				capacitor2->iirLowpassAL = (capacitor2->iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassAL;
				capacitor2->iirHighpassBL = (capacitor2->iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassBL;
				capacitor2->iirLowpassBL = (capacitor2->iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassBL;
				capacitor2->iirHighpassDL = (capacitor2->iirHighpassDL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassDL;
				capacitor2->iirLowpassDL = (capacitor2->iirLowpassDL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassDL;
				capacitor2->iirHighpassAR = (capacitor2->iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassAR;
				capacitor2->iirLowpassAR = (capacitor2->iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassAR;
				capacitor2->iirHighpassBR = (capacitor2->iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassBR;
				capacitor2->iirLowpassBR = (capacitor2->iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassBR;
				capacitor2->iirHighpassDR = (capacitor2->iirHighpassDR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassDR;
				capacitor2->iirLowpassDR = (capacitor2->iirLowpassDR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassDR;
				break;
			case 1:
				capacitor2->iirHighpassAL = (capacitor2->iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassAL;
				capacitor2->iirLowpassAL = (capacitor2->iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassAL;
				capacitor2->iirHighpassCL = (capacitor2->iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassCL;
				capacitor2->iirLowpassCL = (capacitor2->iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassCL;
				capacitor2->iirHighpassEL = (capacitor2->iirHighpassEL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassEL;
				capacitor2->iirLowpassEL = (capacitor2->iirLowpassEL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassEL;
				capacitor2->iirHighpassAR = (capacitor2->iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassAR;
				capacitor2->iirLowpassAR = (capacitor2->iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassAR;
				capacitor2->iirHighpassCR = (capacitor2->iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassCR;
				capacitor2->iirLowpassCR = (capacitor2->iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassCR;
				capacitor2->iirHighpassER = (capacitor2->iirHighpassER * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassER;
				capacitor2->iirLowpassER = (capacitor2->iirLowpassER * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassER;
				break;
			case 2:
				capacitor2->iirHighpassAL = (capacitor2->iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassAL;
				capacitor2->iirLowpassAL = (capacitor2->iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassAL;
				capacitor2->iirHighpassBL = (capacitor2->iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassBL;
				capacitor2->iirLowpassBL = (capacitor2->iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassBL;
				capacitor2->iirHighpassFL = (capacitor2->iirHighpassFL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassFL;
				capacitor2->iirLowpassFL = (capacitor2->iirLowpassFL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassFL;
				capacitor2->iirHighpassAR = (capacitor2->iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassAR;
				capacitor2->iirLowpassAR = (capacitor2->iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassAR;
				capacitor2->iirHighpassBR = (capacitor2->iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassBR;
				capacitor2->iirLowpassBR = (capacitor2->iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassBR;
				capacitor2->iirHighpassFR = (capacitor2->iirHighpassFR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassFR;
				capacitor2->iirLowpassFR = (capacitor2->iirLowpassFR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassFR;
				break;
			case 3:
				capacitor2->iirHighpassAL = (capacitor2->iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassAL;
				capacitor2->iirLowpassAL = (capacitor2->iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassAL;
				capacitor2->iirHighpassCL = (capacitor2->iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassCL;
				capacitor2->iirLowpassCL = (capacitor2->iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassCL;
				capacitor2->iirHighpassDL = (capacitor2->iirHighpassDL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassDL;
				capacitor2->iirLowpassDL = (capacitor2->iirLowpassDL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassDL;
				capacitor2->iirHighpassAR = (capacitor2->iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassAR;
				capacitor2->iirLowpassAR = (capacitor2->iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassAR;
				capacitor2->iirHighpassCR = (capacitor2->iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassCR;
				capacitor2->iirLowpassCR = (capacitor2->iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassCR;
				capacitor2->iirHighpassDR = (capacitor2->iirHighpassDR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassDR;
				capacitor2->iirLowpassDR = (capacitor2->iirLowpassDR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassDR;
				break;
			case 4:
				capacitor2->iirHighpassAL = (capacitor2->iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassAL;
				capacitor2->iirLowpassAL = (capacitor2->iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassAL;
				capacitor2->iirHighpassBL = (capacitor2->iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassBL;
				capacitor2->iirLowpassBL = (capacitor2->iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassBL;
				capacitor2->iirHighpassEL = (capacitor2->iirHighpassEL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassEL;
				capacitor2->iirLowpassEL = (capacitor2->iirLowpassEL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassEL;
				capacitor2->iirHighpassAR = (capacitor2->iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassAR;
				capacitor2->iirLowpassAR = (capacitor2->iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassAR;
				capacitor2->iirHighpassBR = (capacitor2->iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassBR;
				capacitor2->iirLowpassBR = (capacitor2->iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassBR;
				capacitor2->iirHighpassER = (capacitor2->iirHighpassER * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassER;
				capacitor2->iirLowpassER = (capacitor2->iirLowpassER * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassER;
				break;
			case 5:
				capacitor2->iirHighpassAL = (capacitor2->iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassAL;
				capacitor2->iirLowpassAL = (capacitor2->iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassAL;
				capacitor2->iirHighpassCL = (capacitor2->iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassCL;
				capacitor2->iirLowpassCL = (capacitor2->iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassCL;
				capacitor2->iirHighpassFL = (capacitor2->iirHighpassFL * invHighpassL) + (inputSampleL * highpassAmountL);
				inputSampleL -= capacitor2->iirHighpassFL;
				capacitor2->iirLowpassFL = (capacitor2->iirLowpassFL * invLowpassL) + (inputSampleL * lowpassAmountL);
				inputSampleL = capacitor2->iirLowpassFL;
				capacitor2->iirHighpassAR = (capacitor2->iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassAR;
				capacitor2->iirLowpassAR = (capacitor2->iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassAR;
				capacitor2->iirHighpassCR = (capacitor2->iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassCR;
				capacitor2->iirLowpassCR = (capacitor2->iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassCR;
				capacitor2->iirHighpassFR = (capacitor2->iirHighpassFR * invHighpassR) + (inputSampleR * highpassAmountR);
				inputSampleR -= capacitor2->iirHighpassFR;
				capacitor2->iirLowpassFR = (capacitor2->iirLowpassFR * invLowpassR) + (inputSampleR * lowpassAmountR);
				inputSampleR = capacitor2->iirLowpassFR;
				break;
		}
		//Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
		//steepens the filter after minimizing artifacts.

		inputSampleL = (drySampleL * (1.0 - capacitor2->wet)) + (inputSampleL * nonLinTrim * capacitor2->wet);
		inputSampleR = (drySampleR * (1.0 - capacitor2->wet)) + (inputSampleR * nonLinTrim * capacitor2->wet);

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		capacitor2->fpdL ^= capacitor2->fpdL << 13;
		capacitor2->fpdL ^= capacitor2->fpdL >> 17;
		capacitor2->fpdL ^= capacitor2->fpdL << 5;
		inputSampleL += (((double)capacitor2->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		capacitor2->fpdR ^= capacitor2->fpdR << 13;
		capacitor2->fpdR ^= capacitor2->fpdR >> 17;
		capacitor2->fpdR ^= capacitor2->fpdR << 5;
		inputSampleR += (((double)capacitor2->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	CAPACITOR2_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
