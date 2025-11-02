#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define KGUITARHALL2_URI "https://hannesbraun.net/ns/lv2/airwindows/kguitarhall2"

#define d3A 874
#define d3B 1168
#define d3C 1315
#define d3D 590
#define d3E 1060
#define d3F 1162
#define d3G 545
#define d3H 574
#define d3I 1146
#define THREEBYTHREE true
#define d6A 1315
#define d6B 32
#define d6C 459
#define d6D 97
#define d6E 358
#define d6F 65
#define d6G 47
#define d6H 67
#define d6I 1146
#define d6J 312
#define d6K 296
#define d6L 17
#define d6M 35
#define d6N 1162
#define d6O 1060
#define d6P 355
#define d6Q 56
#define d6R 250
#define d6S 161
#define d6T 8
#define d6U 45
#define d6V 545
#define d6W 543
#define d6X 203
#define d6Y 590
#define d6ZA 24
#define d6ZB 121
#define d6ZC 35
#define d6ZD 52
#define d6ZE 874
#define d6ZF 15
#define d6ZG 574
#define d6ZH 343
#define d6ZI 24
#define d6ZJ 102
#define d6ZK 1168
// 2 to 140 ms, 612 seat theater
#define SIXBYSIX true
// 612-GGCCGF-JLBX6 kGuitarHall2
const int early[] = {8, 15, 17, 24, 24, 32, 35, 35, 45, 47, 52, 56, 65, 67, 97, 102, 121, 161, 203, 250, 296, 312, 343, 355, 358, 459, 543, 545, 574, 590, 874, 1060, 1146, 1162, 1168, 1315};

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	REGEN = 4,
	DEREZ = 5,
	FILTER = 6,
	EARLY_RF = 7,
	POSITIN = 8,
	DRY_WET = 9
} PortIndex;

enum {
	bez_AL,
	bez_AR,
	bez_BL,
	bez_BR,
	bez_CL,
	bez_CR,
	bez_InL,
	bez_InR,
	bez_UnInL,
	bez_UnInR,
	bez_SampL,
	bez_SampR,
	bez_AvgInSampL,
	bez_AvgInSampR,
	bez_AvgOutSampL,
	bez_AvgOutSampR,
	bez_cycle,
	bez_total
}; // the new undersampling. bez signifies the bezier curve reconstruction

