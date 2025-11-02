#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI_2 1.57079632679489661923132169163975144

#define KCOSMOS_URI "https://hannesbraun.net/ns/lv2/airwindows/kcosmos"

#define predelay 15000

#define delayA 857
#define delayB 1433
#define delayC 1597
#define delayD 1789
#define delayE 1987
#define delayF 373
#define delayG 883
#define delayH 1471
#define delayI 1601
#define delayJ 1973
#define delayK 191
#define delayL 397
#define delayM 941
#define delayN 1483
#define delayO 1663
#define delayP 149
#define delayQ 227
#define delayR 593
#define delayS 1061
#define delayT 1549
#define delayU 137
#define delayV 167
#define delayW 313
#define delayX 641
#define delayY 1153
// 38 to 188 ms, 1538 seat hall
// 1538-OGBP 2025-05-05 - kCosmos

enum {
	bez_AL,
	bez_AR,
	bez_BL,
	bez_BR,
	bez_CL,
	bez_CR,
	bez_InL,
	bez_InR,
	bez_UnInL,
	bez_UnInR,
	bez_SampL,
	bez_SampR,
	bez_cycle,
	bez_total
}; // the new undersampling. bez signifies the bezier curve reconstruction

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	REGEN = 4,
	DEREZ = 5,
	FILTER = 6,
	EARLY_RF = 7,
	PREDLAY = 8,
	DRY_WET = 9
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* regen;
	const float* derez;
	const float* filter;
	const float* earlyRF;
	const float* predlay;
	const float* dryWet;

	double eAL[delayA + 5];
	double eBL[delayB + 5];
	double eCL[delayC + 5];
	double eDL[delayD + 5];
	double eEL[delayE + 5];
	double eFL[delayF + 5];
	double eGL[delayG + 5];
	double eHL[delayH + 5];
	double eIL[delayI + 5];
	double eJL[delayJ + 5];
	double eKL[delayK + 5];
	double eLL[delayL + 5];
	double eML[delayM + 5];
	double eNL[delayN + 5];
	double eOL[delayO + 5];
	double ePL[delayP + 5];
	double eQL[delayQ + 5];
	double eRL[delayR + 5];
	double eSL[delayS + 5];
	double eTL[delayT + 5];
	double eUL[delayU + 5];
	double eVL[delayV + 5];
	double eWL[delayW + 5];
	double eXL[delayX + 5];
	double eYL[delayY + 5];

	double eAR[delayA + 5];
	double eBR[delayB + 5];
	double eCR[delayC + 5];
	double eDR[delayD + 5];
	double eER[delayE + 5];
	double eFR[delayF + 5];
	double eGR[delayG + 5];
	double eHR[delayH + 5];
	double eIR[delayI + 5];
	double eJR[delayJ + 5];
	double eKR[delayK + 5];
	double eLR[delayL + 5];
	double eMR[delayM + 5];
	double eNR[delayN + 5];
	double eOR[delayO + 5];
	double ePR[delayP + 5];
	double eQR[delayQ + 5];
	double eRR[delayR + 5];
	double eSR[delayS + 5];
	double eTR[delayT + 5];
	double eUR[delayU + 5];
	double eVR[delayV + 5];
	double eWR[delayW + 5];
	double eXR[delayX + 5];
	double eYR[delayY + 5];

	double aAL[delayA + 5];
	double aBL[delayB + 5];
	double aCL[delayC + 5];
	double aDL[delayD + 5];
	double aEL[delayE + 5];
	double aFL[delayF + 5];
	double aGL[delayG + 5];
	double aHL[delayH + 5];
	double aIL[delayI + 5];
	double aJL[delayJ + 5];
	double aKL[delayK + 5];
	double aLL[delayL + 5];
	double aML[delayM + 5];
	double aNL[delayN + 5];
	double aOL[delayO + 5];
	double aPL[delayP + 5];
	double aQL[delayQ + 5];
	double aRL[delayR + 5];
	double aSL[delayS + 5];
	double aTL[delayT + 5];
	double aUL[delayU + 5];
	double aVL[delayV + 5];
	double aWL[delayW + 5];
	double aXL[delayX + 5];
	double aYL[delayY + 5];

	double aAR[delayA + 5];
	double aBR[delayB + 5];
	double aCR[delayC + 5];
	double aDR[delayD + 5];
	double aER[delayE + 5];
	double aFR[delayF + 5];
	double aGR[delayG + 5];
	double aHR[delayH + 5];
	double aIR[delayI + 5];
	double aJR[delayJ + 5];
	double aKR[delayK + 5];
	double aLR[delayL + 5];
	double aMR[delayM + 5];
	double aNR[delayN + 5];
	double aOR[delayO + 5];
	double aPR[delayP + 5];
	double aQR[delayQ + 5];
	double aRR[delayR + 5];
	double aSR[delayS + 5];
	double aTR[delayT + 5];
	double aUR[delayU + 5];
	double aVR[delayV + 5];
	double aWR[delayW + 5];
	double aXR[delayX + 5];
	double aYR[delayY + 5];

	double aZL[predelay + 5];
	double aZR[predelay + 5];

	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double feedbackEL;

	double feedbackER;
	double feedbackJR;
	double feedbackOR;
	double feedbackTR;
	double feedbackYR;

	int countAL;
	int countBL;
	int countCL;
	int countDL;
	int countEL;
	int countFL;
	int countGL;
	int countHL;
	int countIL;
	int countJL;
	int countKL;
	int countLL;
	int countML;
	int countNL;
	int countOL;
	int countPL;
	int countQL;
	int countRL;
	int countSL;
	int countTL;
	int countUL;
	int countVL;
	int countWL;
	int countXL;
	int countYL;

	int countAR;
	int countBR;
	int countCR;
	int countDR;
	int countER;
	int countFR;
	int countGR;
	int countHR;
	int countIR;
	int countJR;
	int countKR;
	int countLR;
	int countMR;
	int countNR;
	int countOR;
	int countPR;
	int countQR;
	int countRR;
	int countSR;
	int countTR;
	int countUR;
	int countVR;
	int countWR;
	int countXR;
	int countYR;

	int countZ;

	double bez[bez_total];

	double firBufferL[32768];
	double firBufferR[32768];
	int firPosition;

	double earlyReflectionL;
	double earlyReflectionR;

	double prevAL;
	double prevBL;
	double prevCL;
	double prevDL;
	double prevEL;
	double prevER;
	double prevJR;
	double prevOR;
	double prevTR;
	double prevYR;

	double derezA, derezB;

	uint32_t fpdL;
	uint32_t fpdR;
} kCosmos;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	kCosmos* kcosmos = (kCosmos*) calloc(1, sizeof(kCosmos));
	kcosmos->sampleRate = rate;
	return (LV2_Handle) kcosmos;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	kCosmos* kcosmos = (kCosmos*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			kcosmos->input[0] = (const float*) data;
			break;
		case INPUT_R:
			kcosmos->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			kcosmos->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			kcosmos->output[1] = (float*) data;
			break;
		case REGEN:
			kcosmos->regen = (const float*) data;
			break;
		case DEREZ:
			kcosmos->derez = (const float*) data;
			break;
		case FILTER:
			kcosmos->filter = (const float*) data;
			break;
		case EARLY_RF:
			kcosmos->earlyRF = (const float*) data;
			break;
		case PREDLAY:
			kcosmos->predlay = (const float*) data;
			break;
		case DRY_WET:
			kcosmos->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	kCosmos* kcosmos = (kCosmos*) instance;

	for (int count = 0; count < delayA + 2; count++) {
		kcosmos->eAL[count] = 0.0;
		kcosmos->eAR[count] = 0.0;
		kcosmos->aAL[count] = 0.0;
		kcosmos->aAR[count] = 0.0;
	}
	for (int count = 0; count < delayB + 2; count++) {
		kcosmos->eBL[count] = 0.0;
		kcosmos->eBR[count] = 0.0;
		kcosmos->aBL[count] = 0.0;
		kcosmos->aBR[count] = 0.0;
	}
	for (int count = 0; count < delayC + 2; count++) {
		kcosmos->eCL[count] = 0.0;
		kcosmos->eCR[count] = 0.0;
		kcosmos->aCL[count] = 0.0;
		kcosmos->aCR[count] = 0.0;
	}
	for (int count = 0; count < delayD + 2; count++) {
		kcosmos->eDL[count] = 0.0;
		kcosmos->eDR[count] = 0.0;
		kcosmos->aDL[count] = 0.0;
		kcosmos->aDR[count] = 0.0;
	}
	for (int count = 0; count < delayE + 2; count++) {
		kcosmos->eEL[count] = 0.0;
		kcosmos->eER[count] = 0.0;
		kcosmos->aEL[count] = 0.0;
		kcosmos->aER[count] = 0.0;
	}
	for (int count = 0; count < delayF + 2; count++) {
		kcosmos->eFL[count] = 0.0;
		kcosmos->eFR[count] = 0.0;
		kcosmos->aFL[count] = 0.0;
		kcosmos->aFR[count] = 0.0;
	}
	for (int count = 0; count < delayG + 2; count++) {
		kcosmos->eGL[count] = 0.0;
		kcosmos->eGR[count] = 0.0;
		kcosmos->aGL[count] = 0.0;
		kcosmos->aGR[count] = 0.0;
	}
	for (int count = 0; count < delayH + 2; count++) {
		kcosmos->eHL[count] = 0.0;
		kcosmos->eHR[count] = 0.0;
		kcosmos->aHL[count] = 0.0;
		kcosmos->aHR[count] = 0.0;
	}
	for (int count = 0; count < delayI + 2; count++) {
		kcosmos->eIL[count] = 0.0;
		kcosmos->eIR[count] = 0.0;
		kcosmos->aIL[count] = 0.0;
		kcosmos->aIR[count] = 0.0;
	}
	for (int count = 0; count < delayJ + 2; count++) {
		kcosmos->eJL[count] = 0.0;
		kcosmos->eJR[count] = 0.0;
		kcosmos->aJL[count] = 0.0;
		kcosmos->aJR[count] = 0.0;
	}
	for (int count = 0; count < delayK + 2; count++) {
		kcosmos->eKL[count] = 0.0;
		kcosmos->eKR[count] = 0.0;
		kcosmos->aKL[count] = 0.0;
		kcosmos->aKR[count] = 0.0;
	}
	for (int count = 0; count < delayL + 2; count++) {
		kcosmos->eLL[count] = 0.0;
		kcosmos->eLR[count] = 0.0;
		kcosmos->aLL[count] = 0.0;
		kcosmos->aLR[count] = 0.0;
	}
	for (int count = 0; count < delayM + 2; count++) {
		kcosmos->eML[count] = 0.0;
		kcosmos->eMR[count] = 0.0;
		kcosmos->aML[count] = 0.0;
		kcosmos->aMR[count] = 0.0;
	}
	for (int count = 0; count < delayN + 2; count++) {
		kcosmos->eNL[count] = 0.0;
		kcosmos->eNR[count] = 0.0;
		kcosmos->aNL[count] = 0.0;
		kcosmos->aNR[count] = 0.0;
	}
	for (int count = 0; count < delayO + 2; count++) {
		kcosmos->eOL[count] = 0.0;
		kcosmos->eOR[count] = 0.0;
		kcosmos->aOL[count] = 0.0;
		kcosmos->aOR[count] = 0.0;
	}
	for (int count = 0; count < delayP + 2; count++) {
		kcosmos->ePL[count] = 0.0;
		kcosmos->ePR[count] = 0.0;
		kcosmos->aPL[count] = 0.0;
		kcosmos->aPR[count] = 0.0;
	}
	for (int count = 0; count < delayQ + 2; count++) {
		kcosmos->eQL[count] = 0.0;
		kcosmos->eQR[count] = 0.0;
		kcosmos->aQL[count] = 0.0;
		kcosmos->aQR[count] = 0.0;
	}
	for (int count = 0; count < delayR + 2; count++) {
		kcosmos->eRL[count] = 0.0;
		kcosmos->eRR[count] = 0.0;
		kcosmos->aRL[count] = 0.0;
		kcosmos->aRR[count] = 0.0;
	}
	for (int count = 0; count < delayS + 2; count++) {
		kcosmos->eSL[count] = 0.0;
		kcosmos->eSR[count] = 0.0;
		kcosmos->aSL[count] = 0.0;
		kcosmos->aSR[count] = 0.0;
	}
	for (int count = 0; count < delayT + 2; count++) {
		kcosmos->eTL[count] = 0.0;
		kcosmos->eTR[count] = 0.0;
		kcosmos->aTL[count] = 0.0;
		kcosmos->aTR[count] = 0.0;
	}
	for (int count = 0; count < delayU + 2; count++) {
		kcosmos->eUL[count] = 0.0;
		kcosmos->eUR[count] = 0.0;
		kcosmos->aUL[count] = 0.0;
		kcosmos->aUR[count] = 0.0;
	}
	for (int count = 0; count < delayV + 2; count++) {
		kcosmos->eVL[count] = 0.0;
		kcosmos->eVR[count] = 0.0;
		kcosmos->aVL[count] = 0.0;
		kcosmos->aVR[count] = 0.0;
	}
	for (int count = 0; count < delayW + 2; count++) {
		kcosmos->eWL[count] = 0.0;
		kcosmos->eWR[count] = 0.0;
		kcosmos->aWL[count] = 0.0;
		kcosmos->aWR[count] = 0.0;
	}
	for (int count = 0; count < delayX + 2; count++) {
		kcosmos->eXL[count] = 0.0;
		kcosmos->eXR[count] = 0.0;
		kcosmos->aXL[count] = 0.0;
		kcosmos->aXR[count] = 0.0;
	}
	for (int count = 0; count < delayY + 2; count++) {
		kcosmos->eYL[count] = 0.0;
		kcosmos->eYR[count] = 0.0;
		kcosmos->aYL[count] = 0.0;
		kcosmos->aYR[count] = 0.0;
	}
	for (int count = 0; count < predelay + 2; count++) {
		kcosmos->aZL[count] = 0.0;
		kcosmos->aZR[count] = 0.0;
	}

	kcosmos->feedbackAL = 0.0;
	kcosmos->feedbackBL = 0.0;
	kcosmos->feedbackCL = 0.0;
	kcosmos->feedbackDL = 0.0;
	kcosmos->feedbackEL = 0.0;
	kcosmos->feedbackER = 0.0;
	kcosmos->feedbackJR = 0.0;
	kcosmos->feedbackOR = 0.0;
	kcosmos->feedbackTR = 0.0;
	kcosmos->feedbackYR = 0.0;

	kcosmos->countAL = 1;
	kcosmos->countBL = 1;
	kcosmos->countCL = 1;
	kcosmos->countDL = 1;
	kcosmos->countEL = 1;
	kcosmos->countFL = 1;
	kcosmos->countGL = 1;
	kcosmos->countHL = 1;
	kcosmos->countIL = 1;
	kcosmos->countJL = 1;
	kcosmos->countKL = 1;
	kcosmos->countLL = 1;
	kcosmos->countML = 1;
	kcosmos->countNL = 1;
	kcosmos->countOL = 1;
	kcosmos->countPL = 1;
	kcosmos->countQL = 1;
	kcosmos->countRL = 1;
	kcosmos->countSL = 1;
	kcosmos->countTL = 1;
	kcosmos->countUL = 1;
	kcosmos->countVL = 1;
	kcosmos->countWL = 1;
	kcosmos->countXL = 1;
	kcosmos->countYL = 1;

	kcosmos->countAR = 1;
	kcosmos->countBR = 1;
	kcosmos->countCR = 1;
	kcosmos->countDR = 1;
	kcosmos->countER = 1;
	kcosmos->countFR = 1;
	kcosmos->countGR = 1;
	kcosmos->countHR = 1;
	kcosmos->countIR = 1;
	kcosmos->countJR = 1;
	kcosmos->countKR = 1;
	kcosmos->countLR = 1;
	kcosmos->countMR = 1;
	kcosmos->countNR = 1;
	kcosmos->countOR = 1;
	kcosmos->countPR = 1;
	kcosmos->countQR = 1;
	kcosmos->countRR = 1;
	kcosmos->countSR = 1;
	kcosmos->countTR = 1;
	kcosmos->countUR = 1;
	kcosmos->countVR = 1;
	kcosmos->countWR = 1;
	kcosmos->countXR = 1;
	kcosmos->countYR = 1;

	kcosmos->countZ = 1;

	for (int x = 0; x < bez_total; x++) kcosmos->bez[x] = 0.0;
	kcosmos->bez[bez_cycle] = 1.0;

	for (int count = 0; count < 32767; count++) {
		kcosmos->firBufferL[count] = 0.0;
		kcosmos->firBufferR[count] = 0.0;
	}
	kcosmos->firPosition = 0;

	kcosmos->earlyReflectionL = 0.0;
	kcosmos->earlyReflectionR = 0.0;
	kcosmos->prevAL = 0.0;
	kcosmos->prevBL = 0.0;
	kcosmos->prevCL = 0.0;
	kcosmos->prevDL = 0.0;
	kcosmos->prevEL = 0.0;
	kcosmos->prevER = 0.0;
	kcosmos->prevJR = 0.0;
	kcosmos->prevOR = 0.0;
	kcosmos->prevTR = 0.0;
	kcosmos->prevYR = 0.0;

	kcosmos->derezA = kcosmos->derezB = 0.0;

	kcosmos->fpdL = 1.0;
	while (kcosmos->fpdL < 16386) kcosmos->fpdL = rand() * UINT32_MAX;
	kcosmos->fpdR = 1.0;
	while (kcosmos->fpdR < 16386) kcosmos->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	kCosmos* kcosmos = (kCosmos*) instance;

	const float* in1 = kcosmos->input[0];
	const float* in2 = kcosmos->input[1];
	float* out1 = kcosmos->output[0];
	float* out2 = kcosmos->output[1];

	uint32_t inFramesToProcess = sampleFrames;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= kcosmos->sampleRate;

	double regenMax = (1.0 - pow(1.0 - *kcosmos->regen, 3.0)) * 0.000321;
	// start this but pad it in the loop by volume of output?
	double feedbackSum = 0.0;
	bool applyCream = (*kcosmos->derez > 0.9999);
	double remainder = (overallscale - 1.0) * 0.0375;
	kcosmos->derezA = kcosmos->derezB;
	kcosmos->derezB = *kcosmos->derez / overallscale;
	if (applyCream) kcosmos->derezB = 1.0 / ((int) (1.0 / kcosmos->derezB));
	else kcosmos->derezB /= (2.0 / pow(overallscale, 0.5 - remainder));
	// this hard-locks derez to exact subdivisions of 1.0
	if (kcosmos->derezB < 0.0005) kcosmos->derezB = 0.0005;
	if (kcosmos->derezB > 1.0) kcosmos->derezB = 1.0;
	double freq = *kcosmos->filter * M_PI_2;
	if (freq < 0.5) freq = 0.5;
	bool applyAvg = (*kcosmos->filter < 1.0);
	double earlyLoudness = *kcosmos->earlyRF;
	int adjPredelay = predelay * *kcosmos->predlay * kcosmos->derezB;
	double wet = *kcosmos->dryWet;

	double fir[74];
	fir[36] = 1.0;
	for (int fip = 0; fip < 36; fip++) {
		fir[fip] = (fip - 36) * freq;
		fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
	}
	for (int fip = 37; fip < 72; fip++) {
		fir[fip] = (fip - 36) * freq;
		fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
	} // setting up the filter which will run inside DeRez

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = kcosmos->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = kcosmos->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double temp = (double) sampleFrames / inFramesToProcess;
		double derez = (kcosmos->derezA * temp) + (kcosmos->derezB * (1.0 - temp));

		kcosmos->bez[bez_cycle] += derez;
		kcosmos->bez[bez_SampL] += ((inputSampleL + kcosmos->bez[bez_InL]) * derez);
		kcosmos->bez[bez_SampR] += ((inputSampleR + kcosmos->bez[bez_InR]) * derez);
		kcosmos->bez[bez_InL] = inputSampleL;
		kcosmos->bez[bez_InR] = inputSampleR;
		if (kcosmos->bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
			if (applyCream) kcosmos->bez[bez_cycle] = 0.0;
			else kcosmos->bez[bez_cycle] -= 1.0;

			// predelay
			kcosmos->aZL[kcosmos->countZ] = kcosmos->bez[bez_SampL];
			kcosmos->aZR[kcosmos->countZ] = kcosmos->bez[bez_SampR];
			kcosmos->countZ++;
			if (kcosmos->countZ < 0 || kcosmos->countZ > adjPredelay) kcosmos->countZ = 0;
			kcosmos->bez[bez_SampL] = kcosmos->aZL[kcosmos->countZ - ((kcosmos->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			kcosmos->bez[bez_SampR] = kcosmos->aZR[kcosmos->countZ - ((kcosmos->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			// end predelay

			kcosmos->eAL[kcosmos->countAL] = kcosmos->bez[bez_SampL];
			kcosmos->eBL[kcosmos->countBL] = kcosmos->bez[bez_SampL];
			kcosmos->eCL[kcosmos->countCL] = kcosmos->bez[bez_SampL];
			kcosmos->eDL[kcosmos->countDL] = kcosmos->bez[bez_SampL];
			kcosmos->eEL[kcosmos->countEL] = kcosmos->bez[bez_SampL];

			kcosmos->eER[kcosmos->countER] = kcosmos->bez[bez_SampR];
			kcosmos->eJR[kcosmos->countJR] = kcosmos->bez[bez_SampR];
			kcosmos->eOR[kcosmos->countOR] = kcosmos->bez[bez_SampR];
			kcosmos->eTR[kcosmos->countTR] = kcosmos->bez[bez_SampR];
			kcosmos->eYR[kcosmos->countYR] = kcosmos->bez[bez_SampR];

			if (kcosmos->firPosition < 0 || kcosmos->firPosition > 32767) kcosmos->firPosition = 32767;
			int firp = kcosmos->firPosition;
			kcosmos->firBufferL[firp] = kcosmos->earlyReflectionL;
			kcosmos->earlyReflectionL = 0.0;
			kcosmos->firBufferR[firp] = kcosmos->earlyReflectionR;
			kcosmos->earlyReflectionR = 0.0;
			if (firp + 72 < 32767) {
				for (int fip = 1; fip < 72; fip++) {
					kcosmos->earlyReflectionL += kcosmos->firBufferL[firp + fip] * fir[fip];
					kcosmos->earlyReflectionR += kcosmos->firBufferR[firp + fip] * fir[fip];
				}
			} else {
				for (int fip = 1; fip < 72; fip++) {
					kcosmos->earlyReflectionL += kcosmos->firBufferL[firp + fip - ((firp + fip > 32767) ? 32768 : 0)] * fir[fip];
					kcosmos->earlyReflectionR += kcosmos->firBufferR[firp + fip - ((firp + fip > 32767) ? 32768 : 0)] * fir[fip];
				}
			}
			kcosmos->earlyReflectionL *= 0.25;
			kcosmos->earlyReflectionR *= 0.25;
			kcosmos->earlyReflectionL *= sqrt(freq);
			kcosmos->earlyReflectionR *= sqrt(freq);
			kcosmos->firPosition--; // here's the brickwall FIR filter, running in front of the Householder matrix

			feedbackSum *= 0.00001;
			feedbackSum += fabs(kcosmos->earlyReflectionL);
			feedbackSum += fabs(kcosmos->earlyReflectionR);
			feedbackSum *= 0.00003;

			double regen = fmax(regenMax - feedbackSum, 0.0);
			if (applyAvg) regen *= 0.5;
			else {
				kcosmos->prevAL = kcosmos->prevBL = kcosmos->prevCL = kcosmos->prevDL = kcosmos->prevEL = 0.0;
				kcosmos->prevER = kcosmos->prevJR = kcosmos->prevOR = kcosmos->prevTR = kcosmos->prevYR = 0.0;
			}
			kcosmos->aAL[kcosmos->countAL] = kcosmos->earlyReflectionL + ((kcosmos->feedbackAL + kcosmos->prevAL) * regen);
			kcosmos->aBL[kcosmos->countBL] = kcosmos->earlyReflectionL + ((kcosmos->feedbackBL + kcosmos->prevBL) * regen);
			kcosmos->aCL[kcosmos->countCL] = kcosmos->earlyReflectionL + ((kcosmos->feedbackCL + kcosmos->prevCL) * regen);
			kcosmos->aDL[kcosmos->countDL] = kcosmos->earlyReflectionL + ((kcosmos->feedbackDL + kcosmos->prevDL) * regen);
			kcosmos->aEL[kcosmos->countEL] = kcosmos->earlyReflectionL + ((kcosmos->feedbackEL + kcosmos->prevEL) * regen);

			kcosmos->aER[kcosmos->countER] = kcosmos->earlyReflectionR + ((kcosmos->feedbackER + kcosmos->prevER) * regen);
			kcosmos->aJR[kcosmos->countJR] = kcosmos->earlyReflectionR + ((kcosmos->feedbackJR + kcosmos->prevJR) * regen);
			kcosmos->aOR[kcosmos->countOR] = kcosmos->earlyReflectionR + ((kcosmos->feedbackOR + kcosmos->prevOR) * regen);
			kcosmos->aTR[kcosmos->countTR] = kcosmos->earlyReflectionR + ((kcosmos->feedbackTR + kcosmos->prevTR) * regen);
			kcosmos->aYR[kcosmos->countYR] = kcosmos->earlyReflectionR + ((kcosmos->feedbackYR + kcosmos->prevYR) * regen);

			kcosmos->prevAL = kcosmos->feedbackAL;
			kcosmos->prevBL = kcosmos->feedbackBL;
			kcosmos->prevCL = kcosmos->feedbackCL;
			kcosmos->prevDL = kcosmos->feedbackDL;
			kcosmos->prevEL = kcosmos->feedbackEL;

			kcosmos->prevER = kcosmos->feedbackER;
			kcosmos->prevJR = kcosmos->feedbackJR;
			kcosmos->prevOR = kcosmos->feedbackOR;
			kcosmos->prevTR = kcosmos->feedbackTR;
			kcosmos->prevYR = kcosmos->feedbackYR;

			kcosmos->countAL++;
			if (kcosmos->countAL < 0 || kcosmos->countAL > delayA) kcosmos->countAL = 0;
			kcosmos->countBL++;
			if (kcosmos->countBL < 0 || kcosmos->countBL > delayB) kcosmos->countBL = 0;
			kcosmos->countCL++;
			if (kcosmos->countCL < 0 || kcosmos->countCL > delayC) kcosmos->countCL = 0;
			kcosmos->countDL++;
			if (kcosmos->countDL < 0 || kcosmos->countDL > delayD) kcosmos->countDL = 0;
			kcosmos->countEL++;
			if (kcosmos->countEL < 0 || kcosmos->countEL > delayE) kcosmos->countEL = 0;

			kcosmos->countER++;
			if (kcosmos->countER < 0 || kcosmos->countER > delayE) kcosmos->countER = 0;
			kcosmos->countJR++;
			if (kcosmos->countJR < 0 || kcosmos->countJR > delayJ) kcosmos->countJR = 0;
			kcosmos->countOR++;
			if (kcosmos->countOR < 0 || kcosmos->countOR > delayO) kcosmos->countOR = 0;
			kcosmos->countTR++;
			if (kcosmos->countTR < 0 || kcosmos->countTR > delayT) kcosmos->countTR = 0;
			kcosmos->countYR++;
			if (kcosmos->countYR < 0 || kcosmos->countYR > delayY) kcosmos->countYR = 0;

			double earlyAL = kcosmos->eAL[kcosmos->countAL - ((kcosmos->countAL > delayA) ? delayA + 1 : 0)];
			double earlyBL = kcosmos->eBL[kcosmos->countBL - ((kcosmos->countBL > delayB) ? delayB + 1 : 0)];
			double earlyCL = kcosmos->eCL[kcosmos->countCL - ((kcosmos->countCL > delayC) ? delayC + 1 : 0)];
			double earlyDL = kcosmos->eDL[kcosmos->countDL - ((kcosmos->countDL > delayD) ? delayD + 1 : 0)];
			double earlyEL = kcosmos->eEL[kcosmos->countEL - ((kcosmos->countEL > delayE) ? delayE + 1 : 0)];

			double earlyER = kcosmos->eER[kcosmos->countER - ((kcosmos->countER > delayE) ? delayE + 1 : 0)];
			double earlyJR = kcosmos->eJR[kcosmos->countJR - ((kcosmos->countJR > delayJ) ? delayJ + 1 : 0)];
			double earlyOR = kcosmos->eOR[kcosmos->countOR - ((kcosmos->countOR > delayO) ? delayO + 1 : 0)];
			double earlyTR = kcosmos->eTR[kcosmos->countTR - ((kcosmos->countTR > delayT) ? delayT + 1 : 0)];
			double earlyYR = kcosmos->eYR[kcosmos->countYR - ((kcosmos->countYR > delayY) ? delayY + 1 : 0)];

			double outAL = kcosmos->aAL[kcosmos->countAL - ((kcosmos->countAL > delayA) ? delayA + 1 : 0)];
			double outBL = kcosmos->aBL[kcosmos->countBL - ((kcosmos->countBL > delayB) ? delayB + 1 : 0)];
			double outCL = kcosmos->aCL[kcosmos->countCL - ((kcosmos->countCL > delayC) ? delayC + 1 : 0)];
			double outDL = kcosmos->aDL[kcosmos->countDL - ((kcosmos->countDL > delayD) ? delayD + 1 : 0)];
			double outEL = kcosmos->aEL[kcosmos->countEL - ((kcosmos->countEL > delayE) ? delayE + 1 : 0)];

			double outER = kcosmos->aER[kcosmos->countER - ((kcosmos->countER > delayE) ? delayE + 1 : 0)];
			double outJR = kcosmos->aJR[kcosmos->countJR - ((kcosmos->countJR > delayJ) ? delayJ + 1 : 0)];
			double outOR = kcosmos->aOR[kcosmos->countOR - ((kcosmos->countOR > delayO) ? delayO + 1 : 0)];
			double outTR = kcosmos->aTR[kcosmos->countTR - ((kcosmos->countTR > delayT) ? delayT + 1 : 0)];
			double outYR = kcosmos->aYR[kcosmos->countYR - ((kcosmos->countYR > delayY) ? delayY + 1 : 0)];

			//-------- one

			kcosmos->eFL[kcosmos->countFL] = ((earlyAL * 3.0) - ((earlyBL + earlyCL + earlyDL + earlyEL) * 2.0));
			kcosmos->eGL[kcosmos->countGL] = ((earlyBL * 3.0) - ((earlyAL + earlyCL + earlyDL + earlyEL) * 2.0));
			kcosmos->eHL[kcosmos->countHL] = ((earlyCL * 3.0) - ((earlyAL + earlyBL + earlyDL + earlyEL) * 2.0));
			kcosmos->eIL[kcosmos->countIL] = ((earlyDL * 3.0) - ((earlyAL + earlyBL + earlyCL + earlyEL) * 2.0));
			kcosmos->eJL[kcosmos->countJL] = ((earlyEL * 3.0) - ((earlyAL + earlyBL + earlyCL + earlyDL) * 2.0));

			kcosmos->eDR[kcosmos->countDR] = ((earlyER * 3.0) - ((earlyJR + earlyOR + earlyTR + earlyYR) * 2.0));
			kcosmos->eIR[kcosmos->countIR] = ((earlyJR * 3.0) - ((earlyER + earlyOR + earlyTR + earlyYR) * 2.0));
			kcosmos->eNR[kcosmos->countNR] = ((earlyOR * 3.0) - ((earlyER + earlyJR + earlyTR + earlyYR) * 2.0));
			kcosmos->eSR[kcosmos->countSR] = ((earlyTR * 3.0) - ((earlyER + earlyJR + earlyOR + earlyYR) * 2.0));
			kcosmos->eXR[kcosmos->countXR] = ((earlyYR * 3.0) - ((earlyER + earlyJR + earlyOR + earlyTR) * 2.0));

			kcosmos->aFL[kcosmos->countFL] = ((outAL * 3.0) - ((outBL + outCL + outDL + outEL) * 2.0));
			kcosmos->aGL[kcosmos->countGL] = ((outBL * 3.0) - ((outAL + outCL + outDL + outEL) * 2.0));
			kcosmos->aHL[kcosmos->countHL] = ((outCL * 3.0) - ((outAL + outBL + outDL + outEL) * 2.0));
			kcosmos->aIL[kcosmos->countIL] = ((outDL * 3.0) - ((outAL + outBL + outCL + outEL) * 2.0));
			kcosmos->aJL[kcosmos->countJL] = ((outEL * 3.0) - ((outAL + outBL + outCL + outDL) * 2.0));

			kcosmos->aDR[kcosmos->countDR] = ((outER * 3.0) - ((outJR + outOR + outTR + outYR) * 2.0));
			kcosmos->aIR[kcosmos->countIR] = ((outJR * 3.0) - ((outER + outOR + outTR + outYR) * 2.0));
			kcosmos->aNR[kcosmos->countNR] = ((outOR * 3.0) - ((outER + outJR + outTR + outYR) * 2.0));
			kcosmos->aSR[kcosmos->countSR] = ((outTR * 3.0) - ((outER + outJR + outOR + outYR) * 2.0));
			kcosmos->aXR[kcosmos->countXR] = ((outYR * 3.0) - ((outER + outJR + outOR + outTR) * 2.0));

			kcosmos->countFL++;
			if (kcosmos->countFL < 0 || kcosmos->countFL > delayF) kcosmos->countFL = 0;
			kcosmos->countGL++;
			if (kcosmos->countGL < 0 || kcosmos->countGL > delayG) kcosmos->countGL = 0;
			kcosmos->countHL++;
			if (kcosmos->countHL < 0 || kcosmos->countHL > delayH) kcosmos->countHL = 0;
			kcosmos->countIL++;
			if (kcosmos->countIL < 0 || kcosmos->countIL > delayI) kcosmos->countIL = 0;
			kcosmos->countJL++;
			if (kcosmos->countJL < 0 || kcosmos->countJL > delayJ) kcosmos->countJL = 0;

			kcosmos->countDR++;
			if (kcosmos->countDR < 0 || kcosmos->countDR > delayD) kcosmos->countDR = 0;
			kcosmos->countIR++;
			if (kcosmos->countIR < 0 || kcosmos->countIR > delayI) kcosmos->countIR = 0;
			kcosmos->countNR++;
			if (kcosmos->countNR < 0 || kcosmos->countNR > delayN) kcosmos->countNR = 0;
			kcosmos->countSR++;
			if (kcosmos->countSR < 0 || kcosmos->countSR > delayS) kcosmos->countSR = 0;
			kcosmos->countXR++;
			if (kcosmos->countXR < 0 || kcosmos->countXR > delayX) kcosmos->countXR = 0;

			double earlyFL = kcosmos->eFL[kcosmos->countFL - ((kcosmos->countFL > delayF) ? delayF + 1 : 0)];
			double earlyGL = kcosmos->eGL[kcosmos->countGL - ((kcosmos->countGL > delayG) ? delayG + 1 : 0)];
			double earlyHL = kcosmos->eHL[kcosmos->countHL - ((kcosmos->countHL > delayH) ? delayH + 1 : 0)];
			double earlyIL = kcosmos->eIL[kcosmos->countIL - ((kcosmos->countIL > delayI) ? delayI + 1 : 0)];
			double earlyJL = kcosmos->eJL[kcosmos->countJL - ((kcosmos->countJL > delayJ) ? delayJ + 1 : 0)];

			double earlyDR = kcosmos->eDR[kcosmos->countDR - ((kcosmos->countDR > delayD) ? delayD + 1 : 0)];
			double earlyIR = kcosmos->eIR[kcosmos->countIR - ((kcosmos->countIR > delayI) ? delayI + 1 : 0)];
			double earlyNR = kcosmos->eNR[kcosmos->countNR - ((kcosmos->countNR > delayN) ? delayN + 1 : 0)];
			double earlySR = kcosmos->eSR[kcosmos->countSR - ((kcosmos->countSR > delayS) ? delayS + 1 : 0)];
			double earlyXR = kcosmos->eXR[kcosmos->countXR - ((kcosmos->countXR > delayX) ? delayX + 1 : 0)];

			double outFL = kcosmos->aFL[kcosmos->countFL - ((kcosmos->countFL > delayF) ? delayF + 1 : 0)];
			double outGL = kcosmos->aGL[kcosmos->countGL - ((kcosmos->countGL > delayG) ? delayG + 1 : 0)];
			double outHL = kcosmos->aHL[kcosmos->countHL - ((kcosmos->countHL > delayH) ? delayH + 1 : 0)];
			double outIL = kcosmos->aIL[kcosmos->countIL - ((kcosmos->countIL > delayI) ? delayI + 1 : 0)];
			double outJL = kcosmos->aJL[kcosmos->countJL - ((kcosmos->countJL > delayJ) ? delayJ + 1 : 0)];

			double outDR = kcosmos->aDR[kcosmos->countDR - ((kcosmos->countDR > delayD) ? delayD + 1 : 0)];
			double outIR = kcosmos->aIR[kcosmos->countIR - ((kcosmos->countIR > delayI) ? delayI + 1 : 0)];
			double outNR = kcosmos->aNR[kcosmos->countNR - ((kcosmos->countNR > delayN) ? delayN + 1 : 0)];
			double outSR = kcosmos->aSR[kcosmos->countSR - ((kcosmos->countSR > delayS) ? delayS + 1 : 0)];
			double outXR = kcosmos->aXR[kcosmos->countXR - ((kcosmos->countXR > delayX) ? delayX + 1 : 0)];

			//-------- two

			kcosmos->eKL[kcosmos->countKL] = ((earlyFL * 3.0) - ((earlyGL + earlyHL + earlyIL + earlyJL) * 2.0));
			kcosmos->eLL[kcosmos->countLL] = ((earlyGL * 3.0) - ((earlyFL + earlyHL + earlyIL + earlyJL) * 2.0));
			kcosmos->eML[kcosmos->countML] = ((earlyHL * 3.0) - ((earlyFL + earlyGL + earlyIL + earlyJL) * 2.0));
			kcosmos->eNL[kcosmos->countNL] = ((earlyIL * 3.0) - ((earlyFL + earlyGL + earlyHL + earlyJL) * 2.0));
			kcosmos->eOL[kcosmos->countOL] = ((earlyJL * 3.0) - ((earlyFL + earlyGL + earlyHL + earlyIL) * 2.0));

			kcosmos->eCR[kcosmos->countCR] = ((earlyDR * 3.0) - ((earlyIR + earlyNR + earlySR + earlyXR) * 2.0));
			kcosmos->eHR[kcosmos->countHR] = ((earlyIR * 3.0) - ((earlyDR + earlyNR + earlySR + earlyXR) * 2.0));
			kcosmos->eMR[kcosmos->countMR] = ((earlyNR * 3.0) - ((earlyDR + earlyIR + earlySR + earlyXR) * 2.0));
			kcosmos->eRR[kcosmos->countRR] = ((earlySR * 3.0) - ((earlyDR + earlyIR + earlyNR + earlyXR) * 2.0));
			kcosmos->eWR[kcosmos->countWR] = ((earlyXR * 3.0) - ((earlyDR + earlyIR + earlyNR + earlySR) * 2.0));

			kcosmos->aKL[kcosmos->countKL] = ((outFL * 3.0) - ((outGL + outHL + outIL + outJL) * 2.0));
			kcosmos->aLL[kcosmos->countLL] = ((outGL * 3.0) - ((outFL + outHL + outIL + outJL) * 2.0));
			kcosmos->aML[kcosmos->countML] = ((outHL * 3.0) - ((outFL + outGL + outIL + outJL) * 2.0));
			kcosmos->aNL[kcosmos->countNL] = ((outIL * 3.0) - ((outFL + outGL + outHL + outJL) * 2.0));
			kcosmos->aOL[kcosmos->countOL] = ((outJL * 3.0) - ((outFL + outGL + outHL + outIL) * 2.0));

			kcosmos->aCR[kcosmos->countCR] = ((outDR * 3.0) - ((outIR + outNR + outSR + outXR) * 2.0));
			kcosmos->aHR[kcosmos->countHR] = ((outIR * 3.0) - ((outDR + outNR + outSR + outXR) * 2.0));
			kcosmos->aMR[kcosmos->countMR] = ((outNR * 3.0) - ((outDR + outIR + outSR + outXR) * 2.0));
			kcosmos->aRR[kcosmos->countRR] = ((outSR * 3.0) - ((outDR + outIR + outNR + outXR) * 2.0));
			kcosmos->aWR[kcosmos->countWR] = ((outXR * 3.0) - ((outDR + outIR + outNR + outSR) * 2.0));

			kcosmos->countKL++;
			if (kcosmos->countKL < 0 || kcosmos->countKL > delayK) kcosmos->countKL = 0;
			kcosmos->countLL++;
			if (kcosmos->countLL < 0 || kcosmos->countLL > delayL) kcosmos->countLL = 0;
			kcosmos->countML++;
			if (kcosmos->countML < 0 || kcosmos->countML > delayM) kcosmos->countML = 0;
			kcosmos->countNL++;
			if (kcosmos->countNL < 0 || kcosmos->countNL > delayN) kcosmos->countNL = 0;
			kcosmos->countOL++;
			if (kcosmos->countOL < 0 || kcosmos->countOL > delayO) kcosmos->countOL = 0;

			kcosmos->countCR++;
			if (kcosmos->countCR < 0 || kcosmos->countCR > delayC) kcosmos->countCR = 0;
			kcosmos->countHR++;
			if (kcosmos->countHR < 0 || kcosmos->countHR > delayH) kcosmos->countHR = 0;
			kcosmos->countMR++;
			if (kcosmos->countMR < 0 || kcosmos->countMR > delayM) kcosmos->countMR = 0;
			kcosmos->countRR++;
			if (kcosmos->countRR < 0 || kcosmos->countRR > delayR) kcosmos->countRR = 0;
			kcosmos->countWR++;
			if (kcosmos->countWR < 0 || kcosmos->countWR > delayW) kcosmos->countWR = 0;

			double earlyKL = kcosmos->eKL[kcosmos->countKL - ((kcosmos->countKL > delayK) ? delayK + 1 : 0)];
			double earlyLL = kcosmos->eLL[kcosmos->countLL - ((kcosmos->countLL > delayL) ? delayL + 1 : 0)];
			double earlyML = kcosmos->eML[kcosmos->countML - ((kcosmos->countML > delayM) ? delayM + 1 : 0)];
			double earlyNL = kcosmos->eNL[kcosmos->countNL - ((kcosmos->countNL > delayN) ? delayN + 1 : 0)];
			double earlyOL = kcosmos->eOL[kcosmos->countOL - ((kcosmos->countOL > delayO) ? delayO + 1 : 0)];

			double earlyCR = kcosmos->eCR[kcosmos->countCR - ((kcosmos->countCR > delayC) ? delayC + 1 : 0)];
			double earlyHR = kcosmos->eHR[kcosmos->countHR - ((kcosmos->countHR > delayH) ? delayH + 1 : 0)];
			double earlyMR = kcosmos->eMR[kcosmos->countMR - ((kcosmos->countMR > delayM) ? delayM + 1 : 0)];
			double earlyRR = kcosmos->eRR[kcosmos->countRR - ((kcosmos->countRR > delayR) ? delayR + 1 : 0)];
			double earlyWR = kcosmos->eWR[kcosmos->countWR - ((kcosmos->countWR > delayW) ? delayW + 1 : 0)];

			double outKL = kcosmos->aKL[kcosmos->countKL - ((kcosmos->countKL > delayK) ? delayK + 1 : 0)];
			double outLL = kcosmos->aLL[kcosmos->countLL - ((kcosmos->countLL > delayL) ? delayL + 1 : 0)];
			double outML = kcosmos->aML[kcosmos->countML - ((kcosmos->countML > delayM) ? delayM + 1 : 0)];
			double outNL = kcosmos->aNL[kcosmos->countNL - ((kcosmos->countNL > delayN) ? delayN + 1 : 0)];
			double outOL = kcosmos->aOL[kcosmos->countOL - ((kcosmos->countOL > delayO) ? delayO + 1 : 0)];

			double outCR = kcosmos->aCR[kcosmos->countCR - ((kcosmos->countCR > delayC) ? delayC + 1 : 0)];
			double outHR = kcosmos->aHR[kcosmos->countHR - ((kcosmos->countHR > delayH) ? delayH + 1 : 0)];
			double outMR = kcosmos->aMR[kcosmos->countMR - ((kcosmos->countMR > delayM) ? delayM + 1 : 0)];
			double outRR = kcosmos->aRR[kcosmos->countRR - ((kcosmos->countRR > delayR) ? delayR + 1 : 0)];
			double outWR = kcosmos->aWR[kcosmos->countWR - ((kcosmos->countWR > delayW) ? delayW + 1 : 0)];

			//-------- three

			kcosmos->ePL[kcosmos->countPL] = ((earlyKL * 3.0) - ((earlyLL + earlyML + earlyNL + earlyOL) * 2.0));
			kcosmos->eQL[kcosmos->countQL] = ((earlyLL * 3.0) - ((earlyKL + earlyML + earlyNL + earlyOL) * 2.0));
			kcosmos->eRL[kcosmos->countRL] = ((earlyML * 3.0) - ((earlyKL + earlyLL + earlyNL + earlyOL) * 2.0));
			kcosmos->eSL[kcosmos->countSL] = ((earlyNL * 3.0) - ((earlyKL + earlyLL + earlyML + earlyOL) * 2.0));
			kcosmos->eTL[kcosmos->countTL] = ((earlyOL * 3.0) - ((earlyKL + earlyLL + earlyML + earlyNL) * 2.0));

			kcosmos->eBR[kcosmos->countBR] = ((earlyCR * 3.0) - ((earlyHR + earlyMR + earlyRR + earlyWR) * 2.0));
			kcosmos->eGR[kcosmos->countGR] = ((earlyHR * 3.0) - ((earlyCR + earlyMR + earlyRR + earlyWR) * 2.0));
			kcosmos->eLR[kcosmos->countLR] = ((earlyMR * 3.0) - ((earlyCR + earlyHR + earlyRR + earlyWR) * 2.0));
			kcosmos->eQR[kcosmos->countQR] = ((earlyRR * 3.0) - ((earlyCR + earlyHR + earlyMR + earlyWR) * 2.0));
			kcosmos->eVR[kcosmos->countVR] = ((earlyWR * 3.0) - ((earlyCR + earlyHR + earlyMR + earlyRR) * 2.0));

			kcosmos->aPL[kcosmos->countPL] = ((outKL * 3.0) - ((outLL + outML + outNL + outOL) * 2.0));
			kcosmos->aQL[kcosmos->countQL] = ((outLL * 3.0) - ((outKL + outML + outNL + outOL) * 2.0));
			kcosmos->aRL[kcosmos->countRL] = ((outML * 3.0) - ((outKL + outLL + outNL + outOL) * 2.0));
			kcosmos->aSL[kcosmos->countSL] = ((outNL * 3.0) - ((outKL + outLL + outML + outOL) * 2.0));
			kcosmos->aTL[kcosmos->countTL] = ((outOL * 3.0) - ((outKL + outLL + outML + outNL) * 2.0));

			kcosmos->aBR[kcosmos->countBR] = ((outCR * 3.0) - ((outHR + outMR + outRR + outWR) * 2.0));
			kcosmos->aGR[kcosmos->countGR] = ((outHR * 3.0) - ((outCR + outMR + outRR + outWR) * 2.0));
			kcosmos->aLR[kcosmos->countLR] = ((outMR * 3.0) - ((outCR + outHR + outRR + outWR) * 2.0));
			kcosmos->aQR[kcosmos->countQR] = ((outRR * 3.0) - ((outCR + outHR + outMR + outWR) * 2.0));
			kcosmos->aVR[kcosmos->countVR] = ((outWR * 3.0) - ((outCR + outHR + outMR + outRR) * 2.0));

			kcosmos->countPL++;
			if (kcosmos->countPL < 0 || kcosmos->countPL > delayP) kcosmos->countPL = 0;
			kcosmos->countQL++;
			if (kcosmos->countQL < 0 || kcosmos->countQL > delayQ) kcosmos->countQL = 0;
			kcosmos->countRL++;
			if (kcosmos->countRL < 0 || kcosmos->countRL > delayR) kcosmos->countRL = 0;
			kcosmos->countSL++;
			if (kcosmos->countSL < 0 || kcosmos->countSL > delayS) kcosmos->countSL = 0;
			kcosmos->countTL++;
			if (kcosmos->countTL < 0 || kcosmos->countTL > delayT) kcosmos->countTL = 0;

			kcosmos->countBR++;
			if (kcosmos->countBR < 0 || kcosmos->countBR > delayB) kcosmos->countBR = 0;
			kcosmos->countGR++;
			if (kcosmos->countGR < 0 || kcosmos->countGR > delayG) kcosmos->countGR = 0;
			kcosmos->countLR++;
			if (kcosmos->countLR < 0 || kcosmos->countLR > delayL) kcosmos->countLR = 0;
			kcosmos->countQR++;
			if (kcosmos->countQR < 0 || kcosmos->countQR > delayQ) kcosmos->countQR = 0;
			kcosmos->countVR++;
			if (kcosmos->countVR < 0 || kcosmos->countVR > delayV) kcosmos->countVR = 0;

			double earlyPL = kcosmos->ePL[kcosmos->countPL - ((kcosmos->countPL > delayP) ? delayP + 1 : 0)];
			double earlyQL = kcosmos->eQL[kcosmos->countQL - ((kcosmos->countQL > delayQ) ? delayQ + 1 : 0)];
			double earlyRL = kcosmos->eRL[kcosmos->countRL - ((kcosmos->countRL > delayR) ? delayR + 1 : 0)];
			double earlySL = kcosmos->eSL[kcosmos->countSL - ((kcosmos->countSL > delayS) ? delayS + 1 : 0)];
			double earlyTL = kcosmos->eTL[kcosmos->countTL - ((kcosmos->countTL > delayT) ? delayT + 1 : 0)];

			double earlyBR = kcosmos->eBR[kcosmos->countBR - ((kcosmos->countBR > delayB) ? delayB + 1 : 0)];
			double earlyGR = kcosmos->eGR[kcosmos->countGR - ((kcosmos->countGR > delayG) ? delayG + 1 : 0)];
			double earlyLR = kcosmos->eLR[kcosmos->countLR - ((kcosmos->countLR > delayL) ? delayL + 1 : 0)];
			double earlyQR = kcosmos->eQR[kcosmos->countQR - ((kcosmos->countQR > delayQ) ? delayQ + 1 : 0)];
			double earlyVR = kcosmos->eVR[kcosmos->countVR - ((kcosmos->countVR > delayV) ? delayV + 1 : 0)];

			double outPL = kcosmos->aPL[kcosmos->countPL - ((kcosmos->countPL > delayP) ? delayP + 1 : 0)];
			double outQL = kcosmos->aQL[kcosmos->countQL - ((kcosmos->countQL > delayQ) ? delayQ + 1 : 0)];
			double outRL = kcosmos->aRL[kcosmos->countRL - ((kcosmos->countRL > delayR) ? delayR + 1 : 0)];
			double outSL = kcosmos->aSL[kcosmos->countSL - ((kcosmos->countSL > delayS) ? delayS + 1 : 0)];
			double outTL = kcosmos->aTL[kcosmos->countTL - ((kcosmos->countTL > delayT) ? delayT + 1 : 0)];

			double outBR = kcosmos->aBR[kcosmos->countBR - ((kcosmos->countBR > delayB) ? delayB + 1 : 0)];
			double outGR = kcosmos->aGR[kcosmos->countGR - ((kcosmos->countGR > delayG) ? delayG + 1 : 0)];
			double outLR = kcosmos->aLR[kcosmos->countLR - ((kcosmos->countLR > delayL) ? delayL + 1 : 0)];
			double outQR = kcosmos->aQR[kcosmos->countQR - ((kcosmos->countQR > delayQ) ? delayQ + 1 : 0)];
			double outVR = kcosmos->aVR[kcosmos->countVR - ((kcosmos->countVR > delayV) ? delayV + 1 : 0)];

			//-------- four

			kcosmos->eUL[kcosmos->countUL] = ((earlyPL * 3.0) - ((earlyQL + earlyRL + earlySL + earlyTL) * 2.0));
			kcosmos->eVL[kcosmos->countVL] = ((earlyQL * 3.0) - ((earlyPL + earlyRL + earlySL + earlyTL) * 2.0));
			kcosmos->eWL[kcosmos->countWL] = ((earlyRL * 3.0) - ((earlyPL + earlyQL + earlySL + earlyTL) * 2.0));
			kcosmos->eXL[kcosmos->countXL] = ((earlySL * 3.0) - ((earlyPL + earlyQL + earlyRL + earlyTL) * 2.0));
			kcosmos->eYL[kcosmos->countYL] = ((earlyTL * 3.0) - ((earlyPL + earlyQL + earlyRL + earlySL) * 2.0));

			kcosmos->eAR[kcosmos->countAR] = ((earlyBR * 3.0) - ((earlyGR + earlyLR + earlyQR + earlyVR) * 2.0));
			kcosmos->eFR[kcosmos->countFR] = ((earlyGR * 3.0) - ((earlyBR + earlyLR + earlyQR + earlyVR) * 2.0));
			kcosmos->eKR[kcosmos->countKR] = ((earlyLR * 3.0) - ((earlyBR + earlyGR + earlyQR + earlyVR) * 2.0));
			kcosmos->ePR[kcosmos->countPR] = ((earlyQR * 3.0) - ((earlyBR + earlyGR + earlyLR + earlyVR) * 2.0));
			kcosmos->eUR[kcosmos->countUR] = ((earlyVR * 3.0) - ((earlyBR + earlyGR + earlyLR + earlyQR) * 2.0));

			kcosmos->aUL[kcosmos->countUL] = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0));
			kcosmos->aVL[kcosmos->countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
			kcosmos->aWL[kcosmos->countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
			kcosmos->aXL[kcosmos->countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
			kcosmos->aYL[kcosmos->countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));

			kcosmos->aAR[kcosmos->countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
			kcosmos->aFR[kcosmos->countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
			kcosmos->aKR[kcosmos->countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
			kcosmos->aPR[kcosmos->countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
			kcosmos->aUR[kcosmos->countUR] = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0));

			kcosmos->countUL++;
			if (kcosmos->countUL < 0 || kcosmos->countUL > delayU) kcosmos->countUL = 0;
			kcosmos->countVL++;
			if (kcosmos->countVL < 0 || kcosmos->countVL > delayV) kcosmos->countVL = 0;
			kcosmos->countWL++;
			if (kcosmos->countWL < 0 || kcosmos->countWL > delayW) kcosmos->countWL = 0;
			kcosmos->countXL++;
			if (kcosmos->countXL < 0 || kcosmos->countXL > delayX) kcosmos->countXL = 0;
			kcosmos->countYL++;
			if (kcosmos->countYL < 0 || kcosmos->countYL > delayY) kcosmos->countYL = 0;

			kcosmos->countAR++;
			if (kcosmos->countAR < 0 || kcosmos->countAR > delayA) kcosmos->countAR = 0;
			kcosmos->countFR++;
			if (kcosmos->countFR < 0 || kcosmos->countFR > delayF) kcosmos->countFR = 0;
			kcosmos->countKR++;
			if (kcosmos->countKR < 0 || kcosmos->countKR > delayK) kcosmos->countKR = 0;
			kcosmos->countPR++;
			if (kcosmos->countPR < 0 || kcosmos->countPR > delayP) kcosmos->countPR = 0;
			kcosmos->countUR++;
			if (kcosmos->countUR < 0 || kcosmos->countUR > delayU) kcosmos->countUR = 0;

			double earlyUL = kcosmos->eUL[kcosmos->countUL - ((kcosmos->countUL > delayU) ? delayU + 1 : 0)];
			double earlyVL = kcosmos->eVL[kcosmos->countVL - ((kcosmos->countVL > delayV) ? delayV + 1 : 0)];
			double earlyWL = kcosmos->eWL[kcosmos->countWL - ((kcosmos->countWL > delayW) ? delayW + 1 : 0)];
			double earlyXL = kcosmos->eXL[kcosmos->countXL - ((kcosmos->countXL > delayX) ? delayX + 1 : 0)];
			double earlyYL = kcosmos->eYL[kcosmos->countYL - ((kcosmos->countYL > delayY) ? delayY + 1 : 0)];

			double earlyAR = kcosmos->eAR[kcosmos->countAR - ((kcosmos->countAR > delayA) ? delayA + 1 : 0)];
			double earlyFR = kcosmos->eFR[kcosmos->countFR - ((kcosmos->countFR > delayF) ? delayF + 1 : 0)];
			double earlyKR = kcosmos->eKR[kcosmos->countKR - ((kcosmos->countKR > delayK) ? delayK + 1 : 0)];
			double earlyPR = kcosmos->ePR[kcosmos->countPR - ((kcosmos->countPR > delayP) ? delayP + 1 : 0)];
			double earlyUR = kcosmos->eUR[kcosmos->countUR - ((kcosmos->countUR > delayU) ? delayU + 1 : 0)];

			double outUL = kcosmos->aUL[kcosmos->countUL - ((kcosmos->countUL > delayU) ? delayU + 1 : 0)];
			double outVL = kcosmos->aVL[kcosmos->countVL - ((kcosmos->countVL > delayV) ? delayV + 1 : 0)];
			double outWL = kcosmos->aWL[kcosmos->countWL - ((kcosmos->countWL > delayW) ? delayW + 1 : 0)];
			double outXL = kcosmos->aXL[kcosmos->countXL - ((kcosmos->countXL > delayX) ? delayX + 1 : 0)];
			double outYL = kcosmos->aYL[kcosmos->countYL - ((kcosmos->countYL > delayY) ? delayY + 1 : 0)];

			double outAR = kcosmos->aAR[kcosmos->countAR - ((kcosmos->countAR > delayA) ? delayA + 1 : 0)];
			double outFR = kcosmos->aFR[kcosmos->countFR - ((kcosmos->countFR > delayF) ? delayF + 1 : 0)];
			double outKR = kcosmos->aKR[kcosmos->countKR - ((kcosmos->countKR > delayK) ? delayK + 1 : 0)];
			double outPR = kcosmos->aPR[kcosmos->countPR - ((kcosmos->countPR > delayP) ? delayP + 1 : 0)];
			double outUR = kcosmos->aUR[kcosmos->countUR - ((kcosmos->countUR > delayU) ? delayU + 1 : 0)];

			//-------- five

			kcosmos->earlyReflectionL = (earlyUL + earlyVL + earlyWL + earlyXL + earlyYL) * 0.0008;
			kcosmos->earlyReflectionR = (earlyAR + earlyFR + earlyKR + earlyPR + earlyUR) * 0.0008;
			// and take the final combined sum of outputs, corrected for Householder gain

			kcosmos->feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
			feedbackSum = fabs(kcosmos->feedbackAL);
			kcosmos->feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackER);
			kcosmos->feedbackBL = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackBL);
			kcosmos->feedbackJR = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackJR);
			kcosmos->feedbackCL = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackCL);
			kcosmos->feedbackOR = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackOR);
			kcosmos->feedbackDL = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackDL);
			kcosmos->feedbackTR = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackTR);
			kcosmos->feedbackEL = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackEL);
			kcosmos->feedbackYR = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
			feedbackSum += fabs(kcosmos->feedbackYR);

			inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0008;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0008;
			// and take the final combined sum of outputs, corrected for Householder gain

			inputSampleL += (kcosmos->earlyReflectionL * earlyLoudness);
			inputSampleR += (kcosmos->earlyReflectionR * earlyLoudness);

			kcosmos->bez[bez_CL] = kcosmos->bez[bez_BL];
			kcosmos->bez[bez_BL] = kcosmos->bez[bez_AL];
			kcosmos->bez[bez_AL] = inputSampleL;
			kcosmos->bez[bez_SampL] = 0.0;

			kcosmos->bez[bez_CR] = kcosmos->bez[bez_BR];
			kcosmos->bez[bez_BR] = kcosmos->bez[bez_AR];
			kcosmos->bez[bez_AR] = inputSampleR;
			kcosmos->bez[bez_SampR] = 0.0;
		}
		double CBL = (kcosmos->bez[bez_CL] * (1.0 - kcosmos->bez[bez_cycle])) + (kcosmos->bez[bez_BL] * kcosmos->bez[bez_cycle]);
		double CBR = (kcosmos->bez[bez_CR] * (1.0 - kcosmos->bez[bez_cycle])) + (kcosmos->bez[bez_BR] * kcosmos->bez[bez_cycle]);
		double BAL = (kcosmos->bez[bez_BL] * (1.0 - kcosmos->bez[bez_cycle])) + (kcosmos->bez[bez_AL] * kcosmos->bez[bez_cycle]);
		double BAR = (kcosmos->bez[bez_BR] * (1.0 - kcosmos->bez[bez_cycle])) + (kcosmos->bez[bez_AR] * kcosmos->bez[bez_cycle]);
		double CBAL = (kcosmos->bez[bez_BL] + (CBL * (1.0 - kcosmos->bez[bez_cycle])) + (BAL * kcosmos->bez[bez_cycle])) * 0.125;
		double CBAR = (kcosmos->bez[bez_BR] + (CBR * (1.0 - kcosmos->bez[bez_cycle])) + (BAR * kcosmos->bez[bez_cycle])) * 0.125;
		inputSampleL = CBAL;
		inputSampleR = CBAR;

		inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
		inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		kcosmos->fpdL ^= kcosmos->fpdL << 13;
		kcosmos->fpdL ^= kcosmos->fpdL >> 17;
		kcosmos->fpdL ^= kcosmos->fpdL << 5;
		inputSampleL += (((double) kcosmos->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		kcosmos->fpdR ^= kcosmos->fpdR << 13;
		kcosmos->fpdR ^= kcosmos->fpdR >> 17;
		kcosmos->fpdR ^= kcosmos->fpdR << 5;
		inputSampleR += (((double) kcosmos->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	KCOSMOS_URI,
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
