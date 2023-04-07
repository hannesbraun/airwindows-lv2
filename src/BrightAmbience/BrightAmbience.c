#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define BRIGHTAMBIENCE_URI "https://hannesbraun.net/ns/lv2/airwindows/brightambience"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	DRY_WET = 4,
	SUSTAIN = 5,
	DECAY = 6
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* dryWet;
	const float* sustain;
	const float* decay;

	int32_t pL[25361];
	int32_t pR[25361];
	int gcount;
	uint32_t fpdL;
	uint32_t fpdR;
} BrightAmbience;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	BrightAmbience* brightAmbience = (BrightAmbience*) calloc(1, sizeof(BrightAmbience));
	return (LV2_Handle) brightAmbience;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BrightAmbience* brightAmbience = (BrightAmbience*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			brightAmbience->input[0] = (const float*) data;
			break;
		case INPUT_R:
			brightAmbience->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			brightAmbience->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			brightAmbience->output[1] = (float*) data;
			break;
		case DRY_WET:
			brightAmbience->dryWet = (const float*) data;
			break;
		case SUSTAIN:
			brightAmbience->sustain = (const float*) data;
			break;
		case DECAY:
			brightAmbience->decay = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	BrightAmbience* brightAmbience = (BrightAmbience*) instance;
	for (int count = 0; count < 25360; count++) {
		brightAmbience->pL[count] = 0;
		brightAmbience->pR[count] = 0;
	}
	brightAmbience->gcount = 0;
	brightAmbience->fpdL = 1.0;
	while (brightAmbience->fpdL < 16386) brightAmbience->fpdL = rand() * UINT32_MAX;
	brightAmbience->fpdR = 1.0;
	while (brightAmbience->fpdR < 16386) brightAmbience->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	BrightAmbience* brightAmbience = (BrightAmbience*) instance;

	const float* in1 = brightAmbience->input[0];
	const float* in2 = brightAmbience->input[1];
	float* out1 = brightAmbience->output[0];
	float* out2 = brightAmbience->output[1];

	double wet = *brightAmbience->dryWet;
	int cpu = (int) (*brightAmbience->sustain * 32);
	double decay = 1.0 + (pow(*brightAmbience->decay, 2) / 2.0);
	double scale = 4;
	int count;
	int32_t temp;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = brightAmbience->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = brightAmbience->fpdR * 1.18e-17;

		if (brightAmbience->gcount < 0 || brightAmbience->gcount > 12680) {
			brightAmbience->gcount = 12680;
		}
		count = brightAmbience->gcount;

		brightAmbience->pL[count + 12680] = brightAmbience->pL[count] = (int32_t) (inputSampleL * 8388352.0);
		brightAmbience->pR[count + 12680] = brightAmbience->pR[count] = (int32_t) (inputSampleR * 8388352.0);
		// double buffer
		//-8388352 to 8388352 is equal to 24 bit linear space

		// begin L
		temp = 0;
		switch (cpu) {
			case 32:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 12679];
				temp += brightAmbience->pL[count + 12671];
				temp += brightAmbience->pL[count + 12661];
				temp += brightAmbience->pL[count + 12613];
				temp += brightAmbience->pL[count + 12587];
				temp += brightAmbience->pL[count + 12577];
				temp += brightAmbience->pL[count + 12563];
				temp += brightAmbience->pL[count + 12557];
				temp += brightAmbience->pL[count + 12521];
				temp += brightAmbience->pL[count + 12499];
				temp += brightAmbience->pL[count + 12493];
				temp += brightAmbience->pL[count + 12457];
				temp += brightAmbience->pL[count + 12427];
				temp += brightAmbience->pL[count + 12401];
				temp += brightAmbience->pL[count + 12373];
				temp += brightAmbience->pL[count + 12367];
				temp += brightAmbience->pL[count + 12349];
				temp += brightAmbience->pL[count + 12337];
				temp += brightAmbience->pL[count + 12323];
				temp += brightAmbience->pL[count + 12301];
			case 31:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 12281];
				temp += brightAmbience->pL[count + 12247];
				temp += brightAmbience->pL[count + 12239];
				temp += brightAmbience->pL[count + 12203];
				temp += brightAmbience->pL[count + 12197];
				temp += brightAmbience->pL[count + 12191];
				temp += brightAmbience->pL[count + 12163];
				temp += brightAmbience->pL[count + 12143];
				temp += brightAmbience->pL[count + 12127];
				temp += brightAmbience->pL[count + 12109];
				temp += brightAmbience->pL[count + 12083];
				temp += brightAmbience->pL[count + 12077];
				temp += brightAmbience->pL[count + 12059];
				temp += brightAmbience->pL[count + 12037];
				temp += brightAmbience->pL[count + 11989];
				temp += brightAmbience->pL[count + 11969];
				temp += brightAmbience->pL[count + 11951];
				temp += brightAmbience->pL[count + 11933];
				temp += brightAmbience->pL[count + 11927];
				temp += brightAmbience->pL[count + 11911];
			case 30:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 11897];
				temp += brightAmbience->pL[count + 11873];
				temp += brightAmbience->pL[count + 11843];
				temp += brightAmbience->pL[count + 11819];
				temp += brightAmbience->pL[count + 11791];
				temp += brightAmbience->pL[count + 11773];
				temp += brightAmbience->pL[count + 11731];
				temp += brightAmbience->pL[count + 11723];
				temp += brightAmbience->pL[count + 11701];
				temp += brightAmbience->pL[count + 11687];
				temp += brightAmbience->pL[count + 11659];
				temp += brightAmbience->pL[count + 11623];
				temp += brightAmbience->pL[count + 11611];
				temp += brightAmbience->pL[count + 11587];
				temp += brightAmbience->pL[count + 11573];
				temp += brightAmbience->pL[count + 11549];
				temp += brightAmbience->pL[count + 11537];
				temp += brightAmbience->pL[count + 11527];
				temp += brightAmbience->pL[count + 11513];
				temp += brightAmbience->pL[count + 11503];
			case 29:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 11467];
				temp += brightAmbience->pL[count + 11441];
				temp += brightAmbience->pL[count + 11413];
				temp += brightAmbience->pL[count + 11399];
				temp += brightAmbience->pL[count + 11363];
				temp += brightAmbience->pL[count + 11353];
				temp += brightAmbience->pL[count + 11329];
				temp += brightAmbience->pL[count + 11323];
				temp += brightAmbience->pL[count + 11303];
				temp += brightAmbience->pL[count + 11293];
				temp += brightAmbience->pL[count + 11281];
				temp += brightAmbience->pL[count + 11251];
				temp += brightAmbience->pL[count + 11231];
				temp += brightAmbience->pL[count + 11209];
				temp += brightAmbience->pL[count + 11201];
				temp += brightAmbience->pL[count + 11177];
				temp += brightAmbience->pL[count + 11171];
				temp += brightAmbience->pL[count + 11153];
				temp += brightAmbience->pL[count + 11141];
				temp += brightAmbience->pL[count + 11117];
			case 28:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 11099];
				temp += brightAmbience->pL[count + 11071];
				temp += brightAmbience->pL[count + 11047];
				temp += brightAmbience->pL[count + 11033];
				temp += brightAmbience->pL[count + 11021];
				temp += brightAmbience->pL[count + 10993];
				temp += brightAmbience->pL[count + 10973];
				temp += brightAmbience->pL[count + 10943];
				temp += brightAmbience->pL[count + 10933];
				temp += brightAmbience->pL[count + 10919];
				temp += brightAmbience->pL[count + 10897];
				temp += brightAmbience->pL[count + 10879];
				temp += brightAmbience->pL[count + 10867];
				temp += brightAmbience->pL[count + 10841];
				temp += brightAmbience->pL[count + 10783];
				temp += brightAmbience->pL[count + 10763];
				temp += brightAmbience->pL[count + 10751];
				temp += brightAmbience->pL[count + 10733];
				temp += brightAmbience->pL[count + 10721];
				temp += brightAmbience->pL[count + 10687];
			case 27:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 10669];
				temp += brightAmbience->pL[count + 10657];
				temp += brightAmbience->pL[count + 10649];
				temp += brightAmbience->pL[count + 10637];
				temp += brightAmbience->pL[count + 10621];
				temp += brightAmbience->pL[count + 10609];
				temp += brightAmbience->pL[count + 10603];
				temp += brightAmbience->pL[count + 10597];
				temp += brightAmbience->pL[count + 10571];
				temp += brightAmbience->pL[count + 10561];
				temp += brightAmbience->pL[count + 10553];
				temp += brightAmbience->pL[count + 10499];
				temp += brightAmbience->pL[count + 10481];
				temp += brightAmbience->pL[count + 10453];
				temp += brightAmbience->pL[count + 10439];
				temp += brightAmbience->pL[count + 10417];
				temp += brightAmbience->pL[count + 10399];
				temp += brightAmbience->pL[count + 10391];
				temp += brightAmbience->pL[count + 10363];
				temp += brightAmbience->pL[count + 10343];
			case 26:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 10321];
				temp += brightAmbience->pL[count + 10301];
				temp += brightAmbience->pL[count + 10261];
				temp += brightAmbience->pL[count + 10253];
				temp += brightAmbience->pL[count + 10231];
				temp += brightAmbience->pL[count + 10211];
				temp += brightAmbience->pL[count + 10189];
				temp += brightAmbience->pL[count + 10159];
				temp += brightAmbience->pL[count + 10139];
				temp += brightAmbience->pL[count + 10099];
				temp += brightAmbience->pL[count + 10093];
				temp += brightAmbience->pL[count + 10079];
				temp += brightAmbience->pL[count + 10039];
				temp += brightAmbience->pL[count + 9983];
				temp += brightAmbience->pL[count + 9973];
				temp += brightAmbience->pL[count + 9953];
				temp += brightAmbience->pL[count + 9929];
				temp += brightAmbience->pL[count + 9911];
				temp += brightAmbience->pL[count + 9893];
				temp += brightAmbience->pL[count + 9871];
			case 25:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 9851];
				temp += brightAmbience->pL[count + 9817];
				temp += brightAmbience->pL[count + 9773];
				temp += brightAmbience->pL[count + 9763];
				temp += brightAmbience->pL[count + 9721];
				temp += brightAmbience->pL[count + 9703];
				temp += brightAmbience->pL[count + 9697];
				temp += brightAmbience->pL[count + 9673];
				temp += brightAmbience->pL[count + 9641];
				temp += brightAmbience->pL[count + 9631];
				temp += brightAmbience->pL[count + 9619];
				temp += brightAmbience->pL[count + 9607];
				temp += brightAmbience->pL[count + 9581];
				temp += brightAmbience->pL[count + 9559];
				temp += brightAmbience->pL[count + 9547];
				temp += brightAmbience->pL[count + 9521];
				temp += brightAmbience->pL[count + 9493];
				temp += brightAmbience->pL[count + 9487];
				temp += brightAmbience->pL[count + 9469];
				temp += brightAmbience->pL[count + 9449];
			case 24:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 9433];
				temp += brightAmbience->pL[count + 9413];
				temp += brightAmbience->pL[count + 9403];
				temp += brightAmbience->pL[count + 9391];
				temp += brightAmbience->pL[count + 9371];
				temp += brightAmbience->pL[count + 9349];
				temp += brightAmbience->pL[count + 9323];
				temp += brightAmbience->pL[count + 9311];
				temp += brightAmbience->pL[count + 9301];
				temp += brightAmbience->pL[count + 9277];
				temp += brightAmbience->pL[count + 9257];
				temp += brightAmbience->pL[count + 9239];
				temp += brightAmbience->pL[count + 9227];
				temp += brightAmbience->pL[count + 9221];
				temp += brightAmbience->pL[count + 9169];
				temp += brightAmbience->pL[count + 9161];
				temp += brightAmbience->pL[count + 9151];
				temp += brightAmbience->pL[count + 9137];
				temp += brightAmbience->pL[count + 9109];
				temp += brightAmbience->pL[count + 9101];
			case 23:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 9089];
				temp += brightAmbience->pL[count + 9071];
				temp += brightAmbience->pL[count + 9043];
				temp += brightAmbience->pL[count + 9019];
				temp += brightAmbience->pL[count + 9007];
				temp += brightAmbience->pL[count + 8971];
				temp += brightAmbience->pL[count + 8963];
				temp += brightAmbience->pL[count + 8929];
				temp += brightAmbience->pL[count + 8917];
				temp += brightAmbience->pL[count + 8893];
				temp += brightAmbience->pL[count + 8863];
				temp += brightAmbience->pL[count + 8849];
				temp += brightAmbience->pL[count + 8837];
				temp += brightAmbience->pL[count + 8803];
				temp += brightAmbience->pL[count + 8779];
				temp += brightAmbience->pL[count + 8761];
				temp += brightAmbience->pL[count + 8753];
				temp += brightAmbience->pL[count + 8731];
				temp += brightAmbience->pL[count + 8699];
				temp += brightAmbience->pL[count + 8677];
			case 22:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 8669];
				temp += brightAmbience->pL[count + 8657];
				temp += brightAmbience->pL[count + 8647];
				temp += brightAmbience->pL[count + 8623];
				temp += brightAmbience->pL[count + 8599];
				temp += brightAmbience->pL[count + 8581];
				temp += brightAmbience->pL[count + 8569];
				temp += brightAmbience->pL[count + 8563];
				temp += brightAmbience->pL[count + 8539];
				temp += brightAmbience->pL[count + 8521];
				temp += brightAmbience->pL[count + 8507];
				temp += brightAmbience->pL[count + 8497];
				temp += brightAmbience->pL[count + 8473];
				temp += brightAmbience->pL[count + 8437];
				temp += brightAmbience->pL[count + 8411];
				temp += brightAmbience->pL[count + 8387];
				temp += brightAmbience->pL[count + 8371];
				temp += brightAmbience->pL[count + 8363];
				temp += brightAmbience->pL[count + 8339];
				temp += brightAmbience->pL[count + 8303];
			case 21:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 8287];
				temp += brightAmbience->pL[count + 8273];
				temp += brightAmbience->pL[count + 8233];
				temp += brightAmbience->pL[count + 8221];
				temp += brightAmbience->pL[count + 8209];
				temp += brightAmbience->pL[count + 8201];
				temp += brightAmbience->pL[count + 8173];
				temp += brightAmbience->pL[count + 8161];
				temp += brightAmbience->pL[count + 8143];
				temp += brightAmbience->pL[count + 8117];
				temp += brightAmbience->pL[count + 8107];
				temp += brightAmbience->pL[count + 8083];
				temp += brightAmbience->pL[count + 8069];
				temp += brightAmbience->pL[count + 8059];
				temp += brightAmbience->pL[count + 8023];
				temp += brightAmbience->pL[count + 8009];
				temp += brightAmbience->pL[count + 7991];
				temp += brightAmbience->pL[count + 7943];
				temp += brightAmbience->pL[count + 7927];
				temp += brightAmbience->pL[count + 7909];
			case 20:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 7879];
				temp += brightAmbience->pL[count + 7871];
				temp += brightAmbience->pL[count + 7841];
				temp += brightAmbience->pL[count + 7831];
				temp += brightAmbience->pL[count + 7813];
				temp += brightAmbience->pL[count + 7799];
				temp += brightAmbience->pL[count + 7783];
				temp += brightAmbience->pL[count + 7747];
				temp += brightAmbience->pL[count + 7733];
				temp += brightAmbience->pL[count + 7709];
				temp += brightAmbience->pL[count + 7673];
				temp += brightAmbience->pL[count + 7661];
				temp += brightAmbience->pL[count + 7633];
				temp += brightAmbience->pL[count + 7627];
				temp += brightAmbience->pL[count + 7607];
				temp += brightAmbience->pL[count + 7597];
				temp += brightAmbience->pL[count + 7561];
				temp += brightAmbience->pL[count + 7543];
				temp += brightAmbience->pL[count + 7529];
				temp += brightAmbience->pL[count + 7513];
			case 19:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 7481];
				temp += brightAmbience->pL[count + 7447];
				temp += brightAmbience->pL[count + 7429];
				temp += brightAmbience->pL[count + 7409];
				temp += brightAmbience->pL[count + 7397];
				temp += brightAmbience->pL[count + 7369];
				temp += brightAmbience->pL[count + 7363];
				temp += brightAmbience->pL[count + 7319];
				temp += brightAmbience->pL[count + 7291];
				temp += brightAmbience->pL[count + 7277];
				temp += brightAmbience->pL[count + 7267];
				temp += brightAmbience->pL[count + 7249];
				temp += brightAmbience->pL[count + 7243];
				temp += brightAmbience->pL[count + 7237];
				temp += brightAmbience->pL[count + 7187];
				temp += brightAmbience->pL[count + 7171];
				temp += brightAmbience->pL[count + 7151];
				temp += brightAmbience->pL[count + 7117];
				temp += brightAmbience->pL[count + 7099];
				temp += brightAmbience->pL[count + 7087];
			case 18:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 7069];
				temp += brightAmbience->pL[count + 7061];
				temp += brightAmbience->pL[count + 7039];
				temp += brightAmbience->pL[count + 6997];
				temp += brightAmbience->pL[count + 6983];
				temp += brightAmbience->pL[count + 6971];
				temp += brightAmbience->pL[count + 6953];
				temp += brightAmbience->pL[count + 6941];
				temp += brightAmbience->pL[count + 6919];
				temp += brightAmbience->pL[count + 6913];
				temp += brightAmbience->pL[count + 6907];
				temp += brightAmbience->pL[count + 6859];
				temp += brightAmbience->pL[count + 6847];
				temp += brightAmbience->pL[count + 6829];
				temp += brightAmbience->pL[count + 6823];
				temp += brightAmbience->pL[count + 6809];
				temp += brightAmbience->pL[count + 6787];
				temp += brightAmbience->pL[count + 6763];
				temp += brightAmbience->pL[count + 6749];
				temp += brightAmbience->pL[count + 6731];
			case 17:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 6707];
				temp += brightAmbience->pL[count + 6677];
				temp += brightAmbience->pL[count + 6653];
				temp += brightAmbience->pL[count + 6641];
				temp += brightAmbience->pL[count + 6623];
				temp += brightAmbience->pL[count + 6607];
				temp += brightAmbience->pL[count + 6581];
				temp += brightAmbience->pL[count + 6563];
				temp += brightAmbience->pL[count + 6553];
				temp += brightAmbience->pL[count + 6539];
				temp += brightAmbience->pL[count + 6511];
				temp += brightAmbience->pL[count + 6499];
				temp += brightAmbience->pL[count + 6487];
				temp += brightAmbience->pL[count + 6469];
				temp += brightAmbience->pL[count + 6457];
				temp += brightAmbience->pL[count + 6449];
				temp += brightAmbience->pL[count + 6439];
				temp += brightAmbience->pL[count + 6413];
				temp += brightAmbience->pL[count + 6397];
				temp += brightAmbience->pL[count + 6373];
			case 16:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 6353];
				temp += brightAmbience->pL[count + 6329];
				temp += brightAmbience->pL[count + 6317];
				temp += brightAmbience->pL[count + 6301];
				temp += brightAmbience->pL[count + 6281];
				temp += brightAmbience->pL[count + 6263];
				temp += brightAmbience->pL[count + 6253];
				temp += brightAmbience->pL[count + 6233];
				temp += brightAmbience->pL[count + 6197];
				temp += brightAmbience->pL[count + 6163];
				temp += brightAmbience->pL[count + 6137];
				temp += brightAmbience->pL[count + 6127];
				temp += brightAmbience->pL[count + 6109];
				temp += brightAmbience->pL[count + 6077];
				temp += brightAmbience->pL[count + 6067];
				temp += brightAmbience->pL[count + 6049];
				temp += brightAmbience->pL[count + 6031];
				temp += brightAmbience->pL[count + 6011];
				temp += brightAmbience->pL[count + 6001];
				temp += brightAmbience->pL[count + 5977];
			case 15:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 5959];
				temp += brightAmbience->pL[count + 5947];
				temp += brightAmbience->pL[count + 5933];
				temp += brightAmbience->pL[count + 5921];
				temp += brightAmbience->pL[count + 5911];
				temp += brightAmbience->pL[count + 5897];
				temp += brightAmbience->pL[count + 5879];
				temp += brightAmbience->pL[count + 5849];
				temp += brightAmbience->pL[count + 5833];
				temp += brightAmbience->pL[count + 5813];
				temp += brightAmbience->pL[count + 5807];
				temp += brightAmbience->pL[count + 5773];
				temp += brightAmbience->pL[count + 5743];
				temp += brightAmbience->pL[count + 5737];
				temp += brightAmbience->pL[count + 5729];
				temp += brightAmbience->pL[count + 5717];
				temp += brightAmbience->pL[count + 5699];
				temp += brightAmbience->pL[count + 5669];
				temp += brightAmbience->pL[count + 5641];
				temp += brightAmbience->pL[count + 5627];
			case 14:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 5611];
				temp += brightAmbience->pL[count + 5599];
				temp += brightAmbience->pL[count + 5587];
				temp += brightAmbience->pL[count + 5569];
				temp += brightAmbience->pL[count + 5549];
				temp += brightAmbience->pL[count + 5521];
				temp += brightAmbience->pL[count + 5503];
				temp += brightAmbience->pL[count + 5483];
				temp += brightAmbience->pL[count + 5461];
				temp += brightAmbience->pL[count + 5429];
				temp += brightAmbience->pL[count + 5401];
				temp += brightAmbience->pL[count + 5377];
				temp += brightAmbience->pL[count + 5353];
				temp += brightAmbience->pL[count + 5333];
				temp += brightAmbience->pL[count + 5323];
				temp += brightAmbience->pL[count + 5267];
				temp += brightAmbience->pL[count + 5261];
				temp += brightAmbience->pL[count + 5237];
				temp += brightAmbience->pL[count + 5213];
				temp += brightAmbience->pL[count + 5191];
			case 13:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 5179];
				temp += brightAmbience->pL[count + 5147];
				temp += brightAmbience->pL[count + 5113];
				temp += brightAmbience->pL[count + 5101];
				temp += brightAmbience->pL[count + 5087];
				temp += brightAmbience->pL[count + 5081];
				temp += brightAmbience->pL[count + 5059];
				temp += brightAmbience->pL[count + 5053];
				temp += brightAmbience->pL[count + 5021];
				temp += brightAmbience->pL[count + 5009];
				temp += brightAmbience->pL[count + 4979];
				temp += brightAmbience->pL[count + 4961];
				temp += brightAmbience->pL[count + 4939];
				temp += brightAmbience->pL[count + 4931];
				temp += brightAmbience->pL[count + 4903];
				temp += brightAmbience->pL[count + 4891];
				temp += brightAmbience->pL[count + 4877];
				temp += brightAmbience->pL[count + 4871];
				temp += brightAmbience->pL[count + 4861];
				temp += brightAmbience->pL[count + 4849];
			case 12:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 4813];
				temp += brightAmbience->pL[count + 4799];
				temp += brightAmbience->pL[count + 4777];
				temp += brightAmbience->pL[count + 4757];
				temp += brightAmbience->pL[count + 4733];
				temp += brightAmbience->pL[count + 4703];
				temp += brightAmbience->pL[count + 4679];
				temp += brightAmbience->pL[count + 4657];
				temp += brightAmbience->pL[count + 4643];
				temp += brightAmbience->pL[count + 4631];
				temp += brightAmbience->pL[count + 4601];
				temp += brightAmbience->pL[count + 4591];
				temp += brightAmbience->pL[count + 4577];
				temp += brightAmbience->pL[count + 4547];
				temp += brightAmbience->pL[count + 4517];
				temp += brightAmbience->pL[count + 4507];
				temp += brightAmbience->pL[count + 4489];
				temp += brightAmbience->pL[count + 4471];
				temp += brightAmbience->pL[count + 4439];
				temp += brightAmbience->pL[count + 4421];
			case 11:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 4399];
				temp += brightAmbience->pL[count + 4367];
				temp += brightAmbience->pL[count + 4357];
				temp += brightAmbience->pL[count + 4343];
				temp += brightAmbience->pL[count + 4331];
				temp += brightAmbience->pL[count + 4307];
				temp += brightAmbience->pL[count + 4289];
				temp += brightAmbience->pL[count + 4279];
				temp += brightAmbience->pL[count + 4243];
				temp += brightAmbience->pL[count + 4223];
				temp += brightAmbience->pL[count + 4213];
				temp += brightAmbience->pL[count + 4187];
				temp += brightAmbience->pL[count + 4181];
				temp += brightAmbience->pL[count + 4159];
				temp += brightAmbience->pL[count + 4147];
				temp += brightAmbience->pL[count + 4133];
				temp += brightAmbience->pL[count + 4103];
				temp += brightAmbience->pL[count + 4097];
				temp += brightAmbience->pL[count + 4087];
				temp += brightAmbience->pL[count + 4073];
			case 10:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 4033];
				temp += brightAmbience->pL[count + 4009];
				temp += brightAmbience->pL[count + 4001];
				temp += brightAmbience->pL[count + 3979];
				temp += brightAmbience->pL[count + 3961];
				temp += brightAmbience->pL[count + 3937];
				temp += brightAmbience->pL[count + 3919];
				temp += brightAmbience->pL[count + 3893];
				temp += brightAmbience->pL[count + 3877];
				temp += brightAmbience->pL[count + 3851];
				temp += brightAmbience->pL[count + 3833];
				temp += brightAmbience->pL[count + 3821];
				temp += brightAmbience->pL[count + 3799];
				temp += brightAmbience->pL[count + 3791];
				temp += brightAmbience->pL[count + 3769];
				temp += brightAmbience->pL[count + 3757];
				temp += brightAmbience->pL[count + 3733];
				temp += brightAmbience->pL[count + 3721];
				temp += brightAmbience->pL[count + 3707];
				temp += brightAmbience->pL[count + 3691];
			case 9:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 3677];
				temp += brightAmbience->pL[count + 3667];
				temp += brightAmbience->pL[count + 3653];
				temp += brightAmbience->pL[count + 3631];
				temp += brightAmbience->pL[count + 3613];
				temp += brightAmbience->pL[count + 3593];
				temp += brightAmbience->pL[count + 3581];
				temp += brightAmbience->pL[count + 3559];
				temp += brightAmbience->pL[count + 3553];
				temp += brightAmbience->pL[count + 3523];
				temp += brightAmbience->pL[count + 3503];
				temp += brightAmbience->pL[count + 3469];
				temp += brightAmbience->pL[count + 3457];
				temp += brightAmbience->pL[count + 3443];
				temp += brightAmbience->pL[count + 3431];
				temp += brightAmbience->pL[count + 3419];
				temp += brightAmbience->pL[count + 3391];
				temp += brightAmbience->pL[count + 3377];
				temp += brightAmbience->pL[count + 3341];
				temp += brightAmbience->pL[count + 3329];
			case 8:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 3299];
				temp += brightAmbience->pL[count + 3289];
				temp += brightAmbience->pL[count + 3281];
				temp += brightAmbience->pL[count + 3253];
				temp += brightAmbience->pL[count + 3229];
				temp += brightAmbience->pL[count + 3221];
				temp += brightAmbience->pL[count + 3191];
				temp += brightAmbience->pL[count + 3167];
				temp += brightAmbience->pL[count + 3151];
				temp += brightAmbience->pL[count + 3139];
				temp += brightAmbience->pL[count + 3121];
				temp += brightAmbience->pL[count + 3113];
				temp += brightAmbience->pL[count + 3103];
				temp += brightAmbience->pL[count + 3083];
				temp += brightAmbience->pL[count + 3067];
				temp += brightAmbience->pL[count + 3043];
				temp += brightAmbience->pL[count + 3013];
				temp += brightAmbience->pL[count + 3007];
				temp += brightAmbience->pL[count + 2993];
				temp += brightAmbience->pL[count + 2981];
			case 7:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 2963];
				temp += brightAmbience->pL[count + 2957];
				temp += brightAmbience->pL[count + 2911];
				temp += brightAmbience->pL[count + 2897];
				temp += brightAmbience->pL[count + 2879];
				temp += brightAmbience->pL[count + 2861];
				temp += brightAmbience->pL[count + 2837];
				temp += brightAmbience->pL[count + 2801];
				temp += brightAmbience->pL[count + 2777];
				temp += brightAmbience->pL[count + 2767];
				temp += brightAmbience->pL[count + 2731];
				temp += brightAmbience->pL[count + 2707];
				temp += brightAmbience->pL[count + 2683];
				temp += brightAmbience->pL[count + 2671];
				temp += brightAmbience->pL[count + 2663];
				temp += brightAmbience->pL[count + 2647];
				temp += brightAmbience->pL[count + 2623];
				temp += brightAmbience->pL[count + 2603];
				temp += brightAmbience->pL[count + 2579];
				temp += brightAmbience->pL[count + 2563];
			case 6:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 2533];
				temp += brightAmbience->pL[count + 2509];
				temp += brightAmbience->pL[count + 2497];
				temp += brightAmbience->pL[count + 2479];
				temp += brightAmbience->pL[count + 2459];
				temp += brightAmbience->pL[count + 2441];
				temp += brightAmbience->pL[count + 2431];
				temp += brightAmbience->pL[count + 2419];
				temp += brightAmbience->pL[count + 2377];
				temp += brightAmbience->pL[count + 2353];
				temp += brightAmbience->pL[count + 2341];
				temp += brightAmbience->pL[count + 2329];
				temp += brightAmbience->pL[count + 2299];
				temp += brightAmbience->pL[count + 2293];
				temp += brightAmbience->pL[count + 2267];
				temp += brightAmbience->pL[count + 2251];
				temp += brightAmbience->pL[count + 2239];
				temp += brightAmbience->pL[count + 2227];
				temp += brightAmbience->pL[count + 2183];
				temp += brightAmbience->pL[count + 2167];
			case 5:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 2153];
				temp += brightAmbience->pL[count + 2129];
				temp += brightAmbience->pL[count + 2117];
				temp += brightAmbience->pL[count + 2083];
				temp += brightAmbience->pL[count + 2069];
				temp += brightAmbience->pL[count + 2047];
				temp += brightAmbience->pL[count + 2039];
				temp += brightAmbience->pL[count + 2027];
				temp += brightAmbience->pL[count + 2011];
				temp += brightAmbience->pL[count + 1969];
				temp += brightAmbience->pL[count + 1951];
				temp += brightAmbience->pL[count + 1919];
				temp += brightAmbience->pL[count + 1909];
				temp += brightAmbience->pL[count + 1891];
				temp += brightAmbience->pL[count + 1871];
				temp += brightAmbience->pL[count + 1847];
				temp += brightAmbience->pL[count + 1837];
				temp += brightAmbience->pL[count + 1819];
				temp += brightAmbience->pL[count + 1807];
				temp += brightAmbience->pL[count + 1787];
			case 4:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 1777];
				temp += brightAmbience->pL[count + 1747];
				temp += brightAmbience->pL[count + 1711];
				temp += brightAmbience->pL[count + 1693];
				temp += brightAmbience->pL[count + 1679];
				temp += brightAmbience->pL[count + 1657];
				temp += brightAmbience->pL[count + 1639];
				temp += brightAmbience->pL[count + 1633];
				temp += brightAmbience->pL[count + 1609];
				temp += brightAmbience->pL[count + 1579];
				temp += brightAmbience->pL[count + 1549];
				temp += brightAmbience->pL[count + 1523];
				temp += brightAmbience->pL[count + 1513];
				temp += brightAmbience->pL[count + 1501];
				temp += brightAmbience->pL[count + 1483];
				temp += brightAmbience->pL[count + 1469];
				temp += brightAmbience->pL[count + 1451];
				temp += brightAmbience->pL[count + 1439];
				temp += brightAmbience->pL[count + 1427];
				temp += brightAmbience->pL[count + 1403];
			case 3:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 1373];
				temp += brightAmbience->pL[count + 1357];
				temp += brightAmbience->pL[count + 1343];
				temp += brightAmbience->pL[count + 1331];
				temp += brightAmbience->pL[count + 1319];
				temp += brightAmbience->pL[count + 1291];
				temp += brightAmbience->pL[count + 1279];
				temp += brightAmbience->pL[count + 1271];
				temp += brightAmbience->pL[count + 1249];
				temp += brightAmbience->pL[count + 1241];
				temp += brightAmbience->pL[count + 1217];
				temp += brightAmbience->pL[count + 1187];
				temp += brightAmbience->pL[count + 1147];
				temp += brightAmbience->pL[count + 1121];
				temp += brightAmbience->pL[count + 1109];
				temp += brightAmbience->pL[count + 1087];
				temp += brightAmbience->pL[count + 1067];
				temp += brightAmbience->pL[count + 1051];
				temp += brightAmbience->pL[count + 1037];
				temp += brightAmbience->pL[count + 1019];
			case 2:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 1009];
				temp += brightAmbience->pL[count + 991];
				temp += brightAmbience->pL[count + 971];
				temp += brightAmbience->pL[count + 961];
				temp += brightAmbience->pL[count + 947];
				temp += brightAmbience->pL[count + 923];
				temp += brightAmbience->pL[count + 913];
				temp += brightAmbience->pL[count + 899];
				temp += brightAmbience->pL[count + 869];
				temp += brightAmbience->pL[count + 857];
				temp += brightAmbience->pL[count + 829];
				temp += brightAmbience->pL[count + 811];
				temp += brightAmbience->pL[count + 803];
				temp += brightAmbience->pL[count + 793];
				temp += brightAmbience->pL[count + 767];
				temp += brightAmbience->pL[count + 727];
				temp += brightAmbience->pL[count + 713];
				temp += brightAmbience->pL[count + 691];
				temp += brightAmbience->pL[count + 671];
				temp += brightAmbience->pL[count + 653];
			case 1:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 643];
				temp += brightAmbience->pL[count + 619];
				temp += brightAmbience->pL[count + 613];
				temp += brightAmbience->pL[count + 601];
				temp += brightAmbience->pL[count + 593];
				temp += brightAmbience->pL[count + 577];
				temp += brightAmbience->pL[count + 557];
				temp += brightAmbience->pL[count + 529];
				temp += brightAmbience->pL[count + 521];
				temp += brightAmbience->pL[count + 503];
				temp += brightAmbience->pL[count + 491];
				temp += brightAmbience->pL[count + 463];
				temp += brightAmbience->pL[count + 457];
				temp += brightAmbience->pL[count + 421];
				temp += brightAmbience->pL[count + 407];
				temp += brightAmbience->pL[count + 389];
				temp += brightAmbience->pL[count + 379];
				temp += brightAmbience->pL[count + 361];
				temp += brightAmbience->pL[count + 341];
				temp += brightAmbience->pL[count + 331];
			case 0:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pL[count + 313];
				temp += brightAmbience->pL[count + 289];
				temp += brightAmbience->pL[count + 271];
				temp += brightAmbience->pL[count + 239];
				temp += brightAmbience->pL[count + 223];
				temp += brightAmbience->pL[count + 209];
				temp += brightAmbience->pL[count + 191];
				temp += brightAmbience->pL[count + 181];
				temp += brightAmbience->pL[count + 151];
				temp += brightAmbience->pL[count + 143];
				temp += brightAmbience->pL[count + 121];
				temp += brightAmbience->pL[count + 109];
				temp += brightAmbience->pL[count + 73];
				temp += brightAmbience->pL[count + 47];
				temp += brightAmbience->pL[count + 41];
				temp += brightAmbience->pL[count + 19];
				temp += brightAmbience->pL[count + 11];
				// Highest Offset = 12679: gcount limit is 12680, Reset() goes to 25360, definition must be p[25361]
		}
		inputSampleL = (inputSampleL * (1 - wet)) + ((double) (temp / (8388352.0 * scale)) * wet);
		// end L

		// begin R
		temp = 0;
		switch (cpu) {
			case 32:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 12679];
				temp += brightAmbience->pR[count + 12671];
				temp += brightAmbience->pR[count + 12661];
				temp += brightAmbience->pR[count + 12613];
				temp += brightAmbience->pR[count + 12587];
				temp += brightAmbience->pR[count + 12577];
				temp += brightAmbience->pR[count + 12563];
				temp += brightAmbience->pR[count + 12557];
				temp += brightAmbience->pR[count + 12521];
				temp += brightAmbience->pR[count + 12499];
				temp += brightAmbience->pR[count + 12493];
				temp += brightAmbience->pR[count + 12457];
				temp += brightAmbience->pR[count + 12427];
				temp += brightAmbience->pR[count + 12401];
				temp += brightAmbience->pR[count + 12373];
				temp += brightAmbience->pR[count + 12367];
				temp += brightAmbience->pR[count + 12349];
				temp += brightAmbience->pR[count + 12337];
				temp += brightAmbience->pR[count + 12323];
				temp += brightAmbience->pR[count + 12301];
			case 31:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 12281];
				temp += brightAmbience->pR[count + 12247];
				temp += brightAmbience->pR[count + 12239];
				temp += brightAmbience->pR[count + 12203];
				temp += brightAmbience->pR[count + 12197];
				temp += brightAmbience->pR[count + 12191];
				temp += brightAmbience->pR[count + 12163];
				temp += brightAmbience->pR[count + 12143];
				temp += brightAmbience->pR[count + 12127];
				temp += brightAmbience->pR[count + 12109];
				temp += brightAmbience->pR[count + 12083];
				temp += brightAmbience->pR[count + 12077];
				temp += brightAmbience->pR[count + 12059];
				temp += brightAmbience->pR[count + 12037];
				temp += brightAmbience->pR[count + 11989];
				temp += brightAmbience->pR[count + 11969];
				temp += brightAmbience->pR[count + 11951];
				temp += brightAmbience->pR[count + 11933];
				temp += brightAmbience->pR[count + 11927];
				temp += brightAmbience->pR[count + 11911];
			case 30:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 11897];
				temp += brightAmbience->pR[count + 11873];
				temp += brightAmbience->pR[count + 11843];
				temp += brightAmbience->pR[count + 11819];
				temp += brightAmbience->pR[count + 11791];
				temp += brightAmbience->pR[count + 11773];
				temp += brightAmbience->pR[count + 11731];
				temp += brightAmbience->pR[count + 11723];
				temp += brightAmbience->pR[count + 11701];
				temp += brightAmbience->pR[count + 11687];
				temp += brightAmbience->pR[count + 11659];
				temp += brightAmbience->pR[count + 11623];
				temp += brightAmbience->pR[count + 11611];
				temp += brightAmbience->pR[count + 11587];
				temp += brightAmbience->pR[count + 11573];
				temp += brightAmbience->pR[count + 11549];
				temp += brightAmbience->pR[count + 11537];
				temp += brightAmbience->pR[count + 11527];
				temp += brightAmbience->pR[count + 11513];
				temp += brightAmbience->pR[count + 11503];
			case 29:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 11467];
				temp += brightAmbience->pR[count + 11441];
				temp += brightAmbience->pR[count + 11413];
				temp += brightAmbience->pR[count + 11399];
				temp += brightAmbience->pR[count + 11363];
				temp += brightAmbience->pR[count + 11353];
				temp += brightAmbience->pR[count + 11329];
				temp += brightAmbience->pR[count + 11323];
				temp += brightAmbience->pR[count + 11303];
				temp += brightAmbience->pR[count + 11293];
				temp += brightAmbience->pR[count + 11281];
				temp += brightAmbience->pR[count + 11251];
				temp += brightAmbience->pR[count + 11231];
				temp += brightAmbience->pR[count + 11209];
				temp += brightAmbience->pR[count + 11201];
				temp += brightAmbience->pR[count + 11177];
				temp += brightAmbience->pR[count + 11171];
				temp += brightAmbience->pR[count + 11153];
				temp += brightAmbience->pR[count + 11141];
				temp += brightAmbience->pR[count + 11117];
			case 28:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 11099];
				temp += brightAmbience->pR[count + 11071];
				temp += brightAmbience->pR[count + 11047];
				temp += brightAmbience->pR[count + 11033];
				temp += brightAmbience->pR[count + 11021];
				temp += brightAmbience->pR[count + 10993];
				temp += brightAmbience->pR[count + 10973];
				temp += brightAmbience->pR[count + 10943];
				temp += brightAmbience->pR[count + 10933];
				temp += brightAmbience->pR[count + 10919];
				temp += brightAmbience->pR[count + 10897];
				temp += brightAmbience->pR[count + 10879];
				temp += brightAmbience->pR[count + 10867];
				temp += brightAmbience->pR[count + 10841];
				temp += brightAmbience->pR[count + 10783];
				temp += brightAmbience->pR[count + 10763];
				temp += brightAmbience->pR[count + 10751];
				temp += brightAmbience->pR[count + 10733];
				temp += brightAmbience->pR[count + 10721];
				temp += brightAmbience->pR[count + 10687];
			case 27:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 10669];
				temp += brightAmbience->pR[count + 10657];
				temp += brightAmbience->pR[count + 10649];
				temp += brightAmbience->pR[count + 10637];
				temp += brightAmbience->pR[count + 10621];
				temp += brightAmbience->pR[count + 10609];
				temp += brightAmbience->pR[count + 10603];
				temp += brightAmbience->pR[count + 10597];
				temp += brightAmbience->pR[count + 10571];
				temp += brightAmbience->pR[count + 10561];
				temp += brightAmbience->pR[count + 10553];
				temp += brightAmbience->pR[count + 10499];
				temp += brightAmbience->pR[count + 10481];
				temp += brightAmbience->pR[count + 10453];
				temp += brightAmbience->pR[count + 10439];
				temp += brightAmbience->pR[count + 10417];
				temp += brightAmbience->pR[count + 10399];
				temp += brightAmbience->pR[count + 10391];
				temp += brightAmbience->pR[count + 10363];
				temp += brightAmbience->pR[count + 10343];
			case 26:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 10321];
				temp += brightAmbience->pR[count + 10301];
				temp += brightAmbience->pR[count + 10261];
				temp += brightAmbience->pR[count + 10253];
				temp += brightAmbience->pR[count + 10231];
				temp += brightAmbience->pR[count + 10211];
				temp += brightAmbience->pR[count + 10189];
				temp += brightAmbience->pR[count + 10159];
				temp += brightAmbience->pR[count + 10139];
				temp += brightAmbience->pR[count + 10099];
				temp += brightAmbience->pR[count + 10093];
				temp += brightAmbience->pR[count + 10079];
				temp += brightAmbience->pR[count + 10039];
				temp += brightAmbience->pR[count + 9983];
				temp += brightAmbience->pR[count + 9973];
				temp += brightAmbience->pR[count + 9953];
				temp += brightAmbience->pR[count + 9929];
				temp += brightAmbience->pR[count + 9911];
				temp += brightAmbience->pR[count + 9893];
				temp += brightAmbience->pR[count + 9871];
			case 25:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 9851];
				temp += brightAmbience->pR[count + 9817];
				temp += brightAmbience->pR[count + 9773];
				temp += brightAmbience->pR[count + 9763];
				temp += brightAmbience->pR[count + 9721];
				temp += brightAmbience->pR[count + 9703];
				temp += brightAmbience->pR[count + 9697];
				temp += brightAmbience->pR[count + 9673];
				temp += brightAmbience->pR[count + 9641];
				temp += brightAmbience->pR[count + 9631];
				temp += brightAmbience->pR[count + 9619];
				temp += brightAmbience->pR[count + 9607];
				temp += brightAmbience->pR[count + 9581];
				temp += brightAmbience->pR[count + 9559];
				temp += brightAmbience->pR[count + 9547];
				temp += brightAmbience->pR[count + 9521];
				temp += brightAmbience->pR[count + 9493];
				temp += brightAmbience->pR[count + 9487];
				temp += brightAmbience->pR[count + 9469];
				temp += brightAmbience->pR[count + 9449];
			case 24:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 9433];
				temp += brightAmbience->pR[count + 9413];
				temp += brightAmbience->pR[count + 9403];
				temp += brightAmbience->pR[count + 9391];
				temp += brightAmbience->pR[count + 9371];
				temp += brightAmbience->pR[count + 9349];
				temp += brightAmbience->pR[count + 9323];
				temp += brightAmbience->pR[count + 9311];
				temp += brightAmbience->pR[count + 9301];
				temp += brightAmbience->pR[count + 9277];
				temp += brightAmbience->pR[count + 9257];
				temp += brightAmbience->pR[count + 9239];
				temp += brightAmbience->pR[count + 9227];
				temp += brightAmbience->pR[count + 9221];
				temp += brightAmbience->pR[count + 9169];
				temp += brightAmbience->pR[count + 9161];
				temp += brightAmbience->pR[count + 9151];
				temp += brightAmbience->pR[count + 9137];
				temp += brightAmbience->pR[count + 9109];
				temp += brightAmbience->pR[count + 9101];
			case 23:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 9089];
				temp += brightAmbience->pR[count + 9071];
				temp += brightAmbience->pR[count + 9043];
				temp += brightAmbience->pR[count + 9019];
				temp += brightAmbience->pR[count + 9007];
				temp += brightAmbience->pR[count + 8971];
				temp += brightAmbience->pR[count + 8963];
				temp += brightAmbience->pR[count + 8929];
				temp += brightAmbience->pR[count + 8917];
				temp += brightAmbience->pR[count + 8893];
				temp += brightAmbience->pR[count + 8863];
				temp += brightAmbience->pR[count + 8849];
				temp += brightAmbience->pR[count + 8837];
				temp += brightAmbience->pR[count + 8803];
				temp += brightAmbience->pR[count + 8779];
				temp += brightAmbience->pR[count + 8761];
				temp += brightAmbience->pR[count + 8753];
				temp += brightAmbience->pR[count + 8731];
				temp += brightAmbience->pR[count + 8699];
				temp += brightAmbience->pR[count + 8677];
			case 22:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 8669];
				temp += brightAmbience->pR[count + 8657];
				temp += brightAmbience->pR[count + 8647];
				temp += brightAmbience->pR[count + 8623];
				temp += brightAmbience->pR[count + 8599];
				temp += brightAmbience->pR[count + 8581];
				temp += brightAmbience->pR[count + 8569];
				temp += brightAmbience->pR[count + 8563];
				temp += brightAmbience->pR[count + 8539];
				temp += brightAmbience->pR[count + 8521];
				temp += brightAmbience->pR[count + 8507];
				temp += brightAmbience->pR[count + 8497];
				temp += brightAmbience->pR[count + 8473];
				temp += brightAmbience->pR[count + 8437];
				temp += brightAmbience->pR[count + 8411];
				temp += brightAmbience->pR[count + 8387];
				temp += brightAmbience->pR[count + 8371];
				temp += brightAmbience->pR[count + 8363];
				temp += brightAmbience->pR[count + 8339];
				temp += brightAmbience->pR[count + 8303];
			case 21:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 8287];
				temp += brightAmbience->pR[count + 8273];
				temp += brightAmbience->pR[count + 8233];
				temp += brightAmbience->pR[count + 8221];
				temp += brightAmbience->pR[count + 8209];
				temp += brightAmbience->pR[count + 8201];
				temp += brightAmbience->pR[count + 8173];
				temp += brightAmbience->pR[count + 8161];
				temp += brightAmbience->pR[count + 8143];
				temp += brightAmbience->pR[count + 8117];
				temp += brightAmbience->pR[count + 8107];
				temp += brightAmbience->pR[count + 8083];
				temp += brightAmbience->pR[count + 8069];
				temp += brightAmbience->pR[count + 8059];
				temp += brightAmbience->pR[count + 8023];
				temp += brightAmbience->pR[count + 8009];
				temp += brightAmbience->pR[count + 7991];
				temp += brightAmbience->pR[count + 7943];
				temp += brightAmbience->pR[count + 7927];
				temp += brightAmbience->pR[count + 7909];
			case 20:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 7879];
				temp += brightAmbience->pR[count + 7871];
				temp += brightAmbience->pR[count + 7841];
				temp += brightAmbience->pR[count + 7831];
				temp += brightAmbience->pR[count + 7813];
				temp += brightAmbience->pR[count + 7799];
				temp += brightAmbience->pR[count + 7783];
				temp += brightAmbience->pR[count + 7747];
				temp += brightAmbience->pR[count + 7733];
				temp += brightAmbience->pR[count + 7709];
				temp += brightAmbience->pR[count + 7673];
				temp += brightAmbience->pR[count + 7661];
				temp += brightAmbience->pR[count + 7633];
				temp += brightAmbience->pR[count + 7627];
				temp += brightAmbience->pR[count + 7607];
				temp += brightAmbience->pR[count + 7597];
				temp += brightAmbience->pR[count + 7561];
				temp += brightAmbience->pR[count + 7543];
				temp += brightAmbience->pR[count + 7529];
				temp += brightAmbience->pR[count + 7513];
			case 19:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 7481];
				temp += brightAmbience->pR[count + 7447];
				temp += brightAmbience->pR[count + 7429];
				temp += brightAmbience->pR[count + 7409];
				temp += brightAmbience->pR[count + 7397];
				temp += brightAmbience->pR[count + 7369];
				temp += brightAmbience->pR[count + 7363];
				temp += brightAmbience->pR[count + 7319];
				temp += brightAmbience->pR[count + 7291];
				temp += brightAmbience->pR[count + 7277];
				temp += brightAmbience->pR[count + 7267];
				temp += brightAmbience->pR[count + 7249];
				temp += brightAmbience->pR[count + 7243];
				temp += brightAmbience->pR[count + 7237];
				temp += brightAmbience->pR[count + 7187];
				temp += brightAmbience->pR[count + 7171];
				temp += brightAmbience->pR[count + 7151];
				temp += brightAmbience->pR[count + 7117];
				temp += brightAmbience->pR[count + 7099];
				temp += brightAmbience->pR[count + 7087];
			case 18:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 7069];
				temp += brightAmbience->pR[count + 7061];
				temp += brightAmbience->pR[count + 7039];
				temp += brightAmbience->pR[count + 6997];
				temp += brightAmbience->pR[count + 6983];
				temp += brightAmbience->pR[count + 6971];
				temp += brightAmbience->pR[count + 6953];
				temp += brightAmbience->pR[count + 6941];
				temp += brightAmbience->pR[count + 6919];
				temp += brightAmbience->pR[count + 6913];
				temp += brightAmbience->pR[count + 6907];
				temp += brightAmbience->pR[count + 6859];
				temp += brightAmbience->pR[count + 6847];
				temp += brightAmbience->pR[count + 6829];
				temp += brightAmbience->pR[count + 6823];
				temp += brightAmbience->pR[count + 6809];
				temp += brightAmbience->pR[count + 6787];
				temp += brightAmbience->pR[count + 6763];
				temp += brightAmbience->pR[count + 6749];
				temp += brightAmbience->pR[count + 6731];
			case 17:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 6707];
				temp += brightAmbience->pR[count + 6677];
				temp += brightAmbience->pR[count + 6653];
				temp += brightAmbience->pR[count + 6641];
				temp += brightAmbience->pR[count + 6623];
				temp += brightAmbience->pR[count + 6607];
				temp += brightAmbience->pR[count + 6581];
				temp += brightAmbience->pR[count + 6563];
				temp += brightAmbience->pR[count + 6553];
				temp += brightAmbience->pR[count + 6539];
				temp += brightAmbience->pR[count + 6511];
				temp += brightAmbience->pR[count + 6499];
				temp += brightAmbience->pR[count + 6487];
				temp += brightAmbience->pR[count + 6469];
				temp += brightAmbience->pR[count + 6457];
				temp += brightAmbience->pR[count + 6449];
				temp += brightAmbience->pR[count + 6439];
				temp += brightAmbience->pR[count + 6413];
				temp += brightAmbience->pR[count + 6397];
				temp += brightAmbience->pR[count + 6373];
			case 16:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 6353];
				temp += brightAmbience->pR[count + 6329];
				temp += brightAmbience->pR[count + 6317];
				temp += brightAmbience->pR[count + 6301];
				temp += brightAmbience->pR[count + 6281];
				temp += brightAmbience->pR[count + 6263];
				temp += brightAmbience->pR[count + 6253];
				temp += brightAmbience->pR[count + 6233];
				temp += brightAmbience->pR[count + 6197];
				temp += brightAmbience->pR[count + 6163];
				temp += brightAmbience->pR[count + 6137];
				temp += brightAmbience->pR[count + 6127];
				temp += brightAmbience->pR[count + 6109];
				temp += brightAmbience->pR[count + 6077];
				temp += brightAmbience->pR[count + 6067];
				temp += brightAmbience->pR[count + 6049];
				temp += brightAmbience->pR[count + 6031];
				temp += brightAmbience->pR[count + 6011];
				temp += brightAmbience->pR[count + 6001];
				temp += brightAmbience->pR[count + 5977];
			case 15:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 5959];
				temp += brightAmbience->pR[count + 5947];
				temp += brightAmbience->pR[count + 5933];
				temp += brightAmbience->pR[count + 5921];
				temp += brightAmbience->pR[count + 5911];
				temp += brightAmbience->pR[count + 5897];
				temp += brightAmbience->pR[count + 5879];
				temp += brightAmbience->pR[count + 5849];
				temp += brightAmbience->pR[count + 5833];
				temp += brightAmbience->pR[count + 5813];
				temp += brightAmbience->pR[count + 5807];
				temp += brightAmbience->pR[count + 5773];
				temp += brightAmbience->pR[count + 5743];
				temp += brightAmbience->pR[count + 5737];
				temp += brightAmbience->pR[count + 5729];
				temp += brightAmbience->pR[count + 5717];
				temp += brightAmbience->pR[count + 5699];
				temp += brightAmbience->pR[count + 5669];
				temp += brightAmbience->pR[count + 5641];
				temp += brightAmbience->pR[count + 5627];
			case 14:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 5611];
				temp += brightAmbience->pR[count + 5599];
				temp += brightAmbience->pR[count + 5587];
				temp += brightAmbience->pR[count + 5569];
				temp += brightAmbience->pR[count + 5549];
				temp += brightAmbience->pR[count + 5521];
				temp += brightAmbience->pR[count + 5503];
				temp += brightAmbience->pR[count + 5483];
				temp += brightAmbience->pR[count + 5461];
				temp += brightAmbience->pR[count + 5429];
				temp += brightAmbience->pR[count + 5401];
				temp += brightAmbience->pR[count + 5377];
				temp += brightAmbience->pR[count + 5353];
				temp += brightAmbience->pR[count + 5333];
				temp += brightAmbience->pR[count + 5323];
				temp += brightAmbience->pR[count + 5267];
				temp += brightAmbience->pR[count + 5261];
				temp += brightAmbience->pR[count + 5237];
				temp += brightAmbience->pR[count + 5213];
				temp += brightAmbience->pR[count + 5191];
			case 13:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 5179];
				temp += brightAmbience->pR[count + 5147];
				temp += brightAmbience->pR[count + 5113];
				temp += brightAmbience->pR[count + 5101];
				temp += brightAmbience->pR[count + 5087];
				temp += brightAmbience->pR[count + 5081];
				temp += brightAmbience->pR[count + 5059];
				temp += brightAmbience->pR[count + 5053];
				temp += brightAmbience->pR[count + 5021];
				temp += brightAmbience->pR[count + 5009];
				temp += brightAmbience->pR[count + 4979];
				temp += brightAmbience->pR[count + 4961];
				temp += brightAmbience->pR[count + 4939];
				temp += brightAmbience->pR[count + 4931];
				temp += brightAmbience->pR[count + 4903];
				temp += brightAmbience->pR[count + 4891];
				temp += brightAmbience->pR[count + 4877];
				temp += brightAmbience->pR[count + 4871];
				temp += brightAmbience->pR[count + 4861];
				temp += brightAmbience->pR[count + 4849];
			case 12:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 4813];
				temp += brightAmbience->pR[count + 4799];
				temp += brightAmbience->pR[count + 4777];
				temp += brightAmbience->pR[count + 4757];
				temp += brightAmbience->pR[count + 4733];
				temp += brightAmbience->pR[count + 4703];
				temp += brightAmbience->pR[count + 4679];
				temp += brightAmbience->pR[count + 4657];
				temp += brightAmbience->pR[count + 4643];
				temp += brightAmbience->pR[count + 4631];
				temp += brightAmbience->pR[count + 4601];
				temp += brightAmbience->pR[count + 4591];
				temp += brightAmbience->pR[count + 4577];
				temp += brightAmbience->pR[count + 4547];
				temp += brightAmbience->pR[count + 4517];
				temp += brightAmbience->pR[count + 4507];
				temp += brightAmbience->pR[count + 4489];
				temp += brightAmbience->pR[count + 4471];
				temp += brightAmbience->pR[count + 4439];
				temp += brightAmbience->pR[count + 4421];
			case 11:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 4399];
				temp += brightAmbience->pR[count + 4367];
				temp += brightAmbience->pR[count + 4357];
				temp += brightAmbience->pR[count + 4343];
				temp += brightAmbience->pR[count + 4331];
				temp += brightAmbience->pR[count + 4307];
				temp += brightAmbience->pR[count + 4289];
				temp += brightAmbience->pR[count + 4279];
				temp += brightAmbience->pR[count + 4243];
				temp += brightAmbience->pR[count + 4223];
				temp += brightAmbience->pR[count + 4213];
				temp += brightAmbience->pR[count + 4187];
				temp += brightAmbience->pR[count + 4181];
				temp += brightAmbience->pR[count + 4159];
				temp += brightAmbience->pR[count + 4147];
				temp += brightAmbience->pR[count + 4133];
				temp += brightAmbience->pR[count + 4103];
				temp += brightAmbience->pR[count + 4097];
				temp += brightAmbience->pR[count + 4087];
				temp += brightAmbience->pR[count + 4073];
			case 10:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 4033];
				temp += brightAmbience->pR[count + 4009];
				temp += brightAmbience->pR[count + 4001];
				temp += brightAmbience->pR[count + 3979];
				temp += brightAmbience->pR[count + 3961];
				temp += brightAmbience->pR[count + 3937];
				temp += brightAmbience->pR[count + 3919];
				temp += brightAmbience->pR[count + 3893];
				temp += brightAmbience->pR[count + 3877];
				temp += brightAmbience->pR[count + 3851];
				temp += brightAmbience->pR[count + 3833];
				temp += brightAmbience->pR[count + 3821];
				temp += brightAmbience->pR[count + 3799];
				temp += brightAmbience->pR[count + 3791];
				temp += brightAmbience->pR[count + 3769];
				temp += brightAmbience->pR[count + 3757];
				temp += brightAmbience->pR[count + 3733];
				temp += brightAmbience->pR[count + 3721];
				temp += brightAmbience->pR[count + 3707];
				temp += brightAmbience->pR[count + 3691];
			case 9:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 3677];
				temp += brightAmbience->pR[count + 3667];
				temp += brightAmbience->pR[count + 3653];
				temp += brightAmbience->pR[count + 3631];
				temp += brightAmbience->pR[count + 3613];
				temp += brightAmbience->pR[count + 3593];
				temp += brightAmbience->pR[count + 3581];
				temp += brightAmbience->pR[count + 3559];
				temp += brightAmbience->pR[count + 3553];
				temp += brightAmbience->pR[count + 3523];
				temp += brightAmbience->pR[count + 3503];
				temp += brightAmbience->pR[count + 3469];
				temp += brightAmbience->pR[count + 3457];
				temp += brightAmbience->pR[count + 3443];
				temp += brightAmbience->pR[count + 3431];
				temp += brightAmbience->pR[count + 3419];
				temp += brightAmbience->pR[count + 3391];
				temp += brightAmbience->pR[count + 3377];
				temp += brightAmbience->pR[count + 3341];
				temp += brightAmbience->pR[count + 3329];
			case 8:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 3299];
				temp += brightAmbience->pR[count + 3289];
				temp += brightAmbience->pR[count + 3281];
				temp += brightAmbience->pR[count + 3253];
				temp += brightAmbience->pR[count + 3229];
				temp += brightAmbience->pR[count + 3221];
				temp += brightAmbience->pR[count + 3191];
				temp += brightAmbience->pR[count + 3167];
				temp += brightAmbience->pR[count + 3151];
				temp += brightAmbience->pR[count + 3139];
				temp += brightAmbience->pR[count + 3121];
				temp += brightAmbience->pR[count + 3113];
				temp += brightAmbience->pR[count + 3103];
				temp += brightAmbience->pR[count + 3083];
				temp += brightAmbience->pR[count + 3067];
				temp += brightAmbience->pR[count + 3043];
				temp += brightAmbience->pR[count + 3013];
				temp += brightAmbience->pR[count + 3007];
				temp += brightAmbience->pR[count + 2993];
				temp += brightAmbience->pR[count + 2981];
			case 7:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 2963];
				temp += brightAmbience->pR[count + 2957];
				temp += brightAmbience->pR[count + 2911];
				temp += brightAmbience->pR[count + 2897];
				temp += brightAmbience->pR[count + 2879];
				temp += brightAmbience->pR[count + 2861];
				temp += brightAmbience->pR[count + 2837];
				temp += brightAmbience->pR[count + 2801];
				temp += brightAmbience->pR[count + 2777];
				temp += brightAmbience->pR[count + 2767];
				temp += brightAmbience->pR[count + 2731];
				temp += brightAmbience->pR[count + 2707];
				temp += brightAmbience->pR[count + 2683];
				temp += brightAmbience->pR[count + 2671];
				temp += brightAmbience->pR[count + 2663];
				temp += brightAmbience->pR[count + 2647];
				temp += brightAmbience->pR[count + 2623];
				temp += brightAmbience->pR[count + 2603];
				temp += brightAmbience->pR[count + 2579];
				temp += brightAmbience->pR[count + 2563];
			case 6:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 2533];
				temp += brightAmbience->pR[count + 2509];
				temp += brightAmbience->pR[count + 2497];
				temp += brightAmbience->pR[count + 2479];
				temp += brightAmbience->pR[count + 2459];
				temp += brightAmbience->pR[count + 2441];
				temp += brightAmbience->pR[count + 2431];
				temp += brightAmbience->pR[count + 2419];
				temp += brightAmbience->pR[count + 2377];
				temp += brightAmbience->pR[count + 2353];
				temp += brightAmbience->pR[count + 2341];
				temp += brightAmbience->pR[count + 2329];
				temp += brightAmbience->pR[count + 2299];
				temp += brightAmbience->pR[count + 2293];
				temp += brightAmbience->pR[count + 2267];
				temp += brightAmbience->pR[count + 2251];
				temp += brightAmbience->pR[count + 2239];
				temp += brightAmbience->pR[count + 2227];
				temp += brightAmbience->pR[count + 2183];
				temp += brightAmbience->pR[count + 2167];
			case 5:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 2153];
				temp += brightAmbience->pR[count + 2129];
				temp += brightAmbience->pR[count + 2117];
				temp += brightAmbience->pR[count + 2083];
				temp += brightAmbience->pR[count + 2069];
				temp += brightAmbience->pR[count + 2047];
				temp += brightAmbience->pR[count + 2039];
				temp += brightAmbience->pR[count + 2027];
				temp += brightAmbience->pR[count + 2011];
				temp += brightAmbience->pR[count + 1969];
				temp += brightAmbience->pR[count + 1951];
				temp += brightAmbience->pR[count + 1919];
				temp += brightAmbience->pR[count + 1909];
				temp += brightAmbience->pR[count + 1891];
				temp += brightAmbience->pR[count + 1871];
				temp += brightAmbience->pR[count + 1847];
				temp += brightAmbience->pR[count + 1837];
				temp += brightAmbience->pR[count + 1819];
				temp += brightAmbience->pR[count + 1807];
				temp += brightAmbience->pR[count + 1787];
			case 4:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 1777];
				temp += brightAmbience->pR[count + 1747];
				temp += brightAmbience->pR[count + 1711];
				temp += brightAmbience->pR[count + 1693];
				temp += brightAmbience->pR[count + 1679];
				temp += brightAmbience->pR[count + 1657];
				temp += brightAmbience->pR[count + 1639];
				temp += brightAmbience->pR[count + 1633];
				temp += brightAmbience->pR[count + 1609];
				temp += brightAmbience->pR[count + 1579];
				temp += brightAmbience->pR[count + 1549];
				temp += brightAmbience->pR[count + 1523];
				temp += brightAmbience->pR[count + 1513];
				temp += brightAmbience->pR[count + 1501];
				temp += brightAmbience->pR[count + 1483];
				temp += brightAmbience->pR[count + 1469];
				temp += brightAmbience->pR[count + 1451];
				temp += brightAmbience->pR[count + 1439];
				temp += brightAmbience->pR[count + 1427];
				temp += brightAmbience->pR[count + 1403];
			case 3:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 1373];
				temp += brightAmbience->pR[count + 1357];
				temp += brightAmbience->pR[count + 1343];
				temp += brightAmbience->pR[count + 1331];
				temp += brightAmbience->pR[count + 1319];
				temp += brightAmbience->pR[count + 1291];
				temp += brightAmbience->pR[count + 1279];
				temp += brightAmbience->pR[count + 1271];
				temp += brightAmbience->pR[count + 1249];
				temp += brightAmbience->pR[count + 1241];
				temp += brightAmbience->pR[count + 1217];
				temp += brightAmbience->pR[count + 1187];
				temp += brightAmbience->pR[count + 1147];
				temp += brightAmbience->pR[count + 1121];
				temp += brightAmbience->pR[count + 1109];
				temp += brightAmbience->pR[count + 1087];
				temp += brightAmbience->pR[count + 1067];
				temp += brightAmbience->pR[count + 1051];
				temp += brightAmbience->pR[count + 1037];
				temp += brightAmbience->pR[count + 1019];
			case 2:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 1009];
				temp += brightAmbience->pR[count + 991];
				temp += brightAmbience->pR[count + 971];
				temp += brightAmbience->pR[count + 961];
				temp += brightAmbience->pR[count + 947];
				temp += brightAmbience->pR[count + 923];
				temp += brightAmbience->pR[count + 913];
				temp += brightAmbience->pR[count + 899];
				temp += brightAmbience->pR[count + 869];
				temp += brightAmbience->pR[count + 857];
				temp += brightAmbience->pR[count + 829];
				temp += brightAmbience->pR[count + 811];
				temp += brightAmbience->pR[count + 803];
				temp += brightAmbience->pR[count + 793];
				temp += brightAmbience->pR[count + 767];
				temp += brightAmbience->pR[count + 727];
				temp += brightAmbience->pR[count + 713];
				temp += brightAmbience->pR[count + 691];
				temp += brightAmbience->pR[count + 671];
				temp += brightAmbience->pR[count + 653];
			case 1:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 643];
				temp += brightAmbience->pR[count + 619];
				temp += brightAmbience->pR[count + 613];
				temp += brightAmbience->pR[count + 601];
				temp += brightAmbience->pR[count + 593];
				temp += brightAmbience->pR[count + 577];
				temp += brightAmbience->pR[count + 557];
				temp += brightAmbience->pR[count + 529];
				temp += brightAmbience->pR[count + 521];
				temp += brightAmbience->pR[count + 503];
				temp += brightAmbience->pR[count + 491];
				temp += brightAmbience->pR[count + 463];
				temp += brightAmbience->pR[count + 457];
				temp += brightAmbience->pR[count + 421];
				temp += brightAmbience->pR[count + 407];
				temp += brightAmbience->pR[count + 389];
				temp += brightAmbience->pR[count + 379];
				temp += brightAmbience->pR[count + 361];
				temp += brightAmbience->pR[count + 341];
				temp += brightAmbience->pR[count + 331];
			case 0:
				temp = (int32_t) ((double) temp / decay);
				temp += brightAmbience->pR[count + 313];
				temp += brightAmbience->pR[count + 289];
				temp += brightAmbience->pR[count + 271];
				temp += brightAmbience->pR[count + 239];
				temp += brightAmbience->pR[count + 223];
				temp += brightAmbience->pR[count + 209];
				temp += brightAmbience->pR[count + 191];
				temp += brightAmbience->pR[count + 181];
				temp += brightAmbience->pR[count + 151];
				temp += brightAmbience->pR[count + 143];
				temp += brightAmbience->pR[count + 121];
				temp += brightAmbience->pR[count + 109];
				temp += brightAmbience->pR[count + 73];
				temp += brightAmbience->pR[count + 47];
				temp += brightAmbience->pR[count + 41];
				temp += brightAmbience->pR[count + 19];
				temp += brightAmbience->pR[count + 11];
				// Highest Offset = 12679: gcount limit is 12680, Reset() goes to 25360, definition must be p[25361]
		}
		inputSampleR = (inputSampleR * (1 - wet)) + ((double) (temp / (8388352.0 * scale)) * wet);
		// end R

		brightAmbience->gcount--;

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		brightAmbience->fpdL ^= brightAmbience->fpdL << 13;
		brightAmbience->fpdL ^= brightAmbience->fpdL >> 17;
		brightAmbience->fpdL ^= brightAmbience->fpdL << 5;
		inputSampleL += (((double) brightAmbience->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		brightAmbience->fpdR ^= brightAmbience->fpdR << 13;
		brightAmbience->fpdR ^= brightAmbience->fpdR >> 17;
		brightAmbience->fpdR ^= brightAmbience->fpdR << 5;
		inputSampleR += (((double) brightAmbience->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	BRIGHTAMBIENCE_URI,
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
