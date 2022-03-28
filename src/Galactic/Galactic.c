#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define GALACTIC_URI "https://hannesbraun.net/ns/lv2/airwindows/galactic"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	REPLACE = 4,
	BRIGHTNESS = 5,
	DETUNE = 6,
	BIGNESS = 7,
	DRYWET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* replace;
	const float* brightness;
	const float* detune;
	const float* bigness;
	const float* drywet;

	double iirAL;
	double iirBL;

	double aIL[6480];
	double aJL[3660];
	double aKL[1720];
	double aLL[680];

	double aAL[9700];
	double aBL[6000];
	double aCL[2320];
	double aDL[940];

	double aEL[15220];
	double aFL[8460];
	double aGL[4540];
	double aHL[3200];

	double aML[3111];
	double aMR[3111];
	double vibML, vibMR, depthM, oldfpd;

	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;

	double lastRefL[7];
	double thunderL;

	double iirAR;
	double iirBR;

	double aIR[6480];
	double aJR[3660];
	double aKR[1720];
	double aLR[680];

	double aAR[9700];
	double aBR[6000];
	double aCR[2320];
	double aDR[940];

	double aER[15220];
	double aFR[8460];
	double aGR[4540];
	double aHR[3200];

	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;

	double lastRefR[7];
	double thunderR;

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
	int cycle; //all these ints are shared across channels, not duplicated

	double vibM;

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
} Galactic;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Galactic* galactic = (Galactic*) calloc(1, sizeof(Galactic));
	galactic->sampleRate = rate;
	return (LV2_Handle) galactic;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Galactic* galactic = (Galactic*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			galactic->input[0] = (const float*) data;
			break;
		case INPUT_R:
			galactic->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			galactic->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			galactic->output[1] = (float*) data;
			break;
		case REPLACE:
			galactic->replace = (float*) data;
			break;
		case BRIGHTNESS:
			galactic->brightness = (float*) data;
			break;
		case DETUNE:
			galactic->detune = (float*) data;
			break;
		case BIGNESS:
			galactic->bigness = (float*) data;
			break;
		case DRYWET:
			galactic->drywet = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Galactic* galactic = (Galactic*) instance;

	galactic->iirAL = 0.0;
	galactic->iirAR = 0.0;
	galactic->iirBL = 0.0;
	galactic->iirBR = 0.0;

	for (int count = 0; count < 6479; count++) {
		galactic->aIL[count] = 0.0;
		galactic->aIR[count] = 0.0;
	}
	for (int count = 0; count < 3659; count++) {
		galactic->aJL[count] = 0.0;
		galactic->aJR[count] = 0.0;
	}
	for (int count = 0; count < 1719; count++) {
		galactic->aKL[count] = 0.0;
		galactic->aKR[count] = 0.0;
	}
	for (int count = 0; count < 679; count++) {
		galactic->aLL[count] = 0.0;
		galactic->aLR[count] = 0.0;
	}

	for (int count = 0; count < 9699; count++) {
		galactic->aAL[count] = 0.0;
		galactic->aAR[count] = 0.0;
	}
	for (int count = 0; count < 5999; count++) {
		galactic->aBL[count] = 0.0;
		galactic->aBR[count] = 0.0;
	}
	for (int count = 0; count < 2319; count++) {
		galactic->aCL[count] = 0.0;
		galactic->aCR[count] = 0.0;
	}
	for (int count = 0; count < 939; count++) {
		galactic->aDL[count] = 0.0;
		galactic->aDR[count] = 0.0;
	}

	for (int count = 0; count < 15219; count++) {
		galactic->aEL[count] = 0.0;
		galactic->aER[count] = 0.0;
	}
	for (int count = 0; count < 8459; count++) {
		galactic->aFL[count] = 0.0;
		galactic->aFR[count] = 0.0;
	}
	for (int count = 0; count < 4539; count++) {
		galactic->aGL[count] = 0.0;
		galactic->aGR[count] = 0.0;
	}
	for (int count = 0; count < 3199; count++) {
		galactic->aHL[count] = 0.0;
		galactic->aHR[count] = 0.0;
	}

	for (int count = 0; count < 3110; count++) {
		galactic->aML[count] = 0.0;
		galactic->aMR[count] = 0.0;
	}

	galactic->feedbackAL = 0.0;
	galactic->feedbackAR = 0.0;
	galactic->feedbackBL = 0.0;
	galactic->feedbackBR = 0.0;
	galactic->feedbackCL = 0.0;
	galactic->feedbackCR = 0.0;
	galactic->feedbackDL = 0.0;
	galactic->feedbackDR = 0.0;

	for (int count = 0; count < 6; count++) {
		galactic->lastRefL[count] = 0.0;
		galactic->lastRefR[count] = 0.0;
	}

	galactic->thunderL = 0;
	galactic->thunderR = 0;

	galactic->countI = 1;
	galactic->countJ = 1;
	galactic->countK = 1;
	galactic->countL = 1;

	galactic->countA = 1;
	galactic->countB = 1;
	galactic->countC = 1;
	galactic->countD = 1;

	galactic->countE = 1;
	galactic->countF = 1;
	galactic->countG = 1;
	galactic->countH = 1;
	galactic->countM = 1;
	//the predelay
	galactic->cycle = 0;

	galactic->vibM = 3.0;

	galactic->oldfpd = 429496.7295;

	galactic->fpdL = 1;
	while (galactic->fpdL < 16386) galactic->fpdL = rand() * UINT32_MAX;
	galactic->fpdR = 1;
	while (galactic->fpdR < 16386) galactic->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Galactic* galactic = (Galactic*) instance;

	const float* in1 = galactic->input[0];
	const float* in2 = galactic->input[1];
	float* out1 = galactic->output[0];
	float* out2 = galactic->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= galactic->sampleRate;

	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (galactic->cycle > cycleEnd - 1) {
		//sanity check
		galactic->cycle = cycleEnd - 1;
	}

	double regen = 0.0625 + ((1.0 - *galactic->replace) * 0.0625);
	double attenuate = (1.0 - (regen / 0.125)) * 1.333;
	double lowpass = pow(1.00001 - (1.0 - *galactic->brightness), 2.0) / sqrt(overallscale);
	double drift = pow(*galactic->detune, 3) * 0.001;
	double size = (*galactic->bigness * 1.77) + 0.1;
	double wet = 1.0 - (pow(1.0 - *galactic->drywet, 3));

	galactic->delayI = 3407.0 * size;
	galactic->delayJ = 1823.0 * size;
	galactic->delayK = 859.0 * size;
	galactic->delayL = 331.0 * size;
	galactic->delayA = 4801.0 * size;
	galactic->delayB = 2909.0 * size;
	galactic->delayC = 1153.0 * size;
	galactic->delayD = 461.0 * size;
	galactic->delayE = 7607.0 * size;
	galactic->delayF = 4217.0 * size;
	galactic->delayG = 2269.0 * size;
	galactic->delayH = 1597.0 * size;
	galactic->delayM = 256;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = galactic->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = galactic->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		galactic->vibM += (galactic->oldfpd * drift);
		if (galactic->vibM > (3.141592653589793238 * 2.0)) {
			galactic->vibM = 0.0;
			galactic->oldfpd = 0.4294967295 + (galactic->fpdL * 0.0000000000618);
		}

		galactic->aML[galactic->countM] = inputSampleL * attenuate;
		galactic->aMR[galactic->countM] = inputSampleR * attenuate;
		galactic->countM++;
		if (galactic->countM < 0 || galactic->countM > galactic->delayM) galactic->countM = 0;

		double offsetML = (sin(galactic->vibM) + 1.0) * 127;
		double offsetMR = (sin(galactic->vibM + (3.141592653589793238 / 2.0)) + 1.0) * 127;
		int workingML = galactic->countM + offsetML;
		int workingMR = galactic->countM + offsetMR;
		double interpolML = (galactic->aML[workingML - ((workingML > galactic->delayM) ? galactic->delayM + 1 : 0)] * (1 - (offsetML - floor(offsetML))));
		interpolML += (galactic->aML[workingML + 1 - ((workingML + 1 > galactic->delayM) ? galactic->delayM + 1 : 0)] * ((offsetML - floor(offsetML))));
		double interpolMR = (galactic->aMR[workingMR - ((workingMR > galactic->delayM) ? galactic->delayM + 1 : 0)] * (1 - (offsetMR - floor(offsetMR))));
		interpolMR += (galactic->aMR[workingMR + 1 - ((workingMR + 1 > galactic->delayM) ? galactic->delayM + 1 : 0)] * ((offsetMR - floor(offsetMR))));
		inputSampleL = interpolML;
		inputSampleR = interpolMR;
		//predelay that applies vibrato
		//want vibrato speed AND depth like in MatrixVerb

		galactic->iirAL = (galactic->iirAL * (1.0 - lowpass)) + (inputSampleL * lowpass);
		inputSampleL = galactic->iirAL;
		galactic->iirAR = (galactic->iirAR * (1.0 - lowpass)) + (inputSampleR * lowpass);
		inputSampleR = galactic->iirAR;
		//initial filter

		galactic->cycle++;
		if (galactic->cycle == cycleEnd) { //hit the end point and we do a reverb sample
			galactic->aIL[galactic->countI] = inputSampleL + (galactic->feedbackAR * regen);
			galactic->aJL[galactic->countJ] = inputSampleL + (galactic->feedbackBR * regen);
			galactic->aKL[galactic->countK] = inputSampleL + (galactic->feedbackCR * regen);
			galactic->aLL[galactic->countL] = inputSampleL + (galactic->feedbackDR * regen);
			galactic->aIR[galactic->countI] = inputSampleR + (galactic->feedbackAL * regen);
			galactic->aJR[galactic->countJ] = inputSampleR + (galactic->feedbackBL * regen);
			galactic->aKR[galactic->countK] = inputSampleR + (galactic->feedbackCL * regen);
			galactic->aLR[galactic->countL] = inputSampleR + (galactic->feedbackDL * regen);

			galactic->countI++;
			if (galactic->countI < 0 || galactic->countI > galactic->delayI) galactic->countI = 0;
			galactic->countJ++;
			if (galactic->countJ < 0 || galactic->countJ > galactic->delayJ) galactic->countJ = 0;
			galactic->countK++;
			if (galactic->countK < 0 || galactic->countK > galactic->delayK) galactic->countK = 0;
			galactic->countL++;
			if (galactic->countL < 0 || galactic->countL > galactic->delayL) galactic->countL = 0;

			double outIL = galactic->aIL[galactic->countI - ((galactic->countI > galactic->delayI) ? galactic->delayI + 1 : 0)];
			double outJL = galactic->aJL[galactic->countJ - ((galactic->countJ > galactic->delayJ) ? galactic->delayJ + 1 : 0)];
			double outKL = galactic->aKL[galactic->countK - ((galactic->countK > galactic->delayK) ? galactic->delayK + 1 : 0)];
			double outLL = galactic->aLL[galactic->countL - ((galactic->countL > galactic->delayL) ? galactic->delayL + 1 : 0)];
			double outIR = galactic->aIR[galactic->countI - ((galactic->countI > galactic->delayI) ? galactic->delayI + 1 : 0)];
			double outJR = galactic->aJR[galactic->countJ - ((galactic->countJ > galactic->delayJ) ? galactic->delayJ + 1 : 0)];
			double outKR = galactic->aKR[galactic->countK - ((galactic->countK > galactic->delayK) ? galactic->delayK + 1 : 0)];
			double outLR = galactic->aLR[galactic->countL - ((galactic->countL > galactic->delayL) ? galactic->delayL + 1 : 0)];
			//first block: now we have four outputs

			galactic->aAL[galactic->countA] = (outIL - (outJL + outKL + outLL));
			galactic->aBL[galactic->countB] = (outJL - (outIL + outKL + outLL));
			galactic->aCL[galactic->countC] = (outKL - (outIL + outJL + outLL));
			galactic->aDL[galactic->countD] = (outLL - (outIL + outJL + outKL));
			galactic->aAR[galactic->countA] = (outIR - (outJR + outKR + outLR));
			galactic->aBR[galactic->countB] = (outJR - (outIR + outKR + outLR));
			galactic->aCR[galactic->countC] = (outKR - (outIR + outJR + outLR));
			galactic->aDR[galactic->countD] = (outLR - (outIR + outJR + outKR));

			galactic->countA++;
			if (galactic->countA < 0 || galactic->countA > galactic->delayA) galactic->countA = 0;
			galactic->countB++;
			if (galactic->countB < 0 || galactic->countB > galactic->delayB) galactic->countB = 0;
			galactic->countC++;
			if (galactic->countC < 0 || galactic->countC > galactic->delayC) galactic->countC = 0;
			galactic->countD++;
			if (galactic->countD < 0 || galactic->countD > galactic->delayD) galactic->countD = 0;

			double outAL = galactic->aAL[galactic->countA - ((galactic->countA > galactic->delayA) ? galactic->delayA + 1 : 0)];
			double outBL = galactic->aBL[galactic->countB - ((galactic->countB > galactic->delayB) ? galactic->delayB + 1 : 0)];
			double outCL = galactic->aCL[galactic->countC - ((galactic->countC > galactic->delayC) ? galactic->delayC + 1 : 0)];
			double outDL = galactic->aDL[galactic->countD - ((galactic->countD > galactic->delayD) ? galactic->delayD + 1 : 0)];
			double outAR = galactic->aAR[galactic->countA - ((galactic->countA > galactic->delayA) ? galactic->delayA + 1 : 0)];
			double outBR = galactic->aBR[galactic->countB - ((galactic->countB > galactic->delayB) ? galactic->delayB + 1 : 0)];
			double outCR = galactic->aCR[galactic->countC - ((galactic->countC > galactic->delayC) ? galactic->delayC + 1 : 0)];
			double outDR = galactic->aDR[galactic->countD - ((galactic->countD > galactic->delayD) ? galactic->delayD + 1 : 0)];
			//second block: four more outputs

			galactic->aEL[galactic->countE] = (outAL - (outBL + outCL + outDL));
			galactic->aFL[galactic->countF] = (outBL - (outAL + outCL + outDL));
			galactic->aGL[galactic->countG] = (outCL - (outAL + outBL + outDL));
			galactic->aHL[galactic->countH] = (outDL - (outAL + outBL + outCL));
			galactic->aER[galactic->countE] = (outAR - (outBR + outCR + outDR));
			galactic->aFR[galactic->countF] = (outBR - (outAR + outCR + outDR));
			galactic->aGR[galactic->countG] = (outCR - (outAR + outBR + outDR));
			galactic->aHR[galactic->countH] = (outDR - (outAR + outBR + outCR));

			galactic->countE++;
			if (galactic->countE < 0 || galactic->countE > galactic->delayE) galactic->countE = 0;
			galactic->countF++;
			if (galactic->countF < 0 || galactic->countF > galactic->delayF) galactic->countF = 0;
			galactic->countG++;
			if (galactic->countG < 0 || galactic->countG > galactic->delayG) galactic->countG = 0;
			galactic->countH++;
			if (galactic->countH < 0 || galactic->countH > galactic->delayH) galactic->countH = 0;

			double outEL = galactic->aEL[galactic->countE - ((galactic->countE > galactic->delayE) ? galactic->delayE + 1 : 0)];
			double outFL = galactic->aFL[galactic->countF - ((galactic->countF > galactic->delayF) ? galactic->delayF + 1 : 0)];
			double outGL = galactic->aGL[galactic->countG - ((galactic->countG > galactic->delayG) ? galactic->delayG + 1 : 0)];
			double outHL = galactic->aHL[galactic->countH - ((galactic->countH > galactic->delayH) ? galactic->delayH + 1 : 0)];
			double outER = galactic->aER[galactic->countE - ((galactic->countE > galactic->delayE) ? galactic->delayE + 1 : 0)];
			double outFR = galactic->aFR[galactic->countF - ((galactic->countF > galactic->delayF) ? galactic->delayF + 1 : 0)];
			double outGR = galactic->aGR[galactic->countG - ((galactic->countG > galactic->delayG) ? galactic->delayG + 1 : 0)];
			double outHR = galactic->aHR[galactic->countH - ((galactic->countH > galactic->delayH) ? galactic->delayH + 1 : 0)];
			//third block: final outputs

			galactic->feedbackAL = (outEL - (outFL + outGL + outHL));
			galactic->feedbackBL = (outFL - (outEL + outGL + outHL));
			galactic->feedbackCL = (outGL - (outEL + outFL + outHL));
			galactic->feedbackDL = (outHL - (outEL + outFL + outGL));
			galactic->feedbackAR = (outER - (outFR + outGR + outHR));
			galactic->feedbackBR = (outFR - (outER + outGR + outHR));
			galactic->feedbackCR = (outGR - (outER + outFR + outHR));
			galactic->feedbackDR = (outHR - (outER + outFR + outGR));
			//which we need to feed back into the input again, a bit

			inputSampleL = (outEL + outFL + outGL + outHL) / 8.0;
			inputSampleR = (outER + outFR + outGR + outHR) / 8.0;
			//and take the final combined sum of outputs
			if (cycleEnd == 4) {
				galactic->lastRefL[0] = galactic->lastRefL[4]; //start from previous last
				galactic->lastRefL[2] = (galactic->lastRefL[0] + inputSampleL) / 2; //half
				galactic->lastRefL[1] = (galactic->lastRefL[0] + galactic->lastRefL[2]) / 2; //one quarter
				galactic->lastRefL[3] = (galactic->lastRefL[2] + inputSampleL) / 2; //three quarters
				galactic->lastRefL[4] = inputSampleL; //full
				galactic->lastRefR[0] = galactic->lastRefR[4]; //start from previous last
				galactic->lastRefR[2] = (galactic->lastRefR[0] + inputSampleR) / 2; //half
				galactic->lastRefR[1] = (galactic->lastRefR[0] + galactic->lastRefR[2]) / 2; //one quarter
				galactic->lastRefR[3] = (galactic->lastRefR[2] + inputSampleR) / 2; //three quarters
				galactic->lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				galactic->lastRefL[0] = galactic->lastRefL[3]; //start from previous last
				galactic->lastRefL[2] = (galactic->lastRefL[0] + galactic->lastRefL[0] + inputSampleL) / 3; //third
				galactic->lastRefL[1] = (galactic->lastRefL[0] + inputSampleL + inputSampleL) / 3; //two thirds
				galactic->lastRefL[3] = inputSampleL; //full
				galactic->lastRefR[0] = galactic->lastRefR[3]; //start from previous last
				galactic->lastRefR[2] = (galactic->lastRefR[0] + galactic->lastRefR[0] + inputSampleR) / 3; //third
				galactic->lastRefR[1] = (galactic->lastRefR[0] + inputSampleR + inputSampleR) / 3; //two thirds
				galactic->lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				galactic->lastRefL[0] = galactic->lastRefL[2]; //start from previous last
				galactic->lastRefL[1] = (galactic->lastRefL[0] + inputSampleL) / 2; //half
				galactic->lastRefL[2] = inputSampleL; //full
				galactic->lastRefR[0] = galactic->lastRefR[2]; //start from previous last
				galactic->lastRefR[1] = (galactic->lastRefR[0] + inputSampleR) / 2; //half
				galactic->lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				galactic->lastRefL[0] = inputSampleL;
				galactic->lastRefR[0] = inputSampleR;
			}
			galactic->cycle = 0; //reset
			inputSampleL = galactic->lastRefL[galactic->cycle];
			inputSampleR = galactic->lastRefR[galactic->cycle];
		} else {
			inputSampleL = galactic->lastRefL[galactic->cycle];
			inputSampleR = galactic->lastRefR[galactic->cycle];
			//we are going through our references now
		}

		galactic->iirBL = (galactic->iirBL * (1.0 - lowpass)) + (inputSampleL * lowpass);
		inputSampleL = galactic->iirBL;
		galactic->iirBR = (galactic->iirBR * (1.0 - lowpass)) + (inputSampleR * lowpass);
		inputSampleR = galactic->iirBR;
		//end filter

		if (wet < 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		galactic->fpdL ^= galactic->fpdL << 13;
		galactic->fpdL ^= galactic->fpdL >> 17;
		galactic->fpdL ^= galactic->fpdL << 5;
		inputSampleL += ((((double) galactic->fpdL) - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		galactic->fpdR ^= galactic->fpdR << 13;
		galactic->fpdR ^= galactic->fpdR >> 17;
		galactic->fpdR ^= galactic->fpdR << 5;
		inputSampleR += ((((double) galactic->fpdL) - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	GALACTIC_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
