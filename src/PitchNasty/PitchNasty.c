#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PITCHNASTY_URI "https://hannesbraun.net/ns/lv2/airwindows/pitchnasty"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	NOTE = 4,
	BEND = 5,
	GRIND = 6,
	FEEDBACK = 7,
	OUTPUT = 8,
	DRY_WET = 9
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* note;
	const float* bend;
	const float* grind;
	const float* feedback;
	const float* outputGain;
	const float* dryWet;

	double dL[10002];
	double dR[10002];
	int inCount;
	double outCount;
	bool switchTransition;
	double switchAmountL;
	double switchAmountR;
	double feedbackSampleL;
	double feedbackSampleR;
	uint32_t fpdL;
	uint32_t fpdR;
} PitchNasty;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PitchNasty* pitchNasty = (PitchNasty*) calloc(1, sizeof(PitchNasty));
	pitchNasty->sampleRate = rate;
	return (LV2_Handle) pitchNasty;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PitchNasty* pitchNasty = (PitchNasty*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pitchNasty->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pitchNasty->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pitchNasty->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pitchNasty->output[1] = (float*) data;
			break;
		case NOTE:
			pitchNasty->note = (const float*) data;
			break;
		case BEND:
			pitchNasty->bend = (const float*) data;
			break;
		case GRIND:
			pitchNasty->grind = (const float*) data;
			break;
		case FEEDBACK:
			pitchNasty->feedback = (const float*) data;
			break;
		case OUTPUT:
			pitchNasty->outputGain = (const float*) data;
			break;
		case DRY_WET:
			pitchNasty->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PitchNasty* pitchNasty = (PitchNasty*) instance;
	for (int temp = 0; temp < 10001; temp++) {
		pitchNasty->dL[temp] = 0.0;
		pitchNasty->dR[temp] = 0.0;
	}
	pitchNasty->inCount = 1;
	pitchNasty->outCount = 1.0;
	pitchNasty->switchTransition = false;
	pitchNasty->switchAmountL = 0.0;
	pitchNasty->switchAmountR = 0.0;
	pitchNasty->feedbackSampleL = 0.0;
	pitchNasty->feedbackSampleR = 0.0;

	pitchNasty->fpdL = 1.0;
	while (pitchNasty->fpdL < 16386) pitchNasty->fpdL = rand() * UINT32_MAX;
	pitchNasty->fpdR = 1.0;
	while (pitchNasty->fpdR < 16386) pitchNasty->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PitchNasty* pitchNasty = (PitchNasty*) instance;

	const float* in1 = pitchNasty->input[0];
	const float* in2 = pitchNasty->input[1];
	float* out1 = pitchNasty->output[0];
	float* out2 = pitchNasty->output[1];

	double note = (int) *pitchNasty->note; // -12 to +12
	double bend = *pitchNasty->bend; // -12 to +12
	double speed = pow(2, note / 12.0) * pow(2, bend / 12.0);

	double grindRef = (1.0 / 261.6) * pitchNasty->sampleRate; // samples per Middle C cycle
	double grindNote = pow(2, ((int) *pitchNasty->grind) / 12.0);
	double width = grindRef / grindNote;
	if (width > 9990) width = 9990; // safety check
	double feedback = *pitchNasty->feedback;
	double trim = *pitchNasty->outputGain;
	double wet = *pitchNasty->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pitchNasty->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pitchNasty->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL += (pitchNasty->feedbackSampleL * feedback);
		inputSampleR += (pitchNasty->feedbackSampleR * feedback);
		if (fabs(pitchNasty->feedbackSampleL) > 1.0) inputSampleL /= fabs(pitchNasty->feedbackSampleL);
		if (fabs(pitchNasty->feedbackSampleR) > 1.0) inputSampleR /= fabs(pitchNasty->feedbackSampleR);

		pitchNasty->dL[pitchNasty->inCount] = inputSampleL;
		pitchNasty->dR[pitchNasty->inCount] = inputSampleR;

		pitchNasty->inCount++;
		pitchNasty->outCount += speed;

		if (pitchNasty->outCount > pitchNasty->inCount && pitchNasty->switchTransition) {
			pitchNasty->switchAmountL = 1.0;
			pitchNasty->switchAmountR = 1.0;
			pitchNasty->switchTransition = false;
		}

		if (pitchNasty->inCount > width) pitchNasty->inCount -= width;
		if (pitchNasty->outCount > width) {
			pitchNasty->outCount -= width;
			pitchNasty->switchTransition = true;
			// reset the transition point
		}

		int count = floor(pitchNasty->outCount);
		int arrayWidth = floor(width);

		inputSampleL = (pitchNasty->dL[count - ((count > arrayWidth) ? arrayWidth + 1 : 0)] * (1 - (pitchNasty->outCount - floor(pitchNasty->outCount))));
		inputSampleL += (pitchNasty->dL[count + 1 - ((count + 1 > arrayWidth) ? arrayWidth + 1 : 0)] * (pitchNasty->outCount - floor(pitchNasty->outCount)));

		inputSampleR = (pitchNasty->dR[count - ((count > arrayWidth) ? arrayWidth + 1 : 0)] * (1 - (pitchNasty->outCount - floor(pitchNasty->outCount))));
		inputSampleR += (pitchNasty->dR[count + 1 - ((count + 1 > arrayWidth) ? arrayWidth + 1 : 0)] * (pitchNasty->outCount - floor(pitchNasty->outCount)));

		if (pitchNasty->switchAmountL > fabs(inputSampleL - pitchNasty->dL[pitchNasty->inCount]) * 2.0) {
			pitchNasty->switchAmountL = (pitchNasty->switchAmountL * 0.5) + (fabs(inputSampleL - pitchNasty->dL[pitchNasty->inCount]));
		}
		if (pitchNasty->switchAmountR > fabs(inputSampleR - pitchNasty->dR[pitchNasty->inCount]) * 2.0) {
			pitchNasty->switchAmountR = (pitchNasty->switchAmountR * 0.5) + (fabs(inputSampleR - pitchNasty->dR[pitchNasty->inCount]));
		}
		inputSampleL = (pitchNasty->dL[pitchNasty->inCount] * pitchNasty->switchAmountL) + (inputSampleL * (1.0 - pitchNasty->switchAmountL));
		inputSampleR = (pitchNasty->dR[pitchNasty->inCount] * pitchNasty->switchAmountR) + (inputSampleR * (1.0 - pitchNasty->switchAmountR));

		pitchNasty->feedbackSampleL = inputSampleL;
		pitchNasty->feedbackSampleR = inputSampleR;

		if (trim != 1.0) {
			inputSampleL *= trim;
			inputSampleR *= trim;
		}

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pitchNasty->fpdL ^= pitchNasty->fpdL << 13;
		pitchNasty->fpdL ^= pitchNasty->fpdL >> 17;
		pitchNasty->fpdL ^= pitchNasty->fpdL << 5;
		inputSampleL += (((double) pitchNasty->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pitchNasty->fpdR ^= pitchNasty->fpdR << 13;
		pitchNasty->fpdR ^= pitchNasty->fpdR >> 17;
		pitchNasty->fpdR ^= pitchNasty->fpdR << 5;
		inputSampleR += (((double) pitchNasty->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PITCHNASTY_URI,
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
