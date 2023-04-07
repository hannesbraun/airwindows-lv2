#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define MATRIXVERB_URI "https://hannesbraun.net/ns/lv2/airwindows/matrixverb"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FILTER = 4,
	DAMPING = 5,
	SPEED = 6,
	VIBRATO = 7,
	RMSIZE = 8,
	FLAVOR = 9,
	DRY_WET = 10
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* filter;
	const float* damping;
	const float* speed;
	const float* vibrato;
	const float* rmSize;
	const float* flavor;
	const float* dryWet;

	double biquadA[11];
	double biquadB[11];
	double biquadC[11];

	double aAL[8111];
	double aBL[7511];
	double aCL[7311];
	double aDL[6911];
	double aEL[6311];
	double aFL[6111];
	double aGL[5511];
	double aHL[4911];
	double aIL[4511];
	double aJL[4311];
	double aKL[3911];
	double aLL[3311];
	double aML[3111];

	double aAR[8111];
	double aBR[7511];
	double aCR[7311];
	double aDR[6911];
	double aER[6311];
	double aFR[6111];
	double aGR[5511];
	double aHR[4911];
	double aIR[4511];
	double aJR[4311];
	double aKR[3911];
	double aLR[3311];
	double aMR[3111];

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
	int countM, delayM;

	double feedbackAL, vibAL, depthA;
	double feedbackBL, vibBL, depthB;
	double feedbackCL, vibCL, depthC;
	double feedbackDL, vibDL, depthD;
	double feedbackEL, vibEL, depthE;
	double feedbackFL, vibFL, depthF;
	double feedbackGL, vibGL, depthG;
	double feedbackHL, vibHL, depthH;

	double feedbackAR, vibAR;
	double feedbackBR, vibBR;
	double feedbackCR, vibCR;
	double feedbackDR, vibDR;
	double feedbackER, vibER;
	double feedbackFR, vibFR;
	double feedbackGR, vibGR;
	double feedbackHR, vibHR;

	uint32_t fpdL;
	uint32_t fpdR;
} MatrixVerb;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	MatrixVerb* matrixVerb = (MatrixVerb*) calloc(1, sizeof(MatrixVerb));
	matrixVerb->sampleRate = rate;
	return (LV2_Handle) matrixVerb;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	MatrixVerb* matrixVerb = (MatrixVerb*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			matrixVerb->input[0] = (const float*) data;
			break;
		case INPUT_R:
			matrixVerb->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			matrixVerb->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			matrixVerb->output[1] = (float*) data;
			break;
		case FILTER:
			matrixVerb->filter = (const float*) data;
			break;
		case DAMPING:
			matrixVerb->damping = (const float*) data;
			break;
		case SPEED:
			matrixVerb->speed = (const float*) data;
			break;
		case VIBRATO:
			matrixVerb->vibrato = (const float*) data;
			break;
		case RMSIZE:
			matrixVerb->rmSize = (const float*) data;
			break;
		case FLAVOR:
			matrixVerb->flavor = (const float*) data;
			break;
		case DRY_WET:
			matrixVerb->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	MatrixVerb* matrixVerb = (MatrixVerb*) instance;

	for (int x = 0; x < 11; x++) {
		matrixVerb->biquadA[x] = 0.0;
		matrixVerb->biquadB[x] = 0.0;
		matrixVerb->biquadC[x] = 0.0;
	}

	matrixVerb->feedbackAL = matrixVerb->feedbackAR = 0.0;
	matrixVerb->feedbackBL = matrixVerb->feedbackBR = 0.0;
	matrixVerb->feedbackCL = matrixVerb->feedbackCR = 0.0;
	matrixVerb->feedbackDL = matrixVerb->feedbackDR = 0.0;
	matrixVerb->feedbackEL = matrixVerb->feedbackER = 0.0;
	matrixVerb->feedbackFL = matrixVerb->feedbackFR = 0.0;
	matrixVerb->feedbackGL = matrixVerb->feedbackGR = 0.0;
	matrixVerb->feedbackHL = matrixVerb->feedbackHR = 0.0;

	int count;
	for (count = 0; count < 8110; count++) {
		matrixVerb->aAL[count] = matrixVerb->aAR[count] = 0.0;
	}
	for (count = 0; count < 7510; count++) {
		matrixVerb->aBL[count] = matrixVerb->aBR[count] = 0.0;
	}
	for (count = 0; count < 7310; count++) {
		matrixVerb->aCL[count] = matrixVerb->aCR[count] = 0.0;
	}
	for (count = 0; count < 6910; count++) {
		matrixVerb->aDL[count] = matrixVerb->aDR[count] = 0.0;
	}
	for (count = 0; count < 6310; count++) {
		matrixVerb->aEL[count] = matrixVerb->aER[count] = 0.0;
	}
	for (count = 0; count < 6110; count++) {
		matrixVerb->aFL[count] = matrixVerb->aFR[count] = 0.0;
	}
	for (count = 0; count < 5510; count++) {
		matrixVerb->aGL[count] = matrixVerb->aGR[count] = 0.0;
	}
	for (count = 0; count < 4910; count++) {
		matrixVerb->aHL[count] = matrixVerb->aHR[count] = 0.0;
	}
	// maximum value needed will be delay * 100, plus 206 (absolute max vibrato depth)
	for (count = 0; count < 4510; count++) {
		matrixVerb->aIL[count] = matrixVerb->aIR[count] = 0.0;
	}
	for (count = 0; count < 4310; count++) {
		matrixVerb->aJL[count] = matrixVerb->aJR[count] = 0.0;
	}
	for (count = 0; count < 3910; count++) {
		matrixVerb->aKL[count] = matrixVerb->aKR[count] = 0.0;
	}
	for (count = 0; count < 3310; count++) {
		matrixVerb->aLL[count] = matrixVerb->aLR[count] = 0.0;
	}
	// maximum value will be delay * 100
	for (count = 0; count < 3110; count++) {
		matrixVerb->aML[count] = matrixVerb->aMR[count] = 0.0;
	}
	// maximum value will be delay * 100
	matrixVerb->countA = 1;
	matrixVerb->delayA = 79;
	matrixVerb->countB = 1;
	matrixVerb->delayB = 73;
	matrixVerb->countC = 1;
	matrixVerb->delayC = 71;
	matrixVerb->countD = 1;
	matrixVerb->delayD = 67;
	matrixVerb->countE = 1;
	matrixVerb->delayE = 61;
	matrixVerb->countF = 1;
	matrixVerb->delayF = 59;
	matrixVerb->countG = 1;
	matrixVerb->delayG = 53;
	matrixVerb->countH = 1;
	matrixVerb->delayH = 47;
	// the householder matrices
	matrixVerb->countI = 1;
	matrixVerb->delayI = 43;
	matrixVerb->countJ = 1;
	matrixVerb->delayJ = 41;
	matrixVerb->countK = 1;
	matrixVerb->delayK = 37;
	matrixVerb->countL = 1;
	matrixVerb->delayL = 31;
	// the allpasses
	matrixVerb->countM = 1;
	matrixVerb->delayM = 29;
	// the predelay
	matrixVerb->depthA = 0.003251;
	matrixVerb->depthB = 0.002999;
	matrixVerb->depthC = 0.002917;
	matrixVerb->depthD = 0.002749;
	matrixVerb->depthE = 0.002503;
	matrixVerb->depthF = 0.002423;
	matrixVerb->depthG = 0.002146;
	matrixVerb->depthH = 0.002088;
	// the individual vibrato rates for the delays
	matrixVerb->vibAL = rand() * -2147483647;
	matrixVerb->vibBL = rand() * -2147483647;
	matrixVerb->vibCL = rand() * -2147483647;
	matrixVerb->vibDL = rand() * -2147483647;
	matrixVerb->vibEL = rand() * -2147483647;
	matrixVerb->vibFL = rand() * -2147483647;
	matrixVerb->vibGL = rand() * -2147483647;
	matrixVerb->vibHL = rand() * -2147483647;

	matrixVerb->vibAR = rand() * -2147483647;
	matrixVerb->vibBR = rand() * -2147483647;
	matrixVerb->vibCR = rand() * -2147483647;
	matrixVerb->vibDR = rand() * -2147483647;
	matrixVerb->vibER = rand() * -2147483647;
	matrixVerb->vibFR = rand() * -2147483647;
	matrixVerb->vibGR = rand() * -2147483647;
	matrixVerb->vibHR = rand() * -2147483647;

	matrixVerb->fpdL = 1.0;
	while (matrixVerb->fpdL < 16386) matrixVerb->fpdL = rand() * UINT32_MAX;
	matrixVerb->fpdR = 1.0;
	while (matrixVerb->fpdR < 16386) matrixVerb->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	MatrixVerb* matrixVerb = (MatrixVerb*) instance;

	const float* in1 = matrixVerb->input[0];
	const float* in2 = matrixVerb->input[1];
	float* out1 = matrixVerb->output[0];
	float* out2 = matrixVerb->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= matrixVerb->sampleRate;

	matrixVerb->biquadC[0] = matrixVerb->biquadB[0] = matrixVerb->biquadA[0] = ((*matrixVerb->filter * 9000.0) + 1000.0) / matrixVerb->sampleRate;
	matrixVerb->biquadA[1] = 1.618033988749894848204586;
	matrixVerb->biquadB[1] = 0.618033988749894848204586;
	matrixVerb->biquadC[1] = 0.5;

	double K = tan(M_PI * matrixVerb->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / matrixVerb->biquadA[1] + K * K);
	matrixVerb->biquadA[2] = K * K * norm;
	matrixVerb->biquadA[3] = 2.0 * matrixVerb->biquadA[2];
	matrixVerb->biquadA[4] = matrixVerb->biquadA[2];
	matrixVerb->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	matrixVerb->biquadA[6] = (1.0 - K / matrixVerb->biquadA[1] + K * K) * norm;

	K = tan(M_PI * matrixVerb->biquadA[0]);
	norm = 1.0 / (1.0 + K / matrixVerb->biquadB[1] + K * K);
	matrixVerb->biquadB[2] = K * K * norm;
	matrixVerb->biquadB[3] = 2.0 * matrixVerb->biquadB[2];
	matrixVerb->biquadB[4] = matrixVerb->biquadB[2];
	matrixVerb->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	matrixVerb->biquadB[6] = (1.0 - K / matrixVerb->biquadB[1] + K * K) * norm;

	K = tan(M_PI * matrixVerb->biquadC[0]);
	norm = 1.0 / (1.0 + K / matrixVerb->biquadC[1] + K * K);
	matrixVerb->biquadC[2] = K * K * norm;
	matrixVerb->biquadC[3] = 2.0 * matrixVerb->biquadC[2];
	matrixVerb->biquadC[4] = matrixVerb->biquadC[2];
	matrixVerb->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	matrixVerb->biquadC[6] = (1.0 - K / matrixVerb->biquadC[1] + K * K) * norm;

	double vibSpeed = 0.06 + *matrixVerb->speed;
	double vibDepth = (0.027 + pow(*matrixVerb->vibrato, 3)) * 100.0;
	double size = (pow(*matrixVerb->rmSize, 2) * 90.0) + 10.0;
	double depthFactor = 1.0 - pow((1.0 - (0.82 - ((*matrixVerb->damping * 0.5) + (size * 0.002)))), 4);
	double blend = 0.955 - (size * 0.007);
	double crossmod = (*matrixVerb->flavor - 0.5) * 2.0;
	crossmod = pow(crossmod, 3) * 0.5;
	double regen = depthFactor * (0.5 - (fabs(crossmod) * 0.031));
	double wet = *matrixVerb->dryWet;

	matrixVerb->delayA = 79 * size;
	matrixVerb->delayB = 73 * size;
	matrixVerb->delayC = 71 * size;
	matrixVerb->delayD = 67 * size;
	matrixVerb->delayE = 61 * size;
	matrixVerb->delayF = 59 * size;
	matrixVerb->delayG = 53 * size;
	matrixVerb->delayH = 47 * size;

	matrixVerb->delayI = 43 * size;
	matrixVerb->delayJ = 41 * size;
	matrixVerb->delayK = 37 * size;
	matrixVerb->delayL = 31 * size;

	matrixVerb->delayM = (29 * size) - (56 * size * fabs(crossmod));
	// predelay for natural spaces, gets cut back for heavily artificial spaces

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = matrixVerb->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = matrixVerb->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		matrixVerb->aML[matrixVerb->countM] = inputSampleL;
		matrixVerb->aMR[matrixVerb->countM] = inputSampleR;
		matrixVerb->countM++;
		if (matrixVerb->countM < 0 || matrixVerb->countM > matrixVerb->delayM) {
			matrixVerb->countM = 0;
		}
		inputSampleL = matrixVerb->aML[matrixVerb->countM];
		inputSampleR = matrixVerb->aMR[matrixVerb->countM];
		// predelay

		double tempSampleL = (inputSampleL * matrixVerb->biquadA[2]) + matrixVerb->biquadA[7];
		matrixVerb->biquadA[7] = (inputSampleL * matrixVerb->biquadA[3]) - (tempSampleL * matrixVerb->biquadA[5]) + matrixVerb->biquadA[8];
		matrixVerb->biquadA[8] = (inputSampleL * matrixVerb->biquadA[4]) - (tempSampleL * matrixVerb->biquadA[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		double tempSampleR = (inputSampleR * matrixVerb->biquadA[2]) + matrixVerb->biquadA[9];
		matrixVerb->biquadA[9] = (inputSampleR * matrixVerb->biquadA[3]) - (tempSampleR * matrixVerb->biquadA[5]) + matrixVerb->biquadA[10];
		matrixVerb->biquadA[10] = (inputSampleR * matrixVerb->biquadA[4]) - (tempSampleR * matrixVerb->biquadA[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		inputSampleL *= wet;
		inputSampleR *= wet;
		// we're going to use this as a kind of balance since the reverb buildup can be so large

		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);

		double allpassIL = inputSampleL;
		double allpassJL = inputSampleL;
		double allpassKL = inputSampleL;
		double allpassLL = inputSampleL;

		double allpassIR = inputSampleR;
		double allpassJR = inputSampleR;
		double allpassKR = inputSampleR;
		double allpassLR = inputSampleR;

		int allpasstemp = matrixVerb->countI + 1;
		if (allpasstemp < 0 || allpasstemp > matrixVerb->delayI) {
			allpasstemp = 0;
		}
		allpassIL -= matrixVerb->aIL[allpasstemp] * 0.5;
		matrixVerb->aIL[matrixVerb->countI] = allpassIL;
		allpassIL *= 0.5;
		allpassIR -= matrixVerb->aIR[allpasstemp] * 0.5;
		matrixVerb->aIR[matrixVerb->countI] = allpassIR;
		allpassIR *= 0.5;
		matrixVerb->countI++;
		if (matrixVerb->countI < 0 || matrixVerb->countI > matrixVerb->delayI) {
			matrixVerb->countI = 0;
		}
		allpassIL += (matrixVerb->aIL[matrixVerb->countI]);
		allpassIR += (matrixVerb->aIR[matrixVerb->countI]);

		allpasstemp = matrixVerb->countJ + 1;
		if (allpasstemp < 0 || allpasstemp > matrixVerb->delayJ) {
			allpasstemp = 0;
		}
		allpassJL -= matrixVerb->aJL[allpasstemp] * 0.5;
		matrixVerb->aJL[matrixVerb->countJ] = allpassJL;
		allpassJL *= 0.5;
		allpassJR -= matrixVerb->aJR[allpasstemp] * 0.5;
		matrixVerb->aJR[matrixVerb->countJ] = allpassJR;
		allpassJR *= 0.5;
		matrixVerb->countJ++;
		if (matrixVerb->countJ < 0 || matrixVerb->countJ > matrixVerb->delayJ) {
			matrixVerb->countJ = 0;
		}
		allpassJL += (matrixVerb->aJL[matrixVerb->countJ]);
		allpassJR += (matrixVerb->aJR[matrixVerb->countJ]);

		allpasstemp = matrixVerb->countK + 1;
		if (allpasstemp < 0 || allpasstemp > matrixVerb->delayK) {
			allpasstemp = 0;
		}
		allpassKL -= matrixVerb->aKL[allpasstemp] * 0.5;
		matrixVerb->aKL[matrixVerb->countK] = allpassKL;
		allpassKL *= 0.5;
		allpassKR -= matrixVerb->aKR[allpasstemp] * 0.5;
		matrixVerb->aKR[matrixVerb->countK] = allpassKR;
		allpassKR *= 0.5;
		matrixVerb->countK++;
		if (matrixVerb->countK < 0 || matrixVerb->countK > matrixVerb->delayK) {
			matrixVerb->countK = 0;
		}
		allpassKL += (matrixVerb->aKL[matrixVerb->countK]);
		allpassKR += (matrixVerb->aKR[matrixVerb->countK]);

		allpasstemp = matrixVerb->countL + 1;
		if (allpasstemp < 0 || allpasstemp > matrixVerb->delayL) {
			allpasstemp = 0;
		}
		allpassLL -= matrixVerb->aLL[allpasstemp] * 0.5;
		matrixVerb->aLL[matrixVerb->countL] = allpassLL;
		allpassLL *= 0.5;
		allpassLR -= matrixVerb->aLR[allpasstemp] * 0.5;
		matrixVerb->aLR[matrixVerb->countL] = allpassLR;
		allpassLR *= 0.5;
		matrixVerb->countL++;
		if (matrixVerb->countL < 0 || matrixVerb->countL > matrixVerb->delayL) {
			matrixVerb->countL = 0;
		}
		allpassLL += (matrixVerb->aLL[matrixVerb->countL]);
		allpassLR += (matrixVerb->aLR[matrixVerb->countL]);
		// the big allpass in front of everything

		matrixVerb->aAL[matrixVerb->countA] = allpassLL + matrixVerb->feedbackAL;
		matrixVerb->aBL[matrixVerb->countB] = allpassKL + matrixVerb->feedbackBL;
		matrixVerb->aCL[matrixVerb->countC] = allpassJL + matrixVerb->feedbackCL;
		matrixVerb->aDL[matrixVerb->countD] = allpassIL + matrixVerb->feedbackDL;
		matrixVerb->aEL[matrixVerb->countE] = allpassIL + matrixVerb->feedbackEL;
		matrixVerb->aFL[matrixVerb->countF] = allpassJL + matrixVerb->feedbackFL;
		matrixVerb->aGL[matrixVerb->countG] = allpassKL + matrixVerb->feedbackGL;
		matrixVerb->aHL[matrixVerb->countH] = allpassLL + matrixVerb->feedbackHL; // L

		matrixVerb->aAR[matrixVerb->countA] = allpassLR + matrixVerb->feedbackAR;
		matrixVerb->aBR[matrixVerb->countB] = allpassKR + matrixVerb->feedbackBR;
		matrixVerb->aCR[matrixVerb->countC] = allpassJR + matrixVerb->feedbackCR;
		matrixVerb->aDR[matrixVerb->countD] = allpassIR + matrixVerb->feedbackDR;
		matrixVerb->aER[matrixVerb->countE] = allpassIR + matrixVerb->feedbackER;
		matrixVerb->aFR[matrixVerb->countF] = allpassJR + matrixVerb->feedbackFR;
		matrixVerb->aGR[matrixVerb->countG] = allpassKR + matrixVerb->feedbackGR;
		matrixVerb->aHR[matrixVerb->countH] = allpassLR + matrixVerb->feedbackHR; // R

		matrixVerb->countA++;
		if (matrixVerb->countA < 0 || matrixVerb->countA > matrixVerb->delayA) {
			matrixVerb->countA = 0;
		}
		matrixVerb->countB++;
		if (matrixVerb->countB < 0 || matrixVerb->countB > matrixVerb->delayB) {
			matrixVerb->countB = 0;
		}
		matrixVerb->countC++;
		if (matrixVerb->countC < 0 || matrixVerb->countC > matrixVerb->delayC) {
			matrixVerb->countC = 0;
		}
		matrixVerb->countD++;
		if (matrixVerb->countD < 0 || matrixVerb->countD > matrixVerb->delayD) {
			matrixVerb->countD = 0;
		}
		matrixVerb->countE++;
		if (matrixVerb->countE < 0 || matrixVerb->countE > matrixVerb->delayE) {
			matrixVerb->countE = 0;
		}
		matrixVerb->countF++;
		if (matrixVerb->countF < 0 || matrixVerb->countF > matrixVerb->delayF) {
			matrixVerb->countF = 0;
		}
		matrixVerb->countG++;
		if (matrixVerb->countG < 0 || matrixVerb->countG > matrixVerb->delayG) {
			matrixVerb->countG = 0;
		}
		matrixVerb->countH++;
		if (matrixVerb->countH < 0 || matrixVerb->countH > matrixVerb->delayH) {
			matrixVerb->countH = 0;
		}
		// the Householder matrices (shared between channels, offset is stereo)

		matrixVerb->vibAL += (matrixVerb->depthA * vibSpeed);
		matrixVerb->vibBL += (matrixVerb->depthB * vibSpeed);
		matrixVerb->vibCL += (matrixVerb->depthC * vibSpeed);
		matrixVerb->vibDL += (matrixVerb->depthD * vibSpeed);
		matrixVerb->vibEL += (matrixVerb->depthE * vibSpeed);
		matrixVerb->vibFL += (matrixVerb->depthF * vibSpeed);
		matrixVerb->vibGL += (matrixVerb->depthG * vibSpeed);
		matrixVerb->vibHL += (matrixVerb->depthH * vibSpeed); // L

		matrixVerb->vibAR += (matrixVerb->depthA * vibSpeed);
		matrixVerb->vibBR += (matrixVerb->depthB * vibSpeed);
		matrixVerb->vibCR += (matrixVerb->depthC * vibSpeed);
		matrixVerb->vibDR += (matrixVerb->depthD * vibSpeed);
		matrixVerb->vibER += (matrixVerb->depthE * vibSpeed);
		matrixVerb->vibFR += (matrixVerb->depthF * vibSpeed);
		matrixVerb->vibGR += (matrixVerb->depthG * vibSpeed);
		matrixVerb->vibHR += (matrixVerb->depthH * vibSpeed); // R
		// Depth is shared, but each started at a random position

		double offsetAL = (sin(matrixVerb->vibAL) + 1.0) * vibDepth;
		double offsetBL = (sin(matrixVerb->vibBL) + 1.0) * vibDepth;
		double offsetCL = (sin(matrixVerb->vibCL) + 1.0) * vibDepth;
		double offsetDL = (sin(matrixVerb->vibDL) + 1.0) * vibDepth;
		double offsetEL = (sin(matrixVerb->vibEL) + 1.0) * vibDepth;
		double offsetFL = (sin(matrixVerb->vibFL) + 1.0) * vibDepth;
		double offsetGL = (sin(matrixVerb->vibGL) + 1.0) * vibDepth;
		double offsetHL = (sin(matrixVerb->vibHL) + 1.0) * vibDepth; // L

		double offsetAR = (sin(matrixVerb->vibAR) + 1.0) * vibDepth;
		double offsetBR = (sin(matrixVerb->vibBR) + 1.0) * vibDepth;
		double offsetCR = (sin(matrixVerb->vibCR) + 1.0) * vibDepth;
		double offsetDR = (sin(matrixVerb->vibDR) + 1.0) * vibDepth;
		double offsetER = (sin(matrixVerb->vibER) + 1.0) * vibDepth;
		double offsetFR = (sin(matrixVerb->vibFR) + 1.0) * vibDepth;
		double offsetGR = (sin(matrixVerb->vibGR) + 1.0) * vibDepth;
		double offsetHR = (sin(matrixVerb->vibHR) + 1.0) * vibDepth; // R

		int workingAL = matrixVerb->countA + offsetAL;
		int workingBL = matrixVerb->countB + offsetBL;
		int workingCL = matrixVerb->countC + offsetCL;
		int workingDL = matrixVerb->countD + offsetDL;
		int workingEL = matrixVerb->countE + offsetEL;
		int workingFL = matrixVerb->countF + offsetFL;
		int workingGL = matrixVerb->countG + offsetGL;
		int workingHL = matrixVerb->countH + offsetHL; // L

		int workingAR = matrixVerb->countA + offsetAR;
		int workingBR = matrixVerb->countB + offsetBR;
		int workingCR = matrixVerb->countC + offsetCR;
		int workingDR = matrixVerb->countD + offsetDR;
		int workingER = matrixVerb->countE + offsetER;
		int workingFR = matrixVerb->countF + offsetFR;
		int workingGR = matrixVerb->countG + offsetGR;
		int workingHR = matrixVerb->countH + offsetHR; // R

		double interpolAL = (matrixVerb->aAL[workingAL - ((workingAL > matrixVerb->delayA) ? matrixVerb->delayA + 1 : 0)] * (1 - (offsetAL - floor(offsetAL))));
		interpolAL += (matrixVerb->aAL[workingAL + 1 - ((workingAL + 1 > matrixVerb->delayA) ? matrixVerb->delayA + 1 : 0)] * ((offsetAL - floor(offsetAL))));

		double interpolBL = (matrixVerb->aBL[workingBL - ((workingBL > matrixVerb->delayB) ? matrixVerb->delayB + 1 : 0)] * (1 - (offsetBL - floor(offsetBL))));
		interpolBL += (matrixVerb->aBL[workingBL + 1 - ((workingBL + 1 > matrixVerb->delayB) ? matrixVerb->delayB + 1 : 0)] * ((offsetBL - floor(offsetBL))));

		double interpolCL = (matrixVerb->aCL[workingCL - ((workingCL > matrixVerb->delayC) ? matrixVerb->delayC + 1 : 0)] * (1 - (offsetCL - floor(offsetCL))));
		interpolCL += (matrixVerb->aCL[workingCL + 1 - ((workingCL + 1 > matrixVerb->delayC) ? matrixVerb->delayC + 1 : 0)] * ((offsetCL - floor(offsetCL))));

		double interpolDL = (matrixVerb->aDL[workingDL - ((workingDL > matrixVerb->delayD) ? matrixVerb->delayD + 1 : 0)] * (1 - (offsetDL - floor(offsetDL))));
		interpolDL += (matrixVerb->aDL[workingDL + 1 - ((workingDL + 1 > matrixVerb->delayD) ? matrixVerb->delayD + 1 : 0)] * ((offsetDL - floor(offsetDL))));

		double interpolEL = (matrixVerb->aEL[workingEL - ((workingEL > matrixVerb->delayE) ? matrixVerb->delayE + 1 : 0)] * (1 - (offsetEL - floor(offsetEL))));
		interpolEL += (matrixVerb->aEL[workingEL + 1 - ((workingEL + 1 > matrixVerb->delayE) ? matrixVerb->delayE + 1 : 0)] * ((offsetEL - floor(offsetEL))));

		double interpolFL = (matrixVerb->aFL[workingFL - ((workingFL > matrixVerb->delayF) ? matrixVerb->delayF + 1 : 0)] * (1 - (offsetFL - floor(offsetFL))));
		interpolFL += (matrixVerb->aFL[workingFL + 1 - ((workingFL + 1 > matrixVerb->delayF) ? matrixVerb->delayF + 1 : 0)] * ((offsetFL - floor(offsetFL))));

		double interpolGL = (matrixVerb->aGL[workingGL - ((workingGL > matrixVerb->delayG) ? matrixVerb->delayG + 1 : 0)] * (1 - (offsetGL - floor(offsetGL))));
		interpolGL += (matrixVerb->aGL[workingGL + 1 - ((workingGL + 1 > matrixVerb->delayG) ? matrixVerb->delayG + 1 : 0)] * ((offsetGL - floor(offsetGL))));

		double interpolHL = (matrixVerb->aHL[workingHL - ((workingHL > matrixVerb->delayH) ? matrixVerb->delayH + 1 : 0)] * (1 - (offsetHL - floor(offsetHL))));
		interpolHL += (matrixVerb->aHL[workingHL + 1 - ((workingHL + 1 > matrixVerb->delayH) ? matrixVerb->delayH + 1 : 0)] * ((offsetHL - floor(offsetHL))));
		// L

		double interpolAR = (matrixVerb->aAR[workingAR - ((workingAR > matrixVerb->delayA) ? matrixVerb->delayA + 1 : 0)] * (1 - (offsetAR - floor(offsetAR))));
		interpolAR += (matrixVerb->aAR[workingAR + 1 - ((workingAR + 1 > matrixVerb->delayA) ? matrixVerb->delayA + 1 : 0)] * ((offsetAR - floor(offsetAR))));

		double interpolBR = (matrixVerb->aBR[workingBR - ((workingBR > matrixVerb->delayB) ? matrixVerb->delayB + 1 : 0)] * (1 - (offsetBR - floor(offsetBR))));
		interpolBR += (matrixVerb->aBR[workingBR + 1 - ((workingBR + 1 > matrixVerb->delayB) ? matrixVerb->delayB + 1 : 0)] * ((offsetBR - floor(offsetBR))));

		double interpolCR = (matrixVerb->aCR[workingCR - ((workingCR > matrixVerb->delayC) ? matrixVerb->delayC + 1 : 0)] * (1 - (offsetCR - floor(offsetCR))));
		interpolCR += (matrixVerb->aCR[workingCR + 1 - ((workingCR + 1 > matrixVerb->delayC) ? matrixVerb->delayC + 1 : 0)] * ((offsetCR - floor(offsetCR))));

		double interpolDR = (matrixVerb->aDR[workingDR - ((workingDR > matrixVerb->delayD) ? matrixVerb->delayD + 1 : 0)] * (1 - (offsetDR - floor(offsetDR))));
		interpolDR += (matrixVerb->aDR[workingDR + 1 - ((workingDR + 1 > matrixVerb->delayD) ? matrixVerb->delayD + 1 : 0)] * ((offsetDR - floor(offsetDR))));

		double interpolER = (matrixVerb->aER[workingER - ((workingER > matrixVerb->delayE) ? matrixVerb->delayE + 1 : 0)] * (1 - (offsetER - floor(offsetER))));
		interpolER += (matrixVerb->aER[workingER + 1 - ((workingER + 1 > matrixVerb->delayE) ? matrixVerb->delayE + 1 : 0)] * ((offsetER - floor(offsetER))));

		double interpolFR = (matrixVerb->aFR[workingFR - ((workingFR > matrixVerb->delayF) ? matrixVerb->delayF + 1 : 0)] * (1 - (offsetFR - floor(offsetFR))));
		interpolFR += (matrixVerb->aFR[workingFR + 1 - ((workingFR + 1 > matrixVerb->delayF) ? matrixVerb->delayF + 1 : 0)] * ((offsetFR - floor(offsetFR))));

		double interpolGR = (matrixVerb->aGR[workingGR - ((workingGR > matrixVerb->delayG) ? matrixVerb->delayG + 1 : 0)] * (1 - (offsetGR - floor(offsetGR))));
		interpolGR += (matrixVerb->aGR[workingGR + 1 - ((workingGR + 1 > matrixVerb->delayG) ? matrixVerb->delayG + 1 : 0)] * ((offsetGR - floor(offsetGR))));

		double interpolHR = (matrixVerb->aHR[workingHR - ((workingHR > matrixVerb->delayH) ? matrixVerb->delayH + 1 : 0)] * (1 - (offsetHR - floor(offsetHR))));
		interpolHR += (matrixVerb->aHR[workingHR + 1 - ((workingHR + 1 > matrixVerb->delayH) ? matrixVerb->delayH + 1 : 0)] * ((offsetHR - floor(offsetHR))));
		// R

		interpolAL = ((1.0 - blend) * interpolAL) + (matrixVerb->aAL[workingAL - ((workingAL > matrixVerb->delayA) ? matrixVerb->delayA + 1 : 0)] * blend);
		interpolBL = ((1.0 - blend) * interpolBL) + (matrixVerb->aBL[workingBL - ((workingBL > matrixVerb->delayB) ? matrixVerb->delayB + 1 : 0)] * blend);
		interpolCL = ((1.0 - blend) * interpolCL) + (matrixVerb->aCL[workingCL - ((workingCL > matrixVerb->delayC) ? matrixVerb->delayC + 1 : 0)] * blend);
		interpolDL = ((1.0 - blend) * interpolDL) + (matrixVerb->aDL[workingDL - ((workingDL > matrixVerb->delayD) ? matrixVerb->delayD + 1 : 0)] * blend);
		interpolEL = ((1.0 - blend) * interpolEL) + (matrixVerb->aEL[workingEL - ((workingEL > matrixVerb->delayE) ? matrixVerb->delayE + 1 : 0)] * blend);
		interpolFL = ((1.0 - blend) * interpolFL) + (matrixVerb->aFL[workingFL - ((workingFL > matrixVerb->delayF) ? matrixVerb->delayF + 1 : 0)] * blend);
		interpolGL = ((1.0 - blend) * interpolGL) + (matrixVerb->aGL[workingGL - ((workingGL > matrixVerb->delayG) ? matrixVerb->delayG + 1 : 0)] * blend);
		interpolHL = ((1.0 - blend) * interpolHL) + (matrixVerb->aHL[workingHL - ((workingHL > matrixVerb->delayH) ? matrixVerb->delayH + 1 : 0)] * blend); // L

		interpolAR = ((1.0 - blend) * interpolAR) + (matrixVerb->aAR[workingAR - ((workingAR > matrixVerb->delayA) ? matrixVerb->delayA + 1 : 0)] * blend);
		interpolBR = ((1.0 - blend) * interpolBR) + (matrixVerb->aBR[workingBR - ((workingBR > matrixVerb->delayB) ? matrixVerb->delayB + 1 : 0)] * blend);
		interpolCR = ((1.0 - blend) * interpolCR) + (matrixVerb->aCR[workingCR - ((workingCR > matrixVerb->delayC) ? matrixVerb->delayC + 1 : 0)] * blend);
		interpolDR = ((1.0 - blend) * interpolDR) + (matrixVerb->aDR[workingDR - ((workingDR > matrixVerb->delayD) ? matrixVerb->delayD + 1 : 0)] * blend);
		interpolER = ((1.0 - blend) * interpolER) + (matrixVerb->aER[workingER - ((workingER > matrixVerb->delayE) ? matrixVerb->delayE + 1 : 0)] * blend);
		interpolFR = ((1.0 - blend) * interpolFR) + (matrixVerb->aFR[workingFR - ((workingFR > matrixVerb->delayF) ? matrixVerb->delayF + 1 : 0)] * blend);
		interpolGR = ((1.0 - blend) * interpolGR) + (matrixVerb->aGR[workingGR - ((workingGR > matrixVerb->delayG) ? matrixVerb->delayG + 1 : 0)] * blend);
		interpolHR = ((1.0 - blend) * interpolHR) + (matrixVerb->aHR[workingHR - ((workingHR > matrixVerb->delayH) ? matrixVerb->delayH + 1 : 0)] * blend); // R

		interpolAL = (interpolAL * (1.0 - fabs(crossmod))) + (interpolEL * crossmod);
		interpolEL = (interpolEL * (1.0 - fabs(crossmod))) + (interpolAL * crossmod); // L

		interpolAR = (interpolAR * (1.0 - fabs(crossmod))) + (interpolER * crossmod);
		interpolER = (interpolER * (1.0 - fabs(crossmod))) + (interpolAR * crossmod); // R

		matrixVerb->feedbackAL = (interpolAL - (interpolBL + interpolCL + interpolDL)) * regen;
		matrixVerb->feedbackBL = (interpolBL - (interpolAL + interpolCL + interpolDL)) * regen;
		matrixVerb->feedbackCL = (interpolCL - (interpolAL + interpolBL + interpolDL)) * regen;
		matrixVerb->feedbackDL = (interpolDL - (interpolAL + interpolBL + interpolCL)) * regen; // Householder feedback matrix, L

		matrixVerb->feedbackEL = (interpolEL - (interpolFL + interpolGL + interpolHL)) * regen;
		matrixVerb->feedbackFL = (interpolFL - (interpolEL + interpolGL + interpolHL)) * regen;
		matrixVerb->feedbackGL = (interpolGL - (interpolEL + interpolFL + interpolHL)) * regen;
		matrixVerb->feedbackHL = (interpolHL - (interpolEL + interpolFL + interpolGL)) * regen; // Householder feedback matrix, L

		matrixVerb->feedbackAR = (interpolAR - (interpolBR + interpolCR + interpolDR)) * regen;
		matrixVerb->feedbackBR = (interpolBR - (interpolAR + interpolCR + interpolDR)) * regen;
		matrixVerb->feedbackCR = (interpolCR - (interpolAR + interpolBR + interpolDR)) * regen;
		matrixVerb->feedbackDR = (interpolDR - (interpolAR + interpolBR + interpolCR)) * regen; // Householder feedback matrix, R

		matrixVerb->feedbackER = (interpolER - (interpolFR + interpolGR + interpolHR)) * regen;
		matrixVerb->feedbackFR = (interpolFR - (interpolER + interpolGR + interpolHR)) * regen;
		matrixVerb->feedbackGR = (interpolGR - (interpolER + interpolFR + interpolHR)) * regen;
		matrixVerb->feedbackHR = (interpolHR - (interpolER + interpolFR + interpolGR)) * regen; // Householder feedback matrix, R

		inputSampleL = (interpolAL + interpolBL + interpolCL + interpolDL + interpolEL + interpolFL + interpolGL + interpolHL) / 8.0;
		inputSampleR = (interpolAR + interpolBR + interpolCR + interpolDR + interpolER + interpolFR + interpolGR + interpolHR) / 8.0;

		tempSampleL = (inputSampleL * matrixVerb->biquadB[2]) + matrixVerb->biquadB[7];
		matrixVerb->biquadB[7] = (inputSampleL * matrixVerb->biquadB[3]) - (tempSampleL * matrixVerb->biquadB[5]) + matrixVerb->biquadB[8];
		matrixVerb->biquadB[8] = (inputSampleL * matrixVerb->biquadB[4]) - (tempSampleL * matrixVerb->biquadB[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * matrixVerb->biquadB[2]) + matrixVerb->biquadB[9];
		matrixVerb->biquadB[9] = (inputSampleR * matrixVerb->biquadB[3]) - (tempSampleR * matrixVerb->biquadB[5]) + matrixVerb->biquadB[10];
		matrixVerb->biquadB[10] = (inputSampleR * matrixVerb->biquadB[4]) - (tempSampleR * matrixVerb->biquadB[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);

		tempSampleL = (inputSampleL * matrixVerb->biquadC[2]) + matrixVerb->biquadC[7];
		matrixVerb->biquadC[7] = (inputSampleL * matrixVerb->biquadC[3]) - (tempSampleL * matrixVerb->biquadC[5]) + matrixVerb->biquadC[8];
		matrixVerb->biquadC[8] = (inputSampleL * matrixVerb->biquadC[4]) - (tempSampleL * matrixVerb->biquadC[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * matrixVerb->biquadC[2]) + matrixVerb->biquadC[9];
		matrixVerb->biquadC[9] = (inputSampleR * matrixVerb->biquadC[3]) - (tempSampleR * matrixVerb->biquadC[5]) + matrixVerb->biquadC[10];
		matrixVerb->biquadC[10] = (inputSampleR * matrixVerb->biquadC[4]) - (tempSampleR * matrixVerb->biquadC[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (wet != 1.0) {
			inputSampleL += (drySampleL * (1.0 - wet));
			inputSampleR += (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		matrixVerb->fpdL ^= matrixVerb->fpdL << 13;
		matrixVerb->fpdL ^= matrixVerb->fpdL >> 17;
		matrixVerb->fpdL ^= matrixVerb->fpdL << 5;
		inputSampleL += (((double) matrixVerb->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		matrixVerb->fpdR ^= matrixVerb->fpdR << 13;
		matrixVerb->fpdR ^= matrixVerb->fpdR >> 17;
		matrixVerb->fpdR ^= matrixVerb->fpdR << 5;
		inputSampleR += (((double) matrixVerb->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MATRIXVERB_URI,
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
