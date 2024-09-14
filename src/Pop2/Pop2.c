#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define POP2_URI "https://hannesbraun.net/ns/lv2/airwindows/pop2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	COMPRESS = 4,
	ATTACK = 5,
	RELEASE = 6,
	DRIVE = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* compress;
	const float* attack;
	const float* release;
	const float* drive;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double muVaryL;
	double muAttackL;
	double muNewSpeedL;
	double muSpeedAL;
	double muSpeedBL;
	double muCoefficientAL;
	double muCoefficientBL;

	double muVaryR;
	double muAttackR;
	double muNewSpeedR;
	double muSpeedAR;
	double muSpeedBR;
	double muCoefficientAR;
	double muCoefficientBR;

	bool flip;

	double lastSampleL;
	double intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	double lastSampleR;
	double intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; // Stereo ClipOnly2
} Pop2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Pop2* pop2 = (Pop2*) calloc(1, sizeof(Pop2));
	pop2->sampleRate = rate;
	return (LV2_Handle) pop2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Pop2* pop2 = (Pop2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pop2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pop2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pop2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pop2->output[1] = (float*) data;
			break;
		case COMPRESS:
			pop2->compress = (const float*) data;
			break;
		case ATTACK:
			pop2->attack = (const float*) data;
			break;
		case RELEASE:
			pop2->release = (const float*) data;
			break;
		case DRIVE:
			pop2->drive = (const float*) data;
			break;
		case DRY_WET:
			pop2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Pop2* pop2 = (Pop2*) instance;

	pop2->fpdL = 1.0;
	while (pop2->fpdL < 16386) pop2->fpdL = rand() * UINT32_MAX;
	pop2->fpdR = 1.0;
	while (pop2->fpdR < 16386) pop2->fpdR = rand() * UINT32_MAX;

	pop2->lastSampleL = 0.0;
	pop2->wasPosClipL = false;
	pop2->wasNegClipL = false;
	pop2->lastSampleR = 0.0;
	pop2->wasPosClipR = false;
	pop2->wasNegClipR = false;
	for (int x = 0; x < 16; x++) {
		pop2->intermediateL[x] = 0.0;
		pop2->intermediateR[x] = 0.0;
	}

	pop2->muVaryL = 0.0;
	pop2->muAttackL = 0.0;
	pop2->muNewSpeedL = 1000.0;
	pop2->muSpeedAL = 1000.0;
	pop2->muSpeedBL = 1000.0;
	pop2->muCoefficientAL = 1.0;
	pop2->muCoefficientBL = 1.0;

	pop2->muVaryR = 0.0;
	pop2->muAttackR = 0.0;
	pop2->muNewSpeedR = 1000.0;
	pop2->muSpeedAR = 1000.0;
	pop2->muSpeedBR = 1000.0;
	pop2->muCoefficientAR = 1.0;
	pop2->muCoefficientBR = 1.0;

	pop2->flip = false;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Pop2* pop2 = (Pop2*) instance;

	const float* in1 = pop2->input[0];
	const float* in2 = pop2->input[1];
	float* out1 = pop2->output[0];
	float* out2 = pop2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= pop2->sampleRate;

	int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;

	double threshold = 1.0 - ((1.0 - pow(1.0 - *pop2->compress, 2)) * 0.9);
	double attack = ((pow(*pop2->attack, 4) * 100000.0) + 10.0) * overallscale;
	double release = ((pow(*pop2->release, 5) * 2000000.0) + 20.0) * overallscale;
	double maxRelease = release * 4.0;
	double muPreGain = 1.0 / threshold;
	double muMakeupGain = sqrt(1.0 / threshold) * (*pop2->drive);
	double wet = *pop2->dryWet;
	// compressor section

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pop2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pop2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		// begin compressor section
		inputSampleL *= muPreGain;
		inputSampleR *= muPreGain;
		// adjust coefficients for L
		if (pop2->flip) {
			if (fabs(inputSampleL) > threshold) {
				pop2->muVaryL = threshold / fabs(inputSampleL);
				pop2->muAttackL = sqrt(fabs(pop2->muSpeedAL));
				pop2->muCoefficientAL = pop2->muCoefficientAL * (pop2->muAttackL - 1.0);
				if (pop2->muVaryL < threshold) pop2->muCoefficientAL = pop2->muCoefficientAL + threshold;
				else pop2->muCoefficientAL = pop2->muCoefficientAL + pop2->muVaryL;
				pop2->muCoefficientAL = pop2->muCoefficientAL / pop2->muAttackL;
				pop2->muNewSpeedL = pop2->muSpeedAL * (pop2->muSpeedAL - 1.0);
				pop2->muNewSpeedL = pop2->muNewSpeedL + release;
				pop2->muSpeedAL = pop2->muNewSpeedL / pop2->muSpeedAL;
				if (pop2->muSpeedAL > maxRelease) pop2->muSpeedAL = maxRelease;
			} else {
				pop2->muCoefficientAL = pop2->muCoefficientAL * ((pop2->muSpeedAL * pop2->muSpeedAL) - 1.0);
				pop2->muCoefficientAL = pop2->muCoefficientAL + 1.0;
				pop2->muCoefficientAL = pop2->muCoefficientAL / (pop2->muSpeedAL * pop2->muSpeedAL);
				pop2->muNewSpeedL = pop2->muSpeedAL * (pop2->muSpeedAL - 1.0);
				pop2->muNewSpeedL = pop2->muNewSpeedL + attack;
				pop2->muSpeedAL = pop2->muNewSpeedL / pop2->muSpeedAL;
			}
		} else {
			if (fabs(inputSampleL) > threshold) {
				pop2->muVaryL = threshold / fabs(inputSampleL);
				pop2->muAttackL = sqrt(fabs(pop2->muSpeedBL));
				pop2->muCoefficientBL = pop2->muCoefficientBL * (pop2->muAttackL - 1);
				if (pop2->muVaryL < threshold) pop2->muCoefficientBL = pop2->muCoefficientBL + threshold;
				else pop2->muCoefficientBL = pop2->muCoefficientBL + pop2->muVaryL;
				pop2->muCoefficientBL = pop2->muCoefficientBL / pop2->muAttackL;
				pop2->muNewSpeedL = pop2->muSpeedBL * (pop2->muSpeedBL - 1.0);
				pop2->muNewSpeedL = pop2->muNewSpeedL + release;
				pop2->muSpeedBL = pop2->muNewSpeedL / pop2->muSpeedBL;
				if (pop2->muSpeedBL > maxRelease) pop2->muSpeedBL = maxRelease;
			} else {
				pop2->muCoefficientBL = pop2->muCoefficientBL * ((pop2->muSpeedBL * pop2->muSpeedBL) - 1.0);
				pop2->muCoefficientBL = pop2->muCoefficientBL + 1.0;
				pop2->muCoefficientBL = pop2->muCoefficientBL / (pop2->muSpeedBL * pop2->muSpeedBL);
				pop2->muNewSpeedL = pop2->muSpeedBL * (pop2->muSpeedBL - 1.0);
				pop2->muNewSpeedL = pop2->muNewSpeedL + attack;
				pop2->muSpeedBL = pop2->muNewSpeedL / pop2->muSpeedBL;
			}
		}
		// got coefficients, adjusted speeds for L

		// adjust coefficients for R
		if (pop2->flip) {
			if (fabs(inputSampleR) > threshold) {
				pop2->muVaryR = threshold / fabs(inputSampleR);
				pop2->muAttackR = sqrt(fabs(pop2->muSpeedAR));
				pop2->muCoefficientAR = pop2->muCoefficientAR * (pop2->muAttackR - 1.0);
				if (pop2->muVaryR < threshold) pop2->muCoefficientAR = pop2->muCoefficientAR + threshold;
				else pop2->muCoefficientAR = pop2->muCoefficientAR + pop2->muVaryR;
				pop2->muCoefficientAR = pop2->muCoefficientAR / pop2->muAttackR;
				pop2->muNewSpeedR = pop2->muSpeedAR * (pop2->muSpeedAR - 1.0);
				pop2->muNewSpeedR = pop2->muNewSpeedR + release;
				pop2->muSpeedAR = pop2->muNewSpeedR / pop2->muSpeedAR;
				if (pop2->muSpeedAR > maxRelease) pop2->muSpeedAR = maxRelease;
			} else {
				pop2->muCoefficientAR = pop2->muCoefficientAR * ((pop2->muSpeedAR * pop2->muSpeedAR) - 1.0);
				pop2->muCoefficientAR = pop2->muCoefficientAR + 1.0;
				pop2->muCoefficientAR = pop2->muCoefficientAR / (pop2->muSpeedAR * pop2->muSpeedAR);
				pop2->muNewSpeedR = pop2->muSpeedAR * (pop2->muSpeedAR - 1.0);
				pop2->muNewSpeedR = pop2->muNewSpeedR + attack;
				pop2->muSpeedAR = pop2->muNewSpeedR / pop2->muSpeedAR;
			}
		} else {
			if (fabs(inputSampleR) > threshold) {
				pop2->muVaryR = threshold / fabs(inputSampleR);
				pop2->muAttackR = sqrt(fabs(pop2->muSpeedBR));
				pop2->muCoefficientBR = pop2->muCoefficientBR * (pop2->muAttackR - 1);
				if (pop2->muVaryR < threshold) pop2->muCoefficientBR = pop2->muCoefficientBR + threshold;
				else pop2->muCoefficientBR = pop2->muCoefficientBR + pop2->muVaryR;
				pop2->muCoefficientBR = pop2->muCoefficientBR / pop2->muAttackR;
				pop2->muNewSpeedR = pop2->muSpeedBR * (pop2->muSpeedBR - 1.0);
				pop2->muNewSpeedR = pop2->muNewSpeedR + release;
				pop2->muSpeedBR = pop2->muNewSpeedR / pop2->muSpeedBR;
				if (pop2->muSpeedBR > maxRelease) pop2->muSpeedBR = maxRelease;
			} else {
				pop2->muCoefficientBR = pop2->muCoefficientBR * ((pop2->muSpeedBR * pop2->muSpeedBR) - 1.0);
				pop2->muCoefficientBR = pop2->muCoefficientBR + 1.0;
				pop2->muCoefficientBR = pop2->muCoefficientBR / (pop2->muSpeedBR * pop2->muSpeedBR);
				pop2->muNewSpeedR = pop2->muSpeedBR * (pop2->muSpeedBR - 1.0);
				pop2->muNewSpeedR = pop2->muNewSpeedR + attack;
				pop2->muSpeedBR = pop2->muNewSpeedR / pop2->muSpeedBR;
			}
		}
		// got coefficients, adjusted speeds for R

		if (pop2->flip) {
			inputSampleL *= pow(pop2->muCoefficientAL, 2);
			inputSampleR *= pow(pop2->muCoefficientAR, 2);
		} else {
			inputSampleL *= pow(pop2->muCoefficientBL, 2);
			inputSampleR *= pow(pop2->muCoefficientBR, 2);
		}
		inputSampleL *= muMakeupGain;
		inputSampleR *= muMakeupGain;
		pop2->flip = !pop2->flip;
		// end compressor section

		// begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (pop2->wasPosClipL == true) { // current will be over
			if (inputSampleL < pop2->lastSampleL) pop2->lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
			else pop2->lastSampleL = 0.2491717 + (pop2->lastSampleL * 0.7390851);
		}
		pop2->wasPosClipL = false;
		if (inputSampleL > 0.9549925859) {
			pop2->wasPosClipL = true;
			inputSampleL = 0.7058208 + (pop2->lastSampleL * 0.2609148);
		}
		if (pop2->wasNegClipL == true) { // current will be -over
			if (inputSampleL > pop2->lastSampleL) pop2->lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
			else pop2->lastSampleL = -0.2491717 + (pop2->lastSampleL * 0.7390851);
		}
		pop2->wasNegClipL = false;
		if (inputSampleL < -0.9549925859) {
			pop2->wasNegClipL = true;
			inputSampleL = -0.7058208 + (pop2->lastSampleL * 0.2609148);
		}
		pop2->intermediateL[spacing] = inputSampleL;
		inputSampleL = pop2->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) pop2->intermediateL[x - 1] = pop2->intermediateL[x];
		pop2->lastSampleL = pop2->intermediateL[0]; // run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (pop2->wasPosClipR == true) { // current will be over
			if (inputSampleR < pop2->lastSampleR) pop2->lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
			else pop2->lastSampleR = 0.2491717 + (pop2->lastSampleR * 0.7390851);
		}
		pop2->wasPosClipR = false;
		if (inputSampleR > 0.9549925859) {
			pop2->wasPosClipR = true;
			inputSampleR = 0.7058208 + (pop2->lastSampleR * 0.2609148);
		}
		if (pop2->wasNegClipR == true) { // current will be -over
			if (inputSampleR > pop2->lastSampleR) pop2->lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
			else pop2->lastSampleR = -0.2491717 + (pop2->lastSampleR * 0.7390851);
		}
		pop2->wasNegClipR = false;
		if (inputSampleR < -0.9549925859) {
			pop2->wasNegClipR = true;
			inputSampleR = -0.7058208 + (pop2->lastSampleR * 0.2609148);
		}
		pop2->intermediateR[spacing] = inputSampleR;
		inputSampleR = pop2->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) pop2->intermediateR[x - 1] = pop2->intermediateR[x];
		pop2->lastSampleR = pop2->intermediateR[0]; // run a little buffer to handle this
		// end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code

		if (wet < 1.0) {
			inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pop2->fpdL ^= pop2->fpdL << 13;
		pop2->fpdL ^= pop2->fpdL >> 17;
		pop2->fpdL ^= pop2->fpdL << 5;
		inputSampleL += (((double) pop2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pop2->fpdR ^= pop2->fpdR << 13;
		pop2->fpdR ^= pop2->fpdR >> 17;
		pop2->fpdR ^= pop2->fpdR << 5;
		inputSampleR += (((double) pop2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POP2_URI,
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
