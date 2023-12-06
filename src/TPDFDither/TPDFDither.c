#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define TPDFDITHER_URI "https://hannesbraun.net/ns/lv2/airwindows/tpdfdither"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	QUANTIZER = 4,
	DEREZ = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* quantizer;
	const float* deRez;

	uint32_t fpdL;
	uint32_t fpdR;
} TPDFDither;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	TPDFDither* tpdfDither = (TPDFDither*) calloc(1, sizeof(TPDFDither));
	return (LV2_Handle) tpdfDither;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	TPDFDither* tpdfDither = (TPDFDither*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			tpdfDither->input[0] = (const float*) data;
			break;
		case INPUT_R:
			tpdfDither->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			tpdfDither->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			tpdfDither->output[1] = (float*) data;
			break;
		case QUANTIZER:
			tpdfDither->quantizer = (const float*) data;
			break;
		case DEREZ:
			tpdfDither->deRez = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	TPDFDither* tpdfDither = (TPDFDither*) instance;
	tpdfDither->fpdL = 1.0;
	while (tpdfDither->fpdL < 16386) tpdfDither->fpdL = rand() * UINT32_MAX;
	tpdfDither->fpdR = 1.0;
	while (tpdfDither->fpdR < 16386) tpdfDither->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	TPDFDither* tpdfDither = (TPDFDither*) instance;

	const float* in1 = tpdfDither->input[0];
	const float* in2 = tpdfDither->input[1];
	float* out1 = tpdfDither->output[0];
	float* out2 = tpdfDither->output[1];

	int processing = (int) *tpdfDither->quantizer;
	if (processing >= 2) {
		processing = 1;
	}
	bool highres = false;
	if (processing == 1) highres = true;
	float scaleFactor;
	if (highres) scaleFactor = 8388608.0;
	else scaleFactor = 32768.0;
	float derez = *tpdfDither->deRez;
	if (derez > 0.0) scaleFactor *= pow(1.0 - derez, 6);
	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	float outScale = scaleFactor;
	if (outScale < 8.0) outScale = 8.0;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = tpdfDither->fpdL * 1.18e-17;
		tpdfDither->fpdL ^= tpdfDither->fpdL << 13;
		tpdfDither->fpdL ^= tpdfDither->fpdL >> 17;
		tpdfDither->fpdL ^= tpdfDither->fpdL << 5;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = tpdfDither->fpdR * 1.18e-17;
		tpdfDither->fpdR ^= tpdfDither->fpdR << 13;
		tpdfDither->fpdR ^= tpdfDither->fpdR >> 17;
		tpdfDither->fpdR ^= tpdfDither->fpdR << 5;

		inputSampleL *= scaleFactor;
		inputSampleR *= scaleFactor;
		// 0-1 is now one bit, now we dither

		inputSampleL -= 1.0;
		inputSampleR -= 1.0;

		inputSampleL += ((double) tpdfDither->fpdL / UINT32_MAX);
		inputSampleR += ((double) tpdfDither->fpdR / UINT32_MAX);
		tpdfDither->fpdL ^= tpdfDither->fpdL << 13;
		tpdfDither->fpdL ^= tpdfDither->fpdL >> 17;
		tpdfDither->fpdL ^= tpdfDither->fpdL << 5;
		tpdfDither->fpdR ^= tpdfDither->fpdR << 13;
		tpdfDither->fpdR ^= tpdfDither->fpdR >> 17;
		tpdfDither->fpdR ^= tpdfDither->fpdR << 5;
		inputSampleL += ((double) tpdfDither->fpdL / UINT32_MAX);
		inputSampleR += ((double) tpdfDither->fpdR / UINT32_MAX);

		inputSampleL = floor(inputSampleL);
		inputSampleR = floor(inputSampleR);
		// TPDF: two 0-1 random noises

		inputSampleL /= outScale;
		inputSampleR /= outScale;

		tpdfDither->fpdL ^= tpdfDither->fpdL << 13;
		tpdfDither->fpdL ^= tpdfDither->fpdL >> 17;
		tpdfDither->fpdL ^= tpdfDither->fpdL << 5;
		tpdfDither->fpdR ^= tpdfDither->fpdR << 13;
		tpdfDither->fpdR ^= tpdfDither->fpdR >> 17;
		tpdfDither->fpdR ^= tpdfDither->fpdR << 5;
		// pseudorandom number updater

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
	TPDFDITHER_URI,
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
