#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MONITORING_URI "https://hannesbraun.net/ns/lv2/airwindows/monitoring"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	MONITOR = 4
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* monitor;

	double bynL[13], bynR[13];
	double noiseShapingL, noiseShapingR;
	// NJAD
	double aL[1503], bL[1503], cL[1503], dL[1503];
	double aR[1503], bR[1503], cR[1503], dR[1503];
	int ax, bx, cx, dx;
	// PeaksOnly
	double lastSampleL, lastSampleR;
	// SlewOnly
	double iirSampleAL, iirSampleBL, iirSampleCL, iirSampleDL, iirSampleEL, iirSampleFL, iirSampleGL;
	double iirSampleHL, iirSampleIL, iirSampleJL, iirSampleKL, iirSampleLL, iirSampleML, iirSampleNL, iirSampleOL, iirSamplePL;
	double iirSampleQL, iirSampleRL, iirSampleSL;
	double iirSampleTL, iirSampleUL, iirSampleVL;
	double iirSampleWL, iirSampleXL, iirSampleYL, iirSampleZL;

	double iirSampleAR, iirSampleBR, iirSampleCR, iirSampleDR, iirSampleER, iirSampleFR, iirSampleGR;
	double iirSampleHR, iirSampleIR, iirSampleJR, iirSampleKR, iirSampleLR, iirSampleMR, iirSampleNR, iirSampleOR, iirSamplePR;
	double iirSampleQR, iirSampleRR, iirSampleSR;
	double iirSampleTR, iirSampleUR, iirSampleVR;
	double iirSampleWR, iirSampleXR, iirSampleYR, iirSampleZR; // o/`
	// SubsOnly
	double biquadL[11];
	double biquadR[11];
	// Bandpasses

	uint32_t fpdL;
	uint32_t fpdR;
} Monitoring;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Monitoring* monitoring = (Monitoring*) calloc(1, sizeof(Monitoring));
	monitoring->sampleRate = rate;
	return (LV2_Handle) monitoring;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Monitoring* monitoring = (Monitoring*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			monitoring->input[0] = (const float*) data;
			break;
		case INPUT_R:
			monitoring->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			monitoring->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			monitoring->output[1] = (float*) data;
			break;
		case MONITOR:
			monitoring->monitor = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Monitoring* monitoring = (Monitoring*) instance;

	monitoring->bynL[0] = 1000.0;
	monitoring->bynL[1] = 301.0;
	monitoring->bynL[2] = 176.0;
	monitoring->bynL[3] = 125.0;
	monitoring->bynL[4] = 97.0;
	monitoring->bynL[5] = 79.0;
	monitoring->bynL[6] = 67.0;
	monitoring->bynL[7] = 58.0;
	monitoring->bynL[8] = 51.0;
	monitoring->bynL[9] = 46.0;
	monitoring->bynL[10] = 1000.0;
	monitoring->noiseShapingL = 0.0;
	monitoring->bynR[0] = 1000.0;
	monitoring->bynR[1] = 301.0;
	monitoring->bynR[2] = 176.0;
	monitoring->bynR[3] = 125.0;
	monitoring->bynR[4] = 97.0;
	monitoring->bynR[5] = 79.0;
	monitoring->bynR[6] = 67.0;
	monitoring->bynR[7] = 58.0;
	monitoring->bynR[8] = 51.0;
	monitoring->bynR[9] = 46.0;
	monitoring->bynR[10] = 1000.0;
	monitoring->noiseShapingR = 0.0;
	// end NJAD
	for (int count = 0; count < 1502; count++) {
		monitoring->aL[count] = 0.0;
		monitoring->bL[count] = 0.0;
		monitoring->cL[count] = 0.0;
		monitoring->dL[count] = 0.0;
		monitoring->aR[count] = 0.0;
		monitoring->bR[count] = 0.0;
		monitoring->cR[count] = 0.0;
		monitoring->dR[count] = 0.0;
	}
	monitoring->ax = 1;
	monitoring->bx = 1;
	monitoring->cx = 1;
	monitoring->dx = 1;
	// PeaksOnly
	monitoring->lastSampleL = 0.0;
	monitoring->lastSampleR = 0.0;
	// SlewOnly
	monitoring->iirSampleAL = 0.0;
	monitoring->iirSampleBL = 0.0;
	monitoring->iirSampleCL = 0.0;
	monitoring->iirSampleDL = 0.0;
	monitoring->iirSampleEL = 0.0;
	monitoring->iirSampleFL = 0.0;
	monitoring->iirSampleGL = 0.0;
	monitoring->iirSampleHL = 0.0;
	monitoring->iirSampleIL = 0.0;
	monitoring->iirSampleJL = 0.0;
	monitoring->iirSampleKL = 0.0;
	monitoring->iirSampleLL = 0.0;
	monitoring->iirSampleML = 0.0;
	monitoring->iirSampleNL = 0.0;
	monitoring->iirSampleOL = 0.0;
	monitoring->iirSamplePL = 0.0;
	monitoring->iirSampleQL = 0.0;
	monitoring->iirSampleRL = 0.0;
	monitoring->iirSampleSL = 0.0;
	monitoring->iirSampleTL = 0.0;
	monitoring->iirSampleUL = 0.0;
	monitoring->iirSampleVL = 0.0;
	monitoring->iirSampleWL = 0.0;
	monitoring->iirSampleXL = 0.0;
	monitoring->iirSampleYL = 0.0;
	monitoring->iirSampleZL = 0.0;

	monitoring->iirSampleAR = 0.0;
	monitoring->iirSampleBR = 0.0;
	monitoring->iirSampleCR = 0.0;
	monitoring->iirSampleDR = 0.0;
	monitoring->iirSampleER = 0.0;
	monitoring->iirSampleFR = 0.0;
	monitoring->iirSampleGR = 0.0;
	monitoring->iirSampleHR = 0.0;
	monitoring->iirSampleIR = 0.0;
	monitoring->iirSampleJR = 0.0;
	monitoring->iirSampleKR = 0.0;
	monitoring->iirSampleLR = 0.0;
	monitoring->iirSampleMR = 0.0;
	monitoring->iirSampleNR = 0.0;
	monitoring->iirSampleOR = 0.0;
	monitoring->iirSamplePR = 0.0;
	monitoring->iirSampleQR = 0.0;
	monitoring->iirSampleRR = 0.0;
	monitoring->iirSampleSR = 0.0;
	monitoring->iirSampleTR = 0.0;
	monitoring->iirSampleUR = 0.0;
	monitoring->iirSampleVR = 0.0;
	monitoring->iirSampleWR = 0.0;
	monitoring->iirSampleXR = 0.0;
	monitoring->iirSampleYR = 0.0;
	monitoring->iirSampleZR = 0.0; // o/`
	// SubsOnly
	for (int x = 0; x < 11; x++) {
		monitoring->biquadL[x] = 0.0;
		monitoring->biquadR[x] = 0.0;
	}
	// Bandpasses
	monitoring->fpdL = 1.0;
	while (monitoring->fpdL < 16386) monitoring->fpdL = rand() * UINT32_MAX;
	monitoring->fpdR = 1.0;
	while (monitoring->fpdR < 16386) monitoring->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Monitoring* monitoring = (Monitoring*) instance;

	const float* in1 = monitoring->input[0];
	const float* in2 = monitoring->input[1];
	float* out1 = monitoring->output[0];
	float* out2 = monitoring->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= monitoring->sampleRate;

	int processing = (int) *monitoring->monitor;
	if (processing > 16) {
		processing = 16;
	} else if (processing < 0) {
		processing = 0;
	}

	int am = (int) 149.0 * overallscale;
	int bm = (int) 179.0 * overallscale;
	int cm = (int) 191.0 * overallscale;
	int dm = (int) 223.0 * overallscale; // these are 'good' primes, spacing out the allpasses
	int allpasstemp;
	// for PeaksOnly
	monitoring->biquadL[0] = 0.0375 / overallscale;
	monitoring->biquadL[1] = 0.1575; // define as AURAT, MONORAT, MONOLAT unless overridden
	if (processing == 7) {
		monitoring->biquadL[0] = 0.0385 / overallscale;
		monitoring->biquadL[1] = 0.0825;
	}
	if (processing == 11) {
		monitoring->biquadL[0] = 0.1245 / overallscale;
		monitoring->biquadL[1] = 0.46;
	}
	double K = tan(M_PI * monitoring->biquadL[0]);
	double norm = 1.0 / (1.0 + K / monitoring->biquadL[1] + K * K);
	monitoring->biquadL[2] = K / monitoring->biquadL[1] * norm;
	monitoring->biquadL[4] = -monitoring->biquadL[2]; // for bandpass, ignore [3] = 0.0
	monitoring->biquadL[5] = 2.0 * (K * K - 1.0) * norm;
	monitoring->biquadL[6] = (1.0 - K / monitoring->biquadL[1] + K * K) * norm;
	// for Bandpasses
	monitoring->biquadR[0] = 0.0375 / overallscale;
	monitoring->biquadR[1] = 0.1575; // define as AURAT, MONORAT, MONOLAT unless overridden
	if (processing == 7) {
		monitoring->biquadR[0] = 0.0385 / overallscale;
		monitoring->biquadR[1] = 0.0825;
	}
	if (processing == 11) {
		monitoring->biquadR[0] = 0.1245 / overallscale;
		monitoring->biquadR[1] = 0.46;
	}
	K = tan(M_PI * monitoring->biquadR[0]);
	norm = 1.0 / (1.0 + K / monitoring->biquadR[1] + K * K);
	monitoring->biquadR[2] = K / monitoring->biquadR[1] * norm;
	monitoring->biquadR[4] = -monitoring->biquadR[2]; // for bandpass, ignore [3] = 0.0
	monitoring->biquadR[5] = 2.0 * (K * K - 1.0) * norm;
	monitoring->biquadR[6] = (1.0 - K / monitoring->biquadR[1] + K * K) * norm;
	// for Bandpasses

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = monitoring->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = monitoring->fpdR * 1.18e-17;

		switch (processing) {
			case 0:
			case 1:
				break;
			case 2:
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleR = asin(inputSampleR);
				// amplitude aspect
				allpasstemp = monitoring->ax - 1;
				if (allpasstemp < 0 || allpasstemp > am) allpasstemp = am;
				inputSampleL -= monitoring->aL[allpasstemp] * 0.5;
				monitoring->aL[monitoring->ax] = inputSampleL;
				inputSampleL *= 0.5;
				inputSampleR -= monitoring->aR[allpasstemp] * 0.5;
				monitoring->aR[monitoring->ax] = inputSampleR;
				inputSampleR *= 0.5;
				monitoring->ax--;
				if (monitoring->ax < 0 || monitoring->ax > am) {
					monitoring->ax = am;
				}
				inputSampleL += (monitoring->aL[monitoring->ax]);
				inputSampleR += (monitoring->aR[monitoring->ax]);
				// a single Midiverb-style allpass

				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleR = asin(inputSampleR);
				// amplitude aspect

				allpasstemp = monitoring->bx - 1;
				if (allpasstemp < 0 || allpasstemp > bm) allpasstemp = bm;
				inputSampleL -= monitoring->bL[allpasstemp] * 0.5;
				monitoring->bL[monitoring->bx] = inputSampleL;
				inputSampleL *= 0.5;
				inputSampleR -= monitoring->bR[allpasstemp] * 0.5;
				monitoring->bR[monitoring->bx] = inputSampleR;
				inputSampleR *= 0.5;
				monitoring->bx--;
				if (monitoring->bx < 0 || monitoring->bx > bm) {
					monitoring->bx = bm;
				}
				inputSampleL += (monitoring->bL[monitoring->bx]);
				inputSampleR += (monitoring->bR[monitoring->bx]);
				// a single Midiverb-style allpass

				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleR = asin(inputSampleR);
				// amplitude aspect

				allpasstemp = monitoring->cx - 1;
				if (allpasstemp < 0 || allpasstemp > cm) allpasstemp = cm;
				inputSampleL -= monitoring->cL[allpasstemp] * 0.5;
				monitoring->cL[monitoring->cx] = inputSampleL;
				inputSampleL *= 0.5;
				inputSampleR -= monitoring->cR[allpasstemp] * 0.5;
				monitoring->cR[monitoring->cx] = inputSampleR;
				inputSampleR *= 0.5;
				monitoring->cx--;
				if (monitoring->cx < 0 || monitoring->cx > cm) {
					monitoring->cx = cm;
				}
				inputSampleL += (monitoring->cL[monitoring->cx]);
				inputSampleR += (monitoring->cR[monitoring->cx]);
				// a single Midiverb-style allpass

				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleR = asin(inputSampleR);
				// amplitude aspect

				allpasstemp = monitoring->dx - 1;
				if (allpasstemp < 0 || allpasstemp > dm) allpasstemp = dm;
				inputSampleL -= monitoring->dL[allpasstemp] * 0.5;
				monitoring->dL[monitoring->dx] = inputSampleL;
				inputSampleL *= 0.5;
				inputSampleR -= monitoring->dR[allpasstemp] * 0.5;
				monitoring->dR[monitoring->dx] = inputSampleR;
				inputSampleR *= 0.5;
				monitoring->dx--;
				if (monitoring->dx < 0 || monitoring->dx > dm) {
					monitoring->dx = dm;
				}
				inputSampleL += (monitoring->dL[monitoring->dx]);
				inputSampleR += (monitoring->dR[monitoring->dx]);
				// a single Midiverb-style allpass

				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleR = asin(inputSampleR);
				// amplitude aspect

				inputSampleL *= 0.63679;
				inputSampleR *= 0.63679; // scale it to 0dB output at full blast
				// PeaksOnly
				break;
			case 3: {
				double trim;
				trim = 2.302585092994045684017991; // natural logarithm of 10
				double slewSample;
				slewSample = (inputSampleL - monitoring->lastSampleL) * trim;
				monitoring->lastSampleL = inputSampleL;
				if (slewSample > 1.0) slewSample = 1.0;
				if (slewSample < -1.0) slewSample = -1.0;
				inputSampleL = slewSample;
				slewSample = (inputSampleR - monitoring->lastSampleR) * trim;
				monitoring->lastSampleR = inputSampleR;
				if (slewSample > 1.0) slewSample = 1.0;
				if (slewSample < -1.0) slewSample = -1.0;
				inputSampleR = slewSample;
				// SlewOnly
			} break;
			case 4: {
				double iirAmount;
				iirAmount = (2250 / 44100.0) / overallscale;
				double gain;
				gain = 1.42;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;

				monitoring->iirSampleAL = (monitoring->iirSampleAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleAL;
				monitoring->iirSampleAR = (monitoring->iirSampleAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleAR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleBL = (monitoring->iirSampleBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleBL;
				monitoring->iirSampleBR = (monitoring->iirSampleBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleBR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleCL = (monitoring->iirSampleCL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleCL;
				monitoring->iirSampleCR = (monitoring->iirSampleCR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleCR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleDL = (monitoring->iirSampleDL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleDL;
				monitoring->iirSampleDR = (monitoring->iirSampleDR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleDR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleEL = (monitoring->iirSampleEL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleEL;
				monitoring->iirSampleER = (monitoring->iirSampleER * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleER;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleFL = (monitoring->iirSampleFL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleFL;
				monitoring->iirSampleFR = (monitoring->iirSampleFR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleFR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleGL = (monitoring->iirSampleGL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleGL;
				monitoring->iirSampleGR = (monitoring->iirSampleGR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleGR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleHL = (monitoring->iirSampleHL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleHL;
				monitoring->iirSampleHR = (monitoring->iirSampleHR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleHR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleIL = (monitoring->iirSampleIL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleIL;
				monitoring->iirSampleIR = (monitoring->iirSampleIR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleIR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleJL = (monitoring->iirSampleJL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleJL;
				monitoring->iirSampleJR = (monitoring->iirSampleJR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleJR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleKL = (monitoring->iirSampleKL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleKL;
				monitoring->iirSampleKR = (monitoring->iirSampleKR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleKR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleLL = (monitoring->iirSampleLL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleLL;
				monitoring->iirSampleLR = (monitoring->iirSampleLR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleLR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleML = (monitoring->iirSampleML * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleML;
				monitoring->iirSampleMR = (monitoring->iirSampleMR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleMR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleNL = (monitoring->iirSampleNL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleNL;
				monitoring->iirSampleNR = (monitoring->iirSampleNR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleNR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleOL = (monitoring->iirSampleOL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleOL;
				monitoring->iirSampleOR = (monitoring->iirSampleOR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleOR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSamplePL = (monitoring->iirSamplePL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSamplePL;
				monitoring->iirSamplePR = (monitoring->iirSamplePR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSamplePR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleQL = (monitoring->iirSampleQL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleQL;
				monitoring->iirSampleQR = (monitoring->iirSampleQR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleQR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleRL = (monitoring->iirSampleRL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleRL;
				monitoring->iirSampleRR = (monitoring->iirSampleRR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleRR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleSL = (monitoring->iirSampleSL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleSL;
				monitoring->iirSampleSR = (monitoring->iirSampleSR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleSR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleTL = (monitoring->iirSampleTL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleTL;
				monitoring->iirSampleTR = (monitoring->iirSampleTR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleTR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleUL = (monitoring->iirSampleUL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleUL;
				monitoring->iirSampleUR = (monitoring->iirSampleUR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleUR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleVL = (monitoring->iirSampleVL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleVL;
				monitoring->iirSampleVR = (monitoring->iirSampleVR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleVR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleWL = (monitoring->iirSampleWL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleWL;
				monitoring->iirSampleWR = (monitoring->iirSampleWR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleWR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleXL = (monitoring->iirSampleXL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleXL;
				monitoring->iirSampleXR = (monitoring->iirSampleXR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleXR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleYL = (monitoring->iirSampleYL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleYL;
				monitoring->iirSampleYR = (monitoring->iirSampleYR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleYR;
				inputSampleL *= gain;
				inputSampleR *= gain;
				gain = ((gain - 1) * 0.75) + 1;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;

				monitoring->iirSampleZL = (monitoring->iirSampleZL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
				inputSampleL = monitoring->iirSampleZL;
				monitoring->iirSampleZR = (monitoring->iirSampleZR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
				inputSampleR = monitoring->iirSampleZR;
				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				// SubsOnly
			} break;
			case 5:
			case 6: {
				double mid;
				mid = inputSampleL + inputSampleR;
				double side;
				side = inputSampleL - inputSampleR;
				if (processing < 6) side = 0.0;
				else mid = 0.0; // mono monitoring, or side-only monitoring
				inputSampleL = (mid + side) / 2.0;
				inputSampleR = (mid - side) / 2.0;
			} break;
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				// Bandpass: changes in EQ are up in the variable defining, not here
				// 7 Vinyl, 8 9 10 Aurat, 11 Phone
				if (processing == 9) {
					inputSampleR = (inputSampleL + inputSampleR) * 0.5;
					inputSampleL = 0.0;
				}
				if (processing == 10) {
					inputSampleL = (inputSampleL + inputSampleR) * 0.5;
					inputSampleR = 0.0;
				}
				if (processing == 11) {
					double M;
					M = (inputSampleL + inputSampleR) * 0.5;
					inputSampleL = M;
					inputSampleR = M;
				}

				inputSampleL = sin(inputSampleL);
				inputSampleR = sin(inputSampleR);
				// encode Console5: good cleanness

				double tempSampleL;
				tempSampleL = (inputSampleL * monitoring->biquadL[2]) + monitoring->biquadL[7];
				monitoring->biquadL[7] = (-tempSampleL * monitoring->biquadL[5]) + monitoring->biquadL[8];
				monitoring->biquadL[8] = (inputSampleL * monitoring->biquadL[4]) - (tempSampleL * monitoring->biquadL[6]);
				inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel

				double tempSampleR;
				tempSampleR = (inputSampleR * monitoring->biquadR[2]) + monitoring->biquadR[7];
				monitoring->biquadR[7] = (-tempSampleR * monitoring->biquadR[5]) + monitoring->biquadR[8];
				monitoring->biquadR[8] = (inputSampleR * monitoring->biquadR[4]) - (tempSampleR * monitoring->biquadR[6]);
				inputSampleR = tempSampleR; // we are using the mono configuration

				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				// without this, you can get a NaN condition where it spits out DC offset at full blast!
				inputSampleL = asin(inputSampleL);
				inputSampleR = asin(inputSampleR);
				// amplitude aspect
				break;
			case 12:
			case 13:
			case 14:
			case 15: {
				double mid;
				double side;
				if (processing == 12) {
					inputSampleL *= 0.855;
					inputSampleR *= 0.855;
				}
				if (processing == 13) {
					inputSampleL *= 0.748;
					inputSampleR *= 0.748;
				}
				if (processing == 14) {
					inputSampleL *= 0.713;
					inputSampleR *= 0.713;
				}
				if (processing == 15) {
					inputSampleL *= 0.680;
					inputSampleR *= 0.680;
				}
				// we do a volume compensation immediately to gain stage stuff cleanly
				inputSampleL = sin(inputSampleL);
				inputSampleR = sin(inputSampleR);
				double drySampleL;
				drySampleL = inputSampleL;
				double drySampleR;
				drySampleR = inputSampleR; // everything runs 'inside' Console
				double bass;
				bass = (processing * processing * 0.00001) / overallscale;
				// we are using the iir filters from out of SubsOnly

				mid = inputSampleL + inputSampleR;
				side = inputSampleL - inputSampleR;
				monitoring->iirSampleAL = (monitoring->iirSampleAL * (1.0 - (bass * 0.618))) + (side * bass * 0.618);
				side = side - monitoring->iirSampleAL;
				inputSampleL = (mid + side) / 2.0;
				inputSampleR = (mid - side) / 2.0;
				// bass narrowing filter

				allpasstemp = monitoring->ax - 1;
				if (allpasstemp < 0 || allpasstemp > am) allpasstemp = am;
				inputSampleL -= monitoring->aL[allpasstemp] * 0.5;
				monitoring->aL[monitoring->ax] = inputSampleL;
				inputSampleL *= 0.5;
				inputSampleR -= monitoring->aR[allpasstemp] * 0.5;
				monitoring->aR[monitoring->ax] = inputSampleR;
				inputSampleR *= 0.5;

				monitoring->ax--;
				if (monitoring->ax < 0 || monitoring->ax > am) {
					monitoring->ax = am;
				}
				inputSampleL += (monitoring->aL[monitoring->ax]) * 0.5;
				inputSampleR += (monitoring->aR[monitoring->ax]) * 0.5;
				if (monitoring->ax == am) {
					inputSampleL += (monitoring->aL[0]) * 0.5;
					inputSampleR += (monitoring->aR[0]) * 0.5;
				} else {
					inputSampleL += (monitoring->aL[monitoring->ax + 1]) * 0.5;
					inputSampleR += (monitoring->aR[monitoring->ax + 1]) * 0.5;
				}
				// a darkened Midiverb-style allpass

				if (processing == 12) {
					inputSampleL *= 0.125;
					inputSampleR *= 0.125;
				}
				if (processing == 13) {
					inputSampleL *= 0.25;
					inputSampleR *= 0.25;
				}
				if (processing == 14) {
					inputSampleL *= 0.30;
					inputSampleR *= 0.30;
				}
				if (processing == 15) {
					inputSampleL *= 0.35;
					inputSampleR *= 0.35;
				}
				// Cans A suppresses the crossfeed more, Cans B makes it louder

				drySampleL += inputSampleR;
				drySampleR += inputSampleL; // the crossfeed

				allpasstemp = monitoring->dx - 1;
				if (allpasstemp < 0 || allpasstemp > dm) allpasstemp = dm;
				inputSampleL -= monitoring->dL[allpasstemp] * 0.5;
				monitoring->dL[monitoring->dx] = inputSampleL;
				inputSampleL *= 0.5;
				inputSampleR -= monitoring->dR[allpasstemp] * 0.5;
				monitoring->dR[monitoring->dx] = inputSampleR;
				inputSampleR *= 0.5;

				monitoring->dx--;
				if (monitoring->dx < 0 || monitoring->dx > dm) {
					monitoring->dx = dm;
				}
				inputSampleL += (monitoring->dL[monitoring->dx]) * 0.5;
				inputSampleR += (monitoring->dR[monitoring->dx]) * 0.5;
				if (monitoring->dx == dm) {
					inputSampleL += (monitoring->dL[0]) * 0.5;
					inputSampleR += (monitoring->dR[0]) * 0.5;
				} else {
					inputSampleL += (monitoring->dL[monitoring->dx + 1]) * 0.5;
					inputSampleR += (monitoring->dR[monitoring->dx + 1]) * 0.5;
				}
				// a darkened Midiverb-style allpass, which is stretching the previous one even more

				inputSampleL *= 0.25;
				inputSampleR *= 0.25;
				// for all versions of Cans the second level of bloom is this far down
				// and, remains on the opposite speaker rather than crossing again to the original side

				drySampleL += inputSampleR;
				drySampleR += inputSampleL; // add the crossfeed and very faint extra verbyness

				inputSampleL = drySampleL;
				inputSampleR = drySampleR; // and output our can-opened headphone feed

				mid = inputSampleL + inputSampleR;
				side = inputSampleL - inputSampleR;
				monitoring->iirSampleAR = (monitoring->iirSampleAR * (1.0 - bass)) + (side * bass);
				side = side - monitoring->iirSampleAR;
				inputSampleL = (mid + side) / 2.0;
				inputSampleR = (mid - side) / 2.0;
				// bass narrowing filter

				if (inputSampleL > 1.0) inputSampleL = 1.0;
				if (inputSampleL < -1.0) inputSampleL = -1.0;
				inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0) inputSampleR = 1.0;
				if (inputSampleR < -1.0) inputSampleR = -1.0;
				inputSampleR = asin(inputSampleR);
				// ConsoleBuss processing
			} break;
			case 16: {
				double inputSample = (inputSampleL + inputSampleR) * 0.5;
				inputSampleL = -inputSample;
				inputSampleR = inputSample;
			} break;
		}

		// begin Not Just Another Dither
		if (processing == 1) {
			inputSampleL = inputSampleL * 32768.0; // or 16 bit option
			inputSampleR = inputSampleR * 32768.0; // or 16 bit option
		} else {
			inputSampleL = inputSampleL * 8388608.0; // for literally everything else
			inputSampleR = inputSampleR * 8388608.0; // we will apply the 24 bit NJAD
		} // on the not unreasonable assumption that we are very likely playing back on 24 bit DAC
		// if we're not, then all we did was apply a Benford Realness function at 24 bits down.

		bool cutbinsL;
		cutbinsL = false;
		bool cutbinsR;
		cutbinsR = false;
		double drySampleL;
		drySampleL = inputSampleL;
		double drySampleR;
		drySampleR = inputSampleR;
		inputSampleL -= monitoring->noiseShapingL;
		inputSampleR -= monitoring->noiseShapingR;
		// NJAD L
		double benfordize;
		benfordize = floor(inputSampleL);
		while (benfordize >= 1.0) benfordize /= 10;
		while (benfordize < 1.0 && benfordize > 0.0000001) benfordize *= 10;
		int hotbinA;
		hotbinA = floor(benfordize);
		// hotbin becomes the Benford bin value for this number floored
		double totalA;
		totalA = 0;
		if ((hotbinA > 0) && (hotbinA < 10)) {
			monitoring->bynL[hotbinA] += 1;
			if (monitoring->bynL[hotbinA] > 982) cutbinsL = true;
			totalA += (301 - monitoring->bynL[1]);
			totalA += (176 - monitoring->bynL[2]);
			totalA += (125 - monitoring->bynL[3]);
			totalA += (97 - monitoring->bynL[4]);
			totalA += (79 - monitoring->bynL[5]);
			totalA += (67 - monitoring->bynL[6]);
			totalA += (58 - monitoring->bynL[7]);
			totalA += (51 - monitoring->bynL[8]);
			totalA += (46 - monitoring->bynL[9]);
			monitoring->bynL[hotbinA] -= 1;
		} else hotbinA = 10;
		// produce total number- smaller is closer to Benford real
		benfordize = ceil(inputSampleL);
		while (benfordize >= 1.0) benfordize /= 10;
		while (benfordize < 1.0 && benfordize > 0.0000001) benfordize *= 10;
		int hotbinB;
		hotbinB = floor(benfordize);
		// hotbin becomes the Benford bin value for this number ceiled
		double totalB;
		totalB = 0;
		if ((hotbinB > 0) && (hotbinB < 10)) {
			monitoring->bynL[hotbinB] += 1;
			if (monitoring->bynL[hotbinB] > 982) cutbinsL = true;
			totalB += (301 - monitoring->bynL[1]);
			totalB += (176 - monitoring->bynL[2]);
			totalB += (125 - monitoring->bynL[3]);
			totalB += (97 - monitoring->bynL[4]);
			totalB += (79 - monitoring->bynL[5]);
			totalB += (67 - monitoring->bynL[6]);
			totalB += (58 - monitoring->bynL[7]);
			totalB += (51 - monitoring->bynL[8]);
			totalB += (46 - monitoring->bynL[9]);
			monitoring->bynL[hotbinB] -= 1;
		} else hotbinB = 10;
		// produce total number- smaller is closer to Benford real
		double outputSample;
		if (totalA < totalB) {
			monitoring->bynL[hotbinA] += 1;
			outputSample = floor(inputSampleL);
		} else {
			monitoring->bynL[hotbinB] += 1;
			outputSample = floor(inputSampleL + 1);
		}
		// assign the relevant one to the delay line
		// and floor/ceil signal accordingly
		if (cutbinsL) {
			monitoring->bynL[1] *= 0.99;
			monitoring->bynL[2] *= 0.99;
			monitoring->bynL[3] *= 0.99;
			monitoring->bynL[4] *= 0.99;
			monitoring->bynL[5] *= 0.99;
			monitoring->bynL[6] *= 0.99;
			monitoring->bynL[7] *= 0.99;
			monitoring->bynL[8] *= 0.99;
			monitoring->bynL[9] *= 0.99;
			monitoring->bynL[10] *= 0.99;
		}
		monitoring->noiseShapingL += outputSample - drySampleL;
		if (monitoring->noiseShapingL > fabs(inputSampleL)) monitoring->noiseShapingL = fabs(inputSampleL);
		if (monitoring->noiseShapingL < -fabs(inputSampleL)) monitoring->noiseShapingL = -fabs(inputSampleL);
		if (processing == 1) inputSampleL = outputSample / 32768.0;
		else inputSampleL = outputSample / 8388608.0;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		// finished NJAD L

		// NJAD R
		benfordize = floor(inputSampleR);
		while (benfordize >= 1.0) benfordize /= 10;
		while (benfordize < 1.0 && benfordize > 0.0000001) benfordize *= 10;
		hotbinA = floor(benfordize);
		// hotbin becomes the Benford bin value for this number floored
		totalA = 0;
		if ((hotbinA > 0) && (hotbinA < 10)) {
			monitoring->bynR[hotbinA] += 1;
			if (monitoring->bynR[hotbinA] > 982) cutbinsR = true;
			totalA += (301 - monitoring->bynR[1]);
			totalA += (176 - monitoring->bynR[2]);
			totalA += (125 - monitoring->bynR[3]);
			totalA += (97 - monitoring->bynR[4]);
			totalA += (79 - monitoring->bynR[5]);
			totalA += (67 - monitoring->bynR[6]);
			totalA += (58 - monitoring->bynR[7]);
			totalA += (51 - monitoring->bynR[8]);
			totalA += (46 - monitoring->bynR[9]);
			monitoring->bynR[hotbinA] -= 1;
		} else hotbinA = 10;
		// produce total number- smaller is closer to Benford real
		benfordize = ceil(inputSampleR);
		while (benfordize >= 1.0) benfordize /= 10;
		while (benfordize < 1.0 && benfordize > 0.0000001) benfordize *= 10;
		hotbinB = floor(benfordize);
		// hotbin becomes the Benford bin value for this number ceiled
		totalB = 0;
		if ((hotbinB > 0) && (hotbinB < 10)) {
			monitoring->bynR[hotbinB] += 1;
			if (monitoring->bynR[hotbinB] > 982) cutbinsR = true;
			totalB += (301 - monitoring->bynR[1]);
			totalB += (176 - monitoring->bynR[2]);
			totalB += (125 - monitoring->bynR[3]);
			totalB += (97 - monitoring->bynR[4]);
			totalB += (79 - monitoring->bynR[5]);
			totalB += (67 - monitoring->bynR[6]);
			totalB += (58 - monitoring->bynR[7]);
			totalB += (51 - monitoring->bynR[8]);
			totalB += (46 - monitoring->bynR[9]);
			monitoring->bynR[hotbinB] -= 1;
		} else hotbinB = 10;
		// produce total number- smaller is closer to Benford real
		if (totalA < totalB) {
			monitoring->bynR[hotbinA] += 1;
			outputSample = floor(inputSampleR);
		} else {
			monitoring->bynR[hotbinB] += 1;
			outputSample = floor(inputSampleR + 1);
		}
		// assign the relevant one to the delay line
		// and floor/ceil signal accordingly
		if (cutbinsR) {
			monitoring->bynR[1] *= 0.99;
			monitoring->bynR[2] *= 0.99;
			monitoring->bynR[3] *= 0.99;
			monitoring->bynR[4] *= 0.99;
			monitoring->bynR[5] *= 0.99;
			monitoring->bynR[6] *= 0.99;
			monitoring->bynR[7] *= 0.99;
			monitoring->bynR[8] *= 0.99;
			monitoring->bynR[9] *= 0.99;
			monitoring->bynR[10] *= 0.99;
		}
		monitoring->noiseShapingR += outputSample - drySampleR;
		if (monitoring->noiseShapingR > fabs(inputSampleR)) monitoring->noiseShapingR = fabs(inputSampleR);
		if (monitoring->noiseShapingR < -fabs(inputSampleR)) monitoring->noiseShapingR = -fabs(inputSampleR);
		if (processing == 1) inputSampleR = outputSample / 32768.0;
		else inputSampleR = outputSample / 8388608.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		// finished NJAD R

		// does not use 32 bit stereo floating point dither

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
	MONITORING_URI,
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
