#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define KPLATEB_URI "https://hannesbraun.net/ns/lv2/airwindows/kplateb"

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

#define earlyA 97
#define earlyB 1123
#define earlyC 379
#define earlyD 619
#define earlyE 1187
#define earlyF 757
#define earlyG 1151
#define earlyH 13
#define earlyI 1093
#define predelay 24010 // 78 ms, 732 seat theater
#define delayA 631
#define delayB 251
#define delayC 137
#define delayD 673
#define delayE 293
#define delayF 163
#define delayG 389
#define delayH 37
#define delayI 641
#define delayJ 661
#define delayK 17
#define delayL 719
#define delayM 787
#define delayN 31
#define delayO 83
#define delayP 823
#define delayQ 127
#define delayR 653
#define delayS 3
#define delayT 613
#define delayU 857
#define delayV 431
#define delayW 53
#define delayX 607
#define delayY 193 // 86 ms, 883 seat hall. Scarcity, 1 in 34934
// this must be included to set up the delay lines

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
	const float* predelayS;
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

	double prevMulchBL;
	double prevMulchBR;
	double prevMulchCL;
	double prevMulchCR;
	double prevMulchDL;
	double prevMulchDR;
	double prevMulchEL;
	double prevMulchER;

	double prevOutAL;
	double prevOutAR;
	double prevOutBL;
	double prevOutBR;
	double prevOutCL;
	double prevOutCR;

	double prevInCL;
	double prevInCR;
	double prevInDL;
	double prevInDR;
	double prevInEL;
	double prevInER;

	uint32_t fpdL;
	uint32_t fpdR;
} KPlateB;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	KPlateB* kPlateB = (KPlateB*) calloc(1, sizeof(KPlateB));
	kPlateB->sampleRate = rate;
	return (LV2_Handle) kPlateB;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	KPlateB* kPlateB = (KPlateB*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			kPlateB->input[0] = (const float*) data;
			break;
		case INPUT_R:
			kPlateB->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			kPlateB->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			kPlateB->output[1] = (float*) data;
			break;
		case INPUT_PAD:
			kPlateB->inputPad = (const float*) data;
			break;
		case DAMPING:
			kPlateB->damping = (const float*) data;
			break;
		case LOW_CUT:
			kPlateB->lowCut = (const float*) data;
			break;
		case PREDELAY:
			kPlateB->predelayS = (const float*) data;
			break;
		case WETNESS:
			kPlateB->wetness = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	KPlateB* kPlateB = (KPlateB*) instance;

	kPlateB->iirAL = 0.0;
	kPlateB->iirBL = 0.0;

	kPlateB->iirAR = 0.0;
	kPlateB->iirBR = 0.0;

	kPlateB->gainIn = kPlateB->gainOutL = kPlateB->gainOutR = 1.0;

	for (int count = 0; count < delayA + 2; count++) {
		kPlateB->aAL[count] = 0.0;
		kPlateB->aAR[count] = 0.0;
	}
	for (int count = 0; count < delayB + 2; count++) {
		kPlateB->aBL[count] = 0.0;
		kPlateB->aBR[count] = 0.0;
	}
	for (int count = 0; count < delayC + 2; count++) {
		kPlateB->aCL[count] = 0.0;
		kPlateB->aCR[count] = 0.0;
	}
	for (int count = 0; count < delayD + 2; count++) {
		kPlateB->aDL[count] = 0.0;
		kPlateB->aDR[count] = 0.0;
	}
	for (int count = 0; count < delayE + 2; count++) {
		kPlateB->aEL[count] = 0.0;
		kPlateB->aER[count] = 0.0;
	}
	for (int count = 0; count < delayF + 2; count++) {
		kPlateB->aFL[count] = 0.0;
		kPlateB->aFR[count] = 0.0;
	}
	for (int count = 0; count < delayG + 2; count++) {
		kPlateB->aGL[count] = 0.0;
		kPlateB->aGR[count] = 0.0;
	}
	for (int count = 0; count < delayH + 2; count++) {
		kPlateB->aHL[count] = 0.0;
		kPlateB->aHR[count] = 0.0;
	}
	for (int count = 0; count < delayI + 2; count++) {
		kPlateB->aIL[count] = 0.0;
		kPlateB->aIR[count] = 0.0;
	}
	for (int count = 0; count < delayJ + 2; count++) {
		kPlateB->aJL[count] = 0.0;
		kPlateB->aJR[count] = 0.0;
	}
	for (int count = 0; count < delayK + 2; count++) {
		kPlateB->aKL[count] = 0.0;
		kPlateB->aKR[count] = 0.0;
	}
	for (int count = 0; count < delayL + 2; count++) {
		kPlateB->aLL[count] = 0.0;
		kPlateB->aLR[count] = 0.0;
	}
	for (int count = 0; count < delayM + 2; count++) {
		kPlateB->aML[count] = 0.0;
		kPlateB->aMR[count] = 0.0;
	}
	for (int count = 0; count < delayN + 2; count++) {
		kPlateB->aNL[count] = 0.0;
		kPlateB->aNR[count] = 0.0;
	}
	for (int count = 0; count < delayO + 2; count++) {
		kPlateB->aOL[count] = 0.0;
		kPlateB->aOR[count] = 0.0;
	}
	for (int count = 0; count < delayP + 2; count++) {
		kPlateB->aPL[count] = 0.0;
		kPlateB->aPR[count] = 0.0;
	}
	for (int count = 0; count < delayQ + 2; count++) {
		kPlateB->aQL[count] = 0.0;
		kPlateB->aQR[count] = 0.0;
	}
	for (int count = 0; count < delayR + 2; count++) {
		kPlateB->aRL[count] = 0.0;
		kPlateB->aRR[count] = 0.0;
	}
	for (int count = 0; count < delayS + 2; count++) {
		kPlateB->aSL[count] = 0.0;
		kPlateB->aSR[count] = 0.0;
	}
	for (int count = 0; count < delayT + 2; count++) {
		kPlateB->aTL[count] = 0.0;
		kPlateB->aTR[count] = 0.0;
	}
	for (int count = 0; count < delayU + 2; count++) {
		kPlateB->aUL[count] = 0.0;
		kPlateB->aUR[count] = 0.0;
	}
	for (int count = 0; count < delayV + 2; count++) {
		kPlateB->aVL[count] = 0.0;
		kPlateB->aVR[count] = 0.0;
	}
	for (int count = 0; count < delayW + 2; count++) {
		kPlateB->aWL[count] = 0.0;
		kPlateB->aWR[count] = 0.0;
	}
	for (int count = 0; count < delayX + 2; count++) {
		kPlateB->aXL[count] = 0.0;
		kPlateB->aXR[count] = 0.0;
	}
	for (int count = 0; count < delayY + 2; count++) {
		kPlateB->aYL[count] = 0.0;
		kPlateB->aYR[count] = 0.0;
	}

	for (int count = 0; count < earlyA + 2; count++) {
		kPlateB->eAL[count] = 0.0;
		kPlateB->eAR[count] = 0.0;
	}
	for (int count = 0; count < earlyB + 2; count++) {
		kPlateB->eBL[count] = 0.0;
		kPlateB->eBR[count] = 0.0;
	}
	for (int count = 0; count < earlyC + 2; count++) {
		kPlateB->eCL[count] = 0.0;
		kPlateB->eCR[count] = 0.0;
	}
	for (int count = 0; count < earlyD + 2; count++) {
		kPlateB->eDL[count] = 0.0;
		kPlateB->eDR[count] = 0.0;
	}
	for (int count = 0; count < earlyE + 2; count++) {
		kPlateB->eEL[count] = 0.0;
		kPlateB->eER[count] = 0.0;
	}
	for (int count = 0; count < earlyF + 2; count++) {
		kPlateB->eFL[count] = 0.0;
		kPlateB->eFR[count] = 0.0;
	}
	for (int count = 0; count < earlyG + 2; count++) {
		kPlateB->eGL[count] = 0.0;
		kPlateB->eGR[count] = 0.0;
	}
	for (int count = 0; count < earlyH + 2; count++) {
		kPlateB->eHL[count] = 0.0;
		kPlateB->eHR[count] = 0.0;
	}
	for (int count = 0; count < earlyI + 2; count++) {
		kPlateB->eIL[count] = 0.0;
		kPlateB->eIR[count] = 0.0;
	}

	for (int count = 0; count < predelay + 2; count++) {
		kPlateB->aZL[count] = 0.0;
		kPlateB->aZR[count] = 0.0;
	}

	kPlateB->feedbackAL = 0.0;
	kPlateB->feedbackBL = 0.0;
	kPlateB->feedbackCL = 0.0;
	kPlateB->feedbackDL = 0.0;
	kPlateB->feedbackEL = 0.0;

	kPlateB->previousAL = 0.0;
	kPlateB->previousBL = 0.0;
	kPlateB->previousCL = 0.0;
	kPlateB->previousDL = 0.0;
	kPlateB->previousEL = 0.0;

	kPlateB->feedbackER = 0.0;
	kPlateB->feedbackJR = 0.0;
	kPlateB->feedbackOR = 0.0;
	kPlateB->feedbackTR = 0.0;
	kPlateB->feedbackYR = 0.0;

	kPlateB->previousAR = 0.0;
	kPlateB->previousBR = 0.0;
	kPlateB->previousCR = 0.0;
	kPlateB->previousDR = 0.0;
	kPlateB->previousER = 0.0;

	kPlateB->prevMulchBL = 0.0;
	kPlateB->prevMulchBR = 0.0;
	kPlateB->prevMulchCL = 0.0;
	kPlateB->prevMulchCR = 0.0;
	kPlateB->prevMulchDL = 0.0;
	kPlateB->prevMulchDR = 0.0;
	kPlateB->prevMulchEL = 0.0;
	kPlateB->prevMulchER = 0.0;

	kPlateB->prevOutAL = 0.0;
	kPlateB->prevOutAR = 0.0;
	kPlateB->prevOutBL = 0.0;
	kPlateB->prevOutBR = 0.0;
	kPlateB->prevOutCL = 0.0;
	kPlateB->prevOutCR = 0.0;

	kPlateB->prevInCL = 0.0;
	kPlateB->prevInCR = 0.0;
	kPlateB->prevInDL = 0.0;
	kPlateB->prevInDR = 0.0;
	kPlateB->prevInEL = 0.0;
	kPlateB->prevInER = 0.0;

	for (int count = 0; count < 6; count++) {
		kPlateB->lastRefL[count] = 0.0;
		kPlateB->lastRefR[count] = 0.0;
	}

	kPlateB->earlyAL = 1;
	kPlateB->earlyBL = 1;
	kPlateB->earlyCL = 1;
	kPlateB->earlyDL = 1;
	kPlateB->earlyEL = 1;
	kPlateB->earlyFL = 1;
	kPlateB->earlyGL = 1;
	kPlateB->earlyHL = 1;
	kPlateB->earlyIL = 1;

	kPlateB->earlyAR = 1;
	kPlateB->earlyBR = 1;
	kPlateB->earlyCR = 1;
	kPlateB->earlyDR = 1;
	kPlateB->earlyER = 1;
	kPlateB->earlyFR = 1;
	kPlateB->earlyGR = 1;
	kPlateB->earlyHR = 1;
	kPlateB->earlyIR = 1;

	kPlateB->countAL = 1;
	kPlateB->countBL = 1;
	kPlateB->countCL = 1;
	kPlateB->countDL = 1;
	kPlateB->countEL = 1;
	kPlateB->countFL = 1;
	kPlateB->countGL = 1;
	kPlateB->countHL = 1;
	kPlateB->countIL = 1;
	kPlateB->countJL = 1;
	kPlateB->countKL = 1;
	kPlateB->countLL = 1;
	kPlateB->countML = 1;
	kPlateB->countNL = 1;
	kPlateB->countOL = 1;
	kPlateB->countPL = 1;
	kPlateB->countQL = 1;
	kPlateB->countRL = 1;
	kPlateB->countSL = 1;
	kPlateB->countTL = 1;
	kPlateB->countUL = 1;
	kPlateB->countVL = 1;
	kPlateB->countWL = 1;
	kPlateB->countXL = 1;
	kPlateB->countYL = 1;

	kPlateB->countAR = 1;
	kPlateB->countBR = 1;
	kPlateB->countCR = 1;
	kPlateB->countDR = 1;
	kPlateB->countER = 1;
	kPlateB->countFR = 1;
	kPlateB->countGR = 1;
	kPlateB->countHR = 1;
	kPlateB->countIR = 1;
	kPlateB->countJR = 1;
	kPlateB->countKR = 1;
	kPlateB->countLR = 1;
	kPlateB->countMR = 1;
	kPlateB->countNR = 1;
	kPlateB->countOR = 1;
	kPlateB->countPR = 1;
	kPlateB->countQR = 1;
	kPlateB->countRR = 1;
	kPlateB->countSR = 1;
	kPlateB->countTR = 1;
	kPlateB->countUR = 1;
	kPlateB->countVR = 1;
	kPlateB->countWR = 1;
	kPlateB->countXR = 1;
	kPlateB->countYR = 1;

	kPlateB->countZ = 1;

	kPlateB->cycle = 0;

	for (int x = 0; x < fix_total; x++) {
		kPlateB->fixA[x] = 0.0;
		kPlateB->fixB[x] = 0.0;
		kPlateB->fixC[x] = 0.0;
		kPlateB->fixD[x] = 0.0;
	}
	// from ZBandpass, so I can use enums with it

	kPlateB->fpdL = 1.0;
	while (kPlateB->fpdL < 16386) kPlateB->fpdL = rand() * UINT32_MAX;
	kPlateB->fpdR = 1.0;
	while (kPlateB->fpdR < 16386) kPlateB->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	KPlateB* kPlateB = (KPlateB*) instance;

	const float* in1 = kPlateB->input[0];
	const float* in2 = kPlateB->input[1];
	float* out1 = kPlateB->output[0];
	float* out2 = kPlateB->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= kPlateB->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (kPlateB->cycle > cycleEnd - 1) kPlateB->cycle = cycleEnd - 1; // sanity check

	double downRate = kPlateB->sampleRate / cycleEnd;
	// we now shift gears between 44.1k and 48k so our tone is the same, slight changes in delay times

	double inputPad = *kPlateB->inputPad;
	double regen = ((*kPlateB->damping / 10.0) * 0.415) + 0.1;
	regen = (regen * 0.0001) + 0.00024;
	double iirAmount = ((*kPlateB->lowCut / 3.0) * 0.3) + 0.04;
	iirAmount = (iirAmount * 1000.0) / downRate;
	double earlyVolume = *kPlateB->predelayS; // predelay to a half-second
	int adjPredelay = (downRate * earlyVolume);
	double wet = *kPlateB->wetness * 2.0;
	double dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	// this reverb makes 50% full dry AND full wet, not crossfaded.
	// that's so it can be on submixes without cutting back dry channel when adjusted:
	// unless you go super heavy, you are only adjusting the added verb loudness.

	kPlateB->fixA[fix_freq] = 12.0 / downRate;
	kPlateB->fixA[fix_reso] = 0.0015625;
	kPlateB->fixD[fix_freq] = 26.0 / downRate;
	kPlateB->fixD[fix_reso] = 0.0003025;
	kPlateB->fixB[fix_freq] = (kPlateB->fixA[fix_freq] + kPlateB->fixA[fix_freq] + kPlateB->fixD[fix_freq]) / 3.0;
	kPlateB->fixB[fix_reso] = 0.0011425;
	kPlateB->fixC[fix_freq] = (kPlateB->fixA[fix_freq] + kPlateB->fixD[fix_freq] + kPlateB->fixD[fix_freq]) / 3.0;
	kPlateB->fixC[fix_reso] = 0.0007225;
	double K = tan(M_PI * kPlateB->fixA[fix_freq]);
	double norm = 1.0 / (1.0 + K / kPlateB->fixA[fix_reso] + K * K);
	kPlateB->fixA[fix_a0] = K / kPlateB->fixA[fix_reso] * norm;
	kPlateB->fixA[fix_a1] = 0.0;
	kPlateB->fixA[fix_a2] = -kPlateB->fixA[fix_a0];
	kPlateB->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateB->fixA[fix_b2] = (1.0 - K / kPlateB->fixA[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateB->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateB->fixB[fix_reso] + K * K);
	kPlateB->fixB[fix_a0] = K / kPlateB->fixB[fix_reso] * norm;
	kPlateB->fixB[fix_a1] = 0.0;
	kPlateB->fixB[fix_a2] = -kPlateB->fixB[fix_a0];
	kPlateB->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateB->fixB[fix_b2] = (1.0 - K / kPlateB->fixB[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateB->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateB->fixC[fix_reso] + K * K);
	kPlateB->fixC[fix_a0] = K / kPlateB->fixC[fix_reso] * norm;
	kPlateB->fixC[fix_a1] = 0.0;
	kPlateB->fixC[fix_a2] = -kPlateB->fixC[fix_a0];
	kPlateB->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateB->fixC[fix_b2] = (1.0 - K / kPlateB->fixC[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateB->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateB->fixD[fix_reso] + K * K);
	kPlateB->fixD[fix_a0] = K / kPlateB->fixD[fix_reso] * norm;
	kPlateB->fixD[fix_a1] = 0.0;
	kPlateB->fixD[fix_a2] = -kPlateB->fixD[fix_a0];
	kPlateB->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateB->fixD[fix_b2] = (1.0 - K / kPlateB->fixD[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = kPlateB->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = kPlateB->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		kPlateB->cycle++;
		if (kPlateB->cycle == cycleEnd) { // hit the end point and we do a reverb sample
			if (inputPad < 1.0) {
				inputSampleL *= inputPad;
				inputSampleR *= inputPad;
			}
			double outSample;
			outSample = (inputSampleL + kPlateB->prevInCL) * 0.5;
			kPlateB->prevInCL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateB->prevInCR) * 0.5;
			kPlateB->prevInCR = inputSampleR;
			inputSampleR = outSample;
			outSample = (inputSampleL + kPlateB->prevInDL) * 0.5;
			kPlateB->prevInDL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateB->prevInDR) * 0.5;
			kPlateB->prevInDR = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			kPlateB->iirAL = (kPlateB->iirAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateB->iirAL;
			kPlateB->iirAR = (kPlateB->iirAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateB->iirAR;
			// 600hz highpass on input

			inputSampleL *= 0.25;
			inputSampleR *= 0.25;
			if (kPlateB->gainIn < 0.0078125) kPlateB->gainIn = 0.0078125;
			if (kPlateB->gainIn > 1.0) kPlateB->gainIn = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateB->gainIn;
			inputSampleR *= kPlateB->gainIn;
			kPlateB->gainIn += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateB->gainIn += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
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

			kPlateB->iirBL = (kPlateB->iirBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateB->iirBL;
			kPlateB->iirBR = (kPlateB->iirBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateB->iirBR;
			// 600hz highpass on input

			outSample = (inputSampleL + kPlateB->prevInEL) * 0.5;
			kPlateB->prevInEL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateB->prevInER) * 0.5;
			kPlateB->prevInER = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			// begin allpasses
			double oeAL = inputSampleL - (kPlateB->eAL[(kPlateB->earlyAL + 1) - ((kPlateB->earlyAL + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeBL = inputSampleL - (kPlateB->eBL[(kPlateB->earlyBL + 1) - ((kPlateB->earlyBL + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeCL = inputSampleL - (kPlateB->eCL[(kPlateB->earlyCL + 1) - ((kPlateB->earlyCL + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeCR = inputSampleR - (kPlateB->eCR[(kPlateB->earlyCR + 1) - ((kPlateB->earlyCR + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeFR = inputSampleR - (kPlateB->eFR[(kPlateB->earlyFR + 1) - ((kPlateB->earlyFR + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeIR = inputSampleR - (kPlateB->eIR[(kPlateB->earlyIR + 1) - ((kPlateB->earlyIR + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);

			kPlateB->eAL[kPlateB->earlyAL] = oeAL;
			oeAL *= 0.5;
			kPlateB->eBL[kPlateB->earlyBL] = oeBL;
			oeBL *= 0.5;
			kPlateB->eCL[kPlateB->earlyCL] = oeCL;
			oeCL *= 0.5;
			kPlateB->eCR[kPlateB->earlyCR] = oeCR;
			oeCR *= 0.5;
			kPlateB->eFR[kPlateB->earlyFR] = oeFR;
			oeFR *= 0.5;
			kPlateB->eIR[kPlateB->earlyIR] = oeIR;
			oeIR *= 0.5;

			kPlateB->earlyAL++;
			if (kPlateB->earlyAL < 0 || kPlateB->earlyAL > earlyA) kPlateB->earlyAL = 0;
			kPlateB->earlyBL++;
			if (kPlateB->earlyBL < 0 || kPlateB->earlyBL > earlyB) kPlateB->earlyBL = 0;
			kPlateB->earlyCL++;
			if (kPlateB->earlyCL < 0 || kPlateB->earlyCL > earlyC) kPlateB->earlyCL = 0;
			kPlateB->earlyCR++;
			if (kPlateB->earlyCR < 0 || kPlateB->earlyCR > earlyC) kPlateB->earlyCR = 0;
			kPlateB->earlyFR++;
			if (kPlateB->earlyFR < 0 || kPlateB->earlyFR > earlyF) kPlateB->earlyFR = 0;
			kPlateB->earlyIR++;
			if (kPlateB->earlyIR < 0 || kPlateB->earlyIR > earlyI) kPlateB->earlyIR = 0;

			oeAL += kPlateB->eAL[kPlateB->earlyAL - ((kPlateB->earlyAL > earlyA) ? earlyA + 1 : 0)];
			oeBL += kPlateB->eBL[kPlateB->earlyBL - ((kPlateB->earlyBL > earlyB) ? earlyB + 1 : 0)];
			oeCL += kPlateB->eCL[kPlateB->earlyCL - ((kPlateB->earlyCL > earlyC) ? earlyC + 1 : 0)];
			oeCR += kPlateB->eCR[kPlateB->earlyCR - ((kPlateB->earlyCR > earlyC) ? earlyC + 1 : 0)];
			oeFR += kPlateB->eFR[kPlateB->earlyFR - ((kPlateB->earlyFR > earlyF) ? earlyF + 1 : 0)];
			oeIR += kPlateB->eIR[kPlateB->earlyIR - ((kPlateB->earlyIR > earlyI) ? earlyI + 1 : 0)];

			double oeDL = ((oeBL + oeCL) - oeAL) - (kPlateB->eDL[(kPlateB->earlyDL + 1) - ((kPlateB->earlyDL + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeEL = ((oeAL + oeCL) - oeBL) - (kPlateB->eEL[(kPlateB->earlyEL + 1) - ((kPlateB->earlyEL + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeFL = ((oeAL + oeBL) - oeCL) - (kPlateB->eFL[(kPlateB->earlyFL + 1) - ((kPlateB->earlyFL + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeBR = ((oeFR + oeIR) - oeCR) - (kPlateB->eBR[(kPlateB->earlyBR + 1) - ((kPlateB->earlyBR + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeER = ((oeCR + oeIR) - oeFR) - (kPlateB->eER[(kPlateB->earlyER + 1) - ((kPlateB->earlyER + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeHR = ((oeCR + oeFR) - oeIR) - (kPlateB->eHR[(kPlateB->earlyHR + 1) - ((kPlateB->earlyHR + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);

			kPlateB->eDL[kPlateB->earlyDL] = oeDL;
			oeDL *= 0.5;
			kPlateB->eEL[kPlateB->earlyEL] = oeEL;
			oeEL *= 0.5;
			kPlateB->eFL[kPlateB->earlyFL] = oeFL;
			oeFL *= 0.5;
			kPlateB->eBR[kPlateB->earlyBR] = oeBR;
			oeBR *= 0.5;
			kPlateB->eER[kPlateB->earlyER] = oeER;
			oeER *= 0.5;
			kPlateB->eHR[kPlateB->earlyHR] = oeHR;
			oeHR *= 0.5;

			kPlateB->earlyDL++;
			if (kPlateB->earlyDL < 0 || kPlateB->earlyDL > earlyD) kPlateB->earlyDL = 0;
			kPlateB->earlyEL++;
			if (kPlateB->earlyEL < 0 || kPlateB->earlyEL > earlyE) kPlateB->earlyEL = 0;
			kPlateB->earlyFL++;
			if (kPlateB->earlyFL < 0 || kPlateB->earlyFL > earlyF) kPlateB->earlyFL = 0;
			kPlateB->earlyBR++;
			if (kPlateB->earlyBR < 0 || kPlateB->earlyBR > earlyB) kPlateB->earlyBR = 0;
			kPlateB->earlyER++;
			if (kPlateB->earlyER < 0 || kPlateB->earlyER > earlyE) kPlateB->earlyER = 0;
			kPlateB->earlyHR++;
			if (kPlateB->earlyHR < 0 || kPlateB->earlyHR > earlyH) kPlateB->earlyHR = 0;

			oeDL += kPlateB->eDL[kPlateB->earlyDL - ((kPlateB->earlyDL > earlyD) ? earlyD + 1 : 0)];
			oeEL += kPlateB->eEL[kPlateB->earlyEL - ((kPlateB->earlyEL > earlyE) ? earlyE + 1 : 0)];
			oeFL += kPlateB->eFL[kPlateB->earlyFL - ((kPlateB->earlyFL > earlyF) ? earlyF + 1 : 0)];
			oeBR += kPlateB->eBR[kPlateB->earlyBR - ((kPlateB->earlyBR > earlyB) ? earlyB + 1 : 0)];
			oeER += kPlateB->eER[kPlateB->earlyER - ((kPlateB->earlyER > earlyE) ? earlyE + 1 : 0)];
			oeHR += kPlateB->eHR[kPlateB->earlyHR - ((kPlateB->earlyHR > earlyH) ? earlyH + 1 : 0)];

			double oeGL = ((oeEL + oeFL) - oeDL) - (kPlateB->eGL[(kPlateB->earlyGL + 1) - ((kPlateB->earlyGL + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);
			double oeHL = ((oeDL + oeFL) - oeEL) - (kPlateB->eHL[(kPlateB->earlyHL + 1) - ((kPlateB->earlyHL + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);
			double oeIL = ((oeDL + oeEL) - oeFL) - (kPlateB->eIL[(kPlateB->earlyIL + 1) - ((kPlateB->earlyIL + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);
			double oeAR = ((oeER + oeHR) - oeBR) - (kPlateB->eAR[(kPlateB->earlyAR + 1) - ((kPlateB->earlyAR + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeDR = ((oeBR + oeHR) - oeER) - (kPlateB->eDR[(kPlateB->earlyDR + 1) - ((kPlateB->earlyDR + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeGR = ((oeBR + oeER) - oeHR) - (kPlateB->eGR[(kPlateB->earlyGR + 1) - ((kPlateB->earlyGR + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);

			kPlateB->eGL[kPlateB->earlyGL] = oeGL;
			oeGL *= 0.5;
			kPlateB->eHL[kPlateB->earlyHL] = oeHL;
			oeHL *= 0.5;
			kPlateB->eIL[kPlateB->earlyIL] = oeIL;
			oeIL *= 0.5;
			kPlateB->eAR[kPlateB->earlyAR] = oeAR;
			oeAR *= 0.5;
			kPlateB->eDR[kPlateB->earlyDR] = oeDR;
			oeDR *= 0.5;
			kPlateB->eGR[kPlateB->earlyGR] = oeGR;
			oeGR *= 0.5;

			kPlateB->earlyGL++;
			if (kPlateB->earlyGL < 0 || kPlateB->earlyGL > earlyG) kPlateB->earlyGL = 0;
			kPlateB->earlyHL++;
			if (kPlateB->earlyHL < 0 || kPlateB->earlyHL > earlyH) kPlateB->earlyHL = 0;
			kPlateB->earlyIL++;
			if (kPlateB->earlyIL < 0 || kPlateB->earlyIL > earlyI) kPlateB->earlyIL = 0;
			kPlateB->earlyAR++;
			if (kPlateB->earlyAR < 0 || kPlateB->earlyAR > earlyA) kPlateB->earlyAR = 0;
			kPlateB->earlyDR++;
			if (kPlateB->earlyDR < 0 || kPlateB->earlyDR > earlyD) kPlateB->earlyDR = 0;
			kPlateB->earlyGR++;
			if (kPlateB->earlyGR < 0 || kPlateB->earlyGR > earlyG) kPlateB->earlyGR = 0;

			oeGL += kPlateB->eGL[kPlateB->earlyGL - ((kPlateB->earlyGL > earlyG) ? earlyG + 1 : 0)];
			oeHL += kPlateB->eHL[kPlateB->earlyHL - ((kPlateB->earlyHL > earlyH) ? earlyH + 1 : 0)];
			oeIL += kPlateB->eIL[kPlateB->earlyIL - ((kPlateB->earlyIL > earlyI) ? earlyI + 1 : 0)];
			oeAR += kPlateB->eAR[kPlateB->earlyAR - ((kPlateB->earlyAR > earlyA) ? earlyA + 1 : 0)];
			oeDR += kPlateB->eDR[kPlateB->earlyDR - ((kPlateB->earlyDR > earlyD) ? earlyD + 1 : 0)];
			oeGR += kPlateB->eGR[kPlateB->earlyGR - ((kPlateB->earlyGR > earlyG) ? earlyG + 1 : 0)];

			// allpasses predelay
			kPlateB->aZL[kPlateB->countZ] = (oeGL + oeHL + oeIL) * 0.25;
			kPlateB->aZR[kPlateB->countZ] = (oeAR + oeDR + oeGR) * 0.25;
			kPlateB->countZ++;
			if (kPlateB->countZ < 0 || kPlateB->countZ > adjPredelay) kPlateB->countZ = 0;
			inputSampleL = kPlateB->aZL[kPlateB->countZ - ((kPlateB->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			inputSampleR = kPlateB->aZR[kPlateB->countZ - ((kPlateB->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			// end allpasses

			kPlateB->aAL[kPlateB->countAL] = inputSampleL + (kPlateB->feedbackAL * regen);
			kPlateB->aBL[kPlateB->countBL] = inputSampleL + (kPlateB->feedbackBL * regen);
			kPlateB->aCL[kPlateB->countCL] = inputSampleL + (kPlateB->feedbackCL * regen);
			kPlateB->aDL[kPlateB->countDL] = inputSampleL + (kPlateB->feedbackDL * regen);
			kPlateB->aEL[kPlateB->countEL] = inputSampleL + (kPlateB->feedbackEL * regen);

			kPlateB->aER[kPlateB->countER] = inputSampleR + (kPlateB->feedbackER * regen);
			kPlateB->aJR[kPlateB->countJR] = inputSampleR + (kPlateB->feedbackJR * regen);
			kPlateB->aOR[kPlateB->countOR] = inputSampleR + (kPlateB->feedbackOR * regen);
			kPlateB->aTR[kPlateB->countTR] = inputSampleR + (kPlateB->feedbackTR * regen);
			kPlateB->aYR[kPlateB->countYR] = inputSampleR + (kPlateB->feedbackYR * regen);

			kPlateB->countAL++;
			if (kPlateB->countAL < 0 || kPlateB->countAL > delayA) kPlateB->countAL = 0;
			kPlateB->countBL++;
			if (kPlateB->countBL < 0 || kPlateB->countBL > delayB) kPlateB->countBL = 0;
			kPlateB->countCL++;
			if (kPlateB->countCL < 0 || kPlateB->countCL > delayC) kPlateB->countCL = 0;
			kPlateB->countDL++;
			if (kPlateB->countDL < 0 || kPlateB->countDL > delayD) kPlateB->countDL = 0;
			kPlateB->countEL++;
			if (kPlateB->countEL < 0 || kPlateB->countEL > delayE) kPlateB->countEL = 0;

			kPlateB->countER++;
			if (kPlateB->countER < 0 || kPlateB->countER > delayE) kPlateB->countER = 0;
			kPlateB->countJR++;
			if (kPlateB->countJR < 0 || kPlateB->countJR > delayJ) kPlateB->countJR = 0;
			kPlateB->countOR++;
			if (kPlateB->countOR < 0 || kPlateB->countOR > delayO) kPlateB->countOR = 0;
			kPlateB->countTR++;
			if (kPlateB->countTR < 0 || kPlateB->countTR > delayT) kPlateB->countTR = 0;
			kPlateB->countYR++;
			if (kPlateB->countYR < 0 || kPlateB->countYR > delayY) kPlateB->countYR = 0;

			double outAL = kPlateB->aAL[kPlateB->countAL - ((kPlateB->countAL > delayA) ? delayA + 1 : 0)];
			double outBL = kPlateB->aBL[kPlateB->countBL - ((kPlateB->countBL > delayB) ? delayB + 1 : 0)];
			double outCL = kPlateB->aCL[kPlateB->countCL - ((kPlateB->countCL > delayC) ? delayC + 1 : 0)];
			double outDL = kPlateB->aDL[kPlateB->countDL - ((kPlateB->countDL > delayD) ? delayD + 1 : 0)];
			double outEL = kPlateB->aEL[kPlateB->countEL - ((kPlateB->countEL > delayE) ? delayE + 1 : 0)];

			double outER = kPlateB->aER[kPlateB->countER - ((kPlateB->countER > delayE) ? delayE + 1 : 0)];
			double outJR = kPlateB->aJR[kPlateB->countJR - ((kPlateB->countJR > delayJ) ? delayJ + 1 : 0)];
			double outOR = kPlateB->aOR[kPlateB->countOR - ((kPlateB->countOR > delayO) ? delayO + 1 : 0)];
			double outTR = kPlateB->aTR[kPlateB->countTR - ((kPlateB->countTR > delayT) ? delayT + 1 : 0)];
			double outYR = kPlateB->aYR[kPlateB->countYR - ((kPlateB->countYR > delayY) ? delayY + 1 : 0)];

			//-------- one

			outSample = (outAL * kPlateB->fixA[fix_a0]) + kPlateB->fixA[fix_sL1];
			kPlateB->fixA[fix_sL1] = (outAL * kPlateB->fixA[fix_a1]) - (outSample * kPlateB->fixA[fix_b1]) + kPlateB->fixA[fix_sL2];
			kPlateB->fixA[fix_sL2] = (outAL * kPlateB->fixA[fix_a2]) - (outSample * kPlateB->fixA[fix_b2]);
			outAL = outSample; // fixed biquad

			outSample = (outER * kPlateB->fixA[fix_a0]) + kPlateB->fixA[fix_sR1];
			kPlateB->fixA[fix_sR1] = (outER * kPlateB->fixA[fix_a1]) - (outSample * kPlateB->fixA[fix_b1]) + kPlateB->fixA[fix_sR2];
			kPlateB->fixA[fix_sR2] = (outER * kPlateB->fixA[fix_a2]) - (outSample * kPlateB->fixA[fix_b2]);
			outER = outSample; // fixed biquad

			//-------- filtered (one path in five, feeding the rest of the matrix

			kPlateB->aFL[kPlateB->countFL] = ((outAL * 3.0) - ((outBL + outCL + outDL + outEL) * 2.0));
			kPlateB->aGL[kPlateB->countGL] = ((outBL * 3.0) - ((outAL + outCL + outDL + outEL) * 2.0));
			kPlateB->aHL[kPlateB->countHL] = ((outCL * 3.0) - ((outAL + outBL + outDL + outEL) * 2.0));
			kPlateB->aIL[kPlateB->countIL] = ((outDL * 3.0) - ((outAL + outBL + outCL + outEL) * 2.0));
			kPlateB->aJL[kPlateB->countJL] = ((outEL * 3.0) - ((outAL + outBL + outCL + outDL) * 2.0));

			kPlateB->aDR[kPlateB->countDR] = ((outER * 3.0) - ((outJR + outOR + outTR + outYR) * 2.0));
			kPlateB->aIR[kPlateB->countIR] = ((outJR * 3.0) - ((outER + outOR + outTR + outYR) * 2.0));
			kPlateB->aNR[kPlateB->countNR] = ((outOR * 3.0) - ((outER + outJR + outTR + outYR) * 2.0));
			kPlateB->aSR[kPlateB->countSR] = ((outTR * 3.0) - ((outER + outJR + outOR + outYR) * 2.0));
			kPlateB->aXR[kPlateB->countXR] = ((outYR * 3.0) - ((outER + outJR + outOR + outTR) * 2.0));

			kPlateB->countFL++;
			if (kPlateB->countFL < 0 || kPlateB->countFL > delayF) kPlateB->countFL = 0;
			kPlateB->countGL++;
			if (kPlateB->countGL < 0 || kPlateB->countGL > delayG) kPlateB->countGL = 0;
			kPlateB->countHL++;
			if (kPlateB->countHL < 0 || kPlateB->countHL > delayH) kPlateB->countHL = 0;
			kPlateB->countIL++;
			if (kPlateB->countIL < 0 || kPlateB->countIL > delayI) kPlateB->countIL = 0;
			kPlateB->countJL++;
			if (kPlateB->countJL < 0 || kPlateB->countJL > delayJ) kPlateB->countJL = 0;

			kPlateB->countDR++;
			if (kPlateB->countDR < 0 || kPlateB->countDR > delayD) kPlateB->countDR = 0;
			kPlateB->countIR++;
			if (kPlateB->countIR < 0 || kPlateB->countIR > delayI) kPlateB->countIR = 0;
			kPlateB->countNR++;
			if (kPlateB->countNR < 0 || kPlateB->countNR > delayN) kPlateB->countNR = 0;
			kPlateB->countSR++;
			if (kPlateB->countSR < 0 || kPlateB->countSR > delayS) kPlateB->countSR = 0;
			kPlateB->countXR++;
			if (kPlateB->countXR < 0 || kPlateB->countXR > delayX) kPlateB->countXR = 0;

			double outFL = kPlateB->aFL[kPlateB->countFL - ((kPlateB->countFL > delayF) ? delayF + 1 : 0)];
			double outGL = kPlateB->aGL[kPlateB->countGL - ((kPlateB->countGL > delayG) ? delayG + 1 : 0)];
			double outHL = kPlateB->aHL[kPlateB->countHL - ((kPlateB->countHL > delayH) ? delayH + 1 : 0)];
			double outIL = kPlateB->aIL[kPlateB->countIL - ((kPlateB->countIL > delayI) ? delayI + 1 : 0)];
			double outJL = kPlateB->aJL[kPlateB->countJL - ((kPlateB->countJL > delayJ) ? delayJ + 1 : 0)];

			double outDR = kPlateB->aDR[kPlateB->countDR - ((kPlateB->countDR > delayD) ? delayD + 1 : 0)];
			double outIR = kPlateB->aIR[kPlateB->countIR - ((kPlateB->countIR > delayI) ? delayI + 1 : 0)];
			double outNR = kPlateB->aNR[kPlateB->countNR - ((kPlateB->countNR > delayN) ? delayN + 1 : 0)];
			double outSR = kPlateB->aSR[kPlateB->countSR - ((kPlateB->countSR > delayS) ? delayS + 1 : 0)];
			double outXR = kPlateB->aXR[kPlateB->countXR - ((kPlateB->countXR > delayX) ? delayX + 1 : 0)];

			//-------- mulch

			outSample = (outFL * kPlateB->fixB[fix_a0]) + kPlateB->fixB[fix_sL1];
			kPlateB->fixB[fix_sL1] = (outFL * kPlateB->fixB[fix_a1]) - (outSample * kPlateB->fixB[fix_b1]) + kPlateB->fixB[fix_sL2];
			kPlateB->fixB[fix_sL2] = (outFL * kPlateB->fixB[fix_a2]) - (outSample * kPlateB->fixB[fix_b2]);
			outFL = outSample; // fixed biquad

			outSample = (outDR * kPlateB->fixB[fix_a0]) + kPlateB->fixB[fix_sR1];
			kPlateB->fixB[fix_sR1] = (outDR * kPlateB->fixB[fix_a1]) - (outSample * kPlateB->fixB[fix_b1]) + kPlateB->fixB[fix_sR2];
			kPlateB->fixB[fix_sR2] = (outDR * kPlateB->fixB[fix_a2]) - (outSample * kPlateB->fixB[fix_b2]);
			outDR = outSample; // fixed biquad

			outSample = (outGL + kPlateB->prevMulchBL) * 0.5;
			kPlateB->prevMulchBL = outGL;
			outGL = outSample;
			outSample = (outIR + kPlateB->prevMulchBR) * 0.5;
			kPlateB->prevMulchBR = outIR;
			outIR = outSample;

			//-------- two

			kPlateB->aKL[kPlateB->countKL] = ((outFL * 3.0) - ((outGL + outHL + outIL + outJL) * 2.0));
			kPlateB->aLL[kPlateB->countLL] = ((outGL * 3.0) - ((outFL + outHL + outIL + outJL) * 2.0));
			kPlateB->aML[kPlateB->countML] = ((outHL * 3.0) - ((outFL + outGL + outIL + outJL) * 2.0));
			kPlateB->aNL[kPlateB->countNL] = ((outIL * 3.0) - ((outFL + outGL + outHL + outJL) * 2.0));
			kPlateB->aOL[kPlateB->countOL] = ((outJL * 3.0) - ((outFL + outGL + outHL + outIL) * 2.0));

			kPlateB->aCR[kPlateB->countCR] = ((outDR * 3.0) - ((outIR + outNR + outSR + outXR) * 2.0));
			kPlateB->aHR[kPlateB->countHR] = ((outIR * 3.0) - ((outDR + outNR + outSR + outXR) * 2.0));
			kPlateB->aMR[kPlateB->countMR] = ((outNR * 3.0) - ((outDR + outIR + outSR + outXR) * 2.0));
			kPlateB->aRR[kPlateB->countRR] = ((outSR * 3.0) - ((outDR + outIR + outNR + outXR) * 2.0));
			kPlateB->aWR[kPlateB->countWR] = ((outXR * 3.0) - ((outDR + outIR + outNR + outSR) * 2.0));

			kPlateB->countKL++;
			if (kPlateB->countKL < 0 || kPlateB->countKL > delayK) kPlateB->countKL = 0;
			kPlateB->countLL++;
			if (kPlateB->countLL < 0 || kPlateB->countLL > delayL) kPlateB->countLL = 0;
			kPlateB->countML++;
			if (kPlateB->countML < 0 || kPlateB->countML > delayM) kPlateB->countML = 0;
			kPlateB->countNL++;
			if (kPlateB->countNL < 0 || kPlateB->countNL > delayN) kPlateB->countNL = 0;
			kPlateB->countOL++;
			if (kPlateB->countOL < 0 || kPlateB->countOL > delayO) kPlateB->countOL = 0;

			kPlateB->countCR++;
			if (kPlateB->countCR < 0 || kPlateB->countCR > delayC) kPlateB->countCR = 0;
			kPlateB->countHR++;
			if (kPlateB->countHR < 0 || kPlateB->countHR > delayH) kPlateB->countHR = 0;
			kPlateB->countMR++;
			if (kPlateB->countMR < 0 || kPlateB->countMR > delayM) kPlateB->countMR = 0;
			kPlateB->countRR++;
			if (kPlateB->countRR < 0 || kPlateB->countRR > delayR) kPlateB->countRR = 0;
			kPlateB->countWR++;
			if (kPlateB->countWR < 0 || kPlateB->countWR > delayW) kPlateB->countWR = 0;

			double outKL = kPlateB->aKL[kPlateB->countKL - ((kPlateB->countKL > delayK) ? delayK + 1 : 0)];
			double outLL = kPlateB->aLL[kPlateB->countLL - ((kPlateB->countLL > delayL) ? delayL + 1 : 0)];
			double outML = kPlateB->aML[kPlateB->countML - ((kPlateB->countML > delayM) ? delayM + 1 : 0)];
			double outNL = kPlateB->aNL[kPlateB->countNL - ((kPlateB->countNL > delayN) ? delayN + 1 : 0)];
			double outOL = kPlateB->aOL[kPlateB->countOL - ((kPlateB->countOL > delayO) ? delayO + 1 : 0)];

			double outCR = kPlateB->aCR[kPlateB->countCR - ((kPlateB->countCR > delayC) ? delayC + 1 : 0)];
			double outHR = kPlateB->aHR[kPlateB->countHR - ((kPlateB->countHR > delayH) ? delayH + 1 : 0)];
			double outMR = kPlateB->aMR[kPlateB->countMR - ((kPlateB->countMR > delayM) ? delayM + 1 : 0)];
			double outRR = kPlateB->aRR[kPlateB->countRR - ((kPlateB->countRR > delayR) ? delayR + 1 : 0)];
			double outWR = kPlateB->aWR[kPlateB->countWR - ((kPlateB->countWR > delayW) ? delayW + 1 : 0)];

			//-------- mulch

			outSample = (outKL * kPlateB->fixC[fix_a0]) + kPlateB->fixC[fix_sL1];
			kPlateB->fixC[fix_sL1] = (outKL * kPlateB->fixC[fix_a1]) - (outSample * kPlateB->fixC[fix_b1]) + kPlateB->fixC[fix_sL2];
			kPlateB->fixC[fix_sL2] = (outKL * kPlateB->fixC[fix_a2]) - (outSample * kPlateB->fixC[fix_b2]);
			outKL = outSample; // fixed biquad

			outSample = (outCR * kPlateB->fixC[fix_a0]) + kPlateB->fixC[fix_sR1];
			kPlateB->fixC[fix_sR1] = (outCR * kPlateB->fixC[fix_a1]) - (outSample * kPlateB->fixC[fix_b1]) + kPlateB->fixC[fix_sR2];
			kPlateB->fixC[fix_sR2] = (outCR * kPlateB->fixC[fix_a2]) - (outSample * kPlateB->fixC[fix_b2]);
			outCR = outSample; // fixed biquad

			outSample = (outLL + kPlateB->prevMulchCL) * 0.5;
			kPlateB->prevMulchCL = outLL;
			outLL = outSample;
			outSample = (outHR + kPlateB->prevMulchCR) * 0.5;
			kPlateB->prevMulchCR = outHR;
			outHR = outSample;

			//-------- three

			kPlateB->aPL[kPlateB->countPL] = ((outKL * 3.0) - ((outLL + outML + outNL + outOL) * 2.0));
			kPlateB->aQL[kPlateB->countQL] = ((outLL * 3.0) - ((outKL + outML + outNL + outOL) * 2.0));
			kPlateB->aRL[kPlateB->countRL] = ((outML * 3.0) - ((outKL + outLL + outNL + outOL) * 2.0));
			kPlateB->aSL[kPlateB->countSL] = ((outNL * 3.0) - ((outKL + outLL + outML + outOL) * 2.0));
			kPlateB->aTL[kPlateB->countTL] = ((outOL * 3.0) - ((outKL + outLL + outML + outNL) * 2.0));

			kPlateB->aBR[kPlateB->countBR] = ((outCR * 3.0) - ((outHR + outMR + outRR + outWR) * 2.0));
			kPlateB->aGR[kPlateB->countGR] = ((outHR * 3.0) - ((outCR + outMR + outRR + outWR) * 2.0));
			kPlateB->aLR[kPlateB->countLR] = ((outMR * 3.0) - ((outCR + outHR + outRR + outWR) * 2.0));
			kPlateB->aQR[kPlateB->countQR] = ((outRR * 3.0) - ((outCR + outHR + outMR + outWR) * 2.0));
			kPlateB->aVR[kPlateB->countVR] = ((outWR * 3.0) - ((outCR + outHR + outMR + outRR) * 2.0));

			kPlateB->countPL++;
			if (kPlateB->countPL < 0 || kPlateB->countPL > delayP) kPlateB->countPL = 0;
			kPlateB->countQL++;
			if (kPlateB->countQL < 0 || kPlateB->countQL > delayQ) kPlateB->countQL = 0;
			kPlateB->countRL++;
			if (kPlateB->countRL < 0 || kPlateB->countRL > delayR) kPlateB->countRL = 0;
			kPlateB->countSL++;
			if (kPlateB->countSL < 0 || kPlateB->countSL > delayS) kPlateB->countSL = 0;
			kPlateB->countTL++;
			if (kPlateB->countTL < 0 || kPlateB->countTL > delayT) kPlateB->countTL = 0;

			kPlateB->countBR++;
			if (kPlateB->countBR < 0 || kPlateB->countBR > delayB) kPlateB->countBR = 0;
			kPlateB->countGR++;
			if (kPlateB->countGR < 0 || kPlateB->countGR > delayG) kPlateB->countGR = 0;
			kPlateB->countLR++;
			if (kPlateB->countLR < 0 || kPlateB->countLR > delayL) kPlateB->countLR = 0;
			kPlateB->countQR++;
			if (kPlateB->countQR < 0 || kPlateB->countQR > delayQ) kPlateB->countQR = 0;
			kPlateB->countVR++;
			if (kPlateB->countVR < 0 || kPlateB->countVR > delayV) kPlateB->countVR = 0;

			double outPL = kPlateB->aPL[kPlateB->countPL - ((kPlateB->countPL > delayP) ? delayP + 1 : 0)];
			double outQL = kPlateB->aQL[kPlateB->countQL - ((kPlateB->countQL > delayQ) ? delayQ + 1 : 0)];
			double outRL = kPlateB->aRL[kPlateB->countRL - ((kPlateB->countRL > delayR) ? delayR + 1 : 0)];
			double outSL = kPlateB->aSL[kPlateB->countSL - ((kPlateB->countSL > delayS) ? delayS + 1 : 0)];
			double outTL = kPlateB->aTL[kPlateB->countTL - ((kPlateB->countTL > delayT) ? delayT + 1 : 0)];

			double outBR = kPlateB->aBR[kPlateB->countBR - ((kPlateB->countBR > delayB) ? delayB + 1 : 0)];
			double outGR = kPlateB->aGR[kPlateB->countGR - ((kPlateB->countGR > delayG) ? delayG + 1 : 0)];
			double outLR = kPlateB->aLR[kPlateB->countLR - ((kPlateB->countLR > delayL) ? delayL + 1 : 0)];
			double outQR = kPlateB->aQR[kPlateB->countQR - ((kPlateB->countQR > delayQ) ? delayQ + 1 : 0)];
			double outVR = kPlateB->aVR[kPlateB->countVR - ((kPlateB->countVR > delayV) ? delayV + 1 : 0)];

			//-------- mulch

			outSample = (outPL * kPlateB->fixD[fix_a0]) + kPlateB->fixD[fix_sL1];
			kPlateB->fixD[fix_sL1] = (outPL * kPlateB->fixD[fix_a1]) - (outSample * kPlateB->fixD[fix_b1]) + kPlateB->fixD[fix_sL2];
			kPlateB->fixD[fix_sL2] = (outPL * kPlateB->fixD[fix_a2]) - (outSample * kPlateB->fixD[fix_b2]);
			outPL = outSample; // fixed biquad

			outSample = (outBR * kPlateB->fixD[fix_a0]) + kPlateB->fixD[fix_sR1];
			kPlateB->fixD[fix_sR1] = (outBR * kPlateB->fixD[fix_a1]) - (outSample * kPlateB->fixD[fix_b1]) + kPlateB->fixD[fix_sR2];
			kPlateB->fixD[fix_sR2] = (outBR * kPlateB->fixD[fix_a2]) - (outSample * kPlateB->fixD[fix_b2]);
			outBR = outSample; // fixed biquad

			outSample = (outQL + kPlateB->prevMulchDL) * 0.5;
			kPlateB->prevMulchDL = outQL;
			outQL = outSample;
			outSample = (outGR + kPlateB->prevMulchDR) * 0.5;
			kPlateB->prevMulchDR = outGR;
			outGR = outSample;

			//-------- four

			kPlateB->aUL[kPlateB->countUL] = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0));
			kPlateB->aVL[kPlateB->countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
			kPlateB->aWL[kPlateB->countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
			kPlateB->aXL[kPlateB->countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
			kPlateB->aYL[kPlateB->countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));

			kPlateB->aAR[kPlateB->countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
			kPlateB->aFR[kPlateB->countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
			kPlateB->aKR[kPlateB->countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
			kPlateB->aPR[kPlateB->countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
			kPlateB->aUR[kPlateB->countUR] = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0));

			kPlateB->countUL++;
			if (kPlateB->countUL < 0 || kPlateB->countUL > delayU) kPlateB->countUL = 0;
			kPlateB->countVL++;
			if (kPlateB->countVL < 0 || kPlateB->countVL > delayV) kPlateB->countVL = 0;
			kPlateB->countWL++;
			if (kPlateB->countWL < 0 || kPlateB->countWL > delayW) kPlateB->countWL = 0;
			kPlateB->countXL++;
			if (kPlateB->countXL < 0 || kPlateB->countXL > delayX) kPlateB->countXL = 0;
			kPlateB->countYL++;
			if (kPlateB->countYL < 0 || kPlateB->countYL > delayY) kPlateB->countYL = 0;

			kPlateB->countAR++;
			if (kPlateB->countAR < 0 || kPlateB->countAR > delayA) kPlateB->countAR = 0;
			kPlateB->countFR++;
			if (kPlateB->countFR < 0 || kPlateB->countFR > delayF) kPlateB->countFR = 0;
			kPlateB->countKR++;
			if (kPlateB->countKR < 0 || kPlateB->countKR > delayK) kPlateB->countKR = 0;
			kPlateB->countPR++;
			if (kPlateB->countPR < 0 || kPlateB->countPR > delayP) kPlateB->countPR = 0;
			kPlateB->countUR++;
			if (kPlateB->countUR < 0 || kPlateB->countUR > delayU) kPlateB->countUR = 0;

			double outUL = kPlateB->aUL[kPlateB->countUL - ((kPlateB->countUL > delayU) ? delayU + 1 : 0)];
			double outVL = kPlateB->aVL[kPlateB->countVL - ((kPlateB->countVL > delayV) ? delayV + 1 : 0)];
			double outWL = kPlateB->aWL[kPlateB->countWL - ((kPlateB->countWL > delayW) ? delayW + 1 : 0)];
			double outXL = kPlateB->aXL[kPlateB->countXL - ((kPlateB->countXL > delayX) ? delayX + 1 : 0)];
			double outYL = kPlateB->aYL[kPlateB->countYL - ((kPlateB->countYL > delayY) ? delayY + 1 : 0)];

			double outAR = kPlateB->aAR[kPlateB->countAR - ((kPlateB->countAR > delayA) ? delayA + 1 : 0)];
			double outFR = kPlateB->aFR[kPlateB->countFR - ((kPlateB->countFR > delayF) ? delayF + 1 : 0)];
			double outKR = kPlateB->aKR[kPlateB->countKR - ((kPlateB->countKR > delayK) ? delayK + 1 : 0)];
			double outPR = kPlateB->aPR[kPlateB->countPR - ((kPlateB->countPR > delayP) ? delayP + 1 : 0)];
			double outUR = kPlateB->aUR[kPlateB->countUR - ((kPlateB->countUR > delayU) ? delayU + 1 : 0)];

			//-------- mulch

			outSample = (outVL + kPlateB->prevMulchEL) * 0.5;
			kPlateB->prevMulchEL = outVL;
			outVL = outSample;
			outSample = (outFR + kPlateB->prevMulchER) * 0.5;
			kPlateB->prevMulchER = outFR;
			outFR = outSample;

			//-------- five

			kPlateB->feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
			kPlateB->feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
			kPlateB->feedbackJR = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
			kPlateB->feedbackBL = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
			kPlateB->feedbackOR = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
			kPlateB->feedbackCL = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
			kPlateB->feedbackDL = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
			kPlateB->feedbackTR = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
			kPlateB->feedbackEL = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
			kPlateB->feedbackYR = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
			// which we need to feed back into the input again, a bit

			inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0016;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0016;
			// and take the final combined sum of outputs, corrected for Householder gain

			inputSampleL *= 0.5;
			inputSampleR *= 0.5;
			if (kPlateB->gainOutL < 0.0078125) kPlateB->gainOutL = 0.0078125;
			if (kPlateB->gainOutL > 1.0) kPlateB->gainOutL = 1.0;
			if (kPlateB->gainOutR < 0.0078125) kPlateB->gainOutR = 0.0078125;
			if (kPlateB->gainOutR > 1.0) kPlateB->gainOutR = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateB->gainOutL;
			inputSampleR *= kPlateB->gainOutR;
			kPlateB->gainOutL += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateB->gainOutR += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			inputSampleL *= 2.0;
			inputSampleR *= 2.0;
			// curve! To get a compressed effect that matches a certain other plugin
			// that is too overprocessed for its own good :)

			outSample = (inputSampleL + kPlateB->prevOutAL) * 0.5;
			kPlateB->prevOutAL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateB->prevOutAR) * 0.5;
			kPlateB->prevOutAR = inputSampleR;
			inputSampleR = outSample;
			outSample = (inputSampleL + kPlateB->prevOutBL) * 0.5;
			kPlateB->prevOutBL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateB->prevOutBR) * 0.5;
			kPlateB->prevOutBR = inputSampleR;
			inputSampleR = outSample;
			outSample = (inputSampleL + kPlateB->prevOutCL) * 0.5;
			kPlateB->prevOutCL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateB->prevOutCR) * 0.5;
			kPlateB->prevOutCR = inputSampleR;
			inputSampleR = outSample;

			if (cycleEnd == 4) {
				kPlateB->lastRefL[0] = kPlateB->lastRefL[4]; // start from previous last
				kPlateB->lastRefL[2] = (kPlateB->lastRefL[0] + inputSampleL) / 2; // half
				kPlateB->lastRefL[1] = (kPlateB->lastRefL[0] + kPlateB->lastRefL[2]) / 2; // one quarter
				kPlateB->lastRefL[3] = (kPlateB->lastRefL[2] + inputSampleL) / 2; // three quarters
				kPlateB->lastRefL[4] = inputSampleL; // full
				kPlateB->lastRefR[0] = kPlateB->lastRefR[4]; // start from previous last
				kPlateB->lastRefR[2] = (kPlateB->lastRefR[0] + inputSampleR) / 2; // half
				kPlateB->lastRefR[1] = (kPlateB->lastRefR[0] + kPlateB->lastRefR[2]) / 2; // one quarter
				kPlateB->lastRefR[3] = (kPlateB->lastRefR[2] + inputSampleR) / 2; // three quarters
				kPlateB->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				kPlateB->lastRefL[0] = kPlateB->lastRefL[3]; // start from previous last
				kPlateB->lastRefL[2] = (kPlateB->lastRefL[0] + kPlateB->lastRefL[0] + inputSampleL) / 3; // third
				kPlateB->lastRefL[1] = (kPlateB->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				kPlateB->lastRefL[3] = inputSampleL; // full
				kPlateB->lastRefR[0] = kPlateB->lastRefR[3]; // start from previous last
				kPlateB->lastRefR[2] = (kPlateB->lastRefR[0] + kPlateB->lastRefR[0] + inputSampleR) / 3; // third
				kPlateB->lastRefR[1] = (kPlateB->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				kPlateB->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				kPlateB->lastRefL[0] = kPlateB->lastRefL[2]; // start from previous last
				kPlateB->lastRefL[1] = (kPlateB->lastRefL[0] + inputSampleL) / 2; // half
				kPlateB->lastRefL[2] = inputSampleL; // full
				kPlateB->lastRefR[0] = kPlateB->lastRefR[2]; // start from previous last
				kPlateB->lastRefR[1] = (kPlateB->lastRefR[0] + inputSampleR) / 2; // half
				kPlateB->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				kPlateB->lastRefL[0] = inputSampleL;
				kPlateB->lastRefR[0] = inputSampleR;
			}
			kPlateB->cycle = 0; // reset
			inputSampleL = kPlateB->lastRefL[kPlateB->cycle];
			inputSampleR = kPlateB->lastRefR[kPlateB->cycle];
		} else {
			inputSampleL = kPlateB->lastRefL[kPlateB->cycle];
			inputSampleR = kPlateB->lastRefR[kPlateB->cycle];
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
		kPlateB->fpdL ^= kPlateB->fpdL << 13;
		kPlateB->fpdL ^= kPlateB->fpdL >> 17;
		kPlateB->fpdL ^= kPlateB->fpdL << 5;
		inputSampleL += (((double) kPlateB->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		kPlateB->fpdR ^= kPlateB->fpdR << 13;
		kPlateB->fpdR ^= kPlateB->fpdR >> 17;
		kPlateB->fpdR ^= kPlateB->fpdR << 5;
		inputSampleR += (((double) kPlateB->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	KPLATEB_URI,
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
