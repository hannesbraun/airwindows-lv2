#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define DRIVE_URI "https://hannesbraun.net/ns/lv2/airwindows/drive"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	HIGHPASS = 5,
	OUT_LEVEL = 6,
	DRY_WET = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* highpass;
	const float* outLevel;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double iirSampleAL;
	double iirSampleBL;
	double iirSampleAR;
	double iirSampleBR;
	bool fpFlip;
} Drive;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Drive* drive = (Drive*) calloc(1, sizeof(Drive));
	drive->sampleRate = rate;
	return (LV2_Handle) drive;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Drive* drive = (Drive*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			drive->input[0] = (const float*) data;
			break;
		case INPUT_R:
			drive->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			drive->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			drive->output[1] = (float*) data;
			break;
		case DRIVE:
			drive->drive = (const float*) data;
			break;
		case HIGHPASS:
			drive->highpass = (const float*) data;
			break;
		case OUT_LEVEL:
			drive->outLevel = (const float*) data;
			break;
		case DRY_WET:
			drive->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Drive* drive = (Drive*) instance;

	drive->iirSampleAL = 0.0;
	drive->iirSampleBL = 0.0;
	drive->iirSampleAR = 0.0;
	drive->iirSampleBR = 0.0;
	drive->fpFlip = true;
	drive->fpdL = 1.0;
	while (drive->fpdL < 16386) drive->fpdL = rand() * UINT32_MAX;
	drive->fpdR = 1.0;
	while (drive->fpdR < 16386) drive->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Drive* drive = (Drive*) instance;

	const float* in1 = drive->input[0];
	const float* in2 = drive->input[1];
	float* out1 = drive->output[0];
	float* out2 = drive->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= drive->sampleRate;

	double driveone = pow(*drive->drive / 50.0, 2);
	double iirAmount = pow(*drive->highpass, 3) / overallscale;
	double output = *drive->outLevel;
	double wet = *drive->dryWet;
	double dry = 1.0 - wet;
	double glitch = 0.60;
	double out;

	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = drive->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = drive->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		if (drive->fpFlip) {
			drive->iirSampleAL = (drive->iirSampleAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= drive->iirSampleAL;
			drive->iirSampleAR = (drive->iirSampleAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= drive->iirSampleAR;
		} else {
			drive->iirSampleBL = (drive->iirSampleBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= drive->iirSampleBL;
			drive->iirSampleBR = (drive->iirSampleBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= drive->iirSampleBR;
		}
		// highpass section
		drive->fpFlip = !drive->fpFlip;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;

		out = driveone;
		while (out > glitch) {
			out -= glitch;
			inputSampleL -= (inputSampleL * (fabs(inputSampleL) * glitch) * (fabs(inputSampleL) * glitch));
			inputSampleL *= (1.0 + glitch);
			inputSampleR -= (inputSampleR * (fabs(inputSampleR) * glitch) * (fabs(inputSampleR) * glitch));
			inputSampleR *= (1.0 + glitch);
		}
		// that's taken care of the really high gain stuff

		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * out) * (fabs(inputSampleL) * out));
		inputSampleL *= (1.0 + out);
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * out) * (fabs(inputSampleR) * out));
		inputSampleR *= (1.0 + out);

		if (output < 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}
		if (wet < 1.0) {
			inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
			inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
		}
		// nice little output stage template: if we have another scale of floating point
		// number, we really don't want to meaninglessly multiply that by 1.0.

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		drive->fpdL ^= drive->fpdL << 13;
		drive->fpdL ^= drive->fpdL >> 17;
		drive->fpdL ^= drive->fpdL << 5;
		inputSampleL += (((double) drive->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		drive->fpdR ^= drive->fpdR << 13;
		drive->fpdR ^= drive->fpdR >> 17;
		drive->fpdR ^= drive->fpdR << 5;
		inputSampleR += (((double) drive->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	DRIVE_URI,
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
