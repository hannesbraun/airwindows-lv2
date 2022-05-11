#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TUBE_URI "https://hannesbraun.net/ns/lv2/airwindows/tube"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TUBE = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* tube;

	double previousSampleA;
	double previousSampleB;
	double previousSampleC;
	double previousSampleD;

	uint32_t fpdL;
	uint32_t fpdR;
} Tube;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Tube* tube = (Tube*) calloc(1, sizeof(Tube));
	tube->sampleRate = rate;
	return (LV2_Handle) tube;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Tube* tube = (Tube*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			tube->input[0] = (const float*) data;
			break;
		case INPUT_R:
			tube->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			tube->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			tube->output[1] = (float*) data;
			break;
		case TUBE:
			tube->tube = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Tube* tube = (Tube*) instance;
	tube->previousSampleA = 0.0;
	tube->previousSampleB = 0.0;
	tube->previousSampleC = 0.0;
	tube->previousSampleD = 0.0;
	tube->fpdL = 1.0;
	while (tube->fpdL < 16386) tube->fpdL = rand() * UINT32_MAX;
	tube->fpdR = 1.0;
	while (tube->fpdR < 16386) tube->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Tube* tube = (Tube*) instance;

	const float* in1 = tube->input[0];
	const float* in2 = tube->input[1];
	float* out1 = tube->output[0];
	float* out2 = tube->output[1];
	const float tubeParam = *tube->tube;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= tube->sampleRate;

	double gain = 1.0 + (tubeParam * 0.2246161992650486);
	//this maxes out at +1.76dB, which is the exact difference between what a triangle/saw wave
	//would be, and a sine (the fullest possible wave at the same peak amplitude). Why do this?
	//Because the nature of this plugin is the 'more FAT TUUUUBE fatness!' knob, and because
	//sticking to this amount maximizes that effect on a 'normal' sound that is itself unclipped
	//while confining the resulting 'clipped' area to what is already 'fattened' into a flat
	//and distorted region. You can always put a gain trim in front of it for more distortion,
	//or cascade them in the DAW for more distortion.
	double iterations = 1.0 - tubeParam;
	int powerfactor = (5.0 * iterations) + 1;
	double gainscaling = 1.0 / (double)(powerfactor + 1);
	double outputscaling = 1.0 + (1.0 / (double)(powerfactor));

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = tube->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = tube->fpdR * 1.18e-17;

		if (overallscale > 1.9) {
			double stored = inputSampleL;
			inputSampleL += tube->previousSampleA;
			tube->previousSampleA = stored;
			inputSampleL *= 0.5;
			stored = inputSampleR;
			inputSampleR += tube->previousSampleB;
			tube->previousSampleB = stored;
			inputSampleR *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average

		inputSampleL *= gain;
		inputSampleR *= gain;

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;

		double factor = inputSampleL; //Left channel

		for (int x = 0; x < powerfactor; x++) factor *= inputSampleL;
		//this applies more and more of a 'curve' to the transfer function

		if ((powerfactor % 2 == 1) && (inputSampleL != 0.0)) factor = (factor / inputSampleL) * fabs(inputSampleL);
		//if we would've got an asymmetrical effect this undoes the last step, and then
		//redoes it using an absolute value to make the effect symmetrical again

		factor *= gainscaling;
		inputSampleL -= factor;
		inputSampleL *= outputscaling;

		factor = inputSampleR; //Right channel

		for (int x = 0; x < powerfactor; x++) factor *= inputSampleR;
		//this applies more and more of a 'curve' to the transfer function

		if ((powerfactor % 2 == 1) && (inputSampleR != 0.0)) factor = (factor / inputSampleR) * fabs(inputSampleR);
		//if we would've got an asymmetrical effect this undoes the last step, and then
		//redoes it using an absolute value to make the effect symmetrical again

		factor *= gainscaling;
		inputSampleR -= factor;
		inputSampleR *= outputscaling;

		/*  This is the simplest raw form of the 'fattest' TUBE boost between -1.0 and 1.0

		 if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		 inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*2.0;

		 */

		if (overallscale > 1.9) {
			double stored = inputSampleL;
			inputSampleL += tube->previousSampleC;
			tube->previousSampleC = stored;
			inputSampleL *= 0.5;
			stored = inputSampleR;
			inputSampleR += tube->previousSampleD;
			tube->previousSampleD = stored;
			inputSampleR *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		tube->fpdL ^= tube->fpdL << 13;
		tube->fpdL ^= tube->fpdL >> 17;
		tube->fpdL ^= tube->fpdL << 5;
		inputSampleL += (((double)tube->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		tube->fpdR ^= tube->fpdR << 13;
		tube->fpdR ^= tube->fpdR >> 17;
		tube->fpdR ^= tube->fpdR << 5;
		inputSampleR += (((double)tube->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TUBE_URI,
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
