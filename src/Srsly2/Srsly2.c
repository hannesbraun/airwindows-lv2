#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SRSLY2_URI "https://hannesbraun.net/ns/lv2/airwindows/srsly2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	CENTER = 4,
	SPACE = 5,
	LEVEL = 6,
	Q = 7,
	DRYWET = 8
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* center;
	const float* space;
	const float* level;
	const float* q;
	const float* dryWet;

	double biquadM2[11];
	double biquadM7[11];
	double biquadM10[11];

	double biquadL3[11];
	double biquadL7[11];
	double biquadR3[11];
	double biquadR7[11];

	double biquadS3[11];
	double biquadS5[11];

	uint32_t fpdL;
	uint32_t fpdR;
} Srsly2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Srsly2* srsly2 = (Srsly2*) calloc(1, sizeof(Srsly2));
	srsly2->sampleRate = rate;
	return (LV2_Handle) srsly2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Srsly2* srsly2 = (Srsly2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			srsly2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			srsly2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			srsly2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			srsly2->output[1] = (float*) data;
			break;
		case CENTER:
			srsly2->center = (const float*) data;
			break;
		case SPACE:
			srsly2->space = (const float*) data;
			break;
		case LEVEL:
			srsly2->level = (const float*) data;
			break;
		case Q:
			srsly2->q = (const float*) data;
			break;
		case DRYWET:
			srsly2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Srsly2* srsly2 = (Srsly2*) instance;

	for (int x = 0; x < 11; x++) {
		srsly2->biquadM2[x] = 0.0;
		srsly2->biquadM7[x] = 0.0;
		srsly2->biquadM10[x] = 0.0;
		srsly2->biquadL3[x] = 0.0;
		srsly2->biquadL7[x] = 0.0;
		srsly2->biquadR3[x] = 0.0;
		srsly2->biquadR7[x] = 0.0;
		srsly2->biquadS3[x] = 0.0;
		srsly2->biquadS5[x] = 0.0;
	}
	srsly2->fpdL = 1.0;
	while (srsly2->fpdL < 16386) srsly2->fpdL = rand() * UINT32_MAX;
	srsly2->fpdR = 1.0;
	while (srsly2->fpdR < 16386) srsly2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Srsly2* srsly2 = (Srsly2*) instance;

	const float* in1 = srsly2->input[0];
	const float* in2 = srsly2->input[1];
	float* out1 = srsly2->output[0];
	float* out2 = srsly2->output[1];

	double sampleRate = srsly2->sampleRate;
	if (sampleRate < 22000) sampleRate = 22000; //keep biquads in range
	double tempSample;

	srsly2->biquadM2[0] = 2000 / sampleRate; //up
	srsly2->biquadM7[0] = 7000 / sampleRate; //down
	srsly2->biquadM10[0] = 10000 / sampleRate; //down

	srsly2->biquadL3[0] = 3000 / sampleRate; //up
	srsly2->biquadL7[0] = 7000 / sampleRate; //way up
	srsly2->biquadR3[0] = 3000 / sampleRate; //up
	srsly2->biquadR7[0] = 7000 / sampleRate; //way up

	srsly2->biquadS3[0] = 3000 / sampleRate; //up
	srsly2->biquadS5[0] = 5000 / sampleRate; //way down

	double focusM = 15.0 - (*srsly2->center * 10.0);
	double focusS = 21.0 - (*srsly2->space * 15.0);
	double Q = *srsly2->q + 0.25; //add Q control: from half to double intensity
	double gainM = *srsly2->center * 2.0;
	double gainS = *srsly2->space * 2.0;
	if (gainS > 1.0) gainM /= gainS;
	if (gainM > 1.0) gainM = 1.0;

	srsly2->biquadM2[1] = focusM * 0.25 * Q; //Q, mid 2K boost is much broader
	srsly2->biquadM7[1] = focusM * Q; //Q
	srsly2->biquadM10[1] = focusM * Q; //Q
	srsly2->biquadS3[1] = focusM * Q; //Q
	srsly2->biquadS5[1] = focusM * Q; //Q

	srsly2->biquadL3[1] = focusS * Q; //Q
	srsly2->biquadL7[1] = focusS * Q; //Q
	srsly2->biquadR3[1] = focusS * Q; //Q
	srsly2->biquadR7[1] = focusS * Q; //Q

	double K = tan(M_PI * srsly2->biquadM2[0]);
	double norm = 1.0 / (1.0 + K / srsly2->biquadM2[1] + K * K);
	srsly2->biquadM2[2] = K / srsly2->biquadM2[1] * norm;
	srsly2->biquadM2[4] = -srsly2->biquadM2[2];
	srsly2->biquadM2[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadM2[6] = (1.0 - K / srsly2->biquadM2[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadM7[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadM7[1] + K * K);
	srsly2->biquadM7[2] = K / srsly2->biquadM7[1] * norm;
	srsly2->biquadM7[4] = -srsly2->biquadM7[2];
	srsly2->biquadM7[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadM7[6] = (1.0 - K / srsly2->biquadM7[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadM10[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadM10[1] + K * K);
	srsly2->biquadM10[2] = K / srsly2->biquadM10[1] * norm;
	srsly2->biquadM10[4] = -srsly2->biquadM10[2];
	srsly2->biquadM10[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadM10[6] = (1.0 - K / srsly2->biquadM10[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadL3[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadL3[1] + K * K);
	srsly2->biquadL3[2] = K / srsly2->biquadL3[1] * norm;
	srsly2->biquadL3[4] = -srsly2->biquadL3[2];
	srsly2->biquadL3[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadL3[6] = (1.0 - K / srsly2->biquadL3[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadL7[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadL7[1] + K * K);
	srsly2->biquadL7[2] = K / srsly2->biquadL7[1] * norm;
	srsly2->biquadL7[4] = -srsly2->biquadL7[2];
	srsly2->biquadL7[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadL7[6] = (1.0 - K / srsly2->biquadL7[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadR3[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadR3[1] + K * K);
	srsly2->biquadR3[2] = K / srsly2->biquadR3[1] * norm;
	srsly2->biquadR3[4] = -srsly2->biquadR3[2];
	srsly2->biquadR3[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadR3[6] = (1.0 - K / srsly2->biquadR3[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadR7[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadR7[1] + K * K);
	srsly2->biquadR7[2] = K / srsly2->biquadR7[1] * norm;
	srsly2->biquadR7[4] = -srsly2->biquadR7[2];
	srsly2->biquadR7[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadR7[6] = (1.0 - K / srsly2->biquadR7[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadS3[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadS3[1] + K * K);
	srsly2->biquadS3[2] = K / srsly2->biquadS3[1] * norm;
	srsly2->biquadS3[4] = -srsly2->biquadS3[2];
	srsly2->biquadS3[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadS3[6] = (1.0 - K / srsly2->biquadS3[1] + K * K) * norm;

	K = tan(M_PI * srsly2->biquadS5[0]);
	norm = 1.0 / (1.0 + K / srsly2->biquadS5[1] + K * K);
	srsly2->biquadS5[2] = K / srsly2->biquadS5[1] * norm;
	srsly2->biquadS5[4] = -srsly2->biquadS5[2];
	srsly2->biquadS5[5] = 2.0 * (K * K - 1.0) * norm;
	srsly2->biquadS5[6] = (1.0 - K / srsly2->biquadS5[1] + K * K) * norm;

	double depthM = pow(*srsly2->center, 2) * 2.0;; //proportion to mix in the filtered stuff
	double depthS = pow(*srsly2->space, 2) * 2.0;; //proportion to mix in the filtered stuff
	double level = *srsly2->level; //output pad
	double wet = *srsly2->dryWet; //dry/wet

	//biquad contains these values:
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	//[2] is a0 but you need distinct ones for additional biquad instances so it's here
	//[3] is a1 but you need distinct ones for additional biquad instances so it's here
	//[4] is a2 but you need distinct ones for additional biquad instances so it's here
	//[5] is b1 but you need distinct ones for additional biquad instances so it's here
	//[6] is b2 but you need distinct ones for additional biquad instances so it's here
	//[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
	//[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = srsly2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = srsly2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		//encode Console5: good cleanness

		double mid = inputSampleL + inputSampleR;
		double rawmid = mid * 0.5; //we'll use this to isolate L&R a little
		double side = inputSampleL - inputSampleR;
		double boostside = side * depthS;
		//assign mid and side.Between these sections, you can do mid/side processing

		tempSample = (mid * srsly2->biquadM2[2]) + srsly2->biquadM2[7];
		srsly2->biquadM2[7] = (-tempSample * srsly2->biquadM2[5]) + srsly2->biquadM2[8];
		srsly2->biquadM2[8] = (mid * srsly2->biquadM2[4]) - (tempSample * srsly2->biquadM2[6]);
		double M2Sample = tempSample; //like mono AU, 7 and 8 store L channel

		tempSample = (mid * srsly2->biquadM7[2]) + srsly2->biquadM7[7];
		srsly2->biquadM7[7] = (-tempSample * srsly2->biquadM7[5]) + srsly2->biquadM7[8];
		srsly2->biquadM7[8] = (mid * srsly2->biquadM7[4]) - (tempSample * srsly2->biquadM7[6]);
		double M7Sample = -tempSample * 2.0; //like mono AU, 7 and 8 store L channel

		tempSample = (mid * srsly2->biquadM10[2]) + srsly2->biquadM10[7];
		srsly2->biquadM10[7] = (-tempSample * srsly2->biquadM10[5]) + srsly2->biquadM10[8];
		srsly2->biquadM10[8] = (mid * srsly2->biquadM10[4]) - (tempSample * srsly2->biquadM10[6]);
		double M10Sample = -tempSample * 2.0; //like mono AU, 7 and 8 store L channel
		//mid

		tempSample = (side * srsly2->biquadS3[2]) + srsly2->biquadS3[7];
		srsly2->biquadS3[7] = (-tempSample * srsly2->biquadS3[5]) + srsly2->biquadS3[8];
		srsly2->biquadS3[8] = (side * srsly2->biquadS3[4]) - (tempSample * srsly2->biquadS3[6]);
		double S3Sample = tempSample * 2.0; //like mono AU, 7 and 8 store L channel

		tempSample = (side * srsly2->biquadS5[2]) + srsly2->biquadS5[7];
		srsly2->biquadS5[7] = (-tempSample * srsly2->biquadS5[5]) + srsly2->biquadS5[8];
		srsly2->biquadS5[8] = (side * srsly2->biquadS5[4]) - (tempSample * srsly2->biquadS5[6]);
		double S5Sample = -tempSample * 5.0; //like mono AU, 7 and 8 store L channel

		mid = (M2Sample + M7Sample + M10Sample) * depthM;
		side = (S3Sample + S5Sample + boostside) * depthS;

		double msOutSampleL = (mid + side) / 2.0;
		double msOutSampleR = (mid - side) / 2.0;
		//unassign mid and side

		double isoSampleL = inputSampleL - rawmid;
		double isoSampleR = inputSampleR - rawmid; //trying to isolate L and R a little

		tempSample = (isoSampleL * srsly2->biquadL3[2]) + srsly2->biquadL3[7];
		srsly2->biquadL3[7] = (-tempSample * srsly2->biquadL3[5]) + srsly2->biquadL3[8];
		srsly2->biquadL3[8] = (isoSampleL * srsly2->biquadL3[4]) - (tempSample * srsly2->biquadL3[6]);
		double L3Sample = tempSample; //like mono AU, 7 and 8 store L channel

		tempSample = (isoSampleR * srsly2->biquadR3[2]) + srsly2->biquadR3[9];
		srsly2->biquadR3[9] = (-tempSample * srsly2->biquadR3[5]) + srsly2->biquadR3[10];
		srsly2->biquadR3[10] = (isoSampleR * srsly2->biquadR3[4]) - (tempSample * srsly2->biquadR3[6]);
		double R3Sample = tempSample; //note: 9 and 10 store the R channel

		tempSample = (isoSampleL * srsly2->biquadL7[2]) + srsly2->biquadL7[7];
		srsly2->biquadL7[7] = (-tempSample * srsly2->biquadL7[5]) + srsly2->biquadL7[8];
		srsly2->biquadL7[8] = (isoSampleL * srsly2->biquadL7[4]) - (tempSample * srsly2->biquadL7[6]);
		double L7Sample = tempSample * 3.0; //like mono AU, 7 and 8 store L channel

		tempSample = (isoSampleR * srsly2->biquadR7[2]) + srsly2->biquadR7[9];
		srsly2->biquadR7[9] = (-tempSample * srsly2->biquadR7[5]) + srsly2->biquadR7[10];
		srsly2->biquadR7[10] = (isoSampleR * srsly2->biquadR7[4]) - (tempSample * srsly2->biquadR7[6]);
		double R7Sample = tempSample * 3.0; //note: 9 and 10 store the R channel

		double processingL = msOutSampleL + ((L3Sample + L7Sample) * depthS);
		double processingR = msOutSampleR + ((R3Sample + R7Sample) * depthS);
		//done with making filters, now we apply them

		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;
		//re-assign mid and side.Between these sections, you can do mid/side processing

		mid *= gainM;
		side *= gainS;
		//we crank things up more than a bit, or cut them in line with how the hardware box works
		if (side > 1.57079633) side = 1.57079633;
		if (side < -1.57079633) side = -1.57079633;
		side = sin(side);
		side *= gainS;


		inputSampleL = ((mid + side) / 2.0) + processingL;
		inputSampleR = ((mid - side) / 2.0) + processingR;
		//unassign mid and side

		if (level < 1.0) {
			inputSampleL *= level;
			inputSampleR *= level;
		}

		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		//amplitude aspect

		if (wet < 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		srsly2->fpdL ^= srsly2->fpdL << 13;
		srsly2->fpdL ^= srsly2->fpdL >> 17;
		srsly2->fpdL ^= srsly2->fpdL << 5;
		inputSampleL += (((double)srsly2->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		srsly2->fpdR ^= srsly2->fpdR << 13;
		srsly2->fpdR ^= srsly2->fpdR >> 17;
		srsly2->fpdR ^= srsly2->fpdR << 5;
		inputSampleR += (((double)srsly2->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	SRSLY2_URI,
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
