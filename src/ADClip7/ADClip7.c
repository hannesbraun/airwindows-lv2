#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ADCLIP7_URI "https://hannesbraun.net/ns/lv2/airwindows/adclip7"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BOOST = 4,
	SOFTEN = 5,
	ENHANCE = 6,
	MODE = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* boost;
	const float* soften;
	const float* enhance;
	const float* mode;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double lastSampleL;
	double lastSampleR;
	float bL[22200];
	float bR[22200];
	int gcount;
	double lowsL;
	double lowsR;
	double iirLowsAL;
	double iirLowsAR;
	double iirLowsBL;
	double iirLowsBR;
	double refclipL;
	double refclipR;
} ADClip7;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ADClip7* adclip7 = (ADClip7*) calloc(1, sizeof(ADClip7));
	adclip7->sampleRate = rate;
	return (LV2_Handle) adclip7;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ADClip7* adclip7 = (ADClip7*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			adclip7->input[0] = (const float*) data;
			break;
		case INPUT_R:
			adclip7->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			adclip7->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			adclip7->output[1] = (float*) data;
			break;
		case BOOST:
			adclip7->boost = (const float*) data;
			break;
		case SOFTEN:
			adclip7->soften = (const float*) data;
			break;
		case ENHANCE:
			adclip7->enhance = (const float*) data;
			break;
		case MODE:
			adclip7->mode = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ADClip7* adclip7 = (ADClip7*) instance;

	adclip7->lastSampleL = 0.0;
	adclip7->lastSampleR = 0.0;
	for (int count = 0; count < 22199; count++) {
		adclip7->bL[count] = 0;
		adclip7->bR[count] = 0;
	}
	adclip7->gcount = 0;
	adclip7->lowsL = 0;
	adclip7->lowsR = 0;
	adclip7->refclipL = 0.99;
	adclip7->refclipR = 0.99;
	adclip7->iirLowsAL = 0.0;
	adclip7->iirLowsAR = 0.0;
	adclip7->iirLowsBL = 0.0;
	adclip7->iirLowsBR = 0.0;

	adclip7->fpdL = 1.0;
	while (adclip7->fpdL < 16386) adclip7->fpdL = rand() * UINT32_MAX;
	adclip7->fpdR = 1.0;
	while (adclip7->fpdR < 16386) adclip7->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ADClip7* adclip7 = (ADClip7*) instance;

	const float* in1 = adclip7->input[0];
	const float* in2 = adclip7->input[1];
	float* out1 = adclip7->output[0];
	float* out2 = adclip7->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= adclip7->sampleRate;
	double fpOld = 0.618033988749894848204586; // golden ratio!
	double fpNew = 1.0 - fpOld;
	double inputGain = pow(10.0, (*adclip7->boost) / 20.0);
	double softness = *adclip7->soften * fpNew;
	double hardness = 1.0 - softness;
	double highslift = 0.307 * *adclip7->enhance;
	double adjust = pow(highslift, 3) * 0.416;
	double subslift = 0.796 * *adclip7->enhance;
	double calibsubs = subslift / 53;
	double invcalibsubs = 1.0 - calibsubs;
	double subs = 0.81 + (calibsubs * 2);
	double bridgerectifier;
	int mode = (int) *adclip7->mode + 1;
	double overshootL;
	double overshootR;
	double offsetH1 = 1.84;
	offsetH1 *= overallscale;
	double offsetH2 = offsetH1 * 1.9;
	double offsetH3 = offsetH1 * 2.7;
	double offsetL1 = 612;
	offsetL1 *= overallscale;
	double offsetL2 = offsetL1 * 2.0;
	int refH1 = (int) floor(offsetH1);
	int refH2 = (int) floor(offsetH2);
	int refH3 = (int) floor(offsetH3);
	int refL1 = (int) floor(offsetL1);
	int refL2 = (int) floor(offsetL2);
	int temp;
	double fractionH1 = offsetH1 - floor(offsetH1);
	double fractionH2 = offsetH2 - floor(offsetH2);
	double fractionH3 = offsetH3 - floor(offsetH3);
	double minusH1 = 1.0 - fractionH1;
	double minusH2 = 1.0 - fractionH2;
	double minusH3 = 1.0 - fractionH3;
	double highsL = 0.0;
	double highsR = 0.0;
	int count = 0;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = adclip7->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = adclip7->fpdR * 1.18e-17;

		if (inputGain != 1.0) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}

		overshootL = fabs(inputSampleL) - adclip7->refclipL;
		overshootR = fabs(inputSampleR) - adclip7->refclipR;
		if (overshootL < 0.0) overshootL = 0.0;
		if (overshootR < 0.0) overshootR = 0.0;

		if (adclip7->gcount < 0 || adclip7->gcount > 11020) {
			adclip7->gcount = 11020;
		}
		count = adclip7->gcount;
		adclip7->bL[count + 11020] = adclip7->bL[count] = overshootL;
		adclip7->bR[count + 11020] = adclip7->bR[count] = overshootR;
		adclip7->gcount--;

		if (highslift > 0.0) {
			// we have a big pile of b[] which is overshoots
			temp = count + refH3;
			highsL = -(adclip7->bL[temp] * minusH3); // less as value moves away from .0
			highsL -= adclip7->bL[temp + 1]; // we can assume always using this in one way or another?
			highsL -= (adclip7->bL[temp + 2] * fractionH3); // greater as value moves away from .0
			highsL += (((adclip7->bL[temp] - adclip7->bL[temp + 1]) - (adclip7->bL[temp + 1] - adclip7->bL[temp + 2])) / 50); // interpolation hacks 'r us
			highsL *= adjust; // add in the kernel elements backwards saves multiplies
			// stage 3 is a negative add
			highsR = -(adclip7->bR[temp] * minusH3); // less as value moves away from .0
			highsR -= adclip7->bR[temp + 1]; // we can assume always using this in one way or another?
			highsR -= (adclip7->bR[temp + 2] * fractionH3); // greater as value moves away from .0
			highsR += (((adclip7->bR[temp] - adclip7->bR[temp + 1]) - (adclip7->bR[temp + 1] - adclip7->bR[temp + 2])) / 50); // interpolation hacks 'r us
			highsR *= adjust; // add in the kernel elements backwards saves multiplies
			// stage 3 is a negative add
			temp = count + refH2;
			highsL += (adclip7->bL[temp] * minusH2); // less as value moves away from .0
			highsL += adclip7->bL[temp + 1]; // we can assume always using this in one way or another?
			highsL += (adclip7->bL[temp + 2] * fractionH2); // greater as value moves away from .0
			highsL -= (((adclip7->bL[temp] - adclip7->bL[temp + 1]) - (adclip7->bL[temp + 1] - adclip7->bL[temp + 2])) / 50); // interpolation hacks 'r us
			highsL *= adjust; // add in the kernel elements backwards saves multiplies
			// stage 2 is a positive feedback of the overshoot
			highsR += (adclip7->bR[temp] * minusH2); // less as value moves away from .0
			highsR += adclip7->bR[temp + 1]; // we can assume always using this in one way or another?
			highsR += (adclip7->bR[temp + 2] * fractionH2); // greater as value moves away from .0
			highsR -= (((adclip7->bR[temp] - adclip7->bR[temp + 1]) - (adclip7->bR[temp + 1] - adclip7->bR[temp + 2])) / 50); // interpolation hacks 'r us
			highsR *= adjust; // add in the kernel elements backwards saves multiplies
			// stage 2 is a positive feedback of the overshoot
			temp = count + refH1;
			highsL -= (adclip7->bL[temp] * minusH1); // less as value moves away from .0
			highsL -= adclip7->bL[temp + 1]; // we can assume always using this in one way or another?
			highsL -= (adclip7->bL[temp + 2] * fractionH1); // greater as value moves away from .0
			highsL += (((adclip7->bL[temp] - adclip7->bL[temp + 1]) - (adclip7->bL[temp + 1] - adclip7->bL[temp + 2])) / 50); // interpolation hacks 'r us
			highsL *= adjust; // add in the kernel elements backwards saves multiplies
			// stage 1 is a negative feedback of the overshoot
			highsR -= (adclip7->bR[temp] * minusH1); // less as value moves away from .0
			highsR -= adclip7->bR[temp + 1]; // we can assume always using this in one way or another?
			highsR -= (adclip7->bR[temp + 2] * fractionH1); // greater as value moves away from .0
			highsR += (((adclip7->bR[temp] - adclip7->bR[temp + 1]) - (adclip7->bR[temp + 1] - adclip7->bR[temp + 2])) / 50); // interpolation hacks 'r us
			highsR *= adjust; // add in the kernel elements backwards saves multiplies
			// stage 1 is a negative feedback of the overshoot
			// done with interpolated mostly negative feedback of the overshoot
		}

		bridgerectifier = sin(fabs(highsL) * hardness);
		// this will wrap around and is scaled back by softness
		// wrap around is the same principle as Fracture: no top limit to sin()
		if (highsL > 0) highsL = bridgerectifier;
		else highsL = -bridgerectifier;

		bridgerectifier = sin(fabs(highsR) * hardness);
		// this will wrap around and is scaled back by softness
		// wrap around is the same principle as Fracture: no top limit to sin()
		if (highsR > 0) highsR = bridgerectifier;
		else highsR = -bridgerectifier;

		if (subslift > 0.0) {
			adclip7->lowsL *= subs;
			adclip7->lowsR *= subs;
			// going in we'll reel back some of the swing
			temp = count + refL1;

			adclip7->lowsL -= adclip7->bL[temp + 127];
			adclip7->lowsL -= adclip7->bL[temp + 113];
			adclip7->lowsL -= adclip7->bL[temp + 109];
			adclip7->lowsL -= adclip7->bL[temp + 107];
			adclip7->lowsL -= adclip7->bL[temp + 103];
			adclip7->lowsL -= adclip7->bL[temp + 101];
			adclip7->lowsL -= adclip7->bL[temp + 97];
			adclip7->lowsL -= adclip7->bL[temp + 89];
			adclip7->lowsL -= adclip7->bL[temp + 83];
			adclip7->lowsL -= adclip7->bL[temp + 79];
			adclip7->lowsL -= adclip7->bL[temp + 73];
			adclip7->lowsL -= adclip7->bL[temp + 71];
			adclip7->lowsL -= adclip7->bL[temp + 67];
			adclip7->lowsL -= adclip7->bL[temp + 61];
			adclip7->lowsL -= adclip7->bL[temp + 59];
			adclip7->lowsL -= adclip7->bL[temp + 53];
			adclip7->lowsL -= adclip7->bL[temp + 47];
			adclip7->lowsL -= adclip7->bL[temp + 43];
			adclip7->lowsL -= adclip7->bL[temp + 41];
			adclip7->lowsL -= adclip7->bL[temp + 37];
			adclip7->lowsL -= adclip7->bL[temp + 31];
			adclip7->lowsL -= adclip7->bL[temp + 29];
			adclip7->lowsL -= adclip7->bL[temp + 23];
			adclip7->lowsL -= adclip7->bL[temp + 19];
			adclip7->lowsL -= adclip7->bL[temp + 17];
			adclip7->lowsL -= adclip7->bL[temp + 13];
			adclip7->lowsL -= adclip7->bL[temp + 11];
			adclip7->lowsL -= adclip7->bL[temp + 7];
			adclip7->lowsL -= adclip7->bL[temp + 5];
			adclip7->lowsL -= adclip7->bL[temp + 3];
			adclip7->lowsL -= adclip7->bL[temp + 2];
			adclip7->lowsL -= adclip7->bL[temp + 1];
			// initial negative lobe

			adclip7->lowsR -= adclip7->bR[temp + 127];
			adclip7->lowsR -= adclip7->bR[temp + 113];
			adclip7->lowsR -= adclip7->bR[temp + 109];
			adclip7->lowsR -= adclip7->bR[temp + 107];
			adclip7->lowsR -= adclip7->bR[temp + 103];
			adclip7->lowsR -= adclip7->bR[temp + 101];
			adclip7->lowsR -= adclip7->bR[temp + 97];
			adclip7->lowsR -= adclip7->bR[temp + 89];
			adclip7->lowsR -= adclip7->bR[temp + 83];
			adclip7->lowsR -= adclip7->bR[temp + 79];
			adclip7->lowsR -= adclip7->bR[temp + 73];
			adclip7->lowsR -= adclip7->bR[temp + 71];
			adclip7->lowsR -= adclip7->bR[temp + 67];
			adclip7->lowsR -= adclip7->bR[temp + 61];
			adclip7->lowsR -= adclip7->bR[temp + 59];
			adclip7->lowsR -= adclip7->bR[temp + 53];
			adclip7->lowsR -= adclip7->bR[temp + 47];
			adclip7->lowsR -= adclip7->bR[temp + 43];
			adclip7->lowsR -= adclip7->bR[temp + 41];
			adclip7->lowsR -= adclip7->bR[temp + 37];
			adclip7->lowsR -= adclip7->bR[temp + 31];
			adclip7->lowsR -= adclip7->bR[temp + 29];
			adclip7->lowsR -= adclip7->bR[temp + 23];
			adclip7->lowsR -= adclip7->bR[temp + 19];
			adclip7->lowsR -= adclip7->bR[temp + 17];
			adclip7->lowsR -= adclip7->bR[temp + 13];
			adclip7->lowsR -= adclip7->bR[temp + 11];
			adclip7->lowsR -= adclip7->bR[temp + 7];
			adclip7->lowsR -= adclip7->bR[temp + 5];
			adclip7->lowsR -= adclip7->bR[temp + 3];
			adclip7->lowsR -= adclip7->bR[temp + 2];
			adclip7->lowsR -= adclip7->bR[temp + 1];
			// initial negative lobe

			adclip7->lowsL *= subs;
			adclip7->lowsL *= subs;
			adclip7->lowsR *= subs;
			adclip7->lowsR *= subs;
			// twice, to minimize the suckout in low boost situations
			temp = count + refL2;

			adclip7->lowsL += adclip7->bL[temp + 127];
			adclip7->lowsL += adclip7->bL[temp + 113];
			adclip7->lowsL += adclip7->bL[temp + 109];
			adclip7->lowsL += adclip7->bL[temp + 107];
			adclip7->lowsL += adclip7->bL[temp + 103];
			adclip7->lowsL += adclip7->bL[temp + 101];
			adclip7->lowsL += adclip7->bL[temp + 97];
			adclip7->lowsL += adclip7->bL[temp + 89];
			adclip7->lowsL += adclip7->bL[temp + 83];
			adclip7->lowsL += adclip7->bL[temp + 79];
			adclip7->lowsL += adclip7->bL[temp + 73];
			adclip7->lowsL += adclip7->bL[temp + 71];
			adclip7->lowsL += adclip7->bL[temp + 67];
			adclip7->lowsL += adclip7->bL[temp + 61];
			adclip7->lowsL += adclip7->bL[temp + 59];
			adclip7->lowsL += adclip7->bL[temp + 53];
			adclip7->lowsL += adclip7->bL[temp + 47];
			adclip7->lowsL += adclip7->bL[temp + 43];
			adclip7->lowsL += adclip7->bL[temp + 41];
			adclip7->lowsL += adclip7->bL[temp + 37];
			adclip7->lowsL += adclip7->bL[temp + 31];
			adclip7->lowsL += adclip7->bL[temp + 29];
			adclip7->lowsL += adclip7->bL[temp + 23];
			adclip7->lowsL += adclip7->bL[temp + 19];
			adclip7->lowsL += adclip7->bL[temp + 17];
			adclip7->lowsL += adclip7->bL[temp + 13];
			adclip7->lowsL += adclip7->bL[temp + 11];
			adclip7->lowsL += adclip7->bL[temp + 7];
			adclip7->lowsL += adclip7->bL[temp + 5];
			adclip7->lowsL += adclip7->bL[temp + 3];
			adclip7->lowsL += adclip7->bL[temp + 2];
			adclip7->lowsL += adclip7->bL[temp + 1];
			// followup positive lobe

			adclip7->lowsR += adclip7->bR[temp + 127];
			adclip7->lowsR += adclip7->bR[temp + 113];
			adclip7->lowsR += adclip7->bR[temp + 109];
			adclip7->lowsR += adclip7->bR[temp + 107];
			adclip7->lowsR += adclip7->bR[temp + 103];
			adclip7->lowsR += adclip7->bR[temp + 101];
			adclip7->lowsR += adclip7->bR[temp + 97];
			adclip7->lowsR += adclip7->bR[temp + 89];
			adclip7->lowsR += adclip7->bR[temp + 83];
			adclip7->lowsR += adclip7->bR[temp + 79];
			adclip7->lowsR += adclip7->bR[temp + 73];
			adclip7->lowsR += adclip7->bR[temp + 71];
			adclip7->lowsR += adclip7->bR[temp + 67];
			adclip7->lowsR += adclip7->bR[temp + 61];
			adclip7->lowsR += adclip7->bR[temp + 59];
			adclip7->lowsR += adclip7->bR[temp + 53];
			adclip7->lowsR += adclip7->bR[temp + 47];
			adclip7->lowsR += adclip7->bR[temp + 43];
			adclip7->lowsR += adclip7->bR[temp + 41];
			adclip7->lowsR += adclip7->bR[temp + 37];
			adclip7->lowsR += adclip7->bR[temp + 31];
			adclip7->lowsR += adclip7->bR[temp + 29];
			adclip7->lowsR += adclip7->bR[temp + 23];
			adclip7->lowsR += adclip7->bR[temp + 19];
			adclip7->lowsR += adclip7->bR[temp + 17];
			adclip7->lowsR += adclip7->bR[temp + 13];
			adclip7->lowsR += adclip7->bR[temp + 11];
			adclip7->lowsR += adclip7->bR[temp + 7];
			adclip7->lowsR += adclip7->bR[temp + 5];
			adclip7->lowsR += adclip7->bR[temp + 3];
			adclip7->lowsR += adclip7->bR[temp + 2];
			adclip7->lowsR += adclip7->bR[temp + 1];
			// followup positive lobe

			adclip7->lowsL *= subs;
			adclip7->lowsR *= subs;
			// now we have the lows content to use
		}

		bridgerectifier = sin(fabs(adclip7->lowsL) * softness);
		// this will wrap around and is scaled back by hardness: hard = less bass push, more treble
		// wrap around is the same principle as Fracture: no top limit to sin()
		if (adclip7->lowsL > 0) adclip7->lowsL = bridgerectifier;
		else adclip7->lowsL = -bridgerectifier;

		bridgerectifier = sin(fabs(adclip7->lowsR) * softness);
		// this will wrap around and is scaled back by hardness: hard = less bass push, more treble
		// wrap around is the same principle as Fracture: no top limit to sin()
		if (adclip7->lowsR > 0) adclip7->lowsR = bridgerectifier;
		else adclip7->lowsR = -bridgerectifier;

		adclip7->iirLowsAL = (adclip7->iirLowsAL * invcalibsubs) + (adclip7->lowsL * calibsubs);
		adclip7->lowsL = adclip7->iirLowsAL;
		bridgerectifier = sin(fabs(adclip7->lowsL));
		if (adclip7->lowsL > 0) adclip7->lowsL = bridgerectifier;
		else adclip7->lowsL = -bridgerectifier;

		adclip7->iirLowsAR = (adclip7->iirLowsAR * invcalibsubs) + (adclip7->lowsR * calibsubs);
		adclip7->lowsR = adclip7->iirLowsAR;
		bridgerectifier = sin(fabs(adclip7->lowsR));
		if (adclip7->lowsR > 0) adclip7->lowsR = bridgerectifier;
		else adclip7->lowsR = -bridgerectifier;

		adclip7->iirLowsBL = (adclip7->iirLowsBL * invcalibsubs) + (adclip7->lowsL * calibsubs);
		adclip7->lowsL = adclip7->iirLowsBL;
		bridgerectifier = sin(fabs(adclip7->lowsL)) * 2.0;
		if (adclip7->lowsL > 0) adclip7->lowsL = bridgerectifier;
		else adclip7->lowsL = -bridgerectifier;

		adclip7->iirLowsBR = (adclip7->iirLowsBR * invcalibsubs) + (adclip7->lowsR * calibsubs);
		adclip7->lowsR = adclip7->iirLowsBR;
		bridgerectifier = sin(fabs(adclip7->lowsR)) * 2.0;
		if (adclip7->lowsR > 0) adclip7->lowsR = bridgerectifier;
		else adclip7->lowsR = -bridgerectifier;

		if (highslift > 0.0) inputSampleL += (highsL * (1.0 - fabs(inputSampleL * hardness)));
		if (subslift > 0.0) inputSampleL += (adclip7->lowsL * (1.0 - fabs(inputSampleL * softness)));

		if (highslift > 0.0) inputSampleR += (highsR * (1.0 - fabs(inputSampleR * hardness)));
		if (subslift > 0.0) inputSampleR += (adclip7->lowsR * (1.0 - fabs(inputSampleR * softness)));

		if (inputSampleL > adclip7->refclipL && adclip7->refclipL > 0.9) adclip7->refclipL -= 0.01;
		if (inputSampleL < -adclip7->refclipL && adclip7->refclipL > 0.9) adclip7->refclipL -= 0.01;
		if (adclip7->refclipL < 0.99) adclip7->refclipL += 0.00001;
		// adjust clip level on the fly

		if (inputSampleR > adclip7->refclipR && adclip7->refclipR > 0.9) adclip7->refclipR -= 0.01;
		if (inputSampleR < -adclip7->refclipR && adclip7->refclipR > 0.9) adclip7->refclipR -= 0.01;
		if (adclip7->refclipR < 0.99) adclip7->refclipR += 0.00001;
		// adjust clip level on the fly

		if (adclip7->lastSampleL >= adclip7->refclipL) {
			if (inputSampleL < adclip7->refclipL) adclip7->lastSampleL = ((adclip7->refclipL * hardness) + (inputSampleL * softness));
			else adclip7->lastSampleL = adclip7->refclipL;
		}

		if (adclip7->lastSampleR >= adclip7->refclipR) {
			if (inputSampleR < adclip7->refclipR) adclip7->lastSampleR = ((adclip7->refclipR * hardness) + (inputSampleR * softness));
			else adclip7->lastSampleR = adclip7->refclipR;
		}

		if (adclip7->lastSampleL <= -adclip7->refclipL) {
			if (inputSampleL > -adclip7->refclipL) adclip7->lastSampleL = ((-adclip7->refclipL * hardness) + (inputSampleL * softness));
			else adclip7->lastSampleL = -adclip7->refclipL;
		}

		if (adclip7->lastSampleR <= -adclip7->refclipR) {
			if (inputSampleR > -adclip7->refclipR) adclip7->lastSampleR = ((-adclip7->refclipR * hardness) + (inputSampleR * softness));
			else adclip7->lastSampleR = -adclip7->refclipR;
		}

		if (inputSampleL > adclip7->refclipL) {
			if (adclip7->lastSampleL < adclip7->refclipL) inputSampleL = ((adclip7->refclipL * hardness) + (adclip7->lastSampleL * softness));
			else inputSampleL = adclip7->refclipL;
		}

		if (inputSampleR > adclip7->refclipR) {
			if (adclip7->lastSampleR < adclip7->refclipR) inputSampleR = ((adclip7->refclipR * hardness) + (adclip7->lastSampleR * softness));
			else inputSampleR = adclip7->refclipR;
		}

		if (inputSampleL < -adclip7->refclipL) {
			if (adclip7->lastSampleL > -adclip7->refclipL) inputSampleL = ((-adclip7->refclipL * hardness) + (adclip7->lastSampleL * softness));
			else inputSampleL = -adclip7->refclipL;
		}

		if (inputSampleR < -adclip7->refclipR) {
			if (adclip7->lastSampleR > -adclip7->refclipR) inputSampleR = ((-adclip7->refclipR * hardness) + (adclip7->lastSampleR * softness));
			else inputSampleR = -adclip7->refclipR;
		}
		adclip7->lastSampleL = inputSampleL;
		adclip7->lastSampleR = inputSampleR;

		switch (mode) {
			case 2:
				inputSampleL /= inputGain;
				inputSampleR /= inputGain;
				break; // Gain Match
			case 3:
			case 4:
				inputSampleL = overshootL + highsL + adclip7->lowsL;
				inputSampleR = overshootR + highsR + adclip7->lowsR;
				break; // Clip Only
			case 1:
			default:
				break; // Normal
		}
		// this is our output mode switch, showing the effects

		if (inputSampleL > adclip7->refclipL) inputSampleL = adclip7->refclipL;
		if (inputSampleL < -adclip7->refclipL) inputSampleL = -adclip7->refclipL;
		if (inputSampleR > adclip7->refclipR) inputSampleR = adclip7->refclipR;
		if (inputSampleR < -adclip7->refclipR) inputSampleR = -adclip7->refclipR;
		// final iron bar

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		adclip7->fpdL ^= adclip7->fpdL << 13;
		adclip7->fpdL ^= adclip7->fpdL >> 17;
		adclip7->fpdL ^= adclip7->fpdL << 5;
		inputSampleL += (((double) adclip7->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		adclip7->fpdR ^= adclip7->fpdR << 13;
		adclip7->fpdR ^= adclip7->fpdR >> 17;
		adclip7->fpdR ^= adclip7->fpdR << 5;
		inputSampleR += (((double) adclip7->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	ADCLIP7_URI,
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
