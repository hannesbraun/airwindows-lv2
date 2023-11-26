#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BASSKIT_URI "https://hannesbraun.net/ns/lv2/airwindows/basskit"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	VOICING = 5,
	BASS_OUT = 6,
	SUB_OUT = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* voicing;
	const float* bassOut;
	const float* subOut;

	double iirDriveSampleA;
	double iirDriveSampleB;
	double iirDriveSampleC;
	double iirDriveSampleD;
	double iirDriveSampleE;
	double iirDriveSampleF;
	bool flip; // drive things

	int bflip;
	bool WasNegative;
	bool SubOctave;
	double iirHeadBumpA;
	double iirHeadBumpB;
	double iirHeadBumpC;

	double iirSubBumpA;
	double iirSubBumpB;
	double iirSubBumpC;

	double lastHeadBump;
	double lastSubBump;

	double iirSampleA;
	double iirSampleB;
	double iirSampleC;
	double iirSampleD;
	double iirSampleE;
	double iirSampleF;
	double iirSampleG;
	double iirSampleH;
	double iirSampleI;
	double iirSampleJ;
	double iirSampleK;
	double iirSampleL;
	double iirSampleM;
	double iirSampleN;
	double iirSampleO;
	double iirSampleP;
	double iirSampleQ;
	double iirSampleR;
	double iirSampleS;
	double iirSampleT;
	double iirSampleU;
	double iirSampleV;
	double iirSampleW;
	double iirSampleX;
	double iirSampleY;
	double iirSampleZ;
	double oscGate;

	uint32_t fpdL;
	uint32_t fpdR;
} BassKit;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BassKit* bassKit = (BassKit*) calloc(1, sizeof(BassKit));
	bassKit->sampleRate = rate;
	return (LV2_Handle) bassKit;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BassKit* bassKit = (BassKit*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			bassKit->input[0] = (const float*) data;
			break;
		case INPUT_R:
			bassKit->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			bassKit->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			bassKit->output[1] = (float*) data;
			break;
		case DRIVE:
			bassKit->drive = (const float*) data;
			break;
		case VOICING:
			bassKit->voicing = (const float*) data;
			break;
		case BASS_OUT:
			bassKit->bassOut = (const float*) data;
			break;
		case SUB_OUT:
			bassKit->subOut = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	BassKit* bassKit = (BassKit*) instance;

	bassKit->WasNegative = false;
	bassKit->SubOctave = false;
	bassKit->flip = false;
	bassKit->bflip = 0;
	bassKit->iirDriveSampleA = 0.0;
	bassKit->iirDriveSampleB = 0.0;
	bassKit->iirDriveSampleC = 0.0;
	bassKit->iirDriveSampleD = 0.0;
	bassKit->iirDriveSampleE = 0.0;
	bassKit->iirDriveSampleF = 0.0;

	bassKit->iirHeadBumpA = 0.0;
	bassKit->iirHeadBumpB = 0.0;
	bassKit->iirHeadBumpC = 0.0;

	bassKit->iirSubBumpA = 0.0;
	bassKit->iirSubBumpB = 0.0;
	bassKit->iirSubBumpC = 0.0;

	bassKit->lastHeadBump = 0.0;
	bassKit->lastSubBump = 0.0;

	bassKit->iirSampleA = 0.0;
	bassKit->iirSampleB = 0.0;
	bassKit->iirSampleC = 0.0;
	bassKit->iirSampleD = 0.0;
	bassKit->iirSampleE = 0.0;
	bassKit->iirSampleF = 0.0;
	bassKit->iirSampleG = 0.0;
	bassKit->iirSampleH = 0.0;
	bassKit->iirSampleI = 0.0;
	bassKit->iirSampleJ = 0.0;
	bassKit->iirSampleK = 0.0;
	bassKit->iirSampleL = 0.0;
	bassKit->iirSampleM = 0.0;
	bassKit->iirSampleN = 0.0;
	bassKit->iirSampleO = 0.0;
	bassKit->iirSampleP = 0.0;
	bassKit->iirSampleQ = 0.0;
	bassKit->iirSampleR = 0.0;
	bassKit->iirSampleS = 0.0;
	bassKit->iirSampleT = 0.0;
	bassKit->iirSampleU = 0.0;
	bassKit->iirSampleV = 0.0;
	bassKit->iirSampleW = 0.0;
	bassKit->iirSampleX = 0.0;
	bassKit->iirSampleY = 0.0;
	bassKit->iirSampleZ = 0.0;

	bassKit->oscGate = 1.0;

	bassKit->fpdL = 1.0;
	while (bassKit->fpdL < 16386) bassKit->fpdL = rand() * UINT32_MAX;
	bassKit->fpdR = 1.0;
	while (bassKit->fpdR < 16386) bassKit->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BassKit* bassKit = (BassKit*) instance;

	const float* in1 = bassKit->input[0];
	const float* in2 = bassKit->input[1];
	float* out1 = bassKit->output[0];
	float* out2 = bassKit->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= bassKit->sampleRate;

	double ataLowpass;
	double randy;
	double invrandy;
	double HeadBump = 0.0;
	double BassGain = *bassKit->drive * 0.1;
	double HeadBumpFreq = ((*bassKit->voicing * 0.1) + 0.02) / overallscale;
	double iirAmount = HeadBumpFreq / 44.1;
	double BassOutGain = *bassKit->bassOut * fabs(*bassKit->bassOut);
	double SubBump = 0.0;
	double SubOutGain = *bassKit->subOut * fabs(*bassKit->subOut) * 4.0;
	double clamp = 0.0;
	double fuzz = 0.111;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = bassKit->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = bassKit->fpdR * 1.18e-17;

		ataLowpass = (inputSampleL + inputSampleR) / 2.0;
		bassKit->iirDriveSampleA = (bassKit->iirDriveSampleA * (1.0 - HeadBumpFreq)) + (ataLowpass * HeadBumpFreq);
		ataLowpass = bassKit->iirDriveSampleA;
		bassKit->iirDriveSampleB = (bassKit->iirDriveSampleB * (1.0 - HeadBumpFreq)) + (ataLowpass * HeadBumpFreq);
		ataLowpass = bassKit->iirDriveSampleB;

		bassKit->oscGate += fabs(ataLowpass * 10.0);
		bassKit->oscGate -= 0.001;
		if (bassKit->oscGate > 1.0) bassKit->oscGate = 1.0;
		if (bassKit->oscGate < 0) bassKit->oscGate = 0;
		// got a value that only goes down low when there's silence or near silence on input
		clamp = 1.0 - bassKit->oscGate;
		clamp *= 0.00001;
		// set up the thing to choke off oscillations- belt and suspenders affair

		if (ataLowpass > 0) {
			if (bassKit->WasNegative) {
				bassKit->SubOctave = !bassKit->SubOctave;
			}
			bassKit->WasNegative = false;
		} else {
			bassKit->WasNegative = true;
		}
		// set up polarities for sub-bass version
		randy = ((double) bassKit->fpdL / UINT32_MAX) * fuzz; // 0 to 1 the noise, may not be needed
		invrandy = (1.0 - randy);
		randy /= 2.0;
		// set up the noise

		bassKit->iirSampleA = (bassKit->iirSampleA * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleA;
		bassKit->iirSampleB = (bassKit->iirSampleB * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleB;
		bassKit->iirSampleC = (bassKit->iirSampleC * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleC;
		bassKit->iirSampleD = (bassKit->iirSampleD * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleD;
		bassKit->iirSampleE = (bassKit->iirSampleE * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleE;
		bassKit->iirSampleF = (bassKit->iirSampleF * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleF;
		bassKit->iirSampleG = (bassKit->iirSampleG * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleG;
		bassKit->iirSampleH = (bassKit->iirSampleH * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleH;
		bassKit->iirSampleI = (bassKit->iirSampleI * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleI;
		bassKit->iirSampleJ = (bassKit->iirSampleJ * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleJ;
		bassKit->iirSampleK = (bassKit->iirSampleK * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleK;
		bassKit->iirSampleL = (bassKit->iirSampleL * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleL;
		bassKit->iirSampleM = (bassKit->iirSampleM * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleM;
		bassKit->iirSampleN = (bassKit->iirSampleN * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleN;
		bassKit->iirSampleO = (bassKit->iirSampleO * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleO;
		bassKit->iirSampleP = (bassKit->iirSampleP * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleP;
		bassKit->iirSampleQ = (bassKit->iirSampleQ * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleQ;
		bassKit->iirSampleR = (bassKit->iirSampleR * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleR;
		bassKit->iirSampleS = (bassKit->iirSampleS * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleS;
		bassKit->iirSampleT = (bassKit->iirSampleT * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleT;
		bassKit->iirSampleU = (bassKit->iirSampleU * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleU;
		bassKit->iirSampleV = (bassKit->iirSampleV * (1.0 - iirAmount)) + (ataLowpass * iirAmount);
		ataLowpass -= bassKit->iirSampleV;

		switch (bassKit->bflip) {
			case 1:
				bassKit->iirHeadBumpA += (ataLowpass * BassGain);
				bassKit->iirHeadBumpA -= (bassKit->iirHeadBumpA * bassKit->iirHeadBumpA * bassKit->iirHeadBumpA * HeadBumpFreq);
				bassKit->iirHeadBumpA = (invrandy * bassKit->iirHeadBumpA) + (randy * bassKit->iirHeadBumpB) + (randy * bassKit->iirHeadBumpC);
				if (bassKit->iirHeadBumpA > 0) bassKit->iirHeadBumpA -= clamp;
				if (bassKit->iirHeadBumpA < 0) bassKit->iirHeadBumpA += clamp;
				HeadBump = bassKit->iirHeadBumpA;
				break;
			case 2:
				bassKit->iirHeadBumpB += (ataLowpass * BassGain);
				bassKit->iirHeadBumpB -= (bassKit->iirHeadBumpB * bassKit->iirHeadBumpB * bassKit->iirHeadBumpB * HeadBumpFreq);
				bassKit->iirHeadBumpB = (randy * bassKit->iirHeadBumpA) + (invrandy * bassKit->iirHeadBumpB) + (randy * bassKit->iirHeadBumpC);
				if (bassKit->iirHeadBumpB > 0) bassKit->iirHeadBumpB -= clamp;
				if (bassKit->iirHeadBumpB < 0) bassKit->iirHeadBumpB += clamp;
				HeadBump = bassKit->iirHeadBumpB;
				break;
			case 3:
				bassKit->iirHeadBumpC += (ataLowpass * BassGain);
				bassKit->iirHeadBumpC -= (bassKit->iirHeadBumpC * bassKit->iirHeadBumpC * bassKit->iirHeadBumpC * HeadBumpFreq);
				bassKit->iirHeadBumpC = (randy * bassKit->iirHeadBumpA) + (randy * bassKit->iirHeadBumpB) + (invrandy * bassKit->iirHeadBumpC);
				if (bassKit->iirHeadBumpC > 0) bassKit->iirHeadBumpC -= clamp;
				if (bassKit->iirHeadBumpC < 0) bassKit->iirHeadBumpC += clamp;
				HeadBump = bassKit->iirHeadBumpC;
				break;
		}

		bassKit->iirSampleW = (bassKit->iirSampleW * (1.0 - iirAmount)) + (HeadBump * iirAmount);
		HeadBump -= bassKit->iirSampleW;
		bassKit->iirSampleX = (bassKit->iirSampleX * (1.0 - iirAmount)) + (HeadBump * iirAmount);
		HeadBump -= bassKit->iirSampleX;

		SubBump = HeadBump;
		bassKit->iirSampleY = (bassKit->iirSampleY * (1.0 - iirAmount)) + (SubBump * iirAmount);
		SubBump -= bassKit->iirSampleY;

		bassKit->iirDriveSampleC = (bassKit->iirDriveSampleC * (1.0 - HeadBumpFreq)) + (SubBump * HeadBumpFreq);
		SubBump = bassKit->iirDriveSampleC;
		bassKit->iirDriveSampleD = (bassKit->iirDriveSampleD * (1.0 - HeadBumpFreq)) + (SubBump * HeadBumpFreq);
		SubBump = bassKit->iirDriveSampleD;

		SubBump = fabs(SubBump);
		if (bassKit->SubOctave == false) {
			SubBump = -SubBump;
		}

		switch (bassKit->bflip) {
			case 1:
				bassKit->iirSubBumpA += SubBump; // * BassGain);
				bassKit->iirSubBumpA -= (bassKit->iirSubBumpA * bassKit->iirSubBumpA * bassKit->iirSubBumpA * HeadBumpFreq);
				bassKit->iirSubBumpA = (invrandy * bassKit->iirSubBumpA) + (randy * bassKit->iirSubBumpB) + (randy * bassKit->iirSubBumpC);
				if (bassKit->iirSubBumpA > 0) bassKit->iirSubBumpA -= clamp;
				if (bassKit->iirSubBumpA < 0) bassKit->iirSubBumpA += clamp;
				SubBump = bassKit->iirSubBumpA;
				break;
			case 2:
				bassKit->iirSubBumpB += SubBump; // * BassGain);
				bassKit->iirSubBumpB -= (bassKit->iirSubBumpB * bassKit->iirSubBumpB * bassKit->iirSubBumpB * HeadBumpFreq);
				bassKit->iirSubBumpB = (randy * bassKit->iirSubBumpA) + (invrandy * bassKit->iirSubBumpB) + (randy * bassKit->iirSubBumpC);
				if (bassKit->iirSubBumpB > 0) bassKit->iirSubBumpB -= clamp;
				if (bassKit->iirSubBumpB < 0) bassKit->iirSubBumpB += clamp;
				SubBump = bassKit->iirSubBumpB;
				break;
			case 3:
				bassKit->iirSubBumpC += SubBump; // * BassGain);
				bassKit->iirSubBumpC -= (bassKit->iirSubBumpC * bassKit->iirSubBumpC * bassKit->iirSubBumpC * HeadBumpFreq);
				bassKit->iirSubBumpC = (randy * bassKit->iirSubBumpA) + (randy * bassKit->iirSubBumpB) + (invrandy * bassKit->iirSubBumpC);
				if (bassKit->iirSubBumpC > 0) bassKit->iirSubBumpC -= clamp;
				if (bassKit->iirSubBumpC < 0) bassKit->iirSubBumpC += clamp;
				SubBump = bassKit->iirSubBumpC;
				break;
		}

		bassKit->iirSampleZ = (bassKit->iirSampleZ * (1.0 - HeadBumpFreq)) + (SubBump * HeadBumpFreq);
		SubBump = bassKit->iirSampleZ;
		bassKit->iirDriveSampleE = (bassKit->iirDriveSampleE * (1.0 - iirAmount)) + (SubBump * iirAmount);
		SubBump = bassKit->iirDriveSampleE;
		bassKit->iirDriveSampleF = (bassKit->iirDriveSampleF * (1.0 - iirAmount)) + (SubBump * iirAmount);
		SubBump = bassKit->iirDriveSampleF;

		inputSampleL += (HeadBump * BassOutGain);
		inputSampleL += (SubBump * SubOutGain);

		inputSampleR += (HeadBump * BassOutGain);
		inputSampleR += (SubBump * SubOutGain);

		bassKit->flip = !bassKit->flip;
		bassKit->bflip++;
		if (bassKit->bflip < 1 || bassKit->bflip > 3) bassKit->bflip = 1;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		bassKit->fpdL ^= bassKit->fpdL << 13;
		bassKit->fpdL ^= bassKit->fpdL >> 17;
		bassKit->fpdL ^= bassKit->fpdL << 5;
		inputSampleL += (((double) bassKit->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		bassKit->fpdR ^= bassKit->fpdR << 13;
		bassKit->fpdR ^= bassKit->fpdR >> 17;
		bassKit->fpdR ^= bassKit->fpdR << 5;
		inputSampleR += (((double) bassKit->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	BASSKIT_URI,
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
