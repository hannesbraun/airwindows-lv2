#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define KPLATEC_URI "https://hannesbraun.net/ns/lv2/airwindows/kplatec"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT_PAD = 4,
	DAMPING = 5,
	LOW_CUT = 6,
	PREDELAY = 7,
	WETNESS = 8
} PortIndex;

// PREDELAY MUST BE 24010 for half second delay at multiple of 48k (scaled down for 44.1)
const int earlyA = 31;
const int earlyB = 661;
const int earlyC = 73;
const int earlyD = 137;
const int earlyE = 811;
const int earlyF = 269;
const int earlyG = 701;
const int earlyH = 7;
const int earlyI = 449;
const int predelay = 24010; // 49 ms, 288 seat club
const int delayA = 547;
const int delayB = 149;
const int delayC = 67;
const int delayD = 619;
const int delayE = 241;
const int delayF = 79;
const int delayG = 359;
const int delayH = 19;
const int delayI = 557;
const int delayJ = 601;
const int delayK = 7;
const int delayL = 769;
const int delayM = 809;
const int delayN = 17;
const int delayO = 41;
const int delayP = 829;
const int delayQ = 47;
const int delayR = 569;
const int delayS = 3;
const int delayT = 509;
const int delayU = 857;
const int delayV = 433;
const int delayW = 29;
const int delayX = 449;
const int delayY = 137; // 84 ms, 843 seat hall. Scarcity, 1 in 188924

