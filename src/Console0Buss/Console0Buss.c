#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSOLE0BUSS_URI "https://hannesbraun.net/ns/lv2/airwindows/console0buss"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	VOL = 4,
	PAN = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* vol;
	const float* pan;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double avgAL;
	double avgAR;
	double avgBL;
	double avgBR;
} Console0Buss;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Console0Buss* console0Buss = (Console0Buss*) calloc(1, sizeof(Console0Buss));
	return (LV2_Handle) console0Buss;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Console0Buss* console0Buss = (Console0Buss*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			console0Buss->input[0] = (const float*) data;
			break;
		case INPUT_R:
			console0Buss->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			console0Buss->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			console0Buss->output[1] = (float*) data;
			break;
		case VOL:
			console0Buss->vol = (const float*) data;
			break;
		case PAN:
			console0Buss->pan = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Console0Buss* console0Buss = (Console0Buss*) instance;
	console0Buss->avgAL = console0Buss->avgAR = console0Buss->avgBL = console0Buss->avgBR = 0.0;
	console0Buss->fpdL = 1.0;
	while (console0Buss->fpdL < 16386) console0Buss->fpdL = rand() * UINT32_MAX;
	console0Buss->fpdR = 1.0;
	while (console0Buss->fpdR < 16386) console0Buss->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Console0Buss* console0Buss = (Console0Buss*) instance;

	const float* in1 = console0Buss->input[0];
	const float* in2 = console0Buss->input[1];
	float* out1 = console0Buss->output[0];
	float* out2 = console0Buss->output[1];

	double gainControl = (*console0Buss->vol * 0.5) + 0.05; // 0.0 to 1.0
	int gainBits = 20; // start beyond maximum attenuation
	if (gainControl > 0.0) gainBits = floor(1.0 / gainControl);
	int bitshiftL = gainBits - 3;
	int bitshiftR = gainBits - 3;
	double panControl = (*console0Buss->pan * 2.0) - 1.0; //-1.0 to 1.0
	double panAttenuation = (1.0 - fabs(panControl));
	int panBits = 20; // start centered
	if (panAttenuation > 0.0) panBits = floor(1.0 / panAttenuation);
	if (panControl > 0.25) bitshiftL += panBits;
	if (panControl < -0.25) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2;
	if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2;
	if (bitshiftR > 17) bitshiftR = 17;
	double gainL = 1.0;
	double gainR = 1.0;
	switch (bitshiftL) {
		case 17:
			gainL = 0.0;
			break;
		case 16:
			gainL = 0.0000152587890625;
			break;
		case 15:
			gainL = 0.000030517578125;
			break;
		case 14:
			gainL = 0.00006103515625;
			break;
		case 13:
			gainL = 0.0001220703125;
			break;
		case 12:
			gainL = 0.000244140625;
			break;
		case 11:
			gainL = 0.00048828125;
			break;
		case 10:
			gainL = 0.0009765625;
			break;
		case 9:
			gainL = 0.001953125;
			break;
		case 8:
			gainL = 0.00390625;
			break;
		case 7:
			gainL = 0.0078125;
			break;
		case 6:
			gainL = 0.015625;
			break;
		case 5:
			gainL = 0.03125;
			break;
		case 4:
			gainL = 0.0625;
			break;
		case 3:
			gainL = 0.125;
			break;
		case 2:
			gainL = 0.25;
			break;
		case 1:
			gainL = 0.5;
			break;
		case 0:
			gainL = 1.0;
			break;
		case -1:
			gainL = 2.0;
			break;
		case -2:
			gainL = 4.0;
			break;
	}
	switch (bitshiftR) {
		case 17:
			gainR = 0.0;
			break;
		case 16:
			gainR = 0.0000152587890625;
			break;
		case 15:
			gainR = 0.000030517578125;
			break;
		case 14:
			gainR = 0.00006103515625;
			break;
		case 13:
			gainR = 0.0001220703125;
			break;
		case 12:
			gainR = 0.000244140625;
			break;
		case 11:
			gainR = 0.00048828125;
			break;
		case 10:
			gainR = 0.0009765625;
			break;
		case 9:
			gainR = 0.001953125;
			break;
		case 8:
			gainR = 0.00390625;
			break;
		case 7:
			gainR = 0.0078125;
			break;
		case 6:
			gainR = 0.015625;
			break;
		case 5:
			gainR = 0.03125;
			break;
		case 4:
			gainR = 0.0625;
			break;
		case 3:
			gainR = 0.125;
			break;
		case 2:
			gainR = 0.25;
			break;
		case 1:
			gainR = 0.5;
			break;
		case 0:
			gainR = 1.0;
			break;
		case -1:
			gainR = 2.0;
			break;
		case -2:
			gainR = 4.0;
			break;
	}
	double temp;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = console0Buss->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = console0Buss->fpdR * 1.18e-17;

		temp = inputSampleL;
		inputSampleL = (inputSampleL + console0Buss->avgAL) * 0.5;
		console0Buss->avgAL = temp;
		temp = inputSampleR;
		inputSampleR = (inputSampleR + console0Buss->avgAR) * 0.5;
		console0Buss->avgAR = temp;

		inputSampleL *= gainL;
		inputSampleR *= gainR;

		if (inputSampleL > 2.8) inputSampleL = 2.8;
		if (inputSampleL < -2.8) inputSampleL = -2.8;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * 2.0) / (3.0 - inputSampleL);
		else inputSampleL = -(inputSampleL * -2.0) / (3.0 + inputSampleL);
		if (inputSampleR > 2.8) inputSampleR = 2.8;
		if (inputSampleR < -2.8) inputSampleR = -2.8;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * 2.0) / (3.0 - inputSampleR);
		else inputSampleR = -(inputSampleR * -2.0) / (3.0 + inputSampleR);
		// BigFastArcSin output stage.

		temp = inputSampleL;
		inputSampleL = (inputSampleL + console0Buss->avgBL) * 0.5;
		console0Buss->avgBL = temp;
		temp = inputSampleR;
		inputSampleR = (inputSampleR + console0Buss->avgBR) * 0.5;
		console0Buss->avgBR = temp;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		console0Buss->fpdL ^= console0Buss->fpdL << 13;
		console0Buss->fpdL ^= console0Buss->fpdL >> 17;
		console0Buss->fpdL ^= console0Buss->fpdL << 5;
		inputSampleL += (((double) console0Buss->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		console0Buss->fpdR ^= console0Buss->fpdR << 13;
		console0Buss->fpdR ^= console0Buss->fpdR >> 17;
		console0Buss->fpdR ^= console0Buss->fpdR << 5;
		inputSampleR += (((double) console0Buss->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLE0BUSS_URI,
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
