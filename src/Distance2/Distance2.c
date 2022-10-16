#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define DISTANCE2_URI "https://hannesbraun.net/ns/lv2/airwindows/distance2"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	ATMOSPHERE = 4,
	DARKEN = 5,
	DRY_WET = 6
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* atmosphere;
	const float* darken;
	const float* dryWet;

	uint32_t fpdL;
	uint32_t fpdR;
	// default stuff

	double lastSampleAL;
	double lastSampleBL;
	double lastSampleCL;
	double lastSampleDL;
	double lastSampleEL;
	double lastSampleFL;
	double lastSampleGL;
	double lastSampleHL;
	double lastSampleIL;
	double lastSampleJL;
	double lastSampleKL;
	double lastSampleLL;
	double lastSampleML;

	double lastSampleAR;
	double lastSampleBR;
	double lastSampleCR;
	double lastSampleDR;
	double lastSampleER;
	double lastSampleFR;
	double lastSampleGR;
	double lastSampleHR;
	double lastSampleIR;
	double lastSampleJR;
	double lastSampleKR;
	double lastSampleLR;
	double lastSampleMR;

	double thresholdA;
	double thresholdB;
	double thresholdC;
	double thresholdD;
	double thresholdE;
	double thresholdF;
	double thresholdG;
	double thresholdH;
	double thresholdI;
	double thresholdJ;
	double thresholdK;
	double thresholdL;
	double thresholdM;

	double thirdSampleL;
	double lastSampleL;

	double thirdSampleR;
	double lastSampleR;
} Distance2;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Distance2* distance2 = (Distance2*) calloc(1, sizeof(Distance2));
	distance2->sampleRate = rate;
	return (LV2_Handle) distance2;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Distance2* distance2 = (Distance2*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			distance2->input[0] = (const float*) data;
			break;
		case INPUT_R:
			distance2->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			distance2->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			distance2->output[1] = (float*) data;
			break;
		case ATMOSPHERE:
			distance2->atmosphere = (const float*) data;
			break;
		case DARKEN:
			distance2->darken = (const float*) data;
			break;
		case DRY_WET:
			distance2->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Distance2* distance2 = (Distance2*) instance;

	distance2->thirdSampleL = 0.0;
	distance2->lastSampleL = 0.0;
	distance2->thirdSampleR = 0.0;
	distance2->lastSampleR = 0.0;

	distance2->lastSampleAL = 0.0;
	distance2->lastSampleBL = 0.0;
	distance2->lastSampleCL = 0.0;
	distance2->lastSampleDL = 0.0;
	distance2->lastSampleEL = 0.0;
	distance2->lastSampleFL = 0.0;
	distance2->lastSampleGL = 0.0;
	distance2->lastSampleHL = 0.0;
	distance2->lastSampleIL = 0.0;
	distance2->lastSampleJL = 0.0;
	distance2->lastSampleKL = 0.0;
	distance2->lastSampleLL = 0.0;
	distance2->lastSampleML = 0.0;

	distance2->lastSampleAR = 0.0;
	distance2->lastSampleBR = 0.0;
	distance2->lastSampleCR = 0.0;
	distance2->lastSampleDR = 0.0;
	distance2->lastSampleER = 0.0;
	distance2->lastSampleFR = 0.0;
	distance2->lastSampleGR = 0.0;
	distance2->lastSampleHR = 0.0;
	distance2->lastSampleIR = 0.0;
	distance2->lastSampleJR = 0.0;
	distance2->lastSampleKR = 0.0;
	distance2->lastSampleLR = 0.0;
	distance2->lastSampleMR = 0.0;

	distance2->thresholdA = 0.618033988749894;
	distance2->thresholdB = 0.679837387624884;
	distance2->thresholdC = 0.747821126387373;
	distance2->thresholdD = 0.82260323902611;
	distance2->thresholdE = 0.904863562928721;
	distance2->thresholdF = 0.995349919221593;
	distance2->thresholdG = 1.094884911143752;
	distance2->thresholdH = 1.204373402258128;
	distance2->thresholdI = 1.32481074248394;
	distance2->thresholdJ = 1.457291816732335;
	distance2->thresholdK = 1.603020998405568;
	distance2->thresholdL = 1.763323098246125;
	distance2->thresholdM = 1.939655408070737;

	distance2->fpdL = 1.0;
	while (distance2->fpdL < 16386) distance2->fpdL = rand() * UINT32_MAX;
	distance2->fpdR = 1.0;
	while (distance2->fpdR < 16386) distance2->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Distance2* distance2 = (Distance2*) instance;

	const float* in1 = distance2->input[0];
	const float* in2 = distance2->input[1];
	float* out1 = distance2->output[0];
	float* out2 = distance2->output[1];

	const float atmosphere = *distance2->atmosphere;
	const float darken = *distance2->darken;

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= distance2->sampleRate;
	distance2->thresholdA = 0.618033988749894 / overallscale;
	distance2->thresholdB = 0.679837387624884 / overallscale;
	distance2->thresholdC = 0.747821126387373 / overallscale;
	distance2->thresholdD = 0.82260323902611 / overallscale;
	distance2->thresholdE = 0.904863562928721 / overallscale;
	distance2->thresholdF = 0.995349919221593 / overallscale;
	distance2->thresholdG = 1.094884911143752 / overallscale;
	distance2->thresholdH = 1.204373402258128 / overallscale;
	distance2->thresholdI = 1.32481074248394 / overallscale;
	distance2->thresholdJ = 1.457291816732335 / overallscale;
	distance2->thresholdK = 1.603020998405568 / overallscale;
	distance2->thresholdL = 1.763323098246125 / overallscale;
	distance2->thresholdM = 1.939655408070737 / overallscale;
	double softslew = (pow(atmosphere, 3) * 24) + .6;
	softslew *= overallscale;
	double filter = softslew * darken;
	double secondfilter = filter / 3.0;
	double thirdfilter = filter / 5.0;
	double offsetScale = atmosphere * 0.1618;
	double levelcorrect = 1.0 + ((filter / 12.0) * atmosphere);
	// bring in top slider again to manage boost level for lower settings
	double wet = *distance2->dryWet;
	// removed extra dry variable

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = distance2->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = distance2->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double offsetL = offsetScale - (distance2->lastSampleL - inputSampleL);
		double offsetR = offsetScale - (distance2->lastSampleR - inputSampleR);

		inputSampleL += (offsetL * offsetScale); // extra bit from Loud: offset air compression
		inputSampleL *= wet; // clean up w. dry introduced
		inputSampleL *= softslew; // scale into Atmosphere algorithm

		inputSampleR += (offsetR * offsetScale); // extra bit from Loud: offset air compression
		inputSampleR *= wet; // clean up w. dry introduced
		inputSampleR *= softslew; // scale into Atmosphere algorithm

		// left
		double clamp = inputSampleL - distance2->lastSampleAL;
		if (clamp > distance2->thresholdA) inputSampleL = distance2->lastSampleAL + distance2->thresholdA;
		if (-clamp > distance2->thresholdA) inputSampleL = distance2->lastSampleAL - distance2->thresholdA;

		clamp = inputSampleL - distance2->lastSampleBL;
		if (clamp > distance2->thresholdB) inputSampleL = distance2->lastSampleBL + distance2->thresholdB;
		if (-clamp > distance2->thresholdB) inputSampleL = distance2->lastSampleBL - distance2->thresholdB;

		clamp = inputSampleL - distance2->lastSampleCL;
		if (clamp > distance2->thresholdC) inputSampleL = distance2->lastSampleCL + distance2->thresholdC;
		if (-clamp > distance2->thresholdC) inputSampleL = distance2->lastSampleCL - distance2->thresholdC;

		clamp = inputSampleL - distance2->lastSampleDL;
		if (clamp > distance2->thresholdD) inputSampleL = distance2->lastSampleDL + distance2->thresholdD;
		if (-clamp > distance2->thresholdD) inputSampleL = distance2->lastSampleDL - distance2->thresholdD;

		clamp = inputSampleL - distance2->lastSampleEL;
		if (clamp > distance2->thresholdE) inputSampleL = distance2->lastSampleEL + distance2->thresholdE;
		if (-clamp > distance2->thresholdE) inputSampleL = distance2->lastSampleEL - distance2->thresholdE;

		clamp = inputSampleL - distance2->lastSampleFL;
		if (clamp > distance2->thresholdF) inputSampleL = distance2->lastSampleFL + distance2->thresholdF;
		if (-clamp > distance2->thresholdF) inputSampleL = distance2->lastSampleFL - distance2->thresholdF;

		clamp = inputSampleL - distance2->lastSampleGL;
		if (clamp > distance2->thresholdG) inputSampleL = distance2->lastSampleGL + distance2->thresholdG;
		if (-clamp > distance2->thresholdG) inputSampleL = distance2->lastSampleGL - distance2->thresholdG;

		clamp = inputSampleL - distance2->lastSampleHL;
		if (clamp > distance2->thresholdH) inputSampleL = distance2->lastSampleHL + distance2->thresholdH;
		if (-clamp > distance2->thresholdH) inputSampleL = distance2->lastSampleHL - distance2->thresholdH;

		clamp = inputSampleL - distance2->lastSampleIL;
		if (clamp > distance2->thresholdI) inputSampleL = distance2->lastSampleIL + distance2->thresholdI;
		if (-clamp > distance2->thresholdI) inputSampleL = distance2->lastSampleIL - distance2->thresholdI;

		clamp = inputSampleL - distance2->lastSampleJL;
		if (clamp > distance2->thresholdJ) inputSampleL = distance2->lastSampleJL + distance2->thresholdJ;
		if (-clamp > distance2->thresholdJ) inputSampleL = distance2->lastSampleJL - distance2->thresholdJ;

		clamp = inputSampleL - distance2->lastSampleKL;
		if (clamp > distance2->thresholdK) inputSampleL = distance2->lastSampleKL + distance2->thresholdK;
		if (-clamp > distance2->thresholdK) inputSampleL = distance2->lastSampleKL - distance2->thresholdK;

		clamp = inputSampleL - distance2->lastSampleLL;
		if (clamp > distance2->thresholdL) inputSampleL = distance2->lastSampleLL + distance2->thresholdL;
		if (-clamp > distance2->thresholdL) inputSampleL = distance2->lastSampleLL - distance2->thresholdL;

		clamp = inputSampleL - distance2->lastSampleML;
		if (clamp > distance2->thresholdM) inputSampleL = distance2->lastSampleML + distance2->thresholdM;
		if (-clamp > distance2->thresholdM) inputSampleL = distance2->lastSampleML - distance2->thresholdM;

		// right
		clamp = inputSampleR - distance2->lastSampleAR;
		if (clamp > distance2->thresholdA) inputSampleR = distance2->lastSampleAR + distance2->thresholdA;
		if (-clamp > distance2->thresholdA) inputSampleR = distance2->lastSampleAR - distance2->thresholdA;

		clamp = inputSampleR - distance2->lastSampleBR;
		if (clamp > distance2->thresholdB) inputSampleR = distance2->lastSampleBR + distance2->thresholdB;
		if (-clamp > distance2->thresholdB) inputSampleR = distance2->lastSampleBR - distance2->thresholdB;

		clamp = inputSampleR - distance2->lastSampleCR;
		if (clamp > distance2->thresholdC) inputSampleR = distance2->lastSampleCR + distance2->thresholdC;
		if (-clamp > distance2->thresholdC) inputSampleR = distance2->lastSampleCR - distance2->thresholdC;

		clamp = inputSampleR - distance2->lastSampleDR;
		if (clamp > distance2->thresholdD) inputSampleR = distance2->lastSampleDR + distance2->thresholdD;
		if (-clamp > distance2->thresholdD) inputSampleR = distance2->lastSampleDR - distance2->thresholdD;

		clamp = inputSampleR - distance2->lastSampleER;
		if (clamp > distance2->thresholdE) inputSampleR = distance2->lastSampleER + distance2->thresholdE;
		if (-clamp > distance2->thresholdE) inputSampleR = distance2->lastSampleER - distance2->thresholdE;

		clamp = inputSampleR - distance2->lastSampleFR;
		if (clamp > distance2->thresholdF) inputSampleR = distance2->lastSampleFR + distance2->thresholdF;
		if (-clamp > distance2->thresholdF) inputSampleR = distance2->lastSampleFR - distance2->thresholdF;

		clamp = inputSampleR - distance2->lastSampleGR;
		if (clamp > distance2->thresholdG) inputSampleR = distance2->lastSampleGR + distance2->thresholdG;
		if (-clamp > distance2->thresholdG) inputSampleR = distance2->lastSampleGR - distance2->thresholdG;

		clamp = inputSampleR - distance2->lastSampleHR;
		if (clamp > distance2->thresholdH) inputSampleR = distance2->lastSampleHR + distance2->thresholdH;
		if (-clamp > distance2->thresholdH) inputSampleR = distance2->lastSampleHR - distance2->thresholdH;

		clamp = inputSampleR - distance2->lastSampleIR;
		if (clamp > distance2->thresholdI) inputSampleR = distance2->lastSampleIR + distance2->thresholdI;
		if (-clamp > distance2->thresholdI) inputSampleR = distance2->lastSampleIR - distance2->thresholdI;

		clamp = inputSampleR - distance2->lastSampleJR;
		if (clamp > distance2->thresholdJ) inputSampleR = distance2->lastSampleJR + distance2->thresholdJ;
		if (-clamp > distance2->thresholdJ) inputSampleR = distance2->lastSampleJR - distance2->thresholdJ;

		clamp = inputSampleR - distance2->lastSampleKR;
		if (clamp > distance2->thresholdK) inputSampleR = distance2->lastSampleKR + distance2->thresholdK;
		if (-clamp > distance2->thresholdK) inputSampleR = distance2->lastSampleKR - distance2->thresholdK;

		clamp = inputSampleR - distance2->lastSampleLR;
		if (clamp > distance2->thresholdL) inputSampleR = distance2->lastSampleLR + distance2->thresholdL;
		if (-clamp > distance2->thresholdL) inputSampleR = distance2->lastSampleLR - distance2->thresholdL;

		clamp = inputSampleR - distance2->lastSampleMR;
		if (clamp > distance2->thresholdM) inputSampleR = distance2->lastSampleMR + distance2->thresholdM;
		if (-clamp > distance2->thresholdM) inputSampleR = distance2->lastSampleMR - distance2->thresholdM;

		distance2->lastSampleML = distance2->lastSampleLL;
		distance2->lastSampleLL = distance2->lastSampleKL;
		distance2->lastSampleKL = distance2->lastSampleJL;
		distance2->lastSampleJL = distance2->lastSampleIL;
		distance2->lastSampleIL = distance2->lastSampleHL;
		distance2->lastSampleHL = distance2->lastSampleGL;
		distance2->lastSampleGL = distance2->lastSampleFL;
		distance2->lastSampleFL = distance2->lastSampleEL;
		distance2->lastSampleEL = distance2->lastSampleDL;
		distance2->lastSampleDL = distance2->lastSampleCL;
		distance2->lastSampleCL = distance2->lastSampleBL;
		distance2->lastSampleBL = distance2->lastSampleAL;
		distance2->lastSampleAL = drySampleL;
		// store the raw input sample again for use next time

		distance2->lastSampleMR = distance2->lastSampleLR;
		distance2->lastSampleLR = distance2->lastSampleKR;
		distance2->lastSampleKR = distance2->lastSampleJR;
		distance2->lastSampleJR = distance2->lastSampleIR;
		distance2->lastSampleIR = distance2->lastSampleHR;
		distance2->lastSampleHR = distance2->lastSampleGR;
		distance2->lastSampleGR = distance2->lastSampleFR;
		distance2->lastSampleFR = distance2->lastSampleER;
		distance2->lastSampleER = distance2->lastSampleDR;
		distance2->lastSampleDR = distance2->lastSampleCR;
		distance2->lastSampleCR = distance2->lastSampleBR;
		distance2->lastSampleBR = distance2->lastSampleAR;
		distance2->lastSampleAR = drySampleR;
		// store the raw input sample again for use next time

		inputSampleL *= levelcorrect;
		inputSampleL /= softslew;
		inputSampleL -= (offsetL * offsetScale);
		// begin IIR stage
		inputSampleR *= levelcorrect;
		inputSampleR /= softslew;
		inputSampleR -= (offsetR * offsetScale);
		// begin IIR stage

		inputSampleL += (distance2->thirdSampleL * thirdfilter);
		inputSampleL /= (thirdfilter + 1.0);
		inputSampleL += (distance2->lastSampleL * secondfilter);
		inputSampleL /= (secondfilter + 1.0);
		// do an IIR like thing to further squish superdistant stuff
		inputSampleR += (distance2->thirdSampleR * thirdfilter);
		inputSampleR /= (thirdfilter + 1.0);
		inputSampleR += (distance2->lastSampleR * secondfilter);
		inputSampleR /= (secondfilter + 1.0);
		// do an IIR like thing to further squish superdistant stuff

		distance2->thirdSampleL = distance2->lastSampleL;
		distance2->lastSampleL = inputSampleL;
		inputSampleL *= levelcorrect;

		distance2->thirdSampleR = distance2->lastSampleR;
		distance2->lastSampleR = inputSampleR;
		inputSampleR *= levelcorrect;

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		distance2->fpdL ^= distance2->fpdL << 13;
		distance2->fpdL ^= distance2->fpdL >> 17;
		distance2->fpdL ^= distance2->fpdL << 5;
		inputSampleL += (((double) distance2->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		distance2->fpdR ^= distance2->fpdR << 13;
		distance2->fpdR ^= distance2->fpdR >> 17;
		distance2->fpdR ^= distance2->fpdR << 5;
		inputSampleR += (((double) distance2->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	DISTANCE2_URI,
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
