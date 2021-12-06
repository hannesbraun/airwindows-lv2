#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ACCELERATION_URI "https://hannesbraun.net/ns/lv2/airwindows/acceleration"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	LIMIT = 4,
	DRYWET = 5
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* limit;
	const float* drywet;

	long double fpNShapeL;
	long double fpNShapeR;
	//default stuff
	double ataLastOutL;
	double s1L;
	double s2L;
	double s3L;
	double o1L;
	double o2L;
	double o3L;
	double m1L;
	double m2L;
	double desL;

	double ataLastOutR;
	double s1R;
	double s2R;
	double s3R;
	double o1R;
	double o2R;
	double o3R;
	double m1R;
	double m2R;
	double desR;
} Acceleration;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Acceleration* acceleration = (Acceleration*) calloc(1, sizeof(Acceleration));
	acceleration->sampleRate = rate;
	return (LV2_Handle) acceleration;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Acceleration* acceleration = (Acceleration*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			acceleration->input[0] = (const float*) data;
			break;
		case INPUT_R:
			acceleration->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			acceleration->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			acceleration->output[1] = (float*) data;
			break;
		case LIMIT:
			acceleration->limit = (const float*) data;
			break;
		case DRYWET:
			acceleration->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Acceleration* acceleration = (Acceleration*) instance;
	acceleration->ataLastOutL = 0.0;
	acceleration->s1L = 0.0;
	acceleration->s2L = 0.0;
	acceleration->s3L = 0.0;
	acceleration->o1L = 0.0;
	acceleration->o2L = 0.0;
	acceleration->o3L = 0.0;
	acceleration->m1L = 0.0;
	acceleration->m2L = 0.0;
	acceleration->desL = 0.0;
	acceleration->ataLastOutR = 0.0;
	acceleration->s1R = 0.0;
	acceleration->s2R = 0.0;
	acceleration->s3R = 0.0;
	acceleration->o1R = 0.0;
	acceleration->o2R = 0.0;
	acceleration->o3R = 0.0;
	acceleration->m1R = 0.0;
	acceleration->m2R = 0.0;
	acceleration->desR = 0.0;

	acceleration->fpNShapeL = 0.0;
	acceleration->fpNShapeR = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Acceleration* acceleration = (Acceleration*) instance;

	const float* in1 = acceleration->input[0];
	const float* in2 = acceleration->input[1];
	float* out1 = acceleration->output[0];
	float* out2 = acceleration->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= acceleration->sampleRate;

	double intensity = pow(*acceleration->limit, 3) * (32 / overallscale);
	double wet = *acceleration->drywet;
	double dry = 1.0 - wet;

	double senseL;
	double smoothL;
	double senseR;
	double smoothR;
	double accumulatorSample;
	double drySampleL;
	double drySampleR;
	long double inputSampleL;
	long double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
			static int noisesource = 0;
			//this declares a variable before anything else is compiled. It won't keep assigning
			//it to 0 for every sample, it's as if the declaration doesn't exist in this context,
			//but it lets me add this denormalization fix in a single place rather than updating
			//it in three different locations. The variable isn't thread-safe but this is only
			//a random seed and we can share it with whatever.
			noisesource = noisesource % 1700021;
			noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003;
			residue *= residue;
			residue = residue % 17011;
			residue *= residue;
			residue = residue % 1709;
			residue *= residue;
			residue = residue % 173;
			residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleL = applyresidue;
		}
		if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
			static int noisesource = 0;
			noisesource = noisesource % 1700021;
			noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003;
			residue *= residue;
			residue = residue % 17011;
			residue *= residue;
			residue = residue % 1709;
			residue *= residue;
			residue = residue % 173;
			residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleR = applyresidue;
			//this denormalization routine produces a white noise at -300 dB which the noise
			//shaping will interact with to produce a bipolar output, but the noise is actually
			//all positive. That should stop any variables from going denormal, and the routine
			//only kicks in if digital black is input. As a final touch, if you save to 24-bit
			//the silence will return to being digital black again.
		}
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		acceleration->s3L = acceleration->s2L;
		acceleration->s2L = acceleration->s1L;
		acceleration->s1L = inputSampleL;
		smoothL = (acceleration->s3L + acceleration->s2L + acceleration->s1L) / 3.0;
		acceleration->m1L = (acceleration->s1L - acceleration->s2L) * ((acceleration->s1L - acceleration->s2L) / 1.3);
		acceleration->m2L = (acceleration->s2L - acceleration->s3L) * ((acceleration->s1L - acceleration->s2L) / 1.3);
		senseL = fabs(acceleration->m1L - acceleration->m2L);
		senseL = (intensity * intensity * senseL);
		acceleration->o3L = acceleration->o2L;
		acceleration->o2L = acceleration->o1L;
		acceleration->o1L = senseL;
		if (acceleration->o2L > senseL) senseL = acceleration->o2L;
		if (acceleration->o3L > senseL) senseL = acceleration->o3L;
		//sense on the most intense

		acceleration->s3R = acceleration->s2R;
		acceleration->s2R = acceleration->s1R;
		acceleration->s1R = inputSampleR;
		smoothR = (acceleration->s3R + acceleration->s2R + acceleration->s1R) / 3.0;
		acceleration->m1R = (acceleration->s1R - acceleration->s2R) * ((acceleration->s1R - acceleration->s2R) / 1.3);
		acceleration->m2R = (acceleration->s2R - acceleration->s3R) * ((acceleration->s1R - acceleration->s2R) / 1.3);
		senseR = fabs(acceleration->m1R - acceleration->m2R);
		senseR = (intensity * intensity * senseR);
		acceleration->o3R = acceleration->o2R;
		acceleration->o2R = acceleration->o1R;
		acceleration->o1R = senseR;
		if (acceleration->o2R > senseR) senseR = acceleration->o2R;
		if (acceleration->o3R > senseR) senseR = acceleration->o3R;
		//sense on the most intense

		if (senseL > 1.0) senseL = 1.0;
		if (senseR > 1.0) senseR = 1.0;

		inputSampleL *= (1.0 - senseL);
		inputSampleR *= (1.0 - senseR);

		inputSampleL += (smoothL * senseL);
		inputSampleR += (smoothR * senseR);

		senseL /= 2.0;
		senseR /= 2.0;

		accumulatorSample = (acceleration->ataLastOutL * senseL) + (inputSampleL * (1.0 - senseL));
		acceleration->ataLastOutL = inputSampleL;
		inputSampleL = accumulatorSample;

		accumulatorSample = (acceleration->ataLastOutR * senseR) + (inputSampleR * (1.0 - senseR));
		acceleration->ataLastOutR = inputSampleR;
		inputSampleR = accumulatorSample;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * dry);
			inputSampleR = (inputSampleR * wet) + (drySampleR * dry);
		}

		//stereo 32 bit dither, made small and tidy.
		int expon;
		frexpf((float)inputSampleL, &expon);
		long double dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
		inputSampleL += (dither - acceleration->fpNShapeL);
		acceleration->fpNShapeL = dither;
		frexpf((float)inputSampleR, &expon);
		dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
		inputSampleR += (dither - acceleration->fpNShapeR);
		acceleration->fpNShapeR = dither;
		//end 32 bit dither

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
	ACCELERATION_URI,
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
