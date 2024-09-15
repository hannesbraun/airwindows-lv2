#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define DOUBLELAY_URI "https://hannesbraun.net/ns/lv2/airwindows/doublelay"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DETUNE = 4,
	DELAY_L = 5,
	DELAY_R = 6,
	FEEDBK = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* detune;
	const float* delayL;
	const float* delayR;
	const float* feedbk;
	const float* dryWet;

	double dL[48010];
	double dR[48010];
	int dcount;
	double pL[5010];
	double pR[5010];
	int gcountL;
	int lastcountL;
	int gcountR;
	int lastcountR;
	int prevwidth;
	double trackingL[9];
	double tempL[9];
	double positionL[9];
	double lastpositionL[9];
	double trackingR[9];
	double tempR[9];
	double positionR[9];
	double lastpositionR[9];
	int activeL;
	int bestspliceL;
	int activeR;
	int bestspliceR;
	double feedbackL;
	double feedbackR;
	double bestyetL;
	double bestyetR;
	double airPrevL;
	double airEvenL;
	double airOddL;
	double airFactorL;
	double airPrevR;
	double airEvenR;
	double airOddR;
	double airFactorR;
	bool flip;

	double lastRefL[7];
	double lastRefR[7];
	int cycle;

	uint32_t fpdL;
	uint32_t fpdR;
} Doublelay;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Doublelay* doublelay = (Doublelay*) calloc(1, sizeof(Doublelay));
	doublelay->sampleRate = rate;
	return (LV2_Handle) doublelay;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Doublelay* doublelay = (Doublelay*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			doublelay->input[0] = (const float*) data;
			break;
		case INPUT_R:
			doublelay->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			doublelay->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			doublelay->output[1] = (float*) data;
			break;
		case DETUNE:
			doublelay->detune = (const float*) data;
			break;
		case DELAY_L:
			doublelay->delayL = (const float*) data;
			break;
		case DELAY_R:
			doublelay->delayR = (const float*) data;
			break;
		case FEEDBK:
			doublelay->feedbk = (const float*) data;
			break;
		case DRY_WET:
			doublelay->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Doublelay* doublelay = (Doublelay*) instance;

	for(int count = 0; count < 48009; count++) {doublelay->dL[count] = 0.0; doublelay->dR[count] = 0.0;}
	doublelay->dcount = 0;
	for(int count = 0; count < 5009; count++) {doublelay->pL[count] = 0.0; doublelay->pR[count] = 0.0;}
	for(int count = 0; count < 8; count++)
	{doublelay->tempL[count] = 0.0; doublelay->positionL[count] = 0.0; doublelay->lastpositionL[count] = 0.0; doublelay->trackingL[count] = 0.0;}
	for(int count = 0; count < 8; count++)
	{doublelay->tempR[count] = 0.0; doublelay->positionR[count] = 0.0; doublelay->lastpositionR[count] = 0.0; doublelay->trackingR[count] = 0.0;}
	doublelay->gcountL = 0;
	doublelay->lastcountL = 0;
	doublelay->gcountR = 0;
	doublelay->lastcountR = 0;
	doublelay->prevwidth = 0;
	doublelay->feedbackL = 0.0;
	doublelay->feedbackR = 0.0;
	doublelay->activeL = 0;
	doublelay->bestspliceL = 4;
	doublelay->activeR = 0;
	doublelay->bestspliceR = 4;
	doublelay->bestyetL = 1.0;
	doublelay->bestyetR = 1.0;
	doublelay->airPrevL = 0.0;
	doublelay->airEvenL = 0.0;
	doublelay->airOddL = 0.0;
	doublelay->airFactorL = 0.0;
	doublelay->airPrevR = 0.0;
	doublelay->airEvenR = 0.0;
	doublelay->airOddR = 0.0;
	doublelay->airFactorR = 0.0;
	doublelay->flip = false;

	for(int count = 0; count < 6; count++) {doublelay->lastRefL[count] = 0.0;doublelay->lastRefR[count] = 0.0;}
	doublelay->cycle = 0;

	doublelay->fpdL = 1.0; while (doublelay->fpdL < 16386) doublelay->fpdL = rand()*UINT32_MAX;
	doublelay->fpdR = 1.0; while (doublelay->fpdR < 16386) doublelay->fpdR = rand()*UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Doublelay* doublelay = (Doublelay*) instance;

	const float* in1 = doublelay->input[0];
	const float* in2 = doublelay->input[1];
	float* out1 = doublelay->output[0];
	float* out2 = doublelay->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	const double sampleRate = doublelay->sampleRate;
	overallscale *= sampleRate;

	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (doublelay->cycle > cycleEnd-1) doublelay->cycle = cycleEnd-1; //sanity check
	double delayTrim = (sampleRate/cycleEnd)/48001.0; //this gives us a time adjustment
	if (delayTrim > 0.99999) delayTrim = 0.99999; //sanity check so we don't smash our delay buffer
	if (delayTrim < 0.0) delayTrim = 0.0; //sanity check so we don't smash our delay buffer

	double trim = *doublelay->detune;
	trim *= fabs(trim);
	trim /= 40;
	double speedL = trim+1.0;
	double speedR = (-trim)+1.0;
	if (speedL < 0.0) speedL = 0.0;
	if (speedR < 0.0) speedR = 0.0;

	int delayL = (*doublelay->delayL*(int)(48000.0*delayTrim));
	int delayR = (*doublelay->delayR*(int)(48000.0*delayTrim));
	//this now adjusts to give exactly one second max delay at all times up to 48k
	//or multipliers of it using undersampling

	double adjust = 1100;
	int width = 2300;
	if (doublelay->prevwidth != width)
	{
		doublelay->positionL[0] = 0;
		doublelay->positionL[1] = (int)(width/3);
		doublelay->positionL[2] = (int)((width/3)*2);
		doublelay->positionL[3] = (int)(width/5);
		doublelay->positionL[4] = (int)((width/5)*2);
		doublelay->positionL[5] = (int)((width/5)*3);
		doublelay->positionL[6] = (int)((width/5)*4);
		doublelay->positionL[7] = (int)(width/2);
		doublelay->positionR[0] = 0;
		doublelay->positionR[1] = (int)(width/3);
		doublelay->positionR[2] = (int)((width/3)*2);
		doublelay->positionR[3] = (int)(width/5);
		doublelay->positionR[4] = (int)((width/5)*2);
		doublelay->positionR[5] = (int)((width/5)*3);
		doublelay->positionR[6] = (int)((width/5)*4);
		doublelay->positionR[7] = (int)(width/2);
		doublelay->prevwidth = width;
	}
	const double feedbk = *doublelay->feedbk;
	double feedbackDirect = feedbk * 0.618033988749894848204586;
	double feedbackCross = feedbk * (1.0-0.618033988749894848204586);
	double wet = *doublelay->dryWet;
	int gplusL;
	int lastplusL;
	int gplusR;
	int lastplusR;
	double posplusL;
	double lastposplusL;
	double posplusR;
	double lastposplusR;
	double depth;
	double crossfade;
	int count;
	int bcountL;
	int bcountR;
	int base;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = doublelay->fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = doublelay->fpdR * 1.18e-17;

		doublelay->cycle++;
		if (doublelay->cycle == cycleEnd) { //hit the end point and we do a doubler sample
			double drySampleL = inputSampleL;
			double drySampleR = inputSampleR;

			//assign working variables
			doublelay->airFactorL = doublelay->airPrevL - inputSampleL;
			if (doublelay->flip) {doublelay->airEvenL += doublelay->airFactorL; doublelay->airOddL -= doublelay->airFactorL; doublelay->airFactorL = doublelay->airEvenL;}
			else {doublelay->airOddL += doublelay->airFactorL; doublelay->airEvenL -= doublelay->airFactorL; doublelay->airFactorL = doublelay->airOddL;}
			doublelay->airOddL = (doublelay->airOddL - ((doublelay->airOddL - doublelay->airEvenL)/256.0)) / 1.0001;
			doublelay->airEvenL = (doublelay->airEvenL - ((doublelay->airEvenL - doublelay->airOddL)/256.0)) / 1.0001;
			doublelay->airPrevL = inputSampleL;
			inputSampleL += doublelay->airFactorL;
			//left
			doublelay->airFactorR = doublelay->airPrevR - inputSampleR;
			if (doublelay->flip) {doublelay->airEvenR += doublelay->airFactorR; doublelay->airOddR -= doublelay->airFactorR; doublelay->airFactorR = doublelay->airEvenR;}
			else {doublelay->airOddR += doublelay->airFactorR; doublelay->airEvenR -= doublelay->airFactorR; doublelay->airFactorR = doublelay->airOddR;}
			doublelay->airOddR = (doublelay->airOddR - ((doublelay->airOddR - doublelay->airEvenR)/256.0)) / 1.0001;
			doublelay->airEvenR = (doublelay->airEvenR - ((doublelay->airEvenR - doublelay->airOddR)/256.0)) / 1.0001;
			doublelay->airPrevR = inputSampleR;
			inputSampleR += doublelay->airFactorR;
			//right
			doublelay->flip = !doublelay->flip;
			//air, compensates for loss of highs in flanger's interpolation

			inputSampleL += doublelay->feedbackL*feedbackDirect;
			inputSampleR += doublelay->feedbackR*feedbackDirect;
			inputSampleL += doublelay->feedbackR*feedbackCross;
			inputSampleR += doublelay->feedbackL*feedbackCross;

			if (doublelay->dcount < 1 || doublelay->dcount > 48005) doublelay->dcount = 48005;
			count = doublelay->dcount;

			doublelay->dL[count] = inputSampleL;
			doublelay->dR[count] = inputSampleR;
			//double buffer

			inputSampleL = doublelay->dL[count+delayL-((count+delayL>48005)?48005:0)];
			inputSampleR = doublelay->dR[count+delayR-((count+delayR>48005)?48005:0)];
			//assign delays

			doublelay->dcount--;
			doublelay->gcountL++;
			doublelay->gcountR++;
			for(count = 0; count < 8; count++) {doublelay->positionL[count] += speedL; doublelay->positionR[count] += speedR;}

			gplusL = doublelay->gcountL+(int)adjust;
			lastplusL = doublelay->lastcountL+(int)adjust;
			if (gplusL > width) {gplusL -= width;}
			if (lastplusL > width) {lastplusL -= width;}

			gplusR = doublelay->gcountR+(int)adjust;
			lastplusR = doublelay->lastcountR+(int)adjust;
			if (gplusR > width) {gplusR -= width;}
			if (lastplusR > width) {lastplusR -= width;}

			if (doublelay->trackingL[doublelay->activeL] == 0.0)
			{
				posplusL = doublelay->positionL[doublelay->activeL]+adjust;
				lastposplusL = doublelay->lastpositionL[doublelay->activeL]+adjust;
				if (posplusL > width) {posplusL -= width;}
				if (lastposplusL > width) {lastposplusL -= width;}
				if ((gplusL > doublelay->positionL[doublelay->activeL]) && (lastplusL < doublelay->lastpositionL[doublelay->activeL])) {doublelay->trackingL[doublelay->activeL] = 1.0;}
				if ((posplusL > doublelay->gcountL) && (lastposplusL < doublelay->lastcountL)) {doublelay->trackingL[doublelay->activeL] = 1.0;}
				//fire splice based on whether somebody moved past somebody else just now
			}

			if (doublelay->trackingR[doublelay->activeR] == 0.0)
			{
				posplusR = doublelay->positionR[doublelay->activeR]+adjust;
				lastposplusR = doublelay->lastpositionR[doublelay->activeR]+adjust;
				if (posplusR > width) {posplusR -= width;}
				if (lastposplusR > width) {lastposplusR -= width;}
				if ((gplusR > doublelay->positionR[doublelay->activeR]) && (lastplusR < doublelay->lastpositionR[doublelay->activeR])) {doublelay->trackingR[doublelay->activeR] = 1.0;}
				if ((posplusR > doublelay->gcountR) && (lastposplusR < doublelay->lastcountR)) {doublelay->trackingR[doublelay->activeR] = 1.0;}
				//fire splice based on whether somebody moved past somebody else just now
			}

			for(count = 0; count < 8; count++)
			{
				if (doublelay->positionL[count] > width) {doublelay->positionL[count] -= width;}
				if (doublelay->positionR[count] > width) {doublelay->positionR[count] -= width;}
				doublelay->lastpositionL[count] = doublelay->positionL[count];
				doublelay->lastpositionR[count] = doublelay->positionR[count];
			}
			if (doublelay->gcountL < 0 || doublelay->gcountL > width) {doublelay->gcountL -= width;}
			doublelay->lastcountL = bcountL = doublelay->gcountL;
			if (doublelay->gcountR < 0 || doublelay->gcountR > width) {doublelay->gcountR -= width;}
			doublelay->lastcountR = bcountR = doublelay->gcountR;

			doublelay->pL[bcountL+width] = doublelay->pL[bcountL] = inputSampleL;
			doublelay->pR[bcountR+width] = doublelay->pR[bcountR] = inputSampleR;


			for(count = 0; count < 8; count++)
			{
				base = (int)floor(doublelay->positionL[count]);
				doublelay->tempL[count] = (doublelay->pL[base] * (1-(doublelay->positionL[count]-base))); //less as value moves away from .0
				doublelay->tempL[count] += doublelay->pL[base+1]; //we can assume always using this in one way or another?
				doublelay->tempL[count] += (doublelay->pL[base+2] * (doublelay->positionL[count]-base)); //greater as value moves away from .0
				doublelay->tempL[count] -= (((doublelay->pL[base]-doublelay->pL[base+1])-(doublelay->pL[base+1]-doublelay->pL[base+2]))/50); //interpolation hacks 'r us
				doublelay->tempL[count] /= 2;

				base = (int)floor(doublelay->positionR[count]);
				doublelay->tempR[count] = (doublelay->pR[base] * (1-(doublelay->positionR[count]-base))); //less as value moves away from .0
				doublelay->tempR[count] += doublelay->pR[base+1]; //we can assume always using this in one way or another?
				doublelay->tempR[count] += (doublelay->pR[base+2] * (doublelay->positionR[count]-base)); //greater as value moves away from .0
				doublelay->tempR[count] -= (((doublelay->pR[base]-doublelay->pR[base+1])-(doublelay->pR[base+1]-doublelay->pR[base+2]))/50); //interpolation hacks 'r us
				doublelay->tempR[count] /= 2;
			}


			if (doublelay->trackingL[doublelay->activeL] > 0.0)
			{
				crossfade = sin(doublelay->trackingL[doublelay->bestspliceL]*1.57);
				inputSampleL = (doublelay->tempL[doublelay->activeL]*crossfade)+(doublelay->tempL[doublelay->bestspliceL]*(1.0-crossfade));

				for(count = 0; count < 8; count++)
				{
					depth = (0.5-fabs(doublelay->tempL[doublelay->activeL]-doublelay->tempL[count]));
					if ((depth > 0) && (count != doublelay->activeL))
					{doublelay->trackingL[count] -= (depth/adjust);
						doublelay->bestspliceL = count;}
					//take down the splicings but skip the current one
				}
				doublelay->bestyetL = 1.0;
				for(count = 0; count < 8; count++)
				{
					if ((doublelay->trackingL[count] < doublelay->bestyetL)&&(count != doublelay->activeL))
					{doublelay->bestspliceL = count; doublelay->bestyetL = doublelay->trackingL[count];}
				}

				if (doublelay->trackingL[doublelay->bestspliceL] < 0.0)
				{
					for(count = 0; count < 8; count++)
					{doublelay->trackingL[count] = 1.0;}
					doublelay->activeL = doublelay->bestspliceL;
					doublelay->trackingL[doublelay->activeL] = 0.0;
				}
			}
			else inputSampleL = doublelay->tempL[doublelay->activeL];

			if (doublelay->trackingR[doublelay->activeR] > 0.0)
			{
				crossfade = sin(doublelay->trackingR[doublelay->bestspliceR]*1.57);
				inputSampleR = (doublelay->tempR[doublelay->activeR]*crossfade)+(doublelay->tempR[doublelay->bestspliceR]*(1.0-crossfade));

				for(count = 0; count < 8; count++)
				{
					depth = (0.5-fabs(doublelay->tempR[doublelay->activeR]-doublelay->tempR[count]));
					if ((depth > 0) && (count != doublelay->activeR))
					{doublelay->trackingR[count] -= (depth/adjust); doublelay->bestspliceR = count;}
					//take down the splicings but skip the current one
				}
				doublelay->bestyetR = 1.0;
				for(count = 0; count < 8; count++)
				{
					if ((doublelay->trackingR[count] < doublelay->bestyetR)&&(count != doublelay->activeR))
					{doublelay->bestspliceR = count; doublelay->bestyetR = doublelay->trackingR[count];}
				}

				if (doublelay->trackingR[doublelay->bestspliceR] < 0.0)
				{
					for(count = 0; count < 8; count++)
					{doublelay->trackingR[count] = 1.0;}
					doublelay->activeR = doublelay->bestspliceR;
					doublelay->trackingR[doublelay->activeR] = 0.0;
				}
			}
			else inputSampleR = doublelay->tempR[doublelay->activeR];

			doublelay->feedbackL = inputSampleL;
			doublelay->feedbackR = inputSampleR;
			//feedback section

			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0-wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0-wet));

			if (cycleEnd == 4) {
				doublelay->lastRefL[0] = doublelay->lastRefL[4]; //start from previous last
				doublelay->lastRefL[2] = (doublelay->lastRefL[0] + inputSampleL)/2; //half
				doublelay->lastRefL[1] = (doublelay->lastRefL[0] + doublelay->lastRefL[2])/2; //one quarter
				doublelay->lastRefL[3] = (doublelay->lastRefL[2] + inputSampleL)/2; //three quarters
				doublelay->lastRefL[4] = inputSampleL; //full
				doublelay->lastRefR[0] = doublelay->lastRefR[4]; //start from previous last
				doublelay->lastRefR[2] = (doublelay->lastRefR[0] + inputSampleR)/2; //half
				doublelay->lastRefR[1] = (doublelay->lastRefR[0] + doublelay->lastRefR[2])/2; //one quarter
				doublelay->lastRefR[3] = (doublelay->lastRefR[2] + inputSampleR)/2; //three quarters
				doublelay->lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				doublelay->lastRefL[0] = doublelay->lastRefL[3]; //start from previous last
				doublelay->lastRefL[2] = (doublelay->lastRefL[0]+doublelay->lastRefL[0]+inputSampleL)/3; //third
				doublelay->lastRefL[1] = (doublelay->lastRefL[0]+inputSampleL+inputSampleL)/3; //two thirds
				doublelay->lastRefL[3] = inputSampleL; //full
				doublelay->lastRefR[0] = doublelay->lastRefR[3]; //start from previous last
				doublelay->lastRefR[2] = (doublelay->lastRefR[0]+doublelay->lastRefR[0]+inputSampleR)/3; //third
				doublelay->lastRefR[1] = (doublelay->lastRefR[0]+inputSampleR+inputSampleR)/3; //two thirds
				doublelay->lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				doublelay->lastRefL[0] = doublelay->lastRefL[2]; //start from previous last
				doublelay->lastRefL[1] = (doublelay->lastRefL[0] + inputSampleL)/2; //half
				doublelay->lastRefL[2] = inputSampleL; //full
				doublelay->lastRefR[0] = doublelay->lastRefR[2]; //start from previous last
				doublelay->lastRefR[1] = (doublelay->lastRefR[0] + inputSampleR)/2; //half
				doublelay->lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				doublelay->lastRefL[0] = inputSampleL;
				doublelay->lastRefR[0] = inputSampleR;
			}
			doublelay->cycle = 0; //reset
			inputSampleL = doublelay->lastRefL[doublelay->cycle];
			inputSampleR = doublelay->lastRefR[doublelay->cycle];
		} else {
			inputSampleL = doublelay->lastRefL[doublelay->cycle];
			inputSampleR = doublelay->lastRefR[doublelay->cycle];
			//we are going through our references now
		}

		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		doublelay->fpdL ^= doublelay->fpdL << 13; doublelay->fpdL ^= doublelay->fpdL >> 17; doublelay->fpdL ^= doublelay->fpdL << 5;
		inputSampleL += (((double)doublelay->fpdL-(uint32_t)0x7fffffff) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		doublelay->fpdR ^= doublelay->fpdR << 13; doublelay->fpdR ^= doublelay->fpdR >> 17; doublelay->fpdR ^= doublelay->fpdR << 5;
		inputSampleR += (((double)doublelay->fpdR-(uint32_t)0x7fffffff) * 5.5e-36l * pow(2,expon+62));
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
	DOUBLELAY_URI,
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
