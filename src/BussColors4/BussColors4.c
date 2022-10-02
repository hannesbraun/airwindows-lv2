#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define BUSSCOLORS4_URI "https://hannesbraun.net/ns/lv2/airwindows/busscolors4"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	COLOR = 4,
	INPUT_TRIM = 5,
	OUTPUT_TRIM = 6,
	DRY_WET = 7
} PortIndex;

typedef struct {
	double sampleRate;
	const float* input[2];
	float* output[2];
	const float* color;
	const float* inputTrim;
	const float* outputTrim;
	const float* dryWet;

	double bL[175]; // full buffer for high sample rates. Scales to 192K
	double bR[175]; // full buffer for high sample rates. Scales to 192K
	double dL[100]; // buffer for calculating sag as it relates to the dynamic impulse synthesis. To 192K.
	double dR[100]; // buffer for calculating sag as it relates to the dynamic impulse synthesis. To 192K.
	int c[35]; // just the number of taps we use, doesn't have to scale
	double g[9]; // console model
	double outg[9]; // console model
	double controlL;
	double controlR;
	double slowdynL;
	double slowdynR;
	int gcount;

	uint32_t fpdL;
	uint32_t fpdR;
} BussColors4;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BussColors4* bussColors4 = (BussColors4*) calloc(1, sizeof(BussColors4));
	bussColors4->sampleRate = rate;
	return (LV2_Handle) bussColors4;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BussColors4* bussColors4 = (BussColors4*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			bussColors4->input[0] = (const float*) data;
			break;
		case INPUT_R:
			bussColors4->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			bussColors4->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			bussColors4->output[1] = (float*) data;
			break;
		case COLOR:
			bussColors4->color = (const float*) data;
			break;
		case INPUT_TRIM:
			bussColors4->inputTrim = (const float*) data;
			break;
		case OUTPUT_TRIM:
			bussColors4->outputTrim = (const float*) data;
			break;
		case DRY_WET:
			bussColors4->dryWet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	BussColors4* bussColors4 = (BussColors4*) instance;

	for (int count = 0; count < 174; count++) {
		bussColors4->bL[count] = 0;
		bussColors4->bR[count] = 0;
	}
	for (int count = 0; count < 99; count++) {
		bussColors4->dL[count] = 0;
		bussColors4->dR[count] = 0;
	}
	for (int count = 0; count < 34; count++) bussColors4->c[count] = count; // initial setup for 44.1K
	bussColors4->g[1] = pow(10.0, -5.2 / 14.0); // dark
	bussColors4->g[2] = pow(10.0, -6.2 / 14.0); // rock
	bussColors4->g[3] = pow(10.0, -2.9 / 14.0); // lush
	bussColors4->g[4] = pow(10.0, -1.1 / 14.0); // vibe
	bussColors4->g[5] = pow(10.0, -5.1 / 14.0); // holo
	bussColors4->g[6] = pow(10.0, -3.6 / 14.0); // punch
	bussColors4->g[7] = pow(10.0, -2.3 / 14.0); // steel
	bussColors4->g[8] = pow(10.0, -2.9 / 14.0); // tube
	// preset gains for models
	bussColors4->outg[1] = pow(10.0, -0.3 / 14.0); // dark
	bussColors4->outg[2] = pow(10.0, 0.5 / 14.0); // rock
	bussColors4->outg[3] = pow(10.0, -0.7 / 14.0); // lush
	bussColors4->outg[4] = pow(10.0, -0.6 / 14.0); // vibe
	bussColors4->outg[5] = pow(10.0, -0.2 / 14.0); // holo
	bussColors4->outg[6] = pow(10.0, 0.3 / 14.0); // punch
	bussColors4->outg[7] = pow(10.0, 0.1 / 14.0); // steel
	bussColors4->outg[8] = pow(10.0, 0.9 / 14.0); // tube
	// preset gains for models
	bussColors4->controlL = 0;
	bussColors4->controlR = 0;
	bussColors4->slowdynL = 0;
	bussColors4->slowdynR = 0;
	bussColors4->gcount = 0;

	bussColors4->fpdL = 1.0;
	while (bussColors4->fpdL < 16386) bussColors4->fpdL = rand() * UINT32_MAX;
	bussColors4->fpdR = 1.0;
	while (bussColors4->fpdR < 16386) bussColors4->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BussColors4* bussColors4 = (BussColors4*) instance;

	const float* in1 = bussColors4->input[0];
	const float* in2 = bussColors4->input[1];
	float* out1 = bussColors4->output[0];
	float* out2 = bussColors4->output[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= bussColors4->sampleRate;
	if (overallscale < 1.0) overallscale = 1.0;
	if (overallscale > 4.5) overallscale = 4.5;
	const int maxConvolutionBufferSize = (int) (34.0 * overallscale); // we won't use more of the buffer than we have to
	for (int count = 0; count < 34; count++) bussColors4->c[count] = (int) (count * overallscale); // assign conv taps
	double drySampleL;
	double drySampleR;
	double applyconvL;
	double applyconvR;
	int offsetA = 3;
	double dynamicconvL = 3.0;
	double dynamicconvR = 3.0;
	double gain = 0.436;
	double outgain = 1.0;
	double bridgerectifier;

	double inputSampleL;
	double inputSampleR;

	int console = (int) (*bussColors4->color);
	switch (console) {
		case 0:
			offsetA = 4;
			gain = bussColors4->g[1];
			outgain = bussColors4->outg[1];
			break; // Dark (Focusrite)
		case 1:
			offsetA = 3;
			gain = bussColors4->g[2];
			outgain = bussColors4->outg[2];
			break; // Rock (SSL)
		case 2:
			offsetA = 5;
			gain = bussColors4->g[3];
			outgain = bussColors4->outg[3];
			break; // Lush (Neve)
		case 3:
			offsetA = 8;
			gain = bussColors4->g[4];
			outgain = bussColors4->outg[4];
			break; // Vibe (Elation)
		case 4:
			offsetA = 5;
			gain = bussColors4->g[5];
			outgain = bussColors4->outg[5];
			break; // Holo (Precision 8)
		case 5:
			offsetA = 7;
			gain = bussColors4->g[6];
			outgain = bussColors4->outg[6];
			break; // Punch (API)
		case 6:
			offsetA = 7;
			gain = bussColors4->g[7];
			outgain = bussColors4->outg[7];
			break; // Steel (Calibre)
		case 7:
		case 8:
			offsetA = 6;
			gain = bussColors4->g[8];
			outgain = bussColors4->outg[8];
			break; // Tube (Manley)
	}
	offsetA = (int) (offsetA * overallscale); // we extend the sag buffer too, at high sample rates
	if (offsetA < 3) offsetA = 3; // are we getting divides by zero?
	gain *= pow(10.0, *bussColors4->inputTrim / 14.0); // add adjustment factor
	outgain *= pow(10.0, (*bussColors4->outputTrim + 3.3) / 14.0); // add adjustment factor
	double wet = *bussColors4->dryWet;
	// removed extra dry variable

	while (sampleFrames-- > 0) {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = bussColors4->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = bussColors4->fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;

		if (gain != 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}

		bridgerectifier = fabs(inputSampleL);
		bussColors4->slowdynL *= 0.999;
		bussColors4->slowdynL += bridgerectifier;
		if (bussColors4->slowdynL > 1.5) {
			bussColors4->slowdynL = 1.5;
		}
		// before the iron bar- fry console for crazy behavior
		dynamicconvL = 2.5 + bussColors4->slowdynL;

		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		// end pre saturation stage L

		bridgerectifier = fabs(inputSampleR);
		bussColors4->slowdynR *= 0.999;
		bussColors4->slowdynR += bridgerectifier;
		if (bussColors4->slowdynR > 1.5) {
			bussColors4->slowdynR = 1.5;
		}
		// before the iron bar- fry console for crazy behavior
		dynamicconvR = 2.5 + bussColors4->slowdynR;

		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		// end pre saturation stage R

		if (bussColors4->gcount < 0 || bussColors4->gcount > 44) bussColors4->gcount = 44;

		bussColors4->dL[bussColors4->gcount + 44] = bussColors4->dL[bussColors4->gcount] = fabs(inputSampleL);
		bussColors4->controlL += (bussColors4->dL[bussColors4->gcount] / offsetA);
		bussColors4->controlL -= (bussColors4->dL[bussColors4->gcount + offsetA] / offsetA);
		bussColors4->controlL -= 0.000001;
		if (bussColors4->controlL < 0) bussColors4->controlL = 0;
		if (bussColors4->controlL > 100) bussColors4->controlL = 100;
		applyconvL = (bussColors4->controlL / offsetA) * dynamicconvL;
		// now we have a 'sag' style average to apply to the conv, L

		bussColors4->dR[bussColors4->gcount + 44] = bussColors4->dR[bussColors4->gcount] = fabs(inputSampleR);
		bussColors4->controlR += (bussColors4->dR[bussColors4->gcount] / offsetA);
		bussColors4->controlR -= (bussColors4->dR[bussColors4->gcount + offsetA] / offsetA);
		bussColors4->controlR -= 0.000001;
		if (bussColors4->controlR < 0) bussColors4->controlR = 0;
		if (bussColors4->controlR > 100) bussColors4->controlR = 100;
		applyconvR = (bussColors4->controlR / offsetA) * dynamicconvR;
		// now we have a 'sag' style average to apply to the conv, R

		bussColors4->gcount--;

		// now the convolution
		for (int count = maxConvolutionBufferSize; count > 0; --count) {
			bussColors4->bL[count] = bussColors4->bL[count - 1];
		} // was 173
		// we're only doing assigns, and it saves us an add inside the convolution calculation
		// therefore, we'll just assign everything one step along and have our buffer that way.
		bussColors4->bL[0] = inputSampleL;

		for (int count = maxConvolutionBufferSize; count > 0; --count) {
			bussColors4->bR[count] = bussColors4->bR[count - 1];
		} // was 173
		// we're only doing assigns, and it saves us an add inside the convolution calculation
		// therefore, we'll just assign everything one step along and have our buffer that way.
		bussColors4->bR[0] = inputSampleR;
		// The reason these are separate is, since it's just a big assign-fest, it's possible compilers can
		// come up with a clever way to do it. Interleaving the samples might make it enough more complicated
		// that the compiler wouldn't know to do 'em in batches or whatever. Just a thought, profiling would
		// be the correct way to find out this (or indeed, whether doing another add insode the convolutions would
		// be the best bet,

		// The convolutions!

		switch (console) {
			case 0:
				// begin Cider (Focusrite) MCI
				inputSampleL += (bussColors4->bL[bussColors4->c[1]] * (0.61283288942201319 + (0.00024011410669522 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[2]] * (0.24036380659761222 - (0.00020789518206241 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[3]] * (0.09104669761717916 + (0.00012829642741548 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[4]] * (0.02378290768554025 - (0.00017673646470440 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[5]] * (0.02832818490275965 - (0.00013536187747384 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[6]] * (0.03268797679215937 + (0.00015035126653359 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[7]] * (0.04024464202655586 - (0.00015034923056735 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[8]] * (0.01864890074318696 + (0.00014513281680642 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[9]] * (0.01632731954100322 - (0.00015509089075614 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[10]] * (0.00318907090555589 - (0.00014784812076550 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[11]] * (0.00208573465221869 - (0.00015350520779465 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[12]] * (0.00907033901519614 - (0.00015442964157250 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[13]] * (0.00199458794148013 - (0.00015595640046297 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[14]] * (0.00705979153201755 - (0.00015730069418051 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[15]] * (0.00429488975412722 - (0.00015743697943505 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[16]] * (0.00497724878704936 - (0.00016014760011861 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[17]] * (0.00506059305562353 - (0.00016194824072466 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[18]] * (0.00483432223285621 - (0.00016329050124225 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[19]] * (0.00495100420886005 - (0.00016297509798749 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[20]] * (0.00489319520555115 - (0.00016472839684661 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.00489177657970308 - (0.00016791875866630 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00487900894707044 - (0.00016755993898534 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.00486234009335561 - (0.00016968157345446 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00485737490288736 - (0.00017180713324431 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.00484106070563455 - (0.00017251073661092 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00483219429408410 - (0.00017321683790891 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.00482013597437550 - (0.00017392186866488 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.00480949628051497 - (0.00017569098775602 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.00479992055604049 - (0.00017746046369449 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[30]] * (0.00478750757986987 - (0.00017745630047554 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.00477828651185740 - (0.00017958043287604 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[32]] * (0.00476906544384494 - (0.00018170456527653 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.00475700712413634 - (0.00018099144598088 * applyconvL)));
				// end Cider (Focusrite) MCI
				break;
			case 1:
				// begin Rock (SSL) conv
				inputSampleL += (bussColors4->bL[bussColors4->c[1]] * (0.67887916185274055 + (0.00068787552301086 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[2]] * (0.25671050678827934 + (0.00017691749454490 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[3]] * (0.15135839896615280 + (0.00007481480365043 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[4]] * (0.11813512969090802 + (0.00005191138121359 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[5]] * (0.08329104347166429 + (0.00001871054659794 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[6]] * (0.07663817456103936 + (0.00002751359071705 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[7]] * (0.05477586152148759 + (0.00000744843212679 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[8]] * (0.05547314737187786 + (0.00001025289931145 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[9]] * (0.03822948356540711 - (0.00000249791561457 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[10]] * (0.04199383340841713 - (0.00000067328840674 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[11]] * (0.02695796542339694 - (0.00000796704606548 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[12]] * (0.03228715059431878 - (0.00000579711816722 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[13]] * (0.01846929689819187 - (0.00000984017804950 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[14]] * (0.02528050435045951 - (0.00000701189792484 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[15]] * (0.01207844846859765 - (0.00001522630289356 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[16]] * (0.01894464378378515 - (0.00001205456372080 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[17]] * (0.00667804407593324 - (0.00001343604283817 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[18]] * (0.01408418045473130 - (0.00001246443581504 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[19]] * (0.00228696509481569 - (0.00001506764046927 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[20]] * (0.01006277891348454 - (0.00000970723079112 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.00132368373546377 + (0.00001188847238761 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00676615715578373 - (0.00001209129844861 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.00426288438418556 + (0.00001286836943559 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00408897698639688 - (0.00001102542567911 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.00662040619382751 + (0.00001206328529063 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00196101294183599 - (0.00000950703614981 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.00845620581010342 + (0.00001279970295678 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.00032595215043616 - (0.00000920518241371 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.00982957737435458 + (0.00001177745362317 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[30]] * (0.00086920573760513 + (0.00000913758382404 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.01079020871452061 + (0.00000900750153697 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[32]] * (0.00167613606334460 + (0.00000732769151038 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.01138050011044332 + (0.00000946908207442 * applyconvL)));
				// end Rock (SSL) conv
				break;
			case 2:
				// begin Lush (Neve) conv
				inputSampleL += (bussColors4->bL[bussColors4->c[1]] * (0.20641602693167951 - (0.00078952185394898 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[2]] * (0.07601816702459827 + (0.00022786334179951 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[3]] * (0.03929765560019285 - (0.00054517993246352 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[4]] * (0.00298333157711103 - (0.00033083756545638 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[5]] * (0.00724006282304610 + (0.00045483683460812 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[6]] * (0.03073108963506036 - (0.00038190060537423 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[7]] * (0.02332434692533051 + (0.00040347288688932 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[8]] * (0.03792606869061214 - (0.00039673687335892 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[9]] * (0.02437059376675688 + (0.00037221210539535 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[10]] * (0.03416764311979521 - (0.00040314850796953 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[11]] * (0.01761669868102127 + (0.00035989484330131 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[12]] * (0.02538237753523052 - (0.00040149119125394 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[13]] * (0.00770737340728377 + (0.00035462118723555 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[14]] * (0.01580706228482803 - (0.00037563141307594 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[15]] * (0.00055119240005586 - (0.00035409299268971 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[16]] * (0.00818552143438768 - (0.00036507661042180 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[17]] * (0.00661842703548304 - (0.00034550528559056 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[18]] * (0.00362447476272098 - (0.00035553012761240 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[19]] * (0.00957098027225745 - (0.00034091691045338 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[20]] * (0.00193621774016660 - (0.00034554529131668 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[21]] * (0.01005433027357935 - (0.00033878223153845 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[22]] * (0.00221712428802004 - (0.00033481410137711 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[23]] * (0.00911255639207995 - (0.00033263425232666 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[24]] * (0.00339667169034909 - (0.00032634428038430 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[25]] * (0.00774096948249924 - (0.00032599868802996 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[26]] * (0.00463907626773794 - (0.00032131993173361 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[27]] * (0.00658222997260378 - (0.00032014977430211 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[28]] * (0.00550347079924993 - (0.00031557153256653 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[29]] * (0.00588754981375325 - (0.00032041307242303 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[30]] * (0.00590293898419892 - (0.00030457857428714 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[31]] * (0.00558952010441800 - (0.00030448053548086 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[32]] * (0.00598183557634295 - (0.00030715064323181 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[33]] * (0.00555223929714115 - (0.00030319367948553 * applyconvL)));
				// end Lush (Neve) conv
				break;
			case 3:
				// begin Elation (LA2A) vibe
				inputSampleL -= (bussColors4->bL[bussColors4->c[1]] * (0.25867935358656502 - (0.00045755657070112 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[2]] * (0.11509367290253694 - (0.00017494270657228 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[3]] * (0.06709853575891785 - (0.00058913102597723 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[4]] * (0.01871006356851681 - (0.00003387358004645 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[5]] * (0.00794797957360465 - (0.00044224784691203 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[6]] * (0.01956921817394220 - (0.00006718936750076 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[7]] * (0.01682120257195205 + (0.00032857446292230 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[8]] * (0.03401069039824205 - (0.00013634182872897 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[9]] * (0.02369950268232634 + (0.00023112685751657 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[10]] * (0.03477071178117132 - (0.00018029792231600 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[11]] * (0.02024369717958201 + (0.00017337813374202 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[12]] * (0.02819087729102172 - (0.00021438538665420 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[13]] * (0.01147946743141303 + (0.00014424066034649 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[14]] * (0.01894777011468867 - (0.00021549146262408 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[15]] * (0.00301370330346873 + (0.00013527460148394 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[16]] * (0.01067147835815486 - (0.00020960689910868 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[17]] * (0.00402715397506384 - (0.00014421582712470 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[18]] * (0.00502221703392005 - (0.00019805767015024 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[19]] * (0.00808788533308497 - (0.00016095444141931 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[20]] * (0.00232696588842683 - (0.00018384470981829 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.00943950821324531 - (0.00017098987347593 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00193709517200834 - (0.00018151995939591 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.00899713952612659 - (0.00017385835059948 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00280584331659089 - (0.00017742164162470 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.00780381001954970 - (0.00018002500755708 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00400370310490333 - (0.00017471691087957 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.00661527728186928 - (0.00018137323370347 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.00496545526864518 - (0.00017681872601767 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.00580728820997532 - (0.00018186220389790 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[30]] * (0.00549309984725666 - (0.00017722985399075 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.00542194777529239 - (0.00018486900185338 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[32]] * (0.00565992080998939 - (0.00018005824393118 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.00532121562846656 - (0.00018643189636216 * applyconvL)));
				// end Elation (LA2A)
				break;
			case 4:
				// begin Precious (Precision 8) Holo
				inputSampleL += (bussColors4->bL[bussColors4->c[1]] * (0.59188440274551890 - (0.00008361469668405 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[2]] * (0.24439750948076133 + (0.00002651678396848 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[3]] * (0.14109876103205621 - (0.00000840487181372 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[4]] * (0.10053507128157971 + (0.00001768100964598 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[5]] * (0.05859287880626238 - (0.00000361398065989 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[6]] * (0.04337406889823660 + (0.00000735941182117 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[7]] * (0.01589900680531097 + (0.00000207347387987 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[8]] * (0.01087234854973281 + (0.00000732123412029 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[9]] * (0.00845782429679176 - (0.00000133058605071 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[10]] * (0.00662278586618295 - (0.00000424594730611 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[11]] * (0.02000592193760155 + (0.00000632896879068 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[12]] * (0.01321157777167565 - (0.00001421171592570 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[13]] * (0.02249955362988238 + (0.00000163937127317 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[14]] * (0.01196492077581504 - (0.00000535385220676 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[15]] * (0.01905917427000097 + (0.00000121672882030 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[16]] * (0.00761909482108073 - (0.00000326242895115 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[17]] * (0.01362744780256239 + (0.00000359274216003 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[18]] * (0.00200183122683721 - (0.00000089207452791 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[19]] * (0.00833042637239315 + (0.00000946767677294 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[20]] * (0.00258481175207224 - (0.00000087429351464 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.00459744479712244 - (0.00000049519758701 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00534277030993820 + (0.00000397547847155 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.00272332919605675 + (0.00000040077229097 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00637243782359372 - (0.00000139419072176 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.00233001590327504 + (0.00000420129915747 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00623296727793041 + (0.00000019010664856 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.00276177096376805 + (0.00000580301901385 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.00559184754866264 + (0.00000080597287792 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.00343180144395919 - (0.00000243701142085 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[30]] * (0.00493325428861701 + (0.00000300985740900 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.00396140827680823 - (0.00000051459681789 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[32]] * (0.00448497879902493 + (0.00000744412841743 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.00425146888772076 - (0.00000082346016542 * applyconvL)));
				// end Precious (Precision 8) Holo
				break;
			case 5:
				// begin Punch (API) conv
				inputSampleL += (bussColors4->bL[bussColors4->c[1]] * (0.09299870608542582 - (0.00009582362368873 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[2]] * (0.11947847710741009 - (0.00004500891602770 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[3]] * (0.09071606264761795 + (0.00005639498984741 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[4]] * (0.08561982770836980 - (0.00004964855606916 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[5]] * (0.06440549220820363 + (0.00002428052139507 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[6]] * (0.05987991812840746 + (0.00000101867082290 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[7]] * (0.03980233135839382 + (0.00003312430049041 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[8]] * (0.03648402630896925 - (0.00002116186381142 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[9]] * (0.01826860869525248 + (0.00003115110025396 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[10]] * (0.01723968622495364 - (0.00002450634121718 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[11]] * (0.00187588812316724 + (0.00002838206198968 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[12]] * (0.00381796423957237 - (0.00003155815499462 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[13]] * (0.00852092214496733 - (0.00001702651162392 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[14]] * (0.00315560292270588 + (0.00002547861676047 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[15]] * (0.01258630914496868 - (0.00004555319243213 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[16]] * (0.00536435648963575 + (0.00001812393657101 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[17]] * (0.01272975658159178 - (0.00004103775306121 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[18]] * (0.00403818975172755 + (0.00003764615492871 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[19]] * (0.01042617366897483 - (0.00003605210426041 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[20]] * (0.00126599583390057 + (0.00004305458668852 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.00747876207688339 - (0.00003731207018977 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00149873689175324 - (0.00005086601800791 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.00503221309488033 - (0.00003636086782783 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00342998224655821 - (0.00004103091180506 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.00355585977903117 - (0.00003698982145400 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00437201792934817 - (0.00002720235666939 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.00299217874451556 - (0.00004446954727956 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.00457924652487249 - (0.00003859065778860 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.00298182934892027 - (0.00002064710931733 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[30]] * (0.00438838441540584 - (0.00005223008424866 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.00323984218794705 - (0.00003397987535887 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[32]] * (0.00407693981307314 - (0.00003935772436894 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.00350435348467321 - (0.00005525463935338 * applyconvL)));
				// end Punch (API) conv
				break;
			case 6:
				// begin Calibre (?) steel
				inputSampleL -= (bussColors4->bL[bussColors4->c[1]] * (0.23505923670562212 - (0.00028312859289245 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[2]] * (0.08188436704577637 - (0.00008817721351341 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[3]] * (0.05075798481700617 - (0.00018817166632483 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[4]] * (0.00455811821873093 + (0.00001922902995296 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[5]] * (0.00027610521433660 - (0.00013252525469291 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[6]] * (0.03529246280346626 - (0.00002772989223299 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[7]] * (0.01784111585586136 + (0.00010230276997291 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[8]] * (0.04394950700298298 - (0.00005910607126944 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[9]] * (0.01990770780547606 + (0.00007640328340556 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[10]] * (0.04073629569741782 - (0.00007712327117090 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[11]] * (0.01349648572795252 + (0.00005959130575917 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[12]] * (0.03191590248003717 - (0.00008418000575151 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[13]] * (0.00348795527924766 + (0.00005489156318238 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[14]] * (0.02198496281481767 - (0.00008471601187581 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[15]] * (0.00504771152505089 - (0.00005525060587917 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[16]] * (0.01391075698598491 - (0.00007929630732607 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[17]] * (0.01142762504081717 - (0.00005967036737742 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[18]] * (0.00893541815021255 - (0.00007535697758141 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[19]] * (0.01459704973464936 - (0.00005969199602841 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[20]] * (0.00694755135226282 - (0.00006930127097865 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.01516695630808575 - (0.00006365800069826 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00705917318113651 - (0.00006497209096539 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.01420501209177591 - (0.00006555654576113 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00815905656808701 - (0.00006105622534761 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.01274326525552961 - (0.00006542652857017 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00937146927845488 - (0.00006051267868722 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.01146573981165209 - (0.00006381511607749 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.01021294359409007 - (0.00005930397856398 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.01065217095323532 - (0.00006371505438319 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[30]] * (0.01058751196699751 - (0.00006042857480233 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.01026557827762401 - (0.00006007776163871 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[32]] * (0.01060929183604604 - (0.00006114703012726 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.01014533525058528 - (0.00005963567932887 * applyconvL)));
				// end Calibre (?)
				break;
			case 7:
			case 8:
				// begin Tube (Manley) conv
				inputSampleL -= (bussColors4->bL[bussColors4->c[1]] * (0.20641602693167951 - (0.00078952185394898 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[2]] * (0.07601816702459827 + (0.00022786334179951 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[3]] * (0.03929765560019285 - (0.00054517993246352 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[4]] * (0.00298333157711103 - (0.00033083756545638 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[5]] * (0.00724006282304610 + (0.00045483683460812 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[6]] * (0.03073108963506036 - (0.00038190060537423 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[7]] * (0.02332434692533051 + (0.00040347288688932 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[8]] * (0.03792606869061214 - (0.00039673687335892 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[9]] * (0.02437059376675688 + (0.00037221210539535 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[10]] * (0.03416764311979521 - (0.00040314850796953 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[11]] * (0.01761669868102127 + (0.00035989484330131 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[12]] * (0.02538237753523052 - (0.00040149119125394 * applyconvL)));
				inputSampleL += (bussColors4->bL[bussColors4->c[13]] * (0.00770737340728377 + (0.00035462118723555 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[14]] * (0.01580706228482803 - (0.00037563141307594 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[15]] * (0.00055119240005586 - (0.00035409299268971 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[16]] * (0.00818552143438768 - (0.00036507661042180 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[17]] * (0.00661842703548304 - (0.00034550528559056 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[18]] * (0.00362447476272098 - (0.00035553012761240 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[19]] * (0.00957098027225745 - (0.00034091691045338 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[20]] * (0.00193621774016660 - (0.00034554529131668 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[21]] * (0.01005433027357935 - (0.00033878223153845 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[22]] * (0.00221712428802004 - (0.00033481410137711 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[23]] * (0.00911255639207995 - (0.00033263425232666 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[24]] * (0.00339667169034909 - (0.00032634428038430 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[25]] * (0.00774096948249924 - (0.00032599868802996 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[26]] * (0.00463907626773794 - (0.00032131993173361 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[27]] * (0.00658222997260378 - (0.00032014977430211 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[28]] * (0.00550347079924993 - (0.00031557153256653 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[29]] * (0.00588754981375325 - (0.00032041307242303 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[30]] * (0.00590293898419892 - (0.00030457857428714 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[31]] * (0.00558952010441800 - (0.00030448053548086 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[32]] * (0.00598183557634295 - (0.00030715064323181 * applyconvL)));
				inputSampleL -= (bussColors4->bL[bussColors4->c[33]] * (0.00555223929714115 - (0.00030319367948553 * applyconvL)));
				// end Tube (Manley) conv
				break;
		}

		switch (console) {
			case 0:
				// begin Cider (Focusrite) MCI
				inputSampleR += (bussColors4->bR[bussColors4->c[1]] * (0.61283288942201319 + (0.00024011410669522 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[2]] * (0.24036380659761222 - (0.00020789518206241 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[3]] * (0.09104669761717916 + (0.00012829642741548 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[4]] * (0.02378290768554025 - (0.00017673646470440 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[5]] * (0.02832818490275965 - (0.00013536187747384 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[6]] * (0.03268797679215937 + (0.00015035126653359 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[7]] * (0.04024464202655586 - (0.00015034923056735 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[8]] * (0.01864890074318696 + (0.00014513281680642 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[9]] * (0.01632731954100322 - (0.00015509089075614 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[10]] * (0.00318907090555589 - (0.00014784812076550 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[11]] * (0.00208573465221869 - (0.00015350520779465 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[12]] * (0.00907033901519614 - (0.00015442964157250 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[13]] * (0.00199458794148013 - (0.00015595640046297 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[14]] * (0.00705979153201755 - (0.00015730069418051 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[15]] * (0.00429488975412722 - (0.00015743697943505 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[16]] * (0.00497724878704936 - (0.00016014760011861 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[17]] * (0.00506059305562353 - (0.00016194824072466 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[18]] * (0.00483432223285621 - (0.00016329050124225 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[19]] * (0.00495100420886005 - (0.00016297509798749 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[20]] * (0.00489319520555115 - (0.00016472839684661 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.00489177657970308 - (0.00016791875866630 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00487900894707044 - (0.00016755993898534 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.00486234009335561 - (0.00016968157345446 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00485737490288736 - (0.00017180713324431 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.00484106070563455 - (0.00017251073661092 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00483219429408410 - (0.00017321683790891 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.00482013597437550 - (0.00017392186866488 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.00480949628051497 - (0.00017569098775602 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.00479992055604049 - (0.00017746046369449 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[30]] * (0.00478750757986987 - (0.00017745630047554 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.00477828651185740 - (0.00017958043287604 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[32]] * (0.00476906544384494 - (0.00018170456527653 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.00475700712413634 - (0.00018099144598088 * applyconvR)));
				// end Cider (Focusrite) MCI
				break;
			case 1:
				// begin Rock (SSL) conv
				inputSampleR += (bussColors4->bR[bussColors4->c[1]] * (0.67887916185274055 + (0.00068787552301086 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[2]] * (0.25671050678827934 + (0.00017691749454490 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[3]] * (0.15135839896615280 + (0.00007481480365043 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[4]] * (0.11813512969090802 + (0.00005191138121359 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[5]] * (0.08329104347166429 + (0.00001871054659794 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[6]] * (0.07663817456103936 + (0.00002751359071705 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[7]] * (0.05477586152148759 + (0.00000744843212679 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[8]] * (0.05547314737187786 + (0.00001025289931145 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[9]] * (0.03822948356540711 - (0.00000249791561457 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[10]] * (0.04199383340841713 - (0.00000067328840674 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[11]] * (0.02695796542339694 - (0.00000796704606548 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[12]] * (0.03228715059431878 - (0.00000579711816722 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[13]] * (0.01846929689819187 - (0.00000984017804950 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[14]] * (0.02528050435045951 - (0.00000701189792484 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[15]] * (0.01207844846859765 - (0.00001522630289356 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[16]] * (0.01894464378378515 - (0.00001205456372080 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[17]] * (0.00667804407593324 - (0.00001343604283817 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[18]] * (0.01408418045473130 - (0.00001246443581504 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[19]] * (0.00228696509481569 - (0.00001506764046927 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[20]] * (0.01006277891348454 - (0.00000970723079112 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.00132368373546377 + (0.00001188847238761 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00676615715578373 - (0.00001209129844861 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.00426288438418556 + (0.00001286836943559 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00408897698639688 - (0.00001102542567911 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.00662040619382751 + (0.00001206328529063 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00196101294183599 - (0.00000950703614981 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.00845620581010342 + (0.00001279970295678 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.00032595215043616 - (0.00000920518241371 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.00982957737435458 + (0.00001177745362317 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[30]] * (0.00086920573760513 + (0.00000913758382404 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.01079020871452061 + (0.00000900750153697 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[32]] * (0.00167613606334460 + (0.00000732769151038 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.01138050011044332 + (0.00000946908207442 * applyconvR)));
				// end Rock (SSL) conv
				break;
			case 2:
				// begin Lush (Neve) conv
				inputSampleR += (bussColors4->bR[bussColors4->c[1]] * (0.20641602693167951 - (0.00078952185394898 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[2]] * (0.07601816702459827 + (0.00022786334179951 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[3]] * (0.03929765560019285 - (0.00054517993246352 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[4]] * (0.00298333157711103 - (0.00033083756545638 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[5]] * (0.00724006282304610 + (0.00045483683460812 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[6]] * (0.03073108963506036 - (0.00038190060537423 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[7]] * (0.02332434692533051 + (0.00040347288688932 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[8]] * (0.03792606869061214 - (0.00039673687335892 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[9]] * (0.02437059376675688 + (0.00037221210539535 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[10]] * (0.03416764311979521 - (0.00040314850796953 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[11]] * (0.01761669868102127 + (0.00035989484330131 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[12]] * (0.02538237753523052 - (0.00040149119125394 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[13]] * (0.00770737340728377 + (0.00035462118723555 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[14]] * (0.01580706228482803 - (0.00037563141307594 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[15]] * (0.00055119240005586 - (0.00035409299268971 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[16]] * (0.00818552143438768 - (0.00036507661042180 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[17]] * (0.00661842703548304 - (0.00034550528559056 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[18]] * (0.00362447476272098 - (0.00035553012761240 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[19]] * (0.00957098027225745 - (0.00034091691045338 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[20]] * (0.00193621774016660 - (0.00034554529131668 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[21]] * (0.01005433027357935 - (0.00033878223153845 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[22]] * (0.00221712428802004 - (0.00033481410137711 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[23]] * (0.00911255639207995 - (0.00033263425232666 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[24]] * (0.00339667169034909 - (0.00032634428038430 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[25]] * (0.00774096948249924 - (0.00032599868802996 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[26]] * (0.00463907626773794 - (0.00032131993173361 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[27]] * (0.00658222997260378 - (0.00032014977430211 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[28]] * (0.00550347079924993 - (0.00031557153256653 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[29]] * (0.00588754981375325 - (0.00032041307242303 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[30]] * (0.00590293898419892 - (0.00030457857428714 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[31]] * (0.00558952010441800 - (0.00030448053548086 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[32]] * (0.00598183557634295 - (0.00030715064323181 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[33]] * (0.00555223929714115 - (0.00030319367948553 * applyconvR)));
				// end Lush (Neve) conv
				break;
			case 3:
				// begin Elation (LA2A) vibe
				inputSampleR -= (bussColors4->bR[bussColors4->c[1]] * (0.25867935358656502 - (0.00045755657070112 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[2]] * (0.11509367290253694 - (0.00017494270657228 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[3]] * (0.06709853575891785 - (0.00058913102597723 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[4]] * (0.01871006356851681 - (0.00003387358004645 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[5]] * (0.00794797957360465 - (0.00044224784691203 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[6]] * (0.01956921817394220 - (0.00006718936750076 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[7]] * (0.01682120257195205 + (0.00032857446292230 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[8]] * (0.03401069039824205 - (0.00013634182872897 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[9]] * (0.02369950268232634 + (0.00023112685751657 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[10]] * (0.03477071178117132 - (0.00018029792231600 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[11]] * (0.02024369717958201 + (0.00017337813374202 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[12]] * (0.02819087729102172 - (0.00021438538665420 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[13]] * (0.01147946743141303 + (0.00014424066034649 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[14]] * (0.01894777011468867 - (0.00021549146262408 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[15]] * (0.00301370330346873 + (0.00013527460148394 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[16]] * (0.01067147835815486 - (0.00020960689910868 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[17]] * (0.00402715397506384 - (0.00014421582712470 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[18]] * (0.00502221703392005 - (0.00019805767015024 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[19]] * (0.00808788533308497 - (0.00016095444141931 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[20]] * (0.00232696588842683 - (0.00018384470981829 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.00943950821324531 - (0.00017098987347593 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00193709517200834 - (0.00018151995939591 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.00899713952612659 - (0.00017385835059948 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00280584331659089 - (0.00017742164162470 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.00780381001954970 - (0.00018002500755708 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00400370310490333 - (0.00017471691087957 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.00661527728186928 - (0.00018137323370347 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.00496545526864518 - (0.00017681872601767 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.00580728820997532 - (0.00018186220389790 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[30]] * (0.00549309984725666 - (0.00017722985399075 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.00542194777529239 - (0.00018486900185338 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[32]] * (0.00565992080998939 - (0.00018005824393118 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.00532121562846656 - (0.00018643189636216 * applyconvR)));
				// end Elation (LA2A)
				break;
			case 4:
				// begin Precious (Precision 8) Holo
				inputSampleR += (bussColors4->bR[bussColors4->c[1]] * (0.59188440274551890 - (0.00008361469668405 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[2]] * (0.24439750948076133 + (0.00002651678396848 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[3]] * (0.14109876103205621 - (0.00000840487181372 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[4]] * (0.10053507128157971 + (0.00001768100964598 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[5]] * (0.05859287880626238 - (0.00000361398065989 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[6]] * (0.04337406889823660 + (0.00000735941182117 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[7]] * (0.01589900680531097 + (0.00000207347387987 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[8]] * (0.01087234854973281 + (0.00000732123412029 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[9]] * (0.00845782429679176 - (0.00000133058605071 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[10]] * (0.00662278586618295 - (0.00000424594730611 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[11]] * (0.02000592193760155 + (0.00000632896879068 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[12]] * (0.01321157777167565 - (0.00001421171592570 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[13]] * (0.02249955362988238 + (0.00000163937127317 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[14]] * (0.01196492077581504 - (0.00000535385220676 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[15]] * (0.01905917427000097 + (0.00000121672882030 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[16]] * (0.00761909482108073 - (0.00000326242895115 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[17]] * (0.01362744780256239 + (0.00000359274216003 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[18]] * (0.00200183122683721 - (0.00000089207452791 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[19]] * (0.00833042637239315 + (0.00000946767677294 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[20]] * (0.00258481175207224 - (0.00000087429351464 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.00459744479712244 - (0.00000049519758701 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00534277030993820 + (0.00000397547847155 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.00272332919605675 + (0.00000040077229097 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00637243782359372 - (0.00000139419072176 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.00233001590327504 + (0.00000420129915747 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00623296727793041 + (0.00000019010664856 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.00276177096376805 + (0.00000580301901385 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.00559184754866264 + (0.00000080597287792 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.00343180144395919 - (0.00000243701142085 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[30]] * (0.00493325428861701 + (0.00000300985740900 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.00396140827680823 - (0.00000051459681789 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[32]] * (0.00448497879902493 + (0.00000744412841743 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.00425146888772076 - (0.00000082346016542 * applyconvR)));
				// end Precious (Precision 8) Holo
				break;
			case 5:
				// begin Punch (API) conv
				inputSampleR += (bussColors4->bR[bussColors4->c[1]] * (0.09299870608542582 - (0.00009582362368873 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[2]] * (0.11947847710741009 - (0.00004500891602770 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[3]] * (0.09071606264761795 + (0.00005639498984741 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[4]] * (0.08561982770836980 - (0.00004964855606916 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[5]] * (0.06440549220820363 + (0.00002428052139507 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[6]] * (0.05987991812840746 + (0.00000101867082290 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[7]] * (0.03980233135839382 + (0.00003312430049041 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[8]] * (0.03648402630896925 - (0.00002116186381142 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[9]] * (0.01826860869525248 + (0.00003115110025396 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[10]] * (0.01723968622495364 - (0.00002450634121718 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[11]] * (0.00187588812316724 + (0.00002838206198968 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[12]] * (0.00381796423957237 - (0.00003155815499462 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[13]] * (0.00852092214496733 - (0.00001702651162392 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[14]] * (0.00315560292270588 + (0.00002547861676047 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[15]] * (0.01258630914496868 - (0.00004555319243213 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[16]] * (0.00536435648963575 + (0.00001812393657101 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[17]] * (0.01272975658159178 - (0.00004103775306121 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[18]] * (0.00403818975172755 + (0.00003764615492871 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[19]] * (0.01042617366897483 - (0.00003605210426041 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[20]] * (0.00126599583390057 + (0.00004305458668852 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.00747876207688339 - (0.00003731207018977 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00149873689175324 - (0.00005086601800791 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.00503221309488033 - (0.00003636086782783 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00342998224655821 - (0.00004103091180506 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.00355585977903117 - (0.00003698982145400 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00437201792934817 - (0.00002720235666939 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.00299217874451556 - (0.00004446954727956 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.00457924652487249 - (0.00003859065778860 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.00298182934892027 - (0.00002064710931733 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[30]] * (0.00438838441540584 - (0.00005223008424866 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.00323984218794705 - (0.00003397987535887 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[32]] * (0.00407693981307314 - (0.00003935772436894 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.00350435348467321 - (0.00005525463935338 * applyconvR)));
				// end Punch (API) conv
				break;
			case 6:
				// begin Calibre (?) steel
				inputSampleR -= (bussColors4->bR[bussColors4->c[1]] * (0.23505923670562212 - (0.00028312859289245 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[2]] * (0.08188436704577637 - (0.00008817721351341 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[3]] * (0.05075798481700617 - (0.00018817166632483 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[4]] * (0.00455811821873093 + (0.00001922902995296 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[5]] * (0.00027610521433660 - (0.00013252525469291 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[6]] * (0.03529246280346626 - (0.00002772989223299 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[7]] * (0.01784111585586136 + (0.00010230276997291 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[8]] * (0.04394950700298298 - (0.00005910607126944 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[9]] * (0.01990770780547606 + (0.00007640328340556 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[10]] * (0.04073629569741782 - (0.00007712327117090 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[11]] * (0.01349648572795252 + (0.00005959130575917 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[12]] * (0.03191590248003717 - (0.00008418000575151 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[13]] * (0.00348795527924766 + (0.00005489156318238 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[14]] * (0.02198496281481767 - (0.00008471601187581 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[15]] * (0.00504771152505089 - (0.00005525060587917 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[16]] * (0.01391075698598491 - (0.00007929630732607 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[17]] * (0.01142762504081717 - (0.00005967036737742 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[18]] * (0.00893541815021255 - (0.00007535697758141 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[19]] * (0.01459704973464936 - (0.00005969199602841 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[20]] * (0.00694755135226282 - (0.00006930127097865 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.01516695630808575 - (0.00006365800069826 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00705917318113651 - (0.00006497209096539 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.01420501209177591 - (0.00006555654576113 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00815905656808701 - (0.00006105622534761 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.01274326525552961 - (0.00006542652857017 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00937146927845488 - (0.00006051267868722 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.01146573981165209 - (0.00006381511607749 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.01021294359409007 - (0.00005930397856398 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.01065217095323532 - (0.00006371505438319 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[30]] * (0.01058751196699751 - (0.00006042857480233 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.01026557827762401 - (0.00006007776163871 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[32]] * (0.01060929183604604 - (0.00006114703012726 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.01014533525058528 - (0.00005963567932887 * applyconvR)));
				// end Calibre (?)
				break;
			case 7:
			case 8:
				// begin Tube (Manley) conv
				inputSampleR -= (bussColors4->bR[bussColors4->c[1]] * (0.20641602693167951 - (0.00078952185394898 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[2]] * (0.07601816702459827 + (0.00022786334179951 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[3]] * (0.03929765560019285 - (0.00054517993246352 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[4]] * (0.00298333157711103 - (0.00033083756545638 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[5]] * (0.00724006282304610 + (0.00045483683460812 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[6]] * (0.03073108963506036 - (0.00038190060537423 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[7]] * (0.02332434692533051 + (0.00040347288688932 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[8]] * (0.03792606869061214 - (0.00039673687335892 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[9]] * (0.02437059376675688 + (0.00037221210539535 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[10]] * (0.03416764311979521 - (0.00040314850796953 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[11]] * (0.01761669868102127 + (0.00035989484330131 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[12]] * (0.02538237753523052 - (0.00040149119125394 * applyconvR)));
				inputSampleR += (bussColors4->bR[bussColors4->c[13]] * (0.00770737340728377 + (0.00035462118723555 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[14]] * (0.01580706228482803 - (0.00037563141307594 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[15]] * (0.00055119240005586 - (0.00035409299268971 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[16]] * (0.00818552143438768 - (0.00036507661042180 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[17]] * (0.00661842703548304 - (0.00034550528559056 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[18]] * (0.00362447476272098 - (0.00035553012761240 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[19]] * (0.00957098027225745 - (0.00034091691045338 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[20]] * (0.00193621774016660 - (0.00034554529131668 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[21]] * (0.01005433027357935 - (0.00033878223153845 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[22]] * (0.00221712428802004 - (0.00033481410137711 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[23]] * (0.00911255639207995 - (0.00033263425232666 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[24]] * (0.00339667169034909 - (0.00032634428038430 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[25]] * (0.00774096948249924 - (0.00032599868802996 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[26]] * (0.00463907626773794 - (0.00032131993173361 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[27]] * (0.00658222997260378 - (0.00032014977430211 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[28]] * (0.00550347079924993 - (0.00031557153256653 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[29]] * (0.00588754981375325 - (0.00032041307242303 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[30]] * (0.00590293898419892 - (0.00030457857428714 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[31]] * (0.00558952010441800 - (0.00030448053548086 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[32]] * (0.00598183557634295 - (0.00030715064323181 * applyconvR)));
				inputSampleR -= (bussColors4->bR[bussColors4->c[33]] * (0.00555223929714115 - (0.00030319367948553 * applyconvR)));
				// end Tube (Manley) conv
				break;
		}

		bridgerectifier = fabs(inputSampleL);
		bridgerectifier = 1.0 - cos(bridgerectifier);
		if (inputSampleL > 0) inputSampleL -= bridgerectifier;
		else inputSampleL += bridgerectifier;

		bridgerectifier = fabs(inputSampleR);
		bridgerectifier = 1.0 - cos(bridgerectifier);
		if (inputSampleR > 0) inputSampleR -= bridgerectifier;
		else inputSampleR += bridgerectifier;

		if (outgain != 1.0) {
			inputSampleL *= outgain;
			inputSampleR *= outgain;
		}

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		// begin 32 bit stereo floating point dither
		/*int expon;
		frexpf((float) inputSampleL, &expon);
		bussColors4->fpdL ^= bussColors4->fpdL << 13;
		bussColors4->fpdL ^= bussColors4->fpdL >> 17;
		bussColors4->fpdL ^= bussColors4->fpdL << 5;
		inputSampleL += (((double) bussColors4->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		bussColors4->fpdR ^= bussColors4->fpdR << 13;
		bussColors4->fpdR ^= bussColors4->fpdR >> 17;
		bussColors4->fpdR ^= bussColors4->fpdR << 5;
		inputSampleR += (((double) bussColors4->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));*/
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
	BUSSCOLORS4_URI,
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
