#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define RESEQ_URI "https://hannesbraun.net/ns/lv2/airwindows/reseq"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	RESO_1 = 4,
	RESO_2 = 5,
	RESO_3 = 6,
	RESO_4 = 7,
	RESO_5 = 8,
	RESO_6 = 9,
	RESO_7 = 10,
	RESO_8 = 11,
	DRY_WET = 12
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* reso1;
	const float* reso2;
	const float* reso3;
	const float* reso4;
	const float* reso5;
	const float* reso6;
	const float* reso7;
	const float* reso8;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double bL[61];
	double fL[61];
	double bR[61];
	double fR[61];
	int framenumber;
} ResEQ;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ResEQ* resEQ = (ResEQ*) calloc(1, sizeof(ResEQ));
	resEQ->sampleRate = rate;
	return (LV2_Handle) resEQ;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ResEQ* resEQ = (ResEQ*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			resEQ->input[0] = (const float*) data;
			break;
		case INPUT_R:
			resEQ->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			resEQ->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			resEQ->output[1] = (float*) data;
			break;
		case RESO_1:
			resEQ->reso1 = (const float*) data;
			break;
		case RESO_2:
			resEQ->reso2 = (const float*) data;
			break;
		case RESO_3:
			resEQ->reso3 = (const float*) data;
			break;
		case RESO_4:
			resEQ->reso4 = (const float*) data;
			break;
		case RESO_5:
			resEQ->reso5 = (const float*) data;
			break;
		case RESO_6:
			resEQ->reso6 = (const float*) data;
			break;
		case RESO_7:
			resEQ->reso7 = (const float*) data;
			break;
		case RESO_8:
			resEQ->reso8 = (const float*) data;
			break;
		case DRY_WET:
			resEQ->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ResEQ* resEQ = (ResEQ*) instance;
	for (int count = 0; count < 60; count++) {
		resEQ->bL[count] = 0.0;
		resEQ->fL[count] = 0.0;
		resEQ->bR[count] = 0.0;
		resEQ->fR[count] = 0.0;
	}
	resEQ->framenumber = 0;
	resEQ->fpdL = 1.0;
	while (resEQ->fpdL < 16386) resEQ->fpdL = rand() * UINT32_MAX;
	resEQ->fpdR = 1.0;
	while (resEQ->fpdR < 16386) resEQ->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ResEQ* resEQ = (ResEQ*) instance;

	const float* in1 = resEQ->input[0];
	const float* in2 = resEQ->input[1];
	float* out1 = resEQ->output[0];
	float* out2 = resEQ->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= resEQ->sampleRate;

	double v1 = *resEQ->reso1;
	double v2 = *resEQ->reso2;
	double v3 = *resEQ->reso3;
	double v4 = *resEQ->reso4;
	double v5 = *resEQ->reso5;
	double v6 = *resEQ->reso6;
	double v7 = *resEQ->reso7;
	double v8 = *resEQ->reso8;
	double f1 = pow(v1, 2);
	double f2 = pow(v2, 2);
	double f3 = pow(v3, 2);
	double f4 = pow(v4, 2);
	double f5 = pow(v5, 2);
	double f6 = pow(v6, 2);
	double f7 = pow(v7, 2);
	double f8 = pow(v8, 2);
	double wet = *resEQ->dryWet;
	double falloff;
	v1 += 0.2;
	v2 += 0.2;
	v3 += 0.2;
	v4 += 0.2;
	v5 += 0.2;
	v6 += 0.2;
	v7 += 0.2;
	v8 += 0.2;
	v1 /= overallscale;
	v2 /= overallscale;
	v3 /= overallscale;
	v4 /= overallscale;
	v5 /= overallscale;
	v6 /= overallscale;
	v7 /= overallscale;
	v8 /= overallscale;
	// each process frame we'll update some of the kernel frames. That way we don't have to crunch the whole thing at once,
	// and we can load a LOT more resonant peaks into the kernel.

	resEQ->framenumber += 1;
	if (resEQ->framenumber > 59) resEQ->framenumber = 1;
	falloff = sin(resEQ->framenumber / 19.098992);
	resEQ->fL[resEQ->framenumber] = 0.0;
	if ((resEQ->framenumber * f1) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f1) * 2.0) * falloff * v1);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f1) * falloff * v1);
	if ((resEQ->framenumber * f2) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f2) * 2.0) * falloff * v2);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f2) * falloff * v2);
	if ((resEQ->framenumber * f3) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f3) * 2.0) * falloff * v3);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f3) * falloff * v3);
	if ((resEQ->framenumber * f4) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f4) * 2.0) * falloff * v4);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f4) * falloff * v4);
	if ((resEQ->framenumber * f5) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f5) * 2.0) * falloff * v5);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f5) * falloff * v5);
	if ((resEQ->framenumber * f6) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f6) * 2.0) * falloff * v6);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f6) * falloff * v6);
	if ((resEQ->framenumber * f7) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f7) * 2.0) * falloff * v7);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f7) * falloff * v7);
	if ((resEQ->framenumber * f8) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f8) * 2.0) * falloff * v8);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f8) * falloff * v8);

	resEQ->fR[resEQ->framenumber] = 0.0;
	if ((resEQ->framenumber * f1) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f1) * 2.0) * falloff * v1);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f1) * falloff * v1);
	if ((resEQ->framenumber * f2) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f2) * 2.0) * falloff * v2);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f2) * falloff * v2);
	if ((resEQ->framenumber * f3) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f3) * 2.0) * falloff * v3);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f3) * falloff * v3);
	if ((resEQ->framenumber * f4) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f4) * 2.0) * falloff * v4);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f4) * falloff * v4);
	if ((resEQ->framenumber * f5) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f5) * 2.0) * falloff * v5);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f5) * falloff * v5);
	if ((resEQ->framenumber * f6) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f6) * 2.0) * falloff * v6);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f6) * falloff * v6);
	if ((resEQ->framenumber * f7) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f7) * 2.0) * falloff * v7);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f7) * falloff * v7);
	if ((resEQ->framenumber * f8) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f8) * 2.0) * falloff * v8);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f8) * falloff * v8);

	resEQ->framenumber += 1;
	if (resEQ->framenumber > 59) resEQ->framenumber = 1;
	falloff = sin(resEQ->framenumber / 19.098992);
	resEQ->fL[resEQ->framenumber] = 0.0;
	if ((resEQ->framenumber * f1) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f1) * 2.0) * falloff * v1);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f1) * falloff * v1);
	if ((resEQ->framenumber * f2) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f2) * 2.0) * falloff * v2);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f2) * falloff * v2);
	if ((resEQ->framenumber * f3) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f3) * 2.0) * falloff * v3);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f3) * falloff * v3);
	if ((resEQ->framenumber * f4) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f4) * 2.0) * falloff * v4);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f4) * falloff * v4);
	if ((resEQ->framenumber * f5) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f5) * 2.0) * falloff * v5);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f5) * falloff * v5);
	if ((resEQ->framenumber * f6) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f6) * 2.0) * falloff * v6);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f6) * falloff * v6);
	if ((resEQ->framenumber * f7) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f7) * 2.0) * falloff * v7);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f7) * falloff * v7);
	if ((resEQ->framenumber * f8) < 1.57079633) resEQ->fL[resEQ->framenumber] += (sin((resEQ->framenumber * f8) * 2.0) * falloff * v8);
	else resEQ->fL[resEQ->framenumber] += (cos(resEQ->framenumber * f8) * falloff * v8);

	resEQ->fR[resEQ->framenumber] = 0.0;
	if ((resEQ->framenumber * f1) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f1) * 2.0) * falloff * v1);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f1) * falloff * v1);
	if ((resEQ->framenumber * f2) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f2) * 2.0) * falloff * v2);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f2) * falloff * v2);
	if ((resEQ->framenumber * f3) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f3) * 2.0) * falloff * v3);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f3) * falloff * v3);
	if ((resEQ->framenumber * f4) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f4) * 2.0) * falloff * v4);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f4) * falloff * v4);
	if ((resEQ->framenumber * f5) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f5) * 2.0) * falloff * v5);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f5) * falloff * v5);
	if ((resEQ->framenumber * f6) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f6) * 2.0) * falloff * v6);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f6) * falloff * v6);
	if ((resEQ->framenumber * f7) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f7) * 2.0) * falloff * v7);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f7) * falloff * v7);
	if ((resEQ->framenumber * f8) < 1.57079633) resEQ->fR[resEQ->framenumber] += (sin((resEQ->framenumber * f8) * 2.0) * falloff * v8);
	else resEQ->fR[resEQ->framenumber] += (cos(resEQ->framenumber * f8) * falloff * v8);

	// done updating the kernel for this go-round

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = resEQ->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = resEQ->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		resEQ->bL[59] = resEQ->bL[58];
		resEQ->bL[58] = resEQ->bL[57];
		resEQ->bL[57] = resEQ->bL[56];
		resEQ->bL[56] = resEQ->bL[55];
		resEQ->bL[55] = resEQ->bL[54];
		resEQ->bL[54] = resEQ->bL[53];
		resEQ->bL[53] = resEQ->bL[52];
		resEQ->bL[52] = resEQ->bL[51];
		resEQ->bL[51] = resEQ->bL[50];
		resEQ->bL[50] = resEQ->bL[49];
		resEQ->bL[49] = resEQ->bL[48];
		resEQ->bL[48] = resEQ->bL[47];
		resEQ->bL[47] = resEQ->bL[46];
		resEQ->bL[46] = resEQ->bL[45];
		resEQ->bL[45] = resEQ->bL[44];
		resEQ->bL[44] = resEQ->bL[43];
		resEQ->bL[43] = resEQ->bL[42];
		resEQ->bL[42] = resEQ->bL[41];
		resEQ->bL[41] = resEQ->bL[40];
		resEQ->bL[40] = resEQ->bL[39];
		resEQ->bL[39] = resEQ->bL[38];
		resEQ->bL[38] = resEQ->bL[37];
		resEQ->bL[37] = resEQ->bL[36];
		resEQ->bL[36] = resEQ->bL[35];
		resEQ->bL[35] = resEQ->bL[34];
		resEQ->bL[34] = resEQ->bL[33];
		resEQ->bL[33] = resEQ->bL[32];
		resEQ->bL[32] = resEQ->bL[31];
		resEQ->bL[31] = resEQ->bL[30];
		resEQ->bL[30] = resEQ->bL[29];
		resEQ->bL[29] = resEQ->bL[28];
		resEQ->bL[28] = resEQ->bL[27];
		resEQ->bL[27] = resEQ->bL[26];
		resEQ->bL[26] = resEQ->bL[25];
		resEQ->bL[25] = resEQ->bL[24];
		resEQ->bL[24] = resEQ->bL[23];
		resEQ->bL[23] = resEQ->bL[22];
		resEQ->bL[22] = resEQ->bL[21];
		resEQ->bL[21] = resEQ->bL[20];
		resEQ->bL[20] = resEQ->bL[19];
		resEQ->bL[19] = resEQ->bL[18];
		resEQ->bL[18] = resEQ->bL[17];
		resEQ->bL[17] = resEQ->bL[16];
		resEQ->bL[16] = resEQ->bL[15];
		resEQ->bL[15] = resEQ->bL[14];
		resEQ->bL[14] = resEQ->bL[13];
		resEQ->bL[13] = resEQ->bL[12];
		resEQ->bL[12] = resEQ->bL[11];
		resEQ->bL[11] = resEQ->bL[10];
		resEQ->bL[10] = resEQ->bL[9];
		resEQ->bL[9] = resEQ->bL[8];
		resEQ->bL[8] = resEQ->bL[7];
		resEQ->bL[7] = resEQ->bL[6];
		resEQ->bL[6] = resEQ->bL[5];
		resEQ->bL[5] = resEQ->bL[4];
		resEQ->bL[4] = resEQ->bL[3];
		resEQ->bL[3] = resEQ->bL[2];
		resEQ->bL[2] = resEQ->bL[1];
		resEQ->bL[1] = resEQ->bL[0];
		resEQ->bL[0] = inputSampleL;

		inputSampleL = (resEQ->bL[1] * resEQ->fL[1]);
		inputSampleL += (resEQ->bL[2] * resEQ->fL[2]);
		inputSampleL += (resEQ->bL[3] * resEQ->fL[3]);
		inputSampleL += (resEQ->bL[4] * resEQ->fL[4]);
		inputSampleL += (resEQ->bL[5] * resEQ->fL[5]);
		inputSampleL += (resEQ->bL[6] * resEQ->fL[6]);
		inputSampleL += (resEQ->bL[7] * resEQ->fL[7]);
		inputSampleL += (resEQ->bL[8] * resEQ->fL[8]);
		inputSampleL += (resEQ->bL[9] * resEQ->fL[9]);
		inputSampleL += (resEQ->bL[10] * resEQ->fL[10]);
		inputSampleL += (resEQ->bL[11] * resEQ->fL[11]);
		inputSampleL += (resEQ->bL[12] * resEQ->fL[12]);
		inputSampleL += (resEQ->bL[13] * resEQ->fL[13]);
		inputSampleL += (resEQ->bL[14] * resEQ->fL[14]);
		inputSampleL += (resEQ->bL[15] * resEQ->fL[15]);
		inputSampleL += (resEQ->bL[16] * resEQ->fL[16]);
		inputSampleL += (resEQ->bL[17] * resEQ->fL[17]);
		inputSampleL += (resEQ->bL[18] * resEQ->fL[18]);
		inputSampleL += (resEQ->bL[19] * resEQ->fL[19]);
		inputSampleL += (resEQ->bL[20] * resEQ->fL[20]);
		inputSampleL += (resEQ->bL[21] * resEQ->fL[21]);
		inputSampleL += (resEQ->bL[22] * resEQ->fL[22]);
		inputSampleL += (resEQ->bL[23] * resEQ->fL[23]);
		inputSampleL += (resEQ->bL[24] * resEQ->fL[24]);
		inputSampleL += (resEQ->bL[25] * resEQ->fL[25]);
		inputSampleL += (resEQ->bL[26] * resEQ->fL[26]);
		inputSampleL += (resEQ->bL[27] * resEQ->fL[27]);
		inputSampleL += (resEQ->bL[28] * resEQ->fL[28]);
		inputSampleL += (resEQ->bL[29] * resEQ->fL[29]);
		inputSampleL += (resEQ->bL[30] * resEQ->fL[30]);
		inputSampleL += (resEQ->bL[31] * resEQ->fL[31]);
		inputSampleL += (resEQ->bL[32] * resEQ->fL[32]);
		inputSampleL += (resEQ->bL[33] * resEQ->fL[33]);
		inputSampleL += (resEQ->bL[34] * resEQ->fL[34]);
		inputSampleL += (resEQ->bL[35] * resEQ->fL[35]);
		inputSampleL += (resEQ->bL[36] * resEQ->fL[36]);
		inputSampleL += (resEQ->bL[37] * resEQ->fL[37]);
		inputSampleL += (resEQ->bL[38] * resEQ->fL[38]);
		inputSampleL += (resEQ->bL[39] * resEQ->fL[39]);
		inputSampleL += (resEQ->bL[40] * resEQ->fL[40]);
		inputSampleL += (resEQ->bL[41] * resEQ->fL[41]);
		inputSampleL += (resEQ->bL[42] * resEQ->fL[42]);
		inputSampleL += (resEQ->bL[43] * resEQ->fL[43]);
		inputSampleL += (resEQ->bL[44] * resEQ->fL[44]);
		inputSampleL += (resEQ->bL[45] * resEQ->fL[45]);
		inputSampleL += (resEQ->bL[46] * resEQ->fL[46]);
		inputSampleL += (resEQ->bL[47] * resEQ->fL[47]);
		inputSampleL += (resEQ->bL[48] * resEQ->fL[48]);
		inputSampleL += (resEQ->bL[49] * resEQ->fL[49]);
		inputSampleL += (resEQ->bL[50] * resEQ->fL[50]);
		inputSampleL += (resEQ->bL[51] * resEQ->fL[51]);
		inputSampleL += (resEQ->bL[52] * resEQ->fL[52]);
		inputSampleL += (resEQ->bL[53] * resEQ->fL[53]);
		inputSampleL += (resEQ->bL[54] * resEQ->fL[54]);
		inputSampleL += (resEQ->bL[55] * resEQ->fL[55]);
		inputSampleL += (resEQ->bL[56] * resEQ->fL[56]);
		inputSampleL += (resEQ->bL[57] * resEQ->fL[57]);
		inputSampleL += (resEQ->bL[58] * resEQ->fL[58]);
		inputSampleL += (resEQ->bL[59] * resEQ->fL[59]);
		inputSampleL /= 12.0;
		// inlined- this is our little EQ kernel. Longer will give better tightness on bass frequencies.
		// Note that normal programmers will make this a loop, which isn't much slower if at all, on modern CPUs.
		// It was unrolled more or less to show how much is done when you define a loop like that: it's easy to specify stuff where a lot of grinding is done.

		resEQ->bR[59] = resEQ->bR[58];
		resEQ->bR[58] = resEQ->bR[57];
		resEQ->bR[57] = resEQ->bR[56];
		resEQ->bR[56] = resEQ->bR[55];
		resEQ->bR[55] = resEQ->bR[54];
		resEQ->bR[54] = resEQ->bR[53];
		resEQ->bR[53] = resEQ->bR[52];
		resEQ->bR[52] = resEQ->bR[51];
		resEQ->bR[51] = resEQ->bR[50];
		resEQ->bR[50] = resEQ->bR[49];
		resEQ->bR[49] = resEQ->bR[48];
		resEQ->bR[48] = resEQ->bR[47];
		resEQ->bR[47] = resEQ->bR[46];
		resEQ->bR[46] = resEQ->bR[45];
		resEQ->bR[45] = resEQ->bR[44];
		resEQ->bR[44] = resEQ->bR[43];
		resEQ->bR[43] = resEQ->bR[42];
		resEQ->bR[42] = resEQ->bR[41];
		resEQ->bR[41] = resEQ->bR[40];
		resEQ->bR[40] = resEQ->bR[39];
		resEQ->bR[39] = resEQ->bR[38];
		resEQ->bR[38] = resEQ->bR[37];
		resEQ->bR[37] = resEQ->bR[36];
		resEQ->bR[36] = resEQ->bR[35];
		resEQ->bR[35] = resEQ->bR[34];
		resEQ->bR[34] = resEQ->bR[33];
		resEQ->bR[33] = resEQ->bR[32];
		resEQ->bR[32] = resEQ->bR[31];
		resEQ->bR[31] = resEQ->bR[30];
		resEQ->bR[30] = resEQ->bR[29];
		resEQ->bR[29] = resEQ->bR[28];
		resEQ->bR[28] = resEQ->bR[27];
		resEQ->bR[27] = resEQ->bR[26];
		resEQ->bR[26] = resEQ->bR[25];
		resEQ->bR[25] = resEQ->bR[24];
		resEQ->bR[24] = resEQ->bR[23];
		resEQ->bR[23] = resEQ->bR[22];
		resEQ->bR[22] = resEQ->bR[21];
		resEQ->bR[21] = resEQ->bR[20];
		resEQ->bR[20] = resEQ->bR[19];
		resEQ->bR[19] = resEQ->bR[18];
		resEQ->bR[18] = resEQ->bR[17];
		resEQ->bR[17] = resEQ->bR[16];
		resEQ->bR[16] = resEQ->bR[15];
		resEQ->bR[15] = resEQ->bR[14];
		resEQ->bR[14] = resEQ->bR[13];
		resEQ->bR[13] = resEQ->bR[12];
		resEQ->bR[12] = resEQ->bR[11];
		resEQ->bR[11] = resEQ->bR[10];
		resEQ->bR[10] = resEQ->bR[9];
		resEQ->bR[9] = resEQ->bR[8];
		resEQ->bR[8] = resEQ->bR[7];
		resEQ->bR[7] = resEQ->bR[6];
		resEQ->bR[6] = resEQ->bR[5];
		resEQ->bR[5] = resEQ->bR[4];
		resEQ->bR[4] = resEQ->bR[3];
		resEQ->bR[3] = resEQ->bR[2];
		resEQ->bR[2] = resEQ->bR[1];
		resEQ->bR[1] = resEQ->bR[0];
		resEQ->bR[0] = inputSampleR;

		inputSampleR = (resEQ->bR[1] * resEQ->fR[1]);
		inputSampleR += (resEQ->bR[2] * resEQ->fR[2]);
		inputSampleR += (resEQ->bR[3] * resEQ->fR[3]);
		inputSampleR += (resEQ->bR[4] * resEQ->fR[4]);
		inputSampleR += (resEQ->bR[5] * resEQ->fR[5]);
		inputSampleR += (resEQ->bR[6] * resEQ->fR[6]);
		inputSampleR += (resEQ->bR[7] * resEQ->fR[7]);
		inputSampleR += (resEQ->bR[8] * resEQ->fR[8]);
		inputSampleR += (resEQ->bR[9] * resEQ->fR[9]);
		inputSampleR += (resEQ->bR[10] * resEQ->fR[10]);
		inputSampleR += (resEQ->bR[11] * resEQ->fR[11]);
		inputSampleR += (resEQ->bR[12] * resEQ->fR[12]);
		inputSampleR += (resEQ->bR[13] * resEQ->fR[13]);
		inputSampleR += (resEQ->bR[14] * resEQ->fR[14]);
		inputSampleR += (resEQ->bR[15] * resEQ->fR[15]);
		inputSampleR += (resEQ->bR[16] * resEQ->fR[16]);
		inputSampleR += (resEQ->bR[17] * resEQ->fR[17]);
		inputSampleR += (resEQ->bR[18] * resEQ->fR[18]);
		inputSampleR += (resEQ->bR[19] * resEQ->fR[19]);
		inputSampleR += (resEQ->bR[20] * resEQ->fR[20]);
		inputSampleR += (resEQ->bR[21] * resEQ->fR[21]);
		inputSampleR += (resEQ->bR[22] * resEQ->fR[22]);
		inputSampleR += (resEQ->bR[23] * resEQ->fR[23]);
		inputSampleR += (resEQ->bR[24] * resEQ->fR[24]);
		inputSampleR += (resEQ->bR[25] * resEQ->fR[25]);
		inputSampleR += (resEQ->bR[26] * resEQ->fR[26]);
		inputSampleR += (resEQ->bR[27] * resEQ->fR[27]);
		inputSampleR += (resEQ->bR[28] * resEQ->fR[28]);
		inputSampleR += (resEQ->bR[29] * resEQ->fR[29]);
		inputSampleR += (resEQ->bR[30] * resEQ->fR[30]);
		inputSampleR += (resEQ->bR[31] * resEQ->fR[31]);
		inputSampleR += (resEQ->bR[32] * resEQ->fR[32]);
		inputSampleR += (resEQ->bR[33] * resEQ->fR[33]);
		inputSampleR += (resEQ->bR[34] * resEQ->fR[34]);
		inputSampleR += (resEQ->bR[35] * resEQ->fR[35]);
		inputSampleR += (resEQ->bR[36] * resEQ->fR[36]);
		inputSampleR += (resEQ->bR[37] * resEQ->fR[37]);
		inputSampleR += (resEQ->bR[38] * resEQ->fR[38]);
		inputSampleR += (resEQ->bR[39] * resEQ->fR[39]);
		inputSampleR += (resEQ->bR[40] * resEQ->fR[40]);
		inputSampleR += (resEQ->bR[41] * resEQ->fR[41]);
		inputSampleR += (resEQ->bR[42] * resEQ->fR[42]);
		inputSampleR += (resEQ->bR[43] * resEQ->fR[43]);
		inputSampleR += (resEQ->bR[44] * resEQ->fR[44]);
		inputSampleR += (resEQ->bR[45] * resEQ->fR[45]);
		inputSampleR += (resEQ->bR[46] * resEQ->fR[46]);
		inputSampleR += (resEQ->bR[47] * resEQ->fR[47]);
		inputSampleR += (resEQ->bR[48] * resEQ->fR[48]);
		inputSampleR += (resEQ->bR[49] * resEQ->fR[49]);
		inputSampleR += (resEQ->bR[50] * resEQ->fR[50]);
		inputSampleR += (resEQ->bR[51] * resEQ->fR[51]);
		inputSampleR += (resEQ->bR[52] * resEQ->fR[52]);
		inputSampleR += (resEQ->bR[53] * resEQ->fR[53]);
		inputSampleR += (resEQ->bR[54] * resEQ->fR[54]);
		inputSampleR += (resEQ->bR[55] * resEQ->fR[55]);
		inputSampleR += (resEQ->bR[56] * resEQ->fR[56]);
		inputSampleR += (resEQ->bR[57] * resEQ->fR[57]);
		inputSampleR += (resEQ->bR[58] * resEQ->fR[58]);
		inputSampleR += (resEQ->bR[59] * resEQ->fR[59]);
		inputSampleR /= 12.0;
		// inlined- this is our little EQ kernel. Longer will give better tightness on bass frequencies.
		// Note that normal programmers will make this a loop, which isn't much slower if at all, on modern CPUs.
		// It was unrolled more or less to show how much is done when you define a loop like that: it's easy to specify stuff where a lot of grinding is done.

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		resEQ->fpdL ^= resEQ->fpdL << 13;
		resEQ->fpdL ^= resEQ->fpdL >> 17;
		resEQ->fpdL ^= resEQ->fpdL << 5;
		inputSampleL += (((double) resEQ->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		resEQ->fpdR ^= resEQ->fpdR << 13;
		resEQ->fpdR ^= resEQ->fpdR >> 17;
		resEQ->fpdR ^= resEQ->fpdR << 5;
		inputSampleR += (((double) resEQ->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	RESEQ_URI,
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
