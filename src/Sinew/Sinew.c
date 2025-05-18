#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SINEW_URI "https://hannesbraun.net/ns/lv2/airwindows/sinew"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	SINEW = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* sinew;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double lastSinewL;
	double lastSinewR;
} Sinew;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Sinew* sinew = (Sinew*) calloc(1, sizeof(Sinew));
	sinew->sampleRate = rate;
	return (LV2_Handle) sinew;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Sinew* sinew = (Sinew*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			sinew->input[0] = (const float*) data;
			break;
		case INPUT_R:
			sinew->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			sinew->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			sinew->output[1] = (float*) data;
			break;
		case SINEW:
			sinew->sinew = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Sinew* sinew = (Sinew*) instance;

	sinew->lastSinewL = 0.0;
	sinew->lastSinewR = 0.0;

	sinew->fpdL = 1.0;
	while (sinew->fpdL < 16386) sinew->fpdL = rand() * UINT32_MAX;
	sinew->fpdR = 1.0;
	while (sinew->fpdR < 16386) sinew->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Sinew* sinewInstance = (Sinew*) instance;

	const float* in1 = sinewInstance->input[0];
	const float* in2 = sinewInstance->input[1];
	float* out1 = sinewInstance->output[0];
	float* out2 = sinewInstance->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= sinewInstance->sampleRate;

	double threshSinew = pow((1.0 - *sinewInstance->sinew), 4) / overallscale;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = sinewInstance->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = sinewInstance->fpdR * 1.18e-17;

		double temp = inputSampleL;
		double clamp = inputSampleL - sinewInstance->lastSinewL;
		double sinew = threshSinew * cos(sinewInstance->lastSinewL * sinewInstance->lastSinewL);
		if (clamp > sinew) temp = sinewInstance->lastSinewL + sinew;
		if (-clamp > sinew) temp = sinewInstance->lastSinewL - sinew;
		inputSampleL = sinewInstance->lastSinewL = temp;

		if (sinewInstance->lastSinewL > 1.0) sinewInstance->lastSinewL = 1.0;
		if (sinewInstance->lastSinewL < -1.0) sinewInstance->lastSinewL = -1.0;

		temp = inputSampleR;
		clamp = inputSampleR - sinewInstance->lastSinewR;
		sinew = threshSinew * cos(sinewInstance->lastSinewR * sinewInstance->lastSinewR);
		if (clamp > sinew) temp = sinewInstance->lastSinewR + sinew;
		if (-clamp > sinew) temp = sinewInstance->lastSinewR - sinew;
		inputSampleR = sinewInstance->lastSinewR = temp;

		if (sinewInstance->lastSinewR > 1.0) sinewInstance->lastSinewR = 1.0;
		if (sinewInstance->lastSinewR < -1.0) sinewInstance->lastSinewR = -1.0;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		sinewInstance->fpdL ^= sinewInstance->fpdL << 13;
		sinewInstance->fpdL ^= sinewInstance->fpdL >> 17;
		sinewInstance->fpdL ^= sinewInstance->fpdL << 5;
		inputSampleL += (((double) sinewInstance->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		sinewInstance->fpdR ^= sinewInstance->fpdR << 13;
		sinewInstance->fpdR ^= sinewInstance->fpdR >> 17;
		sinewInstance->fpdR ^= sinewInstance->fpdR << 5;
		inputSampleR += (((double) sinewInstance->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SINEW_URI,
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
