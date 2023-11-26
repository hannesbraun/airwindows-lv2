#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BASS_DRIVE_URI "https://hannesbraun.net/ns/lv2/airwindows/bassdrive"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	PRESENCE = 4,
	HIGH = 5,
	MID = 6,
	LOW = 7,
	DRIVE = 8
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* presence;
	const float* high;
	const float* mid;
	const float* low;
	const float* drive;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff
	double presenceInAL[7];
	double presenceOutAL[7];
	double highInAL[7];
	double highOutAL[7];
	double midInAL[7];
	double midOutAL[7];
	double lowInAL[7];
	double lowOutAL[7];
	double presenceInBL[7];
	double presenceOutBL[7];
	double highInBL[7];
	double highOutBL[7];
	double midInBL[7];
	double midOutBL[7];
	double lowInBL[7];
	double lowOutBL[7];

	double presenceInAR[7];
	double presenceOutAR[7];
	double highInAR[7];
	double highOutAR[7];
	double midInAR[7];
	double midOutAR[7];
	double lowInAR[7];
	double lowOutAR[7];
	double presenceInBR[7];
	double presenceOutBR[7];
	double highInBR[7];
	double highOutBR[7];
	double midInBR[7];
	double midOutBR[7];
	double lowInBR[7];
	double lowOutBR[7];

	bool flip;
} BassDrive;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BassDrive* bassDrive = (BassDrive*) calloc(1, sizeof(BassDrive));
	return (LV2_Handle) bassDrive;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BassDrive* bassDrive = (BassDrive*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			bassDrive->input[0] = (const float*) data;
			break;
		case INPUT_R:
			bassDrive->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			bassDrive->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			bassDrive->output[1] = (float*) data;
			break;
		case PRESENCE:
			bassDrive->presence = (const float*) data;
			break;
		case HIGH:
			bassDrive->high = (const float*) data;
			break;
		case MID:
			bassDrive->mid = (const float*) data;
			break;
		case LOW:
			bassDrive->low = (const float*) data;
			break;
		case DRIVE:
			bassDrive->drive = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	BassDrive* bassDrive = (BassDrive*) instance;

	for (int fcount = 0; fcount < 7; fcount++) {
		bassDrive->presenceInAL[fcount] = 0.0;
		bassDrive->presenceOutAL[fcount] = 0.0;
		bassDrive->highInAL[fcount] = 0.0;
		bassDrive->highOutAL[fcount] = 0.0;
		bassDrive->midInAL[fcount] = 0.0;
		bassDrive->midOutAL[fcount] = 0.0;
		bassDrive->lowInAL[fcount] = 0.0;
		bassDrive->lowOutAL[fcount] = 0.0;
		bassDrive->presenceInBL[fcount] = 0.0;
		bassDrive->presenceOutBL[fcount] = 0.0;
		bassDrive->highInBL[fcount] = 0.0;
		bassDrive->highOutBL[fcount] = 0.0;
		bassDrive->midInBL[fcount] = 0.0;
		bassDrive->midOutBL[fcount] = 0.0;
		bassDrive->lowInBL[fcount] = 0.0;
		bassDrive->lowOutBL[fcount] = 0.0;

		bassDrive->presenceInAR[fcount] = 0.0;
		bassDrive->presenceOutAR[fcount] = 0.0;
		bassDrive->highInAR[fcount] = 0.0;
		bassDrive->highOutAR[fcount] = 0.0;
		bassDrive->midInAR[fcount] = 0.0;
		bassDrive->midOutAR[fcount] = 0.0;
		bassDrive->lowInAR[fcount] = 0.0;
		bassDrive->lowOutAR[fcount] = 0.0;
		bassDrive->presenceInBR[fcount] = 0.0;
		bassDrive->presenceOutBR[fcount] = 0.0;
		bassDrive->highInBR[fcount] = 0.0;
		bassDrive->highOutBR[fcount] = 0.0;
		bassDrive->midInBR[fcount] = 0.0;
		bassDrive->midOutBR[fcount] = 0.0;
		bassDrive->lowInBR[fcount] = 0.0;
		bassDrive->lowOutBR[fcount] = 0.0;
	}
	bassDrive->flip = false;

	bassDrive->fpdL = 1.0;
	while (bassDrive->fpdL < 16386) bassDrive->fpdL = rand() * UINT32_MAX;
	bassDrive->fpdR = 1.0;
	while (bassDrive->fpdR < 16386) bassDrive->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BassDrive* bassDrive = (BassDrive*) instance;

	const float* in1 = bassDrive->input[0];
	const float* in2 = bassDrive->input[1];
	float* out1 = bassDrive->output[0];
	float* out2 = bassDrive->output[1];

	double sumL;
	double sumR;
	double presence = pow(*bassDrive->presence, 5) * 8.0;
	double high = pow(*bassDrive->high, 3) * 4.0;
	double mid = pow(*bassDrive->mid, 2);
	double low = *bassDrive->low / 4.0;
	double drive = *bassDrive->drive * 2.0;
	double bridgerectifier;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = bassDrive->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = bassDrive->fpdR * 1.18e-17;

		sumL = 0.0;
		sumR = 0.0;

		if (bassDrive->flip) {
			bassDrive->presenceInAL[0] = bassDrive->presenceInAL[1];
			bassDrive->presenceInAL[1] = bassDrive->presenceInAL[2];
			bassDrive->presenceInAL[2] = bassDrive->presenceInAL[3];
			bassDrive->presenceInAL[3] = bassDrive->presenceInAL[4];
			bassDrive->presenceInAL[4] = bassDrive->presenceInAL[5];
			bassDrive->presenceInAL[5] = bassDrive->presenceInAL[6];
			bassDrive->presenceInAL[6] = inputSampleL * presence;
			bassDrive->presenceOutAL[2] = bassDrive->presenceOutAL[3];
			bassDrive->presenceOutAL[3] = bassDrive->presenceOutAL[4];
			bassDrive->presenceOutAL[4] = bassDrive->presenceOutAL[5];
			bassDrive->presenceOutAL[5] = bassDrive->presenceOutAL[6];
			bassDrive->presenceOutAL[6] = (bassDrive->presenceInAL[0] + bassDrive->presenceInAL[6]) + 1.9152966321 * (bassDrive->presenceInAL[1] + bassDrive->presenceInAL[5]) - (bassDrive->presenceInAL[2] + bassDrive->presenceInAL[4]) - 3.8305932641 * bassDrive->presenceInAL[3] + (-0.2828214615 * bassDrive->presenceOutAL[2]) + (0.2613069963 * bassDrive->presenceOutAL[3]) + (-0.8628193852 * bassDrive->presenceOutAL[4]) + (0.5387164389 * bassDrive->presenceOutAL[5]);
			bridgerectifier = fabs(bassDrive->presenceOutAL[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->presenceOutAL[6] > 0.0) {
				sumL += bridgerectifier;
			} else {
				sumL -= bridgerectifier;
			}
			// presence section L
			bassDrive->presenceInAR[0] = bassDrive->presenceInAR[1];
			bassDrive->presenceInAR[1] = bassDrive->presenceInAR[2];
			bassDrive->presenceInAR[2] = bassDrive->presenceInAR[3];
			bassDrive->presenceInAR[3] = bassDrive->presenceInAR[4];
			bassDrive->presenceInAR[4] = bassDrive->presenceInAR[5];
			bassDrive->presenceInAR[5] = bassDrive->presenceInAR[6];
			bassDrive->presenceInAR[6] = inputSampleR * presence;
			bassDrive->presenceOutAR[2] = bassDrive->presenceOutAR[3];
			bassDrive->presenceOutAR[3] = bassDrive->presenceOutAR[4];
			bassDrive->presenceOutAR[4] = bassDrive->presenceOutAR[5];
			bassDrive->presenceOutAR[5] = bassDrive->presenceOutAR[6];
			bassDrive->presenceOutAR[6] = (bassDrive->presenceInAR[0] + bassDrive->presenceInAR[6]) + 1.9152966321 * (bassDrive->presenceInAR[1] + bassDrive->presenceInAR[5]) - (bassDrive->presenceInAR[2] + bassDrive->presenceInAR[4]) - 3.8305932641 * bassDrive->presenceInAR[3] + (-0.2828214615 * bassDrive->presenceOutAR[2]) + (0.2613069963 * bassDrive->presenceOutAR[3]) + (-0.8628193852 * bassDrive->presenceOutAR[4]) + (0.5387164389 * bassDrive->presenceOutAR[5]);
			bridgerectifier = fabs(bassDrive->presenceOutAR[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->presenceOutAR[6] > 0.0) {
				sumR += bridgerectifier;
			} else {
				sumR -= bridgerectifier;
			}
			// presence section R

			bassDrive->highInAL[0] = bassDrive->highInAL[1];
			bassDrive->highInAL[1] = bassDrive->highInAL[2];
			bassDrive->highInAL[2] = bassDrive->highInAL[3];
			bassDrive->highInAL[3] = bassDrive->highInAL[4];
			bassDrive->highInAL[4] = bassDrive->highInAL[5];
			bassDrive->highInAL[5] = bassDrive->highInAL[6];
			bridgerectifier = fabs(inputSampleL) * high;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleL > 0.0) {
				bassDrive->highInAL[6] = bridgerectifier;
			} else {
				bassDrive->highInAL[6] = -bridgerectifier;
			}
			bassDrive->highInAL[6] *= high;
			bassDrive->highOutAL[2] = bassDrive->highOutAL[3];
			bassDrive->highOutAL[3] = bassDrive->highOutAL[4];
			bassDrive->highOutAL[4] = bassDrive->highOutAL[5];
			bassDrive->highOutAL[5] = bassDrive->highOutAL[6];
			bassDrive->highOutAL[6] = (bassDrive->highInAL[0] + bassDrive->highInAL[6]) - 0.5141967433 * (bassDrive->highInAL[1] + bassDrive->highInAL[5]) - (bassDrive->highInAL[2] + bassDrive->highInAL[4]) + 1.0283934866 * bassDrive->highInAL[3] + (-0.2828214615 * bassDrive->highOutAL[2]) + (1.0195930909 * bassDrive->highOutAL[3]) + (-1.9633013869 * bassDrive->highOutAL[4]) + (2.1020162751 * bassDrive->highOutAL[5]);
			bridgerectifier = fabs(bassDrive->highOutAL[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->highOutAL[6] > 0.0) {
				sumL += bridgerectifier;
			} else {
				sumL -= bridgerectifier;
			}
			// high section L
			bassDrive->highInAR[0] = bassDrive->highInAR[1];
			bassDrive->highInAR[1] = bassDrive->highInAR[2];
			bassDrive->highInAR[2] = bassDrive->highInAR[3];
			bassDrive->highInAR[3] = bassDrive->highInAR[4];
			bassDrive->highInAR[4] = bassDrive->highInAR[5];
			bassDrive->highInAR[5] = bassDrive->highInAR[6];
			bridgerectifier = fabs(inputSampleR) * high;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleR > 0.0) {
				bassDrive->highInAR[6] = bridgerectifier;
			} else {
				bassDrive->highInAR[6] = -bridgerectifier;
			}
			bassDrive->highInAR[6] *= high;
			bassDrive->highOutAR[2] = bassDrive->highOutAR[3];
			bassDrive->highOutAR[3] = bassDrive->highOutAR[4];
			bassDrive->highOutAR[4] = bassDrive->highOutAR[5];
			bassDrive->highOutAR[5] = bassDrive->highOutAR[6];
			bassDrive->highOutAR[6] = (bassDrive->highInAR[0] + bassDrive->highInAR[6]) - 0.5141967433 * (bassDrive->highInAR[1] + bassDrive->highInAR[5]) - (bassDrive->highInAR[2] + bassDrive->highInAR[4]) + 1.0283934866 * bassDrive->highInAR[3] + (-0.2828214615 * bassDrive->highOutAR[2]) + (1.0195930909 * bassDrive->highOutAR[3]) + (-1.9633013869 * bassDrive->highOutAR[4]) + (2.1020162751 * bassDrive->highOutAR[5]);
			bridgerectifier = fabs(bassDrive->highOutAR[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->highOutAR[6] > 0.0) {
				sumR += bridgerectifier;
			} else {
				sumR -= bridgerectifier;
			}
			// high section R

			bassDrive->midInAL[0] = bassDrive->midInAL[1];
			bassDrive->midInAL[1] = bassDrive->midInAL[2];
			bassDrive->midInAL[2] = bassDrive->midInAL[3];
			bassDrive->midInAL[3] = bassDrive->midInAL[4];
			bassDrive->midInAL[4] = bassDrive->midInAL[5];
			bassDrive->midInAL[5] = bassDrive->midInAL[6];
			bridgerectifier = fabs(inputSampleL) * mid;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleL > 0.0) {
				bassDrive->midInAL[6] = bridgerectifier;
			} else {
				bassDrive->midInAL[6] = -bridgerectifier;
			}
			bassDrive->midInAL[6] *= mid;
			bassDrive->midOutAL[2] = bassDrive->midOutAL[3];
			bassDrive->midOutAL[3] = bassDrive->midOutAL[4];
			bassDrive->midOutAL[4] = bassDrive->midOutAL[5];
			bassDrive->midOutAL[5] = bassDrive->midOutAL[6];
			bassDrive->midOutAL[6] = (bassDrive->midInAL[0] + bassDrive->midInAL[6]) - 1.1790257790 * (bassDrive->midInAL[1] + bassDrive->midInAL[5]) - (bassDrive->midInAL[2] + bassDrive->midInAL[4]) + 2.3580515580 * bassDrive->midInAL[3] + (-0.6292082828 * bassDrive->midOutAL[2]) + (2.7785843605 * bassDrive->midOutAL[3]) + (-4.6638295236 * bassDrive->midOutAL[4]) + (3.5142515802 * bassDrive->midOutAL[5]);
			sumL += bassDrive->midOutAL[6];
			// mid section L
			bassDrive->midInAR[0] = bassDrive->midInAR[1];
			bassDrive->midInAR[1] = bassDrive->midInAR[2];
			bassDrive->midInAR[2] = bassDrive->midInAR[3];
			bassDrive->midInAR[3] = bassDrive->midInAR[4];
			bassDrive->midInAR[4] = bassDrive->midInAR[5];
			bassDrive->midInAR[5] = bassDrive->midInAR[6];
			bridgerectifier = fabs(inputSampleR) * mid;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleR > 0.0) {
				bassDrive->midInAR[6] = bridgerectifier;
			} else {
				bassDrive->midInAR[6] = -bridgerectifier;
			}
			bassDrive->midInAR[6] *= mid;
			bassDrive->midOutAR[2] = bassDrive->midOutAR[3];
			bassDrive->midOutAR[3] = bassDrive->midOutAR[4];
			bassDrive->midOutAR[4] = bassDrive->midOutAR[5];
			bassDrive->midOutAR[5] = bassDrive->midOutAR[6];
			bassDrive->midOutAR[6] = (bassDrive->midInAR[0] + bassDrive->midInAR[6]) - 1.1790257790 * (bassDrive->midInAR[1] + bassDrive->midInAR[5]) - (bassDrive->midInAR[2] + bassDrive->midInAR[4]) + 2.3580515580 * bassDrive->midInAR[3] + (-0.6292082828 * bassDrive->midOutAR[2]) + (2.7785843605 * bassDrive->midOutAR[3]) + (-4.6638295236 * bassDrive->midOutAR[4]) + (3.5142515802 * bassDrive->midOutAR[5]);
			sumR += bassDrive->midOutAR[6];
			// mid section R

			bassDrive->lowInAL[0] = bassDrive->lowInAL[1];
			bassDrive->lowInAL[1] = bassDrive->lowInAL[2];
			bassDrive->lowInAL[2] = bassDrive->lowInAL[3];
			bassDrive->lowInAL[3] = bassDrive->lowInAL[4];
			bassDrive->lowInAL[4] = bassDrive->lowInAL[5];
			bassDrive->lowInAL[5] = bassDrive->lowInAL[6];
			bridgerectifier = fabs(inputSampleL) * low;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleL > 0.0) {
				bassDrive->lowInAL[6] = bridgerectifier;
			} else {
				bassDrive->lowInAL[6] = -bridgerectifier;
			}
			bassDrive->lowInAL[6] *= low;
			bassDrive->lowOutAL[2] = bassDrive->lowOutAL[3];
			bassDrive->lowOutAL[3] = bassDrive->lowOutAL[4];
			bassDrive->lowOutAL[4] = bassDrive->lowOutAL[5];
			bassDrive->lowOutAL[5] = bassDrive->lowOutAL[6];
			bassDrive->lowOutAL[6] = (bassDrive->lowInAL[0] + bassDrive->lowInAL[6]) - 1.9193504547 * (bassDrive->lowInAL[1] + bassDrive->lowInAL[5]) - (bassDrive->lowInAL[2] + bassDrive->lowInAL[4]) + 3.8387009093 * bassDrive->lowInAL[3] + (-0.9195964462 * bassDrive->lowOutAL[2]) + (3.7538173833 * bassDrive->lowOutAL[3]) + (-5.7487775603 * bassDrive->lowOutAL[4]) + (3.9145559258 * bassDrive->lowOutAL[5]);
			sumL += bassDrive->lowOutAL[6];
			// low section L
			bassDrive->lowInAR[0] = bassDrive->lowInAR[1];
			bassDrive->lowInAR[1] = bassDrive->lowInAR[2];
			bassDrive->lowInAR[2] = bassDrive->lowInAR[3];
			bassDrive->lowInAR[3] = bassDrive->lowInAR[4];
			bassDrive->lowInAR[4] = bassDrive->lowInAR[5];
			bassDrive->lowInAR[5] = bassDrive->lowInAR[6];
			bridgerectifier = fabs(inputSampleR) * low;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleR > 0.0) {
				bassDrive->lowInAR[6] = bridgerectifier;
			} else {
				bassDrive->lowInAR[6] = -bridgerectifier;
			}
			bassDrive->lowInAR[6] *= low;
			bassDrive->lowOutAR[2] = bassDrive->lowOutAR[3];
			bassDrive->lowOutAR[3] = bassDrive->lowOutAR[4];
			bassDrive->lowOutAR[4] = bassDrive->lowOutAR[5];
			bassDrive->lowOutAR[5] = bassDrive->lowOutAR[6];
			bassDrive->lowOutAR[6] = (bassDrive->lowInAR[0] + bassDrive->lowInAR[6]) - 1.9193504547 * (bassDrive->lowInAR[1] + bassDrive->lowInAR[5]) - (bassDrive->lowInAR[2] + bassDrive->lowInAR[4]) + 3.8387009093 * bassDrive->lowInAR[3] + (-0.9195964462 * bassDrive->lowOutAR[2]) + (3.7538173833 * bassDrive->lowOutAR[3]) + (-5.7487775603 * bassDrive->lowOutAR[4]) + (3.9145559258 * bassDrive->lowOutAR[5]);
			sumR += bassDrive->lowOutAR[6];
			// low section R
		} else {
			bassDrive->presenceInBL[0] = bassDrive->presenceInBL[1];
			bassDrive->presenceInBL[1] = bassDrive->presenceInBL[2];
			bassDrive->presenceInBL[2] = bassDrive->presenceInBL[3];
			bassDrive->presenceInBL[3] = bassDrive->presenceInBL[4];
			bassDrive->presenceInBL[4] = bassDrive->presenceInBL[5];
			bassDrive->presenceInBL[5] = bassDrive->presenceInBL[6];
			bassDrive->presenceInBL[6] = inputSampleL * presence;
			bassDrive->presenceOutBL[2] = bassDrive->presenceOutBL[3];
			bassDrive->presenceOutBL[3] = bassDrive->presenceOutBL[4];
			bassDrive->presenceOutBL[4] = bassDrive->presenceOutBL[5];
			bassDrive->presenceOutBL[5] = bassDrive->presenceOutBL[6];
			bassDrive->presenceOutBL[6] = (bassDrive->presenceInBL[0] + bassDrive->presenceInBL[6]) + 1.9152966321 * (bassDrive->presenceInBL[1] + bassDrive->presenceInBL[5]) - (bassDrive->presenceInBL[2] + bassDrive->presenceInBL[4]) - 3.8305932641 * bassDrive->presenceInBL[3] + (-0.2828214615 * bassDrive->presenceOutBL[2]) + (0.2613069963 * bassDrive->presenceOutBL[3]) + (-0.8628193852 * bassDrive->presenceOutBL[4]) + (0.5387164389 * bassDrive->presenceOutBL[5]);
			bridgerectifier = fabs(bassDrive->presenceOutBL[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->presenceOutBL[6] > 0.0) {
				sumL += bridgerectifier;
			} else {
				sumL -= bridgerectifier;
			}
			// presence section L
			bassDrive->presenceInBR[0] = bassDrive->presenceInBR[1];
			bassDrive->presenceInBR[1] = bassDrive->presenceInBR[2];
			bassDrive->presenceInBR[2] = bassDrive->presenceInBR[3];
			bassDrive->presenceInBR[3] = bassDrive->presenceInBR[4];
			bassDrive->presenceInBR[4] = bassDrive->presenceInBR[5];
			bassDrive->presenceInBR[5] = bassDrive->presenceInBR[6];
			bassDrive->presenceInBR[6] = inputSampleR * presence;
			bassDrive->presenceOutBR[2] = bassDrive->presenceOutBR[3];
			bassDrive->presenceOutBR[3] = bassDrive->presenceOutBR[4];
			bassDrive->presenceOutBR[4] = bassDrive->presenceOutBR[5];
			bassDrive->presenceOutBR[5] = bassDrive->presenceOutBR[6];
			bassDrive->presenceOutBR[6] = (bassDrive->presenceInBR[0] + bassDrive->presenceInBR[6]) + 1.9152966321 * (bassDrive->presenceInBR[1] + bassDrive->presenceInBR[5]) - (bassDrive->presenceInBR[2] + bassDrive->presenceInBR[4]) - 3.8305932641 * bassDrive->presenceInBR[3] + (-0.2828214615 * bassDrive->presenceOutBR[2]) + (0.2613069963 * bassDrive->presenceOutBR[3]) + (-0.8628193852 * bassDrive->presenceOutBR[4]) + (0.5387164389 * bassDrive->presenceOutBR[5]);
			bridgerectifier = fabs(bassDrive->presenceOutBR[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->presenceOutBR[6] > 0.0) {
				sumR += bridgerectifier;
			} else {
				sumR -= bridgerectifier;
			}
			// presence section R

			bassDrive->highInBL[0] = bassDrive->highInBL[1];
			bassDrive->highInBL[1] = bassDrive->highInBL[2];
			bassDrive->highInBL[2] = bassDrive->highInBL[3];
			bassDrive->highInBL[3] = bassDrive->highInBL[4];
			bassDrive->highInBL[4] = bassDrive->highInBL[5];
			bassDrive->highInBL[5] = bassDrive->highInBL[6];
			bridgerectifier = fabs(inputSampleL) * high;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleL > 0.0) {
				bassDrive->highInBL[6] = bridgerectifier;
			} else {
				bassDrive->highInBL[6] = -bridgerectifier;
			}
			bassDrive->highInBL[6] *= high;
			bassDrive->highOutBL[2] = bassDrive->highOutBL[3];
			bassDrive->highOutBL[3] = bassDrive->highOutBL[4];
			bassDrive->highOutBL[4] = bassDrive->highOutBL[5];
			bassDrive->highOutBL[5] = bassDrive->highOutBL[6];
			bassDrive->highOutBL[6] = (bassDrive->highInBL[0] + bassDrive->highInBL[6]) - 0.5141967433 * (bassDrive->highInBL[1] + bassDrive->highInBL[5]) - (bassDrive->highInBL[2] + bassDrive->highInBL[4]) + 1.0283934866 * bassDrive->highInBL[3] + (-0.2828214615 * bassDrive->highOutBL[2]) + (1.0195930909 * bassDrive->highOutBL[3]) + (-1.9633013869 * bassDrive->highOutBL[4]) + (2.1020162751 * bassDrive->highOutBL[5]);
			bridgerectifier = fabs(bassDrive->highOutBL[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->highOutBL[6] > 0.0) {
				sumL += bridgerectifier;
			} else {
				sumL -= bridgerectifier;
			}
			// high section L
			bassDrive->highInBR[0] = bassDrive->highInBR[1];
			bassDrive->highInBR[1] = bassDrive->highInBR[2];
			bassDrive->highInBR[2] = bassDrive->highInBR[3];
			bassDrive->highInBR[3] = bassDrive->highInBR[4];
			bassDrive->highInBR[4] = bassDrive->highInBR[5];
			bassDrive->highInBR[5] = bassDrive->highInBR[6];
			bridgerectifier = fabs(inputSampleR) * high;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleR > 0.0) {
				bassDrive->highInBR[6] = bridgerectifier;
			} else {
				bassDrive->highInBR[6] = -bridgerectifier;
			}
			bassDrive->highInBR[6] *= high;
			bassDrive->highOutBR[2] = bassDrive->highOutBR[3];
			bassDrive->highOutBR[3] = bassDrive->highOutBR[4];
			bassDrive->highOutBR[4] = bassDrive->highOutBR[5];
			bassDrive->highOutBR[5] = bassDrive->highOutBR[6];
			bassDrive->highOutBR[6] = (bassDrive->highInBR[0] + bassDrive->highInBR[6]) - 0.5141967433 * (bassDrive->highInBR[1] + bassDrive->highInBR[5]) - (bassDrive->highInBR[2] + bassDrive->highInBR[4]) + 1.0283934866 * bassDrive->highInBR[3] + (-0.2828214615 * bassDrive->highOutBR[2]) + (1.0195930909 * bassDrive->highOutBR[3]) + (-1.9633013869 * bassDrive->highOutBR[4]) + (2.1020162751 * bassDrive->highOutBR[5]);
			bridgerectifier = fabs(bassDrive->highOutBR[6]);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (bassDrive->highOutBR[6] > 0.0) {
				sumR += bridgerectifier;
			} else {
				sumR -= bridgerectifier;
			}
			// high section R

			bassDrive->midInBL[0] = bassDrive->midInBL[1];
			bassDrive->midInBL[1] = bassDrive->midInBL[2];
			bassDrive->midInBL[2] = bassDrive->midInBL[3];
			bassDrive->midInBL[3] = bassDrive->midInBL[4];
			bassDrive->midInBL[4] = bassDrive->midInBL[5];
			bassDrive->midInBL[5] = bassDrive->midInBL[6];
			bridgerectifier = fabs(inputSampleL) * mid;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleL > 0.0) {
				bassDrive->midInBL[6] = bridgerectifier;
			} else {
				bassDrive->midInBL[6] = -bridgerectifier;
			}
			bassDrive->midInBL[6] *= mid;
			bassDrive->midOutBL[2] = bassDrive->midOutBL[3];
			bassDrive->midOutBL[3] = bassDrive->midOutBL[4];
			bassDrive->midOutBL[4] = bassDrive->midOutBL[5];
			bassDrive->midOutBL[5] = bassDrive->midOutBL[6];
			bassDrive->midOutBL[6] = (bassDrive->midInBL[0] + bassDrive->midInBL[6]) - 1.1790257790 * (bassDrive->midInBL[1] + bassDrive->midInBL[5]) - (bassDrive->midInBL[2] + bassDrive->midInBL[4]) + 2.3580515580 * bassDrive->midInBL[3] + (-0.6292082828 * bassDrive->midOutBL[2]) + (2.7785843605 * bassDrive->midOutBL[3]) + (-4.6638295236 * bassDrive->midOutBL[4]) + (3.5142515802 * bassDrive->midOutBL[5]);
			sumL += bassDrive->midOutBL[6];
			// mid section L
			bassDrive->midInBR[0] = bassDrive->midInBR[1];
			bassDrive->midInBR[1] = bassDrive->midInBR[2];
			bassDrive->midInBR[2] = bassDrive->midInBR[3];
			bassDrive->midInBR[3] = bassDrive->midInBR[4];
			bassDrive->midInBR[4] = bassDrive->midInBR[5];
			bassDrive->midInBR[5] = bassDrive->midInBR[6];
			bridgerectifier = fabs(inputSampleR) * mid;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleR > 0.0) {
				bassDrive->midInBR[6] = bridgerectifier;
			} else {
				bassDrive->midInBR[6] = -bridgerectifier;
			}
			bassDrive->midInBR[6] *= mid;
			bassDrive->midOutBR[2] = bassDrive->midOutBR[3];
			bassDrive->midOutBR[3] = bassDrive->midOutBR[4];
			bassDrive->midOutBR[4] = bassDrive->midOutBR[5];
			bassDrive->midOutBR[5] = bassDrive->midOutBR[6];
			bassDrive->midOutBR[6] = (bassDrive->midInBR[0] + bassDrive->midInBR[6]) - 1.1790257790 * (bassDrive->midInBR[1] + bassDrive->midInBR[5]) - (bassDrive->midInBR[2] + bassDrive->midInBR[4]) + 2.3580515580 * bassDrive->midInBR[3] + (-0.6292082828 * bassDrive->midOutBR[2]) + (2.7785843605 * bassDrive->midOutBR[3]) + (-4.6638295236 * bassDrive->midOutBR[4]) + (3.5142515802 * bassDrive->midOutBR[5]);
			sumR += bassDrive->midOutBR[6];
			// mid section R

			bassDrive->lowInBL[0] = bassDrive->lowInBL[1];
			bassDrive->lowInBL[1] = bassDrive->lowInBL[2];
			bassDrive->lowInBL[2] = bassDrive->lowInBL[3];
			bassDrive->lowInBL[3] = bassDrive->lowInBL[4];
			bassDrive->lowInBL[4] = bassDrive->lowInBL[5];
			bassDrive->lowInBL[5] = bassDrive->lowInBL[6];
			bridgerectifier = fabs(inputSampleL) * low;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleL > 0.0) {
				bassDrive->lowInBL[6] = bridgerectifier;
			} else {
				bassDrive->lowInBL[6] = -bridgerectifier;
			}
			bassDrive->lowInBL[6] *= low;
			bassDrive->lowOutBL[2] = bassDrive->lowOutBL[3];
			bassDrive->lowOutBL[3] = bassDrive->lowOutBL[4];
			bassDrive->lowOutBL[4] = bassDrive->lowOutBL[5];
			bassDrive->lowOutBL[5] = bassDrive->lowOutBL[6];
			bassDrive->lowOutBL[6] = (bassDrive->lowInBL[0] + bassDrive->lowInBL[6]) - 1.9193504547 * (bassDrive->lowInBL[1] + bassDrive->lowInBL[5]) - (bassDrive->lowInBL[2] + bassDrive->lowInBL[4]) + 3.8387009093 * bassDrive->lowInBL[3] + (-0.9195964462 * bassDrive->lowOutBL[2]) + (3.7538173833 * bassDrive->lowOutBL[3]) + (-5.7487775603 * bassDrive->lowOutBL[4]) + (3.9145559258 * bassDrive->lowOutBL[5]);
			sumL += bassDrive->lowOutBL[6];
			// low section L
			bassDrive->lowInBR[0] = bassDrive->lowInBR[1];
			bassDrive->lowInBR[1] = bassDrive->lowInBR[2];
			bassDrive->lowInBR[2] = bassDrive->lowInBR[3];
			bassDrive->lowInBR[3] = bassDrive->lowInBR[4];
			bassDrive->lowInBR[4] = bassDrive->lowInBR[5];
			bassDrive->lowInBR[5] = bassDrive->lowInBR[6];
			bridgerectifier = fabs(inputSampleR) * low;
			if (bridgerectifier > 1.57079633) {
				bridgerectifier = 1.57079633;
			}
			bridgerectifier = sin(bridgerectifier);
			if (inputSampleR > 0.0) {
				bassDrive->lowInBR[6] = bridgerectifier;
			} else {
				bassDrive->lowInBR[6] = -bridgerectifier;
			}
			bassDrive->lowInBR[6] *= low;
			bassDrive->lowOutBR[2] = bassDrive->lowOutBR[3];
			bassDrive->lowOutBR[3] = bassDrive->lowOutBR[4];
			bassDrive->lowOutBR[4] = bassDrive->lowOutBR[5];
			bassDrive->lowOutBR[5] = bassDrive->lowOutBR[6];
			bassDrive->lowOutBR[6] = (bassDrive->lowInBR[0] + bassDrive->lowInBR[6]) - 1.9193504547 * (bassDrive->lowInBR[1] + bassDrive->lowInBR[5]) - (bassDrive->lowInBR[2] + bassDrive->lowInBR[4]) + 3.8387009093 * bassDrive->lowInBR[3] + (-0.9195964462 * bassDrive->lowOutBR[2]) + (3.7538173833 * bassDrive->lowOutBR[3]) + (-5.7487775603 * bassDrive->lowOutBR[4]) + (3.9145559258 * bassDrive->lowOutBR[5]);
			sumR += bassDrive->lowOutBR[6];
			// low section R
		}

		inputSampleL = fabs(sumL) * drive;
		if (inputSampleL > 1.57079633) {
			inputSampleL = 1.57079633;
		}
		inputSampleL = sin(inputSampleL);
		if (sumL < 0) inputSampleL = -inputSampleL;
		// output L
		inputSampleR = fabs(sumR) * drive;
		if (inputSampleR > 1.57079633) {
			inputSampleR = 1.57079633;
		}
		inputSampleR = sin(inputSampleR);
		if (sumR < 0) inputSampleR = -inputSampleR;
		// output R

		bassDrive->flip = !bassDrive->flip;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		bassDrive->fpdL ^= bassDrive->fpdL << 13;
		bassDrive->fpdL ^= bassDrive->fpdL >> 17;
		bassDrive->fpdL ^= bassDrive->fpdL << 5;
		inputSampleL += (((double) bassDrive->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		bassDrive->fpdR ^= bassDrive->fpdR << 13;
		bassDrive->fpdR ^= bassDrive->fpdR >> 17;
		bassDrive->fpdR ^= bassDrive->fpdR << 5;
		inputSampleR += (((double) bassDrive->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	BASS_DRIVE_URI,
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
