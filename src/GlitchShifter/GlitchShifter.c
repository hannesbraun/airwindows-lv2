#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define GLITCHSHIFTER_URI "https://hannesbraun.net/ns/lv2/airwindows/glitchshifter"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	NOTE = 4,
	TRIM = 5,
	TIGHTEN = 6,
	FEEDBACK = 7,
	DRY_WET = 8
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* note;
	const float* trim;
	const float* tighten;
	const float* feedback;
	const float* dryWet;

	int32_t pL[131076];
	int32_t offsetL[258];
	int32_t pastzeroL[258];
	int32_t previousL[258];
	int32_t thirdL[258];
	int32_t fourthL[258];
	int32_t tempL;
	int32_t lasttempL;
	int32_t thirdtempL;
	int32_t fourthtempL;
	int32_t sincezerocrossL;
	int crossesL;
	int realzeroesL;
	double positionL;
	bool splicingL;

	double airPrevL;
	double airEvenL;
	double airOddL;
	double airFactorL;

	int32_t pR[131076];
	int32_t offsetR[258];
	int32_t pastzeroR[258];
	int32_t previousR[258];
	int32_t thirdR[258];
	int32_t fourthR[258];
	int32_t tempR;
	int32_t lasttempR;
	int32_t thirdtempR;
	int32_t fourthtempR;
	int32_t sincezerocrossR;
	int crossesR;
	int realzeroesR;
	double positionR;
	bool splicingR;

	double airPrevR;
	double airEvenR;
	double airOddR;
	double airFactorR;

	int gcount;
	int32_t lastwidth;
	bool flip;
	uint32_t fpdL;
	uint32_t fpdR;
} GlitchShifter;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	GlitchShifter* glitchShifter = (GlitchShifter*) calloc(1, sizeof(GlitchShifter));
	return (LV2_Handle) glitchShifter;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	GlitchShifter* glitchShifter = (GlitchShifter*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			glitchShifter->input[0] = (const float*) data;
			break;
		case INPUT_R:
			glitchShifter->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			glitchShifter->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			glitchShifter->output[1] = (float*) data;
			break;
		case NOTE:
			glitchShifter->note = (const float*) data;
			break;
		case TRIM:
			glitchShifter->trim = (const float*) data;
			break;
		case TIGHTEN:
			glitchShifter->tighten = (const float*) data;
			break;
		case FEEDBACK:
			glitchShifter->feedback = (const float*) data;
			break;
		case DRY_WET:
			glitchShifter->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	GlitchShifter* glitchShifter = (GlitchShifter*) instance;

	for (int count = 0; count < 131074; count++) {
		glitchShifter->pL[count] = 0;
		glitchShifter->pR[count] = 0;
	}
	for (int count = 0; count < 257; count++) {
		glitchShifter->offsetL[count] = 0;
		glitchShifter->pastzeroL[count] = 0;
		glitchShifter->previousL[count] = 0;
		glitchShifter->thirdL[count] = 0;
		glitchShifter->fourthL[count] = 0;
		glitchShifter->offsetR[count] = 0;
		glitchShifter->pastzeroR[count] = 0;
		glitchShifter->previousR[count] = 0;
		glitchShifter->thirdR[count] = 0;
		glitchShifter->fourthR[count] = 0;
	}
	glitchShifter->crossesL = 0;
	glitchShifter->realzeroesL = 0;
	glitchShifter->tempL = 0;
	glitchShifter->lasttempL = 0;
	glitchShifter->thirdtempL = 0;
	glitchShifter->fourthtempL = 0;
	glitchShifter->sincezerocrossL = 0;
	glitchShifter->airPrevL = 0.0;
	glitchShifter->airEvenL = 0.0;
	glitchShifter->airOddL = 0.0;
	glitchShifter->airFactorL = 0.0;
	glitchShifter->positionL = 0.0;
	glitchShifter->splicingL = false;

	glitchShifter->crossesR = 0;
	glitchShifter->realzeroesR = 0;
	glitchShifter->tempR = 0;
	glitchShifter->lasttempR = 0;
	glitchShifter->thirdtempR = 0;
	glitchShifter->fourthtempR = 0;
	glitchShifter->sincezerocrossR = 0;
	glitchShifter->airPrevR = 0.0;
	glitchShifter->airEvenR = 0.0;
	glitchShifter->airOddR = 0.0;
	glitchShifter->airFactorR = 0.0;
	glitchShifter->positionR = 0.0;
	glitchShifter->splicingR = false;

	glitchShifter->gcount = 0;
	glitchShifter->lastwidth = 16386;
	glitchShifter->flip = false;

	glitchShifter->fpdL = 1.0;
	while (glitchShifter->fpdL < 16386) glitchShifter->fpdL = rand() * UINT32_MAX;
	glitchShifter->fpdR = 1.0;
	while (glitchShifter->fpdR < 16386) glitchShifter->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	GlitchShifter* glitchShifter = (GlitchShifter*) instance;

	const float* in1 = glitchShifter->input[0];
	const float* in2 = glitchShifter->input[1];
	float* out1 = glitchShifter->output[0];
	float* out2 = glitchShifter->output[1];

	int note = (int) *glitchShifter->note;
	double trim = *glitchShifter->trim;
	double speed = ((1.0 / 12.0) * note) + trim;
	if (speed < 0) speed *= 0.5;
	// include sanity check- maximum pitch lowering cannot be to 0 hz.
	int width = (int) (65536 - ((1 - pow(1 - *glitchShifter->tighten, 2)) * 65530.0));
	double bias = pow(*glitchShifter->tighten, 3);
	double feedback = *glitchShifter->feedback / 1.5;
	double wet = *glitchShifter->dryWet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = glitchShifter->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = glitchShifter->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		glitchShifter->airFactorL = glitchShifter->airPrevL - inputSampleL;
		if (glitchShifter->flip) {
			glitchShifter->airEvenL += glitchShifter->airFactorL;
			glitchShifter->airOddL -= glitchShifter->airFactorL;
			glitchShifter->airFactorL = glitchShifter->airEvenL;
		} else {
			glitchShifter->airOddL += glitchShifter->airFactorL;
			glitchShifter->airEvenL -= glitchShifter->airFactorL;
			glitchShifter->airFactorL = glitchShifter->airOddL;
		}
		glitchShifter->airOddL = (glitchShifter->airOddL - ((glitchShifter->airOddL - glitchShifter->airEvenL) / 256.0)) / 1.0001;
		glitchShifter->airEvenL = (glitchShifter->airEvenL - ((glitchShifter->airEvenL - glitchShifter->airOddL) / 256.0)) / 1.0001;
		glitchShifter->airPrevL = inputSampleL;
		inputSampleL += glitchShifter->airFactorL;

		glitchShifter->airFactorR = glitchShifter->airPrevR - inputSampleR;
		if (glitchShifter->flip) {
			glitchShifter->airEvenR += glitchShifter->airFactorR;
			glitchShifter->airOddR -= glitchShifter->airFactorR;
			glitchShifter->airFactorR = glitchShifter->airEvenR;
		} else {
			glitchShifter->airOddR += glitchShifter->airFactorR;
			glitchShifter->airEvenR -= glitchShifter->airFactorR;
			glitchShifter->airFactorR = glitchShifter->airOddR;
		}
		glitchShifter->airOddR = (glitchShifter->airOddR - ((glitchShifter->airOddR - glitchShifter->airEvenR) / 256.0)) / 1.0001;
		glitchShifter->airEvenR = (glitchShifter->airEvenR - ((glitchShifter->airEvenR - glitchShifter->airOddR) / 256.0)) / 1.0001;
		glitchShifter->airPrevR = inputSampleR;
		inputSampleR += glitchShifter->airFactorR;

		glitchShifter->flip = !glitchShifter->flip;
		// air, compensates for loss of highs of interpolation

		if (glitchShifter->lastwidth != width) {
			glitchShifter->crossesL = 0;
			glitchShifter->realzeroesL = 0;
			glitchShifter->crossesR = 0;
			glitchShifter->realzeroesR = 0;
			glitchShifter->lastwidth = width;
		}
		// global: changing this resets both channels

		glitchShifter->gcount++;
		if (glitchShifter->gcount < 0 || glitchShifter->gcount > width) {
			glitchShifter->gcount = 0;
		}
		int count = glitchShifter->gcount;
		int countone = count - 1;
		int counttwo = count - 2;
		while (count < 0) {
			count += width;
		}
		while (countone < 0) {
			countone += width;
		}
		while (counttwo < 0) {
			counttwo += width;
		}
		while (count > width) {
			count -= width;
		} // this can only happen with very insane variables
		while (countone > width) {
			countone -= width;
		}
		while (counttwo > width) {
			counttwo -= width;
		}
		// yay sanity checks
		// now we have counts zero, one, two, none of which have sanity checked values
		// we are tracking most recent samples and must SUBTRACT.
		// this is a wrap on the overall buffers, so count, one and two are also common to both channels

		glitchShifter->pL[count + width] = glitchShifter->pL[count] = (int) ((inputSampleL * 8388352.0) + (double) (glitchShifter->lasttempL * feedback));
		glitchShifter->pR[count + width] = glitchShifter->pR[count] = (int) ((inputSampleR * 8388352.0) + (double) (glitchShifter->lasttempR * feedback));
		// double buffer -8388352 to 8388352 is equal to 24 bit linear space

		if ((glitchShifter->pL[countone] > 0 && glitchShifter->pL[count] < 0) || (glitchShifter->pL[countone] < 0 && glitchShifter->pL[count] > 0)) // source crossed zero
		{
			glitchShifter->crossesL++;
			glitchShifter->realzeroesL++;
			if (glitchShifter->crossesL > 256) {
				glitchShifter->crossesL = 0;
			} // wrap crosses to keep adding new crosses
			if (glitchShifter->realzeroesL > 256) {
				glitchShifter->realzeroesL = 256;
			} // don't wrap realzeroes, full buffer, use all
			glitchShifter->offsetL[glitchShifter->crossesL] = count;
			glitchShifter->pastzeroL[glitchShifter->crossesL] = glitchShifter->pL[count];
			glitchShifter->previousL[glitchShifter->crossesL] = glitchShifter->pL[countone];
			glitchShifter->thirdL[glitchShifter->crossesL] = glitchShifter->pL[counttwo];
			// we load the zero crosses register with crosses to examine
		} // we just put in a source zero cross in the registry

		if ((glitchShifter->pR[countone] > 0 && glitchShifter->pR[count] < 0) || (glitchShifter->pR[countone] < 0 && glitchShifter->pR[count] > 0)) // source crossed zero
		{
			glitchShifter->crossesR++;
			glitchShifter->realzeroesR++;
			if (glitchShifter->crossesR > 256) {
				glitchShifter->crossesR = 0;
			} // wrap crosses to keep adding new crosses
			if (glitchShifter->realzeroesR > 256) {
				glitchShifter->realzeroesR = 256;
			} // don't wrap realzeroes, full buffer, use all
			glitchShifter->offsetR[glitchShifter->crossesR] = count;
			glitchShifter->pastzeroR[glitchShifter->crossesR] = glitchShifter->pR[count];
			glitchShifter->previousR[glitchShifter->crossesR] = glitchShifter->pR[countone];
			glitchShifter->thirdR[glitchShifter->crossesR] = glitchShifter->pR[counttwo];
			// we load the zero crosses register with crosses to examine
		} // we just put in a source zero cross in the registry
		// in this we don't update count at all, so we can run them one after another because this is
		// feeding the system, not tracking the output of two parallel but non-matching output taps

		glitchShifter->positionL -= speed; // this is individual to each channel!

		if (glitchShifter->positionL > width) { // we just caught up to the buffer end
			if (glitchShifter->realzeroesL > 0) { // we just caught up to the buffer end with zero crosses in the bin
				glitchShifter->positionL = 0;
				double diff = 99999999.0;
				int best = 0; // these can be local, I think
				int scan;
				for (scan = (glitchShifter->realzeroesL - 1); scan >= 0; scan--) {
					int scanone = scan + glitchShifter->crossesL;
					if (scanone > 256) {
						scanone -= 256;
					}
					// try to track the real most recent ones more closely
					double howdiff = (double) ((glitchShifter->tempL - glitchShifter->pastzeroL[scanone]) + (glitchShifter->lasttempL - glitchShifter->previousL[scanone]) + (glitchShifter->thirdtempL - glitchShifter->thirdL[scanone]) + (glitchShifter->fourthtempL - glitchShifter->fourthL[scanone]));
					// got difference factor between things
					howdiff -= (double) (scan * bias);
					// try to bias in favor of more recent crosses
					if (howdiff < diff) {
						diff = howdiff;
						best = scanone;
					}
				} // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				glitchShifter->positionL = glitchShifter->offsetL[best] - glitchShifter->sincezerocrossL;
				glitchShifter->crossesL = 0;
				glitchShifter->realzeroesL = 0;
				glitchShifter->splicingL = true; // we just kicked the delay tap back, changing positionL
			} else { // we just caught up to the buffer end with no crosses- glitch speeds.
				glitchShifter->positionL -= width;
				glitchShifter->crossesL = 0;
				glitchShifter->realzeroesL = 0;
				glitchShifter->splicingL = true; // so, hard splice it.
			}
		}

		if (glitchShifter->positionL < 0) { // we just caught up to the dry tap.
			if (glitchShifter->realzeroesL > 0) { // we just caught up to the dry tap with zero crosses in the bin
				glitchShifter->positionL = 0;
				double diff = 99999999.0;
				int best = 0; // these can be local, I think
				int scan;
				for (scan = (glitchShifter->realzeroesL - 1); scan >= 0; scan--) {
					int scanone = scan + glitchShifter->crossesL;
					if (scanone > 256) {
						scanone -= 256;
					}
					// try to track the real most recent ones more closely
					double howdiff = (double) ((glitchShifter->tempL - glitchShifter->pastzeroL[scanone]) + (glitchShifter->lasttempL - glitchShifter->previousL[scanone]) + (glitchShifter->thirdtempL - glitchShifter->thirdL[scanone]) + (glitchShifter->fourthtempL - glitchShifter->fourthL[scanone]));
					// got difference factor between things
					howdiff -= (double) (scan * bias);
					// try to bias in favor of more recent crosses
					if (howdiff < diff) {
						diff = howdiff;
						best = scanone;
					}
				} // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				glitchShifter->positionL = glitchShifter->offsetL[best] - glitchShifter->sincezerocrossL;
				glitchShifter->crossesL = 0;
				glitchShifter->realzeroesL = 0;
				glitchShifter->splicingL = true; // we just kicked the delay tap back, changing positionL
			} else { // we just caught up to the dry tap with no crosses- glitch speeds.
				glitchShifter->positionL += width;
				glitchShifter->crossesL = 0;
				glitchShifter->realzeroesL = 0;
				glitchShifter->splicingL = true; // so, hard splice it.
			}
		}

		glitchShifter->positionR -= speed; // this is individual to each channel!

		if (glitchShifter->positionR > width) { // we just caught up to the buffer end
			if (glitchShifter->realzeroesR > 0) { // we just caught up to the buffer end with zero crosses in the bin
				glitchShifter->positionR = 0;
				double diff = 99999999.0;
				int best = 0; // these can be local, I think
				int scan;
				for (scan = (glitchShifter->realzeroesR - 1); scan >= 0; scan--) {
					int scanone = scan + glitchShifter->crossesR;
					if (scanone > 256) {
						scanone -= 256;
					}
					// try to track the real most recent ones more closely
					double howdiff = (double) ((glitchShifter->tempR - glitchShifter->pastzeroR[scanone]) + (glitchShifter->lasttempR - glitchShifter->previousR[scanone]) + (glitchShifter->thirdtempR - glitchShifter->thirdR[scanone]) + (glitchShifter->fourthtempR - glitchShifter->fourthR[scanone]));
					// got difference factor between things
					howdiff -= (double) (scan * bias);
					// try to bias in favor of more recent crosses
					if (howdiff < diff) {
						diff = howdiff;
						best = scanone;
					}
				} // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				glitchShifter->positionR = glitchShifter->offsetR[best] - glitchShifter->sincezerocrossR;
				glitchShifter->crossesR = 0;
				glitchShifter->realzeroesR = 0;
				glitchShifter->splicingR = true; // we just kicked the delay tap back, changing positionL
			} else { // we just caught up to the buffer end with no crosses- glitch speeds.
				glitchShifter->positionR -= width;
				glitchShifter->crossesR = 0;
				glitchShifter->realzeroesR = 0;
				glitchShifter->splicingR = true; // so, hard splice it.
			}
		}

		if (glitchShifter->positionR < 0) { // we just caught up to the dry tap.
			if (glitchShifter->realzeroesR > 0) { // we just caught up to the dry tap with zero crosses in the bin
				glitchShifter->positionR = 0;
				double diff = 99999999.0;
				int best = 0; // these can be local, I think
				int scan;
				for (scan = (glitchShifter->realzeroesR - 1); scan >= 0; scan--) {
					int scanone = scan + glitchShifter->crossesR;
					if (scanone > 256) {
						scanone -= 256;
					}
					// try to track the real most recent ones more closely
					double howdiff = (double) ((glitchShifter->tempR - glitchShifter->pastzeroR[scanone]) + (glitchShifter->lasttempR - glitchShifter->previousR[scanone]) + (glitchShifter->thirdtempR - glitchShifter->thirdR[scanone]) + (glitchShifter->fourthtempR - glitchShifter->fourthR[scanone]));
					// got difference factor between things
					howdiff -= (double) (scan * bias);
					// try to bias in favor of more recent crosses
					if (howdiff < diff) {
						diff = howdiff;
						best = scanone;
					}
				} // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				glitchShifter->positionR = glitchShifter->offsetR[best] - glitchShifter->sincezerocrossR;
				glitchShifter->crossesR = 0;
				glitchShifter->realzeroesR = 0;
				glitchShifter->splicingR = true; // we just kicked the delay tap back, changing positionL
			} else { // we just caught up to the dry tap with no crosses- glitch speeds.
				glitchShifter->positionR += width;
				glitchShifter->crossesR = 0;
				glitchShifter->realzeroesR = 0;
				glitchShifter->splicingR = true; // so, hard splice it.
			}
		}

		count = glitchShifter->gcount - (int) floor(glitchShifter->positionL);
		// we go back because the buffer goes forward this time
		countone = count + 1;
		counttwo = count + 2;
		// now we have counts zero, one, two, none of which have sanity checked values
		// we are interpolating, we ADD
		while (count < 0) {
			count += width;
		}
		while (countone < 0) {
			countone += width;
		}
		while (counttwo < 0) {
			counttwo += width;
		}
		while (count > width) {
			count -= width;
		} // this can only happen with very insane variables
		while (countone > width) {
			countone -= width;
		}
		while (counttwo > width) {
			counttwo -= width;
		}

		// here's where we do our shift against the rotating buffer
		glitchShifter->tempL = 0;
		glitchShifter->tempL += (int) (glitchShifter->pL[count] * (1 - (glitchShifter->positionL - floor(glitchShifter->positionL)))); // less as value moves away from .0
		glitchShifter->tempL += glitchShifter->pL[count + 1]; // we can assume always using this in one way or another?
		glitchShifter->tempL += (int) (glitchShifter->pL[count + 2] * (glitchShifter->positionL - floor(glitchShifter->positionL))); // greater as value moves away from .0
		glitchShifter->tempL -= (int) (((glitchShifter->pL[count] - glitchShifter->pL[count + 1]) - (glitchShifter->pL[count + 1] - glitchShifter->pL[count + 2])) / 50); // interpolation hacks 'r us
		glitchShifter->tempL /= 2; // gotta make temp be the same level scale as buffer
		// now we have our delay tap, which is going to do our pitch shifting
		if (abs(glitchShifter->tempL) > 8388352.0) {
			glitchShifter->tempL = (glitchShifter->lasttempL + (glitchShifter->lasttempL - glitchShifter->thirdtempL));
		}
		// kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*

		glitchShifter->sincezerocrossL++;
		if (glitchShifter->sincezerocrossL < 0 || glitchShifter->sincezerocrossL > width) {
			glitchShifter->sincezerocrossL = 0;
		} // just a sanity check
		if (glitchShifter->splicingL) {
			glitchShifter->tempL = (glitchShifter->tempL + (glitchShifter->lasttempL + (glitchShifter->lasttempL - glitchShifter->thirdtempL))) / 2;
			glitchShifter->splicingL = false;
		}
		// do a smoother transition by taking the sample of transition and going half with it

		if ((glitchShifter->lasttempL > 0 && glitchShifter->tempL < 0) || (glitchShifter->lasttempL < 0 && glitchShifter->tempL > 0)) // delay tap crossed zero
		{
			glitchShifter->sincezerocrossL = 0;
		} // we just restarted counting from the delay tap zero cross

		count = glitchShifter->gcount - (int) floor(glitchShifter->positionR);
		// we go back because the buffer goes forward this time
		countone = count + 1;
		counttwo = count + 2;
		// now we have counts zero, one, two, none of which have sanity checked values
		// we are interpolating, we ADD
		while (count < 0) {
			count += width;
		}
		while (countone < 0) {
			countone += width;
		}
		while (counttwo < 0) {
			counttwo += width;
		}
		while (count > width) {
			count -= width;
		} // this can only happen with very insane variables
		while (countone > width) {
			countone -= width;
		}
		while (counttwo > width) {
			counttwo -= width;
		}

		glitchShifter->tempR = 0;
		glitchShifter->tempR += (int) (glitchShifter->pR[count] * (1 - (glitchShifter->positionR - floor(glitchShifter->positionR)))); // less as value moves away from .0
		glitchShifter->tempR += glitchShifter->pR[count + 1]; // we can assume always using this in one way or another?
		glitchShifter->tempR += (int) (glitchShifter->pR[count + 2] * (glitchShifter->positionR - floor(glitchShifter->positionR))); // greater as value moves away from .0
		glitchShifter->tempR -= (int) (((glitchShifter->pR[count] - glitchShifter->pR[count + 1]) - (glitchShifter->pR[count + 1] - glitchShifter->pR[count + 2])) / 50); // interpolation hacks 'r us
		glitchShifter->tempR /= 2; // gotta make temp be the same level scale as buffer
		// now we have our delay tap, which is going to do our pitch shifting
		if (abs(glitchShifter->tempR) > 8388352.0) {
			glitchShifter->tempR = (glitchShifter->lasttempR + (glitchShifter->lasttempR - glitchShifter->thirdtempR));
		}
		// kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*

		glitchShifter->sincezerocrossR++;
		if (glitchShifter->sincezerocrossR < 0 || glitchShifter->sincezerocrossR > width) {
			glitchShifter->sincezerocrossR = 0;
		} // just a sanity check
		if (glitchShifter->splicingR) {
			glitchShifter->tempR = (glitchShifter->tempR + (glitchShifter->lasttempR + (glitchShifter->lasttempR - glitchShifter->thirdtempR))) / 2;
			glitchShifter->splicingR = false;
		}
		// do a smoother transition by taking the sample of transition and going half with it

		if ((glitchShifter->lasttempR > 0 && glitchShifter->tempR < 0) || (glitchShifter->lasttempR < 0 && glitchShifter->tempR > 0)) // delay tap crossed zero
		{
			glitchShifter->sincezerocrossR = 0;
		} // we just restarted counting from the delay tap zero cross

		glitchShifter->fourthtempL = glitchShifter->thirdtempL;
		glitchShifter->thirdtempL = glitchShifter->lasttempL;
		glitchShifter->lasttempL = glitchShifter->tempL;

		glitchShifter->fourthtempR = glitchShifter->thirdtempR;
		glitchShifter->thirdtempR = glitchShifter->lasttempR;
		glitchShifter->lasttempR = glitchShifter->tempR;

		inputSampleL = (drySampleL * (1 - wet)) + ((double) (glitchShifter->tempL / (8388352.0)) * wet);
		if (inputSampleL > 4.0) inputSampleL = 4.0;
		if (inputSampleL < -4.0) inputSampleL = -4.0;

		inputSampleR = (drySampleR * (1 - wet)) + ((double) (glitchShifter->tempR / (8388352.0)) * wet);
		if (inputSampleR > 4.0) inputSampleR = 4.0;
		if (inputSampleR < -4.0) inputSampleR = -4.0;
		// this plugin can throw insane outputs so we'll put in a hard clip

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		glitchShifter->fpdL ^= glitchShifter->fpdL << 13;
		glitchShifter->fpdL ^= glitchShifter->fpdL >> 17;
		glitchShifter->fpdL ^= glitchShifter->fpdL << 5;
		inputSampleL += (((double) glitchShifter->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		glitchShifter->fpdR ^= glitchShifter->fpdR << 13;
		glitchShifter->fpdR ^= glitchShifter->fpdR >> 17;
		glitchShifter->fpdR ^= glitchShifter->fpdR << 5;
		inputSampleR += (((double) glitchShifter->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	GLITCHSHIFTER_URI,
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
