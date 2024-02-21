#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define HULL2_URI "https://hannesbraun.net/ns/lv2/airwindows/hull2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TREBLE = 4,
	MID = 5,
	BASS = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* treble;
	const float* mid;
	const float* bass;

	double hullL[225];
	double hullR[225];
	int hullp;
	double hullbL[5];
	double hullbR[5];

	uint32_t fpdL;
	uint32_t fpdR;
} Hull2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Hull2* hull2 = (Hull2*) calloc(1, sizeof(Hull2));
	hull2->sampleRate = rate;
	return (LV2_Handle) hull2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Hull2* hull2 = (Hull2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			hull2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			hull2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			hull2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			hull2->output[1] = (float*) data;
			break;
		case TREBLE:
			hull2->treble = (const float*) data;
			break;
		case MID:
			hull2->mid = (const float*) data;
			break;
		case BASS:
			hull2->bass = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Hull2* hull2 = (Hull2*) instance;
	for (int count = 0; count < 222; count++) {
		hull2->hullL[count] = 0.0;
		hull2->hullR[count] = 0.0;
	}
	for (int count = 0; count < 4; count++) {
		hull2->hullbL[count] = 0.0;
		hull2->hullbR[count] = 0.0;
	}
	hull2->hullp = 1;

	hull2->fpdL = 1.0;
	while (hull2->fpdL < 16386) hull2->fpdL = rand() * UINT32_MAX;
	hull2->fpdR = 1.0;
	while (hull2->fpdR < 16386) hull2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Hull2* hull2 = (Hull2*) instance;

	const float* in1 = hull2->input[0];
	const float* in2 = hull2->input[1];
	float* out1 = hull2->output[0];
	float* out2 = hull2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= hull2->sampleRate;

	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// max out at 4x, 192k

	double treble = *hull2->treble * 2.0;
	double mid = *hull2->mid * 2.0;
	double bass = *hull2->bass * 2.0;
	double iirAmount = 0.125 / cycleEnd;
	int limit = 4 * cycleEnd;
	double divisor = 2.0 / limit;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = hull2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = hull2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		// begin Hull2 treble crossover
		hull2->hullp--;
		if (hull2->hullp < 0) hull2->hullp += 60;

		hull2->hullL[hull2->hullp] = hull2->hullL[hull2->hullp + 60] = inputSampleL;
		hull2->hullR[hull2->hullp] = hull2->hullR[hull2->hullp + 60] = inputSampleR;
		int x = hull2->hullp;
		double midSampleL = 0.0;
		double midSampleR = 0.0;
		while (x < hull2->hullp + (limit / 2)) {
			midSampleL += hull2->hullL[x] * divisor;
			midSampleR += hull2->hullR[x] * divisor;
			x++;
		}
		midSampleL += midSampleL * 0.125;
		midSampleR += midSampleR * 0.125;
		while (x < hull2->hullp + limit) {
			midSampleL -= hull2->hullL[x] * 0.125 * divisor;
			midSampleR -= hull2->hullR[x] * 0.125 * divisor;
			x++;
		}
		hull2->hullL[hull2->hullp + 20] = hull2->hullL[hull2->hullp + 80] = midSampleL;
		hull2->hullR[hull2->hullp + 20] = hull2->hullR[hull2->hullp + 80] = midSampleR;
		x = hull2->hullp + 20;
		midSampleL = midSampleR = 0.0;
		while (x < hull2->hullp + 20 + (limit / 2)) {
			midSampleL += hull2->hullL[x] * divisor;
			midSampleR += hull2->hullR[x] * divisor;
			x++;
		}
		midSampleL += midSampleL * 0.125;
		midSampleR += midSampleR * 0.125;
		while (x < hull2->hullp + 20 + limit) {
			midSampleL -= hull2->hullL[x] * 0.125 * divisor;
			midSampleR -= hull2->hullR[x] * 0.125 * divisor;
			x++;
		}
		hull2->hullL[hull2->hullp + 40] = hull2->hullL[hull2->hullp + 100] = midSampleL;
		hull2->hullR[hull2->hullp + 40] = hull2->hullR[hull2->hullp + 100] = midSampleR;
		x = hull2->hullp + 40;
		midSampleL = midSampleR = 0.0;
		while (x < hull2->hullp + 40 + (limit / 2)) {
			midSampleL += hull2->hullL[x] * divisor;
			midSampleR += hull2->hullR[x] * divisor;
			x++;
		}
		midSampleL += midSampleL * 0.125;
		midSampleR += midSampleR * 0.125;
		while (x < hull2->hullp + 40 + limit) {
			midSampleL -= hull2->hullL[x] * 0.125 * divisor;
			midSampleR -= hull2->hullR[x] * 0.125 * divisor;
			x++;
		}
		double trebleSampleL = drySampleL - midSampleL;
		double trebleSampleR = drySampleR - midSampleR;
		// end Hull2 treble crossover

		// begin Hull2 midbass crossover
		double bassSampleL = midSampleL;
		double bassSampleR = midSampleR;
		x = 0;
		while (x < 3) {
			hull2->hullbL[x] = (hull2->hullbL[x] * (1.0 - iirAmount)) + (bassSampleL * iirAmount);
			hull2->hullbR[x] = (hull2->hullbR[x] * (1.0 - iirAmount)) + (bassSampleR * iirAmount);
			if (fabs(hull2->hullbL[x]) < 1.18e-37) hull2->hullbL[x] = 0.0;
			if (fabs(hull2->hullbR[x]) < 1.18e-37) hull2->hullbR[x] = 0.0;
			bassSampleL = hull2->hullbL[x];
			bassSampleR = hull2->hullbR[x];
			x++;
		}
		midSampleL -= bassSampleL;
		midSampleR -= bassSampleR;
		// end Hull2 midbass crossover

		inputSampleL = (bassSampleL * bass) + (midSampleL * mid) + (trebleSampleL * treble);
		inputSampleR = (bassSampleR * bass) + (midSampleR * mid) + (trebleSampleR * treble);

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		hull2->fpdL ^= hull2->fpdL << 13;
		hull2->fpdL ^= hull2->fpdL >> 17;
		hull2->fpdL ^= hull2->fpdL << 5;
		inputSampleL += (((double) hull2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		hull2->fpdR ^= hull2->fpdR << 13;
		hull2->fpdR ^= hull2->fpdR >> 17;
		hull2->fpdR ^= hull2->fpdR << 5;
		inputSampleR += (((double) hull2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	HULL2_URI,
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
