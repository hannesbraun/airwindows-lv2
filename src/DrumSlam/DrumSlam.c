#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define DRUMSLAM_URI "https://hannesbraun.net/ns/lv2/airwindows/drumslam"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	OUTPUT = 5,
	DRY_WET = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* outputParam;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double lastSampleL;

	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double iirSampleGR;
	double iirSampleHR;
	double lastSampleR;
	bool fpFlip;
} DrumSlam;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	DrumSlam* drumSlam = (DrumSlam*) calloc(1, sizeof(DrumSlam));
	drumSlam->sampleRate = rate;
	return (LV2_Handle) drumSlam;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	DrumSlam* drumSlam = (DrumSlam*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			drumSlam->input[0] = (const float*) data;
			break;
		case INPUT_R:
			drumSlam->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			drumSlam->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			drumSlam->output[1] = (float*) data;
			break;
		case DRIVE:
			drumSlam->drive = (const float*) data;
			break;
		case OUTPUT:
			drumSlam->outputParam = (const float*) data;
			break;
		case DRY_WET:
			drumSlam->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	DrumSlam* drumSlam = (DrumSlam*) instance;

	drumSlam->iirSampleAL = 0.0;
	drumSlam->iirSampleBL = 0.0;
	drumSlam->iirSampleCL = 0.0;
	drumSlam->iirSampleDL = 0.0;
	drumSlam->iirSampleEL = 0.0;
	drumSlam->iirSampleFL = 0.0;
	drumSlam->iirSampleGL = 0.0;
	drumSlam->iirSampleHL = 0.0;
	drumSlam->lastSampleL = 0.0;

	drumSlam->iirSampleAR = 0.0;
	drumSlam->iirSampleBR = 0.0;
	drumSlam->iirSampleCR = 0.0;
	drumSlam->iirSampleDR = 0.0;
	drumSlam->iirSampleER = 0.0;
	drumSlam->iirSampleFR = 0.0;
	drumSlam->iirSampleGR = 0.0;
	drumSlam->iirSampleHR = 0.0;
	drumSlam->lastSampleR = 0.0;
	drumSlam->fpFlip = true;
	drumSlam->fpdL = 1.0;
	while (drumSlam->fpdL < 16386) drumSlam->fpdL = rand() * UINT32_MAX;
	drumSlam->fpdR = 1.0;
	while (drumSlam->fpdR < 16386) drumSlam->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	DrumSlam* drumSlam = (DrumSlam*) instance;

	const float* in1 = drumSlam->input[0];
	const float* in2 = drumSlam->input[1];
	float* out1 = drumSlam->output[0];
	float* out2 = drumSlam->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= drumSlam->sampleRate;
	double iirAmountL = 0.0819;
	iirAmountL /= overallscale;
	double iirAmountH = 0.377933067;
	iirAmountH /= overallscale;
	double drive = *drumSlam->drive;
	double out = *drumSlam->outputParam;
	double wet = *drumSlam->dryWet;
	// removed extra dry variable

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = drumSlam->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = drumSlam->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		double lowSampleL;
		double lowSampleR;
		double midSampleL;
		double midSampleR;
		double highSampleL;
		double highSampleR;

		inputSampleL *= drive;
		inputSampleR *= drive;

		if (drumSlam->fpFlip) {
			drumSlam->iirSampleAL = (drumSlam->iirSampleAL * (1 - iirAmountL)) + (inputSampleL * iirAmountL);
			drumSlam->iirSampleBL = (drumSlam->iirSampleBL * (1 - iirAmountL)) + (drumSlam->iirSampleAL * iirAmountL);
			lowSampleL = drumSlam->iirSampleBL;

			drumSlam->iirSampleAR = (drumSlam->iirSampleAR * (1 - iirAmountL)) + (inputSampleR * iirAmountL);
			drumSlam->iirSampleBR = (drumSlam->iirSampleBR * (1 - iirAmountL)) + (drumSlam->iirSampleAR * iirAmountL);
			lowSampleR = drumSlam->iirSampleBR;

			drumSlam->iirSampleEL = (drumSlam->iirSampleEL * (1 - iirAmountH)) + (inputSampleL * iirAmountH);
			drumSlam->iirSampleFL = (drumSlam->iirSampleFL * (1 - iirAmountH)) + (drumSlam->iirSampleEL * iirAmountH);
			midSampleL = drumSlam->iirSampleFL - drumSlam->iirSampleBL;

			drumSlam->iirSampleER = (drumSlam->iirSampleER * (1 - iirAmountH)) + (inputSampleR * iirAmountH);
			drumSlam->iirSampleFR = (drumSlam->iirSampleFR * (1 - iirAmountH)) + (drumSlam->iirSampleER * iirAmountH);
			midSampleR = drumSlam->iirSampleFR - drumSlam->iirSampleBR;

			highSampleL = inputSampleL - drumSlam->iirSampleFL;
			highSampleR = inputSampleR - drumSlam->iirSampleFR;
		} else {
			drumSlam->iirSampleCL = (drumSlam->iirSampleCL * (1 - iirAmountL)) + (inputSampleL * iirAmountL);
			drumSlam->iirSampleDL = (drumSlam->iirSampleDL * (1 - iirAmountL)) + (drumSlam->iirSampleCL * iirAmountL);
			lowSampleL = drumSlam->iirSampleDL;

			drumSlam->iirSampleCR = (drumSlam->iirSampleCR * (1 - iirAmountL)) + (inputSampleR * iirAmountL);
			drumSlam->iirSampleDR = (drumSlam->iirSampleDR * (1 - iirAmountL)) + (drumSlam->iirSampleCR * iirAmountL);
			lowSampleR = drumSlam->iirSampleDR;

			drumSlam->iirSampleGL = (drumSlam->iirSampleGL * (1 - iirAmountH)) + (inputSampleL * iirAmountH);
			drumSlam->iirSampleHL = (drumSlam->iirSampleHL * (1 - iirAmountH)) + (drumSlam->iirSampleGL * iirAmountH);
			midSampleL = drumSlam->iirSampleHL - drumSlam->iirSampleDL;

			drumSlam->iirSampleGR = (drumSlam->iirSampleGR * (1 - iirAmountH)) + (inputSampleR * iirAmountH);
			drumSlam->iirSampleHR = (drumSlam->iirSampleHR * (1 - iirAmountH)) + (drumSlam->iirSampleGR * iirAmountH);
			midSampleR = drumSlam->iirSampleHR - drumSlam->iirSampleDR;

			highSampleL = inputSampleL - drumSlam->iirSampleHL;
			highSampleR = inputSampleR - drumSlam->iirSampleHR;
		}
		drumSlam->fpFlip = !drumSlam->fpFlip;

		// generate the tone bands we're using
		if (lowSampleL > 1.0) {
			lowSampleL = 1.0;
		}
		if (lowSampleL < -1.0) {
			lowSampleL = -1.0;
		}
		if (lowSampleR > 1.0) {
			lowSampleR = 1.0;
		}
		if (lowSampleR < -1.0) {
			lowSampleR = -1.0;
		}
		lowSampleL -= (lowSampleL * (fabs(lowSampleL) * 0.448) * (fabs(lowSampleL) * 0.448));
		lowSampleR -= (lowSampleR * (fabs(lowSampleR) * 0.448) * (fabs(lowSampleR) * 0.448));
		lowSampleL *= drive;
		lowSampleR *= drive;

		if (highSampleL > 1.0) {
			highSampleL = 1.0;
		}
		if (highSampleL < -1.0) {
			highSampleL = -1.0;
		}
		if (highSampleR > 1.0) {
			highSampleR = 1.0;
		}
		if (highSampleR < -1.0) {
			highSampleR = -1.0;
		}
		highSampleL -= (highSampleL * (fabs(highSampleL) * 0.599) * (fabs(highSampleL) * 0.599));
		highSampleR -= (highSampleR * (fabs(highSampleR) * 0.599) * (fabs(highSampleR) * 0.599));
		highSampleL *= drive;
		highSampleR *= drive;

		midSampleL = midSampleL * drive;
		midSampleR = midSampleR * drive;

		double skew = (midSampleL - drumSlam->lastSampleL);
		drumSlam->lastSampleL = midSampleL;
		// skew will be direction/angle
		double bridgerectifier = fabs(skew);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skew > 0) skew = bridgerectifier * 3.1415926;
		else skew = -bridgerectifier * 3.1415926;
		// skew is now sined and clamped and then re-amplified again
		skew *= midSampleL;
		// cools off sparkliness and crossover distortion
		skew *= 1.557079633;
		// crank up the gain on this so we can make it sing
		bridgerectifier = fabs(midSampleL);
		bridgerectifier += skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		bridgerectifier *= drive;
		bridgerectifier += skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (midSampleL > 0) {
			midSampleL = bridgerectifier;
		} else {
			midSampleL = -bridgerectifier;
		}
		// blend according to positive and negative controls, left

		skew = (midSampleR - drumSlam->lastSampleR);
		drumSlam->lastSampleR = midSampleR;
		// skew will be direction/angle
		bridgerectifier = fabs(skew);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		// for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skew > 0) skew = bridgerectifier * 3.1415926;
		else skew = -bridgerectifier * 3.1415926;
		// skew is now sined and clamped and then re-amplified again
		skew *= midSampleR;
		// cools off sparkliness and crossover distortion
		skew *= 1.557079633;
		// crank up the gain on this so we can make it sing
		bridgerectifier = fabs(midSampleR);
		bridgerectifier += skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		bridgerectifier *= drive;
		bridgerectifier += skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (midSampleR > 0) {
			midSampleR = bridgerectifier;
		} else {
			midSampleR = -bridgerectifier;
		}
		// blend according to positive and negative controls, right

		inputSampleL = ((lowSampleL + midSampleL + highSampleL) / drive) * out;
		inputSampleR = ((lowSampleR + midSampleR + highSampleR) / drive) * out;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		drumSlam->fpdL ^= drumSlam->fpdL << 13;
		drumSlam->fpdL ^= drumSlam->fpdL >> 17;
		drumSlam->fpdL ^= drumSlam->fpdL << 5;
		inputSampleL += (((double) drumSlam->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		drumSlam->fpdR ^= drumSlam->fpdR << 13;
		drumSlam->fpdR ^= drumSlam->fpdR >> 17;
		drumSlam->fpdR ^= drumSlam->fpdR << 5;
		inputSampleR += (((double) drumSlam->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	DRUMSLAM_URI,
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
