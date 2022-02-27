#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SPIRAL_URI "https://hannesbraun.net/ns/lv2/airwindows/spiral"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];

	uint32_t fpd;
} Spiral;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Spiral* spiral = (Spiral*) calloc(1, sizeof(Spiral));
	return (LV2_Handle) spiral;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Spiral* spiral = (Spiral*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			spiral->input[0] = (const float*) data;
			break;
		case INPUT_R:
			spiral->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			spiral->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			spiral->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Spiral* spiral = (Spiral*) instance;
	spiral->fpd = 17;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Spiral* spiral = (Spiral*) instance;

	const float* in1 = spiral->input[0];
	const float* in2 = spiral->input[1];
	float* out1 = spiral->output[0];
	float* out2 = spiral->output[1];

	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;

		if (fabsl(inputSampleL)<1.18e-37) inputSampleL = spiral->fpd * 1.18e-37;
		if (fabsl(inputSampleR)<1.18e-37) inputSampleR = spiral->fpd * 1.18e-37;


		//clip to 1.2533141373155 to reach maximum output
		inputSampleL = sin(inputSampleL * fabsl(inputSampleL)) / ((fabsl(inputSampleL) == 0.0) ?1:fabsl(inputSampleL));
		inputSampleR = sin(inputSampleR * fabsl(inputSampleR)) / ((fabsl(inputSampleR) == 0.0) ?1:fabsl(inputSampleR));

		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		spiral->fpd ^= spiral->fpd << 13; spiral->fpd ^= spiral->fpd >> 17; spiral->fpd ^= spiral->fpd << 5;
		inputSampleL += (int32_t)spiral->fpd * 5.960464655174751e-36L * pow(2,expon+62);
		frexpf((float)inputSampleR, &expon);
		spiral->fpd ^= spiral->fpd << 13; spiral->fpd ^= spiral->fpd >> 17; spiral->fpd ^= spiral->fpd << 5;
		inputSampleR += (int32_t)spiral->fpd * 5.960464655174751e-36L * pow(2,expon+62);
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
	SPIRAL_URI,
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