enum {
	fix_freq,
	fix_reso,
	fix_a0,
	fix_a1,
	fix_a2,
	fix_b1,
	fix_b2,
	fix_sL1,
	fix_sL2,
	fix_sR1,
	fix_sR2,
	fix_total
}; // fixed frequency biquad filter for ultrasonics, stereo

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputPad;
	const float* damping;
	const float* lowCut;
	const float* predelay;
	const float* wetness;

	double iirAL;
	double iirBL;

	double gainIn;
	double gainOutL;
	double gainOutR;

	double eAL[earlyA + 5];
	double eBL[earlyB + 5];
	double eCL[earlyC + 5];
	double eDL[earlyD + 5];
	double eEL[earlyE + 5];
	double eFL[earlyF + 5];
	double eGL[earlyG + 5];
	double eHL[earlyH + 5];
	double eIL[earlyI + 5];

	double eAR[earlyA + 5];
	double eBR[earlyB + 5];
	double eCR[earlyC + 5];
	double eDR[earlyD + 5];
	double eER[earlyE + 5];
	double eFR[earlyF + 5];
	double eGR[earlyG + 5];
	double eHR[earlyH + 5];
	double eIR[earlyI + 5];

	int earlyAL, earlyAR;
	int earlyBL, earlyBR;
	int earlyCL, earlyCR;
	int earlyDL, earlyDR;
	int earlyEL, earlyER;
	int earlyFL, earlyFR;
	int earlyGL, earlyGR;
	int earlyHL, earlyHR;
	int earlyIL, earlyIR;

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

	double previousAL;
	double previousBL;
	double previousCL;
	double previousDL;
	double previousEL;

	double lastRefL[7];

	double iirAR;
	double iirBR;

	double previousAR;
	double previousBR;
	double previousCR;
	double previousDR;
	double previousER;

	double lastRefR[7];

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

	int cycle;

	double fixA[fix_total];
	double fixB[fix_total];
	double fixC[fix_total];
	double fixD[fix_total];

	double prevMulchDL;
	double prevMulchDR;
	double prevMulchEL;
	double prevMulchER;
	double prevOutDL;
	double prevOutDR;
	double prevOutEL;
	double prevOutER;
	double prevInDL;
	double prevInDR;
	double prevInEL;
	double prevInER;

	uint32_t fpdL;
	uint32_t fpdR;
} KPlateC;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	KPlateC* kPlateC = (KPlateC*) calloc(1, sizeof(KPlateC));
	kPlateC->sampleRate = rate;
	return (LV2_Handle) kPlateC;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	KPlateC* kPlateC = (KPlateC*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			kPlateC->input[0] = (const float*) data;
			break;
		case INPUT_R:
			kPlateC->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			kPlateC->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			kPlateC->output[1] = (float*) data;
			break;
		case INPUT_PAD:
			kPlateC->inputPad = (const float*) data;
			break;
		case DAMPING:
			kPlateC->damping = (const float*) data;
			break;
		case LOW_CUT:
			kPlateC->lowCut = (const float*) data;
			break;
		case PREDELAY:
			kPlateC->predelay = (const float*) data;
			break;
		case WETNESS:
			kPlateC->wetness = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	KPlateC* kPlateC = (KPlateC*) instance;

	kPlateC->iirAL = 0.0;
	kPlateC->iirBL = 0.0;

	kPlateC->iirAR = 0.0;
	kPlateC->iirBR = 0.0;

	kPlateC->gainIn = kPlateC->gainOutL = kPlateC->gainOutR = 1.0;

	for (int count = 0; count < delayA + 2; count++) {
		kPlateC->aAL[count] = 0.0;
		kPlateC->aAR[count] = 0.0;
	}
	for (int count = 0; count < delayB + 2; count++) {
		kPlateC->aBL[count] = 0.0;
		kPlateC->aBR[count] = 0.0;
	}
	for (int count = 0; count < delayC + 2; count++) {
		kPlateC->aCL[count] = 0.0;
		kPlateC->aCR[count] = 0.0;
	}
	for (int count = 0; count < delayD + 2; count++) {
		kPlateC->aDL[count] = 0.0;
		kPlateC->aDR[count] = 0.0;
	}
	for (int count = 0; count < delayE + 2; count++) {
		kPlateC->aEL[count] = 0.0;
		kPlateC->aER[count] = 0.0;
	}
	for (int count = 0; count < delayF + 2; count++) {
		kPlateC->aFL[count] = 0.0;
		kPlateC->aFR[count] = 0.0;
	}
	for (int count = 0; count < delayG + 2; count++) {
		kPlateC->aGL[count] = 0.0;
		kPlateC->aGR[count] = 0.0;
	}
	for (int count = 0; count < delayH + 2; count++) {
		kPlateC->aHL[count] = 0.0;
		kPlateC->aHR[count] = 0.0;
	}
	for (int count = 0; count < delayI + 2; count++) {
		kPlateC->aIL[count] = 0.0;
		kPlateC->aIR[count] = 0.0;
	}
	for (int count = 0; count < delayJ + 2; count++) {
		kPlateC->aJL[count] = 0.0;
		kPlateC->aJR[count] = 0.0;
	}
	for (int count = 0; count < delayK + 2; count++) {
		kPlateC->aKL[count] = 0.0;
		kPlateC->aKR[count] = 0.0;
	}
	for (int count = 0; count < delayL + 2; count++) {
		kPlateC->aLL[count] = 0.0;
		kPlateC->aLR[count] = 0.0;
	}
	for (int count = 0; count < delayM + 2; count++) {
		kPlateC->aML[count] = 0.0;
		kPlateC->aMR[count] = 0.0;
	}
	for (int count = 0; count < delayN + 2; count++) {
		kPlateC->aNL[count] = 0.0;
		kPlateC->aNR[count] = 0.0;
	}
	for (int count = 0; count < delayO + 2; count++) {
		kPlateC->aOL[count] = 0.0;
		kPlateC->aOR[count] = 0.0;
	}
	for (int count = 0; count < delayP + 2; count++) {
		kPlateC->aPL[count] = 0.0;
		kPlateC->aPR[count] = 0.0;
	}
	for (int count = 0; count < delayQ + 2; count++) {
		kPlateC->aQL[count] = 0.0;
		kPlateC->aQR[count] = 0.0;
	}
	for (int count = 0; count < delayR + 2; count++) {
		kPlateC->aRL[count] = 0.0;
		kPlateC->aRR[count] = 0.0;
	}
	for (int count = 0; count < delayS + 2; count++) {
		kPlateC->aSL[count] = 0.0;
		kPlateC->aSR[count] = 0.0;
	}
	for (int count = 0; count < delayT + 2; count++) {
		kPlateC->aTL[count] = 0.0;
		kPlateC->aTR[count] = 0.0;
	}
	for (int count = 0; count < delayU + 2; count++) {
		kPlateC->aUL[count] = 0.0;
		kPlateC->aUR[count] = 0.0;
	}
	for (int count = 0; count < delayV + 2; count++) {
		kPlateC->aVL[count] = 0.0;
		kPlateC->aVR[count] = 0.0;
	}
	for (int count = 0; count < delayW + 2; count++) {
		kPlateC->aWL[count] = 0.0;
		kPlateC->aWR[count] = 0.0;
	}
	for (int count = 0; count < delayX + 2; count++) {
		kPlateC->aXL[count] = 0.0;
		kPlateC->aXR[count] = 0.0;
	}
	for (int count = 0; count < delayY + 2; count++) {
		kPlateC->aYL[count] = 0.0;
		kPlateC->aYR[count] = 0.0;
	}

	for (int count = 0; count < earlyA + 2; count++) {
		kPlateC->eAL[count] = 0.0;
		kPlateC->eAR[count] = 0.0;
	}
	for (int count = 0; count < earlyB + 2; count++) {
		kPlateC->eBL[count] = 0.0;
		kPlateC->eBR[count] = 0.0;
	}
	for (int count = 0; count < earlyC + 2; count++) {
		kPlateC->eCL[count] = 0.0;
		kPlateC->eCR[count] = 0.0;
	}
	for (int count = 0; count < earlyD + 2; count++) {
		kPlateC->eDL[count] = 0.0;
		kPlateC->eDR[count] = 0.0;
	}
	for (int count = 0; count < earlyE + 2; count++) {
		kPlateC->eEL[count] = 0.0;
		kPlateC->eER[count] = 0.0;
	}
	for (int count = 0; count < earlyF + 2; count++) {
		kPlateC->eFL[count] = 0.0;
		kPlateC->eFR[count] = 0.0;
	}
	for (int count = 0; count < earlyG + 2; count++) {
		kPlateC->eGL[count] = 0.0;
		kPlateC->eGR[count] = 0.0;
	}
	for (int count = 0; count < earlyH + 2; count++) {
		kPlateC->eHL[count] = 0.0;
		kPlateC->eHR[count] = 0.0;
	}
	for (int count = 0; count < earlyI + 2; count++) {
		kPlateC->eIL[count] = 0.0;
		kPlateC->eIR[count] = 0.0;
	}

	for (int count = 0; count < predelay + 2; count++) {
		kPlateC->aZL[count] = 0.0;
		kPlateC->aZR[count] = 0.0;
	}

	kPlateC->feedbackAL = 0.0;
	kPlateC->feedbackBL = 0.0;
	kPlateC->feedbackCL = 0.0;
	kPlateC->feedbackDL = 0.0;
	kPlateC->feedbackEL = 0.0;

	kPlateC->previousAL = 0.0;
	kPlateC->previousBL = 0.0;
	kPlateC->previousCL = 0.0;
	kPlateC->previousDL = 0.0;
	kPlateC->previousEL = 0.0;

	kPlateC->feedbackER = 0.0;
	kPlateC->feedbackJR = 0.0;
	kPlateC->feedbackOR = 0.0;
	kPlateC->feedbackTR = 0.0;
	kPlateC->feedbackYR = 0.0;

	kPlateC->previousAR = 0.0;
	kPlateC->previousBR = 0.0;
	kPlateC->previousCR = 0.0;
	kPlateC->previousDR = 0.0;
	kPlateC->previousER = 0.0;

	kPlateC->prevMulchDL = 0.0;
	kPlateC->prevMulchDR = 0.0;
	kPlateC->prevMulchEL = 0.0;
	kPlateC->prevMulchER = 0.0;

	kPlateC->prevOutDL = 0.0;
	kPlateC->prevOutDR = 0.0;
	kPlateC->prevOutEL = 0.0;
	kPlateC->prevOutER = 0.0;

	kPlateC->prevInDL = 0.0;
	kPlateC->prevInDR = 0.0;
	kPlateC->prevInEL = 0.0;
	kPlateC->prevInER = 0.0;

	for (int count = 0; count < 6; count++) {
		kPlateC->lastRefL[count] = 0.0;
		kPlateC->lastRefR[count] = 0.0;
	}

	kPlateC->earlyAL = 1;
	kPlateC->earlyBL = 1;
	kPlateC->earlyCL = 1;
	kPlateC->earlyDL = 1;
	kPlateC->earlyEL = 1;
	kPlateC->earlyFL = 1;
	kPlateC->earlyGL = 1;
	kPlateC->earlyHL = 1;
	kPlateC->earlyIL = 1;

	kPlateC->earlyAR = 1;
	kPlateC->earlyBR = 1;
	kPlateC->earlyCR = 1;
	kPlateC->earlyDR = 1;
	kPlateC->earlyER = 1;
	kPlateC->earlyFR = 1;
	kPlateC->earlyGR = 1;
	kPlateC->earlyHR = 1;
	kPlateC->earlyIR = 1;

	kPlateC->countAL = 1;
	kPlateC->countBL = 1;
	kPlateC->countCL = 1;
	kPlateC->countDL = 1;
	kPlateC->countEL = 1;
	kPlateC->countFL = 1;
	kPlateC->countGL = 1;
	kPlateC->countHL = 1;
	kPlateC->countIL = 1;
	kPlateC->countJL = 1;
	kPlateC->countKL = 1;
	kPlateC->countLL = 1;
	kPlateC->countML = 1;
	kPlateC->countNL = 1;
	kPlateC->countOL = 1;
	kPlateC->countPL = 1;
	kPlateC->countQL = 1;
	kPlateC->countRL = 1;
	kPlateC->countSL = 1;
	kPlateC->countTL = 1;
	kPlateC->countUL = 1;
	kPlateC->countVL = 1;
	kPlateC->countWL = 1;
	kPlateC->countXL = 1;
	kPlateC->countYL = 1;

	kPlateC->countAR = 1;
	kPlateC->countBR = 1;
	kPlateC->countCR = 1;
	kPlateC->countDR = 1;
	kPlateC->countER = 1;
	kPlateC->countFR = 1;
	kPlateC->countGR = 1;
	kPlateC->countHR = 1;
	kPlateC->countIR = 1;
	kPlateC->countJR = 1;
	kPlateC->countKR = 1;
	kPlateC->countLR = 1;
	kPlateC->countMR = 1;
	kPlateC->countNR = 1;
	kPlateC->countOR = 1;
	kPlateC->countPR = 1;
	kPlateC->countQR = 1;
	kPlateC->countRR = 1;
	kPlateC->countSR = 1;
	kPlateC->countTR = 1;
	kPlateC->countUR = 1;
	kPlateC->countVR = 1;
	kPlateC->countWR = 1;
	kPlateC->countXR = 1;
	kPlateC->countYR = 1;

	kPlateC->countZ = 1;

	kPlateC->cycle = 0;

	for (int x = 0; x < fix_total; x++) {
		kPlateC->fixA[x] = 0.0;
		kPlateC->fixB[x] = 0.0;
		kPlateC->fixC[x] = 0.0;
		kPlateC->fixD[x] = 0.0;
	}
	// from ZBandpass, so I can use enums with it

	kPlateC->fpdL = 1.0;
	while (kPlateC->fpdL < 16386) kPlateC->fpdL = rand() * UINT32_MAX;
	kPlateC->fpdR = 1.0;
	while (kPlateC->fpdR < 16386) kPlateC->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	KPlateC* kPlateC = (KPlateC*) instance;

	const float* in1 = kPlateC->input[0];
	const float* in2 = kPlateC->input[1];
	float* out1 = kPlateC->output[0];
	float* out2 = kPlateC->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= kPlateC->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (kPlateC->cycle > cycleEnd - 1) kPlateC->cycle = cycleEnd - 1; // sanity check

	double downRate = kPlateC->sampleRate / cycleEnd;
	// we now shift gears between 44.1k and 48k so our tone is the same, slight changes in delay times

	double inputPad = *kPlateC->inputPad;
	double regen = ((*kPlateC->damping / 10.0) * 0.415) + 0.1;
	regen = (regen * 0.0001) + 0.00024;
	double iirAmount = ((*kPlateC->lowCut / 3.0) * 0.3) + 0.04;
	iirAmount = (iirAmount * 1000.0) / downRate;
	double earlyVolume = *kPlateC->predelay; // predelay to a half-second
	int adjPredelay = (downRate * earlyVolume);
	double wet = *kPlateC->wetness * 2.0;
	double dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	// this reverb makes 50% full dry AND full wet, not crossfaded.
	// that's so it can be on submixes without cutting back dry channel when adjusted:
	// unless you go super heavy, you are only adjusting the added verb loudness.

	kPlateC->fixA[fix_freq] = 21.0 / downRate;
	kPlateC->fixA[fix_reso] = 0.0020736;
	kPlateC->fixD[fix_freq] = 25.0 / downRate;
	kPlateC->fixD[fix_reso] = 0.0016384;
	kPlateC->fixB[fix_freq] = (kPlateC->fixA[fix_freq] + kPlateC->fixA[fix_freq] + kPlateC->fixD[fix_freq]) / 3.0;
	kPlateC->fixB[fix_reso] = 0.0019285;
	kPlateC->fixC[fix_freq] = (kPlateC->fixA[fix_freq] + kPlateC->fixD[fix_freq] + kPlateC->fixD[fix_freq]) / 3.0;
	kPlateC->fixC[fix_reso] = 0.0017834;

	double K = tan(M_PI * kPlateC->fixA[fix_freq]);
	double norm = 1.0 / (1.0 + K / kPlateC->fixA[fix_reso] + K * K);
	kPlateC->fixA[fix_a0] = K / kPlateC->fixA[fix_reso] * norm;
	kPlateC->fixA[fix_a1] = 0.0;
	kPlateC->fixA[fix_a2] = -kPlateC->fixA[fix_a0];
	kPlateC->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateC->fixA[fix_b2] = (1.0 - K / kPlateC->fixA[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateC->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateC->fixB[fix_reso] + K * K);
	kPlateC->fixB[fix_a0] = K / kPlateC->fixB[fix_reso] * norm;
	kPlateC->fixB[fix_a1] = 0.0;
	kPlateC->fixB[fix_a2] = -kPlateC->fixB[fix_a0];
	kPlateC->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateC->fixB[fix_b2] = (1.0 - K / kPlateC->fixB[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateC->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateC->fixC[fix_reso] + K * K);
	kPlateC->fixC[fix_a0] = K / kPlateC->fixC[fix_reso] * norm;
	kPlateC->fixC[fix_a1] = 0.0;
	kPlateC->fixC[fix_a2] = -kPlateC->fixC[fix_a0];
	kPlateC->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateC->fixC[fix_b2] = (1.0 - K / kPlateC->fixC[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateC->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateC->fixD[fix_reso] + K * K);
	kPlateC->fixD[fix_a0] = K / kPlateC->fixD[fix_reso] * norm;
	kPlateC->fixD[fix_a1] = 0.0;
	kPlateC->fixD[fix_a2] = -kPlateC->fixD[fix_a0];
	kPlateC->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateC->fixD[fix_b2] = (1.0 - K / kPlateC->fixD[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = kPlateC->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = kPlateC->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		kPlateC->cycle++;
		if (kPlateC->cycle == cycleEnd) { // hit the end point and we do a reverb sample
			if (inputPad < 1.0) {
				inputSampleL *= inputPad;
				inputSampleR *= inputPad;
			}
			double outSample;
			outSample = (inputSampleL + kPlateC->prevInDL) * 0.5;
			kPlateC->prevInDL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateC->prevInDR) * 0.5;
			kPlateC->prevInDR = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			kPlateC->iirAL = (kPlateC->iirAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateC->iirAL;
			kPlateC->iirAR = (kPlateC->iirAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateC->iirAR;
			// 600hz highpass on input

			inputSampleL *= 0.25;
			inputSampleR *= 0.25;
			if (kPlateC->gainIn < 0.0078125) kPlateC->gainIn = 0.0078125;
			if (kPlateC->gainIn > 1.0) kPlateC->gainIn = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateC->gainIn;
			inputSampleR *= kPlateC->gainIn;
			kPlateC->gainIn += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateC->gainIn += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			inputSampleL *= 2.0;
			inputSampleR *= 2.0;
			// curve! To get a compressed effect that matches a certain other plugin
			// that is too overprocessed for its own good :)

			// begin overdrive
			if (inputSampleL > 1.4137166941154) inputSampleL = 1.4137166941154;
			if (inputSampleL < -1.4137166941154) inputSampleL = -1.4137166941154;
			if (inputSampleL > 0.0) inputSampleL = (inputSampleL / 2.0) * (2.8274333882308 - inputSampleL);
			else inputSampleL = -(inputSampleL / -2.0) * (2.8274333882308 + inputSampleL);
			// BigFastSin channel stage
			if (inputSampleR > 1.4137166941154) inputSampleR = 1.4137166941154;
			if (inputSampleR < -1.4137166941154) inputSampleR = -1.4137166941154;
			if (inputSampleR > 0.0) inputSampleR = (inputSampleR / 2.0) * (2.8274333882308 - inputSampleR);
			else inputSampleR = -(inputSampleR / -2.0) * (2.8274333882308 + inputSampleR);
			// end overdrive

			kPlateC->iirBL = (kPlateC->iirBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateC->iirBL;
			kPlateC->iirBR = (kPlateC->iirBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateC->iirBR;
			// 600hz highpass on input

			outSample = (inputSampleL + kPlateC->prevInEL) * 0.5;
			kPlateC->prevInEL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateC->prevInER) * 0.5;
			kPlateC->prevInER = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			// begin allpasses
			double oeAL = inputSampleL - (kPlateC->eAL[(kPlateC->earlyAL + 1) - ((kPlateC->earlyAL + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeBL = inputSampleL - (kPlateC->eBL[(kPlateC->earlyBL + 1) - ((kPlateC->earlyBL + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeCL = inputSampleL - (kPlateC->eCL[(kPlateC->earlyCL + 1) - ((kPlateC->earlyCL + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeCR = inputSampleR - (kPlateC->eCR[(kPlateC->earlyCR + 1) - ((kPlateC->earlyCR + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeFR = inputSampleR - (kPlateC->eFR[(kPlateC->earlyFR + 1) - ((kPlateC->earlyFR + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeIR = inputSampleR - (kPlateC->eIR[(kPlateC->earlyIR + 1) - ((kPlateC->earlyIR + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);

			kPlateC->eAL[kPlateC->earlyAL] = oeAL;
			oeAL *= 0.5;
			kPlateC->eBL[kPlateC->earlyBL] = oeBL;
			oeBL *= 0.5;
			kPlateC->eCL[kPlateC->earlyCL] = oeCL;
			oeCL *= 0.5;
			kPlateC->eCR[kPlateC->earlyCR] = oeCR;
			oeCR *= 0.5;
			kPlateC->eFR[kPlateC->earlyFR] = oeFR;
			oeFR *= 0.5;
			kPlateC->eIR[kPlateC->earlyIR] = oeIR;
			oeIR *= 0.5;

			kPlateC->earlyAL++;
			if (kPlateC->earlyAL < 0 || kPlateC->earlyAL > earlyA) kPlateC->earlyAL = 0;
			kPlateC->earlyBL++;
			if (kPlateC->earlyBL < 0 || kPlateC->earlyBL > earlyB) kPlateC->earlyBL = 0;
			kPlateC->earlyCL++;
			if (kPlateC->earlyCL < 0 || kPlateC->earlyCL > earlyC) kPlateC->earlyCL = 0;
			kPlateC->earlyCR++;
			if (kPlateC->earlyCR < 0 || kPlateC->earlyCR > earlyC) kPlateC->earlyCR = 0;
			kPlateC->earlyFR++;
			if (kPlateC->earlyFR < 0 || kPlateC->earlyFR > earlyF) kPlateC->earlyFR = 0;
			kPlateC->earlyIR++;
			if (kPlateC->earlyIR < 0 || kPlateC->earlyIR > earlyI) kPlateC->earlyIR = 0;

			oeAL += kPlateC->eAL[kPlateC->earlyAL - ((kPlateC->earlyAL > earlyA) ? earlyA + 1 : 0)];
			oeBL += kPlateC->eBL[kPlateC->earlyBL - ((kPlateC->earlyBL > earlyB) ? earlyB + 1 : 0)];
			oeCL += kPlateC->eCL[kPlateC->earlyCL - ((kPlateC->earlyCL > earlyC) ? earlyC + 1 : 0)];
			oeCR += kPlateC->eCR[kPlateC->earlyCR - ((kPlateC->earlyCR > earlyC) ? earlyC + 1 : 0)];
			oeFR += kPlateC->eFR[kPlateC->earlyFR - ((kPlateC->earlyFR > earlyF) ? earlyF + 1 : 0)];
			oeIR += kPlateC->eIR[kPlateC->earlyIR - ((kPlateC->earlyIR > earlyI) ? earlyI + 1 : 0)];

			double oeDL = ((oeBL + oeCL) - oeAL) - (kPlateC->eDL[(kPlateC->earlyDL + 1) - ((kPlateC->earlyDL + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeEL = ((oeAL + oeCL) - oeBL) - (kPlateC->eEL[(kPlateC->earlyEL + 1) - ((kPlateC->earlyEL + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeFL = ((oeAL + oeBL) - oeCL) - (kPlateC->eFL[(kPlateC->earlyFL + 1) - ((kPlateC->earlyFL + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeBR = ((oeFR + oeIR) - oeCR) - (kPlateC->eBR[(kPlateC->earlyBR + 1) - ((kPlateC->earlyBR + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeER = ((oeCR + oeIR) - oeFR) - (kPlateC->eER[(kPlateC->earlyER + 1) - ((kPlateC->earlyER + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeHR = ((oeCR + oeFR) - oeIR) - (kPlateC->eHR[(kPlateC->earlyHR + 1) - ((kPlateC->earlyHR + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);

			kPlateC->eDL[kPlateC->earlyDL] = oeDL;
			oeDL *= 0.5;
			kPlateC->eEL[kPlateC->earlyEL] = oeEL;
			oeEL *= 0.5;
			kPlateC->eFL[kPlateC->earlyFL] = oeFL;
			oeFL *= 0.5;
			kPlateC->eBR[kPlateC->earlyBR] = oeBR;
			oeBR *= 0.5;
			kPlateC->eER[kPlateC->earlyER] = oeER;
			oeER *= 0.5;
			kPlateC->eHR[kPlateC->earlyHR] = oeHR;
			oeHR *= 0.5;

			kPlateC->earlyDL++;
			if (kPlateC->earlyDL < 0 || kPlateC->earlyDL > earlyD) kPlateC->earlyDL = 0;
			kPlateC->earlyEL++;
			if (kPlateC->earlyEL < 0 || kPlateC->earlyEL > earlyE) kPlateC->earlyEL = 0;
			kPlateC->earlyFL++;
			if (kPlateC->earlyFL < 0 || kPlateC->earlyFL > earlyF) kPlateC->earlyFL = 0;
			kPlateC->earlyBR++;
			if (kPlateC->earlyBR < 0 || kPlateC->earlyBR > earlyB) kPlateC->earlyBR = 0;
			kPlateC->earlyER++;
			if (kPlateC->earlyER < 0 || kPlateC->earlyER > earlyE) kPlateC->earlyER = 0;
			kPlateC->earlyHR++;
			if (kPlateC->earlyHR < 0 || kPlateC->earlyHR > earlyH) kPlateC->earlyHR = 0;

			oeDL += kPlateC->eDL[kPlateC->earlyDL - ((kPlateC->earlyDL > earlyD) ? earlyD + 1 : 0)];
			oeEL += kPlateC->eEL[kPlateC->earlyEL - ((kPlateC->earlyEL > earlyE) ? earlyE + 1 : 0)];
			oeFL += kPlateC->eFL[kPlateC->earlyFL - ((kPlateC->earlyFL > earlyF) ? earlyF + 1 : 0)];
			oeBR += kPlateC->eBR[kPlateC->earlyBR - ((kPlateC->earlyBR > earlyB) ? earlyB + 1 : 0)];
			oeER += kPlateC->eER[kPlateC->earlyER - ((kPlateC->earlyER > earlyE) ? earlyE + 1 : 0)];
			oeHR += kPlateC->eHR[kPlateC->earlyHR - ((kPlateC->earlyHR > earlyH) ? earlyH + 1 : 0)];

			double oeGL = ((oeEL + oeFL) - oeDL) - (kPlateC->eGL[(kPlateC->earlyGL + 1) - ((kPlateC->earlyGL + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);
			double oeHL = ((oeDL + oeFL) - oeEL) - (kPlateC->eHL[(kPlateC->earlyHL + 1) - ((kPlateC->earlyHL + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);
			double oeIL = ((oeDL + oeEL) - oeFL) - (kPlateC->eIL[(kPlateC->earlyIL + 1) - ((kPlateC->earlyIL + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);
			double oeAR = ((oeER + oeHR) - oeBR) - (kPlateC->eAR[(kPlateC->earlyAR + 1) - ((kPlateC->earlyAR + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeDR = ((oeBR + oeHR) - oeER) - (kPlateC->eDR[(kPlateC->earlyDR + 1) - ((kPlateC->earlyDR + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeGR = ((oeBR + oeER) - oeHR) - (kPlateC->eGR[(kPlateC->earlyGR + 1) - ((kPlateC->earlyGR + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);

			kPlateC->eGL[kPlateC->earlyGL] = oeGL;
			oeGL *= 0.5;
			kPlateC->eHL[kPlateC->earlyHL] = oeHL;
			oeHL *= 0.5;
			kPlateC->eIL[kPlateC->earlyIL] = oeIL;
			oeIL *= 0.5;
			kPlateC->eAR[kPlateC->earlyAR] = oeAR;
			oeAR *= 0.5;
			kPlateC->eDR[kPlateC->earlyDR] = oeDR;
			oeDR *= 0.5;
			kPlateC->eGR[kPlateC->earlyGR] = oeGR;
			oeGR *= 0.5;

			kPlateC->earlyGL++;
			if (kPlateC->earlyGL < 0 || kPlateC->earlyGL > earlyG) kPlateC->earlyGL = 0;
			kPlateC->earlyHL++;
			if (kPlateC->earlyHL < 0 || kPlateC->earlyHL > earlyH) kPlateC->earlyHL = 0;
			kPlateC->earlyIL++;
			if (kPlateC->earlyIL < 0 || kPlateC->earlyIL > earlyI) kPlateC->earlyIL = 0;
			kPlateC->earlyAR++;
			if (kPlateC->earlyAR < 0 || kPlateC->earlyAR > earlyA) kPlateC->earlyAR = 0;
			kPlateC->earlyDR++;
			if (kPlateC->earlyDR < 0 || kPlateC->earlyDR > earlyD) kPlateC->earlyDR = 0;
			kPlateC->earlyGR++;
			if (kPlateC->earlyGR < 0 || kPlateC->earlyGR > earlyG) kPlateC->earlyGR = 0;

			oeGL += kPlateC->eGL[kPlateC->earlyGL - ((kPlateC->earlyGL > earlyG) ? earlyG + 1 : 0)];
			oeHL += kPlateC->eHL[kPlateC->earlyHL - ((kPlateC->earlyHL > earlyH) ? earlyH + 1 : 0)];
			oeIL += kPlateC->eIL[kPlateC->earlyIL - ((kPlateC->earlyIL > earlyI) ? earlyI + 1 : 0)];
			oeAR += kPlateC->eAR[kPlateC->earlyAR - ((kPlateC->earlyAR > earlyA) ? earlyA + 1 : 0)];
			oeDR += kPlateC->eDR[kPlateC->earlyDR - ((kPlateC->earlyDR > earlyD) ? earlyD + 1 : 0)];
			oeGR += kPlateC->eGR[kPlateC->earlyGR - ((kPlateC->earlyGR > earlyG) ? earlyG + 1 : 0)];

			// allpasses predelay
			kPlateC->aZL[kPlateC->countZ] = (oeGL + oeHL + oeIL) * 0.25;
			kPlateC->aZR[kPlateC->countZ] = (oeAR + oeDR + oeGR) * 0.25;
			kPlateC->countZ++;
			if (kPlateC->countZ < 0 || kPlateC->countZ > adjPredelay) kPlateC->countZ = 0;
			inputSampleL = kPlateC->aZL[kPlateC->countZ - ((kPlateC->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			inputSampleR = kPlateC->aZR[kPlateC->countZ - ((kPlateC->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			// end allpasses

			kPlateC->aAL[kPlateC->countAL] = inputSampleL + (kPlateC->feedbackAL * regen);
			kPlateC->aBL[kPlateC->countBL] = inputSampleL + (kPlateC->feedbackBL * regen);
			kPlateC->aCL[kPlateC->countCL] = inputSampleL + (kPlateC->feedbackCL * regen);
			kPlateC->aDL[kPlateC->countDL] = inputSampleL + (kPlateC->feedbackDL * regen);
			kPlateC->aEL[kPlateC->countEL] = inputSampleL + (kPlateC->feedbackEL * regen);

			kPlateC->aER[kPlateC->countER] = inputSampleR + (kPlateC->feedbackER * regen);
			kPlateC->aJR[kPlateC->countJR] = inputSampleR + (kPlateC->feedbackJR * regen);
			kPlateC->aOR[kPlateC->countOR] = inputSampleR + (kPlateC->feedbackOR * regen);
			kPlateC->aTR[kPlateC->countTR] = inputSampleR + (kPlateC->feedbackTR * regen);
			kPlateC->aYR[kPlateC->countYR] = inputSampleR + (kPlateC->feedbackYR * regen);

			kPlateC->countAL++;
			if (kPlateC->countAL < 0 || kPlateC->countAL > delayA) kPlateC->countAL = 0;
			kPlateC->countBL++;
			if (kPlateC->countBL < 0 || kPlateC->countBL > delayB) kPlateC->countBL = 0;
			kPlateC->countCL++;
			if (kPlateC->countCL < 0 || kPlateC->countCL > delayC) kPlateC->countCL = 0;
			kPlateC->countDL++;
			if (kPlateC->countDL < 0 || kPlateC->countDL > delayD) kPlateC->countDL = 0;
			kPlateC->countEL++;
			if (kPlateC->countEL < 0 || kPlateC->countEL > delayE) kPlateC->countEL = 0;

			kPlateC->countER++;
			if (kPlateC->countER < 0 || kPlateC->countER > delayE) kPlateC->countER = 0;
			kPlateC->countJR++;
			if (kPlateC->countJR < 0 || kPlateC->countJR > delayJ) kPlateC->countJR = 0;
			kPlateC->countOR++;
			if (kPlateC->countOR < 0 || kPlateC->countOR > delayO) kPlateC->countOR = 0;
			kPlateC->countTR++;
			if (kPlateC->countTR < 0 || kPlateC->countTR > delayT) kPlateC->countTR = 0;
			kPlateC->countYR++;
			if (kPlateC->countYR < 0 || kPlateC->countYR > delayY) kPlateC->countYR = 0;

			double outAL = kPlateC->aAL[kPlateC->countAL - ((kPlateC->countAL > delayA) ? delayA + 1 : 0)];
			double outBL = kPlateC->aBL[kPlateC->countBL - ((kPlateC->countBL > delayB) ? delayB + 1 : 0)];
			double outCL = kPlateC->aCL[kPlateC->countCL - ((kPlateC->countCL > delayC) ? delayC + 1 : 0)];
			double outDL = kPlateC->aDL[kPlateC->countDL - ((kPlateC->countDL > delayD) ? delayD + 1 : 0)];
			double outEL = kPlateC->aEL[kPlateC->countEL - ((kPlateC->countEL > delayE) ? delayE + 1 : 0)];

			double outER = kPlateC->aER[kPlateC->countER - ((kPlateC->countER > delayE) ? delayE + 1 : 0)];
			double outJR = kPlateC->aJR[kPlateC->countJR - ((kPlateC->countJR > delayJ) ? delayJ + 1 : 0)];
			double outOR = kPlateC->aOR[kPlateC->countOR - ((kPlateC->countOR > delayO) ? delayO + 1 : 0)];
			double outTR = kPlateC->aTR[kPlateC->countTR - ((kPlateC->countTR > delayT) ? delayT + 1 : 0)];
			double outYR = kPlateC->aYR[kPlateC->countYR - ((kPlateC->countYR > delayY) ? delayY + 1 : 0)];

			//-------- one

			outSample = (outAL * kPlateC->fixA[fix_a0]) + kPlateC->fixA[fix_sL1];
			kPlateC->fixA[fix_sL1] = (outAL * kPlateC->fixA[fix_a1]) - (outSample * kPlateC->fixA[fix_b1]) + kPlateC->fixA[fix_sL2];
			kPlateC->fixA[fix_sL2] = (outAL * kPlateC->fixA[fix_a2]) - (outSample * kPlateC->fixA[fix_b2]);
			outAL = outSample; // fixed biquad

			outSample = (outER * kPlateC->fixA[fix_a0]) + kPlateC->fixA[fix_sR1];
			kPlateC->fixA[fix_sR1] = (outER * kPlateC->fixA[fix_a1]) - (outSample * kPlateC->fixA[fix_b1]) + kPlateC->fixA[fix_sR2];
			kPlateC->fixA[fix_sR2] = (outER * kPlateC->fixA[fix_a2]) - (outSample * kPlateC->fixA[fix_b2]);
			outER = outSample; // fixed biquad

			//-------- filtered (one path in five, feeding the rest of the matrix

			kPlateC->aFL[kPlateC->countFL] = ((outAL * 3.0) - ((outBL + outCL + outDL + outEL) * 2.0));
			kPlateC->aGL[kPlateC->countGL] = ((outBL * 3.0) - ((outAL + outCL + outDL + outEL) * 2.0));
			kPlateC->aHL[kPlateC->countHL] = ((outCL * 3.0) - ((outAL + outBL + outDL + outEL) * 2.0));
			kPlateC->aIL[kPlateC->countIL] = ((outDL * 3.0) - ((outAL + outBL + outCL + outEL) * 2.0));
			kPlateC->aJL[kPlateC->countJL] = ((outEL * 3.0) - ((outAL + outBL + outCL + outDL) * 2.0));

			kPlateC->aDR[kPlateC->countDR] = ((outER * 3.0) - ((outJR + outOR + outTR + outYR) * 2.0));
			kPlateC->aIR[kPlateC->countIR] = ((outJR * 3.0) - ((outER + outOR + outTR + outYR) * 2.0));
			kPlateC->aNR[kPlateC->countNR] = ((outOR * 3.0) - ((outER + outJR + outTR + outYR) * 2.0));
			kPlateC->aSR[kPlateC->countSR] = ((outTR * 3.0) - ((outER + outJR + outOR + outYR) * 2.0));
			kPlateC->aXR[kPlateC->countXR] = ((outYR * 3.0) - ((outER + outJR + outOR + outTR) * 2.0));

			kPlateC->countFL++;
			if (kPlateC->countFL < 0 || kPlateC->countFL > delayF) kPlateC->countFL = 0;
			kPlateC->countGL++;
			if (kPlateC->countGL < 0 || kPlateC->countGL > delayG) kPlateC->countGL = 0;
			kPlateC->countHL++;
			if (kPlateC->countHL < 0 || kPlateC->countHL > delayH) kPlateC->countHL = 0;
			kPlateC->countIL++;
			if (kPlateC->countIL < 0 || kPlateC->countIL > delayI) kPlateC->countIL = 0;
			kPlateC->countJL++;
			if (kPlateC->countJL < 0 || kPlateC->countJL > delayJ) kPlateC->countJL = 0;

			kPlateC->countDR++;
			if (kPlateC->countDR < 0 || kPlateC->countDR > delayD) kPlateC->countDR = 0;
			kPlateC->countIR++;
			if (kPlateC->countIR < 0 || kPlateC->countIR > delayI) kPlateC->countIR = 0;
			kPlateC->countNR++;
			if (kPlateC->countNR < 0 || kPlateC->countNR > delayN) kPlateC->countNR = 0;
			kPlateC->countSR++;
			if (kPlateC->countSR < 0 || kPlateC->countSR > delayS) kPlateC->countSR = 0;
			kPlateC->countXR++;
			if (kPlateC->countXR < 0 || kPlateC->countXR > delayX) kPlateC->countXR = 0;

			double outFL = kPlateC->aFL[kPlateC->countFL - ((kPlateC->countFL > delayF) ? delayF + 1 : 0)];
			double outGL = kPlateC->aGL[kPlateC->countGL - ((kPlateC->countGL > delayG) ? delayG + 1 : 0)];
			double outHL = kPlateC->aHL[kPlateC->countHL - ((kPlateC->countHL > delayH) ? delayH + 1 : 0)];
			double outIL = kPlateC->aIL[kPlateC->countIL - ((kPlateC->countIL > delayI) ? delayI + 1 : 0)];
			double outJL = kPlateC->aJL[kPlateC->countJL - ((kPlateC->countJL > delayJ) ? delayJ + 1 : 0)];

			double outDR = kPlateC->aDR[kPlateC->countDR - ((kPlateC->countDR > delayD) ? delayD + 1 : 0)];
			double outIR = kPlateC->aIR[kPlateC->countIR - ((kPlateC->countIR > delayI) ? delayI + 1 : 0)];
			double outNR = kPlateC->aNR[kPlateC->countNR - ((kPlateC->countNR > delayN) ? delayN + 1 : 0)];
			double outSR = kPlateC->aSR[kPlateC->countSR - ((kPlateC->countSR > delayS) ? delayS + 1 : 0)];
			double outXR = kPlateC->aXR[kPlateC->countXR - ((kPlateC->countXR > delayX) ? delayX + 1 : 0)];

			//-------- mulch

			outSample = (outFL * kPlateC->fixB[fix_a0]) + kPlateC->fixB[fix_sL1];
			kPlateC->fixB[fix_sL1] = (outFL * kPlateC->fixB[fix_a1]) - (outSample * kPlateC->fixB[fix_b1]) + kPlateC->fixB[fix_sL2];
			kPlateC->fixB[fix_sL2] = (outFL * kPlateC->fixB[fix_a2]) - (outSample * kPlateC->fixB[fix_b2]);
			outFL = outSample; // fixed biquad

			outSample = (outDR * kPlateC->fixB[fix_a0]) + kPlateC->fixB[fix_sR1];
			kPlateC->fixB[fix_sR1] = (outDR * kPlateC->fixB[fix_a1]) - (outSample * kPlateC->fixB[fix_b1]) + kPlateC->fixB[fix_sR2];
			kPlateC->fixB[fix_sR2] = (outDR * kPlateC->fixB[fix_a2]) - (outSample * kPlateC->fixB[fix_b2]);
			outDR = outSample; // fixed biquad

			//-------- two

			kPlateC->aKL[kPlateC->countKL] = ((outFL * 3.0) - ((outGL + outHL + outIL + outJL) * 2.0));
			kPlateC->aLL[kPlateC->countLL] = ((outGL * 3.0) - ((outFL + outHL + outIL + outJL) * 2.0));
			kPlateC->aML[kPlateC->countML] = ((outHL * 3.0) - ((outFL + outGL + outIL + outJL) * 2.0));
			kPlateC->aNL[kPlateC->countNL] = ((outIL * 3.0) - ((outFL + outGL + outHL + outJL) * 2.0));
			kPlateC->aOL[kPlateC->countOL] = ((outJL * 3.0) - ((outFL + outGL + outHL + outIL) * 2.0));

			kPlateC->aCR[kPlateC->countCR] = ((outDR * 3.0) - ((outIR + outNR + outSR + outXR) * 2.0));
			kPlateC->aHR[kPlateC->countHR] = ((outIR * 3.0) - ((outDR + outNR + outSR + outXR) * 2.0));
			kPlateC->aMR[kPlateC->countMR] = ((outNR * 3.0) - ((outDR + outIR + outSR + outXR) * 2.0));
			kPlateC->aRR[kPlateC->countRR] = ((outSR * 3.0) - ((outDR + outIR + outNR + outXR) * 2.0));
			kPlateC->aWR[kPlateC->countWR] = ((outXR * 3.0) - ((outDR + outIR + outNR + outSR) * 2.0));

			kPlateC->countKL++;
			if (kPlateC->countKL < 0 || kPlateC->countKL > delayK) kPlateC->countKL = 0;
			kPlateC->countLL++;
			if (kPlateC->countLL < 0 || kPlateC->countLL > delayL) kPlateC->countLL = 0;
			kPlateC->countML++;
			if (kPlateC->countML < 0 || kPlateC->countML > delayM) kPlateC->countML = 0;
			kPlateC->countNL++;
			if (kPlateC->countNL < 0 || kPlateC->countNL > delayN) kPlateC->countNL = 0;
			kPlateC->countOL++;
			if (kPlateC->countOL < 0 || kPlateC->countOL > delayO) kPlateC->countOL = 0;

			kPlateC->countCR++;
			if (kPlateC->countCR < 0 || kPlateC->countCR > delayC) kPlateC->countCR = 0;
			kPlateC->countHR++;
			if (kPlateC->countHR < 0 || kPlateC->countHR > delayH) kPlateC->countHR = 0;
			kPlateC->countMR++;
			if (kPlateC->countMR < 0 || kPlateC->countMR > delayM) kPlateC->countMR = 0;
			kPlateC->countRR++;
			if (kPlateC->countRR < 0 || kPlateC->countRR > delayR) kPlateC->countRR = 0;
			kPlateC->countWR++;
			if (kPlateC->countWR < 0 || kPlateC->countWR > delayW) kPlateC->countWR = 0;

			double outKL = kPlateC->aKL[kPlateC->countKL - ((kPlateC->countKL > delayK) ? delayK + 1 : 0)];
			double outLL = kPlateC->aLL[kPlateC->countLL - ((kPlateC->countLL > delayL) ? delayL + 1 : 0)];
			double outML = kPlateC->aML[kPlateC->countML - ((kPlateC->countML > delayM) ? delayM + 1 : 0)];
			double outNL = kPlateC->aNL[kPlateC->countNL - ((kPlateC->countNL > delayN) ? delayN + 1 : 0)];
			double outOL = kPlateC->aOL[kPlateC->countOL - ((kPlateC->countOL > delayO) ? delayO + 1 : 0)];

			double outCR = kPlateC->aCR[kPlateC->countCR - ((kPlateC->countCR > delayC) ? delayC + 1 : 0)];
			double outHR = kPlateC->aHR[kPlateC->countHR - ((kPlateC->countHR > delayH) ? delayH + 1 : 0)];
			double outMR = kPlateC->aMR[kPlateC->countMR - ((kPlateC->countMR > delayM) ? delayM + 1 : 0)];
			double outRR = kPlateC->aRR[kPlateC->countRR - ((kPlateC->countRR > delayR) ? delayR + 1 : 0)];
			double outWR = kPlateC->aWR[kPlateC->countWR - ((kPlateC->countWR > delayW) ? delayW + 1 : 0)];

			//-------- mulch

			outSample = (outKL * kPlateC->fixC[fix_a0]) + kPlateC->fixC[fix_sL1];
			kPlateC->fixC[fix_sL1] = (outKL * kPlateC->fixC[fix_a1]) - (outSample * kPlateC->fixC[fix_b1]) + kPlateC->fixC[fix_sL2];
			kPlateC->fixC[fix_sL2] = (outKL * kPlateC->fixC[fix_a2]) - (outSample * kPlateC->fixC[fix_b2]);
			outKL = outSample; // fixed biquad

			outSample = (outCR * kPlateC->fixC[fix_a0]) + kPlateC->fixC[fix_sR1];
			kPlateC->fixC[fix_sR1] = (outCR * kPlateC->fixC[fix_a1]) - (outSample * kPlateC->fixC[fix_b1]) + kPlateC->fixC[fix_sR2];
			kPlateC->fixC[fix_sR2] = (outCR * kPlateC->fixC[fix_a2]) - (outSample * kPlateC->fixC[fix_b2]);
			outCR = outSample; // fixed biquad

			//-------- three

			kPlateC->aPL[kPlateC->countPL] = ((outKL * 3.0) - ((outLL + outML + outNL + outOL) * 2.0));
			kPlateC->aQL[kPlateC->countQL] = ((outLL * 3.0) - ((outKL + outML + outNL + outOL) * 2.0));
			kPlateC->aRL[kPlateC->countRL] = ((outML * 3.0) - ((outKL + outLL + outNL + outOL) * 2.0));
			kPlateC->aSL[kPlateC->countSL] = ((outNL * 3.0) - ((outKL + outLL + outML + outOL) * 2.0));
			kPlateC->aTL[kPlateC->countTL] = ((outOL * 3.0) - ((outKL + outLL + outML + outNL) * 2.0));

			kPlateC->aBR[kPlateC->countBR] = ((outCR * 3.0) - ((outHR + outMR + outRR + outWR) * 2.0));
			kPlateC->aGR[kPlateC->countGR] = ((outHR * 3.0) - ((outCR + outMR + outRR + outWR) * 2.0));
			kPlateC->aLR[kPlateC->countLR] = ((outMR * 3.0) - ((outCR + outHR + outRR + outWR) * 2.0));
			kPlateC->aQR[kPlateC->countQR] = ((outRR * 3.0) - ((outCR + outHR + outMR + outWR) * 2.0));
			kPlateC->aVR[kPlateC->countVR] = ((outWR * 3.0) - ((outCR + outHR + outMR + outRR) * 2.0));

			kPlateC->countPL++;
			if (kPlateC->countPL < 0 || kPlateC->countPL > delayP) kPlateC->countPL = 0;
			kPlateC->countQL++;
			if (kPlateC->countQL < 0 || kPlateC->countQL > delayQ) kPlateC->countQL = 0;
			kPlateC->countRL++;
			if (kPlateC->countRL < 0 || kPlateC->countRL > delayR) kPlateC->countRL = 0;
			kPlateC->countSL++;
			if (kPlateC->countSL < 0 || kPlateC->countSL > delayS) kPlateC->countSL = 0;
			kPlateC->countTL++;
			if (kPlateC->countTL < 0 || kPlateC->countTL > delayT) kPlateC->countTL = 0;

			kPlateC->countBR++;
			if (kPlateC->countBR < 0 || kPlateC->countBR > delayB) kPlateC->countBR = 0;
			kPlateC->countGR++;
			if (kPlateC->countGR < 0 || kPlateC->countGR > delayG) kPlateC->countGR = 0;
			kPlateC->countLR++;
			if (kPlateC->countLR < 0 || kPlateC->countLR > delayL) kPlateC->countLR = 0;
			kPlateC->countQR++;
			if (kPlateC->countQR < 0 || kPlateC->countQR > delayQ) kPlateC->countQR = 0;
			kPlateC->countVR++;
			if (kPlateC->countVR < 0 || kPlateC->countVR > delayV) kPlateC->countVR = 0;

			double outPL = kPlateC->aPL[kPlateC->countPL - ((kPlateC->countPL > delayP) ? delayP + 1 : 0)];
			double outQL = kPlateC->aQL[kPlateC->countQL - ((kPlateC->countQL > delayQ) ? delayQ + 1 : 0)];
			double outRL = kPlateC->aRL[kPlateC->countRL - ((kPlateC->countRL > delayR) ? delayR + 1 : 0)];
			double outSL = kPlateC->aSL[kPlateC->countSL - ((kPlateC->countSL > delayS) ? delayS + 1 : 0)];
			double outTL = kPlateC->aTL[kPlateC->countTL - ((kPlateC->countTL > delayT) ? delayT + 1 : 0)];

			double outBR = kPlateC->aBR[kPlateC->countBR - ((kPlateC->countBR > delayB) ? delayB + 1 : 0)];
			double outGR = kPlateC->aGR[kPlateC->countGR - ((kPlateC->countGR > delayG) ? delayG + 1 : 0)];
			double outLR = kPlateC->aLR[kPlateC->countLR - ((kPlateC->countLR > delayL) ? delayL + 1 : 0)];
			double outQR = kPlateC->aQR[kPlateC->countQR - ((kPlateC->countQR > delayQ) ? delayQ + 1 : 0)];
			double outVR = kPlateC->aVR[kPlateC->countVR - ((kPlateC->countVR > delayV) ? delayV + 1 : 0)];

			//-------- mulch

			outSample = (outPL * kPlateC->fixD[fix_a0]) + kPlateC->fixD[fix_sL1];
			kPlateC->fixD[fix_sL1] = (outPL * kPlateC->fixD[fix_a1]) - (outSample * kPlateC->fixD[fix_b1]) + kPlateC->fixD[fix_sL2];
			kPlateC->fixD[fix_sL2] = (outPL * kPlateC->fixD[fix_a2]) - (outSample * kPlateC->fixD[fix_b2]);
			outPL = outSample; // fixed biquad

			outSample = (outBR * kPlateC->fixD[fix_a0]) + kPlateC->fixD[fix_sR1];
			kPlateC->fixD[fix_sR1] = (outBR * kPlateC->fixD[fix_a1]) - (outSample * kPlateC->fixD[fix_b1]) + kPlateC->fixD[fix_sR2];
			kPlateC->fixD[fix_sR2] = (outBR * kPlateC->fixD[fix_a2]) - (outSample * kPlateC->fixD[fix_b2]);
			outBR = outSample; // fixed biquad

			outSample = (outQL + kPlateC->prevMulchDL) * 0.5;
			kPlateC->prevMulchDL = outQL;
			outQL = outSample;
			outSample = (outGR + kPlateC->prevMulchDR) * 0.5;
			kPlateC->prevMulchDR = outGR;
			outGR = outSample;

			//-------- four

			kPlateC->aUL[kPlateC->countUL] = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0));
			kPlateC->aVL[kPlateC->countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
			kPlateC->aWL[kPlateC->countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
			kPlateC->aXL[kPlateC->countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
			kPlateC->aYL[kPlateC->countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));

			kPlateC->aAR[kPlateC->countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
			kPlateC->aFR[kPlateC->countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
			kPlateC->aKR[kPlateC->countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
			kPlateC->aPR[kPlateC->countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
			kPlateC->aUR[kPlateC->countUR] = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0));

			kPlateC->countUL++;
			if (kPlateC->countUL < 0 || kPlateC->countUL > delayU) kPlateC->countUL = 0;
			kPlateC->countVL++;
			if (kPlateC->countVL < 0 || kPlateC->countVL > delayV) kPlateC->countVL = 0;
			kPlateC->countWL++;
			if (kPlateC->countWL < 0 || kPlateC->countWL > delayW) kPlateC->countWL = 0;
			kPlateC->countXL++;
			if (kPlateC->countXL < 0 || kPlateC->countXL > delayX) kPlateC->countXL = 0;
			kPlateC->countYL++;
			if (kPlateC->countYL < 0 || kPlateC->countYL > delayY) kPlateC->countYL = 0;

			kPlateC->countAR++;
			if (kPlateC->countAR < 0 || kPlateC->countAR > delayA) kPlateC->countAR = 0;
			kPlateC->countFR++;
			if (kPlateC->countFR < 0 || kPlateC->countFR > delayF) kPlateC->countFR = 0;
			kPlateC->countKR++;
			if (kPlateC->countKR < 0 || kPlateC->countKR > delayK) kPlateC->countKR = 0;
			kPlateC->countPR++;
			if (kPlateC->countPR < 0 || kPlateC->countPR > delayP) kPlateC->countPR = 0;
			kPlateC->countUR++;
			if (kPlateC->countUR < 0 || kPlateC->countUR > delayU) kPlateC->countUR = 0;

			double outUL = kPlateC->aUL[kPlateC->countUL - ((kPlateC->countUL > delayU) ? delayU + 1 : 0)];
			double outVL = kPlateC->aVL[kPlateC->countVL - ((kPlateC->countVL > delayV) ? delayV + 1 : 0)];
			double outWL = kPlateC->aWL[kPlateC->countWL - ((kPlateC->countWL > delayW) ? delayW + 1 : 0)];
			double outXL = kPlateC->aXL[kPlateC->countXL - ((kPlateC->countXL > delayX) ? delayX + 1 : 0)];
			double outYL = kPlateC->aYL[kPlateC->countYL - ((kPlateC->countYL > delayY) ? delayY + 1 : 0)];

			double outAR = kPlateC->aAR[kPlateC->countAR - ((kPlateC->countAR > delayA) ? delayA + 1 : 0)];
			double outFR = kPlateC->aFR[kPlateC->countFR - ((kPlateC->countFR > delayF) ? delayF + 1 : 0)];
			double outKR = kPlateC->aKR[kPlateC->countKR - ((kPlateC->countKR > delayK) ? delayK + 1 : 0)];
			double outPR = kPlateC->aPR[kPlateC->countPR - ((kPlateC->countPR > delayP) ? delayP + 1 : 0)];
			double outUR = kPlateC->aUR[kPlateC->countUR - ((kPlateC->countUR > delayU) ? delayU + 1 : 0)];

			//-------- mulch

			outSample = (outVL + kPlateC->prevMulchEL) * 0.5;
			kPlateC->prevMulchEL = outVL;
			outVL = outSample;
			outSample = (outFR + kPlateC->prevMulchER) * 0.5;
			kPlateC->prevMulchER = outFR;
			outFR = outSample;

			//-------- five

			kPlateC->feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
			kPlateC->feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
			kPlateC->feedbackJR = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
			kPlateC->feedbackBL = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
			kPlateC->feedbackCL = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
			kPlateC->feedbackOR = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
			kPlateC->feedbackDL = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
			kPlateC->feedbackTR = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
			kPlateC->feedbackEL = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
			kPlateC->feedbackYR = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
			// which we need to feed back into the input again, a bit

			inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0016;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0016;
			// and take the final combined sum of outputs, corrected for Householder gain

			inputSampleL *= 0.5;
			inputSampleR *= 0.5;
			if (kPlateC->gainOutL < 0.0078125) kPlateC->gainOutL = 0.0078125;
			if (kPlateC->gainOutL > 1.0) kPlateC->gainOutL = 1.0;
			if (kPlateC->gainOutR < 0.0078125) kPlateC->gainOutR = 0.0078125;
			if (kPlateC->gainOutR > 1.0) kPlateC->gainOutR = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateC->gainOutL;
			inputSampleR *= kPlateC->gainOutR;
			kPlateC->gainOutL += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateC->gainOutR += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			inputSampleL *= 2.0;
			inputSampleR *= 2.0;
			// curve! To get a compressed effect that matches a certain other plugin
			// that is too overprocessed for its own good :)

			outSample = (inputSampleL + kPlateC->prevOutDL) * 0.5;
			kPlateC->prevOutDL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateC->prevOutDR) * 0.5;
			kPlateC->prevOutDR = inputSampleR;
			inputSampleR = outSample;
			outSample = (inputSampleL + kPlateC->prevOutEL) * 0.5;
			kPlateC->prevOutEL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateC->prevOutER) * 0.5;
			kPlateC->prevOutER = inputSampleR;
			inputSampleR = outSample;

			if (cycleEnd == 4) {
				kPlateC->lastRefL[0] = kPlateC->lastRefL[4]; // start from previous last
				kPlateC->lastRefL[2] = (kPlateC->lastRefL[0] + inputSampleL) / 2; // half
				kPlateC->lastRefL[1] = (kPlateC->lastRefL[0] + kPlateC->lastRefL[2]) / 2; // one quarter
				kPlateC->lastRefL[3] = (kPlateC->lastRefL[2] + inputSampleL) / 2; // three quarters
				kPlateC->lastRefL[4] = inputSampleL; // full
				kPlateC->lastRefR[0] = kPlateC->lastRefR[4]; // start from previous last
				kPlateC->lastRefR[2] = (kPlateC->lastRefR[0] + inputSampleR) / 2; // half
				kPlateC->lastRefR[1] = (kPlateC->lastRefR[0] + kPlateC->lastRefR[2]) / 2; // one quarter
				kPlateC->lastRefR[3] = (kPlateC->lastRefR[2] + inputSampleR) / 2; // three quarters
				kPlateC->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				kPlateC->lastRefL[0] = kPlateC->lastRefL[3]; // start from previous last
				kPlateC->lastRefL[2] = (kPlateC->lastRefL[0] + kPlateC->lastRefL[0] + inputSampleL) / 3; // third
				kPlateC->lastRefL[1] = (kPlateC->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				kPlateC->lastRefL[3] = inputSampleL; // full
				kPlateC->lastRefR[0] = kPlateC->lastRefR[3]; // start from previous last
				kPlateC->lastRefR[2] = (kPlateC->lastRefR[0] + kPlateC->lastRefR[0] + inputSampleR) / 3; // third
				kPlateC->lastRefR[1] = (kPlateC->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				kPlateC->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				kPlateC->lastRefL[0] = kPlateC->lastRefL[2]; // start from previous last
				kPlateC->lastRefL[1] = (kPlateC->lastRefL[0] + inputSampleL) / 2; // half
				kPlateC->lastRefL[2] = inputSampleL; // full
				kPlateC->lastRefR[0] = kPlateC->lastRefR[2]; // start from previous last
				kPlateC->lastRefR[1] = (kPlateC->lastRefR[0] + inputSampleR) / 2; // half
				kPlateC->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				kPlateC->lastRefL[0] = inputSampleL;
				kPlateC->lastRefR[0] = inputSampleR;
			}
			kPlateC->cycle = 0; // reset
			inputSampleL = kPlateC->lastRefL[kPlateC->cycle];
			inputSampleR = kPlateC->lastRefR[kPlateC->cycle];
		} else {
			inputSampleL = kPlateC->lastRefL[kPlateC->cycle];
			inputSampleR = kPlateC->lastRefR[kPlateC->cycle];
			// we are going through our references now
		}

		if (inputSampleL > 2.8) inputSampleL = 2.8;
		if (inputSampleL < -2.8) inputSampleL = -2.8;
		if (inputSampleR > 2.8) inputSampleR = 2.8;
		if (inputSampleR < -2.8) inputSampleR = -2.8; // clip BigFastArcSin harder
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL * 2.0) / (2.8274333882308 - inputSampleL);
		else inputSampleL = -(inputSampleL * -2.0) / (2.8274333882308 + inputSampleL);
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR * 2.0) / (2.8274333882308 - inputSampleR);
		else inputSampleR = -(inputSampleR * -2.0) / (2.8274333882308 + inputSampleR);
		// BigFastArcSin output stage

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
		kPlateC->fpdL ^= kPlateC->fpdL << 13;
		kPlateC->fpdL ^= kPlateC->fpdL >> 17;
		kPlateC->fpdL ^= kPlateC->fpdL << 5;
		inputSampleL += (((double) kPlateC->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		kPlateC->fpdR ^= kPlateC->fpdR << 13;
		kPlateC->fpdR ^= kPlateC->fpdR >> 17;
		kPlateC->fpdR ^= kPlateC->fpdR << 5;
		inputSampleR += (((double) kPlateC->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	KPLATEC_URI,
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
