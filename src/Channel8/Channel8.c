#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CHANNEL8_URI "https://hannesbraun.net/ns/lv2/airwindows/channel8"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CONSOLE_TYPE = 4,
	DRIVE = 5,
	OUTPUT = 6,
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* consoletype;
	const float* drive;
	const float* outputGain;

	uint32_t fpd;
	//default stuff
	double iirSampleLA;
	double iirSampleRA;
	double iirSampleLB;
	double iirSampleRB;
	double lastSampleAL;
	double lastSampleBL;
	double lastSampleCL;
	double lastSampleAR;
	double lastSampleBR;
	double lastSampleCR;
	double iirAmount;
	double threshold;
	bool flip;
} Channel8;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Channel8* channel8 = (Channel8*) calloc(1, sizeof(Channel8));
	channel8->sampleRate = rate;
	return (LV2_Handle) channel8;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Channel8* channel8 = (Channel8*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			channel8->input[0] = (const float*) data;
			break;
		case INPUT_R:
			channel8->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			channel8->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			channel8->output[1] = (float*) data;
			break;
		case CONSOLE_TYPE:
			channel8->consoletype = (const float*) data;
			break;
		case DRIVE:
			channel8->drive = (const float*) data;
			break;
		case OUTPUT:
			channel8->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Channel8* channel8 = (Channel8*) instance;

	channel8->fpd = 17;
	channel8->iirSampleLA = 0.0;
	channel8->iirSampleRA = 0.0;
	channel8->iirSampleLB = 0.0;
	channel8->iirSampleRB = 0.0;
	channel8->lastSampleAL = 0.0;
	channel8->lastSampleBL = 0.0;
	channel8->lastSampleCL = 0.0;
	channel8->lastSampleAR = 0.0;
	channel8->lastSampleBR = 0.0;
	channel8->lastSampleCR = 0.0;
	channel8->flip = false;
	channel8->iirAmount = 0.005832;
	channel8->threshold = 0.33362176; //instantiating with Neve values
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Channel8* channel8 = (Channel8*) instance;

	const float* in1 = channel8->input[0];
	const float* in2 = channel8->input[1];
	float* out1 = channel8->output[0];
	float* out2 = channel8->output[1];

	float consoletype = *channel8->consoletype;
	switch ((int)consoletype) {
		case 1:
			channel8->iirAmount = 0.004096;
			channel8->threshold = 0.59969536;
			break; //API
		case 2:
		case 3:
			channel8->iirAmount = 0.004913;
			channel8->threshold = 0.84934656;
			break; //SSL
		case 0:
		default:
			channel8->iirAmount = 0.005832;
			channel8->threshold = 0.33362176;
			break; //Neve
	}

	float output = *channel8->outputGain;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= channel8->sampleRate;
	double localiirAmount = channel8->iirAmount / overallscale;
	double localthreshold = channel8->threshold; //we've learned not to try and adjust threshold for sample rate
	double density = *channel8->drive / 100.0; //0-2
	double phattity = density - 1.0;
	if (density > 1.0) density = 1.0; //max out at full wet for Spiral aspect
	if (phattity < 0.0) phattity = 0.0;
	double nonLin = 5.0 - density; //number is smaller for more intense, larger for more subtle


	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabsl(inputSampleL) < 1.18e-37) inputSampleL = channel8->fpd * 1.18e-37;
		if (fabsl(inputSampleR) < 1.18e-37) inputSampleR = channel8->fpd * 1.18e-37;

		double dielectricScaleL = fabsl(2.0 - ((inputSampleL + nonLin) / nonLin));
		double dielectricScaleR = fabsl(2.0 - ((inputSampleR + nonLin) / nonLin));

		if (channel8->flip) {
			channel8->iirSampleLA = (channel8->iirSampleLA * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
			inputSampleL = inputSampleL - channel8->iirSampleLA;
			channel8->iirSampleRA = (channel8->iirSampleRA * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
			inputSampleR = inputSampleR - channel8->iirSampleRA;
		} else {
			channel8->iirSampleLB = (channel8->iirSampleLB * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
			inputSampleL = inputSampleL - channel8->iirSampleLB;
			channel8->iirSampleRB = (channel8->iirSampleRB * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
			inputSampleR = inputSampleR - channel8->iirSampleRB;
		}
		//highpass section
		long double drySampleL = inputSampleL;
		long double drySampleR = inputSampleR;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		long double phatSampleL = sin(inputSampleL * 1.57079633);
		inputSampleL *= 1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version

		long double distSampleL = sin(inputSampleL * fabsl(inputSampleL)) / ((fabsl(inputSampleL) == 0.0) ? 1 : fabsl(inputSampleL));

		inputSampleL = distSampleL; //purest form is full Spiral
		if (density < 1.0) inputSampleL = (drySampleL * (1 - density)) + (distSampleL * density); //fade Spiral aspect
		if (phattity > 0.0) inputSampleL = (inputSampleL * (1 - phattity)) + (phatSampleL * phattity); //apply original Density on top

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		long double phatSampleR = sin(inputSampleR * 1.57079633);
		inputSampleR *= 1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version

		long double distSampleR = sin(inputSampleR * fabsl(inputSampleR)) / ((fabsl(inputSampleR) == 0.0) ? 1 : fabsl(inputSampleR));

		inputSampleR = distSampleR; //purest form is full Spiral
		if (density < 1.0) inputSampleR = (drySampleR * (1 - density)) + (distSampleR * density); //fade Spiral aspect
		if (phattity > 0.0) inputSampleR = (inputSampleR * (1 - phattity)) + (phatSampleR * phattity); //apply original Density on top

		//begin L
		double clamp = (channel8->lastSampleBL - channel8->lastSampleCL) * 0.381966011250105;
		clamp -= (channel8->lastSampleAL - channel8->lastSampleBL) * 0.6180339887498948482045;
		clamp += inputSampleL - channel8->lastSampleAL; //regular slew clamping added

		channel8->lastSampleCL = channel8->lastSampleBL;
		channel8->lastSampleBL = channel8->lastSampleAL;
		channel8->lastSampleAL = inputSampleL; //now our output relates off lastSampleB

		if (clamp > localthreshold)
			inputSampleL = channel8->lastSampleBL + localthreshold;
		if (-clamp > localthreshold)
			inputSampleL = channel8->lastSampleBL - localthreshold;

		channel8->lastSampleAL = (channel8->lastSampleAL * 0.381966011250105) + (inputSampleL * 0.6180339887498948482045); //split the difference between raw and smoothed for buffer
		//end L

		//begin R
		clamp = (channel8->lastSampleBR - channel8->lastSampleCR) * 0.381966011250105;
		clamp -= (channel8->lastSampleAR - channel8->lastSampleBR) * 0.6180339887498948482045;
		clamp += inputSampleR - channel8->lastSampleAR; //regular slew clamping added

		channel8->lastSampleCR = channel8->lastSampleBR;
		channel8->lastSampleBR = channel8->lastSampleAR;
		channel8->lastSampleAR = inputSampleR; //now our output relates off lastSampleB

		if (clamp > localthreshold)
			inputSampleR = channel8->lastSampleBR + localthreshold;
		if (-clamp > localthreshold)
			inputSampleR = channel8->lastSampleBR - localthreshold;

		channel8->lastSampleAR = (channel8->lastSampleAR * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045); //split the difference between raw and smoothed for buffer
		//end R

		channel8->flip = !channel8->flip;

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		channel8->fpd ^= channel8->fpd << 13;
		channel8->fpd ^= channel8->fpd >> 17;
		channel8->fpd ^= channel8->fpd << 5;
		inputSampleL += (((double)channel8->fpd - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		channel8->fpd ^= channel8->fpd << 13;
		channel8->fpd ^= channel8->fpd >> 17;
		channel8->fpd ^= channel8->fpd << 5;
		inputSampleR += (((double)channel8->fpd - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CHANNEL8_URI,
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
