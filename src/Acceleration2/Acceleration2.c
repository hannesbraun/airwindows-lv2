#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ACCELERATION2_URI "https://hannesbraun.net/ns/lv2/airwindows/acceleration2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	LIMIT = 4,
	DRYWET = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* limit;
	const float* drywet;

	double lastSampleL;
	double sL[34];
	double m1L;
	double m2L;

	double lastSampleR;
	double sR[34];
	double m1R;
	double m2R;

	long double biquadA[11];
	long double biquadB[11]; //note that this stereo form doesn't require L and R forms!
	//This is because so much of it is coefficients etc. that are the same on both channels.
	//So the stored samples are in 7-8 and 9-10, and freq/res/coefficients serve both.

	uint32_t fpdL;
	uint32_t fpdR;
} Acceleration2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Acceleration2* acceleration2 = (Acceleration2*) calloc(1, sizeof(Acceleration2));
	acceleration2->sampleRate = rate;
	return (LV2_Handle) acceleration2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Acceleration2* acceleration2 = (Acceleration2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			acceleration2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			acceleration2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			acceleration2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			acceleration2->output[1] = (float*) data;
			break;
		case LIMIT:
			acceleration2->limit = (const float*) data;
			break;
		case DRYWET:
			acceleration2->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Acceleration2* acceleration2 = (Acceleration2*) instance;

	acceleration2->lastSampleL = 0.0;
	acceleration2->lastSampleR = 0.0;
	for (int count = 0; count < 33; count++) {
		acceleration2->sL[count] = acceleration2->sR[count] = 0.0;
	}
	acceleration2->m1L = 0.0;
	acceleration2->m2L = 0.0;
	acceleration2->m1R = 0.0;
	acceleration2->m2R = 0.0;
	for (int x = 0; x < 11; x++) {
		acceleration2->biquadA[x] = 0.0;
		acceleration2->biquadB[x] = 0.0;
	}

	acceleration2->fpdL = 1.0;
	while (acceleration2->fpdL < 16386) acceleration2->fpdL = rand() * UINT32_MAX;
	acceleration2->fpdR = 1.0;
	while (acceleration2->fpdR < 16386) acceleration2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Acceleration2* acceleration2 = (Acceleration2*) instance;

	const float* in1 = acceleration2->input[0];
	const float* in2 = acceleration2->input[1];
	float* out1 = acceleration2->output[0];
	float* out2 = acceleration2->output[1];
	const float limit = *acceleration2->limit;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= acceleration2->sampleRate;

	double intensity = pow(limit, 3) * 32;
	float wet = *acceleration2->drywet;
	int spacing = (int)(1.73 * overallscale) + 1;
	if (spacing > 16) spacing = 16;

	acceleration2->biquadA[0] = (20000.0 * (1.0 - (limit * 0.618033988749894848204586))) / acceleration2->sampleRate;
	acceleration2->biquadB[0] = 20000.0 / acceleration2->sampleRate;
	acceleration2-> biquadA[1] = 0.7071;
	acceleration2->biquadB[1] = 0.7071;

	long double K = tan(M_PI * acceleration2->biquadA[0]);
	long double norm = 1.0 / (1.0 + K / acceleration2->biquadA[1] + K * K);
	acceleration2->biquadA[2] = K * K * norm;
	acceleration2->biquadA[3] = 2.0 * acceleration2->biquadA[2];
	acceleration2->biquadA[4] = acceleration2->biquadA[2];
	acceleration2->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	acceleration2->biquadA[6] = (1.0 - K / acceleration2->biquadA[1] + K * K) * norm;

	K = tan(M_PI * acceleration2->biquadB[0]);
	norm = 1.0 / (1.0 + K / acceleration2->biquadB[1] + K * K);
	acceleration2->biquadB[2] = K * K * norm;
	acceleration2->biquadB[3] = 2.0 * acceleration2->biquadB[2];
	acceleration2->biquadB[4] = acceleration2->biquadB[2];
	acceleration2->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	acceleration2->biquadB[6] = (1.0 - K / acceleration2->biquadB[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabsl(inputSampleL) < 1.18e-37) inputSampleL = acceleration2->fpdL * 1.18e-37;
		if (fabsl(inputSampleR) < 1.18e-37) inputSampleR = acceleration2->fpdR * 1.18e-37;
		long double drySampleL = inputSampleL;
		long double drySampleR = inputSampleR;

		long double tempSample = (inputSampleL * acceleration2->biquadA[2]) + acceleration2->biquadA[7];
		acceleration2->biquadA[7] = (inputSampleL * acceleration2->biquadA[3]) - (tempSample * acceleration2->biquadA[5]) + acceleration2->biquadA[8];
		acceleration2->biquadA[8] = (inputSampleL * acceleration2->biquadA[4]) - (tempSample * acceleration2->biquadA[6]);
		double smoothL = tempSample; //like mono AU, 7 and 8 store L channel

		tempSample = (inputSampleR * acceleration2->biquadA[2]) + acceleration2->biquadA[9];
		acceleration2->biquadA[9] = (inputSampleR * acceleration2->biquadA[3]) - (tempSample * acceleration2->biquadA[5]) + acceleration2->biquadA[10];
		acceleration2->biquadA[10] = (inputSampleR * acceleration2->biquadA[4]) - (tempSample * acceleration2->biquadA[6]);
		double smoothR = tempSample; //note: 9 and 10 store the R channel

		for (int count = spacing * 2; count >= 0; count--) {
			acceleration2->sL[count + 1] = acceleration2->sL[count];
			acceleration2->sR[count + 1] = acceleration2->sR[count];
		}
		acceleration2->sL[0] = inputSampleL;
		acceleration2->sR[0] = inputSampleR;

		acceleration2->m1L = (acceleration2->sL[0] - acceleration2->sL[spacing]) * (fabs(acceleration2->sL[0] - acceleration2->sL[spacing]));
		acceleration2->m2L = (acceleration2->sL[spacing] - acceleration2->sL[spacing * 2]) * (fabs(acceleration2->sL[spacing] - acceleration2->sL[spacing * 2]));
		double senseL = (intensity * intensity * fabs(acceleration2->m1L - acceleration2->m2L));
		if (senseL > 1.0) senseL = 1.0;
		inputSampleL = (inputSampleL * (1.0 - senseL)) + (smoothL * senseL);

		acceleration2->m1R = (acceleration2->sR[0] - acceleration2->sR[spacing]) * (fabs(acceleration2->sR[0] - acceleration2->sR[spacing]));
		acceleration2->m2R = (acceleration2->sR[spacing] - acceleration2->sR[spacing * 2]) * (fabs(acceleration2->sR[spacing] - acceleration2->sR[spacing * 2]));
		double senseR = (intensity * intensity * fabs(acceleration2->m1R - acceleration2->m2R));
		if (senseR > 1.0) senseR = 1.0;
		inputSampleR = (inputSampleR * (1.0 - senseR)) + (smoothR * senseR);

		tempSample = (inputSampleL * acceleration2->biquadB[2]) + acceleration2->biquadB[7];
		acceleration2->biquadB[7] = (inputSampleL * acceleration2->biquadB[3]) - (tempSample * acceleration2->biquadB[5]) + acceleration2->biquadB[8];
		acceleration2->biquadB[8] = (inputSampleL * acceleration2->biquadB[4]) - (tempSample * acceleration2->biquadB[6]);
		inputSampleL = tempSample; //like mono AU, 7 and 8 store L channel

		tempSample = (inputSampleR * acceleration2->biquadB[2]) + acceleration2->biquadB[9];
		acceleration2->biquadB[9] = (inputSampleR * acceleration2->biquadB[3]) - (tempSample * acceleration2->biquadB[5]) + acceleration2->biquadB[10];
		acceleration2->biquadB[10] = (inputSampleR * acceleration2->biquadB[4]) - (tempSample * acceleration2->biquadB[6]);
		inputSampleR = tempSample; //note: 9 and 10 store the R channel

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		acceleration2->fpdL ^= acceleration2->fpdL << 13;
		acceleration2->fpdL ^= acceleration2->fpdL >> 17;
		acceleration2->fpdL ^= acceleration2->fpdL << 5;
		inputSampleL += (((double)acceleration2->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		acceleration2->fpdR ^= acceleration2->fpdR << 13;
		acceleration2->fpdR ^= acceleration2->fpdR >> 17;
		acceleration2->fpdR ^= acceleration2->fpdR << 5;
		inputSampleR += (((double)acceleration2->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	ACCELERATION2_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
