#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define VERBITY_URI "https://hannesbraun.net/ns/lv2/airwindows/verbity"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BIGNESS = 4,
	LONGNESS = 5,
	DARKNESS = 6,
	WETNESS = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* bigness;
	const float* longness;
	const float* darkness;
	const float* wetness;

	double iirAL;
	double iirBL;

	double aIL[6480];
	double aJL[3660];
	double aKL[1720];
	double aLL[680];

	double aAL[9700];
	double aBL[6000];
	double aCL[2320];
	double aDL[940];

	double aEL[15220];
	double aFL[8460];
	double aGL[4540];
	double aHL[3200];

	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double previousAL;
	double previousBL;
	double previousCL;
	double previousDL;

	double lastRefL[7];
	double thunderL;

	double iirAR;
	double iirBR;

	double aIR[6480];
	double aJR[3660];
	double aKR[1720];
	double aLR[680];

	double aAR[9700];
	double aBR[6000];
	double aCR[2320];
	double aDR[940];

	double aER[15220];
	double aFR[8460];
	double aGR[4540];
	double aHR[3200];

	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	double previousAR;
	double previousBR;
	double previousCR;
	double previousDR;

	double lastRefR[7];
	double thunderR;

	int countA, delayA;
	int countB, delayB;
	int countC, delayC;
	int countD, delayD;
	int countE, delayE;
	int countF, delayF;
	int countG, delayG;
	int countH, delayH;
	int countI, delayI;
	int countJ, delayJ;
	int countK, delayK;
	int countL, delayL;
	int cycle; // all these ints are shared across channels, not duplicated

	uint32_t fpdL;
	uint32_t fpdR;
} Verbity;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Verbity* verbity = (Verbity*) calloc(1, sizeof(Verbity));
	verbity->sampleRate = rate;
	return (LV2_Handle) verbity;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Verbity* verbity = (Verbity*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			verbity->input[0] = (const float*) data;
			break;
		case INPUT_R:
			verbity->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			verbity->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			verbity->output[1] = (float*) data;
			break;
		case BIGNESS:
			verbity->bigness = (const float*) data;
			break;
		case LONGNESS:
			verbity->longness = (const float*) data;
			break;
		case DARKNESS:
			verbity->darkness = (const float*) data;
			break;
		case WETNESS:
			verbity->wetness = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Verbity* verbity = (Verbity*) instance;
	verbity->iirAL = 0.0;
	verbity->iirAR = 0.0;
	verbity->iirBL = 0.0;
	verbity->iirBR = 0.0;

	for (int count = 0; count < 6479; count++) {
		verbity->aIL[count] = 0.0;
		verbity->aIR[count] = 0.0;
	}
	for (int count = 0; count < 3659; count++) {
		verbity->aJL[count] = 0.0;
		verbity->aJR[count] = 0.0;
	}
	for (int count = 0; count < 1719; count++) {
		verbity->aKL[count] = 0.0;
		verbity->aKR[count] = 0.0;
	}
	for (int count = 0; count < 679; count++) {
		verbity->aLL[count] = 0.0;
		verbity->aLR[count] = 0.0;
	}

	for (int count = 0; count < 9699; count++) {
		verbity->aAL[count] = 0.0;
		verbity->aAR[count] = 0.0;
	}
	for (int count = 0; count < 5999; count++) {
		verbity->aBL[count] = 0.0;
		verbity->aBR[count] = 0.0;
	}
	for (int count = 0; count < 2319; count++) {
		verbity->aCL[count] = 0.0;
		verbity->aCR[count] = 0.0;
	}
	for (int count = 0; count < 939; count++) {
		verbity->aDL[count] = 0.0;
		verbity->aDR[count] = 0.0;
	}

	for (int count = 0; count < 15219; count++) {
		verbity->aEL[count] = 0.0;
		verbity->aER[count] = 0.0;
	}
	for (int count = 0; count < 8459; count++) {
		verbity->aFL[count] = 0.0;
		verbity->aFR[count] = 0.0;
	}
	for (int count = 0; count < 4539; count++) {
		verbity->aGL[count] = 0.0;
		verbity->aGR[count] = 0.0;
	}
	for (int count = 0; count < 3199; count++) {
		verbity->aHL[count] = 0.0;
		verbity->aHR[count] = 0.0;
	}

	verbity->feedbackAL = 0.0;
	verbity->feedbackAR = 0.0;
	verbity->feedbackBL = 0.0;
	verbity->feedbackBR = 0.0;
	verbity->feedbackCL = 0.0;
	verbity->feedbackCR = 0.0;
	verbity->feedbackDL = 0.0;
	verbity->feedbackDR = 0.0;
	verbity->previousAL = 0.0;
	verbity->previousAR = 0.0;
	verbity->previousBL = 0.0;
	verbity->previousBR = 0.0;
	verbity->previousCL = 0.0;
	verbity->previousCR = 0.0;
	verbity->previousDL = 0.0;
	verbity->previousDR = 0.0;

	for (int count = 0; count < 6; count++) {
		verbity->lastRefL[count] = 0.0;
		verbity->lastRefR[count] = 0.0;
	}

	verbity->thunderL = 0;
	verbity->thunderR = 0;

	verbity->countI = 1;
	verbity->countJ = 1;
	verbity->countK = 1;
	verbity->countL = 1;

	verbity->countA = 1;
	verbity->countB = 1;
	verbity->countC = 1;
	verbity->countD = 1;

	verbity->countE = 1;
	verbity->countF = 1;
	verbity->countG = 1;
	verbity->countH = 1;
	verbity->cycle = 0;

	verbity->fpdL = 1.0;
	while (verbity->fpdL < 16386) verbity->fpdL = rand() * UINT32_MAX;
	verbity->fpdR = 1.0;
	while (verbity->fpdR < 16386) verbity->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Verbity* verbity = (Verbity*) instance;

	const float* in1 = verbity->input[0];
	const float* in2 = verbity->input[1];
	float* out1 = verbity->output[0];
	float* out2 = verbity->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= verbity->sampleRate;

	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (verbity->cycle > cycleEnd - 1) verbity->cycle = cycleEnd - 1; // sanity check

	const float longness = *verbity->longness;
	const float darkness = *verbity->darkness;
	double size = (*verbity->bigness * 1.77) + 0.1;
	double regen = 0.0625 + (longness * 0.03125); // 0.09375 max;
	double lowpass = (1.0 - pow(darkness, 2.0)) / sqrt(overallscale);
	double interpolate = pow(darkness, 2.0) * 0.618033988749894848204586; // has IIRlike qualities
	double thunderAmount = (0.3 - (longness * 0.22)) * darkness * 0.1;
	double wet = *verbity->wetness * 2.0;
	double dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	// this reverb makes 50% full dry AND full wet, not crossfaded.
	// that's so it can be on submixes without cutting back dry channel when adjusted:
	// unless you go super heavy, you are only adjusting the added verb loudness.

	verbity->delayI = 3407.0 * size;
	verbity->delayJ = 1823.0 * size;
	verbity->delayK = 859.0 * size;
	verbity->delayL = 331.0 * size;

	verbity->delayA = 4801.0 * size;
	verbity->delayB = 2909.0 * size;
	verbity->delayC = 1153.0 * size;
	verbity->delayD = 461.0 * size;

	verbity->delayE = 7607.0 * size;
	verbity->delayF = 4217.0 * size;
	verbity->delayG = 2269.0 * size;
	verbity->delayH = 1597.0 * size;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = verbity->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = verbity->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		if (fabs(verbity->iirAL) < 1.18e-37) verbity->iirAL = 0.0;
		verbity->iirAL = (verbity->iirAL * (1.0 - lowpass)) + (inputSampleL * lowpass);
		inputSampleL = verbity->iirAL;
		if (fabs(verbity->iirAR) < 1.18e-37) verbity->iirAR = 0.0;
		verbity->iirAR = (verbity->iirAR * (1.0 - lowpass)) + (inputSampleR * lowpass);
		inputSampleR = verbity->iirAR;
		// initial filter

		verbity->cycle++;
		if (verbity->cycle == cycleEnd) { // hit the end point and we do a reverb sample
			verbity->feedbackAL = (verbity->feedbackAL * (1.0 - interpolate)) + (verbity->previousAL * interpolate);
			verbity->previousAL = verbity->feedbackAL;
			verbity->feedbackBL = (verbity->feedbackBL * (1.0 - interpolate)) + (verbity->previousBL * interpolate);
			verbity->previousBL = verbity->feedbackBL;
			verbity->feedbackCL = (verbity->feedbackCL * (1.0 - interpolate)) + (verbity->previousCL * interpolate);
			verbity->previousCL = verbity->feedbackCL;
			verbity->feedbackDL = (verbity->feedbackDL * (1.0 - interpolate)) + (verbity->previousDL * interpolate);
			verbity->previousDL = verbity->feedbackDL;
			verbity->feedbackAR = (verbity->feedbackAR * (1.0 - interpolate)) + (verbity->previousAR * interpolate);
			verbity->previousAR = verbity->feedbackAR;
			verbity->feedbackBR = (verbity->feedbackBR * (1.0 - interpolate)) + (verbity->previousBR * interpolate);
			verbity->previousBR = verbity->feedbackBR;
			verbity->feedbackCR = (verbity->feedbackCR * (1.0 - interpolate)) + (verbity->previousCR * interpolate);
			verbity->previousCR = verbity->feedbackCR;
			verbity->feedbackDR = (verbity->feedbackDR * (1.0 - interpolate)) + (verbity->previousDR * interpolate);
			verbity->previousDR = verbity->feedbackDR;

			verbity->thunderL = (verbity->thunderL * 0.99) - (verbity->feedbackAL * thunderAmount);
			verbity->thunderR = (verbity->thunderR * 0.99) - (verbity->feedbackAR * thunderAmount);

			verbity->aIL[verbity->countI] = inputSampleL + ((verbity->feedbackAL + verbity->thunderL) * regen);
			verbity->aJL[verbity->countJ] = inputSampleL + (verbity->feedbackBL * regen);
			verbity->aKL[verbity->countK] = inputSampleL + (verbity->feedbackCL * regen);
			verbity->aLL[verbity->countL] = inputSampleL + (verbity->feedbackDL * regen);
			verbity->aIR[verbity->countI] = inputSampleR + ((verbity->feedbackAR + verbity->thunderR) * regen);
			verbity->aJR[verbity->countJ] = inputSampleR + (verbity->feedbackBR * regen);
			verbity->aKR[verbity->countK] = inputSampleR + (verbity->feedbackCR * regen);
			verbity->aLR[verbity->countL] = inputSampleR + (verbity->feedbackDR * regen);

			verbity->countI++;
			if (verbity->countI < 0 || verbity->countI > verbity->delayI) verbity->countI = 0;
			verbity->countJ++;
			if (verbity->countJ < 0 || verbity->countJ > verbity->delayJ) verbity->countJ = 0;
			verbity->countK++;
			if (verbity->countK < 0 || verbity->countK > verbity->delayK) verbity->countK = 0;
			verbity->countL++;
			if (verbity->countL < 0 || verbity->countL > verbity->delayL) verbity->countL = 0;

			double outIL = verbity->aIL[verbity->countI - ((verbity->countI > verbity->delayI) ? verbity->delayI + 1 : 0)];
			double outJL = verbity->aJL[verbity->countJ - ((verbity->countJ > verbity->delayJ) ? verbity->delayJ + 1 : 0)];
			double outKL = verbity->aKL[verbity->countK - ((verbity->countK > verbity->delayK) ? verbity->delayK + 1 : 0)];
			double outLL = verbity->aLL[verbity->countL - ((verbity->countL > verbity->delayL) ? verbity->delayL + 1 : 0)];
			double outIR = verbity->aIR[verbity->countI - ((verbity->countI > verbity->delayI) ? verbity->delayI + 1 : 0)];
			double outJR = verbity->aJR[verbity->countJ - ((verbity->countJ > verbity->delayJ) ? verbity->delayJ + 1 : 0)];
			double outKR = verbity->aKR[verbity->countK - ((verbity->countK > verbity->delayK) ? verbity->delayK + 1 : 0)];
			double outLR = verbity->aLR[verbity->countL - ((verbity->countL > verbity->delayL) ? verbity->delayL + 1 : 0)];
			// first block: now we have four outputs

			verbity->aAL[verbity->countA] = (outIL - (outJL + outKL + outLL));
			verbity->aBL[verbity->countB] = (outJL - (outIL + outKL + outLL));
			verbity->aCL[verbity->countC] = (outKL - (outIL + outJL + outLL));
			verbity->aDL[verbity->countD] = (outLL - (outIL + outJL + outKL));
			verbity->aAR[verbity->countA] = (outIR - (outJR + outKR + outLR));
			verbity->aBR[verbity->countB] = (outJR - (outIR + outKR + outLR));
			verbity->aCR[verbity->countC] = (outKR - (outIR + outJR + outLR));
			verbity->aDR[verbity->countD] = (outLR - (outIR + outJR + outKR));

			verbity->countA++;
			if (verbity->countA < 0 || verbity->countA > verbity->delayA) verbity->countA = 0;
			verbity->countB++;
			if (verbity->countB < 0 || verbity->countB > verbity->delayB) verbity->countB = 0;
			verbity->countC++;
			if (verbity->countC < 0 || verbity->countC > verbity->delayC) verbity->countC = 0;
			verbity->countD++;
			if (verbity->countD < 0 || verbity->countD > verbity->delayD) verbity->countD = 0;

			double outAL = verbity->aAL[verbity->countA - ((verbity->countA > verbity->delayA) ? verbity->delayA + 1 : 0)];
			double outBL = verbity->aBL[verbity->countB - ((verbity->countB > verbity->delayB) ? verbity->delayB + 1 : 0)];
			double outCL = verbity->aCL[verbity->countC - ((verbity->countC > verbity->delayC) ? verbity->delayC + 1 : 0)];
			double outDL = verbity->aDL[verbity->countD - ((verbity->countD > verbity->delayD) ? verbity->delayD + 1 : 0)];
			double outAR = verbity->aAR[verbity->countA - ((verbity->countA > verbity->delayA) ? verbity->delayA + 1 : 0)];
			double outBR = verbity->aBR[verbity->countB - ((verbity->countB > verbity->delayB) ? verbity->delayB + 1 : 0)];
			double outCR = verbity->aCR[verbity->countC - ((verbity->countC > verbity->delayC) ? verbity->delayC + 1 : 0)];
			double outDR = verbity->aDR[verbity->countD - ((verbity->countD > verbity->delayD) ? verbity->delayD + 1 : 0)];
			// second block: four more outputs

			verbity->aEL[verbity->countE] = (outAL - (outBL + outCL + outDL));
			verbity->aFL[verbity->countF] = (outBL - (outAL + outCL + outDL));
			verbity->aGL[verbity->countG] = (outCL - (outAL + outBL + outDL));
			verbity->aHL[verbity->countH] = (outDL - (outAL + outBL + outCL));
			verbity->aER[verbity->countE] = (outAR - (outBR + outCR + outDR));
			verbity->aFR[verbity->countF] = (outBR - (outAR + outCR + outDR));
			verbity->aGR[verbity->countG] = (outCR - (outAR + outBR + outDR));
			verbity->aHR[verbity->countH] = (outDR - (outAR + outBR + outCR));

			verbity->countE++;
			if (verbity->countE < 0 || verbity->countE > verbity->delayE) verbity->countE = 0;
			verbity->countF++;
			if (verbity->countF < 0 || verbity->countF > verbity->delayF) verbity->countF = 0;
			verbity->countG++;
			if (verbity->countG < 0 || verbity->countG > verbity->delayG) verbity->countG = 0;
			verbity->countH++;
			if (verbity->countH < 0 || verbity->countH > verbity->delayH) verbity->countH = 0;

			double outEL = verbity->aEL[verbity->countE - ((verbity->countE > verbity->delayE) ? verbity->delayE + 1 : 0)];
			double outFL = verbity->aFL[verbity->countF - ((verbity->countF > verbity->delayF) ? verbity->delayF + 1 : 0)];
			double outGL = verbity->aGL[verbity->countG - ((verbity->countG > verbity->delayG) ? verbity->delayG + 1 : 0)];
			double outHL = verbity->aHL[verbity->countH - ((verbity->countH > verbity->delayH) ? verbity->delayH + 1 : 0)];
			double outER = verbity->aER[verbity->countE - ((verbity->countE > verbity->delayE) ? verbity->delayE + 1 : 0)];
			double outFR = verbity->aFR[verbity->countF - ((verbity->countF > verbity->delayF) ? verbity->delayF + 1 : 0)];
			double outGR = verbity->aGR[verbity->countG - ((verbity->countG > verbity->delayG) ? verbity->delayG + 1 : 0)];
			double outHR = verbity->aHR[verbity->countH - ((verbity->countH > verbity->delayH) ? verbity->delayH + 1 : 0)];
			// third block: final outputs

			verbity->feedbackAL = (outEL - (outFL + outGL + outHL));
			verbity->feedbackBL = (outFL - (outEL + outGL + outHL));
			verbity->feedbackCL = (outGL - (outEL + outFL + outHL));
			verbity->feedbackDL = (outHL - (outEL + outFL + outGL));
			verbity->feedbackAR = (outER - (outFR + outGR + outHR));
			verbity->feedbackBR = (outFR - (outER + outGR + outHR));
			verbity->feedbackCR = (outGR - (outER + outFR + outHR));
			verbity->feedbackDR = (outHR - (outER + outFR + outGR));
			// which we need to feed back into the input again, a bit

			inputSampleL = (outEL + outFL + outGL + outHL) / 8.0;
			inputSampleR = (outER + outFR + outGR + outHR) / 8.0;
			// and take the final combined sum of outputs
			if (cycleEnd == 4) {
				verbity->lastRefL[0] = verbity->lastRefL[4]; // start from previous last
				verbity->lastRefL[2] = (verbity->lastRefL[0] + inputSampleL) / 2; // half
				verbity->lastRefL[1] = (verbity->lastRefL[0] + verbity->lastRefL[2]) / 2; // one quarter
				verbity->lastRefL[3] = (verbity->lastRefL[2] + inputSampleL) / 2; // three quarters
				verbity->lastRefL[4] = inputSampleL; // full
				verbity->lastRefR[0] = verbity->lastRefR[4]; // start from previous last
				verbity->lastRefR[2] = (verbity->lastRefR[0] + inputSampleR) / 2; // half
				verbity->lastRefR[1] = (verbity->lastRefR[0] + verbity->lastRefR[2]) / 2; // one quarter
				verbity->lastRefR[3] = (verbity->lastRefR[2] + inputSampleR) / 2; // three quarters
				verbity->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				verbity->lastRefL[0] = verbity->lastRefL[3]; // start from previous last
				verbity->lastRefL[2] = (verbity->lastRefL[0] + verbity->lastRefL[0] + inputSampleL) / 3; // third
				verbity->lastRefL[1] = (verbity->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				verbity->lastRefL[3] = inputSampleL; // full
				verbity->lastRefR[0] = verbity->lastRefR[3]; // start from previous last
				verbity->lastRefR[2] = (verbity->lastRefR[0] + verbity->lastRefR[0] + inputSampleR) / 3; // third
				verbity->lastRefR[1] = (verbity->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				verbity->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				verbity->lastRefL[0] = verbity->lastRefL[2]; // start from previous last
				verbity->lastRefL[1] = (verbity->lastRefL[0] + inputSampleL) / 2; // half
				verbity->lastRefL[2] = inputSampleL; // full
				verbity->lastRefR[0] = verbity->lastRefR[2]; // start from previous last
				verbity->lastRefR[1] = (verbity->lastRefR[0] + inputSampleR) / 2; // half
				verbity->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				verbity->lastRefL[0] = inputSampleL;
				verbity->lastRefR[0] = inputSampleR;
			}
			verbity->cycle = 0; // reset
			inputSampleL = verbity->lastRefL[verbity->cycle];
			inputSampleR = verbity->lastRefR[verbity->cycle];
		} else {
			inputSampleL = verbity->lastRefL[verbity->cycle];
			inputSampleR = verbity->lastRefR[verbity->cycle];
			// we are going through our references now
		}

		if (fabs(verbity->iirBL) < 1.18e-37) verbity->iirBL = 0.0;
		verbity->iirBL = (verbity->iirBL * (1.0 - lowpass)) + (inputSampleL * lowpass);
		inputSampleL = verbity->iirBL;
		if (fabs(verbity->iirBR) < 1.18e-37) verbity->iirBR = 0.0;
		verbity->iirBR = (verbity->iirBR * (1.0 - lowpass)) + (inputSampleR * lowpass);
		inputSampleR = verbity->iirBR;
		// end filter

		if (wet < 1.0) {
			inputSampleL *= wet;
			inputSampleR *= wet;
		}
		if (dry < 1.0) {
			drySampleL *= dry;
			drySampleR *= dry;
		}
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		// this is our submix verb dry/wet: 0.5 is BOTH at FULL VOLUME
		// purpose is that, if you're adding verb, you're not altering other balances

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		verbity->fpdL ^= verbity->fpdL << 13;
		verbity->fpdL ^= verbity->fpdL >> 17;
		verbity->fpdL ^= verbity->fpdL << 5;
		inputSampleL += (((double) verbity->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		verbity->fpdR ^= verbity->fpdR << 13;
		verbity->fpdR ^= verbity->fpdR >> 17;
		verbity->fpdR ^= verbity->fpdR << 5;
		inputSampleR += (((double) verbity->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	VERBITY_URI,
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
