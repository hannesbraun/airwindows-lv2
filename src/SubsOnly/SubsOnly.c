#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SUBSONLY_URI "https://hannesbraun.net/ns/lv2/airwindows/subsonly"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];

	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double iirSampleIL;
	double iirSampleJL;
	double iirSampleKL;
	double iirSampleLL;
	double iirSampleML;
	double iirSampleNL;
	double iirSampleOL;
	double iirSamplePL;
	double iirSampleQL;
	double iirSampleRL;
	double iirSampleSL;
	double iirSampleTL;
	double iirSampleUL;
	double iirSampleVL;
	double iirSampleWL;
	double iirSampleXL;
	double iirSampleYL;
	double iirSampleZL;

	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double iirSampleFR;
	double iirSampleGR;
	double iirSampleHR;
	double iirSampleIR;
	double iirSampleJR;
	double iirSampleKR;
	double iirSampleLR;
	double iirSampleMR;
	double iirSampleNR;
	double iirSampleOR;
	double iirSamplePR;
	double iirSampleQR;
	double iirSampleRR;
	double iirSampleSR;
	double iirSampleTR;
	double iirSampleUR;
	double iirSampleVR;
	double iirSampleWR;
	double iirSampleXR;
	double iirSampleYR;
	double iirSampleZR;

	uint32_t fpdL;
	uint32_t fpdR;
} SubsOnly;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	SubsOnly* subsonly = (SubsOnly*) calloc(1, sizeof(SubsOnly));
	subsonly->sampleRate = rate;
	return (LV2_Handle) subsonly;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	SubsOnly* subsonly = (SubsOnly*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			subsonly->input[0] = (const float*) data;
			break;
		case INPUT_R:
			subsonly->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			subsonly->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			subsonly->output[1] = (float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	SubsOnly* subsonly = (SubsOnly*) instance;
	subsonly->iirSampleAL = 0.0;
	subsonly->iirSampleBL = 0.0;
	subsonly->iirSampleCL = 0.0;
	subsonly->iirSampleDL = 0.0;
	subsonly->iirSampleEL = 0.0;
	subsonly->iirSampleFL = 0.0;
	subsonly->iirSampleGL = 0.0;
	subsonly->iirSampleHL = 0.0;
	subsonly->iirSampleIL = 0.0;
	subsonly->iirSampleJL = 0.0;
	subsonly->iirSampleKL = 0.0;
	subsonly->iirSampleLL = 0.0;
	subsonly->iirSampleML = 0.0;
	subsonly->iirSampleNL = 0.0;
	subsonly->iirSampleOL = 0.0;
	subsonly->iirSamplePL = 0.0;
	subsonly->iirSampleQL = 0.0;
	subsonly->iirSampleRL = 0.0;
	subsonly->iirSampleSL = 0.0;
	subsonly->iirSampleTL = 0.0;
	subsonly->iirSampleUL = 0.0;
	subsonly->iirSampleVL = 0.0;
	subsonly->iirSampleWL = 0.0;
	subsonly->iirSampleXL = 0.0;
	subsonly->iirSampleYL = 0.0;
	subsonly->iirSampleZL = 0.0;

	subsonly->iirSampleAR = 0.0;
	subsonly->iirSampleBR = 0.0;
	subsonly->iirSampleCR = 0.0;
	subsonly->iirSampleDR = 0.0;
	subsonly->iirSampleER = 0.0;
	subsonly->iirSampleFR = 0.0;
	subsonly->iirSampleGR = 0.0;
	subsonly->iirSampleHR = 0.0;
	subsonly->iirSampleIR = 0.0;
	subsonly->iirSampleJR = 0.0;
	subsonly->iirSampleKR = 0.0;
	subsonly->iirSampleLR = 0.0;
	subsonly->iirSampleMR = 0.0;
	subsonly->iirSampleNR = 0.0;
	subsonly->iirSampleOR = 0.0;
	subsonly->iirSamplePR = 0.0;
	subsonly->iirSampleQR = 0.0;
	subsonly->iirSampleRR = 0.0;
	subsonly->iirSampleSR = 0.0;
	subsonly->iirSampleTR = 0.0;
	subsonly->iirSampleUR = 0.0;
	subsonly->iirSampleVR = 0.0;
	subsonly->iirSampleWR = 0.0;
	subsonly->iirSampleXR = 0.0;
	subsonly->iirSampleYR = 0.0;
	subsonly->iirSampleZR = 0.0;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	SubsOnly* subsonly = (SubsOnly*) instance;

	const float* in1 = subsonly->input[0];
	const float* in2 = subsonly->input[1];
	float* out1 = subsonly->output[0];
	float* out2 = subsonly->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= subsonly->sampleRate;
	double iirAmount = 2250 / 44100.0;
	double gaintarget = 1.42;
	double gain;
	iirAmount /= overallscale;
	double altAmount = 1.0 - iirAmount;
	double inputSampleL;
	double inputSampleR;

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = subsonly->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = subsonly->fpdR * 1.18e-17;

		gain = gaintarget;

		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		subsonly->iirSampleAL = (subsonly->iirSampleAL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleAL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleBL = (subsonly->iirSampleBL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleBL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleCL = (subsonly->iirSampleCL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleCL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleDL = (subsonly->iirSampleDL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleDL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleEL = (subsonly->iirSampleEL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleEL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleFL = (subsonly->iirSampleFL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleFL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleGL = (subsonly->iirSampleGL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleGL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleHL = (subsonly->iirSampleHL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleHL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleIL = (subsonly->iirSampleIL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleIL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleJL = (subsonly->iirSampleJL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleJL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleKL = (subsonly->iirSampleKL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleKL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleLL = (subsonly->iirSampleLL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleLL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleML = (subsonly->iirSampleML * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleML;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleNL = (subsonly->iirSampleNL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleNL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleOL = (subsonly->iirSampleOL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleOL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSamplePL = (subsonly->iirSamplePL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSamplePL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleQL = (subsonly->iirSampleQL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleQL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleRL = (subsonly->iirSampleRL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleRL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleSL = (subsonly->iirSampleSL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleSL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleTL = (subsonly->iirSampleTL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleTL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleUL = (subsonly->iirSampleUL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleUL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleVL = (subsonly->iirSampleVL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleVL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleWL = (subsonly->iirSampleWL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleWL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleXL = (subsonly->iirSampleXL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleXL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleYL = (subsonly->iirSampleYL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleYL;
		inputSampleL *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		subsonly->iirSampleZL = (subsonly->iirSampleZL * altAmount) + (inputSampleL * iirAmount);
		inputSampleL = subsonly->iirSampleZL;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		gain = gaintarget;

		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		subsonly->iirSampleAR = (subsonly->iirSampleAR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleAR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleBR = (subsonly->iirSampleBR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleBR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleCR = (subsonly->iirSampleCR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleCR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleDR = (subsonly->iirSampleDR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleDR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleER = (subsonly->iirSampleER * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleER;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleFR = (subsonly->iirSampleFR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleFR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleGR = (subsonly->iirSampleGR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleGR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleHR = (subsonly->iirSampleHR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleHR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleIR = (subsonly->iirSampleIR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleIR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleJR = (subsonly->iirSampleJR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleJR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleKR = (subsonly->iirSampleKR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleKR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleLR = (subsonly->iirSampleLR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleLR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleMR = (subsonly->iirSampleMR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleMR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleNR = (subsonly->iirSampleNR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleNR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleOR = (subsonly->iirSampleOR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleOR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSamplePR = (subsonly->iirSamplePR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSamplePR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleQR = (subsonly->iirSampleQR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleQR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleRR = (subsonly->iirSampleRR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleRR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleSR = (subsonly->iirSampleSR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleSR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleTR = (subsonly->iirSampleTR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleTR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleUR = (subsonly->iirSampleUR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleUR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleVR = (subsonly->iirSampleVR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleVR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleWR = (subsonly->iirSampleWR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleWR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleXR = (subsonly->iirSampleXR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleXR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleYR = (subsonly->iirSampleYR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleYR;
		inputSampleR *= gain;
		gain = ((gain - 1) * 0.75) + 1;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		subsonly->iirSampleZR = (subsonly->iirSampleZR * altAmount) + (inputSampleR * iirAmount);
		inputSampleR = subsonly->iirSampleZR;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;

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
	SUBSONLY_URI,
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
