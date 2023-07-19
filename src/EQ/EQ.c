#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define EQ_URI "https://hannesbraun.net/ns/lv2/airwindows/eq"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TREBLE = 4,
	MID = 5,
	BASS = 6,
	LOWPASS = 7,
	TREBFREQ = 8,
	BASSFREQ = 9,
	HIPASS = 10,
	OUTGAIN = 11
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* treble;
	const float* mid;
	const float* bass;
	const float* lowpass;
	const float* trebfreq;
	const float* bassfreq;
	const float* hipass;
	const float* outgain;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double lastSampleL;
	double last2SampleL;
	double lastSampleR;
	double last2SampleR;

	// begin EQ
	double iirHighSampleLA;
	double iirHighSampleLB;
	double iirHighSampleLC;
	double iirHighSampleLD;
	double iirHighSampleLE;
	double iirLowSampleLA;
	double iirLowSampleLB;
	double iirLowSampleLC;
	double iirLowSampleLD;
	double iirLowSampleLE;
	double iirHighSampleL;
	double iirLowSampleL;

	double iirHighSampleRA;
	double iirHighSampleRB;
	double iirHighSampleRC;
	double iirHighSampleRD;
	double iirHighSampleRE;
	double iirLowSampleRA;
	double iirLowSampleRB;
	double iirLowSampleRC;
	double iirLowSampleRD;
	double iirLowSampleRE;
	double iirHighSampleR;
	double iirLowSampleR;

	double tripletLA;
	double tripletLB;
	double tripletLC;
	double tripletFactorL;

	double tripletRA;
	double tripletRB;
	double tripletRC;
	double tripletFactorR;

	double lowpassSampleLAA;
	double lowpassSampleLAB;
	double lowpassSampleLBA;
	double lowpassSampleLBB;
	double lowpassSampleLCA;
	double lowpassSampleLCB;
	double lowpassSampleLDA;
	double lowpassSampleLDB;
	double lowpassSampleLE;
	double lowpassSampleLF;
	double lowpassSampleLG;

	double lowpassSampleRAA;
	double lowpassSampleRAB;
	double lowpassSampleRBA;
	double lowpassSampleRBB;
	double lowpassSampleRCA;
	double lowpassSampleRCB;
	double lowpassSampleRDA;
	double lowpassSampleRDB;
	double lowpassSampleRE;
	double lowpassSampleRF;
	double lowpassSampleRG;

	double highpassSampleLAA;
	double highpassSampleLAB;
	double highpassSampleLBA;
	double highpassSampleLBB;
	double highpassSampleLCA;
	double highpassSampleLCB;
	double highpassSampleLDA;
	double highpassSampleLDB;
	double highpassSampleLE;
	double highpassSampleLF;

	double highpassSampleRAA;
	double highpassSampleRAB;
	double highpassSampleRBA;
	double highpassSampleRBB;
	double highpassSampleRCA;
	double highpassSampleRCB;
	double highpassSampleRDA;
	double highpassSampleRDB;
	double highpassSampleRE;
	double highpassSampleRF;

	bool flip;
	int flipthree;
	// end EQ
} EQ;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	EQ* eq = (EQ*) calloc(1, sizeof(EQ));
	eq->sampleRate = rate;
	return (LV2_Handle) eq;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	EQ* eq = (EQ*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			eq->input[0] = (const float*) data;
			break;
		case INPUT_R:
			eq->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			eq->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			eq->output[1] = (float*) data;
			break;
		case TREBLE:
			eq->treble = (const float*) data;
			break;
		case MID:
			eq->mid = (const float*) data;
			break;
		case BASS:
			eq->bass = (const float*) data;
			break;
		case LOWPASS:
			eq->lowpass = (const float*) data;
			break;
		case TREBFREQ:
			eq->trebfreq = (const float*) data;
			break;
		case BASSFREQ:
			eq->bassfreq = (const float*) data;
			break;
		case HIPASS:
			eq->hipass = (const float*) data;
			break;
		case OUTGAIN:
			eq->outgain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	EQ* eq = (EQ*) instance;

	eq->lastSampleL = 0.0;
	eq->last2SampleL = 0.0;
	eq->lastSampleR = 0.0;
	eq->last2SampleR = 0.0;

	eq->iirHighSampleLA = 0.0;
	eq->iirHighSampleLB = 0.0;
	eq->iirHighSampleLC = 0.0;
	eq->iirHighSampleLD = 0.0;
	eq->iirHighSampleLE = 0.0;
	eq->iirLowSampleLA = 0.0;
	eq->iirLowSampleLB = 0.0;
	eq->iirLowSampleLC = 0.0;
	eq->iirLowSampleLD = 0.0;
	eq->iirLowSampleLE = 0.0;
	eq->iirHighSampleL = 0.0;
	eq->iirLowSampleL = 0.0;

	eq->iirHighSampleRA = 0.0;
	eq->iirHighSampleRB = 0.0;
	eq->iirHighSampleRC = 0.0;
	eq->iirHighSampleRD = 0.0;
	eq->iirHighSampleRE = 0.0;
	eq->iirLowSampleRA = 0.0;
	eq->iirLowSampleRB = 0.0;
	eq->iirLowSampleRC = 0.0;
	eq->iirLowSampleRD = 0.0;
	eq->iirLowSampleRE = 0.0;
	eq->iirHighSampleR = 0.0;
	eq->iirLowSampleR = 0.0;

	eq->tripletLA = 0.0;
	eq->tripletLB = 0.0;
	eq->tripletLC = 0.0;
	eq->tripletFactorL = 0.0;

	eq->tripletRA = 0.0;
	eq->tripletRB = 0.0;
	eq->tripletRC = 0.0;
	eq->tripletFactorR = 0.0;

	eq->lowpassSampleLAA = 0.0;
	eq->lowpassSampleLAB = 0.0;
	eq->lowpassSampleLBA = 0.0;
	eq->lowpassSampleLBB = 0.0;
	eq->lowpassSampleLCA = 0.0;
	eq->lowpassSampleLCB = 0.0;
	eq->lowpassSampleLDA = 0.0;
	eq->lowpassSampleLDB = 0.0;
	eq->lowpassSampleLE = 0.0;
	eq->lowpassSampleLF = 0.0;
	eq->lowpassSampleLG = 0.0;

	eq->lowpassSampleRAA = 0.0;
	eq->lowpassSampleRAB = 0.0;
	eq->lowpassSampleRBA = 0.0;
	eq->lowpassSampleRBB = 0.0;
	eq->lowpassSampleRCA = 0.0;
	eq->lowpassSampleRCB = 0.0;
	eq->lowpassSampleRDA = 0.0;
	eq->lowpassSampleRDB = 0.0;
	eq->lowpassSampleRE = 0.0;
	eq->lowpassSampleRF = 0.0;
	eq->lowpassSampleRG = 0.0;

	eq->highpassSampleLAA = 0.0;
	eq->highpassSampleLAB = 0.0;
	eq->highpassSampleLBA = 0.0;
	eq->highpassSampleLBB = 0.0;
	eq->highpassSampleLCA = 0.0;
	eq->highpassSampleLCB = 0.0;
	eq->highpassSampleLDA = 0.0;
	eq->highpassSampleLDB = 0.0;
	eq->highpassSampleLE = 0.0;
	eq->highpassSampleLF = 0.0;

	eq->highpassSampleRAA = 0.0;
	eq->highpassSampleRAB = 0.0;
	eq->highpassSampleRBA = 0.0;
	eq->highpassSampleRBB = 0.0;
	eq->highpassSampleRCA = 0.0;
	eq->highpassSampleRCB = 0.0;
	eq->highpassSampleRDA = 0.0;
	eq->highpassSampleRDB = 0.0;
	eq->highpassSampleRE = 0.0;
	eq->highpassSampleRF = 0.0;

	eq->flip = false;
	eq->flipthree = 0;

	eq->fpdL = 1.0;
	while (eq->fpdL < 16386) eq->fpdL = rand() * UINT32_MAX;
	eq->fpdR = 1.0;
	while (eq->fpdR < 16386) eq->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	EQ* eq = (EQ*) instance;

	const float* in1 = eq->input[0];
	const float* in2 = eq->input[1];
	float* out1 = eq->output[0];
	float* out2 = eq->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	double compscale = overallscale;
	overallscale = eq->sampleRate;
	compscale = compscale * overallscale;
	// compscale is the one that's 1 or something like 2.2 for 96K rates

	double inputSampleL;
	double inputSampleR;

	double highSampleL = 0.0;
	double midSampleL = 0.0;
	double bassSampleL = 0.0;

	double highSampleR = 0.0;
	double midSampleR = 0.0;
	double bassSampleR = 0.0;

	double densityA = *eq->treble * 0.5;
	double densityB = *eq->mid * 0.5;
	double densityC = *eq->bass * 0.5;
	bool engageEQ = true;
	if ((0.0 == densityA) && (0.0 == densityB) && (0.0 == densityC)) engageEQ = false;

	densityA = pow(10.0, densityA / 20.0) - 1.0;
	densityB = pow(10.0, densityB / 20.0) - 1.0;
	densityC = pow(10.0, densityC / 20.0) - 1.0;
	// convert to 0 to X multiplier with 1.0 being O db
	// minus one gives nearly -1 to ? (should top out at 1)
	// calibrate so that X db roughly equals X db with maximum topping out at 1 internally

	double tripletIntensity = -densityA;

	double iirAmountC = (*eq->lowpass * 0.0188) + 0.7;
	if (iirAmountC > 1.0) iirAmountC = 1.0;
	bool engageLowpass = false;
	if (*eq->lowpass < 15.99) engageLowpass = true;

	double iirAmountA = (*eq->trebfreq * 1000) / overallscale;
	double iirAmountB = (*eq->bassfreq * 10) / overallscale;
	double iirAmountD = (*eq->hipass * 1.0) / overallscale;
	bool engageHighpass = false;
	if (*eq->hipass > 30.01) engageHighpass = true;
	// bypass the highpass and lowpass if set to extremes
	double bridgerectifier;
	double outA = fabs(densityA);
	double outB = fabs(densityB);
	double outC = fabs(densityC);
	// end EQ
	double outputgain = pow(10.0, *eq->outgain / 20.0);

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = eq->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = eq->fpdR * 1.18e-17;

		eq->last2SampleL = eq->lastSampleL;
		eq->lastSampleL = inputSampleL;

		eq->last2SampleR = eq->lastSampleR;
		eq->lastSampleR = inputSampleR;

		eq->flip = !eq->flip;
		eq->flipthree++;
		if (eq->flipthree < 1 || eq->flipthree > 3) eq->flipthree = 1;
		// counters

		// begin highpass
		if (engageHighpass) {
			if (eq->flip) {
				eq->highpassSampleLAA = (eq->highpassSampleLAA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLAA;
				eq->highpassSampleLBA = (eq->highpassSampleLBA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLBA;
				eq->highpassSampleLCA = (eq->highpassSampleLCA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLCA;
				eq->highpassSampleLDA = (eq->highpassSampleLDA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLDA;
			} else {
				eq->highpassSampleLAB = (eq->highpassSampleLAB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLAB;
				eq->highpassSampleLBB = (eq->highpassSampleLBB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLBB;
				eq->highpassSampleLCB = (eq->highpassSampleLCB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLCB;
				eq->highpassSampleLDB = (eq->highpassSampleLDB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
				inputSampleL -= eq->highpassSampleLDB;
			}
			eq->highpassSampleLE = (eq->highpassSampleLE * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
			inputSampleL -= eq->highpassSampleLE;
			eq->highpassSampleLF = (eq->highpassSampleLF * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
			inputSampleL -= eq->highpassSampleLF;

			if (eq->flip) {
				eq->highpassSampleRAA = (eq->highpassSampleRAA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRAA;
				eq->highpassSampleRBA = (eq->highpassSampleRBA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRBA;
				eq->highpassSampleRCA = (eq->highpassSampleRCA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRCA;
				eq->highpassSampleRDA = (eq->highpassSampleRDA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRDA;
			} else {
				eq->highpassSampleRAB = (eq->highpassSampleRAB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRAB;
				eq->highpassSampleRBB = (eq->highpassSampleRBB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRBB;
				eq->highpassSampleRCB = (eq->highpassSampleRCB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRCB;
				eq->highpassSampleRDB = (eq->highpassSampleRDB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
				inputSampleR -= eq->highpassSampleRDB;
			}
			eq->highpassSampleRE = (eq->highpassSampleRE * (1 - iirAmountD)) + (inputSampleR * iirAmountD);
			inputSampleR -= eq->highpassSampleRE;
			eq->highpassSampleRF = (eq->highpassSampleRF * (1 - iirAmountD)) + (inputSampleR * iirAmountD);
			inputSampleR -= eq->highpassSampleRF;
		}
		// end highpass

		// begin EQ
		if (engageEQ) {
			switch (eq->flipthree) {
				case 1:
					eq->tripletFactorL = eq->last2SampleL - inputSampleL;
					eq->tripletLA += eq->tripletFactorL;
					eq->tripletLC -= eq->tripletFactorL;
					eq->tripletFactorL = eq->tripletLA * tripletIntensity;
					eq->iirHighSampleLC = (eq->iirHighSampleLC * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
					highSampleL = inputSampleL - eq->iirHighSampleLC;
					eq->iirLowSampleLC = (eq->iirLowSampleLC * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
					bassSampleL = eq->iirLowSampleLC;

					eq->tripletFactorR = eq->last2SampleR - inputSampleR;
					eq->tripletRA += eq->tripletFactorR;
					eq->tripletRC -= eq->tripletFactorR;
					eq->tripletFactorR = eq->tripletRA * tripletIntensity;
					eq->iirHighSampleRC = (eq->iirHighSampleRC * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
					highSampleR = inputSampleR - eq->iirHighSampleRC;
					eq->iirLowSampleRC = (eq->iirLowSampleRC * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
					bassSampleR = eq->iirLowSampleRC;
					break;
				case 2:
					eq->tripletFactorL = eq->last2SampleL - inputSampleL;
					eq->tripletLB += eq->tripletFactorL;
					eq->tripletLA -= eq->tripletFactorL;
					eq->tripletFactorL = eq->tripletLB * tripletIntensity;
					eq->iirHighSampleLD = (eq->iirHighSampleLD * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
					highSampleL = inputSampleL - eq->iirHighSampleLD;
					eq->iirLowSampleLD = (eq->iirLowSampleLD * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
					bassSampleL = eq->iirLowSampleLD;

					eq->tripletFactorR = eq->last2SampleR - inputSampleR;
					eq->tripletRB += eq->tripletFactorR;
					eq->tripletRA -= eq->tripletFactorR;
					eq->tripletFactorR = eq->tripletRB * tripletIntensity;
					eq->iirHighSampleRD = (eq->iirHighSampleRD * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
					highSampleR = inputSampleR - eq->iirHighSampleRD;
					eq->iirLowSampleRD = (eq->iirLowSampleRD * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
					bassSampleR = eq->iirLowSampleRD;
					break;
				case 3:
					eq->tripletFactorL = eq->last2SampleL - inputSampleL;
					eq->tripletLC += eq->tripletFactorL;
					eq->tripletLB -= eq->tripletFactorL;
					eq->tripletFactorL = eq->tripletLC * tripletIntensity;
					eq->iirHighSampleLE = (eq->iirHighSampleLE * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
					highSampleL = inputSampleL - eq->iirHighSampleLE;
					eq->iirLowSampleLE = (eq->iirLowSampleLE * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
					bassSampleL = eq->iirLowSampleLE;

					eq->tripletFactorR = eq->last2SampleR - inputSampleR;
					eq->tripletRC += eq->tripletFactorR;
					eq->tripletRB -= eq->tripletFactorR;
					eq->tripletFactorR = eq->tripletRC * tripletIntensity;
					eq->iirHighSampleRE = (eq->iirHighSampleRE * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
					highSampleR = inputSampleR - eq->iirHighSampleRE;
					eq->iirLowSampleRE = (eq->iirLowSampleRE * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
					bassSampleR = eq->iirLowSampleRE;
					break;
			}
			eq->tripletLA /= 2.0;
			eq->tripletLB /= 2.0;
			eq->tripletLC /= 2.0;
			highSampleL = highSampleL + eq->tripletFactorL;

			eq->tripletRA /= 2.0;
			eq->tripletRB /= 2.0;
			eq->tripletRC /= 2.0;
			highSampleR = highSampleR + eq->tripletFactorR;

			if (eq->flip) {
				eq->iirHighSampleLA = (eq->iirHighSampleLA * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
				highSampleL -= eq->iirHighSampleLA;
				eq->iirLowSampleLA = (eq->iirLowSampleLA * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
				bassSampleL = eq->iirLowSampleLA;

				eq->iirHighSampleRA = (eq->iirHighSampleRA * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
				highSampleR -= eq->iirHighSampleRA;
				eq->iirLowSampleRA = (eq->iirLowSampleRA * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
				bassSampleR = eq->iirLowSampleRA;
			} else {
				eq->iirHighSampleLB = (eq->iirHighSampleLB * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
				highSampleL -= eq->iirHighSampleLB;
				eq->iirLowSampleLB = (eq->iirLowSampleLB * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
				bassSampleL = eq->iirLowSampleLB;

				eq->iirHighSampleRB = (eq->iirHighSampleRB * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
				highSampleR -= eq->iirHighSampleRB;
				eq->iirLowSampleRB = (eq->iirLowSampleRB * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
				bassSampleR = eq->iirLowSampleRB;
			}

			eq->iirHighSampleL = (eq->iirHighSampleL * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
			highSampleL -= eq->iirHighSampleL;
			eq->iirLowSampleL = (eq->iirLowSampleL * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
			bassSampleL = eq->iirLowSampleL;

			eq->iirHighSampleR = (eq->iirHighSampleR * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
			highSampleR -= eq->iirHighSampleR;
			eq->iirLowSampleR = (eq->iirLowSampleR * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
			bassSampleR = eq->iirLowSampleR;

			midSampleL = (inputSampleL - bassSampleL) - highSampleL;
			midSampleR = (inputSampleR - bassSampleR) - highSampleR;

			// drive section
			highSampleL *= (densityA + 1.0);
			bridgerectifier = fabs(highSampleL) * 1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			// max value for sine function
			if (densityA > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1 - cos(bridgerectifier);
			// produce either boosted or starved version
			if (highSampleL > 0) highSampleL = (highSampleL * (1 - outA)) + (bridgerectifier * outA);
			else highSampleL = (highSampleL * (1 - outA)) - (bridgerectifier * outA);
			// blend according to densityA control

			highSampleR *= (densityA + 1.0);
			bridgerectifier = fabs(highSampleR) * 1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			// max value for sine function
			if (densityA > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1 - cos(bridgerectifier);
			// produce either boosted or starved version
			if (highSampleR > 0) highSampleR = (highSampleR * (1 - outA)) + (bridgerectifier * outA);
			else highSampleR = (highSampleR * (1 - outA)) - (bridgerectifier * outA);
			// blend according to densityA control

			midSampleL *= (densityB + 1.0);
			bridgerectifier = fabs(midSampleL) * 1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			// max value for sine function
			if (densityB > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1 - cos(bridgerectifier);
			// produce either boosted or starved version
			if (midSampleL > 0) midSampleL = (midSampleL * (1 - outB)) + (bridgerectifier * outB);
			else midSampleL = (midSampleL * (1 - outB)) - (bridgerectifier * outB);
			// blend according to densityB control

			midSampleR *= (densityB + 1.0);
			bridgerectifier = fabs(midSampleR) * 1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			// max value for sine function
			if (densityB > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1 - cos(bridgerectifier);
			// produce either boosted or starved version
			if (midSampleR > 0) midSampleR = (midSampleR * (1 - outB)) + (bridgerectifier * outB);
			else midSampleR = (midSampleR * (1 - outB)) - (bridgerectifier * outB);
			// blend according to densityB control

			bassSampleL *= (densityC + 1.0);
			bridgerectifier = fabs(bassSampleL) * 1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			// max value for sine function
			if (densityC > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1 - cos(bridgerectifier);
			// produce either boosted or starved version
			if (bassSampleL > 0) bassSampleL = (bassSampleL * (1 - outC)) + (bridgerectifier * outC);
			else bassSampleL = (bassSampleL * (1 - outC)) - (bridgerectifier * outC);
			// blend according to densityC control

			bassSampleR *= (densityC + 1.0);
			bridgerectifier = fabs(bassSampleR) * 1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			// max value for sine function
			if (densityC > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1 - cos(bridgerectifier);
			// produce either boosted or starved version
			if (bassSampleR > 0) bassSampleR = (bassSampleR * (1 - outC)) + (bridgerectifier * outC);
			else bassSampleR = (bassSampleR * (1 - outC)) - (bridgerectifier * outC);
			// blend according to densityC control

			inputSampleL = midSampleL;
			inputSampleL += highSampleL;
			inputSampleL += bassSampleL;

			inputSampleR = midSampleR;
			inputSampleR += highSampleR;
			inputSampleR += bassSampleR;
		}
		// end EQ

		// EQ lowpass is after all processing like the compressor that might produce hash
		if (engageLowpass) {
			if (eq->flip) {
				eq->lowpassSampleLAA = (eq->lowpassSampleLAA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLAA;
				eq->lowpassSampleLBA = (eq->lowpassSampleLBA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLBA;
				eq->lowpassSampleLCA = (eq->lowpassSampleLCA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLCA;
				eq->lowpassSampleLDA = (eq->lowpassSampleLDA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLDA;
				eq->lowpassSampleLE = (eq->lowpassSampleLE * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLE;

				eq->lowpassSampleRAA = (eq->lowpassSampleRAA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRAA;
				eq->lowpassSampleRBA = (eq->lowpassSampleRBA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRBA;
				eq->lowpassSampleRCA = (eq->lowpassSampleRCA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRCA;
				eq->lowpassSampleRDA = (eq->lowpassSampleRDA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRDA;
				eq->lowpassSampleRE = (eq->lowpassSampleRE * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRE;
			} else {
				eq->lowpassSampleLAB = (eq->lowpassSampleLAB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLAB;
				eq->lowpassSampleLBB = (eq->lowpassSampleLBB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLBB;
				eq->lowpassSampleLCB = (eq->lowpassSampleLCB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLCB;
				eq->lowpassSampleLDB = (eq->lowpassSampleLDB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLDB;
				eq->lowpassSampleLF = (eq->lowpassSampleLF * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
				inputSampleL = eq->lowpassSampleLF;

				eq->lowpassSampleRAB = (eq->lowpassSampleRAB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRAB;
				eq->lowpassSampleRBB = (eq->lowpassSampleRBB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRBB;
				eq->lowpassSampleRCB = (eq->lowpassSampleRCB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRCB;
				eq->lowpassSampleRDB = (eq->lowpassSampleRDB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRDB;
				eq->lowpassSampleRF = (eq->lowpassSampleRF * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
				inputSampleR = eq->lowpassSampleRF;
			}
			eq->lowpassSampleLG = (eq->lowpassSampleLG * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
			eq->lowpassSampleRG = (eq->lowpassSampleRG * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);

			inputSampleL = (eq->lowpassSampleLG * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleR = (eq->lowpassSampleRG * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
		}

		// built in output trim and dry/wet if desired
		if (outputgain != 1.0) {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		eq->fpdL ^= eq->fpdL << 13;
		eq->fpdL ^= eq->fpdL >> 17;
		eq->fpdL ^= eq->fpdL << 5;
		inputSampleL += (((double) eq->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		eq->fpdR ^= eq->fpdR << 13;
		eq->fpdR ^= eq->fpdR >> 17;
		eq->fpdR ^= eq->fpdR << 5;
		inputSampleR += (((double) eq->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	EQ_URI,
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
