#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BASSAMP_URI "https://hannesbraun.net/ns/lv2/airwindows/bassamp"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	HIGH = 4,
	DRY = 5,
	DUB = 6,
	SUB = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* high;
	const float* dry;
	const float* dub;
	const float* sub;

	double LataLast6Sample;
	double LataLast5Sample;
	double LataLast4Sample;
	double LataLast3Sample;
	double LataLast2Sample;
	double LataLast1Sample;
	double LataHalfwaySample;
	double LataHalfDrySample;
	double LataHalfDiffSample;
	double LataLastDiffSample;
	double LataDrySample;
	double LataDiffSample;
	double LataPrevDiffSample;

	double LiirDriveSampleA;
	double LiirDriveSampleB;
	double LiirDriveSampleC;
	double LiirDriveSampleD;
	double LiirDriveSampleE;
	double LiirDriveSampleF;

	bool LWasNegative;
	bool LSubOctave;
	double LiirHeadBumpA;
	double LiirHeadBumpB;
	double LiirHeadBumpC;

	double LiirSubBumpA;
	double LiirSubBumpB;
	double LiirSubBumpC;

	double LiirSampleA;
	double LiirSampleB;
	double LiirSampleC;
	double LiirSampleD;
	double LiirSampleE;
	double LiirSampleF;
	double LiirSampleG;
	double LiirSampleH;
	double LiirSampleI;
	double LiirSampleJ;
	double LiirSampleK;
	double LiirSampleL;
	double LiirSampleM;
	double LiirSampleN;
	double LiirSampleO;
	double LiirSampleP;
	double LiirSampleQ;
	double LiirSampleR;
	double LiirSampleS;
	double LiirSampleT;
	double LiirSampleU;
	double LiirSampleV;
	double LiirSampleW;
	double LiirSampleX;
	double LiirSampleY;
	double LiirSampleZ;

	double RataLast6Sample;
	double RataLast5Sample;
	double RataLast4Sample;
	double RataLast3Sample;
	double RataLast2Sample;
	double RataLast1Sample;
	double RataHalfwaySample;
	double RataHalfDrySample;
	double RataHalfDiffSample;
	double RataLastDiffSample;
	double RataDrySample;
	double RataDiffSample;
	double RataPrevDiffSample;

	double RiirDriveSampleA;
	double RiirDriveSampleB;
	double RiirDriveSampleC;
	double RiirDriveSampleD;
	double RiirDriveSampleE;
	double RiirDriveSampleF;

	bool RWasNegative;
	bool RSubOctave;
	double RiirHeadBumpA;
	double RiirHeadBumpB;
	double RiirHeadBumpC;

	double RiirSubBumpA;
	double RiirSubBumpB;
	double RiirSubBumpC;

	double RiirSampleA;
	double RiirSampleB;
	double RiirSampleC;
	double RiirSampleD;
	double RiirSampleE;
	double RiirSampleF;
	double RiirSampleG;
	double RiirSampleH;
	double RiirSampleI;
	double RiirSampleJ;
	double RiirSampleK;
	double RiirSampleL;
	double RiirSampleM;
	double RiirSampleN;
	double RiirSampleO;
	double RiirSampleP;
	double RiirSampleQ;
	double RiirSampleR;
	double RiirSampleS;
	double RiirSampleT;
	double RiirSampleU;
	double RiirSampleV;
	double RiirSampleW;
	double RiirSampleX;
	double RiirSampleY;
	double RiirSampleZ;

	double ataK1;
	double ataK2;
	double ataK3;
	double ataK4;
	double ataK5;
	double ataK6;
	double ataK7;
	double ataK8; // end antialiasing variables

	bool flip; // drive things
	int bflip;

	uint32_t fpdL;
	uint32_t fpdR;
} BassAmp;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BassAmp* bassAmp = (BassAmp*) calloc(1, sizeof(BassAmp));
	bassAmp->sampleRate = rate;
	return (LV2_Handle) bassAmp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BassAmp* bassAmp = (BassAmp*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			bassAmp->input[0] = (const float*) data;
			break;
		case INPUT_R:
			bassAmp->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			bassAmp->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			bassAmp->output[1] = (float*) data;
			break;
		case HIGH:
			bassAmp->high = (const float*) data;
			break;
		case DRY:
			bassAmp->dry = (const float*) data;
			break;
		case DUB:
			bassAmp->dub = (const float*) data;
			break;
		case SUB:
			bassAmp->sub = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	BassAmp* bassAmp = (BassAmp*) instance;

	bassAmp->LataLast6Sample = 0.0;
	bassAmp->LataLast5Sample = 0.0;
	bassAmp->LataLast4Sample = 0.0;
	bassAmp->LataLast3Sample = 0.0;
	bassAmp->LataLast2Sample = 0.0;
	bassAmp->LataLast1Sample = 0.0;
	bassAmp->LataHalfwaySample = 0.0;
	bassAmp->LataHalfDrySample = 0.0;
	bassAmp->LataHalfDiffSample = 0.0;
	bassAmp->LataLastDiffSample = 0.0;
	bassAmp->LataDrySample = 0.0;
	bassAmp->LataDiffSample = 0.0;
	bassAmp->LataPrevDiffSample = 0.0;
	bassAmp->LWasNegative = false;
	bassAmp->LSubOctave = false;
	bassAmp->LiirDriveSampleA = 0.0;
	bassAmp->LiirDriveSampleB = 0.0;
	bassAmp->LiirDriveSampleC = 0.0;
	bassAmp->LiirDriveSampleD = 0.0;
	bassAmp->LiirDriveSampleE = 0.0;
	bassAmp->LiirDriveSampleF = 0.0;
	bassAmp->LiirHeadBumpA = 0.0;
	bassAmp->LiirHeadBumpB = 0.0;
	bassAmp->LiirHeadBumpC = 0.0;
	bassAmp->LiirSubBumpA = 0.0;
	bassAmp->LiirSubBumpB = 0.0;
	bassAmp->LiirSubBumpC = 0.0;
	bassAmp->LiirSampleA = 0.0;
	bassAmp->LiirSampleB = 0.0;
	bassAmp->LiirSampleC = 0.0;
	bassAmp->LiirSampleD = 0.0;
	bassAmp->LiirSampleE = 0.0;
	bassAmp->LiirSampleF = 0.0;
	bassAmp->LiirSampleG = 0.0;
	bassAmp->LiirSampleH = 0.0;
	bassAmp->LiirSampleI = 0.0;
	bassAmp->LiirSampleJ = 0.0;
	bassAmp->LiirSampleK = 0.0;
	bassAmp->LiirSampleL = 0.0;
	bassAmp->LiirSampleM = 0.0;
	bassAmp->LiirSampleN = 0.0;
	bassAmp->LiirSampleO = 0.0;
	bassAmp->LiirSampleP = 0.0;
	bassAmp->LiirSampleQ = 0.0;
	bassAmp->LiirSampleR = 0.0;
	bassAmp->LiirSampleS = 0.0;
	bassAmp->LiirSampleT = 0.0;
	bassAmp->LiirSampleU = 0.0;
	bassAmp->LiirSampleV = 0.0;
	bassAmp->LiirSampleW = 0.0;
	bassAmp->LiirSampleX = 0.0;
	bassAmp->LiirSampleY = 0.0;
	bassAmp->LiirSampleZ = 0.0;

	bassAmp->RataLast6Sample = 0.0;
	bassAmp->RataLast5Sample = 0.0;
	bassAmp->RataLast4Sample = 0.0;
	bassAmp->RataLast3Sample = 0.0;
	bassAmp->RataLast2Sample = 0.0;
	bassAmp->RataLast1Sample = 0.0;
	bassAmp->RataHalfwaySample = 0.0;
	bassAmp->RataHalfDrySample = 0.0;
	bassAmp->RataHalfDiffSample = 0.0;
	bassAmp->RataLastDiffSample = 0.0;
	bassAmp->RataDrySample = 0.0;
	bassAmp->RataDiffSample = 0.0;
	bassAmp->RataPrevDiffSample = 0.0;
	bassAmp->RWasNegative = false;
	bassAmp->RSubOctave = false;
	bassAmp->RiirDriveSampleA = 0.0;
	bassAmp->RiirDriveSampleB = 0.0;
	bassAmp->RiirDriveSampleC = 0.0;
	bassAmp->RiirDriveSampleD = 0.0;
	bassAmp->RiirDriveSampleE = 0.0;
	bassAmp->RiirDriveSampleF = 0.0;
	bassAmp->RiirHeadBumpA = 0.0;
	bassAmp->RiirHeadBumpB = 0.0;
	bassAmp->RiirHeadBumpC = 0.0;
	bassAmp->RiirSubBumpA = 0.0;
	bassAmp->RiirSubBumpB = 0.0;
	bassAmp->RiirSubBumpC = 0.0;
	bassAmp->RiirSampleA = 0.0;
	bassAmp->RiirSampleB = 0.0;
	bassAmp->RiirSampleC = 0.0;
	bassAmp->RiirSampleD = 0.0;
	bassAmp->RiirSampleE = 0.0;
	bassAmp->RiirSampleF = 0.0;
	bassAmp->RiirSampleG = 0.0;
	bassAmp->RiirSampleH = 0.0;
	bassAmp->RiirSampleI = 0.0;
	bassAmp->RiirSampleJ = 0.0;
	bassAmp->RiirSampleK = 0.0;
	bassAmp->RiirSampleL = 0.0;
	bassAmp->RiirSampleM = 0.0;
	bassAmp->RiirSampleN = 0.0;
	bassAmp->RiirSampleO = 0.0;
	bassAmp->RiirSampleP = 0.0;
	bassAmp->RiirSampleQ = 0.0;
	bassAmp->RiirSampleR = 0.0;
	bassAmp->RiirSampleS = 0.0;
	bassAmp->RiirSampleT = 0.0;
	bassAmp->RiirSampleU = 0.0;
	bassAmp->RiirSampleV = 0.0;
	bassAmp->RiirSampleW = 0.0;
	bassAmp->RiirSampleX = 0.0;
	bassAmp->RiirSampleY = 0.0;
	bassAmp->RiirSampleZ = 0.0;

	bassAmp->flip = false;
	bassAmp->bflip = 0;
	bassAmp->ataK1 = -0.646; // first FIR shaping of interpolated sample, brightens
	bassAmp->ataK2 = 0.311; // second FIR shaping of interpolated sample, thickens
	bassAmp->ataK6 = -0.093; // third FIR shaping of interpolated sample, brings air
	bassAmp->ataK7 = 0.057; // fourth FIR shaping of interpolated sample, thickens
	bassAmp->ataK8 = -0.023; // fifth FIR shaping of interpolated sample, brings air
	bassAmp->ataK3 = 0.114; // add raw to interpolated dry, toughens
	bassAmp->ataK4 = 0.886; // remainder of interpolated dry, adds up to 1.0
	bassAmp->ataK5 = 0.122; // subtract this much prev. diff sample, brightens
	bassAmp->fpdL = 1.0;
	while (bassAmp->fpdL < 16386) bassAmp->fpdL = rand() * UINT32_MAX;
	bassAmp->fpdR = 1.0;
	while (bassAmp->fpdR < 16386) bassAmp->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BassAmp* bassAmp = (BassAmp*) instance;

	const float* in1 = bassAmp->input[0];
	const float* in2 = bassAmp->input[1];
	float* out1 = bassAmp->output[0];
	float* out2 = bassAmp->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= bassAmp->sampleRate;

	double contHigh = *bassAmp->high;
	double dry = *bassAmp->dry;
	double contDub = *bassAmp->dub * 1.3;
	double contSub = *bassAmp->sub / 2.0;

	double driveone = pow(contHigh * 3.0, 2);
	double driveoutput = contHigh;
	double iirAmount = 0.344 / overallscale;
	double BassGain = contDub * 0.1;
	double HeadBumpFreq = (BassGain + 0.0001) / overallscale;
	double BassOutGain = contDub * 0.2;
	double SubGain = contSub * 0.1;
	double SubBumpFreq = (SubGain + 0.0001) / overallscale;
	double SubOutGain = contSub * 0.3;
	double iirHPAmount = 0.0000014 / overallscale;

	while (sampleFrames-- > 0) {
		double LinputSample = *in1;
		double RinputSample = *in2;
		if (fabs(LinputSample) < 1.18e-37) LinputSample = bassAmp->fpdL * 1.18e-37;
		if (fabs(RinputSample) < 1.18e-37) RinputSample = bassAmp->fpdR * 1.18e-37;

		bassAmp->LataDrySample = LinputSample;
		bassAmp->LataHalfDrySample = bassAmp->LataHalfwaySample = (LinputSample + bassAmp->LataLast1Sample + (bassAmp->LataLast2Sample * bassAmp->ataK1) + (bassAmp->LataLast3Sample * bassAmp->ataK2) + (bassAmp->LataLast4Sample * bassAmp->ataK6) + (bassAmp->LataLast5Sample * bassAmp->ataK7) + (bassAmp->LataLast6Sample * bassAmp->ataK8)) / 2.0;
		bassAmp->LataLast6Sample = bassAmp->LataLast5Sample;
		bassAmp->LataLast5Sample = bassAmp->LataLast4Sample;
		bassAmp->LataLast4Sample = bassAmp->LataLast3Sample;
		bassAmp->LataLast3Sample = bassAmp->LataLast2Sample;
		bassAmp->LataLast2Sample = bassAmp->LataLast1Sample;
		bassAmp->LataLast1Sample = LinputSample;
		// setting up oversampled special antialiasing
		bassAmp->RataDrySample = RinputSample;
		bassAmp->RataHalfDrySample = bassAmp->RataHalfwaySample = (RinputSample + bassAmp->RataLast1Sample + (bassAmp->RataLast2Sample * bassAmp->ataK1) + (bassAmp->RataLast3Sample * bassAmp->ataK2) + (bassAmp->RataLast4Sample * bassAmp->ataK6) + (bassAmp->RataLast5Sample * bassAmp->ataK7) + (bassAmp->RataLast6Sample * bassAmp->ataK8)) / 2.0;
		bassAmp->RataLast6Sample = bassAmp->RataLast5Sample;
		bassAmp->RataLast5Sample = bassAmp->RataLast4Sample;
		bassAmp->RataLast4Sample = bassAmp->RataLast3Sample;
		bassAmp->RataLast3Sample = bassAmp->RataLast2Sample;
		bassAmp->RataLast2Sample = bassAmp->RataLast1Sample;
		bassAmp->RataLast1Sample = RinputSample;
		// setting up oversampled special antialiasing

		double correction;
		double subtractSample;
		double tempSample; // used repeatedly in different places

		// begin first half- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample

		double LataHalfwayLowpass;
		double RataHalfwayLowpass;
		if (bassAmp->flip) {
			tempSample = subtractSample = bassAmp->LataHalfwaySample;
			bassAmp->LiirDriveSampleA = (bassAmp->LiirDriveSampleA * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleA;
			correction = bassAmp->LiirDriveSampleA;
			bassAmp->LiirDriveSampleC = (bassAmp->LiirDriveSampleC * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleC;
			correction += bassAmp->LiirDriveSampleC;
			bassAmp->LiirDriveSampleE = (bassAmp->LiirDriveSampleE * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleE;
			correction += bassAmp->LiirDriveSampleE;
			bassAmp->LataHalfwaySample -= correction;
			LataHalfwayLowpass = subtractSample - bassAmp->LataHalfwaySample;

			tempSample = subtractSample = bassAmp->RataHalfwaySample;
			bassAmp->RiirDriveSampleA = (bassAmp->RiirDriveSampleA * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleA;
			correction = bassAmp->RiirDriveSampleA;
			bassAmp->RiirDriveSampleC = (bassAmp->RiirDriveSampleC * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleC;
			correction += bassAmp->RiirDriveSampleC;
			bassAmp->RiirDriveSampleE = (bassAmp->RiirDriveSampleE * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleE;
			correction += bassAmp->RiirDriveSampleE;
			bassAmp->RataHalfwaySample -= correction;
			RataHalfwayLowpass = subtractSample - bassAmp->RataHalfwaySample;
		} else {
			tempSample = subtractSample = bassAmp->LataHalfwaySample;
			bassAmp->LiirDriveSampleB = (bassAmp->LiirDriveSampleB * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleB;
			correction = bassAmp->LiirDriveSampleB;
			bassAmp->LiirDriveSampleD = (bassAmp->LiirDriveSampleD * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleD;
			correction += bassAmp->LiirDriveSampleD;
			bassAmp->LiirDriveSampleF = (bassAmp->LiirDriveSampleF * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleF;
			correction += bassAmp->RiirDriveSampleF;
			bassAmp->LataHalfwaySample -= correction;
			LataHalfwayLowpass = subtractSample - bassAmp->LataHalfwaySample;

			tempSample = subtractSample = bassAmp->RataHalfwaySample;
			bassAmp->RiirDriveSampleB = (bassAmp->RiirDriveSampleB * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleB;
			correction = bassAmp->RiirDriveSampleB;
			bassAmp->RiirDriveSampleD = (bassAmp->RiirDriveSampleD * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleD;
			correction += bassAmp->RiirDriveSampleD;
			bassAmp->RiirDriveSampleF = (bassAmp->RiirDriveSampleF * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleF;
			correction += bassAmp->RiirDriveSampleF;
			bassAmp->RataHalfwaySample -= correction;
			RataHalfwayLowpass = subtractSample - bassAmp->RataHalfwaySample;
		}
		// highpass section
		if (bassAmp->LataHalfwaySample > 1.0) {
			bassAmp->LataHalfwaySample = 1.0;
		}
		if (bassAmp->LataHalfwaySample < -1.0) {
			bassAmp->LataHalfwaySample = -1.0;
		}
		if (bassAmp->RataHalfwaySample > 1.0) {
			bassAmp->RataHalfwaySample = 1.0;
		}
		if (bassAmp->RataHalfwaySample < -1.0) {
			bassAmp->RataHalfwaySample = -1.0;
		}
		double drivetwo = driveone;
		while (drivetwo > 0.60) {
			drivetwo -= 0.60;
			bassAmp->LataHalfwaySample -= (bassAmp->LataHalfwaySample * (fabs(bassAmp->LataHalfwaySample) * 0.60) * (fabs(bassAmp->LataHalfwaySample) * 0.60));
			bassAmp->RataHalfwaySample -= (bassAmp->RataHalfwaySample * (fabs(bassAmp->RataHalfwaySample) * 0.60) * (fabs(bassAmp->RataHalfwaySample) * 0.60));
			bassAmp->LataHalfwaySample *= (1.0 + 0.60);
			bassAmp->RataHalfwaySample *= (1.0 + 0.60);
		}
		// that's taken care of the really high gain stuff

		bassAmp->LataHalfwaySample -= (bassAmp->LataHalfwaySample * (fabs(bassAmp->LataHalfwaySample) * drivetwo) * (fabs(bassAmp->LataHalfwaySample) * drivetwo));
		bassAmp->RataHalfwaySample -= (bassAmp->RataHalfwaySample * (fabs(bassAmp->RataHalfwaySample) * drivetwo) * (fabs(bassAmp->RataHalfwaySample) * drivetwo));
		bassAmp->LataHalfwaySample *= (1.0 + drivetwo);
		bassAmp->RataHalfwaySample *= (1.0 + drivetwo);

		// end first half
		// begin second half- inputSample and ataDrySample handled separately here

		double LataLowpass;
		double RataLowpass;
		if (bassAmp->flip) {
			tempSample = subtractSample = LinputSample;
			bassAmp->LiirDriveSampleA = (bassAmp->LiirDriveSampleA * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleA;
			correction = bassAmp->LiirDriveSampleA;
			bassAmp->LiirDriveSampleC = (bassAmp->LiirDriveSampleC * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleC;
			correction += bassAmp->LiirDriveSampleC;
			bassAmp->LiirDriveSampleE = (bassAmp->LiirDriveSampleE * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleE;
			correction += bassAmp->LiirDriveSampleE;
			LinputSample -= correction;
			LataLowpass = subtractSample - LinputSample;

			tempSample = subtractSample = RinputSample;
			bassAmp->RiirDriveSampleA = (bassAmp->RiirDriveSampleA * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleA;
			correction = bassAmp->RiirDriveSampleA;
			bassAmp->RiirDriveSampleC = (bassAmp->RiirDriveSampleC * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleC;
			correction += bassAmp->RiirDriveSampleC;
			bassAmp->RiirDriveSampleE = (bassAmp->RiirDriveSampleE * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleE;
			correction += bassAmp->RiirDriveSampleE;
			RinputSample -= correction;
			RataLowpass = subtractSample - RinputSample;
		} else {
			tempSample = subtractSample = LinputSample;
			bassAmp->LiirDriveSampleB = (bassAmp->LiirDriveSampleB * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleB;
			correction = bassAmp->LiirDriveSampleB;
			bassAmp->LiirDriveSampleD = (bassAmp->LiirDriveSampleD * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleD;
			correction += bassAmp->LiirDriveSampleD;
			bassAmp->LiirDriveSampleF = (bassAmp->LiirDriveSampleF * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->LiirDriveSampleF;
			correction += bassAmp->LiirDriveSampleF;
			LinputSample -= correction;
			LataLowpass = subtractSample - LinputSample;

			tempSample = subtractSample = RinputSample;
			bassAmp->RiirDriveSampleB = (bassAmp->RiirDriveSampleB * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleB;
			correction = bassAmp->RiirDriveSampleB;
			bassAmp->RiirDriveSampleD = (bassAmp->RiirDriveSampleD * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleD;
			correction += bassAmp->RiirDriveSampleD;
			bassAmp->RiirDriveSampleF = (bassAmp->RiirDriveSampleF * (1.0 - iirAmount)) + (tempSample * iirAmount);
			tempSample -= bassAmp->RiirDriveSampleF;
			correction += bassAmp->RiirDriveSampleF;
			RinputSample -= correction;
			RataLowpass = subtractSample - RinputSample;
		}
		// highpass section
		if (LinputSample > 1.0) {
			LinputSample = 1.0;
		}
		if (LinputSample < -1.0) {
			LinputSample = -1.0;
		}
		if (RinputSample > 1.0) {
			RinputSample = 1.0;
		}
		if (RinputSample < -1.0) {
			RinputSample = -1.0;
		}
		drivetwo = driveone;
		while (drivetwo > 0.60) {
			drivetwo -= 0.60;
			LinputSample -= (LinputSample * (fabs(LinputSample) * 0.60) * (fabs(LinputSample) * 0.60));
			RinputSample -= (RinputSample * (fabs(RinputSample) * 0.60) * (fabs(RinputSample) * 0.60));
			LinputSample *= (1.0 + 0.60);
			RinputSample *= (1.0 + 0.60);
		}
		// that's taken care of the really high gain stuff

		LinputSample -= (LinputSample * (fabs(LinputSample) * drivetwo) * (fabs(LinputSample) * drivetwo));
		RinputSample -= (RinputSample * (fabs(RinputSample) * drivetwo) * (fabs(RinputSample) * drivetwo));
		LinputSample *= (1.0 + drivetwo);
		RinputSample *= (1.0 + drivetwo);
		// end second half

		bassAmp->LataHalfDrySample = (bassAmp->LataDrySample * bassAmp->ataK3) + (bassAmp->LataHalfDrySample * bassAmp->ataK4);
		bassAmp->LataHalfDiffSample = (bassAmp->LataHalfwaySample - bassAmp->LataHalfDrySample) / 2.0;
		bassAmp->LataLastDiffSample = bassAmp->LataDiffSample * bassAmp->ataK5;
		bassAmp->LataDiffSample = (LinputSample - bassAmp->LataDrySample) / 2.0;
		bassAmp->LataDiffSample += bassAmp->LataHalfDiffSample;
		bassAmp->LataDiffSample -= bassAmp->LataLastDiffSample;
		LinputSample = bassAmp->LataDrySample;
		LinputSample += bassAmp->LataDiffSample;
		// apply processing as difference to non-oversampled raw input
		bassAmp->RataHalfDrySample = (bassAmp->RataDrySample * bassAmp->ataK3) + (bassAmp->RataHalfDrySample * bassAmp->ataK4);
		bassAmp->RataHalfDiffSample = (bassAmp->RataHalfwaySample - bassAmp->RataHalfDrySample) / 2.0;
		bassAmp->RataLastDiffSample = bassAmp->RataDiffSample * bassAmp->ataK5;
		bassAmp->RataDiffSample = (RinputSample - bassAmp->RataDrySample) / 2.0;
		bassAmp->RataDiffSample += bassAmp->RataHalfDiffSample;
		bassAmp->RataDiffSample -= bassAmp->RataLastDiffSample;
		RinputSample = bassAmp->RataDrySample;
		RinputSample += bassAmp->RataDiffSample;
		// apply processing as difference to non-oversampled raw input

		// now we've got inputSample as the Drive top-end output, and we have ataLowpass and ataHalfwayLowpass
		LataLowpass += LataHalfwayLowpass; // and combined them. Now we make sub-octaves
		RataLowpass += RataHalfwayLowpass; // and combined them. Now we make sub-octaves

		double randy = ((double) bassAmp->fpdL / UINT32_MAX) * 0.0555; // 0 to 1 the noise, may not be needed

		switch (bassAmp->bflip) {
			case 1:
				bassAmp->LiirHeadBumpA += (LataLowpass * BassGain);
				bassAmp->LiirHeadBumpA -= (bassAmp->LiirHeadBumpA * bassAmp->LiirHeadBumpA * bassAmp->LiirHeadBumpA * HeadBumpFreq);
				bassAmp->LiirHeadBumpA = ((1.0 - randy) * bassAmp->LiirHeadBumpA) + (randy * 0.5 * bassAmp->LiirHeadBumpB) + (randy * 0.5 * bassAmp->LiirHeadBumpC);

				bassAmp->RiirHeadBumpA += (RataLowpass * BassGain);
				bassAmp->RiirHeadBumpA -= (bassAmp->RiirHeadBumpA * bassAmp->RiirHeadBumpA * bassAmp->RiirHeadBumpA * HeadBumpFreq);
				bassAmp->RiirHeadBumpA = ((1.0 - randy) * bassAmp->RiirHeadBumpA) + (randy * 0.5 * bassAmp->RiirHeadBumpB) + (randy * 0.5 * bassAmp->RiirHeadBumpC);
				break;
			case 2:
				bassAmp->LiirHeadBumpB += (LataLowpass * BassGain);
				bassAmp->LiirHeadBumpB -= (bassAmp->LiirHeadBumpB * bassAmp->LiirHeadBumpB * bassAmp->LiirHeadBumpB * HeadBumpFreq);
				bassAmp->LiirHeadBumpB = (randy * 0.5 * bassAmp->LiirHeadBumpA) + ((1.0 - randy) * bassAmp->LiirHeadBumpB) + (randy * 0.5 * bassAmp->LiirHeadBumpC);

				bassAmp->RiirHeadBumpB += (RataLowpass * BassGain);
				bassAmp->RiirHeadBumpB -= (bassAmp->RiirHeadBumpB * bassAmp->RiirHeadBumpB * bassAmp->RiirHeadBumpB * HeadBumpFreq);
				bassAmp->RiirHeadBumpB = (randy * 0.5 * bassAmp->RiirHeadBumpA) + ((1.0 - randy) * bassAmp->RiirHeadBumpB) + (randy * 0.5 * bassAmp->RiirHeadBumpC);
				break;
			case 3:
				bassAmp->LiirHeadBumpC += (LataLowpass * BassGain);
				bassAmp->LiirHeadBumpC -= (bassAmp->LiirHeadBumpC * bassAmp->LiirHeadBumpC * bassAmp->LiirHeadBumpC * HeadBumpFreq);
				bassAmp->LiirHeadBumpC = (randy * 0.5 * bassAmp->LiirHeadBumpA) + (randy * 0.5 * bassAmp->LiirHeadBumpB) + ((1.0 - randy) * bassAmp->LiirHeadBumpC);

				bassAmp->RiirHeadBumpC += (RataLowpass * BassGain);
				bassAmp->RiirHeadBumpC -= (bassAmp->RiirHeadBumpC * bassAmp->RiirHeadBumpC * bassAmp->RiirHeadBumpC * HeadBumpFreq);
				bassAmp->RiirHeadBumpC = (randy * 0.5 * bassAmp->RiirHeadBumpA) + (randy * 0.5 * bassAmp->RiirHeadBumpB) + ((1.0 - randy) * bassAmp->RiirHeadBumpC);
				break;
		}
		double LHeadBump = bassAmp->LiirHeadBumpA + bassAmp->LiirHeadBumpB + bassAmp->LiirHeadBumpC;
		double RHeadBump = bassAmp->RiirHeadBumpA + bassAmp->RiirHeadBumpB + bassAmp->RiirHeadBumpC;

		double LSubBump = fabs(LHeadBump);
		double RSubBump = fabs(RHeadBump);

		if (LHeadBump > 0) {
			if (bassAmp->LWasNegative) {
				bassAmp->LSubOctave = !bassAmp->LSubOctave;
			}
			bassAmp->LWasNegative = false;
		} else {
			bassAmp->LWasNegative = true;
		}
		// set up polarities for sub-bass version
		if (RHeadBump > 0) {
			if (bassAmp->RWasNegative) {
				bassAmp->RSubOctave = !bassAmp->RSubOctave;
			}
			bassAmp->RWasNegative = false;
		} else {
			bassAmp->RWasNegative = true;
		}
		// set up polarities for sub-bass version

		if (bassAmp->LSubOctave == false) {
			LSubBump = -LSubBump;
		}
		if (bassAmp->RSubOctave == false) {
			RSubBump = -RSubBump;
		}

		switch (bassAmp->bflip) {
			case 1:
				bassAmp->LiirSubBumpA += (LSubBump * SubGain);
				bassAmp->LiirSubBumpA -= (bassAmp->LiirSubBumpA * bassAmp->LiirSubBumpA * bassAmp->LiirSubBumpA * SubBumpFreq);
				bassAmp->LiirSubBumpA = ((1.0 - randy) * bassAmp->LiirSubBumpA) + (randy * 0.5 * bassAmp->LiirSubBumpB) + (randy * 0.5 * bassAmp->LiirSubBumpC);

				bassAmp->RiirSubBumpA += (RSubBump * SubGain);
				bassAmp->RiirSubBumpA -= (bassAmp->RiirSubBumpA * bassAmp->RiirSubBumpA * bassAmp->RiirSubBumpA * SubBumpFreq);
				bassAmp->RiirSubBumpA = ((1.0 - randy) * bassAmp->RiirSubBumpA) + (randy * 0.5 * bassAmp->RiirSubBumpB) + (randy * 0.5 * bassAmp->RiirSubBumpC);
				break;
			case 2:
				bassAmp->LiirSubBumpB += (LSubBump * SubGain);
				bassAmp->LiirSubBumpB -= (bassAmp->LiirSubBumpB * bassAmp->LiirSubBumpB * bassAmp->LiirSubBumpB * SubBumpFreq);
				bassAmp->LiirSubBumpB = (randy * 0.5 * bassAmp->LiirSubBumpA) + ((1.0 - randy) * bassAmp->LiirSubBumpB) + (randy * 0.5 * bassAmp->LiirSubBumpC);

				bassAmp->RiirSubBumpB += (RSubBump * SubGain);
				bassAmp->RiirSubBumpB -= (bassAmp->RiirSubBumpB * bassAmp->RiirSubBumpB * bassAmp->RiirSubBumpB * SubBumpFreq);
				bassAmp->RiirSubBumpB = (randy * 0.5 * bassAmp->RiirSubBumpA) + ((1.0 - randy) * bassAmp->RiirSubBumpB) + (randy * 0.5 * bassAmp->RiirSubBumpC);
				break;
			case 3:
				bassAmp->LiirSubBumpC += (LSubBump * SubGain);
				bassAmp->LiirSubBumpC -= (bassAmp->LiirSubBumpC * bassAmp->LiirSubBumpC * bassAmp->LiirSubBumpC * SubBumpFreq);
				bassAmp->LiirSubBumpC = (randy * 0.5 * bassAmp->LiirSubBumpA) + (randy * 0.5 * bassAmp->LiirSubBumpB) + ((1.0 - randy) * bassAmp->LiirSubBumpC);

				bassAmp->RiirSubBumpC += (RSubBump * SubGain);
				bassAmp->RiirSubBumpC -= (bassAmp->RiirSubBumpC * bassAmp->RiirSubBumpC * bassAmp->RiirSubBumpC * SubBumpFreq);
				bassAmp->RiirSubBumpC = (randy * 0.5 * bassAmp->RiirSubBumpA) + (randy * 0.5 * bassAmp->RiirSubBumpB) + ((1.0 - randy) * bassAmp->RiirSubBumpC);
				break;
		}
		LSubBump = bassAmp->LiirSubBumpA + bassAmp->LiirSubBumpB + bassAmp->LiirSubBumpC;
		RSubBump = bassAmp->RiirSubBumpA + bassAmp->RiirSubBumpB + bassAmp->RiirSubBumpC;

		bassAmp->flip = !bassAmp->flip;
		bassAmp->bflip++;
		if (bassAmp->bflip < 1 || bassAmp->bflip > 3) bassAmp->bflip = 1;

		LinputSample *= driveoutput;
		RinputSample *= driveoutput;

		correction = bassAmp->LataDrySample * dry;
		correction += (LHeadBump * BassOutGain);
		correction += (LSubBump * SubOutGain);
		LinputSample += correction;

		correction = bassAmp->RataDrySample * dry;
		correction += (RHeadBump * BassOutGain);
		correction += (RSubBump * SubOutGain);
		RinputSample += correction;

		tempSample = LinputSample;
		bassAmp->LiirSampleA = (bassAmp->LiirSampleA * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleA;
		correction = bassAmp->LiirSampleA;
		bassAmp->LiirSampleB = (bassAmp->LiirSampleB * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleB;
		correction += bassAmp->LiirSampleB;
		bassAmp->LiirSampleC = (bassAmp->LiirSampleC * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleC;
		correction += bassAmp->LiirSampleC;
		bassAmp->LiirSampleD = (bassAmp->LiirSampleD * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleD;
		correction += bassAmp->LiirSampleD;
		bassAmp->LiirSampleE = (bassAmp->LiirSampleE * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleE;
		correction += bassAmp->LiirSampleE;
		bassAmp->LiirSampleF = (bassAmp->LiirSampleF * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleF;
		correction += bassAmp->LiirSampleF;
		bassAmp->LiirSampleG = (bassAmp->LiirSampleG * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleG;
		correction += bassAmp->LiirSampleG;
		bassAmp->LiirSampleH = (bassAmp->LiirSampleH * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleH;
		correction += bassAmp->LiirSampleH;
		bassAmp->LiirSampleI = (bassAmp->LiirSampleI * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleI;
		correction += bassAmp->LiirSampleI;
		bassAmp->LiirSampleJ = (bassAmp->LiirSampleJ * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleJ;
		correction += bassAmp->LiirSampleJ;
		bassAmp->LiirSampleK = (bassAmp->LiirSampleK * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleK;
		correction += bassAmp->LiirSampleK;
		bassAmp->LiirSampleL = (bassAmp->LiirSampleL * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleL;
		correction += bassAmp->LiirSampleL;
		bassAmp->LiirSampleM = (bassAmp->LiirSampleM * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleM;
		correction += bassAmp->LiirSampleM;
		bassAmp->LiirSampleN = (bassAmp->LiirSampleN * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleN;
		correction += bassAmp->LiirSampleN;
		bassAmp->LiirSampleO = (bassAmp->LiirSampleO * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleO;
		correction += bassAmp->LiirSampleO;
		bassAmp->LiirSampleP = (bassAmp->LiirSampleP * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleP;
		correction += bassAmp->LiirSampleP;
		bassAmp->LiirSampleQ = (bassAmp->LiirSampleQ * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleQ;
		correction += bassAmp->LiirSampleQ;
		bassAmp->LiirSampleR = (bassAmp->LiirSampleR * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleR;
		correction += bassAmp->LiirSampleR;
		bassAmp->LiirSampleS = (bassAmp->LiirSampleS * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleS;
		correction += bassAmp->LiirSampleS;
		bassAmp->LiirSampleT = (bassAmp->LiirSampleT * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleT;
		correction += bassAmp->LiirSampleT;
		bassAmp->LiirSampleU = (bassAmp->LiirSampleU * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleU;
		correction += bassAmp->LiirSampleU;
		bassAmp->LiirSampleV = (bassAmp->LiirSampleV * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleV;
		correction += bassAmp->LiirSampleV;
		bassAmp->LiirSampleW = (bassAmp->LiirSampleW * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleW;
		correction += bassAmp->LiirSampleW;
		bassAmp->LiirSampleX = (bassAmp->LiirSampleX * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleX;
		correction += bassAmp->LiirSampleX;
		bassAmp->LiirSampleY = (bassAmp->LiirSampleY * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleY;
		correction += bassAmp->LiirSampleY;
		bassAmp->LiirSampleZ = (bassAmp->LiirSampleZ * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->LiirSampleZ;
		correction += bassAmp->LiirSampleZ;
		// do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
		// numbers being used. Don't keep doing it against the possibly high level signal number.
		LinputSample -= correction;
		// apply stored up tiny corrections

		tempSample = RinputSample;
		bassAmp->RiirSampleA = (bassAmp->RiirSampleA * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleA;
		correction = bassAmp->RiirSampleA;
		bassAmp->RiirSampleB = (bassAmp->RiirSampleB * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleB;
		correction += bassAmp->RiirSampleB;
		bassAmp->RiirSampleC = (bassAmp->RiirSampleC * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleC;
		correction += bassAmp->RiirSampleC;
		bassAmp->RiirSampleD = (bassAmp->RiirSampleD * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleD;
		correction += bassAmp->RiirSampleD;
		bassAmp->RiirSampleE = (bassAmp->RiirSampleE * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleE;
		correction += bassAmp->RiirSampleE;
		bassAmp->RiirSampleF = (bassAmp->RiirSampleF * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleF;
		correction += bassAmp->RiirSampleF;
		bassAmp->RiirSampleG = (bassAmp->RiirSampleG * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleG;
		correction += bassAmp->RiirSampleG;
		bassAmp->RiirSampleH = (bassAmp->RiirSampleH * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleH;
		correction += bassAmp->RiirSampleH;
		bassAmp->RiirSampleI = (bassAmp->RiirSampleI * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleI;
		correction += bassAmp->RiirSampleI;
		bassAmp->RiirSampleJ = (bassAmp->RiirSampleJ * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleJ;
		correction += bassAmp->RiirSampleJ;
		bassAmp->RiirSampleK = (bassAmp->RiirSampleK * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleK;
		correction += bassAmp->RiirSampleK;
		bassAmp->RiirSampleL = (bassAmp->RiirSampleL * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleL;
		correction += bassAmp->RiirSampleL;
		bassAmp->RiirSampleM = (bassAmp->RiirSampleM * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleM;
		correction += bassAmp->RiirSampleM;
		bassAmp->RiirSampleN = (bassAmp->RiirSampleN * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleN;
		correction += bassAmp->RiirSampleN;
		bassAmp->RiirSampleO = (bassAmp->RiirSampleO * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleO;
		correction += bassAmp->RiirSampleO;
		bassAmp->RiirSampleP = (bassAmp->RiirSampleP * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleP;
		correction += bassAmp->RiirSampleP;
		bassAmp->RiirSampleQ = (bassAmp->RiirSampleQ * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleQ;
		correction += bassAmp->RiirSampleQ;
		bassAmp->RiirSampleR = (bassAmp->RiirSampleR * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleR;
		correction += bassAmp->RiirSampleR;
		bassAmp->RiirSampleS = (bassAmp->RiirSampleS * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleS;
		correction += bassAmp->RiirSampleS;
		bassAmp->RiirSampleT = (bassAmp->RiirSampleT * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleT;
		correction += bassAmp->RiirSampleT;
		bassAmp->RiirSampleU = (bassAmp->RiirSampleU * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleU;
		correction += bassAmp->RiirSampleU;
		bassAmp->RiirSampleV = (bassAmp->RiirSampleV * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleV;
		correction += bassAmp->RiirSampleV;
		bassAmp->RiirSampleW = (bassAmp->RiirSampleW * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleW;
		correction += bassAmp->RiirSampleW;
		bassAmp->RiirSampleX = (bassAmp->RiirSampleX * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleX;
		correction += bassAmp->RiirSampleX;
		bassAmp->RiirSampleY = (bassAmp->RiirSampleY * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleY;
		correction += bassAmp->RiirSampleY;
		bassAmp->RiirSampleZ = (bassAmp->RiirSampleZ * (1.0 - iirHPAmount)) + (tempSample * iirHPAmount);
		tempSample -= bassAmp->RiirSampleZ;
		correction += bassAmp->RiirSampleZ;
		// do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
		// numbers being used. Don't keep doing it against the possibly high level signal number.
		RinputSample -= correction;
		// apply stored up tiny corrections

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) LinputSample, &expon);
		bassAmp->fpdL ^= bassAmp->fpdL << 13;
		bassAmp->fpdL ^= bassAmp->fpdL >> 17;
		bassAmp->fpdL ^= bassAmp->fpdL << 5;
		LinputSample += (((double) bassAmp->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) RinputSample, &expon);
		bassAmp->fpdR ^= bassAmp->fpdR << 13;
		bassAmp->fpdR ^= bassAmp->fpdR >> 17;
		bassAmp->fpdR ^= bassAmp->fpdR << 5;
		RinputSample += (((double) bassAmp->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		// end 32 bit stereo floating point dither

		*out1 = (float) LinputSample;
		*out2 = (float) RinputSample;

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
	BASSAMP_URI,
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
