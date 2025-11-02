#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define POINTYGUITAR_URI "https://hannesbraun.net/ns/lv2/airwindows/pointyguitar"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRIVE = 4,
	PRESNCE = 5,
	HIGH = 6,
	MID = 7,
	LOW = 8,
	SUB = 9,
	HSPEAKR = 10,
	LSPEAKR = 11,
	GATE = 12,
	OUTPUT = 13
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* drive;
	const float* presnce;
	const float* high;
	const float* mid;
	const float* low;
	const float* sub;
	const float* hspeakr;
	const float* lspeakr;
	const float* gate;
	const float* outputGain;

	double angSL[18][12];
	double angAL[18][12];
	double iirHPositionL[37];
	double iirHAngleL[37];
	double iirBPositionL[37];
	double iirBAngleL[37];
	bool WasNegativeL;
	int ZeroCrossL;
	double gaterollerL;
	double gateL;

	double angSR[18][12];
	double angAR[18][12];
	double iirHPositionR[37];
	double iirHAngleR[37];
	double iirBPositionR[37];
	double iirBAngleR[37];
	bool WasNegativeR;
	int ZeroCrossR;
	double gaterollerR;
	double gateR;

	double angG[12];

	uint32_t fpdL;
	uint32_t fpdR;

} PointyGuitar;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PointyGuitar* pointyGuitar = (PointyGuitar*) calloc(1, sizeof(PointyGuitar));
	pointyGuitar->sampleRate = rate;
	return (LV2_Handle) pointyGuitar;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PointyGuitar* pointyGuitar = (PointyGuitar*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pointyGuitar->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pointyGuitar->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pointyGuitar->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pointyGuitar->output[1] = (float*) data;
			break;
		case DRIVE:
			pointyGuitar->drive = (const float*) data;
			break;
		case PRESNCE:
			pointyGuitar->presnce = (const float*) data;
			break;
		case HIGH:
			pointyGuitar->high = (const float*) data;
			break;
		case MID:
			pointyGuitar->mid = (const float*) data;
			break;
		case LOW:
			pointyGuitar->low = (const float*) data;
			break;
		case SUB:
			pointyGuitar->sub = (const float*) data;
			break;
		case HSPEAKR:
			pointyGuitar->hspeakr = (const float*) data;
			break;
		case LSPEAKR:
			pointyGuitar->lspeakr = (const float*) data;
			break;
		case GATE:
			pointyGuitar->gate = (const float*) data;
			break;
		case OUTPUT:
			pointyGuitar->outputGain = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PointyGuitar* pointyGuitar = (PointyGuitar*) instance;

	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 11; y++) {
			pointyGuitar->angSL[x][y] = 0.0;
			pointyGuitar->angAL[x][y] = 0.0;
			pointyGuitar->angSR[x][y] = 0.0;
			pointyGuitar->angAR[x][y] = 0.0;
		}
	}
	for (int y = 0; y < 11; y++) pointyGuitar->angG[y] = 0.0;
	for (int count = 0; count < 36; count++) {
		pointyGuitar->iirHPositionL[count] = 0.0;
		pointyGuitar->iirHAngleL[count] = 0.0;
		pointyGuitar->iirBPositionL[count] = 0.0;
		pointyGuitar->iirBAngleL[count] = 0.0;
		pointyGuitar->iirHPositionR[count] = 0.0;
		pointyGuitar->iirHAngleR[count] = 0.0;
		pointyGuitar->iirBPositionR[count] = 0.0;
		pointyGuitar->iirBAngleR[count] = 0.0;
	}
	pointyGuitar->WasNegativeL = false;
	pointyGuitar->ZeroCrossL = 0;
	pointyGuitar->gaterollerL = 0.0;
	pointyGuitar->WasNegativeR = false;
	pointyGuitar->ZeroCrossR = 0;
	pointyGuitar->gaterollerR = 0.0;

	pointyGuitar->fpdL = 1.0;
	while (pointyGuitar->fpdL < 16386) pointyGuitar->fpdL = rand() * UINT32_MAX;
	pointyGuitar->fpdR = 1.0;
	while (pointyGuitar->fpdR < 16386) pointyGuitar->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PointyGuitar* pointyGuitar = (PointyGuitar*) instance;

	const float* in1 = pointyGuitar->input[0];
	const float* in2 = pointyGuitar->input[1];
	float* out1 = pointyGuitar->output[0];
	float* out2 = pointyGuitar->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= pointyGuitar->sampleRate;

	double drive = *pointyGuitar->drive + 0.618033988749894;
	pointyGuitar->angG[0] = sqrt(*pointyGuitar->presnce * 2.0);
	pointyGuitar->angG[2] = sqrt(*pointyGuitar->high * 2.0);
	pointyGuitar->angG[4] = sqrt(*pointyGuitar->mid * 2.0);
	pointyGuitar->angG[6] = sqrt(*pointyGuitar->low * 2.0);
	pointyGuitar->angG[8] = sqrt(*pointyGuitar->sub * 2.0);
	pointyGuitar->angG[1] = (pointyGuitar->angG[0] + pointyGuitar->angG[2]) * 0.5;
	pointyGuitar->angG[3] = (pointyGuitar->angG[2] + pointyGuitar->angG[4]) * 0.5;
	pointyGuitar->angG[5] = (pointyGuitar->angG[4] + pointyGuitar->angG[6]) * 0.5;
	pointyGuitar->angG[7] = (pointyGuitar->angG[6] + pointyGuitar->angG[8]) * 0.5;
	pointyGuitar->angG[9] = pointyGuitar->angG[8];
	int poles = (int) (drive * 10.0);
	double hFreq = pow(*pointyGuitar->hspeakr, overallscale);
	double lFreq = pow(*pointyGuitar->lspeakr, overallscale + 3.0);
	// begin Gate
	double onthreshold = (pow(*pointyGuitar->gate, 3) / 3) + 0.00018;
	double offthreshold = onthreshold * 1.1;
	double release = 0.028331119964586;
	double absmax = 220.9;
	// end Gate
	double output = *pointyGuitar->outputGain;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pointyGuitar->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pointyGuitar->fpdR * 1.18e-17;

		// begin Gate
		if (inputSampleL > 0.0) {
			if (pointyGuitar->WasNegativeL == true) pointyGuitar->ZeroCrossL = absmax * 0.3;
			pointyGuitar->WasNegativeL = false;
		} else {
			pointyGuitar->ZeroCrossL += 1;
			pointyGuitar->WasNegativeL = true;
		}
		if (pointyGuitar->ZeroCrossL > absmax) pointyGuitar->ZeroCrossL = absmax;
		if (pointyGuitar->gateL == 0.0) {
			// if gateL is totally silent
			if (fabs(inputSampleL) > onthreshold) {
				if (pointyGuitar->gaterollerL == 0.0) pointyGuitar->gaterollerL = pointyGuitar->ZeroCrossL;
				else pointyGuitar->gaterollerL -= release;
				// trigger from total silence only- if we're active then signal must clear offthreshold
			} else pointyGuitar->gaterollerL -= release;
		} else {
			// gateL is not silent but closing
			if (fabs(inputSampleL) > offthreshold) {
				if (pointyGuitar->gaterollerL < pointyGuitar->ZeroCrossL) pointyGuitar->gaterollerL = pointyGuitar->ZeroCrossL;
				else pointyGuitar->gaterollerL -= release;
				// always trigger if gateL is over offthreshold, otherwise close anyway
			} else pointyGuitar->gaterollerL -= release;
		}
		if (pointyGuitar->gaterollerL < 0.0) pointyGuitar->gaterollerL = 0.0;

		for (int x = 0; x < poles; x++) {
			double fr = 0.9 / overallscale;
			double band = inputSampleL;
			inputSampleL = 0.0;
			for (int y = 0; y < 9; y++) {
				pointyGuitar->angAL[x][y] = (pointyGuitar->angAL[x][y] * (1.0 - fr)) + ((band - pointyGuitar->angSL[x][y]) * fr);
				double temp = band;
				band = ((pointyGuitar->angSL[x][y] + (pointyGuitar->angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
				pointyGuitar->angSL[x][y] = ((pointyGuitar->angSL[x][y] + (pointyGuitar->angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
				inputSampleL += ((temp - band) * pointyGuitar->angG[y]);
				fr *= 0.618033988749894;
			}
			inputSampleL += (band * pointyGuitar->angG[9]);
			inputSampleL *= drive;
			inputSampleL -= fmin(fmax((inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654)), -1.0), 1.0);
		}

		if (pointyGuitar->gaterollerL < 1.0) {
			pointyGuitar->gateL = pointyGuitar->gaterollerL;
			double bridgerectifier = 1 - cos(fabs(inputSampleL));
			if (inputSampleL > 0) inputSampleL = (inputSampleL * pointyGuitar->gateL) + (bridgerectifier * (1.0 - pointyGuitar->gateL));
			else inputSampleL = (inputSampleL * pointyGuitar->gateL) - (bridgerectifier * (1.0 - pointyGuitar->gateL));
			if (pointyGuitar->gateL == 0.0) inputSampleL = 0.0;
		} else pointyGuitar->gateL = 1.0;
		// end Gate

		double lowSample = inputSampleL;
		for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
			pointyGuitar->iirBAngleL[count] = (pointyGuitar->iirBAngleL[count] * (1.0 - lFreq)) + ((lowSample - pointyGuitar->iirBPositionL[count]) * lFreq);
			lowSample = ((pointyGuitar->iirBPositionL[count] + (pointyGuitar->iirBAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
			pointyGuitar->iirBPositionL[count] = ((pointyGuitar->iirBPositionL[count] + (pointyGuitar->iirBAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
			inputSampleL -= (lowSample * (1.0 / (3.0 + (lFreq * 32.0))));
		}

		for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
			pointyGuitar->iirHAngleL[count] = (pointyGuitar->iirHAngleL[count] * (1.0 - hFreq)) + ((inputSampleL - pointyGuitar->iirHPositionL[count]) * hFreq);
			inputSampleL = ((pointyGuitar->iirHPositionL[count] + (pointyGuitar->iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
			pointyGuitar->iirHPositionL[count] = ((pointyGuitar->iirHPositionL[count] + (pointyGuitar->iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
		} // the lowpass
		inputSampleL *= output;

		// begin Gate
		if (inputSampleR > 0.0) {
			if (pointyGuitar->WasNegativeR == true) pointyGuitar->ZeroCrossR = absmax * 0.3;
			pointyGuitar->WasNegativeR = false;
		} else {
			pointyGuitar->ZeroCrossR += 1;
			pointyGuitar->WasNegativeR = true;
		}
		if (pointyGuitar->ZeroCrossR > absmax) pointyGuitar->ZeroCrossR = absmax;
		if (pointyGuitar->gateR == 0.0) {
			// if gateR is totally silent
			if (fabs(inputSampleR) > onthreshold) {
				if (pointyGuitar->gaterollerR == 0.0) pointyGuitar->gaterollerR = pointyGuitar->ZeroCrossR;
				else pointyGuitar->gaterollerR -= release;
				// trigger from total silence only- if we're active then signal must clear offthreshold
			} else pointyGuitar->gaterollerR -= release;
		} else {
			// gateR is not silent but closing
			if (fabs(inputSampleR) > offthreshold) {
				if (pointyGuitar->gaterollerR < pointyGuitar->ZeroCrossR) pointyGuitar->gaterollerR = pointyGuitar->ZeroCrossR;
				else pointyGuitar->gaterollerR -= release;
				// always trigger if gateR is over offthreshold, otherwise close anyway
			} else pointyGuitar->gaterollerR -= release;
		}
		if (pointyGuitar->gaterollerR < 0.0) pointyGuitar->gaterollerR = 0.0;

		for (int x = 0; x < poles; x++) {
			double fr = 0.9 / overallscale;
			double band = inputSampleR;
			inputSampleR = 0.0;
			for (int y = 0; y < 9; y++) {
				pointyGuitar->angAR[x][y] = (pointyGuitar->angAR[x][y] * (1.0 - fr)) + ((band - pointyGuitar->angSR[x][y]) * fr);
				double temp = band;
				band = ((pointyGuitar->angSR[x][y] + (pointyGuitar->angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
				pointyGuitar->angSR[x][y] = ((pointyGuitar->angSR[x][y] + (pointyGuitar->angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
				inputSampleR += ((temp - band) * pointyGuitar->angG[y]);
				fr *= 0.618033988749894;
			}
			inputSampleR += (band * pointyGuitar->angG[9]);
			inputSampleR *= drive;
			inputSampleR -= fmin(fmax((inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654)), -1.0), 1.0);
		}

		if (pointyGuitar->gaterollerR < 1.0) {
			pointyGuitar->gateR = pointyGuitar->gaterollerR;
			double bridgerectifier = 1 - cos(fabs(inputSampleR));
			if (inputSampleR > 0) inputSampleR = (inputSampleR * pointyGuitar->gateR) + (bridgerectifier * (1.0 - pointyGuitar->gateR));
			else inputSampleR = (inputSampleR * pointyGuitar->gateR) - (bridgerectifier * (1.0 - pointyGuitar->gateR));
			if (pointyGuitar->gateR == 0.0) inputSampleR = 0.0;
		} else pointyGuitar->gateR = 1.0;
		// end Gate

		lowSample = inputSampleR;
		for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
			pointyGuitar->iirBAngleR[count] = (pointyGuitar->iirBAngleR[count] * (1.0 - lFreq)) + ((lowSample - pointyGuitar->iirBPositionR[count]) * lFreq);
			lowSample = ((pointyGuitar->iirBPositionR[count] + (pointyGuitar->iirBAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
			pointyGuitar->iirBPositionR[count] = ((pointyGuitar->iirBPositionR[count] + (pointyGuitar->iirBAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
			inputSampleR -= (lowSample * (1.0 / (3.0 + (lFreq * 32.0))));
		}

		for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
			pointyGuitar->iirHAngleR[count] = (pointyGuitar->iirHAngleR[count] * (1.0 - hFreq)) + ((inputSampleR - pointyGuitar->iirHPositionR[count]) * hFreq);
			inputSampleR = ((pointyGuitar->iirHPositionR[count] + (pointyGuitar->iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
			pointyGuitar->iirHPositionR[count] = ((pointyGuitar->iirHPositionR[count] + (pointyGuitar->iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
		} // the lowpass
		inputSampleR *= output;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pointyGuitar->fpdL ^= pointyGuitar->fpdL << 13;
		pointyGuitar->fpdL ^= pointyGuitar->fpdL >> 17;
		pointyGuitar->fpdL ^= pointyGuitar->fpdL << 5;
		inputSampleL += (((double) pointyGuitar->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pointyGuitar->fpdR ^= pointyGuitar->fpdR << 13;
		pointyGuitar->fpdR ^= pointyGuitar->fpdR >> 17;
		pointyGuitar->fpdR ^= pointyGuitar->fpdR << 5;
		inputSampleR += (((double) pointyGuitar->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POINTYGUITAR_URI,
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
