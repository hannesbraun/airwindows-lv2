#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define DITHERBOX_URI "https://hannesbraun.net/ns/lv2/airwindows/ditherbox"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TYPE = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* type;

	double bynL[13];
	double bynR[13];
	double noiseShapingL;
	double noiseShapingR;
	double contingentErrL;
	double contingentErrR;
	double currentDitherL;
	double currentDitherR;
	int Position;
	bool flip;
	double NSOddL;
	double prevL;
	double nsL[16];
	double NSOddR;
	double prevR;
	double nsR[16];
	double lastSampleL;
	double outSampleL;
	double lastSampleR;
	double outSampleR;

	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double iirSampleIL;
	double iirSampleJL;
	double iirSampleKL;
	double iirSampleLL;
	double iirSampleML;
	double iirSampleNL;
	double iirSampleOL;
	double iirSamplePL;
	double iirSampleQL;
	double iirSampleRL;
	double iirSampleSL;
	double iirSampleTL;
	double iirSampleUL;
	double iirSampleVL;
	double iirSampleWL;
	double iirSampleXL;
	double iirSampleYL;
	double iirSampleZL;

	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double iirSampleGR;
	double iirSampleHR;
	double iirSampleIR;
	double iirSampleJR;
	double iirSampleKR;
	double iirSampleLR;
	double iirSampleMR;
	double iirSampleNR;
	double iirSampleOR;
	double iirSamplePR;
	double iirSampleQR;
	double iirSampleRR;
	double iirSampleSR;
	double iirSampleTR;
	double iirSampleUR;
	double iirSampleVR;
	double iirSampleWR;
	double iirSampleXR;
	double iirSampleYR;
	double iirSampleZR;

	uint32_t fpdL;
	uint32_t fpdR;
} Ditherbox;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Ditherbox* ditherbox = (Ditherbox*) calloc(1, sizeof(Ditherbox));
	ditherbox->sampleRate = rate;
	return (LV2_Handle) ditherbox;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Ditherbox* ditherbox = (Ditherbox*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			ditherbox->input[0] = (const float*) data;
			break;
		case INPUT_R:
			ditherbox->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			ditherbox->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			ditherbox->output[1] = (float*) data;
			break;
		case TYPE:
			ditherbox->type = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Ditherbox* ditherbox = (Ditherbox*) instance;

	ditherbox->Position = 99999999;
	ditherbox->contingentErrL = 0.0;
	ditherbox->contingentErrR = 0.0;
	ditherbox->currentDitherL = 0.0;
	ditherbox->currentDitherR = 0.0;
	ditherbox->bynL[0] = 1000;
	ditherbox->bynL[1] = 301;
	ditherbox->bynL[2] = 176;
	ditherbox->bynL[3] = 125;
	ditherbox->bynL[4] = 97;
	ditherbox->bynL[5] = 79;
	ditherbox->bynL[6] = 67;
	ditherbox->bynL[7] = 58;
	ditherbox->bynL[8] = 51;
	ditherbox->bynL[9] = 46;
	ditherbox->bynL[10] = 1000;
	ditherbox->noiseShapingL = 0.0;
	ditherbox->bynR[0] = 1000;
	ditherbox->bynR[1] = 301;
	ditherbox->bynR[2] = 176;
	ditherbox->bynR[3] = 125;
	ditherbox->bynR[4] = 97;
	ditherbox->bynR[5] = 79;
	ditherbox->bynR[6] = 67;
	ditherbox->bynR[7] = 58;
	ditherbox->bynR[8] = 51;
	ditherbox->bynR[9] = 46;
	ditherbox->bynR[10] = 1000;
	ditherbox->noiseShapingR = 0.0;

	ditherbox->NSOddL = 0.0;
	ditherbox->prevL = 0.0;
	ditherbox->nsL[0] = 0;
	ditherbox->nsL[1] = 0;
	ditherbox->nsL[2] = 0;
	ditherbox->nsL[3] = 0;
	ditherbox->nsL[4] = 0;
	ditherbox->nsL[5] = 0;
	ditherbox->nsL[6] = 0;
	ditherbox->nsL[7] = 0;
	ditherbox->nsL[8] = 0;
	ditherbox->nsL[9] = 0;
	ditherbox->nsL[10] = 0;
	ditherbox->nsL[11] = 0;
	ditherbox->nsL[12] = 0;
	ditherbox->nsL[13] = 0;
	ditherbox->nsL[14] = 0;
	ditherbox->nsL[15] = 0;
	ditherbox->NSOddR = 0.0;
	ditherbox->prevR = 0.0;
	ditherbox->nsR[0] = 0;
	ditherbox->nsR[1] = 0;
	ditherbox->nsR[2] = 0;
	ditherbox->nsR[3] = 0;
	ditherbox->nsR[4] = 0;
	ditherbox->nsR[5] = 0;
	ditherbox->nsR[6] = 0;
	ditherbox->nsR[7] = 0;
	ditherbox->nsR[8] = 0;
	ditherbox->nsR[9] = 0;
	ditherbox->nsR[10] = 0;
	ditherbox->nsR[11] = 0;
	ditherbox->nsR[12] = 0;
	ditherbox->nsR[13] = 0;
	ditherbox->nsR[14] = 0;
	ditherbox->nsR[15] = 0;

	ditherbox->lastSampleL = 0.0;
	ditherbox->outSampleL = 0.0;
	ditherbox->lastSampleR = 0.0;
	ditherbox->outSampleR = 0.0;

	ditherbox->iirSampleAL = 0.0;
	ditherbox->iirSampleBL = 0.0;
	ditherbox->iirSampleCL = 0.0;
	ditherbox->iirSampleDL = 0.0;
	ditherbox->iirSampleEL = 0.0;
	ditherbox->iirSampleFL = 0.0;
	ditherbox->iirSampleGL = 0.0;
	ditherbox->iirSampleHL = 0.0;
	ditherbox->iirSampleIL = 0.0;
	ditherbox->iirSampleJL = 0.0;
	ditherbox->iirSampleKL = 0.0;
	ditherbox->iirSampleLL = 0.0;
	ditherbox->iirSampleML = 0.0;
	ditherbox->iirSampleNL = 0.0;
	ditherbox->iirSampleOL = 0.0;
	ditherbox->iirSamplePL = 0.0;
	ditherbox->iirSampleQL = 0.0;
	ditherbox->iirSampleRL = 0.0;
	ditherbox->iirSampleSL = 0.0;
	ditherbox->iirSampleTL = 0.0;
	ditherbox->iirSampleUL = 0.0;
	ditherbox->iirSampleVL = 0.0;
	ditherbox->iirSampleWL = 0.0;
	ditherbox->iirSampleXL = 0.0;
	ditherbox->iirSampleYL = 0.0;
	ditherbox->iirSampleZL = 0.0;

	ditherbox->iirSampleAR = 0.0;
	ditherbox->iirSampleBR = 0.0;
	ditherbox->iirSampleCR = 0.0;
	ditherbox->iirSampleDR = 0.0;
	ditherbox->iirSampleER = 0.0;
	ditherbox->iirSampleFR = 0.0;
	ditherbox->iirSampleGR = 0.0;
	ditherbox->iirSampleHR = 0.0;
	ditherbox->iirSampleIR = 0.0;
	ditherbox->iirSampleJR = 0.0;
	ditherbox->iirSampleKR = 0.0;
	ditherbox->iirSampleLR = 0.0;
	ditherbox->iirSampleMR = 0.0;
	ditherbox->iirSampleNR = 0.0;
	ditherbox->iirSampleOR = 0.0;
	ditherbox->iirSamplePR = 0.0;
	ditherbox->iirSampleQR = 0.0;
	ditherbox->iirSampleRR = 0.0;
	ditherbox->iirSampleSR = 0.0;
	ditherbox->iirSampleTR = 0.0;
	ditherbox->iirSampleUR = 0.0;
	ditherbox->iirSampleVR = 0.0;
	ditherbox->iirSampleWR = 0.0;
	ditherbox->iirSampleXR = 0.0;
	ditherbox->iirSampleYR = 0.0;
	ditherbox->iirSampleZR = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Ditherbox* ditherbox = (Ditherbox*) instance;

	const float* in1 = ditherbox->input[0];
	const float* in2 = ditherbox->input[1];
	float* out1 = ditherbox->output[0];
	float* out2 = ditherbox->output[1];

	int dtype = (int) (*ditherbox->type) + 1; // +1 for Reaper bug workaround
	if (dtype < 1) dtype = 1;
	else if (dtype > 25) dtype = 25;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= ditherbox->sampleRate;
	double iirAmount = 2250 / 44100.0;
	double gaintarget = 1.42;
	double gain;
	iirAmount /= overallscale;
	double altAmount = 1.0 - iirAmount;
	double outputSampleL;
	double outputSampleR;
	double silhouette;
	double smoother;
	double bridgerectifier;
	double benfordize;
	int hotbinA;
	int hotbinB;
	double totalA;
	double totalB;
	double contingentRnd;
	double absSample;
	double contingent;
	double randyConstant = 1.61803398874989484820458683436563811772030917980576;
	double omegaConstant = 0.56714329040978387299996866221035554975381578718651;
	double expConstant = 0.06598803584531253707679018759684642493857704825279;
	double trim = 2.302585092994045684017991; // natural logarithm of 10
	bool highRes = false;
	bool dithering = true;
	if (dtype > 11) {
		highRes = true;
		dtype -= 11;
	}
	if (dtype > 11) {
		dithering = false;
		highRes = false;
	}
	// follow up by switching high res back off for the monitoring

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = ditherbox->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = ditherbox->fpdR * 1.18e-17;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;

		if (dtype == 8) {
			inputSampleL -= ditherbox->noiseShapingL;
			inputSampleR -= ditherbox->noiseShapingR;
		}

		if (dithering) {
			inputSampleL *= 32768.0;
			inputSampleR *= 32768.0;
		}
		// denormalizing as way of controlling insane detail boosting
		if (highRes) {
			inputSampleL *= 256.0;
			inputSampleR *= 256.0;
		} // 256 for 16/24 version

		switch (dtype) {
			case 1:
				inputSampleL = floor(inputSampleL);
				inputSampleR = floor(inputSampleR);
				// truncate
				break;

			case 2:
				inputSampleL += ((double) ditherbox->fpdL / UINT32_MAX);
				inputSampleL -= 0.5;
				inputSampleL = floor(inputSampleL);
				inputSampleR += ((double) ditherbox->fpdR / UINT32_MAX);
				inputSampleR -= 0.5;
				inputSampleR = floor(inputSampleR);
				// flat dither
				break;

			case 3:
				inputSampleL += ((double) ditherbox->fpdL / UINT32_MAX);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				inputSampleL += ((double) ditherbox->fpdL / UINT32_MAX);
				inputSampleL -= 1.0;
				inputSampleL = floor(inputSampleL);
				inputSampleR += ((double) ditherbox->fpdR / UINT32_MAX);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				inputSampleR += ((double) ditherbox->fpdR / UINT32_MAX);
				inputSampleR -= 1.0;
				inputSampleR = floor(inputSampleR);
				// TPDF dither
				break;

			case 4:
				ditherbox->currentDitherL = ((double) ditherbox->fpdL / UINT32_MAX);
				inputSampleL += ditherbox->currentDitherL;
				inputSampleL -= ditherbox->lastSampleL;
				inputSampleL = floor(inputSampleL);
				ditherbox->lastSampleL = ditherbox->currentDitherL;
				ditherbox->currentDitherR = ((double) ditherbox->fpdR / UINT32_MAX);
				inputSampleR += ditherbox->currentDitherR;
				inputSampleR -= ditherbox->lastSampleR;
				inputSampleR = floor(inputSampleR);
				ditherbox->lastSampleR = ditherbox->currentDitherR;
				// Paul dither
				break;

			case 5:
				ditherbox->nsL[9] = ditherbox->nsL[8];
				ditherbox->nsL[8] = ditherbox->nsL[7];
				ditherbox->nsL[7] = ditherbox->nsL[6];
				ditherbox->nsL[6] = ditherbox->nsL[5];
				ditherbox->nsL[5] = ditherbox->nsL[4];
				ditherbox->nsL[4] = ditherbox->nsL[3];
				ditherbox->nsL[3] = ditherbox->nsL[2];
				ditherbox->nsL[2] = ditherbox->nsL[1];
				ditherbox->nsL[1] = ditherbox->nsL[0];
				ditherbox->nsL[0] = ((double) ditherbox->fpdL / UINT32_MAX);

				ditherbox->currentDitherL = (ditherbox->nsL[0] * 0.061);
				ditherbox->currentDitherL -= (ditherbox->nsL[1] * 0.11);
				ditherbox->currentDitherL += (ditherbox->nsL[8] * 0.126);
				ditherbox->currentDitherL -= (ditherbox->nsL[7] * 0.23);
				ditherbox->currentDitherL += (ditherbox->nsL[2] * 0.25);
				ditherbox->currentDitherL -= (ditherbox->nsL[3] * 0.43);
				ditherbox->currentDitherL += (ditherbox->nsL[6] * 0.5);
				ditherbox->currentDitherL -= ditherbox->nsL[5];
				ditherbox->currentDitherL += ditherbox->nsL[4];
				// this sounds different from doing it in order of sample position
				// cumulative tiny errors seem to build up even at this buss depth
				// considerably more pronounced at 32 bit float.
				// Therefore we add the most significant components LAST.
				// trying to keep values on like exponents of the floating point value.
				inputSampleL += ditherbox->currentDitherL;

				inputSampleL = floor(inputSampleL);
				// done with L

				ditherbox->nsR[9] = ditherbox->nsR[8];
				ditherbox->nsR[8] = ditherbox->nsR[7];
				ditherbox->nsR[7] = ditherbox->nsR[6];
				ditherbox->nsR[6] = ditherbox->nsR[5];
				ditherbox->nsR[5] = ditherbox->nsR[4];
				ditherbox->nsR[4] = ditherbox->nsR[3];
				ditherbox->nsR[3] = ditherbox->nsR[2];
				ditherbox->nsR[2] = ditherbox->nsR[1];
				ditherbox->nsR[1] = ditherbox->nsR[0];
				ditherbox->nsR[0] = ((double) ditherbox->fpdR / UINT32_MAX);

				ditherbox->currentDitherR = (ditherbox->nsR[0] * 0.061);
				ditherbox->currentDitherR -= (ditherbox->nsR[1] * 0.11);
				ditherbox->currentDitherR += (ditherbox->nsR[8] * 0.126);
				ditherbox->currentDitherR -= (ditherbox->nsR[7] * 0.23);
				ditherbox->currentDitherR += (ditherbox->nsR[2] * 0.25);
				ditherbox->currentDitherR -= (ditherbox->nsR[3] * 0.43);
				ditherbox->currentDitherR += (ditherbox->nsR[6] * 0.5);
				ditherbox->currentDitherR -= ditherbox->nsR[5];
				ditherbox->currentDitherR += ditherbox->nsR[4];
				// this sounds different from doing it in order of sample position
				// cumulative tiny errors seem to build up even at this buss depth
				// considerably more pronounced at 32 bit float.
				// Therefore we add the most significant components LAST.
				// trying to keep values on like exponents of the floating point value.
				inputSampleR += ditherbox->currentDitherR;

				inputSampleR = floor(inputSampleR);
				// done with R

				// DoublePaul dither
				break;

			case 6:
				ditherbox->currentDitherL = ((double) ditherbox->fpdL / UINT32_MAX);
				ditherbox->currentDitherR = ((double) ditherbox->fpdR / UINT32_MAX);

				inputSampleL += ditherbox->currentDitherL;
				inputSampleR += ditherbox->currentDitherR;
				inputSampleL -= ditherbox->nsL[4];
				inputSampleR -= ditherbox->nsR[4];

				inputSampleL = floor(inputSampleL);
				inputSampleR = floor(inputSampleR);

				ditherbox->nsL[4] = ditherbox->nsL[3];
				ditherbox->nsL[3] = ditherbox->nsL[2];
				ditherbox->nsL[2] = ditherbox->nsL[1];
				ditherbox->nsL[1] = ditherbox->currentDitherL;

				ditherbox->nsR[4] = ditherbox->nsR[3];
				ditherbox->nsR[3] = ditherbox->nsR[2];
				ditherbox->nsR[2] = ditherbox->nsR[1];
				ditherbox->nsR[1] = ditherbox->currentDitherR;
				// Tape dither
				break;

			case 7:
				ditherbox->Position += 1;
				// Note- uses integer overflow as a 'mod' operator
				hotbinA = ditherbox->Position * ditherbox->Position;
				hotbinA = hotbinA % 170003; //% is C++ mod operator
				hotbinA *= hotbinA;
				hotbinA = hotbinA % 17011; //% is C++ mod operator
				hotbinA *= hotbinA;
				hotbinA = hotbinA % 1709; //% is C++ mod operator
				hotbinA *= hotbinA;
				hotbinA = hotbinA % 173; //% is C++ mod operator
				hotbinA *= hotbinA;
				hotbinA = hotbinA % 17;
				hotbinA *= 0.0635;
				if (ditherbox->flip) hotbinA = -hotbinA;
				inputSampleL += hotbinA;
				inputSampleR += hotbinA;
				inputSampleL = floor(inputSampleL);
				inputSampleR = floor(inputSampleR);
				// Quadratic dither
				break;

			case 8:
				absSample = (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[0] += absSample;
				ditherbox->nsL[0] /= 2;
				absSample -= ditherbox->nsL[0];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[1] += absSample;
				ditherbox->nsL[1] /= 2;
				absSample -= ditherbox->nsL[1];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[2] += absSample;
				ditherbox->nsL[2] /= 2;
				absSample -= ditherbox->nsL[2];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[3] += absSample;
				ditherbox->nsL[3] /= 2;
				absSample -= ditherbox->nsL[3];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[4] += absSample;
				ditherbox->nsL[4] /= 2;
				absSample -= ditherbox->nsL[4];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[5] += absSample;
				ditherbox->nsL[5] /= 2;
				absSample -= ditherbox->nsL[5];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[6] += absSample;
				ditherbox->nsL[6] /= 2;
				absSample -= ditherbox->nsL[6];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[7] += absSample;
				ditherbox->nsL[7] /= 2;
				absSample -= ditherbox->nsL[7];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[8] += absSample;
				ditherbox->nsL[8] /= 2;
				absSample -= ditherbox->nsL[8];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[9] += absSample;
				ditherbox->nsL[9] /= 2;
				absSample -= ditherbox->nsL[9];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[10] += absSample;
				ditherbox->nsL[10] /= 2;
				absSample -= ditherbox->nsL[10];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[11] += absSample;
				ditherbox->nsL[11] /= 2;
				absSample -= ditherbox->nsL[11];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[12] += absSample;
				ditherbox->nsL[12] /= 2;
				absSample -= ditherbox->nsL[12];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[13] += absSample;
				ditherbox->nsL[13] /= 2;
				absSample -= ditherbox->nsL[13];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				ditherbox->nsL[14] += absSample;
				ditherbox->nsL[14] /= 2;
				absSample -= ditherbox->nsL[14];
				absSample += (((double) ditherbox->fpdL / UINT32_MAX) - 0.5);
				ditherbox->nsL[15] += absSample;
				ditherbox->nsL[15] /= 2;
				absSample -= ditherbox->nsL[15];
				// install noise and then shape it
				absSample += inputSampleL;

				if (ditherbox->NSOddL > 0) ditherbox->NSOddL -= 0.97;
				if (ditherbox->NSOddL < 0) ditherbox->NSOddL += 0.97;

				ditherbox->NSOddL -= (ditherbox->NSOddL * ditherbox->NSOddL * ditherbox->NSOddL * 0.475);

				ditherbox->NSOddL += ditherbox->prevL;
				absSample += (ditherbox->NSOddL * 0.475);
				ditherbox->prevL = floor(absSample) - inputSampleL;
				inputSampleL = floor(absSample);
				// TenNines dither L

				absSample = (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[0] += absSample;
				ditherbox->nsR[0] /= 2;
				absSample -= ditherbox->nsR[0];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[1] += absSample;
				ditherbox->nsR[1] /= 2;
				absSample -= ditherbox->nsR[1];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[2] += absSample;
				ditherbox->nsR[2] /= 2;
				absSample -= ditherbox->nsR[2];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[3] += absSample;
				ditherbox->nsR[3] /= 2;
				absSample -= ditherbox->nsR[3];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[4] += absSample;
				ditherbox->nsR[4] /= 2;
				absSample -= ditherbox->nsR[4];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[5] += absSample;
				ditherbox->nsR[5] /= 2;
				absSample -= ditherbox->nsR[5];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[6] += absSample;
				ditherbox->nsR[6] /= 2;
				absSample -= ditherbox->nsR[6];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[7] += absSample;
				ditherbox->nsR[7] /= 2;
				absSample -= ditherbox->nsR[7];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[8] += absSample;
				ditherbox->nsR[8] /= 2;
				absSample -= ditherbox->nsR[8];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[9] += absSample;
				ditherbox->nsR[9] /= 2;
				absSample -= ditherbox->nsR[9];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[10] += absSample;
				ditherbox->nsR[10] /= 2;
				absSample -= ditherbox->nsR[10];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[11] += absSample;
				ditherbox->nsR[11] /= 2;
				absSample -= ditherbox->nsR[11];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[12] += absSample;
				ditherbox->nsR[12] /= 2;
				absSample -= ditherbox->nsR[12];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[13] += absSample;
				ditherbox->nsR[13] /= 2;
				absSample -= ditherbox->nsR[13];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				ditherbox->nsR[14] += absSample;
				ditherbox->nsR[14] /= 2;
				absSample -= ditherbox->nsR[14];
				absSample += (((double) ditherbox->fpdR / UINT32_MAX) - 0.5);
				ditherbox->nsR[15] += absSample;
				ditherbox->nsR[15] /= 2;
				absSample -= ditherbox->nsR[15];
				// install noise and then shape it
				absSample += inputSampleR;

				if (ditherbox->NSOddR > 0) ditherbox->NSOddR -= 0.97;
				if (ditherbox->NSOddR < 0) ditherbox->NSOddR += 0.97;

				ditherbox->NSOddR -= (ditherbox->NSOddR * ditherbox->NSOddR * ditherbox->NSOddR * 0.475);

				ditherbox->NSOddR += ditherbox->prevR;
				absSample += (ditherbox->NSOddR * 0.475);
				ditherbox->prevR = floor(absSample) - inputSampleR;
				inputSampleR = floor(absSample);
				// TenNines dither R
				break;

			case 9:
				if (inputSampleL > 0) inputSampleL += 0.383;
				if (inputSampleL < 0) inputSampleL -= 0.383;
				if (inputSampleR > 0) inputSampleR += 0.383;
				if (inputSampleR < 0) inputSampleR -= 0.383;
				// adjusting to permit more information drug outta the noisefloor

				contingentRnd = ((double) ditherbox->fpdL / UINT32_MAX);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				contingentRnd += (((double) ditherbox->fpdL / UINT32_MAX) - 1.0);
				contingentRnd *= randyConstant; // produce TPDF dist, scale
				contingentRnd -= ditherbox->contingentErrL * omegaConstant; // include err
				absSample = fabs(inputSampleL);
				ditherbox->contingentErrL = absSample - floor(absSample); // get next err
				contingent = ditherbox->contingentErrL * 2.0; // scale of quantization levels
				if (contingent > 1.0) contingent = ((-contingent + 2.0) * omegaConstant) + expConstant;
				else contingent = (contingent * omegaConstant) + expConstant;
				// zero is next to a quantization level, one is exactly between them
				if (ditherbox->flip) contingentRnd = (contingentRnd * (1.0 - contingent)) + contingent + 0.5;
				else contingentRnd = (contingentRnd * (1.0 - contingent)) - contingent + 0.5;
				inputSampleL += (contingentRnd * contingent);
				// Contingent Dither
				inputSampleL = floor(inputSampleL);

				contingentRnd = ((double) ditherbox->fpdR / UINT32_MAX);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				contingentRnd += (((double) ditherbox->fpdR / UINT32_MAX) - 1.0);
				contingentRnd *= randyConstant; // produce TPDF dist, scale
				contingentRnd -= ditherbox->contingentErrR * omegaConstant; // include err
				absSample = fabs(inputSampleR);
				ditherbox->contingentErrR = absSample - floor(absSample); // get next err
				contingent = ditherbox->contingentErrR * 2.0; // scale of quantization levels
				if (contingent > 1.0) contingent = ((-contingent + 2.0) * omegaConstant) + expConstant;
				else contingent = (contingent * omegaConstant) + expConstant;
				// zero is next to a quantization level, one is exactly between them
				if (ditherbox->flip) contingentRnd = (contingentRnd * (1.0 - contingent)) + contingent + 0.5;
				else contingentRnd = (contingentRnd * (1.0 - contingent)) - contingent + 0.5;
				inputSampleR += (contingentRnd * contingent);
				// Contingent Dither
				inputSampleR = floor(inputSampleR);

				// note: this does not dither for values exactly the same as 16 bit values-
				// which forces the dither to gate at 0.0. It goes to digital black,
				// and does a teeny parallel-compression thing when almost at digital black.
				break;

			case 10: // this one is the original Naturalize
				if (inputSampleL > 0) inputSampleL += (0.3333333333);
				if (inputSampleL < 0) inputSampleL -= (0.3333333333);
				inputSampleL += ((double) ditherbox->fpdL / UINT32_MAX) * 0.6666666666;

				if (inputSampleR > 0) inputSampleR += (0.3333333333);
				if (inputSampleR < 0) inputSampleR -= (0.3333333333);
				inputSampleR += ((double) ditherbox->fpdR / UINT32_MAX) * 0.6666666666;

				// begin L
				benfordize = floor(inputSampleL);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinA = floor(benfordize);
				// hotbin becomes the Benford bin value for this number floored
				totalA = 0;
				if ((hotbinA > 0) && (hotbinA < 10)) {
					ditherbox->bynL[hotbinA] += 1;
					totalA += (301 - ditherbox->bynL[1]);
					totalA += (176 - ditherbox->bynL[2]);
					totalA += (125 - ditherbox->bynL[3]);
					totalA += (97 - ditherbox->bynL[4]);
					totalA += (79 - ditherbox->bynL[5]);
					totalA += (67 - ditherbox->bynL[6]);
					totalA += (58 - ditherbox->bynL[7]);
					totalA += (51 - ditherbox->bynL[8]);
					totalA += (46 - ditherbox->bynL[9]);
					ditherbox->bynL[hotbinA] -= 1;
				} else {
					hotbinA = 10;
				}
				// produce total number- smaller is closer to Benford real

				benfordize = ceil(inputSampleL);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinB = floor(benfordize);
				// hotbin becomes the Benford bin value for this number ceiled
				totalB = 0;
				if ((hotbinB > 0) && (hotbinB < 10)) {
					ditherbox->bynL[hotbinB] += 1;
					totalB += (301 - ditherbox->bynL[1]);
					totalB += (176 - ditherbox->bynL[2]);
					totalB += (125 - ditherbox->bynL[3]);
					totalB += (97 - ditherbox->bynL[4]);
					totalB += (79 - ditherbox->bynL[5]);
					totalB += (67 - ditherbox->bynL[6]);
					totalB += (58 - ditherbox->bynL[7]);
					totalB += (51 - ditherbox->bynL[8]);
					totalB += (46 - ditherbox->bynL[9]);
					ditherbox->bynL[hotbinB] -= 1;
				} else {
					hotbinB = 10;
				}
				// produce total number- smaller is closer to Benford real

				if (totalA < totalB) {
					ditherbox->bynL[hotbinA] += 1;
					inputSampleL = floor(inputSampleL);
				} else {
					ditherbox->bynL[hotbinB] += 1;
					inputSampleL = ceil(inputSampleL);
				}
				// assign the relevant one to the delay line
				// and floor/ceil signal accordingly

				totalA = ditherbox->bynL[1] + ditherbox->bynL[2] + ditherbox->bynL[3] + ditherbox->bynL[4] + ditherbox->bynL[5] + ditherbox->bynL[6] + ditherbox->bynL[7] + ditherbox->bynL[8] + ditherbox->bynL[9];
				totalA /= 1000;
				totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
				ditherbox->bynL[1] /= totalA;
				ditherbox->bynL[2] /= totalA;
				ditherbox->bynL[3] /= totalA;
				ditherbox->bynL[4] /= totalA;
				ditherbox->bynL[5] /= totalA;
				ditherbox->bynL[6] /= totalA;
				ditherbox->bynL[7] /= totalA;
				ditherbox->bynL[8] /= totalA;
				ditherbox->bynL[9] /= totalA;
				ditherbox->bynL[10] /= 2; // catchall for garbage data
				// end L

				// begin R
				benfordize = floor(inputSampleR);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinA = floor(benfordize);
				// hotbin becomes the Benford bin value for this number floored
				totalA = 0;
				if ((hotbinA > 0) && (hotbinA < 10)) {
					ditherbox->bynR[hotbinA] += 1;
					totalA += (301 - ditherbox->bynR[1]);
					totalA += (176 - ditherbox->bynR[2]);
					totalA += (125 - ditherbox->bynR[3]);
					totalA += (97 - ditherbox->bynR[4]);
					totalA += (79 - ditherbox->bynR[5]);
					totalA += (67 - ditherbox->bynR[6]);
					totalA += (58 - ditherbox->bynR[7]);
					totalA += (51 - ditherbox->bynR[8]);
					totalA += (46 - ditherbox->bynR[9]);
					ditherbox->bynR[hotbinA] -= 1;
				} else {
					hotbinA = 10;
				}
				// produce total number- smaller is closer to Benford real

				benfordize = ceil(inputSampleR);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinB = floor(benfordize);
				// hotbin becomes the Benford bin value for this number ceiled
				totalB = 0;
				if ((hotbinB > 0) && (hotbinB < 10)) {
					ditherbox->bynR[hotbinB] += 1;
					totalB += (301 - ditherbox->bynR[1]);
					totalB += (176 - ditherbox->bynR[2]);
					totalB += (125 - ditherbox->bynR[3]);
					totalB += (97 - ditherbox->bynR[4]);
					totalB += (79 - ditherbox->bynR[5]);
					totalB += (67 - ditherbox->bynR[6]);
					totalB += (58 - ditherbox->bynR[7]);
					totalB += (51 - ditherbox->bynR[8]);
					totalB += (46 - ditherbox->bynR[9]);
					ditherbox->bynR[hotbinB] -= 1;
				} else {
					hotbinB = 10;
				}
				// produce total number- smaller is closer to Benford real

				if (totalA < totalB) {
					ditherbox->bynR[hotbinA] += 1;
					inputSampleR = floor(inputSampleR);
				} else {
					ditherbox->bynR[hotbinB] += 1;
					inputSampleR = ceil(inputSampleR);
				}
				// assign the relevant one to the delay line
				// and floor/ceil signal accordingly

				totalA = ditherbox->bynR[1] + ditherbox->bynR[2] + ditherbox->bynR[3] + ditherbox->bynR[4] + ditherbox->bynR[5] + ditherbox->bynR[6] + ditherbox->bynR[7] + ditherbox->bynR[8] + ditherbox->bynR[9];
				totalA /= 1000;
				totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
				ditherbox->bynR[1] /= totalA;
				ditherbox->bynR[2] /= totalA;
				ditherbox->bynR[3] /= totalA;
				ditherbox->bynR[4] /= totalA;
				ditherbox->bynR[5] /= totalA;
				ditherbox->bynR[6] /= totalA;
				ditherbox->bynR[7] /= totalA;
				ditherbox->bynR[8] /= totalA;
				ditherbox->bynR[9] /= totalA;
				ditherbox->bynR[10] /= 2; // catchall for garbage data
				// end R
				break;

			case 11: // this one is the Not Just Another Dither

				// begin L
				benfordize = floor(inputSampleL);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinA = floor(benfordize);
				// hotbin becomes the Benford bin value for this number floored
				totalA = 0;
				if ((hotbinA > 0) && (hotbinA < 10)) {
					ditherbox->bynL[hotbinA] += 1;
					totalA += (301 - ditherbox->bynL[1]);
					totalA += (176 - ditherbox->bynL[2]);
					totalA += (125 - ditherbox->bynL[3]);
					totalA += (97 - ditherbox->bynL[4]);
					totalA += (79 - ditherbox->bynL[5]);
					totalA += (67 - ditherbox->bynL[6]);
					totalA += (58 - ditherbox->bynL[7]);
					totalA += (51 - ditherbox->bynL[8]);
					totalA += (46 - ditherbox->bynL[9]);
					ditherbox->bynL[hotbinA] -= 1;
				} else {
					hotbinA = 10;
				}
				// produce total number- smaller is closer to Benford real

				benfordize = ceil(inputSampleL);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinB = floor(benfordize);
				// hotbin becomes the Benford bin value for this number ceiled
				totalB = 0;
				if ((hotbinB > 0) && (hotbinB < 10)) {
					ditherbox->bynL[hotbinB] += 1;
					totalB += (301 - ditherbox->bynL[1]);
					totalB += (176 - ditherbox->bynL[2]);
					totalB += (125 - ditherbox->bynL[3]);
					totalB += (97 - ditherbox->bynL[4]);
					totalB += (79 - ditherbox->bynL[5]);
					totalB += (67 - ditherbox->bynL[6]);
					totalB += (58 - ditherbox->bynL[7]);
					totalB += (51 - ditherbox->bynL[8]);
					totalB += (46 - ditherbox->bynL[9]);
					ditherbox->bynL[hotbinB] -= 1;
				} else {
					hotbinB = 10;
				}
				// produce total number- smaller is closer to Benford real

				if (totalA < totalB) {
					ditherbox->bynL[hotbinA] += 1;
					inputSampleL = floor(inputSampleL);
				} else {
					ditherbox->bynL[hotbinB] += 1;
					inputSampleL = ceil(inputSampleL);
				}
				// assign the relevant one to the delay line
				// and floor/ceil signal accordingly

				totalA = ditherbox->bynL[1] + ditherbox->bynL[2] + ditherbox->bynL[3] + ditherbox->bynL[4] + ditherbox->bynL[5] + ditherbox->bynL[6] + ditherbox->bynL[7] + ditherbox->bynL[8] + ditherbox->bynL[9];
				totalA /= 1000;
				totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
				ditherbox->bynL[1] /= totalA;
				ditherbox->bynL[2] /= totalA;
				ditherbox->bynL[3] /= totalA;
				ditherbox->bynL[4] /= totalA;
				ditherbox->bynL[5] /= totalA;
				ditherbox->bynL[6] /= totalA;
				ditherbox->bynL[7] /= totalA;
				ditherbox->bynL[8] /= totalA;
				ditherbox->bynL[9] /= totalA;
				ditherbox->bynL[10] /= 2; // catchall for garbage data
				// end L

				// begin R
				benfordize = floor(inputSampleR);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinA = floor(benfordize);
				// hotbin becomes the Benford bin value for this number floored
				totalA = 0;
				if ((hotbinA > 0) && (hotbinA < 10)) {
					ditherbox->bynR[hotbinA] += 1;
					totalA += (301 - ditherbox->bynR[1]);
					totalA += (176 - ditherbox->bynR[2]);
					totalA += (125 - ditherbox->bynR[3]);
					totalA += (97 - ditherbox->bynR[4]);
					totalA += (79 - ditherbox->bynR[5]);
					totalA += (67 - ditherbox->bynR[6]);
					totalA += (58 - ditherbox->bynR[7]);
					totalA += (51 - ditherbox->bynR[8]);
					totalA += (46 - ditherbox->bynR[9]);
					ditherbox->bynR[hotbinA] -= 1;
				} else {
					hotbinA = 10;
				}
				// produce total number- smaller is closer to Benford real

				benfordize = ceil(inputSampleR);
				while (benfordize >= 1.0) {
					benfordize /= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				if (benfordize < 1.0) {
					benfordize *= 10;
				}
				hotbinB = floor(benfordize);
				// hotbin becomes the Benford bin value for this number ceiled
				totalB = 0;
				if ((hotbinB > 0) && (hotbinB < 10)) {
					ditherbox->bynR[hotbinB] += 1;
					totalB += (301 - ditherbox->bynR[1]);
					totalB += (176 - ditherbox->bynR[2]);
					totalB += (125 - ditherbox->bynR[3]);
					totalB += (97 - ditherbox->bynR[4]);
					totalB += (79 - ditherbox->bynR[5]);
					totalB += (67 - ditherbox->bynR[6]);
					totalB += (58 - ditherbox->bynR[7]);
					totalB += (51 - ditherbox->bynR[8]);
					totalB += (46 - ditherbox->bynR[9]);
					ditherbox->bynR[hotbinB] -= 1;
				} else {
					hotbinB = 10;
				}
				// produce total number- smaller is closer to Benford real

				if (totalA < totalB) {
					ditherbox->bynR[hotbinA] += 1;
					inputSampleR = floor(inputSampleR);
				} else {
					ditherbox->bynR[hotbinB] += 1;
					inputSampleR = ceil(inputSampleR);
				}
				// assign the relevant one to the delay line
				// and floor/ceil signal accordingly

				totalA = ditherbox->bynR[1] + ditherbox->bynR[2] + ditherbox->bynR[3] + ditherbox->bynR[4] + ditherbox->bynR[5] + ditherbox->bynR[6] + ditherbox->bynR[7] + ditherbox->bynR[8] + ditherbox->bynR[9];
				totalA /= 1000;
				totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
				ditherbox->bynR[1] /= totalA;
				ditherbox->bynR[2] /= totalA;
				ditherbox->bynR[3] /= totalA;
				ditherbox->bynR[4] /= totalA;
				ditherbox->bynR[5] /= totalA;
				ditherbox->bynR[6] /= totalA;
				ditherbox->bynR[7] /= totalA;
				ditherbox->bynR[8] /= totalA;
				ditherbox->bynR[9] /= totalA;
				ditherbox->bynR[10] /= 2; // catchall for garbage data
				// end R
				break;

			case 12:
				// slew only
				outputSampleL = (inputSampleL - ditherbox->lastSampleL) * trim;
				outputSampleR = (inputSampleR - ditherbox->lastSampleR) * trim;
				ditherbox->lastSampleL = inputSampleL;
				ditherbox->lastSampleR = inputSampleR;
				if (outputSampleL > 1.0) outputSampleL = 1.0;
				if (outputSampleR > 1.0) outputSampleR = 1.0;
				if (outputSampleL < -1.0) outputSampleL = -1.0;
				if (outputSampleR < -1.0) outputSampleR = -1.0;
				inputSampleL = outputSampleL;
				inputSampleR = outputSampleR;
				break;

			case 13:
				// subs only
				gain = gaintarget;

				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				ditherbox->iirSampleAL = (ditherbox->iirSampleAL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleAL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleBL = (ditherbox->iirSampleBL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleBL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleCL = (ditherbox->iirSampleCL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleCL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleDL = (ditherbox->iirSampleDL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleDL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleEL = (ditherbox->iirSampleEL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleEL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleFL = (ditherbox->iirSampleFL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleFL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleGL = (ditherbox->iirSampleGL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleGL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleHL = (ditherbox->iirSampleHL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleHL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleIL = (ditherbox->iirSampleIL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleIL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleJL = (ditherbox->iirSampleJL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleJL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleKL = (ditherbox->iirSampleKL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleKL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleLL = (ditherbox->iirSampleLL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleLL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleML = (ditherbox->iirSampleML * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleML;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleNL = (ditherbox->iirSampleNL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleNL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleOL = (ditherbox->iirSampleOL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleOL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSamplePL = (ditherbox->iirSamplePL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSamplePL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleQL = (ditherbox->iirSampleQL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleQL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleRL = (ditherbox->iirSampleRL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleRL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleSL = (ditherbox->iirSampleSL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleSL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleTL = (ditherbox->iirSampleTL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleTL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleUL = (ditherbox->iirSampleUL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleUL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleVL = (ditherbox->iirSampleVL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleVL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleWL = (ditherbox->iirSampleWL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleWL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleXL = (ditherbox->iirSampleXL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleXL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleYL = (ditherbox->iirSampleYL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleYL;
				inputSampleL *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				ditherbox->iirSampleZL = (ditherbox->iirSampleZL * altAmount) + (inputSampleL * iirAmount);
				inputSampleL = ditherbox->iirSampleZL;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				gain = gaintarget;

				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				ditherbox->iirSampleAR = (ditherbox->iirSampleAR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleAR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleBR = (ditherbox->iirSampleBR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleBR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleCR = (ditherbox->iirSampleCR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleCR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleDR = (ditherbox->iirSampleDR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleDR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleER = (ditherbox->iirSampleER * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleER;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleFR = (ditherbox->iirSampleFR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleFR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleGR = (ditherbox->iirSampleGR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleGR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleHR = (ditherbox->iirSampleHR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleHR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleIR = (ditherbox->iirSampleIR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleIR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleJR = (ditherbox->iirSampleJR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleJR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleKR = (ditherbox->iirSampleKR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleKR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleLR = (ditherbox->iirSampleLR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleLR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleMR = (ditherbox->iirSampleMR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleMR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleNR = (ditherbox->iirSampleNR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleNR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleOR = (ditherbox->iirSampleOR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleOR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSamplePR = (ditherbox->iirSamplePR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSamplePR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleQR = (ditherbox->iirSampleQR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleQR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleRR = (ditherbox->iirSampleRR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleRR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleSR = (ditherbox->iirSampleSR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleSR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleTR = (ditherbox->iirSampleTR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleTR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleUR = (ditherbox->iirSampleUR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleUR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleVR = (ditherbox->iirSampleVR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleVR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleWR = (ditherbox->iirSampleWR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleWR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleXR = (ditherbox->iirSampleXR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleXR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleYR = (ditherbox->iirSampleYR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleYR;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				ditherbox->iirSampleZR = (ditherbox->iirSampleZR * altAmount) + (inputSampleR * iirAmount);
				inputSampleR = ditherbox->iirSampleZR;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				break;

			case 14:
				// silhouette
				// begin L
				bridgerectifier = fabs(inputSampleL) * 1.57079633;
				if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
				bridgerectifier = 1.0 - cos(bridgerectifier);
				if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
				else inputSampleL = -bridgerectifier;

				silhouette = ((double) ditherbox->fpdL / UINT32_MAX);
				ditherbox->fpdL ^= ditherbox->fpdL << 13;
				ditherbox->fpdL ^= ditherbox->fpdL >> 17;
				ditherbox->fpdL ^= ditherbox->fpdL << 5;
				silhouette -= 0.5;
				silhouette *= 2.0;
				silhouette *= fabs(inputSampleL);

				smoother = ((double) ditherbox->fpdL / UINT32_MAX);
				smoother -= 0.5;
				smoother *= 2.0;
				smoother *= fabs(ditherbox->lastSampleL);
				ditherbox->lastSampleL = inputSampleL;

				silhouette += smoother;

				bridgerectifier = fabs(silhouette) * 1.57079633;
				if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
				bridgerectifier = sin(bridgerectifier);
				if (silhouette > 0.0) silhouette = bridgerectifier;
				else silhouette = -bridgerectifier;

				inputSampleL = (silhouette + ditherbox->outSampleL) / 2.0;
				ditherbox->outSampleL = silhouette;
				// end L

				// begin R
				bridgerectifier = fabs(inputSampleR) * 1.57079633;
				if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
				bridgerectifier = 1.0 - cos(bridgerectifier);
				if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
				else inputSampleR = -bridgerectifier;

				silhouette = ((double) ditherbox->fpdR / UINT32_MAX);
				ditherbox->fpdR ^= ditherbox->fpdR << 13;
				ditherbox->fpdR ^= ditherbox->fpdR >> 17;
				ditherbox->fpdR ^= ditherbox->fpdR << 5;
				silhouette -= 0.5;
				silhouette *= 2.0;
				silhouette *= fabs(inputSampleR);

				smoother = ((double) ditherbox->fpdR / UINT32_MAX);
				smoother -= 0.5;
				smoother *= 2.0;
				smoother *= fabs(ditherbox->lastSampleR);
				ditherbox->lastSampleR = inputSampleR;

				silhouette += smoother;

				bridgerectifier = fabs(silhouette) * 1.57079633;
				if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
				bridgerectifier = sin(bridgerectifier);
				if (silhouette > 0.0) silhouette = bridgerectifier;
				else silhouette = -bridgerectifier;

				inputSampleR = (silhouette + ditherbox->outSampleR) / 2.0;
				ditherbox->outSampleR = silhouette;
				// end R
				break;
		}

		ditherbox->flip = !ditherbox->flip;
		// several dithers use this

		if (highRes) {
			inputSampleL /= 256.0;
			inputSampleR /= 256.0;
		} // 256 for 16/24 version
		if (dithering) {
			inputSampleL /= 32768.0;
			inputSampleR /= 32768.0;
		}

		if (dtype == 8) {
			ditherbox->noiseShapingL += inputSampleL - drySampleL;
			ditherbox->noiseShapingR += inputSampleR - drySampleR;
		}

		ditherbox->fpdL ^= ditherbox->fpdL << 13;
		ditherbox->fpdL ^= ditherbox->fpdL >> 17;
		ditherbox->fpdL ^= ditherbox->fpdL << 5;
		ditherbox->fpdR ^= ditherbox->fpdR << 13;
		ditherbox->fpdR ^= ditherbox->fpdR >> 17;
		ditherbox->fpdR ^= ditherbox->fpdR << 5;
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
	DITHERBOX_URI,
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
