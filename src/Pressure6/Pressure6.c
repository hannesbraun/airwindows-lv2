#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI_2 1.57079632679489661923132169163975144

#define PRESSURE6_URI "https://hannesbraun.net/ns/lv2/airwindows/pressure6"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	COMPRES = 4,
	RATIO = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* compres;
	const float* ratio;

	double muComp;
	double muSpd;

	uint32_t fpdL;
	uint32_t fpdR;
} Pressure6;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Pressure6* pressure6 = (Pressure6*) calloc(1, sizeof(Pressure6));
	pressure6->sampleRate = rate;
	return (LV2_Handle) pressure6;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Pressure6* pressure6 = (Pressure6*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pressure6->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pressure6->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pressure6->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pressure6->output[1] = (float*) data;
			break;
		case COMPRES:
			pressure6->compres = (const float*) data;
			break;
		case RATIO:
			pressure6->ratio = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Pressure6* pressure6 = (Pressure6*) instance;
	pressure6->fpdL = 1.0;
	while (pressure6->fpdL < 16386) pressure6->fpdL = rand() * UINT32_MAX;
	pressure6->fpdR = 1.0;
	while (pressure6->fpdR < 16386) pressure6->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Pressure6* pressure6 = (Pressure6*) instance;

	const float* in1 = pressure6->input[0];
	const float* in2 = pressure6->input[1];
	float* out1 = pressure6->output[0];
	float* out2 = pressure6->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= pressure6->sampleRate;

	const float compres = *pressure6->compres;

	double threshold = 1.0 - pow(compres * 0.9, 3.0);
	double adjSpd = ((compres * 92.0) + 92.0) * overallscale;
	double wet = *pressure6->ratio * 0.9;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pressure6->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pressure6->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double inputSample = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSample) inputSample = fabs(inputSampleR);

		inputSample *= (pressure6->muComp / threshold);
		inputSampleL *= (pressure6->muComp / threshold);
		inputSampleR *= (pressure6->muComp / threshold);
		if (fabs(inputSample) > threshold) {
			pressure6->muComp *= pressure6->muSpd;
			if (threshold / fabs(inputSample) < threshold) pressure6->muComp += threshold * fabs(inputSample);
			else pressure6->muComp -= threshold / fabs(inputSample);
			pressure6->muComp /= pressure6->muSpd;
		} else {
			pressure6->muComp *= (pressure6->muSpd * pressure6->muSpd);
			pressure6->muComp += ((1.1 + threshold) - fabs(inputSample));
			pressure6->muComp /= (pressure6->muSpd * pressure6->muSpd);
		}
		pressure6->muComp = fmax(fmin(pressure6->muComp, 1.0), threshold);
		inputSample *= (pressure6->muComp * pressure6->muComp);
		inputSampleL *= (pressure6->muComp * pressure6->muComp);
		inputSampleR *= (pressure6->muComp * pressure6->muComp);
		pressure6->muSpd = fmax(fmin(((pressure6->muSpd * (pressure6->muSpd - 1.0)) + (fabs(inputSample * adjSpd))) / pressure6->muSpd, adjSpd * 2.0), adjSpd);

		inputSampleL = (drySampleL * (1.0 - (wet * 1.1))) + (sin(fmax(fmin(inputSampleL * wet, M_PI_2), -M_PI_2)) * wet);
		inputSampleR = (drySampleR * (1.0 - (wet * 1.1))) + (sin(fmax(fmin(inputSampleR * wet, M_PI_2), -M_PI_2)) * wet);

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pressure6->fpdL ^= pressure6->fpdL << 13;
		pressure6->fpdL ^= pressure6->fpdL >> 17;
		pressure6->fpdL ^= pressure6->fpdL << 5;
		inputSampleL += (((double) pressure6->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pressure6->fpdR ^= pressure6->fpdR << 13;
		pressure6->fpdR ^= pressure6->fpdR >> 17;
		pressure6->fpdR ^= pressure6->fpdR << 5;
		inputSampleR += (((double) pressure6->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PRESSURE6_URI,
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
