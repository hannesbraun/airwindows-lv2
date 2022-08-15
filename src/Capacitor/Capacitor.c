#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CAPACITOR_URI "https://hannesbraun.net/ns/lv2/airwindows/capacitor"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	LOWPASS = 4,
	HIGHPASS = 5,
	DRY_WET = 6
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* lowpass;
	const float* highpass;
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

	double lowpassAmount;
	double highpassAmount;
	double wet;

	double lastLowpass;
	double lastHighpass;
	double lastWet;

	uint32_t fpdL;
	uint32_t fpdR;
} Capacitor;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Capacitor* capacitor = (Capacitor*) calloc(1, sizeof(Capacitor));
	return (LV2_Handle) capacitor;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Capacitor* capacitor = (Capacitor*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			capacitor->input[0] = (const float*) data;
			break;
		case INPUT_R:
			capacitor->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			capacitor->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			capacitor->output[1] = (float*) data;
			break;
		case LOWPASS:
			capacitor->lowpass = (const float*) data;
			break;
		case HIGHPASS:
			capacitor->highpass = (const float*) data;
			break;
		case DRY_WET:
			capacitor->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Capacitor* capacitor = (Capacitor*) instance;

	capacitor->iirHighpassAL = 0.0;
	capacitor->iirHighpassBL = 0.0;
	capacitor->iirHighpassCL = 0.0;
	capacitor->iirHighpassDL = 0.0;
	capacitor->iirHighpassEL = 0.0;
	capacitor->iirHighpassFL = 0.0;
	capacitor->iirLowpassAL = 0.0;
	capacitor->iirLowpassBL = 0.0;
	capacitor->iirLowpassCL = 0.0;
	capacitor->iirLowpassDL = 0.0;
	capacitor->iirLowpassEL = 0.0;
	capacitor->iirLowpassFL = 0.0;

	capacitor->iirHighpassAR = 0.0;
	capacitor->iirHighpassBR = 0.0;
	capacitor->iirHighpassCR = 0.0;
	capacitor->iirHighpassDR = 0.0;
	capacitor->iirHighpassER = 0.0;
	capacitor->iirHighpassFR = 0.0;
	capacitor->iirLowpassAR = 0.0;
	capacitor->iirLowpassBR = 0.0;
	capacitor->iirLowpassCR = 0.0;
	capacitor->iirLowpassDR = 0.0;
	capacitor->iirLowpassER = 0.0;
	capacitor->iirLowpassFR = 0.0;
	capacitor->count = 0;
	capacitor->lowpassChase = 0.0;
	capacitor->highpassChase = 0.0;
	capacitor->wetChase = 0.0;
	capacitor->lowpassAmount = 1.0;
	capacitor->highpassAmount = 0.0;
	capacitor->wet = 1.0;
	capacitor->lastLowpass = 1000.0;
	capacitor->lastHighpass = 1000.0;
	capacitor->lastWet = 1000.0;

	capacitor->fpdL = 1.0;
	while (capacitor->fpdL < 16386) capacitor->fpdL = rand() * UINT32_MAX;
	capacitor->fpdR = 1.0;
	while (capacitor->fpdR < 16386) capacitor->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Capacitor* capacitor = (Capacitor*) instance;

	const float* in1 = capacitor->input[0];
	const float* in2 = capacitor->input[1];
	float* out1 = capacitor->output[0];
	float* out2 = capacitor->output[1];

	capacitor->lowpassChase = pow(*capacitor->lowpass, 2);
	capacitor->highpassChase = pow(*capacitor->highpass, 2);
	capacitor->wetChase = *capacitor->drywet;
	// should not scale with sample rate, because values reaching 1 are important
	// to its ability to bypass when set to max
	double lowpassSpeed = 300 / (fabs(capacitor->lastLowpass - capacitor->lowpassChase) + 1.0);
	double highpassSpeed = 300 / (fabs(capacitor->lastHighpass - capacitor->highpassChase) + 1.0);
	double wetSpeed = 300 / (fabs(capacitor->lastWet - capacitor->wetChase) + 1.0);
	capacitor->lastLowpass = capacitor->lowpassChase;
	capacitor->lastHighpass = capacitor->highpassChase;
	capacitor->lastWet = capacitor->wetChase;

	double invLowpass;
	double invHighpass;
	double dry;

	double inputSampleL;
	double inputSampleR;
	float drySampleL;
	float drySampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = capacitor->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = capacitor->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		capacitor->lowpassAmount = (((capacitor->lowpassAmount * lowpassSpeed) + capacitor->lowpassChase) / (lowpassSpeed + 1.0));
		invLowpass = 1.0 - capacitor->lowpassAmount;
		capacitor->highpassAmount = (((capacitor->highpassAmount * highpassSpeed) + capacitor->highpassChase) / (highpassSpeed + 1.0));
		invHighpass = 1.0 - capacitor->highpassAmount;
		capacitor->wet = (((capacitor->wet * wetSpeed) + capacitor->wetChase) / (wetSpeed + 1.0));
		dry = 1.0 - capacitor->wet;

		capacitor->count++;
		if (capacitor->count > 5) capacitor->count = 0;
		switch (capacitor->count) {
			case 0:
				capacitor->iirHighpassAL = (capacitor->iirHighpassAL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassAL;
				capacitor->iirLowpassAL = (capacitor->iirLowpassAL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassAL;
				capacitor->iirHighpassBL = (capacitor->iirHighpassBL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassBL;
				capacitor->iirLowpassBL = (capacitor->iirLowpassBL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassBL;
				capacitor->iirHighpassDL = (capacitor->iirHighpassDL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassDL;
				capacitor->iirLowpassDL = (capacitor->iirLowpassDL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassDL;
				capacitor->iirHighpassAR = (capacitor->iirHighpassAR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassAR;
				capacitor->iirLowpassAR = (capacitor->iirLowpassAR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassAR;
				capacitor->iirHighpassBR = (capacitor->iirHighpassBR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassBR;
				capacitor->iirLowpassBR = (capacitor->iirLowpassBR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassBR;
				capacitor->iirHighpassDR = (capacitor->iirHighpassDR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassDR;
				capacitor->iirLowpassDR = (capacitor->iirLowpassDR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassDR;
				break;
			case 1:
				capacitor->iirHighpassAL = (capacitor->iirHighpassAL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassAL;
				capacitor->iirLowpassAL = (capacitor->iirLowpassAL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassAL;
				capacitor->iirHighpassCL = (capacitor->iirHighpassCL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassCL;
				capacitor->iirLowpassCL = (capacitor->iirLowpassCL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassCL;
				capacitor->iirHighpassEL = (capacitor->iirHighpassEL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassEL;
				capacitor->iirLowpassEL = (capacitor->iirLowpassEL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassEL;
				capacitor->iirHighpassAR = (capacitor->iirHighpassAR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassAR;
				capacitor->iirLowpassAR = (capacitor->iirLowpassAR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassAR;
				capacitor->iirHighpassCR = (capacitor->iirHighpassCR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassCR;
				capacitor->iirLowpassCR = (capacitor->iirLowpassCR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassCR;
				capacitor->iirHighpassER = (capacitor->iirHighpassER * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassER;
				capacitor->iirLowpassER = (capacitor->iirLowpassER * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassER;
				break;
			case 2:
				capacitor->iirHighpassAL = (capacitor->iirHighpassAL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassAL;
				capacitor->iirLowpassAL = (capacitor->iirLowpassAL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassAL;
				capacitor->iirHighpassBL = (capacitor->iirHighpassBL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassBL;
				capacitor->iirLowpassBL = (capacitor->iirLowpassBL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassBL;
				capacitor->iirHighpassFL = (capacitor->iirHighpassFL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassFL;
				capacitor->iirLowpassFL = (capacitor->iirLowpassFL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassFL;
				capacitor->iirHighpassAR = (capacitor->iirHighpassAR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassAR;
				capacitor->iirLowpassAR = (capacitor->iirLowpassAR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassAR;
				capacitor->iirHighpassBR = (capacitor->iirHighpassBR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassBR;
				capacitor->iirLowpassBR = (capacitor->iirLowpassBR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassBR;
				capacitor->iirHighpassFR = (capacitor->iirHighpassFR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassFR;
				capacitor->iirLowpassFR = (capacitor->iirLowpassFR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassFR;
				break;
			case 3:
				capacitor->iirHighpassAL = (capacitor->iirHighpassAL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassAL;
				capacitor->iirLowpassAL = (capacitor->iirLowpassAL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassAL;
				capacitor->iirHighpassCL = (capacitor->iirHighpassCL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassCL;
				capacitor->iirLowpassCL = (capacitor->iirLowpassCL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassCL;
				capacitor->iirHighpassDL = (capacitor->iirHighpassDL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassDL;
				capacitor->iirLowpassDL = (capacitor->iirLowpassDL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassDL;
				capacitor->iirHighpassAR = (capacitor->iirHighpassAR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassAR;
				capacitor->iirLowpassAR = (capacitor->iirLowpassAR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassAR;
				capacitor->iirHighpassCR = (capacitor->iirHighpassCR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassCR;
				capacitor->iirLowpassCR = (capacitor->iirLowpassCR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassCR;
				capacitor->iirHighpassDR = (capacitor->iirHighpassDR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassDR;
				capacitor->iirLowpassDR = (capacitor->iirLowpassDR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassDR;
				break;
			case 4:
				capacitor->iirHighpassAL = (capacitor->iirHighpassAL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassAL;
				capacitor->iirLowpassAL = (capacitor->iirLowpassAL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassAL;
				capacitor->iirHighpassBL = (capacitor->iirHighpassBL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassBL;
				capacitor->iirLowpassBL = (capacitor->iirLowpassBL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassBL;
				capacitor->iirHighpassEL = (capacitor->iirHighpassEL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassEL;
				capacitor->iirLowpassEL = (capacitor->iirLowpassEL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassEL;
				capacitor->iirHighpassAR = (capacitor->iirHighpassAR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassAR;
				capacitor->iirLowpassAR = (capacitor->iirLowpassAR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassAR;
				capacitor->iirHighpassBR = (capacitor->iirHighpassBR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassBR;
				capacitor->iirLowpassBR = (capacitor->iirLowpassBR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassBR;
				capacitor->iirHighpassER = (capacitor->iirHighpassER * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassER;
				capacitor->iirLowpassER = (capacitor->iirLowpassER * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassER;
				break;
			case 5:
				capacitor->iirHighpassAL = (capacitor->iirHighpassAL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassAL;
				capacitor->iirLowpassAL = (capacitor->iirLowpassAL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassAL;
				capacitor->iirHighpassCL = (capacitor->iirHighpassCL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassCL;
				capacitor->iirLowpassCL = (capacitor->iirLowpassCL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassCL;
				capacitor->iirHighpassFL = (capacitor->iirHighpassFL * invHighpass) + (inputSampleL * capacitor->highpassAmount);
				inputSampleL -= capacitor->iirHighpassFL;
				capacitor->iirLowpassFL = (capacitor->iirLowpassFL * invLowpass) + (inputSampleL * capacitor->lowpassAmount);
				inputSampleL = capacitor->iirLowpassFL;
				capacitor->iirHighpassAR = (capacitor->iirHighpassAR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassAR;
				capacitor->iirLowpassAR = (capacitor->iirLowpassAR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassAR;
				capacitor->iirHighpassCR = (capacitor->iirHighpassCR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassCR;
				capacitor->iirLowpassCR = (capacitor->iirLowpassCR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassCR;
				capacitor->iirHighpassFR = (capacitor->iirHighpassFR * invHighpass) + (inputSampleR * capacitor->highpassAmount);
				inputSampleR -= capacitor->iirHighpassFR;
				capacitor->iirLowpassFR = (capacitor->iirLowpassFR * invLowpass) + (inputSampleR * capacitor->lowpassAmount);
				inputSampleR = capacitor->iirLowpassFR;
				break;
		}
		// Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
		// steepens the filter after minimizing artifacts.

		inputSampleL = (drySampleL * dry) + (inputSampleL * capacitor->wet);
		inputSampleR = (drySampleR * dry) + (inputSampleR * capacitor->wet);

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		capacitor->fpdL ^= capacitor->fpdL << 13;
		capacitor->fpdL ^= capacitor->fpdL >> 17;
		capacitor->fpdL ^= capacitor->fpdL << 5;
		inputSampleL += (((double) capacitor->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		capacitor->fpdR ^= capacitor->fpdR << 13;
		capacitor->fpdR ^= capacitor->fpdR >> 17;
		capacitor->fpdR ^= capacitor->fpdR << 5;
		inputSampleR += (((double) capacitor->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CAPACITOR_URI,
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
