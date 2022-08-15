#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define VIBRATO_URI "https://hannesbraun.net/ns/lv2/airwindows/vibrato"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	SPEED = 4,
	DEPTH = 5,
	FM_SPEED = 6,
	FM_DEPTH = 7,
	INV_WET = 8
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* speed;
	const float* depth;
	const float* fmSpeed;
	const float* fmDepth;
	const float* invWet;

	double pL[16386]; // this is processed, not raw incoming samples
	double pR[16386]; // this is processed, not raw incoming samples
	double sweep;
	double sweepB;
	int gcount;

	double airPrevL;
	double airEvenL;
	double airOddL;
	double airFactorL;
	double airPrevR;
	double airEvenR;
	double airOddR;
	double airFactorR;

	bool flip;
	uint32_t fpdL;
	uint32_t fpdR;
} Vibrato;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Vibrato* vibrato = (Vibrato*) calloc(1, sizeof(Vibrato));
	return (LV2_Handle) vibrato;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Vibrato* vibrato = (Vibrato*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			vibrato->input[0] = (const float*) data;
			break;
		case INPUT_R:
			vibrato->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			vibrato->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			vibrato->output[1] = (float*) data;
			break;
		case SPEED:
			vibrato->speed = (const float*) data;
			break;
		case DEPTH:
			vibrato->depth = (const float*) data;
			break;
		case FM_SPEED:
			vibrato->fmSpeed = (const float*) data;
			break;
		case FM_DEPTH:
			vibrato->fmDepth = (const float*) data;
			break;
		case INV_WET:
			vibrato->invWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Vibrato* vibrato = (Vibrato*) instance;

	for (int count = 0; count < 16385; count++) {
		vibrato->pL[count] = 0.0;
		vibrato->pR[count] = 0.0;
	}
	vibrato->sweep = 3.141592653589793238 / 2.0;
	vibrato->sweepB = 3.141592653589793238 / 2.0;
	vibrato->gcount = 0;

	vibrato->airPrevL = 0.0;
	vibrato->airEvenL = 0.0;
	vibrato->airOddL = 0.0;
	vibrato->airFactorL = 0.0;
	vibrato->airPrevR = 0.0;
	vibrato->airEvenR = 0.0;
	vibrato->airOddR = 0.0;
	vibrato->airFactorR = 0.0;

	vibrato->flip = false;

	vibrato->fpdL = 1.0;
	while (vibrato->fpdL < 16386) vibrato->fpdL = rand() * UINT32_MAX;
	vibrato->fpdR = 1.0;
	while (vibrato->fpdR < 16386) vibrato->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Vibrato* vibrato = (Vibrato*) instance;

	const float* in1 = vibrato->input[0];
	const float* in2 = vibrato->input[1];
	float* out1 = vibrato->output[0];
	float* out2 = vibrato->output[1];

	double speed = pow(0.1 + *vibrato->speed, 6);
	double depth = (pow(*vibrato->depth, 3) / sqrt(speed)) * 4.0;
	double speedB = pow(0.1 + *vibrato->fmSpeed, 6);
	double depthB = pow(*vibrato->fmDepth, 3) / sqrt(speedB);
	double tupi = 3.141592653589793238 * 2.0;
	double wet = *vibrato->invWet; // note: inv/dry/wet

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = vibrato->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = vibrato->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		vibrato->airFactorL = vibrato->airPrevL - inputSampleL;
		vibrato->airFactorR = vibrato->airPrevR - inputSampleR;

		if (vibrato->flip) {
			vibrato->airEvenL += vibrato->airFactorL;
			vibrato->airOddL -= vibrato->airFactorL;
			vibrato->airFactorL = vibrato->airEvenL;
			vibrato->airEvenR += vibrato->airFactorR;
			vibrato->airOddR -= vibrato->airFactorR;
			vibrato->airFactorR = vibrato->airEvenR;
		} else {
			vibrato->airOddL += vibrato->airFactorL;
			vibrato->airEvenL -= vibrato->airFactorL;
			vibrato->airFactorL = vibrato->airOddL;
			vibrato->airOddR += vibrato->airFactorR;
			vibrato->airEvenR -= vibrato->airFactorR;
			vibrato->airFactorR = vibrato->airOddR;
		}

		vibrato->airOddL = (vibrato->airOddL - ((vibrato->airOddL - vibrato->airEvenL) / 256.0)) / 1.0001;
		vibrato->airOddR = (vibrato->airOddR - ((vibrato->airOddR - vibrato->airEvenR) / 256.0)) / 1.0001;
		vibrato->airEvenL = (vibrato->airEvenL - ((vibrato->airEvenL - vibrato->airOddL) / 256.0)) / 1.0001;
		vibrato->airEvenR = (vibrato->airEvenR - ((vibrato->airEvenR - vibrato->airOddR) / 256.0)) / 1.0001;
		vibrato->airPrevL = inputSampleL;
		vibrato->airPrevR = inputSampleR;
		inputSampleL += vibrato->airFactorL;
		inputSampleR += vibrato->airFactorR;

		vibrato->flip = !vibrato->flip;
		// air, compensates for loss of highs in the interpolation

		if (vibrato->gcount < 1 || vibrato->gcount > 8192) {
			vibrato->gcount = 8192;
		}
		int count = vibrato->gcount;
		vibrato->pL[count + 8192] = vibrato->pL[count] = inputSampleL;
		vibrato->pR[count + 8192] = vibrato->pR[count] = inputSampleR;

		double offset = depth + (depth * sin(vibrato->sweep));
		count += (int) floor(offset);

		inputSampleL = vibrato->pL[count] * (1.0 - (offset - floor(offset))); // less as value moves away from .0
		inputSampleL += vibrato->pL[count + 1]; // we can assume always using this in one way or another?
		inputSampleL += vibrato->pL[count + 2] * (offset - floor(offset)); // greater as value moves away from .0
		inputSampleL -= ((vibrato->pL[count] - vibrato->pL[count + 1]) - (vibrato->pL[count + 1] - vibrato->pL[count + 2])) / 50.0; // interpolation hacks 'r us
		inputSampleL *= 0.5; // gain trim

		inputSampleR = vibrato->pR[count] * (1.0 - (offset - floor(offset))); // less as value moves away from .0
		inputSampleR += vibrato->pR[count + 1]; // we can assume always using this in one way or another?
		inputSampleR += vibrato->pR[count + 2] * (offset - floor(offset)); // greater as value moves away from .0
		inputSampleR -= ((vibrato->pR[count] - vibrato->pR[count + 1]) - (vibrato->pR[count + 1] - vibrato->pR[count + 2])) / 50.0; // interpolation hacks 'r us
		inputSampleR *= 0.5; // gain trim

		vibrato->sweep += (speed + (speedB * sin(vibrato->sweepB) * depthB));
		vibrato->sweepB += speedB;
		if (vibrato->sweep > tupi) {
			vibrato->sweep -= tupi;
		}
		if (vibrato->sweep < 0.0) {
			vibrato->sweep += tupi; // through zero FM
		}
		if (vibrato->sweepB > tupi) {
			vibrato->sweepB -= tupi;
		}
		vibrato->gcount--;
		// still scrolling through the samples, remember

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - fabs(wet)));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - fabs(wet)));
		}
		// Inv/Dry/Wet control

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		vibrato->fpdL ^= vibrato->fpdL << 13;
		vibrato->fpdL ^= vibrato->fpdL >> 17;
		vibrato->fpdL ^= vibrato->fpdL << 5;
		inputSampleL += (((double) vibrato->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		vibrato->fpdR ^= vibrato->fpdR << 13;
		vibrato->fpdR ^= vibrato->fpdR >> 17;
		vibrato->fpdR ^= vibrato->fpdR << 5;
		inputSampleR += (((double) vibrato->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	VIBRATO_URI,
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
