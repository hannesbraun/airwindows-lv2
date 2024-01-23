#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define REVERB_URI "https://hannesbraun.net/ns/lv2/airwindows/reverb"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	BIG = 4,
	WET = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* big;
	const float* wet;

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
} Reverb;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Reverb* reverb = (Reverb*) calloc(1, sizeof(Reverb));
	reverb->sampleRate = rate;
	return (LV2_Handle) reverb;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Reverb* reverb = (Reverb*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			reverb->input[0] = (const float*) data;
			break;
		case INPUT_R:
			reverb->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			reverb->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			reverb->output[1] = (float*) data;
			break;
		case BIG:
			reverb->big = (const float*) data;
			break;
		case WET:
			reverb->wet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Reverb* reverb = (Reverb*) instance;

	for (int x = 0; x < 11; x++) {
		reverb->biquadA[x] = 0.0;
		reverb->biquadB[x] = 0.0;
		reverb->biquadC[x] = 0.0;
	}

	reverb->feedbackAL = reverb->feedbackAR = 0.0;
	reverb->feedbackBL = reverb->feedbackBR = 0.0;
	reverb->feedbackCL = reverb->feedbackCR = 0.0;
	reverb->feedbackDL = reverb->feedbackDR = 0.0;
	reverb->feedbackEL = reverb->feedbackER = 0.0;
	reverb->feedbackFL = reverb->feedbackFR = 0.0;
	reverb->feedbackGL = reverb->feedbackGR = 0.0;
	reverb->feedbackHL = reverb->feedbackHR = 0.0;

	int count;
	for (count = 0; count < 8110; count++) {
		reverb->aAL[count] = reverb->aAR[count] = 0.0;
	}
	for (count = 0; count < 7510; count++) {
		reverb->aBL[count] = reverb->aBR[count] = 0.0;
	}
	for (count = 0; count < 7310; count++) {
		reverb->aCL[count] = reverb->aCR[count] = 0.0;
	}
	for (count = 0; count < 6910; count++) {
		reverb->aDL[count] = reverb->aDR[count] = 0.0;
	}
	for (count = 0; count < 6310; count++) {
		reverb->aEL[count] = reverb->aER[count] = 0.0;
	}
	for (count = 0; count < 6110; count++) {
		reverb->aFL[count] = reverb->aFR[count] = 0.0;
	}
	for (count = 0; count < 5510; count++) {
		reverb->aGL[count] = reverb->aGR[count] = 0.0;
	}
	for (count = 0; count < 4910; count++) {
		reverb->aHL[count] = reverb->aHR[count] = 0.0;
	}
	// maximum value needed will be delay * 100, plus 206 (absolute max vibrato depth)
	for (count = 0; count < 4510; count++) {
		reverb->aIL[count] = reverb->aIR[count] = 0.0;
	}
	for (count = 0; count < 4310; count++) {
		reverb->aJL[count] = reverb->aJR[count] = 0.0;
	}
	for (count = 0; count < 3910; count++) {
		reverb->aKL[count] = reverb->aKR[count] = 0.0;
	}
	for (count = 0; count < 3310; count++) {
		reverb->aLL[count] = reverb->aLR[count] = 0.0;
	}
	// maximum value will be delay * 100
	for (count = 0; count < 3110; count++) {
		reverb->aML[count] = reverb->aMR[count] = 0.0;
	}
	// maximum value will be delay * 100
	reverb->countA = 1;
	reverb->delayA = 79;
	reverb->countB = 1;
	reverb->delayB = 73;
	reverb->countC = 1;
	reverb->delayC = 71;
	reverb->countD = 1;
	reverb->delayD = 67;
	reverb->countE = 1;
	reverb->delayE = 61;
	reverb->countF = 1;
	reverb->delayF = 59;
	reverb->countG = 1;
	reverb->delayG = 53;
	reverb->countH = 1;
	reverb->delayH = 47;
	// the householder matrices
	reverb->countI = 1;
	reverb->delayI = 43;
	reverb->countJ = 1;
	reverb->delayJ = 41;
	reverb->countK = 1;
	reverb->delayK = 37;
	reverb->countL = 1;
	reverb->delayL = 31;
	// the allpasses
	reverb->countM = 1;
	reverb->delayM = 29;
	// the predelay
	reverb->depthA = 0.003251;
	reverb->depthB = 0.002999;
	reverb->depthC = 0.002917;
	reverb->depthD = 0.002749;
	reverb->depthE = 0.002503;
	reverb->depthF = 0.002423;
	reverb->depthG = 0.002146;
	reverb->depthH = 0.002088;
	// the individual vibrato rates for the delays
	reverb->vibAL = rand() * -2147483647;
	reverb->vibBL = rand() * -2147483647;
	reverb->vibCL = rand() * -2147483647;
	reverb->vibDL = rand() * -2147483647;
	reverb->vibEL = rand() * -2147483647;
	reverb->vibFL = rand() * -2147483647;
	reverb->vibGL = rand() * -2147483647;
	reverb->vibHL = rand() * -2147483647;

	reverb->vibAR = rand() * -2147483647;
	reverb->vibBR = rand() * -2147483647;
	reverb->vibCR = rand() * -2147483647;
	reverb->vibDR = rand() * -2147483647;
	reverb->vibER = rand() * -2147483647;
	reverb->vibFR = rand() * -2147483647;
	reverb->vibGR = rand() * -2147483647;
	reverb->vibHR = rand() * -2147483647;

	reverb->fpdL = 1.0;
	while (reverb->fpdL < 16386) reverb->fpdL = rand() * UINT32_MAX;
	reverb->fpdR = 1.0;
	while (reverb->fpdR < 16386) reverb->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Reverb* reverb = (Reverb*) instance;

	const float* in1 = reverb->input[0];
	const float* in2 = reverb->input[1];
	float* out1 = reverb->output[0];
	float* out2 = reverb->output[1];

	double big = *reverb->big;
	double wet = *reverb->wet;
	double vibSpeed = 0.1;
	double vibDepth = 7.0;
	double size = (pow(big, 2) * 75.0) + 25.0;
	double depthFactor = 1.0 - pow((1.0 - (0.82 - (((1.0 - big) * 0.7) + (size * 0.002)))), 4);
	double blend = 0.955 - (size * 0.007);
	double regen = depthFactor * 0.5;

	reverb->delayA = 79 * size;
	reverb->delayB = 73 * size;
	reverb->delayC = 71 * size;
	reverb->delayD = 67 * size;
	reverb->delayE = 61 * size;
	reverb->delayF = 59 * size;
	reverb->delayG = 53 * size;
	reverb->delayH = 47 * size;
	reverb->delayI = 43 * size;
	reverb->delayJ = 41 * size;
	reverb->delayK = 37 * size;
	reverb->delayL = 31 * size;
	reverb->delayM = 29 * size;

	reverb->biquadC[0] = reverb->biquadB[0] = reverb->biquadA[0] = (10000.0 - (big * wet * 3000.0)) / reverb->sampleRate;
	reverb->biquadA[1] = 1.618033988749894848204586;
	reverb->biquadB[1] = 0.618033988749894848204586;
	reverb->biquadC[1] = 0.5;

	double K = tan(M_PI * reverb->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / reverb->biquadA[1] + K * K);
	reverb->biquadA[2] = K * K * norm;
	reverb->biquadA[3] = 2.0 * reverb->biquadA[2];
	reverb->biquadA[4] = reverb->biquadA[2];
	reverb->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	reverb->biquadA[6] = (1.0 - K / reverb->biquadA[1] + K * K) * norm;

	K = tan(M_PI * reverb->biquadA[0]);
	norm = 1.0 / (1.0 + K / reverb->biquadB[1] + K * K);
	reverb->biquadB[2] = K * K * norm;
	reverb->biquadB[3] = 2.0 * reverb->biquadB[2];
	reverb->biquadB[4] = reverb->biquadB[2];
	reverb->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	reverb->biquadB[6] = (1.0 - K / reverb->biquadB[1] + K * K) * norm;

	K = tan(M_PI * reverb->biquadC[0]);
	norm = 1.0 / (1.0 + K / reverb->biquadC[1] + K * K);
	reverb->biquadC[2] = K * K * norm;
	reverb->biquadC[3] = 2.0 * reverb->biquadC[2];
	reverb->biquadC[4] = reverb->biquadC[2];
	reverb->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	reverb->biquadC[6] = (1.0 - K / reverb->biquadC[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = reverb->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = reverb->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		reverb->aML[reverb->countM] = inputSampleL;
		reverb->aMR[reverb->countM] = inputSampleR;
		reverb->countM++;
		if (reverb->countM < 0 || reverb->countM > reverb->delayM) {
			reverb->countM = 0;
		}
		inputSampleL = reverb->aML[reverb->countM];
		inputSampleR = reverb->aMR[reverb->countM];
		// predelay

		double tempSampleL = (inputSampleL * reverb->biquadA[2]) + reverb->biquadA[7];
		reverb->biquadA[7] = (inputSampleL * reverb->biquadA[3]) - (tempSampleL * reverb->biquadA[5]) + reverb->biquadA[8];
		reverb->biquadA[8] = (inputSampleL * reverb->biquadA[4]) - (tempSampleL * reverb->biquadA[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		double tempSampleR = (inputSampleR * reverb->biquadA[2]) + reverb->biquadA[9];
		reverb->biquadA[9] = (inputSampleR * reverb->biquadA[3]) - (tempSampleR * reverb->biquadA[5]) + reverb->biquadA[10];
		reverb->biquadA[10] = (inputSampleR * reverb->biquadA[4]) - (tempSampleR * reverb->biquadA[6]);
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

		int allpasstemp = reverb->countI + 1;
		if (allpasstemp < 0 || allpasstemp > reverb->delayI) {
			allpasstemp = 0;
		}
		allpassIL -= reverb->aIL[allpasstemp] * 0.5;
		reverb->aIL[reverb->countI] = allpassIL;
		allpassIL *= 0.5;
		allpassIR -= reverb->aIR[allpasstemp] * 0.5;
		reverb->aIR[reverb->countI] = allpassIR;
		allpassIR *= 0.5;
		reverb->countI++;
		if (reverb->countI < 0 || reverb->countI > reverb->delayI) {
			reverb->countI = 0;
		}
		allpassIL += (reverb->aIL[reverb->countI]);
		allpassIR += (reverb->aIR[reverb->countI]);

		allpasstemp = reverb->countJ + 1;
		if (allpasstemp < 0 || allpasstemp > reverb->delayJ) {
			allpasstemp = 0;
		}
		allpassJL -= reverb->aJL[allpasstemp] * 0.5;
		reverb->aJL[reverb->countJ] = allpassJL;
		allpassJL *= 0.5;
		allpassJR -= reverb->aJR[allpasstemp] * 0.5;
		reverb->aJR[reverb->countJ] = allpassJR;
		allpassJR *= 0.5;
		reverb->countJ++;
		if (reverb->countJ < 0 || reverb->countJ > reverb->delayJ) {
			reverb->countJ = 0;
		}
		allpassJL += (reverb->aJL[reverb->countJ]);
		allpassJR += (reverb->aJR[reverb->countJ]);

		allpasstemp = reverb->countK + 1;
		if (allpasstemp < 0 || allpasstemp > reverb->delayK) {
			allpasstemp = 0;
		}
		allpassKL -= reverb->aKL[allpasstemp] * 0.5;
		reverb->aKL[reverb->countK] = allpassKL;
		allpassKL *= 0.5;
		allpassKR -= reverb->aKR[allpasstemp] * 0.5;
		reverb->aKR[reverb->countK] = allpassKR;
		allpassKR *= 0.5;
		reverb->countK++;
		if (reverb->countK < 0 || reverb->countK > reverb->delayK) {
			reverb->countK = 0;
		}
		allpassKL += (reverb->aKL[reverb->countK]);
		allpassKR += (reverb->aKR[reverb->countK]);

		allpasstemp = reverb->countL + 1;
		if (allpasstemp < 0 || allpasstemp > reverb->delayL) {
			allpasstemp = 0;
		}
		allpassLL -= reverb->aLL[allpasstemp] * 0.5;
		reverb->aLL[reverb->countL] = allpassLL;
		allpassLL *= 0.5;
		allpassLR -= reverb->aLR[allpasstemp] * 0.5;
		reverb->aLR[reverb->countL] = allpassLR;
		allpassLR *= 0.5;
		reverb->countL++;
		if (reverb->countL < 0 || reverb->countL > reverb->delayL) {
			reverb->countL = 0;
		}
		allpassLL += (reverb->aLL[reverb->countL]);
		allpassLR += (reverb->aLR[reverb->countL]);
		// the big allpass in front of everything

		reverb->aAL[reverb->countA] = allpassLL + reverb->feedbackAL;
		reverb->aBL[reverb->countB] = allpassKL + reverb->feedbackBL;
		reverb->aCL[reverb->countC] = allpassJL + reverb->feedbackCL;
		reverb->aDL[reverb->countD] = allpassIL + reverb->feedbackDL;
		reverb->aEL[reverb->countE] = allpassIL + reverb->feedbackEL;
		reverb->aFL[reverb->countF] = allpassJL + reverb->feedbackFL;
		reverb->aGL[reverb->countG] = allpassKL + reverb->feedbackGL;
		reverb->aHL[reverb->countH] = allpassLL + reverb->feedbackHL; // L

		reverb->aAR[reverb->countA] = allpassLR + reverb->feedbackAR;
		reverb->aBR[reverb->countB] = allpassKR + reverb->feedbackBR;
		reverb->aCR[reverb->countC] = allpassJR + reverb->feedbackCR;
		reverb->aDR[reverb->countD] = allpassIR + reverb->feedbackDR;
		reverb->aER[reverb->countE] = allpassIR + reverb->feedbackER;
		reverb->aFR[reverb->countF] = allpassJR + reverb->feedbackFR;
		reverb->aGR[reverb->countG] = allpassKR + reverb->feedbackGR;
		reverb->aHR[reverb->countH] = allpassLR + reverb->feedbackHR; // R

		reverb->countA++;
		if (reverb->countA < 0 || reverb->countA > reverb->delayA) {
			reverb->countA = 0;
		}
		reverb->countB++;
		if (reverb->countB < 0 || reverb->countB > reverb->delayB) {
			reverb->countB = 0;
		}
		reverb->countC++;
		if (reverb->countC < 0 || reverb->countC > reverb->delayC) {
			reverb->countC = 0;
		}
		reverb->countD++;
		if (reverb->countD < 0 || reverb->countD > reverb->delayD) {
			reverb->countD = 0;
		}
		reverb->countE++;
		if (reverb->countE < 0 || reverb->countE > reverb->delayE) {
			reverb->countE = 0;
		}
		reverb->countF++;
		if (reverb->countF < 0 || reverb->countF > reverb->delayF) {
			reverb->countF = 0;
		}
		reverb->countG++;
		if (reverb->countG < 0 || reverb->countG > reverb->delayG) {
			reverb->countG = 0;
		}
		reverb->countH++;
		if (reverb->countH < 0 || reverb->countH > reverb->delayH) {
			reverb->countH = 0;
		}
		// the Householder matrices (shared between channels, offset is stereo)

		reverb->vibAL += (reverb->depthA * vibSpeed);
		reverb->vibBL += (reverb->depthB * vibSpeed);
		reverb->vibCL += (reverb->depthC * vibSpeed);
		reverb->vibDL += (reverb->depthD * vibSpeed);
		reverb->vibEL += (reverb->depthE * vibSpeed);
		reverb->vibFL += (reverb->depthF * vibSpeed);
		reverb->vibGL += (reverb->depthG * vibSpeed);
		reverb->vibHL += (reverb->depthH * vibSpeed); // L

		reverb->vibAR += (reverb->depthA * vibSpeed);
		reverb->vibBR += (reverb->depthB * vibSpeed);
		reverb->vibCR += (reverb->depthC * vibSpeed);
		reverb->vibDR += (reverb->depthD * vibSpeed);
		reverb->vibER += (reverb->depthE * vibSpeed);
		reverb->vibFR += (reverb->depthF * vibSpeed);
		reverb->vibGR += (reverb->depthG * vibSpeed);
		reverb->vibHR += (reverb->depthH * vibSpeed); // R
		// Depth is shared, but each started at a random position

		double offsetAL = (sin(reverb->vibAL) + 1.0) * vibDepth;
		double offsetBL = (sin(reverb->vibBL) + 1.0) * vibDepth;
		double offsetCL = (sin(reverb->vibCL) + 1.0) * vibDepth;
		double offsetDL = (sin(reverb->vibDL) + 1.0) * vibDepth;
		double offsetEL = (sin(reverb->vibEL) + 1.0) * vibDepth;
		double offsetFL = (sin(reverb->vibFL) + 1.0) * vibDepth;
		double offsetGL = (sin(reverb->vibGL) + 1.0) * vibDepth;
		double offsetHL = (sin(reverb->vibHL) + 1.0) * vibDepth; // L

		double offsetAR = (sin(reverb->vibAR) + 1.0) * vibDepth;
		double offsetBR = (sin(reverb->vibBR) + 1.0) * vibDepth;
		double offsetCR = (sin(reverb->vibCR) + 1.0) * vibDepth;
		double offsetDR = (sin(reverb->vibDR) + 1.0) * vibDepth;
		double offsetER = (sin(reverb->vibER) + 1.0) * vibDepth;
		double offsetFR = (sin(reverb->vibFR) + 1.0) * vibDepth;
		double offsetGR = (sin(reverb->vibGR) + 1.0) * vibDepth;
		double offsetHR = (sin(reverb->vibHR) + 1.0) * vibDepth; // R

		int workingAL = reverb->countA + offsetAL;
		int workingBL = reverb->countB + offsetBL;
		int workingCL = reverb->countC + offsetCL;
		int workingDL = reverb->countD + offsetDL;
		int workingEL = reverb->countE + offsetEL;
		int workingFL = reverb->countF + offsetFL;
		int workingGL = reverb->countG + offsetGL;
		int workingHL = reverb->countH + offsetHL; // L

		int workingAR = reverb->countA + offsetAR;
		int workingBR = reverb->countB + offsetBR;
		int workingCR = reverb->countC + offsetCR;
		int workingDR = reverb->countD + offsetDR;
		int workingER = reverb->countE + offsetER;
		int workingFR = reverb->countF + offsetFR;
		int workingGR = reverb->countG + offsetGR;
		int workingHR = reverb->countH + offsetHR; // R

		double interpolAL = (reverb->aAL[workingAL - ((workingAL > reverb->delayA) ? reverb->delayA + 1 : 0)] * (1 - (offsetAL - floor(offsetAL))));
		interpolAL += (reverb->aAL[workingAL + 1 - ((workingAL + 1 > reverb->delayA) ? reverb->delayA + 1 : 0)] * ((offsetAL - floor(offsetAL))));

		double interpolBL = (reverb->aBL[workingBL - ((workingBL > reverb->delayB) ? reverb->delayB + 1 : 0)] * (1 - (offsetBL - floor(offsetBL))));
		interpolBL += (reverb->aBL[workingBL + 1 - ((workingBL + 1 > reverb->delayB) ? reverb->delayB + 1 : 0)] * ((offsetBL - floor(offsetBL))));

		double interpolCL = (reverb->aCL[workingCL - ((workingCL > reverb->delayC) ? reverb->delayC + 1 : 0)] * (1 - (offsetCL - floor(offsetCL))));
		interpolCL += (reverb->aCL[workingCL + 1 - ((workingCL + 1 > reverb->delayC) ? reverb->delayC + 1 : 0)] * ((offsetCL - floor(offsetCL))));

		double interpolDL = (reverb->aDL[workingDL - ((workingDL > reverb->delayD) ? reverb->delayD + 1 : 0)] * (1 - (offsetDL - floor(offsetDL))));
		interpolDL += (reverb->aDL[workingDL + 1 - ((workingDL + 1 > reverb->delayD) ? reverb->delayD + 1 : 0)] * ((offsetDL - floor(offsetDL))));

		double interpolEL = (reverb->aEL[workingEL - ((workingEL > reverb->delayE) ? reverb->delayE + 1 : 0)] * (1 - (offsetEL - floor(offsetEL))));
		interpolEL += (reverb->aEL[workingEL + 1 - ((workingEL + 1 > reverb->delayE) ? reverb->delayE + 1 : 0)] * ((offsetEL - floor(offsetEL))));

		double interpolFL = (reverb->aFL[workingFL - ((workingFL > reverb->delayF) ? reverb->delayF + 1 : 0)] * (1 - (offsetFL - floor(offsetFL))));
		interpolFL += (reverb->aFL[workingFL + 1 - ((workingFL + 1 > reverb->delayF) ? reverb->delayF + 1 : 0)] * ((offsetFL - floor(offsetFL))));

		double interpolGL = (reverb->aGL[workingGL - ((workingGL > reverb->delayG) ? reverb->delayG + 1 : 0)] * (1 - (offsetGL - floor(offsetGL))));
		interpolGL += (reverb->aGL[workingGL + 1 - ((workingGL + 1 > reverb->delayG) ? reverb->delayG + 1 : 0)] * ((offsetGL - floor(offsetGL))));

		double interpolHL = (reverb->aHL[workingHL - ((workingHL > reverb->delayH) ? reverb->delayH + 1 : 0)] * (1 - (offsetHL - floor(offsetHL))));
		interpolHL += (reverb->aHL[workingHL + 1 - ((workingHL + 1 > reverb->delayH) ? reverb->delayH + 1 : 0)] * ((offsetHL - floor(offsetHL))));
		// L

		double interpolAR = (reverb->aAR[workingAR - ((workingAR > reverb->delayA) ? reverb->delayA + 1 : 0)] * (1 - (offsetAR - floor(offsetAR))));
		interpolAR += (reverb->aAR[workingAR + 1 - ((workingAR + 1 > reverb->delayA) ? reverb->delayA + 1 : 0)] * ((offsetAR - floor(offsetAR))));

		double interpolBR = (reverb->aBR[workingBR - ((workingBR > reverb->delayB) ? reverb->delayB + 1 : 0)] * (1 - (offsetBR - floor(offsetBR))));
		interpolBR += (reverb->aBR[workingBR + 1 - ((workingBR + 1 > reverb->delayB) ? reverb->delayB + 1 : 0)] * ((offsetBR - floor(offsetBR))));

		double interpolCR = (reverb->aCR[workingCR - ((workingCR > reverb->delayC) ? reverb->delayC + 1 : 0)] * (1 - (offsetCR - floor(offsetCR))));
		interpolCR += (reverb->aCR[workingCR + 1 - ((workingCR + 1 > reverb->delayC) ? reverb->delayC + 1 : 0)] * ((offsetCR - floor(offsetCR))));

		double interpolDR = (reverb->aDR[workingDR - ((workingDR > reverb->delayD) ? reverb->delayD + 1 : 0)] * (1 - (offsetDR - floor(offsetDR))));
		interpolDR += (reverb->aDR[workingDR + 1 - ((workingDR + 1 > reverb->delayD) ? reverb->delayD + 1 : 0)] * ((offsetDR - floor(offsetDR))));

		double interpolER = (reverb->aER[workingER - ((workingER > reverb->delayE) ? reverb->delayE + 1 : 0)] * (1 - (offsetER - floor(offsetER))));
		interpolER += (reverb->aER[workingER + 1 - ((workingER + 1 > reverb->delayE) ? reverb->delayE + 1 : 0)] * ((offsetER - floor(offsetER))));

		double interpolFR = (reverb->aFR[workingFR - ((workingFR > reverb->delayF) ? reverb->delayF + 1 : 0)] * (1 - (offsetFR - floor(offsetFR))));
		interpolFR += (reverb->aFR[workingFR + 1 - ((workingFR + 1 > reverb->delayF) ? reverb->delayF + 1 : 0)] * ((offsetFR - floor(offsetFR))));

		double interpolGR = (reverb->aGR[workingGR - ((workingGR > reverb->delayG) ? reverb->delayG + 1 : 0)] * (1 - (offsetGR - floor(offsetGR))));
		interpolGR += (reverb->aGR[workingGR + 1 - ((workingGR + 1 > reverb->delayG) ? reverb->delayG + 1 : 0)] * ((offsetGR - floor(offsetGR))));

		double interpolHR = (reverb->aHR[workingHR - ((workingHR > reverb->delayH) ? reverb->delayH + 1 : 0)] * (1 - (offsetHR - floor(offsetHR))));
		interpolHR += (reverb->aHR[workingHR + 1 - ((workingHR + 1 > reverb->delayH) ? reverb->delayH + 1 : 0)] * ((offsetHR - floor(offsetHR))));
		// R

		interpolAL = ((1.0 - blend) * interpolAL) + (reverb->aAL[workingAL - ((workingAL > reverb->delayA) ? reverb->delayA + 1 : 0)] * blend);
		interpolBL = ((1.0 - blend) * interpolBL) + (reverb->aBL[workingBL - ((workingBL > reverb->delayB) ? reverb->delayB + 1 : 0)] * blend);
		interpolCL = ((1.0 - blend) * interpolCL) + (reverb->aCL[workingCL - ((workingCL > reverb->delayC) ? reverb->delayC + 1 : 0)] * blend);
		interpolDL = ((1.0 - blend) * interpolDL) + (reverb->aDL[workingDL - ((workingDL > reverb->delayD) ? reverb->delayD + 1 : 0)] * blend);
		interpolEL = ((1.0 - blend) * interpolEL) + (reverb->aEL[workingEL - ((workingEL > reverb->delayE) ? reverb->delayE + 1 : 0)] * blend);
		interpolFL = ((1.0 - blend) * interpolFL) + (reverb->aFL[workingFL - ((workingFL > reverb->delayF) ? reverb->delayF + 1 : 0)] * blend);
		interpolGL = ((1.0 - blend) * interpolGL) + (reverb->aGL[workingGL - ((workingGL > reverb->delayG) ? reverb->delayG + 1 : 0)] * blend);
		interpolHL = ((1.0 - blend) * interpolHL) + (reverb->aHL[workingHL - ((workingHL > reverb->delayH) ? reverb->delayH + 1 : 0)] * blend); // L

		interpolAR = ((1.0 - blend) * interpolAR) + (reverb->aAR[workingAR - ((workingAR > reverb->delayA) ? reverb->delayA + 1 : 0)] * blend);
		interpolBR = ((1.0 - blend) * interpolBR) + (reverb->aBR[workingBR - ((workingBR > reverb->delayB) ? reverb->delayB + 1 : 0)] * blend);
		interpolCR = ((1.0 - blend) * interpolCR) + (reverb->aCR[workingCR - ((workingCR > reverb->delayC) ? reverb->delayC + 1 : 0)] * blend);
		interpolDR = ((1.0 - blend) * interpolDR) + (reverb->aDR[workingDR - ((workingDR > reverb->delayD) ? reverb->delayD + 1 : 0)] * blend);
		interpolER = ((1.0 - blend) * interpolER) + (reverb->aER[workingER - ((workingER > reverb->delayE) ? reverb->delayE + 1 : 0)] * blend);
		interpolFR = ((1.0 - blend) * interpolFR) + (reverb->aFR[workingFR - ((workingFR > reverb->delayF) ? reverb->delayF + 1 : 0)] * blend);
		interpolGR = ((1.0 - blend) * interpolGR) + (reverb->aGR[workingGR - ((workingGR > reverb->delayG) ? reverb->delayG + 1 : 0)] * blend);
		interpolHR = ((1.0 - blend) * interpolHR) + (reverb->aHR[workingHR - ((workingHR > reverb->delayH) ? reverb->delayH + 1 : 0)] * blend); // R

		reverb->feedbackAL = (interpolAL - (interpolBL + interpolCL + interpolDL)) * regen;
		reverb->feedbackBL = (interpolBL - (interpolAL + interpolCL + interpolDL)) * regen;
		reverb->feedbackCL = (interpolCL - (interpolAL + interpolBL + interpolDL)) * regen;
		reverb->feedbackDL = (interpolDL - (interpolAL + interpolBL + interpolCL)) * regen; // Householder feedback matrix, L

		reverb->feedbackEL = (interpolEL - (interpolFL + interpolGL + interpolHL)) * regen;
		reverb->feedbackFL = (interpolFL - (interpolEL + interpolGL + interpolHL)) * regen;
		reverb->feedbackGL = (interpolGL - (interpolEL + interpolFL + interpolHL)) * regen;
		reverb->feedbackHL = (interpolHL - (interpolEL + interpolFL + interpolGL)) * regen; // Householder feedback matrix, L

		reverb->feedbackAR = (interpolAR - (interpolBR + interpolCR + interpolDR)) * regen;
		reverb->feedbackBR = (interpolBR - (interpolAR + interpolCR + interpolDR)) * regen;
		reverb->feedbackCR = (interpolCR - (interpolAR + interpolBR + interpolDR)) * regen;
		reverb->feedbackDR = (interpolDR - (interpolAR + interpolBR + interpolCR)) * regen; // Householder feedback matrix, R

		reverb->feedbackER = (interpolER - (interpolFR + interpolGR + interpolHR)) * regen;
		reverb->feedbackFR = (interpolFR - (interpolER + interpolGR + interpolHR)) * regen;
		reverb->feedbackGR = (interpolGR - (interpolER + interpolFR + interpolHR)) * regen;
		reverb->feedbackHR = (interpolHR - (interpolER + interpolFR + interpolGR)) * regen; // Householder feedback matrix, R

		inputSampleL = (interpolAL + interpolBL + interpolCL + interpolDL + interpolEL + interpolFL + interpolGL + interpolHL) / 8.0;
		inputSampleR = (interpolAR + interpolBR + interpolCR + interpolDR + interpolER + interpolFR + interpolGR + interpolHR) / 8.0;

		tempSampleL = (inputSampleL * reverb->biquadB[2]) + reverb->biquadB[7];
		reverb->biquadB[7] = (inputSampleL * reverb->biquadB[3]) - (tempSampleL * reverb->biquadB[5]) + reverb->biquadB[8];
		reverb->biquadB[8] = (inputSampleL * reverb->biquadB[4]) - (tempSampleL * reverb->biquadB[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * reverb->biquadB[2]) + reverb->biquadB[9];
		reverb->biquadB[9] = (inputSampleR * reverb->biquadB[3]) - (tempSampleR * reverb->biquadB[5]) + reverb->biquadB[10];
		reverb->biquadB[10] = (inputSampleR * reverb->biquadB[4]) - (tempSampleR * reverb->biquadB[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);

		tempSampleL = (inputSampleL * reverb->biquadC[2]) + reverb->biquadC[7];
		reverb->biquadC[7] = (inputSampleL * reverb->biquadC[3]) - (tempSampleL * reverb->biquadC[5]) + reverb->biquadC[8];
		reverb->biquadC[8] = (inputSampleL * reverb->biquadC[4]) - (tempSampleL * reverb->biquadC[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * reverb->biquadC[2]) + reverb->biquadC[9];
		reverb->biquadC[9] = (inputSampleR * reverb->biquadC[3]) - (tempSampleR * reverb->biquadC[5]) + reverb->biquadC[10];
		reverb->biquadC[10] = (inputSampleR * reverb->biquadC[4]) - (tempSampleR * reverb->biquadC[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (wet != 1.0) {
			inputSampleL += (drySampleL * (1.0 - wet));
			inputSampleR += (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		reverb->fpdL ^= reverb->fpdL << 13;
		reverb->fpdL ^= reverb->fpdL >> 17;
		reverb->fpdL ^= reverb->fpdL << 5;
		inputSampleL += (((double) reverb->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		reverb->fpdR ^= reverb->fpdR << 13;
		reverb->fpdR ^= reverb->fpdR >> 17;
		reverb->fpdR ^= reverb->fpdR << 5;
		inputSampleR += (((double) reverb->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	REVERB_URI,
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
