#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI_2 1.57079632679489661923132169163975144

#define CONSOLELACHANNEL_URI "https://hannesbraun.net/ns/lv2/airwindows/consolelachannel"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TREBLE = 4,
	MID = 5,
	BASS = 6,
	PAN = 7,
	FADER = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* treble;
	const float* mid;
	const float* bass;
	const float* pan;
	const float* fader;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double subAL;
	double subAR;
	double subBL;
	double subBR;
	double subCL;
	double subCR;
	double hullL[225];
	double hullR[225];
	int hullp;
	double pearB[22];
	double midA;
	double midB;
	double bassA;
	double bassB;
	double gainA;
	double gainB; // smoothed master fader for channel, from Z2 series filter code
} ConsoleLAChannel;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	ConsoleLAChannel* consoleLAChannel = (ConsoleLAChannel*) calloc(1, sizeof(ConsoleLAChannel));
	consoleLAChannel->sampleRate = rate;
	return (LV2_Handle) consoleLAChannel;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	ConsoleLAChannel* consoleLAChannel = (ConsoleLAChannel*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			consoleLAChannel->input[0] = (const float*) data;
			break;
		case INPUT_R:
			consoleLAChannel->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			consoleLAChannel->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			consoleLAChannel->output[1] = (float*) data;
			break;
		case TREBLE:
			consoleLAChannel->treble = (const float*) data;
			break;
		case MID:
			consoleLAChannel->mid = (const float*) data;
			break;
		case BASS:
			consoleLAChannel->bass = (const float*) data;
			break;
		case PAN:
			consoleLAChannel->pan = (const float*) data;
			break;
		case FADER:
			consoleLAChannel->fader = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	ConsoleLAChannel* consoleLAChannel = (ConsoleLAChannel*) instance;

	for (int count = 0; count < 222; count++) {
		consoleLAChannel->hullL[count] = 0.0;
		consoleLAChannel->hullR[count] = 0.0;
	}
	consoleLAChannel->hullp = 1;
	for (int x = 0; x < 21; x++) consoleLAChannel->pearB[x] = 0.0;
	consoleLAChannel->subAL = consoleLAChannel->subAR = consoleLAChannel->subBL = consoleLAChannel->subBR = consoleLAChannel->subCL = consoleLAChannel->subCR = 0.0;
	consoleLAChannel->midA = consoleLAChannel->midB = 0.0;
	consoleLAChannel->bassA = consoleLAChannel->bassB = 0.0;
	consoleLAChannel->gainA = consoleLAChannel->gainB = 1.0;

	consoleLAChannel->fpdL = 1.0;
	while (consoleLAChannel->fpdL < 16386) consoleLAChannel->fpdL = rand() * UINT32_MAX;
	consoleLAChannel->fpdR = 1.0;
	while (consoleLAChannel->fpdR < 16386) consoleLAChannel->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	ConsoleLAChannel* consoleLAChannel = (ConsoleLAChannel*) instance;

	const float* in1 = consoleLAChannel->input[0];
	const float* in2 = consoleLAChannel->input[1];
	float* out1 = consoleLAChannel->output[0];
	float* out2 = consoleLAChannel->output[1];

	uint32_t inFramesToProcess = sampleFrames;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= consoleLAChannel->sampleRate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;

	int limit = 4 * cycleEnd;
	double divisor = 2.0 / limit;

	double treble = (*consoleLAChannel->treble * 6.0) - 3.0;
	consoleLAChannel->midA = consoleLAChannel->midB;
	consoleLAChannel->midB = (*consoleLAChannel->mid * 6.0) - 3.0;
	consoleLAChannel->bassA = consoleLAChannel->bassB;
	consoleLAChannel->bassB = (*consoleLAChannel->bass * 6.0) - 3.0;
	// these should stack to go up to -3.0 to 3.0
	if (treble < 0.0) treble /= 3.0;
	if (consoleLAChannel->midB < 0.0) consoleLAChannel->midB /= 3.0;
	if (consoleLAChannel->bassB < 0.0) consoleLAChannel->bassB /= 3.0;
	// and then become -1.0 to 3.0;
	// there will be successive sin/cos stages w. dry/wet in these
	double freqMid = 0.024;
	switch (cycleEnd) {
		case 1: // base sample rate, no change
			break;
		case 2: // 96k tier
			freqMid = 0.012;
			break;
		case 3: // 192k tier
			freqMid = 0.006;
			break;
	}

	int bitshiftL = 0;
	int bitshiftR = 0;
	double panControl = (*consoleLAChannel->pan * 2.0) - 1.0; //-1.0 to 1.0
	double panAttenuation = (1.0 - fabs(panControl));
	int panBits = 20; // start centered
	if (panAttenuation > 0.0) panBits = floor(1.0 / panAttenuation);
	if (panControl > 0.25) bitshiftL += panBits;
	if (panControl < -0.25) bitshiftR += panBits;
	if (bitshiftL < 0) bitshiftL = 0;
	if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < 0) bitshiftR = 0;
	if (bitshiftR > 17) bitshiftR = 17;
	double gainL = 1.0;
	double gainR = 1.0;
	switch (bitshiftL) {
		case 17:
			gainL = 0.0;
			break;
		case 16:
			gainL = 0.0000152587890625;
			break;
		case 15:
			gainL = 0.000030517578125;
			break;
		case 14:
			gainL = 0.00006103515625;
			break;
		case 13:
			gainL = 0.0001220703125;
			break;
		case 12:
			gainL = 0.000244140625;
			break;
		case 11:
			gainL = 0.00048828125;
			break;
		case 10:
			gainL = 0.0009765625;
			break;
		case 9:
			gainL = 0.001953125;
			break;
		case 8:
			gainL = 0.00390625;
			break;
		case 7:
			gainL = 0.0078125;
			break;
		case 6:
			gainL = 0.015625;
			break;
		case 5:
			gainL = 0.03125;
			break;
		case 4:
			gainL = 0.0625;
			break;
		case 3:
			gainL = 0.125;
			break;
		case 2:
			gainL = 0.25;
			break;
		case 1:
			gainL = 0.5;
			break;
		case 0:
			break;
	}
	switch (bitshiftR) {
		case 17:
			gainR = 0.0;
			break;
		case 16:
			gainR = 0.0000152587890625;
			break;
		case 15:
			gainR = 0.000030517578125;
			break;
		case 14:
			gainR = 0.00006103515625;
			break;
		case 13:
			gainR = 0.0001220703125;
			break;
		case 12:
			gainR = 0.000244140625;
			break;
		case 11:
			gainR = 0.00048828125;
			break;
		case 10:
			gainR = 0.0009765625;
			break;
		case 9:
			gainR = 0.001953125;
			break;
		case 8:
			gainR = 0.00390625;
			break;
		case 7:
			gainR = 0.0078125;
			break;
		case 6:
			gainR = 0.015625;
			break;
		case 5:
			gainR = 0.03125;
			break;
		case 4:
			gainR = 0.0625;
			break;
		case 3:
			gainR = 0.125;
			break;
		case 2:
			gainR = 0.25;
			break;
		case 1:
			gainR = 0.5;
			break;
		case 0:
			break;
	}

	consoleLAChannel->gainA = consoleLAChannel->gainB;
	consoleLAChannel->gainB = *consoleLAChannel->fader * 2.0; // smoothed master fader from Z2 filters
	// BitShiftGain pre gain trim goes here

	double subTrim = 0.0011 / overallscale;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = consoleLAChannel->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = consoleLAChannel->fpdR * 1.18e-17;

		double temp = (double) sampleFrames / inFramesToProcess;
		double gain = (consoleLAChannel->gainA * temp) + (consoleLAChannel->gainB * (1.0 - temp));
		double mid = (consoleLAChannel->midA * temp) + (consoleLAChannel->midB * (1.0 - temp));
		double bass = (consoleLAChannel->bassA * temp) + (consoleLAChannel->bassB * (1.0 - temp));

		// begin Hull2 Treble
		consoleLAChannel->hullp--;
		if (consoleLAChannel->hullp < 0) consoleLAChannel->hullp += 60;
		consoleLAChannel->hullL[consoleLAChannel->hullp] = consoleLAChannel->hullL[consoleLAChannel->hullp + 60] = inputSampleL;
		consoleLAChannel->hullR[consoleLAChannel->hullp] = consoleLAChannel->hullR[consoleLAChannel->hullp + 60] = inputSampleR;

		int x = consoleLAChannel->hullp;
		double bassL = 0.0;
		double bassR = 0.0;
		while (x < consoleLAChannel->hullp + (limit / 2)) {
			bassL += consoleLAChannel->hullL[x] * divisor;
			bassR += consoleLAChannel->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125;
		bassR += bassR * 0.125;
		while (x < consoleLAChannel->hullp + limit) {
			bassL -= consoleLAChannel->hullL[x] * 0.125 * divisor;
			bassR -= consoleLAChannel->hullR[x] * 0.125 * divisor;
			x++;
		}
		consoleLAChannel->hullL[consoleLAChannel->hullp + 20] = consoleLAChannel->hullL[consoleLAChannel->hullp + 80] = bassL;
		consoleLAChannel->hullR[consoleLAChannel->hullp + 20] = consoleLAChannel->hullR[consoleLAChannel->hullp + 80] = bassR;
		x = consoleLAChannel->hullp + 20;
		bassL = bassR = 0.0;
		while (x < consoleLAChannel->hullp + 20 + (limit / 2)) {
			bassL += consoleLAChannel->hullL[x] * divisor;
			bassR += consoleLAChannel->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125;
		bassR += bassR * 0.125;
		while (x < consoleLAChannel->hullp + 20 + limit) {
			bassL -= consoleLAChannel->hullL[x] * 0.125 * divisor;
			bassR -= consoleLAChannel->hullR[x] * 0.125 * divisor;
			x++;
		}
		consoleLAChannel->hullL[consoleLAChannel->hullp + 40] = consoleLAChannel->hullL[consoleLAChannel->hullp + 100] = bassL;
		consoleLAChannel->hullR[consoleLAChannel->hullp + 40] = consoleLAChannel->hullR[consoleLAChannel->hullp + 100] = bassR;
		x = consoleLAChannel->hullp + 40;
		bassL = bassR = 0.0;
		while (x < consoleLAChannel->hullp + 40 + (limit / 2)) {
			bassL += consoleLAChannel->hullL[x] * divisor;
			bassR += consoleLAChannel->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125;
		bassR += bassR * 0.125;
		while (x < consoleLAChannel->hullp + 40 + limit) {
			bassL -= consoleLAChannel->hullL[x] * 0.125 * divisor;
			bassR -= consoleLAChannel->hullR[x] * 0.125 * divisor;
			x++;
		}
		double trebleL = inputSampleL - bassL;
		inputSampleL = bassL;
		double trebleR = inputSampleR - bassR;
		inputSampleR = bassR;
		// end Hull2 treble

		// begin Pear filter stages
		// at this point 'bass' is actually still mid and bass
		double slew = ((bassL - consoleLAChannel->pearB[0]) + consoleLAChannel->pearB[1]) * freqMid * 0.5;
		consoleLAChannel->pearB[0] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (consoleLAChannel->pearB[0] + consoleLAChannel->pearB[1]));
		consoleLAChannel->pearB[1] = slew;
		slew = ((bassR - consoleLAChannel->pearB[2]) + consoleLAChannel->pearB[3]) * freqMid * 0.5;
		consoleLAChannel->pearB[2] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (consoleLAChannel->pearB[2] + consoleLAChannel->pearB[3]));
		consoleLAChannel->pearB[3] = slew;
		slew = ((bassL - consoleLAChannel->pearB[4]) + consoleLAChannel->pearB[5]) * freqMid * 0.5;
		consoleLAChannel->pearB[4] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (consoleLAChannel->pearB[4] + consoleLAChannel->pearB[5]));
		consoleLAChannel->pearB[5] = slew;
		slew = ((bassR - consoleLAChannel->pearB[6]) + consoleLAChannel->pearB[7]) * freqMid * 0.5;
		consoleLAChannel->pearB[6] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (consoleLAChannel->pearB[6] + consoleLAChannel->pearB[7]));
		consoleLAChannel->pearB[7] = slew;
		slew = ((bassL - consoleLAChannel->pearB[8]) + consoleLAChannel->pearB[9]) * freqMid * 0.5;
		consoleLAChannel->pearB[8] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (consoleLAChannel->pearB[8] + consoleLAChannel->pearB[9]));
		consoleLAChannel->pearB[9] = slew;
		slew = ((bassR - consoleLAChannel->pearB[10]) + consoleLAChannel->pearB[11]) * freqMid * 0.5;
		consoleLAChannel->pearB[10] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (consoleLAChannel->pearB[10] + consoleLAChannel->pearB[11]));
		consoleLAChannel->pearB[11] = slew;
		slew = ((bassL - consoleLAChannel->pearB[12]) + consoleLAChannel->pearB[13]) * freqMid * 0.5;
		consoleLAChannel->pearB[12] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (consoleLAChannel->pearB[12] + consoleLAChannel->pearB[13]));
		consoleLAChannel->pearB[13] = slew;
		slew = ((bassR - consoleLAChannel->pearB[14]) + consoleLAChannel->pearB[15]) * freqMid * 0.5;
		consoleLAChannel->pearB[14] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (consoleLAChannel->pearB[14] + consoleLAChannel->pearB[15]));
		consoleLAChannel->pearB[15] = slew;
		slew = ((bassL - consoleLAChannel->pearB[16]) + consoleLAChannel->pearB[17]) * freqMid * 0.5;
		consoleLAChannel->pearB[16] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (consoleLAChannel->pearB[16] + consoleLAChannel->pearB[17]));
		consoleLAChannel->pearB[17] = slew;
		slew = ((bassR - consoleLAChannel->pearB[18]) + consoleLAChannel->pearB[19]) * freqMid * 0.5;
		consoleLAChannel->pearB[18] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (consoleLAChannel->pearB[18] + consoleLAChannel->pearB[19]));
		consoleLAChannel->pearB[19] = slew;
		double midL = inputSampleL - bassL;
		double midR = inputSampleR - bassR;
		// we now have three bands out of hull and pear filters

		double w = 0.0; // filter into bands, apply the sin/cos to each band
		if (treble > 0.0) {
			w = treble;
			if (w > 1.0) w = 1.0;
			trebleL = (trebleL * (1.0 - w)) + (sin(trebleL * M_PI_2) * treble);
			trebleR = (trebleR * (1.0 - w)) + (sin(trebleR * M_PI_2) * treble);
		}
		if (treble < 0.0) {
			if (trebleL > 1.0) trebleL = 1.0;
			if (trebleL < -1.0) trebleL = -1.0;
			if (trebleR > 1.0) trebleR = 1.0;
			if (trebleR < -1.0) trebleR = -1.0;
			w = -treble;
			if (w > 1.0) w = 1.0;
			if (trebleL > 0) trebleL = (trebleL * (1.0 - w)) + ((1.0 - cos(trebleL * w)) * (1.0 - w));
			else trebleL = (trebleL * (1.0 - w)) + ((-1.0 + cos(-trebleL * w)) * (1.0 - w));
			if (trebleR > 0) trebleR = (trebleR * (1.0 - w)) + ((1.0 - cos(trebleR * w)) * (1.0 - w));
			else trebleR = (trebleR * (1.0 - w)) + ((-1.0 + cos(-trebleR * w)) * (1.0 - w));
		} // cosine stages for EQ or expansion

		if (midL > 1.0) midL = 1.0;
		if (midL < -1.0) midL = -1.0;
		if (midR > 1.0) midR = 1.0;
		if (midR < -1.0) midR = -1.0;
		if (mid > 0.0) {
			w = mid;
			if (w > 1.0) w = 1.0;
			midL = (midL * (1.0 - w)) + (sin(midL * M_PI_2) * mid);
			midR = (midR * (1.0 - w)) + (sin(midR * M_PI_2) * mid);
		}
		if (mid < 0.0) {
			w = -mid;
			if (w > 1.0) w = 1.0;
			if (midL > 0) midL = (midL * (1.0 - w)) + ((1.0 - cos(midL * w)) * (1.0 - w));
			else midL = (midL * (1.0 - w)) + ((-1.0 + cos(-midL * w)) * (1.0 - w));
			if (midR > 0) midR = (midR * (1.0 - w)) + ((1.0 - cos(midR * w)) * (1.0 - w));
			else midR = (midR * (1.0 - w)) + ((-1.0 + cos(-midR * w)) * (1.0 - w));
		} // cosine stages for EQ or expansion

		if (bassL > 1.0) bassL = 1.0;
		if (bassL < -1.0) bassL = -1.0;
		if (bassR > 1.0) bassR = 1.0;
		if (bassR < -1.0) bassR = -1.0;
		if (bass > 0.0) {
			w = bass;
			if (w > 1.0) w = 1.0;
			bassL = (bassL * (1.0 - w)) + (sin(bassL * M_PI_2) * bass);
			bassR = (bassR * (1.0 - w)) + (sin(bassR * M_PI_2) * bass);
		}
		if (bass < 0.0) {
			w = -bass;
			if (w > 1.0) w = 1.0;
			if (bassL > 0) bassL = (bassL * (1.0 - w)) + ((1.0 - cos(bassL * w)) * (1.0 - w));
			else bassL = (bassL * (1.0 - w)) + ((-1.0 + cos(-bassL * w)) * (1.0 - w));
			if (bassR > 0) bassR = (bassR * (1.0 - w)) + ((1.0 - cos(bassR * w)) * (1.0 - w));
			else bassR = (bassR * (1.0 - w)) + ((-1.0 + cos(-bassR * w)) * (1.0 - w));
		} // cosine stages for EQ or expansion

		inputSampleL = (bassL + midL + trebleL) * gainL * gain;
		inputSampleR = (bassR + midR + trebleR) * gainR * gain;
		// applies BitShiftPan pan section, and smoothed fader gain

		// begin SubTight section
		double subSampleL = inputSampleL * subTrim;
		double subSampleR = inputSampleR * subTrim;

		double scale = 0.5 + fabs(subSampleL * 0.5);
		subSampleL = (consoleLAChannel->subAL + (sin(consoleLAChannel->subAL - subSampleL) * scale));
		consoleLAChannel->subAL = subSampleL * scale;
		scale = 0.5 + fabs(subSampleR * 0.5);
		subSampleR = (consoleLAChannel->subAR + (sin(consoleLAChannel->subAR - subSampleR) * scale));
		consoleLAChannel->subAR = subSampleR * scale;
		scale = 0.5 + fabs(subSampleL * 0.5);
		subSampleL = (consoleLAChannel->subBL + (sin(consoleLAChannel->subBL - subSampleL) * scale));
		consoleLAChannel->subBL = subSampleL * scale;
		scale = 0.5 + fabs(subSampleR * 0.5);
		subSampleR = (consoleLAChannel->subBR + (sin(consoleLAChannel->subBR - subSampleR) * scale));
		consoleLAChannel->subBR = subSampleR * scale;
		scale = 0.5 + fabs(subSampleL * 0.5);
		subSampleL = (consoleLAChannel->subCL + (sin(consoleLAChannel->subCL - subSampleL) * scale));
		consoleLAChannel->subCL = subSampleL * scale;
		scale = 0.5 + fabs(subSampleR * 0.5);
		subSampleR = (consoleLAChannel->subCR + (sin(consoleLAChannel->subCR - subSampleR) * scale));
		consoleLAChannel->subCR = subSampleR * scale;
		if (subSampleL > 0.25) subSampleL = 0.25;
		if (subSampleL < -0.25) subSampleL = -0.25;
		if (subSampleR > 0.25) subSampleR = 0.25;
		if (subSampleR < -0.25) subSampleR = -0.25;
		inputSampleL += (subSampleL * 16.0);
		inputSampleR += (subSampleR * 16.0);
		// end SubTight section

		// begin Console7 Channel processing
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
		inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
		// this is a version of Spiral blended 80/20 with regular Density.
		// It's blending between two different harmonics in the overtones of the algorithm

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		consoleLAChannel->fpdL ^= consoleLAChannel->fpdL << 13;
		consoleLAChannel->fpdL ^= consoleLAChannel->fpdL >> 17;
		consoleLAChannel->fpdL ^= consoleLAChannel->fpdL << 5;
		inputSampleL += (((double) consoleLAChannel->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		consoleLAChannel->fpdR ^= consoleLAChannel->fpdR << 13;
		consoleLAChannel->fpdR ^= consoleLAChannel->fpdR >> 17;
		consoleLAChannel->fpdR ^= consoleLAChannel->fpdR << 5;
		inputSampleR += (((double) consoleLAChannel->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	CONSOLELACHANNEL_URI,
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
