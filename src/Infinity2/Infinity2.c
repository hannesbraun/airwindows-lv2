#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define INFINITY2_URI "https://hannesbraun.net/ns/lv2/airwindows/infinity2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	FILTER = 4,
	SIZE = 5,
	DAMPING = 6,
	ALLPASS = 7,
	FEEDBACK = 8,
	DRY_WET = 9
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* filter;
	const float* size;
	const float* damping;
	const float* allpass;
	const float* feedback;
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
} Infinity2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Infinity2* infinity2 = (Infinity2*) calloc(1, sizeof(Infinity2));
	infinity2->sampleRate = rate;
	return (LV2_Handle) infinity2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Infinity2* infinity2 = (Infinity2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			infinity2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			infinity2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			infinity2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			infinity2->output[1] = (float*) data;
			break;
		case FILTER:
			infinity2->filter = (const float*) data;
			break;
		case SIZE:
			infinity2->size = (const float*) data;
			break;
		case DAMPING:
			infinity2->damping = (const float*) data;
			break;
		case ALLPASS:
			infinity2->allpass = (const float*) data;
			break;
		case FEEDBACK:
			infinity2->feedback = (const float*) data;
			break;
		case DRY_WET:
			infinity2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Infinity2* infinity2 = (Infinity2*) instance;

	for (int x = 0; x < 11; x++) {
		infinity2->biquadA[x] = 0.0;
		infinity2->biquadB[x] = 0.0;
		infinity2->biquadC[x] = 0.0;
	}

	infinity2->feedbackAL = infinity2->feedbackAR = 0.0;
	infinity2->feedbackBL = infinity2->feedbackBR = 0.0;
	infinity2->feedbackCL = infinity2->feedbackCR = 0.0;
	infinity2->feedbackDL = infinity2->feedbackDR = 0.0;
	infinity2->feedbackEL = infinity2->feedbackER = 0.0;
	infinity2->feedbackFL = infinity2->feedbackFR = 0.0;
	infinity2->feedbackGL = infinity2->feedbackGR = 0.0;
	infinity2->feedbackHL = infinity2->feedbackHR = 0.0;

	int count;
	for (count = 0; count < 8110; count++) {
		infinity2->aAL[count] = infinity2->aAR[count] = 0.0;
	}
	for (count = 0; count < 7510; count++) {
		infinity2->aBL[count] = infinity2->aBR[count] = 0.0;
	}
	for (count = 0; count < 7310; count++) {
		infinity2->aCL[count] = infinity2->aCR[count] = 0.0;
	}
	for (count = 0; count < 6910; count++) {
		infinity2->aDL[count] = infinity2->aDR[count] = 0.0;
	}
	for (count = 0; count < 6310; count++) {
		infinity2->aEL[count] = infinity2->aER[count] = 0.0;
	}
	for (count = 0; count < 6110; count++) {
		infinity2->aFL[count] = infinity2->aFR[count] = 0.0;
	}
	for (count = 0; count < 5510; count++) {
		infinity2->aGL[count] = infinity2->aGR[count] = 0.0;
	}
	for (count = 0; count < 4910; count++) {
		infinity2->aHL[count] = infinity2->aHR[count] = 0.0;
	}
	// maximum value needed will be delay * 100, plus 206 (absolute max vibrato depth)
	for (count = 0; count < 4510; count++) {
		infinity2->aIL[count] = infinity2->aIR[count] = 0.0;
	}
	for (count = 0; count < 4310; count++) {
		infinity2->aJL[count] = infinity2->aJR[count] = 0.0;
	}
	for (count = 0; count < 3910; count++) {
		infinity2->aKL[count] = infinity2->aKR[count] = 0.0;
	}
	for (count = 0; count < 3310; count++) {
		infinity2->aLL[count] = infinity2->aLR[count] = 0.0;
	}
	// maximum value will be delay * 100
	infinity2->countA = 1;
	infinity2->delayA = 79;
	infinity2->countB = 1;
	infinity2->delayB = 73;
	infinity2->countC = 1;
	infinity2->delayC = 71;
	infinity2->countD = 1;
	infinity2->delayD = 67;
	infinity2->countE = 1;
	infinity2->delayE = 61;
	infinity2->countF = 1;
	infinity2->delayF = 59;
	infinity2->countG = 1;
	infinity2->delayG = 53;
	infinity2->countH = 1;
	infinity2->delayH = 47;
	// the householder matrices
	infinity2->countI = 1;
	infinity2->delayI = 43;
	infinity2->countJ = 1;
	infinity2->delayJ = 41;
	infinity2->countK = 1;
	infinity2->delayK = 37;
	infinity2->countL = 1;
	infinity2->delayL = 31;
	// the allpasses

	infinity2->fpdL = 1.0;
	while (infinity2->fpdL < 16386) infinity2->fpdL = rand() * UINT32_MAX;
	infinity2->fpdR = 1.0;
	while (infinity2->fpdR < 16386) infinity2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Infinity2* infinity2 = (Infinity2*) instance;

	const float* in1 = infinity2->input[0];
	const float* in2 = infinity2->input[1];
	float* out1 = infinity2->output[0];
	float* out2 = infinity2->output[1];

	const float filter = *infinity2->filter;

	infinity2->biquadC[0] = infinity2->biquadB[0] = infinity2->biquadA[0] = ((pow(filter, 2) * 9900.0) + 100.0) / infinity2->sampleRate;
	infinity2->biquadA[1] = 0.618033988749894848204586;
	infinity2->biquadB[1] = (filter * 0.5) + 0.118033988749894848204586;
	infinity2->biquadC[1] = 0.5;

	double K = tan(M_PI * infinity2->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / infinity2->biquadA[1] + K * K);
	infinity2->biquadA[2] = K * K * norm;
	infinity2->biquadA[3] = 2.0 * infinity2->biquadA[2];
	infinity2->biquadA[4] = infinity2->biquadA[2];
	infinity2->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	infinity2->biquadA[6] = (1.0 - K / infinity2->biquadA[1] + K * K) * norm;

	K = tan(M_PI * infinity2->biquadA[0]);
	norm = 1.0 / (1.0 + K / infinity2->biquadB[1] + K * K);
	infinity2->biquadB[2] = K * K * norm;
	infinity2->biquadB[3] = 2.0 * infinity2->biquadB[2];
	infinity2->biquadB[4] = infinity2->biquadB[2];
	infinity2->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	infinity2->biquadB[6] = (1.0 - K / infinity2->biquadB[1] + K * K) * norm;

	K = tan(M_PI * infinity2->biquadC[0]);
	norm = 1.0 / (1.0 + K / infinity2->biquadC[1] + K * K);
	infinity2->biquadC[2] = K * K * norm;
	infinity2->biquadC[3] = 2.0 * infinity2->biquadC[2];
	infinity2->biquadC[4] = infinity2->biquadC[2];
	infinity2->biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	infinity2->biquadC[6] = (1.0 - K / infinity2->biquadC[1] + K * K) * norm;

	double size = (pow(*infinity2->size, 2) * 99.0) + 1.0;
	double damping = pow(*infinity2->damping, 2) * 0.5;
	double rawPass = *infinity2->allpass;
	double feedback = 1.0 - (pow(1.0 - *infinity2->feedback, 4));
	double wet = *infinity2->dryWet;

	infinity2->delayA = 79 * size;
	infinity2->delayB = 73 * size;
	infinity2->delayC = 71 * size;
	infinity2->delayD = 67 * size;
	infinity2->delayE = 61 * size;
	infinity2->delayF = 59 * size;
	infinity2->delayG = 53 * size;
	infinity2->delayH = 47 * size;

	infinity2->delayI = 43 * size;
	infinity2->delayJ = 41 * size;
	infinity2->delayK = 37 * size;
	infinity2->delayL = 31 * size;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = infinity2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = infinity2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double tempSampleL = (inputSampleL * infinity2->biquadA[2]) + infinity2->biquadA[7];
		infinity2->biquadA[7] = (inputSampleL * infinity2->biquadA[3]) - (tempSampleL * infinity2->biquadA[5]) + infinity2->biquadA[8];
		infinity2->biquadA[8] = (inputSampleL * infinity2->biquadA[4]) - (tempSampleL * infinity2->biquadA[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		double tempSampleR = (inputSampleR * infinity2->biquadA[2]) + infinity2->biquadA[9];
		infinity2->biquadA[9] = (inputSampleR * infinity2->biquadA[3]) - (tempSampleR * infinity2->biquadA[5]) + infinity2->biquadA[10];
		infinity2->biquadA[10] = (inputSampleR * infinity2->biquadA[4]) - (tempSampleR * infinity2->biquadA[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		double allpassIL = inputSampleL;
		double allpassJL = inputSampleL;
		double allpassKL = inputSampleL;
		double allpassLL = inputSampleL;

		double allpassIR = inputSampleR;
		double allpassJR = inputSampleR;
		double allpassKR = inputSampleR;
		double allpassLR = inputSampleR;

		int allpasstemp = infinity2->countI + 1;
		if (allpasstemp < 0 || allpasstemp > infinity2->delayI) {
			allpasstemp = 0;
		}
		allpassIL -= infinity2->aIL[allpasstemp] * 0.5;
		infinity2->aIL[infinity2->countI] = allpassIL;
		allpassIL *= 0.5;
		allpassIR -= infinity2->aIR[allpasstemp] * 0.5;
		infinity2->aIR[infinity2->countI] = allpassIR;
		allpassIR *= 0.5;
		infinity2->countI++;
		if (infinity2->countI < 0 || infinity2->countI > infinity2->delayI) {
			infinity2->countI = 0;
		}
		allpassIL += (infinity2->aIL[infinity2->countI]);
		allpassIR += (infinity2->aIR[infinity2->countI]);

		allpasstemp = infinity2->countJ + 1;
		if (allpasstemp < 0 || allpasstemp > infinity2->delayJ) {
			allpasstemp = 0;
		}
		allpassJL -= infinity2->aJL[allpasstemp] * 0.5;
		infinity2->aJL[infinity2->countJ] = allpassJL;
		allpassJL *= 0.5;
		allpassJR -= infinity2->aJR[allpasstemp] * 0.5;
		infinity2->aJR[infinity2->countJ] = allpassJR;
		allpassJR *= 0.5;
		infinity2->countJ++;
		if (infinity2->countJ < 0 || infinity2->countJ > infinity2->delayJ) {
			infinity2->countJ = 0;
		}
		allpassJL += (infinity2->aJL[infinity2->countJ]);
		allpassJR += (infinity2->aJR[infinity2->countJ]);

		allpasstemp = infinity2->countK + 1;
		if (allpasstemp < 0 || allpasstemp > infinity2->delayK) {
			allpasstemp = 0;
		}
		allpassKL -= infinity2->aKL[allpasstemp] * 0.5;
		infinity2->aKL[infinity2->countK] = allpassKL;
		allpassKL *= 0.5;
		allpassKR -= infinity2->aKR[allpasstemp] * 0.5;
		infinity2->aKR[infinity2->countK] = allpassKR;
		allpassKR *= 0.5;
		infinity2->countK++;
		if (infinity2->countK < 0 || infinity2->countK > infinity2->delayK) {
			infinity2->countK = 0;
		}
		allpassKL += (infinity2->aKL[infinity2->countK]);
		allpassKR += (infinity2->aKR[infinity2->countK]);

		allpasstemp = infinity2->countL + 1;
		if (allpasstemp < 0 || allpasstemp > infinity2->delayL) {
			allpasstemp = 0;
		}
		allpassLL -= infinity2->aLL[allpasstemp] * 0.5;
		infinity2->aLL[infinity2->countL] = allpassLL;
		allpassLL *= 0.5;
		allpassLR -= infinity2->aLR[allpasstemp] * 0.5;
		infinity2->aLR[infinity2->countL] = allpassLR;
		allpassLR *= 0.5;
		infinity2->countL++;
		if (infinity2->countL < 0 || infinity2->countL > infinity2->delayL) {
			infinity2->countL = 0;
		}
		allpassLL += (infinity2->aLL[infinity2->countL]);
		allpassLR += (infinity2->aLR[infinity2->countL]);
		// the big allpass in front of everything

		if (rawPass != 1.0) {
			allpassIL = (allpassIL * rawPass) + (drySampleL * (1.0 - rawPass));
			allpassJL = (allpassJL * rawPass) + (drySampleL * (1.0 - rawPass));
			allpassKL = (allpassKL * rawPass) + (drySampleL * (1.0 - rawPass));
			allpassLL = (allpassLL * rawPass) + (drySampleL * (1.0 - rawPass));
			allpassIR = (allpassIR * rawPass) + (drySampleR * (1.0 - rawPass));
			allpassJR = (allpassJR * rawPass) + (drySampleR * (1.0 - rawPass));
			allpassKR = (allpassKR * rawPass) + (drySampleR * (1.0 - rawPass));
			allpassLR = (allpassLR * rawPass) + (drySampleR * (1.0 - rawPass));
		}

		infinity2->aAL[infinity2->countA] = allpassIL + (infinity2->feedbackAL * feedback);
		infinity2->aBL[infinity2->countB] = allpassJL + (infinity2->feedbackBL * feedback);
		infinity2->aCL[infinity2->countC] = allpassKL + (infinity2->feedbackCL * feedback);
		infinity2->aDL[infinity2->countD] = allpassLL + (infinity2->feedbackDL * feedback);
		infinity2->aEL[infinity2->countE] = allpassIL + (infinity2->feedbackEL * feedback);
		infinity2->aFL[infinity2->countF] = allpassJL + (infinity2->feedbackFL * feedback);
		infinity2->aGL[infinity2->countG] = allpassKL + (infinity2->feedbackGL * feedback);
		infinity2->aHL[infinity2->countH] = allpassLL + (infinity2->feedbackHL * feedback); // L

		infinity2->aAR[infinity2->countA] = allpassIR + (infinity2->feedbackAR * feedback);
		infinity2->aBR[infinity2->countB] = allpassJR + (infinity2->feedbackBR * feedback);
		infinity2->aCR[infinity2->countC] = allpassKR + (infinity2->feedbackCR * feedback);
		infinity2->aDR[infinity2->countD] = allpassLR + (infinity2->feedbackDR * feedback);
		infinity2->aER[infinity2->countE] = allpassIR + (infinity2->feedbackER * feedback);
		infinity2->aFR[infinity2->countF] = allpassJR + (infinity2->feedbackFR * feedback);
		infinity2->aGR[infinity2->countG] = allpassKR + (infinity2->feedbackGR * feedback);
		infinity2->aHR[infinity2->countH] = allpassLR + (infinity2->feedbackHR * feedback); // R

		infinity2->countA++;
		if (infinity2->countA < 0 || infinity2->countA > infinity2->delayA) {
			infinity2->countA = 0;
		}
		infinity2->countB++;
		if (infinity2->countB < 0 || infinity2->countB > infinity2->delayB) {
			infinity2->countB = 0;
		}
		infinity2->countC++;
		if (infinity2->countC < 0 || infinity2->countC > infinity2->delayC) {
			infinity2->countC = 0;
		}
		infinity2->countD++;
		if (infinity2->countD < 0 || infinity2->countD > infinity2->delayD) {
			infinity2->countD = 0;
		}
		infinity2->countE++;
		if (infinity2->countE < 0 || infinity2->countE > infinity2->delayE) {
			infinity2->countE = 0;
		}
		infinity2->countF++;
		if (infinity2->countF < 0 || infinity2->countF > infinity2->delayF) {
			infinity2->countF = 0;
		}
		infinity2->countG++;
		if (infinity2->countG < 0 || infinity2->countG > infinity2->delayG) {
			infinity2->countG = 0;
		}
		infinity2->countH++;
		if (infinity2->countH < 0 || infinity2->countH > infinity2->delayH) {
			infinity2->countH = 0;
		}
		// the Householder matrices (shared between channels, offset is stereo)

		double infiniteAL = (infinity2->aAL[infinity2->countA - ((infinity2->countA > infinity2->delayA) ? infinity2->delayA + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteAL += (infinity2->aAL[infinity2->countA + 1 - ((infinity2->countA + 1 > infinity2->delayA) ? infinity2->delayA + 1 : 0)] * ((damping - floor(damping))));
		double infiniteBL = (infinity2->aBL[infinity2->countB - ((infinity2->countB > infinity2->delayB) ? infinity2->delayB + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteBL += (infinity2->aBL[infinity2->countB + 1 - ((infinity2->countB + 1 > infinity2->delayB) ? infinity2->delayB + 1 : 0)] * ((damping - floor(damping))));
		double infiniteCL = (infinity2->aCL[infinity2->countC - ((infinity2->countC > infinity2->delayC) ? infinity2->delayC + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteCL += (infinity2->aCL[infinity2->countC + 1 - ((infinity2->countC + 1 > infinity2->delayC) ? infinity2->delayC + 1 : 0)] * ((damping - floor(damping))));
		double infiniteDL = (infinity2->aDL[infinity2->countD - ((infinity2->countD > infinity2->delayD) ? infinity2->delayD + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteDL += (infinity2->aDL[infinity2->countD + 1 - ((infinity2->countD + 1 > infinity2->delayD) ? infinity2->delayD + 1 : 0)] * ((damping - floor(damping))));

		double infiniteAR = (infinity2->aAR[infinity2->countA - ((infinity2->countA > infinity2->delayA) ? infinity2->delayA + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteAR += (infinity2->aAR[infinity2->countA + 1 - ((infinity2->countA + 1 > infinity2->delayA) ? infinity2->delayA + 1 : 0)] * ((damping - floor(damping))));
		double infiniteBR = (infinity2->aBR[infinity2->countB - ((infinity2->countB > infinity2->delayB) ? infinity2->delayB + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteBR += (infinity2->aBR[infinity2->countB + 1 - ((infinity2->countB + 1 > infinity2->delayB) ? infinity2->delayB + 1 : 0)] * ((damping - floor(damping))));
		double infiniteCR = (infinity2->aCR[infinity2->countC - ((infinity2->countC > infinity2->delayC) ? infinity2->delayC + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteCR += (infinity2->aCR[infinity2->countC + 1 - ((infinity2->countC + 1 > infinity2->delayC) ? infinity2->delayC + 1 : 0)] * ((damping - floor(damping))));
		double infiniteDR = (infinity2->aDR[infinity2->countD - ((infinity2->countD > infinity2->delayD) ? infinity2->delayD + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteDR += (infinity2->aDR[infinity2->countD + 1 - ((infinity2->countD + 1 > infinity2->delayD) ? infinity2->delayD + 1 : 0)] * ((damping - floor(damping))));

		double infiniteEL = (infinity2->aEL[infinity2->countE - ((infinity2->countE > infinity2->delayE) ? infinity2->delayE + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteEL += (infinity2->aEL[infinity2->countE + 1 - ((infinity2->countE + 1 > infinity2->delayE) ? infinity2->delayE + 1 : 0)] * ((damping - floor(damping))));
		double infiniteFL = (infinity2->aFL[infinity2->countF - ((infinity2->countF > infinity2->delayF) ? infinity2->delayF + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteFL += (infinity2->aFL[infinity2->countF + 1 - ((infinity2->countF + 1 > infinity2->delayF) ? infinity2->delayF + 1 : 0)] * ((damping - floor(damping))));
		double infiniteGL = (infinity2->aGL[infinity2->countG - ((infinity2->countG > infinity2->delayG) ? infinity2->delayG + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteGL += (infinity2->aGL[infinity2->countG + 1 - ((infinity2->countG + 1 > infinity2->delayG) ? infinity2->delayG + 1 : 0)] * ((damping - floor(damping))));
		double infiniteHL = (infinity2->aHL[infinity2->countH - ((infinity2->countH > infinity2->delayH) ? infinity2->delayH + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteHL += (infinity2->aHL[infinity2->countH + 1 - ((infinity2->countH + 1 > infinity2->delayH) ? infinity2->delayH + 1 : 0)] * ((damping - floor(damping))));

		double infiniteER = (infinity2->aER[infinity2->countE - ((infinity2->countE > infinity2->delayE) ? infinity2->delayE + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteER += (infinity2->aER[infinity2->countE + 1 - ((infinity2->countE + 1 > infinity2->delayE) ? infinity2->delayE + 1 : 0)] * ((damping - floor(damping))));
		double infiniteFR = (infinity2->aFR[infinity2->countF - ((infinity2->countF > infinity2->delayF) ? infinity2->delayF + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteFR += (infinity2->aFR[infinity2->countF + 1 - ((infinity2->countF + 1 > infinity2->delayF) ? infinity2->delayF + 1 : 0)] * ((damping - floor(damping))));
		double infiniteGR = (infinity2->aGR[infinity2->countG - ((infinity2->countG > infinity2->delayG) ? infinity2->delayG + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteGR += (infinity2->aGR[infinity2->countG + 1 - ((infinity2->countG + 1 > infinity2->delayG) ? infinity2->delayG + 1 : 0)] * ((damping - floor(damping))));
		double infiniteHR = (infinity2->aHR[infinity2->countH - ((infinity2->countH > infinity2->delayH) ? infinity2->delayH + 1 : 0)] * (1 - (damping - floor(damping))));
		infiniteHR += (infinity2->aHR[infinity2->countH + 1 - ((infinity2->countH + 1 > infinity2->delayH) ? infinity2->delayH + 1 : 0)] * ((damping - floor(damping))));

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

		infinity2->feedbackAL = (infiniteAL - (infiniteBL + infiniteCL + infiniteDL)) * dialBackAL;
		infinity2->feedbackBL = (infiniteBL - (infiniteAL + infiniteCL + infiniteDL)) * dialBackDryL;
		infinity2->feedbackCL = (infiniteCL - (infiniteAL + infiniteBL + infiniteDL)) * dialBackDryL;
		infinity2->feedbackDL = (infiniteDL - (infiniteAL + infiniteBL + infiniteCL)) * dialBackDryL; // Householder feedback matrix, L

		infinity2->feedbackEL = (infiniteEL - (infiniteFL + infiniteGL + infiniteHL)) * dialBackEL;
		infinity2->feedbackFL = (infiniteFL - (infiniteEL + infiniteGL + infiniteHL)) * dialBackDryL;
		infinity2->feedbackGL = (infiniteGL - (infiniteEL + infiniteFL + infiniteHL)) * dialBackDryL;
		infinity2->feedbackHL = (infiniteHL - (infiniteEL + infiniteFL + infiniteGL)) * dialBackDryL; // Householder feedback matrix, L

		infinity2->feedbackAR = (infiniteAR - (infiniteBR + infiniteCR + infiniteDR)) * dialBackAR;
		infinity2->feedbackBR = (infiniteBR - (infiniteAR + infiniteCR + infiniteDR)) * dialBackDryR;
		infinity2->feedbackCR = (infiniteCR - (infiniteAR + infiniteBR + infiniteDR)) * dialBackDryR;
		infinity2->feedbackDR = (infiniteDR - (infiniteAR + infiniteBR + infiniteCR)) * dialBackDryR; // Householder feedback matrix, R

		infinity2->feedbackER = (infiniteER - (infiniteFR + infiniteGR + infiniteHR)) * dialBackER;
		infinity2->feedbackFR = (infiniteFR - (infiniteER + infiniteGR + infiniteHR)) * dialBackDryR;
		infinity2->feedbackGR = (infiniteGR - (infiniteER + infiniteFR + infiniteHR)) * dialBackDryR;
		infinity2->feedbackHR = (infiniteHR - (infiniteER + infiniteFR + infiniteGR)) * dialBackDryR; // Householder feedback matrix, R

		inputSampleL = (infiniteAL + infiniteBL + infiniteCL + infiniteDL + infiniteEL + infiniteFL + infiniteGL + infiniteHL) / 8.0;
		inputSampleR = (infiniteAR + infiniteBR + infiniteCR + infiniteDR + infiniteER + infiniteFR + infiniteGR + infiniteHR) / 8.0;

		tempSampleL = (inputSampleL * infinity2->biquadB[2]) + infinity2->biquadB[7];
		infinity2->biquadB[7] = (inputSampleL * infinity2->biquadB[3]) - (tempSampleL * infinity2->biquadB[5]) + infinity2->biquadB[8];
		infinity2->biquadB[8] = (inputSampleL * infinity2->biquadB[4]) - (tempSampleL * infinity2->biquadB[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * infinity2->biquadB[2]) + infinity2->biquadB[9];
		infinity2->biquadB[9] = (inputSampleR * infinity2->biquadB[3]) - (tempSampleR * infinity2->biquadB[5]) + infinity2->biquadB[10];
		infinity2->biquadB[10] = (inputSampleR * infinity2->biquadB[4]) - (tempSampleR * infinity2->biquadB[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);

		tempSampleL = (inputSampleL * infinity2->biquadC[2]) + infinity2->biquadC[7];
		infinity2->biquadC[7] = (inputSampleL * infinity2->biquadC[3]) - (tempSampleL * infinity2->biquadC[5]) + infinity2->biquadC[8];
		infinity2->biquadC[8] = (inputSampleL * infinity2->biquadC[4]) - (tempSampleL * infinity2->biquadC[6]);
		inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

		tempSampleR = (inputSampleR * infinity2->biquadC[2]) + infinity2->biquadC[9];
		infinity2->biquadC[9] = (inputSampleR * infinity2->biquadC[3]) - (tempSampleR * infinity2->biquadC[5]) + infinity2->biquadC[10];
		infinity2->biquadC[10] = (inputSampleR * infinity2->biquadC[4]) - (tempSampleR * infinity2->biquadC[6]);
		inputSampleR = tempSampleR; // note: 9 and 10 store the R channel

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		infinity2->fpdL ^= infinity2->fpdL << 13;
		infinity2->fpdL ^= infinity2->fpdL >> 17;
		infinity2->fpdL ^= infinity2->fpdL << 5;
		inputSampleL += (((double) infinity2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		infinity2->fpdR ^= infinity2->fpdR << 13;
		infinity2->fpdR ^= infinity2->fpdR >> 17;
		infinity2->fpdR ^= infinity2->fpdR << 5;
		inputSampleR += (((double) infinity2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	INFINITY2_URI,
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
