#include <lv2/core/lv2.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define NIKOLA_URI "https://hannesbraun.net/ns/lv2/airwindows/nikola"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	VOLTAGE = 4,
	DRYWET = 5
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* voltage;
	const float* drywet;

	int framenumberL;
	bool wasNegativeL;
	double outlevelL;

	int framenumberR;
	bool wasNegativeR;
	double outlevelR;

	uint32_t fpdL;
	uint32_t fpdR;
} Nikola;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	Nikola* nikola = (Nikola*) calloc(1, sizeof(Nikola));
	return (LV2_Handle) nikola;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	Nikola* nikola = (Nikola*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			nikola->input[0] = (const float*) data;
			break;
		case INPUT_R:
			nikola->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			nikola->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			nikola->output[1] = (float*) data;
			break;
		case VOLTAGE:
			nikola->voltage = (const float*) data;
			break;
		case DRYWET:
			nikola->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	Nikola* nikola = (Nikola*) instance;
	nikola->wasNegativeL = false;
	nikola->outlevelL = 0.0;
	nikola->framenumberL = 0;

	nikola->wasNegativeR = false;
	nikola->outlevelR = 0.0;
	nikola->framenumberR = 0;

	nikola->fpdL = 1.0;
	while (nikola->fpdL < 16386) nikola->fpdL = rand() * UINT32_MAX;
	nikola->fpdR = 1.0;
	while (nikola->fpdR < 16386) nikola->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	Nikola* nikola = (Nikola*) instance;

	const float* in1 = nikola->input[0];
	const float* in2 = nikola->input[1];
	float* out1 = nikola->output[0];
	float* out2 = nikola->output[1];

	double decay = pow(*nikola->voltage, 3) * 32767.0;
	int highsBoost = 16 - (int)(pow(*nikola->voltage, 2) * 16);
	double wet = *nikola->drywet;

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = nikola->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = nikola->fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		double bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > nikola->outlevelL) nikola->outlevelL = bridgerectifier;
		else nikola->outlevelL = ((nikola->outlevelL * decay) + bridgerectifier) / (decay + 1.0);
		//set up envelope for handling quieter stuff if desired
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > nikola->outlevelR) nikola->outlevelR = bridgerectifier;
		else nikola->outlevelR = ((nikola->outlevelR * decay) + bridgerectifier) / (decay + 1.0);
		//set up envelope for handling quieter stuff if desired

		nikola->framenumberL += 1;
		nikola->framenumberR += 1;
		//we're having the pulse wave start with a 0 if it's starting

		if (inputSampleL > 0) {
			if (nikola->wasNegativeL) nikola->framenumberL = highsBoost;
			nikola->wasNegativeL = false;
		} else nikola->wasNegativeL = true;
		//our crossover trigger. If framenumber is 0, we've just gone into the positive cycle of the wave.
		if (inputSampleR > 0) {
			if (nikola->wasNegativeR) nikola->framenumberR = highsBoost;
			nikola->wasNegativeR = false;
		} else nikola->wasNegativeR = true;
		//our crossover trigger. If framenumber is 0, we've just gone into the positive cycle of the wave.

		if ((nikola->framenumberL > 0) && (nikola->framenumberL < 469)) {
			switch (nikola->framenumberL) {
				case 0:
					inputSampleL = 0.0;
					break;
				case 1:
					inputSampleL = 0.043397916539811392;
					break;
				case 2:
					inputSampleL = 0.083011537904293956;
					break;
				case 3:
					inputSampleL = 0.115117486724753554;
					break;
				case 4:
					inputSampleL = 0.129278313547471546;
					break;
				case 5:
					inputSampleL = 0.126867351298867753;
					break;
				case 6:
					inputSampleL = 0.115941486480605482;
					break;
				case 7:
					inputSampleL = 0.117162226859645377;
					break;
				case 8:
					inputSampleL = 0.152472738388012313;
					break;
				case 9:
					inputSampleL = 0.226817019600817904;
					break;
				case 10:
					inputSampleL = 0.362687450407727319;
					break;
				case 11:
					inputSampleL = 0.548425960188909545;
					break;
				case 12:
					inputSampleL = 0.761967189741508233;
					break;
				case 13:
					inputSampleL = 0.963667118404492373;
					break;
				case 14:
					inputSampleL = 0.970747531815851383;
					break;
				case 15:
					inputSampleL = 0.978133130321970246;
					break;
				case 16:
					inputSampleL = 0.98234468462965796;
					break;
				case 17:
					inputSampleL = 0.779637645153965941;
					break;
				case 18:
					inputSampleL = 0.420765008431043475;
					break;
				case 19:
					inputSampleL = -0.006744709806817921;
					break;
				case 20:
					inputSampleL = -0.427998133602099762;
					break;
				case 21:
					inputSampleL = -0.776677468947111538;
					break;
				case 22:
					inputSampleL = -0.970656095499740634;
					break;
				case 23:
					inputSampleL = -0.976790435117343758;
					break;
				case 24:
					inputSampleL = -0.982924655522019242;
					break;
				case 25:
					inputSampleL = -0.913249825471968868;
					break;
				case 26:
					inputSampleL = -0.707521453558153102;
					break;
				case 27:
					inputSampleL = -0.498252815332194066;
					break;
				case 28:
					inputSampleL = -0.319930422566606754;
					break;
				case 29:
					inputSampleL = -0.204965528389538354;
					break;
				case 30:
					inputSampleL = -0.152533894619281685;
					break;
				case 31:
					inputSampleL = -0.14597229586901464;
					break;
				case 32:
					inputSampleL = -0.169685535370647372;
					break;
				case 33:
					inputSampleL = -0.198434448148747306;
					break;
				case 34:
					inputSampleL = -0.212503719443037281;
					break;
				case 35:
					inputSampleL = -0.200692817849971089;
					break;
				case 36:
					inputSampleL = -0.16089608542863254;
					break;
				case 37:
					inputSampleL = -0.098973075998413124;
					break;
				case 38:
					inputSampleL = -0.028993060853602797;
					break;
				case 39:
					inputSampleL = 0.031831282235480818;
					break;
				case 40:
					inputSampleL = 0.083896574679097863;
					break;
				case 41:
					inputSampleL = 0.115025931196325565;
					break;
				case 42:
					inputSampleL = 0.11221822832453382;
					break;
				case 43:
					inputSampleL = 0.082187538148442027;
					break;
				case 44:
					inputSampleL = 0.023194305627918334;
					break;
				case 45:
					inputSampleL = -0.057436788536942743;
					break;
				case 46:
					inputSampleL = -0.148383258117618433;
					break;
				case 47:
					inputSampleL = -0.230754026535538892;
					break;
				case 48:
					inputSampleL = -0.291364740058290439;
					break;
				case 49:
					inputSampleL = -0.32316550378399006;
					break;
				case 50:
					inputSampleL = -0.312972202406079369;
					break;
				case 51:
					inputSampleL = -0.270215055352641453;
					break;
				case 52:
					inputSampleL = -0.196267633975951511;
					break;
				case 53:
					inputSampleL = -0.109166377376323828;
					break;
				case 54:
					inputSampleL = -0.026582098604999018;
					break;
				case 55:
					inputSampleL = 0.038179132206488234;
					break;
				case 56:
					inputSampleL = 0.078281168935514386;
					break;
				case 57:
					inputSampleL = 0.090061432806176941;
					break;
				case 58:
					inputSampleL = 0.082523241752678;
					break;
				case 59:
					inputSampleL = 0.0721773478273873;
					break;
				case 60:
					inputSampleL = 0.060427483253273108;
					break;
				case 61:
					inputSampleL = 0.064822267830744337;
					break;
				case 62:
					inputSampleL = 0.081088871807306132;
					break;
				case 63:
					inputSampleL = 0.107609814180730609;
					break;
				case 64:
					inputSampleL = 0.136328208449354532;
					break;
				case 65:
					inputSampleL = 0.15158770161320842;
					break;
				case 66:
					inputSampleL = 0.151129923971068447;
					break;
				case 67:
					inputSampleL = 0.128881572924283566;
					break;
				case 68:
					inputSampleL = 0.094791920987884151;
					break;
				case 69:
					inputSampleL = 0.059725557535325172;
					break;
				case 70:
					inputSampleL = 0.034059133427228612;
					break;
				case 71:
					inputSampleL = 0.030366274567705312;
					break;
				case 72:
					inputSampleL = 0.044954360523087246;
					break;
				case 73:
					inputSampleL = 0.07425260647175512;
					break;
				case 74:
					inputSampleL = 0.103733963477278965;
					break;
				case 75:
					inputSampleL = 0.112737042985625785;
					break;
				case 76:
					inputSampleL = 0.086490767197485274;
					break;
				case 77:
					inputSampleL = 0.019135343867610705;
					break;
				case 78:
					inputSampleL = -0.083164249663991302;
					break;
				case 79:
					inputSampleL = -0.19562674527695556;
					break;
				case 80:
					inputSampleL = -0.283765511985839491;
					break;
				case 81:
					inputSampleL = -0.322799281670278104;
					break;
				case 82:
					inputSampleL = -0.288312769897763055;
					break;
				case 83:
					inputSampleL = -0.171516645939207207;
					break;
				case 84:
					inputSampleL = 0.015076262894375438;
					break;
				case 85:
					inputSampleL = 0.243449845691091654;
					break;
				case 86:
					inputSampleL = 0.469503902554704478;
					break;
				case 87:
					inputSampleL = 0.648131361202124112;
					break;
				case 88:
					inputSampleL = 0.74451036389538261;
					break;
				case 89:
					inputSampleL = 0.733553980567644315;
					break;
				case 90:
					inputSampleL = 0.618070271729483922;
					break;
				case 91:
					inputSampleL = 0.409137337107760901;
					break;
				case 92:
					inputSampleL = 0.135046431051362659;
					break;
				case 93:
					inputSampleL = -0.165931758705099741;
					break;
				case 94:
					inputSampleL = -0.451131521423688564;
					break;
				case 95:
					inputSampleL = -0.68655487990936015;
					break;
				case 96:
					inputSampleL = -0.842262699514450652;
					break;
				case 97:
					inputSampleL = -0.900889709921262383;
					break;
				case 98:
					inputSampleL = -0.856484563356120554;
					break;
				case 99:
					inputSampleL = -0.716158310952787924;
					break;
				case 100:
					inputSampleL = -0.500877526360057446;
					break;
				case 101:
					inputSampleL = -0.240825253875545597;
					break;
				case 102:
					inputSampleL = 0.024720350314645832;
					break;
				case 103:
					inputSampleL = 0.264263707579577012;
					break;
				case 104:
					inputSampleL = 0.44478343302743617;
					break;
				case 105:
					inputSampleL = 0.540826732116458708;
					break;
				case 106:
					inputSampleL = 0.542444213118686513;
					break;
				case 107:
					inputSampleL = 0.451497624325083169;
					break;
				case 108:
					inputSampleL = 0.278241423345439054;
					break;
				case 109:
					inputSampleL = 0.048921885967894527;
					break;
				case 110:
					inputSampleL = -0.203348047387310493;
					break;
				case 111:
					inputSampleL = -0.445638189718009059;
					break;
				case 112:
					inputSampleL = -0.64224140808557395;
					break;
				case 113:
					inputSampleL = -0.765202270958281305;
					break;
				case 114:
					inputSampleL = -0.791509702978301477;
					break;
				case 115:
					inputSampleL = -0.713472682118900248;
					break;
				case 116:
					inputSampleL = -0.537011918431348656;
					break;
				case 117:
					inputSampleL = -0.286664770386059253;
					break;
				case 118:
					inputSampleL = 0.001312295907773064;
					break;
				case 119:
					inputSampleL = 0.277692090174871065;
					break;
				case 120:
					inputSampleL = 0.500450148015076146;
					break;
				case 121:
					inputSampleL = 0.634519986762901778;
					break;
				case 122:
					inputSampleL = 0.666290231979125402;
					break;
				case 123:
					inputSampleL = 0.602291963904538097;
					break;
				case 124:
					inputSampleL = 0.46706230258369702;
					break;
				case 125:
					inputSampleL = 0.301069626072267837;
					break;
				case 126:
					inputSampleL = 0.147223435545213166;
					break;
				case 127:
					inputSampleL = 0.037049947355876335;
					break;
				case 128:
					inputSampleL = -0.013611493651844931;
					break;
				case 129:
					inputSampleL = -0.00317404419812625;
					break;
				case 130:
					inputSampleL = 0.051943218406018246;
					break;
				case 131:
					inputSampleL = 0.123662788590960415;
					break;
				case 132:
					inputSampleL = 0.190804582354503011;
					break;
				case 133:
					inputSampleL = 0.234874025315408796;
					break;
				case 134:
					inputSampleL = 0.243846586314279606;
					break;
				case 135:
					inputSampleL = 0.221018383587450795;
					break;
				case 136:
					inputSampleL = 0.177162570192876984;
					break;
				case 137:
					inputSampleL = 0.126470610675679801;
					break;
				case 138:
					inputSampleL = 0.080417464598834185;
					break;
				case 139:
					inputSampleL = 0.042940138897671433;
					break;
				case 140:
					inputSampleL = 0.016022455901058993;
					break;
				case 141:
					inputSampleL = -0.009033717230445352;
					break;
				case 142:
					inputSampleL = -0.03940011101077312;
					break;
				case 143:
					inputSampleL = -0.075076725439924399;
					break;
				case 144:
					inputSampleL = -0.117406613360698353;
					break;
				case 145:
					inputSampleL = -0.168007017349467547;
					break;
				case 146:
					inputSampleL = -0.220163984534440221;
					break;
				case 147:
					inputSampleL = -0.262280123675954546;
					break;
				case 148:
					inputSampleL = -0.287580325670339143;
					break;
				case 149:
					inputSampleL = -0.293714665287942212;
					break;
				case 150:
					inputSampleL = -0.282361660549943605;
					break;
				case 151:
					inputSampleL = -0.254314673489791643;
					break;
				case 152:
					inputSampleL = -0.219767243911252269;
					break;
				case 153:
					inputSampleL = -0.186287842951445126;
					break;
				case 154:
					inputSampleL = -0.157478012367320869;
					break;
				case 155:
					inputSampleL = -0.1383120307776117;
					break;
				case 156:
					inputSampleL = -0.129919321458784848;
					break;
				case 157:
					inputSampleL = -0.131048506309396739;
					break;
				case 158:
					inputSampleL = -0.136084179585863912;
					break;
				case 159:
					inputSampleL = -0.142035408146611003;
					break;
				case 160:
					inputSampleL = -0.148047554513382446;
					break;
				case 161:
					inputSampleL = -0.147711850909146486;
					break;
				case 162:
					inputSampleL = -0.140387289421979505;
					break;
				case 163:
					inputSampleL = -0.126775914982757115;
					break;
				case 164:
					inputSampleL = -0.108525488677327892;
					break;
				case 165:
					inputSampleL = -0.087436960203558539;
					break;
				case 166:
					inputSampleL = -0.065585349779961638;
					break;
				case 167:
					inputSampleL = -0.045076553773308596;
					break;
				case 168:
					inputSampleL = -0.026612617114475014;
					break;
				case 169:
					inputSampleL = -0.012787493895993006;
					break;
				case 170:
					inputSampleL = -0.005066191785638076;
					break;
				case 171:
					inputSampleL = -0.001739674252754382;
					break;
				case 172:
					inputSampleL = -0.001709155743278385;
					break;
				case 173:
					inputSampleL = -0.002258488913846334;
					break;
				case 174:
					inputSampleL = -0.001281896610614424;
					break;
				case 175:
					inputSampleL = 0.005188146611224708;
					break;
				case 176:
					inputSampleL = 0.018250307092806787;
					break;
				case 177:
					inputSampleL = 0.038148613697012237;
					break;
				case 178:
					inputSampleL = 0.065676786096072262;
					break;
				case 179:
					inputSampleL = 0.097691179388103885;
					break;
				case 180:
					inputSampleL = 0.128820535905331574;
					break;
				case 181:
					inputSampleL = 0.156012885487227998;
					break;
				case 182:
					inputSampleL = 0.177498273797112943;
					break;
				case 183:
					inputSampleL = 0.189003990295419172;
					break;
				case 184:
					inputSampleL = 0.18885139774803919;
					break;
				case 185:
					inputSampleL = 0.17816968100558489;
					break;
				case 186:
					inputSampleL = 0.156806485946531571;
					break;
				case 187:
					inputSampleL = 0.127172536393627744;
					break;
				case 188:
					inputSampleL = 0.09524969863002411;
					break;
				case 189:
					inputSampleL = 0.066470267342448194;
					break;
				case 190:
					inputSampleL = 0.043977768219855336;
					break;
				case 191:
					inputSampleL = 0.032014393292336796;
					break;
				case 192:
					inputSampleL = 0.033570837275612657;
					break;
				case 193:
					inputSampleL = 0.04724336794671468;
					break;
				case 194:
					inputSampleL = 0.070163006989043847;
					break;
				case 195:
					inputSampleL = 0.096043179876400028;
					break;
				case 196:
					inputSampleL = 0.117650642224188973;
					break;
				case 197:
					inputSampleL = 0.127172536393627744;
					break;
				case 198:
					inputSampleL = 0.120427826586504708;
					break;
				case 199:
					inputSampleL = 0.09720288323648793;
					break;
				case 200:
					inputSampleL = 0.059634002006897183;
					break;
				case 201:
					inputSampleL = 0.0144048556859035;
					break;
				case 202:
					inputSampleL = -0.029817060609454722;
					break;
				case 203:
					inputSampleL = -0.064639275986205724;
					break;
				case 204:
					inputSampleL = -0.083866175381939231;
					break;
				case 205:
					inputSampleL = -0.085514174893643075;
					break;
				case 206:
					inputSampleL = -0.071841763435468692;
					break;
				case 207:
					inputSampleL = -0.050936226805627698;
					break;
				case 208:
					inputSampleL = -0.028443727683034846;
					break;
				case 209:
					inputSampleL = -0.010468087175817219;
					break;
				case 210:
					inputSampleL = -0.002105896366466348;
					break;
				case 211:
					inputSampleL = -0.003662340349742205;
					break;
				case 212:
					inputSampleL = -0.01220764221594906;
					break;
				case 213:
					inputSampleL = -0.022248351046479778;
					break;
				case 214:
					inputSampleL = -0.029847579118930719;
					break;
				case 215:
					inputSampleL = -0.031343105296182221;
					break;
				case 216:
					inputSampleL = -0.024750988036439187;
					break;
				case 217:
					inputSampleL = -0.013550456632892937;
					break;
				case 218:
					inputSampleL = -0.000854637477950463;
					break;
				case 219:
					inputSampleL = 0.010071227340311899;
					break;
				case 220:
					inputSampleL = 0.016663344600054933;
					break;
				case 221:
					inputSampleL = 0.016846455656910918;
					break;
				case 222:
					inputSampleL = 0.01190233790887173;
					break;
				case 223:
					inputSampleL = 0.006378368480788599;
					break;
				case 224:
					inputSampleL = 0.004242072817468795;
					break;
				case 225:
					inputSampleL = 0.007934931676992095;
					break;
				case 226:
					inputSampleL = 0.01995934362346263;
					break;
				case 227:
					inputSampleL = 0.041017472800683609;
					break;
				case 228:
					inputSampleL = 0.067507896664632097;
					break;
				case 229:
					inputSampleL = 0.095280217139500106;
					break;
				case 230:
					inputSampleL = 0.119237604716940823;
					break;
				case 231:
					inputSampleL = 0.134863319994506653;
					break;
				case 232:
					inputSampleL = 0.139074874302194285;
					break;
				case 233:
					inputSampleL = 0.130285424360179453;
					break;
				case 234:
					inputSampleL = 0.109227295182958456;
					break;
				case 235:
					inputSampleL = 0.077731716552018798;
					break;
				case 236:
					inputSampleL = 0.038667428358104189;
					break;
				case 237:
					inputSampleL = -0.002929896122318273;
					break;
				case 238:
					inputSampleL = -0.042360406429944847;
					break;
				case 239:
					inputSampleL = -0.075229317987304381;
					break;
				case 240:
					inputSampleL = -0.097386113505661273;
					break;
				case 241:
					inputSampleL = -0.105595711767632161;
					break;
				case 242:
					inputSampleL = -0.098606853884701154;
					break;
				case 243:
					inputSampleL = -0.07764028023590816;
					break;
				case 244:
					inputSampleL = -0.046632997756584457;
					break;
				case 245:
					inputSampleL = -0.011505716498001124;
					break;
				case 246:
					inputSampleL = 0.021668380154118472;
					break;
				case 247:
					inputSampleL = 0.048555663854182557;
					break;
				case 248:
					inputSampleL = 0.061800816179692977;
					break;
				case 249:
					inputSampleL = 0.060244372196417123;
					break;
				case 250:
					inputSampleL = 0.046663397053743089;
					break;
				case 251:
					inputSampleL = 0.024995016899929806;
					break;
				case 252:
					inputSampleL = 0.000061037019257179;
					break;
				case 253:
					inputSampleL = -0.022461980612811756;
					break;
				case 254:
					inputSampleL = -0.036927992530289708;
					break;
				case 255:
					inputSampleL = -0.038942214155705521;
					break;
				case 256:
					inputSampleL = -0.026917802209234988;
					break;
				case 257:
					inputSampleL = -0.002624711027558301;
					break;
				case 258:
					inputSampleL = 0.029603311830805383;
					break;
				case 259:
					inputSampleL = 0.064456045717032381;
					break;
				case 260:
					inputSampleL = 0.096531476028015983;
					break;
				case 261:
					inputSampleL = 0.120824567209692674;
					break;
				case 262:
					inputSampleL = 0.132910135388042833;
					break;
				case 263:
					inputSampleL = 0.129858165227515476;
					break;
				case 264:
					inputSampleL = 0.111302673040253916;
					break;
				case 265:
					inputSampleL = 0.080173316523026214;
					break;
				case 266:
					inputSampleL = 0.042482361255531474;
					break;
				case 267:
					inputSampleL = 0.006225775933408613;
					break;
				case 268:
					inputSampleL = -0.020295166440015958;
					break;
				case 269:
					inputSampleL = -0.030457949308450662;
					break;
				case 270:
					inputSampleL = -0.020844499610583905;
					break;
				case 271:
					inputSampleL = 0.007202368236640523;
					break;
				case 272:
					inputSampleL = 0.048006330683614609;
					break;
				case 273:
					inputSampleL = 0.092655506111636698;
					break;
				case 274:
					inputSampleL = 0.13120097964445937;
					break;
				case 275:
					inputSampleL = 0.154975256165044095;
					break;
				case 276:
					inputSampleL = 0.15885122608142338;
					break;
				case 277:
					inputSampleL = 0.142798251671193588;
					break;
				case 278:
					inputSampleL = 0.111333191549729912;
					break;
				case 279:
					inputSampleL = 0.071811125713675344;
					break;
				case 280:
					inputSampleL = 0.032472170934476755;
					break;
				case 281:
					inputSampleL = 0.000732444227729118;
					break;
				case 282:
					inputSampleL = -0.018616648418836111;
					break;
				case 283:
					inputSampleL = -0.024903580583819172;
					break;
				case 284:
					inputSampleL = -0.020722425572679917;
					break;
				case 285:
					inputSampleL = -0.010681716742149201;
					break;
				case 286:
					inputSampleL = 0.000030518509781182;
					break;
				case 287:
					inputSampleL = 0.006958220160832545;
					break;
				case 288:
					inputSampleL = 0.007538071840876492;
					break;
				case 289:
					inputSampleL = 0.001831110568865017;
					break;
				case 290:
					inputSampleL = -0.008179198965117431;
					break;
				case 291:
					inputSampleL = -0.019501685193640029;
					break;
				case 292:
					inputSampleL = -0.028443727683034846;
					break;
				case 293:
					inputSampleL = -0.031678808900418187;
					break;
				case 294:
					inputSampleL = -0.027467135379802936;
					break;
				case 295:
					inputSampleL = -0.016083612132328343;
					break;
				case 296:
					inputSampleL = 0.000518814661397137;
					break;
				case 297:
					inputSampleL = 0.019593121509750663;
					break;
				case 298:
					inputSampleL = 0.037782391583300268;
					break;
				case 299:
					inputSampleL = 0.052126329462874231;
					break;
				case 300:
					inputSampleL = 0.060946297914365059;
					break;
				case 301:
					inputSampleL = 0.063601527451704457;
					break;
				case 302:
					inputSampleL = 0.059939187101657153;
					break;
				case 303:
					inputSampleL = 0.050386774422742392;
					break;
				case 304:
					inputSampleL = 0.036714243751640369;
					break;
				case 305:
					inputSampleL = 0.021729417173070469;
					break;
				case 306:
					inputSampleL = 0.008453746338084048;
					break;
				case 307:
					inputSampleL = -0.000579970892666489;
					break;
				case 308:
					inputSampleL = -0.003814932897122191;
					break;
				case 309:
					inputSampleL = -0.000793600458998469;
					break;
				case 310:
					inputSampleL = 0.007507553331400495;
					break;
				case 311:
					inputSampleL = 0.019654158528702659;
					break;
				case 312:
					inputSampleL = 0.033692911313516642;
					break;
				case 313:
					inputSampleL = 0.047579071550950647;
					break;
				case 314:
					inputSampleL = 0.059542446478469187;
					break;
				case 315:
					inputSampleL = 0.068881229591051973;
					break;
				case 316:
					inputSampleL = 0.075961643002410956;
					break;
				case 317:
					inputSampleL = 0.081821316034730057;
					break;
				case 318:
					inputSampleL = 0.087864100123905151;
					break;
				case 319:
					inputSampleL = 0.094944513535264133;
					break;
				case 320:
					inputSampleL = 0.10254374160771508;
					break;
				case 321:
					inputSampleL = 0.108342258408154549;
					break;
				case 322:
					inputSampleL = 0.109257813692434452;
					break;
				case 323:
					inputSampleL = 0.102574260117191077;
					break;
				case 324:
					inputSampleL = 0.086032989555345316;
					break;
				case 325:
					inputSampleL = 0.058748965232093261;
					break;
				case 326:
					inputSampleL = 0.02255341692892239;
					break;
				case 327:
					inputSampleL = -0.017701093134556194;
					break;
				case 328:
					inputSampleL = -0.0557277520062869;
					break;
				case 329:
					inputSampleL = -0.085025878742027119;
					break;
				case 330:
					inputSampleL = -0.100071742339549019;
					break;
				case 331:
					inputSampleL = -0.09766078009094524;
					break;
				case 332:
					inputSampleL = -0.07873906578997171;
					break;
				case 333:
					inputSampleL = -0.048220079462263948;
					break;
				case 334:
					inputSampleL = -0.013458901104464944;
					break;
				case 335:
					inputSampleL = 0.017304233299050873;
					break;
				case 336:
					inputSampleL = 0.036439577166356395;
					break;
				case 337:
					inputSampleL = 0.039033650471816152;
					break;
				case 338:
					inputSampleL = 0.024567757767265847;
					break;
				case 339:
					inputSampleL = -0.003753895878170197;
					break;
				case 340:
					inputSampleL = -0.040620851389813008;
					break;
				case 341:
					inputSampleL = -0.078555954733115718;
					break;
				case 342:
					inputSampleL = -0.109135858866847832;
					break;
				case 343:
					inputSampleL = -0.12497520371074565;
					break;
				case 344:
					inputSampleL = -0.121618167668385957;
					break;
				case 345:
					inputSampleL = -0.097599743071993247;
					break;
				case 346:
					inputSampleL = -0.054140670300607402;
					break;
				case 347:
					inputSampleL = 0.004272591326944792;
					break;
				case 348:
					inputSampleL = 0.069857821894283884;
					break;
				case 349:
					inputSampleL = 0.133306876011230813;
					break;
				case 350:
					inputSampleL = 0.185463723983275847;
					break;
				case 351:
					inputSampleL = 0.218088487464827424;
					break;
				case 352:
					inputSampleL = 0.225413048951994377;
					break;
				case 353:
					inputSampleL = 0.206582890179448836;
					break;
				case 354:
					inputSampleL = 0.166389417134922335;
					break;
				case 355:
					inputSampleL = 0.113957783364665666;
					break;
				case 356:
					inputSampleL = 0.05987815008270516;
					break;
				case 357:
					inputSampleL = 0.014435374195379498;
					break;
				case 358:
					inputSampleL = -0.01416082682241288;
					break;
				case 359:
					inputSampleL = -0.021790573404339819;
					break;
				case 360:
					inputSampleL = -0.00991875400524927;
					break;
				case 361:
					inputSampleL = 0.0144048556859035;
					break;
				case 362:
					inputSampleL = 0.040742806215399635;
					break;
				case 363:
					inputSampleL = 0.059176224364757224;
					break;
				case 364:
					inputSampleL = 0.063784638508560434;
					break;
				case 365:
					inputSampleL = 0.053255514313486123;
					break;
				case 366:
					inputSampleL = 0.030244200529801323;
					break;
				case 367:
					inputSampleL = 0.000610370189825129;
					break;
				case 368:
					inputSampleL = -0.017395908039796223;
					break;
				case 369:
					inputSampleL = -0.039918925671865071;
					break;
				case 370:
					inputSampleL = -0.052797855883663529;
					break;
				case 371:
					inputSampleL = -0.055544640949430915;
					break;
				case 372:
					inputSampleL = -0.043153887676320772;
					break;
				case 373:
					inputSampleL = -0.021210721724295872;
					break;
				case 374:
					inputSampleL = 0.004119998779564807;
					break;
				case 375:
					inputSampleL = 0.029237089717093417;
					break;
				case 376:
					inputSampleL = 0.048982922986846519;
					break;
				case 377:
					inputSampleL = 0.061739779160740985;
					break;
				case 378:
					inputSampleL = 0.067141674550920127;
					break;
				case 379:
					inputSampleL = 0.065615749077120269;
					break;
				case 380:
					inputSampleL = 0.060305409215369116;
					break;
				case 381:
					inputSampleL = 0.052034773934446235;
					break;
				case 382:
					inputSampleL = 0.047731664098330635;
					break;
				case 383:
					inputSampleL = 0.042390805727103485;
					break;
				case 384:
					inputSampleL = 0.035096762749412515;
					break;
				case 385:
					inputSampleL = 0.025819016655781731;
					break;
				case 386:
					inputSampleL = 0.01278737468367565;
					break;
				case 387:
					inputSampleL = -0.004303229048738146;
					break;
				case 388:
					inputSampleL = -0.022461980612811756;
					break;
				case 389:
					inputSampleL = -0.041200703069856952;
					break;
				case 390:
					inputSampleL = -0.058138714254890679;
					break;
				case 391:
					inputSampleL = -0.073215096361888568;
					break;
				case 392:
					inputSampleL = -0.077213021103244198;
					break;
				case 393:
					inputSampleL = -0.073001466795556594;
					break;
				case 394:
					inputSampleL = -0.063937350268257781;
					break;
				case 395:
					inputSampleL = -0.054171188810083405;
					break;
				case 396:
					inputSampleL = -0.040773443937192996;
					break;
				case 397:
					inputSampleL = -0.031739845919370187;
					break;
				case 398:
					inputSampleL = -0.023682840204779284;
					break;
				case 399:
					inputSampleL = -0.020508796006347935;
					break;
				case 400:
					inputSampleL = -0.025910691396527082;
					break;
				case 401:
					inputSampleL = -0.04406944296060069;
					break;
				case 402:
					inputSampleL = -0.065646386798913631;
					break;
				case 403:
					inputSampleL = -0.091160218359630202;
					break;
				case 404:
					inputSampleL = -0.117101309053010735;
					break;
				case 405:
					inputSampleL = -0.140478844950407522;
					break;
				case 406:
					inputSampleL = -0.152778042695089655;
					break;
				case 407:
					inputSampleL = -0.149024266029541996;
					break;
				case 408:
					inputSampleL = -0.126867470511185132;
					break;
				case 409:
					inputSampleL = -0.084934323213599131;
					break;
				case 410:
					inputSampleL = -0.027711283455610913;
					break;
				case 411:
					inputSampleL = 0.028779312074953458;
					break;
				case 412:
					inputSampleL = 0.082279093676870016;
					break;
				case 413:
					inputSampleL = 0.120580419133884703;
					break;
				case 414:
					inputSampleL = 0.137945689451582393;
					break;
				case 415:
					inputSampleL = 0.128332239753715632;
					break;
				case 416:
					inputSampleL = 0.098209994049195837;
					break;
				case 417:
					inputSampleL = 0.057345113796197389;
					break;
				case 418:
					inputSampleL = 0.014282781647999511;
					break;
				case 419:
					inputSampleL = -0.019440648174688036;
					break;
				case 420:
					inputSampleL = -0.040712406918240997;
					break;
				case 421:
					inputSampleL = -0.052675781845759537;
					break;
				case 422:
					inputSampleL = -0.049166153256019862;
					break;
				case 423:
					inputSampleL = -0.03909492591601315;
					break;
				case 424:
					inputSampleL = -0.02404906231849125;
					break;
				case 425:
					inputSampleL = -0.010071346552629257;
					break;
				case 426:
					inputSampleL = -0.005920710050965997;
					break;
				case 427:
					inputSampleL = -0.004547377124546123;
					break;
				case 428:
					inputSampleL = -0.014008234275032894;
					break;
				case 429:
					inputSampleL = -0.021515906819055845;
					break;
				case 430:
					inputSampleL = -0.027009357737662981;
					break;
				case 431:
					inputSampleL = -0.037538362719809648;
					break;
				case 432:
					inputSampleL = -0.043520109790032742;
					break;
				case 433:
					inputSampleL = -0.04736556119693603;
					break;
				case 434:
					inputSampleL = -0.048159042443311956;
					break;
				case 435:
					inputSampleL = -0.045076553773308596;
					break;
				case 436:
					inputSampleL = -0.039216999953917135;
					break;
				case 437:
					inputSampleL = -0.030946245460066617;
					break;
				case 438:
					inputSampleL = -0.026490543076571026;
					break;
				case 439:
					inputSampleL = -0.020234129421063961;
					break;
				case 440:
					inputSampleL = -0.012085568178045071;
					break;
				case 441:
					inputSampleL = -0.007660265091097837;
					break;
				case 442:
					inputSampleL = -0.005615524956206025;
					break;
				case 443:
					inputSampleL = -0.005218784333018062;
					break;
				case 444:
					inputSampleL = -0.008698013626209382;
					break;
				case 445:
					inputSampleL = -0.015931019584948358;
					break;
				case 446:
					inputSampleL = -0.026521061586047025;
					break;
				case 447:
					inputSampleL = -0.036165029793389779;
					break;
				case 448:
					inputSampleL = -0.043733739356364723;
					break;
				case 449:
					inputSampleL = -0.04843370902859593;
					break;
				case 450:
					inputSampleL = -0.048616820085451914;
					break;
				case 451:
					inputSampleL = -0.043031813638416787;
					break;
				case 452:
					inputSampleL = -0.032167105052034142;
					break;
				case 453:
					inputSampleL = -0.019288055627308048;
					break;
				case 454:
					inputSampleL = -0.00415063650135816;
					break;
				case 455:
					inputSampleL = 0.010467967963499863;
					break;
				case 456:
					inputSampleL = 0.020539195303506577;
					break;
				case 457:
					inputSampleL = 0.025696942617877742;
					break;
				case 458:
					inputSampleL = 0.027375460639057589;
					break;
				case 459:
					inputSampleL = 0.023621683973509933;
					break;
				case 460:
					inputSampleL = 0.016358159505294963;
					break;
				case 461:
					inputSampleL = 0.008911523980224005;
					break;
				case 462:
					inputSampleL = 0.002319406720480971;
					break;
				case 463:
					inputSampleL = -0.002716266555986292;
					break;
				case 464:
					inputSampleL = -0.00589019154149;
					break;
				case 465:
					inputSampleL = -0.006683672787865927;
					break;
				case 466:
					inputSampleL = -0.005279821351970056;
					break;
				case 467:
					inputSampleL = -0.003387673764458231;
					break;
				case 468:
					inputSampleL = -0.001281896610614424;
					break;
			}
		}

		if ((nikola->framenumberR > 0) && (nikola->framenumberR < 469)) {
			switch (nikola->framenumberR) {
				case 0:
					inputSampleR = 0.0;
					break;
				case 1:
					inputSampleR = 0.043397916539811392;
					break;
				case 2:
					inputSampleR = 0.083011537904293956;
					break;
				case 3:
					inputSampleR = 0.115117486724753554;
					break;
				case 4:
					inputSampleR = 0.129278313547471546;
					break;
				case 5:
					inputSampleR = 0.126867351298867753;
					break;
				case 6:
					inputSampleR = 0.115941486480605482;
					break;
				case 7:
					inputSampleR = 0.117162226859645377;
					break;
				case 8:
					inputSampleR = 0.152472738388012313;
					break;
				case 9:
					inputSampleR = 0.226817019600817904;
					break;
				case 10:
					inputSampleR = 0.362687450407727319;
					break;
				case 11:
					inputSampleR = 0.548425960188909545;
					break;
				case 12:
					inputSampleR = 0.761967189741508233;
					break;
				case 13:
					inputSampleR = 0.963667118404492373;
					break;
				case 14:
					inputSampleR = 0.970747531815851383;
					break;
				case 15:
					inputSampleR = 0.978133130321970246;
					break;
				case 16:
					inputSampleR = 0.98234468462965796;
					break;
				case 17:
					inputSampleR = 0.779637645153965941;
					break;
				case 18:
					inputSampleR = 0.420765008431043475;
					break;
				case 19:
					inputSampleR = -0.006744709806817921;
					break;
				case 20:
					inputSampleR = -0.427998133602099762;
					break;
				case 21:
					inputSampleR = -0.776677468947111538;
					break;
				case 22:
					inputSampleR = -0.970656095499740634;
					break;
				case 23:
					inputSampleR = -0.976790435117343758;
					break;
				case 24:
					inputSampleR = -0.982924655522019242;
					break;
				case 25:
					inputSampleR = -0.913249825471968868;
					break;
				case 26:
					inputSampleR = -0.707521453558153102;
					break;
				case 27:
					inputSampleR = -0.498252815332194066;
					break;
				case 28:
					inputSampleR = -0.319930422566606754;
					break;
				case 29:
					inputSampleR = -0.204965528389538354;
					break;
				case 30:
					inputSampleR = -0.152533894619281685;
					break;
				case 31:
					inputSampleR = -0.14597229586901464;
					break;
				case 32:
					inputSampleR = -0.169685535370647372;
					break;
				case 33:
					inputSampleR = -0.198434448148747306;
					break;
				case 34:
					inputSampleR = -0.212503719443037281;
					break;
				case 35:
					inputSampleR = -0.200692817849971089;
					break;
				case 36:
					inputSampleR = -0.16089608542863254;
					break;
				case 37:
					inputSampleR = -0.098973075998413124;
					break;
				case 38:
					inputSampleR = -0.028993060853602797;
					break;
				case 39:
					inputSampleR = 0.031831282235480818;
					break;
				case 40:
					inputSampleR = 0.083896574679097863;
					break;
				case 41:
					inputSampleR = 0.115025931196325565;
					break;
				case 42:
					inputSampleR = 0.11221822832453382;
					break;
				case 43:
					inputSampleR = 0.082187538148442027;
					break;
				case 44:
					inputSampleR = 0.023194305627918334;
					break;
				case 45:
					inputSampleR = -0.057436788536942743;
					break;
				case 46:
					inputSampleR = -0.148383258117618433;
					break;
				case 47:
					inputSampleR = -0.230754026535538892;
					break;
				case 48:
					inputSampleR = -0.291364740058290439;
					break;
				case 49:
					inputSampleR = -0.32316550378399006;
					break;
				case 50:
					inputSampleR = -0.312972202406079369;
					break;
				case 51:
					inputSampleR = -0.270215055352641453;
					break;
				case 52:
					inputSampleR = -0.196267633975951511;
					break;
				case 53:
					inputSampleR = -0.109166377376323828;
					break;
				case 54:
					inputSampleR = -0.026582098604999018;
					break;
				case 55:
					inputSampleR = 0.038179132206488234;
					break;
				case 56:
					inputSampleR = 0.078281168935514386;
					break;
				case 57:
					inputSampleR = 0.090061432806176941;
					break;
				case 58:
					inputSampleR = 0.082523241752678;
					break;
				case 59:
					inputSampleR = 0.0721773478273873;
					break;
				case 60:
					inputSampleR = 0.060427483253273108;
					break;
				case 61:
					inputSampleR = 0.064822267830744337;
					break;
				case 62:
					inputSampleR = 0.081088871807306132;
					break;
				case 63:
					inputSampleR = 0.107609814180730609;
					break;
				case 64:
					inputSampleR = 0.136328208449354532;
					break;
				case 65:
					inputSampleR = 0.15158770161320842;
					break;
				case 66:
					inputSampleR = 0.151129923971068447;
					break;
				case 67:
					inputSampleR = 0.128881572924283566;
					break;
				case 68:
					inputSampleR = 0.094791920987884151;
					break;
				case 69:
					inputSampleR = 0.059725557535325172;
					break;
				case 70:
					inputSampleR = 0.034059133427228612;
					break;
				case 71:
					inputSampleR = 0.030366274567705312;
					break;
				case 72:
					inputSampleR = 0.044954360523087246;
					break;
				case 73:
					inputSampleR = 0.07425260647175512;
					break;
				case 74:
					inputSampleR = 0.103733963477278965;
					break;
				case 75:
					inputSampleR = 0.112737042985625785;
					break;
				case 76:
					inputSampleR = 0.086490767197485274;
					break;
				case 77:
					inputSampleR = 0.019135343867610705;
					break;
				case 78:
					inputSampleR = -0.083164249663991302;
					break;
				case 79:
					inputSampleR = -0.19562674527695556;
					break;
				case 80:
					inputSampleR = -0.283765511985839491;
					break;
				case 81:
					inputSampleR = -0.322799281670278104;
					break;
				case 82:
					inputSampleR = -0.288312769897763055;
					break;
				case 83:
					inputSampleR = -0.171516645939207207;
					break;
				case 84:
					inputSampleR = 0.015076262894375438;
					break;
				case 85:
					inputSampleR = 0.243449845691091654;
					break;
				case 86:
					inputSampleR = 0.469503902554704478;
					break;
				case 87:
					inputSampleR = 0.648131361202124112;
					break;
				case 88:
					inputSampleR = 0.74451036389538261;
					break;
				case 89:
					inputSampleR = 0.733553980567644315;
					break;
				case 90:
					inputSampleR = 0.618070271729483922;
					break;
				case 91:
					inputSampleR = 0.409137337107760901;
					break;
				case 92:
					inputSampleR = 0.135046431051362659;
					break;
				case 93:
					inputSampleR = -0.165931758705099741;
					break;
				case 94:
					inputSampleR = -0.451131521423688564;
					break;
				case 95:
					inputSampleR = -0.68655487990936015;
					break;
				case 96:
					inputSampleR = -0.842262699514450652;
					break;
				case 97:
					inputSampleR = -0.900889709921262383;
					break;
				case 98:
					inputSampleR = -0.856484563356120554;
					break;
				case 99:
					inputSampleR = -0.716158310952787924;
					break;
				case 100:
					inputSampleR = -0.500877526360057446;
					break;
				case 101:
					inputSampleR = -0.240825253875545597;
					break;
				case 102:
					inputSampleR = 0.024720350314645832;
					break;
				case 103:
					inputSampleR = 0.264263707579577012;
					break;
				case 104:
					inputSampleR = 0.44478343302743617;
					break;
				case 105:
					inputSampleR = 0.540826732116458708;
					break;
				case 106:
					inputSampleR = 0.542444213118686513;
					break;
				case 107:
					inputSampleR = 0.451497624325083169;
					break;
				case 108:
					inputSampleR = 0.278241423345439054;
					break;
				case 109:
					inputSampleR = 0.048921885967894527;
					break;
				case 110:
					inputSampleR = -0.203348047387310493;
					break;
				case 111:
					inputSampleR = -0.445638189718009059;
					break;
				case 112:
					inputSampleR = -0.64224140808557395;
					break;
				case 113:
					inputSampleR = -0.765202270958281305;
					break;
				case 114:
					inputSampleR = -0.791509702978301477;
					break;
				case 115:
					inputSampleR = -0.713472682118900248;
					break;
				case 116:
					inputSampleR = -0.537011918431348656;
					break;
				case 117:
					inputSampleR = -0.286664770386059253;
					break;
				case 118:
					inputSampleR = 0.001312295907773064;
					break;
				case 119:
					inputSampleR = 0.277692090174871065;
					break;
				case 120:
					inputSampleR = 0.500450148015076146;
					break;
				case 121:
					inputSampleR = 0.634519986762901778;
					break;
				case 122:
					inputSampleR = 0.666290231979125402;
					break;
				case 123:
					inputSampleR = 0.602291963904538097;
					break;
				case 124:
					inputSampleR = 0.46706230258369702;
					break;
				case 125:
					inputSampleR = 0.301069626072267837;
					break;
				case 126:
					inputSampleR = 0.147223435545213166;
					break;
				case 127:
					inputSampleR = 0.037049947355876335;
					break;
				case 128:
					inputSampleR = -0.013611493651844931;
					break;
				case 129:
					inputSampleR = -0.00317404419812625;
					break;
				case 130:
					inputSampleR = 0.051943218406018246;
					break;
				case 131:
					inputSampleR = 0.123662788590960415;
					break;
				case 132:
					inputSampleR = 0.190804582354503011;
					break;
				case 133:
					inputSampleR = 0.234874025315408796;
					break;
				case 134:
					inputSampleR = 0.243846586314279606;
					break;
				case 135:
					inputSampleR = 0.221018383587450795;
					break;
				case 136:
					inputSampleR = 0.177162570192876984;
					break;
				case 137:
					inputSampleR = 0.126470610675679801;
					break;
				case 138:
					inputSampleR = 0.080417464598834185;
					break;
				case 139:
					inputSampleR = 0.042940138897671433;
					break;
				case 140:
					inputSampleR = 0.016022455901058993;
					break;
				case 141:
					inputSampleR = -0.009033717230445352;
					break;
				case 142:
					inputSampleR = -0.03940011101077312;
					break;
				case 143:
					inputSampleR = -0.075076725439924399;
					break;
				case 144:
					inputSampleR = -0.117406613360698353;
					break;
				case 145:
					inputSampleR = -0.168007017349467547;
					break;
				case 146:
					inputSampleR = -0.220163984534440221;
					break;
				case 147:
					inputSampleR = -0.262280123675954546;
					break;
				case 148:
					inputSampleR = -0.287580325670339143;
					break;
				case 149:
					inputSampleR = -0.293714665287942212;
					break;
				case 150:
					inputSampleR = -0.282361660549943605;
					break;
				case 151:
					inputSampleR = -0.254314673489791643;
					break;
				case 152:
					inputSampleR = -0.219767243911252269;
					break;
				case 153:
					inputSampleR = -0.186287842951445126;
					break;
				case 154:
					inputSampleR = -0.157478012367320869;
					break;
				case 155:
					inputSampleR = -0.1383120307776117;
					break;
				case 156:
					inputSampleR = -0.129919321458784848;
					break;
				case 157:
					inputSampleR = -0.131048506309396739;
					break;
				case 158:
					inputSampleR = -0.136084179585863912;
					break;
				case 159:
					inputSampleR = -0.142035408146611003;
					break;
				case 160:
					inputSampleR = -0.148047554513382446;
					break;
				case 161:
					inputSampleR = -0.147711850909146486;
					break;
				case 162:
					inputSampleR = -0.140387289421979505;
					break;
				case 163:
					inputSampleR = -0.126775914982757115;
					break;
				case 164:
					inputSampleR = -0.108525488677327892;
					break;
				case 165:
					inputSampleR = -0.087436960203558539;
					break;
				case 166:
					inputSampleR = -0.065585349779961638;
					break;
				case 167:
					inputSampleR = -0.045076553773308596;
					break;
				case 168:
					inputSampleR = -0.026612617114475014;
					break;
				case 169:
					inputSampleR = -0.012787493895993006;
					break;
				case 170:
					inputSampleR = -0.005066191785638076;
					break;
				case 171:
					inputSampleR = -0.001739674252754382;
					break;
				case 172:
					inputSampleR = -0.001709155743278385;
					break;
				case 173:
					inputSampleR = -0.002258488913846334;
					break;
				case 174:
					inputSampleR = -0.001281896610614424;
					break;
				case 175:
					inputSampleR = 0.005188146611224708;
					break;
				case 176:
					inputSampleR = 0.018250307092806787;
					break;
				case 177:
					inputSampleR = 0.038148613697012237;
					break;
				case 178:
					inputSampleR = 0.065676786096072262;
					break;
				case 179:
					inputSampleR = 0.097691179388103885;
					break;
				case 180:
					inputSampleR = 0.128820535905331574;
					break;
				case 181:
					inputSampleR = 0.156012885487227998;
					break;
				case 182:
					inputSampleR = 0.177498273797112943;
					break;
				case 183:
					inputSampleR = 0.189003990295419172;
					break;
				case 184:
					inputSampleR = 0.18885139774803919;
					break;
				case 185:
					inputSampleR = 0.17816968100558489;
					break;
				case 186:
					inputSampleR = 0.156806485946531571;
					break;
				case 187:
					inputSampleR = 0.127172536393627744;
					break;
				case 188:
					inputSampleR = 0.09524969863002411;
					break;
				case 189:
					inputSampleR = 0.066470267342448194;
					break;
				case 190:
					inputSampleR = 0.043977768219855336;
					break;
				case 191:
					inputSampleR = 0.032014393292336796;
					break;
				case 192:
					inputSampleR = 0.033570837275612657;
					break;
				case 193:
					inputSampleR = 0.04724336794671468;
					break;
				case 194:
					inputSampleR = 0.070163006989043847;
					break;
				case 195:
					inputSampleR = 0.096043179876400028;
					break;
				case 196:
					inputSampleR = 0.117650642224188973;
					break;
				case 197:
					inputSampleR = 0.127172536393627744;
					break;
				case 198:
					inputSampleR = 0.120427826586504708;
					break;
				case 199:
					inputSampleR = 0.09720288323648793;
					break;
				case 200:
					inputSampleR = 0.059634002006897183;
					break;
				case 201:
					inputSampleR = 0.0144048556859035;
					break;
				case 202:
					inputSampleR = -0.029817060609454722;
					break;
				case 203:
					inputSampleR = -0.064639275986205724;
					break;
				case 204:
					inputSampleR = -0.083866175381939231;
					break;
				case 205:
					inputSampleR = -0.085514174893643075;
					break;
				case 206:
					inputSampleR = -0.071841763435468692;
					break;
				case 207:
					inputSampleR = -0.050936226805627698;
					break;
				case 208:
					inputSampleR = -0.028443727683034846;
					break;
				case 209:
					inputSampleR = -0.010468087175817219;
					break;
				case 210:
					inputSampleR = -0.002105896366466348;
					break;
				case 211:
					inputSampleR = -0.003662340349742205;
					break;
				case 212:
					inputSampleR = -0.01220764221594906;
					break;
				case 213:
					inputSampleR = -0.022248351046479778;
					break;
				case 214:
					inputSampleR = -0.029847579118930719;
					break;
				case 215:
					inputSampleR = -0.031343105296182221;
					break;
				case 216:
					inputSampleR = -0.024750988036439187;
					break;
				case 217:
					inputSampleR = -0.013550456632892937;
					break;
				case 218:
					inputSampleR = -0.000854637477950463;
					break;
				case 219:
					inputSampleR = 0.010071227340311899;
					break;
				case 220:
					inputSampleR = 0.016663344600054933;
					break;
				case 221:
					inputSampleR = 0.016846455656910918;
					break;
				case 222:
					inputSampleR = 0.01190233790887173;
					break;
				case 223:
					inputSampleR = 0.006378368480788599;
					break;
				case 224:
					inputSampleR = 0.004242072817468795;
					break;
				case 225:
					inputSampleR = 0.007934931676992095;
					break;
				case 226:
					inputSampleR = 0.01995934362346263;
					break;
				case 227:
					inputSampleR = 0.041017472800683609;
					break;
				case 228:
					inputSampleR = 0.067507896664632097;
					break;
				case 229:
					inputSampleR = 0.095280217139500106;
					break;
				case 230:
					inputSampleR = 0.119237604716940823;
					break;
				case 231:
					inputSampleR = 0.134863319994506653;
					break;
				case 232:
					inputSampleR = 0.139074874302194285;
					break;
				case 233:
					inputSampleR = 0.130285424360179453;
					break;
				case 234:
					inputSampleR = 0.109227295182958456;
					break;
				case 235:
					inputSampleR = 0.077731716552018798;
					break;
				case 236:
					inputSampleR = 0.038667428358104189;
					break;
				case 237:
					inputSampleR = -0.002929896122318273;
					break;
				case 238:
					inputSampleR = -0.042360406429944847;
					break;
				case 239:
					inputSampleR = -0.075229317987304381;
					break;
				case 240:
					inputSampleR = -0.097386113505661273;
					break;
				case 241:
					inputSampleR = -0.105595711767632161;
					break;
				case 242:
					inputSampleR = -0.098606853884701154;
					break;
				case 243:
					inputSampleR = -0.07764028023590816;
					break;
				case 244:
					inputSampleR = -0.046632997756584457;
					break;
				case 245:
					inputSampleR = -0.011505716498001124;
					break;
				case 246:
					inputSampleR = 0.021668380154118472;
					break;
				case 247:
					inputSampleR = 0.048555663854182557;
					break;
				case 248:
					inputSampleR = 0.061800816179692977;
					break;
				case 249:
					inputSampleR = 0.060244372196417123;
					break;
				case 250:
					inputSampleR = 0.046663397053743089;
					break;
				case 251:
					inputSampleR = 0.024995016899929806;
					break;
				case 252:
					inputSampleR = 0.000061037019257179;
					break;
				case 253:
					inputSampleR = -0.022461980612811756;
					break;
				case 254:
					inputSampleR = -0.036927992530289708;
					break;
				case 255:
					inputSampleR = -0.038942214155705521;
					break;
				case 256:
					inputSampleR = -0.026917802209234988;
					break;
				case 257:
					inputSampleR = -0.002624711027558301;
					break;
				case 258:
					inputSampleR = 0.029603311830805383;
					break;
				case 259:
					inputSampleR = 0.064456045717032381;
					break;
				case 260:
					inputSampleR = 0.096531476028015983;
					break;
				case 261:
					inputSampleR = 0.120824567209692674;
					break;
				case 262:
					inputSampleR = 0.132910135388042833;
					break;
				case 263:
					inputSampleR = 0.129858165227515476;
					break;
				case 264:
					inputSampleR = 0.111302673040253916;
					break;
				case 265:
					inputSampleR = 0.080173316523026214;
					break;
				case 266:
					inputSampleR = 0.042482361255531474;
					break;
				case 267:
					inputSampleR = 0.006225775933408613;
					break;
				case 268:
					inputSampleR = -0.020295166440015958;
					break;
				case 269:
					inputSampleR = -0.030457949308450662;
					break;
				case 270:
					inputSampleR = -0.020844499610583905;
					break;
				case 271:
					inputSampleR = 0.007202368236640523;
					break;
				case 272:
					inputSampleR = 0.048006330683614609;
					break;
				case 273:
					inputSampleR = 0.092655506111636698;
					break;
				case 274:
					inputSampleR = 0.13120097964445937;
					break;
				case 275:
					inputSampleR = 0.154975256165044095;
					break;
				case 276:
					inputSampleR = 0.15885122608142338;
					break;
				case 277:
					inputSampleR = 0.142798251671193588;
					break;
				case 278:
					inputSampleR = 0.111333191549729912;
					break;
				case 279:
					inputSampleR = 0.071811125713675344;
					break;
				case 280:
					inputSampleR = 0.032472170934476755;
					break;
				case 281:
					inputSampleR = 0.000732444227729118;
					break;
				case 282:
					inputSampleR = -0.018616648418836111;
					break;
				case 283:
					inputSampleR = -0.024903580583819172;
					break;
				case 284:
					inputSampleR = -0.020722425572679917;
					break;
				case 285:
					inputSampleR = -0.010681716742149201;
					break;
				case 286:
					inputSampleR = 0.000030518509781182;
					break;
				case 287:
					inputSampleR = 0.006958220160832545;
					break;
				case 288:
					inputSampleR = 0.007538071840876492;
					break;
				case 289:
					inputSampleR = 0.001831110568865017;
					break;
				case 290:
					inputSampleR = -0.008179198965117431;
					break;
				case 291:
					inputSampleR = -0.019501685193640029;
					break;
				case 292:
					inputSampleR = -0.028443727683034846;
					break;
				case 293:
					inputSampleR = -0.031678808900418187;
					break;
				case 294:
					inputSampleR = -0.027467135379802936;
					break;
				case 295:
					inputSampleR = -0.016083612132328343;
					break;
				case 296:
					inputSampleR = 0.000518814661397137;
					break;
				case 297:
					inputSampleR = 0.019593121509750663;
					break;
				case 298:
					inputSampleR = 0.037782391583300268;
					break;
				case 299:
					inputSampleR = 0.052126329462874231;
					break;
				case 300:
					inputSampleR = 0.060946297914365059;
					break;
				case 301:
					inputSampleR = 0.063601527451704457;
					break;
				case 302:
					inputSampleR = 0.059939187101657153;
					break;
				case 303:
					inputSampleR = 0.050386774422742392;
					break;
				case 304:
					inputSampleR = 0.036714243751640369;
					break;
				case 305:
					inputSampleR = 0.021729417173070469;
					break;
				case 306:
					inputSampleR = 0.008453746338084048;
					break;
				case 307:
					inputSampleR = -0.000579970892666489;
					break;
				case 308:
					inputSampleR = -0.003814932897122191;
					break;
				case 309:
					inputSampleR = -0.000793600458998469;
					break;
				case 310:
					inputSampleR = 0.007507553331400495;
					break;
				case 311:
					inputSampleR = 0.019654158528702659;
					break;
				case 312:
					inputSampleR = 0.033692911313516642;
					break;
				case 313:
					inputSampleR = 0.047579071550950647;
					break;
				case 314:
					inputSampleR = 0.059542446478469187;
					break;
				case 315:
					inputSampleR = 0.068881229591051973;
					break;
				case 316:
					inputSampleR = 0.075961643002410956;
					break;
				case 317:
					inputSampleR = 0.081821316034730057;
					break;
				case 318:
					inputSampleR = 0.087864100123905151;
					break;
				case 319:
					inputSampleR = 0.094944513535264133;
					break;
				case 320:
					inputSampleR = 0.10254374160771508;
					break;
				case 321:
					inputSampleR = 0.108342258408154549;
					break;
				case 322:
					inputSampleR = 0.109257813692434452;
					break;
				case 323:
					inputSampleR = 0.102574260117191077;
					break;
				case 324:
					inputSampleR = 0.086032989555345316;
					break;
				case 325:
					inputSampleR = 0.058748965232093261;
					break;
				case 326:
					inputSampleR = 0.02255341692892239;
					break;
				case 327:
					inputSampleR = -0.017701093134556194;
					break;
				case 328:
					inputSampleR = -0.0557277520062869;
					break;
				case 329:
					inputSampleR = -0.085025878742027119;
					break;
				case 330:
					inputSampleR = -0.100071742339549019;
					break;
				case 331:
					inputSampleR = -0.09766078009094524;
					break;
				case 332:
					inputSampleR = -0.07873906578997171;
					break;
				case 333:
					inputSampleR = -0.048220079462263948;
					break;
				case 334:
					inputSampleR = -0.013458901104464944;
					break;
				case 335:
					inputSampleR = 0.017304233299050873;
					break;
				case 336:
					inputSampleR = 0.036439577166356395;
					break;
				case 337:
					inputSampleR = 0.039033650471816152;
					break;
				case 338:
					inputSampleR = 0.024567757767265847;
					break;
				case 339:
					inputSampleR = -0.003753895878170197;
					break;
				case 340:
					inputSampleR = -0.040620851389813008;
					break;
				case 341:
					inputSampleR = -0.078555954733115718;
					break;
				case 342:
					inputSampleR = -0.109135858866847832;
					break;
				case 343:
					inputSampleR = -0.12497520371074565;
					break;
				case 344:
					inputSampleR = -0.121618167668385957;
					break;
				case 345:
					inputSampleR = -0.097599743071993247;
					break;
				case 346:
					inputSampleR = -0.054140670300607402;
					break;
				case 347:
					inputSampleR = 0.004272591326944792;
					break;
				case 348:
					inputSampleR = 0.069857821894283884;
					break;
				case 349:
					inputSampleR = 0.133306876011230813;
					break;
				case 350:
					inputSampleR = 0.185463723983275847;
					break;
				case 351:
					inputSampleR = 0.218088487464827424;
					break;
				case 352:
					inputSampleR = 0.225413048951994377;
					break;
				case 353:
					inputSampleR = 0.206582890179448836;
					break;
				case 354:
					inputSampleR = 0.166389417134922335;
					break;
				case 355:
					inputSampleR = 0.113957783364665666;
					break;
				case 356:
					inputSampleR = 0.05987815008270516;
					break;
				case 357:
					inputSampleR = 0.014435374195379498;
					break;
				case 358:
					inputSampleR = -0.01416082682241288;
					break;
				case 359:
					inputSampleR = -0.021790573404339819;
					break;
				case 360:
					inputSampleR = -0.00991875400524927;
					break;
				case 361:
					inputSampleR = 0.0144048556859035;
					break;
				case 362:
					inputSampleR = 0.040742806215399635;
					break;
				case 363:
					inputSampleR = 0.059176224364757224;
					break;
				case 364:
					inputSampleR = 0.063784638508560434;
					break;
				case 365:
					inputSampleR = 0.053255514313486123;
					break;
				case 366:
					inputSampleR = 0.030244200529801323;
					break;
				case 367:
					inputSampleR = 0.000610370189825129;
					break;
				case 368:
					inputSampleR = -0.017395908039796223;
					break;
				case 369:
					inputSampleR = -0.039918925671865071;
					break;
				case 370:
					inputSampleR = -0.052797855883663529;
					break;
				case 371:
					inputSampleR = -0.055544640949430915;
					break;
				case 372:
					inputSampleR = -0.043153887676320772;
					break;
				case 373:
					inputSampleR = -0.021210721724295872;
					break;
				case 374:
					inputSampleR = 0.004119998779564807;
					break;
				case 375:
					inputSampleR = 0.029237089717093417;
					break;
				case 376:
					inputSampleR = 0.048982922986846519;
					break;
				case 377:
					inputSampleR = 0.061739779160740985;
					break;
				case 378:
					inputSampleR = 0.067141674550920127;
					break;
				case 379:
					inputSampleR = 0.065615749077120269;
					break;
				case 380:
					inputSampleR = 0.060305409215369116;
					break;
				case 381:
					inputSampleR = 0.052034773934446235;
					break;
				case 382:
					inputSampleR = 0.047731664098330635;
					break;
				case 383:
					inputSampleR = 0.042390805727103485;
					break;
				case 384:
					inputSampleR = 0.035096762749412515;
					break;
				case 385:
					inputSampleR = 0.025819016655781731;
					break;
				case 386:
					inputSampleR = 0.01278737468367565;
					break;
				case 387:
					inputSampleR = -0.004303229048738146;
					break;
				case 388:
					inputSampleR = -0.022461980612811756;
					break;
				case 389:
					inputSampleR = -0.041200703069856952;
					break;
				case 390:
					inputSampleR = -0.058138714254890679;
					break;
				case 391:
					inputSampleR = -0.073215096361888568;
					break;
				case 392:
					inputSampleR = -0.077213021103244198;
					break;
				case 393:
					inputSampleR = -0.073001466795556594;
					break;
				case 394:
					inputSampleR = -0.063937350268257781;
					break;
				case 395:
					inputSampleR = -0.054171188810083405;
					break;
				case 396:
					inputSampleR = -0.040773443937192996;
					break;
				case 397:
					inputSampleR = -0.031739845919370187;
					break;
				case 398:
					inputSampleR = -0.023682840204779284;
					break;
				case 399:
					inputSampleR = -0.020508796006347935;
					break;
				case 400:
					inputSampleR = -0.025910691396527082;
					break;
				case 401:
					inputSampleR = -0.04406944296060069;
					break;
				case 402:
					inputSampleR = -0.065646386798913631;
					break;
				case 403:
					inputSampleR = -0.091160218359630202;
					break;
				case 404:
					inputSampleR = -0.117101309053010735;
					break;
				case 405:
					inputSampleR = -0.140478844950407522;
					break;
				case 406:
					inputSampleR = -0.152778042695089655;
					break;
				case 407:
					inputSampleR = -0.149024266029541996;
					break;
				case 408:
					inputSampleR = -0.126867470511185132;
					break;
				case 409:
					inputSampleR = -0.084934323213599131;
					break;
				case 410:
					inputSampleR = -0.027711283455610913;
					break;
				case 411:
					inputSampleR = 0.028779312074953458;
					break;
				case 412:
					inputSampleR = 0.082279093676870016;
					break;
				case 413:
					inputSampleR = 0.120580419133884703;
					break;
				case 414:
					inputSampleR = 0.137945689451582393;
					break;
				case 415:
					inputSampleR = 0.128332239753715632;
					break;
				case 416:
					inputSampleR = 0.098209994049195837;
					break;
				case 417:
					inputSampleR = 0.057345113796197389;
					break;
				case 418:
					inputSampleR = 0.014282781647999511;
					break;
				case 419:
					inputSampleR = -0.019440648174688036;
					break;
				case 420:
					inputSampleR = -0.040712406918240997;
					break;
				case 421:
					inputSampleR = -0.052675781845759537;
					break;
				case 422:
					inputSampleR = -0.049166153256019862;
					break;
				case 423:
					inputSampleR = -0.03909492591601315;
					break;
				case 424:
					inputSampleR = -0.02404906231849125;
					break;
				case 425:
					inputSampleR = -0.010071346552629257;
					break;
				case 426:
					inputSampleR = -0.005920710050965997;
					break;
				case 427:
					inputSampleR = -0.004547377124546123;
					break;
				case 428:
					inputSampleR = -0.014008234275032894;
					break;
				case 429:
					inputSampleR = -0.021515906819055845;
					break;
				case 430:
					inputSampleR = -0.027009357737662981;
					break;
				case 431:
					inputSampleR = -0.037538362719809648;
					break;
				case 432:
					inputSampleR = -0.043520109790032742;
					break;
				case 433:
					inputSampleR = -0.04736556119693603;
					break;
				case 434:
					inputSampleR = -0.048159042443311956;
					break;
				case 435:
					inputSampleR = -0.045076553773308596;
					break;
				case 436:
					inputSampleR = -0.039216999953917135;
					break;
				case 437:
					inputSampleR = -0.030946245460066617;
					break;
				case 438:
					inputSampleR = -0.026490543076571026;
					break;
				case 439:
					inputSampleR = -0.020234129421063961;
					break;
				case 440:
					inputSampleR = -0.012085568178045071;
					break;
				case 441:
					inputSampleR = -0.007660265091097837;
					break;
				case 442:
					inputSampleR = -0.005615524956206025;
					break;
				case 443:
					inputSampleR = -0.005218784333018062;
					break;
				case 444:
					inputSampleR = -0.008698013626209382;
					break;
				case 445:
					inputSampleR = -0.015931019584948358;
					break;
				case 446:
					inputSampleR = -0.026521061586047025;
					break;
				case 447:
					inputSampleR = -0.036165029793389779;
					break;
				case 448:
					inputSampleR = -0.043733739356364723;
					break;
				case 449:
					inputSampleR = -0.04843370902859593;
					break;
				case 450:
					inputSampleR = -0.048616820085451914;
					break;
				case 451:
					inputSampleR = -0.043031813638416787;
					break;
				case 452:
					inputSampleR = -0.032167105052034142;
					break;
				case 453:
					inputSampleR = -0.019288055627308048;
					break;
				case 454:
					inputSampleR = -0.00415063650135816;
					break;
				case 455:
					inputSampleR = 0.010467967963499863;
					break;
				case 456:
					inputSampleR = 0.020539195303506577;
					break;
				case 457:
					inputSampleR = 0.025696942617877742;
					break;
				case 458:
					inputSampleR = 0.027375460639057589;
					break;
				case 459:
					inputSampleR = 0.023621683973509933;
					break;
				case 460:
					inputSampleR = 0.016358159505294963;
					break;
				case 461:
					inputSampleR = 0.008911523980224005;
					break;
				case 462:
					inputSampleR = 0.002319406720480971;
					break;
				case 463:
					inputSampleR = -0.002716266555986292;
					break;
				case 464:
					inputSampleR = -0.00589019154149;
					break;
				case 465:
					inputSampleR = -0.006683672787865927;
					break;
				case 466:
					inputSampleR = -0.005279821351970056;
					break;
				case 467:
					inputSampleR = -0.003387673764458231;
					break;
				case 468:
					inputSampleR = -0.001281896610614424;
					break;
			}
		}

		inputSampleL *= nikola->outlevelL;
		inputSampleR *= nikola->outlevelR;
		//if envelope is in use, reduce by amount of quietness in source

		if (wet != 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
		}

		//begin 32 bit stereo floating point dither
		int expon;
		frexpf((float)inputSampleL, &expon);
		nikola->fpdL ^= nikola->fpdL << 13;
		nikola->fpdL ^= nikola->fpdL >> 17;
		nikola->fpdL ^= nikola->fpdL << 5;
		inputSampleL += (((double)nikola->fpdL - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float)inputSampleR, &expon);
		nikola->fpdR ^= nikola->fpdR << 13;
		nikola->fpdR ^= nikola->fpdR >> 17;
		nikola->fpdR ^= nikola->fpdR << 5;
		inputSampleR += (((double)nikola->fpdR - (uint32_t)0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	NIKOLA_URI,
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