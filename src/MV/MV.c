#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define MV_URI "https://hannesbraun.net/ns/lv2/airwindows/mv"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DEPTH = 4,
	BRIGHT = 5,
	REGEN = 6,
	OUTPUT = 7,
	DRYWET = 8
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* depth;
	const float* bright;
	const float* regen;
	const float* gain;
	const float* drywet;

	uint32_t fpdL;
	uint32_t fpdR;

	double aAL[15150];
	double aBL[14618];
	double aCL[14358];
	double aDL[13818];
	double aEL[13562];
	double aFL[13046];
	double aGL[11966];
	double aHL[11130];
	double aIL[10598];
	double aJL[9810];
	double aKL[9522];
	double aLL[8982];
	double aML[8786];
	double aNL[8462];
	double aOL[8310];
	double aPL[7982];
	double aQL[7322];
	double aRL[6818];
	double aSL[6506];
	double aTL[6002];
	double aUL[5838];
	double aVL[5502];
	double aWL[5010];
	double aXL[4850];
	double aYL[4296];
	double aZL[4180];

	double avgAL;
	double avgBL;
	double avgCL;
	double avgDL;
	double avgEL;
	double avgFL;
	double avgGL;
	double avgHL;
	double avgIL;
	double avgJL;
	double avgKL;
	double avgLL;
	double avgML;
	double avgNL;
	double avgOL;
	double avgPL;
	double avgQL;
	double avgRL;
	double avgSL;
	double avgTL;
	double avgUL;
	double avgVL;
	double avgWL;
	double avgXL;
	double avgYL;
	double avgZL;

	double feedbackL;

	double aAR[15150];
	double aBR[14618];
	double aCR[14358];
	double aDR[13818];
	double aER[13562];
	double aFR[13046];
	double aGR[11966];
	double aHR[11130];
	double aIR[10598];
	double aJR[9810];
	double aKR[9522];
	double aLR[8982];
	double aMR[8786];
	double aNR[8462];
	double aOR[8310];
	double aPR[7982];
	double aQR[7322];
	double aRR[6818];
	double aSR[6506];
	double aTR[6002];
	double aUR[5838];
	double aVR[5502];
	double aWR[5010];
	double aXR[4850];
	double aYR[4296];
	double aZR[4180];

	double avgAR;
	double avgBR;
	double avgCR;
	double avgDR;
	double avgER;
	double avgFR;
	double avgGR;
	double avgHR;
	double avgIR;
	double avgJR;
	double avgKR;
	double avgLR;
	double avgMR;
	double avgNR;
	double avgOR;
	double avgPR;
	double avgQR;
	double avgRR;
	double avgSR;
	double avgTR;
	double avgUR;
	double avgVR;
	double avgWR;
	double avgXR;
	double avgYR;
	double avgZR;

	double feedbackR;

	// these are delay lengths and same for both sides
	int alpA, delayA;
	int alpB, delayB;
	int alpC, delayC;
	int alpD, delayD;
	int alpE, delayE;
	int alpF, delayF;
	int alpG, delayG;
	int alpH, delayH;
	int alpI, delayI;
	int alpJ, delayJ;
	int alpK, delayK;
	int alpL, delayL;
	int alpM, delayM;
	int alpN, delayN;
	int alpO, delayO;
	int alpP, delayP;
	int alpQ, delayQ;
	int alpR, delayR;
	int alpS, delayS;
	int alpT, delayT;
	int alpU, delayU;
	int alpV, delayV;
	int alpW, delayW;
	int alpX, delayX;
	int alpY, delayY;
	int alpZ, delayZ;
} MV;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	MV* mv = (MV*) calloc(1, sizeof(MV));
	return (LV2_Handle) mv;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	MV* mv = (MV*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			mv->input[0] = (const float*) data;
			break;
		case INPUT_R:
			mv->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			mv->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			mv->output[1] = (float*) data;
			break;
		case DEPTH:
			mv->depth = (const float*) data;
			break;
		case BRIGHT:
			mv->bright = (const float*) data;
			break;
		case REGEN:
			mv->regen = (const float*) data;
			break;
		case OUTPUT:
			mv->gain = (const float*) data;
			break;
		case DRYWET:
			mv->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	MV* mv = (MV*) instance;

	int count;
	for (count = 0; count < 15149; count++) {
		mv->aAL[count] = 0.0;
		mv->aAR[count] = 0.0;
	}
	for (count = 0; count < 14617; count++) {
		mv->aBL[count] = 0.0;
		mv->aBR[count] = 0.0;
	}
	for (count = 0; count < 14357; count++) {
		mv->aCL[count] = 0.0;
		mv->aCR[count] = 0.0;
	}
	for (count = 0; count < 13817; count++) {
		mv->aDL[count] = 0.0;
		mv->aDR[count] = 0.0;
	}
	for (count = 0; count < 13561; count++) {
		mv->aEL[count] = 0.0;
		mv->aER[count] = 0.0;
	}
	for (count = 0; count < 13045; count++) {
		mv->aFL[count] = 0.0;
		mv->aFR[count] = 0.0;
	}
	for (count = 0; count < 11965; count++) {
		mv->aGL[count] = 0.0;
		mv->aGR[count] = 0.0;
	}
	for (count = 0; count < 11129; count++) {
		mv->aHL[count] = 0.0;
		mv->aHR[count] = 0.0;
	}
	for (count = 0; count < 10597; count++) {
		mv->aIL[count] = 0.0;
		mv->aIR[count] = 0.0;
	}
	for (count = 0; count < 9809; count++) {
		mv->aJL[count] = 0.0;
		mv->aJR[count] = 0.0;
	}
	for (count = 0; count < 9521; count++) {
		mv->aKL[count] = 0.0;
		mv->aKR[count] = 0.0;
	}
	for (count = 0; count < 8981; count++) {
		mv->aLL[count] = 0.0;
		mv->aLR[count] = 0.0;
	}
	for (count = 0; count < 8785; count++) {
		mv->aML[count] = 0.0;
		mv->aMR[count] = 0.0;
	}
	for (count = 0; count < 8461; count++) {
		mv->aNL[count] = 0.0;
		mv->aNR[count] = 0.0;
	}
	for (count = 0; count < 8309; count++) {
		mv->aOL[count] = 0.0;
		mv->aOR[count] = 0.0;
	}
	for (count = 0; count < 7981; count++) {
		mv->aPL[count] = 0.0;
		mv->aPR[count] = 0.0;
	}
	for (count = 0; count < 7321; count++) {
		mv->aQL[count] = 0.0;
		mv->aQR[count] = 0.0;
	}
	for (count = 0; count < 6817; count++) {
		mv->aRL[count] = 0.0;
		mv->aRR[count] = 0.0;
	}
	for (count = 0; count < 6505; count++) {
		mv->aSL[count] = 0.0;
		mv->aSR[count] = 0.0;
	}
	for (count = 0; count < 6001; count++) {
		mv->aTL[count] = 0.0;
		mv->aTR[count] = 0.0;
	}
	for (count = 0; count < 5837; count++) {
		mv->aUL[count] = 0.0;
		mv->aUR[count] = 0.0;
	}
	for (count = 0; count < 5501; count++) {
		mv->aVL[count] = 0.0;
		mv->aVR[count] = 0.0;
	}
	for (count = 0; count < 5009; count++) {
		mv->aWL[count] = 0.0;
		mv->aWR[count] = 0.0;
	}
	for (count = 0; count < 4849; count++) {
		mv->aXL[count] = 0.0;
		mv->aXR[count] = 0.0;
	}
	for (count = 0; count < 4295; count++) {
		mv->aYL[count] = 0.0;
		mv->aYR[count] = 0.0;
	}
	for (count = 0; count < 4179; count++) {
		mv->aZL[count] = 0.0;
		mv->aZR[count] = 0.0;
	}

	mv->alpA = 1;
	mv->delayA = 7573;
	mv->avgAL = 0.0;
	mv->avgAR = 0.0;
	mv->alpB = 1;
	mv->delayB = 7307;
	mv->avgBL = 0.0;
	mv->avgBR = 0.0;
	mv->alpC = 1;
	mv->delayC = 7177;
	mv->avgCL = 0.0;
	mv->avgCR = 0.0;
	mv->alpD = 1;
	mv->delayD = 6907;
	mv->avgDL = 0.0;
	mv->avgDR = 0.0;
	mv->alpE = 1;
	mv->delayE = 6779;
	mv->avgEL = 0.0;
	mv->avgER = 0.0;
	mv->alpF = 1;
	mv->delayF = 6521;
	mv->avgFL = 0.0;
	mv->avgFR = 0.0;
	mv->alpG = 1;
	mv->delayG = 5981;
	mv->avgGL = 0.0;
	mv->avgGR = 0.0;
	mv->alpH = 1;
	mv->delayH = 5563;
	mv->avgHL = 0.0;
	mv->avgHR = 0.0;
	mv->alpI = 1;
	mv->delayI = 5297;
	mv->avgIL = 0.0;
	mv->avgIR = 0.0;
	mv->alpJ = 1;
	mv->delayJ = 4903;
	mv->avgJL = 0.0;
	mv->avgJR = 0.0;
	mv->alpK = 1;
	mv->delayK = 4759;
	mv->avgKL = 0.0;
	mv->avgKR = 0.0;
	mv->alpL = 1;
	mv->delayL = 4489;
	mv->avgLL = 0.0;
	mv->avgLR = 0.0;
	mv->alpM = 1;
	mv->delayM = 4391;
	mv->avgML = 0.0;
	mv->avgMR = 0.0;
	mv->alpN = 1;
	mv->delayN = 4229;
	mv->avgNL = 0.0;
	mv->avgNR = 0.0;
	mv->alpO = 1;
	mv->delayO = 4153;
	mv->avgOL = 0.0;
	mv->avgOR = 0.0;
	mv->alpP = 1;
	mv->delayP = 3989;
	mv->avgPL = 0.0;
	mv->avgPR = 0.0;
	mv->alpQ = 1;
	mv->delayQ = 3659;
	mv->avgQL = 0.0;
	mv->avgQR = 0.0;
	mv->alpR = 1;
	mv->delayR = 3407;
	mv->avgRL = 0.0;
	mv->avgRR = 0.0;
	mv->alpS = 1;
	mv->delayS = 3251;
	mv->avgSL = 0.0;
	mv->avgSR = 0.0;
	mv->alpT = 1;
	mv->delayT = 2999;
	mv->avgTL = 0.0;
	mv->avgTR = 0.0;
	mv->alpU = 1;
	mv->delayU = 2917;
	mv->avgUL = 0.0;
	mv->avgUR = 0.0;
	mv->alpV = 1;
	mv->delayV = 2749;
	mv->avgVL = 0.0;
	mv->avgVR = 0.0;
	mv->alpW = 1;
	mv->delayW = 2503;
	mv->avgWL = 0.0;
	mv->avgWR = 0.0;
	mv->alpX = 1;
	mv->delayX = 2423;
	mv->avgXL = 0.0;
	mv->avgXR = 0.0;
	mv->alpY = 1;
	mv->delayY = 2146;
	mv->avgYL = 0.0;
	mv->avgYR = 0.0;
	mv->alpZ = 1;
	mv->delayZ = 2088;
	mv->avgZL = 0.0;
	mv->avgZR = 0.0;

	mv->feedbackL = 0.0;
	mv->feedbackR = 0.0;

	mv->fpdL = 1.0;
	while (mv->fpdL < 16386) mv->fpdL = rand() * UINT32_MAX;
	mv->fpdR = 1.0;
	while (mv->fpdR < 16386) mv->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	MV* mv = (MV*) instance;

	const float* in1 = mv->input[0];
	const float* in2 = mv->input[1];
	float* out1 = mv->output[0];
	float* out2 = mv->output[1];

	int allpasstemp;
	double avgtemp;
	const int stage = *mv->depth * 27.0;
	const int damp = (1.0 - *mv->bright) * stage;
	double feedbacklevel = *mv->regen;
	if (feedbacklevel <= 0.0625) feedbacklevel = 0.0;
	if (feedbacklevel > 0.0625 && feedbacklevel <= 0.125) feedbacklevel = 0.0625; //-24db
	if (feedbacklevel > 0.125 && feedbacklevel <= 0.25) feedbacklevel = 0.125; //-18db
	if (feedbacklevel > 0.25 && feedbacklevel <= 0.5) feedbacklevel = 0.25; //-12db
	if (feedbacklevel > 0.5 && feedbacklevel <= 0.99) feedbacklevel = 0.5; //-6db
	if (feedbacklevel > 0.99) feedbacklevel = 1.0;
	// we're forcing even the feedback level to be Midiverb-ized
	const double gain = *mv->gain;
	const double wet = *mv->drywet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = mv->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = mv->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL += mv->feedbackL;
		inputSampleR += mv->feedbackR;

		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);

		switch (stage) {
			case 27:
			case 26:
				allpasstemp = mv->alpA - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayA) {
					allpasstemp = mv->delayA;
				}
				inputSampleL -= mv->aAL[allpasstemp] * 0.5;
				mv->aAL[mv->alpA] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aAR[allpasstemp] * 0.5;
				mv->aAR[mv->alpA] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpA--;
				if (mv->alpA < 0 || mv->alpA > mv->delayA) {
					mv->alpA = mv->delayA;
				}
				inputSampleL += (mv->aAL[mv->alpA]);
				inputSampleR += (mv->aAR[mv->alpA]);
				if (damp > 26) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgAL;
					inputSampleL *= 0.5;
					mv->avgAL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgAR;
					inputSampleR *= 0.5;
					mv->avgAR = avgtemp;
				}
			// allpass filter A
			case 25:
				allpasstemp = mv->alpB - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayB) {
					allpasstemp = mv->delayB;
				}
				inputSampleL -= mv->aBL[allpasstemp] * 0.5;
				mv->aBL[mv->alpB] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aBR[allpasstemp] * 0.5;
				mv->aBR[mv->alpB] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpB--;
				if (mv->alpB < 0 || mv->alpB > mv->delayB) {
					mv->alpB = mv->delayB;
				}
				inputSampleL += (mv->aBL[mv->alpB]);
				inputSampleR += (mv->aBR[mv->alpB]);
				if (damp > 25) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgBL;
					inputSampleL *= 0.5;
					mv->avgBL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgBR;
					inputSampleR *= 0.5;
					mv->avgBR = avgtemp;
				}
			// allpass filter B
			case 24:
				allpasstemp = mv->alpC - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayC) {
					allpasstemp = mv->delayC;
				}
				inputSampleL -= mv->aCL[allpasstemp] * 0.5;
				mv->aCL[mv->alpC] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aCR[allpasstemp] * 0.5;
				mv->aCR[mv->alpC] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpC--;
				if (mv->alpC < 0 || mv->alpC > mv->delayC) {
					mv->alpC = mv->delayC;
				}
				inputSampleL += (mv->aCL[mv->alpC]);
				inputSampleR += (mv->aCR[mv->alpC]);
				if (damp > 24) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgCL;
					inputSampleL *= 0.5;
					mv->avgCL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgCR;
					inputSampleR *= 0.5;
					mv->avgCR = avgtemp;
				}
			// allpass filter C
			case 23:
				allpasstemp = mv->alpD - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayD) {
					allpasstemp = mv->delayD;
				}
				inputSampleL -= mv->aDL[allpasstemp] * 0.5;
				mv->aDL[mv->alpD] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aDR[allpasstemp] * 0.5;
				mv->aDR[mv->alpD] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpD--;
				if (mv->alpD < 0 || mv->alpD > mv->delayD) {
					mv->alpD = mv->delayD;
				}
				inputSampleL += (mv->aDL[mv->alpD]);
				inputSampleR += (mv->aDR[mv->alpD]);
				if (damp > 23) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgDL;
					inputSampleL *= 0.5;
					mv->avgDL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgDR;
					inputSampleR *= 0.5;
					mv->avgDR = avgtemp;
				}
			// allpass filter D
			case 22:
				allpasstemp = mv->alpE - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayE) {
					allpasstemp = mv->delayE;
				}
				inputSampleL -= mv->aEL[allpasstemp] * 0.5;
				mv->aEL[mv->alpE] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aER[allpasstemp] * 0.5;
				mv->aER[mv->alpE] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpE--;
				if (mv->alpE < 0 || mv->alpE > mv->delayE) {
					mv->alpE = mv->delayE;
				}
				inputSampleL += (mv->aEL[mv->alpE]);
				inputSampleR += (mv->aER[mv->alpE]);
				if (damp > 22) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgEL;
					inputSampleL *= 0.5;
					mv->avgEL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgER;
					inputSampleR *= 0.5;
					mv->avgER = avgtemp;
				}
			// allpass filter E
			case 21:
				allpasstemp = mv->alpF - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayF) {
					allpasstemp = mv->delayF;
				}
				inputSampleL -= mv->aFL[allpasstemp] * 0.5;
				mv->aFL[mv->alpF] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aFR[allpasstemp] * 0.5;
				mv->aFR[mv->alpF] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpF--;
				if (mv->alpF < 0 || mv->alpF > mv->delayF) {
					mv->alpF = mv->delayF;
				}
				inputSampleL += (mv->aFL[mv->alpF]);
				inputSampleR += (mv->aFR[mv->alpF]);
				if (damp > 21) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgFL;
					inputSampleL *= 0.5;
					mv->avgFL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgFR;
					inputSampleR *= 0.5;
					mv->avgFR = avgtemp;
				}
			// allpass filter F
			case 20:
				allpasstemp = mv->alpG - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayG) {
					allpasstemp = mv->delayG;
				}
				inputSampleL -= mv->aGL[allpasstemp] * 0.5;
				mv->aGL[mv->alpG] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aGR[allpasstemp] * 0.5;
				mv->aGR[mv->alpG] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpG--;
				if (mv->alpG < 0 || mv->alpG > mv->delayG) {
					mv->alpG = mv->delayG;
				}
				inputSampleL += (mv->aGL[mv->alpG]);
				inputSampleR += (mv->aGR[mv->alpG]);
				if (damp > 20) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgGL;
					inputSampleL *= 0.5;
					mv->avgGL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgGR;
					inputSampleR *= 0.5;
					mv->avgGR = avgtemp;
				}
			// allpass filter G
			case 19:
				allpasstemp = mv->alpH - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayH) {
					allpasstemp = mv->delayH;
				}
				inputSampleL -= mv->aHL[allpasstemp] * 0.5;
				mv->aHL[mv->alpH] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aHR[allpasstemp] * 0.5;
				mv->aHR[mv->alpH] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpH--;
				if (mv->alpH < 0 || mv->alpH > mv->delayH) {
					mv->alpH = mv->delayH;
				}
				inputSampleL += (mv->aHL[mv->alpH]);
				inputSampleR += (mv->aHR[mv->alpH]);
				if (damp > 19) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgHL;
					inputSampleL *= 0.5;
					mv->avgHL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgHR;
					inputSampleR *= 0.5;
					mv->avgHR = avgtemp;
				}
			// allpass filter H
			case 18:
				allpasstemp = mv->alpI - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayI) {
					allpasstemp = mv->delayI;
				}
				inputSampleL -= mv->aIL[allpasstemp] * 0.5;
				mv->aIL[mv->alpI] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aIR[allpasstemp] * 0.5;
				mv->aIR[mv->alpI] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpI--;
				if (mv->alpI < 0 || mv->alpI > mv->delayI) {
					mv->alpI = mv->delayI;
				}
				inputSampleL += (mv->aIL[mv->alpI]);
				inputSampleR += (mv->aIR[mv->alpI]);
				if (damp > 18) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgIL;
					inputSampleL *= 0.5;
					mv->avgIL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgIR;
					inputSampleR *= 0.5;
					mv->avgIR = avgtemp;
				}
			// allpass filter I
			case 17:
				allpasstemp = mv->alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayJ) {
					allpasstemp = mv->delayJ;
				}
				inputSampleL -= mv->aJL[allpasstemp] * 0.5;
				mv->aJL[mv->alpJ] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aJR[allpasstemp] * 0.5;
				mv->aJR[mv->alpJ] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpJ--;
				if (mv->alpJ < 0 || mv->alpJ > mv->delayJ) {
					mv->alpJ = mv->delayJ;
				}
				inputSampleL += (mv->aJL[mv->alpJ]);
				inputSampleR += (mv->aJR[mv->alpJ]);
				if (damp > 17) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgJL;
					inputSampleL *= 0.5;
					mv->avgJL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgJR;
					inputSampleR *= 0.5;
					mv->avgJR = avgtemp;
				}
			// allpass filter J
			case 16:
				allpasstemp = mv->alpK - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayK) {
					allpasstemp = mv->delayK;
				}
				inputSampleL -= mv->aKL[allpasstemp] * 0.5;
				mv->aKL[mv->alpK] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aKR[allpasstemp] * 0.5;
				mv->aKR[mv->alpK] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpK--;
				if (mv->alpK < 0 || mv->alpK > mv->delayK) {
					mv->alpK = mv->delayK;
				}
				inputSampleL += (mv->aKL[mv->alpK]);
				inputSampleR += (mv->aKR[mv->alpK]);
				if (damp > 16) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgKL;
					inputSampleL *= 0.5;
					mv->avgKL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgKR;
					inputSampleR *= 0.5;
					mv->avgKR = avgtemp;
				}
			// allpass filter K
			case 15:
				allpasstemp = mv->alpL - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayL) {
					allpasstemp = mv->delayL;
				}
				inputSampleL -= mv->aLL[allpasstemp] * 0.5;
				mv->aLL[mv->alpL] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aLR[allpasstemp] * 0.5;
				mv->aLR[mv->alpL] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpL--;
				if (mv->alpL < 0 || mv->alpL > mv->delayL) {
					mv->alpL = mv->delayL;
				}
				inputSampleL += (mv->aLL[mv->alpL]);
				inputSampleR += (mv->aLR[mv->alpL]);
				if (damp > 15) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgLL;
					inputSampleL *= 0.5;
					mv->avgLL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgLR;
					inputSampleR *= 0.5;
					mv->avgLR = avgtemp;
				}
			// allpass filter L
			case 14:
				allpasstemp = mv->alpM - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayM) {
					allpasstemp = mv->delayM;
				}
				inputSampleL -= mv->aML[allpasstemp] * 0.5;
				mv->aML[mv->alpM] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aMR[allpasstemp] * 0.5;
				mv->aMR[mv->alpM] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpM--;
				if (mv->alpM < 0 || mv->alpM > mv->delayM) {
					mv->alpM = mv->delayM;
				}
				inputSampleL += (mv->aML[mv->alpM]);
				inputSampleR += (mv->aMR[mv->alpM]);
				if (damp > 14) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgML;
					inputSampleL *= 0.5;
					mv->avgML = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgMR;
					inputSampleR *= 0.5;
					mv->avgMR = avgtemp;
				}
			// allpass filter M
			case 13:
				allpasstemp = mv->alpN - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayN) {
					allpasstemp = mv->delayN;
				}
				inputSampleL -= mv->aNL[allpasstemp] * 0.5;
				mv->aNL[mv->alpN] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aNR[allpasstemp] * 0.5;
				mv->aNR[mv->alpN] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpN--;
				if (mv->alpN < 0 || mv->alpN > mv->delayN) {
					mv->alpN = mv->delayN;
				}
				inputSampleL += (mv->aNL[mv->alpN]);
				inputSampleR += (mv->aNR[mv->alpN]);
				if (damp > 13) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgNL;
					inputSampleL *= 0.5;
					mv->avgNL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgNR;
					inputSampleR *= 0.5;
					mv->avgNR = avgtemp;
				}
			// allpass filter N
			case 12:
				allpasstemp = mv->alpO - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayO) {
					allpasstemp = mv->delayO;
				}
				inputSampleL -= mv->aOL[allpasstemp] * 0.5;
				mv->aOL[mv->alpO] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aOR[allpasstemp] * 0.5;
				mv->aOR[mv->alpO] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpO--;
				if (mv->alpO < 0 || mv->alpO > mv->delayO) {
					mv->alpO = mv->delayO;
				}
				inputSampleL += (mv->aOL[mv->alpO]);
				inputSampleR += (mv->aOR[mv->alpO]);
				if (damp > 12) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgOL;
					inputSampleL *= 0.5;
					mv->avgOL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgOR;
					inputSampleR *= 0.5;
					mv->avgOR = avgtemp;
				}
			// allpass filter O
			case 11:
				allpasstemp = mv->alpP - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayP) {
					allpasstemp = mv->delayP;
				}
				inputSampleL -= mv->aPL[allpasstemp] * 0.5;
				mv->aPL[mv->alpP] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aPR[allpasstemp] * 0.5;
				mv->aPR[mv->alpP] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpP--;
				if (mv->alpP < 0 || mv->alpP > mv->delayP) {
					mv->alpP = mv->delayP;
				}
				inputSampleL += (mv->aPL[mv->alpP]);
				inputSampleR += (mv->aPR[mv->alpP]);
				if (damp > 11) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgPL;
					inputSampleL *= 0.5;
					mv->avgPL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgPR;
					inputSampleR *= 0.5;
					mv->avgPR = avgtemp;
				}
			// allpass filter P
			case 10:
				allpasstemp = mv->alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayQ) {
					allpasstemp = mv->delayQ;
				}
				inputSampleL -= mv->aQL[allpasstemp] * 0.5;
				mv->aQL[mv->alpQ] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aQR[allpasstemp] * 0.5;
				mv->aQR[mv->alpQ] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpQ--;
				if (mv->alpQ < 0 || mv->alpQ > mv->delayQ) {
					mv->alpQ = mv->delayQ;
				}
				inputSampleL += (mv->aQL[mv->alpQ]);
				inputSampleR += (mv->aQR[mv->alpQ]);
				if (damp > 10) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgQL;
					inputSampleL *= 0.5;
					mv->avgQL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgQR;
					inputSampleR *= 0.5;
					mv->avgQR = avgtemp;
				}
			// allpass filter Q
			case 9:
				allpasstemp = mv->alpR - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayR) {
					allpasstemp = mv->delayR;
				}
				inputSampleL -= mv->aRL[allpasstemp] * 0.5;
				mv->aRL[mv->alpR] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aRR[allpasstemp] * 0.5;
				mv->aRR[mv->alpR] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpR--;
				if (mv->alpR < 0 || mv->alpR > mv->delayR) {
					mv->alpR = mv->delayR;
				}
				inputSampleL += (mv->aRL[mv->alpR]);
				inputSampleR += (mv->aRR[mv->alpR]);
				if (damp > 9) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgRL;
					inputSampleL *= 0.5;
					mv->avgRL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgRR;
					inputSampleR *= 0.5;
					mv->avgRR = avgtemp;
				}
			// allpass filter R
			case 8:
				allpasstemp = mv->alpS - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayS) {
					allpasstemp = mv->delayS;
				}
				inputSampleL -= mv->aSL[allpasstemp] * 0.5;
				mv->aSL[mv->alpS] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aSR[allpasstemp] * 0.5;
				mv->aSR[mv->alpS] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpS--;
				if (mv->alpS < 0 || mv->alpS > mv->delayS) {
					mv->alpS = mv->delayS;
				}
				inputSampleL += (mv->aSL[mv->alpS]);
				inputSampleR += (mv->aSR[mv->alpS]);
				if (damp > 8) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgSL;
					inputSampleL *= 0.5;
					mv->avgSL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgSR;
					inputSampleR *= 0.5;
					mv->avgSR = avgtemp;
				}
			// allpass filter S
			case 7:
				allpasstemp = mv->alpT - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayT) {
					allpasstemp = mv->delayT;
				}
				inputSampleL -= mv->aTL[allpasstemp] * 0.5;
				mv->aTL[mv->alpT] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aTR[allpasstemp] * 0.5;
				mv->aTR[mv->alpT] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpT--;
				if (mv->alpT < 0 || mv->alpT > mv->delayT) {
					mv->alpT = mv->delayT;
				}
				inputSampleL += (mv->aTL[mv->alpT]);
				inputSampleR += (mv->aTR[mv->alpT]);
				if (damp > 7) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgTL;
					inputSampleL *= 0.5;
					mv->avgTL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgTR;
					inputSampleR *= 0.5;
					mv->avgTR = avgtemp;
				}
			// allpass filter T
			case 6:
				allpasstemp = mv->alpU - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayU) {
					allpasstemp = mv->delayU;
				}
				inputSampleL -= mv->aUL[allpasstemp] * 0.5;
				mv->aUL[mv->alpU] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aUR[allpasstemp] * 0.5;
				mv->aUR[mv->alpU] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpU--;
				if (mv->alpU < 0 || mv->alpU > mv->delayU) {
					mv->alpU = mv->delayU;
				}
				inputSampleL += (mv->aUL[mv->alpU]);
				inputSampleR += (mv->aUR[mv->alpU]);
				if (damp > 6) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgUL;
					inputSampleL *= 0.5;
					mv->avgUL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgUR;
					inputSampleR *= 0.5;
					mv->avgUR = avgtemp;
				}
			// allpass filter U
			case 5:
				allpasstemp = mv->alpV - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayV) {
					allpasstemp = mv->delayV;
				}
				inputSampleL -= mv->aVL[allpasstemp] * 0.5;
				mv->aVL[mv->alpV] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aVR[allpasstemp] * 0.5;
				mv->aVR[mv->alpV] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpV--;
				if (mv->alpV < 0 || mv->alpV > mv->delayV) {
					mv->alpV = mv->delayV;
				}
				inputSampleL += (mv->aVL[mv->alpV]);
				inputSampleR += (mv->aVR[mv->alpV]);
				if (damp > 5) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgVL;
					inputSampleL *= 0.5;
					mv->avgVL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgVR;
					inputSampleR *= 0.5;
					mv->avgVR = avgtemp;
				}
			// allpass filter V
			case 4:
				allpasstemp = mv->alpW - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayW) {
					allpasstemp = mv->delayW;
				}
				inputSampleL -= mv->aWL[allpasstemp] * 0.5;
				mv->aWL[mv->alpW] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aWR[allpasstemp] * 0.5;
				mv->aWR[mv->alpW] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpW--;
				if (mv->alpW < 0 || mv->alpW > mv->delayW) {
					mv->alpW = mv->delayW;
				}
				inputSampleL += (mv->aWL[mv->alpW]);
				inputSampleR += (mv->aWR[mv->alpW]);
				if (damp > 4) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgWL;
					inputSampleL *= 0.5;
					mv->avgWL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgWR;
					inputSampleR *= 0.5;
					mv->avgWR = avgtemp;
				}
			// allpass filter W
			case 3:
				allpasstemp = mv->alpX - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayX) {
					allpasstemp = mv->delayX;
				}
				inputSampleL -= mv->aXL[allpasstemp] * 0.5;
				mv->aXL[mv->alpX] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aXR[allpasstemp] * 0.5;
				mv->aXR[mv->alpX] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpX--;
				if (mv->alpX < 0 || mv->alpX > mv->delayX) {
					mv->alpX = mv->delayX;
				}
				inputSampleL += (mv->aXL[mv->alpX]);
				inputSampleR += (mv->aXR[mv->alpX]);
				if (damp > 3) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgXL;
					inputSampleL *= 0.5;
					mv->avgXL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgXR;
					inputSampleR *= 0.5;
					mv->avgXR = avgtemp;
				}
			// allpass filter X
			case 2:
				allpasstemp = mv->alpY - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayY) {
					allpasstemp = mv->delayY;
				}
				inputSampleL -= mv->aYL[allpasstemp] * 0.5;
				mv->aYL[mv->alpY] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aYR[allpasstemp] * 0.5;
				mv->aYR[mv->alpY] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpY--;
				if (mv->alpY < 0 || mv->alpY > mv->delayY) {
					mv->alpY = mv->delayY;
				}
				inputSampleL += (mv->aYL[mv->alpY]);
				inputSampleR += (mv->aYR[mv->alpY]);
				if (damp > 2) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgYL;
					inputSampleL *= 0.5;
					mv->avgYL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgYR;
					inputSampleR *= 0.5;
					mv->avgYR = avgtemp;
				}
			// allpass filter Y
			case 1:
				allpasstemp = mv->alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > mv->delayZ) {
					allpasstemp = mv->delayZ;
				}
				inputSampleL -= mv->aZL[allpasstemp] * 0.5;
				mv->aZL[mv->alpZ] = inputSampleL;
				inputSampleL *= 0.5;

				inputSampleR -= mv->aZR[allpasstemp] * 0.5;
				mv->aZR[mv->alpZ] = inputSampleR;
				inputSampleR *= 0.5;

				mv->alpZ--;
				if (mv->alpZ < 0 || mv->alpZ > mv->delayZ) {
					mv->alpZ = mv->delayZ;
				}
				inputSampleL += (mv->aZL[mv->alpZ]);
				inputSampleR += (mv->aZR[mv->alpZ]);
				if (damp > 1) {
					avgtemp = inputSampleL;
					inputSampleL += mv->avgZL;
					inputSampleL *= 0.5;
					mv->avgZL = avgtemp;

					avgtemp = inputSampleR;
					inputSampleR += mv->avgZR;
					inputSampleR *= 0.5;
					mv->avgZR = avgtemp;
				}
				// allpass filter Z
		}

		mv->feedbackL = inputSampleL * feedbacklevel;
		mv->feedbackR = inputSampleR * feedbacklevel;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}
		// we can pad with the gain to tame distortyness from the PurestConsole code

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// without this, you can get a NaN condition where it spits out DC offset at full blast!

		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, defaults to the last slider

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		mv->fpdL ^= mv->fpdL << 13;
		mv->fpdL ^= mv->fpdL >> 17;
		mv->fpdL ^= mv->fpdL << 5;
		inputSampleL += (((double) mv->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		mv->fpdR ^= mv->fpdR << 13;
		mv->fpdR ^= mv->fpdR >> 17;
		mv->fpdR ^= mv->fpdR << 5;
		inputSampleR += (((double) mv->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	MV_URI,
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
