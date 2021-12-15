#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BAXANDALL_URI "https://hannesbraun.net/ns/lv2/airwindows/baxandall"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TREBLE = 4,
	BASS = 5,
	OUTPUT = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* treble;
	const float* bass;
	const float* outputGain;

	uint32_t fpd;
	//default stuff
	long double trebleAL[9];
	long double trebleBL[9];
	long double bassAL[9];
	long double bassBL[9];

	long double trebleAR[9];
	long double trebleBR[9];
	long double bassAR[9];
	long double bassBR[9];
	bool flip;
} Baxandall;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Baxandall* baxandall = (Baxandall*) calloc(1, sizeof(Baxandall));
	baxandall->sampleRate = rate;
	return (LV2_Handle) baxandall;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Baxandall* baxandall = (Baxandall*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			baxandall->input[0] = (const float*) data;
			break;
		case INPUT_R:
			baxandall->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			baxandall->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			baxandall->output[1] = (float*) data;
			break;
		case TREBLE:
			baxandall->treble = (const float*) data;
			break;
		case BASS:
			baxandall->bass = (const float*) data;
			break;
		case OUTPUT:
			baxandall->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Baxandall* baxandall = (Baxandall*) instance;
	for (int x = 0; x < 9; x++) {
		baxandall->trebleAL[x] = 0.0;
		baxandall->trebleBL[x] = 0.0;
		baxandall->bassAL[x] = 0.0;
		baxandall->bassBL[x] = 0.0;
		baxandall->trebleAR[x] = 0.0;
		baxandall->trebleBR[x] = 0.0;
		baxandall->bassAR[x] = 0.0;
		baxandall->bassBR[x] = 0.0;
	}
	baxandall->flip = false;
	baxandall->fpd = 17;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Baxandall* baxandall = (Baxandall*) instance;

	const float* in1 = baxandall->input[0];
	const float* in2 = baxandall->input[1];
	float* out1 = baxandall->output[0];
	float* out2 = baxandall->output[1];

	double trebleGain = pow(10.0, *baxandall->treble / 20.0);
	double trebleFreq = (4410.0 * trebleGain) / baxandall->sampleRate;
	if (trebleFreq > 0.45) trebleFreq = 0.45;
	baxandall->trebleAL[0] = trebleFreq;
	baxandall->trebleBL[0] = trebleFreq;
	baxandall->trebleAR[0] = trebleFreq;
	baxandall->trebleBR[0] = trebleFreq;
	double bassGain = pow(10.0, (*baxandall->bass) / 20.0);
	double bassFreq = pow(10.0, -(*baxandall->bass) / 20.0);
	bassFreq = (8820.0 * bassFreq) / baxandall->sampleRate;
	if (bassFreq > 0.45) bassFreq = 0.45;
	baxandall->bassAL[0] = bassFreq;
	baxandall->bassBL[0] = bassFreq;
	baxandall->bassAR[0] = bassFreq;
	baxandall->bassBR[0] = bassFreq;
	baxandall->trebleAL[1] = 0.4;
	baxandall->trebleBL[1] = 0.4;
	baxandall->trebleAR[1] = 0.4;
	baxandall->trebleBR[1] = 0.4;
	baxandall->bassAL[1] = 0.2;
	baxandall->bassBL[1] = 0.2;
	baxandall->bassAR[1] = 0.2;
	baxandall->bassBR[1] = 0.2;
	double output = pow(10.0, *baxandall->outputGain / 20.0);

	long double K = tan(M_PI * baxandall->trebleAL[0]);
	long double norm = 1.0 / (1.0 + K / baxandall->trebleAL[1] + K * K);
	baxandall->trebleBL[2] = K * K * norm;
	baxandall->trebleAL[2] = baxandall->trebleBL[2];
	baxandall->trebleBR[2] = baxandall->trebleBL[2];
	baxandall->trebleAR[2] = baxandall->trebleBL[2];
	baxandall->trebleBL[3] = 2.0 * baxandall->trebleAL[2];
	baxandall->trebleAL[3] = baxandall->trebleBL[3];
	baxandall->trebleBR[3] = baxandall->trebleBL[3];
	baxandall->trebleAR[3] = baxandall->trebleBL[3];
	baxandall->trebleBL[4] = baxandall->trebleAL[2];
	baxandall->trebleAL[4] = baxandall->trebleAL[2];
	baxandall->trebleBR[4] = baxandall->trebleAL[2];
	baxandall->trebleAR[4] = baxandall->trebleAL[2];
	baxandall->trebleBL[5] = 2.0 * (K * K - 1.0) * norm;
	baxandall->trebleAL[5] = baxandall->trebleBL[5];
	baxandall->trebleBR[5] = baxandall->trebleBL[5];
	baxandall->trebleAR[5] = baxandall->trebleBL[5];
	baxandall->trebleBL[6] = (1.0 - K / baxandall->trebleAL[1] + K * K) * norm;
	baxandall->trebleAL[6] = baxandall->trebleBL[6];
	baxandall->trebleBR[6] = baxandall->trebleBL[6];
	baxandall->trebleAR[6] = baxandall->trebleBL[6];

	K = tan(M_PI * baxandall->bassAL[0]);
	norm = 1.0 / (1.0 + K / baxandall->bassAL[1] + K * K);
	baxandall->bassBL[2] = K * K * norm;
	baxandall->bassAL[2] = baxandall->bassBL[2];
	baxandall->bassBR[2] = baxandall->bassBL[2];
	baxandall->bassAR[2] = baxandall->bassBL[2];
	baxandall->bassBL[3] = 2.0 * baxandall->bassAL[2];
	baxandall->bassAL[3] = baxandall->bassBL[3];
	baxandall->bassBR[3] = baxandall->bassBL[3];
	baxandall->bassAR[3] = baxandall->bassBL[3];
	baxandall->bassBL[4] = baxandall->bassAL[2];
	baxandall->bassAL[4] = baxandall->bassBL[4];
	baxandall->bassBR[4] = baxandall->bassBL[4];
	baxandall->bassAR[4] = baxandall->bassBL[4];
	baxandall->bassBL[5] = 2.0 * (K * K - 1.0) * norm;
	baxandall->bassAL[5] = baxandall->bassBL[5];
	baxandall->bassBR[5] = baxandall->bassBL[5];
	baxandall->bassAR[5] = baxandall->bassBL[5];
	baxandall->bassBL[6] = (1.0 - K / baxandall->bassAL[1] + K * K) * norm;
	baxandall->bassAL[6] = baxandall->bassBL[6];
	baxandall->bassBR[6] = baxandall->bassBL[6];
	baxandall->bassAR[6] = baxandall->bassBL[6];

	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabsl(inputSampleL) < 1.18e-37) inputSampleL = baxandall->fpd * 1.18e-37;
		if (fabsl(inputSampleR) < 1.18e-37) inputSampleR = baxandall->fpd * 1.18e-37;

		if (output != 1.0) {
			inputSampleL *= output;
			inputSampleR *= output;
		}//gain trim in front of plugin, in case Console stage clips

		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		//encode Console5: good cleanness

		long double trebleSampleL;
		long double bassSampleL;
		long double trebleSampleR;
		long double bassSampleR;

		if (baxandall->flip) {
			trebleSampleL = (inputSampleL * baxandall->trebleAL[2]) + baxandall->trebleAL[7];
			baxandall->trebleAL[7] = (inputSampleL * baxandall->trebleAL[3]) - (trebleSampleL * baxandall->trebleAL[5]) + baxandall->trebleAL[8];
			baxandall->trebleAL[8] = (inputSampleL * baxandall->trebleAL[4]) - (trebleSampleL * baxandall->trebleAL[6]);
			trebleSampleL = inputSampleL - trebleSampleL;

			bassSampleL = (inputSampleL * baxandall->bassAL[2]) + baxandall->bassAL[7];
			baxandall->bassAL[7] = (inputSampleL * baxandall->bassAL[3]) - (bassSampleL * baxandall->bassAL[5]) + baxandall->bassAL[8];
			baxandall->bassAL[8] = (inputSampleL * baxandall->bassAL[4]) - (bassSampleL * baxandall->bassAL[6]);

			trebleSampleR = (inputSampleR * baxandall->trebleAR[2]) + baxandall->trebleAR[7];
			baxandall->trebleAR[7] = (inputSampleR * baxandall->trebleAR[3]) - (trebleSampleR * baxandall->trebleAR[5]) + baxandall->trebleAR[8];
			baxandall->trebleAR[8] = (inputSampleR * baxandall->trebleAR[4]) - (trebleSampleR * baxandall->trebleAR[6]);
			trebleSampleR = inputSampleR - trebleSampleR;

			bassSampleR = (inputSampleR * baxandall->bassAR[2]) + baxandall->bassAR[7];
			baxandall->bassAR[7] = (inputSampleR * baxandall->bassAR[3]) - (bassSampleR * baxandall->bassAR[5]) + baxandall->bassAR[8];
			baxandall->bassAR[8] = (inputSampleR * baxandall->bassAR[4]) - (bassSampleR * baxandall->bassAR[6]);
		} else {
			trebleSampleL = (inputSampleL * baxandall->trebleBL[2]) + baxandall->trebleBL[7];
			baxandall->trebleBL[7] = (inputSampleL * baxandall->trebleBL[3]) - (trebleSampleL * baxandall->trebleBL[5]) + baxandall->trebleBL[8];
			baxandall->trebleBL[8] = (inputSampleL * baxandall->trebleBL[4]) - (trebleSampleL * baxandall->trebleBL[6]);
			trebleSampleL = inputSampleL - trebleSampleL;

			bassSampleL = (inputSampleL * baxandall->bassBL[2]) + baxandall->bassBL[7];
			baxandall->bassBL[7] = (inputSampleL * baxandall->bassBL[3]) - (bassSampleL * baxandall->bassBL[5]) + baxandall->bassBL[8];
			baxandall->bassBL[8] = (inputSampleL * baxandall->bassBL[4]) - (bassSampleL * baxandall->bassBL[6]);

			trebleSampleR = (inputSampleR * baxandall->trebleBR[2]) + baxandall->trebleBR[7];
			baxandall->trebleBR[7] = (inputSampleR * baxandall->trebleBR[3]) - (trebleSampleR * baxandall->trebleBR[5]) + baxandall->trebleBR[8];
			baxandall->trebleBR[8] = (inputSampleR * baxandall->trebleBR[4]) - (trebleSampleR * baxandall->trebleBR[6]);
			trebleSampleR = inputSampleR - trebleSampleR;

			bassSampleR = (inputSampleR * baxandall->bassBR[2]) + baxandall->bassBR[7];
			baxandall->bassBR[7] = (inputSampleR * baxandall->bassBR[3]) - (bassSampleR * baxandall->bassBR[5]) + baxandall->bassBR[8];
			baxandall->bassBR[8] = (inputSampleR * baxandall->bassBR[4]) - (bassSampleR * baxandall->bassBR[6]);
		}
		baxandall->flip = !baxandall->flip;

		trebleSampleL *= trebleGain;
		bassSampleL *= bassGain;
		inputSampleL = bassSampleL + trebleSampleL; //interleaved biquad
		trebleSampleR *= trebleGain;
		bassSampleR *= bassGain;
		inputSampleR = bassSampleR + trebleSampleR; //interleaved biquad

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleL = asin(inputSampleL);
		//amplitude aspect

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleR = asin(inputSampleR);
		//amplitude aspect

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		baxandall->fpd ^= baxandall->fpd << 13;
		baxandall->fpd ^= baxandall->fpd >> 17;
		baxandall->fpd ^= baxandall->fpd << 5;
		inputSampleL += (((double)baxandall->fpd - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		baxandall->fpd ^= baxandall->fpd << 13;
		baxandall->fpd ^= baxandall->fpd >> 17;
		baxandall->fpd ^= baxandall->fpd << 5;
		inputSampleR += (((double)baxandall->fpd - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	BAXANDALL_URI,
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
