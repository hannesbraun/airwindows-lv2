#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define INFLAMER_URI "https://hannesbraun.net/ns/lv2/airwindows/inflamer"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	CURVE = 5,
	EFFECT = 6
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* curve;
	const float* effect;

	uint32_t fpdL;
	uint32_t fpdR;
} Inflamer;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Inflamer* inflamer = (Inflamer*) calloc(1, sizeof(Inflamer));
	return (LV2_Handle) inflamer;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Inflamer* inflamer = (Inflamer*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			inflamer->input[0] = (const float*) data;
			break;
		case INPUT_R:
			inflamer->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			inflamer->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			inflamer->output[1] = (float*) data;
			break;
		case DRIVE:
			inflamer->drive = (const float*) data;
			break;
		case CURVE:
			inflamer->curve = (const float*) data;
			break;
		case EFFECT:
			inflamer->effect = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Inflamer* inflamer = (Inflamer*) instance;

	inflamer->fpdL = 1.0;
	while (inflamer->fpdL < 16386) inflamer->fpdL = rand() * UINT32_MAX;
	inflamer->fpdR = 1.0;
	while (inflamer->fpdR < 16386) inflamer->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Inflamer* inflamer = (Inflamer*) instance;

	const float* in1 = inflamer->input[0];
	const float* in2 = inflamer->input[1];
	float* out1 = inflamer->output[0];
	float* out2 = inflamer->output[1];

	double gainControl = (*inflamer->drive * 0.5) + 0.05; // 0.0 to 1.0
	int gainBits = 20; // start beyond maximum attenuation
	if (gainControl > 0.0) gainBits = floor(1.0 / gainControl) - 2;
	if (gainBits < -2) gainBits = -2;
	if (gainBits > 17) gainBits = 17;
	double gain = 1.0;
	switch (gainBits) {
		case 17:
			gain = 0.0;
			break;
		case 16:
			gain = 0.0000152587890625;
			break;
		case 15:
			gain = 0.000030517578125;
			break;
		case 14:
			gain = 0.00006103515625;
			break;
		case 13:
			gain = 0.0001220703125;
			break;
		case 12:
			gain = 0.000244140625;
			break;
		case 11:
			gain = 0.00048828125;
			break;
		case 10:
			gain = 0.0009765625;
			break;
		case 9:
			gain = 0.001953125;
			break;
		case 8:
			gain = 0.00390625;
			break;
		case 7:
			gain = 0.0078125;
			break;
		case 6:
			gain = 0.015625;
			break;
		case 5:
			gain = 0.03125;
			break;
		case 4:
			gain = 0.0625;
			break;
		case 3:
			gain = 0.125;
			break;
		case 2:
			gain = 0.25;
			break;
		case 1:
			gain = 0.5;
			break;
		case 0:
			gain = 1.0;
			break;
		case -1:
			gain = 2.0;
			break;
		case -2:
			gain = 4.0;
			break;
	} // now we have our input trim

	int bitshiftL = 1;
	int bitshiftR = 1;
	double panControl = (*inflamer->curve * 2.0) - 1.0; //-1.0 to 1.0
	double panAttenuation = (1.0 - fabs(panControl));
	int panBits = 20; // start centered
	if (panAttenuation > 0.0) panBits = floor(1.0 / panAttenuation);
	if (panControl > 0.25) bitshiftL += panBits;
	if (panControl < -0.25) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2;
	if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2;
	if (bitshiftR > 17) bitshiftR = 17;
	double negacurve = 0.5;
	double curve = 0.5;
	switch (bitshiftL) {
		case 17:
			negacurve = 0.0;
			break;
		case 16:
			negacurve = 0.0000152587890625;
			break;
		case 15:
			negacurve = 0.000030517578125;
			break;
		case 14:
			negacurve = 0.00006103515625;
			break;
		case 13:
			negacurve = 0.0001220703125;
			break;
		case 12:
			negacurve = 0.000244140625;
			break;
		case 11:
			negacurve = 0.00048828125;
			break;
		case 10:
			negacurve = 0.0009765625;
			break;
		case 9:
			negacurve = 0.001953125;
			break;
		case 8:
			negacurve = 0.00390625;
			break;
		case 7:
			negacurve = 0.0078125;
			break;
		case 6:
			negacurve = 0.015625;
			break;
		case 5:
			negacurve = 0.03125;
			break;
		case 4:
			negacurve = 0.0625;
			break;
		case 3:
			negacurve = 0.125;
			break;
		case 2:
			negacurve = 0.25;
			break;
		case 1:
			negacurve = 0.5;
			break;
		case 0:
			negacurve = 1.0;
			break;
		case -1:
			negacurve = 2.0;
			break;
		case -2:
			negacurve = 4.0;
			break;
	}
	switch (bitshiftR) {
		case 17:
			curve = 0.0;
			break;
		case 16:
			curve = 0.0000152587890625;
			break;
		case 15:
			curve = 0.000030517578125;
			break;
		case 14:
			curve = 0.00006103515625;
			break;
		case 13:
			curve = 0.0001220703125;
			break;
		case 12:
			curve = 0.000244140625;
			break;
		case 11:
			curve = 0.00048828125;
			break;
		case 10:
			curve = 0.0009765625;
			break;
		case 9:
			curve = 0.001953125;
			break;
		case 8:
			curve = 0.00390625;
			break;
		case 7:
			curve = 0.0078125;
			break;
		case 6:
			curve = 0.015625;
			break;
		case 5:
			curve = 0.03125;
			break;
		case 4:
			curve = 0.0625;
			break;
		case 3:
			curve = 0.125;
			break;
		case 2:
			curve = 0.25;
			break;
		case 1:
			curve = 0.5;
			break;
		case 0:
			curve = 1.0;
			break;
		case -1:
			curve = 2.0;
			break;
		case -2:
			curve = 4.0;
			break;
	}
	double effectOut = *inflamer->effect;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = inflamer->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = inflamer->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL *= gain; // input trim
		double inflamerPlus = (inputSampleL * 2.0) - pow(inputSampleL, 2); //+50, very much just second harmonic
		double inflamerMinus = inputSampleL + (pow(inputSampleL, 3) * 0.25) - ((pow(inputSampleL, 2) + pow(inputSampleL, 4)) * 0.0625); //-50
		inputSampleL = (inflamerPlus * curve) + (inflamerMinus * negacurve);
		inputSampleL = (inputSampleL * effectOut) + (drySampleL * (1.0 - effectOut));

		inputSampleR *= gain; // input trim
		inflamerPlus = (inputSampleR * 2.0) - pow(inputSampleR, 2); //+50, very much just second harmonic
		inflamerMinus = inputSampleR + (pow(inputSampleR, 3) * 0.25) - ((pow(inputSampleR, 2) + pow(inputSampleR, 4)) * 0.0625); //-50
		inputSampleR = (inflamerPlus * curve) + (inflamerMinus * negacurve);
		inputSampleR = (inputSampleR * effectOut) + (drySampleR * (1.0 - effectOut));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		inflamer->fpdL ^= inflamer->fpdL << 13;
		inflamer->fpdL ^= inflamer->fpdL >> 17;
		inflamer->fpdL ^= inflamer->fpdL << 5;
		inputSampleL += (((double) inflamer->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		inflamer->fpdR ^= inflamer->fpdR << 13;
		inflamer->fpdR ^= inflamer->fpdR >> 17;
		inflamer->fpdR ^= inflamer->fpdR << 5;
		inputSampleR += (((double) inflamer->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	INFLAMER_URI,
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
