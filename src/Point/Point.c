#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define POINT_URI "https://hannesbraun.net/ns/lv2/airwindows/point"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT_TRIM = 4,
	POINT = 5,
	REACTION_SPEED = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputTrim;
	const float* point;
	const float* reactionSpeed;

	uint32_t fpdL;
	uint32_t fpdR;
	bool fpFlip;
	// default stuff
	double nibAL;
	double nobAL;
	double nibBL;
	double nobBL;
	double nibAR;
	double nobAR;
	double nibBR;
	double nobBR;
} Point;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Point* point = (Point*) calloc(1, sizeof(Point));
	point->sampleRate = rate;
	return (LV2_Handle) point;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Point* point = (Point*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			point->input[0] = (const float*) data;
			break;
		case INPUT_R:
			point->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			point->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			point->output[1] = (float*) data;
			break;
		case INPUT_TRIM:
			point->inputTrim = (const float*) data;
			break;
		case POINT:
			point->point = (const float*) data;
			break;
		case REACTION_SPEED:
			point->reactionSpeed = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Point* point = (Point*) instance;
	point->nibAL = 0.0;
	point->nobAL = 0.0;
	point->nibBL = 0.0;
	point->nobBL = 0.0;
	point->nibAR = 0.0;
	point->nobAR = 0.0;
	point->nibBR = 0.0;
	point->nobBR = 0.0;

	point->fpdL = 1.0;
	while (point->fpdL < 16386) point->fpdL = rand() * UINT32_MAX;
	point->fpdR = 1.0;
	while (point->fpdR < 16386) point->fpdR = rand() * UINT32_MAX;
	point->fpFlip = true;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Point* point = (Point*) instance;

	const float* in1 = point->input[0];
	const float* in2 = point->input[1];
	float* out1 = point->output[0];
	float* out2 = point->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= point->sampleRate;

	double gaintrim = pow(10.0, *point->inputTrim / 20);
	double nibDiv = 1 / pow(*point->reactionSpeed + 0.2, 7);
	nibDiv /= overallscale;
	double nobDiv;
	const float pointParam = *point->point;
	if (pointParam > 0) nobDiv = nibDiv / (1.001 - pointParam);
	else nobDiv = nibDiv * (1.001 - pow(pointParam * 0.75, 2));
	double nibnobFactor = 0.0; // start with the fallthrough value, why not
	double absolute;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = point->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = point->fpdR * 1.18e-17;

		inputSampleL *= gaintrim;
		absolute = fabs(inputSampleL);
		if (point->fpFlip) {
			point->nibAL = point->nibAL + (absolute / nibDiv);
			point->nibAL = point->nibAL / (1 + (1 / nibDiv));
			point->nobAL = point->nobAL + (absolute / nobDiv);
			point->nobAL = point->nobAL / (1 + (1 / nobDiv));
			if (point->nobAL > 0) {
				nibnobFactor = point->nibAL / point->nobAL;
			}
		} else {
			point->nibBL = point->nibBL + (absolute / nibDiv);
			point->nibBL = point->nibBL / (1 + (1 / nibDiv));
			point->nobBL = point->nobBL + (absolute / nobDiv);
			point->nobBL = point->nobBL / (1 + (1 / nobDiv));
			if (point->nobBL > 0) {
				nibnobFactor = point->nibBL / point->nobBL;
			}
		}
		inputSampleL *= nibnobFactor;

		inputSampleR *= gaintrim;
		absolute = fabs(inputSampleR);
		if (point->fpFlip) {
			point->nibAR = point->nibAR + (absolute / nibDiv);
			point->nibAR = point->nibAR / (1 + (1 / nibDiv));
			point->nobAR = point->nobAR + (absolute / nobDiv);
			point->nobAR = point->nobAR / (1 + (1 / nobDiv));
			if (point->nobAR > 0) {
				nibnobFactor = point->nibAR / point->nobAR;
			}
		} else {
			point->nibBR = point->nibBR + (absolute / nibDiv);
			point->nibBR = point->nibBR / (1 + (1 / nibDiv));
			point->nobBR = point->nobBR + (absolute / nobDiv);
			point->nobBR = point->nobBR / (1 + (1 / nobDiv));
			if (point->nobBR > 0) {
				nibnobFactor = point->nibBR / point->nobBR;
			}
		}
		inputSampleR *= nibnobFactor;
		point->fpFlip = !point->fpFlip;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		point->fpdL ^= point->fpdL << 13;
		point->fpdL ^= point->fpdL >> 17;
		point->fpdL ^= point->fpdL << 5;
		inputSampleL += (((double) point->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		point->fpdR ^= point->fpdR << 13;
		point->fpdR ^= point->fpdR >> 17;
		point->fpdR ^= point->fpdR << 5;
		inputSampleR += (((double) point->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POINT_URI,
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
