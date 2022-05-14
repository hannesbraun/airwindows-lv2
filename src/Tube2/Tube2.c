#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TUBE2_URI "https://hannesbraun.net/ns/lv2/airwindows/tube2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT = 4,
	TUBE = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputPad;
	const float* tube;

	double previousSampleA;
	double previousSampleB;
	double previousSampleC;
	double previousSampleD;
	double previousSampleE;
	double previousSampleF;

	uint32_t fpdL;
	uint32_t fpdR;
} Tube2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Tube2* tube2 = (Tube2*) calloc(1, sizeof(Tube2));
	tube2->sampleRate = rate;
	return (LV2_Handle) tube2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Tube2* tube2 = (Tube2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			tube2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			tube2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			tube2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			tube2->output[1] = (float*) data;
			break;
		case INPUT:
			tube2->inputPad = (const float*) data;
			break;
		case TUBE:
			tube2->tube = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Tube2* tube2 = (Tube2*) instance;
	tube2->previousSampleA = 0.0;
	tube2->previousSampleB = 0.0;
	tube2->previousSampleC = 0.0;
	tube2->previousSampleD = 0.0;
	tube2->previousSampleE = 0.0;
	tube2->previousSampleF = 0.0;
	tube2->fpdL = 1.0;
	while (tube2->fpdL < 16386) tube2->fpdL = rand() * UINT32_MAX;
	tube2->fpdR = 1.0;
	while (tube2->fpdR < 16386) tube2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Tube2* tube2 = (Tube2*) instance;

	const float* in1 = tube2->input[0];
	const float* in2 = tube2->input[1];
	float* out1 = tube2->output[0];
	float* out2 = tube2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= tube2->sampleRate;

	double inputPad = *tube2->inputPad;
	double iterations = 1.0 - *tube2->tube;
	int powerfactor = (9.0 * iterations) + 1;
	double asymPad = (double)powerfactor;
	double gainscaling = 1.0 / (double)(powerfactor + 1);
	double outputscaling = 1.0 + (1.0 / (double)(powerfactor));

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = tube2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = tube2->fpdR * 1.18e-17;

		if (inputPad < 1.0) {
			inputSampleL *= inputPad;
			inputSampleR *= inputPad;
		}

		if (overallscale > 1.9) {
			double stored = inputSampleL;
			inputSampleL += tube2->previousSampleA;
			tube2->previousSampleA = stored;
			inputSampleL *= 0.5;
			stored = inputSampleR;
			inputSampleR += tube2->previousSampleB;
			tube2->previousSampleB = stored;
			inputSampleR *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;

		//flatten bottom, point top of sine waveshaper L
		inputSampleL /= asymPad;
		double sharpen = -inputSampleL;
		if (sharpen > 0.0) sharpen = 1.0 + sqrt(sharpen);
		else sharpen = 1.0 - sqrt(-sharpen);
		inputSampleL -= inputSampleL * fabs(inputSampleL) * sharpen * 0.25;
		//this will take input from exactly -1.0 to 1.0 max
		inputSampleL *= asymPad;
		//flatten bottom, point top of sine waveshaper R
		inputSampleR /= asymPad;
		sharpen = -inputSampleR;
		if (sharpen > 0.0) sharpen = 1.0 + sqrt(sharpen);
		else sharpen = 1.0 - sqrt(-sharpen);
		inputSampleR -= inputSampleR * fabs(inputSampleR) * sharpen * 0.25;
		//this will take input from exactly -1.0 to 1.0 max
		inputSampleR *= asymPad;
		//end first asym section: later boosting can mitigate the extreme
		//softclipping of one side of the wave
		//and we are asym clipping more when Tube is cranked, to compensate

		//original Tube algorithm: powerfactor widens the more linear region of the wave
		double factor = inputSampleL; //Left channel
		for (int x = 0; x < powerfactor; x++) factor *= inputSampleL;
		if ((powerfactor % 2 == 1) && (inputSampleL != 0.0)) factor = (factor / inputSampleL) * fabs(inputSampleL);
		factor *= gainscaling;
		inputSampleL -= factor;
		inputSampleL *= outputscaling;
		factor = inputSampleR; //Right channel
		for (int x = 0; x < powerfactor; x++) factor *= inputSampleR;
		if ((powerfactor % 2 == 1) && (inputSampleR != 0.0)) factor = (factor / inputSampleR) * fabs(inputSampleR);
		factor *= gainscaling;
		inputSampleR -= factor;
		inputSampleR *= outputscaling;

		if (overallscale > 1.9) {
			double stored = inputSampleL;
			inputSampleL += tube2->previousSampleC;
			tube2->previousSampleC = stored;
			inputSampleL *= 0.5;
			stored = inputSampleR;
			inputSampleR += tube2->previousSampleD;
			tube2->previousSampleD = stored;
			inputSampleR *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average
		//end original Tube. Now we have a boosted fat sound peaking at 0dB exactly

		//hysteresis and spiky fuzz L
		double slew = tube2->previousSampleE - inputSampleL;
		if (overallscale > 1.9) {
			double stored = inputSampleL;
			inputSampleL += tube2->previousSampleE;
			tube2->previousSampleE = stored;
			inputSampleL *= 0.5;
		} else tube2->previousSampleE = inputSampleL; //for this, need previousSampleC always
		if (slew > 0.0) slew = 1.0 + (sqrt(slew) * 0.5);
		else slew = 1.0 - (sqrt(-slew) * 0.5);
		inputSampleL -= inputSampleL * fabs(inputSampleL) * slew * gainscaling;
		//reusing gainscaling that's part of another algorithm
		if (inputSampleL > 0.52) inputSampleL = 0.52;
		if (inputSampleL < -0.52) inputSampleL = -0.52;
		inputSampleL *= 1.923076923076923;
		//hysteresis and spiky fuzz R
		slew = tube2->previousSampleF - inputSampleR;
		if (overallscale > 1.9) {
			double stored = inputSampleR;
			inputSampleR += tube2->previousSampleF;
			tube2->previousSampleF = stored;
			inputSampleR *= 0.5;
		} else tube2->previousSampleF = inputSampleR; //for this, need previousSampleC always
		if (slew > 0.0) slew = 1.0 + (sqrt(slew) * 0.5);
		else slew = 1.0 - (sqrt(-slew) * 0.5);
		inputSampleR -= inputSampleR * fabs(inputSampleR) * slew * gainscaling;
		//reusing gainscaling that's part of another algorithm
		if (inputSampleR > 0.52) inputSampleR = 0.52;
		if (inputSampleR < -0.52) inputSampleR = -0.52;
		inputSampleR *= 1.923076923076923;
		//end hysteresis and spiky fuzz section

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		tube2->fpdL ^= tube2->fpdL << 13;
		tube2->fpdL ^= tube2->fpdL >> 17;
		tube2->fpdL ^= tube2->fpdL << 5;
		inputSampleL += (((double)tube2->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		tube2->fpdR ^= tube2->fpdR << 13;
		tube2->fpdR ^= tube2->fpdR >> 17;
		tube2->fpdR ^= tube2->fpdR << 5;
		inputSampleR += (((double)tube2->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TUBE2_URI,
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
