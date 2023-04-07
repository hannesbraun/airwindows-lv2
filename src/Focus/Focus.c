#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define FOCUS_URI "https://hannesbraun.net/ns/lv2/airwindows/focus"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BOOST = 4,
	FOCUS = 5,
	MODE = 6,
	OUT_GAIN = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* boost;
	const float* focus;
	const float* mode;
	const float* outGain;
	const float* dryWet;

	double figureL[9];
	double figureR[9];
	uint32_t fpdL;
	uint32_t fpdR;
} Focus;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Focus* focus = (Focus*) calloc(1, sizeof(Focus));
	focus->sampleRate = rate;
	return (LV2_Handle) focus;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Focus* focus = (Focus*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			focus->input[0] = (const float*) data;
			break;
		case INPUT_R:
			focus->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			focus->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			focus->output[1] = (float*) data;
			break;
		case BOOST:
			focus->boost = (const float*) data;
			break;
		case FOCUS:
			focus->focus = (const float*) data;
			break;
		case MODE:
			focus->mode = (const float*) data;
			break;
		case OUT_GAIN:
			focus->outGain = (const float*) data;
			break;
		case DRY_WET:
			focus->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Focus* focus = (Focus*) instance;

	for (int x = 0; x < 9; x++) {
		focus->figureL[x] = 0.0;
		focus->figureR[x] = 0.0;
	}
	focus->fpdL = 1.0;
	while (focus->fpdL < 16386) focus->fpdL = rand() * UINT32_MAX;
	focus->fpdR = 1.0;
	while (focus->fpdR < 16386) focus->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Focus* focus = (Focus*) instance;

	const float* in1 = focus->input[0];
	const float* in2 = focus->input[1];
	float* out1 = focus->output[0];
	float* out2 = focus->output[1];

	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	double boost = pow(10.0, *focus->boost / 20.0);
	focus->figureL[0] = 3515.775 / focus->sampleRate; // fixed frequency, 3.515775k
	focus->figureR[0] = focus->figureL[0];
	focus->figureL[1] = pow(pow(*focus->focus, 3) * 2, 2) + 0.0001; // resonance
	focus->figureR[1] = focus->figureL[1];
	int mode = (int) *focus->mode;
	double output = *focus->outGain;
	double wet = *focus->dryWet;

	double K = tan(M_PI * focus->figureR[0]);
	double norm = 1.0 / (1.0 + K / focus->figureR[1] + K * K);
	focus->figureL[2] = K / focus->figureR[1] * norm;
	focus->figureR[2] = focus->figureL[2];
	focus->figureL[4] = -focus->figureR[2];
	focus->figureR[4] = focus->figureL[4];
	focus->figureL[5] = 2.0 * (K * K - 1.0) * norm;
	focus->figureR[5] = focus->figureL[5];
	focus->figureL[6] = (1.0 - K / focus->figureR[1] + K * K) * norm;
	focus->figureR[6] = focus->figureL[6];

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = focus->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = focus->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		// encode Console5: good cleanness

		double tempSample = (inputSampleL * focus->figureL[2]) + focus->figureL[7];
		focus->figureL[7] = -(tempSample * focus->figureL[5]) + focus->figureL[8];
		focus->figureL[8] = (inputSampleL * focus->figureL[4]) - (tempSample * focus->figureL[6]);
		inputSampleL = tempSample;

		tempSample = (inputSampleR * focus->figureR[2]) + focus->figureR[7];
		focus->figureR[7] = -(tempSample * focus->figureR[5]) + focus->figureR[8];
		focus->figureR[8] = (inputSampleR * focus->figureR[4]) - (tempSample * focus->figureR[6]);
		inputSampleR = tempSample;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		// decode Console5

		double groundSampleL = drySampleL - inputSampleL; // set up UnBox
		double groundSampleR = drySampleR - inputSampleR; // set up UnBox
		inputSampleL *= boost; // now, focussed area gets cranked before distort
		inputSampleR *= boost; // now, focussed area gets cranked before distort

		switch (mode) {
			case 1: // Drive
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.6) * (fabs(inputSampleL) * 0.6));
				inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.6) * (fabs(inputSampleR) * 0.6));
				inputSampleL *= 1.6;
				inputSampleR *= 1.6;
				break;
			case 2: // Spiral
				if (inputSampleL > 1.2533141373155) inputSampleL = 1.2533141373155;
				if (inputSampleL < -1.2533141373155) inputSampleL = -1.2533141373155;
				if (inputSampleR > 1.2533141373155) inputSampleR = 1.2533141373155;
				if (inputSampleR < -1.2533141373155) inputSampleR = -1.2533141373155;
				// clip to 1.2533141373155 to reach maximum output
				inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
				inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
				break;
			case 3: { // Mojo
				double mojo;
				mojo = pow(fabs(inputSampleL), 0.25);
				if (mojo > 0.0) inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo) * 0.987654321;
				mojo = pow(fabs(inputSampleR), 0.25);
				if (mojo > 0.0) inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo) * 0.987654321;
				// mojo is the one that flattens WAAAAY out very softly before wavefolding
				break;
			}
			case 4: // Dyno
			case 5: {
				double dyno;
				dyno = pow(fabs(inputSampleL), 4);
				if (dyno > 0.0) inputSampleL = (sin(inputSampleL * dyno) / dyno) * 1.1654321;
				dyno = pow(fabs(inputSampleR), 4);
				if (dyno > 0.0) inputSampleR = (sin(inputSampleR * dyno) / dyno) * 1.1654321;
				// dyno is the one that tries to raise peak energy
				break;
			}
			case 0: // Density
			default:
				if (inputSampleL > 1.570796326794897) inputSampleL = 1.570796326794897;
				if (inputSampleL < -1.570796326794897) inputSampleL = -1.570796326794897;
				if (inputSampleR > 1.570796326794897) inputSampleR = 1.570796326794897;
				if (inputSampleR < -1.570796326794897) inputSampleR = -1.570796326794897;
				// clip to 1.570796326794897 to reach maximum output
				inputSampleL = sin(inputSampleL);
				inputSampleR = sin(inputSampleR);
				break;
		}

		if (output != 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}

		inputSampleL += groundSampleL; // effectively UnBox
		inputSampleR += groundSampleR; // effectively UnBox

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		focus->fpdL ^= focus->fpdL << 13;
		focus->fpdL ^= focus->fpdL >> 17;
		focus->fpdL ^= focus->fpdL << 5;
		inputSampleL += (((double) focus->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		focus->fpdR ^= focus->fpdR << 13;
		focus->fpdR ^= focus->fpdR >> 17;
		focus->fpdR ^= focus->fpdR << 5;
		inputSampleR += (((double) focus->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	FOCUS_URI,
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
