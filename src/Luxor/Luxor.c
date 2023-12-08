#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define LUXOR_URI "https://hannesbraun.net/ns/lv2/airwindows/luxor"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	HARDNESS = 4,
	PERSONALITY = 5,
	DRIVE = 6,
	OUTPUT_LEVEL = 7
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* hardness;
	const float* personality;
	const float* drive;
	const float* outputLevel;

	double bR[35];
	double lastSampleR;
	double bL[35];
	double lastSampleL;
	uint32_t fpdL;
	uint32_t fpdR;
} Luxor;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Luxor* luxor = (Luxor*) calloc(1, sizeof(Luxor));
	return (LV2_Handle) luxor;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Luxor* luxor = (Luxor*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			luxor->input[0] = (const float*) data;
			break;
		case INPUT_R:
			luxor->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			luxor->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			luxor->output[1] = (float*) data;
			break;
		case HARDNESS:
			luxor->hardness = (const float*) data;
			break;
		case PERSONALITY:
			luxor->personality = (const float*) data;
			break;
		case DRIVE:
			luxor->drive = (const float*) data;
			break;
		case OUTPUT_LEVEL:
			luxor->outputLevel = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Luxor* luxor = (Luxor*) instance;

	for (int count = 0; count < 34; count++) {
		luxor->bR[count] = 0;
		luxor->bL[count] = 0;
	}
	luxor->lastSampleR = 0.0;
	luxor->lastSampleL = 0.0;
	luxor->fpdL = 1.0;
	while (luxor->fpdL < 16386) luxor->fpdL = rand() * UINT32_MAX;
	luxor->fpdR = 1.0;
	while (luxor->fpdR < 16386) luxor->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Luxor* luxor = (Luxor*) instance;

	const float* in1 = luxor->input[0];
	const float* in2 = luxor->input[1];
	float* out1 = luxor->output[0];
	float* out2 = luxor->output[1];

	double threshold = *luxor->hardness;
	double hardness;
	double breakup = (1.0 - (threshold / 2.0)) * 3.14159265358979;
	double bridgerectifier;
	double sqdrive = *luxor->personality;
	if (sqdrive > 1.0) sqdrive *= sqdrive;
	sqdrive = sqrt(sqdrive);
	double indrive = *luxor->drive;
	if (indrive > 1.0) indrive *= indrive;
	indrive *= (1.0 + (0.1935 * sqdrive));
	// correct for gain loss of convolution
	// calibrate this to match noise level with character at 1.0
	// you get for instance 0.819 and 1.0-0.819 is 0.181
	double randy;
	double outlevel = *luxor->outputLevel;

	if (threshold < 1) hardness = 1.0 / (1.0 - threshold);
	else hardness = 999999999999999999999.0;
	// set up hardness to exactly fill gap between threshold and 0db
	// if threshold is literally 1 then hardness is infinite, so we make it very big

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = luxor->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = luxor->fpdR * 1.18e-17;

		inputSampleL *= indrive;
		inputSampleR *= indrive;
		// calibrated to match gain through convolution and -0.3 correction

		if (sqdrive > 0.0) {
			luxor->bL[33] = luxor->bL[32];
			luxor->bL[32] = luxor->bL[31];
			luxor->bL[31] = luxor->bL[30];
			luxor->bL[30] = luxor->bL[29];
			luxor->bL[29] = luxor->bL[28];
			luxor->bL[28] = luxor->bL[27];
			luxor->bL[27] = luxor->bL[26];
			luxor->bL[26] = luxor->bL[25];
			luxor->bL[25] = luxor->bL[24];
			luxor->bL[24] = luxor->bL[23];
			luxor->bL[23] = luxor->bL[22];
			luxor->bL[22] = luxor->bL[21];
			luxor->bL[21] = luxor->bL[20];
			luxor->bL[20] = luxor->bL[19];
			luxor->bL[19] = luxor->bL[18];
			luxor->bL[18] = luxor->bL[17];
			luxor->bL[17] = luxor->bL[16];
			luxor->bL[16] = luxor->bL[15];
			luxor->bL[15] = luxor->bL[14];
			luxor->bL[14] = luxor->bL[13];
			luxor->bL[13] = luxor->bL[12];
			luxor->bL[12] = luxor->bL[11];
			luxor->bL[11] = luxor->bL[10];
			luxor->bL[10] = luxor->bL[9];
			luxor->bL[9] = luxor->bL[8];
			luxor->bL[8] = luxor->bL[7];
			luxor->bL[7] = luxor->bL[6];
			luxor->bL[6] = luxor->bL[5];
			luxor->bL[5] = luxor->bL[4];
			luxor->bL[4] = luxor->bL[3];
			luxor->bL[3] = luxor->bL[2];
			luxor->bL[2] = luxor->bL[1];
			luxor->bL[1] = luxor->bL[0];
			luxor->bL[0] = inputSampleL * sqdrive;

			inputSampleL -= (luxor->bL[1] * (0.20641602693167951 - (0.00078952185394898 * fabs(luxor->bL[1]))));
			inputSampleL += (luxor->bL[2] * (0.07601816702459827 + (0.00022786334179951 * fabs(luxor->bL[2]))));
			inputSampleL -= (luxor->bL[3] * (0.03929765560019285 - (0.00054517993246352 * fabs(luxor->bL[3]))));
			inputSampleL -= (luxor->bL[4] * (0.00298333157711103 - (0.00033083756545638 * fabs(luxor->bL[4]))));
			inputSampleL += (luxor->bL[5] * (0.00724006282304610 + (0.00045483683460812 * fabs(luxor->bL[5]))));
			inputSampleL -= (luxor->bL[6] * (0.03073108963506036 - (0.00038190060537423 * fabs(luxor->bL[6]))));
			inputSampleL += (luxor->bL[7] * (0.02332434692533051 + (0.00040347288688932 * fabs(luxor->bL[7]))));
			inputSampleL -= (luxor->bL[8] * (0.03792606869061214 - (0.00039673687335892 * fabs(luxor->bL[8]))));
			inputSampleL += (luxor->bL[9] * (0.02437059376675688 + (0.00037221210539535 * fabs(luxor->bL[9]))));
			inputSampleL -= (luxor->bL[10] * (0.03416764311979521 - (0.00040314850796953 * fabs(luxor->bL[10]))));
			inputSampleL += (luxor->bL[11] * (0.01761669868102127 + (0.00035989484330131 * fabs(luxor->bL[11]))));
			inputSampleL -= (luxor->bL[12] * (0.02538237753523052 - (0.00040149119125394 * fabs(luxor->bL[12]))));
			inputSampleL += (luxor->bL[13] * (0.00770737340728377 + (0.00035462118723555 * fabs(luxor->bL[13]))));
			inputSampleL -= (luxor->bL[14] * (0.01580706228482803 - (0.00037563141307594 * fabs(luxor->bL[14]))));
			inputSampleL -= (luxor->bL[15] * (0.00055119240005586 - (0.00035409299268971 * fabs(luxor->bL[15]))));
			inputSampleL -= (luxor->bL[16] * (0.00818552143438768 - (0.00036507661042180 * fabs(luxor->bL[16]))));
			inputSampleL -= (luxor->bL[17] * (0.00661842703548304 - (0.00034550528559056 * fabs(luxor->bL[17]))));
			inputSampleL -= (luxor->bL[18] * (0.00362447476272098 - (0.00035553012761240 * fabs(luxor->bL[18]))));
			inputSampleL -= (luxor->bL[19] * (0.00957098027225745 - (0.00034091691045338 * fabs(luxor->bL[19]))));
			inputSampleL -= (luxor->bL[20] * (0.00193621774016660 - (0.00034554529131668 * fabs(luxor->bL[20]))));
			inputSampleL -= (luxor->bL[21] * (0.01005433027357935 - (0.00033878223153845 * fabs(luxor->bL[21]))));
			inputSampleL -= (luxor->bL[22] * (0.00221712428802004 - (0.00033481410137711 * fabs(luxor->bL[22]))));
			inputSampleL -= (luxor->bL[23] * (0.00911255639207995 - (0.00033263425232666 * fabs(luxor->bL[23]))));
			inputSampleL -= (luxor->bL[24] * (0.00339667169034909 - (0.00032634428038430 * fabs(luxor->bL[24]))));
			inputSampleL -= (luxor->bL[25] * (0.00774096948249924 - (0.00032599868802996 * fabs(luxor->bL[25]))));
			inputSampleL -= (luxor->bL[26] * (0.00463907626773794 - (0.00032131993173361 * fabs(luxor->bL[26]))));
			inputSampleL -= (luxor->bL[27] * (0.00658222997260378 - (0.00032014977430211 * fabs(luxor->bL[27]))));
			inputSampleL -= (luxor->bL[28] * (0.00550347079924993 - (0.00031557153256653 * fabs(luxor->bL[28]))));
			inputSampleL -= (luxor->bL[29] * (0.00588754981375325 - (0.00032041307242303 * fabs(luxor->bL[29]))));
			inputSampleL -= (luxor->bL[30] * (0.00590293898419892 - (0.00030457857428714 * fabs(luxor->bL[30]))));
			inputSampleL -= (luxor->bL[31] * (0.00558952010441800 - (0.00030448053548086 * fabs(luxor->bL[31]))));
			inputSampleL -= (luxor->bL[32] * (0.00598183557634295 - (0.00030715064323181 * fabs(luxor->bL[32]))));
			inputSampleL -= (luxor->bL[33] * (0.00555223929714115 - (0.00030319367948553 * fabs(luxor->bL[33]))));
			// voxbox 40 db

			luxor->bR[33] = luxor->bR[32];
			luxor->bR[32] = luxor->bR[31];
			luxor->bR[31] = luxor->bR[30];
			luxor->bR[30] = luxor->bR[29];
			luxor->bR[29] = luxor->bR[28];
			luxor->bR[28] = luxor->bR[27];
			luxor->bR[27] = luxor->bR[26];
			luxor->bR[26] = luxor->bR[25];
			luxor->bR[25] = luxor->bR[24];
			luxor->bR[24] = luxor->bR[23];
			luxor->bR[23] = luxor->bR[22];
			luxor->bR[22] = luxor->bR[21];
			luxor->bR[21] = luxor->bR[20];
			luxor->bR[20] = luxor->bR[19];
			luxor->bR[19] = luxor->bR[18];
			luxor->bR[18] = luxor->bR[17];
			luxor->bR[17] = luxor->bR[16];
			luxor->bR[16] = luxor->bR[15];
			luxor->bR[15] = luxor->bR[14];
			luxor->bR[14] = luxor->bR[13];
			luxor->bR[13] = luxor->bR[12];
			luxor->bR[12] = luxor->bR[11];
			luxor->bR[11] = luxor->bR[10];
			luxor->bR[10] = luxor->bR[9];
			luxor->bR[9] = luxor->bR[8];
			luxor->bR[8] = luxor->bR[7];
			luxor->bR[7] = luxor->bR[6];
			luxor->bR[6] = luxor->bR[5];
			luxor->bR[5] = luxor->bR[4];
			luxor->bR[4] = luxor->bR[3];
			luxor->bR[3] = luxor->bR[2];
			luxor->bR[2] = luxor->bR[1];
			luxor->bR[1] = luxor->bR[0];
			luxor->bR[0] = inputSampleR * sqdrive;

			inputSampleR -= (luxor->bR[1] * (0.20641602693167951 - (0.00078952185394898 * fabs(luxor->bR[1]))));
			inputSampleR += (luxor->bR[2] * (0.07601816702459827 + (0.00022786334179951 * fabs(luxor->bR[2]))));
			inputSampleR -= (luxor->bR[3] * (0.03929765560019285 - (0.00054517993246352 * fabs(luxor->bR[3]))));
			inputSampleR -= (luxor->bR[4] * (0.00298333157711103 - (0.00033083756545638 * fabs(luxor->bR[4]))));
			inputSampleR += (luxor->bR[5] * (0.00724006282304610 + (0.00045483683460812 * fabs(luxor->bR[5]))));
			inputSampleR -= (luxor->bR[6] * (0.03073108963506036 - (0.00038190060537423 * fabs(luxor->bR[6]))));
			inputSampleR += (luxor->bR[7] * (0.02332434692533051 + (0.00040347288688932 * fabs(luxor->bR[7]))));
			inputSampleR -= (luxor->bR[8] * (0.03792606869061214 - (0.00039673687335892 * fabs(luxor->bR[8]))));
			inputSampleR += (luxor->bR[9] * (0.02437059376675688 + (0.00037221210539535 * fabs(luxor->bR[9]))));
			inputSampleR -= (luxor->bR[10] * (0.03416764311979521 - (0.00040314850796953 * fabs(luxor->bR[10]))));
			inputSampleR += (luxor->bR[11] * (0.01761669868102127 + (0.00035989484330131 * fabs(luxor->bR[11]))));
			inputSampleR -= (luxor->bR[12] * (0.02538237753523052 - (0.00040149119125394 * fabs(luxor->bR[12]))));
			inputSampleR += (luxor->bR[13] * (0.00770737340728377 + (0.00035462118723555 * fabs(luxor->bR[13]))));
			inputSampleR -= (luxor->bR[14] * (0.01580706228482803 - (0.00037563141307594 * fabs(luxor->bR[14]))));
			inputSampleR -= (luxor->bR[15] * (0.00055119240005586 - (0.00035409299268971 * fabs(luxor->bR[15]))));
			inputSampleR -= (luxor->bR[16] * (0.00818552143438768 - (0.00036507661042180 * fabs(luxor->bR[16]))));
			inputSampleR -= (luxor->bR[17] * (0.00661842703548304 - (0.00034550528559056 * fabs(luxor->bR[17]))));
			inputSampleR -= (luxor->bR[18] * (0.00362447476272098 - (0.00035553012761240 * fabs(luxor->bR[18]))));
			inputSampleR -= (luxor->bR[19] * (0.00957098027225745 - (0.00034091691045338 * fabs(luxor->bR[19]))));
			inputSampleR -= (luxor->bR[20] * (0.00193621774016660 - (0.00034554529131668 * fabs(luxor->bR[20]))));
			inputSampleR -= (luxor->bR[21] * (0.01005433027357935 - (0.00033878223153845 * fabs(luxor->bR[21]))));
			inputSampleR -= (luxor->bR[22] * (0.00221712428802004 - (0.00033481410137711 * fabs(luxor->bR[22]))));
			inputSampleR -= (luxor->bR[23] * (0.00911255639207995 - (0.00033263425232666 * fabs(luxor->bR[23]))));
			inputSampleR -= (luxor->bR[24] * (0.00339667169034909 - (0.00032634428038430 * fabs(luxor->bR[24]))));
			inputSampleR -= (luxor->bR[25] * (0.00774096948249924 - (0.00032599868802996 * fabs(luxor->bR[25]))));
			inputSampleR -= (luxor->bR[26] * (0.00463907626773794 - (0.00032131993173361 * fabs(luxor->bR[26]))));
			inputSampleR -= (luxor->bR[27] * (0.00658222997260378 - (0.00032014977430211 * fabs(luxor->bR[27]))));
			inputSampleR -= (luxor->bR[28] * (0.00550347079924993 - (0.00031557153256653 * fabs(luxor->bR[28]))));
			inputSampleR -= (luxor->bR[29] * (0.00588754981375325 - (0.00032041307242303 * fabs(luxor->bR[29]))));
			inputSampleR -= (luxor->bR[30] * (0.00590293898419892 - (0.00030457857428714 * fabs(luxor->bR[30]))));
			inputSampleR -= (luxor->bR[31] * (0.00558952010441800 - (0.00030448053548086 * fabs(luxor->bR[31]))));
			inputSampleR -= (luxor->bR[32] * (0.00598183557634295 - (0.00030715064323181 * fabs(luxor->bR[32]))));
			inputSampleR -= (luxor->bR[33] * (0.00555223929714115 - (0.00030319367948553 * fabs(luxor->bR[33]))));
			// voxbox 40 db
		}

		if (fabs(inputSampleL) > threshold) {
			bridgerectifier = (fabs(inputSampleL) - threshold) * hardness;
			// skip flat area if any, scale to distortion limit
			if (bridgerectifier > breakup) bridgerectifier = breakup;
			// max value for sine function, 'breakup' modeling for trashed console tone
			// more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
			bridgerectifier = sin(bridgerectifier) / hardness;
			// do the sine factor, scale back to proper amount
			if (inputSampleL > 0) inputSampleL = bridgerectifier + threshold;
			else inputSampleL = -(bridgerectifier + threshold);
		}
		// otherwise we leave it untouched by the overdrive stuff
		if (fabs(inputSampleR) > threshold) {
			bridgerectifier = (fabs(inputSampleR) - threshold) * hardness;
			// skip flat area if any, scale to distortion limit
			if (bridgerectifier > breakup) bridgerectifier = breakup;
			// max value for sine function, 'breakup' modeling for trashed console tone
			// more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
			bridgerectifier = sin(bridgerectifier) / hardness;
			// do the sine factor, scale back to proper amount
			if (inputSampleR > 0) inputSampleR = bridgerectifier + threshold;
			else inputSampleR = -(bridgerectifier + threshold);
		}
		// otherwise we leave it untouched by the overdrive stuff

		randy = (((double) luxor->fpdL / UINT32_MAX) * 0.031);
		inputSampleL = ((inputSampleL * (1 - randy)) + (luxor->lastSampleL * randy)) * outlevel;
		luxor->lastSampleL = inputSampleL;

		randy = (((double) luxor->fpdR / UINT32_MAX) * 0.031);
		inputSampleR = ((inputSampleR * (1 - randy)) + (luxor->lastSampleR * randy)) * outlevel;
		luxor->lastSampleR = inputSampleR;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		luxor->fpdL ^= luxor->fpdL << 13;
		luxor->fpdL ^= luxor->fpdL >> 17;
		luxor->fpdL ^= luxor->fpdL << 5;
		inputSampleL += (((double) luxor->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		luxor->fpdR ^= luxor->fpdR << 13;
		luxor->fpdR ^= luxor->fpdR >> 17;
		luxor->fpdR ^= luxor->fpdR << 5;
		inputSampleR += (((double) luxor->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	LUXOR_URI,
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
