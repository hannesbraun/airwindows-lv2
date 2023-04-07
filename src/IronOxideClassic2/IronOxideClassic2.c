#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define IRONOXIDECLASSIC2_URI "https://hannesbraun.net/ns/lv2/airwindows/ironoxideclassic2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	INPUT_TRIM = 4,
	TAPE_SPEED = 5,
	OUTPUT_TRIM = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* inputTrim;
	const float* tapeSpeed;
	const float* outputTrim;

	double iirSampleAL;
	double iirSampleBL;
	double dL[264];
	double fastIIRAL;
	double fastIIRBL;
	double slowIIRAL;
	double slowIIRBL;
	double lastRefL[7];

	double iirSampleAR;
	double iirSampleBR;
	double dR[264];
	double fastIIRAR;
	double fastIIRBR;
	double slowIIRAR;
	double slowIIRBR;
	double lastRefR[7];

	double biquadA[15];
	double biquadB[15];

	int cycle;
	int gcount;
	bool flip;

	uint32_t fpdL;
	uint32_t fpdR;
} IronOxideClassic2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	IronOxideClassic2* ironOxideClassic2 = (IronOxideClassic2*) calloc(1, sizeof(IronOxideClassic2));
	ironOxideClassic2->sampleRate = rate;
	return (LV2_Handle) ironOxideClassic2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	IronOxideClassic2* ironOxideClassic2 = (IronOxideClassic2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			ironOxideClassic2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			ironOxideClassic2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			ironOxideClassic2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			ironOxideClassic2->output[1] = (float*) data;
			break;
		case INPUT_TRIM:
			ironOxideClassic2->inputTrim = (const float*) data;
			break;
		case TAPE_SPEED:
			ironOxideClassic2->tapeSpeed = (const float*) data;
			break;
		case OUTPUT_TRIM:
			ironOxideClassic2->outputTrim = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	IronOxideClassic2* ironOxideClassic2 = (IronOxideClassic2*) instance;

	for (int x = 0; x < 15; x++) {
		ironOxideClassic2->biquadA[x] = 0.0;
		ironOxideClassic2->biquadB[x] = 0.0;
	}
	for (int temp = 0; temp < 263; temp++) {
		ironOxideClassic2->dL[temp] = 0.0;
		ironOxideClassic2->dR[temp] = 0.0;
	}
	for (int count = 0; count < 6; count++) {
		ironOxideClassic2->lastRefL[count] = 0.0;
		ironOxideClassic2->lastRefR[count] = 0.0;
	}
	ironOxideClassic2->cycle = 0;
	ironOxideClassic2->gcount = 0;
	ironOxideClassic2->fastIIRAL = 0.0;
	ironOxideClassic2->fastIIRBL = 0.0;
	ironOxideClassic2->slowIIRAL = 0.0;
	ironOxideClassic2->slowIIRBL = 0.0;
	ironOxideClassic2->iirSampleAL = 0.0;
	ironOxideClassic2->iirSampleBL = 0.0;
	ironOxideClassic2->fastIIRAR = 0.0;
	ironOxideClassic2->fastIIRBR = 0.0;
	ironOxideClassic2->slowIIRAR = 0.0;
	ironOxideClassic2->slowIIRBR = 0.0;
	ironOxideClassic2->iirSampleAR = 0.0;
	ironOxideClassic2->iirSampleBR = 0.0;
	ironOxideClassic2->flip = true;
	ironOxideClassic2->fpdL = 1.0;
	while (ironOxideClassic2->fpdL < 16386) ironOxideClassic2->fpdL = rand() * UINT32_MAX;
	ironOxideClassic2->fpdR = 1.0;
	while (ironOxideClassic2->fpdR < 16386) ironOxideClassic2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	IronOxideClassic2* ironOxideClassic2 = (IronOxideClassic2*) instance;

	const float* in1 = ironOxideClassic2->input[0];
	const float* in2 = ironOxideClassic2->input[1];
	float* out1 = ironOxideClassic2->output[0];
	float* out2 = ironOxideClassic2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= ironOxideClassic2->sampleRate;

	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	// this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (ironOxideClassic2->cycle > cycleEnd - 1) ironOxideClassic2->cycle = cycleEnd - 1; // sanity check

	double inputgain = pow(10.0, *ironOxideClassic2->inputTrim / 20.0);
	double outputgain = pow(10.0, *ironOxideClassic2->outputTrim / 20.0);
	double ips = *ironOxideClassic2->tapeSpeed * 1.1;
	// slight correction to dial in convincing ips settings
	if (ips < 1 || ips > 200) {
		ips = 33.0;
	}
	// sanity checks are always key
	double iirAmount = ips / 430.0; // for low leaning
	double fastTaper = ips / 15.0;
	double slowTaper = 2.0 / (ips * ips);

	iirAmount /= overallscale;
	fastTaper /= overallscale;
	slowTaper /= overallscale;
	// now that we have this, we must multiply it back up
	fastTaper *= cycleEnd;
	slowTaper *= cycleEnd;
	// because we're only running that part one sample in two, or three, or four
	fastTaper += 1.0;
	slowTaper += 1.0;

	ironOxideClassic2->biquadA[0] = 24000.0 / ironOxideClassic2->sampleRate;
	ironOxideClassic2->biquadA[1] = 1.618033988749894848204586;
	ironOxideClassic2->biquadB[0] = 24000.0 / ironOxideClassic2->sampleRate;
	ironOxideClassic2->biquadB[1] = 0.618033988749894848204586;

	double K = tan(M_PI * ironOxideClassic2->biquadA[0]); // lowpass
	double norm = 1.0 / (1.0 + K / ironOxideClassic2->biquadA[1] + K * K);
	ironOxideClassic2->biquadA[2] = K * K * norm;
	ironOxideClassic2->biquadA[3] = 2.0 * ironOxideClassic2->biquadA[2];
	ironOxideClassic2->biquadA[4] = ironOxideClassic2->biquadA[2];
	ironOxideClassic2->biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	ironOxideClassic2->biquadA[6] = (1.0 - K / ironOxideClassic2->biquadA[1] + K * K) * norm;

	K = tan(M_PI * ironOxideClassic2->biquadB[0]); // lowpass
	norm = 1.0 / (1.0 + K / ironOxideClassic2->biquadB[1] + K * K);
	ironOxideClassic2->biquadB[2] = K * K * norm;
	ironOxideClassic2->biquadB[3] = 2.0 * ironOxideClassic2->biquadB[2];
	ironOxideClassic2->biquadB[4] = ironOxideClassic2->biquadB[2];
	ironOxideClassic2->biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	ironOxideClassic2->biquadB[6] = (1.0 - K / ironOxideClassic2->biquadB[1] + K * K) * norm;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = ironOxideClassic2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = ironOxideClassic2->fpdR * 1.18e-17;
		double tempSample;

		if (ironOxideClassic2->flip) {
			if (fabs(ironOxideClassic2->iirSampleAL) < 1.18e-37) ironOxideClassic2->iirSampleAL = 0.0;
			if (fabs(ironOxideClassic2->iirSampleAR) < 1.18e-37) ironOxideClassic2->iirSampleAR = 0.0;
			ironOxideClassic2->iirSampleAL = (ironOxideClassic2->iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			ironOxideClassic2->iirSampleAR = (ironOxideClassic2->iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleL -= ironOxideClassic2->iirSampleAL;
			inputSampleR -= ironOxideClassic2->iirSampleAR;
		} else {
			if (fabs(ironOxideClassic2->iirSampleBL) < 1.18e-37) ironOxideClassic2->iirSampleBL = 0.0;
			if (fabs(ironOxideClassic2->iirSampleBR) < 1.18e-37) ironOxideClassic2->iirSampleBR = 0.0;
			ironOxideClassic2->iirSampleBL = (ironOxideClassic2->iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			ironOxideClassic2->iirSampleBR = (ironOxideClassic2->iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleL -= ironOxideClassic2->iirSampleBL;
			inputSampleR -= ironOxideClassic2->iirSampleBR;
		}
		// do IIR highpass for leaning out

		if (ironOxideClassic2->biquadA[0] < 0.49999) {
			tempSample = ironOxideClassic2->biquadA[2] * inputSampleL + ironOxideClassic2->biquadA[3] * ironOxideClassic2->biquadA[7] + ironOxideClassic2->biquadA[4] * ironOxideClassic2->biquadA[8] - ironOxideClassic2->biquadA[5] * ironOxideClassic2->biquadA[9] - ironOxideClassic2->biquadA[6] * ironOxideClassic2->biquadA[10];
			ironOxideClassic2->biquadA[8] = ironOxideClassic2->biquadA[7];
			ironOxideClassic2->biquadA[7] = inputSampleL;
			inputSampleL = tempSample;
			ironOxideClassic2->biquadA[10] = ironOxideClassic2->biquadA[9];
			ironOxideClassic2->biquadA[9] = inputSampleL; // DF1 left
			tempSample = ironOxideClassic2->biquadA[2] * inputSampleR + ironOxideClassic2->biquadA[3] * ironOxideClassic2->biquadA[11] + ironOxideClassic2->biquadA[4] * ironOxideClassic2->biquadA[12] - ironOxideClassic2->biquadA[5] * ironOxideClassic2->biquadA[13] - ironOxideClassic2->biquadA[6] * ironOxideClassic2->biquadA[14];
			ironOxideClassic2->biquadA[12] = ironOxideClassic2->biquadA[11];
			ironOxideClassic2->biquadA[11] = inputSampleR;
			inputSampleR = tempSample;
			ironOxideClassic2->biquadA[14] = ironOxideClassic2->biquadA[13];
			ironOxideClassic2->biquadA[13] = inputSampleR; // DF1 right
		}

		if (inputgain != 1.0) {
			inputSampleL *= inputgain;
			inputSampleR *= inputgain;
		}

		double bridgerectifierL = fabs(inputSampleL);
		if (bridgerectifierL > 1.57079633) bridgerectifierL = 1.57079633;
		bridgerectifierL = sin(bridgerectifierL);
		if (inputSampleL > 0.0) inputSampleL = bridgerectifierL;
		else inputSampleL = -bridgerectifierL;
		// preliminary gain stage using antialiasing

		double bridgerectifierR = fabs(inputSampleR);
		if (bridgerectifierR > 1.57079633) bridgerectifierR = 1.57079633;
		bridgerectifierR = sin(bridgerectifierR);
		if (inputSampleR > 0.0) inputSampleR = bridgerectifierR;
		else inputSampleR = -bridgerectifierR;
		// preliminary gain stage using antialiasing

		ironOxideClassic2->cycle++;
		if (ironOxideClassic2->cycle == cycleEnd) { // hit the end point and we do a tape sample

			// over to the Iron Oxide shaping code using inputsample
			if (ironOxideClassic2->gcount < 0 || ironOxideClassic2->gcount > 131) ironOxideClassic2->gcount = 131;
			int count = ironOxideClassic2->gcount;
			// increment the counter

			double temp;
			ironOxideClassic2->dL[count + 131] = ironOxideClassic2->dL[count] = inputSampleL;
			ironOxideClassic2->dR[count + 131] = ironOxideClassic2->dR[count] = inputSampleR;

			if (ironOxideClassic2->flip) {
				if (fabs(ironOxideClassic2->fastIIRAL) < 1.18e-37) ironOxideClassic2->fastIIRAL = 0.0;
				if (fabs(ironOxideClassic2->slowIIRAL) < 1.18e-37) ironOxideClassic2->slowIIRAL = 0.0;
				ironOxideClassic2->fastIIRAL = ironOxideClassic2->fastIIRAL / fastTaper;
				ironOxideClassic2->slowIIRAL = ironOxideClassic2->slowIIRAL / slowTaper;
				ironOxideClassic2->fastIIRAL += ironOxideClassic2->dL[count];
				// scale stuff down

				if (fabs(ironOxideClassic2->fastIIRAR) < 1.18e-37) ironOxideClassic2->fastIIRAR = 0.0;
				if (fabs(ironOxideClassic2->slowIIRAR) < 1.18e-37) ironOxideClassic2->slowIIRAR = 0.0;
				ironOxideClassic2->fastIIRAR = ironOxideClassic2->fastIIRAR / fastTaper;
				ironOxideClassic2->slowIIRAR = ironOxideClassic2->slowIIRAR / slowTaper;
				ironOxideClassic2->fastIIRAR += ironOxideClassic2->dR[count];
				// scale stuff down
				count += 3;

				temp = ironOxideClassic2->dL[count + 127];
				temp += ironOxideClassic2->dL[count + 113];
				temp += ironOxideClassic2->dL[count + 109];
				temp += ironOxideClassic2->dL[count + 107];
				temp += ironOxideClassic2->dL[count + 103];
				temp += ironOxideClassic2->dL[count + 101];
				temp += ironOxideClassic2->dL[count + 97];
				temp += ironOxideClassic2->dL[count + 89];
				temp += ironOxideClassic2->dL[count + 83];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 79];
				temp += ironOxideClassic2->dL[count + 73];
				temp += ironOxideClassic2->dL[count + 71];
				temp += ironOxideClassic2->dL[count + 67];
				temp += ironOxideClassic2->dL[count + 61];
				temp += ironOxideClassic2->dL[count + 59];
				temp += ironOxideClassic2->dL[count + 53];
				temp += ironOxideClassic2->dL[count + 47];
				temp += ironOxideClassic2->dL[count + 43];
				temp += ironOxideClassic2->dL[count + 41];
				temp += ironOxideClassic2->dL[count + 37];
				temp += ironOxideClassic2->dL[count + 31];
				temp += ironOxideClassic2->dL[count + 29];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 23];
				temp += ironOxideClassic2->dL[count + 19];
				temp += ironOxideClassic2->dL[count + 17];
				temp += ironOxideClassic2->dL[count + 13];
				temp += ironOxideClassic2->dL[count + 11];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 7];
				temp += ironOxideClassic2->dL[count + 5];
				temp += ironOxideClassic2->dL[count + 3];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 2];
				temp += ironOxideClassic2->dL[count + 1]; // end L
				ironOxideClassic2->slowIIRAL += (temp / 128);

				temp = ironOxideClassic2->dR[count + 127];
				temp += ironOxideClassic2->dR[count + 113];
				temp += ironOxideClassic2->dR[count + 109];
				temp += ironOxideClassic2->dR[count + 107];
				temp += ironOxideClassic2->dR[count + 103];
				temp += ironOxideClassic2->dR[count + 101];
				temp += ironOxideClassic2->dR[count + 97];
				temp += ironOxideClassic2->dR[count + 89];
				temp += ironOxideClassic2->dR[count + 83];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 79];
				temp += ironOxideClassic2->dR[count + 73];
				temp += ironOxideClassic2->dR[count + 71];
				temp += ironOxideClassic2->dR[count + 67];
				temp += ironOxideClassic2->dR[count + 61];
				temp += ironOxideClassic2->dR[count + 59];
				temp += ironOxideClassic2->dR[count + 53];
				temp += ironOxideClassic2->dR[count + 47];
				temp += ironOxideClassic2->dR[count + 43];
				temp += ironOxideClassic2->dR[count + 41];
				temp += ironOxideClassic2->dR[count + 37];
				temp += ironOxideClassic2->dR[count + 31];
				temp += ironOxideClassic2->dR[count + 29];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 23];
				temp += ironOxideClassic2->dR[count + 19];
				temp += ironOxideClassic2->dR[count + 17];
				temp += ironOxideClassic2->dR[count + 13];
				temp += ironOxideClassic2->dR[count + 11];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 7];
				temp += ironOxideClassic2->dR[count + 5];
				temp += ironOxideClassic2->dR[count + 3];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 2];
				temp += ironOxideClassic2->dR[count + 1]; // end R
				ironOxideClassic2->slowIIRAR += (temp / 128);

				inputSampleL = ironOxideClassic2->fastIIRAL - (ironOxideClassic2->slowIIRAL / slowTaper);
				inputSampleR = ironOxideClassic2->fastIIRAR - (ironOxideClassic2->slowIIRAR / slowTaper);
			} else {
				if (fabs(ironOxideClassic2->fastIIRBL) < 1.18e-37) ironOxideClassic2->fastIIRBL = 0.0;
				if (fabs(ironOxideClassic2->slowIIRBL) < 1.18e-37) ironOxideClassic2->slowIIRBL = 0.0;
				ironOxideClassic2->fastIIRBL = ironOxideClassic2->fastIIRBL / fastTaper;
				ironOxideClassic2->slowIIRBL = ironOxideClassic2->slowIIRBL / slowTaper;
				ironOxideClassic2->fastIIRBL += ironOxideClassic2->dL[count];
				// scale stuff down

				if (fabs(ironOxideClassic2->fastIIRBR) < 1.18e-37) ironOxideClassic2->fastIIRBR = 0.0;
				if (fabs(ironOxideClassic2->slowIIRBR) < 1.18e-37) ironOxideClassic2->slowIIRBR = 0.0;
				ironOxideClassic2->fastIIRBR = ironOxideClassic2->fastIIRBR / fastTaper;
				ironOxideClassic2->slowIIRBR = ironOxideClassic2->slowIIRBR / slowTaper;
				ironOxideClassic2->fastIIRBR += ironOxideClassic2->dR[count];
				// scale stuff down
				count += 3;

				temp = ironOxideClassic2->dL[count + 127];
				temp += ironOxideClassic2->dL[count + 113];
				temp += ironOxideClassic2->dL[count + 109];
				temp += ironOxideClassic2->dL[count + 107];
				temp += ironOxideClassic2->dL[count + 103];
				temp += ironOxideClassic2->dL[count + 101];
				temp += ironOxideClassic2->dL[count + 97];
				temp += ironOxideClassic2->dL[count + 89];
				temp += ironOxideClassic2->dL[count + 83];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 79];
				temp += ironOxideClassic2->dL[count + 73];
				temp += ironOxideClassic2->dL[count + 71];
				temp += ironOxideClassic2->dL[count + 67];
				temp += ironOxideClassic2->dL[count + 61];
				temp += ironOxideClassic2->dL[count + 59];
				temp += ironOxideClassic2->dL[count + 53];
				temp += ironOxideClassic2->dL[count + 47];
				temp += ironOxideClassic2->dL[count + 43];
				temp += ironOxideClassic2->dL[count + 41];
				temp += ironOxideClassic2->dL[count + 37];
				temp += ironOxideClassic2->dL[count + 31];
				temp += ironOxideClassic2->dL[count + 29];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 23];
				temp += ironOxideClassic2->dL[count + 19];
				temp += ironOxideClassic2->dL[count + 17];
				temp += ironOxideClassic2->dL[count + 13];
				temp += ironOxideClassic2->dL[count + 11];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 7];
				temp += ironOxideClassic2->dL[count + 5];
				temp += ironOxideClassic2->dL[count + 3];
				temp /= 2;
				temp += ironOxideClassic2->dL[count + 2];
				temp += ironOxideClassic2->dL[count + 1];
				ironOxideClassic2->slowIIRBL += (temp / 128);

				temp = ironOxideClassic2->dR[count + 127];
				temp += ironOxideClassic2->dR[count + 113];
				temp += ironOxideClassic2->dR[count + 109];
				temp += ironOxideClassic2->dR[count + 107];
				temp += ironOxideClassic2->dR[count + 103];
				temp += ironOxideClassic2->dR[count + 101];
				temp += ironOxideClassic2->dR[count + 97];
				temp += ironOxideClassic2->dR[count + 89];
				temp += ironOxideClassic2->dR[count + 83];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 79];
				temp += ironOxideClassic2->dR[count + 73];
				temp += ironOxideClassic2->dR[count + 71];
				temp += ironOxideClassic2->dR[count + 67];
				temp += ironOxideClassic2->dR[count + 61];
				temp += ironOxideClassic2->dR[count + 59];
				temp += ironOxideClassic2->dR[count + 53];
				temp += ironOxideClassic2->dR[count + 47];
				temp += ironOxideClassic2->dR[count + 43];
				temp += ironOxideClassic2->dR[count + 41];
				temp += ironOxideClassic2->dR[count + 37];
				temp += ironOxideClassic2->dR[count + 31];
				temp += ironOxideClassic2->dR[count + 29];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 23];
				temp += ironOxideClassic2->dR[count + 19];
				temp += ironOxideClassic2->dR[count + 17];
				temp += ironOxideClassic2->dR[count + 13];
				temp += ironOxideClassic2->dR[count + 11];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 7];
				temp += ironOxideClassic2->dR[count + 5];
				temp += ironOxideClassic2->dR[count + 3];
				temp /= 2;
				temp += ironOxideClassic2->dR[count + 2];
				temp += ironOxideClassic2->dR[count + 1];
				ironOxideClassic2->slowIIRBR += (temp / 128);

				inputSampleL = ironOxideClassic2->fastIIRBL - (ironOxideClassic2->slowIIRBL / slowTaper);
				inputSampleR = ironOxideClassic2->fastIIRBR - (ironOxideClassic2->slowIIRBR / slowTaper);
			}

			if (cycleEnd == 4) {
				ironOxideClassic2->lastRefL[0] = ironOxideClassic2->lastRefL[4]; // start from previous last
				ironOxideClassic2->lastRefL[2] = (ironOxideClassic2->lastRefL[0] + inputSampleL) / 2; // half
				ironOxideClassic2->lastRefL[1] = (ironOxideClassic2->lastRefL[0] + ironOxideClassic2->lastRefL[2]) / 2; // one quarter
				ironOxideClassic2->lastRefL[3] = (ironOxideClassic2->lastRefL[2] + inputSampleL) / 2; // three quarters
				ironOxideClassic2->lastRefL[4] = inputSampleL; // full
				ironOxideClassic2->lastRefR[0] = ironOxideClassic2->lastRefR[4]; // start from previous last
				ironOxideClassic2->lastRefR[2] = (ironOxideClassic2->lastRefR[0] + inputSampleR) / 2; // half
				ironOxideClassic2->lastRefR[1] = (ironOxideClassic2->lastRefR[0] + ironOxideClassic2->lastRefR[2]) / 2; // one quarter
				ironOxideClassic2->lastRefR[3] = (ironOxideClassic2->lastRefR[2] + inputSampleR) / 2; // three quarters
				ironOxideClassic2->lastRefR[4] = inputSampleR; // full
			}
			if (cycleEnd == 3) {
				ironOxideClassic2->lastRefL[0] = ironOxideClassic2->lastRefL[3]; // start from previous last
				ironOxideClassic2->lastRefL[2] = (ironOxideClassic2->lastRefL[0] + ironOxideClassic2->lastRefL[0] + inputSampleL) / 3; // third
				ironOxideClassic2->lastRefL[1] = (ironOxideClassic2->lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
				ironOxideClassic2->lastRefL[3] = inputSampleL; // full
				ironOxideClassic2->lastRefR[0] = ironOxideClassic2->lastRefR[3]; // start from previous last
				ironOxideClassic2->lastRefR[2] = (ironOxideClassic2->lastRefR[0] + ironOxideClassic2->lastRefR[0] + inputSampleR) / 3; // third
				ironOxideClassic2->lastRefR[1] = (ironOxideClassic2->lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
				ironOxideClassic2->lastRefR[3] = inputSampleR; // full
			}
			if (cycleEnd == 2) {
				ironOxideClassic2->lastRefL[0] = ironOxideClassic2->lastRefL[2]; // start from previous last
				ironOxideClassic2->lastRefL[1] = (ironOxideClassic2->lastRefL[0] + inputSampleL) / 2; // half
				ironOxideClassic2->lastRefL[2] = inputSampleL; // full
				ironOxideClassic2->lastRefR[0] = ironOxideClassic2->lastRefR[2]; // start from previous last
				ironOxideClassic2->lastRefR[1] = (ironOxideClassic2->lastRefR[0] + inputSampleR) / 2; // half
				ironOxideClassic2->lastRefR[2] = inputSampleR; // full
			}
			if (cycleEnd == 1) {
				ironOxideClassic2->lastRefL[0] = inputSampleL;
				ironOxideClassic2->lastRefR[0] = inputSampleR;
			}
			ironOxideClassic2->cycle = 0; // reset
			inputSampleL = ironOxideClassic2->lastRefL[ironOxideClassic2->cycle];
			inputSampleR = ironOxideClassic2->lastRefR[ironOxideClassic2->cycle];
		} else {
			inputSampleL = ironOxideClassic2->lastRefL[ironOxideClassic2->cycle];
			inputSampleR = ironOxideClassic2->lastRefR[ironOxideClassic2->cycle];
			// we are going through our references now
		}

		bridgerectifierL = fabs(inputSampleL);
		if (bridgerectifierL > 1.57079633) bridgerectifierL = 1.57079633;
		bridgerectifierL = sin(bridgerectifierL);
		// can use as an output limiter
		if (inputSampleL > 0.0) inputSampleL = bridgerectifierL;
		else inputSampleL = -bridgerectifierL;
		// second stage of overdrive to prevent overs and allow bloody loud extremeness

		bridgerectifierR = fabs(inputSampleR);
		if (bridgerectifierR > 1.57079633) bridgerectifierR = 1.57079633;
		bridgerectifierR = sin(bridgerectifierR);
		// can use as an output limiter
		if (inputSampleR > 0.0) inputSampleR = bridgerectifierR;
		else inputSampleR = -bridgerectifierR;
		// second stage of overdrive to prevent overs and allow bloody loud extremeness

		if (ironOxideClassic2->biquadB[0] < 0.49999) {
			tempSample = ironOxideClassic2->biquadB[2] * inputSampleL + ironOxideClassic2->biquadB[3] * ironOxideClassic2->biquadB[7] + ironOxideClassic2->biquadB[4] * ironOxideClassic2->biquadB[8] - ironOxideClassic2->biquadB[5] * ironOxideClassic2->biquadB[9] - ironOxideClassic2->biquadB[6] * ironOxideClassic2->biquadB[10];
			ironOxideClassic2->biquadB[8] = ironOxideClassic2->biquadB[7];
			ironOxideClassic2->biquadB[7] = inputSampleL;
			inputSampleL = tempSample;
			ironOxideClassic2->biquadB[10] = ironOxideClassic2->biquadB[9];
			ironOxideClassic2->biquadB[9] = inputSampleL; // DF1 left
			tempSample = ironOxideClassic2->biquadB[2] * inputSampleR + ironOxideClassic2->biquadB[3] * ironOxideClassic2->biquadB[11] + ironOxideClassic2->biquadB[4] * ironOxideClassic2->biquadB[12] - ironOxideClassic2->biquadB[5] * ironOxideClassic2->biquadB[13] - ironOxideClassic2->biquadB[6] * ironOxideClassic2->biquadB[14];
			ironOxideClassic2->biquadB[12] = ironOxideClassic2->biquadB[11];
			ironOxideClassic2->biquadB[11] = inputSampleR;
			inputSampleR = tempSample;
			ironOxideClassic2->biquadB[14] = ironOxideClassic2->biquadB[13];
			ironOxideClassic2->biquadB[13] = inputSampleR; // DF1 right
		}

		if (outputgain != 1.0) {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
		}
		ironOxideClassic2->flip = !ironOxideClassic2->flip;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		ironOxideClassic2->fpdL ^= ironOxideClassic2->fpdL << 13;
		ironOxideClassic2->fpdL ^= ironOxideClassic2->fpdL >> 17;
		ironOxideClassic2->fpdL ^= ironOxideClassic2->fpdL << 5;
		inputSampleL += (((double) ironOxideClassic2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		ironOxideClassic2->fpdR ^= ironOxideClassic2->fpdR << 13;
		ironOxideClassic2->fpdR ^= ironOxideClassic2->fpdR >> 17;
		ironOxideClassic2->fpdR ^= ironOxideClassic2->fpdR << 5;
		inputSampleR += (((double) ironOxideClassic2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	IRONOXIDECLASSIC2_URI,
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
