#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define KPLATEA_URI "https://hannesbraun.net/ns/lv2/airwindows/kplatea"

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

const int earlyA = 61;
const int earlyB = 499;
const int earlyC = 107;
const int earlyD = 127;
const int earlyE = 607;
const int earlyF = 313;
const int earlyG = 563;
const int earlyH = 47;
const int earlyI = 409;
const int predelay = 24010; // 37 ms, 170 seat club
const int delayA = 631;
const int delayB = 281;
const int delayC = 97;
const int delayD = 709;
const int delayE = 307;
const int delayF = 149;
const int delayG = 313;
const int delayH = 37;
const int delayI = 659;
const int delayJ = 701;
const int delayK = 13;
const int delayL = 733;
const int delayM = 773;
const int delayN = 23;
const int delayO = 61;
const int delayP = 787;
const int delayQ = 83;
const int delayR = 691;
const int delayS = 5;
const int delayT = 587;
const int delayU = 829;
const int delayV = 449;
const int delayW = 53;
const int delayX = 503;
const int delayY = 181; // 86 ms, 882 seat hall. Scarcity, 1 in 41582

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

	double prevMulchAL;
	double prevMulchAR;
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

	double prevInAL;
	double prevInAR;
	double prevInBL;
	double prevInBR;

	uint32_t fpdL;
	uint32_t fpdR;
} KPlateA;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	KPlateA* kPlateA = (KPlateA*) calloc(1, sizeof(KPlateA));
	kPlateA->sampleRate = rate;
	return (LV2_Handle) kPlateA;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	KPlateA* kPlateA = (KPlateA*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			kPlateA->input[0] = (const float*) data;
			break;
		case INPUT_R:
			kPlateA->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			kPlateA->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			kPlateA->output[1] = (float*) data;
			break;
		case INPUT_PAD:
			kPlateA->inputPad = (const float*) data;
			break;
		case DAMPING:
			kPlateA->damping = (const float*) data;
			break;
		case LOW_CUT:
			kPlateA->lowCut = (const float*) data;
			break;
		case PREDELAY:
			kPlateA->predelay = (const float*) data;
			break;
		case WETNESS:
			kPlateA->wetness = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	KPlateA* kPlateA = (KPlateA*) instance;

	kPlateA->iirAL = 0.0;
	kPlateA->iirBL = 0.0;

	kPlateA->iirAR = 0.0;
	kPlateA->iirBR = 0.0;

	kPlateA->gainIn = kPlateA->gainOutL = kPlateA->gainOutR = 1.0;

	for (int count = 0; count < delayA + 2; count++) {
		kPlateA->aAL[count] = 0.0;
		kPlateA->aAR[count] = 0.0;
	}
	for (int count = 0; count < delayB + 2; count++) {
		kPlateA->aBL[count] = 0.0;
		kPlateA->aBR[count] = 0.0;
	}
	for (int count = 0; count < delayC + 2; count++) {
		kPlateA->aCL[count] = 0.0;
		kPlateA->aCR[count] = 0.0;
	}
	for (int count = 0; count < delayD + 2; count++) {
		kPlateA->aDL[count] = 0.0;
		kPlateA->aDR[count] = 0.0;
	}
	for (int count = 0; count < delayE + 2; count++) {
		kPlateA->aEL[count] = 0.0;
		kPlateA->aER[count] = 0.0;
	}
	for (int count = 0; count < delayF + 2; count++) {
		kPlateA->aFL[count] = 0.0;
		kPlateA->aFR[count] = 0.0;
	}
	for (int count = 0; count < delayG + 2; count++) {
		kPlateA->aGL[count] = 0.0;
		kPlateA->aGR[count] = 0.0;
	}
	for (int count = 0; count < delayH + 2; count++) {
		kPlateA->aHL[count] = 0.0;
		kPlateA->aHR[count] = 0.0;
	}
	for (int count = 0; count < delayI + 2; count++) {
		kPlateA->aIL[count] = 0.0;
		kPlateA->aIR[count] = 0.0;
	}
	for (int count = 0; count < delayJ + 2; count++) {
		kPlateA->aJL[count] = 0.0;
		kPlateA->aJR[count] = 0.0;
	}
	for (int count = 0; count < delayK + 2; count++) {
		kPlateA->aKL[count] = 0.0;
		kPlateA->aKR[count] = 0.0;
	}
	for (int count = 0; count < delayL + 2; count++) {
		kPlateA->aLL[count] = 0.0;
		kPlateA->aLR[count] = 0.0;
	}
	for (int count = 0; count < delayM + 2; count++) {
		kPlateA->aML[count] = 0.0;
		kPlateA->aMR[count] = 0.0;
	}
	for (int count = 0; count < delayN + 2; count++) {
		kPlateA->aNL[count] = 0.0;
		kPlateA->aNR[count] = 0.0;
	}
	for (int count = 0; count < delayO + 2; count++) {
		kPlateA->aOL[count] = 0.0;
		kPlateA->aOR[count] = 0.0;
	}
	for (int count = 0; count < delayP + 2; count++) {
		kPlateA->aPL[count] = 0.0;
		kPlateA->aPR[count] = 0.0;
	}
	for (int count = 0; count < delayQ + 2; count++) {
		kPlateA->aQL[count] = 0.0;
		kPlateA->aQR[count] = 0.0;
	}
	for (int count = 0; count < delayR + 2; count++) {
		kPlateA->aRL[count] = 0.0;
		kPlateA->aRR[count] = 0.0;
	}
	for (int count = 0; count < delayS + 2; count++) {
		kPlateA->aSL[count] = 0.0;
		kPlateA->aSR[count] = 0.0;
	}
	for (int count = 0; count < delayT + 2; count++) {
		kPlateA->aTL[count] = 0.0;
		kPlateA->aTR[count] = 0.0;
	}
	for (int count = 0; count < delayU + 2; count++) {
		kPlateA->aUL[count] = 0.0;
		kPlateA->aUR[count] = 0.0;
	}
	for (int count = 0; count < delayV + 2; count++) {
		kPlateA->aVL[count] = 0.0;
		kPlateA->aVR[count] = 0.0;
	}
	for (int count = 0; count < delayW + 2; count++) {
		kPlateA->aWL[count] = 0.0;
		kPlateA->aWR[count] = 0.0;
	}
	for (int count = 0; count < delayX + 2; count++) {
		kPlateA->aXL[count] = 0.0;
		kPlateA->aXR[count] = 0.0;
	}
	for (int count = 0; count < delayY + 2; count++) {
		kPlateA->aYL[count] = 0.0;
		kPlateA->aYR[count] = 0.0;
	}

	for (int count = 0; count < earlyA + 2; count++) {
		kPlateA->eAL[count] = 0.0;
		kPlateA->eAR[count] = 0.0;
	}
	for (int count = 0; count < earlyB + 2; count++) {
		kPlateA->eBL[count] = 0.0;
		kPlateA->eBR[count] = 0.0;
	}
	for (int count = 0; count < earlyC + 2; count++) {
		kPlateA->eCL[count] = 0.0;
		kPlateA->eCR[count] = 0.0;
	}
	for (int count = 0; count < earlyD + 2; count++) {
		kPlateA->eDL[count] = 0.0;
		kPlateA->eDR[count] = 0.0;
	}
	for (int count = 0; count < earlyE + 2; count++) {
		kPlateA->eEL[count] = 0.0;
		kPlateA->eER[count] = 0.0;
	}
	for (int count = 0; count < earlyF + 2; count++) {
		kPlateA->eFL[count] = 0.0;
		kPlateA->eFR[count] = 0.0;
	}
	for (int count = 0; count < earlyG + 2; count++) {
		kPlateA->eGL[count] = 0.0;
		kPlateA->eGR[count] = 0.0;
	}
	for (int count = 0; count < earlyH + 2; count++) {
		kPlateA->eHL[count] = 0.0;
		kPlateA->eHR[count] = 0.0;
	}
	for (int count = 0; count < earlyI + 2; count++) {
		kPlateA->eIL[count] = 0.0;
		kPlateA->eIR[count] = 0.0;
	}

	for (int count = 0; count < predelay + 2; count++) {
		kPlateA->aZL[count] = 0.0;
		kPlateA->aZR[count] = 0.0;
	}

	kPlateA->feedbackAL = 0.0;
	kPlateA->feedbackBL = 0.0;
	kPlateA->feedbackCL = 0.0;
	kPlateA->feedbackDL = 0.0;
	kPlateA->feedbackEL = 0.0;

	kPlateA->previousAL = 0.0;
	kPlateA->previousBL = 0.0;
	kPlateA->previousCL = 0.0;
	kPlateA->previousDL = 0.0;
	kPlateA->previousEL = 0.0;

	kPlateA->feedbackER = 0.0;
	kPlateA->feedbackJR = 0.0;
	kPlateA->feedbackOR = 0.0;
	kPlateA->feedbackTR = 0.0;
	kPlateA->feedbackYR = 0.0;

	kPlateA->previousAR = 0.0;
	kPlateA->previousBR = 0.0;
	kPlateA->previousCR = 0.0;
	kPlateA->previousDR = 0.0;
	kPlateA->previousER = 0.0;

	kPlateA->prevMulchAL = 0.0;
	kPlateA->prevMulchAR = 0.0;
	kPlateA->prevMulchBL = 0.0;
	kPlateA->prevMulchBR = 0.0;
	kPlateA->prevMulchCL = 0.0;
	kPlateA->prevMulchCR = 0.0;
	kPlateA->prevMulchDL = 0.0;
	kPlateA->prevMulchDR = 0.0;
	kPlateA->prevMulchEL = 0.0;
	kPlateA->prevMulchER = 0.0;

	kPlateA->prevOutAL = 0.0;
	kPlateA->prevOutAR = 0.0;

	kPlateA->prevInAL = 0.0;
	kPlateA->prevInAR = 0.0;
	kPlateA->prevInBL = 0.0;
	kPlateA->prevInBR = 0.0;

	for (int count = 0; count < 6; count++) {
		kPlateA->lastRefL[count] = 0.0;
		kPlateA->lastRefR[count] = 0.0;
	}

	kPlateA->earlyAL = 1;
	kPlateA->earlyBL = 1;
	kPlateA->earlyCL = 1;
	kPlateA->earlyDL = 1;
	kPlateA->earlyEL = 1;
	kPlateA->earlyFL = 1;
	kPlateA->earlyGL = 1;
	kPlateA->earlyHL = 1;
	kPlateA->earlyIL = 1;

	kPlateA->earlyAR = 1;
	kPlateA->earlyBR = 1;
	kPlateA->earlyCR = 1;
	kPlateA->earlyDR = 1;
	kPlateA->earlyER = 1;
	kPlateA->earlyFR = 1;
	kPlateA->earlyGR = 1;
	kPlateA->earlyHR = 1;
	kPlateA->earlyIR = 1;

	kPlateA->countAL = 1;
	kPlateA->countBL = 1;
	kPlateA->countCL = 1;
	kPlateA->countDL = 1;
	kPlateA->countEL = 1;
	kPlateA->countFL = 1;
	kPlateA->countGL = 1;
	kPlateA->countHL = 1;
	kPlateA->countIL = 1;
	kPlateA->countJL = 1;
	kPlateA->countKL = 1;
	kPlateA->countLL = 1;
	kPlateA->countML = 1;
	kPlateA->countNL = 1;
	kPlateA->countOL = 1;
	kPlateA->countPL = 1;
	kPlateA->countQL = 1;
	kPlateA->countRL = 1;
	kPlateA->countSL = 1;
	kPlateA->countTL = 1;
	kPlateA->countUL = 1;
	kPlateA->countVL = 1;
	kPlateA->countWL = 1;
	kPlateA->countXL = 1;
	kPlateA->countYL = 1;

	kPlateA->countAR = 1;
	kPlateA->countBR = 1;
	kPlateA->countCR = 1;
	kPlateA->countDR = 1;
	kPlateA->countER = 1;
	kPlateA->countFR = 1;
	kPlateA->countGR = 1;
	kPlateA->countHR = 1;
	kPlateA->countIR = 1;
	kPlateA->countJR = 1;
	kPlateA->countKR = 1;
	kPlateA->countLR = 1;
	kPlateA->countMR = 1;
	kPlateA->countNR = 1;
	kPlateA->countOR = 1;
	kPlateA->countPR = 1;
	kPlateA->countQR = 1;
	kPlateA->countRR = 1;
	kPlateA->countSR = 1;
	kPlateA->countTR = 1;
	kPlateA->countUR = 1;
	kPlateA->countVR = 1;
	kPlateA->countWR = 1;
	kPlateA->countXR = 1;
	kPlateA->countYR = 1;

	kPlateA->countZ = 1;

	kPlateA->cycle = 0;

	for (int x = 0; x < fix_total; x++) {
		kPlateA->fixA[x] = 0.0;
		kPlateA->fixB[x] = 0.0;
		kPlateA->fixC[x] = 0.0;
		kPlateA->fixD[x] = 0.0;
	}
	// from ZBandpass, so I can use enums with it

	kPlateA->fpdL = 1.0;
	while (kPlateA->fpdL < 16386) kPlateA->fpdL = rand() * UINT32_MAX;
	kPlateA->fpdR = 1.0;
	while (kPlateA->fpdR < 16386) kPlateA->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	KPlateA* kPlateA = (KPlateA*) instance;

	const float* in1 = kPlateA->input[0];
	const float* in2 = kPlateA->input[1];
	float* out1 = kPlateA->output[0];
	float* out2 = kPlateA->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= kPlateA->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (kPlateA->cycle > cycleEnd - 1) kPlateA->cycle = cycleEnd - 1; // sanity check

	double downRate = kPlateA->sampleRate / cycleEnd;
	// we now shift gears between 44.1k and 48k so our tone is the same, slight changes in delay times

	double inputPad = *kPlateA->inputPad;
	double regen = ((*kPlateA->damping / 10.0) * 0.415) + 0.16;
	regen = (regen * 0.0001) + 0.00024;
	double iirAmount = ((*kPlateA->lowCut / 3.0) * 0.3) + 0.04;
	iirAmount = (iirAmount * 1000.0) / downRate;
	double earlyVolume = *kPlateA->predelay; // predelay to a half-second
	int adjPredelay = (downRate * earlyVolume);
	double wet = *kPlateA->wetness * 2.0;
	double dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	// this reverb makes 50% full dry AND full wet, not crossfaded.
	// that's so it can be on submixes without cutting back dry channel when adjusted:
	// unless you go super heavy, you are only adjusting the added verb loudness.

	kPlateA->fixA[fix_freq] = 46.0 / downRate;
	kPlateA->fixA[fix_reso] = 0.0061504;
	kPlateA->fixD[fix_freq] = 22.0 / downRate;
	kPlateA->fixD[fix_reso] = 0.00144;
	kPlateA->fixB[fix_freq] = (kPlateA->fixA[fix_freq] + kPlateA->fixA[fix_freq] + kPlateA->fixD[fix_freq]) / 3.0;
	kPlateA->fixB[fix_reso] = 0.0045802;
	kPlateA->fixC[fix_freq] = (kPlateA->fixA[fix_freq] + kPlateA->fixD[fix_freq] + kPlateA->fixD[fix_freq]) / 3.0;
	kPlateA->fixC[fix_reso] = 0.0030101;
	double K = tan(M_PI * kPlateA->fixA[fix_freq]);
	double norm = 1.0 / (1.0 + K / kPlateA->fixA[fix_reso] + K * K);
	kPlateA->fixA[fix_a0] = K / kPlateA->fixA[fix_reso] * norm;
	kPlateA->fixA[fix_a1] = 0.0;
	kPlateA->fixA[fix_a2] = -kPlateA->fixA[fix_a0];
	kPlateA->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateA->fixA[fix_b2] = (1.0 - K / kPlateA->fixA[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateA->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateA->fixB[fix_reso] + K * K);
	kPlateA->fixB[fix_a0] = K / kPlateA->fixB[fix_reso] * norm;
	kPlateA->fixB[fix_a1] = 0.0;
	kPlateA->fixB[fix_a2] = -kPlateA->fixB[fix_a0];
	kPlateA->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateA->fixB[fix_b2] = (1.0 - K / kPlateA->fixB[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateA->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateA->fixC[fix_reso] + K * K);
	kPlateA->fixC[fix_a0] = K / kPlateA->fixC[fix_reso] * norm;
	kPlateA->fixC[fix_a1] = 0.0;
	kPlateA->fixC[fix_a2] = -kPlateA->fixC[fix_a0];
	kPlateA->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateA->fixC[fix_b2] = (1.0 - K / kPlateA->fixC[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix
	K = tan(M_PI * kPlateA->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / kPlateA->fixD[fix_reso] + K * K);
	kPlateA->fixD[fix_a0] = K / kPlateA->fixD[fix_reso] * norm;
	kPlateA->fixD[fix_a1] = 0.0;
	kPlateA->fixD[fix_a2] = -kPlateA->fixD[fix_a0];
	kPlateA->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	kPlateA->fixD[fix_b2] = (1.0 - K / kPlateA->fixD[fix_reso] + K * K) * norm;
	// stereo biquad bandpasses we can put into the reverb matrix

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = kPlateA->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = kPlateA->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		kPlateA->cycle++;
		if (kPlateA->cycle == cycleEnd) { // hit the end point and we do a reverb sample
			if (inputPad < 1.0) {
				inputSampleL *= inputPad;
				inputSampleR *= inputPad;
			}
			double outSample;
			outSample = (inputSampleL + kPlateA->prevInAL) * 0.5;
			kPlateA->prevInAL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateA->prevInAR) * 0.5;
			kPlateA->prevInAR = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			kPlateA->iirAL = (kPlateA->iirAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateA->iirAL;
			kPlateA->iirAR = (kPlateA->iirAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateA->iirAR;
			// 600hz highpass on input

			inputSampleL *= 0.5;
			inputSampleR *= 0.5;
			if (kPlateA->gainIn < 0.0078125) kPlateA->gainIn = 0.0078125;
			if (kPlateA->gainIn > 1.0) kPlateA->gainIn = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateA->gainIn;
			inputSampleR *= kPlateA->gainIn;
			kPlateA->gainIn += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateA->gainIn += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			// inputSampleL *= 2.0; inputSampleR *= 2.0;
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

			kPlateA->iirBL = (kPlateA->iirBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL = inputSampleL - kPlateA->iirBL;
			kPlateA->iirBR = (kPlateA->iirBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR = inputSampleR - kPlateA->iirBR;
			// 600hz highpass on input

			outSample = (inputSampleL + kPlateA->prevInBL) * 0.5;
			kPlateA->prevInBL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateA->prevInBR) * 0.5;
			kPlateA->prevInBR = inputSampleR;
			inputSampleR = outSample;
			// 10k filter on input

			// begin allpasses
			double oeAL = inputSampleL - (kPlateA->eAL[(kPlateA->earlyAL + 1) - ((kPlateA->earlyAL + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeBL = inputSampleL - (kPlateA->eBL[(kPlateA->earlyBL + 1) - ((kPlateA->earlyBL + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeCL = inputSampleL - (kPlateA->eCL[(kPlateA->earlyCL + 1) - ((kPlateA->earlyCL + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeCR = inputSampleR - (kPlateA->eCR[(kPlateA->earlyCR + 1) - ((kPlateA->earlyCR + 1 > earlyC) ? earlyC + 1 : 0)] * 0.5);
			double oeFR = inputSampleR - (kPlateA->eFR[(kPlateA->earlyFR + 1) - ((kPlateA->earlyFR + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeIR = inputSampleR - (kPlateA->eIR[(kPlateA->earlyIR + 1) - ((kPlateA->earlyIR + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);

			kPlateA->eAL[kPlateA->earlyAL] = oeAL;
			oeAL *= 0.5;
			kPlateA->eBL[kPlateA->earlyBL] = oeBL;
			oeBL *= 0.5;
			kPlateA->eCL[kPlateA->earlyCL] = oeCL;
			oeCL *= 0.5;
			kPlateA->eCR[kPlateA->earlyCR] = oeCR;
			oeCR *= 0.5;
			kPlateA->eFR[kPlateA->earlyFR] = oeFR;
			oeFR *= 0.5;
			kPlateA->eIR[kPlateA->earlyIR] = oeIR;
			oeIR *= 0.5;

			kPlateA->earlyAL++;
			if (kPlateA->earlyAL < 0 || kPlateA->earlyAL > earlyA) kPlateA->earlyAL = 0;
			kPlateA->earlyBL++;
			if (kPlateA->earlyBL < 0 || kPlateA->earlyBL > earlyB) kPlateA->earlyBL = 0;
			kPlateA->earlyCL++;
			if (kPlateA->earlyCL < 0 || kPlateA->earlyCL > earlyC) kPlateA->earlyCL = 0;
			kPlateA->earlyCR++;
			if (kPlateA->earlyCR < 0 || kPlateA->earlyCR > earlyC) kPlateA->earlyCR = 0;
			kPlateA->earlyFR++;
			if (kPlateA->earlyFR < 0 || kPlateA->earlyFR > earlyF) kPlateA->earlyFR = 0;
			kPlateA->earlyIR++;
			if (kPlateA->earlyIR < 0 || kPlateA->earlyIR > earlyI) kPlateA->earlyIR = 0;

			oeAL += kPlateA->eAL[kPlateA->earlyAL - ((kPlateA->earlyAL > earlyA) ? earlyA + 1 : 0)];
			oeBL += kPlateA->eBL[kPlateA->earlyBL - ((kPlateA->earlyBL > earlyB) ? earlyB + 1 : 0)];
			oeCL += kPlateA->eCL[kPlateA->earlyCL - ((kPlateA->earlyCL > earlyC) ? earlyC + 1 : 0)];
			oeCR += kPlateA->eCR[kPlateA->earlyCR - ((kPlateA->earlyCR > earlyC) ? earlyC + 1 : 0)];
			oeFR += kPlateA->eFR[kPlateA->earlyFR - ((kPlateA->earlyFR > earlyF) ? earlyF + 1 : 0)];
			oeIR += kPlateA->eIR[kPlateA->earlyIR - ((kPlateA->earlyIR > earlyI) ? earlyI + 1 : 0)];

			double oeDL = ((oeBL + oeCL) - oeAL) - (kPlateA->eDL[(kPlateA->earlyDL + 1) - ((kPlateA->earlyDL + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeEL = ((oeAL + oeCL) - oeBL) - (kPlateA->eEL[(kPlateA->earlyEL + 1) - ((kPlateA->earlyEL + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeFL = ((oeAL + oeBL) - oeCL) - (kPlateA->eFL[(kPlateA->earlyFL + 1) - ((kPlateA->earlyFL + 1 > earlyF) ? earlyF + 1 : 0)] * 0.5);
			double oeBR = ((oeFR + oeIR) - oeCR) - (kPlateA->eBR[(kPlateA->earlyBR + 1) - ((kPlateA->earlyBR + 1 > earlyB) ? earlyB + 1 : 0)] * 0.5);
			double oeER = ((oeCR + oeIR) - oeFR) - (kPlateA->eER[(kPlateA->earlyER + 1) - ((kPlateA->earlyER + 1 > earlyE) ? earlyE + 1 : 0)] * 0.5);
			double oeHR = ((oeCR + oeFR) - oeIR) - (kPlateA->eHR[(kPlateA->earlyHR + 1) - ((kPlateA->earlyHR + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);

			kPlateA->eDL[kPlateA->earlyDL] = oeDL;
			oeDL *= 0.5;
			kPlateA->eEL[kPlateA->earlyEL] = oeEL;
			oeEL *= 0.5;
			kPlateA->eFL[kPlateA->earlyFL] = oeFL;
			oeFL *= 0.5;
			kPlateA->eBR[kPlateA->earlyBR] = oeBR;
			oeBR *= 0.5;
			kPlateA->eER[kPlateA->earlyER] = oeER;
			oeER *= 0.5;
			kPlateA->eHR[kPlateA->earlyHR] = oeHR;
			oeHR *= 0.5;

			kPlateA->earlyDL++;
			if (kPlateA->earlyDL < 0 || kPlateA->earlyDL > earlyD) kPlateA->earlyDL = 0;
			kPlateA->earlyEL++;
			if (kPlateA->earlyEL < 0 || kPlateA->earlyEL > earlyE) kPlateA->earlyEL = 0;
			kPlateA->earlyFL++;
			if (kPlateA->earlyFL < 0 || kPlateA->earlyFL > earlyF) kPlateA->earlyFL = 0;
			kPlateA->earlyBR++;
			if (kPlateA->earlyBR < 0 || kPlateA->earlyBR > earlyB) kPlateA->earlyBR = 0;
			kPlateA->earlyER++;
			if (kPlateA->earlyER < 0 || kPlateA->earlyER > earlyE) kPlateA->earlyER = 0;
			kPlateA->earlyHR++;
			if (kPlateA->earlyHR < 0 || kPlateA->earlyHR > earlyH) kPlateA->earlyHR = 0;

			oeDL += kPlateA->eDL[kPlateA->earlyDL - ((kPlateA->earlyDL > earlyD) ? earlyD + 1 : 0)];
			oeEL += kPlateA->eEL[kPlateA->earlyEL - ((kPlateA->earlyEL > earlyE) ? earlyE + 1 : 0)];
			oeFL += kPlateA->eFL[kPlateA->earlyFL - ((kPlateA->earlyFL > earlyF) ? earlyF + 1 : 0)];
			oeBR += kPlateA->eBR[kPlateA->earlyBR - ((kPlateA->earlyBR > earlyB) ? earlyB + 1 : 0)];
			oeER += kPlateA->eER[kPlateA->earlyER - ((kPlateA->earlyER > earlyE) ? earlyE + 1 : 0)];
			oeHR += kPlateA->eHR[kPlateA->earlyHR - ((kPlateA->earlyHR > earlyH) ? earlyH + 1 : 0)];

			double oeGL = ((oeEL + oeFL) - oeDL) - (kPlateA->eGL[(kPlateA->earlyGL + 1) - ((kPlateA->earlyGL + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);
			double oeHL = ((oeDL + oeFL) - oeEL) - (kPlateA->eHL[(kPlateA->earlyHL + 1) - ((kPlateA->earlyHL + 1 > earlyH) ? earlyH + 1 : 0)] * 0.5);
			double oeIL = ((oeDL + oeEL) - oeFL) - (kPlateA->eIL[(kPlateA->earlyIL + 1) - ((kPlateA->earlyIL + 1 > earlyI) ? earlyI + 1 : 0)] * 0.5);
			double oeAR = ((oeER + oeHR) - oeBR) - (kPlateA->eAR[(kPlateA->earlyAR + 1) - ((kPlateA->earlyAR + 1 > earlyA) ? earlyA + 1 : 0)] * 0.5);
			double oeDR = ((oeBR + oeHR) - oeER) - (kPlateA->eDR[(kPlateA->earlyDR + 1) - ((kPlateA->earlyDR + 1 > earlyD) ? earlyD + 1 : 0)] * 0.5);
			double oeGR = ((oeBR + oeER) - oeHR) - (kPlateA->eGR[(kPlateA->earlyGR + 1) - ((kPlateA->earlyGR + 1 > earlyG) ? earlyG + 1 : 0)] * 0.5);

			kPlateA->eGL[kPlateA->earlyGL] = oeGL;
			oeGL *= 0.5;
			kPlateA->eHL[kPlateA->earlyHL] = oeHL;
			oeHL *= 0.5;
			kPlateA->eIL[kPlateA->earlyIL] = oeIL;
			oeIL *= 0.5;
			kPlateA->eAR[kPlateA->earlyAR] = oeAR;
			oeAR *= 0.5;
			kPlateA->eDR[kPlateA->earlyDR] = oeDR;
			oeDR *= 0.5;
			kPlateA->eGR[kPlateA->earlyGR] = oeGR;
			oeGR *= 0.5;

			kPlateA->earlyGL++;
			if (kPlateA->earlyGL < 0 || kPlateA->earlyGL > earlyG) kPlateA->earlyGL = 0;
			kPlateA->earlyHL++;
			if (kPlateA->earlyHL < 0 || kPlateA->earlyHL > earlyH) kPlateA->earlyHL = 0;
			kPlateA->earlyIL++;
			if (kPlateA->earlyIL < 0 || kPlateA->earlyIL > earlyI) kPlateA->earlyIL = 0;
			kPlateA->earlyAR++;
			if (kPlateA->earlyAR < 0 || kPlateA->earlyAR > earlyA) kPlateA->earlyAR = 0;
			kPlateA->earlyDR++;
			if (kPlateA->earlyDR < 0 || kPlateA->earlyDR > earlyD) kPlateA->earlyDR = 0;
			kPlateA->earlyGR++;
			if (kPlateA->earlyGR < 0 || kPlateA->earlyGR > earlyG) kPlateA->earlyGR = 0;

			oeGL += kPlateA->eGL[kPlateA->earlyGL - ((kPlateA->earlyGL > earlyG) ? earlyG + 1 : 0)];
			oeHL += kPlateA->eHL[kPlateA->earlyHL - ((kPlateA->earlyHL > earlyH) ? earlyH + 1 : 0)];
			oeIL += kPlateA->eIL[kPlateA->earlyIL - ((kPlateA->earlyIL > earlyI) ? earlyI + 1 : 0)];
			oeAR += kPlateA->eAR[kPlateA->earlyAR - ((kPlateA->earlyAR > earlyA) ? earlyA + 1 : 0)];
			oeDR += kPlateA->eDR[kPlateA->earlyDR - ((kPlateA->earlyDR > earlyD) ? earlyD + 1 : 0)];
			oeGR += kPlateA->eGR[kPlateA->earlyGR - ((kPlateA->earlyGR > earlyG) ? earlyG + 1 : 0)];

			// allpasses predelay
			kPlateA->aZL[kPlateA->countZ] = (oeGL + oeHL + oeIL) * 0.25;
			kPlateA->aZR[kPlateA->countZ] = (oeAR + oeDR + oeGR) * 0.25;
			kPlateA->countZ++;
			if (kPlateA->countZ < 0 || kPlateA->countZ > adjPredelay) kPlateA->countZ = 0;
			inputSampleL = kPlateA->aZL[kPlateA->countZ - ((kPlateA->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			inputSampleR = kPlateA->aZR[kPlateA->countZ - ((kPlateA->countZ > adjPredelay) ? adjPredelay + 1 : 0)];
			// end allpasses

			kPlateA->aAL[kPlateA->countAL] = inputSampleL + (kPlateA->feedbackAL * regen);
			kPlateA->aBL[kPlateA->countBL] = inputSampleL + (kPlateA->feedbackBL * regen);
			kPlateA->aCL[kPlateA->countCL] = inputSampleL + (kPlateA->feedbackCL * regen);
			kPlateA->aDL[kPlateA->countDL] = inputSampleL + (kPlateA->feedbackDL * regen);
			kPlateA->aEL[kPlateA->countEL] = inputSampleL + (kPlateA->feedbackEL * regen);

			kPlateA->aER[kPlateA->countER] = inputSampleR + (kPlateA->feedbackER * regen);
			kPlateA->aJR[kPlateA->countJR] = inputSampleR + (kPlateA->feedbackJR * regen);
			kPlateA->aOR[kPlateA->countOR] = inputSampleR + (kPlateA->feedbackOR * regen);
			kPlateA->aTR[kPlateA->countTR] = inputSampleR + (kPlateA->feedbackTR * regen);
			kPlateA->aYR[kPlateA->countYR] = inputSampleR + (kPlateA->feedbackYR * regen);

			kPlateA->countAL++;
			if (kPlateA->countAL < 0 || kPlateA->countAL > delayA) kPlateA->countAL = 0;
			kPlateA->countBL++;
			if (kPlateA->countBL < 0 || kPlateA->countBL > delayB) kPlateA->countBL = 0;
			kPlateA->countCL++;
			if (kPlateA->countCL < 0 || kPlateA->countCL > delayC) kPlateA->countCL = 0;
			kPlateA->countDL++;
			if (kPlateA->countDL < 0 || kPlateA->countDL > delayD) kPlateA->countDL = 0;
			kPlateA->countEL++;
			if (kPlateA->countEL < 0 || kPlateA->countEL > delayE) kPlateA->countEL = 0;

			kPlateA->countER++;
			if (kPlateA->countER < 0 || kPlateA->countER > delayE) kPlateA->countER = 0;
			kPlateA->countJR++;
			if (kPlateA->countJR < 0 || kPlateA->countJR > delayJ) kPlateA->countJR = 0;
			kPlateA->countOR++;
			if (kPlateA->countOR < 0 || kPlateA->countOR > delayO) kPlateA->countOR = 0;
			kPlateA->countTR++;
			if (kPlateA->countTR < 0 || kPlateA->countTR > delayT) kPlateA->countTR = 0;
			kPlateA->countYR++;
			if (kPlateA->countYR < 0 || kPlateA->countYR > delayY) kPlateA->countYR = 0;

			double outAL = kPlateA->aAL[kPlateA->countAL - ((kPlateA->countAL > delayA) ? delayA + 1 : 0)];
			double outBL = kPlateA->aBL[kPlateA->countBL - ((kPlateA->countBL > delayB) ? delayB + 1 : 0)];
			double outCL = kPlateA->aCL[kPlateA->countCL - ((kPlateA->countCL > delayC) ? delayC + 1 : 0)];
			double outDL = kPlateA->aDL[kPlateA->countDL - ((kPlateA->countDL > delayD) ? delayD + 1 : 0)];
			double outEL = kPlateA->aEL[kPlateA->countEL - ((kPlateA->countEL > delayE) ? delayE + 1 : 0)];

			double outER = kPlateA->aER[kPlateA->countER - ((kPlateA->countER > delayE) ? delayE + 1 : 0)];
			double outJR = kPlateA->aJR[kPlateA->countJR - ((kPlateA->countJR > delayJ) ? delayJ + 1 : 0)];
			double outOR = kPlateA->aOR[kPlateA->countOR - ((kPlateA->countOR > delayO) ? delayO + 1 : 0)];
			double outTR = kPlateA->aTR[kPlateA->countTR - ((kPlateA->countTR > delayT) ? delayT + 1 : 0)];
			double outYR = kPlateA->aYR[kPlateA->countYR - ((kPlateA->countYR > delayY) ? delayY + 1 : 0)];

			//-------- one

			outSample = (outAL * kPlateA->fixA[fix_a0]) + kPlateA->fixA[fix_sL1];
			kPlateA->fixA[fix_sL1] = (outAL * kPlateA->fixA[fix_a1]) - (outSample * kPlateA->fixA[fix_b1]) + kPlateA->fixA[fix_sL2];
			kPlateA->fixA[fix_sL2] = (outAL * kPlateA->fixA[fix_a2]) - (outSample * kPlateA->fixA[fix_b2]);
			outAL = outSample; // fixed biquad

			outSample = (outER * kPlateA->fixA[fix_a0]) + kPlateA->fixA[fix_sR1];
			kPlateA->fixA[fix_sR1] = (outER * kPlateA->fixA[fix_a1]) - (outSample * kPlateA->fixA[fix_b1]) + kPlateA->fixA[fix_sR2];
			kPlateA->fixA[fix_sR2] = (outER * kPlateA->fixA[fix_a2]) - (outSample * kPlateA->fixA[fix_b2]);
			outER = outSample; // fixed biquad

			//-------- mulch
			outSample = (outBL + kPlateA->prevMulchAL) * 0.5;
			kPlateA->prevMulchAL = outBL;
			outBL = outSample;
			outSample = (outJR + kPlateA->prevMulchAR) * 0.5;
			kPlateA->prevMulchAR = outJR;
			outJR = outSample;
			//-------- filtered (one path in five, feeding the rest of the matrix

			kPlateA->aFL[kPlateA->countFL] = ((outAL * 3.0) - ((outBL + outCL + outDL + outEL) * 2.0));
			kPlateA->aGL[kPlateA->countGL] = ((outBL * 3.0) - ((outAL + outCL + outDL + outEL) * 2.0));
			kPlateA->aHL[kPlateA->countHL] = ((outCL * 3.0) - ((outAL + outBL + outDL + outEL) * 2.0));
			kPlateA->aIL[kPlateA->countIL] = ((outDL * 3.0) - ((outAL + outBL + outCL + outEL) * 2.0));
			kPlateA->aJL[kPlateA->countJL] = ((outEL * 3.0) - ((outAL + outBL + outCL + outDL) * 2.0));

			kPlateA->aDR[kPlateA->countDR] = ((outER * 3.0) - ((outJR + outOR + outTR + outYR) * 2.0));
			kPlateA->aIR[kPlateA->countIR] = ((outJR * 3.0) - ((outER + outOR + outTR + outYR) * 2.0));
			kPlateA->aNR[kPlateA->countNR] = ((outOR * 3.0) - ((outER + outJR + outTR + outYR) * 2.0));
			kPlateA->aSR[kPlateA->countSR] = ((outTR * 3.0) - ((outER + outJR + outOR + outYR) * 2.0));
			kPlateA->aXR[kPlateA->countXR] = ((outYR * 3.0) - ((outER + outJR + outOR + outTR) * 2.0));

			kPlateA->countFL++;
			if (kPlateA->countFL < 0 || kPlateA->countFL > delayF) kPlateA->countFL = 0;
			kPlateA->countGL++;
			if (kPlateA->countGL < 0 || kPlateA->countGL > delayG) kPlateA->countGL = 0;
			kPlateA->countHL++;
			if (kPlateA->countHL < 0 || kPlateA->countHL > delayH) kPlateA->countHL = 0;
			kPlateA->countIL++;
			if (kPlateA->countIL < 0 || kPlateA->countIL > delayI) kPlateA->countIL = 0;
			kPlateA->countJL++;
			if (kPlateA->countJL < 0 || kPlateA->countJL > delayJ) kPlateA->countJL = 0;

			kPlateA->countDR++;
			if (kPlateA->countDR < 0 || kPlateA->countDR > delayD) kPlateA->countDR = 0;
			kPlateA->countIR++;
			if (kPlateA->countIR < 0 || kPlateA->countIR > delayI) kPlateA->countIR = 0;
			kPlateA->countNR++;
			if (kPlateA->countNR < 0 || kPlateA->countNR > delayN) kPlateA->countNR = 0;
			kPlateA->countSR++;
			if (kPlateA->countSR < 0 || kPlateA->countSR > delayS) kPlateA->countSR = 0;
			kPlateA->countXR++;
			if (kPlateA->countXR < 0 || kPlateA->countXR > delayX) kPlateA->countXR = 0;

			double outFL = kPlateA->aFL[kPlateA->countFL - ((kPlateA->countFL > delayF) ? delayF + 1 : 0)];
			double outGL = kPlateA->aGL[kPlateA->countGL - ((kPlateA->countGL > delayG) ? delayG + 1 : 0)];
			double outHL = kPlateA->aHL[kPlateA->countHL - ((kPlateA->countHL > delayH) ? delayH + 1 : 0)];
			double outIL = kPlateA->aIL[kPlateA->countIL - ((kPlateA->countIL > delayI) ? delayI + 1 : 0)];
			double outJL = kPlateA->aJL[kPlateA->countJL - ((kPlateA->countJL > delayJ) ? delayJ + 1 : 0)];

			double outDR = kPlateA->aDR[kPlateA->countDR - ((kPlateA->countDR > delayD) ? delayD + 1 : 0)];
			double outIR = kPlateA->aIR[kPlateA->countIR - ((kPlateA->countIR > delayI) ? delayI + 1 : 0)];
			double outNR = kPlateA->aNR[kPlateA->countNR - ((kPlateA->countNR > delayN) ? delayN + 1 : 0)];
			double outSR = kPlateA->aSR[kPlateA->countSR - ((kPlateA->countSR > delayS) ? delayS + 1 : 0)];
			double outXR = kPlateA->aXR[kPlateA->countXR - ((kPlateA->countXR > delayX) ? delayX + 1 : 0)];

			//-------- mulch

			outSample = (outFL * kPlateA->fixB[fix_a0]) + kPlateA->fixB[fix_sL1];
			kPlateA->fixB[fix_sL1] = (outFL * kPlateA->fixB[fix_a1]) - (outSample * kPlateA->fixB[fix_b1]) + kPlateA->fixB[fix_sL2];
			kPlateA->fixB[fix_sL2] = (outFL * kPlateA->fixB[fix_a2]) - (outSample * kPlateA->fixB[fix_b2]);
			outFL = outSample; // fixed biquad

			outSample = (outDR * kPlateA->fixB[fix_a0]) + kPlateA->fixB[fix_sR1];
			kPlateA->fixB[fix_sR1] = (outDR * kPlateA->fixB[fix_a1]) - (outSample * kPlateA->fixB[fix_b1]) + kPlateA->fixB[fix_sR2];
			kPlateA->fixB[fix_sR2] = (outDR * kPlateA->fixB[fix_a2]) - (outSample * kPlateA->fixB[fix_b2]);
			outDR = outSample; // fixed biquad

			outSample = (outGL + kPlateA->prevMulchBL) * 0.5;
			kPlateA->prevMulchBL = outGL;
			outGL = outSample;
			outSample = (outIR + kPlateA->prevMulchBR) * 0.5;
			kPlateA->prevMulchBR = outIR;
			outIR = outSample;

			//-------- two

			kPlateA->aKL[kPlateA->countKL] = ((outFL * 3.0) - ((outGL + outHL + outIL + outJL) * 2.0));
			kPlateA->aLL[kPlateA->countLL] = ((outGL * 3.0) - ((outFL + outHL + outIL + outJL) * 2.0));
			kPlateA->aML[kPlateA->countML] = ((outHL * 3.0) - ((outFL + outGL + outIL + outJL) * 2.0));
			kPlateA->aNL[kPlateA->countNL] = ((outIL * 3.0) - ((outFL + outGL + outHL + outJL) * 2.0));
			kPlateA->aOL[kPlateA->countOL] = ((outJL * 3.0) - ((outFL + outGL + outHL + outIL) * 2.0));

			kPlateA->aCR[kPlateA->countCR] = ((outDR * 3.0) - ((outIR + outNR + outSR + outXR) * 2.0));
			kPlateA->aHR[kPlateA->countHR] = ((outIR * 3.0) - ((outDR + outNR + outSR + outXR) * 2.0));
			kPlateA->aMR[kPlateA->countMR] = ((outNR * 3.0) - ((outDR + outIR + outSR + outXR) * 2.0));
			kPlateA->aRR[kPlateA->countRR] = ((outSR * 3.0) - ((outDR + outIR + outNR + outXR) * 2.0));
			kPlateA->aWR[kPlateA->countWR] = ((outXR * 3.0) - ((outDR + outIR + outNR + outSR) * 2.0));

			kPlateA->countKL++;
			if (kPlateA->countKL < 0 || kPlateA->countKL > delayK) kPlateA->countKL = 0;
			kPlateA->countLL++;
			if (kPlateA->countLL < 0 || kPlateA->countLL > delayL) kPlateA->countLL = 0;
			kPlateA->countML++;
			if (kPlateA->countML < 0 || kPlateA->countML > delayM) kPlateA->countML = 0;
			kPlateA->countNL++;
			if (kPlateA->countNL < 0 || kPlateA->countNL > delayN) kPlateA->countNL = 0;
			kPlateA->countOL++;
			if (kPlateA->countOL < 0 || kPlateA->countOL > delayO) kPlateA->countOL = 0;

			kPlateA->countCR++;
			if (kPlateA->countCR < 0 || kPlateA->countCR > delayC) kPlateA->countCR = 0;
			kPlateA->countHR++;
			if (kPlateA->countHR < 0 || kPlateA->countHR > delayH) kPlateA->countHR = 0;
			kPlateA->countMR++;
			if (kPlateA->countMR < 0 || kPlateA->countMR > delayM) kPlateA->countMR = 0;
			kPlateA->countRR++;
			if (kPlateA->countRR < 0 || kPlateA->countRR > delayR) kPlateA->countRR = 0;
			kPlateA->countWR++;
			if (kPlateA->countWR < 0 || kPlateA->countWR > delayW) kPlateA->countWR = 0;

			double outKL = kPlateA->aKL[kPlateA->countKL - ((kPlateA->countKL > delayK) ? delayK + 1 : 0)];
			double outLL = kPlateA->aLL[kPlateA->countLL - ((kPlateA->countLL > delayL) ? delayL + 1 : 0)];
			double outML = kPlateA->aML[kPlateA->countML - ((kPlateA->countML > delayM) ? delayM + 1 : 0)];
			double outNL = kPlateA->aNL[kPlateA->countNL - ((kPlateA->countNL > delayN) ? delayN + 1 : 0)];
			double outOL = kPlateA->aOL[kPlateA->countOL - ((kPlateA->countOL > delayO) ? delayO + 1 : 0)];

			double outCR = kPlateA->aCR[kPlateA->countCR - ((kPlateA->countCR > delayC) ? delayC + 1 : 0)];
			double outHR = kPlateA->aHR[kPlateA->countHR - ((kPlateA->countHR > delayH) ? delayH + 1 : 0)];
			double outMR = kPlateA->aMR[kPlateA->countMR - ((kPlateA->countMR > delayM) ? delayM + 1 : 0)];
			double outRR = kPlateA->aRR[kPlateA->countRR - ((kPlateA->countRR > delayR) ? delayR + 1 : 0)];
			double outWR = kPlateA->aWR[kPlateA->countWR - ((kPlateA->countWR > delayW) ? delayW + 1 : 0)];

			//-------- mulch

			outSample = (outKL * kPlateA->fixC[fix_a0]) + kPlateA->fixC[fix_sL1];
			kPlateA->fixC[fix_sL1] = (outKL * kPlateA->fixC[fix_a1]) - (outSample * kPlateA->fixC[fix_b1]) + kPlateA->fixC[fix_sL2];
			kPlateA->fixC[fix_sL2] = (outKL * kPlateA->fixC[fix_a2]) - (outSample * kPlateA->fixC[fix_b2]);
			outKL = outSample; // fixed biquad

			outSample = (outCR * kPlateA->fixC[fix_a0]) + kPlateA->fixC[fix_sR1];
			kPlateA->fixC[fix_sR1] = (outCR * kPlateA->fixC[fix_a1]) - (outSample * kPlateA->fixC[fix_b1]) + kPlateA->fixC[fix_sR2];
			kPlateA->fixC[fix_sR2] = (outCR * kPlateA->fixC[fix_a2]) - (outSample * kPlateA->fixC[fix_b2]);
			outCR = outSample; // fixed biquad

			outSample = (outLL + kPlateA->prevMulchCL) * 0.5;
			kPlateA->prevMulchCL = outLL;
			outLL = outSample;
			outSample = (outHR + kPlateA->prevMulchCR) * 0.5;
			kPlateA->prevMulchCR = outHR;
			outHR = outSample;

			//-------- three

			kPlateA->aPL[kPlateA->countPL] = ((outKL * 3.0) - ((outLL + outML + outNL + outOL) * 2.0));
			kPlateA->aQL[kPlateA->countQL] = ((outLL * 3.0) - ((outKL + outML + outNL + outOL) * 2.0));
			kPlateA->aRL[kPlateA->countRL] = ((outML * 3.0) - ((outKL + outLL + outNL + outOL) * 2.0));
			kPlateA->aSL[kPlateA->countSL] = ((outNL * 3.0) - ((outKL + outLL + outML + outOL) * 2.0));
			kPlateA->aTL[kPlateA->countTL] = ((outOL * 3.0) - ((outKL + outLL + outML + outNL) * 2.0));

			kPlateA->aBR[kPlateA->countBR] = ((outCR * 3.0) - ((outHR + outMR + outRR + outWR) * 2.0));
			kPlateA->aGR[kPlateA->countGR] = ((outHR * 3.0) - ((outCR + outMR + outRR + outWR) * 2.0));
			kPlateA->aLR[kPlateA->countLR] = ((outMR * 3.0) - ((outCR + outHR + outRR + outWR) * 2.0));
			kPlateA->aQR[kPlateA->countQR] = ((outRR * 3.0) - ((outCR + outHR + outMR + outWR) * 2.0));
			kPlateA->aVR[kPlateA->countVR] = ((outWR * 3.0) - ((outCR + outHR + outMR + outRR) * 2.0));

			kPlateA->countPL++;
			if (kPlateA->countPL < 0 || kPlateA->countPL > delayP) kPlateA->countPL = 0;
			kPlateA->countQL++;
			if (kPlateA->countQL < 0 || kPlateA->countQL > delayQ) kPlateA->countQL = 0;
			kPlateA->countRL++;
			if (kPlateA->countRL < 0 || kPlateA->countRL > delayR) kPlateA->countRL = 0;
			kPlateA->countSL++;
			if (kPlateA->countSL < 0 || kPlateA->countSL > delayS) kPlateA->countSL = 0;
			kPlateA->countTL++;
			if (kPlateA->countTL < 0 || kPlateA->countTL > delayT) kPlateA->countTL = 0;

			kPlateA->countBR++;
			if (kPlateA->countBR < 0 || kPlateA->countBR > delayB) kPlateA->countBR = 0;
			kPlateA->countGR++;
			if (kPlateA->countGR < 0 || kPlateA->countGR > delayG) kPlateA->countGR = 0;
			kPlateA->countLR++;
			if (kPlateA->countLR < 0 || kPlateA->countLR > delayL) kPlateA->countLR = 0;
			kPlateA->countQR++;
			if (kPlateA->countQR < 0 || kPlateA->countQR > delayQ) kPlateA->countQR = 0;
			kPlateA->countVR++;
			if (kPlateA->countVR < 0 || kPlateA->countVR > delayV) kPlateA->countVR = 0;

			double outPL = kPlateA->aPL[kPlateA->countPL - ((kPlateA->countPL > delayP) ? delayP + 1 : 0)];
			double outQL = kPlateA->aQL[kPlateA->countQL - ((kPlateA->countQL > delayQ) ? delayQ + 1 : 0)];
			double outRL = kPlateA->aRL[kPlateA->countRL - ((kPlateA->countRL > delayR) ? delayR + 1 : 0)];
			double outSL = kPlateA->aSL[kPlateA->countSL - ((kPlateA->countSL > delayS) ? delayS + 1 : 0)];
			double outTL = kPlateA->aTL[kPlateA->countTL - ((kPlateA->countTL > delayT) ? delayT + 1 : 0)];

			double outBR = kPlateA->aBR[kPlateA->countBR - ((kPlateA->countBR > delayB) ? delayB + 1 : 0)];
			double outGR = kPlateA->aGR[kPlateA->countGR - ((kPlateA->countGR > delayG) ? delayG + 1 : 0)];
			double outLR = kPlateA->aLR[kPlateA->countLR - ((kPlateA->countLR > delayL) ? delayL + 1 : 0)];
			double outQR = kPlateA->aQR[kPlateA->countQR - ((kPlateA->countQR > delayQ) ? delayQ + 1 : 0)];
			double outVR = kPlateA->aVR[kPlateA->countVR - ((kPlateA->countVR > delayV) ? delayV + 1 : 0)];

			//-------- mulch

			outSample = (outPL * kPlateA->fixD[fix_a0]) + kPlateA->fixD[fix_sL1];
			kPlateA->fixD[fix_sL1] = (outPL * kPlateA->fixD[fix_a1]) - (outSample * kPlateA->fixD[fix_b1]) + kPlateA->fixD[fix_sL2];
			kPlateA->fixD[fix_sL2] = (outPL * kPlateA->fixD[fix_a2]) - (outSample * kPlateA->fixD[fix_b2]);
			outPL = outSample; // fixed biquad

			outSample = (outBR * kPlateA->fixD[fix_a0]) + kPlateA->fixD[fix_sR1];
			kPlateA->fixD[fix_sR1] = (outBR * kPlateA->fixD[fix_a1]) - (outSample * kPlateA->fixD[fix_b1]) + kPlateA->fixD[fix_sR2];
			kPlateA->fixD[fix_sR2] = (outBR * kPlateA->fixD[fix_a2]) - (outSample * kPlateA->fixD[fix_b2]);
			outBR = outSample; // fixed biquad

			outSample = (outQL + kPlateA->prevMulchDL) * 0.5;
			kPlateA->prevMulchDL = outQL;
			outQL = outSample;
			outSample = (outGR + kPlateA->prevMulchDR) * 0.5;
			kPlateA->prevMulchDR = outGR;
			outGR = outSample;

			//-------- four

			kPlateA->aUL[kPlateA->countUL] = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0));
			kPlateA->aVL[kPlateA->countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
			kPlateA->aWL[kPlateA->countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
			kPlateA->aXL[kPlateA->countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
			kPlateA->aYL[kPlateA->countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));

			kPlateA->aAR[kPlateA->countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
			kPlateA->aFR[kPlateA->countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
			kPlateA->aKR[kPlateA->countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
			kPlateA->aPR[kPlateA->countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
			kPlateA->aUR[kPlateA->countUR] = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0));

			kPlateA->countUL++;
			if (kPlateA->countUL < 0 || kPlateA->countUL > delayU) kPlateA->countUL = 0;
			kPlateA->countVL++;
			if (kPlateA->countVL < 0 || kPlateA->countVL > delayV) kPlateA->countVL = 0;
			kPlateA->countWL++;
			if (kPlateA->countWL < 0 || kPlateA->countWL > delayW) kPlateA->countWL = 0;
			kPlateA->countXL++;
			if (kPlateA->countXL < 0 || kPlateA->countXL > delayX) kPlateA->countXL = 0;
			kPlateA->countYL++;
			if (kPlateA->countYL < 0 || kPlateA->countYL > delayY) kPlateA->countYL = 0;

			kPlateA->countAR++;
			if (kPlateA->countAR < 0 || kPlateA->countAR > delayA) kPlateA->countAR = 0;
			kPlateA->countFR++;
			if (kPlateA->countFR < 0 || kPlateA->countFR > delayF) kPlateA->countFR = 0;
			kPlateA->countKR++;
			if (kPlateA->countKR < 0 || kPlateA->countKR > delayK) kPlateA->countKR = 0;
			kPlateA->countPR++;
			if (kPlateA->countPR < 0 || kPlateA->countPR > delayP) kPlateA->countPR = 0;
			kPlateA->countUR++;
			if (kPlateA->countUR < 0 || kPlateA->countUR > delayU) kPlateA->countUR = 0;

			double outUL = kPlateA->aUL[kPlateA->countUL - ((kPlateA->countUL > delayU) ? delayU + 1 : 0)];
			double outVL = kPlateA->aVL[kPlateA->countVL - ((kPlateA->countVL > delayV) ? delayV + 1 : 0)];
			double outWL = kPlateA->aWL[kPlateA->countWL - ((kPlateA->countWL > delayW) ? delayW + 1 : 0)];
			double outXL = kPlateA->aXL[kPlateA->countXL - ((kPlateA->countXL > delayX) ? delayX + 1 : 0)];
			double outYL = kPlateA->aYL[kPlateA->countYL - ((kPlateA->countYL > delayY) ? delayY + 1 : 0)];

			double outAR = kPlateA->aAR[kPlateA->countAR - ((kPlateA->countAR > delayA) ? delayA + 1 : 0)];
			double outFR = kPlateA->aFR[kPlateA->countFR - ((kPlateA->countFR > delayF) ? delayF + 1 : 0)];
			double outKR = kPlateA->aKR[kPlateA->countKR - ((kPlateA->countKR > delayK) ? delayK + 1 : 0)];
			double outPR = kPlateA->aPR[kPlateA->countPR - ((kPlateA->countPR > delayP) ? delayP + 1 : 0)];
			double outUR = kPlateA->aUR[kPlateA->countUR - ((kPlateA->countUR > delayU) ? delayU + 1 : 0)];

			//-------- mulch
			outSample = (outVL + kPlateA->prevMulchEL) * 0.5;
			kPlateA->prevMulchEL = outVL;
			outVL = outSample;
			outSample = (outFR + kPlateA->prevMulchER) * 0.5;
			kPlateA->prevMulchER = outFR;
			outFR = outSample;
			//-------- five

			kPlateA->feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
			kPlateA->feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
			kPlateA->feedbackJR = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
			kPlateA->feedbackBL = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
			kPlateA->feedbackOR = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
			kPlateA->feedbackCL = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
			kPlateA->feedbackDL = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
			kPlateA->feedbackTR = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
			kPlateA->feedbackEL = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
			kPlateA->feedbackYR = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
			// which we need to feed back into the input again, a bit

			inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0016;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0016;
			// and take the final combined sum of outputs, corrected for Householder gain

			inputSampleL *= 0.5;
			inputSampleR *= 0.5;
			if (kPlateA->gainOutL < 0.0078125) kPlateA->gainOutL = 0.0078125;
			if (kPlateA->gainOutL > 1.0) kPlateA->gainOutL = 1.0;
			if (kPlateA->gainOutR < 0.0078125) kPlateA->gainOutR = 0.0078125;
			if (kPlateA->gainOutR > 1.0) kPlateA->gainOutR = 1.0;
			// gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			// smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= kPlateA->gainOutL;
			inputSampleR *= kPlateA->gainOutR;
			kPlateA->gainOutL += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
			kPlateA->gainOutR += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
			// 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			inputSampleL *= 2.0;
			inputSampleR *= 2.0;
			// curve! To get a compressed effect that matches a certain other plugin
			// that is too overprocessed for its own good :)

			outSample = (inputSampleL + kPlateA->prevOutAL) * 0.5;
			kPlateA->prevOutAL = inputSampleL;
			inputSampleL = outSample;
			outSample = (inputSampleR + kPlateA->prevOutAR) * 0.5;
			kPlateA->prevOutAR = inputSampleR;
			inputSampleR = outSample;

			if (cycleEnd == 4) {
				kPlateA->lastRefL[0] = kPlateA->lastRefL[4]; // start from previous last
				kPlateA->lastRefL[2] = (kPlateA->lastRefL[0] + inputSampleL) / 2; // half
				kPlateA->lastRefL[1] = (kPlateA->lastRefL[0] + kPlateA->lastRefL[2]) / 2; // one quarter
				kPlateA->lastRefL[3] = (kPlateA->lastRefL[2] + inputSampleL) / 2; // three quarters
				kPlateA->lastRefL[4] = inputSampleL; // full
				kPlateA->lastRefR[0] = kPlateA->lastRefR[4]; // start from previous last
				kPlateA->lastRefR[2] = (kPlateA->lastRefR[0] + inputSampleR) / 2; // half
				kPlateA->lastRefR[1] = (kPlateA->lastRefR[0] + kPlateA->lastRefR[2]) / 2; // one quarter
				kPlateA->lastRefR[3] = (kPlateA->lastRefR[2] + inputSampleR) / 2; // three quarters
				kPlateA->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				kPlateA->lastRefL[0] = kPlateA->lastRefL[3]; // start from previous last
				kPlateA->lastRefL[2] = (kPlateA->lastRefL[0] + kPlateA->lastRefL[0] + inputSampleL) / 3; // third
				kPlateA->lastRefL[1] = (kPlateA->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				kPlateA->lastRefL[3] = inputSampleL; // full
				kPlateA->lastRefR[0] = kPlateA->lastRefR[3]; // start from previous last
				kPlateA->lastRefR[2] = (kPlateA->lastRefR[0] + kPlateA->lastRefR[0] + inputSampleR) / 3; // third
				kPlateA->lastRefR[1] = (kPlateA->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				kPlateA->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				kPlateA->lastRefL[0] = kPlateA->lastRefL[2]; // start from previous last
				kPlateA->lastRefL[1] = (kPlateA->lastRefL[0] + inputSampleL) / 2; // half
				kPlateA->lastRefL[2] = inputSampleL; // full
				kPlateA->lastRefR[0] = kPlateA->lastRefR[2]; // start from previous last
				kPlateA->lastRefR[1] = (kPlateA->lastRefR[0] + inputSampleR) / 2; // half
				kPlateA->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				kPlateA->lastRefL[0] = inputSampleL;
				kPlateA->lastRefR[0] = inputSampleR;
			}
			kPlateA->cycle = 0; // reset
			inputSampleL = kPlateA->lastRefL[kPlateA->cycle];
			inputSampleR = kPlateA->lastRefR[kPlateA->cycle];
		} else {
			inputSampleL = kPlateA->lastRefL[kPlateA->cycle];
			inputSampleR = kPlateA->lastRefR[kPlateA->cycle];
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
		kPlateA->fpdL ^= kPlateA->fpdL << 13;
		kPlateA->fpdL ^= kPlateA->fpdL >> 17;
		kPlateA->fpdL ^= kPlateA->fpdL << 5;
		inputSampleL += (((double) kPlateA->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		kPlateA->fpdR ^= kPlateA->fpdR << 13;
		kPlateA->fpdR ^= kPlateA->fpdR >> 17;
		kPlateA->fpdR ^= kPlateA->fpdR << 5;
		inputSampleR += (((double) kPlateA->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	KPLATEA_URI,
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
