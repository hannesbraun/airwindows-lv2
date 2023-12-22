#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define KPLATED_URI "https://hannesbraun.net/ns/lv2/airwindows/kplated"

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

#define earlyA 103
#define earlyB 709
#define earlyC 151
#define earlyD 263
#define earlyE 1433
#define earlyF 593
#define earlyG 1361
#define earlyH 31
#define earlyI 691
#define predelay 24010 // 79 ms, 750 seat theater. Scarcity, 1 in 60270
#define delayA 619
#define delayB 181
#define delayC 101
#define delayD 677
#define delayE 401
#define delayF 151
#define delayG 409
#define delayH 31
#define delayI 641
#define delayJ 661
#define delayK 11
#define delayL 691
#define delayM 719
#define delayN 17
#define delayO 61
#define delayP 743
#define delayQ 89
#define delayR 659
#define delayS 5
#define delayT 547
#define delayU 769
#define delayV 421
#define delayW 47
#define delayX 521
#define delayY 163 // 80 ms, 778 seat theater. Scarcity, 1 in 94194

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
} KPlateD;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	KPlateD* kPlateD = (KPlateD*) calloc(1, sizeof(KPlateD));
	kPlateD->sampleRate = rate;
	return (LV2_Handle) kPlateD;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	KPlateD* kPlateD = (KPlateD*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			kPlateD->input[0] = (const float*) data;
			break;
		case INPUT_R:
			kPlateD->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			kPlateD->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			kPlateD->output[1] = (float*) data;
			break;
		case INPUT_PAD:
			kPlateD->inputPad = (const float*) data;
			break;
		case DAMPING:
			kPlateD->damping = (const float*) data;
			break;
		case LOW_CUT:
			kPlateD->lowCut = (const float*) data;
			break;
		case PREDELAY:
			kPlateD->predelayS = (const float*) data;
			break;
		case WETNESS:
			kPlateD->wetness = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	KPlateD* kPlateD = (KPlateD*) instance;

	kPlateD->iirAL = 0.0;
	kPlateD->iirBL = 0.0;

	kPlateD->iirAR = 0.0;
	kPlateD->iirBR = 0.0;

	kPlateD->gainIn = kPlateD->gainOutL = kPlateD->gainOutR = 1.0;

	for (int count = 0; count < delayA + 2; count++) {
		kPlateD->aAL[count] = 0.0;
		kPlateD->aAR[count] = 0.0;
	}
	for (int count = 0; count < delayB + 2; count++) {
		kPlateD->aBL[count] = 0.0;
		kPlateD->aBR[count] = 0.0;
	}
	for (int count = 0; count < delayC + 2; count++) {
		kPlateD->aCL[count] = 0.0;
		kPlateD->aCR[count] = 0.0;
	}
	for (int count = 0; count < delayD + 2; count++) {
		kPlateD->aDL[count] = 0.0;
		kPlateD->aDR[count] = 0.0;
	}
	for (int count = 0; count < delayE + 2; count++) {
		kPlateD->aEL[count] = 0.0;
		kPlateD->aER[count] = 0.0;
	}
	for (int count = 0; count < delayF + 2; count++) {
		kPlateD->aFL[count] = 0.0;
		kPlateD->aFR[count] = 0.0;
	}
	for (int count = 0; count < delayG + 2; count++) {
		kPlateD->aGL[count] = 0.0;
		kPlateD->aGR[count] = 0.0;
	}
	for (int count = 0; count < delayH + 2; count++) {
		kPlateD->aHL[count] = 0.0;
		kPlateD->aHR[count] = 0.0;
	}
	for (int count = 0; count < delayI + 2; count++) {
		kPlateD->aIL[count] = 0.0;
		kPlateD->aIR[count] = 0.0;
	}
	for (int count = 0; count < delayJ + 2; count++) {
		kPlateD->aJL[count] = 0.0;
		kPlateD->aJR[count] = 0.0;
	}
	for (int count = 0; count < delayK + 2; count++) {
		kPlateD->aKL[count] = 0.0;
		kPlateD->aKR[count] = 0.0;
	}
	for (int count = 0; count < delayL + 2; count++) {
		kPlateD->aLL[count] = 0.0;
		kPlateD->aLR[count] = 0.0;
	}
	for (int count = 0; count < delayM + 2; count++) {
		kPlateD->aML[count] = 0.0;
		kPlateD->aMR[count] = 0.0;
	}
	for (int count = 0; count < delayN + 2; count++) {
		kPlateD->aNL[count] = 0.0;
		kPlateD->aNR[count] = 0.0;
	}
	for (int count = 0; count < delayO + 2; count++) {
		kPlateD->aOL[count] = 0.0;
		kPlateD->aOR[count] = 0.0;
	}
	for (int count = 0; count < delayP + 2; count++) {
		kPlateD->aPL[count] = 0.0;
		kPlateD->aPR[count] = 0.0;
	}
	for (int count = 0; count < delayQ + 2; count++) {
		kPlateD->aQL[count] = 0.0;
		kPlateD->aQR[count] = 0.0;
	}
	for (int count = 0; count < delayR + 2; count++) {
		kPlateD->aRL[count] = 0.0;
		kPlateD->aRR[count] = 0.0;
	}
	for (int count = 0; count < delayS + 2; count++) {
		kPlateD->aSL[count] = 0.0;
		kPlateD->aSR[count] = 0.0;
	}
	for (int count = 0; count < delayT + 2; count++) {
		kPlateD->aTL[count] = 0.0;
		kPlateD->aTR[count] = 0.0;
	}
	for (int count = 0; count < delayU + 2; count++) {
		kPlateD->aUL[count] = 0.0;
		kPlateD->aUR[count] = 0.0;
	}
	for (int count = 0; count < delayV + 2; count++) {
		kPlateD->aVL[count] = 0.0;
		kPlateD->aVR[count] = 0.0;
	}
	for (int count = 0; count < delayW + 2; count++) {
		kPlateD->aWL[count] = 0.0;
		kPlateD->aWR[count] = 0.0;
	}
	for (int count = 0; count < delayX + 2; count++) {
		kPlateD->aXL[count] = 0.0;
		kPlateD->aXR[count] = 0.0;
	}
	for (int count = 0; count < delayY + 2; count++) {
		kPlateD->aYL[count] = 0.0;
		kPlateD->aYR[count] = 0.0;
	}

	for (int count = 0; count < earlyA + 2; count++) {
		kPlateD->eAL[count] = 0.0;
		kPlateD->eAR[count] = 0.0;
	}
	for (int count = 0; count < earlyB + 2; count++) {
		kPlateD->eBL[count] = 0.0;
		kPlateD->eBR[count] = 0.0;
	}
	for (int count = 0; count < earlyC + 2; count++) {
		kPlateD->eCL[count] = 0.0;
		kPlateD->eCR[count] = 0.0;
	}
	for (int count = 0; count < earlyD + 2; count++) {
		kPlateD->eDL[count] = 0.0;
		kPlateD->eDR[count] = 0.0;
	}
	for (int count = 0; count < earlyE + 2; count++) {
		kPlateD->eEL[count] = 0.0;
		kPlateD->eER[count] = 0.0;
	}
	for (int count = 0; count < earlyF + 2; count++) {
		kPlateD->eFL[count] = 0.0;
		kPlateD->eFR[count] = 0.0;
	}
	for (int count = 0; count < earlyG + 2; count++) {
		kPlateD->eGL[count] = 0.0;
		kPlateD->eGR[count] = 0.0;
	}
	for (int count = 0; count < earlyH + 2; count++) {
		kPlateD->eHL[count] = 0.0;
		kPlateD->eHR[count] = 0.0;
	}
	for (int count = 0; count < earlyI + 2; count++) {
		kPlateD->eIL[count] = 0.0;
		kPlateD->eIR[count] = 0.0;
	}

	for (int count = 0; count < predelay + 2; count++) {
		kPlateD->aZL[count] = 0.0;
		kPlateD->aZR[count] = 0.0;
	}

	kPlateD->feedbackAL = 0.0;
	kPlateD->feedbackBL = 0.0;
	kPlateD->feedbackCL = 0.0;
	kPlateD->feedbackDL = 0.0;
	kPlateD->feedbackEL = 0.0;

	kPlateD->previousAL = 0.0;
	kPlateD->previousBL = 0.0;
	kPlateD->previousCL = 0.0;
	kPlateD->previousDL = 0.0;
	kPlateD->previousEL = 0.0;

	kPlateD->feedbackER = 0.0;
	kPlateD->feedbackJR = 0.0;
	kPlateD->feedbackOR = 0.0;
	kPlateD->feedbackTR = 0.0;
	kPlateD->feedbackYR = 0.0;

	kPlateD->previousAR = 0.0;
	kPlateD->previousBR = 0.0;
	kPlateD->previousCR = 0.0;
	kPlateD->previousDR = 0.0;
	kPlateD->previousER = 0.0;

	kPlateD->prevMulchBL = 0.0;
	kPlateD->prevMulchBR = 0.0;
	kPlateD->prevMulchCL = 0.0;
	kPlateD->prevMulchCR = 0.0;
	kPlateD->prevMulchDL = 0.0;
	kPlateD->prevMulchDR = 0.0;
	kPlateD->prevMulchEL = 0.0;
	kPlateD->prevMulchER = 0.0;

	kPlateD->prevOutDL = 0.0;
	kPlateD->prevOutDR = 0.0;
	kPlateD->prevOutEL = 0.0;
	kPlateD->prevOutER = 0.0;

	kPlateD->prevInDL = 0.0;
	kPlateD->prevInDR = 0.0;
	kPlateD->prevInEL = 0.0;
	kPlateD->prevInER = 0.0;

	for (int count = 0; count < 6; count++) {
		kPlateD->lastRefL[count] = 0.0;
		kPlateD->lastRefR[count] = 0.0;
	}

	kPlateD->earlyAL = 1;
	kPlateD->earlyBL = 1;
	kPlateD->earlyCL = 1;
	kPlateD->earlyDL = 1;
	kPlateD->earlyEL = 1;
	kPlateD->earlyFL = 1;
	kPlateD->earlyGL = 1;
	kPlateD->earlyHL = 1;
	kPlateD->earlyIL = 1;

	kPlateD->earlyAR = 1;
	kPlateD->earlyBR = 1;
	kPlateD->earlyCR = 1;
	kPlateD->earlyDR = 1;
	kPlateD->earlyER = 1;
	kPlateD->earlyFR = 1;
	kPlateD->earlyGR = 1;
	kPlateD->earlyHR = 1;
	kPlateD->earlyIR = 1;

	kPlateD->countAL = 1;
	kPlateD->countBL = 1;
	kPlateD->countCL = 1;
	kPlateD->countDL = 1;
	kPlateD->countEL = 1;
	kPlateD->countFL = 1;
	kPlateD->countGL = 1;
	kPlateD->countHL = 1;
	kPlateD->countIL = 1;
	kPlateD->countJL = 1;
	kPlateD->countKL = 1;
	kPlateD->countLL = 1;
	kPlateD->countML = 1;
	kPlateD->countNL = 1;
	kPlateD->countOL = 1;
	kPlateD->countPL = 1;
	kPlateD->countQL = 1;
	kPlateD->countRL = 1;
	kPlateD->countSL = 1;
	kPlateD->countTL = 1;
	kPlateD->countUL = 1;
	kPlateD->countVL = 1;
	kPlateD->countWL = 1;
	kPlateD->countXL = 1;
	kPlateD->countYL = 1;

	kPlateD->countAR = 1;
	kPlateD->countBR = 1;
	kPlateD->countCR = 1;
	kPlateD->countDR = 1;
	kPlateD->countER = 1;
	kPlateD->countFR = 1;
	kPlateD->countGR = 1;
	kPlateD->countHR = 1;
	kPlateD->countIR = 1;
	kPlateD->countJR = 1;
	kPlateD->countKR = 1;
	kPlateD->countLR = 1;
	kPlateD->countMR = 1;
	kPlateD->countNR = 1;
	kPlateD->countOR = 1;
	kPlateD->countPR = 1;
	kPlateD->countQR = 1;
	kPlateD->countRR = 1;
	kPlateD->countSR = 1;
	kPlateD->countTR = 1;
	kPlateD->countUR = 1;
	kPlateD->countVR = 1;
	kPlateD->countWR = 1;
	kPlateD->countXR = 1;
	kPlateD->countYR = 1;

	kPlateD->countZ = 1;

	kPlateD->cycle = 0;

	for (int x = 0; x < fix_total; x++) {
		kPlateD->fixA[x] = 0.0;
		kPlateD->fixB[x] = 0.0;
		kPlateD->fixC[x] = 0.0;
		kPlateD->fixD[x] = 0.0;
	}
	// from ZBandpass, so I can use enums with it

	kPlateD->fpdL = 1.0;
	while (kPlateD->fpdL < 16386) kPlateD->fpdL = rand() * UINT32_MAX;
	kPlateD->fpdR = 1.0;
	while (kPlateD->fpdR < 16386) kPlateD->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	KPlateD* kPlateD = (KPlateD*) instance;

	const float* in1 = kPlateD->input[0];
	const float* in2 = kPlateD->input[1];
	float* out1 = kPlateD->output[0];
	float* out2 = kPlateD->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= kPlateD->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (kPlateD->cycle > cycleEnd - 1) kPlateD->cycle = cycleEnd - 1; // sanity check

	double downRate = kPlateD->sampleRate / cycleEnd;
	// we now shift gears between 44.1k and 48k so our tone is the same, slight changes in delay times

	double inputPad = *kPlateD->inputPad;
	double regen = ((*kPlateD->damping / 10.0) * 0.425) + 0.16;
	regen = (regen * 0.0001) + 0.00024;
	double iirAmount = ((*kPlateD->lowCut / 3.0) * 0.3) + 0.04;
	iirAmount = (iirAmount * 1000.0) / downRate;
	double earlyVolume = *kPlateD->predelayS; // predelay to a half-second
	int adjPredelay = (downRate * earlyVolume);
	double wet = *kPlateD->wetness * 2.0;
	double dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	// this reverb makes 50% full dry AND full wet, not crossfaded.
	// that's so it can be on submixes without cutting back dry channel when adjusted:
	// unless you go super heavy, you are only adjusting the added verb loudness.

	kPlateD->fixA[fix_freq] = 20.0 / downRate;
	kPlateD->fixA[fix_reso] = 0.0018769;
	kPlateD->fixD[fix_freq] = 14.0 / downRate;
	kPlateD->fixD[fix_reso] = 0.0024964;
	kPlateD->fixB[fix_freq] = (kPlateD->fixA[fix_freq] + kPlateD->fixA[fix_freq] + kPlateD->fixD[fix_freq]) / 3.0;
	kPlateD->fixB[fix_reso] = 0.0020834;
	kPlateD->fixC[fix_freq] = (kPlateD->fixA[fix_freq] + kPlateD->fixD[fix_freq] + kPlateD->fixD[fix_freq]) / 3.0;
	kPlateD->fixC[fix_reso] = 0.0022899;

	double K = tan(M_PI * kPlateD->fixA[fix_freq]);
	double norm = 1.0 / (1.0 + K / kPlateD->fixA[fix_reso] + K * K);
	kPlateD->fixA[fix_a0] = K / kPlateD->fixA[fix_reso] * norm;
	kPlateD->fixA[fix_a1] = 0.0;
	kPlateD->fixA[fix_a2] = -kPlateD->fixA[fix_a0];
	kPlateD->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateD->fixA[fix_b2] = (1.0 - K / kPlateD->fixA[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateD->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateD->fixB[fix_reso] + K * K);
	kPlateD->fixB[fix_a0] = K / kPlateD->fixB[fix_reso] * norm;
	kPlateD->fixB[fix_a1] = 0.0;
	kPlateD->fixB[fix_a2] = -kPlateD->fixB[fix_a0];
	kPlateD->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateD->fixB[fix_b2] = (1.0 - K / kPlateD->fixB[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateD->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateD->fixC[fix_reso] + K * K);
	kPlateD->fixC[fix_a0] = K / kPlateD->fixC[fix_reso] * norm;
	kPlateD->fixC[fix_a1] = 0.0;
	kPlateD->fixC[fix_a2] = -kPlateD->fixC[fix_a0];
	kPlateD->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateD->fixC[fix_b2] = (1.0 - K / kPlateD->fixC[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateD->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateD->fixD[fix_reso] + K * K);
	kPlateD->fixD[fix_a0] = K / kPlateD->fixD[fix_reso] * norm;
	kPlateD->fixD[fix_a1] = 0.0;
	kPlateD->fixD[fix_a2] = -kPlateD->fixD[fix_a0];
	kPlateD->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateD->fixD[fix_b2] = (1.0 - K / kPlateD->fixD[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = kPlateD->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = kPlateD->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		kPlateD->cycle++;
		if (kPlateD->cycle == cycleEnd) { // hit the end point and we do a reverb sample
			if (inputPad < 1.0) {
				inputSampleL *= inputPad;
				inputSampleR *= inputPad;
			}
			double outSample;
			outSample = (inputSampleL + kPlateD->prevInDL) * 0.5;
			kPlateD->prevInDL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateD->prevInDR) * 0.5;
			kPlateD->prevInDR = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
			if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
			if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
			if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
			inputSampleL = sin(inputSampleL);
			inputSampleR = sin(inputSampleR);
			// amplitude aspect

			kPlateD->iirAL = (kPlateD->iirAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateD->iirAL;
			kPlateD->iirAR = (kPlateD->iirAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateD->iirAR;
			// 600hz highpass on input

			inputSampleL *= 0.5;
			inputSampleR *= 0.5;
			if (kPlateD->gainIn < 0.0078125) kPlateD->gainIn = 0.0078125;
			if (kPlateD->gainIn > 1.0) kPlateD->gainIn = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateD->gainIn;
			inputSampleR *= kPlateD->gainIn;
			kPlateD->gainIn += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateD->gainIn += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			// inputSampleL *= 2.0; inputSampleR *= 2.0;
			// curve! To get a compressed effect that matches a certain other plugin
			// that is too overprocessed for its own good :)

			kPlateD->iirBL = (kPlateD->iirBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateD->iirBL;
			kPlateD->iirBR = (kPlateD->iirBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateD->iirBR;
			// 600hz highpass on input

			outSample = (inputSampleL + kPlateD->prevInEL) * 0.5;
			kPlateD->prevInEL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateD->prevInER) * 0.5;
			kPlateD->prevInER = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			// begin allpasses
			double oeAL = inputSampleL - (kPlateD->eAL[(kPlateD->earlyAL + 1) - ((kPlateD->earlyAL + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeBL = inputSampleL - (kPlateD->eBL[(kPlateD->earlyBL + 1) - ((kPlateD->earlyBL + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeCL = inputSampleL - (kPlateD->eCL[(kPlateD->earlyCL + 1) - ((kPlateD->earlyCL + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeCR = inputSampleR - (kPlateD->eCR[(kPlateD->earlyCR + 1) - ((kPlateD->earlyCR + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeFR = inputSampleR - (kPlateD->eFR[(kPlateD->earlyFR + 1) - ((kPlateD->earlyFR + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeIR = inputSampleR - (kPlateD->eIR[(kPlateD->earlyIR + 1) - ((kPlateD->earlyIR + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);

			kPlateD->eAL[kPlateD->earlyAL] = oeAL;
			oeAL *= 0.5;
			kPlateD->eBL[kPlateD->earlyBL] = oeBL;
			oeBL *= 0.5;
			kPlateD->eCL[kPlateD->earlyCL] = oeCL;
			oeCL *= 0.5;
			kPlateD->eCR[kPlateD->earlyCR] = oeCR;
			oeCR *= 0.5;
			kPlateD->eFR[kPlateD->earlyFR] = oeFR;
			oeFR *= 0.5;
			kPlateD->eIR[kPlateD->earlyIR] = oeIR;
			oeIR *= 0.5;

			kPlateD->earlyAL++;
			if (kPlateD->earlyAL < 0 || kPlateD->earlyAL > earlyA) kPlateD->earlyAL = 0;
			kPlateD->earlyBL++;
			if (kPlateD->earlyBL < 0 || kPlateD->earlyBL > earlyB) kPlateD->earlyBL = 0;
			kPlateD->earlyCL++;
			if (kPlateD->earlyCL < 0 || kPlateD->earlyCL > earlyC) kPlateD->earlyCL = 0;
			kPlateD->earlyCR++;
			if (kPlateD->earlyCR < 0 || kPlateD->earlyCR > earlyC) kPlateD->earlyCR = 0;
			kPlateD->earlyFR++;
			if (kPlateD->earlyFR < 0 || kPlateD->earlyFR > earlyF) kPlateD->earlyFR = 0;
			kPlateD->earlyIR++;
			if (kPlateD->earlyIR < 0 || kPlateD->earlyIR > earlyI) kPlateD->earlyIR = 0;

			oeAL += kPlateD->eAL[kPlateD->earlyAL - ((kPlateD->earlyAL > earlyA) ? earlyA + 1 : 0)];
			oeBL += kPlateD->eBL[kPlateD->earlyBL - ((kPlateD->earlyBL > earlyB) ? earlyB + 1 : 0)];
			oeCL += kPlateD->eCL[kPlateD->earlyCL - ((kPlateD->earlyCL > earlyC) ? earlyC + 1 : 0)];
			oeCR += kPlateD->eCR[kPlateD->earlyCR - ((kPlateD->earlyCR > earlyC) ? earlyC + 1 : 0)];
			oeFR += kPlateD->eFR[kPlateD->earlyFR - ((kPlateD->earlyFR > earlyF) ? earlyF + 1 : 0)];
			oeIR += kPlateD->eIR[kPlateD->earlyIR - ((kPlateD->earlyIR > earlyI) ? earlyI + 1 : 0)];

			double oeDL = ((oeBL + oeCL) - oeAL) - (kPlateD->eDL[(kPlateD->earlyDL + 1) - ((kPlateD->earlyDL + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeEL = ((oeAL + oeCL) - oeBL) - (kPlateD->eEL[(kPlateD->earlyEL + 1) - ((kPlateD->earlyEL + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeFL = ((oeAL + oeBL) - oeCL) - (kPlateD->eFL[(kPlateD->earlyFL + 1) - ((kPlateD->earlyFL + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeBR = ((oeFR + oeIR) - oeCR) - (kPlateD->eBR[(kPlateD->earlyBR + 1) - ((kPlateD->earlyBR + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeER = ((oeCR + oeIR) - oeFR) - (kPlateD->eER[(kPlateD->earlyER + 1) - ((kPlateD->earlyER + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeHR = ((oeCR + oeFR) - oeIR) - (kPlateD->eHR[(kPlateD->earlyHR + 1) - ((kPlateD->earlyHR + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);

			kPlateD->eDL[kPlateD->earlyDL] = oeDL;
			oeDL *= 0.5;
			kPlateD->eEL[kPlateD->earlyEL] = oeEL;
			oeEL *= 0.5;
			kPlateD->eFL[kPlateD->earlyFL] = oeFL;
			oeFL *= 0.5;
			kPlateD->eBR[kPlateD->earlyBR] = oeBR;
			oeBR *= 0.5;
			kPlateD->eER[kPlateD->earlyER] = oeER;
			oeER *= 0.5;
			kPlateD->eHR[kPlateD->earlyHR] = oeHR;
			oeHR *= 0.5;

			kPlateD->earlyDL++;
			if (kPlateD->earlyDL < 0 || kPlateD->earlyDL > earlyD) kPlateD->earlyDL = 0;
			kPlateD->earlyEL++;
			if (kPlateD->earlyEL < 0 || kPlateD->earlyEL > earlyE) kPlateD->earlyEL = 0;
			kPlateD->earlyFL++;
			if (kPlateD->earlyFL < 0 || kPlateD->earlyFL > earlyF) kPlateD->earlyFL = 0;
			kPlateD->earlyBR++;
			if (kPlateD->earlyBR < 0 || kPlateD->earlyBR > earlyB) kPlateD->earlyBR = 0;
			kPlateD->earlyER++;
			if (kPlateD->earlyER < 0 || kPlateD->earlyER > earlyE) kPlateD->earlyER = 0;
			kPlateD->earlyHR++;
			if (kPlateD->earlyHR < 0 || kPlateD->earlyHR > earlyH) kPlateD->earlyHR = 0;

			oeDL += kPlateD->eDL[kPlateD->earlyDL - ((kPlateD->earlyDL > earlyD) ? earlyD + 1 : 0)];
			oeEL += kPlateD->eEL[kPlateD->earlyEL - ((kPlateD->earlyEL > earlyE) ? earlyE + 1 : 0)];
			oeFL += kPlateD->eFL[kPlateD->earlyFL - ((kPlateD->earlyFL > earlyF) ? earlyF + 1 : 0)];
			oeBR += kPlateD->eBR[kPlateD->earlyBR - ((kPlateD->earlyBR > earlyB) ? earlyB + 1 : 0)];
			oeER += kPlateD->eER[kPlateD->earlyER - ((kPlateD->earlyER > earlyE) ? earlyE + 1 : 0)];
			oeHR += kPlateD->eHR[kPlateD->earlyHR - ((kPlateD->earlyHR > earlyH) ? earlyH + 1 : 0)];

			double oeGL = ((oeEL + oeFL) - oeDL) - (kPlateD->eGL[(kPlateD->earlyGL + 1) - ((kPlateD->earlyGL + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);
			double oeHL = ((oeDL + oeFL) - oeEL) - (kPlateD->eHL[(kPlateD->earlyHL + 1) - ((kPlateD->earlyHL + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);
			double oeIL = ((oeDL + oeEL) - oeFL) - (kPlateD->eIL[(kPlateD->earlyIL + 1) - ((kPlateD->earlyIL + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);
			double oeAR = ((oeER + oeHR) - oeBR) - (kPlateD->eAR[(kPlateD->earlyAR + 1) - ((kPlateD->earlyAR + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeDR = ((oeBR + oeHR) - oeER) - (kPlateD->eDR[(kPlateD->earlyDR + 1) - ((kPlateD->earlyDR + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeGR = ((oeBR + oeER) - oeHR) - (kPlateD->eGR[(kPlateD->earlyGR + 1) - ((kPlateD->earlyGR + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);

			kPlateD->eGL[kPlateD->earlyGL] = oeGL;
			oeGL *= 0.5;
			kPlateD->eHL[kPlateD->earlyHL] = oeHL;
			oeHL *= 0.5;
			kPlateD->eIL[kPlateD->earlyIL] = oeIL;
			oeIL *= 0.5;
			kPlateD->eAR[kPlateD->earlyAR] = oeAR;
			oeAR *= 0.5;
			kPlateD->eDR[kPlateD->earlyDR] = oeDR;
			oeDR *= 0.5;
			kPlateD->eGR[kPlateD->earlyGR] = oeGR;
			oeGR *= 0.5;

			kPlateD->earlyGL++;
			if (kPlateD->earlyGL < 0 || kPlateD->earlyGL > earlyG) kPlateD->earlyGL = 0;
			kPlateD->earlyHL++;
			if (kPlateD->earlyHL < 0 || kPlateD->earlyHL > earlyH) kPlateD->earlyHL = 0;
			kPlateD->earlyIL++;
			if (kPlateD->earlyIL < 0 || kPlateD->earlyIL > earlyI) kPlateD->earlyIL = 0;
			kPlateD->earlyAR++;
			if (kPlateD->earlyAR < 0 || kPlateD->earlyAR > earlyA) kPlateD->earlyAR = 0;
			kPlateD->earlyDR++;
			if (kPlateD->earlyDR < 0 || kPlateD->earlyDR > earlyD) kPlateD->earlyDR = 0;
			kPlateD->earlyGR++;
			if (kPlateD->earlyGR < 0 || kPlateD->earlyGR > earlyG) kPlateD->earlyGR = 0;

			oeGL += kPlateD->eGL[kPlateD->earlyGL - ((kPlateD->earlyGL > earlyG) ? earlyG + 1 : 0)];
			oeHL += kPlateD->eHL[kPlateD->earlyHL - ((kPlateD->earlyHL > earlyH) ? earlyH + 1 : 0)];
			oeIL += kPlateD->eIL[kPlateD->earlyIL - ((kPlateD->earlyIL > earlyI) ? earlyI + 1 : 0)];
			oeAR += kPlateD->eAR[kPlateD->earlyAR - ((kPlateD->earlyAR > earlyA) ? earlyA + 1 : 0)];
			oeDR += kPlateD->eDR[kPlateD->earlyDR - ((kPlateD->earlyDR > earlyD) ? earlyD + 1 : 0)];
			oeGR += kPlateD->eGR[kPlateD->earlyGR - ((kPlateD->earlyGR > earlyG) ? earlyG + 1 : 0)];

			// allpasses predelay
			kPlateD->aZL[kPlateD->countZ] = (oeGL + oeHL + oeIL) * 0.25;
			kPlateD->aZR[kPlateD->countZ] = (oeAR + oeDR + oeGR) * 0.25;
			kPlateD->countZ++;
			if (kPlateD->countZ < 0 || kPlateD->countZ > adjPredelay) kPlateD->countZ = 0;
			inputSampleL = kPlateD->aZL[kPlateD->countZ - ((kPlateD->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			inputSampleR = kPlateD->aZR[kPlateD->countZ - ((kPlateD->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			// end allpasses

			kPlateD->aAL[kPlateD->countAL] = inputSampleL + (kPlateD->feedbackAL * regen);
			kPlateD->aBL[kPlateD->countBL] = inputSampleL + (kPlateD->feedbackBL * regen);
			kPlateD->aCL[kPlateD->countCL] = inputSampleL + (kPlateD->feedbackCL * regen);
			kPlateD->aDL[kPlateD->countDL] = inputSampleL + (kPlateD->feedbackDL * regen);
			kPlateD->aEL[kPlateD->countEL] = inputSampleL + (kPlateD->feedbackEL * regen);

			kPlateD->aER[kPlateD->countER] = inputSampleR + (kPlateD->feedbackER * regen);
			kPlateD->aJR[kPlateD->countJR] = inputSampleR + (kPlateD->feedbackJR * regen);
			kPlateD->aOR[kPlateD->countOR] = inputSampleR + (kPlateD->feedbackOR * regen);
			kPlateD->aTR[kPlateD->countTR] = inputSampleR + (kPlateD->feedbackTR * regen);
			kPlateD->aYR[kPlateD->countYR] = inputSampleR + (kPlateD->feedbackYR * regen);

			kPlateD->countAL++;
			if (kPlateD->countAL < 0 || kPlateD->countAL > delayA) kPlateD->countAL = 0;
			kPlateD->countBL++;
			if (kPlateD->countBL < 0 || kPlateD->countBL > delayB) kPlateD->countBL = 0;
			kPlateD->countCL++;
			if (kPlateD->countCL < 0 || kPlateD->countCL > delayC) kPlateD->countCL = 0;
			kPlateD->countDL++;
			if (kPlateD->countDL < 0 || kPlateD->countDL > delayD) kPlateD->countDL = 0;
			kPlateD->countEL++;
			if (kPlateD->countEL < 0 || kPlateD->countEL > delayE) kPlateD->countEL = 0;

			kPlateD->countER++;
			if (kPlateD->countER < 0 || kPlateD->countER > delayE) kPlateD->countER = 0;
			kPlateD->countJR++;
			if (kPlateD->countJR < 0 || kPlateD->countJR > delayJ) kPlateD->countJR = 0;
			kPlateD->countOR++;
			if (kPlateD->countOR < 0 || kPlateD->countOR > delayO) kPlateD->countOR = 0;
			kPlateD->countTR++;
			if (kPlateD->countTR < 0 || kPlateD->countTR > delayT) kPlateD->countTR = 0;
			kPlateD->countYR++;
			if (kPlateD->countYR < 0 || kPlateD->countYR > delayY) kPlateD->countYR = 0;

			double outAL = kPlateD->aAL[kPlateD->countAL - ((kPlateD->countAL > delayA) ? delayA + 1 : 0)];
			double outBL = kPlateD->aBL[kPlateD->countBL - ((kPlateD->countBL > delayB) ? delayB + 1 : 0)];
			double outCL = kPlateD->aCL[kPlateD->countCL - ((kPlateD->countCL > delayC) ? delayC + 1 : 0)];
			double outDL = kPlateD->aDL[kPlateD->countDL - ((kPlateD->countDL > delayD) ? delayD + 1 : 0)];
			double outEL = kPlateD->aEL[kPlateD->countEL - ((kPlateD->countEL > delayE) ? delayE + 1 : 0)];

			double outER = kPlateD->aER[kPlateD->countER - ((kPlateD->countER > delayE) ? delayE + 1 : 0)];
			double outJR = kPlateD->aJR[kPlateD->countJR - ((kPlateD->countJR > delayJ) ? delayJ + 1 : 0)];
			double outOR = kPlateD->aOR[kPlateD->countOR - ((kPlateD->countOR > delayO) ? delayO + 1 : 0)];
			double outTR = kPlateD->aTR[kPlateD->countTR - ((kPlateD->countTR > delayT) ? delayT + 1 : 0)];
			double outYR = kPlateD->aYR[kPlateD->countYR - ((kPlateD->countYR > delayY) ? delayY + 1 : 0)];

			//-------- one

			outSample = (outAL * kPlateD->fixA[fix_a0]) + kPlateD->fixA[fix_sL1];
			kPlateD->fixA[fix_sL1] = (outAL * kPlateD->fixA[fix_a1]) - (outSample * kPlateD->fixA[fix_b1]) + kPlateD->fixA[fix_sL2];
			kPlateD->fixA[fix_sL2] = (outAL * kPlateD->fixA[fix_a2]) - (outSample * kPlateD->fixA[fix_b2]);
			outAL = outSample; // fixed biquad

			outSample = (outER * kPlateD->fixA[fix_a0]) + kPlateD->fixA[fix_sR1];
			kPlateD->fixA[fix_sR1] = (outER * kPlateD->fixA[fix_a1]) - (outSample * kPlateD->fixA[fix_b1]) + kPlateD->fixA[fix_sR2];
			kPlateD->fixA[fix_sR2] = (outER * kPlateD->fixA[fix_a2]) - (outSample * kPlateD->fixA[fix_b2]);
			outER = outSample; // fixed biquad

			//-------- filtered (one path in five, feeding the rest of the matrix

			kPlateD->aFL[kPlateD->countFL] = ((outAL * 3.0) - ((outBL + outCL + outDL + outEL) * 2.0));
			kPlateD->aGL[kPlateD->countGL] = ((outBL * 3.0) - ((outAL + outCL + outDL + outEL) * 2.0));
			kPlateD->aHL[kPlateD->countHL] = ((outCL * 3.0) - ((outAL + outBL + outDL + outEL) * 2.0));
			kPlateD->aIL[kPlateD->countIL] = ((outDL * 3.0) - ((outAL + outBL + outCL + outEL) * 2.0));
			kPlateD->aJL[kPlateD->countJL] = ((outEL * 3.0) - ((outAL + outBL + outCL + outDL) * 2.0));

			kPlateD->aDR[kPlateD->countDR] = ((outER * 3.0) - ((outJR + outOR + outTR + outYR) * 2.0));
			kPlateD->aIR[kPlateD->countIR] = ((outJR * 3.0) - ((outER + outOR + outTR + outYR) * 2.0));
			kPlateD->aNR[kPlateD->countNR] = ((outOR * 3.0) - ((outER + outJR + outTR + outYR) * 2.0));
			kPlateD->aSR[kPlateD->countSR] = ((outTR * 3.0) - ((outER + outJR + outOR + outYR) * 2.0));
			kPlateD->aXR[kPlateD->countXR] = ((outYR * 3.0) - ((outER + outJR + outOR + outTR) * 2.0));

			kPlateD->countFL++;
			if (kPlateD->countFL < 0 || kPlateD->countFL > delayF) kPlateD->countFL = 0;
			kPlateD->countGL++;
			if (kPlateD->countGL < 0 || kPlateD->countGL > delayG) kPlateD->countGL = 0;
			kPlateD->countHL++;
			if (kPlateD->countHL < 0 || kPlateD->countHL > delayH) kPlateD->countHL = 0;
			kPlateD->countIL++;
			if (kPlateD->countIL < 0 || kPlateD->countIL > delayI) kPlateD->countIL = 0;
			kPlateD->countJL++;
			if (kPlateD->countJL < 0 || kPlateD->countJL > delayJ) kPlateD->countJL = 0;

			kPlateD->countDR++;
			if (kPlateD->countDR < 0 || kPlateD->countDR > delayD) kPlateD->countDR = 0;
			kPlateD->countIR++;
			if (kPlateD->countIR < 0 || kPlateD->countIR > delayI) kPlateD->countIR = 0;
			kPlateD->countNR++;
			if (kPlateD->countNR < 0 || kPlateD->countNR > delayN) kPlateD->countNR = 0;
			kPlateD->countSR++;
			if (kPlateD->countSR < 0 || kPlateD->countSR > delayS) kPlateD->countSR = 0;
			kPlateD->countXR++;
			if (kPlateD->countXR < 0 || kPlateD->countXR > delayX) kPlateD->countXR = 0;

			double outFL = kPlateD->aFL[kPlateD->countFL - ((kPlateD->countFL > delayF) ? delayF + 1 : 0)];
			double outGL = kPlateD->aGL[kPlateD->countGL - ((kPlateD->countGL > delayG) ? delayG + 1 : 0)];
			double outHL = kPlateD->aHL[kPlateD->countHL - ((kPlateD->countHL > delayH) ? delayH + 1 : 0)];
			double outIL = kPlateD->aIL[kPlateD->countIL - ((kPlateD->countIL > delayI) ? delayI + 1 : 0)];
			double outJL = kPlateD->aJL[kPlateD->countJL - ((kPlateD->countJL > delayJ) ? delayJ + 1 : 0)];

			double outDR = kPlateD->aDR[kPlateD->countDR - ((kPlateD->countDR > delayD) ? delayD + 1 : 0)];
			double outIR = kPlateD->aIR[kPlateD->countIR - ((kPlateD->countIR > delayI) ? delayI + 1 : 0)];
			double outNR = kPlateD->aNR[kPlateD->countNR - ((kPlateD->countNR > delayN) ? delayN + 1 : 0)];
			double outSR = kPlateD->aSR[kPlateD->countSR - ((kPlateD->countSR > delayS) ? delayS + 1 : 0)];
			double outXR = kPlateD->aXR[kPlateD->countXR - ((kPlateD->countXR > delayX) ? delayX + 1 : 0)];

			//-------- mulch

			outSample = (outFL * kPlateD->fixB[fix_a0]) + kPlateD->fixB[fix_sL1];
			kPlateD->fixB[fix_sL1] = (outFL * kPlateD->fixB[fix_a1]) - (outSample * kPlateD->fixB[fix_b1]) + kPlateD->fixB[fix_sL2];
			kPlateD->fixB[fix_sL2] = (outFL * kPlateD->fixB[fix_a2]) - (outSample * kPlateD->fixB[fix_b2]);
			outFL = outSample; // fixed biquad

			outSample = (outDR * kPlateD->fixB[fix_a0]) + kPlateD->fixB[fix_sR1];
			kPlateD->fixB[fix_sR1] = (outDR * kPlateD->fixB[fix_a1]) - (outSample * kPlateD->fixB[fix_b1]) + kPlateD->fixB[fix_sR2];
			kPlateD->fixB[fix_sR2] = (outDR * kPlateD->fixB[fix_a2]) - (outSample * kPlateD->fixB[fix_b2]);
			outDR = outSample; // fixed biquad

			outSample = (outGL + kPlateD->prevMulchBL) * 0.5;
			kPlateD->prevMulchBL = outGL;
			outGL = outSample;
			outSample = (outIR + kPlateD->prevMulchBR) * 0.5;
			kPlateD->prevMulchBR = outIR;
			outIR = outSample;

			//-------- two

			kPlateD->aKL[kPlateD->countKL] = ((outFL * 3.0) - ((outGL + outHL + outIL + outJL) * 2.0));
			kPlateD->aLL[kPlateD->countLL] = ((outGL * 3.0) - ((outFL + outHL + outIL + outJL) * 2.0));
			kPlateD->aML[kPlateD->countML] = ((outHL * 3.0) - ((outFL + outGL + outIL + outJL) * 2.0));
			kPlateD->aNL[kPlateD->countNL] = ((outIL * 3.0) - ((outFL + outGL + outHL + outJL) * 2.0));
			kPlateD->aOL[kPlateD->countOL] = ((outJL * 3.0) - ((outFL + outGL + outHL + outIL) * 2.0));

			kPlateD->aCR[kPlateD->countCR] = ((outDR * 3.0) - ((outIR + outNR + outSR + outXR) * 2.0));
			kPlateD->aHR[kPlateD->countHR] = ((outIR * 3.0) - ((outDR + outNR + outSR + outXR) * 2.0));
			kPlateD->aMR[kPlateD->countMR] = ((outNR * 3.0) - ((outDR + outIR + outSR + outXR) * 2.0));
			kPlateD->aRR[kPlateD->countRR] = ((outSR * 3.0) - ((outDR + outIR + outNR + outXR) * 2.0));
			kPlateD->aWR[kPlateD->countWR] = ((outXR * 3.0) - ((outDR + outIR + outNR + outSR) * 2.0));

			kPlateD->countKL++;
			if (kPlateD->countKL < 0 || kPlateD->countKL > delayK) kPlateD->countKL = 0;
			kPlateD->countLL++;
			if (kPlateD->countLL < 0 || kPlateD->countLL > delayL) kPlateD->countLL = 0;
			kPlateD->countML++;
			if (kPlateD->countML < 0 || kPlateD->countML > delayM) kPlateD->countML = 0;
			kPlateD->countNL++;
			if (kPlateD->countNL < 0 || kPlateD->countNL > delayN) kPlateD->countNL = 0;
			kPlateD->countOL++;
			if (kPlateD->countOL < 0 || kPlateD->countOL > delayO) kPlateD->countOL = 0;

			kPlateD->countCR++;
			if (kPlateD->countCR < 0 || kPlateD->countCR > delayC) kPlateD->countCR = 0;
			kPlateD->countHR++;
			if (kPlateD->countHR < 0 || kPlateD->countHR > delayH) kPlateD->countHR = 0;
			kPlateD->countMR++;
			if (kPlateD->countMR < 0 || kPlateD->countMR > delayM) kPlateD->countMR = 0;
			kPlateD->countRR++;
			if (kPlateD->countRR < 0 || kPlateD->countRR > delayR) kPlateD->countRR = 0;
			kPlateD->countWR++;
			if (kPlateD->countWR < 0 || kPlateD->countWR > delayW) kPlateD->countWR = 0;

			double outKL = kPlateD->aKL[kPlateD->countKL - ((kPlateD->countKL > delayK) ? delayK + 1 : 0)];
			double outLL = kPlateD->aLL[kPlateD->countLL - ((kPlateD->countLL > delayL) ? delayL + 1 : 0)];
			double outML = kPlateD->aML[kPlateD->countML - ((kPlateD->countML > delayM) ? delayM + 1 : 0)];
			double outNL = kPlateD->aNL[kPlateD->countNL - ((kPlateD->countNL > delayN) ? delayN + 1 : 0)];
			double outOL = kPlateD->aOL[kPlateD->countOL - ((kPlateD->countOL > delayO) ? delayO + 1 : 0)];

			double outCR = kPlateD->aCR[kPlateD->countCR - ((kPlateD->countCR > delayC) ? delayC + 1 : 0)];
			double outHR = kPlateD->aHR[kPlateD->countHR - ((kPlateD->countHR > delayH) ? delayH + 1 : 0)];
			double outMR = kPlateD->aMR[kPlateD->countMR - ((kPlateD->countMR > delayM) ? delayM + 1 : 0)];
			double outRR = kPlateD->aRR[kPlateD->countRR - ((kPlateD->countRR > delayR) ? delayR + 1 : 0)];
			double outWR = kPlateD->aWR[kPlateD->countWR - ((kPlateD->countWR > delayW) ? delayW + 1 : 0)];

			//-------- mulch

			outSample = (outKL * kPlateD->fixC[fix_a0]) + kPlateD->fixC[fix_sL1];
			kPlateD->fixC[fix_sL1] = (outKL * kPlateD->fixC[fix_a1]) - (outSample * kPlateD->fixC[fix_b1]) + kPlateD->fixC[fix_sL2];
			kPlateD->fixC[fix_sL2] = (outKL * kPlateD->fixC[fix_a2]) - (outSample * kPlateD->fixC[fix_b2]);
			outKL = outSample; // fixed biquad

			outSample = (outCR * kPlateD->fixC[fix_a0]) + kPlateD->fixC[fix_sR1];
			kPlateD->fixC[fix_sR1] = (outCR * kPlateD->fixC[fix_a1]) - (outSample * kPlateD->fixC[fix_b1]) + kPlateD->fixC[fix_sR2];
			kPlateD->fixC[fix_sR2] = (outCR * kPlateD->fixC[fix_a2]) - (outSample * kPlateD->fixC[fix_b2]);
			outCR = outSample; // fixed biquad

			outSample = (outLL + kPlateD->prevMulchCL) * 0.5;
			kPlateD->prevMulchCL = outLL;
			outLL = outSample;
			outSample = (outHR + kPlateD->prevMulchCR) * 0.5;
			kPlateD->prevMulchCR = outHR;
			outHR = outSample;

			//-------- three

			kPlateD->aPL[kPlateD->countPL] = ((outKL * 3.0) - ((outLL + outML + outNL + outOL) * 2.0));
			kPlateD->aQL[kPlateD->countQL] = ((outLL * 3.0) - ((outKL + outML + outNL + outOL) * 2.0));
			kPlateD->aRL[kPlateD->countRL] = ((outML * 3.0) - ((outKL + outLL + outNL + outOL) * 2.0));
			kPlateD->aSL[kPlateD->countSL] = ((outNL * 3.0) - ((outKL + outLL + outML + outOL) * 2.0));
			kPlateD->aTL[kPlateD->countTL] = ((outOL * 3.0) - ((outKL + outLL + outML + outNL) * 2.0));

			kPlateD->aBR[kPlateD->countBR] = ((outCR * 3.0) - ((outHR + outMR + outRR + outWR) * 2.0));
			kPlateD->aGR[kPlateD->countGR] = ((outHR * 3.0) - ((outCR + outMR + outRR + outWR) * 2.0));
			kPlateD->aLR[kPlateD->countLR] = ((outMR * 3.0) - ((outCR + outHR + outRR + outWR) * 2.0));
			kPlateD->aQR[kPlateD->countQR] = ((outRR * 3.0) - ((outCR + outHR + outMR + outWR) * 2.0));
			kPlateD->aVR[kPlateD->countVR] = ((outWR * 3.0) - ((outCR + outHR + outMR + outRR) * 2.0));

			kPlateD->countPL++;
			if (kPlateD->countPL < 0 || kPlateD->countPL > delayP) kPlateD->countPL = 0;
			kPlateD->countQL++;
			if (kPlateD->countQL < 0 || kPlateD->countQL > delayQ) kPlateD->countQL = 0;
			kPlateD->countRL++;
			if (kPlateD->countRL < 0 || kPlateD->countRL > delayR) kPlateD->countRL = 0;
			kPlateD->countSL++;
			if (kPlateD->countSL < 0 || kPlateD->countSL > delayS) kPlateD->countSL = 0;
			kPlateD->countTL++;
			if (kPlateD->countTL < 0 || kPlateD->countTL > delayT) kPlateD->countTL = 0;

			kPlateD->countBR++;
			if (kPlateD->countBR < 0 || kPlateD->countBR > delayB) kPlateD->countBR = 0;
			kPlateD->countGR++;
			if (kPlateD->countGR < 0 || kPlateD->countGR > delayG) kPlateD->countGR = 0;
			kPlateD->countLR++;
			if (kPlateD->countLR < 0 || kPlateD->countLR > delayL) kPlateD->countLR = 0;
			kPlateD->countQR++;
			if (kPlateD->countQR < 0 || kPlateD->countQR > delayQ) kPlateD->countQR = 0;
			kPlateD->countVR++;
			if (kPlateD->countVR < 0 || kPlateD->countVR > delayV) kPlateD->countVR = 0;

			double outPL = kPlateD->aPL[kPlateD->countPL - ((kPlateD->countPL > delayP) ? delayP + 1 : 0)];
			double outQL = kPlateD->aQL[kPlateD->countQL - ((kPlateD->countQL > delayQ) ? delayQ + 1 : 0)];
			double outRL = kPlateD->aRL[kPlateD->countRL - ((kPlateD->countRL > delayR) ? delayR + 1 : 0)];
			double outSL = kPlateD->aSL[kPlateD->countSL - ((kPlateD->countSL > delayS) ? delayS + 1 : 0)];
			double outTL = kPlateD->aTL[kPlateD->countTL - ((kPlateD->countTL > delayT) ? delayT + 1 : 0)];

			double outBR = kPlateD->aBR[kPlateD->countBR - ((kPlateD->countBR > delayB) ? delayB + 1 : 0)];
			double outGR = kPlateD->aGR[kPlateD->countGR - ((kPlateD->countGR > delayG) ? delayG + 1 : 0)];
			double outLR = kPlateD->aLR[kPlateD->countLR - ((kPlateD->countLR > delayL) ? delayL + 1 : 0)];
			double outQR = kPlateD->aQR[kPlateD->countQR - ((kPlateD->countQR > delayQ) ? delayQ + 1 : 0)];
			double outVR = kPlateD->aVR[kPlateD->countVR - ((kPlateD->countVR > delayV) ? delayV + 1 : 0)];

			//-------- mulch

			outSample = (outPL * kPlateD->fixD[fix_a0]) + kPlateD->fixD[fix_sL1];
			kPlateD->fixD[fix_sL1] = (outPL * kPlateD->fixD[fix_a1]) - (outSample * kPlateD->fixD[fix_b1]) + kPlateD->fixD[fix_sL2];
			kPlateD->fixD[fix_sL2] = (outPL * kPlateD->fixD[fix_a2]) - (outSample * kPlateD->fixD[fix_b2]);
			outPL = outSample; // fixed biquad

			outSample = (outBR * kPlateD->fixD[fix_a0]) + kPlateD->fixD[fix_sR1];
			kPlateD->fixD[fix_sR1] = (outBR * kPlateD->fixD[fix_a1]) - (outSample * kPlateD->fixD[fix_b1]) + kPlateD->fixD[fix_sR2];
			kPlateD->fixD[fix_sR2] = (outBR * kPlateD->fixD[fix_a2]) - (outSample * kPlateD->fixD[fix_b2]);
			outBR = outSample; // fixed biquad

			outSample = (outQL + kPlateD->prevMulchDL) * 0.5;
			kPlateD->prevMulchDL = outQL;
			outQL = outSample;
			outSample = (outGR + kPlateD->prevMulchDR) * 0.5;
			kPlateD->prevMulchDR = outGR;
			outGR = outSample;

			//-------- four

			kPlateD->aUL[kPlateD->countUL] = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0));
			kPlateD->aVL[kPlateD->countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
			kPlateD->aWL[kPlateD->countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
			kPlateD->aXL[kPlateD->countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
			kPlateD->aYL[kPlateD->countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));

			kPlateD->aAR[kPlateD->countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
			kPlateD->aFR[kPlateD->countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
			kPlateD->aKR[kPlateD->countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
			kPlateD->aPR[kPlateD->countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
			kPlateD->aUR[kPlateD->countUR] = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0));

			kPlateD->countUL++;
			if (kPlateD->countUL < 0 || kPlateD->countUL > delayU) kPlateD->countUL = 0;
			kPlateD->countVL++;
			if (kPlateD->countVL < 0 || kPlateD->countVL > delayV) kPlateD->countVL = 0;
			kPlateD->countWL++;
			if (kPlateD->countWL < 0 || kPlateD->countWL > delayW) kPlateD->countWL = 0;
			kPlateD->countXL++;
			if (kPlateD->countXL < 0 || kPlateD->countXL > delayX) kPlateD->countXL = 0;
			kPlateD->countYL++;
			if (kPlateD->countYL < 0 || kPlateD->countYL > delayY) kPlateD->countYL = 0;

			kPlateD->countAR++;
			if (kPlateD->countAR < 0 || kPlateD->countAR > delayA) kPlateD->countAR = 0;
			kPlateD->countFR++;
			if (kPlateD->countFR < 0 || kPlateD->countFR > delayF) kPlateD->countFR = 0;
			kPlateD->countKR++;
			if (kPlateD->countKR < 0 || kPlateD->countKR > delayK) kPlateD->countKR = 0;
			kPlateD->countPR++;
			if (kPlateD->countPR < 0 || kPlateD->countPR > delayP) kPlateD->countPR = 0;
			kPlateD->countUR++;
			if (kPlateD->countUR < 0 || kPlateD->countUR > delayU) kPlateD->countUR = 0;

			double outUL = kPlateD->aUL[kPlateD->countUL - ((kPlateD->countUL > delayU) ? delayU + 1 : 0)];
			double outVL = kPlateD->aVL[kPlateD->countVL - ((kPlateD->countVL > delayV) ? delayV + 1 : 0)];
			double outWL = kPlateD->aWL[kPlateD->countWL - ((kPlateD->countWL > delayW) ? delayW + 1 : 0)];
			double outXL = kPlateD->aXL[kPlateD->countXL - ((kPlateD->countXL > delayX) ? delayX + 1 : 0)];
			double outYL = kPlateD->aYL[kPlateD->countYL - ((kPlateD->countYL > delayY) ? delayY + 1 : 0)];

			double outAR = kPlateD->aAR[kPlateD->countAR - ((kPlateD->countAR > delayA) ? delayA + 1 : 0)];
			double outFR = kPlateD->aFR[kPlateD->countFR - ((kPlateD->countFR > delayF) ? delayF + 1 : 0)];
			double outKR = kPlateD->aKR[kPlateD->countKR - ((kPlateD->countKR > delayK) ? delayK + 1 : 0)];
			double outPR = kPlateD->aPR[kPlateD->countPR - ((kPlateD->countPR > delayP) ? delayP + 1 : 0)];
			double outUR = kPlateD->aUR[kPlateD->countUR - ((kPlateD->countUR > delayU) ? delayU + 1 : 0)];

			//-------- mulch

			outSample = (outVL + kPlateD->prevMulchEL) * 0.5;
			kPlateD->prevMulchEL = outVL;
			outVL = outSample;
			outSample = (outFR + kPlateD->prevMulchER) * 0.5;
			kPlateD->prevMulchER = outFR;
			outFR = outSample;

			//-------- five

			kPlateD->feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
			kPlateD->feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
			kPlateD->feedbackJR = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
			kPlateD->feedbackBL = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
			kPlateD->feedbackOR = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
			kPlateD->feedbackCL = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
			kPlateD->feedbackTR = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
			kPlateD->feedbackDL = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
			kPlateD->feedbackYR = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
			kPlateD->feedbackEL = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
			// which we need to feed back into the input again, a bit

			inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0016;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0016;
			// and take the final combined sum of outputs, corrected for Householder gain

			inputSampleL *= 0.5;
			inputSampleR *= 0.5;
			if (kPlateD->gainOutL < 0.0078125) kPlateD->gainOutL = 0.0078125;
			if (kPlateD->gainOutL > 1.0) kPlateD->gainOutL = 1.0;
			if (kPlateD->gainOutR < 0.0078125) kPlateD->gainOutR = 0.0078125;
			if (kPlateD->gainOutR > 1.0) kPlateD->gainOutR = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateD->gainOutL;
			inputSampleR *= kPlateD->gainOutR;
			kPlateD->gainOutL += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateD->gainOutR += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			inputSampleL *= 2.0;
			inputSampleR *= 2.0;
			// curve! To get a compressed effect that matches a certain other plugin
			// that is too overprocessed for its own good :)

			outSample = (inputSampleL + kPlateD->prevOutDL) * 0.5;
			kPlateD->prevOutDL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateD->prevOutDR) * 0.5;
			kPlateD->prevOutDR = inputSampleR;
			inputSampleR = outSample;
			outSample = (inputSampleL + kPlateD->prevOutEL) * 0.5;
			kPlateD->prevOutEL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateD->prevOutER) * 0.5;
			kPlateD->prevOutER = inputSampleR;
			inputSampleR = outSample;

			if (cycleEnd == 4) {
				kPlateD->lastRefL[0] = kPlateD->lastRefL[4]; // start from previous last
				kPlateD->lastRefL[2] = (kPlateD->lastRefL[0] + inputSampleL) / 2; // half
				kPlateD->lastRefL[1] = (kPlateD->lastRefL[0] + kPlateD->lastRefL[2]) / 2; // one quarter
				kPlateD->lastRefL[3] = (kPlateD->lastRefL[2] + inputSampleL) / 2; // three quarters
				kPlateD->lastRefL[4] = inputSampleL; // full
				kPlateD->lastRefR[0] = kPlateD->lastRefR[4]; // start from previous last
				kPlateD->lastRefR[2] = (kPlateD->lastRefR[0] + inputSampleR) / 2; // half
				kPlateD->lastRefR[1] = (kPlateD->lastRefR[0] + kPlateD->lastRefR[2]) / 2; // one quarter
				kPlateD->lastRefR[3] = (kPlateD->lastRefR[2] + inputSampleR) / 2; // three quarters
				kPlateD->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				kPlateD->lastRefL[0] = kPlateD->lastRefL[3]; // start from previous last
				kPlateD->lastRefL[2] = (kPlateD->lastRefL[0] + kPlateD->lastRefL[0] + inputSampleL) / 3; // third
				kPlateD->lastRefL[1] = (kPlateD->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				kPlateD->lastRefL[3] = inputSampleL; // full
				kPlateD->lastRefR[0] = kPlateD->lastRefR[3]; // start from previous last
				kPlateD->lastRefR[2] = (kPlateD->lastRefR[0] + kPlateD->lastRefR[0] + inputSampleR) / 3; // third
				kPlateD->lastRefR[1] = (kPlateD->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				kPlateD->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				kPlateD->lastRefL[0] = kPlateD->lastRefL[2]; // start from previous last
				kPlateD->lastRefL[1] = (kPlateD->lastRefL[0] + inputSampleL) / 2; // half
				kPlateD->lastRefL[2] = inputSampleL; // full
				kPlateD->lastRefR[0] = kPlateD->lastRefR[2]; // start from previous last
				kPlateD->lastRefR[1] = (kPlateD->lastRefR[0] + inputSampleR) / 2; // half
				kPlateD->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				kPlateD->lastRefL[0] = inputSampleL;
				kPlateD->lastRefR[0] = inputSampleR;
			}
			kPlateD->cycle = 0; // reset
			inputSampleL = kPlateD->lastRefL[kPlateD->cycle];
			inputSampleR = kPlateD->lastRefR[kPlateD->cycle];
		} else {
			inputSampleL = kPlateD->lastRefL[kPlateD->cycle];
			inputSampleR = kPlateD->lastRefR[kPlateD->cycle];
			// we are going through our references now
		}

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		// amplitude aspect

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
		kPlateD->fpdL ^= kPlateD->fpdL << 13;
		kPlateD->fpdL ^= kPlateD->fpdL >> 17;
		kPlateD->fpdL ^= kPlateD->fpdL << 5;
		inputSampleL += (((double) kPlateD->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		kPlateD->fpdR ^= kPlateD->fpdR << 13;
		kPlateD->fpdR ^= kPlateD->fpdR >> 17;
		kPlateD->fpdR ^= kPlateD->fpdR << 5;
		inputSampleR += (((double) kPlateD->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	KPLATED_URI,
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
