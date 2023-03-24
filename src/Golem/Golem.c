#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define GOLEM_URI "https://hannesbraun.net/ns/lv2/airwindows/golem"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BALANCE = 4,
	OFFSET = 5,
	PHASE = 6
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* balance;
	const float* offset;
	const float* phase;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double p[4099];
	int count;
} Golem;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Golem* golem = (Golem*) calloc(1, sizeof(Golem));
	return (LV2_Handle) golem;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Golem* golem = (Golem*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			golem->input[0] = (const float*) data;
			break;
		case INPUT_R:
			golem->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			golem->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			golem->output[1] = (float*) data;
			break;
		case BALANCE:
			golem->balance = (const float*) data;
			break;
		case OFFSET:
			golem->offset = (const float*) data;
			break;
		case PHASE:
			golem->phase = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Golem* golem = (Golem*) instance;

	for (int count = 0; count < 4098; count++) {
		golem->p[count] = 0.0;
	}
	golem->count = 0;
	golem->fpdL = 1.0;
	while (golem->fpdL < 16386) golem->fpdL = rand() * UINT32_MAX;
	golem->fpdR = 1.0;
	while (golem->fpdR < 16386) golem->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Golem* golem = (Golem*) instance;

	const float* in1 = golem->input[0];
	const float* in2 = golem->input[1];
	float* out1 = golem->output[0];
	float* out2 = golem->output[1];

	int phase = ((int) *golem->phase) + 1;
	if (phase > 6) {
		phase = 6;
	}
	double balance = *golem->balance / 2.0;
	double gainL = 0.5 - balance;
	double gainR = 0.5 + balance;
	double range = 30.0;
	if (phase == 3) range = 700.0;
	if (phase == 4) range = 700.0;
	double offset = pow(*golem->offset, 5) * range;
	if (phase > 4) offset = 0.0;
	if (phase > 5) {
		gainL = 0.5;
		gainR = 0.5;
	}
	int near = (int) floor(fabs(offset));
	double farLevel = fabs(offset) - near;
	int far = near + 1;
	double nearLevel = 1.0 - farLevel;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = golem->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = golem->fpdR * 1.18e-17;
		// assign working variables

		if (phase == 2) inputSampleL = -inputSampleL;
		if (phase == 4) inputSampleL = -inputSampleL;

		inputSampleL *= gainL;
		inputSampleR *= gainR;

		if (golem->count < 1 || golem->count > 2048) {
			golem->count = 2048;
		}

		if (offset > 0) {
			golem->p[golem->count + 2048] = golem->p[golem->count] = inputSampleL;
			inputSampleL = golem->p[golem->count + near] * nearLevel;
			inputSampleL += golem->p[golem->count + far] * farLevel;

			// consider adding third sample just to bring out superhighs subtly, like old interpolation hacks
			// or third and fifth samples, ditto
		}

		if (offset < 0) {
			golem->p[golem->count + 2048] = golem->p[golem->count] = inputSampleR;
			inputSampleR = golem->p[golem->count + near] * nearLevel;
			inputSampleR += golem->p[golem->count + far] * farLevel;
		}

		golem->count -= 1;

		inputSampleL = inputSampleL + inputSampleR;
		inputSampleR = inputSampleL;
		// the output is totally mono

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		golem->fpdL ^= golem->fpdL << 13;
		golem->fpdL ^= golem->fpdL >> 17;
		golem->fpdL ^= golem->fpdL << 5;
		inputSampleL += (((double) golem->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		golem->fpdR ^= golem->fpdR << 13;
		golem->fpdR ^= golem->fpdR >> 17;
		golem->fpdR ^= golem->fpdR << 5;
		inputSampleR += (((double) golem->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	GOLEM_URI,
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
