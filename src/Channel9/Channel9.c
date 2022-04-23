#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CHANNEL9_URI "https://hannesbraun.net/ns/lv2/airwindows/channel9"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CONSOLE_TYPE = 4,
	DRIVE = 5,
	OUTPUT = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* consoletype;
	const float* drive;
	const float* outputGain;

	uint32_t fpdL;
	uint32_t fpdR;
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
	double biquadA[15];
	double biquadB[15];
	double iirAmount;
	double threshold;
	double cutoff;
	bool flip;
} Channel9;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Channel9* channel9 = (Channel9*) calloc(1, sizeof(Channel9));
	channel9->sampleRate = rate;
	return (LV2_Handle) channel9;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Channel9* channel9 = (Channel9*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			channel9->input[0] = (const float*) data;
			break;
		case INPUT_R:
			channel9->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			channel9->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			channel9->output[1] = (float*) data;
			break;
		case CONSOLE_TYPE:
			channel9->consoletype = (const float*) data;
			break;
		case DRIVE:
			channel9->drive = (const float*) data;
			break;
		case OUTPUT:
			channel9->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Channel9* channel9 = (Channel9*) instance;

	for (int x = 0; x < 15; x++) {
		channel9->biquadA[x] = 0.0;
		channel9->biquadB[x] = 0.0;
	}
	channel9->fpdL = 1.0;
	while (channel9->fpdL < 16386) channel9->fpdL = rand() * UINT32_MAX;
	channel9->fpdR = 1.0;
	while (channel9->fpdR < 16386) channel9->fpdR = rand() * UINT32_MAX;
	channel9->iirSampleLA = 0.0;
	channel9->iirSampleRA = 0.0;
	channel9->iirSampleLB = 0.0;
	channel9->iirSampleRB = 0.0;
	channel9->lastSampleAL = 0.0;
	channel9->lastSampleBL = 0.0;
	channel9->lastSampleCL = 0.0;
	channel9->lastSampleAR = 0.0;
	channel9->lastSampleBR = 0.0;
	channel9->lastSampleCR = 0.0;
	channel9->flip = false;
	channel9->iirAmount = 0.005832;
	channel9->threshold = 0.33362176; //instantiating with Neve values
	channel9->cutoff = 28811.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Channel9* channel9 = (Channel9*) instance;

	const float* in1 = channel9->input[0];
	const float* in2 = channel9->input[1];
	float* out1 = channel9->output[0];
	float* out2 = channel9->output[1];

	float consoletype = *channel9->consoletype;
	switch ((int)consoletype) {
		case 1:
			channel9->iirAmount = 0.004096;
			channel9->threshold = 0.59969536;
			channel9->cutoff = 27216.0;
			break; //API
		case 2:
			channel9->iirAmount = 0.004913;
			channel9->threshold = 0.84934656;
			channel9->cutoff = 23011.0;
			break; //SSL
		case 3:
			channel9->iirAmount = 0.009216;
			channel9->threshold = 0.149;
			channel9->cutoff = 18544.0;
			break; //Teac
		case 4:
		case 5:
			channel9->iirAmount = 0.011449;
			channel9->threshold = 0.092;
			channel9->cutoff = 19748.0;
			break; //Mackie
		case 0:
		default:
			channel9->iirAmount = 0.005832;
			channel9->threshold = 0.33362176;
			channel9->cutoff = 28811.0;
			break; //Neve
	}
	float output = *channel9->outputGain;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= channel9->sampleRate;
	double localiirAmount = channel9->iirAmount / overallscale;
	double localthreshold = channel9->threshold; //we've learned not to try and adjust threshold for sample rate
	double density = *channel9->drive / 100.0; //0-2
	double phattity = density - 1.0;
	if (density > 1.0) density = 1.0; //max out at full wet for Spiral aspect
	if (phattity < 0.0) phattity = 0.0; //
	double nonLin = 5.0 - density; //number is smaller for more intense, larger for more subtle
	channel9->biquadB[0] = channel9->biquadA[0] = channel9->cutoff / channel9->sampleRate;
	channel9->biquadA[1] = 1.618033988749894848204586;
	channel9->biquadB[1] = 0.618033988749894848204586;

	double K = tan(M_PI * channel9->biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / channel9->biquadA[1] + K * K);
	channel9->biquadA[2] = K * K * norm;
	channel9->biquadA[3] = 2.0 * channel9->biquadA[2];
	channel9->biquadA[4] = channel9->biquadA[2];
	channel9->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	channel9->biquadA[6] = (1.0 - K / channel9->biquadA[1] + K * K) * norm;

	K = tan(M_PI * channel9->biquadA[0]);
	norm = 1.0 / (1.0 + K / channel9->biquadB[1] + K * K);
	channel9->biquadB[2] = K * K * norm;
	channel9->biquadB[3] = 2.0 * channel9->biquadB[2];
	channel9->biquadB[4] = channel9->biquadB[2];
	channel9->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	channel9->biquadB[6] = (1.0 - K / channel9->biquadB[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = channel9->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = channel9->fpdR * 1.18e-17;
		double tempSample;

		if (channel9->biquadA[0] < 0.49999) {
			tempSample = channel9->biquadA[2] * inputSampleL + channel9->biquadA[3] * channel9->biquadA[7] + channel9->biquadA[4] * channel9->biquadA[8] - channel9->biquadA[5] * channel9->biquadA[9] - channel9->biquadA[6] * channel9->biquadA[10];
			channel9->biquadA[8] = channel9->biquadA[7];
			channel9->biquadA[7] = inputSampleL;
			if (fabs(tempSample) < 1.18e-37) tempSample = 0.0;
			inputSampleL = tempSample;
			channel9->biquadA[10] = channel9->biquadA[9];
			channel9->biquadA[9] = inputSampleL; //DF1 left
			tempSample = channel9->biquadA[2] * inputSampleR + channel9->biquadA[3] * channel9->biquadA[11] + channel9->biquadA[4] * channel9->biquadA[12] - channel9->biquadA[5] * channel9->biquadA[13] - channel9->biquadA[6] * channel9->biquadA[14];
			channel9->biquadA[12] = channel9->biquadA[11];
			channel9->biquadA[11] = inputSampleR;
			if (fabs(tempSample) < 1.18e-37) tempSample = 0.0;
			inputSampleR = tempSample;
			channel9->biquadA[14] = channel9->biquadA[13];
			channel9->biquadA[13] = inputSampleR; //DF1 right
		}

		double dielectricScaleL = fabs(2.0 - ((inputSampleL + nonLin) / nonLin));
		double dielectricScaleR = fabs(2.0 - ((inputSampleR + nonLin) / nonLin));

		if (channel9->flip) {
			if (fabs(channel9->iirSampleLA) < 1.18e-37) channel9->iirSampleLA = 0.0;
			channel9->iirSampleLA = (channel9->iirSampleLA * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
			inputSampleL = inputSampleL - channel9->iirSampleLA;
			if (fabs(channel9->iirSampleRA) < 1.18e-37) channel9->iirSampleRA = 0.0;
			channel9->iirSampleRA = (channel9->iirSampleRA * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
			inputSampleR = inputSampleR - channel9->iirSampleRA;
		} else {
			if (fabs(channel9->iirSampleLB) < 1.18e-37) channel9->iirSampleLB = 0.0;
			channel9->iirSampleLB = (channel9->iirSampleLB * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
			inputSampleL = inputSampleL - channel9->iirSampleLB;
			if (fabs(channel9->iirSampleRB) < 1.18e-37) channel9->iirSampleRB = 0.0;
			channel9->iirSampleRB = (channel9->iirSampleRB * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
			inputSampleR = inputSampleR - channel9->iirSampleRB;
		}
		//highpass section
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		double phatSampleL = sin(inputSampleL * 1.57079633);
		inputSampleL *= 1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version

		double distSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));

		inputSampleL = distSampleL; //purest form is full Spiral
		if (density < 1.0) inputSampleL = (drySampleL * (1 - density)) + (distSampleL * density); //fade Spiral aspect
		if (phattity > 0.0) inputSampleL = (inputSampleL * (1 - phattity)) + (phatSampleL * phattity); //apply original Density on top

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		double phatSampleR = sin(inputSampleR * 1.57079633);
		inputSampleR *= 1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version

		double distSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));

		inputSampleR = distSampleR; //purest form is full Spiral
		if (density < 1.0) inputSampleR = (drySampleR * (1 - density)) + (distSampleR * density); //fade Spiral aspect
		if (phattity > 0.0) inputSampleR = (inputSampleR * (1 - phattity)) + (phatSampleR * phattity); //apply original Density on top

		//begin L
		double clamp = (channel9->lastSampleBL - channel9->lastSampleCL) * 0.381966011250105;
		clamp -= (channel9->lastSampleAL - channel9->lastSampleBL) * 0.6180339887498948482045;
		clamp += inputSampleL - channel9->lastSampleAL; //regular slew clamping added

		channel9->lastSampleCL = channel9->lastSampleBL;
		channel9->lastSampleBL = channel9->lastSampleAL;
		channel9->lastSampleAL = inputSampleL; //now our output relates off lastSampleB

		if (clamp > localthreshold)
			inputSampleL = channel9->lastSampleBL + localthreshold;
		if (-clamp > localthreshold)
			inputSampleL = channel9->lastSampleBL - localthreshold;

		channel9->lastSampleAL = (channel9->lastSampleAL * 0.381966011250105) + (inputSampleL * 0.6180339887498948482045); //split the difference between raw and smoothed for buffer
		//end L

		//begin R
		clamp = (channel9->lastSampleBR - channel9->lastSampleCR) * 0.381966011250105;
		clamp -= (channel9->lastSampleAR - channel9->lastSampleBR) * 0.6180339887498948482045;
		clamp += inputSampleR - channel9->lastSampleAR; //regular slew clamping added

		channel9->lastSampleCR = channel9->lastSampleBR;
		channel9->lastSampleBR = channel9->lastSampleAR;
		channel9->lastSampleAR = inputSampleR; //now our output relates off lastSampleB

		if (clamp > localthreshold)
			inputSampleR = channel9->lastSampleBR + localthreshold;
		if (-clamp > localthreshold)
			inputSampleR = channel9->lastSampleBR - localthreshold;

		channel9->lastSampleAR = (channel9->lastSampleAR * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045); //split the difference between raw and smoothed for buffer
		//end R

		channel9->flip = !channel9->flip;

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}

		if (channel9->biquadB[0] < 0.49999) {
			tempSample = channel9->biquadB[2] * inputSampleL + channel9->biquadB[3] * channel9->biquadB[7] + channel9->biquadB[4] * channel9->biquadB[8] - channel9->biquadB[5] * channel9->biquadB[9] - channel9->biquadB[6] * channel9->biquadB[10];
			channel9->biquadB[8] = channel9->biquadB[7];
			channel9->biquadB[7] = inputSampleL;
			if (fabs(tempSample) < 1.18e-37) tempSample = 0.0;
			inputSampleL = tempSample;
			channel9->biquadB[10] = channel9->biquadB[9];
			channel9->biquadB[9] = inputSampleL; //DF1 left
			tempSample = channel9->biquadB[2] * inputSampleR + channel9->biquadB[3] * channel9->biquadB[11] + channel9->biquadB[4] * channel9->biquadB[12] - channel9->biquadB[5] * channel9->biquadB[13] - channel9->biquadB[6] * channel9->biquadB[14];
			channel9->biquadB[12] = channel9->biquadB[11];
			channel9->biquadB[11] = inputSampleR;
			if (fabs(tempSample) < 1.18e-37) tempSample = 0.0;
			inputSampleR = tempSample;
			channel9->biquadB[14] = channel9->biquadB[13];
			channel9->biquadB[13] = inputSampleR; //DF1 right
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		channel9->fpdL ^= channel9->fpdL << 13;
		channel9->fpdL ^= channel9->fpdL >> 17;
		channel9->fpdL ^= channel9->fpdL << 5;
		inputSampleL += (((double)channel9->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		channel9->fpdR ^= channel9->fpdR << 13;
		channel9->fpdR ^= channel9->fpdR >> 17;
		channel9->fpdR ^= channel9->fpdR << 5;
		inputSampleR += (((double)channel9->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CHANNEL9_URI,
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
