#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE7CHANNEL_URI "https://hannesbraun.net/ns/lv2/airwindows/console7channel"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FADER = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* fader;

	double gainchase;
	double chasespeed;

	double biquadA[15];

	uint32_t fpdL;
	uint32_t fpdR;
} Console7Channel;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console7Channel* console7Channel = (Console7Channel*) calloc(1, sizeof(Console7Channel));
	console7Channel->sampleRate = rate;
	return (LV2_Handle) console7Channel;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console7Channel* console7Channel = (Console7Channel*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console7Channel->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console7Channel->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console7Channel->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console7Channel->output[1] = (float*) data;
			break;
		case FADER:
			console7Channel->fader = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console7Channel* console7Channel = (Console7Channel*) instance;
	console7Channel->gainchase = -1.0;
	console7Channel->chasespeed = 64.0;
	for (int x = 0; x < 15; x++) {
		console7Channel->biquadA[x] = 0.0;
	}
	console7Channel->fpdL = 1.0;
	while (console7Channel->fpdL < 16386) console7Channel->fpdL = rand() * UINT32_MAX;
	console7Channel->fpdR = 1.0;
	while (console7Channel->fpdR < 16386) console7Channel->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console7Channel* console7Channel = (Console7Channel*) instance;

	const float* in1 = console7Channel->input[0];
	const float* in2 = console7Channel->input[1];
	float* out1 = console7Channel->output[0];
	float* out2 = console7Channel->output[1];

	double inputgain = *console7Channel->fader * 1.272019649514069;
	// which is, in fact, the square root of 1.618033988749894848204586...
	// this happens to give us a boost factor where the track continues to get louder even
	// as it saturates and loses a bit of peak energy. Console7Channel channels go to 12! (.272,etc)
	// Neutral gain through the whole system with a full scale sine ia 0.772 on the gain knob
	if (console7Channel->gainchase != inputgain) console7Channel->chasespeed *= 2.0;
	if (console7Channel->chasespeed > sampleFrames) console7Channel->chasespeed = sampleFrames;
	if (console7Channel->gainchase < 0.0) console7Channel->gainchase = inputgain;

	console7Channel->biquadA[0] = 20000.0 / console7Channel->sampleRate;
	console7Channel->biquadA[1] = 1.618033988749894848204586;

	double K = tan(M_PI * console7Channel->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / console7Channel->biquadA[1] + K * K);
	console7Channel->biquadA[2] = K * K * norm;
	console7Channel->biquadA[3] = 2.0 * console7Channel->biquadA[2];
	console7Channel->biquadA[4] = console7Channel->biquadA[2];
	console7Channel->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	console7Channel->biquadA[6] = (1.0 - K / console7Channel->biquadA[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console7Channel->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console7Channel->fpdR * 1.18e-17;

		double outSampleL = console7Channel->biquadA[2] * inputSampleL + console7Channel->biquadA[3] * console7Channel->biquadA[7] + console7Channel->biquadA[4] * console7Channel->biquadA[8] - console7Channel->biquadA[5] * console7Channel->biquadA[9] - console7Channel->biquadA[6] * console7Channel->biquadA[10];
		console7Channel->biquadA[8] = console7Channel->biquadA[7];
		console7Channel->biquadA[7] = inputSampleL;
		inputSampleL = outSampleL;
		console7Channel->biquadA[10] = console7Channel->biquadA[9];
		console7Channel->biquadA[9] = inputSampleL; // DF1 left

		double outSampleR = console7Channel->biquadA[2] * inputSampleR + console7Channel->biquadA[3] * console7Channel->biquadA[11] + console7Channel->biquadA[4] * console7Channel->biquadA[12] - console7Channel->biquadA[5] * console7Channel->biquadA[13] - console7Channel->biquadA[6] * console7Channel->biquadA[14];
		console7Channel->biquadA[12] = console7Channel->biquadA[11];
		console7Channel->biquadA[11] = inputSampleR;
		inputSampleR = outSampleR;
		console7Channel->biquadA[14] = console7Channel->biquadA[13];
		console7Channel->biquadA[13] = inputSampleR; // DF1 right

		console7Channel->chasespeed *= 0.9999;
		console7Channel->chasespeed -= 0.01;
		if (console7Channel->chasespeed < 64.0) console7Channel->chasespeed = 64.0;
		// we have our chase speed compensated for recent fader activity
		console7Channel->gainchase = (((console7Channel->gainchase * console7Channel->chasespeed) + inputgain) / (console7Channel->chasespeed + 1.0));
		// gainchase is chasing the target, as a simple multiply gain factor
		if (1.0 != console7Channel->gainchase) {
			inputSampleL *= pow(console7Channel->gainchase, 3);
			inputSampleR *= pow(console7Channel->gainchase, 3);
		}
		// this trim control cuts back extra hard because we will amplify after the distortion
		// that will shift the distortion/antidistortion curve, in order to make faded settings
		// slightly 'expanded' and fall back in the soundstage, subtly

		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		// this is a version of Spiral blended 80/20 with regular Density ConsoleChannel.
		// It's blending between two different harmonics in the overtones of the algorithm

		if (1.0 != console7Channel->gainchase && 0.0 != console7Channel->gainchase) {
			inputSampleL /= console7Channel->gainchase;
			inputSampleR /= console7Channel->gainchase;
		}
		// we re-amplify after the distortion relative to how much we cut back previously.

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console7Channel->fpdL ^= console7Channel->fpdL << 13;
		console7Channel->fpdL ^= console7Channel->fpdL >> 17;
		console7Channel->fpdL ^= console7Channel->fpdL << 5;
		inputSampleL += (((double) console7Channel->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console7Channel->fpdR ^= console7Channel->fpdR << 13;
		console7Channel->fpdR ^= console7Channel->fpdR >> 17;
		console7Channel->fpdR ^= console7Channel->fpdR << 5;
		inputSampleR += (((double) console7Channel->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE7CHANNEL_URI,
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