enum {
	prevSampL1,
	prevSlewL1,
	prevSampR1,
	prevSlewR1,
	prevSampL2,
	prevSlewL2,
	prevSampR2,
	prevSlewR2,
	prevSampL3,
	prevSlewL3,
	prevSampR3,
	prevSlewR3,
	pear_total
}; // fixed frequency pear filter for ultrasonics, stereo

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* regen;
	const float* derez;
	const float* filter;
	const float* earlyRF;
	const float* positin;
	const float* dryWet;

	double a3AL[d3A + 5];
	double a3BL[d3B + 5];
	double a3CL[d3C + 5];
	double a3DL[d3D + 5];
	double a3EL[d3E + 5];
	double a3FL[d3F + 5];
	double a3GL[d3G + 5];
	double a3HL[d3H + 5];
	double a3IL[d3I + 5];
	double a3AR[d3A + 5];
	double a3BR[d3B + 5];
	double a3CR[d3C + 5];
	double a3DR[d3D + 5];
	double a3ER[d3E + 5];
	double a3FR[d3F + 5];
	double a3GR[d3G + 5];
	double a3HR[d3H + 5];
	double a3IR[d3I + 5];
	int c3AL, c3AR, c3BL, c3BR, c3CL, c3CR, c3DL, c3DR, c3EL, c3ER;
	int c3FL, c3FR, c3GL, c3GR, c3HL, c3HR, c3IL, c3IR;

	double a6AL[d6A + 5];
	double a6BL[d6B + 5];
	double a6CL[d6C + 5];
	double a6DL[d6D + 5];
	double a6EL[d6E + 5];
	double a6FL[d6F + 5];
	double a6GL[d6G + 5];
	double a6HL[d6H + 5];
	double a6IL[d6I + 5];
	double a6JL[d6J + 5];
	double a6KL[d6K + 5];
	double a6LL[d6L + 5];
	double a6ML[d6M + 5];
	double a6NL[d6N + 5];
	double a6OL[d6O + 5];
	double a6PL[d6P + 5];
	double a6QL[d6Q + 5];
	double a6RL[d6R + 5];
	double a6SL[d6S + 5];
	double a6TL[d6T + 5];
	double a6UL[d6U + 5];
	double a6VL[d6V + 5];
	double a6WL[d6W + 5];
	double a6XL[d6X + 5];
	double a6YL[d6Y + 5];
	double a6ZAL[d6ZA + 5];
	double a6ZBL[d6ZB + 5];
	double a6ZCL[d6ZC + 5];
	double a6ZDL[d6ZD + 5];
	double a6ZEL[d6ZE + 5];
	double a6ZFL[d6ZF + 5];
	double a6ZGL[d6ZG + 5];
	double a6ZHL[d6ZH + 5];
	double a6ZIL[d6ZI + 5];
	double a6ZJL[d6ZJ + 5];
	double a6ZKL[d6ZK + 5];
	double a6AR[d6A + 5];
	double a6BR[d6B + 5];
	double a6CR[d6C + 5];
	double a6DR[d6D + 5];
	double a6ER[d6E + 5];
	double a6FR[d6F + 5];
	double a6GR[d6G + 5];
	double a6HR[d6H + 5];
	double a6IR[d6I + 5];
	double a6JR[d6J + 5];
	double a6KR[d6K + 5];
	double a6LR[d6L + 5];
	double a6MR[d6M + 5];
	double a6NR[d6N + 5];
	double a6OR[d6O + 5];
	double a6PR[d6P + 5];
	double a6QR[d6Q + 5];
	double a6RR[d6R + 5];
	double a6SR[d6S + 5];
	double a6TR[d6T + 5];
	double a6UR[d6U + 5];
	double a6VR[d6V + 5];
	double a6WR[d6W + 5];
	double a6XR[d6X + 5];
	double a6YR[d6Y + 5];
	double a6ZAR[d6ZA + 5];
	double a6ZBR[d6ZB + 5];
	double a6ZCR[d6ZC + 5];
	double a6ZDR[d6ZD + 5];
	double a6ZER[d6ZE + 5];
	double a6ZFR[d6ZF + 5];
	double a6ZGR[d6ZG + 5];
	double a6ZHR[d6ZH + 5];
	double a6ZIR[d6ZI + 5];
	double a6ZJR[d6ZJ + 5];
	double a6ZKR[d6ZK + 5];
	int c6AL, c6BL, c6CL, c6DL, c6EL, c6FL, c6GL, c6HL, c6IL;
	int c6JL, c6KL, c6LL, c6ML, c6NL, c6OL, c6PL, c6QL, c6RL;
	int c6SL, c6TL, c6UL, c6VL, c6WL, c6XL, c6YL, c6ZAL, c6ZBL;
	int c6ZCL, c6ZDL, c6ZEL, c6ZFL, c6ZGL, c6ZHL, c6ZIL, c6ZJL, c6ZKL;
	int c6AR, c6BR, c6CR, c6DR, c6ER, c6FR, c6GR, c6HR, c6IR;
	int c6JR, c6KR, c6LR, c6MR, c6NR, c6OR, c6PR, c6QR, c6RR;
	int c6SR, c6TR, c6UR, c6VR, c6WR, c6XR, c6YR, c6ZAR, c6ZBR;
	int c6ZCR, c6ZDR, c6ZER, c6ZFR, c6ZGR, c6ZHR, c6ZIR, c6ZJR, c6ZKR;
	double f6AL, f6BL, f6CL, f6DL, f6EL, f6FL;
	double f6FR, f6LR, f6RR, f6XR, f6ZER, f6ZKR;
	double avg6L, avg6R;

	double bez[bez_total];

	double pear[pear_total]; // probably worth just using a number here

	uint32_t fpdL;
	uint32_t fpdR;
} KGuitarHall2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	KGuitarHall2* kGuitarHall2 = (KGuitarHall2*) calloc(1, sizeof(KGuitarHall2));
	kGuitarHall2->sampleRate = rate;
	return (LV2_Handle) kGuitarHall2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	KGuitarHall2* kGuitarHall2 = (KGuitarHall2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			kGuitarHall2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			kGuitarHall2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			kGuitarHall2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			kGuitarHall2->output[1] = (float*) data;
			break;
		case REGEN:
			kGuitarHall2->regen = (const float*) data;
			break;
		case DEREZ:
			kGuitarHall2->derez = (const float*) data;
			break;
		case FILTER:
			kGuitarHall2->filter = (const float*) data;
			break;
		case EARLY_RF:
			kGuitarHall2->earlyRF = (const float*) data;
			break;
		case POSITIN:
			kGuitarHall2->positin = (const float*) data;
			break;
		case DRY_WET:
			kGuitarHall2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	KGuitarHall2* kGuitarHall2 = (KGuitarHall2*) instance;

	for (int x = 0; x < d3A + 2; x++) {
		kGuitarHall2->a3AL[x] = 0.0;
		kGuitarHall2->a3AR[x] = 0.0;
	}
	for (int x = 0; x < d3B + 2; x++) {
		kGuitarHall2->a3BL[x] = 0.0;
		kGuitarHall2->a3BR[x] = 0.0;
	}
	for (int x = 0; x < d3C + 2; x++) {
		kGuitarHall2->a3CL[x] = 0.0;
		kGuitarHall2->a3CR[x] = 0.0;
	}
	for (int x = 0; x < d3D + 2; x++) {
		kGuitarHall2->a3DL[x] = 0.0;
		kGuitarHall2->a3DR[x] = 0.0;
	}
	for (int x = 0; x < d3E + 2; x++) {
		kGuitarHall2->a3EL[x] = 0.0;
		kGuitarHall2->a3ER[x] = 0.0;
	}
	for (int x = 0; x < d3F + 2; x++) {
		kGuitarHall2->a3FL[x] = 0.0;
		kGuitarHall2->a3FR[x] = 0.0;
	}
	for (int x = 0; x < d3G + 2; x++) {
		kGuitarHall2->a3GL[x] = 0.0;
		kGuitarHall2->a3GR[x] = 0.0;
	}
	for (int x = 0; x < d3H + 2; x++) {
		kGuitarHall2->a3HL[x] = 0.0;
		kGuitarHall2->a3HR[x] = 0.0;
	}
	for (int x = 0; x < d3I + 2; x++) {
		kGuitarHall2->a3IL[x] = 0.0;
		kGuitarHall2->a3IR[x] = 0.0;
	}
	kGuitarHall2->c3AL = kGuitarHall2->c3BL = kGuitarHall2->c3CL = kGuitarHall2->c3DL = kGuitarHall2->c3EL = kGuitarHall2->c3FL = kGuitarHall2->c3GL = kGuitarHall2->c3HL = kGuitarHall2->c3IL = 1;
	kGuitarHall2->c3AR = kGuitarHall2->c3BR = kGuitarHall2->c3CR = kGuitarHall2->c3DR = kGuitarHall2->c3ER = kGuitarHall2->c3FR = kGuitarHall2->c3GR = kGuitarHall2->c3HR = kGuitarHall2->c3IR = 1;

	for (int x = 0; x < d6A + 2; x++) {
		kGuitarHall2->a6AL[x] = 0.0;
		kGuitarHall2->a6AR[x] = 0.0;
	}
	for (int x = 0; x < d6B + 2; x++) {
		kGuitarHall2->a6BL[x] = 0.0;
		kGuitarHall2->a6BR[x] = 0.0;
	}
	for (int x = 0; x < d6C + 2; x++) {
		kGuitarHall2->a6CL[x] = 0.0;
		kGuitarHall2->a6CR[x] = 0.0;
	}
	for (int x = 0; x < d6D + 2; x++) {
		kGuitarHall2->a6DL[x] = 0.0;
		kGuitarHall2->a6DR[x] = 0.0;
	}
	for (int x = 0; x < d6E + 2; x++) {
		kGuitarHall2->a6EL[x] = 0.0;
		kGuitarHall2->a6ER[x] = 0.0;
	}
	for (int x = 0; x < d6F + 2; x++) {
		kGuitarHall2->a6FL[x] = 0.0;
		kGuitarHall2->a6FR[x] = 0.0;
	}
	for (int x = 0; x < d6G + 2; x++) {
		kGuitarHall2->a6GL[x] = 0.0;
		kGuitarHall2->a6GR[x] = 0.0;
	}
	for (int x = 0; x < d6H + 2; x++) {
		kGuitarHall2->a6HL[x] = 0.0;
		kGuitarHall2->a6HR[x] = 0.0;
	}
	for (int x = 0; x < d6I + 2; x++) {
		kGuitarHall2->a6IL[x] = 0.0;
		kGuitarHall2->a6IR[x] = 0.0;
	}
	for (int x = 0; x < d6J + 2; x++) {
		kGuitarHall2->a6JL[x] = 0.0;
		kGuitarHall2->a6JR[x] = 0.0;
	}
	for (int x = 0; x < d6K + 2; x++) {
		kGuitarHall2->a6KL[x] = 0.0;
		kGuitarHall2->a6KR[x] = 0.0;
	}
	for (int x = 0; x < d6L + 2; x++) {
		kGuitarHall2->a6LL[x] = 0.0;
		kGuitarHall2->a6LR[x] = 0.0;
	}
	for (int x = 0; x < d6M + 2; x++) {
		kGuitarHall2->a6ML[x] = 0.0;
		kGuitarHall2->a6MR[x] = 0.0;
	}
	for (int x = 0; x < d6N + 2; x++) {
		kGuitarHall2->a6NL[x] = 0.0;
		kGuitarHall2->a6NR[x] = 0.0;
	}
	for (int x = 0; x < d6O + 2; x++) {
		kGuitarHall2->a6OL[x] = 0.0;
		kGuitarHall2->a6OR[x] = 0.0;
	}
	for (int x = 0; x < d6P + 2; x++) {
		kGuitarHall2->a6PL[x] = 0.0;
		kGuitarHall2->a6PR[x] = 0.0;
	}
	for (int x = 0; x < d6Q + 2; x++) {
		kGuitarHall2->a6QL[x] = 0.0;
		kGuitarHall2->a6QR[x] = 0.0;
	}
	for (int x = 0; x < d6R + 2; x++) {
		kGuitarHall2->a6RL[x] = 0.0;
		kGuitarHall2->a6RR[x] = 0.0;
	}
	for (int x = 0; x < d6S + 2; x++) {
		kGuitarHall2->a6SL[x] = 0.0;
		kGuitarHall2->a6SR[x] = 0.0;
	}
	for (int x = 0; x < d6T + 2; x++) {
		kGuitarHall2->a6TL[x] = 0.0;
		kGuitarHall2->a6TR[x] = 0.0;
	}
	for (int x = 0; x < d6U + 2; x++) {
		kGuitarHall2->a6UL[x] = 0.0;
		kGuitarHall2->a6UR[x] = 0.0;
	}
	for (int x = 0; x < d6V + 2; x++) {
		kGuitarHall2->a6VL[x] = 0.0;
		kGuitarHall2->a6VR[x] = 0.0;
	}
	for (int x = 0; x < d6W + 2; x++) {
		kGuitarHall2->a6WL[x] = 0.0;
		kGuitarHall2->a6WR[x] = 0.0;
	}
	for (int x = 0; x < d6X + 2; x++) {
		kGuitarHall2->a6XL[x] = 0.0;
		kGuitarHall2->a6XR[x] = 0.0;
	}
	for (int x = 0; x < d6Y + 2; x++) {
		kGuitarHall2->a6YL[x] = 0.0;
		kGuitarHall2->a6YR[x] = 0.0;
	}
	for (int x = 0; x < d6ZA + 2; x++) {
		kGuitarHall2->a6ZAL[x] = 0.0;
		kGuitarHall2->a6ZAR[x] = 0.0;
	}
	for (int x = 0; x < d6ZB + 2; x++) {
		kGuitarHall2->a6ZBL[x] = 0.0;
		kGuitarHall2->a6ZBR[x] = 0.0;
	}
	for (int x = 0; x < d6ZC + 2; x++) {
		kGuitarHall2->a6ZCL[x] = 0.0;
		kGuitarHall2->a6ZCR[x] = 0.0;
	}
	for (int x = 0; x < d6ZD + 2; x++) {
		kGuitarHall2->a6ZDL[x] = 0.0;
		kGuitarHall2->a6ZDR[x] = 0.0;
	}
	for (int x = 0; x < d6ZE + 2; x++) {
		kGuitarHall2->a6ZEL[x] = 0.0;
		kGuitarHall2->a6ZER[x] = 0.0;
	}
	for (int x = 0; x < d6ZF + 2; x++) {
		kGuitarHall2->a6ZFL[x] = 0.0;
		kGuitarHall2->a6ZFR[x] = 0.0;
	}
	for (int x = 0; x < d6ZG + 2; x++) {
		kGuitarHall2->a6ZGL[x] = 0.0;
		kGuitarHall2->a6ZGR[x] = 0.0;
	}
	for (int x = 0; x < d6ZH + 2; x++) {
		kGuitarHall2->a6ZHL[x] = 0.0;
		kGuitarHall2->a6ZHR[x] = 0.0;
	}
	for (int x = 0; x < d6ZI + 2; x++) {
		kGuitarHall2->a6ZIL[x] = 0.0;
		kGuitarHall2->a6ZIR[x] = 0.0;
	}
	for (int x = 0; x < d6ZJ + 2; x++) {
		kGuitarHall2->a6ZJL[x] = 0.0;
		kGuitarHall2->a6ZJR[x] = 0.0;
	}
	for (int x = 0; x < d6ZK + 2; x++) {
		kGuitarHall2->a6ZKL[x] = 0.0;
		kGuitarHall2->a6ZKR[x] = 0.0;
	}
	kGuitarHall2->c6AL = kGuitarHall2->c6BL = kGuitarHall2->c6CL = kGuitarHall2->c6DL = kGuitarHall2->c6EL = kGuitarHall2->c6FL = kGuitarHall2->c6GL = kGuitarHall2->c6HL = kGuitarHall2->c6IL = 1;
	kGuitarHall2->c6JL = kGuitarHall2->c6KL = kGuitarHall2->c6LL = kGuitarHall2->c6ML = kGuitarHall2->c6NL = kGuitarHall2->c6OL = kGuitarHall2->c6PL = kGuitarHall2->c6QL = kGuitarHall2->c6RL = 1;
	kGuitarHall2->c6SL = kGuitarHall2->c6TL = kGuitarHall2->c6UL = kGuitarHall2->c6VL = kGuitarHall2->c6WL = kGuitarHall2->c6XL = kGuitarHall2->c6YL = kGuitarHall2->c6ZAL = kGuitarHall2->c6ZBL = 1;
	kGuitarHall2->c6ZCL = kGuitarHall2->c6ZDL = kGuitarHall2->c6ZEL = kGuitarHall2->c6ZFL = kGuitarHall2->c6ZGL = kGuitarHall2->c6ZHL = kGuitarHall2->c6ZIL = kGuitarHall2->c6ZJL = kGuitarHall2->c6ZKL = 1;
	kGuitarHall2->c6AR = kGuitarHall2->c6BR = kGuitarHall2->c6CR = kGuitarHall2->c6DR = kGuitarHall2->c6ER = kGuitarHall2->c6FR = kGuitarHall2->c6GR = kGuitarHall2->c6HR = kGuitarHall2->c6IR = 1;
	kGuitarHall2->c6JR = kGuitarHall2->c6KR = kGuitarHall2->c6LR = kGuitarHall2->c6MR = kGuitarHall2->c6NR = kGuitarHall2->c6OR = kGuitarHall2->c6PR = kGuitarHall2->c6QR = kGuitarHall2->c6RR = 1;
	kGuitarHall2->c6SR = kGuitarHall2->c6TR = kGuitarHall2->c6UR = kGuitarHall2->c6VR = kGuitarHall2->c6WR = kGuitarHall2->c6XR = kGuitarHall2->c6YR = kGuitarHall2->c6ZAR = kGuitarHall2->c6ZBR = 1;
	kGuitarHall2->c6ZCR = kGuitarHall2->c6ZDR = kGuitarHall2->c6ZER = kGuitarHall2->c6ZFR = kGuitarHall2->c6ZGR = kGuitarHall2->c6ZHR = kGuitarHall2->c6ZIR = kGuitarHall2->c6ZJR = kGuitarHall2->c6ZKR = 1;
	kGuitarHall2->f6AL = kGuitarHall2->f6BL = kGuitarHall2->f6CL = kGuitarHall2->f6DL = kGuitarHall2->f6EL = kGuitarHall2->f6FL = 0.0;
	kGuitarHall2->f6FR = kGuitarHall2->f6LR = kGuitarHall2->f6RR = kGuitarHall2->f6XR = kGuitarHall2->f6ZER = kGuitarHall2->f6ZKR = 0.0;
	kGuitarHall2->avg6L = kGuitarHall2->avg6R = 0.0;

	for (int x = 0; x < bez_total; x++) kGuitarHall2->bez[x] = 0.0;
	kGuitarHall2->bez[bez_cycle] = 1.0;

	for (int x = 0; x < pear_total; x++) kGuitarHall2->pear[x] = 0.0;

	kGuitarHall2->fpdL = 1.0;
	while (kGuitarHall2->fpdL < 16386) kGuitarHall2->fpdL = rand() * UINT32_MAX;
	kGuitarHall2->fpdR = 1.0;
	while (kGuitarHall2->fpdR < 16386) kGuitarHall2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	KGuitarHall2* kGuitarHall2 = (KGuitarHall2*) instance;

	const float* in1 = kGuitarHall2->input[0];
	const float* in2 = kGuitarHall2->input[1];
	float* out1 = kGuitarHall2->output[0];
	float* out2 = kGuitarHall2->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= kGuitarHall2->sampleRate;

	double fdb6ck = (0.0009765625 + 0.0009765625 + 0.001953125) * 0.3333333;
	double reg6n = (1.0 - pow(1.0 - *kGuitarHall2->regen, 3.0)) * fdb6ck;

	double derez = *kGuitarHall2->derez * 2.0;
	bool stepped = true; // Revised Bezier Undersampling
	if (derez > 1.0) { // has full rez at center, stepped
		stepped = false; // to left, continuous to right
		derez = 1.0 - (derez - 1.0);
	} // if it's set up like that it's the revised algorithm
	derez = fmin(fmax(derez / overallscale, 0.0005), 1.0);
	int bezFraction = (int) (1.0 / derez);
	double bezTrim = (double) bezFraction / (bezFraction + 1.0);
	if (stepped) { // this hard-locks derez to exact subdivisions of 1.0
		derez = 1.0 / bezFraction;
		bezTrim = 1.0 - (derez * bezTrim);
	} else { // this makes it match the 1.0 case using stepped
		derez /= (2.0 / pow(overallscale, 0.5 - ((overallscale - 1.0) * 0.0375)));
		bezTrim = 1.0 - pow(derez * 0.5, 1.0 / (derez * 0.5));
	} // the revision more accurately connects the bezier curves

	double freq = *kGuitarHall2->filter + 0.02;
	double earlyLoudness = *kGuitarHall2->earlyRF;
	int start = (int) (*kGuitarHall2->positin * 27.0);
	int ld3G = early[start];
	int ld3H = early[start + 1];
	int ld3D = early[start + 2];
	int ld3A = early[start + 3];
	int ld3E = early[start + 4];
	int ld3I = early[start + 5];
	int ld3F = early[start + 6];
	int ld3B = early[start + 7];
	int ld3C = early[start + 8];
	double wet = *kGuitarHall2->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = kGuitarHall2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = kGuitarHall2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		kGuitarHall2->bez[bez_cycle] += derez;
		kGuitarHall2->bez[bez_SampL] += ((inputSampleL + kGuitarHall2->bez[bez_InL]) * derez);
		kGuitarHall2->bez[bez_SampR] += ((inputSampleR + kGuitarHall2->bez[bez_InR]) * derez);
		kGuitarHall2->bez[bez_InL] = inputSampleL;
		kGuitarHall2->bez[bez_InR] = inputSampleR;
		if (kGuitarHall2->bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
			if (stepped) kGuitarHall2->bez[bez_cycle] = 0.0;
			else kGuitarHall2->bez[bez_cycle] -= 1.0;

			inputSampleL = (kGuitarHall2->bez[bez_SampL] + kGuitarHall2->bez[bez_AvgInSampL]) * 0.5;
			kGuitarHall2->bez[bez_AvgInSampL] = kGuitarHall2->bez[bez_SampL];
			inputSampleR = (kGuitarHall2->bez[bez_SampR] + kGuitarHall2->bez[bez_AvgInSampR]) * 0.5;
			kGuitarHall2->bez[bez_AvgInSampR] = kGuitarHall2->bez[bez_SampR];

			kGuitarHall2->a3AL[kGuitarHall2->c3AL] = inputSampleL; // + (f3AL * reg3n);
			kGuitarHall2->a3BL[kGuitarHall2->c3BL] = inputSampleL; // + (f3BL * reg3n);
			kGuitarHall2->a3CL[kGuitarHall2->c3CL] = inputSampleL; // + (f3CL * reg3n);

			kGuitarHall2->a3CR[kGuitarHall2->c3CR] = inputSampleR; // + (f3CR * reg3n);
			kGuitarHall2->a3FR[kGuitarHall2->c3FR] = inputSampleR; // + (f3FR * reg3n);
			kGuitarHall2->a3IR[kGuitarHall2->c3IR] = inputSampleR; // + (f3IR * reg3n);

			kGuitarHall2->c3AL++;
			if (kGuitarHall2->c3AL < 0 || kGuitarHall2->c3AL > ld3A) kGuitarHall2->c3AL = 0;
			kGuitarHall2->c3BL++;
			if (kGuitarHall2->c3BL < 0 || kGuitarHall2->c3BL > ld3B) kGuitarHall2->c3BL = 0;
			kGuitarHall2->c3CL++;
			if (kGuitarHall2->c3CL < 0 || kGuitarHall2->c3CL > ld3C) kGuitarHall2->c3CL = 0;
			kGuitarHall2->c3CR++;
			if (kGuitarHall2->c3CR < 0 || kGuitarHall2->c3CR > ld3C) kGuitarHall2->c3CR = 0;
			kGuitarHall2->c3FR++;
			if (kGuitarHall2->c3FR < 0 || kGuitarHall2->c3FR > ld3F) kGuitarHall2->c3FR = 0;
			kGuitarHall2->c3IR++;
			if (kGuitarHall2->c3IR < 0 || kGuitarHall2->c3IR > ld3I) kGuitarHall2->c3IR = 0;

			double o3AL = kGuitarHall2->a3AL[kGuitarHall2->c3AL - ((kGuitarHall2->c3AL > ld3A) ? kGuitarHall2->c3AL + 1 : 0)];
			double o3BL = kGuitarHall2->a3BL[kGuitarHall2->c3BL - ((kGuitarHall2->c3BL > ld3B) ? kGuitarHall2->c3BL + 1 : 0)];
			double o3CL = kGuitarHall2->a3CL[kGuitarHall2->c3CL - ((kGuitarHall2->c3CL > ld3C) ? kGuitarHall2->c3CL + 1 : 0)];
			double o3CR = kGuitarHall2->a3CR[kGuitarHall2->c3CR - ((kGuitarHall2->c3CR > ld3C) ? kGuitarHall2->c3CR + 1 : 0)];
			double o3FR = kGuitarHall2->a3FR[kGuitarHall2->c3FR - ((kGuitarHall2->c3FR > ld3F) ? kGuitarHall2->c3FR + 1 : 0)];
			double o3IR = kGuitarHall2->a3IR[kGuitarHall2->c3IR - ((kGuitarHall2->c3IR > ld3I) ? kGuitarHall2->c3IR + 1 : 0)];

			kGuitarHall2->a3DL[kGuitarHall2->c3DL] = (((o3BL + o3CL) * -2.0) + o3AL);
			kGuitarHall2->a3EL[kGuitarHall2->c3EL] = (((o3AL + o3CL) * -2.0) + o3BL);
			kGuitarHall2->a3FL[kGuitarHall2->c3FL] = (((o3AL + o3BL) * -2.0) + o3CL);
			kGuitarHall2->a3BR[kGuitarHall2->c3BR] = (((o3FR + o3IR) * -2.0) + o3CR);
			kGuitarHall2->a3ER[kGuitarHall2->c3ER] = (((o3CR + o3IR) * -2.0) + o3FR);
			kGuitarHall2->a3HR[kGuitarHall2->c3HR] = (((o3CR + o3FR) * -2.0) + o3IR);

			kGuitarHall2->c3DL++;
			if (kGuitarHall2->c3DL < 0 || kGuitarHall2->c3DL > ld3D) kGuitarHall2->c3DL = 0;
			kGuitarHall2->c3EL++;
			if (kGuitarHall2->c3EL < 0 || kGuitarHall2->c3EL > ld3E) kGuitarHall2->c3EL = 0;
			kGuitarHall2->c3FL++;
			if (kGuitarHall2->c3FL < 0 || kGuitarHall2->c3FL > ld3F) kGuitarHall2->c3FL = 0;
			kGuitarHall2->c3BR++;
			if (kGuitarHall2->c3BR < 0 || kGuitarHall2->c3BR > ld3B) kGuitarHall2->c3BR = 0;
			kGuitarHall2->c3ER++;
			if (kGuitarHall2->c3ER < 0 || kGuitarHall2->c3ER > ld3E) kGuitarHall2->c3ER = 0;
			kGuitarHall2->c3HR++;
			if (kGuitarHall2->c3HR < 0 || kGuitarHall2->c3HR > ld3H) kGuitarHall2->c3HR = 0;

			double o3DL = kGuitarHall2->a3DL[kGuitarHall2->c3DL - ((kGuitarHall2->c3DL > ld3D) ? kGuitarHall2->c3DL + 1 : 0)];
			double o3EL = kGuitarHall2->a3EL[kGuitarHall2->c3EL - ((kGuitarHall2->c3EL > ld3E) ? kGuitarHall2->c3EL + 1 : 0)];
			double o3FL = kGuitarHall2->a3FL[kGuitarHall2->c3FL - ((kGuitarHall2->c3FL > ld3F) ? kGuitarHall2->c3FL + 1 : 0)];
			double o3BR = kGuitarHall2->a3BR[kGuitarHall2->c3BR - ((kGuitarHall2->c3BR > ld3B) ? kGuitarHall2->c3BR + 1 : 0)];
			double o3ER = kGuitarHall2->a3ER[kGuitarHall2->c3ER - ((kGuitarHall2->c3ER > ld3E) ? kGuitarHall2->c3ER + 1 : 0)];
			double o3HR = kGuitarHall2->a3HR[kGuitarHall2->c3HR - ((kGuitarHall2->c3HR > ld3H) ? kGuitarHall2->c3HR + 1 : 0)];

			kGuitarHall2->a3GL[kGuitarHall2->c3GL] = (((o3EL + o3FL) * -2.0) + o3DL);
			kGuitarHall2->a3HL[kGuitarHall2->c3HL] = (((o3DL + o3FL) * -2.0) + o3EL);
			kGuitarHall2->a3IL[kGuitarHall2->c3IL] = (((o3DL + o3EL) * -2.0) + o3FL);
			kGuitarHall2->a3AR[kGuitarHall2->c3AR] = (((o3ER + o3HR) * -2.0) + o3BR);
			kGuitarHall2->a3DR[kGuitarHall2->c3DR] = (((o3BR + o3HR) * -2.0) + o3ER);
			kGuitarHall2->a3GR[kGuitarHall2->c3GR] = (((o3BR + o3ER) * -2.0) + o3HR);

			kGuitarHall2->c3GL++;
			if (kGuitarHall2->c3GL < 0 || kGuitarHall2->c3GL > ld3G) kGuitarHall2->c3GL = 0;
			kGuitarHall2->c3HL++;
			if (kGuitarHall2->c3HL < 0 || kGuitarHall2->c3HL > ld3H) kGuitarHall2->c3HL = 0;
			kGuitarHall2->c3IL++;
			if (kGuitarHall2->c3IL < 0 || kGuitarHall2->c3IL > ld3I) kGuitarHall2->c3IL = 0;
			kGuitarHall2->c3AR++;
			if (kGuitarHall2->c3AR < 0 || kGuitarHall2->c3AR > ld3A) kGuitarHall2->c3AR = 0;
			kGuitarHall2->c3DR++;
			if (kGuitarHall2->c3DR < 0 || kGuitarHall2->c3DR > ld3D) kGuitarHall2->c3DR = 0;
			kGuitarHall2->c3GR++;
			if (kGuitarHall2->c3GR < 0 || kGuitarHall2->c3GR > ld3G) kGuitarHall2->c3GR = 0;

			double o3GL = kGuitarHall2->a3GL[kGuitarHall2->c3GL - ((kGuitarHall2->c3GL > ld3G) ? kGuitarHall2->c3GL + 1 : 0)];
			double o3HL = kGuitarHall2->a3HL[kGuitarHall2->c3HL - ((kGuitarHall2->c3HL > ld3H) ? kGuitarHall2->c3HL + 1 : 0)];
			double o3IL = kGuitarHall2->a3IL[kGuitarHall2->c3IL - ((kGuitarHall2->c3IL > ld3I) ? kGuitarHall2->c3IL + 1 : 0)];
			double o3AR = kGuitarHall2->a3AR[kGuitarHall2->c3AR - ((kGuitarHall2->c3AR > ld3A) ? kGuitarHall2->c3AR + 1 : 0)];
			double o3DR = kGuitarHall2->a3DR[kGuitarHall2->c3DR - ((kGuitarHall2->c3DR > ld3D) ? kGuitarHall2->c3DR + 1 : 0)];
			double o3GR = kGuitarHall2->a3GR[kGuitarHall2->c3GR - ((kGuitarHall2->c3GR > ld3G) ? kGuitarHall2->c3GR + 1 : 0)];

			double inputSampleL = (o3GL + o3HL + o3IL) * 0.03125;
			double inputSampleR = (o3AR + o3DR + o3GR) * 0.03125;

			double earlyReflectionL = inputSampleL;
			double earlyReflectionR = inputSampleR;

			if (freq < 1.0) {
				double di = fabs(freq * (1.0 + (inputSampleL * 0.125)));
				if (di > 1.0) di = 1.0;
				double slew = ((inputSampleL - kGuitarHall2->pear[prevSampL1]) + kGuitarHall2->pear[prevSlewL1]) * di * 0.5;
				kGuitarHall2->pear[prevSampL1] = inputSampleL = (di * inputSampleL) + ((1.0 - di) * (kGuitarHall2->pear[prevSampL1] + kGuitarHall2->pear[prevSlewL1]));
				kGuitarHall2->pear[prevSlewL1] = slew;
				di = fabs(freq * (1.0 + (inputSampleL)));
				if (di > 1.0) di = 1.0;
				slew = ((inputSampleL - kGuitarHall2->pear[prevSampL2]) + kGuitarHall2->pear[prevSlewL2]) * di * 0.5;
				kGuitarHall2->pear[prevSampL2] = inputSampleL = (di * inputSampleL) + ((1.0 - di) * (kGuitarHall2->pear[prevSampL2] + kGuitarHall2->pear[prevSlewL2]));
				kGuitarHall2->pear[prevSlewL2] = slew;
				di = fabs(freq * (1.0 + (inputSampleL)));
				if (di > 1.0) di = 1.0;
				slew = ((inputSampleL - kGuitarHall2->pear[prevSampL3]) + kGuitarHall2->pear[prevSlewL3]) * di * 0.5;
				kGuitarHall2->pear[prevSampL3] = inputSampleL = (di * inputSampleL) + ((1.0 - di) * (kGuitarHall2->pear[prevSampL3] + kGuitarHall2->pear[prevSlewL3]));
				kGuitarHall2->pear[prevSlewL3] = slew;

				di = fabs(freq * (1.0 + (inputSampleR * 0.125)));
				if (di > 1.0) di = 1.0;
				slew = ((inputSampleR - kGuitarHall2->pear[prevSampR1]) + kGuitarHall2->pear[prevSlewR1]) * di * 0.5;
				kGuitarHall2->pear[prevSampR1] = inputSampleR = (di * inputSampleR) + ((1.0 - di) * (kGuitarHall2->pear[prevSampR1] + kGuitarHall2->pear[prevSlewR1]));
				kGuitarHall2->pear[prevSlewR1] = slew;
				di = fabs(freq * (1.0 + (inputSampleR)));
				if (di > 1.0) di = 1.0;
				slew = ((inputSampleR - kGuitarHall2->pear[prevSampR2]) + kGuitarHall2->pear[prevSlewR2]) * di * 0.5;
				kGuitarHall2->pear[prevSampR2] = inputSampleR = (di * inputSampleR) + ((1.0 - di) * (kGuitarHall2->pear[prevSampR2] + kGuitarHall2->pear[prevSlewR2]));
				kGuitarHall2->pear[prevSlewR2] = slew;
				di = fabs(freq * (1.0 + (inputSampleR)));
				if (di > 1.0) di = 1.0;
				slew = ((inputSampleR - kGuitarHall2->pear[prevSampR3]) + kGuitarHall2->pear[prevSlewR3]) * di * 0.5;
				kGuitarHall2->pear[prevSampR3] = inputSampleR = (di * inputSampleR) + ((1.0 - di) * (kGuitarHall2->pear[prevSampR3] + kGuitarHall2->pear[prevSlewR3]));
				kGuitarHall2->pear[prevSlewR3] = slew;
			}

			kGuitarHall2->a6AL[kGuitarHall2->c6AL] = inputSampleL + (kGuitarHall2->f6BL * reg6n);
			kGuitarHall2->a6BL[kGuitarHall2->c6BL] = inputSampleL + (kGuitarHall2->f6CL * reg6n);
			kGuitarHall2->a6CL[kGuitarHall2->c6CL] = inputSampleL + (kGuitarHall2->f6DL * reg6n);
			kGuitarHall2->a6DL[kGuitarHall2->c6DL] = inputSampleL + (kGuitarHall2->f6EL * reg6n);
			kGuitarHall2->a6EL[kGuitarHall2->c6EL] = inputSampleL + (kGuitarHall2->f6FL * reg6n);
			kGuitarHall2->a6FL[kGuitarHall2->c6FL] = inputSampleL + (kGuitarHall2->f6AL * reg6n);

			kGuitarHall2->c6AL++;
			if (kGuitarHall2->c6AL < 0 || kGuitarHall2->c6AL > d6A) kGuitarHall2->c6AL = 0;
			kGuitarHall2->c6BL++;
			if (kGuitarHall2->c6BL < 0 || kGuitarHall2->c6BL > d6B) kGuitarHall2->c6BL = 0;
			kGuitarHall2->c6CL++;
			if (kGuitarHall2->c6CL < 0 || kGuitarHall2->c6CL > d6C) kGuitarHall2->c6CL = 0;
			kGuitarHall2->c6DL++;
			if (kGuitarHall2->c6DL < 0 || kGuitarHall2->c6DL > d6D) kGuitarHall2->c6DL = 0;
			kGuitarHall2->c6EL++;
			if (kGuitarHall2->c6EL < 0 || kGuitarHall2->c6EL > d6E) kGuitarHall2->c6EL = 0;
			kGuitarHall2->c6FL++;
			if (kGuitarHall2->c6FL < 0 || kGuitarHall2->c6FL > d6F) kGuitarHall2->c6FL = 0;

			double o6AL = kGuitarHall2->a6AL[kGuitarHall2->c6AL - ((kGuitarHall2->c6AL > d6A) ? d6A + 1 : 0)];
			double o6BL = kGuitarHall2->a6BL[kGuitarHall2->c6BL - ((kGuitarHall2->c6BL > d6B) ? d6B + 1 : 0)];
			double o6CL = kGuitarHall2->a6CL[kGuitarHall2->c6CL - ((kGuitarHall2->c6CL > d6C) ? d6C + 1 : 0)];
			double o6DL = kGuitarHall2->a6DL[kGuitarHall2->c6DL - ((kGuitarHall2->c6DL > d6D) ? d6D + 1 : 0)];
			double o6EL = kGuitarHall2->a6EL[kGuitarHall2->c6EL - ((kGuitarHall2->c6EL > d6E) ? d6E + 1 : 0)];
			double o6FL = kGuitarHall2->a6FL[kGuitarHall2->c6FL - ((kGuitarHall2->c6FL > d6F) ? d6F + 1 : 0)];

			kGuitarHall2->a6FR[kGuitarHall2->c6FR] = inputSampleR + (kGuitarHall2->f6LR * reg6n);
			kGuitarHall2->a6LR[kGuitarHall2->c6LR] = inputSampleR + (kGuitarHall2->f6RR * reg6n);
			kGuitarHall2->a6RR[kGuitarHall2->c6RR] = inputSampleR + (kGuitarHall2->f6XR * reg6n);
			kGuitarHall2->a6XR[kGuitarHall2->c6XR] = inputSampleR + (kGuitarHall2->f6ZER * reg6n);
			kGuitarHall2->a6ZER[kGuitarHall2->c6ZER] = inputSampleR + (kGuitarHall2->f6ZKR * reg6n);
			kGuitarHall2->a6ZKR[kGuitarHall2->c6ZKR] = inputSampleR + (kGuitarHall2->f6FR * reg6n);

			kGuitarHall2->c6FR++;
			if (kGuitarHall2->c6FR < 0 || kGuitarHall2->c6FR > d6F) kGuitarHall2->c6FR = 0;
			kGuitarHall2->c6LR++;
			if (kGuitarHall2->c6LR < 0 || kGuitarHall2->c6LR > d6L) kGuitarHall2->c6LR = 0;
			kGuitarHall2->c6RR++;
			if (kGuitarHall2->c6RR < 0 || kGuitarHall2->c6RR > d6R) kGuitarHall2->c6RR = 0;
			kGuitarHall2->c6XR++;
			if (kGuitarHall2->c6XR < 0 || kGuitarHall2->c6XR > d6X) kGuitarHall2->c6XR = 0;
			kGuitarHall2->c6ZER++;
			if (kGuitarHall2->c6ZER < 0 || kGuitarHall2->c6ZER > d6ZE) kGuitarHall2->c6ZER = 0;
			kGuitarHall2->c6ZKR++;
			if (kGuitarHall2->c6ZKR < 0 || kGuitarHall2->c6ZKR > d6ZK) kGuitarHall2->c6ZKR = 0;

			double o6FR = kGuitarHall2->a6FR[kGuitarHall2->c6FR - ((kGuitarHall2->c6FR > d6F) ? d6F + 1 : 0)];
			double o6LR = kGuitarHall2->a6LR[kGuitarHall2->c6LR - ((kGuitarHall2->c6LR > d6L) ? d6L + 1 : 0)];
			double o6RR = kGuitarHall2->a6RR[kGuitarHall2->c6RR - ((kGuitarHall2->c6RR > d6R) ? d6R + 1 : 0)];
			double o6XR = kGuitarHall2->a6XR[kGuitarHall2->c6XR - ((kGuitarHall2->c6XR > d6X) ? d6X + 1 : 0)];
			double o6ZER = kGuitarHall2->a6ZER[kGuitarHall2->c6ZER - ((kGuitarHall2->c6ZER > d6ZE) ? d6ZE + 1 : 0)];
			double o6ZKR = kGuitarHall2->a6ZKR[kGuitarHall2->c6ZKR - ((kGuitarHall2->c6ZKR > d6ZK) ? d6ZK + 1 : 0)];

			//-------- one

			kGuitarHall2->a6GL[kGuitarHall2->c6GL] = ((o6AL * 2.0) - (o6BL + o6CL + o6DL + o6EL + o6FL));
			kGuitarHall2->a6HL[kGuitarHall2->c6HL] = ((o6BL * 2.0) - (o6AL + o6CL + o6DL + o6EL + o6FL));
			kGuitarHall2->a6IL[kGuitarHall2->c6IL] = ((o6CL * 2.0) - (o6AL + o6BL + o6DL + o6EL + o6FL));
			kGuitarHall2->a6JL[kGuitarHall2->c6JL] = ((o6DL * 2.0) - (o6AL + o6BL + o6CL + o6EL + o6FL));
			kGuitarHall2->a6KL[kGuitarHall2->c6KL] = ((o6EL * 2.0) - (o6AL + o6BL + o6CL + o6DL + o6FL));
			kGuitarHall2->a6LL[kGuitarHall2->c6LL] = ((o6FL * 2.0) - (o6AL + o6BL + o6CL + o6DL + o6EL));

			kGuitarHall2->c6GL++;
			if (kGuitarHall2->c6GL < 0 || kGuitarHall2->c6GL > d6G) kGuitarHall2->c6GL = 0;
			kGuitarHall2->c6HL++;
			if (kGuitarHall2->c6HL < 0 || kGuitarHall2->c6HL > d6H) kGuitarHall2->c6HL = 0;
			kGuitarHall2->c6IL++;
			if (kGuitarHall2->c6IL < 0 || kGuitarHall2->c6IL > d6I) kGuitarHall2->c6IL = 0;
			kGuitarHall2->c6JL++;
			if (kGuitarHall2->c6JL < 0 || kGuitarHall2->c6JL > d6J) kGuitarHall2->c6JL = 0;
			kGuitarHall2->c6KL++;
			if (kGuitarHall2->c6KL < 0 || kGuitarHall2->c6KL > d6K) kGuitarHall2->c6KL = 0;
			kGuitarHall2->c6LL++;
			if (kGuitarHall2->c6LL < 0 || kGuitarHall2->c6LL > d6L) kGuitarHall2->c6LL = 0;

			double o6GL = kGuitarHall2->a6GL[kGuitarHall2->c6GL - ((kGuitarHall2->c6GL > d6G) ? d6G + 1 : 0)];
			double o6HL = kGuitarHall2->a6HL[kGuitarHall2->c6HL - ((kGuitarHall2->c6HL > d6H) ? d6H + 1 : 0)];
			double o6IL = kGuitarHall2->a6IL[kGuitarHall2->c6IL - ((kGuitarHall2->c6IL > d6I) ? d6I + 1 : 0)];
			double o6JL = kGuitarHall2->a6JL[kGuitarHall2->c6JL - ((kGuitarHall2->c6JL > d6J) ? d6J + 1 : 0)];
			double o6KL = kGuitarHall2->a6KL[kGuitarHall2->c6KL - ((kGuitarHall2->c6KL > d6K) ? d6K + 1 : 0)];
			double o6LL = kGuitarHall2->a6LL[kGuitarHall2->c6LL - ((kGuitarHall2->c6LL > d6L) ? d6L + 1 : 0)];

			kGuitarHall2->a6ER[kGuitarHall2->c6ER] = ((o6FR * 2.0) - (o6LR + o6RR + o6XR + o6ZER + o6ZKR));
			kGuitarHall2->a6KR[kGuitarHall2->c6KR] = ((o6LR * 2.0) - (o6FR + o6RR + o6XR + o6ZER + o6ZKR));
			kGuitarHall2->a6QR[kGuitarHall2->c6QR] = ((o6RR * 2.0) - (o6FR + o6LR + o6XR + o6ZER + o6ZKR));
			kGuitarHall2->a6WR[kGuitarHall2->c6WR] = ((o6XR * 2.0) - (o6FR + o6LR + o6RR + o6ZER + o6ZKR));
			kGuitarHall2->a6ZDR[kGuitarHall2->c6ZDR] = ((o6ZER * 2.0) - (o6FR + o6LR + o6RR + o6XR + o6ZKR));
			kGuitarHall2->a6ZJR[kGuitarHall2->c6ZJR] = ((o6ZKR * 2.0) - (o6FR + o6LR + o6RR + o6XR + o6ZER));

			kGuitarHall2->c6ER++;
			if (kGuitarHall2->c6ER < 0 || kGuitarHall2->c6ER > d6E) kGuitarHall2->c6ER = 0;
			kGuitarHall2->c6KR++;
			if (kGuitarHall2->c6KR < 0 || kGuitarHall2->c6KR > d6K) kGuitarHall2->c6KR = 0;
			kGuitarHall2->c6QR++;
			if (kGuitarHall2->c6QR < 0 || kGuitarHall2->c6QR > d6Q) kGuitarHall2->c6QR = 0;
			kGuitarHall2->c6WR++;
			if (kGuitarHall2->c6WR < 0 || kGuitarHall2->c6WR > d6W) kGuitarHall2->c6WR = 0;
			kGuitarHall2->c6ZDR++;
			if (kGuitarHall2->c6ZDR < 0 || kGuitarHall2->c6ZDR > d6ZD) kGuitarHall2->c6ZDR = 0;
			kGuitarHall2->c6ZJR++;
			if (kGuitarHall2->c6ZJR < 0 || kGuitarHall2->c6ZJR > d6ZJ) kGuitarHall2->c6ZJR = 0;

			double o6ER = kGuitarHall2->a6ER[kGuitarHall2->c6ER - ((kGuitarHall2->c6ER > d6E) ? d6E + 1 : 0)];
			double o6KR = kGuitarHall2->a6KR[kGuitarHall2->c6KR - ((kGuitarHall2->c6KR > d6K) ? d6K + 1 : 0)];
			double o6QR = kGuitarHall2->a6QR[kGuitarHall2->c6QR - ((kGuitarHall2->c6QR > d6Q) ? d6Q + 1 : 0)];
			double o6WR = kGuitarHall2->a6WR[kGuitarHall2->c6WR - ((kGuitarHall2->c6WR > d6W) ? d6W + 1 : 0)];
			double o6ZDR = kGuitarHall2->a6ZDR[kGuitarHall2->c6ZDR - ((kGuitarHall2->c6ZDR > d6ZD) ? d6ZD + 1 : 0)];
			double o6ZJR = kGuitarHall2->a6ZJR[kGuitarHall2->c6ZJR - ((kGuitarHall2->c6ZJR > d6ZJ) ? d6ZJ + 1 : 0)];

			//-------- two

			kGuitarHall2->a6ML[kGuitarHall2->c6ML] = ((o6GL * 2.0) - (o6HL + o6IL + o6JL + o6KL + o6LL));
			kGuitarHall2->a6NL[kGuitarHall2->c6NL] = ((o6HL * 2.0) - (o6GL + o6IL + o6JL + o6KL + o6LL));
			kGuitarHall2->a6OL[kGuitarHall2->c6OL] = ((o6IL * 2.0) - (o6GL + o6HL + o6JL + o6KL + o6LL));
			kGuitarHall2->a6PL[kGuitarHall2->c6PL] = ((o6JL * 2.0) - (o6GL + o6HL + o6IL + o6KL + o6LL));
			kGuitarHall2->a6QL[kGuitarHall2->c6QL] = ((o6KL * 2.0) - (o6GL + o6HL + o6IL + o6JL + o6LL));
			kGuitarHall2->a6RL[kGuitarHall2->c6RL] = ((o6LL * 2.0) - (o6GL + o6HL + o6IL + o6JL + o6KL));

			kGuitarHall2->c6ML++;
			if (kGuitarHall2->c6ML < 0 || kGuitarHall2->c6ML > d6M) kGuitarHall2->c6ML = 0;
			kGuitarHall2->c6NL++;
			if (kGuitarHall2->c6NL < 0 || kGuitarHall2->c6NL > d6N) kGuitarHall2->c6NL = 0;
			kGuitarHall2->c6OL++;
			if (kGuitarHall2->c6OL < 0 || kGuitarHall2->c6OL > d6O) kGuitarHall2->c6OL = 0;
			kGuitarHall2->c6PL++;
			if (kGuitarHall2->c6PL < 0 || kGuitarHall2->c6PL > d6P) kGuitarHall2->c6PL = 0;
			kGuitarHall2->c6QL++;
			if (kGuitarHall2->c6QL < 0 || kGuitarHall2->c6QL > d6Q) kGuitarHall2->c6QL = 0;
			kGuitarHall2->c6RL++;
			if (kGuitarHall2->c6RL < 0 || kGuitarHall2->c6RL > d6R) kGuitarHall2->c6RL = 0;

			double o6ML = kGuitarHall2->a6ML[kGuitarHall2->c6ML - ((kGuitarHall2->c6ML > d6M) ? d6M + 1 : 0)];
			double o6NL = kGuitarHall2->a6NL[kGuitarHall2->c6NL - ((kGuitarHall2->c6NL > d6N) ? d6N + 1 : 0)];
			double o6OL = kGuitarHall2->a6OL[kGuitarHall2->c6OL - ((kGuitarHall2->c6OL > d6O) ? d6O + 1 : 0)];
			double o6PL = kGuitarHall2->a6PL[kGuitarHall2->c6PL - ((kGuitarHall2->c6PL > d6P) ? d6P + 1 : 0)];
			double o6QL = kGuitarHall2->a6QL[kGuitarHall2->c6QL - ((kGuitarHall2->c6QL > d6Q) ? d6Q + 1 : 0)];
			double o6RL = kGuitarHall2->a6RL[kGuitarHall2->c6RL - ((kGuitarHall2->c6RL > d6R) ? d6R + 1 : 0)];

			kGuitarHall2->a6DR[kGuitarHall2->c6DR] = ((o6ER * 2.0) - (o6KR + o6QR + o6WR + o6ZDR + o6ZJR));
			kGuitarHall2->a6JR[kGuitarHall2->c6JR] = ((o6KR * 2.0) - (o6ER + o6QR + o6WR + o6ZDR + o6ZJR));
			kGuitarHall2->a6PR[kGuitarHall2->c6PR] = ((o6QR * 2.0) - (o6ER + o6KR + o6WR + o6ZDR + o6ZJR));
			kGuitarHall2->a6VR[kGuitarHall2->c6VR] = ((o6WR * 2.0) - (o6ER + o6KR + o6QR + o6ZDR + o6ZJR));
			kGuitarHall2->a6ZCR[kGuitarHall2->c6ZCR] = ((o6ZDR * 2.0) - (o6ER + o6KR + o6QR + o6WR + o6ZJR));
			kGuitarHall2->a6ZIR[kGuitarHall2->c6ZIR] = ((o6ZJR * 2.0) - (o6ER + o6KR + o6QR + o6WR + o6ZDR));

			kGuitarHall2->c6DR++;
			if (kGuitarHall2->c6DR < 0 || kGuitarHall2->c6DR > d6D) kGuitarHall2->c6DR = 0;
			kGuitarHall2->c6JR++;
			if (kGuitarHall2->c6JR < 0 || kGuitarHall2->c6JR > d6J) kGuitarHall2->c6JR = 0;
			kGuitarHall2->c6PR++;
			if (kGuitarHall2->c6PR < 0 || kGuitarHall2->c6PR > d6P) kGuitarHall2->c6PR = 0;
			kGuitarHall2->c6VR++;
			if (kGuitarHall2->c6VR < 0 || kGuitarHall2->c6VR > d6V) kGuitarHall2->c6VR = 0;
			kGuitarHall2->c6ZCR++;
			if (kGuitarHall2->c6ZCR < 0 || kGuitarHall2->c6ZCR > d6ZC) kGuitarHall2->c6ZCR = 0;
			kGuitarHall2->c6ZIR++;
			if (kGuitarHall2->c6ZIR < 0 || kGuitarHall2->c6ZIR > d6ZI) kGuitarHall2->c6ZIR = 0;

			double o6DR = kGuitarHall2->a6DR[kGuitarHall2->c6DR - ((kGuitarHall2->c6DR > d6D) ? d6D + 1 : 0)];
			double o6JR = kGuitarHall2->a6JR[kGuitarHall2->c6JR - ((kGuitarHall2->c6JR > d6J) ? d6J + 1 : 0)];
			double o6PR = kGuitarHall2->a6PR[kGuitarHall2->c6PR - ((kGuitarHall2->c6PR > d6P) ? d6P + 1 : 0)];
			double o6VR = kGuitarHall2->a6VR[kGuitarHall2->c6VR - ((kGuitarHall2->c6VR > d6V) ? d6V + 1 : 0)];
			double o6ZCR = kGuitarHall2->a6ZCR[kGuitarHall2->c6ZCR - ((kGuitarHall2->c6ZCR > d6ZC) ? d6ZC + 1 : 0)];
			double o6ZIR = kGuitarHall2->a6ZIR[kGuitarHall2->c6ZIR - ((kGuitarHall2->c6ZIR > d6ZI) ? d6ZI + 1 : 0)];

			//-------- three

			kGuitarHall2->a6SL[kGuitarHall2->c6SL] = ((o6ML * 2.0) - (o6NL + o6OL + o6PL + o6QL + o6RL));
			kGuitarHall2->a6TL[kGuitarHall2->c6TL] = ((o6NL * 2.0) - (o6ML + o6OL + o6PL + o6QL + o6RL));
			kGuitarHall2->a6UL[kGuitarHall2->c6UL] = ((o6OL * 2.0) - (o6ML + o6NL + o6PL + o6QL + o6RL));
			kGuitarHall2->a6VL[kGuitarHall2->c6VL] = ((o6PL * 2.0) - (o6ML + o6NL + o6OL + o6QL + o6RL));
			kGuitarHall2->a6WL[kGuitarHall2->c6WL] = ((o6QL * 2.0) - (o6ML + o6NL + o6OL + o6PL + o6RL));
			kGuitarHall2->a6XL[kGuitarHall2->c6XL] = ((o6RL * 2.0) - (o6ML + o6NL + o6OL + o6PL + o6QL));

			kGuitarHall2->c6SL++;
			if (kGuitarHall2->c6SL < 0 || kGuitarHall2->c6SL > d6S) kGuitarHall2->c6SL = 0;
			kGuitarHall2->c6TL++;
			if (kGuitarHall2->c6TL < 0 || kGuitarHall2->c6TL > d6T) kGuitarHall2->c6TL = 0;
			kGuitarHall2->c6UL++;
			if (kGuitarHall2->c6UL < 0 || kGuitarHall2->c6UL > d6U) kGuitarHall2->c6UL = 0;
			kGuitarHall2->c6VL++;
			if (kGuitarHall2->c6VL < 0 || kGuitarHall2->c6VL > d6V) kGuitarHall2->c6VL = 0;
			kGuitarHall2->c6WL++;
			if (kGuitarHall2->c6WL < 0 || kGuitarHall2->c6WL > d6W) kGuitarHall2->c6WL = 0;
			kGuitarHall2->c6XL++;
			if (kGuitarHall2->c6XL < 0 || kGuitarHall2->c6XL > d6X) kGuitarHall2->c6XL = 0;

			double o6SL = kGuitarHall2->a6SL[kGuitarHall2->c6SL - ((kGuitarHall2->c6SL > d6S) ? d6S + 1 : 0)];
			double o6TL = kGuitarHall2->a6TL[kGuitarHall2->c6TL - ((kGuitarHall2->c6TL > d6T) ? d6T + 1 : 0)];
			double o6UL = kGuitarHall2->a6UL[kGuitarHall2->c6UL - ((kGuitarHall2->c6UL > d6U) ? d6U + 1 : 0)];
			double o6VL = kGuitarHall2->a6VL[kGuitarHall2->c6VL - ((kGuitarHall2->c6VL > d6V) ? d6V + 1 : 0)];
			double o6WL = kGuitarHall2->a6WL[kGuitarHall2->c6WL - ((kGuitarHall2->c6WL > d6W) ? d6W + 1 : 0)];
			double o6XL = kGuitarHall2->a6XL[kGuitarHall2->c6XL - ((kGuitarHall2->c6XL > d6X) ? d6X + 1 : 0)];

			kGuitarHall2->a6CR[kGuitarHall2->c6CR] = ((o6DR * 2.0) - (o6JR + o6PR + o6VR + o6ZCR + o6ZIR));
			kGuitarHall2->a6IR[kGuitarHall2->c6IR] = ((o6JR * 2.0) - (o6DR + o6PR + o6VR + o6ZCR + o6ZIR));
			kGuitarHall2->a6OR[kGuitarHall2->c6OR] = ((o6PR * 2.0) - (o6DR + o6JR + o6VR + o6ZCR + o6ZIR));
			kGuitarHall2->a6UR[kGuitarHall2->c6UR] = ((o6VR * 2.0) - (o6DR + o6JR + o6PR + o6ZCR + o6ZIR));
			kGuitarHall2->a6ZBR[kGuitarHall2->c6ZBR] = ((o6ZCR * 2.0) - (o6DR + o6JR + o6PR + o6VR + o6ZIR));
			kGuitarHall2->a6ZHR[kGuitarHall2->c6ZHR] = ((o6ZIR * 2.0) - (o6DR + o6JR + o6PR + o6VR + o6ZCR));

			kGuitarHall2->c6CR++;
			if (kGuitarHall2->c6CR < 0 || kGuitarHall2->c6CR > d6C) kGuitarHall2->c6CR = 0;
			kGuitarHall2->c6IR++;
			if (kGuitarHall2->c6IR < 0 || kGuitarHall2->c6IR > d6I) kGuitarHall2->c6IR = 0;
			kGuitarHall2->c6OR++;
			if (kGuitarHall2->c6OR < 0 || kGuitarHall2->c6OR > d6O) kGuitarHall2->c6OR = 0;
			kGuitarHall2->c6UR++;
			if (kGuitarHall2->c6UR < 0 || kGuitarHall2->c6UR > d6U) kGuitarHall2->c6UR = 0;
			kGuitarHall2->c6ZBR++;
			if (kGuitarHall2->c6ZBR < 0 || kGuitarHall2->c6ZBR > d6ZB) kGuitarHall2->c6ZBR = 0;
			kGuitarHall2->c6ZHR++;
			if (kGuitarHall2->c6ZHR < 0 || kGuitarHall2->c6ZHR > d6ZH) kGuitarHall2->c6ZHR = 0;

			double o6CR = kGuitarHall2->a6CR[kGuitarHall2->c6CR - ((kGuitarHall2->c6CR > d6C) ? d6C + 1 : 0)];
			double o6IR = kGuitarHall2->a6IR[kGuitarHall2->c6IR - ((kGuitarHall2->c6IR > d6I) ? d6I + 1 : 0)];
			double o6OR = kGuitarHall2->a6OR[kGuitarHall2->c6OR - ((kGuitarHall2->c6OR > d6O) ? d6O + 1 : 0)];
			double o6UR = kGuitarHall2->a6UR[kGuitarHall2->c6UR - ((kGuitarHall2->c6UR > d6U) ? d6U + 1 : 0)];
			double o6ZBR = kGuitarHall2->a6ZBR[kGuitarHall2->c6ZBR - ((kGuitarHall2->c6ZBR > d6ZB) ? d6ZB + 1 : 0)];
			double o6ZHR = kGuitarHall2->a6ZHR[kGuitarHall2->c6ZHR - ((kGuitarHall2->c6ZHR > d6ZH) ? d6ZH + 1 : 0)];

			//-------- four

			kGuitarHall2->a6YL[kGuitarHall2->c6YL] = ((o6SL * 2.0) - (o6TL + o6UL + o6VL + o6WL + o6XL));
			kGuitarHall2->a6ZAL[kGuitarHall2->c6ZAL] = ((o6TL * 2.0) - (o6SL + o6UL + o6VL + o6WL + o6XL));
			kGuitarHall2->a6ZBL[kGuitarHall2->c6ZBL] = ((o6UL * 2.0) - (o6SL + o6TL + o6VL + o6WL + o6XL));
			kGuitarHall2->a6ZCL[kGuitarHall2->c6ZCL] = ((o6VL * 2.0) - (o6SL + o6TL + o6UL + o6WL + o6XL));
			kGuitarHall2->a6ZDL[kGuitarHall2->c6ZDL] = ((o6WL * 2.0) - (o6SL + o6TL + o6UL + o6VL + o6XL));
			kGuitarHall2->a6ZEL[kGuitarHall2->c6ZEL] = ((o6XL * 2.0) - (o6SL + o6TL + o6UL + o6VL + o6WL));

			kGuitarHall2->c6YL++;
			if (kGuitarHall2->c6YL < 0 || kGuitarHall2->c6YL > d6Y) kGuitarHall2->c6YL = 0;
			kGuitarHall2->c6ZAL++;
			if (kGuitarHall2->c6ZAL < 0 || kGuitarHall2->c6ZAL > d6ZA) kGuitarHall2->c6ZAL = 0;
			kGuitarHall2->c6ZBL++;
			if (kGuitarHall2->c6ZBL < 0 || kGuitarHall2->c6ZBL > d6ZB) kGuitarHall2->c6ZBL = 0;
			kGuitarHall2->c6ZCL++;
			if (kGuitarHall2->c6ZCL < 0 || kGuitarHall2->c6ZCL > d6ZC) kGuitarHall2->c6ZCL = 0;
			kGuitarHall2->c6ZDL++;
			if (kGuitarHall2->c6ZDL < 0 || kGuitarHall2->c6ZDL > d6ZD) kGuitarHall2->c6ZDL = 0;
			kGuitarHall2->c6ZEL++;
			if (kGuitarHall2->c6ZEL < 0 || kGuitarHall2->c6ZEL > d6ZE) kGuitarHall2->c6ZEL = 0;

			double o6YL = kGuitarHall2->a6YL[kGuitarHall2->c6YL - ((kGuitarHall2->c6YL > d6Y) ? d6Y + 1 : 0)];
			double o6ZAL = kGuitarHall2->a6ZAL[kGuitarHall2->c6ZAL - ((kGuitarHall2->c6ZAL > d6ZA) ? d6ZA + 1 : 0)];
			double o6ZBL = kGuitarHall2->a6ZBL[kGuitarHall2->c6ZBL - ((kGuitarHall2->c6ZBL > d6ZB) ? d6ZB + 1 : 0)];
			double o6ZCL = kGuitarHall2->a6ZCL[kGuitarHall2->c6ZCL - ((kGuitarHall2->c6ZCL > d6ZC) ? d6ZC + 1 : 0)];
			double o6ZDL = kGuitarHall2->a6ZDL[kGuitarHall2->c6ZDL - ((kGuitarHall2->c6ZDL > d6ZD) ? d6ZD + 1 : 0)];
			double o6ZEL = kGuitarHall2->a6ZEL[kGuitarHall2->c6ZEL - ((kGuitarHall2->c6ZEL > d6ZE) ? d6ZE + 1 : 0)];

			kGuitarHall2->a6BR[kGuitarHall2->c6BR] = ((o6CR * 2.0) - (o6IR + o6OR + o6UR + o6ZBR + o6ZHR));
			kGuitarHall2->a6HR[kGuitarHall2->c6HR] = ((o6IR * 2.0) - (o6CR + o6OR + o6UR + o6ZBR + o6ZHR));
			kGuitarHall2->a6NR[kGuitarHall2->c6NR] = ((o6OR * 2.0) - (o6CR + o6IR + o6UR + o6ZBR + o6ZHR));
			kGuitarHall2->a6TR[kGuitarHall2->c6TR] = ((o6UR * 2.0) - (o6CR + o6IR + o6OR + o6ZBR + o6ZHR));
			kGuitarHall2->a6ZAR[kGuitarHall2->c6ZAR] = ((o6ZBR * 2.0) - (o6CR + o6IR + o6OR + o6UR + o6ZHR));
			kGuitarHall2->a6ZGR[kGuitarHall2->c6ZGR] = ((o6ZHR * 2.0) - (o6CR + o6IR + o6OR + o6UR + o6ZBR));

			kGuitarHall2->c6BR++;
			if (kGuitarHall2->c6BR < 0 || kGuitarHall2->c6BR > d6B) kGuitarHall2->c6BR = 0;
			kGuitarHall2->c6HR++;
			if (kGuitarHall2->c6HR < 0 || kGuitarHall2->c6HR > d6H) kGuitarHall2->c6HR = 0;
			kGuitarHall2->c6NR++;
			if (kGuitarHall2->c6NR < 0 || kGuitarHall2->c6NR > d6N) kGuitarHall2->c6NR = 0;
			kGuitarHall2->c6TR++;
			if (kGuitarHall2->c6TR < 0 || kGuitarHall2->c6TR > d6T) kGuitarHall2->c6TR = 0;
			kGuitarHall2->c6ZBR++;
			if (kGuitarHall2->c6ZBR < 0 || kGuitarHall2->c6ZBR > d6ZB) kGuitarHall2->c6ZBR = 0;
			kGuitarHall2->c6ZGR++;
			if (kGuitarHall2->c6ZGR < 0 || kGuitarHall2->c6ZGR > d6ZG) kGuitarHall2->c6ZGR = 0;

			double o6BR = kGuitarHall2->a6BR[kGuitarHall2->c6BR - ((kGuitarHall2->c6BR > d6B) ? d6B + 1 : 0)];
			double o6HR = kGuitarHall2->a6HR[kGuitarHall2->c6HR - ((kGuitarHall2->c6HR > d6H) ? d6H + 1 : 0)];
			double o6NR = kGuitarHall2->a6NR[kGuitarHall2->c6NR - ((kGuitarHall2->c6NR > d6N) ? d6N + 1 : 0)];
			double o6TR = kGuitarHall2->a6TR[kGuitarHall2->c6TR - ((kGuitarHall2->c6TR > d6T) ? d6T + 1 : 0)];
			double o6ZAR = kGuitarHall2->a6ZAR[kGuitarHall2->c6ZAR - ((kGuitarHall2->c6ZAR > d6ZA) ? d6ZA + 1 : 0)];
			double o6ZGR = kGuitarHall2->a6ZGR[kGuitarHall2->c6ZGR - ((kGuitarHall2->c6ZGR > d6ZG) ? d6ZG + 1 : 0)];

			//-------- five

			kGuitarHall2->a6ZFL[kGuitarHall2->c6ZFL] = ((o6YL * 2.0) - (o6ZAL + o6ZBL + o6ZCL + o6ZDL + o6ZEL));
			kGuitarHall2->a6ZGL[kGuitarHall2->c6ZGL] = ((o6ZAL * 2.0) - (o6YL + o6ZBL + o6ZCL + o6ZDL + o6ZEL));
			kGuitarHall2->a6ZHL[kGuitarHall2->c6ZHL] = ((o6ZBL * 2.0) - (o6YL + o6ZAL + o6ZCL + o6ZDL + o6ZEL));
			kGuitarHall2->a6ZIL[kGuitarHall2->c6ZIL] = ((o6ZCL * 2.0) - (o6YL + o6ZAL + o6ZBL + o6ZDL + o6ZEL));
			kGuitarHall2->a6ZJL[kGuitarHall2->c6ZJL] = ((o6ZDL * 2.0) - (o6YL + o6ZAL + o6ZBL + o6ZCL + o6ZEL));
			kGuitarHall2->a6ZKL[kGuitarHall2->c6ZKL] = ((o6ZEL * 2.0) - (o6YL + o6ZAL + o6ZBL + o6ZCL + o6ZDL));

			kGuitarHall2->c6ZFL++;
			if (kGuitarHall2->c6ZFL < 0 || kGuitarHall2->c6ZFL > d6ZF) kGuitarHall2->c6ZFL = 0;
			kGuitarHall2->c6ZGL++;
			if (kGuitarHall2->c6ZGL < 0 || kGuitarHall2->c6ZGL > d6ZG) kGuitarHall2->c6ZGL = 0;
			kGuitarHall2->c6ZHL++;
			if (kGuitarHall2->c6ZHL < 0 || kGuitarHall2->c6ZHL > d6ZH) kGuitarHall2->c6ZHL = 0;
			kGuitarHall2->c6ZIL++;
			if (kGuitarHall2->c6ZIL < 0 || kGuitarHall2->c6ZIL > d6ZI) kGuitarHall2->c6ZIL = 0;
			kGuitarHall2->c6ZJL++;
			if (kGuitarHall2->c6ZJL < 0 || kGuitarHall2->c6ZJL > d6ZJ) kGuitarHall2->c6ZJL = 0;
			kGuitarHall2->c6ZKL++;
			if (kGuitarHall2->c6ZKL < 0 || kGuitarHall2->c6ZKL > d6ZK) kGuitarHall2->c6ZKL = 0;

			double o6ZFL = kGuitarHall2->a6ZFL[kGuitarHall2->c6ZFL - ((kGuitarHall2->c6ZFL > d6ZF) ? d6ZF + 1 : 0)];
			double o6ZGL = kGuitarHall2->a6ZGL[kGuitarHall2->c6ZGL - ((kGuitarHall2->c6ZGL > d6ZG) ? d6ZG + 1 : 0)];
			double o6ZHL = kGuitarHall2->a6ZHL[kGuitarHall2->c6ZHL - ((kGuitarHall2->c6ZHL > d6ZH) ? d6ZH + 1 : 0)];
			double o6ZIL = kGuitarHall2->a6ZIL[kGuitarHall2->c6ZIL - ((kGuitarHall2->c6ZIL > d6ZI) ? d6ZI + 1 : 0)];
			double o6ZJL = kGuitarHall2->a6ZJL[kGuitarHall2->c6ZJL - ((kGuitarHall2->c6ZJL > d6ZJ) ? d6ZJ + 1 : 0)];
			double o6ZKL = kGuitarHall2->a6ZKL[kGuitarHall2->c6ZKL - ((kGuitarHall2->c6ZKL > d6ZK) ? d6ZK + 1 : 0)];

			kGuitarHall2->a6AR[kGuitarHall2->c6AR] = ((o6BR * 2.0) - (o6HR + o6NR + o6TR + o6ZAR + o6ZGR));
			kGuitarHall2->a6GR[kGuitarHall2->c6GR] = ((o6HR * 2.0) - (o6BR + o6NR + o6TR + o6ZAR + o6ZGR));
			kGuitarHall2->a6MR[kGuitarHall2->c6MR] = ((o6NR * 2.0) - (o6BR + o6HR + o6TR + o6ZAR + o6ZGR));
			kGuitarHall2->a6SR[kGuitarHall2->c6SR] = ((o6TR * 2.0) - (o6BR + o6HR + o6NR + o6ZAR + o6ZGR));
			kGuitarHall2->a6YR[kGuitarHall2->c6YR] = ((o6ZAR * 2.0) - (o6BR + o6HR + o6NR + o6TR + o6ZGR));
			kGuitarHall2->a6ZFR[kGuitarHall2->c6ZFR] = ((o6ZGR * 2.0) - (o6BR + o6HR + o6NR + o6TR + o6ZAR));

			kGuitarHall2->c6AR++;
			if (kGuitarHall2->c6AR < 0 || kGuitarHall2->c6AR > d6A) kGuitarHall2->c6AR = 0;
			kGuitarHall2->c6GR++;
			if (kGuitarHall2->c6GR < 0 || kGuitarHall2->c6GR > d6G) kGuitarHall2->c6GR = 0;
			kGuitarHall2->c6MR++;
			if (kGuitarHall2->c6MR < 0 || kGuitarHall2->c6MR > d6M) kGuitarHall2->c6MR = 0;
			kGuitarHall2->c6SR++;
			if (kGuitarHall2->c6SR < 0 || kGuitarHall2->c6SR > d6S) kGuitarHall2->c6SR = 0;
			kGuitarHall2->c6YR++;
			if (kGuitarHall2->c6YR < 0 || kGuitarHall2->c6YR > d6Y) kGuitarHall2->c6YR = 0;
			kGuitarHall2->c6ZFR++;
			if (kGuitarHall2->c6ZFR < 0 || kGuitarHall2->c6ZFR > d6ZF) kGuitarHall2->c6ZFR = 0;

			double o6AR = kGuitarHall2->a6AR[kGuitarHall2->c6AR - ((kGuitarHall2->c6AR > d6A) ? d6A + 1 : 0)];
			double o6GR = kGuitarHall2->a6GR[kGuitarHall2->c6GR - ((kGuitarHall2->c6GR > d6G) ? d6G + 1 : 0)];
			double o6MR = kGuitarHall2->a6MR[kGuitarHall2->c6MR - ((kGuitarHall2->c6MR > d6M) ? d6M + 1 : 0)];
			double o6SR = kGuitarHall2->a6SR[kGuitarHall2->c6SR - ((kGuitarHall2->c6SR > d6S) ? d6S + 1 : 0)];
			double o6YR = kGuitarHall2->a6YR[kGuitarHall2->c6YR - ((kGuitarHall2->c6YR > d6Y) ? d6Y + 1 : 0)];
			double o6ZFR = kGuitarHall2->a6ZFR[kGuitarHall2->c6ZFR - ((kGuitarHall2->c6ZFR > d6ZF) ? d6ZF + 1 : 0)];

			//-------- six

			kGuitarHall2->f6AL = ((o6AR * 2.0) - (o6GR + o6MR + o6SR + o6YR + o6ZFR));
			kGuitarHall2->f6BL = ((o6GR * 2.0) - (o6AR + o6MR + o6SR + o6YR + o6ZFR));
			kGuitarHall2->f6CL = ((o6MR * 2.0) - (o6AR + o6GR + o6SR + o6YR + o6ZFR));
			kGuitarHall2->f6DL = ((o6SR * 2.0) - (o6AR + o6GR + o6MR + o6YR + o6ZFR));
			kGuitarHall2->f6EL = ((o6YR * 2.0) - (o6AR + o6GR + o6MR + o6SR + o6ZFR));
			kGuitarHall2->f6FL = ((o6ZFR * 2.0) - (o6AR + o6GR + o6MR + o6SR + o6YR));

			kGuitarHall2->f6FR = ((o6ZFL * 2.0) - (o6ZGL + o6ZHL + o6ZIL + o6ZJL + o6ZKL));
			kGuitarHall2->f6LR = ((o6ZGL * 2.0) - (o6ZFL + o6ZHL + o6ZIL + o6ZJL + o6ZKL));
			kGuitarHall2->f6RR = ((o6ZHL * 2.0) - (o6ZFL + o6ZGL + o6ZIL + o6ZJL + o6ZKL));
			kGuitarHall2->f6XR = ((o6ZIL * 2.0) - (o6ZFL + o6ZGL + o6ZHL + o6ZJL + o6ZKL));
			kGuitarHall2->f6ZER = ((o6ZJL * 2.0) - (o6ZFL + o6ZGL + o6ZHL + o6ZIL + o6ZKL));
			kGuitarHall2->f6ZKR = ((o6ZKL * 2.0) - (o6ZFL + o6ZGL + o6ZHL + o6ZIL + o6ZJL));

			inputSampleL = (o6ZFL + o6ZGL + o6ZHL + o6ZIL + o6ZJL + o6ZKL) * 0.001953125;
			inputSampleR = (o6AR + o6GR + o6MR + o6SR + o6YR + o6ZFR) * 0.001953125;

			kGuitarHall2->f6AL = (kGuitarHall2->f6AL + kGuitarHall2->f6AL + kGuitarHall2->f6AL + fabs(kGuitarHall2->avg6L)) * 0.25;
			kGuitarHall2->avg6L = kGuitarHall2->f6AL;
			kGuitarHall2->f6FR = (kGuitarHall2->f6FR + kGuitarHall2->f6FR + kGuitarHall2->f6FR + fabs(kGuitarHall2->avg6R)) * 0.25;
			kGuitarHall2->avg6R = kGuitarHall2->f6FR;
			// manipulating deep reverb tail for realism

			inputSampleL += (earlyReflectionL * earlyLoudness);
			inputSampleR += (earlyReflectionR * earlyLoudness);

			kGuitarHall2->bez[bez_CL] = kGuitarHall2->bez[bez_BL];
			kGuitarHall2->bez[bez_BL] = kGuitarHall2->bez[bez_AL];
			kGuitarHall2->bez[bez_AL] = inputSampleL;
			kGuitarHall2->bez[bez_SampL] = 0.0;

			kGuitarHall2->bez[bez_CR] = kGuitarHall2->bez[bez_BR];
			kGuitarHall2->bez[bez_BR] = kGuitarHall2->bez[bez_AR];
			kGuitarHall2->bez[bez_AR] = inputSampleR;
			kGuitarHall2->bez[bez_SampR] = 0.0;
		}
		double X = kGuitarHall2->bez[bez_cycle] * bezTrim;
		double CBL = (kGuitarHall2->bez[bez_CL] * (1.0 - X)) + (kGuitarHall2->bez[bez_BL] * X);
		double CBR = (kGuitarHall2->bez[bez_CR] * (1.0 - X)) + (kGuitarHall2->bez[bez_BR] * X);
		double BAL = (kGuitarHall2->bez[bez_BL] * (1.0 - X)) + (kGuitarHall2->bez[bez_AL] * X);
		double BAR = (kGuitarHall2->bez[bez_BR] * (1.0 - X)) + (kGuitarHall2->bez[bez_AR] * X);
		double CBAL = (kGuitarHall2->bez[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * -0.0625;
		double CBAR = (kGuitarHall2->bez[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * -0.0625;
		inputSampleL = CBAL + kGuitarHall2->bez[bez_AvgOutSampL];
		kGuitarHall2->bez[bez_AvgOutSampL] = CBAL;
		inputSampleR = CBAR + kGuitarHall2->bez[bez_AvgOutSampR];
		kGuitarHall2->bez[bez_AvgOutSampR] = CBAR;

		inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
		inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		kGuitarHall2->fpdL ^= kGuitarHall2->fpdL << 13;
		kGuitarHall2->fpdL ^= kGuitarHall2->fpdL >> 17;
		kGuitarHall2->fpdL ^= kGuitarHall2->fpdL << 5;
		inputSampleL += (((double) kGuitarHall2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		kGuitarHall2->fpdR ^= kGuitarHall2->fpdR << 13;
		kGuitarHall2->fpdR ^= kGuitarHall2->fpdR >> 17;
		kGuitarHall2->fpdR ^= kGuitarHall2->fpdR << 5;
		inputSampleR += (((double) kGuitarHall2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	KGUITARHALL2_URI,
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
