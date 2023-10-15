#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI_2 1.57079632679489661923132169163975144
#define M_PI_4 0.785398163397448309615660845819875721

#define RESEQ2_URI "https://hannesbraun.net/ns/lv2/airwindows/reseq2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	M_SWEEP = 4,
	M_BOOST = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* mSweep;
	const float* mBoost;

	double mpkL[2005];
	double mpkR[2005];
	double f[66];
	double prevfreqMPeak;
	double prevamountMPeak;
	int mpc;

	uint32_t fpdL;
	uint32_t fpdR;
} ResEQ2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ResEQ2* resEQ2 = (ResEQ2*) calloc(1, sizeof(ResEQ2));
	resEQ2->sampleRate = rate;
	return (LV2_Handle) resEQ2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ResEQ2* resEQ2 = (ResEQ2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			resEQ2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			resEQ2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			resEQ2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			resEQ2->output[1] = (float*) data;
			break;
		case M_SWEEP:
			resEQ2->mSweep = (const float*) data;
			break;
		case M_BOOST:
			resEQ2->mBoost = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ResEQ2* resEQ2 = (ResEQ2*) instance;
	for (int count = 0; count < 2004; count++) {
		resEQ2->mpkL[count] = 0.0;
		resEQ2->mpkR[count] = 0.0;
	}
	for (int count = 0; count < 65; count++) {
		resEQ2->f[count] = 0.0;
	}
	resEQ2->prevfreqMPeak = -1;
	resEQ2->prevamountMPeak = -1;
	resEQ2->mpc = 1;

	resEQ2->fpdL = 1.0;
	while (resEQ2->fpdL < 16386) resEQ2->fpdL = rand() * UINT32_MAX;
	resEQ2->fpdR = 1.0;
	while (resEQ2->fpdR < 16386) resEQ2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ResEQ2* resEQ2 = (ResEQ2*) instance;

	const float* in1 = resEQ2->input[0];
	const float* in2 = resEQ2->input[1];
	float* out1 = resEQ2->output[0];
	float* out2 = resEQ2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= resEQ2->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k

	// begin ResEQ2 Mid Boost
	double freqMPeak = pow(*resEQ2->mSweep + 0.15, 3);
	double amountMPeak = pow(*resEQ2->mBoost, 2);
	int maxMPeak = (amountMPeak * 63.0) + 1;
	if ((freqMPeak != resEQ2->prevfreqMPeak) || (amountMPeak != resEQ2->prevamountMPeak)) {
		for (int x = 0; x < maxMPeak; x++) {
			if (((double) x * freqMPeak) < M_PI_4) resEQ2->f[x] = sin(((double) x * freqMPeak) * 4.0) * freqMPeak * sin(((double) (maxMPeak - x) / (double) maxMPeak) * M_PI_2);
			else resEQ2->f[x] = cos((double) x * freqMPeak) * freqMPeak * sin(((double) (maxMPeak - x) / (double) maxMPeak) * M_PI_2);
		}
		resEQ2->prevfreqMPeak = freqMPeak;
		resEQ2->prevamountMPeak = amountMPeak;
	} // end ResEQ2 Mid Boost

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = resEQ2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = resEQ2->fpdR * 1.18e-17;

		// begin ResEQ2 Mid Boost
		resEQ2->mpc++;
		if (resEQ2->mpc < 1 || resEQ2->mpc > 2001) resEQ2->mpc = 1;
		resEQ2->mpkL[resEQ2->mpc] = inputSampleL;
		resEQ2->mpkR[resEQ2->mpc] = inputSampleR;
		double midMPeakL = 0.0;
		double midMPeakR = 0.0;
		for (int x = 0; x < maxMPeak; x++) {
			int y = x * cycleEnd;
			switch (cycleEnd) {
				case 1:
					midMPeakL += (resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]);
					midMPeakR += (resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]);
					break;
				case 2:
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.5);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.5);
					y--;
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.5);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.5);
					break;
				case 3:
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.333);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.333);
					y--;
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.333);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.333);
					y--;
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.333);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.333);
					break;
				case 4:
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					y--;
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					y--;
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					y--;
					midMPeakL += ((resEQ2->mpkL[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25);
					midMPeakR += ((resEQ2->mpkR[(resEQ2->mpc - y) + ((resEQ2->mpc - y < 1) ? 2001 : 0)] * resEQ2->f[x]) * 0.25); // break
			}
		}
		inputSampleL = (midMPeakL * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? inputSampleL : inputSampleL * (1.5 - amountMPeak));
		inputSampleR = (midMPeakR * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? inputSampleR : inputSampleR * (1.5 - amountMPeak));
		// end ResEQ2 Mid Boost

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		resEQ2->fpdL ^= resEQ2->fpdL << 13;
		resEQ2->fpdL ^= resEQ2->fpdL >> 17;
		resEQ2->fpdL ^= resEQ2->fpdL << 5;
		inputSampleL += (((double) resEQ2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		resEQ2->fpdR ^= resEQ2->fpdR << 13;
		resEQ2->fpdR ^= resEQ2->fpdR >> 17;
		resEQ2->fpdR ^= resEQ2->fpdR << 5;
		inputSampleR += (((double) resEQ2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	RESEQ2_URI,
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
