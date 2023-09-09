#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define TOTAPE5_URI "https://hannesbraun.net/ns/lv2/airwindows/totape5"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	LOUDER = 4,
	SOFTER = 5,
	FATTER = 6,
	FLUTTER = 7,
	OUTPUT = 8,
	DRY_WET = 9
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* louder;
	const float* softer;
	const float* fatter;
	const float* flutter;
	const float* outputGain;
	const float* dryWet;

	int gcount;
	double rateof;
	double sweep;
	double nextmax;
	int hcount;
	int flip;

	double dL[1000];
	double eL[1000];
	double dR[1000];
	double eR[1000];

	double iirMidRollerAL;
	double iirMidRollerBL;
	double iirMidRollerCL;
	double iirHeadBumpAL;
	double iirHeadBumpBL;
	double iirHeadBumpCL;
	double iirMinHeadBumpL;

	double iirMidRollerAR;
	double iirMidRollerBR;
	double iirMidRollerCR;
	double iirHeadBumpAR;
	double iirHeadBumpBR;
	double iirHeadBumpCR;
	double iirMinHeadBumpR;

	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double iirSampleIL;
	double iirSampleJL;
	double iirSampleKL;
	double iirSampleLL;
	double iirSampleML;
	double iirSampleNL;
	double iirSampleOL;
	double iirSamplePL;
	double iirSampleQL;
	double iirSampleRL;
	double iirSampleSL;
	double iirSampleTL;
	double iirSampleUL;
	double iirSampleVL;
	double iirSampleWL;
	double iirSampleXL;
	double iirSampleYL;
	double iirSampleZL;

	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double iirSampleGR;
	double iirSampleHR;
	double iirSampleIR;
	double iirSampleJR;
	double iirSampleKR;
	double iirSampleLR;
	double iirSampleMR;
	double iirSampleNR;
	double iirSampleOR;
	double iirSamplePR;
	double iirSampleQR;
	double iirSampleRR;
	double iirSampleSR;
	double iirSampleTR;
	double iirSampleUR;
	double iirSampleVR;
	double iirSampleWR;
	double iirSampleXR;
	double iirSampleYR;
	double iirSampleZR;

	uint32_t fpdL;
	uint32_t fpdR;
} ToTape5;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ToTape5* toTape5 = (ToTape5*) calloc(1, sizeof(ToTape5));
	toTape5->sampleRate = rate;
	return (LV2_Handle) toTape5;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ToTape5* toTape5 = (ToTape5*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			toTape5->input[0] = (const float*) data;
			break;
		case INPUT_R:
			toTape5->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			toTape5->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			toTape5->output[1] = (float*) data;
			break;
		case LOUDER:
			toTape5->louder = (const float*) data;
			break;
		case SOFTER:
			toTape5->softer = (const float*) data;
			break;
		case FATTER:
			toTape5->fatter = (const float*) data;
			break;
		case FLUTTER:
			toTape5->flutter = (const float*) data;
			break;
		case OUTPUT:
			toTape5->outputGain = (const float*) data;
			break;
		case DRY_WET:
			toTape5->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ToTape5* toTape5 = (ToTape5*) instance;

	toTape5->iirMidRollerAL = 0.0;
	toTape5->iirMidRollerBL = 0.0;
	toTape5->iirMidRollerCL = 0.0;
	toTape5->iirHeadBumpAL = 0.0;
	toTape5->iirHeadBumpBL = 0.0;
	toTape5->iirHeadBumpCL = 0.0;
	toTape5->iirMinHeadBumpL = 0.0;

	toTape5->iirMidRollerAR = 0.0;
	toTape5->iirMidRollerBR = 0.0;
	toTape5->iirMidRollerCR = 0.0;
	toTape5->iirHeadBumpAR = 0.0;
	toTape5->iirHeadBumpBR = 0.0;
	toTape5->iirHeadBumpCR = 0.0;
	toTape5->iirMinHeadBumpR = 0.0;

	toTape5->iirSampleAL = 0.0;
	toTape5->iirSampleBL = 0.0;
	toTape5->iirSampleCL = 0.0;
	toTape5->iirSampleDL = 0.0;
	toTape5->iirSampleEL = 0.0;
	toTape5->iirSampleFL = 0.0;
	toTape5->iirSampleGL = 0.0;
	toTape5->iirSampleHL = 0.0;
	toTape5->iirSampleIL = 0.0;
	toTape5->iirSampleJL = 0.0;
	toTape5->iirSampleKL = 0.0;
	toTape5->iirSampleLL = 0.0;
	toTape5->iirSampleML = 0.0;
	toTape5->iirSampleNL = 0.0;
	toTape5->iirSampleOL = 0.0;
	toTape5->iirSamplePL = 0.0;
	toTape5->iirSampleQL = 0.0;
	toTape5->iirSampleRL = 0.0;
	toTape5->iirSampleSL = 0.0;
	toTape5->iirSampleTL = 0.0;
	toTape5->iirSampleUL = 0.0;
	toTape5->iirSampleVL = 0.0;
	toTape5->iirSampleWL = 0.0;
	toTape5->iirSampleXL = 0.0;
	toTape5->iirSampleYL = 0.0;
	toTape5->iirSampleZL = 0.0;

	toTape5->iirSampleAR = 0.0;
	toTape5->iirSampleBR = 0.0;
	toTape5->iirSampleCR = 0.0;
	toTape5->iirSampleDR = 0.0;
	toTape5->iirSampleER = 0.0;
	toTape5->iirSampleFR = 0.0;
	toTape5->iirSampleGR = 0.0;
	toTape5->iirSampleHR = 0.0;
	toTape5->iirSampleIR = 0.0;
	toTape5->iirSampleJR = 0.0;
	toTape5->iirSampleKR = 0.0;
	toTape5->iirSampleLR = 0.0;
	toTape5->iirSampleMR = 0.0;
	toTape5->iirSampleNR = 0.0;
	toTape5->iirSampleOR = 0.0;
	toTape5->iirSamplePR = 0.0;
	toTape5->iirSampleQR = 0.0;
	toTape5->iirSampleRR = 0.0;
	toTape5->iirSampleSR = 0.0;
	toTape5->iirSampleTR = 0.0;
	toTape5->iirSampleUR = 0.0;
	toTape5->iirSampleVR = 0.0;
	toTape5->iirSampleWR = 0.0;
	toTape5->iirSampleXR = 0.0;
	toTape5->iirSampleYR = 0.0;
	toTape5->iirSampleZR = 0.0;

	for (int temp = 0; temp < 999; temp++) {
		toTape5->dL[temp] = 0.0;
		toTape5->eL[temp] = 0.0;
		toTape5->dR[temp] = 0.0;
		toTape5->eR[temp] = 0.0;
	}

	toTape5->gcount = 0;
	toTape5->rateof = 0.5;
	toTape5->sweep = 0.0;
	toTape5->nextmax = 0.5;
	toTape5->hcount = 0;
	toTape5->flip = 0;

	toTape5->fpdL = 1.0;
	while (toTape5->fpdL < 16386) toTape5->fpdL = rand() * UINT32_MAX;
	toTape5->fpdR = 1.0;
	while (toTape5->fpdR < 16386) toTape5->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ToTape5* toTape5 = (ToTape5*) instance;

	const float* in1 = toTape5->input[0];
	const float* in2 = toTape5->input[1];
	float* out1 = toTape5->output[0];
	float* out2 = toTape5->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= toTape5->sampleRate;
	double fpOld = 0.618033988749894848204586; // golden ratio!
	double inputgain = pow(*toTape5->louder + 1.0, 3);
	double outputgain = *toTape5->outputGain;
	double wet = *toTape5->dryWet;
	// removed extra dry variable
	double trim = 0.211324865405187117745425;
	double SoftenControl = pow(*toTape5->softer, 2);
	double tempRandy = 0.06 + (SoftenControl / 10.0);
	double RollAmount = (1.0 - (SoftenControl * 0.45)) / overallscale;
	double HeadBumpControl = pow(*toTape5->fatter, 2);
	int allpasstemp;
	int maxdelay = (int) (floor(((HeadBumpControl + 0.3) * 2.2) * overallscale));
	HeadBumpControl *= fabs(HeadBumpControl);
	double HeadBumpFreq = 0.044 / overallscale;
	double iirAmount = 0.000001 / overallscale;
	double altAmount = 1.0 - iirAmount;
	double iirHBoostAmount = 0.0001 / overallscale;
	double altHBoostAmount = 1.0 - iirAmount;
	double depth = pow(*toTape5->flutter, 2) * overallscale;
	double fluttertrim = 0.005 / overallscale;
	double sweeptrim = (0.0006 * depth) / overallscale;
	double offset;
	double tupi = 3.141592653589793238 * 2.0;
	double newrate = 0.005 / overallscale;
	double oldrate = 1.0 - newrate;
	double flutterrandy;
	double randy;
	double invrandy;
	int count;

	double HighsSampleL = 0.0;
	double NonHighsSampleL = 0.0;
	double HeadBumpL = 0.0;
	double SubtractL;
	double bridgerectifierL;
	double tempSampleL;
	double drySampleL;

	double HighsSampleR = 0.0;
	double NonHighsSampleR = 0.0;
	double HeadBumpR = 0.0;
	double SubtractR;
	double bridgerectifierR;
	double tempSampleR;
	double drySampleR;

	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = toTape5->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = toTape5->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		flutterrandy = ((double) toTape5->fpdL / UINT32_MAX);
		randy = flutterrandy * tempRandy; // for soften
		invrandy = (1.0 - randy);
		randy /= 2.0;
		// we've set up so that we dial in the amount of the alt sections (in pairs) with invrandy being the source section

		// now we've got a random flutter, so we're messing with the pitch before tape effects go on
		if (toTape5->gcount < 0 || toTape5->gcount > 300) {
			toTape5->gcount = 300;
		}
		count = toTape5->gcount;
		toTape5->dL[count + 301] = toTape5->dL[count] = inputSampleL;
		toTape5->dR[count + 301] = toTape5->dR[count] = inputSampleR;
		toTape5->gcount--;
		// we will also keep the buffer going, even when not in use

		if (depth != 0.0) {
			offset = (1.0 + sin(toTape5->sweep)) * depth;
			count += (int) floor(offset);

			bridgerectifierL = (toTape5->dL[count] * (1 - (offset - floor(offset))));
			bridgerectifierL += (toTape5->dL[count + 1] * (offset - floor(offset)));
			bridgerectifierL -= ((toTape5->dL[count + 2] * (offset - floor(offset))) * trim);

			bridgerectifierR = (toTape5->dR[count] * (1 - (offset - floor(offset))));
			bridgerectifierR += (toTape5->dR[count + 1] * (offset - floor(offset)));
			bridgerectifierR -= ((toTape5->dR[count + 2] * (offset - floor(offset))) * trim);

			toTape5->rateof = (toTape5->nextmax * newrate) + (toTape5->rateof * oldrate);
			toTape5->sweep += toTape5->rateof * fluttertrim;
			toTape5->sweep += toTape5->sweep * sweeptrim;
			if (toTape5->sweep >= tupi) {
				toTape5->sweep = 0.0;
				toTape5->nextmax = 0.02 + (flutterrandy * 0.98);
			}
			inputSampleL = bridgerectifierL;
			inputSampleR = bridgerectifierR;
			// apply to input signal only when flutter is present, interpolate samples
		}

		if (inputgain != 1.0) {
			inputSampleL *= inputgain;
			inputSampleR *= inputgain;
		}

		if (toTape5->flip < 1 || toTape5->flip > 3) toTape5->flip = 1;
		switch (toTape5->flip) {
			case 1:
				toTape5->iirMidRollerAL = (toTape5->iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
				toTape5->iirMidRollerAL = (invrandy * toTape5->iirMidRollerAL) + (randy * toTape5->iirMidRollerBL) + (randy * toTape5->iirMidRollerCL);
				HighsSampleL = inputSampleL - toTape5->iirMidRollerAL;
				NonHighsSampleL = toTape5->iirMidRollerAL;

				toTape5->iirHeadBumpAL += (inputSampleL * 0.05);
				toTape5->iirHeadBumpAL -= (toTape5->iirHeadBumpAL * toTape5->iirHeadBumpAL * toTape5->iirHeadBumpAL * HeadBumpFreq);
				toTape5->iirHeadBumpAL = (invrandy * toTape5->iirHeadBumpAL) + (randy * toTape5->iirHeadBumpBL) + (randy * toTape5->iirHeadBumpCL);

				toTape5->iirMidRollerAR = (toTape5->iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
				toTape5->iirMidRollerAR = (invrandy * toTape5->iirMidRollerAR) + (randy * toTape5->iirMidRollerBR) + (randy * toTape5->iirMidRollerCR);
				HighsSampleR = inputSampleR - toTape5->iirMidRollerAR;
				NonHighsSampleR = toTape5->iirMidRollerAR;

				toTape5->iirHeadBumpAR += (inputSampleR * 0.05);
				toTape5->iirHeadBumpAR -= (toTape5->iirHeadBumpAR * toTape5->iirHeadBumpAR * toTape5->iirHeadBumpAR * HeadBumpFreq);
				toTape5->iirHeadBumpAR = (invrandy * toTape5->iirHeadBumpAR) + (randy * toTape5->iirHeadBumpBR) + (randy * toTape5->iirHeadBumpCR);
				break;
			case 2:
				toTape5->iirMidRollerBL = (toTape5->iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
				toTape5->iirMidRollerBL = (randy * toTape5->iirMidRollerAL) + (invrandy * toTape5->iirMidRollerBL) + (randy * toTape5->iirMidRollerCL);
				HighsSampleL = inputSampleL - toTape5->iirMidRollerBL;
				NonHighsSampleL = toTape5->iirMidRollerBL;

				toTape5->iirHeadBumpBL += (inputSampleL * 0.05);
				toTape5->iirHeadBumpBL -= (toTape5->iirHeadBumpBL * toTape5->iirHeadBumpBL * toTape5->iirHeadBumpBL * HeadBumpFreq);
				toTape5->iirHeadBumpBL = (randy * toTape5->iirHeadBumpAL) + (invrandy * toTape5->iirHeadBumpBL) + (randy * toTape5->iirHeadBumpCL);

				toTape5->iirMidRollerBR = (toTape5->iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
				toTape5->iirMidRollerBR = (randy * toTape5->iirMidRollerAR) + (invrandy * toTape5->iirMidRollerBR) + (randy * toTape5->iirMidRollerCR);
				HighsSampleR = inputSampleR - toTape5->iirMidRollerBR;
				NonHighsSampleR = toTape5->iirMidRollerBR;

				toTape5->iirHeadBumpBR += (inputSampleR * 0.05);
				toTape5->iirHeadBumpBR -= (toTape5->iirHeadBumpBR * toTape5->iirHeadBumpBR * toTape5->iirHeadBumpBR * HeadBumpFreq);
				toTape5->iirHeadBumpBR = (randy * toTape5->iirHeadBumpAR) + (invrandy * toTape5->iirHeadBumpBR) + (randy * toTape5->iirHeadBumpCR);
				break;
			case 3:
				toTape5->iirMidRollerCL = (toTape5->iirMidRollerCL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
				toTape5->iirMidRollerCL = (randy * toTape5->iirMidRollerAL) + (randy * toTape5->iirMidRollerBL) + (invrandy * toTape5->iirMidRollerCL);
				HighsSampleL = inputSampleL - toTape5->iirMidRollerCL;
				NonHighsSampleL = toTape5->iirMidRollerCL;

				toTape5->iirHeadBumpCL += (inputSampleL * 0.05);
				toTape5->iirHeadBumpCL -= (toTape5->iirHeadBumpCL * toTape5->iirHeadBumpCL * toTape5->iirHeadBumpCL * HeadBumpFreq);
				toTape5->iirHeadBumpCL = (randy * toTape5->iirHeadBumpAL) + (randy * toTape5->iirHeadBumpBL) + (invrandy * toTape5->iirHeadBumpCL);

				toTape5->iirMidRollerCR = (toTape5->iirMidRollerCR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
				toTape5->iirMidRollerCR = (randy * toTape5->iirMidRollerAR) + (randy * toTape5->iirMidRollerBR) + (invrandy * toTape5->iirMidRollerCR);
				HighsSampleR = inputSampleR - toTape5->iirMidRollerCR;
				NonHighsSampleR = toTape5->iirMidRollerCR;

				toTape5->iirHeadBumpCR += (inputSampleR * 0.05);
				toTape5->iirHeadBumpCR -= (toTape5->iirHeadBumpCR * toTape5->iirHeadBumpCR * toTape5->iirHeadBumpCR * HeadBumpFreq);
				toTape5->iirHeadBumpCR = (randy * toTape5->iirHeadBumpAR) + (randy * toTape5->iirHeadBumpBR) + (invrandy * toTape5->iirHeadBumpCR);
				break;
		}
		toTape5->flip++; // increment the triplet counter

		SubtractL = HighsSampleL;
		bridgerectifierL = fabs(SubtractL) * 1.57079633;
		if (bridgerectifierL > 1.57079633) bridgerectifierL = 1.57079633;
		bridgerectifierL = 1 - cos(bridgerectifierL);
		if (SubtractL > 0) SubtractL = bridgerectifierL;
		if (SubtractL < 0) SubtractL = -bridgerectifierL;
		inputSampleL -= SubtractL;

		SubtractR = HighsSampleR;
		bridgerectifierR = fabs(SubtractR) * 1.57079633;
		if (bridgerectifierR > 1.57079633) bridgerectifierR = 1.57079633;
		bridgerectifierR = 1 - cos(bridgerectifierR);
		if (SubtractR > 0) SubtractR = bridgerectifierR;
		if (SubtractR < 0) SubtractR = -bridgerectifierR;
		inputSampleR -= SubtractR;
		// Soften works using the MidRoller stuff, defining a bright parallel channel that we apply negative Density
		// to, and then subtract from the main audio. That makes the 'highs channel subtract' hit only the loudest
		// transients, plus we are subtracting any artifacts we got from the negative Density.

		bridgerectifierL = fabs(inputSampleL);
		if (bridgerectifierL > 1.57079633) bridgerectifierL = 1.57079633;
		bridgerectifierL = sin(bridgerectifierL);
		if (inputSampleL > 0) inputSampleL = bridgerectifierL;
		if (inputSampleL < 0) inputSampleL = -bridgerectifierL;

		bridgerectifierR = fabs(inputSampleR);
		if (bridgerectifierR > 1.57079633) bridgerectifierR = 1.57079633;
		bridgerectifierR = sin(bridgerectifierR);
		if (inputSampleR > 0) inputSampleR = bridgerectifierR;
		if (inputSampleR < 0) inputSampleR = -bridgerectifierR;
		// drive section: the tape sound includes a very gentle saturation curve, which is always an attenuation.
		// we cut back on highs before hitting this, and then we're going to subtract highs a second time after.

		HeadBumpL = toTape5->iirHeadBumpAL + toTape5->iirHeadBumpBL + toTape5->iirHeadBumpCL;
		HeadBumpR = toTape5->iirHeadBumpAR + toTape5->iirHeadBumpBR + toTape5->iirHeadBumpCR;
		// begin PhaseNudge
		allpasstemp = toTape5->hcount - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay) {
			allpasstemp = maxdelay;
		}

		HeadBumpL -= toTape5->eL[allpasstemp] * fpOld;
		toTape5->eL[toTape5->hcount] = HeadBumpL;
		inputSampleL *= fpOld;

		HeadBumpR -= toTape5->eR[allpasstemp] * fpOld;
		toTape5->eR[toTape5->hcount] = HeadBumpR;
		inputSampleR *= fpOld;

		toTape5->hcount--;
		if (toTape5->hcount < 0 || toTape5->hcount > maxdelay) {
			toTape5->hcount = maxdelay;
		}
		HeadBumpL += (toTape5->eL[toTape5->hcount]);
		HeadBumpR += (toTape5->eR[toTape5->hcount]);
		// end PhaseNudge on head bump in lieu of delay.
		SubtractL -= (HeadBumpL * (HeadBumpControl + toTape5->iirMinHeadBumpL));
		SubtractR -= (HeadBumpR * (HeadBumpControl + toTape5->iirMinHeadBumpR));
		// makes a second soften and a single head bump after saturation.
		// we are going to retain this, and then feed it into the highpass filter. That way, we can skip a subtract.
		// Head Bump retains a trace which is roughly as large as what the highpass will do.

		tempSampleL = inputSampleL;
		tempSampleR = inputSampleR;

		toTape5->iirMinHeadBumpL = (toTape5->iirMinHeadBumpL * altHBoostAmount) + (fabs(inputSampleL) * iirHBoostAmount);
		if (toTape5->iirMinHeadBumpL > 0.01) toTape5->iirMinHeadBumpL = 0.01;

		toTape5->iirMinHeadBumpR = (toTape5->iirMinHeadBumpR * altHBoostAmount) + (fabs(inputSampleR) * iirHBoostAmount);
		if (toTape5->iirMinHeadBumpR > 0.01) toTape5->iirMinHeadBumpR = 0.01;
		// we want this one rectified so that it's a relatively steady positive value. Boosts can cause it to be
		// greater than 1 so we clamp it in that case.

		toTape5->iirSampleAL = (toTape5->iirSampleAL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleAL;
		SubtractL += toTape5->iirSampleAL;
		toTape5->iirSampleBL = (toTape5->iirSampleBL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleBL;
		SubtractL += toTape5->iirSampleBL;
		toTape5->iirSampleCL = (toTape5->iirSampleCL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleCL;
		SubtractL += toTape5->iirSampleCL;
		toTape5->iirSampleDL = (toTape5->iirSampleDL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleDL;
		SubtractL += toTape5->iirSampleDL;
		toTape5->iirSampleEL = (toTape5->iirSampleEL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleEL;
		SubtractL += toTape5->iirSampleEL;
		toTape5->iirSampleFL = (toTape5->iirSampleFL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleFL;
		SubtractL += toTape5->iirSampleFL;
		toTape5->iirSampleGL = (toTape5->iirSampleGL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleGL;
		SubtractL += toTape5->iirSampleGL;
		toTape5->iirSampleHL = (toTape5->iirSampleHL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleHL;
		SubtractL += toTape5->iirSampleHL;
		toTape5->iirSampleIL = (toTape5->iirSampleIL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleIL;
		SubtractL += toTape5->iirSampleIL;
		toTape5->iirSampleJL = (toTape5->iirSampleJL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleJL;
		SubtractL += toTape5->iirSampleJL;
		toTape5->iirSampleKL = (toTape5->iirSampleKL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleKL;
		SubtractL += toTape5->iirSampleKL;
		toTape5->iirSampleLL = (toTape5->iirSampleLL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleLL;
		SubtractL += toTape5->iirSampleLL;
		toTape5->iirSampleML = (toTape5->iirSampleML * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleML;
		SubtractL += toTape5->iirSampleML;
		toTape5->iirSampleNL = (toTape5->iirSampleNL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleNL;
		SubtractL += toTape5->iirSampleNL;
		toTape5->iirSampleOL = (toTape5->iirSampleOL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleOL;
		SubtractL += toTape5->iirSampleOL;
		toTape5->iirSamplePL = (toTape5->iirSamplePL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSamplePL;
		SubtractL += toTape5->iirSamplePL;
		toTape5->iirSampleQL = (toTape5->iirSampleQL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleQL;
		SubtractL += toTape5->iirSampleQL;
		toTape5->iirSampleRL = (toTape5->iirSampleRL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleRL;
		SubtractL += toTape5->iirSampleRL;
		toTape5->iirSampleSL = (toTape5->iirSampleSL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleSL;
		SubtractL += toTape5->iirSampleSL;
		toTape5->iirSampleTL = (toTape5->iirSampleTL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleTL;
		SubtractL += toTape5->iirSampleTL;
		toTape5->iirSampleUL = (toTape5->iirSampleUL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleUL;
		SubtractL += toTape5->iirSampleUL;
		toTape5->iirSampleVL = (toTape5->iirSampleVL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleVL;
		SubtractL += toTape5->iirSampleVL;
		toTape5->iirSampleWL = (toTape5->iirSampleWL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleWL;
		SubtractL += toTape5->iirSampleWL;
		toTape5->iirSampleXL = (toTape5->iirSampleXL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleXL;
		SubtractL += toTape5->iirSampleXL;
		toTape5->iirSampleYL = (toTape5->iirSampleYL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleYL;
		SubtractL += toTape5->iirSampleYL;
		toTape5->iirSampleZL = (toTape5->iirSampleZL * altAmount) + (tempSampleL * iirAmount);
		tempSampleL -= toTape5->iirSampleZL;
		SubtractL += toTape5->iirSampleZL;

		toTape5->iirSampleAR = (toTape5->iirSampleAR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleAR;
		SubtractR += toTape5->iirSampleAR;
		toTape5->iirSampleBR = (toTape5->iirSampleBR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleBR;
		SubtractR += toTape5->iirSampleBR;
		toTape5->iirSampleCR = (toTape5->iirSampleCR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleCR;
		SubtractR += toTape5->iirSampleCR;
		toTape5->iirSampleDR = (toTape5->iirSampleDR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleDR;
		SubtractR += toTape5->iirSampleDR;
		toTape5->iirSampleER = (toTape5->iirSampleER * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleER;
		SubtractR += toTape5->iirSampleER;
		toTape5->iirSampleFR = (toTape5->iirSampleFR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleFR;
		SubtractR += toTape5->iirSampleFR;
		toTape5->iirSampleGR = (toTape5->iirSampleGR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleGR;
		SubtractR += toTape5->iirSampleGR;
		toTape5->iirSampleHR = (toTape5->iirSampleHR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleHR;
		SubtractR += toTape5->iirSampleHR;
		toTape5->iirSampleIR = (toTape5->iirSampleIR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleIR;
		SubtractR += toTape5->iirSampleIR;
		toTape5->iirSampleJR = (toTape5->iirSampleJR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleJR;
		SubtractR += toTape5->iirSampleJR;
		toTape5->iirSampleKR = (toTape5->iirSampleKR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleKR;
		SubtractR += toTape5->iirSampleKR;
		toTape5->iirSampleLR = (toTape5->iirSampleLR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleLR;
		SubtractR += toTape5->iirSampleLR;
		toTape5->iirSampleMR = (toTape5->iirSampleMR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleMR;
		SubtractR += toTape5->iirSampleMR;
		toTape5->iirSampleNR = (toTape5->iirSampleNR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleNR;
		SubtractR += toTape5->iirSampleNR;
		toTape5->iirSampleOR = (toTape5->iirSampleOR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleOR;
		SubtractR += toTape5->iirSampleOR;
		toTape5->iirSamplePR = (toTape5->iirSamplePR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSamplePR;
		SubtractR += toTape5->iirSamplePR;
		toTape5->iirSampleQR = (toTape5->iirSampleQR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleQR;
		SubtractR += toTape5->iirSampleQR;
		toTape5->iirSampleRR = (toTape5->iirSampleRR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleRR;
		SubtractR += toTape5->iirSampleRR;
		toTape5->iirSampleSR = (toTape5->iirSampleSR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleSR;
		SubtractR += toTape5->iirSampleSR;
		toTape5->iirSampleTR = (toTape5->iirSampleTR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleTR;
		SubtractR += toTape5->iirSampleTR;
		toTape5->iirSampleUR = (toTape5->iirSampleUR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleUR;
		SubtractR += toTape5->iirSampleUR;
		toTape5->iirSampleVR = (toTape5->iirSampleVR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleVR;
		SubtractR += toTape5->iirSampleVR;
		toTape5->iirSampleWR = (toTape5->iirSampleWR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleWR;
		SubtractR += toTape5->iirSampleWR;
		toTape5->iirSampleXR = (toTape5->iirSampleXR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleXR;
		SubtractR += toTape5->iirSampleXR;
		toTape5->iirSampleYR = (toTape5->iirSampleYR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleYR;
		SubtractR += toTape5->iirSampleYR;
		toTape5->iirSampleZR = (toTape5->iirSampleZR * altAmount) + (tempSampleR * iirAmount);
		tempSampleR -= toTape5->iirSampleZR;
		SubtractR += toTape5->iirSampleZR;
		// do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
		// numbers being used. Don't keep doing it against the possibly high level signal number.
		// This has been known to add a resonant quality to the cutoff, which we're using on purpose.

		inputSampleL -= SubtractL;
		inputSampleR -= SubtractR;
		// apply stored up tiny corrections.

		if (outputgain != 1.0) {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
		}

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		toTape5->fpdL ^= toTape5->fpdL << 13;
		toTape5->fpdL ^= toTape5->fpdL >> 17;
		toTape5->fpdL ^= toTape5->fpdL << 5;
		inputSampleL += (((double) toTape5->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		toTape5->fpdR ^= toTape5->fpdR << 13;
		toTape5->fpdR ^= toTape5->fpdR >> 17;
		toTape5->fpdR ^= toTape5->fpdR << 5;
		inputSampleR += (((double) toTape5->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	TOTAPE5_URI,
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
