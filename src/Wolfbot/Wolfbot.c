#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define WOLFBOT_URI "https://hannesbraun.net/ns/lv2/airwindows/wolfbot"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

enum {
	prevSampL1,
	prevSlewL1,
	accSlewL1,
	prevSampL2,
	prevSlewL2,
	accSlewL2,
	prevSampL3,
	prevSlewL3,
	accSlewL3,
	kalGainL,
	kalOutL,
	prevSampR1,
	prevSlewR1,
	accSlewR1,
	prevSampR2,
	prevSlewR2,
	accSlewR2,
	prevSampR3,
	prevSlewR3,
	accSlewR3,
	kalGainR,
	kalOutR,
	kal_total
};

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];

	double kHP[kal_total];
	double kLP[kal_total];

	uint32_t fpdL;
	uint32_t fpdR;
} Wolfbot;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Wolfbot* wolfbot = (Wolfbot*) calloc(1, sizeof(Wolfbot));
	wolfbot->sampleRate = rate;
	return (LV2_Handle) wolfbot;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Wolfbot* wolfbot = (Wolfbot*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			wolfbot->input[0] = (const float*) data;
			break;
		case INPUT_R:
			wolfbot->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			wolfbot->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			wolfbot->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Wolfbot* wolfbot = (Wolfbot*) instance;

	for (int x = 0; x < kal_total; x++) {
		wolfbot->kHP[x] = 0.0;
		wolfbot->kLP[x] = 0.0;
	}

	wolfbot->fpdL = 1.0;
	while (wolfbot->fpdL < 16386) wolfbot->fpdL = rand() * UINT32_MAX;
	wolfbot->fpdR = 1.0;
	while (wolfbot->fpdR < 16386) wolfbot->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Wolfbot* wolfbot = (Wolfbot*) instance;

	const float* in1 = wolfbot->input[0];
	const float* in2 = wolfbot->input[1];
	float* out1 = wolfbot->output[0];
	float* out2 = wolfbot->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= wolfbot->sampleRate;

	double kalHP = 1.0 - (0.004225 / overallscale);
	double kalLP = 1.0 - (0.954529 / overallscale);

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = wolfbot->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = wolfbot->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		// begin Kalman Filter L
		double dryKal = inputSampleL = inputSampleL * (1.0 - kalHP) * 0.777;
		inputSampleL *= (1.0 - kalHP);
		// set up gain levels to control the beast
		wolfbot->kHP[prevSlewL3] += wolfbot->kHP[prevSampL3] - wolfbot->kHP[prevSampL2];
		wolfbot->kHP[prevSlewL3] *= 0.5;
		wolfbot->kHP[prevSlewL2] += wolfbot->kHP[prevSampL2] - wolfbot->kHP[prevSampL1];
		wolfbot->kHP[prevSlewL2] *= 0.5;
		wolfbot->kHP[prevSlewL1] += wolfbot->kHP[prevSampL1] - inputSampleL;
		wolfbot->kHP[prevSlewL1] *= 0.5;
		// make slews from each set of samples used
		wolfbot->kHP[accSlewL2] += wolfbot->kHP[prevSlewL3] - wolfbot->kHP[prevSlewL2];
		wolfbot->kHP[accSlewL2] *= 0.5;
		wolfbot->kHP[accSlewL1] += wolfbot->kHP[prevSlewL2] - wolfbot->kHP[prevSlewL1];
		wolfbot->kHP[accSlewL1] *= 0.5;
		// differences between slews: rate of change of rate of change
		wolfbot->kHP[accSlewL3] += (wolfbot->kHP[accSlewL2] - wolfbot->kHP[accSlewL1]);
		wolfbot->kHP[accSlewL3] *= 0.5;
		// entering the abyss, what even is this
		wolfbot->kHP[kalOutL] += wolfbot->kHP[prevSampL1] + wolfbot->kHP[prevSlewL2] + wolfbot->kHP[accSlewL3];
		wolfbot->kHP[kalOutL] *= 0.5;
		// resynthesizing predicted result (all iir smoothed)
		wolfbot->kHP[kalGainL] += fabs(dryKal - wolfbot->kHP[kalOutL]) * kalHP * 8.0;
		wolfbot->kHP[kalGainL] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (wolfbot->kHP[kalGainL] > kalHP * 0.5) wolfbot->kHP[kalGainL] = kalHP * 0.5;
		// attempts to avoid explosions
		wolfbot->kHP[kalOutL] += (dryKal * (1.0 - (0.68 + (kalHP * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		wolfbot->kHP[prevSampL3] = wolfbot->kHP[prevSampL2];
		wolfbot->kHP[prevSampL2] = wolfbot->kHP[prevSampL1];
		wolfbot->kHP[prevSampL1] = (wolfbot->kHP[kalGainL] * wolfbot->kHP[kalOutL]) + ((1.0 - wolfbot->kHP[kalGainL]) * dryKal);
		// feed the chain of previous samples
		if (wolfbot->kHP[prevSampL1] > 1.0) wolfbot->kHP[prevSampL1] = 1.0;
		if (wolfbot->kHP[prevSampL1] < -1.0) wolfbot->kHP[prevSampL1] = -1.0;
		// end Kalman Filter, except for trim on output
		inputSampleL = drySampleL + (wolfbot->kHP[kalOutL] * -0.777); // highpass

		// begin Kalman Filter L
		dryKal = inputSampleL = inputSampleL * (1.0 - kalLP) * 0.777;
		inputSampleL *= (1.0 - kalLP);
		// set up gain levels to control the beast
		wolfbot->kLP[prevSlewL3] += wolfbot->kLP[prevSampL3] - wolfbot->kLP[prevSampL2];
		wolfbot->kLP[prevSlewL3] *= 0.5;
		wolfbot->kLP[prevSlewL2] += wolfbot->kLP[prevSampL2] - wolfbot->kLP[prevSampL1];
		wolfbot->kLP[prevSlewL2] *= 0.5;
		wolfbot->kLP[prevSlewL1] += wolfbot->kLP[prevSampL1] - inputSampleL;
		wolfbot->kLP[prevSlewL1] *= 0.5;
		// make slews from each set of samples used
		wolfbot->kLP[accSlewL2] += wolfbot->kLP[prevSlewL3] - wolfbot->kLP[prevSlewL2];
		wolfbot->kLP[accSlewL2] *= 0.5;
		wolfbot->kLP[accSlewL1] += wolfbot->kLP[prevSlewL2] - wolfbot->kLP[prevSlewL1];
		wolfbot->kLP[accSlewL1] *= 0.5;
		// differences between slews: rate of change of rate of change
		wolfbot->kLP[accSlewL3] += (wolfbot->kLP[accSlewL2] - wolfbot->kLP[accSlewL1]);
		wolfbot->kLP[accSlewL3] *= 0.5;
		// entering the abyss, what even is this
		wolfbot->kLP[kalOutL] += wolfbot->kLP[prevSampL1] + wolfbot->kLP[prevSlewL2] + wolfbot->kLP[accSlewL3];
		wolfbot->kLP[kalOutL] *= 0.5;
		// resynthesizing predicted result (all iir smoothed)
		wolfbot->kLP[kalGainL] += fabs(dryKal - wolfbot->kLP[kalOutL]) * kalLP * 8.0;
		wolfbot->kLP[kalGainL] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (wolfbot->kLP[kalGainL] > kalLP * 0.5) wolfbot->kLP[kalGainL] = kalLP * 0.5;
		// attempts to avoid explosions
		wolfbot->kLP[kalOutL] += (dryKal * (1.0 - (0.68 + (kalLP * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		wolfbot->kLP[prevSampL3] = wolfbot->kLP[prevSampL2];
		wolfbot->kLP[prevSampL2] = wolfbot->kLP[prevSampL1];
		wolfbot->kLP[prevSampL1] = (wolfbot->kLP[kalGainL] * wolfbot->kLP[kalOutL]) + ((1.0 - wolfbot->kLP[kalGainL]) * dryKal);
		// feed the chain of previous samples
		if (wolfbot->kLP[prevSampL1] > 1.0) wolfbot->kLP[prevSampL1] = 1.0;
		if (wolfbot->kLP[prevSampL1] < -1.0) wolfbot->kLP[prevSampL1] = -1.0;
		// end Kalman Filter, except for trim on output
		inputSampleL = sin(wolfbot->kLP[kalOutL] * 0.7943) * 1.2589; // lowpass

		// begin Kalman Filter R
		dryKal = inputSampleR = inputSampleR * (1.0 - kalHP) * 0.777;
		inputSampleR *= (1.0 - kalHP);
		// set up gain levels to control the beast
		wolfbot->kHP[prevSlewR3] += wolfbot->kHP[prevSampR3] - wolfbot->kHP[prevSampR2];
		wolfbot->kHP[prevSlewR3] *= 0.5;
		wolfbot->kHP[prevSlewR2] += wolfbot->kHP[prevSampR2] - wolfbot->kHP[prevSampR1];
		wolfbot->kHP[prevSlewR2] *= 0.5;
		wolfbot->kHP[prevSlewR1] += wolfbot->kHP[prevSampR1] - inputSampleR;
		wolfbot->kHP[prevSlewR1] *= 0.5;
		// make slews from each set of samples used
		wolfbot->kHP[accSlewR2] += wolfbot->kHP[prevSlewR3] - wolfbot->kHP[prevSlewR2];
		wolfbot->kHP[accSlewR2] *= 0.5;
		wolfbot->kHP[accSlewR1] += wolfbot->kHP[prevSlewR2] - wolfbot->kHP[prevSlewR1];
		wolfbot->kHP[accSlewR1] *= 0.5;
		// differences between slews: rate of change of rate of change
		wolfbot->kHP[accSlewR3] += (wolfbot->kHP[accSlewR2] - wolfbot->kHP[accSlewR1]);
		wolfbot->kHP[accSlewR3] *= 0.5;
		// entering the abyss, what even is this
		wolfbot->kHP[kalOutR] += wolfbot->kHP[prevSampR1] + wolfbot->kHP[prevSlewR2] + wolfbot->kHP[accSlewR3];
		wolfbot->kHP[kalOutR] *= 0.5;
		// resynthesizing predicted result (all iir smoothed)
		wolfbot->kHP[kalGainR] += fabs(dryKal - wolfbot->kHP[kalOutR]) * kalHP * 8.0;
		wolfbot->kHP[kalGainR] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (wolfbot->kHP[kalGainR] > kalHP * 0.5) wolfbot->kHP[kalGainR] = kalHP * 0.5;
		// attempts to avoid explosions
		wolfbot->kHP[kalOutR] += (dryKal * (1.0 - (0.68 + (kalHP * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		wolfbot->kHP[prevSampR3] = wolfbot->kHP[prevSampR2];
		wolfbot->kHP[prevSampR2] = wolfbot->kHP[prevSampR1];
		wolfbot->kHP[prevSampR1] = (wolfbot->kHP[kalGainR] * wolfbot->kHP[kalOutR]) + ((1.0 - wolfbot->kHP[kalGainR]) * dryKal);
		// feed the chain of previous samples
		if (wolfbot->kHP[prevSampR1] > 1.0) wolfbot->kHP[prevSampR1] = 1.0;
		if (wolfbot->kHP[prevSampR1] < -1.0) wolfbot->kHP[prevSampR1] = -1.0;
		// end Kalman Filter, except for trim on output
		inputSampleR = drySampleR + (wolfbot->kHP[kalOutR] * -0.777); // highpass

		// begin Kalman Filter R
		dryKal = inputSampleR = inputSampleR * (1.0 - kalLP) * 0.777;
		inputSampleR *= (1.0 - kalLP);
		// set up gain levels to control the beast
		wolfbot->kLP[prevSlewR3] += wolfbot->kLP[prevSampR3] - wolfbot->kLP[prevSampR2];
		wolfbot->kLP[prevSlewR3] *= 0.5;
		wolfbot->kLP[prevSlewR2] += wolfbot->kLP[prevSampR2] - wolfbot->kLP[prevSampR1];
		wolfbot->kLP[prevSlewR2] *= 0.5;
		wolfbot->kLP[prevSlewR1] += wolfbot->kLP[prevSampR1] - inputSampleR;
		wolfbot->kLP[prevSlewR1] *= 0.5;
		// make slews from each set of samples used
		wolfbot->kLP[accSlewR2] += wolfbot->kLP[prevSlewR3] - wolfbot->kLP[prevSlewR2];
		wolfbot->kLP[accSlewR2] *= 0.5;
		wolfbot->kLP[accSlewR1] += wolfbot->kLP[prevSlewR2] - wolfbot->kLP[prevSlewR1];
		wolfbot->kLP[accSlewR1] *= 0.5;
		// differences between slews: rate of change of rate of change
		wolfbot->kLP[accSlewR3] += (wolfbot->kLP[accSlewR2] - wolfbot->kLP[accSlewR1]);
		wolfbot->kLP[accSlewR3] *= 0.5;
		// entering the abyss, what even is this
		wolfbot->kLP[kalOutR] += wolfbot->kLP[prevSampR1] + wolfbot->kLP[prevSlewR2] + wolfbot->kLP[accSlewR3];
		wolfbot->kLP[kalOutR] *= 0.5;
		// resynthesizing predicted result (all iir smoothed)
		wolfbot->kLP[kalGainR] += fabs(dryKal - wolfbot->kLP[kalOutR]) * kalLP * 8.0;
		wolfbot->kLP[kalGainR] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (wolfbot->kLP[kalGainR] > kalLP * 0.5) wolfbot->kLP[kalGainR] = kalLP * 0.5;
		// attempts to avoid explosions
		wolfbot->kLP[kalOutR] += (dryKal * (1.0 - (0.68 + (kalLP * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		wolfbot->kLP[prevSampR3] = wolfbot->kLP[prevSampR2];
		wolfbot->kLP[prevSampR2] = wolfbot->kLP[prevSampR1];
		wolfbot->kLP[prevSampR1] = (wolfbot->kLP[kalGainR] * wolfbot->kLP[kalOutR]) + ((1.0 - wolfbot->kLP[kalGainR]) * dryKal);
		// feed the chain of previous samples
		if (wolfbot->kLP[prevSampR1] > 1.0) wolfbot->kLP[prevSampR1] = 1.0;
		if (wolfbot->kLP[prevSampR1] < -1.0) wolfbot->kLP[prevSampR1] = -1.0;
		// end Kalman Filter, except for trim on output
		inputSampleR = sin(wolfbot->kLP[kalOutR] * 0.7943) * 1.2589; // lowpass

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		wolfbot->fpdL ^= wolfbot->fpdL << 13;
		wolfbot->fpdL ^= wolfbot->fpdL >> 17;
		wolfbot->fpdL ^= wolfbot->fpdL << 5;
		inputSampleL += (((double) wolfbot->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		wolfbot->fpdR ^= wolfbot->fpdR << 13;
		wolfbot->fpdR ^= wolfbot->fpdR >> 17;
		wolfbot->fpdR ^= wolfbot->fpdR << 5;
		inputSampleR += (((double) wolfbot->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	WOLFBOT_URI,
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
