#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define INFINITY_URI "https://hannesbraun.net/ns/lv2/airwindows/infinity"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FILTER = 4,
	DAMPING = 5,
	SIZE = 6,
	DRY_WET = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* filter;
	const float* damping;
	const float* size;
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

	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double feedbackEL;
	double feedbackFL;
	double feedbackGL;
	double feedbackHL;

	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	double feedbackER;
	double feedbackFR;
	double feedbackGR;
	double feedbackHR;

	uint32_t fpdL;
	uint32_t fpdR;
} Infinity;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Infinity* infinity = (Infinity*) calloc(1, sizeof(Infinity));
	infinity->sampleRate = rate;
	return (LV2_Handle) infinity;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Infinity* infinity = (Infinity*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			infinity->input[0] = (const float*) data;
			break;
		case INPUT_R:
			infinity->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			infinity->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			infinity->output[1] = (float*) data;
			break;
		case FILTER:
			infinity->filter = (const float*) data;
			break;
		case DAMPING:
			infinity->damping = (const float*) data;
			break;
		case SIZE:
			infinity->size = (const float*) data;
			break;
		case DRY_WET:
			infinity->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Infinity* infinity = (Infinity*) instance;

	for (int x = 0; x < 11; x++) {
		infinity->biquadA[x] = 0.0;
		infinity->biquadB[x] = 0.0;
		infinity->biquadC[x] = 0.0;
	}

	infinity->feedbackAL = infinity->feedbackAR = 0.0;
	infinity->feedbackBL = infinity->feedbackBR = 0.0;
	infinity->feedbackCL = infinity->feedbackCR = 0.0;
	infinity->feedbackDL = infinity->feedbackDR = 0.0;
	infinity->feedbackEL = infinity->feedbackER = 0.0;
	infinity->feedbackFL = infinity->feedbackFR = 0.0;
	infinity->feedbackGL = infinity->feedbackGR = 0.0;
	infinity->feedbackHL = infinity->feedbackHR = 0.0;

	int count;
	for (count = 0; count < 8110; count++) {
		infinity->aAL[count] = infinity->aAR[count] = 0.0;
	}
	for (count = 0; count < 7510; count++) {
		infinity->aBL[count] = infinity->aBR[count] = 0.0;
	}
	for (count = 0; count < 7310; count++) {
		infinity->aCL[count] = infinity->aCR[count] = 0.0;
	}
	for (count = 0; count < 6910; count++) {
		infinity->aDL[count] = infinity->aDR[count] = 0.0;
	}
	for (count = 0; count < 6310; count++) {
		infinity->aEL[count] = infinity->aER[count] = 0.0;
	}
	for (count = 0; count < 6110; count++) {
		infinity->aFL[count] = infinity->aFR[count] = 0.0;
	}
	for (count = 0; count < 5510; count++) {
		infinity->aGL[count] = infinity->aGR[count] = 0.0;
	}
	for (count = 0; count < 4910; count++) {
		infinity->aHL[count] = infinity->aHR[count] = 0.0;
	}
	// maximum value needed will be delay * 100, plus 206 (absolute max vibrato depth)
	for (count = 0; count < 4510; count++) {
		infinity->aIL[count] = infinity->aIR[count] = 0.0;
	}
	for (count = 0; count < 4310; count++) {
		infinity->aJL[count] = infinity->aJR[count] = 0.0;
	}
	for (count = 0; count < 3910; count++) {
		infinity->aKL[count] = infinity->aKR[count] = 0.0;
	}
	for (count = 0; count < 3310; count++) {
		infinity->aLL[count] = infinity->aLR[count] = 0.0;
	}
	// maximum value will be delay * 100
	infinity->countA = 1;
	infinity->delayA = 79;
	infinity->countB = 1;
	infinity->delayB = 73;
	infinity->countC = 1;
	infinity->delayC = 71;
	infinity->countD = 1;
	infinity->delayD = 67;
	infinity->countE = 1;
	infinity->delayE = 61;
	infinity->countF = 1;
	infinity->delayF = 59;
	infinity->countG = 1;
	infinity->delayG = 53;
	infinity->countH = 1;
	infinity->delayH = 47;
	// the householder matrices
	infinity->countI = 1;
	infinity->delayI = 43;
	infinity->countJ = 1;
	infinity->delayJ = 41;
	infinity->countK = 1;
	infinity->delayK = 37;
	infinity->countL = 1;
	infinity->delayL = 31;
	// the allpasses

	infinity->fpdL = 1.0;
	while (infinity->fpdL < 16386) infinity->fpdL = rand() * UINT32_MAX;
	infinity->fpdR = 1.0;
	while (infinity->fpdR < 16386) infinity->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Infinity* infinity = (Infinity*) instance;

	const float* in1 = infinity->input[0];
	const float* in2 = infinity->input[1];
	float* out1 = infinity->output[0];
	float* out2 = infinity->output[1];

	const float filter = *infinity->filter;

	infinity->biquadC[0] = infinity->biquadB[0] = infinity->biquadA[0] = ((pow(filter, 2) * 9900.0) + 100.0) / infinity->sampleRate;
	infinity->biquadA[1] = 0.618033988749894848204586;
	infinity->biquadB[1] = (filter * 0.5) + 0.118033988749894848204586;
	infinity->biquadC[1] = 0.5;

	double K = tan(M_PI * infinity->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / infinity->biquadA[1] + K * K);
	infinity->biquadA[2] = K * K * norm;
	infinity->biquadA[3] = 2.0 * infinity->biquadA[2];
	infinity->biquadA[4] = infinity->biquadA[2];
	infinity->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	infinity->biquadA[6] = (1.0 - K / infinity->biquadA[1] + K * K) * norm;

	K = tan(M_PI * infinity->biquadA[0]);
	norm = 1.0 / (1.0 + K / infinity->biquadB[1] + K * K);
	infinity->biquadB[2] = K * K * norm;
	infinity->biquadB[3] = 2.0 * infinity->biquadB[2];
	infinity->biquadB[4] = infinity->biquadB[2];
	infinity->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	infinity->biquadB[6] = (1.0 - K / infinity->biquadB[1] + K * K) * norm;

	K = tan(M_PI * infinity->biquadC[0]);
	norm = 1.0 / (1.0 + K / infinity->biquadC[1] + K * K);
	infinity->biquadC[2] = K * K * norm;
	infinity->biquadC[3] = 2.0 * infinity->biquadC[2];
	infinity->biquadC[4] = infinity->biquadC[2];
	infinity->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	infinity->biquadC[6] = (1.0 - K / infinity->biquadC[1] + K * K) * norm;

	double damping = pow(*infinity->damping, 2) * 0.5;

	double size = (pow(*infinity->size, 2) * 90.0) + 10.0;

	double wet = *infinity->dryWet;

	infinity->delayA = 79 * size;
	infinity->delayB = 73 * size;
	infinity->delayC = 71 * size;
	infinity->delayD = 67 * size;
	infinity->delayE = 61 * size;
	infinity->delayF = 59 * size;
	infinity->delayG = 53 * size;
	infinity->delayH = 47 * size;

	infinity->delayI = 43 * size;
	infinity->delayJ = 41 * size;
	infinity->delayK = 37 * size;
	infinity->delayL = 31 * size;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = infinity->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = infinity->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double tempSampleL = (inputSampleL * infinity->biquadA[2]) + infinity->biquadA[7];
		infinity->biquadA[7] = (inputSampleL * infinity->biquadA[3]) - (tempSampleL * infinity->biquadA[5]) + infinity->biquadA[8];
		infinity->biquadA[8] = (inputSampleL * infinity->biquadA[4]) - (tempSampleL * infinity->biquadA[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		double tempSampleR = (inputSampleR * infinity->biquadA[2]) + infinity->biquadA[9];
		infinity->biquadA[9] = (inputSampleR * infinity->biquadA[3]) - (tempSampleR * infinity->biquadA[5]) + infinity->biquadA[10];
		infinity->biquadA[10] = (inputSampleR * infinity->biquadA[4]) - (tempSampleR * infinity->biquadA[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		inputSampleL *= wet;
		inputSampleR *= wet;
		// we're going to use this as a kind of balance since the reverb buildup can be so large
		inputSampleL *= 0.5;
		inputSampleR *= 0.5;

		double allpassIL = inputSampleL;
		double allpassJL = inputSampleL;
		double allpassKL = inputSampleL;
		double allpassLL = inputSampleL;

		double allpassIR = inputSampleR;
		double allpassJR = inputSampleR;
		double allpassKR = inputSampleR;
		double allpassLR = inputSampleR;

		int allpasstemp = infinity->countI + 1;
		if (allpasstemp < 0 || allpasstemp > infinity->delayI) {
			allpasstemp = 0;
		}
		allpassIL -= infinity->aIL[allpasstemp] * 0.5;
		infinity->aIL[infinity->countI] = allpassIL;
		allpassIL *= 0.5;
		allpassIR -= infinity->aIR[allpasstemp] * 0.5;
		infinity->aIR[infinity->countI] = allpassIR;
		allpassIR *= 0.5;
		infinity->countI++;
		if (infinity->countI < 0 || infinity->countI > infinity->delayI) {
			infinity->countI = 0;
		}
		allpassIL += (infinity->aIL[infinity->countI]);
		allpassIR += (infinity->aIR[infinity->countI]);

		allpasstemp = infinity->countJ + 1;
		if (allpasstemp < 0 || allpasstemp > infinity->delayJ) {
			allpasstemp = 0;
		}
		allpassJL -= infinity->aJL[allpasstemp] * 0.5;
		infinity->aJL[infinity->countJ] = allpassJL;
		allpassJL *= 0.5;
		allpassJR -= infinity->aJR[allpasstemp] * 0.5;
		infinity->aJR[infinity->countJ] = allpassJR;
		allpassJR *= 0.5;
		infinity->countJ++;
		if (infinity->countJ < 0 || infinity->countJ > infinity->delayJ) {
			infinity->countJ = 0;
		}
		allpassJL += (infinity->aJL[infinity->countJ]);
		allpassJR += (infinity->aJR[infinity->countJ]);

		allpasstemp = infinity->countK + 1;
		if (allpasstemp < 0 || allpasstemp > infinity->delayK) {
			allpasstemp = 0;
		}
		allpassKL -= infinity->aKL[allpasstemp] * 0.5;
		infinity->aKL[infinity->countK] = allpassKL;
		allpassKL *= 0.5;
		allpassKR -= infinity->aKR[allpasstemp] * 0.5;
		infinity->aKR[infinity->countK] = allpassKR;
		allpassKR *= 0.5;
		infinity->countK++;
		if (infinity->countK < 0 || infinity->countK > infinity->delayK) {
			infinity->countK = 0;
		}
		allpassKL += (infinity->aKL[infinity->countK]);
		allpassKR += (infinity->aKR[infinity->countK]);

		allpasstemp = infinity->countL + 1;
		if (allpasstemp < 0 || allpasstemp > infinity->delayL) {
			allpasstemp = 0;
		}
		allpassLL -= infinity->aLL[allpasstemp] * 0.5;
		infinity->aLL[infinity->countL] = allpassLL;
		allpassLL *= 0.5;
		allpassLR -= infinity->aLR[allpasstemp] * 0.5;
		infinity->aLR[infinity->countL] = allpassLR;
		allpassLR *= 0.5;
		infinity->countL++;
		if (infinity->countL < 0 || infinity->countL > infinity->delayL) {
			infinity->countL = 0;
		}
		allpassLL += (infinity->aLL[infinity->countL]);
		allpassLR += (infinity->aLR[infinity->countL]);
		// the big allpass in front of everything

		infinity->aAL[infinity->countA] = allpassLL + infinity->feedbackAL;
		infinity->aBL[infinity->countB] = allpassKL + infinity->feedbackBL;
		infinity->aCL[infinity->countC] = allpassJL + infinity->feedbackCL;
		infinity->aDL[infinity->countD] = allpassIL + infinity->feedbackDL;
		infinity->aEL[infinity->countE] = allpassIL + infinity->feedbackEL;
		infinity->aFL[infinity->countF] = allpassJL + infinity->feedbackFL;
		infinity->aGL[infinity->countG] = allpassKL + infinity->feedbackGL;
		infinity->aHL[infinity->countH] = allpassLL + infinity->feedbackHL; // L

		infinity->aAR[infinity->countA] = allpassLR + infinity->feedbackAR;
		infinity->aBR[infinity->countB] = allpassKR + infinity->feedbackBR;
		infinity->aCR[infinity->countC] = allpassJR + infinity->feedbackCR;
		infinity->aDR[infinity->countD] = allpassIR + infinity->feedbackDR;
		infinity->aER[infinity->countE] = allpassIR + infinity->feedbackER;
		infinity->aFR[infinity->countF] = allpassJR + infinity->feedbackFR;
		infinity->aGR[infinity->countG] = allpassKR + infinity->feedbackGR;
		infinity->aHR[infinity->countH] = allpassLR + infinity->feedbackHR; // R

		infinity->countA++;
		if (infinity->countA < 0 || infinity->countA > infinity->delayA) {
			infinity->countA = 0;
		}
		infinity->countB++;
		if (infinity->countB < 0 || infinity->countB > infinity->delayB) {
			infinity->countB = 0;
		}
		infinity->countC++;
		if (infinity->countC < 0 || infinity->countC > infinity->delayC) {
			infinity->countC = 0;
		}
		infinity->countD++;
		if (infinity->countD < 0 || infinity->countD > infinity->delayD) {
			infinity->countD = 0;
		}
		infinity->countE++;
		if (infinity->countE < 0 || infinity->countE > infinity->delayE) {
			infinity->countE = 0;
		}
		infinity->countF++;
		if (infinity->countF < 0 || infinity->countF > infinity->delayF) {
			infinity->countF = 0;
		}
		infinity->countG++;
		if (infinity->countG < 0 || infinity->countG > infinity->delayG) {
			infinity->countG = 0;
		}
		infinity->countH++;
		if (infinity->countH < 0 || infinity->countH > infinity->delayH) {
			infinity->countH = 0;
		}
		// the Householder matrices (shared between channels, offset is stereo)

		double infiniteAL = (infinity->aAL[infinity->countA - ((infinity->countA > infinity->delayA) ? infinity->delayA + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteAL += (infinity->aAL[infinity->countA + 1 - ((infinity->countA + 1 > infinity->delayA) ? infinity->delayA + 1 : 0)] * ((damping - floor(damping))));
		double infiniteBL = infinity->aBL[infinity->countB - ((infinity->countB > infinity->delayB) ? infinity->delayB + 1 : 0)];
		double infiniteCL = infinity->aCL[infinity->countC - ((infinity->countC > infinity->delayC) ? infinity->delayC + 1 : 0)];
		double infiniteDL = infinity->aDL[infinity->countD - ((infinity->countD > infinity->delayD) ? infinity->delayD + 1 : 0)];

		double infiniteAR = (infinity->aAR[infinity->countA - ((infinity->countA > infinity->delayA) ? infinity->delayA + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteAR += (infinity->aAR[infinity->countA + 1 - ((infinity->countA + 1 > infinity->delayA) ? infinity->delayA + 1 : 0)] * ((damping - floor(damping))));
		double infiniteBR = infinity->aBR[infinity->countB - ((infinity->countB > infinity->delayB) ? infinity->delayB + 1 : 0)];
		double infiniteCR = infinity->aCR[infinity->countC - ((infinity->countC > infinity->delayC) ? infinity->delayC + 1 : 0)];
		double infiniteDR = infinity->aDR[infinity->countD - ((infinity->countD > infinity->delayD) ? infinity->delayD + 1 : 0)];

		double infiniteEL = (infinity->aEL[infinity->countE - ((infinity->countE > infinity->delayE) ? infinity->delayE + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteEL += (infinity->aEL[infinity->countE + 1 - ((infinity->countE + 1 > infinity->delayE) ? infinity->delayE + 1 : 0)] * ((damping - floor(damping))));
		double infiniteFL = infinity->aFL[infinity->countF - ((infinity->countF > infinity->delayF) ? infinity->delayF + 1 : 0)];
		double infiniteGL = infinity->aGL[infinity->countG - ((infinity->countG > infinity->delayG) ? infinity->delayG + 1 : 0)];
		double infiniteHL = infinity->aHL[infinity->countH - ((infinity->countH > infinity->delayH) ? infinity->delayH + 1 : 0)];

		double infiniteER = (infinity->aER[infinity->countE - ((infinity->countE > infinity->delayE) ? infinity->delayE + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteER += (infinity->aER[infinity->countE + 1 - ((infinity->countE + 1 > infinity->delayE) ? infinity->delayE + 1 : 0)] * ((damping - floor(damping))));
		double infiniteFR = infinity->aFR[infinity->countF - ((infinity->countF > infinity->delayF) ? infinity->delayF + 1 : 0)];
		double infiniteGR = infinity->aGR[infinity->countG - ((infinity->countG > infinity->delayG) ? infinity->delayG + 1 : 0)];
		double infiniteHR = infinity->aHR[infinity->countH - ((infinity->countH > infinity->delayH) ? infinity->delayH + 1 : 0)];

		double dialBackAL = 0.5;
		double dialBackEL = 0.5;
		double dialBackDryL = 0.5;
		if (fabs(infiniteAL) > 0.4) dialBackAL -= ((fabs(infiniteAL) - 0.4) * 0.2);
		if (fabs(infiniteEL) > 0.4) dialBackEL -= ((fabs(infiniteEL) - 0.4) * 0.2);
		if (fabs(drySampleL) > 0.4) dialBackDryL -= ((fabs(drySampleL) - 0.4) * 0.2);
		// we're compressing things subtly so we can feed energy in and not overload

		double dialBackAR = 0.5;
		double dialBackER = 0.5;
		double dialBackDryR = 0.5;
		if (fabs(infiniteAR) > 0.4) dialBackAR -= ((fabs(infiniteAR) - 0.4) * 0.2);
		if (fabs(infiniteER) > 0.4) dialBackER -= ((fabs(infiniteER) - 0.4) * 0.2);
		if (fabs(drySampleR) > 0.4) dialBackDryR -= ((fabs(drySampleR) - 0.4) * 0.2);
		// we're compressing things subtly so we can feed energy in and not overload

		infinity->feedbackAL = (infiniteAL - (infiniteBL + infiniteCL + infiniteDL)) * dialBackAL;
		infinity->feedbackBL = (infiniteBL - (infiniteAL + infiniteCL + infiniteDL)) * dialBackDryL;
		infinity->feedbackCL = (infiniteCL - (infiniteAL + infiniteBL + infiniteDL)) * dialBackDryL;
		infinity->feedbackDL = (infiniteDL - (infiniteAL + infiniteBL + infiniteCL)) * dialBackDryL; // Householder feedback matrix, L

		infinity->feedbackEL = (infiniteEL - (infiniteFL + infiniteGL + infiniteHL)) * dialBackEL;
		infinity->feedbackFL = (infiniteFL - (infiniteEL + infiniteGL + infiniteHL)) * dialBackDryL;
		infinity->feedbackGL = (infiniteGL - (infiniteEL + infiniteFL + infiniteHL)) * dialBackDryL;
		infinity->feedbackHL = (infiniteHL - (infiniteEL + infiniteFL + infiniteGL)) * dialBackDryL; // Householder feedback matrix, L

		infinity->feedbackAR = (infiniteAR - (infiniteBR + infiniteCR + infiniteDR)) * dialBackAR;
		infinity->feedbackBR = (infiniteBR - (infiniteAR + infiniteCR + infiniteDR)) * dialBackDryR;
		infinity->feedbackCR = (infiniteCR - (infiniteAR + infiniteBR + infiniteDR)) * dialBackDryR;
		infinity->feedbackDR = (infiniteDR - (infiniteAR + infiniteBR + infiniteCR)) * dialBackDryR; // Householder feedback matrix, R

		infinity->feedbackER = (infiniteER - (infiniteFR + infiniteGR + infiniteHR)) * dialBackER;
		infinity->feedbackFR = (infiniteFR - (infiniteER + infiniteGR + infiniteHR)) * dialBackDryR;
		infinity->feedbackGR = (infiniteGR - (infiniteER + infiniteFR + infiniteHR)) * dialBackDryR;
		infinity->feedbackHR = (infiniteHR - (infiniteER + infiniteFR + infiniteGR)) * dialBackDryR; // Householder feedback matrix, R

		inputSampleL = (infiniteAL + infiniteBL + infiniteCL + infiniteDL + infiniteEL + infiniteFL + infiniteGL + infiniteHL) / 8.0;
		inputSampleR = (infiniteAR + infiniteBR + infiniteCR + infiniteDR + infiniteER + infiniteFR + infiniteGR + infiniteHR) / 8.0;

		tempSampleL = (inputSampleL * infinity->biquadB[2]) + infinity->biquadB[7];
		infinity->biquadB[7] = (inputSampleL * infinity->biquadB[3]) - (tempSampleL * infinity->biquadB[5]) + infinity->biquadB[8];
		infinity->biquadB[8] = (inputSampleL * infinity->biquadB[4]) - (tempSampleL * infinity->biquadB[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * infinity->biquadB[2]) + infinity->biquadB[9];
		infinity->biquadB[9] = (inputSampleR * infinity->biquadB[3]) - (tempSampleR * infinity->biquadB[5]) + infinity->biquadB[10];
		infinity->biquadB[10] = (inputSampleR * infinity->biquadB[4]) - (tempSampleR * infinity->biquadB[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);

		tempSampleL = (inputSampleL * infinity->biquadC[2]) + infinity->biquadC[7];
		infinity->biquadC[7] = (inputSampleL * infinity->biquadC[3]) - (tempSampleL * infinity->biquadC[5]) + infinity->biquadC[8];
		infinity->biquadC[8] = (inputSampleL * infinity->biquadC[4]) - (tempSampleL * infinity->biquadC[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * infinity->biquadC[2]) + infinity->biquadC[9];
		infinity->biquadC[9] = (inputSampleR * infinity->biquadC[3]) - (tempSampleR * infinity->biquadC[5]) + infinity->biquadC[10];
		infinity->biquadC[10] = (inputSampleR * infinity->biquadC[4]) - (tempSampleR * infinity->biquadC[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		infinity->fpdL ^= infinity->fpdL << 13;
		infinity->fpdL ^= infinity->fpdL >> 17;
		infinity->fpdL ^= infinity->fpdL << 5;
		inputSampleL += (((double) infinity->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		infinity->fpdR ^= infinity->fpdR << 13;
		infinity->fpdR ^= infinity->fpdR >> 17;
		infinity->fpdR ^= infinity->fpdR << 5;
		inputSampleR += (((double) infinity->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	INFINITY_URI,
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
