#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define SPIRAL2_URI "https://hannesbraun.net/ns/lv2/airwindows/spiral2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4,
	HIGHPASS = 5,
	PRESENCE = 6,
	OUTPUT = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* in;
	const float* highpass;
	const float* presence;
	const float* out;
	const float* dryWet;

	double iirSampleAL;
	double iirSampleBL;
	double prevSampleL;
	uint32_t fpdL;
	uint32_t fpdR;

	double iirSampleAR;
	double iirSampleBR;
	double prevSampleR;
	bool flip;
} Spiral2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Spiral2* spiral2 = (Spiral2*) calloc(1, sizeof(Spiral2));
	spiral2->sampleRate = rate;
	return (LV2_Handle) spiral2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Spiral2* spiral2 = (Spiral2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			spiral2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			spiral2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			spiral2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			spiral2->output[1] = (float*) data;
			break;
		case INPUT:
			spiral2->in = (const float*) data;
			break;
		case HIGHPASS:
			spiral2->highpass = (const float*) data;
			break;
		case PRESENCE:
			spiral2->presence = (const float*) data;
			break;
		case OUTPUT:
			spiral2->out = (const float*) data;
			break;
		case DRY_WET:
			spiral2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Spiral2* spiral2 = (Spiral2*) instance;

	spiral2->iirSampleAL = 0.0;
	spiral2->iirSampleBL = 0.0;
	spiral2->prevSampleL = 0.0;
	spiral2->fpdL = 1.0;
	while (spiral2->fpdL < 16386) spiral2->fpdL = rand() * UINT32_MAX;
	spiral2->fpdR = 1.0;
	while (spiral2->fpdR < 16386) spiral2->fpdR = rand() * UINT32_MAX;

	spiral2->iirSampleAR = 0.0;
	spiral2->iirSampleBR = 0.0;
	spiral2->prevSampleR = 0.0;
	spiral2->flip = true;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Spiral2* spiral2 = (Spiral2*) instance;

	const float* in1 = spiral2->input[0];
	const float* in2 = spiral2->input[1];
	float* out1 = spiral2->output[0];
	float* out2 = spiral2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= spiral2->sampleRate;

	double gain = pow(*spiral2->in * 2.0, 2.0);
	double iirAmount = pow(*spiral2->highpass, 3.0) / overallscale;
	double presence = *spiral2->presence;
	double output = *spiral2->out;
	double wet = *spiral2->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = spiral2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = spiral2->fpdR * 1.18e-17;

		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
			spiral2->prevSampleL *= gain;
			spiral2->prevSampleR *= gain;
		}

		if (spiral2->flip) {
			spiral2->iirSampleAL = (spiral2->iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			spiral2->iirSampleAR = (spiral2->iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleL -= spiral2->iirSampleAL;
			inputSampleR -= spiral2->iirSampleAR;
		} else {
			spiral2->iirSampleBL = (spiral2->iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			spiral2->iirSampleBR = (spiral2->iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleL -= spiral2->iirSampleBL;
			inputSampleR -= spiral2->iirSampleBR;
		}
		// highpass section

		double presenceSampleL = sin(inputSampleL * fabs(spiral2->prevSampleL)) / ((spiral2->prevSampleL == 0.0) ? 1 : fabs(spiral2->prevSampleL));
		double presenceSampleR = sin(inputSampleR * fabs(spiral2->prevSampleR)) / ((spiral2->prevSampleR == 0.0) ? 1 : fabs(spiral2->prevSampleR));
		// change from first Spiral: delay of one sample on the scaling factor.
		inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
		inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
			presenceSampleL *= output;
			presenceSampleR *= output;
		}
		if (presence > 0.0) {
			inputSampleL = (inputSampleL * (1.0 - presence)) + (presenceSampleL * presence);
			inputSampleR = (inputSampleR * (1.0 - presence)) + (presenceSampleR * presence);
		}
		if (wet < 1.0) {
			inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
		}
		// nice little output stage template: if we have another scale of floating point
		// number, we really don't want to meaninglessly multiply that by 1.0.

		spiral2->prevSampleL = drySampleL;
		spiral2->prevSampleR = drySampleR;
		spiral2->flip = !spiral2->flip;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		spiral2->fpdL ^= spiral2->fpdL << 13;
		spiral2->fpdL ^= spiral2->fpdL >> 17;
		spiral2->fpdL ^= spiral2->fpdL << 5;
		inputSampleL += (((double) spiral2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		spiral2->fpdR ^= spiral2->fpdR << 13;
		spiral2->fpdR ^= spiral2->fpdR >> 17;
		spiral2->fpdR ^= spiral2->fpdR << 5;
		inputSampleR += (((double) spiral2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SPIRAL2_URI,
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
