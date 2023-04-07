#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846264338327950288

#define PRESSURE5_URI "https://hannesbraun.net/ns/lv2/airwindows/pressure5"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	PRESSURE = 4,
	SPEED = 5,
	MEWINESS = 6,
	PAWCLAW = 7,
	OUTPUT = 8,
	DRYWET = 9
} PortIndex;

enum {
	fix_freq,
	fix_reso,
	fix_a0,
	fix_a1,
	fix_a2,
	fix_b1,
	fix_b2,
	fix_sL1,
	fix_sL2,
	fix_sR1,
	fix_sR2,
	fix_lastSampleL,
	fix_lastSampleR,
	fix_total
};

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* pressure;
	const float* speed;
	const float* mewiness;
	const float* pawclaw;
	const float* outputGain;
	const float* drywet;

	double muVary;
	double muAttack;
	double muNewSpeed;
	double muSpeedA;
	double muSpeedB;
	double muCoefficientA;
	double muCoefficientB;
	bool flip; // Pressure

	double fixA[fix_total];
	double fixB[fix_total]; // fixed frequency biquad filter for ultrasonics, stereo

	double lastSampleL;
	double intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	double lastSampleR;
	double intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; // Stereo ClipOnly2

	double slewMax; // to adust mewiness

	uint32_t fpdL;
	uint32_t fpdR;
} Pressure5;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Pressure5* pressure5 = (Pressure5*) calloc(1, sizeof(Pressure5));
	pressure5->sampleRate = rate;
	return (LV2_Handle) pressure5;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Pressure5* pressure5 = (Pressure5*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pressure5->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pressure5->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pressure5->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pressure5->output[1] = (float*) data;
			break;
		case PRESSURE:
			pressure5->pressure = (const float*) data;
			break;
		case SPEED:
			pressure5->speed = (const float*) data;
			break;
		case MEWINESS:
			pressure5->mewiness = (const float*) data;
			break;
		case PAWCLAW:
			pressure5->pawclaw = (const float*) data;
			break;
		case OUTPUT:
			pressure5->outputGain = (const float*) data;
			break;
		case DRYWET:
			pressure5->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Pressure5* pressure5 = (Pressure5*) instance;

	pressure5->muSpeedA = 10000;
	pressure5->muSpeedB = 10000;
	pressure5->muCoefficientA = 1;
	pressure5->muCoefficientB = 1;
	pressure5->muVary = 1;
	pressure5->flip = false;
	for (int x = 0; x < fix_total; x++) {
		pressure5->fixA[x] = 0.0;
		pressure5->fixB[x] = 0.0;
	}
	pressure5->lastSampleL = 0.0;
	pressure5->wasPosClipL = false;
	pressure5->wasNegClipL = false;
	pressure5->lastSampleR = 0.0;
	pressure5->wasPosClipR = false;
	pressure5->wasNegClipR = false;
	for (int x = 0; x < 16; x++) {
		pressure5->intermediateL[x] = 0.0;
		pressure5->intermediateR[x] = 0.0;
	}
	pressure5->slewMax = 0.0;

	pressure5->fpdL = 1.0;
	while (pressure5->fpdL < 16386) pressure5->fpdL = rand() * UINT32_MAX;
	pressure5->fpdR = 1.0;
	while (pressure5->fpdR < 16386) pressure5->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Pressure5* pressure5 = (Pressure5*) instance;

	const float* in1 = pressure5->input[0];
	const float* in2 = pressure5->input[1];
	float* out1 = pressure5->output[0];
	float* out2 = pressure5->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= pressure5->sampleRate;
	int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1;
	if (spacing > 16) spacing = 16;

	double threshold = 1.0 - (*pressure5->pressure * 0.95);
	double muMakeupGain = 1.0 / threshold;
	// gain settings around threshold
	double release = pow((1.28 - *pressure5->speed), 5) * 32768.0;
	double fastest = sqrt(release);
	release /= overallscale;
	fastest /= overallscale;
	// speed settings around release
	double mewinessRef = *pressure5->mewiness;
	double pawsClaws = -(*pressure5->pawclaw - 0.5) * 1.618033988749894848204586;
	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
	double outputGain = pow(*pressure5->outputGain * 2.0, 2); // max 4.0 gain
	double wet = *pressure5->drywet;

	pressure5->fixA[fix_freq] = 24000.0 / pressure5->sampleRate;
	pressure5->fixA[fix_reso] = 0.7071; // butterworth Q
	double K = tan(M_PI * pressure5->fixA[fix_freq]);
	double norm = 1.0 / (1.0 + K / pressure5->fixA[fix_reso] + K * K);
	pressure5->fixA[fix_a0] = K * K * norm;
	pressure5->fixA[fix_a1] = 2.0 * pressure5->fixA[fix_a0];
	pressure5->fixA[fix_a2] = pressure5->fixA[fix_a0];
	pressure5->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	pressure5->fixA[fix_b2] = (1.0 - K / pressure5->fixA[fix_reso] + K * K) * norm;
	// for the fixed-position biquad filter
	for (int x = 0; x < fix_sL1; x++) pressure5->fixB[x] = pressure5->fixA[x];
	// make the second filter same as the first, don't use sample slots

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pressure5->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pressure5->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		inputSampleL = inputSampleL * muMakeupGain;
		inputSampleR = inputSampleR * muMakeupGain;

		if (pressure5->fixA[fix_freq] < 0.4999) {
			double temp = (inputSampleL * pressure5->fixA[fix_a0]) + pressure5->fixA[fix_sL1];
			pressure5->fixA[fix_sL1] = (inputSampleL * pressure5->fixA[fix_a1]) - (temp * pressure5->fixA[fix_b1]) + pressure5->fixA[fix_sL2];
			pressure5->fixA[fix_sL2] = (inputSampleL * pressure5->fixA[fix_a2]) - (temp * pressure5->fixA[fix_b2]);
			inputSampleL = temp;
			temp = (inputSampleR * pressure5->fixA[fix_a0]) + pressure5->fixA[fix_sR1];
			pressure5->fixA[fix_sR1] = (inputSampleR * pressure5->fixA[fix_a1]) - (temp * pressure5->fixA[fix_b1]) + pressure5->fixA[fix_sR2];
			pressure5->fixA[fix_sR2] = (inputSampleR * pressure5->fixA[fix_a2]) - (temp * pressure5->fixA[fix_b2]);
			inputSampleR = temp; // fixed biquad filtering ultrasonics before Pressure
		}

		double inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		// we will take the greater of either channel and just use that, then apply the result
		// to both stereo channels.

		double mewiness = sin(mewinessRef + (pressure5->slewMax * pawsClaws));
		bool positivemu = true;
		if (mewiness < 0) {
			positivemu = false;
			mewiness = -mewiness;
		}

		if (pressure5->flip) {
			if (inputSense > threshold) {
				pressure5->muVary = threshold / inputSense;
				pressure5->muAttack = sqrt(fabs(pressure5->muSpeedA));
				pressure5->muCoefficientA = pressure5->muCoefficientA * (pressure5->muAttack - 1.0);
				if (pressure5->muVary < threshold) {
					pressure5->muCoefficientA = pressure5->muCoefficientA + threshold;
				} else {
					pressure5->muCoefficientA = pressure5->muCoefficientA + pressure5->muVary;
				}
				pressure5->muCoefficientA = pressure5->muCoefficientA / pressure5->muAttack;
			} else {
				pressure5->muCoefficientA = pressure5->muCoefficientA * ((pressure5->muSpeedA * pressure5->muSpeedA) - 1.0);
				pressure5->muCoefficientA = pressure5->muCoefficientA + 1.0;
				pressure5->muCoefficientA = pressure5->muCoefficientA / (pressure5->muSpeedA * pressure5->muSpeedA);
			}
			pressure5->muNewSpeed = pressure5->muSpeedA * (pressure5->muSpeedA - 1);
			pressure5->muNewSpeed = pressure5->muNewSpeed + fabs(inputSense * release) + fastest;
			pressure5->muSpeedA = pressure5->muNewSpeed / pressure5->muSpeedA;
		} else {
			if (inputSense > threshold) {
				pressure5->muVary = threshold / inputSense;
				pressure5->muAttack = sqrt(fabs(pressure5->muSpeedB));
				pressure5->muCoefficientB = pressure5->muCoefficientB * (pressure5->muAttack - 1);
				if (pressure5->muVary < threshold) {
					pressure5->muCoefficientB = pressure5->muCoefficientB + threshold;
				} else {
					pressure5->muCoefficientB = pressure5->muCoefficientB + pressure5->muVary;
				}
				pressure5->muCoefficientB = pressure5->muCoefficientB / pressure5->muAttack;
			} else {
				pressure5->muCoefficientB = pressure5->muCoefficientB * ((pressure5->muSpeedB * pressure5->muSpeedB) - 1.0);
				pressure5->muCoefficientB = pressure5->muCoefficientB + 1.0;
				pressure5->muCoefficientB = pressure5->muCoefficientB / (pressure5->muSpeedB * pressure5->muSpeedB);
			}
			pressure5->muNewSpeed = pressure5->muSpeedB * (pressure5->muSpeedB - 1);
			pressure5->muNewSpeed = pressure5->muNewSpeed + fabs(inputSense * release) + fastest;
			pressure5->muSpeedB = pressure5->muNewSpeed / pressure5->muSpeedB;
		}
		// got coefficients, adjusted speeds

		double coefficient;
		if (pressure5->flip) {
			if (positivemu) coefficient = pow(pressure5->muCoefficientA, 2);
			else coefficient = sqrt(pressure5->muCoefficientA);
			coefficient = (coefficient * mewiness) + (pressure5->muCoefficientA * (1.0 - mewiness));
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		} else {
			if (positivemu) coefficient = pow(pressure5->muCoefficientB, 2);
			else coefficient = sqrt(pressure5->muCoefficientB);
			coefficient = (coefficient * mewiness) + (pressure5->muCoefficientB * (1.0 - mewiness));
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		// applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		// applied gain correction to control output level- tends to constrain sound rather than inflate it

		if (outputGain != 1.0) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}

		pressure5->flip = !pressure5->flip;

		if (pressure5->fixB[fix_freq] < 0.49999) {
			double temp = (inputSampleL * pressure5->fixB[fix_a0]) + pressure5->fixB[fix_sL1];
			pressure5->fixB[fix_sL1] = (inputSampleL * pressure5->fixB[fix_a1]) - (temp * pressure5->fixB[fix_b1]) + pressure5->fixB[fix_sL2];
			pressure5->fixB[fix_sL2] = (inputSampleL * pressure5->fixB[fix_a2]) - (temp * pressure5->fixB[fix_b2]);
			inputSampleL = temp;
			temp = (inputSampleR * pressure5->fixB[fix_a0]) + pressure5->fixB[fix_sR1];
			pressure5->fixB[fix_sR1] = (inputSampleR * pressure5->fixB[fix_a1]) - (temp * pressure5->fixB[fix_b1]) + pressure5->fixB[fix_sR2];
			pressure5->fixB[fix_sR2] = (inputSampleR * pressure5->fixB[fix_a2]) - (temp * pressure5->fixB[fix_b2]);
			inputSampleR = temp; // fixed biquad filtering ultrasonics between Pressure and ClipOnly
		}

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}
		// Dry/Wet control, BEFORE ClipOnly

		pressure5->slewMax = fabs(inputSampleL - pressure5->lastSampleL);
		if (pressure5->slewMax < fabs(inputSampleR - pressure5->lastSampleR)) pressure5->slewMax = fabs(inputSampleR - pressure5->lastSampleR);
		// set up for fiddling with mewiness. Largest amount of slew in any direction

		// begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (pressure5->wasPosClipL == true) { // current will be over
			if (inputSampleL < pressure5->lastSampleL) pressure5->lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
			else pressure5->lastSampleL = 0.2491717 + (pressure5->lastSampleL * 0.7390851);
		}
		pressure5->wasPosClipL = false;
		if (inputSampleL > 0.9549925859) {
			pressure5->wasPosClipL = true;
			inputSampleL = 0.7058208 + (pressure5->lastSampleL * 0.2609148);
		}
		if (pressure5->wasNegClipL == true) { // current will be -over
			if (inputSampleL > pressure5->lastSampleL) pressure5->lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
			else pressure5->lastSampleL = -0.2491717 + (pressure5->lastSampleL * 0.7390851);
		}
		pressure5->wasNegClipL = false;
		if (inputSampleL < -0.9549925859) {
			pressure5->wasNegClipL = true;
			inputSampleL = -0.7058208 + (pressure5->lastSampleL * 0.2609148);
		}
		pressure5->intermediateL[spacing] = inputSampleL;
		inputSampleL = pressure5->lastSampleL; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) pressure5->intermediateL[x - 1] = pressure5->intermediateL[x];
		pressure5->lastSampleL = pressure5->intermediateL[0]; // run a little buffer to handle this

		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (pressure5->wasPosClipR == true) { // current will be over
			if (inputSampleR < pressure5->lastSampleR) pressure5->lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
			else pressure5->lastSampleR = 0.2491717 + (pressure5->lastSampleR * 0.7390851);
		}
		pressure5->wasPosClipR = false;
		if (inputSampleR > 0.9549925859) {
			pressure5->wasPosClipR = true;
			inputSampleR = 0.7058208 + (pressure5->lastSampleR * 0.2609148);
		}
		if (pressure5->wasNegClipR == true) { // current will be -over
			if (inputSampleR > pressure5->lastSampleR) pressure5->lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
			else pressure5->lastSampleR = -0.2491717 + (pressure5->lastSampleR * 0.7390851);
		}
		pressure5->wasNegClipR = false;
		if (inputSampleR < -0.9549925859) {
			pressure5->wasNegClipR = true;
			inputSampleR = -0.7058208 + (pressure5->lastSampleR * 0.2609148);
		}
		pressure5->intermediateR[spacing] = inputSampleR;
		inputSampleR = pressure5->lastSampleR; // Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) pressure5->intermediateR[x - 1] = pressure5->intermediateR[x];
		pressure5->lastSampleR = pressure5->intermediateR[0]; // run a little buffer to handle this
		// end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		// final clip runs AFTER the Dry/Wet. It serves as a safety clip even if you're not full wet

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pressure5->fpdL ^= pressure5->fpdL << 13;
		pressure5->fpdL ^= pressure5->fpdL >> 17;
		pressure5->fpdL ^= pressure5->fpdL << 5;
		inputSampleL += (((double) pressure5->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pressure5->fpdR ^= pressure5->fpdR << 13;
		pressure5->fpdR ^= pressure5->fpdR >> 17;
		pressure5->fpdR ^= pressure5->fpdR << 5;
		inputSampleR += (((double) pressure5->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	PRESSURE5_URI,
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
