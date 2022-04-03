#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define STARCHILD_URI "https://hannesbraun.net/ns/lv2/airwindows/starchild"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	SUSTAIN = 4,
	GRAIN = 5,
	DRYWET = 6
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* sustain;
	const float* grain;
	const float* drywet;

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff

	double d[45102];
	int dCount;

	int p[171];
	int t[171];
	double outL[171];
	double outR[171];
	int pitchCounter;
	int increment;
	int dutyCycle;

	double wearL[11];
	double wearR[11];
	double factor[11];
	double wearLPrev;
	double wearRPrev;
} StarChild;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	StarChild* starchild = (StarChild*) calloc(1, sizeof(StarChild));
	return (LV2_Handle) starchild;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	StarChild* starchild = (StarChild*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			starchild->input[0] = (const float*) data;
			break;
		case INPUT_R:
			starchild->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			starchild->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			starchild->output[1] = (float*) data;
			break;
		case SUSTAIN:
			starchild->sustain = (const float*) data;
			break;
		case GRAIN:
			starchild->grain = (const float*) data;
			break;
		case DRYWET:
			starchild->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	StarChild* starchild = (StarChild*) instance;
	int count;

	for (count = 0; count < 44101; count++) {
		starchild->d[count] = 0.0;
	}
	starchild->dCount = 0;

	for (count = 0; count < 11; count++) {
		starchild->wearL[count] = 0.0;
		starchild->wearR[count] = 0.0;
		starchild->factor[count] = 0.0;
	}

	starchild->wearLPrev = 0.0;
	starchild->wearRPrev = 0.0;
	starchild->p[0] = 1;
	starchild->p[1] = 11;
	starchild->p[2] = 13;
	starchild->p[3] = 17;
	starchild->p[4] = 19;
	starchild->p[5] = 23;
	starchild->p[6] = 29;
	starchild->p[7] = 31;
	starchild->p[8] = 37;
	starchild->p[9] = 41;
	starchild->p[10] = 43;
	starchild->p[11] = 47;
	starchild->p[12] = 53;
	starchild->p[13] = 59;
	starchild->p[14] = 61;
	starchild->p[15] = 67;
	starchild->p[16] = 71;
	starchild->p[17] = 73;
	starchild->p[18] = 79;
	starchild->p[19] = 83;
	starchild->p[20] = 89;
	starchild->p[21] = 97;
	starchild->p[22] = 101;
	starchild->p[23] = 103;
	starchild->p[24] = 107;
	starchild->p[25] = 109;
	starchild->p[26] = 113;
	starchild->p[27] = 127;
	starchild->p[28] = 131;
	starchild->p[29] = 137;
	starchild->p[30] = 139;
	starchild->p[31] = 149;
	starchild->p[32] = 151;
	starchild->p[33] = 157;
	starchild->p[34] = 163;
	starchild->p[35] = 167;
	starchild->p[36] = 173;
	starchild->p[37] = 179;
	starchild->p[38] = 181;
	starchild->p[39] = 191;
	starchild->p[40] = 193;
	starchild->p[41] = 197;
	starchild->p[42] = 199;
	starchild->p[43] = 211;
	starchild->p[44] = 223;
	starchild->p[45] = 227;
	starchild->p[46] = 229;
	starchild->p[47] = 233;
	starchild->p[48] = 239;
	starchild->p[49] = 241;
	starchild->p[50] = 251;
	starchild->p[51] = 257;
	starchild->p[52] = 263;
	starchild->p[53] = 269;
	starchild->p[54] = 271;
	starchild->p[55] = 277;
	starchild->p[56] = 281;
	starchild->p[57] = 283;
	starchild->p[58] = 293;
	starchild->p[59] = 307;
	starchild->p[60] = 311;
	starchild->p[61] = 313;
	starchild->p[62] = 317;
	starchild->p[63] = 331;
	starchild->p[64] = 337;
	starchild->p[65] = 347;
	starchild->p[66] = 349;
	starchild->p[67] = 353;
	starchild->p[68] = 359;
	starchild->p[69] = 367;
	starchild->p[70] = 373;
	starchild->p[71] = 379;
	starchild->p[72] = 383;
	starchild->p[73] = 389;
	starchild->p[74] = 397;
	starchild->p[75] = 401;
	starchild->p[76] = 409;
	starchild->p[77] = 419;
	starchild->p[78] = 421;
	starchild->p[79] = 431;
	starchild->p[80] = 433;
	starchild->p[81] = 439;
	starchild->p[82] = 443;
	starchild->p[83] = 449;
	starchild->p[84] = 457;
	starchild->p[85] = 461;
	starchild->p[86] = 463;
	starchild->p[87] = 467;
	starchild->p[88] = 479;
	starchild->p[89] = 487;
	starchild->p[90] = 491;
	starchild->p[91] = 499;
	starchild->p[92] = 503;
	starchild->p[93] = 509;
	starchild->p[94] = 521;
	starchild->p[95] = 523;
	starchild->p[96] = 541;
	starchild->p[97] = 547;
	starchild->p[98] = 557;
	starchild->p[99] = 563;
	starchild->p[100] = 569;
	starchild->p[101] = 571;
	starchild->p[102] = 577;
	starchild->p[103] = 587;
	starchild->p[104] = 593;
	starchild->p[105] = 599;
	starchild->p[106] = 601;
	starchild->p[107] = 607;
	starchild->p[108] = 613;
	starchild->p[109] = 617;
	starchild->p[110] = 619;
	starchild->p[111] = 631;
	starchild->p[112] = 641;
	starchild->p[113] = 643;
	starchild->p[114] = 647;
	starchild->p[115] = 653;
	starchild->p[116] = 659;
	starchild->p[117] = 661;
	starchild->p[118] = 673;
	starchild->p[119] = 677;
	starchild->p[120] = 683;
	starchild->p[121] = 691;
	starchild->p[122] = 701;
	starchild->p[123] = 709;
	starchild->p[124] = 719;
	starchild->p[125] = 727;
	starchild->p[126] = 733;
	starchild->p[127] = 739;
	starchild->p[128] = 743;
	starchild->p[129] = 751;
	starchild->p[130] = 757;
	starchild->p[131] = 761;
	starchild->p[132] = 769;
	starchild->p[133] = 773;
	starchild->p[134] = 787;
	starchild->p[135] = 797;
	starchild->p[136] = 809;
	starchild->p[137] = 811;
	starchild->p[138] = 821;
	starchild->p[139] = 823;
	starchild->p[140] = 827;
	starchild->p[141] = 829;
	starchild->p[142] = 839;
	starchild->p[143] = 853;
	starchild->p[144] = 857;
	starchild->p[145] = 859;
	starchild->p[146] = 863;
	starchild->p[147] = 877;
	starchild->p[148] = 881;
	starchild->p[149] = 883;
	starchild->p[150] = 887;
	starchild->p[151] = 907;
	starchild->p[152] = 911;
	starchild->p[153] = 919;
	starchild->p[154] = 929;
	starchild->p[155] = 937;
	starchild->p[156] = 941;
	starchild->p[157] = 947;
	starchild->p[158] = 953;
	starchild->p[159] = 967;
	starchild->p[160] = 971;
	starchild->p[161] = 977;
	starchild->p[162] = 983;
	starchild->p[163] = 991;
	starchild->p[164] = 997;
	starchild->p[165] = 998;
	starchild->p[166] = 999;

	int assign;
	for (count = 0; count < 165; count++) {
		starchild->t[count] = starchild->p[count];
		//these get assigned again but we'll give them real values in case of trouble. They are 32 bit unsigned ints
		assign = starchild->p[count] % 10;
		//give us the 1, 3, 7 or 9 on the end

		switch (assign) {
			case 1:
				starchild->outL[count] = 0.0;
				starchild->outR[count] = starchild->p[count];
				break;
			case 3:
				starchild->outL[count] = starchild->p[count] * 0.25;
				starchild->outR[count] = starchild->p[count] * 0.75;
				break;
			case 7:
				starchild->outL[count] = starchild->p[count] * 0.75;
				starchild->outR[count] = starchild->p[count] * 0.25;
				break;
			case 9:
				starchild->outL[count] = starchild->p[count];
				starchild->outR[count] = 0.0;
				break;
		}
		//this gives us a set of stereo offsets that are always the same. When building our delay outputs,
		//we multiply our -1 to 1 float values by this and add the result to a simple signed int.
		//The result gives us a coarser and coarser output the fewer taps we have,
		//and we divide the result by count*count to drop the volume down again.
	}
	starchild->pitchCounter = 2;
	starchild->increment = 1;
	starchild->dutyCycle = 1;

	starchild->fpdL = 1.0;
	while (starchild->fpdL < 16386) starchild->fpdL = rand() * UINT32_MAX;
	starchild->fpdR = 1.0;
	while (starchild->fpdR < 16386) starchild->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	StarChild* starchild = (StarChild*) instance;

	const float* in1 = starchild->input[0];
	const float* in2 = starchild->input[1];
	float* out1 = starchild->output[0];
	float* out2 = starchild->output[1];

	double drySampleL;
	double drySampleR;
	double inputSampleL;
	double inputSampleR;

	int bufferL = 0;
	int bufferR = 0;
	//these are to build up the reverb tank outs

	int rangeDirect = (pow(*starchild->grain, 2) * 156.0) + 7.0;
	//maximum safe delay is 259 * the prime tap, not including room for the pitch shift offset

	float scaleDirect = (pow(*starchild->sustain, 2) * (3280.0 / rangeDirect)) + 2.0;
	//let's try making it always be the max delay: smaller range forces scale to be longer

	float outputPad = 4 * rangeDirect * sqrt(rangeDirect);

	float overallscale = ((1.0 - *starchild->grain) * 9.0) + 1.0;
	//apply the singlestage groove wear strongest when bits are heavily crushed
	float gain = overallscale;
	if (gain > 1.0) {
		starchild->factor[0] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[0] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[1] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[1] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[2] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[2] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[3] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[3] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[4] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[4] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[5] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[5] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[6] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[6] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[7] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[7] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[8] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[8] = gain;
		gain = 0.0;
	}
	if (gain > 1.0) {
		starchild->factor[9] = 1.0;
		gain -= 1.0;
	} else {
		starchild->factor[9] = gain;
		gain = 0.0;
	}
	//there, now we have a neat little moving average with remainders

	if (overallscale < 1.0) overallscale = 1.0;
	starchild->factor[0] /= overallscale;
	starchild->factor[1] /= overallscale;
	starchild->factor[2] /= overallscale;
	starchild->factor[3] /= overallscale;
	starchild->factor[4] /= overallscale;
	starchild->factor[5] /= overallscale;
	starchild->factor[6] /= overallscale;
	starchild->factor[7] /= overallscale;
	starchild->factor[8] /= overallscale;
	starchild->factor[9] /= overallscale;
	//and now it's neatly scaled, too
	float accumulatorSample;
	float correction;
	float wetness = *starchild->drywet;
	float dryness = 1.0 - wetness;  //reverb setup

	int count;
	for (count = 1; count < 165; count++) {
		starchild->t[count] = starchild->p[count] * scaleDirect;
		//this is the scaled tap for direct out, in number of samples delay
	}

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = starchild->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = starchild->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		//assign working variables like the dry

		if (starchild->dCount < 0 || starchild->dCount > 22050) {
			starchild->dCount = 22050;
		}
		starchild->d[starchild->dCount + 22050] = starchild->d[starchild->dCount] = inputSampleL + inputSampleR;
		starchild->dCount--;
		//feed the delay line with summed channels. The stuff we're reading back
		//will always be plus dCount, because we're counting back to 0.

		//now we're going to start pitch shifting.
		starchild->dutyCycle += 1;
		if (starchild->dutyCycle > scaleDirect) {
			starchild->dutyCycle = 1;
			//this whole routine doesn't run every sample, it's making a wacky hypervibrato
			starchild->t[starchild->pitchCounter] += starchild->increment;
			starchild->pitchCounter += 1;
			//pitchCounter always goes up, t[] goes up and down
			//possibly do that not every sample? Let's see what we get
			if (starchild->pitchCounter > rangeDirect) {
				if (starchild->increment == 1) {
					starchild->pitchCounter = 1;
					if (starchild->t[1] > ((11 * scaleDirect) + 1000)) starchild->increment = -1;
					//let's try hardcoding a big 1000 sample buffer
				} else {
					//increment is -1 so we have been counting down!
					starchild->pitchCounter = 1;

					if (starchild->t[1] < (11 * scaleDirect)) {
						//we've scaled everything back so we're going up again
						starchild->increment = 1;
						//and we're gonna reset the lot in case of screw-ups (control manipulations)
						for (count = 1; count < 165; count++) {
							starchild->t[count] = starchild->p[count] * scaleDirect;
						}
						//which means we're back to normal and counting up again.
					}
				}
				//wrap around to begin again, and if our first tap is greater than
				//its base value plus scaleDirect, start going down.
			}
		}
		//always wrap around to the first tap

		//Now we do a select case where we jump into the middle of some repetitive math, unrolled.
		bufferL = 0;
		bufferR = 0;
		//clear before making our delay sound
		switch (rangeDirect) {
			case 164:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[164]] * starchild->outL[164]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[164]] * starchild->outR[164]);
			case 163:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[163]] * starchild->outL[163]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[163]] * starchild->outR[163]);
			case 162:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[162]] * starchild->outL[162]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[162]] * starchild->outR[162]);
			case 161:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[161]] * starchild->outL[161]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[161]] * starchild->outR[161]);
			case 160:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[160]] * starchild->outL[160]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[160]] * starchild->outR[160]);
			case 159:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[159]] * starchild->outL[159]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[159]] * starchild->outR[159]);
			case 158:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[158]] * starchild->outL[158]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[158]] * starchild->outR[158]);
			case 157:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[157]] * starchild->outL[157]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[157]] * starchild->outR[157]);
			case 156:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[156]] * starchild->outL[156]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[156]] * starchild->outR[156]);
			case 155:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[155]] * starchild->outL[155]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[155]] * starchild->outR[155]);
			case 154:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[154]] * starchild->outL[154]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[154]] * starchild->outR[154]);
			case 153:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[153]] * starchild->outL[153]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[153]] * starchild->outR[153]);
			case 152:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[152]] * starchild->outL[152]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[152]] * starchild->outR[152]);
			case 151:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[151]] * starchild->outL[151]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[151]] * starchild->outR[151]);
			case 150:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[150]] * starchild->outL[150]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[150]] * starchild->outR[150]);
			case 149:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[149]] * starchild->outL[149]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[149]] * starchild->outR[149]);
			case 148:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[148]] * starchild->outL[148]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[148]] * starchild->outR[148]);
			case 147:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[147]] * starchild->outL[147]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[147]] * starchild->outR[147]);
			case 146:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[146]] * starchild->outL[146]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[146]] * starchild->outR[146]);
			case 145:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[145]] * starchild->outL[145]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[145]] * starchild->outR[145]);
			case 144:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[144]] * starchild->outL[144]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[144]] * starchild->outR[144]);
			case 143:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[143]] * starchild->outL[143]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[143]] * starchild->outR[143]);
			case 142:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[142]] * starchild->outL[142]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[142]] * starchild->outR[142]);
			case 141:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[141]] * starchild->outL[141]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[141]] * starchild->outR[141]);
			case 140:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[140]] * starchild->outL[140]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[140]] * starchild->outR[140]);
			case 139:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[139]] * starchild->outL[139]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[139]] * starchild->outR[139]);
			case 138:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[138]] * starchild->outL[138]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[138]] * starchild->outR[138]);
			case 137:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[137]] * starchild->outL[137]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[137]] * starchild->outR[137]);
			case 136:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[136]] * starchild->outL[136]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[136]] * starchild->outR[136]);
			case 135:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[135]] * starchild->outL[135]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[135]] * starchild->outR[135]);
			case 134:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[134]] * starchild->outL[134]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[134]] * starchild->outR[134]);
			case 133:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[133]] * starchild->outL[133]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[133]] * starchild->outR[133]);
			case 132:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[132]] * starchild->outL[132]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[132]] * starchild->outR[132]);
			case 131:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[131]] * starchild->outL[131]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[131]] * starchild->outR[131]);
			case 130:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[130]] * starchild->outL[130]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[130]] * starchild->outR[130]);
			case 129:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[129]] * starchild->outL[129]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[129]] * starchild->outR[129]);
			case 128:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[128]] * starchild->outL[128]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[128]] * starchild->outR[128]);
			case 127:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[127]] * starchild->outL[127]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[127]] * starchild->outR[127]);
			case 126:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[126]] * starchild->outL[126]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[126]] * starchild->outR[126]);
			case 125:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[125]] * starchild->outL[125]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[125]] * starchild->outR[125]);
			case 124:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[124]] * starchild->outL[124]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[124]] * starchild->outR[124]);
			case 123:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[123]] * starchild->outL[123]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[123]] * starchild->outR[123]);
			case 122:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[122]] * starchild->outL[122]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[122]] * starchild->outR[122]);
			case 121:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[121]] * starchild->outL[121]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[121]] * starchild->outR[121]);
			case 120:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[120]] * starchild->outL[120]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[120]] * starchild->outR[120]);
			case 119:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[119]] * starchild->outL[119]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[119]] * starchild->outR[119]);
			case 118:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[118]] * starchild->outL[118]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[118]] * starchild->outR[118]);
			case 117:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[117]] * starchild->outL[117]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[117]] * starchild->outR[117]);
			case 116:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[116]] * starchild->outL[116]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[116]] * starchild->outR[116]);
			case 115:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[115]] * starchild->outL[115]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[115]] * starchild->outR[115]);
			case 114:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[114]] * starchild->outL[114]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[114]] * starchild->outR[114]);
			case 113:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[113]] * starchild->outL[113]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[113]] * starchild->outR[113]);
			case 112:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[112]] * starchild->outL[112]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[112]] * starchild->outR[112]);
			case 111:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[111]] * starchild->outL[111]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[111]] * starchild->outR[111]);
			case 110:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[110]] * starchild->outL[110]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[110]] * starchild->outR[110]);
			case 109:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[109]] * starchild->outL[109]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[109]] * starchild->outR[109]);
			case 108:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[108]] * starchild->outL[108]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[108]] * starchild->outR[108]);
			case 107:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[107]] * starchild->outL[107]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[107]] * starchild->outR[107]);
			case 106:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[106]] * starchild->outL[106]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[106]] * starchild->outR[106]);
			case 105:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[105]] * starchild->outL[105]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[105]] * starchild->outR[105]);
			case 104:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[104]] * starchild->outL[104]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[104]] * starchild->outR[104]);
			case 103:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[103]] * starchild->outL[103]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[103]] * starchild->outR[103]);
			case 102:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[102]] * starchild->outL[102]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[102]] * starchild->outR[102]);
			case 101:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[101]] * starchild->outL[101]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[101]] * starchild->outR[101]);
			case 100:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[100]] * starchild->outL[100]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[100]] * starchild->outR[100]);
			case  99:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 99]] * starchild->outL[ 99]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 99]] * starchild->outR[ 99]);
			case  98:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 98]] * starchild->outL[ 98]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 98]] * starchild->outR[ 98]);
			case  97:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 97]] * starchild->outL[ 97]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 97]] * starchild->outR[ 97]);
			case  96:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 96]] * starchild->outL[ 96]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 96]] * starchild->outR[ 96]);
			case  95:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 95]] * starchild->outL[ 95]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 95]] * starchild->outR[ 95]);
			case  94:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 94]] * starchild->outL[ 94]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 94]] * starchild->outR[ 94]);
			case  93:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 93]] * starchild->outL[ 93]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 93]] * starchild->outR[ 93]);
			case  92:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 92]] * starchild->outL[ 92]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 92]] * starchild->outR[ 92]);
			case  91:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 91]] * starchild->outL[ 91]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 91]] * starchild->outR[ 91]);
			case  90:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 90]] * starchild->outL[ 90]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 90]] * starchild->outR[ 90]);
			case  89:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 89]] * starchild->outL[ 89]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 89]] * starchild->outR[ 89]);
			case  88:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 88]] * starchild->outL[ 88]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 88]] * starchild->outR[ 88]);
			case  87:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 87]] * starchild->outL[ 87]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 87]] * starchild->outR[ 87]);
			case  86:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 86]] * starchild->outL[ 86]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 86]] * starchild->outR[ 86]);
			case  85:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 85]] * starchild->outL[ 85]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 85]] * starchild->outR[ 85]);
			case  84:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 84]] * starchild->outL[ 84]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 84]] * starchild->outR[ 84]);
			case  83:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 83]] * starchild->outL[ 83]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 83]] * starchild->outR[ 83]);
			case  82:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 82]] * starchild->outL[ 82]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 82]] * starchild->outR[ 82]);
			case  81:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 81]] * starchild->outL[ 81]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 81]] * starchild->outR[ 81]);
			case  80:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 80]] * starchild->outL[ 80]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 80]] * starchild->outR[ 80]);
			case  79:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 79]] * starchild->outL[ 79]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 79]] * starchild->outR[ 79]);
			case  78:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 78]] * starchild->outL[ 78]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 78]] * starchild->outR[ 78]);
			case  77:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 77]] * starchild->outL[ 77]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 77]] * starchild->outR[ 77]);
			case  76:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 76]] * starchild->outL[ 76]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 76]] * starchild->outR[ 76]);
			case  75:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 75]] * starchild->outL[ 75]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 75]] * starchild->outR[ 75]);
			case  74:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 74]] * starchild->outL[ 74]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 74]] * starchild->outR[ 74]);
			case  73:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 73]] * starchild->outL[ 73]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 73]] * starchild->outR[ 73]);
			case  72:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 72]] * starchild->outL[ 72]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 72]] * starchild->outR[ 72]);
			case  71:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 71]] * starchild->outL[ 71]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 71]] * starchild->outR[ 71]);
			case  70:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 70]] * starchild->outL[ 70]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 70]] * starchild->outR[ 70]);
			case  69:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 69]] * starchild->outL[ 69]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 69]] * starchild->outR[ 69]);
			case  68:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 68]] * starchild->outL[ 68]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 68]] * starchild->outR[ 68]);
			case  67:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 67]] * starchild->outL[ 67]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 67]] * starchild->outR[ 67]);
			case  66:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 66]] * starchild->outL[ 66]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 66]] * starchild->outR[ 66]);
			case  65:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 65]] * starchild->outL[ 65]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 65]] * starchild->outR[ 65]);
			case  64:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 64]] * starchild->outL[ 64]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 64]] * starchild->outR[ 64]);
			case  63:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 63]] * starchild->outL[ 63]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 63]] * starchild->outR[ 63]);
			case  62:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 62]] * starchild->outL[ 62]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 62]] * starchild->outR[ 62]);
			case  61:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 61]] * starchild->outL[ 61]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 61]] * starchild->outR[ 61]);
			case  60:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 60]] * starchild->outL[ 60]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 60]] * starchild->outR[ 60]);
			case  59:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 59]] * starchild->outL[ 59]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 59]] * starchild->outR[ 59]);
			case  58:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 58]] * starchild->outL[ 58]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 58]] * starchild->outR[ 58]);
			case  57:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 57]] * starchild->outL[ 57]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 57]] * starchild->outR[ 57]);
			case  56:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 56]] * starchild->outL[ 56]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 56]] * starchild->outR[ 56]);
			case  55:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 55]] * starchild->outL[ 55]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 55]] * starchild->outR[ 55]);
			case  54:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 54]] * starchild->outL[ 54]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 54]] * starchild->outR[ 54]);
			case  53:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 53]] * starchild->outL[ 53]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 53]] * starchild->outR[ 53]);
			case  52:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 52]] * starchild->outL[ 52]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 52]] * starchild->outR[ 52]);
			case  51:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 51]] * starchild->outL[ 51]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 51]] * starchild->outR[ 51]);
			case  50:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 50]] * starchild->outL[ 50]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 50]] * starchild->outR[ 50]);
			case  49:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 49]] * starchild->outL[ 49]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 49]] * starchild->outR[ 49]);
			case  48:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 48]] * starchild->outL[ 48]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 48]] * starchild->outR[ 48]);
			case  47:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 47]] * starchild->outL[ 47]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 47]] * starchild->outR[ 47]);
			case  46:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 46]] * starchild->outL[ 46]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 46]] * starchild->outR[ 46]);
			case  45:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 45]] * starchild->outL[ 45]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 45]] * starchild->outR[ 45]);
			case  44:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 44]] * starchild->outL[ 44]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 44]] * starchild->outR[ 44]);
			case  43:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 43]] * starchild->outL[ 43]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 43]] * starchild->outR[ 43]);
			case  42:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 42]] * starchild->outL[ 42]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 42]] * starchild->outR[ 42]);
			case  41:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 41]] * starchild->outL[ 41]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 41]] * starchild->outR[ 41]);
			case  40:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 40]] * starchild->outL[ 40]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 40]] * starchild->outR[ 40]);
			case  39:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 39]] * starchild->outL[ 39]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 39]] * starchild->outR[ 39]);
			case  38:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 38]] * starchild->outL[ 38]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 38]] * starchild->outR[ 38]);
			case  37:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 37]] * starchild->outL[ 37]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 37]] * starchild->outR[ 37]);
			case  36:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 36]] * starchild->outL[ 36]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 36]] * starchild->outR[ 36]);
			case  35:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 35]] * starchild->outL[ 35]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 35]] * starchild->outR[ 35]);
			case  34:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 34]] * starchild->outL[ 34]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 34]] * starchild->outR[ 34]);
			case  33:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 33]] * starchild->outL[ 33]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 33]] * starchild->outR[ 33]);
			case  32:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 32]] * starchild->outL[ 32]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 32]] * starchild->outR[ 32]);
			case  31:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 31]] * starchild->outL[ 31]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 31]] * starchild->outR[ 31]);
			case  30:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 30]] * starchild->outL[ 30]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 30]] * starchild->outR[ 30]);
			case  29:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 29]] * starchild->outL[ 29]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 29]] * starchild->outR[ 29]);
			case  28:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 28]] * starchild->outL[ 28]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 28]] * starchild->outR[ 28]);
			case  27:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 27]] * starchild->outL[ 27]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 27]] * starchild->outR[ 27]);
			case  26:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 26]] * starchild->outL[ 26]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 26]] * starchild->outR[ 26]);
			case  25:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 25]] * starchild->outL[ 25]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 25]] * starchild->outR[ 25]);
			case  24:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 24]] * starchild->outL[ 24]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 24]] * starchild->outR[ 24]);
			case  23:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 23]] * starchild->outL[ 23]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 23]] * starchild->outR[ 23]);
			case  22:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 22]] * starchild->outL[ 22]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 22]] * starchild->outR[ 22]);
			case  21:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 21]] * starchild->outL[ 21]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 21]] * starchild->outR[ 21]);
			case  20:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 20]] * starchild->outL[ 20]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 20]] * starchild->outR[ 20]);
			case  19:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 19]] * starchild->outL[ 19]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 19]] * starchild->outR[ 19]);
			case  18:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 18]] * starchild->outL[ 18]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 18]] * starchild->outR[ 18]);
			case  17:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 17]] * starchild->outL[ 17]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 17]] * starchild->outR[ 17]);
			case  16:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 16]] * starchild->outL[ 16]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 16]] * starchild->outR[ 16]);
			case  15:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 15]] * starchild->outL[ 15]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 15]] * starchild->outR[ 15]);
			case  14:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 14]] * starchild->outL[ 14]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 14]] * starchild->outR[ 14]);
			case  13:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 13]] * starchild->outL[ 13]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 13]] * starchild->outR[ 13]);
			case  12:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 12]] * starchild->outL[ 12]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 12]] * starchild->outR[ 12]);
			case  11:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 11]] * starchild->outL[ 11]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 11]] * starchild->outR[ 11]);
			case  10:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[ 10]] * starchild->outL[ 10]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[ 10]] * starchild->outR[ 10]);
			case   9:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  9]] * starchild->outL[  9]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  9]] * starchild->outR[  9]);
			case   8:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  8]] * starchild->outL[  8]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  8]] * starchild->outR[  8]);
			case   7:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  7]] * starchild->outL[  7]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  7]] * starchild->outR[  7]);
			case   6:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  6]] * starchild->outL[  6]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  6]] * starchild->outR[  6]);
			case   5:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  5]] * starchild->outL[  5]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  5]] * starchild->outR[  5]);
			case   4:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  4]] * starchild->outL[  4]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  4]] * starchild->outR[  4]);
			case   3:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  3]] * starchild->outL[  3]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  3]] * starchild->outR[  3]);
			case   2:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  2]] * starchild->outL[  2]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  2]] * starchild->outR[  2]);
			case   1:
				bufferL += (int)(starchild->d[starchild->dCount + starchild->t[  1]] * starchild->outL[  1]);
				bufferR += (int)(starchild->d[starchild->dCount + starchild->t[  1]] * starchild->outR[  1]);
		}
		//test to see that delay is working at all: it will be a big stack of case with no break

		inputSampleL = bufferL;
		inputSampleR = bufferR;
		//scale back the reverb buffers based on how big of a range we used


		starchild->wearR[9] = starchild->wearR[8];
		starchild->wearR[8] = starchild->wearR[7];
		starchild->wearR[7] = starchild->wearR[6];
		starchild->wearR[6] = starchild->wearR[5];
		starchild->wearR[5] = starchild->wearR[4];
		starchild->wearR[4] = starchild->wearR[3];
		starchild->wearR[3] = starchild->wearR[2];
		starchild->wearR[2] = starchild->wearR[1];
		starchild->wearR[1] = starchild->wearR[0];
		starchild->wearR[0] = accumulatorSample = (inputSampleR - starchild->wearRPrev);

		accumulatorSample *= starchild->factor[0];
		accumulatorSample += (starchild->wearR[1] * starchild->factor[1]);
		accumulatorSample += (starchild->wearR[2] * starchild->factor[2]);
		accumulatorSample += (starchild->wearR[3] * starchild->factor[3]);
		accumulatorSample += (starchild->wearR[4] * starchild->factor[4]);
		accumulatorSample += (starchild->wearR[5] * starchild->factor[5]);
		accumulatorSample += (starchild->wearR[6] * starchild->factor[6]);
		accumulatorSample += (starchild->wearR[7] * starchild->factor[7]);
		accumulatorSample += (starchild->wearR[8] * starchild->factor[8]);
		accumulatorSample += (starchild->wearR[9] * starchild->factor[9]);
		//we are doing our repetitive calculations on a separate value
		correction = (inputSampleR - starchild->wearRPrev) + accumulatorSample;
		starchild->wearRPrev = inputSampleR;
		inputSampleR += correction;

		starchild->wearL[9] = starchild->wearL[8];
		starchild->wearL[8] = starchild->wearL[7];
		starchild->wearL[7] = starchild->wearL[6];
		starchild->wearL[6] = starchild->wearL[5];
		starchild->wearL[5] = starchild->wearL[4];
		starchild->wearL[4] = starchild->wearL[3];
		starchild->wearL[3] = starchild->wearL[2];
		starchild->wearL[2] = starchild->wearL[1];
		starchild->wearL[1] = starchild->wearL[0];
		starchild->wearL[0] = accumulatorSample = (inputSampleL - starchild->wearLPrev);

		accumulatorSample *= starchild->factor[0];
		accumulatorSample += (starchild->wearL[1] * starchild->factor[1]);
		accumulatorSample += (starchild->wearL[2] * starchild->factor[2]);
		accumulatorSample += (starchild->wearL[3] * starchild->factor[3]);
		accumulatorSample += (starchild->wearL[4] * starchild->factor[4]);
		accumulatorSample += (starchild->wearL[5] * starchild->factor[5]);
		accumulatorSample += (starchild->wearL[6] * starchild->factor[6]);
		accumulatorSample += (starchild->wearL[7] * starchild->factor[7]);
		accumulatorSample += (starchild->wearL[8] * starchild->factor[8]);
		accumulatorSample += (starchild->wearL[9] * starchild->factor[9]);
		//we are doing our repetitive calculations on a separate value
		correction = (inputSampleL - starchild->wearLPrev) + accumulatorSample;
		starchild->wearLPrev = inputSampleL;
		inputSampleL += correction;
		//completed Groove Wear section

		inputSampleL /= outputPad;
		inputSampleR /= outputPad;

		//back to previous plugin
		drySampleL *= dryness;
		drySampleR *= dryness;

		inputSampleL *= wetness;
		inputSampleR *= wetness;

		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		//here we combine the tanks with the dry signal

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		starchild->fpdL ^= starchild->fpdL << 13;
		starchild->fpdL ^= starchild->fpdL >> 17;
		starchild->fpdL ^= starchild->fpdL << 5;
		inputSampleL += (((double)starchild->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		starchild->fpdR ^= starchild->fpdR << 13;
		starchild->fpdR ^= starchild->fpdR >> 17;
		starchild->fpdR ^= starchild->fpdR << 5;
		inputSampleR += (((double)starchild->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		//end 32 bit stereo floating point dither

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
	STARCHILD_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}
