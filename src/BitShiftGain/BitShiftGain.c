#include <lv2/core/lv2.h>

#include <stdint.h>
#include <stdlib.h>

#define BITSHIFTGAIN_URI "https://hannesbraun.net/ns/lv2/airwindows/bitshiftgain"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BIT_SHIFT = 4
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* bitshift;
} BitShiftGain;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BitShiftGain* bitshiftgain = (BitShiftGain*) calloc(1, sizeof(BitShiftGain));
	return (LV2_Handle) bitshiftgain;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BitShiftGain* bitshiftgain = (BitShiftGain*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			bitshiftgain->input[0] = (const float*) data;
			break;
		case INPUT_R:
			bitshiftgain->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			bitshiftgain->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			bitshiftgain->output[1] = (float*) data;
			break;
		case BIT_SHIFT:
			bitshiftgain->bitshift = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance) {}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BitShiftGain* bitshiftgain = (BitShiftGain*) instance;

	const float* in1 = bitshiftgain->input[0];
	const float* in2 = bitshiftgain->input[1];
	float* out1 = bitshiftgain->output[0];
	float* out2 = bitshiftgain->output[1];
	const int bitshift = (int) * bitshiftgain->bitshift;

	double gain = 1.0;
	switch (bitshift) {
		case -16:
			gain = 0.0000152587890625;
			break;
		case -15:
			gain = 0.000030517578125;
			break;
		case -14:
			gain = 0.00006103515625;
			break;
		case -13:
			gain = 0.0001220703125;
			break;
		case -12:
			gain = 0.000244140625;
			break;
		case -11:
			gain = 0.00048828125;
			break;
		case -10:
			gain = 0.0009765625;
			break;
		case -9:
			gain = 0.001953125;
			break;
		case -8:
			gain = 0.00390625;
			break;
		case -7:
			gain = 0.0078125;
			break;
		case -6:
			gain = 0.015625;
			break;
		case -5:
			gain = 0.03125;
			break;
		case -4:
			gain = 0.0625;
			break;
		case -3:
			gain = 0.125;
			break;
		case -2:
			gain = 0.25;
			break;
		case -1:
			gain = 0.5;
			break;
		case 0:
			gain = 1.0;
			break;
		case 1:
			gain = 2.0;
			break;
		case 2:
			gain = 4.0;
			break;
		case 3:
			gain = 8.0;
			break;
		case 4:
			gain = 16.0;
			break;
		case 5:
			gain = 32.0;
			break;
		case 6:
			gain = 64.0;
			break;
		case 7:
			gain = 128.0;
			break;
		case 8:
			gain = 256.0;
			break;
		case 9:
			gain = 512.0;
			break;
		case 10:
			gain = 1024.0;
			break;
		case 11:
			gain = 2048.0;
			break;
		case 12:
			gain = 4096.0;
			break;
		case 13:
			gain = 8192.0;
			break;
		case 14:
			gain = 16384.0;
			break;
		case 15:
			gain = 32768.0;
			break;
		case 16:
			gain = 65536.0;
			break;
	}

	while (sampleFrames-- > 0) {
		*out1 = *in1 * gain;
		*out2 = *in2 * gain;

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
	BITSHIFTGAIN_URI,
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
