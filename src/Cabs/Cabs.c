#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CABS_URI "https://hannesbraun.net/ns/lv2/airwindows/cabs"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CAB_TYPE = 4,
	INTENSE = 5,
	RM_LOUD = 6,
	CAB_SIZE = 7,
	OFF_AXIS = 8,
	OUTPUT = 9
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* cabType;
	const float* intense;
	const float* rmLoud;
	const float* cabSize;
	const float* offAxis;
	const float* outputGain;

	double ataLast3SampleL; // begin L
	double ataLast2SampleL;
	double ataLast1SampleL;
	double ataHalfwaySampleL;
	double ataHalfDrySampleL;
	double ataHalfDiffSampleL;
	double ataAL;
	double ataBL;
	double ataCL;
	double ataDrySampleL;
	double ataDiffSampleL;
	double ataPrevDiffSampleL;
	double bL[90];
	double lastSampleL;
	double lastHalfSampleL;
	double lastPostSampleL;
	double lastPostHalfSampleL;
	double postPostSampleL;
	double dL[21];
	double controlL;
	double iirHeadBumpAL;
	double iirHeadBumpBL;
	double iirHalfHeadBumpAL;
	double iirHalfHeadBumpBL;
	double lastRefL[7]; // end L

	double ataLast3SampleR; // begin R
	double ataLast2SampleR;
	double ataLast1SampleR;
	double ataHalfwaySampleR;
	double ataHalfDrySampleR;
	double ataHalfDiffSampleR;
	double ataAR;
	double ataBR;
	double ataCR;
	double ataDrySampleR;
	double ataDiffSampleR;
	double ataPrevDiffSampleR;
	double bR[90];
	double lastSampleR;
	double lastHalfSampleR;
	double lastPostSampleR;
	double lastPostHalfSampleR;
	double postPostSampleR;
	double dR[21];
	double controlR;
	double iirHeadBumpAR;
	double iirHeadBumpBR;
	double iirHalfHeadBumpAR;
	double iirHalfHeadBumpBR;
	double lastRefR[7]; // end R

	bool flip;
	bool ataFlip; // end defining of antialiasing variables
	int gcount;
	int cycle;

	uint32_t fpdL;
	uint32_t fpdR;
} Cabs;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Cabs* cabs = (Cabs*) calloc(1, sizeof(Cabs));
	cabs->sampleRate = rate;
	return (LV2_Handle) cabs;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Cabs* cabs = (Cabs*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			cabs->input[0] = (const float*) data;
			break;
		case INPUT_R:
			cabs->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			cabs->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			cabs->output[1] = (float*) data;
			break;
		case CAB_TYPE:
			cabs->cabType = (const float*) data;
			break;
		case INTENSE:
			cabs->intense = (const float*) data;
			break;
		case RM_LOUD:
			cabs->rmLoud = (const float*) data;
			break;
		case CAB_SIZE:
			cabs->cabSize = (const float*) data;
			break;
		case OFF_AXIS:
			cabs->offAxis = (const float*) data;
			break;
		case OUTPUT:
			cabs->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Cabs* cabs = (Cabs*) instance;

	cabs->ataLast3SampleL = cabs->ataLast2SampleL = cabs->ataLast1SampleL = 0.0; // begin L
	cabs->ataHalfwaySampleL = cabs->ataHalfDrySampleL = cabs->ataHalfDiffSampleL = 0.0;
	cabs->ataAL = cabs->ataBL = cabs->ataCL = cabs->ataDrySampleL = cabs->ataDiffSampleL = cabs->ataPrevDiffSampleL = 0.0;

	for (int count = 0; count < 90; count++) {
		cabs->bL[count] = 0;
	}
	cabs->lastSampleL = 0.0;
	cabs->lastHalfSampleL = 0.0;
	cabs->lastPostSampleL = 0.0;
	cabs->lastPostHalfSampleL = 0.0;
	cabs->postPostSampleL = 0.0;
	for (int count = 0; count < 20; count++) {
		cabs->dL[count] = 0;
	}
	cabs->controlL = 0;
	cabs->iirHeadBumpAL = 0.0;
	cabs->iirHeadBumpBL = 0.0;
	cabs->iirHalfHeadBumpAL = 0.0;
	cabs->iirHalfHeadBumpBL = 0.0;
	for (int count = 0; count < 6; count++) cabs->lastRefL[count] = 0.0; // end L

	cabs->ataLast3SampleR = cabs->ataLast2SampleR = cabs->ataLast1SampleR = 0.0; // begin R
	cabs->ataHalfwaySampleR = cabs->ataHalfDrySampleR = cabs->ataHalfDiffSampleR = 0.0;
	cabs->ataAR = cabs->ataBR = cabs->ataCR = cabs->ataDrySampleR = cabs->ataDiffSampleR = cabs->ataPrevDiffSampleR = 0.0;

	for (int count = 0; count < 90; count++) {
		cabs->bR[count] = 0;
	}
	cabs->lastSampleR = 0.0;
	cabs->lastHalfSampleR = 0.0;
	cabs->lastPostSampleR = 0.0;
	cabs->lastPostHalfSampleR = 0.0;
	cabs->postPostSampleR = 0.0;
	for (int count = 0; count < 20; count++) {
		cabs->dR[count] = 0;
	}
	cabs->controlR = 0;
	cabs->iirHeadBumpAR = 0.0;
	cabs->iirHeadBumpBR = 0.0;
	cabs->iirHalfHeadBumpAR = 0.0;
	cabs->iirHalfHeadBumpBR = 0.0;
	for (int count = 0; count < 6; count++) cabs->lastRefR[count] = 0.0; // end R

	cabs->flip = false;
	cabs->ataFlip = false;
	cabs->gcount = 0;
	cabs->cycle = 0;
	cabs->fpdL = 1.0;
	while (cabs->fpdL < 16386) cabs->fpdL = rand() * UINT32_MAX;
	cabs->fpdR = 1.0;
	while (cabs->fpdR < 16386) cabs->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Cabs* cabs = (Cabs*) instance;

	const float* in1 = cabs->input[0];
	const float* in2 = cabs->input[1];
	float* out1 = cabs->output[0];
	float* out2 = cabs->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= cabs->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cabs->cycle > cycleEnd - 1) cabs->cycle = cycleEnd - 1; // sanity check

	int speaker = (int) (*cabs->cabType) + 1;
	if (speaker > 6) speaker = 6;
	else if (speaker < 1) speaker = 1;
	double colorIntensity = pow(*cabs->intense, 4);
	double correctboost = 1.0 + (colorIntensity * 4);
	double correctdrygain = 1.0 - colorIntensity;
	double threshold = pow((1.0 - *cabs->rmLoud), 5) + 0.021; // room loud is slew
	double rarefaction = cbrt(threshold);
	double postThreshold = sqrt(rarefaction);
	double postRarefaction = cbrt(postThreshold);
	double postTrim = sqrt(postRarefaction);
	double HeadBumpFreq = 0.0298 + ((1.0 - *cabs->cabSize) / 8.0);
	double LowsPad = 0.12 + (HeadBumpFreq * 12.0);
	double dcblock = pow(HeadBumpFreq, 2) / 8.0;
	double heavy = pow(*cabs->offAxis, 3); // wet of head bump
	double output = pow(*cabs->outputGain, 2);
	double dynamicconv = 5.0;
	dynamicconv *= pow(*cabs->intense, 2);
	dynamicconv *= pow(*cabs->rmLoud, 2);
	// set constants for sag speed
	int offsetA = 4 + ((int) (*cabs->cabSize * 5.0));

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = cabs->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = cabs->fpdR * 1.18e-17;

		cabs->cycle++;
		if (cabs->cycle == cycleEnd) { // hit the end point and we do a chorus sample
			// everything in here is undersampled, including the dry/wet

			double ataDrySampleL = inputSampleL;
			double ataHalfwaySampleL = (inputSampleL + cabs->ataLast1SampleL + ((-cabs->ataLast2SampleL + cabs->ataLast3SampleL) * 0.05)) / 2.0;
			double ataHalfDrySampleL = ataHalfwaySampleL;
			cabs->ataLast3SampleL = cabs->ataLast2SampleL;
			cabs->ataLast2SampleL = cabs->ataLast1SampleL;
			cabs->ataLast1SampleL = inputSampleL; //-----------------
			double ataDrySampleR = inputSampleR;
			double ataHalfwaySampleR = (inputSampleR + cabs->ataLast1SampleR + ((-cabs->ataLast2SampleR + cabs->ataLast3SampleR) * 0.05)) / 2.0;
			double ataHalfDrySampleR = ataHalfwaySampleR;
			cabs->ataLast3SampleR = cabs->ataLast2SampleR;
			cabs->ataLast2SampleR = cabs->ataLast1SampleR;
			cabs->ataLast1SampleR = inputSampleR;
			// setting up oversampled special antialiasing
			// pre-center code on inputSample and halfwaySample in parallel
			// begin raw sample- inputSample and ataDrySample handled separately here
			double clamp = inputSampleL - cabs->lastSampleL;
			if (clamp > threshold) inputSampleL = cabs->lastSampleL + threshold;
			if (-clamp > rarefaction) inputSampleL = cabs->lastSampleL - rarefaction;
			cabs->lastSampleL = inputSampleL; //----------------------------------------------
			clamp = inputSampleR - cabs->lastSampleR;
			if (clamp > threshold) inputSampleR = cabs->lastSampleR + threshold;
			if (-clamp > rarefaction) inputSampleR = cabs->lastSampleR - rarefaction;
			cabs->lastSampleR = inputSampleR;
			// end raw sample

			// begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
			clamp = ataHalfwaySampleL - cabs->lastHalfSampleL;
			if (clamp > threshold) ataHalfwaySampleL = cabs->lastHalfSampleL + threshold;
			if (-clamp > rarefaction) ataHalfwaySampleL = cabs->lastHalfSampleL - rarefaction;
			cabs->lastHalfSampleL = ataHalfwaySampleL; //-------------------------------------------
			clamp = ataHalfwaySampleR - cabs->lastHalfSampleR;
			if (clamp > threshold) ataHalfwaySampleR = cabs->lastHalfSampleR + threshold;
			if (-clamp > rarefaction) ataHalfwaySampleR = cabs->lastHalfSampleR - rarefaction;
			cabs->lastHalfSampleR = ataHalfwaySampleR;
			// end interpolated sample

			// begin center code handling conv stuff tied to 44.1K, or stuff in time domain like delays
			ataHalfwaySampleL -= inputSampleL;
			ataHalfwaySampleR -= inputSampleR;
			// retain only difference with raw signal

			if (cabs->gcount < 0 || cabs->gcount > 10) cabs->gcount = 10;

			cabs->dL[cabs->gcount + 10] = cabs->dL[cabs->gcount] = fabs(inputSampleL);
			cabs->controlL += (cabs->dL[cabs->gcount] / offsetA);
			cabs->controlL -= (cabs->dL[cabs->gcount + offsetA] / offsetA);
			cabs->controlL -= 0.0001;
			if (cabs->controlL < 0) cabs->controlL = 0;
			if (cabs->controlL > 13) cabs->controlL = 13; //-------------------------
			cabs->dR[cabs->gcount + 10] = cabs->dR[cabs->gcount] = fabs(inputSampleR);
			cabs->controlR += (cabs->dR[cabs->gcount] / offsetA);
			cabs->controlR -= (cabs->dR[cabs->gcount + offsetA] / offsetA);
			cabs->controlR -= 0.0001;
			if (cabs->controlR < 0) cabs->controlR = 0;
			if (cabs->controlR > 13) cabs->controlR = 13;

			cabs->gcount--;

			double applyconvL = (cabs->controlL / offsetA) * dynamicconv;
			double applyconvR = (cabs->controlR / offsetA) * dynamicconv;
			// now we have a 'sag' style average to apply to the conv
			cabs->bL[82] = cabs->bL[81];
			cabs->bL[81] = cabs->bL[80];
			cabs->bL[80] = cabs->bL[79];
			cabs->bL[79] = cabs->bL[78];
			cabs->bL[78] = cabs->bL[77];
			cabs->bL[77] = cabs->bL[76];
			cabs->bL[76] = cabs->bL[75];
			cabs->bL[75] = cabs->bL[74];
			cabs->bL[74] = cabs->bL[73];
			cabs->bL[73] = cabs->bL[72];
			cabs->bL[72] = cabs->bL[71];
			cabs->bL[71] = cabs->bL[70];
			cabs->bL[70] = cabs->bL[69];
			cabs->bL[69] = cabs->bL[68];
			cabs->bL[68] = cabs->bL[67];
			cabs->bL[67] = cabs->bL[66];
			cabs->bL[66] = cabs->bL[65];
			cabs->bL[65] = cabs->bL[64];
			cabs->bL[64] = cabs->bL[63];
			cabs->bL[63] = cabs->bL[62];
			cabs->bL[62] = cabs->bL[61];
			cabs->bL[61] = cabs->bL[60];
			cabs->bL[60] = cabs->bL[59];
			cabs->bL[59] = cabs->bL[58];
			cabs->bL[58] = cabs->bL[57];
			cabs->bL[57] = cabs->bL[56];
			cabs->bL[56] = cabs->bL[55];
			cabs->bL[55] = cabs->bL[54];
			cabs->bL[54] = cabs->bL[53];
			cabs->bL[53] = cabs->bL[52];
			cabs->bL[52] = cabs->bL[51];
			cabs->bL[51] = cabs->bL[50];
			cabs->bL[50] = cabs->bL[49];
			cabs->bL[49] = cabs->bL[48];
			cabs->bL[48] = cabs->bL[47];
			cabs->bL[47] = cabs->bL[46];
			cabs->bL[46] = cabs->bL[45];
			cabs->bL[45] = cabs->bL[44];
			cabs->bL[44] = cabs->bL[43];
			cabs->bL[43] = cabs->bL[42];
			cabs->bL[42] = cabs->bL[41];
			cabs->bL[41] = cabs->bL[40];
			cabs->bL[40] = cabs->bL[39];
			cabs->bL[39] = cabs->bL[38];
			cabs->bL[38] = cabs->bL[37];
			cabs->bL[37] = cabs->bL[36];
			cabs->bL[36] = cabs->bL[35];
			cabs->bL[35] = cabs->bL[34];
			cabs->bL[34] = cabs->bL[33];
			cabs->bL[33] = cabs->bL[32];
			cabs->bL[32] = cabs->bL[31];
			cabs->bL[31] = cabs->bL[30];
			cabs->bL[30] = cabs->bL[29];
			cabs->bL[29] = cabs->bL[28];
			cabs->bL[28] = cabs->bL[27];
			cabs->bL[27] = cabs->bL[26];
			cabs->bL[26] = cabs->bL[25];
			cabs->bL[25] = cabs->bL[24];
			cabs->bL[24] = cabs->bL[23];
			cabs->bL[23] = cabs->bL[22];
			cabs->bL[22] = cabs->bL[21];
			cabs->bL[21] = cabs->bL[20];
			cabs->bL[20] = cabs->bL[19];
			cabs->bL[19] = cabs->bL[18];
			cabs->bL[18] = cabs->bL[17];
			cabs->bL[17] = cabs->bL[16];
			cabs->bL[16] = cabs->bL[15];
			cabs->bL[15] = cabs->bL[14];
			cabs->bL[14] = cabs->bL[13];
			cabs->bL[13] = cabs->bL[12];
			cabs->bL[12] = cabs->bL[11];
			cabs->bL[11] = cabs->bL[10];
			cabs->bL[10] = cabs->bL[9];
			cabs->bL[9] = cabs->bL[8];
			cabs->bL[8] = cabs->bL[7];
			cabs->bL[7] = cabs->bL[6];
			cabs->bL[6] = cabs->bL[5];
			cabs->bL[5] = cabs->bL[4];
			cabs->bL[4] = cabs->bL[3];
			cabs->bL[3] = cabs->bL[2];
			cabs->bL[2] = cabs->bL[1];
			cabs->bL[1] = cabs->bL[0];
			cabs->bL[0] = inputSampleL; //-------
			cabs->bR[82] = cabs->bR[81];
			cabs->bR[81] = cabs->bR[80];
			cabs->bR[80] = cabs->bR[79];
			cabs->bR[79] = cabs->bR[78];
			cabs->bR[78] = cabs->bR[77];
			cabs->bR[77] = cabs->bR[76];
			cabs->bR[76] = cabs->bR[75];
			cabs->bR[75] = cabs->bR[74];
			cabs->bR[74] = cabs->bR[73];
			cabs->bR[73] = cabs->bR[72];
			cabs->bR[72] = cabs->bR[71];
			cabs->bR[71] = cabs->bR[70];
			cabs->bR[70] = cabs->bR[69];
			cabs->bR[69] = cabs->bR[68];
			cabs->bR[68] = cabs->bR[67];
			cabs->bR[67] = cabs->bR[66];
			cabs->bR[66] = cabs->bR[65];
			cabs->bR[65] = cabs->bR[64];
			cabs->bR[64] = cabs->bR[63];
			cabs->bR[63] = cabs->bR[62];
			cabs->bR[62] = cabs->bR[61];
			cabs->bR[61] = cabs->bR[60];
			cabs->bR[60] = cabs->bR[59];
			cabs->bR[59] = cabs->bR[58];
			cabs->bR[58] = cabs->bR[57];
			cabs->bR[57] = cabs->bR[56];
			cabs->bR[56] = cabs->bR[55];
			cabs->bR[55] = cabs->bR[54];
			cabs->bR[54] = cabs->bR[53];
			cabs->bR[53] = cabs->bR[52];
			cabs->bR[52] = cabs->bR[51];
			cabs->bR[51] = cabs->bR[50];
			cabs->bR[50] = cabs->bR[49];
			cabs->bR[49] = cabs->bR[48];
			cabs->bR[48] = cabs->bR[47];
			cabs->bR[47] = cabs->bR[46];
			cabs->bR[46] = cabs->bR[45];
			cabs->bR[45] = cabs->bR[44];
			cabs->bR[44] = cabs->bR[43];
			cabs->bR[43] = cabs->bR[42];
			cabs->bR[42] = cabs->bR[41];
			cabs->bR[41] = cabs->bR[40];
			cabs->bR[40] = cabs->bR[39];
			cabs->bR[39] = cabs->bR[38];
			cabs->bR[38] = cabs->bR[37];
			cabs->bR[37] = cabs->bR[36];
			cabs->bR[36] = cabs->bR[35];
			cabs->bR[35] = cabs->bR[34];
			cabs->bR[34] = cabs->bR[33];
			cabs->bR[33] = cabs->bR[32];
			cabs->bR[32] = cabs->bR[31];
			cabs->bR[31] = cabs->bR[30];
			cabs->bR[30] = cabs->bR[29];
			cabs->bR[29] = cabs->bR[28];
			cabs->bR[28] = cabs->bR[27];
			cabs->bR[27] = cabs->bR[26];
			cabs->bR[26] = cabs->bR[25];
			cabs->bR[25] = cabs->bR[24];
			cabs->bR[24] = cabs->bR[23];
			cabs->bR[23] = cabs->bR[22];
			cabs->bR[22] = cabs->bR[21];
			cabs->bR[21] = cabs->bR[20];
			cabs->bR[20] = cabs->bR[19];
			cabs->bR[19] = cabs->bR[18];
			cabs->bR[18] = cabs->bR[17];
			cabs->bR[17] = cabs->bR[16];
			cabs->bR[16] = cabs->bR[15];
			cabs->bR[15] = cabs->bR[14];
			cabs->bR[14] = cabs->bR[13];
			cabs->bR[13] = cabs->bR[12];
			cabs->bR[12] = cabs->bR[11];
			cabs->bR[11] = cabs->bR[10];
			cabs->bR[10] = cabs->bR[9];
			cabs->bR[9] = cabs->bR[8];
			cabs->bR[8] = cabs->bR[7];
			cabs->bR[7] = cabs->bR[6];
			cabs->bR[6] = cabs->bR[5];
			cabs->bR[5] = cabs->bR[4];
			cabs->bR[4] = cabs->bR[3];
			cabs->bR[3] = cabs->bR[2];
			cabs->bR[2] = cabs->bR[1];
			cabs->bR[1] = cabs->bR[0];
			cabs->bR[0] = inputSampleR;
			// load conv

			double tempSampleL = 0.0; //---------
			double tempSampleR = 0.0; // set up for applying the cab sound
			switch (speaker) {
				case 1:
					// begin HighPowerStack conv L
					tempSampleL += (cabs->bL[1] * (1.29550481610475132 + (0.19713872057074355 * applyconvL)));
					tempSampleL += (cabs->bL[2] * (1.42302569895462616 + (0.30599505521284787 * applyconvL)));
					tempSampleL += (cabs->bL[3] * (1.28728195804197565 + (0.23168333460446133 * applyconvL)));
					tempSampleL += (cabs->bL[4] * (0.88553784290822690 + (0.14263256172918892 * applyconvL)));
					tempSampleL += (cabs->bL[5] * (0.37129054918432319 + (0.00150040944205920 * applyconvL)));
					tempSampleL -= (cabs->bL[6] * (0.12150959412556320 + (0.32776273620569107 * applyconvL)));
					tempSampleL -= (cabs->bL[7] * (0.44900065463203775 + (0.74101214925298819 * applyconvL)));
					tempSampleL -= (cabs->bL[8] * (0.54058781908186482 + (1.07821707459008387 * applyconvL)));
					tempSampleL -= (cabs->bL[9] * (0.49361966401791391 + (1.23540109014850508 * applyconvL)));
					tempSampleL -= (cabs->bL[10] * (0.39819495093078133 + (1.11247213730917749 * applyconvL)));
					tempSampleL -= (cabs->bL[11] * (0.31379279985435521 + (0.80330360359638298 * applyconvL)));
					tempSampleL -= (cabs->bL[12] * (0.30744359242808555 + (0.42132528876858205 * applyconvL)));
					tempSampleL -= (cabs->bL[13] * (0.33943170284673974 + (0.09183418349389982 * applyconvL)));
					tempSampleL -= (cabs->bL[14] * (0.33838775119286391 - (0.06453051658561271 * applyconvL)));
					tempSampleL -= (cabs->bL[15] * (0.30682305697961665 - (0.09549380253249232 * applyconvL)));
					tempSampleL -= (cabs->bL[16] * (0.23408741339295336 - (0.08083404732361277 * applyconvL)));
					tempSampleL -= (cabs->bL[17] * (0.10411746814025019 + (0.00253651281245780 * applyconvL)));
					tempSampleL += (cabs->bL[18] * (0.00133623776084696 - (0.04447267870865820 * applyconvL)));
					tempSampleL += (cabs->bL[19] * (0.02461903992114161 + (0.07530671732655550 * applyconvL)));
					tempSampleL += (cabs->bL[20] * (0.02086715842475373 + (0.22795860236804899 * applyconvL)));
					tempSampleL += (cabs->bL[21] * (0.02761433637100917 + (0.26108320417844094 * applyconvL)));
					tempSampleL += (cabs->bL[22] * (0.04475285369162533 + (0.19160705011061663 * applyconvL)));
					tempSampleL += (cabs->bL[23] * (0.09447338372862381 + (0.03681550508743799 * applyconvL)));
					tempSampleL += (cabs->bL[24] * (0.13445890343722280 - (0.13713036462146147 * applyconvL)));
					tempSampleL += (cabs->bL[25] * (0.13872868945088121 - (0.22401242373298191 * applyconvL)));
					tempSampleL += (cabs->bL[26] * (0.14915650097434549 - (0.26718804981526367 * applyconvL)));
					tempSampleL += (cabs->bL[27] * (0.12766643217091783 - (0.27745664795660430 * applyconvL)));
					tempSampleL += (cabs->bL[28] * (0.03675849788393101 - (0.18338278173550679 * applyconvL)));
					tempSampleL -= (cabs->bL[29] * (0.06307306864232835 + (0.06089480869040766 * applyconvL)));
					tempSampleL -= (cabs->bL[30] * (0.14947389348962944 + (0.04642103054798480 * applyconvL)));
					tempSampleL -= (cabs->bL[31] * (0.25235266566401526 + (0.08423062596460507 * applyconvL)));
					tempSampleL -= (cabs->bL[32] * (0.33496344048679683 + (0.09808328256677995 * applyconvL)));
					tempSampleL -= (cabs->bL[33] * (0.36590030482175445 + (0.10622650888958179 * applyconvL)));
					tempSampleL -= (cabs->bL[34] * (0.35015197011464372 + (0.08982043516016047 * applyconvL)));
					tempSampleL -= (cabs->bL[35] * (0.26808437585665090 + (0.00735561860229533 * applyconvL)));
					tempSampleL -= (cabs->bL[36] * (0.11624318543291220 - (0.07142484314510467 * applyconvL)));
					tempSampleL += (cabs->bL[37] * (0.05617084165377551 + (0.11785854050350089 * applyconvL)));
					tempSampleL += (cabs->bL[38] * (0.20540028692589385 + (0.20479174663329586 * applyconvL)));
					tempSampleL += (cabs->bL[39] * (0.30455415003043818 + (0.29074864580096849 * applyconvL)));
					tempSampleL += (cabs->bL[40] * (0.33810750937829476 + (0.29182307921316802 * applyconvL)));
					tempSampleL += (cabs->bL[41] * (0.31936133365277430 + (0.26535537727394987 * applyconvL)));
					tempSampleL += (cabs->bL[42] * (0.27388548321981876 + (0.19735049990538350 * applyconvL)));
					tempSampleL += (cabs->bL[43] * (0.21454597517994098 + (0.06415909270247236 * applyconvL)));
					tempSampleL += (cabs->bL[44] * (0.15001045817707717 - (0.03831118543404573 * applyconvL)));
					tempSampleL += (cabs->bL[45] * (0.07283437284653138 - (0.09281952429543777 * applyconvL)));
					tempSampleL -= (cabs->bL[46] * (0.03917872184241358 + (0.14306291461398810 * applyconvL)));
					tempSampleL -= (cabs->bL[47] * (0.16695932032148642 + (0.19138995946950504 * applyconvL)));
					tempSampleL -= (cabs->bL[48] * (0.27055854466909462 + (0.22531296466343192 * applyconvL)));
					tempSampleL -= (cabs->bL[49] * (0.33256357307578271 + (0.23305840475692102 * applyconvL)));
					tempSampleL -= (cabs->bL[50] * (0.33459770116834442 + (0.24091822618917569 * applyconvL)));
					tempSampleL -= (cabs->bL[51] * (0.27156687236338090 + (0.24062938573512443 * applyconvL)));
					tempSampleL -= (cabs->bL[52] * (0.17197093288412094 + (0.19083085091993421 * applyconvL)));
					tempSampleL -= (cabs->bL[53] * (0.06738628195910543 + (0.10268609751019808 * applyconvL)));
					tempSampleL += (cabs->bL[54] * (0.00222429218204290 + (0.01439664435720548 * applyconvL)));
					tempSampleL += (cabs->bL[55] * (0.01346992803494091 + (0.15947137113534526 * applyconvL)));
					tempSampleL -= (cabs->bL[56] * (0.02038911881377448 - (0.26763170752416160 * applyconvL)));
					tempSampleL -= (cabs->bL[57] * (0.08233579178189687 - (0.29415931086406055 * applyconvL)));
					tempSampleL -= (cabs->bL[58] * (0.15447855089824883 - (0.26489186990840807 * applyconvL)));
					tempSampleL -= (cabs->bL[59] * (0.20518281113362655 - (0.16135382257522859 * applyconvL)));
					tempSampleL -= (cabs->bL[60] * (0.22244686050232007 + (0.00847180390247432 * applyconvL)));
					tempSampleL -= (cabs->bL[61] * (0.21849243134998034 + (0.14460595245753741 * applyconvL)));
					tempSampleL -= (cabs->bL[62] * (0.20256105734574054 + (0.18932793221831667 * applyconvL)));
					tempSampleL -= (cabs->bL[63] * (0.18604070054295399 + (0.17250665610927965 * applyconvL)));
					tempSampleL -= (cabs->bL[64] * (0.17222844322058231 + (0.12992472027850357 * applyconvL)));
					tempSampleL -= (cabs->bL[65] * (0.14447856616566443 + (0.09089219002147308 * applyconvL)));
					tempSampleL -= (cabs->bL[66] * (0.10385520794251019 + (0.08600465834570559 * applyconvL)));
					tempSampleL -= (cabs->bL[67] * (0.07124435678265063 + (0.09071532210549428 * applyconvL)));
					tempSampleL -= (cabs->bL[68] * (0.05216857461197572 + (0.06794061706070262 * applyconvL)));
					tempSampleL -= (cabs->bL[69] * (0.05235381920184123 + (0.02818101717909346 * applyconvL)));
					tempSampleL -= (cabs->bL[70] * (0.07569701245553526 - (0.00634228544764946 * applyconvL)));
					tempSampleL -= (cabs->bL[71] * (0.10320125382718826 - (0.02751486906644141 * applyconvL)));
					tempSampleL -= (cabs->bL[72] * (0.12122120969079088 - (0.05434007312178933 * applyconvL)));
					tempSampleL -= (cabs->bL[73] * (0.13438969117200902 - (0.09135218559713874 * applyconvL)));
					tempSampleL -= (cabs->bL[74] * (0.13534390437529981 - (0.10437672041458675 * applyconvL)));
					tempSampleL -= (cabs->bL[75] * (0.11424128854188388 - (0.08693450726462598 * applyconvL)));
					tempSampleL -= (cabs->bL[76] * (0.08166894518596159 - (0.06949989431475120 * applyconvL)));
					tempSampleL -= (cabs->bL[77] * (0.04293976378555305 - (0.05718625137421843 * applyconvL)));
					tempSampleL += (cabs->bL[78] * (0.00933076320644409 + (0.01728285211520138 * applyconvL)));
					tempSampleL += (cabs->bL[79] * (0.06450430362918153 - (0.02492994833691022 * applyconvL)));
					tempSampleL += (cabs->bL[80] * (0.10187400687649277 - (0.03578455940532403 * applyconvL)));
					tempSampleL += (cabs->bL[81] * (0.11039763294094571 - (0.03995523517573508 * applyconvL)));
					tempSampleL += (cabs->bL[82] * (0.08557960776024547 - (0.03482514309492527 * applyconvL)));
					tempSampleL += (cabs->bL[83] * (0.02730881850805332 - (0.00514750108411127 * applyconvL)));
					// begin HighPowerStack conv R
					tempSampleR += (cabs->bR[1] * (1.29550481610475132 + (0.19713872057074355 * applyconvR)));
					tempSampleR += (cabs->bR[2] * (1.42302569895462616 + (0.30599505521284787 * applyconvR)));
					tempSampleR += (cabs->bR[3] * (1.28728195804197565 + (0.23168333460446133 * applyconvR)));
					tempSampleR += (cabs->bR[4] * (0.88553784290822690 + (0.14263256172918892 * applyconvR)));
					tempSampleR += (cabs->bR[5] * (0.37129054918432319 + (0.00150040944205920 * applyconvR)));
					tempSampleR -= (cabs->bR[6] * (0.12150959412556320 + (0.32776273620569107 * applyconvR)));
					tempSampleR -= (cabs->bR[7] * (0.44900065463203775 + (0.74101214925298819 * applyconvR)));
					tempSampleR -= (cabs->bR[8] * (0.54058781908186482 + (1.07821707459008387 * applyconvR)));
					tempSampleR -= (cabs->bR[9] * (0.49361966401791391 + (1.23540109014850508 * applyconvR)));
					tempSampleR -= (cabs->bR[10] * (0.39819495093078133 + (1.11247213730917749 * applyconvR)));
					tempSampleR -= (cabs->bR[11] * (0.31379279985435521 + (0.80330360359638298 * applyconvR)));
					tempSampleR -= (cabs->bR[12] * (0.30744359242808555 + (0.42132528876858205 * applyconvR)));
					tempSampleR -= (cabs->bR[13] * (0.33943170284673974 + (0.09183418349389982 * applyconvR)));
					tempSampleR -= (cabs->bR[14] * (0.33838775119286391 - (0.06453051658561271 * applyconvR)));
					tempSampleR -= (cabs->bR[15] * (0.30682305697961665 - (0.09549380253249232 * applyconvR)));
					tempSampleR -= (cabs->bR[16] * (0.23408741339295336 - (0.08083404732361277 * applyconvR)));
					tempSampleR -= (cabs->bR[17] * (0.10411746814025019 + (0.00253651281245780 * applyconvR)));
					tempSampleR += (cabs->bR[18] * (0.00133623776084696 - (0.04447267870865820 * applyconvR)));
					tempSampleR += (cabs->bR[19] * (0.02461903992114161 + (0.07530671732655550 * applyconvR)));
					tempSampleR += (cabs->bR[20] * (0.02086715842475373 + (0.22795860236804899 * applyconvR)));
					tempSampleR += (cabs->bR[21] * (0.02761433637100917 + (0.26108320417844094 * applyconvR)));
					tempSampleR += (cabs->bR[22] * (0.04475285369162533 + (0.19160705011061663 * applyconvR)));
					tempSampleR += (cabs->bR[23] * (0.09447338372862381 + (0.03681550508743799 * applyconvR)));
					tempSampleR += (cabs->bR[24] * (0.13445890343722280 - (0.13713036462146147 * applyconvR)));
					tempSampleR += (cabs->bR[25] * (0.13872868945088121 - (0.22401242373298191 * applyconvR)));
					tempSampleR += (cabs->bR[26] * (0.14915650097434549 - (0.26718804981526367 * applyconvR)));
					tempSampleR += (cabs->bR[27] * (0.12766643217091783 - (0.27745664795660430 * applyconvR)));
					tempSampleR += (cabs->bR[28] * (0.03675849788393101 - (0.18338278173550679 * applyconvR)));
					tempSampleR -= (cabs->bR[29] * (0.06307306864232835 + (0.06089480869040766 * applyconvR)));
					tempSampleR -= (cabs->bR[30] * (0.14947389348962944 + (0.04642103054798480 * applyconvR)));
					tempSampleR -= (cabs->bR[31] * (0.25235266566401526 + (0.08423062596460507 * applyconvR)));
					tempSampleR -= (cabs->bR[32] * (0.33496344048679683 + (0.09808328256677995 * applyconvR)));
					tempSampleR -= (cabs->bR[33] * (0.36590030482175445 + (0.10622650888958179 * applyconvR)));
					tempSampleR -= (cabs->bR[34] * (0.35015197011464372 + (0.08982043516016047 * applyconvR)));
					tempSampleR -= (cabs->bR[35] * (0.26808437585665090 + (0.00735561860229533 * applyconvR)));
					tempSampleR -= (cabs->bR[36] * (0.11624318543291220 - (0.07142484314510467 * applyconvR)));
					tempSampleR += (cabs->bR[37] * (0.05617084165377551 + (0.11785854050350089 * applyconvR)));
					tempSampleR += (cabs->bR[38] * (0.20540028692589385 + (0.20479174663329586 * applyconvR)));
					tempSampleR += (cabs->bR[39] * (0.30455415003043818 + (0.29074864580096849 * applyconvR)));
					tempSampleR += (cabs->bR[40] * (0.33810750937829476 + (0.29182307921316802 * applyconvR)));
					tempSampleR += (cabs->bR[41] * (0.31936133365277430 + (0.26535537727394987 * applyconvR)));
					tempSampleR += (cabs->bR[42] * (0.27388548321981876 + (0.19735049990538350 * applyconvR)));
					tempSampleR += (cabs->bR[43] * (0.21454597517994098 + (0.06415909270247236 * applyconvR)));
					tempSampleR += (cabs->bR[44] * (0.15001045817707717 - (0.03831118543404573 * applyconvR)));
					tempSampleR += (cabs->bR[45] * (0.07283437284653138 - (0.09281952429543777 * applyconvR)));
					tempSampleR -= (cabs->bR[46] * (0.03917872184241358 + (0.14306291461398810 * applyconvR)));
					tempSampleR -= (cabs->bR[47] * (0.16695932032148642 + (0.19138995946950504 * applyconvR)));
					tempSampleR -= (cabs->bR[48] * (0.27055854466909462 + (0.22531296466343192 * applyconvR)));
					tempSampleR -= (cabs->bR[49] * (0.33256357307578271 + (0.23305840475692102 * applyconvR)));
					tempSampleR -= (cabs->bR[50] * (0.33459770116834442 + (0.24091822618917569 * applyconvR)));
					tempSampleR -= (cabs->bR[51] * (0.27156687236338090 + (0.24062938573512443 * applyconvR)));
					tempSampleR -= (cabs->bR[52] * (0.17197093288412094 + (0.19083085091993421 * applyconvR)));
					tempSampleR -= (cabs->bR[53] * (0.06738628195910543 + (0.10268609751019808 * applyconvR)));
					tempSampleR += (cabs->bR[54] * (0.00222429218204290 + (0.01439664435720548 * applyconvR)));
					tempSampleR += (cabs->bR[55] * (0.01346992803494091 + (0.15947137113534526 * applyconvR)));
					tempSampleR -= (cabs->bR[56] * (0.02038911881377448 - (0.26763170752416160 * applyconvR)));
					tempSampleR -= (cabs->bR[57] * (0.08233579178189687 - (0.29415931086406055 * applyconvR)));
					tempSampleR -= (cabs->bR[58] * (0.15447855089824883 - (0.26489186990840807 * applyconvR)));
					tempSampleR -= (cabs->bR[59] * (0.20518281113362655 - (0.16135382257522859 * applyconvR)));
					tempSampleR -= (cabs->bR[60] * (0.22244686050232007 + (0.00847180390247432 * applyconvR)));
					tempSampleR -= (cabs->bR[61] * (0.21849243134998034 + (0.14460595245753741 * applyconvR)));
					tempSampleR -= (cabs->bR[62] * (0.20256105734574054 + (0.18932793221831667 * applyconvR)));
					tempSampleR -= (cabs->bR[63] * (0.18604070054295399 + (0.17250665610927965 * applyconvR)));
					tempSampleR -= (cabs->bR[64] * (0.17222844322058231 + (0.12992472027850357 * applyconvR)));
					tempSampleR -= (cabs->bR[65] * (0.14447856616566443 + (0.09089219002147308 * applyconvR)));
					tempSampleR -= (cabs->bR[66] * (0.10385520794251019 + (0.08600465834570559 * applyconvR)));
					tempSampleR -= (cabs->bR[67] * (0.07124435678265063 + (0.09071532210549428 * applyconvR)));
					tempSampleR -= (cabs->bR[68] * (0.05216857461197572 + (0.06794061706070262 * applyconvR)));
					tempSampleR -= (cabs->bR[69] * (0.05235381920184123 + (0.02818101717909346 * applyconvR)));
					tempSampleR -= (cabs->bR[70] * (0.07569701245553526 - (0.00634228544764946 * applyconvR)));
					tempSampleR -= (cabs->bR[71] * (0.10320125382718826 - (0.02751486906644141 * applyconvR)));
					tempSampleR -= (cabs->bR[72] * (0.12122120969079088 - (0.05434007312178933 * applyconvR)));
					tempSampleR -= (cabs->bR[73] * (0.13438969117200902 - (0.09135218559713874 * applyconvR)));
					tempSampleR -= (cabs->bR[74] * (0.13534390437529981 - (0.10437672041458675 * applyconvR)));
					tempSampleR -= (cabs->bR[75] * (0.11424128854188388 - (0.08693450726462598 * applyconvR)));
					tempSampleR -= (cabs->bR[76] * (0.08166894518596159 - (0.06949989431475120 * applyconvR)));
					tempSampleR -= (cabs->bR[77] * (0.04293976378555305 - (0.05718625137421843 * applyconvR)));
					tempSampleR += (cabs->bR[78] * (0.00933076320644409 + (0.01728285211520138 * applyconvR)));
					tempSampleR += (cabs->bR[79] * (0.06450430362918153 - (0.02492994833691022 * applyconvR)));
					tempSampleR += (cabs->bR[80] * (0.10187400687649277 - (0.03578455940532403 * applyconvR)));
					tempSampleR += (cabs->bR[81] * (0.11039763294094571 - (0.03995523517573508 * applyconvR)));
					tempSampleR += (cabs->bR[82] * (0.08557960776024547 - (0.03482514309492527 * applyconvR)));
					tempSampleR += (cabs->bR[83] * (0.02730881850805332 - (0.00514750108411127 * applyconvR)));
					// end HighPowerStack conv
					break;
				case 2:
					// begin VintageStack conv L
					tempSampleL += (cabs->bL[1] * (1.31698250313308396 - (0.08140616497621633 * applyconvL)));
					tempSampleL += (cabs->bL[2] * (1.47229016949915326 - (0.27680278993637253 * applyconvL)));
					tempSampleL += (cabs->bL[3] * (1.30410109086044956 - (0.35629113432046489 * applyconvL)));
					tempSampleL += (cabs->bL[4] * (0.81766210474551260 - (0.26808782337659753 * applyconvL)));
					tempSampleL += (cabs->bL[5] * (0.19868872545506663 - (0.11105517193919669 * applyconvL)));
					tempSampleL -= (cabs->bL[6] * (0.39115909132567039 - (0.12630622002682679 * applyconvL)));
					tempSampleL -= (cabs->bL[7] * (0.76881891559343574 - (0.40879849500403143 * applyconvL)));
					tempSampleL -= (cabs->bL[8] * (0.87146861782680340 - (0.59529560488000599 * applyconvL)));
					tempSampleL -= (cabs->bL[9] * (0.79504575932563670 - (0.60877047551611796 * applyconvL)));
					tempSampleL -= (cabs->bL[10] * (0.61653017622406314 - (0.47662851438557335 * applyconvL)));
					tempSampleL -= (cabs->bL[11] * (0.40718195794382067 - (0.24955839378539713 * applyconvL)));
					tempSampleL -= (cabs->bL[12] * (0.31794900040616203 - (0.04169792259600613 * applyconvL)));
					tempSampleL -= (cabs->bL[13] * (0.41075032540217843 + (0.00368483996076280 * applyconvL)));
					tempSampleL -= (cabs->bL[14] * (0.56901352922170667 - (0.11027360805893105 * applyconvL)));
					tempSampleL -= (cabs->bL[15] * (0.62443222391889264 - (0.22198075154245228 * applyconvL)));
					tempSampleL -= (cabs->bL[16] * (0.53462856723129204 - (0.22933544545324852 * applyconvL)));
					tempSampleL -= (cabs->bL[17] * (0.34441703361995046 - (0.12956809502269492 * applyconvL)));
					tempSampleL -= (cabs->bL[18] * (0.13947052337867882 + (0.00339775055962799 * applyconvL)));
					tempSampleL += (cabs->bL[19] * (0.03771252648928484 - (0.10863931549251718 * applyconvL)));
					tempSampleL += (cabs->bL[20] * (0.18280210770271693 - (0.17413646599296417 * applyconvL)));
					tempSampleL += (cabs->bL[21] * (0.24621986701761467 - (0.14547053270435095 * applyconvL)));
					tempSampleL += (cabs->bL[22] * (0.22347075142737360 - (0.02493869490104031 * applyconvL)));
					tempSampleL += (cabs->bL[23] * (0.14346348482123716 + (0.11284054747963246 * applyconvL)));
					tempSampleL += (cabs->bL[24] * (0.00834364862916028 + (0.24284684053733926 * applyconvL)));
					tempSampleL -= (cabs->bL[25] * (0.11559740296078347 - (0.32623054435304538 * applyconvL)));
					tempSampleL -= (cabs->bL[26] * (0.18067604561283060 - (0.32311481551122478 * applyconvL)));
					tempSampleL -= (cabs->bL[27] * (0.22927997789035612 - (0.26991539052832925 * applyconvL)));
					tempSampleL -= (cabs->bL[28] * (0.28487666578669446 - (0.22437227250279349 * applyconvL)));
					tempSampleL -= (cabs->bL[29] * (0.31992973037153838 - (0.15289876100963865 * applyconvL)));
					tempSampleL -= (cabs->bL[30] * (0.35174606303520733 - (0.05656293023086628 * applyconvL)));
					tempSampleL -= (cabs->bL[31] * (0.36894898011375254 + (0.04333925421463558 * applyconvL)));
					tempSampleL -= (cabs->bL[32] * (0.32567576055307507 + (0.14594589410921388 * applyconvL)));
					tempSampleL -= (cabs->bL[33] * (0.27440135050585784 + (0.15529667398122521 * applyconvL)));
					tempSampleL -= (cabs->bL[34] * (0.21998973785078091 + (0.05083553737157104 * applyconvL)));
					tempSampleL -= (cabs->bL[35] * (0.10323624876862457 - (0.04651829594199963 * applyconvL)));
					tempSampleL += (cabs->bL[36] * (0.02091603687851074 + (0.12000046818439322 * applyconvL)));
					tempSampleL += (cabs->bL[37] * (0.11344930914138468 + (0.17697142512225839 * applyconvL)));
					tempSampleL += (cabs->bL[38] * (0.22766779627643968 + (0.13645102964003858 * applyconvL)));
					tempSampleL += (cabs->bL[39] * (0.38378309953638229 - (0.01997653307333791 * applyconvL)));
					tempSampleL += (cabs->bL[40] * (0.52789400804568076 - (0.21409137428422448 * applyconvL)));
					tempSampleL += (cabs->bL[41] * (0.55444630296938280 - (0.32331980931576626 * applyconvL)));
					tempSampleL += (cabs->bL[42] * (0.42333237669264601 - (0.26855847463044280 * applyconvL)));
					tempSampleL += (cabs->bL[43] * (0.21942831522035078 - (0.12051365248820624 * applyconvL)));
					tempSampleL -= (cabs->bL[44] * (0.00584169427830633 - (0.03706970171280329 * applyconvL)));
					tempSampleL -= (cabs->bL[45] * (0.24279799124660351 - (0.17296440491477982 * applyconvL)));
					tempSampleL -= (cabs->bL[46] * (0.40173760787507085 - (0.21717989835163351 * applyconvL)));
					tempSampleL -= (cabs->bL[47] * (0.43930035724188155 - (0.16425928481378199 * applyconvL)));
					tempSampleL -= (cabs->bL[48] * (0.41067765934041811 - (0.10390115786636855 * applyconvL)));
					tempSampleL -= (cabs->bL[49] * (0.34409235547165967 - (0.07268159377411920 * applyconvL)));
					tempSampleL -= (cabs->bL[50] * (0.26542883122568151 - (0.05483457497365785 * applyconvL)));
					tempSampleL -= (cabs->bL[51] * (0.22024754776138800 - (0.06484897950087598 * applyconvL)));
					tempSampleL -= (cabs->bL[52] * (0.20394367993632415 - (0.08746309731952180 * applyconvL)));
					tempSampleL -= (cabs->bL[53] * (0.17565242431124092 - (0.07611309538078760 * applyconvL)));
					tempSampleL -= (cabs->bL[54] * (0.10116623231246825 - (0.00642818706295112 * applyconvL)));
					tempSampleL -= (cabs->bL[55] * (0.00782648272053632 + (0.08004141267685004 * applyconvL)));
					tempSampleL += (cabs->bL[56] * (0.05059046006747323 - (0.12436676387548490 * applyconvL)));
					tempSampleL += (cabs->bL[57] * (0.06241531553254467 - (0.11530779547021434 * applyconvL)));
					tempSampleL += (cabs->bL[58] * (0.04952694587101836 - (0.08340945324333944 * applyconvL)));
					tempSampleL += (cabs->bL[59] * (0.00843873294401687 - (0.03279659052562903 * applyconvL)));
					tempSampleL -= (cabs->bL[60] * (0.05161338949440241 - (0.03428181149163798 * applyconvL)));
					tempSampleL -= (cabs->bL[61] * (0.08165520146902012 - (0.08196746092283110 * applyconvL)));
					tempSampleL -= (cabs->bL[62] * (0.06639532849935320 - (0.09797462781896329 * applyconvL)));
					tempSampleL -= (cabs->bL[63] * (0.02953430910661621 - (0.09175612938515763 * applyconvL)));
					tempSampleL += (cabs->bL[64] * (0.00741058547442938 + (0.05442091048731967 * applyconvL)));
					tempSampleL += (cabs->bL[65] * (0.01832866125391727 + (0.00306243693643687 * applyconvL)));
					tempSampleL += (cabs->bL[66] * (0.00526964230373573 - (0.04364102661136410 * applyconvL)));
					tempSampleL -= (cabs->bL[67] * (0.00300984373848200 + (0.09742737841278880 * applyconvL)));
					tempSampleL -= (cabs->bL[68] * (0.00413616769576694 + (0.14380661694523073 * applyconvL)));
					tempSampleL -= (cabs->bL[69] * (0.00588769034931419 + (0.16012843578892538 * applyconvL)));
					tempSampleL -= (cabs->bL[70] * (0.00688588239450581 + (0.14074464279305798 * applyconvL)));
					tempSampleL -= (cabs->bL[71] * (0.02277307992926315 + (0.07914752191801366 * applyconvL)));
					tempSampleL -= (cabs->bL[72] * (0.04627166091180877 - (0.00192787268067208 * applyconvL)));
					tempSampleL -= (cabs->bL[73] * (0.05562045897455786 - (0.05932868727665747 * applyconvL)));
					tempSampleL -= (cabs->bL[74] * (0.05134243784922165 - (0.08245334798868090 * applyconvL)));
					tempSampleL -= (cabs->bL[75] * (0.04719409472239919 - (0.07498680629253825 * applyconvL)));
					tempSampleL -= (cabs->bL[76] * (0.05889738914266415 - (0.06116127018043697 * applyconvL)));
					tempSampleL -= (cabs->bL[77] * (0.09428363535111127 - (0.06535868867863834 * applyconvL)));
					tempSampleL -= (cabs->bL[78] * (0.15181756953225126 - (0.08982979655234427 * applyconvL)));
					tempSampleL -= (cabs->bL[79] * (0.20878969456036670 - (0.10761070891499538 * applyconvL)));
					tempSampleL -= (cabs->bL[80] * (0.22647885581813790 - (0.08462542510349125 * applyconvL)));
					tempSampleL -= (cabs->bL[81] * (0.19723482443646323 - (0.02665160920736287 * applyconvL)));
					tempSampleL -= (cabs->bL[82] * (0.16441643451155163 + (0.02314691954338197 * applyconvL)));
					tempSampleL -= (cabs->bL[83] * (0.15201914054931515 + (0.04424903493886839 * applyconvL)));
					tempSampleL -= (cabs->bL[84] * (0.15454370641307855 + (0.04223203797913008 * applyconvL)));
					// begin VintageStack conv R
					tempSampleR += (cabs->bR[1] * (1.31698250313308396 - (0.08140616497621633 * applyconvR)));
					tempSampleR += (cabs->bR[2] * (1.47229016949915326 - (0.27680278993637253 * applyconvR)));
					tempSampleR += (cabs->bR[3] * (1.30410109086044956 - (0.35629113432046489 * applyconvR)));
					tempSampleR += (cabs->bR[4] * (0.81766210474551260 - (0.26808782337659753 * applyconvR)));
					tempSampleR += (cabs->bR[5] * (0.19868872545506663 - (0.11105517193919669 * applyconvR)));
					tempSampleR -= (cabs->bR[6] * (0.39115909132567039 - (0.12630622002682679 * applyconvR)));
					tempSampleR -= (cabs->bR[7] * (0.76881891559343574 - (0.40879849500403143 * applyconvR)));
					tempSampleR -= (cabs->bR[8] * (0.87146861782680340 - (0.59529560488000599 * applyconvR)));
					tempSampleR -= (cabs->bR[9] * (0.79504575932563670 - (0.60877047551611796 * applyconvR)));
					tempSampleR -= (cabs->bR[10] * (0.61653017622406314 - (0.47662851438557335 * applyconvR)));
					tempSampleR -= (cabs->bR[11] * (0.40718195794382067 - (0.24955839378539713 * applyconvR)));
					tempSampleR -= (cabs->bR[12] * (0.31794900040616203 - (0.04169792259600613 * applyconvR)));
					tempSampleR -= (cabs->bR[13] * (0.41075032540217843 + (0.00368483996076280 * applyconvR)));
					tempSampleR -= (cabs->bR[14] * (0.56901352922170667 - (0.11027360805893105 * applyconvR)));
					tempSampleR -= (cabs->bR[15] * (0.62443222391889264 - (0.22198075154245228 * applyconvR)));
					tempSampleR -= (cabs->bR[16] * (0.53462856723129204 - (0.22933544545324852 * applyconvR)));
					tempSampleR -= (cabs->bR[17] * (0.34441703361995046 - (0.12956809502269492 * applyconvR)));
					tempSampleR -= (cabs->bR[18] * (0.13947052337867882 + (0.00339775055962799 * applyconvR)));
					tempSampleR += (cabs->bR[19] * (0.03771252648928484 - (0.10863931549251718 * applyconvR)));
					tempSampleR += (cabs->bR[20] * (0.18280210770271693 - (0.17413646599296417 * applyconvR)));
					tempSampleR += (cabs->bR[21] * (0.24621986701761467 - (0.14547053270435095 * applyconvR)));
					tempSampleR += (cabs->bR[22] * (0.22347075142737360 - (0.02493869490104031 * applyconvR)));
					tempSampleR += (cabs->bR[23] * (0.14346348482123716 + (0.11284054747963246 * applyconvR)));
					tempSampleR += (cabs->bR[24] * (0.00834364862916028 + (0.24284684053733926 * applyconvR)));
					tempSampleR -= (cabs->bR[25] * (0.11559740296078347 - (0.32623054435304538 * applyconvR)));
					tempSampleR -= (cabs->bR[26] * (0.18067604561283060 - (0.32311481551122478 * applyconvR)));
					tempSampleR -= (cabs->bR[27] * (0.22927997789035612 - (0.26991539052832925 * applyconvR)));
					tempSampleR -= (cabs->bR[28] * (0.28487666578669446 - (0.22437227250279349 * applyconvR)));
					tempSampleR -= (cabs->bR[29] * (0.31992973037153838 - (0.15289876100963865 * applyconvR)));
					tempSampleR -= (cabs->bR[30] * (0.35174606303520733 - (0.05656293023086628 * applyconvR)));
					tempSampleR -= (cabs->bR[31] * (0.36894898011375254 + (0.04333925421463558 * applyconvR)));
					tempSampleR -= (cabs->bR[32] * (0.32567576055307507 + (0.14594589410921388 * applyconvR)));
					tempSampleR -= (cabs->bR[33] * (0.27440135050585784 + (0.15529667398122521 * applyconvR)));
					tempSampleR -= (cabs->bR[34] * (0.21998973785078091 + (0.05083553737157104 * applyconvR)));
					tempSampleR -= (cabs->bR[35] * (0.10323624876862457 - (0.04651829594199963 * applyconvR)));
					tempSampleR += (cabs->bR[36] * (0.02091603687851074 + (0.12000046818439322 * applyconvR)));
					tempSampleR += (cabs->bR[37] * (0.11344930914138468 + (0.17697142512225839 * applyconvR)));
					tempSampleR += (cabs->bR[38] * (0.22766779627643968 + (0.13645102964003858 * applyconvR)));
					tempSampleR += (cabs->bR[39] * (0.38378309953638229 - (0.01997653307333791 * applyconvR)));
					tempSampleR += (cabs->bR[40] * (0.52789400804568076 - (0.21409137428422448 * applyconvR)));
					tempSampleR += (cabs->bR[41] * (0.55444630296938280 - (0.32331980931576626 * applyconvR)));
					tempSampleR += (cabs->bR[42] * (0.42333237669264601 - (0.26855847463044280 * applyconvR)));
					tempSampleR += (cabs->bR[43] * (0.21942831522035078 - (0.12051365248820624 * applyconvR)));
					tempSampleR -= (cabs->bR[44] * (0.00584169427830633 - (0.03706970171280329 * applyconvR)));
					tempSampleR -= (cabs->bR[45] * (0.24279799124660351 - (0.17296440491477982 * applyconvR)));
					tempSampleR -= (cabs->bR[46] * (0.40173760787507085 - (0.21717989835163351 * applyconvR)));
					tempSampleR -= (cabs->bR[47] * (0.43930035724188155 - (0.16425928481378199 * applyconvR)));
					tempSampleR -= (cabs->bR[48] * (0.41067765934041811 - (0.10390115786636855 * applyconvR)));
					tempSampleR -= (cabs->bR[49] * (0.34409235547165967 - (0.07268159377411920 * applyconvR)));
					tempSampleR -= (cabs->bR[50] * (0.26542883122568151 - (0.05483457497365785 * applyconvR)));
					tempSampleR -= (cabs->bR[51] * (0.22024754776138800 - (0.06484897950087598 * applyconvR)));
					tempSampleR -= (cabs->bR[52] * (0.20394367993632415 - (0.08746309731952180 * applyconvR)));
					tempSampleR -= (cabs->bR[53] * (0.17565242431124092 - (0.07611309538078760 * applyconvR)));
					tempSampleR -= (cabs->bR[54] * (0.10116623231246825 - (0.00642818706295112 * applyconvR)));
					tempSampleR -= (cabs->bR[55] * (0.00782648272053632 + (0.08004141267685004 * applyconvR)));
					tempSampleR += (cabs->bR[56] * (0.05059046006747323 - (0.12436676387548490 * applyconvR)));
					tempSampleR += (cabs->bR[57] * (0.06241531553254467 - (0.11530779547021434 * applyconvR)));
					tempSampleR += (cabs->bR[58] * (0.04952694587101836 - (0.08340945324333944 * applyconvR)));
					tempSampleR += (cabs->bR[59] * (0.00843873294401687 - (0.03279659052562903 * applyconvR)));
					tempSampleR -= (cabs->bR[60] * (0.05161338949440241 - (0.03428181149163798 * applyconvR)));
					tempSampleR -= (cabs->bR[61] * (0.08165520146902012 - (0.08196746092283110 * applyconvR)));
					tempSampleR -= (cabs->bR[62] * (0.06639532849935320 - (0.09797462781896329 * applyconvR)));
					tempSampleR -= (cabs->bR[63] * (0.02953430910661621 - (0.09175612938515763 * applyconvR)));
					tempSampleR += (cabs->bR[64] * (0.00741058547442938 + (0.05442091048731967 * applyconvR)));
					tempSampleR += (cabs->bR[65] * (0.01832866125391727 + (0.00306243693643687 * applyconvR)));
					tempSampleR += (cabs->bR[66] * (0.00526964230373573 - (0.04364102661136410 * applyconvR)));
					tempSampleR -= (cabs->bR[67] * (0.00300984373848200 + (0.09742737841278880 * applyconvR)));
					tempSampleR -= (cabs->bR[68] * (0.00413616769576694 + (0.14380661694523073 * applyconvR)));
					tempSampleR -= (cabs->bR[69] * (0.00588769034931419 + (0.16012843578892538 * applyconvR)));
					tempSampleR -= (cabs->bR[70] * (0.00688588239450581 + (0.14074464279305798 * applyconvR)));
					tempSampleR -= (cabs->bR[71] * (0.02277307992926315 + (0.07914752191801366 * applyconvR)));
					tempSampleR -= (cabs->bR[72] * (0.04627166091180877 - (0.00192787268067208 * applyconvR)));
					tempSampleR -= (cabs->bR[73] * (0.05562045897455786 - (0.05932868727665747 * applyconvR)));
					tempSampleR -= (cabs->bR[74] * (0.05134243784922165 - (0.08245334798868090 * applyconvR)));
					tempSampleR -= (cabs->bR[75] * (0.04719409472239919 - (0.07498680629253825 * applyconvR)));
					tempSampleR -= (cabs->bR[76] * (0.05889738914266415 - (0.06116127018043697 * applyconvR)));
					tempSampleR -= (cabs->bR[77] * (0.09428363535111127 - (0.06535868867863834 * applyconvR)));
					tempSampleR -= (cabs->bR[78] * (0.15181756953225126 - (0.08982979655234427 * applyconvR)));
					tempSampleR -= (cabs->bR[79] * (0.20878969456036670 - (0.10761070891499538 * applyconvR)));
					tempSampleR -= (cabs->bR[80] * (0.22647885581813790 - (0.08462542510349125 * applyconvR)));
					tempSampleR -= (cabs->bR[81] * (0.19723482443646323 - (0.02665160920736287 * applyconvR)));
					tempSampleR -= (cabs->bR[82] * (0.16441643451155163 + (0.02314691954338197 * applyconvR)));
					tempSampleR -= (cabs->bR[83] * (0.15201914054931515 + (0.04424903493886839 * applyconvR)));
					tempSampleR -= (cabs->bR[84] * (0.15454370641307855 + (0.04223203797913008 * applyconvR)));
					// end VintageStack conv
					break;
				case 3:
					// begin BoutiqueStack conv L
					tempSampleL += (cabs->bL[1] * (1.30406584776167445 - (0.01410622186823351 * applyconvL)));
					tempSampleL += (cabs->bL[2] * (1.09350974154373559 + (0.34478044709202327 * applyconvL)));
					tempSampleL += (cabs->bL[3] * (0.52285510059938256 + (0.84225842837363574 * applyconvL)));
					tempSampleL -= (cabs->bL[4] * (0.00018126260714707 - (1.02446537989058117 * applyconvL)));
					tempSampleL -= (cabs->bL[5] * (0.34943699771860115 - (0.84094709567790016 * applyconvL)));
					tempSampleL -= (cabs->bL[6] * (0.53068048407937285 - (0.49231169327705593 * applyconvL)));
					tempSampleL -= (cabs->bL[7] * (0.48631669406792399 - (0.08965111766223610 * applyconvL)));
					tempSampleL -= (cabs->bL[8] * (0.28099201947014130 + (0.23921137841068607 * applyconvL)));
					tempSampleL -= (cabs->bL[9] * (0.10333290012666248 + (0.35058962687321482 * applyconvL)));
					tempSampleL -= (cabs->bL[10] * (0.06605032198166226 + (0.23447405567823365 * applyconvL)));
					tempSampleL -= (cabs->bL[11] * (0.10485808661261729 + (0.05025985449763527 * applyconvL)));
					tempSampleL -= (cabs->bL[12] * (0.13231190973014911 - (0.05484648240248013 * applyconvL)));
					tempSampleL -= (cabs->bL[13] * (0.12926184767180304 - (0.04054223744746116 * applyconvL)));
					tempSampleL -= (cabs->bL[14] * (0.13802696739839460 + (0.01876754906568237 * applyconvL)));
					tempSampleL -= (cabs->bL[15] * (0.16548980700926913 + (0.06772130758771169 * applyconvL)));
					tempSampleL -= (cabs->bL[16] * (0.14469310965751475 + (0.10590928840978781 * applyconvL)));
					tempSampleL -= (cabs->bL[17] * (0.07838457396093310 + (0.13120101199677947 * applyconvL)));
					tempSampleL -= (cabs->bL[18] * (0.05123031606187391 + (0.13883400806512292 * applyconvL)));
					tempSampleL -= (cabs->bL[19] * (0.08906103481939850 + (0.07840461228402337 * applyconvL)));
					tempSampleL -= (cabs->bL[20] * (0.13939265522625241 + (0.01194366471800457 * applyconvL)));
					tempSampleL -= (cabs->bL[21] * (0.14957600717294034 + (0.07687598594361914 * applyconvL)));
					tempSampleL -= (cabs->bL[22] * (0.14112708654047090 + (0.20118461131186977 * applyconvL)));
					tempSampleL -= (cabs->bL[23] * (0.14961020766492997 + (0.30005716443826147 * applyconvL)));
					tempSampleL -= (cabs->bL[24] * (0.16130382224652270 + (0.40459872030013055 * applyconvL)));
					tempSampleL -= (cabs->bL[25] * (0.15679868471080052 + (0.47292767226083465 * applyconvL)));
					tempSampleL -= (cabs->bL[26] * (0.16456530552807727 + (0.45182121471666481 * applyconvL)));
					tempSampleL -= (cabs->bL[27] * (0.16852385701909278 + (0.38272684270752266 * applyconvL)));
					tempSampleL -= (cabs->bL[28] * (0.13317562760966850 + (0.28829580273670768 * applyconvL)));
					tempSampleL -= (cabs->bL[29] * (0.09396196532150952 + (0.18886898332071317 * applyconvL)));
					tempSampleL -= (cabs->bL[30] * (0.10133496956734221 + (0.11158788414137354 * applyconvL)));
					tempSampleL -= (cabs->bL[31] * (0.16097596389376778 + (0.02621299102374547 * applyconvL)));
					tempSampleL -= (cabs->bL[32] * (0.21419006394821866 - (0.03585678078834797 * applyconvL)));
					tempSampleL -= (cabs->bL[33] * (0.21273234570555244 - (0.02574469802924526 * applyconvL)));
					tempSampleL -= (cabs->bL[34] * (0.16934948798707830 + (0.01354331184333835 * applyconvL)));
					tempSampleL -= (cabs->bL[35] * (0.11970436472852493 + (0.04242183865883427 * applyconvL)));
					tempSampleL -= (cabs->bL[36] * (0.09329023656747724 + (0.06890873292358397 * applyconvL)));
					tempSampleL -= (cabs->bL[37] * (0.10255328436608116 + (0.11482972519137427 * applyconvL)));
					tempSampleL -= (cabs->bL[38] * (0.13883223352796811 + (0.18016014431438840 * applyconvL)));
					tempSampleL -= (cabs->bL[39] * (0.16532844286979087 + (0.24521957638633446 * applyconvL)));
					tempSampleL -= (cabs->bL[40] * (0.16254607738965438 + (0.25669472097572482 * applyconvL)));
					tempSampleL -= (cabs->bL[41] * (0.15353207135544752 + (0.15048064682912729 * applyconvL)));
					tempSampleL -= (cabs->bL[42] * (0.13039046390746015 - (0.00200335414623601 * applyconvL)));
					tempSampleL -= (cabs->bL[43] * (0.06707245032180627 - (0.06498125592578702 * applyconvL)));
					tempSampleL += (cabs->bL[44] * (0.01427326441869788 + (0.01940451360783622 * applyconvL)));
					tempSampleL += (cabs->bL[45] * (0.06151238306578224 - (0.07335755969763329 * applyconvL)));
					tempSampleL += (cabs->bL[46] * (0.04685840498892526 - (0.14258849371688248 * applyconvL)));
					tempSampleL -= (cabs->bL[47] * (0.00950136304466093 + (0.14379354707665129 * applyconvL)));
					tempSampleL -= (cabs->bL[48] * (0.06245771575493557 + (0.07639718586346110 * applyconvL)));
					tempSampleL -= (cabs->bL[49] * (0.07159593175777741 - (0.00595536565276915 * applyconvL)));
					tempSampleL -= (cabs->bL[50] * (0.03167929390245019 - (0.03856769526301793 * applyconvL)));
					tempSampleL += (cabs->bL[51] * (0.01890898565110766 + (0.00760539424271147 * applyconvL)));
					tempSampleL += (cabs->bL[52] * (0.04926161137832240 - (0.06411014430053390 * applyconvL)));
					tempSampleL += (cabs->bL[53] * (0.05768814623421683 - (0.15068618173358578 * applyconvL)));
					tempSampleL += (cabs->bL[54] * (0.06144258297076708 - (0.21200636329120301 * applyconvL)));
					tempSampleL += (cabs->bL[55] * (0.06348341960185613 - (0.19620269813094307 * applyconvL)));
					tempSampleL += (cabs->bL[56] * (0.04877736350310589 - (0.11864999881200111 * applyconvL)));
					tempSampleL += (cabs->bL[57] * (0.01010950997574472 - (0.02630070679113791 * applyconvL)));
					tempSampleL -= (cabs->bL[58] * (0.02929178864801191 - (0.04439260202207482 * applyconvL)));
					tempSampleL -= (cabs->bL[59] * (0.03484517126321562 - (0.04508635396034735 * applyconvL)));
					tempSampleL -= (cabs->bL[60] * (0.00547176780437610 - (0.00205637806941426 * applyconvL)));
					tempSampleL += (cabs->bL[61] * (0.02278296865283977 - (0.00063732526427685 * applyconvL)));
					tempSampleL += (cabs->bL[62] * (0.02688982591366477 + (0.05333738901586284 * applyconvL)));
					tempSampleL += (cabs->bL[63] * (0.01942012754957055 + (0.10942832669749143 * applyconvL)));
					tempSampleL += (cabs->bL[64] * (0.01572585258756565 + (0.11189204189054594 * applyconvL)));
					tempSampleL += (cabs->bL[65] * (0.01490550715016034 + (0.04449822818925343 * applyconvL)));
					tempSampleL += (cabs->bL[66] * (0.01715683226376727 - (0.06944648050933899 * applyconvL)));
					tempSampleL += (cabs->bL[67] * (0.02822659878011318 - (0.17843652160132820 * applyconvL)));
					tempSampleL += (cabs->bL[68] * (0.03758307610456144 - (0.21986013433664692 * applyconvL)));
					tempSampleL += (cabs->bL[69] * (0.03275008021608433 - (0.15869878676112170 * applyconvL)));
					tempSampleL += (cabs->bL[70] * (0.01855749786752354 - (0.02337224995718105 * applyconvL)));
					tempSampleL += (cabs->bL[71] * (0.00217095395782931 + (0.10971764224593601 * applyconvL)));
					tempSampleL -= (cabs->bL[72] * (0.01851381451105007 - (0.17214910008793413 * applyconvL)));
					tempSampleL -= (cabs->bL[73] * (0.04722574936345419 - (0.14341588977845254 * applyconvL)));
					tempSampleL -= (cabs->bL[74] * (0.07151540514482006 - (0.04684695724814321 * applyconvL)));
					tempSampleL -= (cabs->bL[75] * (0.06827195484995092 + (0.07022207121861397 * applyconvL)));
					tempSampleL -= (cabs->bL[76] * (0.03290227240464227 + (0.16328400808152735 * applyconvL)));
					tempSampleL += (cabs->bL[77] * (0.01043861198275382 - (0.20184486126076279 * applyconvL)));
					tempSampleL += (cabs->bL[78] * (0.03236563559476477 - (0.17125821306380920 * applyconvL)));
					tempSampleL += (cabs->bL[79] * (0.02040121529932702 - (0.09103660189829657 * applyconvL)));
					tempSampleL -= (cabs->bL[80] * (0.00509649513318102 + (0.01170360991547489 * applyconvL)));
					tempSampleL -= (cabs->bL[81] * (0.01388353426600228 - (0.03588955538451771 * applyconvL)));
					tempSampleL -= (cabs->bL[82] * (0.00523671715033842 - (0.07068798057534148 * applyconvL)));
					tempSampleL += (cabs->bL[83] * (0.00665852487721137 + (0.11666210640054926 * applyconvL)));
					tempSampleL += (cabs->bL[84] * (0.01593540832939290 + (0.15844892856402149 * applyconvL)));
					tempSampleL += (cabs->bL[85] * (0.02080509201836796 + (0.17186274420065850 * applyconvL)));
					// begin BoutiqueStack conv R
					tempSampleR += (cabs->bR[1] * (1.30406584776167445 - (0.01410622186823351 * applyconvR)));
					tempSampleR += (cabs->bR[2] * (1.09350974154373559 + (0.34478044709202327 * applyconvR)));
					tempSampleR += (cabs->bR[3] * (0.52285510059938256 + (0.84225842837363574 * applyconvR)));
					tempSampleR -= (cabs->bR[4] * (0.00018126260714707 - (1.02446537989058117 * applyconvR)));
					tempSampleR -= (cabs->bR[5] * (0.34943699771860115 - (0.84094709567790016 * applyconvR)));
					tempSampleR -= (cabs->bR[6] * (0.53068048407937285 - (0.49231169327705593 * applyconvR)));
					tempSampleR -= (cabs->bR[7] * (0.48631669406792399 - (0.08965111766223610 * applyconvR)));
					tempSampleR -= (cabs->bR[8] * (0.28099201947014130 + (0.23921137841068607 * applyconvR)));
					tempSampleR -= (cabs->bR[9] * (0.10333290012666248 + (0.35058962687321482 * applyconvR)));
					tempSampleR -= (cabs->bR[10] * (0.06605032198166226 + (0.23447405567823365 * applyconvR)));
					tempSampleR -= (cabs->bR[11] * (0.10485808661261729 + (0.05025985449763527 * applyconvR)));
					tempSampleR -= (cabs->bR[12] * (0.13231190973014911 - (0.05484648240248013 * applyconvR)));
					tempSampleR -= (cabs->bR[13] * (0.12926184767180304 - (0.04054223744746116 * applyconvR)));
					tempSampleR -= (cabs->bR[14] * (0.13802696739839460 + (0.01876754906568237 * applyconvR)));
					tempSampleR -= (cabs->bR[15] * (0.16548980700926913 + (0.06772130758771169 * applyconvR)));
					tempSampleR -= (cabs->bR[16] * (0.14469310965751475 + (0.10590928840978781 * applyconvR)));
					tempSampleR -= (cabs->bR[17] * (0.07838457396093310 + (0.13120101199677947 * applyconvR)));
					tempSampleR -= (cabs->bR[18] * (0.05123031606187391 + (0.13883400806512292 * applyconvR)));
					tempSampleR -= (cabs->bR[19] * (0.08906103481939850 + (0.07840461228402337 * applyconvR)));
					tempSampleR -= (cabs->bR[20] * (0.13939265522625241 + (0.01194366471800457 * applyconvR)));
					tempSampleR -= (cabs->bR[21] * (0.14957600717294034 + (0.07687598594361914 * applyconvR)));
					tempSampleR -= (cabs->bR[22] * (0.14112708654047090 + (0.20118461131186977 * applyconvR)));
					tempSampleR -= (cabs->bR[23] * (0.14961020766492997 + (0.30005716443826147 * applyconvR)));
					tempSampleR -= (cabs->bR[24] * (0.16130382224652270 + (0.40459872030013055 * applyconvR)));
					tempSampleR -= (cabs->bR[25] * (0.15679868471080052 + (0.47292767226083465 * applyconvR)));
					tempSampleR -= (cabs->bR[26] * (0.16456530552807727 + (0.45182121471666481 * applyconvR)));
					tempSampleR -= (cabs->bR[27] * (0.16852385701909278 + (0.38272684270752266 * applyconvR)));
					tempSampleR -= (cabs->bR[28] * (0.13317562760966850 + (0.28829580273670768 * applyconvR)));
					tempSampleR -= (cabs->bR[29] * (0.09396196532150952 + (0.18886898332071317 * applyconvR)));
					tempSampleR -= (cabs->bR[30] * (0.10133496956734221 + (0.11158788414137354 * applyconvR)));
					tempSampleR -= (cabs->bR[31] * (0.16097596389376778 + (0.02621299102374547 * applyconvR)));
					tempSampleR -= (cabs->bR[32] * (0.21419006394821866 - (0.03585678078834797 * applyconvR)));
					tempSampleR -= (cabs->bR[33] * (0.21273234570555244 - (0.02574469802924526 * applyconvR)));
					tempSampleR -= (cabs->bR[34] * (0.16934948798707830 + (0.01354331184333835 * applyconvR)));
					tempSampleR -= (cabs->bR[35] * (0.11970436472852493 + (0.04242183865883427 * applyconvR)));
					tempSampleR -= (cabs->bR[36] * (0.09329023656747724 + (0.06890873292358397 * applyconvR)));
					tempSampleR -= (cabs->bR[37] * (0.10255328436608116 + (0.11482972519137427 * applyconvR)));
					tempSampleR -= (cabs->bR[38] * (0.13883223352796811 + (0.18016014431438840 * applyconvR)));
					tempSampleR -= (cabs->bR[39] * (0.16532844286979087 + (0.24521957638633446 * applyconvR)));
					tempSampleR -= (cabs->bR[40] * (0.16254607738965438 + (0.25669472097572482 * applyconvR)));
					tempSampleR -= (cabs->bR[41] * (0.15353207135544752 + (0.15048064682912729 * applyconvR)));
					tempSampleR -= (cabs->bR[42] * (0.13039046390746015 - (0.00200335414623601 * applyconvR)));
					tempSampleR -= (cabs->bR[43] * (0.06707245032180627 - (0.06498125592578702 * applyconvR)));
					tempSampleR += (cabs->bR[44] * (0.01427326441869788 + (0.01940451360783622 * applyconvR)));
					tempSampleR += (cabs->bR[45] * (0.06151238306578224 - (0.07335755969763329 * applyconvR)));
					tempSampleR += (cabs->bR[46] * (0.04685840498892526 - (0.14258849371688248 * applyconvR)));
					tempSampleR -= (cabs->bR[47] * (0.00950136304466093 + (0.14379354707665129 * applyconvR)));
					tempSampleR -= (cabs->bR[48] * (0.06245771575493557 + (0.07639718586346110 * applyconvR)));
					tempSampleR -= (cabs->bR[49] * (0.07159593175777741 - (0.00595536565276915 * applyconvR)));
					tempSampleR -= (cabs->bR[50] * (0.03167929390245019 - (0.03856769526301793 * applyconvR)));
					tempSampleR += (cabs->bR[51] * (0.01890898565110766 + (0.00760539424271147 * applyconvR)));
					tempSampleR += (cabs->bR[52] * (0.04926161137832240 - (0.06411014430053390 * applyconvR)));
					tempSampleR += (cabs->bR[53] * (0.05768814623421683 - (0.15068618173358578 * applyconvR)));
					tempSampleR += (cabs->bR[54] * (0.06144258297076708 - (0.21200636329120301 * applyconvR)));
					tempSampleR += (cabs->bR[55] * (0.06348341960185613 - (0.19620269813094307 * applyconvR)));
					tempSampleR += (cabs->bR[56] * (0.04877736350310589 - (0.11864999881200111 * applyconvR)));
					tempSampleR += (cabs->bR[57] * (0.01010950997574472 - (0.02630070679113791 * applyconvR)));
					tempSampleR -= (cabs->bR[58] * (0.02929178864801191 - (0.04439260202207482 * applyconvR)));
					tempSampleR -= (cabs->bR[59] * (0.03484517126321562 - (0.04508635396034735 * applyconvR)));
					tempSampleR -= (cabs->bR[60] * (0.00547176780437610 - (0.00205637806941426 * applyconvR)));
					tempSampleR += (cabs->bR[61] * (0.02278296865283977 - (0.00063732526427685 * applyconvR)));
					tempSampleR += (cabs->bR[62] * (0.02688982591366477 + (0.05333738901586284 * applyconvR)));
					tempSampleR += (cabs->bR[63] * (0.01942012754957055 + (0.10942832669749143 * applyconvR)));
					tempSampleR += (cabs->bR[64] * (0.01572585258756565 + (0.11189204189054594 * applyconvR)));
					tempSampleR += (cabs->bR[65] * (0.01490550715016034 + (0.04449822818925343 * applyconvR)));
					tempSampleR += (cabs->bR[66] * (0.01715683226376727 - (0.06944648050933899 * applyconvR)));
					tempSampleR += (cabs->bR[67] * (0.02822659878011318 - (0.17843652160132820 * applyconvR)));
					tempSampleR += (cabs->bR[68] * (0.03758307610456144 - (0.21986013433664692 * applyconvR)));
					tempSampleR += (cabs->bR[69] * (0.03275008021608433 - (0.15869878676112170 * applyconvR)));
					tempSampleR += (cabs->bR[70] * (0.01855749786752354 - (0.02337224995718105 * applyconvR)));
					tempSampleR += (cabs->bR[71] * (0.00217095395782931 + (0.10971764224593601 * applyconvR)));
					tempSampleR -= (cabs->bR[72] * (0.01851381451105007 - (0.17214910008793413 * applyconvR)));
					tempSampleR -= (cabs->bR[73] * (0.04722574936345419 - (0.14341588977845254 * applyconvR)));
					tempSampleR -= (cabs->bR[74] * (0.07151540514482006 - (0.04684695724814321 * applyconvR)));
					tempSampleR -= (cabs->bR[75] * (0.06827195484995092 + (0.07022207121861397 * applyconvR)));
					tempSampleR -= (cabs->bR[76] * (0.03290227240464227 + (0.16328400808152735 * applyconvR)));
					tempSampleR += (cabs->bR[77] * (0.01043861198275382 - (0.20184486126076279 * applyconvR)));
					tempSampleR += (cabs->bR[78] * (0.03236563559476477 - (0.17125821306380920 * applyconvR)));
					tempSampleR += (cabs->bR[79] * (0.02040121529932702 - (0.09103660189829657 * applyconvR)));
					tempSampleR -= (cabs->bR[80] * (0.00509649513318102 + (0.01170360991547489 * applyconvR)));
					tempSampleR -= (cabs->bR[81] * (0.01388353426600228 - (0.03588955538451771 * applyconvR)));
					tempSampleR -= (cabs->bR[82] * (0.00523671715033842 - (0.07068798057534148 * applyconvR)));
					tempSampleR += (cabs->bR[83] * (0.00665852487721137 + (0.11666210640054926 * applyconvR)));
					tempSampleR += (cabs->bR[84] * (0.01593540832939290 + (0.15844892856402149 * applyconvR)));
					tempSampleR += (cabs->bR[85] * (0.02080509201836796 + (0.17186274420065850 * applyconvR)));
					// end BoutiqueStack conv
					break;
				case 4:
					// begin LargeCombo conv L
					tempSampleL += (cabs->bL[1] * (1.31819680801404560 + (0.00362521700518292 * applyconvL)));
					tempSampleL += (cabs->bL[2] * (1.37738284126127919 + (0.14134596126256205 * applyconvL)));
					tempSampleL += (cabs->bL[3] * (1.09957637225311622 + (0.33199581815501555 * applyconvL)));
					tempSampleL += (cabs->bL[4] * (0.62025358899656258 + (0.37476042042088142 * applyconvL)));
					tempSampleL += (cabs->bL[5] * (0.12926194402137478 + (0.24702655568406759 * applyconvL)));
					tempSampleL -= (cabs->bL[6] * (0.28927985011367602 - (0.13289168298307708 * applyconvL)));
					tempSampleL -= (cabs->bL[7] * (0.56518146339033448 - (0.11026641793526121 * applyconvL)));
					tempSampleL -= (cabs->bL[8] * (0.59843200696815069 - (0.10139909232154271 * applyconvL)));
					tempSampleL -= (cabs->bL[9] * (0.45219971861789204 - (0.13313355255903159 * applyconvL)));
					tempSampleL -= (cabs->bL[10] * (0.32520490032331351 - (0.29009061730364216 * applyconvL)));
					tempSampleL -= (cabs->bL[11] * (0.29773131872442909 - (0.45307495356996669 * applyconvL)));
					tempSampleL -= (cabs->bL[12] * (0.31738895975218867 - (0.43198591958928922 * applyconvL)));
					tempSampleL -= (cabs->bL[13] * (0.33336150604703757 - (0.24240412850274029 * applyconvL)));
					tempSampleL -= (cabs->bL[14] * (0.32461638442042151 - (0.02779297492397464 * applyconvL)));
					tempSampleL -= (cabs->bL[15] * (0.27812829473787770 + (0.15565718905032455 * applyconvL)));
					tempSampleL -= (cabs->bL[16] * (0.19413454458668097 + (0.32087693535188599 * applyconvL)));
					tempSampleL -= (cabs->bL[17] * (0.12378036344480114 + (0.37736575956794161 * applyconvL)));
					tempSampleL -= (cabs->bL[18] * (0.12550494837257106 + (0.25593811142722300 * applyconvL)));
					tempSampleL -= (cabs->bL[19] * (0.17725736033713696 + (0.07708896413593636 * applyconvL)));
					tempSampleL -= (cabs->bL[20] * (0.22023699647700670 - (0.01600371273599124 * applyconvL)));
					tempSampleL -= (cabs->bL[21] * (0.21987645486953747 + (0.00973336938352798 * applyconvL)));
					tempSampleL -= (cabs->bL[22] * (0.15014276479707978 + (0.11602269600138954 * applyconvL)));
					tempSampleL -= (cabs->bL[23] * (0.05176520203073560 + (0.20383164255692698 * applyconvL)));
					tempSampleL -= (cabs->bL[24] * (0.04276687165294867 + (0.17785002166834518 * applyconvL)));
					tempSampleL -= (cabs->bL[25] * (0.15951546388137597 + (0.06748854885822464 * applyconvL)));
					tempSampleL -= (cabs->bL[26] * (0.30211952144352616 - (0.03440494237025149 * applyconvL)));
					tempSampleL -= (cabs->bL[27] * (0.36462803375134506 - (0.05874284362202409 * applyconvL)));
					tempSampleL -= (cabs->bL[28] * (0.32283960219377539 + (0.01189623197958362 * applyconvL)));
					tempSampleL -= (cabs->bL[29] * (0.19245178663350720 + (0.11088858383712991 * applyconvL)));
					tempSampleL += (cabs->bL[30] * (0.00681589563349590 - (0.16314250963457660 * applyconvL)));
					tempSampleL += (cabs->bL[31] * (0.20927798345622584 - (0.16952981620487462 * applyconvL)));
					tempSampleL += (cabs->bL[32] * (0.25638846543430976 - (0.11462562122281153 * applyconvL)));
					tempSampleL += (cabs->bL[33] * (0.04584495673888605 + (0.04669671229804190 * applyconvL)));
					tempSampleL -= (cabs->bL[34] * (0.25221561978187662 - (0.19250758741703761 * applyconvL)));
					tempSampleL -= (cabs->bL[35] * (0.35662801992424953 - (0.12244680002787561 * applyconvL)));
					tempSampleL -= (cabs->bL[36] * (0.21498114329314663 + (0.12152120956991189 * applyconvL)));
					tempSampleL += (cabs->bL[37] * (0.00968605571673376 - (0.30597812512858558 * applyconvL)));
					tempSampleL += (cabs->bL[38] * (0.18029119870614621 - (0.31569386468576782 * applyconvL)));
					tempSampleL += (cabs->bL[39] * (0.22744437185251629 - (0.18028438460422197 * applyconvL)));
					tempSampleL += (cabs->bL[40] * (0.09725687638959078 + (0.05479918522830433 * applyconvL)));
					tempSampleL -= (cabs->bL[41] * (0.17970389267353537 - (0.29222750363124067 * applyconvL)));
					tempSampleL -= (cabs->bL[42] * (0.42371969704763018 - (0.34924957781842314 * applyconvL)));
					tempSampleL -= (cabs->bL[43] * (0.43313266755788055 - (0.11503731970288061 * applyconvL)));
					tempSampleL -= (cabs->bL[44] * (0.22178165627851801 + (0.25002925550036226 * applyconvL)));
					tempSampleL -= (cabs->bL[45] * (0.00410198176852576 + (0.43283281457037676 * applyconvL)));
					tempSampleL += (cabs->bL[46] * (0.09072426344812032 - (0.35318250460706446 * applyconvL)));
					tempSampleL += (cabs->bL[47] * (0.08405839183965140 - (0.16936391987143717 * applyconvL)));
					tempSampleL -= (cabs->bL[48] * (0.01110419756114383 - (0.01247164991313877 * applyconvL)));
					tempSampleL -= (cabs->bL[49] * (0.18593334646855278 - (0.14513260199423966 * applyconvL)));
					tempSampleL -= (cabs->bL[50] * (0.33665010871497486 - (0.14456206192169668 * applyconvL)));
					tempSampleL -= (cabs->bL[51] * (0.32644968491439380 + (0.01594380759082303 * applyconvL)));
					tempSampleL -= (cabs->bL[52] * (0.14855437679485431 + (0.23555511219002742 * applyconvL)));
					tempSampleL += (cabs->bL[53] * (0.05113019250820622 - (0.35556617126595202 * applyconvL)));
					tempSampleL += (cabs->bL[54] * (0.12915754942362243 - (0.28571671825750300 * applyconvL)));
					tempSampleL += (cabs->bL[55] * (0.07406865846069306 - (0.10543886479975995 * applyconvL)));
					tempSampleL -= (cabs->bL[56] * (0.03669573814193980 - (0.03194267657582078 * applyconvL)));
					tempSampleL -= (cabs->bL[57] * (0.13429103278009327 - (0.06145796486786051 * applyconvL)));
					tempSampleL -= (cabs->bL[58] * (0.17884021749974641 - (0.00156626902982124 * applyconvL)));
					tempSampleL -= (cabs->bL[59] * (0.16138212225178239 + (0.09402070836837134 * applyconvL)));
					tempSampleL -= (cabs->bL[60] * (0.10867028245257521 + (0.15407963447815898 * applyconvL)));
					tempSampleL -= (cabs->bL[61] * (0.06312416389213464 + (0.11241095544179526 * applyconvL)));
					tempSampleL -= (cabs->bL[62] * (0.05826376574081994 - (0.03635253545701986 * applyconvL)));
					tempSampleL -= (cabs->bL[63] * (0.07991631148258237 - (0.18041947557579863 * applyconvL)));
					tempSampleL -= (cabs->bL[64] * (0.07777397532506500 - (0.20817156738202205 * applyconvL)));
					tempSampleL -= (cabs->bL[65] * (0.03812528734394271 - (0.13679963125162486 * applyconvL)));
					tempSampleL += (cabs->bL[66] * (0.00204900323943951 + (0.04009000730101046 * applyconvL)));
					tempSampleL += (cabs->bL[67] * (0.01779599498119764 - (0.04218637577942354 * applyconvL)));
					tempSampleL += (cabs->bL[68] * (0.00950301949319113 - (0.07908911305044238 * applyconvL)));
					tempSampleL -= (cabs->bL[69] * (0.04283600714814891 + (0.02716262334097985 * applyconvL)));
					tempSampleL -= (cabs->bL[70] * (0.14478320837041933 - (0.08823515277628832 * applyconvL)));
					tempSampleL -= (cabs->bL[71] * (0.23250267035795688 - (0.15334197814956568 * applyconvL)));
					tempSampleL -= (cabs->bL[72] * (0.22369031446225857 - (0.08550989980799503 * applyconvL)));
					tempSampleL -= (cabs->bL[73] * (0.11142757883989868 + (0.08321482928259660 * applyconvL)));
					tempSampleL += (cabs->bL[74] * (0.02752318631713307 - (0.25252906099212968 * applyconvL)));
					tempSampleL += (cabs->bL[75] * (0.11940028414727490 - (0.34358127205009553 * applyconvL)));
					tempSampleL += (cabs->bL[76] * (0.12702057126698307 - (0.31808560130583663 * applyconvL)));
					tempSampleL += (cabs->bL[77] * (0.03639067777025356 - (0.17970282734717846 * applyconvL)));
					tempSampleL -= (cabs->bL[78] * (0.11389848143835518 + (0.00470616711331971 * applyconvL)));
					tempSampleL -= (cabs->bL[79] * (0.23024072979374310 - (0.09772245468884058 * applyconvL)));
					tempSampleL -= (cabs->bL[80] * (0.24389015061112601 - (0.09600959885615798 * applyconvL)));
					tempSampleL -= (cabs->bL[81] * (0.16680269075295703 - (0.05194978963662898 * applyconvL)));
					tempSampleL -= (cabs->bL[82] * (0.05108041495077725 - (0.01796071525570735 * applyconvL)));
					tempSampleL += (cabs->bL[83] * (0.06489835353859555 - (0.00808013770331126 * applyconvL)));
					tempSampleL += (cabs->bL[84] * (0.15481511440233464 - (0.02674063848284838 * applyconvL)));
					tempSampleL += (cabs->bL[85] * (0.18620867857907253 - (0.01786423699465214 * applyconvL)));
					tempSampleL += (cabs->bL[86] * (0.13879832139055756 + (0.01584446839973597 * applyconvL)));
					tempSampleL += (cabs->bL[87] * (0.04878235109120615 + (0.02962866516075816 * applyconvL)));
					// begin LargeCombo conv R
					tempSampleR += (cabs->bR[1] * (1.31819680801404560 + (0.00362521700518292 * applyconvR)));
					tempSampleR += (cabs->bR[2] * (1.37738284126127919 + (0.14134596126256205 * applyconvR)));
					tempSampleR += (cabs->bR[3] * (1.09957637225311622 + (0.33199581815501555 * applyconvR)));
					tempSampleR += (cabs->bR[4] * (0.62025358899656258 + (0.37476042042088142 * applyconvR)));
					tempSampleR += (cabs->bR[5] * (0.12926194402137478 + (0.24702655568406759 * applyconvR)));
					tempSampleR -= (cabs->bR[6] * (0.28927985011367602 - (0.13289168298307708 * applyconvR)));
					tempSampleR -= (cabs->bR[7] * (0.56518146339033448 - (0.11026641793526121 * applyconvR)));
					tempSampleR -= (cabs->bR[8] * (0.59843200696815069 - (0.10139909232154271 * applyconvR)));
					tempSampleR -= (cabs->bR[9] * (0.45219971861789204 - (0.13313355255903159 * applyconvR)));
					tempSampleR -= (cabs->bR[10] * (0.32520490032331351 - (0.29009061730364216 * applyconvR)));
					tempSampleR -= (cabs->bR[11] * (0.29773131872442909 - (0.45307495356996669 * applyconvR)));
					tempSampleR -= (cabs->bR[12] * (0.31738895975218867 - (0.43198591958928922 * applyconvR)));
					tempSampleR -= (cabs->bR[13] * (0.33336150604703757 - (0.24240412850274029 * applyconvR)));
					tempSampleR -= (cabs->bR[14] * (0.32461638442042151 - (0.02779297492397464 * applyconvR)));
					tempSampleR -= (cabs->bR[15] * (0.27812829473787770 + (0.15565718905032455 * applyconvR)));
					tempSampleR -= (cabs->bR[16] * (0.19413454458668097 + (0.32087693535188599 * applyconvR)));
					tempSampleR -= (cabs->bR[17] * (0.12378036344480114 + (0.37736575956794161 * applyconvR)));
					tempSampleR -= (cabs->bR[18] * (0.12550494837257106 + (0.25593811142722300 * applyconvR)));
					tempSampleR -= (cabs->bR[19] * (0.17725736033713696 + (0.07708896413593636 * applyconvR)));
					tempSampleR -= (cabs->bR[20] * (0.22023699647700670 - (0.01600371273599124 * applyconvR)));
					tempSampleR -= (cabs->bR[21] * (0.21987645486953747 + (0.00973336938352798 * applyconvR)));
					tempSampleR -= (cabs->bR[22] * (0.15014276479707978 + (0.11602269600138954 * applyconvR)));
					tempSampleR -= (cabs->bR[23] * (0.05176520203073560 + (0.20383164255692698 * applyconvR)));
					tempSampleR -= (cabs->bR[24] * (0.04276687165294867 + (0.17785002166834518 * applyconvR)));
					tempSampleR -= (cabs->bR[25] * (0.15951546388137597 + (0.06748854885822464 * applyconvR)));
					tempSampleR -= (cabs->bR[26] * (0.30211952144352616 - (0.03440494237025149 * applyconvR)));
					tempSampleR -= (cabs->bR[27] * (0.36462803375134506 - (0.05874284362202409 * applyconvR)));
					tempSampleR -= (cabs->bR[28] * (0.32283960219377539 + (0.01189623197958362 * applyconvR)));
					tempSampleR -= (cabs->bR[29] * (0.19245178663350720 + (0.11088858383712991 * applyconvR)));
					tempSampleR += (cabs->bR[30] * (0.00681589563349590 - (0.16314250963457660 * applyconvR)));
					tempSampleR += (cabs->bR[31] * (0.20927798345622584 - (0.16952981620487462 * applyconvR)));
					tempSampleR += (cabs->bR[32] * (0.25638846543430976 - (0.11462562122281153 * applyconvR)));
					tempSampleR += (cabs->bR[33] * (0.04584495673888605 + (0.04669671229804190 * applyconvR)));
					tempSampleR -= (cabs->bR[34] * (0.25221561978187662 - (0.19250758741703761 * applyconvR)));
					tempSampleR -= (cabs->bR[35] * (0.35662801992424953 - (0.12244680002787561 * applyconvR)));
					tempSampleR -= (cabs->bR[36] * (0.21498114329314663 + (0.12152120956991189 * applyconvR)));
					tempSampleR += (cabs->bR[37] * (0.00968605571673376 - (0.30597812512858558 * applyconvR)));
					tempSampleR += (cabs->bR[38] * (0.18029119870614621 - (0.31569386468576782 * applyconvR)));
					tempSampleR += (cabs->bR[39] * (0.22744437185251629 - (0.18028438460422197 * applyconvR)));
					tempSampleR += (cabs->bR[40] * (0.09725687638959078 + (0.05479918522830433 * applyconvR)));
					tempSampleR -= (cabs->bR[41] * (0.17970389267353537 - (0.29222750363124067 * applyconvR)));
					tempSampleR -= (cabs->bR[42] * (0.42371969704763018 - (0.34924957781842314 * applyconvR)));
					tempSampleR -= (cabs->bR[43] * (0.43313266755788055 - (0.11503731970288061 * applyconvR)));
					tempSampleR -= (cabs->bR[44] * (0.22178165627851801 + (0.25002925550036226 * applyconvR)));
					tempSampleR -= (cabs->bR[45] * (0.00410198176852576 + (0.43283281457037676 * applyconvR)));
					tempSampleR += (cabs->bR[46] * (0.09072426344812032 - (0.35318250460706446 * applyconvR)));
					tempSampleR += (cabs->bR[47] * (0.08405839183965140 - (0.16936391987143717 * applyconvR)));
					tempSampleR -= (cabs->bR[48] * (0.01110419756114383 - (0.01247164991313877 * applyconvR)));
					tempSampleR -= (cabs->bR[49] * (0.18593334646855278 - (0.14513260199423966 * applyconvR)));
					tempSampleR -= (cabs->bR[50] * (0.33665010871497486 - (0.14456206192169668 * applyconvR)));
					tempSampleR -= (cabs->bR[51] * (0.32644968491439380 + (0.01594380759082303 * applyconvR)));
					tempSampleR -= (cabs->bR[52] * (0.14855437679485431 + (0.23555511219002742 * applyconvR)));
					tempSampleR += (cabs->bR[53] * (0.05113019250820622 - (0.35556617126595202 * applyconvR)));
					tempSampleR += (cabs->bR[54] * (0.12915754942362243 - (0.28571671825750300 * applyconvR)));
					tempSampleR += (cabs->bR[55] * (0.07406865846069306 - (0.10543886479975995 * applyconvR)));
					tempSampleR -= (cabs->bR[56] * (0.03669573814193980 - (0.03194267657582078 * applyconvR)));
					tempSampleR -= (cabs->bR[57] * (0.13429103278009327 - (0.06145796486786051 * applyconvR)));
					tempSampleR -= (cabs->bR[58] * (0.17884021749974641 - (0.00156626902982124 * applyconvR)));
					tempSampleR -= (cabs->bR[59] * (0.16138212225178239 + (0.09402070836837134 * applyconvR)));
					tempSampleR -= (cabs->bR[60] * (0.10867028245257521 + (0.15407963447815898 * applyconvR)));
					tempSampleR -= (cabs->bR[61] * (0.06312416389213464 + (0.11241095544179526 * applyconvR)));
					tempSampleR -= (cabs->bR[62] * (0.05826376574081994 - (0.03635253545701986 * applyconvR)));
					tempSampleR -= (cabs->bR[63] * (0.07991631148258237 - (0.18041947557579863 * applyconvR)));
					tempSampleR -= (cabs->bR[64] * (0.07777397532506500 - (0.20817156738202205 * applyconvR)));
					tempSampleR -= (cabs->bR[65] * (0.03812528734394271 - (0.13679963125162486 * applyconvR)));
					tempSampleR += (cabs->bR[66] * (0.00204900323943951 + (0.04009000730101046 * applyconvR)));
					tempSampleR += (cabs->bR[67] * (0.01779599498119764 - (0.04218637577942354 * applyconvR)));
					tempSampleR += (cabs->bR[68] * (0.00950301949319113 - (0.07908911305044238 * applyconvR)));
					tempSampleR -= (cabs->bR[69] * (0.04283600714814891 + (0.02716262334097985 * applyconvR)));
					tempSampleR -= (cabs->bR[70] * (0.14478320837041933 - (0.08823515277628832 * applyconvR)));
					tempSampleR -= (cabs->bR[71] * (0.23250267035795688 - (0.15334197814956568 * applyconvR)));
					tempSampleR -= (cabs->bR[72] * (0.22369031446225857 - (0.08550989980799503 * applyconvR)));
					tempSampleR -= (cabs->bR[73] * (0.11142757883989868 + (0.08321482928259660 * applyconvR)));
					tempSampleR += (cabs->bR[74] * (0.02752318631713307 - (0.25252906099212968 * applyconvR)));
					tempSampleR += (cabs->bR[75] * (0.11940028414727490 - (0.34358127205009553 * applyconvR)));
					tempSampleR += (cabs->bR[76] * (0.12702057126698307 - (0.31808560130583663 * applyconvR)));
					tempSampleR += (cabs->bR[77] * (0.03639067777025356 - (0.17970282734717846 * applyconvR)));
					tempSampleR -= (cabs->bR[78] * (0.11389848143835518 + (0.00470616711331971 * applyconvR)));
					tempSampleR -= (cabs->bR[79] * (0.23024072979374310 - (0.09772245468884058 * applyconvR)));
					tempSampleR -= (cabs->bR[80] * (0.24389015061112601 - (0.09600959885615798 * applyconvR)));
					tempSampleR -= (cabs->bR[81] * (0.16680269075295703 - (0.05194978963662898 * applyconvR)));
					tempSampleR -= (cabs->bR[82] * (0.05108041495077725 - (0.01796071525570735 * applyconvR)));
					tempSampleR += (cabs->bR[83] * (0.06489835353859555 - (0.00808013770331126 * applyconvR)));
					tempSampleR += (cabs->bR[84] * (0.15481511440233464 - (0.02674063848284838 * applyconvR)));
					tempSampleR += (cabs->bR[85] * (0.18620867857907253 - (0.01786423699465214 * applyconvR)));
					tempSampleR += (cabs->bR[86] * (0.13879832139055756 + (0.01584446839973597 * applyconvR)));
					tempSampleR += (cabs->bR[87] * (0.04878235109120615 + (0.02962866516075816 * applyconvR)));
					// end LargeCombo conv
					break;
				case 5:
					// begin SmallCombo conv L
					tempSampleL += (cabs->bL[1] * (1.42133070619855229 - (0.18270903813104500 * applyconvL)));
					tempSampleL += (cabs->bL[2] * (1.47209686171873821 - (0.27954009590498585 * applyconvL)));
					tempSampleL += (cabs->bL[3] * (1.34648011331265294 - (0.47178343556301960 * applyconvL)));
					tempSampleL += (cabs->bL[4] * (0.82133804036124580 - (0.41060189990353935 * applyconvL)));
					tempSampleL += (cabs->bL[5] * (0.21628057120466901 - (0.26062442734317454 * applyconvL)));
					tempSampleL -= (cabs->bL[6] * (0.30306716082877883 + (0.10067648425439185 * applyconvL)));
					tempSampleL -= (cabs->bL[7] * (0.69484313178531876 - (0.09655574841702286 * applyconvL)));
					tempSampleL -= (cabs->bL[8] * (0.88320822356980833 - (0.26501644327144314 * applyconvL)));
					tempSampleL -= (cabs->bL[9] * (0.81326147029423723 - (0.31115926837054075 * applyconvL)));
					tempSampleL -= (cabs->bL[10] * (0.56728759049069222 - (0.23304233545561287 * applyconvL)));
					tempSampleL -= (cabs->bL[11] * (0.33340326645198737 - (0.12361361388240180 * applyconvL)));
					tempSampleL -= (cabs->bL[12] * (0.20280263733605616 - (0.03531960962500105 * applyconvL)));
					tempSampleL -= (cabs->bL[13] * (0.15864533788751345 + (0.00355160825317868 * applyconvL)));
					tempSampleL -= (cabs->bL[14] * (0.12544767480555119 + (0.01979010423176500 * applyconvL)));
					tempSampleL -= (cabs->bL[15] * (0.06666788902658917 + (0.00188830739903378 * applyconvL)));
					tempSampleL += (cabs->bL[16] * (0.02977793355081072 + (0.02304216615605394 * applyconvL)));
					tempSampleL += (cabs->bL[17] * (0.12821526330916558 + (0.02636238376777800 * applyconvL)));
					tempSampleL += (cabs->bL[18] * (0.19933812710210136 - (0.02932657234709721 * applyconvL)));
					tempSampleL += (cabs->bL[19] * (0.18346460191225772 - (0.12859581955080629 * applyconvL)));
					tempSampleL -= (cabs->bL[20] * (0.00088697526755385 + (0.15855257539277415 * applyconvL)));
					tempSampleL -= (cabs->bL[21] * (0.28904286712096761 + (0.06226286786982616 * applyconvL)));
					tempSampleL -= (cabs->bL[22] * (0.49133546282552537 - (0.06512851581813534 * applyconvL)));
					tempSampleL -= (cabs->bL[23] * (0.52908013030763046 - (0.13606992188523465 * applyconvL)));
					tempSampleL -= (cabs->bL[24] * (0.45897241332311706 - (0.15527194946346906 * applyconvL)));
					tempSampleL -= (cabs->bL[25] * (0.35535938629924352 - (0.13634771941703441 * applyconvL)));
					tempSampleL -= (cabs->bL[26] * (0.26185269405237693 - (0.08736651482771546 * applyconvL)));
					tempSampleL -= (cabs->bL[27] * (0.19997351944186473 - (0.01714565029656306 * applyconvL)));
					tempSampleL -= (cabs->bL[28] * (0.18894054145105646 + (0.04557612705740050 * applyconvL)));
					tempSampleL -= (cabs->bL[29] * (0.24043993691153928 + (0.05267500387081067 * applyconvL)));
					tempSampleL -= (cabs->bL[30] * (0.29191852873822671 + (0.01922151122971644 * applyconvL)));
					tempSampleL -= (cabs->bL[31] * (0.29399783430587761 - (0.02238952856106585 * applyconvL)));
					tempSampleL -= (cabs->bL[32] * (0.26662219155294159 - (0.07760819463416335 * applyconvL)));
					tempSampleL -= (cabs->bL[33] * (0.20881206667122221 - (0.11930017354479640 * applyconvL)));
					tempSampleL -= (cabs->bL[34] * (0.12916658879944876 - (0.11798638949823513 * applyconvL)));
					tempSampleL -= (cabs->bL[35] * (0.07678815166012012 - (0.06826864734598684 * applyconvL)));
					tempSampleL -= (cabs->bL[36] * (0.08568505484529348 - (0.00510459741104792 * applyconvL)));
					tempSampleL -= (cabs->bL[37] * (0.13613615872486634 + (0.02288223583971244 * applyconvL)));
					tempSampleL -= (cabs->bL[38] * (0.17426657494209266 + (0.02723737220296440 * applyconvL)));
					tempSampleL -= (cabs->bL[39] * (0.17343619261009030 + (0.01412920547179825 * applyconvL)));
					tempSampleL -= (cabs->bL[40] * (0.14548368977428555 - (0.02640418940455951 * applyconvL)));
					tempSampleL -= (cabs->bL[41] * (0.10485295885802372 - (0.06334665781931498 * applyconvL)));
					tempSampleL -= (cabs->bL[42] * (0.06632268974717079 - (0.05960343688612868 * applyconvL)));
					tempSampleL -= (cabs->bL[43] * (0.06915692039882040 - (0.03541337869596061 * applyconvL)));
					tempSampleL -= (cabs->bL[44] * (0.11889611687783583 - (0.02250608307287119 * applyconvL)));
					tempSampleL -= (cabs->bL[45] * (0.14598456370320673 + (0.00280345943128246 * applyconvL)));
					tempSampleL -= (cabs->bL[46] * (0.12312084125613143 + (0.04947283933434576 * applyconvL)));
					tempSampleL -= (cabs->bL[47] * (0.11379940289994711 + (0.06590080966570636 * applyconvL)));
					tempSampleL -= (cabs->bL[48] * (0.12963290754003182 + (0.02597647654256477 * applyconvL)));
					tempSampleL -= (cabs->bL[49] * (0.12723837402978638 - (0.04942071966927938 * applyconvL)));
					tempSampleL -= (cabs->bL[50] * (0.09185015882996231 - (0.10420810015956679 * applyconvL)));
					tempSampleL -= (cabs->bL[51] * (0.04011592913036545 - (0.10234174227772008 * applyconvL)));
					tempSampleL += (cabs->bL[52] * (0.00992597785057113 + (0.05674042373836896 * applyconvL)));
					tempSampleL += (cabs->bL[53] * (0.04921452178306781 - (0.00222698867111080 * applyconvL)));
					tempSampleL += (cabs->bL[54] * (0.06096504883783566 - (0.04040426549982253 * applyconvL)));
					tempSampleL += (cabs->bL[55] * (0.04113530718724200 - (0.04190143593049960 * applyconvL)));
					tempSampleL += (cabs->bL[56] * (0.01292699017654650 - (0.01121994018532499 * applyconvL)));
					tempSampleL -= (cabs->bL[57] * (0.00437123132431870 - (0.02482497612289103 * applyconvL)));
					tempSampleL -= (cabs->bL[58] * (0.02090571264211918 - (0.03732746039260295 * applyconvL)));
					tempSampleL -= (cabs->bL[59] * (0.04749751678612051 - (0.02960060937328099 * applyconvL)));
					tempSampleL -= (cabs->bL[60] * (0.07675095194206227 - (0.02241927084099648 * applyconvL)));
					tempSampleL -= (cabs->bL[61] * (0.08879414028581609 - (0.01144281133042115 * applyconvL)));
					tempSampleL -= (cabs->bL[62] * (0.07378854974999530 + (0.02518742701599147 * applyconvL)));
					tempSampleL -= (cabs->bL[63] * (0.04677309194488959 + (0.08984657372223502 * applyconvL)));
					tempSampleL -= (cabs->bL[64] * (0.02911874044176449 + (0.14202665940555093 * applyconvL)));
					tempSampleL -= (cabs->bL[65] * (0.02103564720234969 + (0.14640411976171003 * applyconvL)));
					tempSampleL -= (cabs->bL[66] * (0.01940626429101940 + (0.10867274382865903 * applyconvL)));
					tempSampleL -= (cabs->bL[67] * (0.03965401793931531 + (0.04775225375522835 * applyconvL)));
					tempSampleL -= (cabs->bL[68] * (0.08102486457314527 - (0.03204447425666343 * applyconvL)));
					tempSampleL -= (cabs->bL[69] * (0.11794849372825778 - (0.12755667382696789 * applyconvL)));
					tempSampleL -= (cabs->bL[70] * (0.11946469076758266 - (0.20151394599125422 * applyconvL)));
					tempSampleL -= (cabs->bL[71] * (0.07404630324668053 - (0.21300634351769704 * applyconvL)));
					tempSampleL -= (cabs->bL[72] * (0.00477584437144086 - (0.16864707684978708 * applyconvL)));
					tempSampleL += (cabs->bL[73] * (0.05924822014377220 + (0.09394651445109450 * applyconvL)));
					tempSampleL += (cabs->bL[74] * (0.10060989907457370 + (0.00419196431884887 * applyconvL)));
					tempSampleL += (cabs->bL[75] * (0.10817907203844988 - (0.07459664480796091 * applyconvL)));
					tempSampleL += (cabs->bL[76] * (0.08701102204768002 - (0.11129477437630560 * applyconvL)));
					tempSampleL += (cabs->bL[77] * (0.05673785623180162 - (0.10638640242375266 * applyconvL)));
					tempSampleL += (cabs->bL[78] * (0.02944190197442081 - (0.08499792583420167 * applyconvL)));
					tempSampleL += (cabs->bL[79] * (0.01570145445652971 - (0.06190456843465320 * applyconvL)));
					tempSampleL += (cabs->bL[80] * (0.02770233032476748 - (0.04573713136865480 * applyconvL)));
					tempSampleL += (cabs->bL[81] * (0.05417160459175360 - (0.03965651064634598 * applyconvL)));
					tempSampleL += (cabs->bL[82] * (0.06080831637644498 - (0.02909500789113911 * applyconvL)));
					// begin SmallCombo conv R
					tempSampleR += (cabs->bR[1] * (1.42133070619855229 - (0.18270903813104500 * applyconvR)));
					tempSampleR += (cabs->bR[2] * (1.47209686171873821 - (0.27954009590498585 * applyconvR)));
					tempSampleR += (cabs->bR[3] * (1.34648011331265294 - (0.47178343556301960 * applyconvR)));
					tempSampleR += (cabs->bR[4] * (0.82133804036124580 - (0.41060189990353935 * applyconvR)));
					tempSampleR += (cabs->bR[5] * (0.21628057120466901 - (0.26062442734317454 * applyconvR)));
					tempSampleR -= (cabs->bR[6] * (0.30306716082877883 + (0.10067648425439185 * applyconvR)));
					tempSampleR -= (cabs->bR[7] * (0.69484313178531876 - (0.09655574841702286 * applyconvR)));
					tempSampleR -= (cabs->bR[8] * (0.88320822356980833 - (0.26501644327144314 * applyconvR)));
					tempSampleR -= (cabs->bR[9] * (0.81326147029423723 - (0.31115926837054075 * applyconvR)));
					tempSampleR -= (cabs->bR[10] * (0.56728759049069222 - (0.23304233545561287 * applyconvR)));
					tempSampleR -= (cabs->bR[11] * (0.33340326645198737 - (0.12361361388240180 * applyconvR)));
					tempSampleR -= (cabs->bR[12] * (0.20280263733605616 - (0.03531960962500105 * applyconvR)));
					tempSampleR -= (cabs->bR[13] * (0.15864533788751345 + (0.00355160825317868 * applyconvR)));
					tempSampleR -= (cabs->bR[14] * (0.12544767480555119 + (0.01979010423176500 * applyconvR)));
					tempSampleR -= (cabs->bR[15] * (0.06666788902658917 + (0.00188830739903378 * applyconvR)));
					tempSampleR += (cabs->bR[16] * (0.02977793355081072 + (0.02304216615605394 * applyconvR)));
					tempSampleR += (cabs->bR[17] * (0.12821526330916558 + (0.02636238376777800 * applyconvR)));
					tempSampleR += (cabs->bR[18] * (0.19933812710210136 - (0.02932657234709721 * applyconvR)));
					tempSampleR += (cabs->bR[19] * (0.18346460191225772 - (0.12859581955080629 * applyconvR)));
					tempSampleR -= (cabs->bR[20] * (0.00088697526755385 + (0.15855257539277415 * applyconvR)));
					tempSampleR -= (cabs->bR[21] * (0.28904286712096761 + (0.06226286786982616 * applyconvR)));
					tempSampleR -= (cabs->bR[22] * (0.49133546282552537 - (0.06512851581813534 * applyconvR)));
					tempSampleR -= (cabs->bR[23] * (0.52908013030763046 - (0.13606992188523465 * applyconvR)));
					tempSampleR -= (cabs->bR[24] * (0.45897241332311706 - (0.15527194946346906 * applyconvR)));
					tempSampleR -= (cabs->bR[25] * (0.35535938629924352 - (0.13634771941703441 * applyconvR)));
					tempSampleR -= (cabs->bR[26] * (0.26185269405237693 - (0.08736651482771546 * applyconvR)));
					tempSampleR -= (cabs->bR[27] * (0.19997351944186473 - (0.01714565029656306 * applyconvR)));
					tempSampleR -= (cabs->bR[28] * (0.18894054145105646 + (0.04557612705740050 * applyconvR)));
					tempSampleR -= (cabs->bR[29] * (0.24043993691153928 + (0.05267500387081067 * applyconvR)));
					tempSampleR -= (cabs->bR[30] * (0.29191852873822671 + (0.01922151122971644 * applyconvR)));
					tempSampleR -= (cabs->bR[31] * (0.29399783430587761 - (0.02238952856106585 * applyconvR)));
					tempSampleR -= (cabs->bR[32] * (0.26662219155294159 - (0.07760819463416335 * applyconvR)));
					tempSampleR -= (cabs->bR[33] * (0.20881206667122221 - (0.11930017354479640 * applyconvR)));
					tempSampleR -= (cabs->bR[34] * (0.12916658879944876 - (0.11798638949823513 * applyconvR)));
					tempSampleR -= (cabs->bR[35] * (0.07678815166012012 - (0.06826864734598684 * applyconvR)));
					tempSampleR -= (cabs->bR[36] * (0.08568505484529348 - (0.00510459741104792 * applyconvR)));
					tempSampleR -= (cabs->bR[37] * (0.13613615872486634 + (0.02288223583971244 * applyconvR)));
					tempSampleR -= (cabs->bR[38] * (0.17426657494209266 + (0.02723737220296440 * applyconvR)));
					tempSampleR -= (cabs->bR[39] * (0.17343619261009030 + (0.01412920547179825 * applyconvR)));
					tempSampleR -= (cabs->bR[40] * (0.14548368977428555 - (0.02640418940455951 * applyconvR)));
					tempSampleR -= (cabs->bR[41] * (0.10485295885802372 - (0.06334665781931498 * applyconvR)));
					tempSampleR -= (cabs->bR[42] * (0.06632268974717079 - (0.05960343688612868 * applyconvR)));
					tempSampleR -= (cabs->bR[43] * (0.06915692039882040 - (0.03541337869596061 * applyconvR)));
					tempSampleR -= (cabs->bR[44] * (0.11889611687783583 - (0.02250608307287119 * applyconvR)));
					tempSampleR -= (cabs->bR[45] * (0.14598456370320673 + (0.00280345943128246 * applyconvR)));
					tempSampleR -= (cabs->bR[46] * (0.12312084125613143 + (0.04947283933434576 * applyconvR)));
					tempSampleR -= (cabs->bR[47] * (0.11379940289994711 + (0.06590080966570636 * applyconvR)));
					tempSampleR -= (cabs->bR[48] * (0.12963290754003182 + (0.02597647654256477 * applyconvR)));
					tempSampleR -= (cabs->bR[49] * (0.12723837402978638 - (0.04942071966927938 * applyconvR)));
					tempSampleR -= (cabs->bR[50] * (0.09185015882996231 - (0.10420810015956679 * applyconvR)));
					tempSampleR -= (cabs->bR[51] * (0.04011592913036545 - (0.10234174227772008 * applyconvR)));
					tempSampleR += (cabs->bR[52] * (0.00992597785057113 + (0.05674042373836896 * applyconvR)));
					tempSampleR += (cabs->bR[53] * (0.04921452178306781 - (0.00222698867111080 * applyconvR)));
					tempSampleR += (cabs->bR[54] * (0.06096504883783566 - (0.04040426549982253 * applyconvR)));
					tempSampleR += (cabs->bR[55] * (0.04113530718724200 - (0.04190143593049960 * applyconvR)));
					tempSampleR += (cabs->bR[56] * (0.01292699017654650 - (0.01121994018532499 * applyconvR)));
					tempSampleR -= (cabs->bR[57] * (0.00437123132431870 - (0.02482497612289103 * applyconvR)));
					tempSampleR -= (cabs->bR[58] * (0.02090571264211918 - (0.03732746039260295 * applyconvR)));
					tempSampleR -= (cabs->bR[59] * (0.04749751678612051 - (0.02960060937328099 * applyconvR)));
					tempSampleR -= (cabs->bR[60] * (0.07675095194206227 - (0.02241927084099648 * applyconvR)));
					tempSampleR -= (cabs->bR[61] * (0.08879414028581609 - (0.01144281133042115 * applyconvR)));
					tempSampleR -= (cabs->bR[62] * (0.07378854974999530 + (0.02518742701599147 * applyconvR)));
					tempSampleR -= (cabs->bR[63] * (0.04677309194488959 + (0.08984657372223502 * applyconvR)));
					tempSampleR -= (cabs->bR[64] * (0.02911874044176449 + (0.14202665940555093 * applyconvR)));
					tempSampleR -= (cabs->bR[65] * (0.02103564720234969 + (0.14640411976171003 * applyconvR)));
					tempSampleR -= (cabs->bR[66] * (0.01940626429101940 + (0.10867274382865903 * applyconvR)));
					tempSampleR -= (cabs->bR[67] * (0.03965401793931531 + (0.04775225375522835 * applyconvR)));
					tempSampleR -= (cabs->bR[68] * (0.08102486457314527 - (0.03204447425666343 * applyconvR)));
					tempSampleR -= (cabs->bR[69] * (0.11794849372825778 - (0.12755667382696789 * applyconvR)));
					tempSampleR -= (cabs->bR[70] * (0.11946469076758266 - (0.20151394599125422 * applyconvR)));
					tempSampleR -= (cabs->bR[71] * (0.07404630324668053 - (0.21300634351769704 * applyconvR)));
					tempSampleR -= (cabs->bR[72] * (0.00477584437144086 - (0.16864707684978708 * applyconvR)));
					tempSampleR += (cabs->bR[73] * (0.05924822014377220 + (0.09394651445109450 * applyconvR)));
					tempSampleR += (cabs->bR[74] * (0.10060989907457370 + (0.00419196431884887 * applyconvR)));
					tempSampleR += (cabs->bR[75] * (0.10817907203844988 - (0.07459664480796091 * applyconvR)));
					tempSampleR += (cabs->bR[76] * (0.08701102204768002 - (0.11129477437630560 * applyconvR)));
					tempSampleR += (cabs->bR[77] * (0.05673785623180162 - (0.10638640242375266 * applyconvR)));
					tempSampleR += (cabs->bR[78] * (0.02944190197442081 - (0.08499792583420167 * applyconvR)));
					tempSampleR += (cabs->bR[79] * (0.01570145445652971 - (0.06190456843465320 * applyconvR)));
					tempSampleR += (cabs->bR[80] * (0.02770233032476748 - (0.04573713136865480 * applyconvR)));
					tempSampleR += (cabs->bR[81] * (0.05417160459175360 - (0.03965651064634598 * applyconvR)));
					tempSampleR += (cabs->bR[82] * (0.06080831637644498 - (0.02909500789113911 * applyconvR)));
					// end SmallCombo conv
					break;
				case 6:
					// begin Bass conv L
					tempSampleL += (cabs->bL[1] * (1.35472031405494242 + (0.00220914099195157 * applyconvL)));
					tempSampleL += (cabs->bL[2] * (1.63534207755253003 - (0.11406232654509685 * applyconvL)));
					tempSampleL += (cabs->bL[3] * (1.82334575691525869 - (0.42647194712964054 * applyconvL)));
					tempSampleL += (cabs->bL[4] * (1.86156386235405868 - (0.76744187887586590 * applyconvL)));
					tempSampleL += (cabs->bL[5] * (1.67332739338852599 - (0.95161997324293013 * applyconvL)));
					tempSampleL += (cabs->bL[6] * (1.25054130794899021 - (0.98410433514572859 * applyconvL)));
					tempSampleL += (cabs->bL[7] * (0.70049121047281737 - (0.87375612110718992 * applyconvL)));
					tempSampleL += (cabs->bL[8] * (0.15291791448081560 - (0.61195266024519046 * applyconvL)));
					tempSampleL -= (cabs->bL[9] * (0.37301992914152693 + (0.16755422915252094 * applyconvL)));
					tempSampleL -= (cabs->bL[10] * (0.76568539228498433 - (0.28554435228965386 * applyconvL)));
					tempSampleL -= (cabs->bL[11] * (0.95726568749937369 - (0.61659719162806048 * applyconvL)));
					tempSampleL -= (cabs->bL[12] * (1.01273552193911032 - (0.81827288407943954 * applyconvL)));
					tempSampleL -= (cabs->bL[13] * (0.93920108117234447 - (0.80077111864205874 * applyconvL)));
					tempSampleL -= (cabs->bL[14] * (0.79831898832953974 - (0.65814750339694406 * applyconvL)));
					tempSampleL -= (cabs->bL[15] * (0.64200088100452313 - (0.46135833001232618 * applyconvL)));
					tempSampleL -= (cabs->bL[16] * (0.48807302802822128 - (0.15506178974799034 * applyconvL)));
					tempSampleL -= (cabs->bL[17] * (0.36545171501947982 + (0.16126103769376721 * applyconvL)));
					tempSampleL -= (cabs->bL[18] * (0.31469581455759105 + (0.32250870039053953 * applyconvL)));
					tempSampleL -= (cabs->bL[19] * (0.36893534817945800 + (0.25409418897237473 * applyconvL)));
					tempSampleL -= (cabs->bL[20] * (0.41092557722975687 + (0.13114730488878301 * applyconvL)));
					tempSampleL -= (cabs->bL[21] * (0.38584044480710594 + (0.06825323739722661 * applyconvL)));
					tempSampleL -= (cabs->bL[22] * (0.33378434007178670 + (0.04144255489164217 * applyconvL)));
					tempSampleL -= (cabs->bL[23] * (0.26144203061699706 + (0.06031313105098152 * applyconvL)));
					tempSampleL -= (cabs->bL[24] * (0.25818342000920502 + (0.03642289242586355 * applyconvL)));
					tempSampleL -= (cabs->bL[25] * (0.28096018498822661 + (0.00976973667327174 * applyconvL)));
					tempSampleL -= (cabs->bL[26] * (0.25845682019095384 + (0.02749015358080831 * applyconvL)));
					tempSampleL -= (cabs->bL[27] * (0.26655607865953096 - (0.00329839675455690 * applyconvL)));
					tempSampleL -= (cabs->bL[28] * (0.30590085026938518 - (0.07375043215328811 * applyconvL)));
					tempSampleL -= (cabs->bL[29] * (0.32875683916470899 - (0.12454134857516502 * applyconvL)));
					tempSampleL -= (cabs->bL[30] * (0.38166643180506560 - (0.19973911428609989 * applyconvL)));
					tempSampleL -= (cabs->bL[31] * (0.49068186937289598 - (0.34785166842136384 * applyconvL)));
					tempSampleL -= (cabs->bL[32] * (0.60274753867622777 - (0.48685038872711034 * applyconvL)));
					tempSampleL -= (cabs->bL[33] * (0.65944678627090636 - (0.49844657885975518 * applyconvL)));
					tempSampleL -= (cabs->bL[34] * (0.64488955808717285 - (0.40514406499806987 * applyconvL)));
					tempSampleL -= (cabs->bL[35] * (0.55818730353434354 - (0.28029870614987346 * applyconvL)));
					tempSampleL -= (cabs->bL[36] * (0.43110859113387556 - (0.15373504582939335 * applyconvL)));
					tempSampleL -= (cabs->bL[37] * (0.37726894966096269 - (0.11570983506028532 * applyconvL)));
					tempSampleL -= (cabs->bL[38] * (0.39953242355200935 - (0.17879231130484088 * applyconvL)));
					tempSampleL -= (cabs->bL[39] * (0.36726676379100875 - (0.22013553023983223 * applyconvL)));
					tempSampleL -= (cabs->bL[40] * (0.27187029469227386 - (0.18461171768478427 * applyconvL)));
					tempSampleL -= (cabs->bL[41] * (0.21109334552321635 - (0.14497481318083569 * applyconvL)));
					tempSampleL -= (cabs->bL[42] * (0.19808797405293213 - (0.14916579928186940 * applyconvL)));
					tempSampleL -= (cabs->bL[43] * (0.16287926785495671 - (0.15146098461120627 * applyconvL)));
					tempSampleL -= (cabs->bL[44] * (0.11086621477163359 - (0.13182973443924018 * applyconvL)));
					tempSampleL -= (cabs->bL[45] * (0.07531043236890560 - (0.08062172796472888 * applyconvL)));
					tempSampleL -= (cabs->bL[46] * (0.01747364473230771 + (0.02201865873632456 * applyconvL)));
					tempSampleL += (cabs->bL[47] * (0.03080279125662693 - (0.08721756240972101 * applyconvL)));
					tempSampleL += (cabs->bL[48] * (0.02354148659185142 - (0.06376361763053796 * applyconvL)));
					tempSampleL -= (cabs->bL[49] * (0.02835772372098715 + (0.00589978513642627 * applyconvL)));
					tempSampleL -= (cabs->bL[50] * (0.08983370744565244 - (0.02350960427706536 * applyconvL)));
					tempSampleL -= (cabs->bL[51] * (0.14148947620055380 - (0.03329826628693369 * applyconvL)));
					tempSampleL -= (cabs->bL[52] * (0.17576502674572581 - (0.06507546651241880 * applyconvL)));
					tempSampleL -= (cabs->bL[53] * (0.17168865666573860 - (0.07734801128437317 * applyconvL)));
					tempSampleL -= (cabs->bL[54] * (0.14107027738292105 - (0.03136459344220402 * applyconvL)));
					tempSampleL -= (cabs->bL[55] * (0.12287163395380074 + (0.01933408169185258 * applyconvL)));
					tempSampleL -= (cabs->bL[56] * (0.12276622398112971 + (0.01983508766241737 * applyconvL)));
					tempSampleL -= (cabs->bL[57] * (0.12349721440213673 - (0.01111031415304768 * applyconvL)));
					tempSampleL -= (cabs->bL[58] * (0.08649454142716655 + (0.02252815645513927 * applyconvL)));
					tempSampleL -= (cabs->bL[59] * (0.00953083685474757 + (0.13778878548343007 * applyconvL)));
					tempSampleL += (cabs->bL[60] * (0.06045983158868478 - (0.23966318224935096 * applyconvL)));
					tempSampleL += (cabs->bL[61] * (0.09053229817093242 - (0.27190119941572544 * applyconvL)));
					tempSampleL += (cabs->bL[62] * (0.08112662178843048 - (0.22456862606452327 * applyconvL)));
					tempSampleL += (cabs->bL[63] * (0.07503525686243730 - (0.14330154410548213 * applyconvL)));
					tempSampleL += (cabs->bL[64] * (0.07372595404399729 - (0.06185193766408734 * applyconvL)));
					tempSampleL += (cabs->bL[65] * (0.06073789200080433 + (0.01261857435786178 * applyconvL)));
					tempSampleL += (cabs->bL[66] * (0.04616712695742254 + (0.05851771967084609 * applyconvL)));
					tempSampleL += (cabs->bL[67] * (0.01036235510345900 + (0.08286534414423796 * applyconvL)));
					tempSampleL -= (cabs->bL[68] * (0.03708389413229191 - (0.06695282381039531 * applyconvL)));
					tempSampleL -= (cabs->bL[69] * (0.07092204876981217 - (0.01915829199112784 * applyconvL)));
					tempSampleL -= (cabs->bL[70] * (0.09443579589460312 + (0.01210082455316246 * applyconvL)));
					tempSampleL -= (cabs->bL[71] * (0.07824038577769601 + (0.06121988546065113 * applyconvL)));
					tempSampleL -= (cabs->bL[72] * (0.00854730633079399 + (0.14468518752295506 * applyconvL)));
					tempSampleL += (cabs->bL[73] * (0.06845589924191028 - (0.18902431382592944 * applyconvL)));
					tempSampleL += (cabs->bL[74] * (0.10351569998375465 - (0.13204443060279647 * applyconvL)));
					tempSampleL += (cabs->bL[75] * (0.10513368758532179 - (0.02993199294485649 * applyconvL)));
					tempSampleL += (cabs->bL[76] * (0.08896978950235003 + (0.04074499273825906 * applyconvL)));
					tempSampleL += (cabs->bL[77] * (0.03697537734050980 + (0.09217751130846838 * applyconvL)));
					tempSampleL -= (cabs->bL[78] * (0.04014322441280276 - (0.14062297149365666 * applyconvL)));
					tempSampleL -= (cabs->bL[79] * (0.10505934581398618 - (0.16988861157275814 * applyconvL)));
					tempSampleL -= (cabs->bL[80] * (0.13937661651676272 - (0.15083294570551492 * applyconvL)));
					tempSampleL -= (cabs->bL[81] * (0.13183458845108439 - (0.06657454442471208 * applyconvL)));
					// begin Bass conv R
					tempSampleR += (cabs->bR[1] * (1.35472031405494242 + (0.00220914099195157 * applyconvR)));
					tempSampleR += (cabs->bR[2] * (1.63534207755253003 - (0.11406232654509685 * applyconvR)));
					tempSampleR += (cabs->bR[3] * (1.82334575691525869 - (0.42647194712964054 * applyconvR)));
					tempSampleR += (cabs->bR[4] * (1.86156386235405868 - (0.76744187887586590 * applyconvR)));
					tempSampleR += (cabs->bR[5] * (1.67332739338852599 - (0.95161997324293013 * applyconvR)));
					tempSampleR += (cabs->bR[6] * (1.25054130794899021 - (0.98410433514572859 * applyconvR)));
					tempSampleR += (cabs->bR[7] * (0.70049121047281737 - (0.87375612110718992 * applyconvR)));
					tempSampleR += (cabs->bR[8] * (0.15291791448081560 - (0.61195266024519046 * applyconvR)));
					tempSampleR -= (cabs->bR[9] * (0.37301992914152693 + (0.16755422915252094 * applyconvR)));
					tempSampleR -= (cabs->bR[10] * (0.76568539228498433 - (0.28554435228965386 * applyconvR)));
					tempSampleR -= (cabs->bR[11] * (0.95726568749937369 - (0.61659719162806048 * applyconvR)));
					tempSampleR -= (cabs->bR[12] * (1.01273552193911032 - (0.81827288407943954 * applyconvR)));
					tempSampleR -= (cabs->bR[13] * (0.93920108117234447 - (0.80077111864205874 * applyconvR)));
					tempSampleR -= (cabs->bR[14] * (0.79831898832953974 - (0.65814750339694406 * applyconvR)));
					tempSampleR -= (cabs->bR[15] * (0.64200088100452313 - (0.46135833001232618 * applyconvR)));
					tempSampleR -= (cabs->bR[16] * (0.48807302802822128 - (0.15506178974799034 * applyconvR)));
					tempSampleR -= (cabs->bR[17] * (0.36545171501947982 + (0.16126103769376721 * applyconvR)));
					tempSampleR -= (cabs->bR[18] * (0.31469581455759105 + (0.32250870039053953 * applyconvR)));
					tempSampleR -= (cabs->bR[19] * (0.36893534817945800 + (0.25409418897237473 * applyconvR)));
					tempSampleR -= (cabs->bR[20] * (0.41092557722975687 + (0.13114730488878301 * applyconvR)));
					tempSampleR -= (cabs->bR[21] * (0.38584044480710594 + (0.06825323739722661 * applyconvR)));
					tempSampleR -= (cabs->bR[22] * (0.33378434007178670 + (0.04144255489164217 * applyconvR)));
					tempSampleR -= (cabs->bR[23] * (0.26144203061699706 + (0.06031313105098152 * applyconvR)));
					tempSampleR -= (cabs->bR[24] * (0.25818342000920502 + (0.03642289242586355 * applyconvR)));
					tempSampleR -= (cabs->bR[25] * (0.28096018498822661 + (0.00976973667327174 * applyconvR)));
					tempSampleR -= (cabs->bR[26] * (0.25845682019095384 + (0.02749015358080831 * applyconvR)));
					tempSampleR -= (cabs->bR[27] * (0.26655607865953096 - (0.00329839675455690 * applyconvR)));
					tempSampleR -= (cabs->bR[28] * (0.30590085026938518 - (0.07375043215328811 * applyconvR)));
					tempSampleR -= (cabs->bR[29] * (0.32875683916470899 - (0.12454134857516502 * applyconvR)));
					tempSampleR -= (cabs->bR[30] * (0.38166643180506560 - (0.19973911428609989 * applyconvR)));
					tempSampleR -= (cabs->bR[31] * (0.49068186937289598 - (0.34785166842136384 * applyconvR)));
					tempSampleR -= (cabs->bR[32] * (0.60274753867622777 - (0.48685038872711034 * applyconvR)));
					tempSampleR -= (cabs->bR[33] * (0.65944678627090636 - (0.49844657885975518 * applyconvR)));
					tempSampleR -= (cabs->bR[34] * (0.64488955808717285 - (0.40514406499806987 * applyconvR)));
					tempSampleR -= (cabs->bR[35] * (0.55818730353434354 - (0.28029870614987346 * applyconvR)));
					tempSampleR -= (cabs->bR[36] * (0.43110859113387556 - (0.15373504582939335 * applyconvR)));
					tempSampleR -= (cabs->bR[37] * (0.37726894966096269 - (0.11570983506028532 * applyconvR)));
					tempSampleR -= (cabs->bR[38] * (0.39953242355200935 - (0.17879231130484088 * applyconvR)));
					tempSampleR -= (cabs->bR[39] * (0.36726676379100875 - (0.22013553023983223 * applyconvR)));
					tempSampleR -= (cabs->bR[40] * (0.27187029469227386 - (0.18461171768478427 * applyconvR)));
					tempSampleR -= (cabs->bR[41] * (0.21109334552321635 - (0.14497481318083569 * applyconvR)));
					tempSampleR -= (cabs->bR[42] * (0.19808797405293213 - (0.14916579928186940 * applyconvR)));
					tempSampleR -= (cabs->bR[43] * (0.16287926785495671 - (0.15146098461120627 * applyconvR)));
					tempSampleR -= (cabs->bR[44] * (0.11086621477163359 - (0.13182973443924018 * applyconvR)));
					tempSampleR -= (cabs->bR[45] * (0.07531043236890560 - (0.08062172796472888 * applyconvR)));
					tempSampleR -= (cabs->bR[46] * (0.01747364473230771 + (0.02201865873632456 * applyconvR)));
					tempSampleR += (cabs->bR[47] * (0.03080279125662693 - (0.08721756240972101 * applyconvR)));
					tempSampleR += (cabs->bR[48] * (0.02354148659185142 - (0.06376361763053796 * applyconvR)));
					tempSampleR -= (cabs->bR[49] * (0.02835772372098715 + (0.00589978513642627 * applyconvR)));
					tempSampleR -= (cabs->bR[50] * (0.08983370744565244 - (0.02350960427706536 * applyconvR)));
					tempSampleR -= (cabs->bR[51] * (0.14148947620055380 - (0.03329826628693369 * applyconvR)));
					tempSampleR -= (cabs->bR[52] * (0.17576502674572581 - (0.06507546651241880 * applyconvR)));
					tempSampleR -= (cabs->bR[53] * (0.17168865666573860 - (0.07734801128437317 * applyconvR)));
					tempSampleR -= (cabs->bR[54] * (0.14107027738292105 - (0.03136459344220402 * applyconvR)));
					tempSampleR -= (cabs->bR[55] * (0.12287163395380074 + (0.01933408169185258 * applyconvR)));
					tempSampleR -= (cabs->bR[56] * (0.12276622398112971 + (0.01983508766241737 * applyconvR)));
					tempSampleR -= (cabs->bR[57] * (0.12349721440213673 - (0.01111031415304768 * applyconvR)));
					tempSampleR -= (cabs->bR[58] * (0.08649454142716655 + (0.02252815645513927 * applyconvR)));
					tempSampleR -= (cabs->bR[59] * (0.00953083685474757 + (0.13778878548343007 * applyconvR)));
					tempSampleR += (cabs->bR[60] * (0.06045983158868478 - (0.23966318224935096 * applyconvR)));
					tempSampleR += (cabs->bR[61] * (0.09053229817093242 - (0.27190119941572544 * applyconvR)));
					tempSampleR += (cabs->bR[62] * (0.08112662178843048 - (0.22456862606452327 * applyconvR)));
					tempSampleR += (cabs->bR[63] * (0.07503525686243730 - (0.14330154410548213 * applyconvR)));
					tempSampleR += (cabs->bR[64] * (0.07372595404399729 - (0.06185193766408734 * applyconvR)));
					tempSampleR += (cabs->bR[65] * (0.06073789200080433 + (0.01261857435786178 * applyconvR)));
					tempSampleR += (cabs->bR[66] * (0.04616712695742254 + (0.05851771967084609 * applyconvR)));
					tempSampleR += (cabs->bR[67] * (0.01036235510345900 + (0.08286534414423796 * applyconvR)));
					tempSampleR -= (cabs->bR[68] * (0.03708389413229191 - (0.06695282381039531 * applyconvR)));
					tempSampleR -= (cabs->bR[69] * (0.07092204876981217 - (0.01915829199112784 * applyconvR)));
					tempSampleR -= (cabs->bR[70] * (0.09443579589460312 + (0.01210082455316246 * applyconvR)));
					tempSampleR -= (cabs->bR[71] * (0.07824038577769601 + (0.06121988546065113 * applyconvR)));
					tempSampleR -= (cabs->bR[72] * (0.00854730633079399 + (0.14468518752295506 * applyconvR)));
					tempSampleR += (cabs->bR[73] * (0.06845589924191028 - (0.18902431382592944 * applyconvR)));
					tempSampleR += (cabs->bR[74] * (0.10351569998375465 - (0.13204443060279647 * applyconvR)));
					tempSampleR += (cabs->bR[75] * (0.10513368758532179 - (0.02993199294485649 * applyconvR)));
					tempSampleR += (cabs->bR[76] * (0.08896978950235003 + (0.04074499273825906 * applyconvR)));
					tempSampleR += (cabs->bR[77] * (0.03697537734050980 + (0.09217751130846838 * applyconvR)));
					tempSampleR -= (cabs->bR[78] * (0.04014322441280276 - (0.14062297149365666 * applyconvR)));
					tempSampleR -= (cabs->bR[79] * (0.10505934581398618 - (0.16988861157275814 * applyconvR)));
					tempSampleR -= (cabs->bR[80] * (0.13937661651676272 - (0.15083294570551492 * applyconvR)));
					tempSampleR -= (cabs->bR[81] * (0.13183458845108439 - (0.06657454442471208 * applyconvR)));
					// end Bass conv
					break;
			}
			inputSampleL *= correctdrygain;
			inputSampleL += (tempSampleL * colorIntensity);
			inputSampleL /= correctboost;
			ataHalfwaySampleL += inputSampleL; //--------------------
			inputSampleR *= correctdrygain;
			inputSampleR += (tempSampleR * colorIntensity);
			inputSampleR /= correctboost;
			ataHalfwaySampleR += inputSampleR;
			// restore interpolated signal including time domain stuff now
			// end center code for handling timedomain/conv stuff

			// second wave of Cabs style slew clamping
			clamp = inputSampleL - cabs->lastPostSampleL;
			if (clamp > threshold) inputSampleL = cabs->lastPostSampleL + threshold;
			if (-clamp > rarefaction) inputSampleL = cabs->lastPostSampleL - rarefaction;
			cabs->lastPostSampleL = inputSampleL; //-------------------------------------
			clamp = inputSampleR - cabs->lastPostSampleR;
			if (clamp > threshold) inputSampleR = cabs->lastPostSampleR + threshold;
			if (-clamp > rarefaction) inputSampleR = cabs->lastPostSampleR - rarefaction;
			cabs->lastPostSampleR = inputSampleR;
			// end raw sample

			// begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
			clamp = ataHalfwaySampleL - cabs->lastPostHalfSampleL;
			if (clamp > threshold) ataHalfwaySampleL = cabs->lastPostHalfSampleL + threshold;
			if (-clamp > rarefaction) ataHalfwaySampleL = cabs->lastPostHalfSampleL - rarefaction;
			cabs->lastPostHalfSampleL = ataHalfwaySampleL; //----------------------------------
			clamp = ataHalfwaySampleR - cabs->lastPostHalfSampleR;
			if (clamp > threshold) ataHalfwaySampleR = cabs->lastPostHalfSampleR + threshold;
			if (-clamp > rarefaction) ataHalfwaySampleR = cabs->lastPostHalfSampleR - rarefaction;
			cabs->lastPostHalfSampleR = ataHalfwaySampleR;
			// end interpolated sample

			// post-center code on inputSample and halfwaySample in parallel
			// begin raw sample- inputSample and ataDrySample handled separately here
			double HeadBumpL;
			double HeadBumpR;
			if (cabs->flip) {
				cabs->iirHeadBumpAL += (inputSampleL * HeadBumpFreq);
				cabs->iirHeadBumpAL -= (cabs->iirHeadBumpAL * cabs->iirHeadBumpAL * cabs->iirHeadBumpAL * HeadBumpFreq);
				if (cabs->iirHeadBumpAL > 0) cabs->iirHeadBumpAL -= dcblock;
				if (cabs->iirHeadBumpAL < 0) cabs->iirHeadBumpAL += dcblock;
				if (fabs(cabs->iirHeadBumpAL) > 100.0) {
					cabs->iirHeadBumpAL = 0.0;
					cabs->iirHeadBumpBL = 0.0;
					cabs->iirHalfHeadBumpAL = 0.0;
					cabs->iirHalfHeadBumpBL = 0.0;
				}
				HeadBumpL = cabs->iirHeadBumpAL; //-----------------------------------------------
				cabs->iirHeadBumpAR += (inputSampleR * HeadBumpFreq);
				cabs->iirHeadBumpAR -= (cabs->iirHeadBumpAR * cabs->iirHeadBumpAR * cabs->iirHeadBumpAR * HeadBumpFreq);
				if (cabs->iirHeadBumpAR > 0) cabs->iirHeadBumpAR -= dcblock;
				if (cabs->iirHeadBumpAR < 0) cabs->iirHeadBumpAR += dcblock;
				if (fabs(cabs->iirHeadBumpAR) > 100.0) {
					cabs->iirHeadBumpAR = 0.0;
					cabs->iirHeadBumpBR = 0.0;
					cabs->iirHalfHeadBumpAR = 0.0;
					cabs->iirHalfHeadBumpBR = 0.0;
				}
				HeadBumpR = cabs->iirHeadBumpAR;
			} else {
				cabs->iirHeadBumpBL += (inputSampleL * HeadBumpFreq);
				cabs->iirHeadBumpBL -= (cabs->iirHeadBumpBL * cabs->iirHeadBumpBL * cabs->iirHeadBumpBL * HeadBumpFreq);
				if (cabs->iirHeadBumpBL > 0) cabs->iirHeadBumpBL -= dcblock;
				if (cabs->iirHeadBumpBL < 0) cabs->iirHeadBumpBL += dcblock;
				if (fabs(cabs->iirHeadBumpBL) > 100.0) {
					cabs->iirHeadBumpAL = 0.0;
					cabs->iirHeadBumpBL = 0.0;
					cabs->iirHalfHeadBumpAL = 0.0;
					cabs->iirHalfHeadBumpBL = 0.0;
				}
				HeadBumpL = cabs->iirHeadBumpBL; //---------------------------------------------------
				cabs->iirHeadBumpBR += (inputSampleR * HeadBumpFreq);
				cabs->iirHeadBumpBR -= (cabs->iirHeadBumpBR * cabs->iirHeadBumpBR * cabs->iirHeadBumpBR * HeadBumpFreq);
				if (cabs->iirHeadBumpBR > 0) cabs->iirHeadBumpBR -= dcblock;
				if (cabs->iirHeadBumpBR < 0) cabs->iirHeadBumpBR += dcblock;
				if (fabs(cabs->iirHeadBumpBR) > 100.0) {
					cabs->iirHeadBumpAR = 0.0;
					cabs->iirHeadBumpBR = 0.0;
					cabs->iirHalfHeadBumpAR = 0.0;
					cabs->iirHalfHeadBumpBR = 0.0;
				}
				HeadBumpR = cabs->iirHeadBumpBR;
			}
			HeadBumpL /= LowsPad;
			inputSampleL = (inputSampleL * (1.0 - heavy)) + (HeadBumpL * heavy); //---------------------
			HeadBumpR /= LowsPad;
			inputSampleR = (inputSampleR * (1.0 - heavy)) + (HeadBumpR * heavy);
			// end raw sample

			// begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
			if (cabs->flip) {
				cabs->iirHalfHeadBumpAL += (ataHalfwaySampleL * HeadBumpFreq);
				cabs->iirHalfHeadBumpAL -= (cabs->iirHalfHeadBumpAL * cabs->iirHalfHeadBumpAL * cabs->iirHalfHeadBumpAL * HeadBumpFreq);
				if (cabs->iirHalfHeadBumpAL > 0) cabs->iirHalfHeadBumpAL -= dcblock;
				if (cabs->iirHalfHeadBumpAL < 0) cabs->iirHalfHeadBumpAL += dcblock;
				if (fabs(cabs->iirHalfHeadBumpAL) > 100.0) {
					cabs->iirHeadBumpAL = 0.0;
					cabs->iirHeadBumpBL = 0.0;
					cabs->iirHalfHeadBumpAL = 0.0;
					cabs->iirHalfHeadBumpBL = 0.0;
				}
				HeadBumpL = cabs->iirHalfHeadBumpAL; //------------------------------------------------------
				cabs->iirHalfHeadBumpAR += (ataHalfwaySampleR * HeadBumpFreq);
				cabs->iirHalfHeadBumpAR -= (cabs->iirHalfHeadBumpAR * cabs->iirHalfHeadBumpAR * cabs->iirHalfHeadBumpAR * HeadBumpFreq);
				if (cabs->iirHalfHeadBumpAR > 0) cabs->iirHalfHeadBumpAR -= dcblock;
				if (cabs->iirHalfHeadBumpAR < 0) cabs->iirHalfHeadBumpAR += dcblock;
				if (fabs(cabs->iirHalfHeadBumpAR) > 100.0) {
					cabs->iirHeadBumpAR = 0.0;
					cabs->iirHeadBumpBR = 0.0;
					cabs->iirHalfHeadBumpAR = 0.0;
					cabs->iirHalfHeadBumpBR = 0.0;
				}
				HeadBumpR = cabs->iirHalfHeadBumpAR;
			} else {
				cabs->iirHalfHeadBumpBL += (ataHalfwaySampleL * HeadBumpFreq);
				cabs->iirHalfHeadBumpBL -= (cabs->iirHalfHeadBumpBL * cabs->iirHalfHeadBumpBL * cabs->iirHalfHeadBumpBL * HeadBumpFreq);
				if (cabs->iirHalfHeadBumpBL > 0) cabs->iirHalfHeadBumpBL -= dcblock;
				if (cabs->iirHalfHeadBumpBL < 0) cabs->iirHalfHeadBumpBL += dcblock;
				if (fabs(cabs->iirHalfHeadBumpBL) > 100.0) {
					cabs->iirHeadBumpAL = 0.0;
					cabs->iirHeadBumpBL = 0.0;
					cabs->iirHalfHeadBumpAL = 0.0;
					cabs->iirHalfHeadBumpBL = 0.0;
				}
				HeadBumpL = cabs->iirHalfHeadBumpBL; //---------------------------------------------------
				cabs->iirHalfHeadBumpBR += (ataHalfwaySampleR * HeadBumpFreq);
				cabs->iirHalfHeadBumpBR -= (cabs->iirHalfHeadBumpBR * cabs->iirHalfHeadBumpBR * cabs->iirHalfHeadBumpBR * HeadBumpFreq);
				if (cabs->iirHalfHeadBumpBR > 0) cabs->iirHalfHeadBumpBR -= dcblock;
				if (cabs->iirHalfHeadBumpBR < 0) cabs->iirHalfHeadBumpBR += dcblock;
				if (fabs(cabs->iirHalfHeadBumpBR) > 100.0) {
					cabs->iirHeadBumpAR = 0.0;
					cabs->iirHeadBumpBR = 0.0;
					cabs->iirHalfHeadBumpAR = 0.0;
					cabs->iirHalfHeadBumpBR = 0.0;
				}
				HeadBumpR = cabs->iirHalfHeadBumpBR;
			}
			HeadBumpL /= LowsPad;
			ataHalfwaySampleL = (ataHalfwaySampleL * (1.0 - heavy)) + (HeadBumpL * heavy); //--------------------
			HeadBumpR /= LowsPad;
			ataHalfwaySampleR = (ataHalfwaySampleR * (1.0 - heavy)) + (HeadBumpR * heavy);
			// end interpolated sample

			// begin antialiasing section for halfway sample
			cabs->ataCL = ataHalfwaySampleL - ataHalfDrySampleL;
			if (cabs->flip) {
				cabs->ataAL *= 0.94;
				cabs->ataBL *= 0.94;
				cabs->ataAL += cabs->ataCL;
				cabs->ataBL -= cabs->ataCL;
				cabs->ataCL = cabs->ataAL;
			} else {
				cabs->ataBL *= 0.94;
				cabs->ataAL *= 0.94;
				cabs->ataBL += cabs->ataCL;
				cabs->ataAL -= cabs->ataCL;
				cabs->ataCL = cabs->ataBL;
			}
			cabs->ataHalfDiffSampleL = (cabs->ataCL * 0.94); //---------------------------------
			cabs->ataCR = ataHalfwaySampleR - ataHalfDrySampleR;
			if (cabs->flip) {
				cabs->ataAR *= 0.94;
				cabs->ataBR *= 0.94;
				cabs->ataAR += cabs->ataCR;
				cabs->ataBR -= cabs->ataCR;
				cabs->ataCR = cabs->ataAR;
			} else {
				cabs->ataBR *= 0.94;
				cabs->ataAR *= 0.94;
				cabs->ataBR += cabs->ataCR;
				cabs->ataAR -= cabs->ataCR;
				cabs->ataCR = cabs->ataBR;
			}
			cabs->ataHalfDiffSampleR = (cabs->ataCR * 0.94);
			// end antialiasing section for halfway sample

			// begin antialiasing section for raw sample
			cabs->ataCL = inputSampleL - ataDrySampleL;
			if (cabs->flip) {
				cabs->ataAL *= 0.94;
				cabs->ataBL *= 0.94;
				cabs->ataAL += cabs->ataCL;
				cabs->ataBL -= cabs->ataCL;
				cabs->ataCL = cabs->ataAL;
			} else {
				cabs->ataBL *= 0.94;
				cabs->ataAL *= 0.94;
				cabs->ataBL += cabs->ataCL;
				cabs->ataAL -= cabs->ataCL;
				cabs->ataCL = cabs->ataBL;
			}
			cabs->ataDiffSampleL = (cabs->ataCL * 0.94); //-----------------------------
			cabs->ataCR = inputSampleR - ataDrySampleR;
			if (cabs->flip) {
				cabs->ataAR *= 0.94;
				cabs->ataBR *= 0.94;
				cabs->ataAR += cabs->ataCR;
				cabs->ataBR -= cabs->ataCR;
				cabs->ataCR = cabs->ataAR;
			} else {
				cabs->ataBR *= 0.94;
				cabs->ataAR *= 0.94;
				cabs->ataBR += cabs->ataCR;
				cabs->ataAR -= cabs->ataCR;
				cabs->ataCR = cabs->ataBR;
			}
			cabs->ataDiffSampleR = (cabs->ataCR * 0.94);
			// end antialiasing section for input sample
			cabs->flip = !cabs->flip;

			inputSampleL = ataDrySampleL;
			inputSampleL += ((cabs->ataDiffSampleL + cabs->ataHalfDiffSampleL + cabs->ataPrevDiffSampleL) / 1.0);
			cabs->ataPrevDiffSampleL = cabs->ataDiffSampleL / 2.0; //----------------------------
			inputSampleR = ataDrySampleR;
			inputSampleR += ((cabs->ataDiffSampleR + cabs->ataHalfDiffSampleR + cabs->ataPrevDiffSampleR) / 1.0);
			cabs->ataPrevDiffSampleR = cabs->ataDiffSampleR / 2.0;
			// apply processing as difference to non-oversampled raw input

			clamp = inputSampleL - cabs->postPostSampleL;
			if (clamp > postThreshold) inputSampleL = cabs->postPostSampleL + postThreshold;
			if (-clamp > postRarefaction) inputSampleL = cabs->postPostSampleL - postRarefaction;
			cabs->postPostSampleL = inputSampleL;
			inputSampleL /= postTrim;
			inputSampleL *= output; //------------------------------
			clamp = inputSampleR - cabs->postPostSampleR;
			if (clamp > postThreshold) inputSampleR = cabs->postPostSampleR + postThreshold;
			if (-clamp > postRarefaction) inputSampleR = cabs->postPostSampleR - postRarefaction;
			cabs->postPostSampleR = inputSampleR;
			inputSampleR /= postTrim;
			inputSampleR *= output;

			if (cycleEnd == 4) {
				cabs->lastRefL[0] = cabs->lastRefL[4]; // start from previous last
				cabs->lastRefL[2] = (cabs->lastRefL[0] + inputSampleL) / 2; // half
				cabs->lastRefL[1] = (cabs->lastRefL[0] + cabs->lastRefL[2]) / 2; // one quarter
				cabs->lastRefL[3] = (cabs->lastRefL[2] + inputSampleL) / 2; // three quarters
				cabs->lastRefL[4] = inputSampleL; // full
				cabs->lastRefR[0] = cabs->lastRefR[4]; // start from previous last
				cabs->lastRefR[2] = (cabs->lastRefR[0] + inputSampleR) / 2; // half
				cabs->lastRefR[1] = (cabs->lastRefR[0] + cabs->lastRefR[2]) / 2; // one quarter
				cabs->lastRefR[3] = (cabs->lastRefR[2] + inputSampleR) / 2; // three quarters
				cabs->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				cabs->lastRefL[0] = cabs->lastRefL[3]; // start from previous last
				cabs->lastRefL[2] = (cabs->lastRefL[0] + cabs->lastRefL[0] + inputSampleL) / 3; // third
				cabs->lastRefL[1] = (cabs->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				cabs->lastRefL[3] = inputSampleL; // full
				cabs->lastRefR[0] = cabs->lastRefR[3]; // start from previous last
				cabs->lastRefR[2] = (cabs->lastRefR[0] + cabs->lastRefR[0] + inputSampleR) / 3; // third
				cabs->lastRefR[1] = (cabs->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				cabs->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				cabs->lastRefL[0] = cabs->lastRefL[2]; // start from previous last
				cabs->lastRefL[1] = (cabs->lastRefL[0] + inputSampleL) / 2; // half
				cabs->lastRefL[2] = inputSampleL; // full
				cabs->lastRefR[0] = cabs->lastRefR[2]; // start from previous last
				cabs->lastRefR[1] = (cabs->lastRefR[0] + inputSampleR) / 2; // half
				cabs->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				cabs->lastRefL[0] = inputSampleL;
				cabs->lastRefR[0] = inputSampleR;
			}
			cabs->cycle = 0; // reset
			inputSampleL = cabs->lastRefL[cabs->cycle];
			inputSampleR = cabs->lastRefR[cabs->cycle];
		} else {
			inputSampleL = cabs->lastRefL[cabs->cycle];
			inputSampleR = cabs->lastRefR[cabs->cycle];
			// we are going through our references now
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		cabs->fpdL ^= cabs->fpdL << 13;
		cabs->fpdL ^= cabs->fpdL >> 17;
		cabs->fpdL ^= cabs->fpdL << 5;
		inputSampleL += (((double) cabs->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		cabs->fpdR ^= cabs->fpdR << 13;
		cabs->fpdR ^= cabs->fpdR >> 17;
		cabs->fpdR ^= cabs->fpdR << 5;
		inputSampleR += (((double) cabs->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CABS_URI,
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
