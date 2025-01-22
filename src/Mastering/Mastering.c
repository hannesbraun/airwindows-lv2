#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MASTERING_URI "https://hannesbraun.net/ns/lv2/airwindows/mastering"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	GLUE = 4,
	SCOPE = 5,
	SKRONK = 6,
	GIRTH = 7,
	DRIVE = 8,
	DITHER = 9
} PortIndex;

enum {
	pvAL1,
	pvSL1,
	accSL1,
	acc2SL1,
	pvAL2,
	pvSL2,
	accSL2,
	acc2SL2,
	pvAL3,
	pvSL3,
	accSL3,
	pvAL4,
	pvSL4,
	gndavgL,
	outAL,
	gainAL,
	pvAR1,
	pvSR1,
	accSR1,
	acc2SR1,
	pvAR2,
	pvSR2,
	accSR2,
	acc2SR2,
	pvAR3,
	pvSR3,
	accSR3,
	pvAR4,
	pvSR4,
	gndavgR,
	outAR,
	gainAR,
	air_total
};

enum {
	prevSampL1,
	prevSlewL1,
	accSlewL1,
	prevSampL2,
	prevSlewL2,
	accSlewL2,
	prevSampL3,
	prevSlewL3,
	accSlewL3,
	kalGainL,
	kalOutL,
	kalAvgL,
	prevSampR1,
	prevSlewR1,
	accSlewR1,
	prevSampR2,
	prevSlewR2,
	accSlewR2,
	prevSampR3,
	prevSlewR3,
	accSlewR3,
	kalGainR,
	kalOutR,
	kalAvgR,
	kal_total
};

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* glue;
	const float* scope;
	const float* skronk;
	const float* girth;
	const float* drive;
	const float* dither;

	double air[air_total];

	double kalM[kal_total];
	double kalS[kal_total];

	long double lastSinewL;
	long double lastSinewR;
	// this is overkill, used to run both Zoom and Sinew stages as they are after
	// the summing in StoneFire, which sums three doubles to a long double.

	double lastSampleL;
	double intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	double lastSampleR;
	double intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; // Stereo ClipOnly2

	int quantA;
	int quantB;
	float expectedSlew;
	float testA;
	float testB;
	double correction;
	double shapedSampleL;
	double shapedSampleR;
	double currentDither;
	double ditherL;
	double ditherR;
	bool cutbinsL;
	bool cutbinsR;
	int hotbinA;
	int hotbinB;
	double benfordize;
	double totalA;
	double totalB;
	double outputSample;
	int expon; // internal dither variables

	double NSOddL; // dither section!
	double NSEvenL;
	double prevShapeL;
	double NSOddR;
	double NSEvenR;
	double prevShapeR;
	bool flip; // VinylDither
	double darkSampleL[100];
	double darkSampleR[100]; // Dark
	double previousDitherL;
	double previousDitherR; // PaulWide
	double bynL[13], bynR[13];
	double noiseShapingL, noiseShapingR; // NJAD

	uint32_t fpdL;
	uint32_t fpdR;
} Mastering;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Mastering* mastering = (Mastering*) calloc(1, sizeof(Mastering));
	mastering->sampleRate = rate;
	return (LV2_Handle) mastering;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Mastering* mastering = (Mastering*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			mastering->input[0] = (const float*) data;
			break;
		case INPUT_R:
			mastering->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			mastering->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			mastering->output[1] = (float*) data;
			break;
		case GLUE:
			mastering->glue = (const float*) data;
			break;
		case SCOPE:
			mastering->scope = (const float*) data;
			break;
		case SKRONK:
			mastering->skronk = (const float*) data;
			break;
		case GIRTH:
			mastering->girth = (const float*) data;
			break;
		case DRIVE:
			mastering->drive = (const float*) data;
			break;
		case DITHER:
			mastering->dither = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Mastering* mastering = (Mastering*) instance;

	for (int x = 0; x < air_total; x++) mastering->air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) {
		mastering->kalM[x] = 0.0;
		mastering->kalS[x] = 0.0;
	}

	mastering->lastSinewL = 0.0;
	mastering->lastSinewR = 0.0;

	mastering->lastSampleL = 0.0;
	mastering->wasPosClipL = false;
	mastering->wasNegClipL = false;
	mastering->lastSampleR = 0.0;
	mastering->wasPosClipR = false;
	mastering->wasNegClipR = false;
	for (int x = 0; x < 16; x++) {
		mastering->intermediateL[x] = 0.0;
		mastering->intermediateR[x] = 0.0;
	}

	mastering->quantA = 0;
	mastering->quantB = 1;
	mastering->expectedSlew = 0.0;
	mastering->testA = 0.0;
	mastering->testB = 0.0;
	mastering->correction = 0.0;
	mastering->shapedSampleL = 0.0;
	mastering->shapedSampleR = 0.0;
	mastering->currentDither = 0.0;
	mastering->ditherL = 0.0;
	mastering->ditherR = 0.0;
	mastering->cutbinsL = false;
	mastering->cutbinsR = false;
	mastering->hotbinA = 0;
	mastering->hotbinB = 0;
	mastering->benfordize = 0.0;
	mastering->totalA = 0.0;
	mastering->totalB = 0.0;
	mastering->outputSample = 0.0;
	mastering->expon = 0; // internal dither variables
	// these didn't like to be defined inside a case statement

	mastering->NSOddL = 0.0;
	mastering->NSEvenL = 0.0;
	mastering->prevShapeL = 0.0;
	mastering->NSOddR = 0.0;
	mastering->NSEvenR = 0.0;
	mastering->prevShapeR = 0.0;
	mastering->flip = true; // Ten Nines
	for (int count = 0; count < 99; count++) {
		mastering->darkSampleL[count] = 0;
		mastering->darkSampleR[count] = 0;
	} // Dark
	mastering->previousDitherL = 0.0;
	mastering->previousDitherR = 0.0; // PaulWide
	mastering->bynL[0] = 1000.0;
	mastering->bynL[1] = 301.0;
	mastering->bynL[2] = 176.0;
	mastering->bynL[3] = 125.0;
	mastering->bynL[4] = 97.0;
	mastering->bynL[5] = 79.0;
	mastering->bynL[6] = 67.0;
	mastering->bynL[7] = 58.0;
	mastering->bynL[8] = 51.0;
	mastering->bynL[9] = 46.0;
	mastering->bynL[10] = 1000.0;
	mastering->noiseShapingL = 0.0;
	mastering->bynR[0] = 1000.0;
	mastering->bynR[1] = 301.0;
	mastering->bynR[2] = 176.0;
	mastering->bynR[3] = 125.0;
	mastering->bynR[4] = 97.0;
	mastering->bynR[5] = 79.0;
	mastering->bynR[6] = 67.0;
	mastering->bynR[7] = 58.0;
	mastering->bynR[8] = 51.0;
	mastering->bynR[9] = 46.0;
	mastering->bynR[10] = 1000.0;
	mastering->noiseShapingR = 0.0; // NJAD

	mastering->fpdL = 1.0;
	while (mastering->fpdL < 16386) mastering->fpdL = rand() * UINT32_MAX;
	mastering->fpdR = 1.0;
	while (mastering->fpdR < 16386) mastering->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Mastering* mastering = (Mastering*) instance;

	const float* in1 = mastering->input[0];
	const float* in2 = mastering->input[1];
	float* out1 = mastering->output[0];
	float* out2 = mastering->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= mastering->sampleRate;

	double threshSinew = (0.25 + ((1.0 - *mastering->glue) * 0.333)) / overallscale;
	double depthSinew = 1.0 - pow(1.0 - *mastering->glue, 2.0);

	double trebleZoom = *mastering->scope - 0.5;
	long double trebleGain = (trebleZoom * fabs(trebleZoom)) + 1.0;
	if (trebleGain > 1.0) trebleGain = pow(trebleGain, 3.0 + sqrt(overallscale));
	// this boost is necessary to adapt to higher sample rates

	double midZoom = *mastering->skronk - 0.5;
	long double midGain = (midZoom * fabs(midZoom)) + 1.0;
	double kalMid = 0.35 - (*mastering->skronk * 0.25); // crossover frequency between mid/bass
	double kalSub = 0.45 + (*mastering->skronk * 0.25); // crossover frequency between bass/sub

	double bassZoom = (*mastering->girth * 0.5) - 0.25;
	long double bassGain = (-bassZoom * fabs(bassZoom)) + 1.0; // control inverted
	long double subGain = ((*mastering->girth * 0.25) - 0.125) + 1.0;
	if (subGain < 1.0) subGain = 1.0; // very small sub shift, only pos.

	long double driveIn = (*mastering->drive - 0.5) + 1.0;
	long double driveOut = (-(*mastering->drive - 0.5) * fabs(*mastering->drive - 0.5)) + 1.0;

	int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;
	int dither = ((int) (*mastering->dither));
	if (dither > 5) {
		dither = 5;
	}
	int depth = (int) (17.0 * overallscale);
	if (depth < 3) depth = 3;
	if (depth > 98) depth = 98; // for Dark

	while (sampleFrames-- > 0) {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabsl(inputSampleL) < 1.18e-23) inputSampleL = mastering->fpdL * 1.18e-17;
		if (fabsl(inputSampleR) < 1.18e-23) inputSampleR = mastering->fpdR * 1.18e-17;
		inputSampleL *= driveIn;
		inputSampleR *= driveIn;
		long double drySampleL = inputSampleL;
		long double drySampleR = inputSampleR;

		// begin Air3L
		mastering->air[pvSL4] = mastering->air[pvAL4] - mastering->air[pvAL3];
		mastering->air[pvSL3] = mastering->air[pvAL3] - mastering->air[pvAL2];
		mastering->air[pvSL2] = mastering->air[pvAL2] - mastering->air[pvAL1];
		mastering->air[pvSL1] = mastering->air[pvAL1] - inputSampleL;
		mastering->air[accSL3] = mastering->air[pvSL4] - mastering->air[pvSL3];
		mastering->air[accSL2] = mastering->air[pvSL3] - mastering->air[pvSL2];
		mastering->air[accSL1] = mastering->air[pvSL2] - mastering->air[pvSL1];
		mastering->air[acc2SL2] = mastering->air[accSL3] - mastering->air[accSL2];
		mastering->air[acc2SL1] = mastering->air[accSL2] - mastering->air[accSL1];
		mastering->air[outAL] = -(mastering->air[pvAL1] + mastering->air[pvSL3] + mastering->air[acc2SL2] - ((mastering->air[acc2SL2] + mastering->air[acc2SL1]) * 0.5));
		mastering->air[gainAL] *= 0.5;
		mastering->air[gainAL] += fabsl(drySampleL - mastering->air[outAL]) * 0.5;
		if (mastering->air[gainAL] > 0.3 * sqrt(overallscale)) mastering->air[gainAL] = 0.3 * sqrt(overallscale);
		mastering->air[pvAL4] = mastering->air[pvAL3];
		mastering->air[pvAL3] = mastering->air[pvAL2];
		mastering->air[pvAL2] = mastering->air[pvAL1];
		mastering->air[pvAL1] = (mastering->air[gainAL] * mastering->air[outAL]) + drySampleL;
		long double midL = drySampleL - ((mastering->air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
		long double temp = (midL + mastering->air[gndavgL]) * 0.5;
		mastering->air[gndavgL] = midL;
		midL = temp;
		long double trebleL = drySampleL - midL;
		// end Air3L

		// begin Air3R
		mastering->air[pvSR4] = mastering->air[pvAR4] - mastering->air[pvAR3];
		mastering->air[pvSR3] = mastering->air[pvAR3] - mastering->air[pvAR2];
		mastering->air[pvSR2] = mastering->air[pvAR2] - mastering->air[pvAR1];
		mastering->air[pvSR1] = mastering->air[pvAR1] - inputSampleR;
		mastering->air[accSR3] = mastering->air[pvSR4] - mastering->air[pvSR3];
		mastering->air[accSR2] = mastering->air[pvSR3] - mastering->air[pvSR2];
		mastering->air[accSR1] = mastering->air[pvSR2] - mastering->air[pvSR1];
		mastering->air[acc2SR2] = mastering->air[accSR3] - mastering->air[accSR2];
		mastering->air[acc2SR1] = mastering->air[accSR2] - mastering->air[accSR1];
		mastering->air[outAR] = -(mastering->air[pvAR1] + mastering->air[pvSR3] + mastering->air[acc2SR2] - ((mastering->air[acc2SR2] + mastering->air[acc2SR1]) * 0.5));
		mastering->air[gainAR] *= 0.5;
		mastering->air[gainAR] += fabsl(drySampleR - mastering->air[outAR]) * 0.5;
		if (mastering->air[gainAR] > 0.3 * sqrt(overallscale)) mastering->air[gainAR] = 0.3 * sqrt(overallscale);
		mastering->air[pvAR4] = mastering->air[pvAR3];
		mastering->air[pvAR3] = mastering->air[pvAR2];
		mastering->air[pvAR2] = mastering->air[pvAR1];
		mastering->air[pvAR1] = (mastering->air[gainAR] * mastering->air[outAR]) + drySampleR;
		long double midR = drySampleR - ((mastering->air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
		temp = (midR + mastering->air[gndavgR]) * 0.5;
		mastering->air[gndavgR] = midR;
		midR = temp;
		long double trebleR = drySampleR - midR;
		// end Air3R

		// begin KalmanML
		temp = midL;
		mastering->kalM[prevSlewL3] += mastering->kalM[prevSampL3] - mastering->kalM[prevSampL2];
		mastering->kalM[prevSlewL3] *= 0.5;
		mastering->kalM[prevSlewL2] += mastering->kalM[prevSampL2] - mastering->kalM[prevSampL1];
		mastering->kalM[prevSlewL2] *= 0.5;
		mastering->kalM[prevSlewL1] += mastering->kalM[prevSampL1] - midL;
		mastering->kalM[prevSlewL1] *= 0.5;
		// make slews from each set of samples used
		mastering->kalM[accSlewL2] += mastering->kalM[prevSlewL3] - mastering->kalM[prevSlewL2];
		mastering->kalM[accSlewL2] *= 0.5;
		mastering->kalM[accSlewL1] += mastering->kalM[prevSlewL2] - mastering->kalM[prevSlewL1];
		mastering->kalM[accSlewL1] *= 0.5;
		// differences between slews: rate of change of rate of change
		mastering->kalM[accSlewL3] += (mastering->kalM[accSlewL2] - mastering->kalM[accSlewL1]);
		mastering->kalM[accSlewL3] *= 0.5;
		// entering the abyss, what even is this
		mastering->kalM[kalOutL] += mastering->kalM[prevSampL1] + mastering->kalM[prevSlewL2] + mastering->kalM[accSlewL3];
		mastering->kalM[kalOutL] *= 0.5;

		// resynthesizing predicted result (all iir smoothed)
		mastering->kalM[kalGainL] += fabsl(temp - mastering->kalM[kalOutL]) * kalMid * 8.0;
		mastering->kalM[kalGainL] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (mastering->kalM[kalGainL] > kalMid * 0.5) mastering->kalM[kalGainL] = kalMid * 0.5;
		// attempts to avoid explosions
		mastering->kalM[kalOutL] += (temp * (1.0 - (0.68 + (kalMid * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		mastering->kalM[prevSampL3] = mastering->kalM[prevSampL2];
		mastering->kalM[prevSampL2] = mastering->kalM[prevSampL1];
		mastering->kalM[prevSampL1] = (mastering->kalM[kalGainL] * mastering->kalM[kalOutL]) + ((1.0 - mastering->kalM[kalGainL]) * temp);
		// feed the chain of previous samples
		long double bassL = (mastering->kalM[kalOutL] + mastering->kalM[kalAvgL]) * 0.5;
		mastering->kalM[kalAvgL] = mastering->kalM[kalOutL];
		midL -= bassL;
		// end KalmanML

		// begin KalmanMR
		temp = midR;
		mastering->kalM[prevSlewR3] += mastering->kalM[prevSampR3] - mastering->kalM[prevSampR2];
		mastering->kalM[prevSlewR3] *= 0.5;
		mastering->kalM[prevSlewR2] += mastering->kalM[prevSampR2] - mastering->kalM[prevSampR1];
		mastering->kalM[prevSlewR2] *= 0.5;
		mastering->kalM[prevSlewR1] += mastering->kalM[prevSampR1] - midR;
		mastering->kalM[prevSlewR1] *= 0.5;
		// make slews from each set of samples used
		mastering->kalM[accSlewR2] += mastering->kalM[prevSlewR3] - mastering->kalM[prevSlewR2];
		mastering->kalM[accSlewR2] *= 0.5;
		mastering->kalM[accSlewR1] += mastering->kalM[prevSlewR2] - mastering->kalM[prevSlewR1];
		mastering->kalM[accSlewR1] *= 0.5;
		// differences between slews: rate of change of rate of change
		mastering->kalM[accSlewR3] += (mastering->kalM[accSlewR2] - mastering->kalM[accSlewR1]);
		mastering->kalM[accSlewR3] *= 0.5;
		// entering the abyss, what even is this
		mastering->kalM[kalOutR] += mastering->kalM[prevSampR1] + mastering->kalM[prevSlewR2] + mastering->kalM[accSlewR3];
		mastering->kalM[kalOutR] *= 0.5;

		// resynthesizing predicted result (all iir smoothed)
		mastering->kalM[kalGainR] += fabsl(temp - mastering->kalM[kalOutR]) * kalMid * 8.0;
		mastering->kalM[kalGainR] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (mastering->kalM[kalGainR] > kalMid * 0.5) mastering->kalM[kalGainR] = kalMid * 0.5;
		// attempts to avoid explosions
		mastering->kalM[kalOutR] += (temp * (1.0 - (0.68 + (kalMid * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		mastering->kalM[prevSampR3] = mastering->kalM[prevSampR2];
		mastering->kalM[prevSampR2] = mastering->kalM[prevSampR1];
		mastering->kalM[prevSampR1] = (mastering->kalM[kalGainR] * mastering->kalM[kalOutR]) + ((1.0 - mastering->kalM[kalGainR]) * temp);
		// feed the chain of previous samples
		long double bassR = (mastering->kalM[kalOutR] + mastering->kalM[kalAvgR]) * 0.5;
		mastering->kalM[kalAvgR] = mastering->kalM[kalOutR];
		midR -= bassR;
		// end KalmanMR

		// begin KalmanSL
		temp = bassL;
		mastering->kalS[prevSlewL3] += mastering->kalS[prevSampL3] - mastering->kalS[prevSampL2];
		mastering->kalS[prevSlewL3] *= 0.5;
		mastering->kalS[prevSlewL2] += mastering->kalS[prevSampL2] - mastering->kalS[prevSampL1];
		mastering->kalS[prevSlewL2] *= 0.5;
		mastering->kalS[prevSlewL1] += mastering->kalS[prevSampL1] - bassL;
		mastering->kalS[prevSlewL1] *= 0.5;
		// make slews from each set of samples used
		mastering->kalS[accSlewL2] += mastering->kalS[prevSlewL3] - mastering->kalS[prevSlewL2];
		mastering->kalS[accSlewL2] *= 0.5;
		mastering->kalS[accSlewL1] += mastering->kalS[prevSlewL2] - mastering->kalS[prevSlewL1];
		mastering->kalS[accSlewL1] *= 0.5;
		// differences between slews: rate of change of rate of change
		mastering->kalS[accSlewL3] += (mastering->kalS[accSlewL2] - mastering->kalS[accSlewL1]);
		mastering->kalS[accSlewL3] *= 0.5;
		// entering the abyss, what even is this
		mastering->kalS[kalOutL] += mastering->kalS[prevSampL1] + mastering->kalS[prevSlewL2] + mastering->kalS[accSlewL3];
		mastering->kalS[kalOutL] *= 0.5;
		// resynthesizing predicted result (all iir smoothed)
		mastering->kalS[kalGainL] += fabsl(temp - mastering->kalS[kalOutL]) * kalSub * 8.0;
		mastering->kalS[kalGainL] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (mastering->kalS[kalGainL] > kalSub * 0.5) mastering->kalS[kalGainL] = kalSub * 0.5;
		// attempts to avoid explosions
		mastering->kalS[kalOutL] += (temp * (1.0 - (0.68 + (kalSub * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		mastering->kalS[prevSampL3] = mastering->kalS[prevSampL2];
		mastering->kalS[prevSampL2] = mastering->kalS[prevSampL1];
		mastering->kalS[prevSampL1] = (mastering->kalS[kalGainL] * mastering->kalS[kalOutL]) + ((1.0 - mastering->kalS[kalGainL]) * temp);
		// feed the chain of previous samples
		long double subL = (mastering->kalS[kalOutL] + mastering->kalS[kalAvgL]) * 0.5;
		mastering->kalS[kalAvgL] = mastering->kalS[kalOutL];
		bassL -= subL;
		// end KalmanSL

		// begin KalmanSR
		temp = bassR;
		mastering->kalS[prevSlewR3] += mastering->kalS[prevSampR3] - mastering->kalS[prevSampR2];
		mastering->kalS[prevSlewR3] *= 0.5;
		mastering->kalS[prevSlewR2] += mastering->kalS[prevSampR2] - mastering->kalS[prevSampR1];
		mastering->kalS[prevSlewR2] *= 0.5;
		mastering->kalS[prevSlewR1] += mastering->kalS[prevSampR1] - bassR;
		mastering->kalS[prevSlewR1] *= 0.5;
		// make slews from each set of samples used
		mastering->kalS[accSlewR2] += mastering->kalS[prevSlewR3] - mastering->kalS[prevSlewR2];
		mastering->kalS[accSlewR2] *= 0.5;
		mastering->kalS[accSlewR1] += mastering->kalS[prevSlewR2] - mastering->kalS[prevSlewR1];
		mastering->kalS[accSlewR1] *= 0.5;
		// differences between slews: rate of change of rate of change
		mastering->kalS[accSlewR3] += (mastering->kalS[accSlewR2] - mastering->kalS[accSlewR1]);
		mastering->kalS[accSlewR3] *= 0.5;
		// entering the abyss, what even is this
		mastering->kalS[kalOutR] += mastering->kalS[prevSampR1] + mastering->kalS[prevSlewR2] + mastering->kalS[accSlewR3];
		mastering->kalS[kalOutR] *= 0.5;
		// resynthesizing predicted result (all iir smoothed)
		mastering->kalS[kalGainR] += fabsl(temp - mastering->kalS[kalOutR]) * kalSub * 8.0;
		mastering->kalS[kalGainR] *= 0.5;
		// madness takes its toll. Kalman Gain: how much dry to retain
		if (mastering->kalS[kalGainR] > kalSub * 0.5) mastering->kalS[kalGainR] = kalSub * 0.5;
		// attempts to avoid explosions
		mastering->kalS[kalOutR] += (temp * (1.0 - (0.68 + (kalSub * 0.157))));
		// this is for tuning a really complete cancellation up around Nyquist
		mastering->kalS[prevSampR3] = mastering->kalS[prevSampR2];
		mastering->kalS[prevSampR2] = mastering->kalS[prevSampR1];
		mastering->kalS[prevSampR1] = (mastering->kalS[kalGainR] * mastering->kalS[kalOutR]) + ((1.0 - mastering->kalS[kalGainR]) * temp);
		// feed the chain of previous samples
		long double subR = (mastering->kalS[kalOutR] + mastering->kalS[kalAvgR]) * 0.5;
		mastering->kalS[kalAvgR] = mastering->kalS[kalOutR];
		bassR -= subR;
		// end KalmanSR
		inputSampleL = (subL * subGain);
		inputSampleR = (subR * subGain);

		if (bassZoom > 0.0) {
			double closer = bassL * 1.57079633;
			if (closer > 1.57079633) closer = 1.57079633;
			if (closer < -1.57079633) closer = -1.57079633;
			bassL = (bassL * (1.0 - bassZoom)) + (sin(closer) * bassZoom);
			closer = bassR * 1.57079633;
			if (closer > 1.57079633) closer = 1.57079633;
			if (closer < -1.57079633) closer = -1.57079633;
			bassR = (bassR * (1.0 - bassZoom)) + (sin(closer) * bassZoom);
		} // zooming in will make the body of the sound louder: it's just Density
		if (bassZoom < 0.0) {
			double farther = fabsl(bassL) * 1.57079633;
			if (farther > 1.57079633) farther = 1.0;
			else farther = 1.0 - cos(farther);
			if (bassL > 0.0) bassL = (bassL * (1.0 + bassZoom)) - (farther * bassZoom * 1.57079633);
			if (bassL < 0.0) bassL = (bassL * (1.0 + bassZoom)) + (farther * bassZoom * 1.57079633);
			farther = fabsl(bassR) * 1.57079633;
			if (farther > 1.57079633) farther = 1.0;
			else farther = 1.0 - cos(farther);
			if (bassR > 0.0) bassR = (bassR * (1.0 + bassZoom)) - (farther * bassZoom * 1.57079633);
			if (bassR < 0.0) bassR = (bassR * (1.0 + bassZoom)) + (farther * bassZoom * 1.57079633);
		} // zooming out boosts the hottest peaks but cuts back softer stuff
		inputSampleL += (bassL * bassGain);
		inputSampleR += (bassR * bassGain);

		if (midZoom > 0.0) {
			double closer = midL * 1.57079633;
			if (closer > 1.57079633) closer = 1.57079633;
			if (closer < -1.57079633) closer = -1.57079633;
			midL = (midL * (1.0 - midZoom)) + (sin(closer) * midZoom);
			closer = midR * 1.57079633;
			if (closer > 1.57079633) closer = 1.57079633;
			if (closer < -1.57079633) closer = -1.57079633;
			midR = (midR * (1.0 - midZoom)) + (sin(closer) * midZoom);
		} // zooming in will make the body of the sound louder: it's just Density
		if (midZoom < 0.0) {
			double farther = fabsl(midL) * 1.57079633;
			if (farther > 1.57079633) farther = 1.0;
			else farther = 1.0 - cos(farther);
			if (midL > 0.0) midL = (midL * (1.0 + midZoom)) - (farther * midZoom * 1.57079633);
			if (midL < 0.0) midL = (midL * (1.0 + midZoom)) + (farther * midZoom * 1.57079633);
			farther = fabsl(midR) * 1.57079633;
			if (farther > 1.57079633) farther = 1.0;
			else farther = 1.0 - cos(farther);
			if (midR > 0.0) midR = (midR * (1.0 + midZoom)) - (farther * midZoom * 1.57079633);
			if (midR < 0.0) midR = (midR * (1.0 + midZoom)) + (farther * midZoom * 1.57079633);
		} // zooming out boosts the hottest peaks but cuts back softer stuff
		inputSampleL += (midL * midGain);
		inputSampleR += (midR * midGain);

		if (trebleZoom > 0.0) {
			double closer = trebleL * 1.57079633;
			if (closer > 1.57079633) closer = 1.57079633;
			if (closer < -1.57079633) closer = -1.57079633;
			trebleL = (trebleL * (1.0 - trebleZoom)) + (sin(closer) * trebleZoom);
			closer = trebleR * 1.57079633;
			if (closer > 1.57079633) closer = 1.57079633;
			if (closer < -1.57079633) closer = -1.57079633;
			trebleR = (trebleR * (1.0 - trebleZoom)) + (sin(closer) * trebleZoom);
		} // zooming in will make the body of the sound louder: it's just Density
		if (trebleZoom < 0.0) {
			double farther = fabsl(trebleL) * 1.57079633;
			if (farther > 1.57079633) farther = 1.0;
			else farther = 1.0 - cos(farther);
			if (trebleL > 0.0) trebleL = (trebleL * (1.0 + trebleZoom)) - (farther * trebleZoom * 1.57079633);
			if (trebleL < 0.0) trebleL = (trebleL * (1.0 + trebleZoom)) + (farther * trebleZoom * 1.57079633);
			farther = fabsl(trebleR) * 1.57079633;
			if (farther > 1.57079633) farther = 1.0;
			else farther = 1.0 - cos(farther);
			if (trebleR > 0.0) trebleR = (trebleR * (1.0 + trebleZoom)) - (farther * trebleZoom * 1.57079633);
			if (trebleR < 0.0) trebleR = (trebleR * (1.0 + trebleZoom)) + (farther * trebleZoom * 1.57079633);
		} // zooming out boosts the hottest peaks but cuts back softer stuff
		inputSampleL += (trebleL * trebleGain);
		inputSampleR += (trebleR * trebleGain);

		inputSampleL *= driveOut;
		inputSampleR *= driveOut;

		// begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (mastering->wasPosClipL == true) { // current will be over
			if (inputSampleL < mastering->lastSampleL) mastering->lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
			else mastering->lastSampleL = 0.2491717 + (mastering->lastSampleL * 0.7390851);
		}
		mastering->wasPosClipL = false;
		if (inputSampleL > 0.9549925859) {
			mastering->wasPosClipL = true;
			inputSampleL = 0.7058208 + (mastering->lastSampleL * 0.2609148);
		}
		if (mastering->wasNegClipL == true) { // current will be -over
			if (inputSampleL > mastering->lastSampleL) mastering->lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
			else mastering->lastSampleL = -0.2491717 + (mastering->lastSampleL * 0.7390851);
		}
		mastering->wasNegClipL = false;
		if (inputSampleL < -0.9549925859) {
			mastering->wasNegClipL = true;
			inputSampleL = -0.7058208 + (mastering->lastSampleL * 0.2609148);
		}
		mastering->intermediateL[spacing] = inputSampleL;
		inputSampleL = mastering->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) mastering->intermediateL[x - 1] = mastering->intermediateL[x];
		mastering->lastSampleL = mastering->intermediateL[0]; // run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (mastering->wasPosClipR == true) { // current will be over
			if (inputSampleR < mastering->lastSampleR) mastering->lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
			else mastering->lastSampleR = 0.2491717 + (mastering->lastSampleR * 0.7390851);
		}
		mastering->wasPosClipR = false;
		if (inputSampleR > 0.9549925859) {
			mastering->wasPosClipR = true;
			inputSampleR = 0.7058208 + (mastering->lastSampleR * 0.2609148);
		}
		if (mastering->wasNegClipR == true) { // current will be -over
			if (inputSampleR > mastering->lastSampleR) mastering->lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
			else mastering->lastSampleR = -0.2491717 + (mastering->lastSampleR * 0.7390851);
		}
		mastering->wasNegClipR = false;
		if (inputSampleR < -0.9549925859) {
			mastering->wasNegClipR = true;
			inputSampleR = -0.7058208 + (mastering->lastSampleR * 0.2609148);
		}
		mastering->intermediateR[spacing] = inputSampleR;
		inputSampleR = mastering->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) mastering->intermediateR[x - 1] = mastering->intermediateR[x];
		mastering->lastSampleR = mastering->intermediateR[0]; // run a little buffer to handle this
		// end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code

		temp = inputSampleL;
		long double sinew = threshSinew * cos(mastering->lastSinewL * mastering->lastSinewL);
		if (inputSampleL - mastering->lastSinewL > sinew) temp = mastering->lastSinewL + sinew;
		if (-(inputSampleL - mastering->lastSinewL) > sinew) temp = mastering->lastSinewL - sinew;
		mastering->lastSinewL = temp;
		inputSampleL = (inputSampleL * (1.0 - depthSinew)) + (mastering->lastSinewL * depthSinew);
		temp = inputSampleR;
		sinew = threshSinew * cos(mastering->lastSinewR * mastering->lastSinewR);
		if (inputSampleR - mastering->lastSinewR > sinew) temp = mastering->lastSinewR + sinew;
		if (-(inputSampleR - mastering->lastSinewR) > sinew) temp = mastering->lastSinewR - sinew;
		mastering->lastSinewR = temp;
		inputSampleR = (inputSampleR * (1.0 - depthSinew)) + (mastering->lastSinewR * depthSinew);
		// run Sinew to stop excess slews, but run a dry/wet to allow a range of brights

		switch (dither) {
			case 0:
				// begin Dark
				inputSampleL *= 8388608.0;
				inputSampleR *= 8388608.0; // we will apply the 24 bit Dark
				// We are doing it first Left, then Right, because the loops may run faster if
				// they aren't too jammed full of variables. This means re-running code.

				// begin left
				mastering->quantA = floor(inputSampleL);
				mastering->quantB = floor(inputSampleL + 1.0);
				// to do this style of dither, we quantize in either direction and then
				// do a reconstruction of what the result will be for each choice.
				// We then evaluate which one we like, and keep a history of what we previously had

				mastering->expectedSlew = 0;
				for (int x = 0; x < depth; x++) {
					mastering->expectedSlew += (mastering->darkSampleL[x + 1] - mastering->darkSampleL[x]);
				}
				mastering->expectedSlew /= depth; // we have an average of all recent slews
				// we are doing that to voice the thing down into the upper mids a bit
				// it mustn't just soften the brightest treble, it must smooth high mids too

				mastering->testA = fabs((mastering->darkSampleL[0] - mastering->quantA) - mastering->expectedSlew);
				mastering->testB = fabs((mastering->darkSampleL[0] - mastering->quantB) - mastering->expectedSlew);

				if (mastering->testA < mastering->testB) inputSampleL = mastering->quantA;
				else inputSampleL = mastering->quantB;
				// select whichever one departs LEAST from the vector of averaged
				// reconstructed previous final samples. This will force a kind of dithering
				// as it'll make the output end up as smooth as possible

				for (int x = depth; x >= 0; x--) {
					mastering->darkSampleL[x + 1] = mastering->darkSampleL[x];
				}
				mastering->darkSampleL[0] = inputSampleL;
				// end Dark left

				// begin right
				mastering->quantA = floor(inputSampleR);
				mastering->quantB = floor(inputSampleR + 1.0);
				// to do this style of dither, we quantize in either direction and then
				// do a reconstruction of what the result will be for each choice.
				// We then evaluate which one we like, and keep a history of what we previously had

				mastering->expectedSlew = 0;
				for (int x = 0; x < depth; x++) {
					mastering->expectedSlew += (mastering->darkSampleR[x + 1] - mastering->darkSampleR[x]);
				}
				mastering->expectedSlew /= depth; // we have an average of all recent slews
				// we are doing that to voice the thing down into the upper mids a bit
				// it mustn't just soften the brightest treble, it must smooth high mids too

				mastering->testA = fabs((mastering->darkSampleR[0] - mastering->quantA) - mastering->expectedSlew);
				mastering->testB = fabs((mastering->darkSampleR[0] - mastering->quantB) - mastering->expectedSlew);

				if (mastering->testA < mastering->testB) inputSampleR = mastering->quantA;
				else inputSampleR = mastering->quantB;
				// select whichever one departs LEAST from the vector of averaged
				// reconstructed previous final samples. This will force a kind of dithering
				// as it'll make the output end up as smooth as possible

				for (int x = depth; x >= 0; x--) {
					mastering->darkSampleR[x + 1] = mastering->darkSampleR[x];
				}
				mastering->darkSampleR[0] = inputSampleR;
				// end Dark right

				inputSampleL /= 8388608.0;
				inputSampleR /= 8388608.0;
				break; // Dark (Monitoring2)
			case 1:
				// begin Dark	 for Ten Nines
				inputSampleL *= 8388608.0;
				inputSampleR *= 8388608.0; // we will apply the 24 bit Dark
				// We are doing it first Left, then Right, because the loops may run faster if
				// they aren't too jammed full of variables. This means re-running code.

				// begin L
				mastering->correction = 0;
				if (mastering->flip) {
					mastering->NSOddL = (mastering->NSOddL * 0.9999999999) + mastering->prevShapeL;
					mastering->NSEvenL = (mastering->NSEvenL * 0.9999999999) - mastering->prevShapeL;
					mastering->correction = mastering->NSOddL;
				} else {
					mastering->NSOddL = (mastering->NSOddL * 0.9999999999) - mastering->prevShapeL;
					mastering->NSEvenL = (mastering->NSEvenL * 0.9999999999) + mastering->prevShapeL;
					mastering->correction = mastering->NSEvenL;
				}
				mastering->shapedSampleL = inputSampleL + mastering->correction;
				// end Ten Nines L

				// begin Dark L
				mastering->quantA = floor(mastering->shapedSampleL);
				mastering->quantB = floor(mastering->shapedSampleL + 1.0);
				// to do this style of dither, we quantize in either direction and then
				// do a reconstruction of what the result will be for each choice.
				// We then evaluate which one we like, and keep a history of what we previously had

				mastering->expectedSlew = 0;
				for (int x = 0; x < depth; x++) {
					mastering->expectedSlew += (mastering->darkSampleL[x + 1] - mastering->darkSampleL[x]);
				}
				mastering->expectedSlew /= depth; // we have an average of all recent slews
				// we are doing that to voice the thing down into the upper mids a bit
				// it mustn't just soften the brightest treble, it must smooth high mids too

				mastering->testA = fabs((mastering->darkSampleL[0] - mastering->quantA) - mastering->expectedSlew);
				mastering->testB = fabs((mastering->darkSampleL[0] - mastering->quantB) - mastering->expectedSlew);

				if (mastering->testA < mastering->testB) inputSampleL = mastering->quantA;
				else inputSampleL = mastering->quantB;
				// select whichever one departs LEAST from the vector of averaged
				// reconstructed previous final samples. This will force a kind of dithering
				// as it'll make the output end up as smooth as possible

				for (int x = depth; x >= 0; x--) {
					mastering->darkSampleL[x + 1] = mastering->darkSampleL[x];
				}
				mastering->darkSampleL[0] = inputSampleL;
				// end Dark L

				mastering->prevShapeL = (floor(mastering->shapedSampleL) - inputSampleL) * 0.9999999999;
				// end Ten Nines L

				// begin R
				mastering->correction = 0;
				if (mastering->flip) {
					mastering->NSOddR = (mastering->NSOddR * 0.9999999999) + mastering->prevShapeR;
					mastering->NSEvenR = (mastering->NSEvenR * 0.9999999999) - mastering->prevShapeR;
					mastering->correction = mastering->NSOddR;
				} else {
					mastering->NSOddR = (mastering->NSOddR * 0.9999999999) - mastering->prevShapeR;
					mastering->NSEvenR = (mastering->NSEvenR * 0.9999999999) + mastering->prevShapeR;
					mastering->correction = mastering->NSEvenR;
				}
				mastering->shapedSampleR = inputSampleR + mastering->correction;
				// end Ten Nines R

				// begin Dark R
				mastering->quantA = floor(mastering->shapedSampleR);
				mastering->quantB = floor(mastering->shapedSampleR + 1.0);
				// to do this style of dither, we quantize in either direction and then
				// do a reconstruction of what the result will be for each choice.
				// We then evaluate which one we like, and keep a history of what we previously had

				mastering->expectedSlew = 0;
				for (int x = 0; x < depth; x++) {
					mastering->expectedSlew += (mastering->darkSampleR[x + 1] - mastering->darkSampleR[x]);
				}
				mastering->expectedSlew /= depth; // we have an average of all recent slews
				// we are doing that to voice the thing down into the upper mids a bit
				// it mustn't just soften the brightest treble, it must smooth high mids too

				mastering->testA = fabs((mastering->darkSampleR[0] - mastering->quantA) - mastering->expectedSlew);
				mastering->testB = fabs((mastering->darkSampleR[0] - mastering->quantB) - mastering->expectedSlew);

				if (mastering->testA < mastering->testB) inputSampleR = mastering->quantA;
				else inputSampleR = mastering->quantB;
				// select whichever one departs LEAST from the vector of averaged
				// reconstructed previous final samples. This will force a kind of dithering
				// as it'll make the output end up as smooth as possible

				for (int x = depth; x >= 0; x--) {
					mastering->darkSampleR[x + 1] = mastering->darkSampleR[x];
				}
				mastering->darkSampleR[0] = inputSampleR;
				// end Dark R

				mastering->prevShapeR = (floor(mastering->shapedSampleR) - inputSampleR) * 0.9999999999;
				// end Ten Nines
				mastering->flip = !mastering->flip;

				inputSampleL /= 8388608.0;
				inputSampleR /= 8388608.0;
				break; // Ten Nines (which goes into Dark in Monitoring3)
			case 2:
				inputSampleL *= 8388608.0;
				inputSampleR *= 8388608.0;

				mastering->ditherL = -1.0;
				mastering->ditherL += ((double) mastering->fpdL / UINT32_MAX);
				mastering->fpdL ^= mastering->fpdL << 13;
				mastering->fpdL ^= mastering->fpdL >> 17;
				mastering->fpdL ^= mastering->fpdL << 5;
				mastering->ditherL += ((double) mastering->fpdL / UINT32_MAX);
				// TPDF: two 0-1 random noises

				mastering->ditherR = -1.0;
				mastering->ditherR += ((double) mastering->fpdR / UINT32_MAX);
				mastering->fpdR ^= mastering->fpdR << 13;
				mastering->fpdR ^= mastering->fpdR >> 17;
				mastering->fpdR ^= mastering->fpdR << 5;
				mastering->ditherR += ((double) mastering->fpdR / UINT32_MAX);
				// TPDF: two 0-1 random noises

				if (fabs(mastering->ditherL - mastering->ditherR) < 0.5) {
					mastering->ditherL = -1.0;
					mastering->ditherL += ((double) mastering->fpdL / UINT32_MAX);
					mastering->fpdL ^= mastering->fpdL << 13;
					mastering->fpdL ^= mastering->fpdL >> 17;
					mastering->fpdL ^= mastering->fpdL << 5;
					mastering->ditherL += ((double) mastering->fpdL / UINT32_MAX);
				}

				if (fabs(mastering->ditherL - mastering->ditherR) < 0.5) {
					mastering->ditherR = -1.0;
					mastering->ditherR += ((double) mastering->fpdR / UINT32_MAX);
					mastering->fpdR ^= mastering->fpdR << 13;
					mastering->fpdR ^= mastering->fpdR >> 17;
					mastering->fpdR ^= mastering->fpdR << 5;
					mastering->ditherR += ((double) mastering->fpdR / UINT32_MAX);
				}

				if (fabs(mastering->ditherL - mastering->ditherR) < 0.5) {
					mastering->ditherL = -1.0;
					mastering->ditherL += ((double) mastering->fpdL / UINT32_MAX);
					mastering->fpdL ^= mastering->fpdL << 13;
					mastering->fpdL ^= mastering->fpdL >> 17;
					mastering->fpdL ^= mastering->fpdL << 5;
					mastering->ditherL += ((double) mastering->fpdL / UINT32_MAX);
				}

				inputSampleL = floor(inputSampleL + mastering->ditherL);
				inputSampleR = floor(inputSampleR + mastering->ditherR);

				inputSampleL /= 8388608.0;
				inputSampleR /= 8388608.0;
				break; // TPDFWide (a good neutral with the width enhancement)
			case 3:
				inputSampleL *= 8388608.0;
				inputSampleR *= 8388608.0;
				// Paul Frindle: It's true that the dither itself can sound different
				// if it's given a different freq response and you get to hear it.
				// The one we use most is triangular single pole high pass dither.
				// It's not freq bent enough to sound odd, but is slightly less audible than
				// flat dither. It can also be easily made by taking one sample of dither
				// away from the previous one - this gives you the triangular PDF and the
				// filtering in one go :-)

				mastering->currentDither = ((double) mastering->fpdL / UINT32_MAX);
				mastering->ditherL = mastering->currentDither;
				mastering->ditherL -= mastering->previousDitherL;
				mastering->previousDitherL = mastering->currentDither;
				// TPDF: two 0-1 random noises

				mastering->currentDither = ((double) mastering->fpdR / UINT32_MAX);
				mastering->ditherR = mastering->currentDither;
				mastering->ditherR -= mastering->previousDitherR;
				mastering->previousDitherR = mastering->currentDither;
				// TPDF: two 0-1 random noises

				if (fabs(mastering->ditherL - mastering->ditherR) < 0.5) {
					mastering->fpdL ^= mastering->fpdL << 13;
					mastering->fpdL ^= mastering->fpdL >> 17;
					mastering->fpdL ^= mastering->fpdL << 5;
					mastering->currentDither = ((double) mastering->fpdL / UINT32_MAX);
					mastering->ditherL = mastering->currentDither;
					mastering->ditherL -= mastering->previousDitherL;
					mastering->previousDitherL = mastering->currentDither;
				}

				if (fabs(mastering->ditherL - mastering->ditherR) < 0.5) {
					mastering->fpdR ^= mastering->fpdR << 13;
					mastering->fpdR ^= mastering->fpdR >> 17;
					mastering->fpdR ^= mastering->fpdR << 5;
					mastering->currentDither = ((double) mastering->fpdR / UINT32_MAX);
					mastering->ditherR = mastering->currentDither;
					mastering->ditherR -= mastering->previousDitherR;
					mastering->previousDitherR = mastering->currentDither;
				}

				if (fabs(mastering->ditherL - mastering->ditherR) < 0.5) {
					mastering->fpdL ^= mastering->fpdL << 13;
					mastering->fpdL ^= mastering->fpdL >> 17;
					mastering->fpdL ^= mastering->fpdL << 5;
					mastering->currentDither = ((double) mastering->fpdL / UINT32_MAX);
					mastering->ditherL = mastering->currentDither;
					mastering->ditherL -= mastering->previousDitherL;
					mastering->previousDitherL = mastering->currentDither;
				}

				inputSampleL = floor(inputSampleL + mastering->ditherL);
				inputSampleR = floor(inputSampleR + mastering->ditherR);

				inputSampleL /= 8388608.0;
				inputSampleR /= 8388608.0;
				break; // PaulWide (brighter neutral that's still TPDF and wide)
			case 4:
				inputSampleL *= 8388608.0;
				inputSampleR *= 8388608.0;
				mastering->cutbinsL = false;
				mastering->cutbinsR = false;
				drySampleL = inputSampleL; // re-using in NJAD
				inputSampleL -= mastering->noiseShapingL;
				// NJAD L
				mastering->benfordize = floor(inputSampleL);
				while (mastering->benfordize >= 1.0) mastering->benfordize /= 10;
				while (mastering->benfordize < 1.0 && mastering->benfordize > 0.0000001) mastering->benfordize *= 10;
				mastering->hotbinA = floor(mastering->benfordize);
				// hotbin becomes the Benford bin value for this number floored
				mastering->totalA = 0.0;
				if ((mastering->hotbinA > 0) && (mastering->hotbinA < 10)) {
					mastering->bynL[mastering->hotbinA] += 1;
					if (mastering->bynL[mastering->hotbinA] > 982) mastering->cutbinsL = true;
					mastering->totalA += (301 - mastering->bynL[1]);
					mastering->totalA += (176 - mastering->bynL[2]);
					mastering->totalA += (125 - mastering->bynL[3]);
					mastering->totalA += (97 - mastering->bynL[4]);
					mastering->totalA += (79 - mastering->bynL[5]);
					mastering->totalA += (67 - mastering->bynL[6]);
					mastering->totalA += (58 - mastering->bynL[7]);
					mastering->totalA += (51 - mastering->bynL[8]);
					mastering->totalA += (46 - mastering->bynL[9]);
					mastering->bynL[mastering->hotbinA] -= 1;
				} else mastering->hotbinA = 10;
				// produce total number- smaller is closer to Benford real
				mastering->benfordize = ceil(inputSampleL);
				while (mastering->benfordize >= 1.0) mastering->benfordize /= 10;
				while (mastering->benfordize < 1.0 && mastering->benfordize > 0.0000001) mastering->benfordize *= 10;
				mastering->hotbinB = floor(mastering->benfordize);
				// hotbin becomes the Benford bin value for this number ceiled
				mastering->totalB = 0.0;
				if ((mastering->hotbinB > 0) && (mastering->hotbinB < 10)) {
					mastering->bynL[mastering->hotbinB] += 1;
					if (mastering->bynL[mastering->hotbinB] > 982) mastering->cutbinsL = true;
					mastering->totalB += (301 - mastering->bynL[1]);
					mastering->totalB += (176 - mastering->bynL[2]);
					mastering->totalB += (125 - mastering->bynL[3]);
					mastering->totalB += (97 - mastering->bynL[4]);
					mastering->totalB += (79 - mastering->bynL[5]);
					mastering->totalB += (67 - mastering->bynL[6]);
					mastering->totalB += (58 - mastering->bynL[7]);
					mastering->totalB += (51 - mastering->bynL[8]);
					mastering->totalB += (46 - mastering->bynL[9]);
					mastering->bynL[mastering->hotbinB] -= 1;
				} else mastering->hotbinB = 10;
				// produce total number- smaller is closer to Benford real
				if (mastering->totalA < mastering->totalB) {
					mastering->bynL[mastering->hotbinA] += 1;
					mastering->outputSample = floor(inputSampleL);
				} else {
					mastering->bynL[mastering->hotbinB] += 1;
					mastering->outputSample = floor(inputSampleL + 1);
				}
				// assign the relevant one to the delay line
				// and floor/ceil signal accordingly
				if (mastering->cutbinsL) {
					mastering->bynL[1] *= 0.99;
					mastering->bynL[2] *= 0.99;
					mastering->bynL[3] *= 0.99;
					mastering->bynL[4] *= 0.99;
					mastering->bynL[5] *= 0.99;
					mastering->bynL[6] *= 0.99;
					mastering->bynL[7] *= 0.99;
					mastering->bynL[8] *= 0.99;
					mastering->bynL[9] *= 0.99;
					mastering->bynL[10] *= 0.99;
				}
				mastering->noiseShapingL += mastering->outputSample - drySampleL;
				if (mastering->noiseShapingL > fabsl(inputSampleL)) mastering->noiseShapingL = fabsl(inputSampleL);
				if (mastering->noiseShapingL < -fabsl(inputSampleL)) mastering->noiseShapingL = -fabsl(inputSampleL);
				inputSampleL /= 8388608.0;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				// finished NJAD L

				// NJAD R
				drySampleR = inputSampleR;
				inputSampleR -= mastering->noiseShapingR;
				mastering->benfordize = floor(inputSampleR);
				while (mastering->benfordize >= 1.0) mastering->benfordize /= 10;
				while (mastering->benfordize < 1.0 && mastering->benfordize > 0.0000001) mastering->benfordize *= 10;
				mastering->hotbinA = floor(mastering->benfordize);
				// hotbin becomes the Benford bin value for this number floored
				mastering->totalA = 0.0;
				if ((mastering->hotbinA > 0) && (mastering->hotbinA < 10)) {
					mastering->bynR[mastering->hotbinA] += 1;
					if (mastering->bynR[mastering->hotbinA] > 982) mastering->cutbinsR = true;
					mastering->totalA += (301 - mastering->bynR[1]);
					mastering->totalA += (176 - mastering->bynR[2]);
					mastering->totalA += (125 - mastering->bynR[3]);
					mastering->totalA += (97 - mastering->bynR[4]);
					mastering->totalA += (79 - mastering->bynR[5]);
					mastering->totalA += (67 - mastering->bynR[6]);
					mastering->totalA += (58 - mastering->bynR[7]);
					mastering->totalA += (51 - mastering->bynR[8]);
					mastering->totalA += (46 - mastering->bynR[9]);
					mastering->bynR[mastering->hotbinA] -= 1;
				} else mastering->hotbinA = 10;
				// produce total number- smaller is closer to Benford real
				mastering->benfordize = ceil(inputSampleR);
				while (mastering->benfordize >= 1.0) mastering->benfordize /= 10;
				while (mastering->benfordize < 1.0 && mastering->benfordize > 0.0000001) mastering->benfordize *= 10;
				mastering->hotbinB = floor(mastering->benfordize);
				// hotbin becomes the Benford bin value for this number ceiled
				mastering->totalB = 0.0;
				if ((mastering->hotbinB > 0) && (mastering->hotbinB < 10)) {
					mastering->bynR[mastering->hotbinB] += 1;
					if (mastering->bynR[mastering->hotbinB] > 982) mastering->cutbinsR = true;
					mastering->totalB += (301 - mastering->bynR[1]);
					mastering->totalB += (176 - mastering->bynR[2]);
					mastering->totalB += (125 - mastering->bynR[3]);
					mastering->totalB += (97 - mastering->bynR[4]);
					mastering->totalB += (79 - mastering->bynR[5]);
					mastering->totalB += (67 - mastering->bynR[6]);
					mastering->totalB += (58 - mastering->bynR[7]);
					mastering->totalB += (51 - mastering->bynR[8]);
					mastering->totalB += (46 - mastering->bynR[9]);
					mastering->bynR[mastering->hotbinB] -= 1;
				} else mastering->hotbinB = 10;
				// produce total number- smaller is closer to Benford real
				if (mastering->totalA < mastering->totalB) {
					mastering->bynR[mastering->hotbinA] += 1;
					mastering->outputSample = floor(inputSampleR);
				} else {
					mastering->bynR[mastering->hotbinB] += 1;
					mastering->outputSample = floor(inputSampleR + 1);
				}
				// assign the relevant one to the delay line
				// and floor/ceil signal accordingly
				if (mastering->cutbinsR) {
					mastering->bynR[1] *= 0.99;
					mastering->bynR[2] *= 0.99;
					mastering->bynR[3] *= 0.99;
					mastering->bynR[4] *= 0.99;
					mastering->bynR[5] *= 0.99;
					mastering->bynR[6] *= 0.99;
					mastering->bynR[7] *= 0.99;
					mastering->bynR[8] *= 0.99;
					mastering->bynR[9] *= 0.99;
					mastering->bynR[10] *= 0.99;
				}
				mastering->noiseShapingR += mastering->outputSample - drySampleR;
				if (mastering->noiseShapingR > fabsl(inputSampleR)) mastering->noiseShapingR = fabsl(inputSampleR);
				if (mastering->noiseShapingR < -fabsl(inputSampleR)) mastering->noiseShapingR = -fabsl(inputSampleR);
				inputSampleR /= 8388608.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				break; // NJAD (Monitoring. Brightest)
			case 5:
				// begin 32 bit stereo floating point dither
				frexpf((float) inputSampleL, &mastering->expon);
				mastering->fpdL ^= mastering->fpdL << 13;
				mastering->fpdL ^= mastering->fpdL >> 17;
				mastering->fpdL ^= mastering->fpdL << 5;
				inputSampleL += (((double) mastering->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, mastering->expon + 62));
				frexpf((float) inputSampleR, &mastering->expon);
				mastering->fpdR ^= mastering->fpdR << 13;
				mastering->fpdR ^= mastering->fpdR >> 17;
				mastering->fpdR ^= mastering->fpdR << 5;
				inputSampleR += (((double) mastering->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, mastering->expon + 62));
				// end 32 bit stereo floating point dither
				break; // Bypass for saving floating point files directly
		}

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
	MASTERING_URI,
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
