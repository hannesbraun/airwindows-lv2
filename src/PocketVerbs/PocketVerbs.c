#include <lv2/core/lv2.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define POCKETVERBS_URI "https://hannesbraun.net/ns/lv2/airwindows/pocketverbs"

typedef enum {
	INPUT_L = 0,
	INPUT_R = 1,
	OUTPUT_L = 2,
	OUTPUT_R = 3,
	TYPE = 4,
	SIZE = 5,
	GATING = 6,
	DRYWET = 7
} PortIndex;

typedef struct {
	const float* input[2];
	float* output[2];
	const float* type;
	const float* size;
	const float* gating;
	const float* drywet;

	double dAR[7];
	double dBR[7];
	double dCR[7];
	double dDR[7];
	double dER[7];
	double dFR[7];
	double dGR[7];
	double dHR[7];
	double dIR[7];
	double dJR[7];
	double dKR[7];
	double dLR[7];
	double dMR[7];
	double dNR[7];
	double dOR[7];
	double dPR[7];
	double dQR[7];
	double dRR[7];
	double dSR[7];
	double dTR[7];
	double dUR[7];
	double dVR[7];
	double dWR[7];
	double dXR[7];
	double dYR[7];
	double dZR[7];

	double aAR[15150];
	double aBR[14618];
	double aCR[14358];
	double aDR[13818];
	double aER[13562];
	double aFR[13046];
	double aGR[11966];
	double aHR[11130];
	double aIR[10598];
	double aJR[9810];
	double aKR[9522];
	double aLR[8982];
	double aMR[8786];
	double aNR[8462];
	double aOR[8310];
	double aPR[7982];
	double aQR[7322];
	double aRR[6818];
	double aSR[6506];
	double aTR[6002];
	double aUR[5838];
	double aVR[5502];
	double aWR[5010];
	double aXR[4850];
	double aYR[4296];
	double aZR[4180];

	double oAR[15150];
	double oBR[14618];
	double oCR[14358];
	double oDR[13818];
	double oER[13562];
	double oFR[13046];
	double oGR[11966];
	double oHR[11130];
	double oIR[10598];
	double oJR[9810];
	double oKR[9522];
	double oLR[8982];
	double oMR[8786];
	double oNR[8462];
	double oOR[8310];
	double oPR[7982];
	double oQR[7322];
	double oRR[6818];
	double oSR[6506];
	double oTR[6002];
	double oUR[5838];
	double oVR[5502];
	double oWR[5010];
	double oXR[4850];
	double oYR[4296];
	double oZR[4180];

	double dAL[7];
	double dBL[7];
	double dCL[7];
	double dDL[7];
	double dEL[7];
	double dFL[7];
	double dGL[7];
	double dHL[7];
	double dIL[7];
	double dJL[7];
	double dKL[7];
	double dLL[7];
	double dML[7];
	double dNL[7];
	double dOL[7];
	double dPL[7];
	double dQL[7];
	double dRL[7];
	double dSL[7];
	double dTL[7];
	double dUL[7];
	double dVL[7];
	double dWL[7];
	double dXL[7];
	double dYL[7];
	double dZL[7];

	double aAL[15150];
	double aBL[14618];
	double aCL[14358];
	double aDL[13818];
	double aEL[13562];
	double aFL[13046];
	double aGL[11966];
	double aHL[11130];
	double aIL[10598];
	double aJL[9810];
	double aKL[9522];
	double aLL[8982];
	double aML[8786];
	double aNL[8462];
	double aOL[8310];
	double aPL[7982];
	double aQL[7322];
	double aRL[6818];
	double aSL[6506];
	double aTL[6002];
	double aUL[5838];
	double aVL[5502];
	double aWL[5010];
	double aXL[4850];
	double aYL[4296];
	double aZL[4180];

	double oAL[15150];
	double oBL[14618];
	double oCL[14358];
	double oDL[13818];
	double oEL[13562];
	double oFL[13046];
	double oGL[11966];
	double oHL[11130];
	double oIL[10598];
	double oJL[9810];
	double oKL[9522];
	double oLL[8982];
	double oML[8786];
	double oNL[8462];
	double oOL[8310];
	double oPL[7982];
	double oQL[7322];
	double oRL[6818];
	double oSL[6506];
	double oTL[6002];
	double oUL[5838];
	double oVL[5502];
	double oWL[5010];
	double oXL[4850];
	double oYL[4296];
	double oZL[4180];

	int outAL, alpAL;
	int outBL, alpBL;
	int outCL, alpCL;
	int outDL, alpDL;
	int outEL, alpEL;
	int outFL, alpFL;
	int outGL, alpGL;
	int outHL, alpHL;
	int outIL, alpIL;
	int outJL, alpJL;
	int outKL, alpKL;
	int outLL, alpLL;
	int outML, alpML;
	int outNL, alpNL;
	int outOL, alpOL;
	int outPL, alpPL;
	int outQL, alpQL;
	int outRL, alpRL;
	int outSL, alpSL;
	int outTL, alpTL;
	int outUL, alpUL;
	int outVL, alpVL;
	int outWL, alpWL;
	int outXL, alpXL;
	int outYL, alpYL;
	int outZL, alpZL;

	int outAR, alpAR, maxdelayA, delayA;
	int outBR, alpBR, maxdelayB, delayB;
	int outCR, alpCR, maxdelayC, delayC;
	int outDR, alpDR, maxdelayD, delayD;
	int outER, alpER, maxdelayE, delayE;
	int outFR, alpFR, maxdelayF, delayF;
	int outGR, alpGR, maxdelayG, delayG;
	int outHR, alpHR, maxdelayH, delayH;
	int outIR, alpIR, maxdelayI, delayI;
	int outJR, alpJR, maxdelayJ, delayJ;
	int outKR, alpKR, maxdelayK, delayK;
	int outLR, alpLR, maxdelayL, delayL;
	int outMR, alpMR, maxdelayM, delayM;
	int outNR, alpNR, maxdelayN, delayN;
	int outOR, alpOR, maxdelayO, delayO;
	int outPR, alpPR, maxdelayP, delayP;
	int outQR, alpQR, maxdelayQ, delayQ;
	int outRR, alpRR, maxdelayR, delayR;
	int outSR, alpSR, maxdelayS, delayS;
	int outTR, alpTR, maxdelayT, delayT;
	int outUR, alpUR, maxdelayU, delayU;
	int outVR, alpVR, maxdelayV, delayV;
	int outWR, alpWR, maxdelayW, delayW;
	int outXR, alpXR, maxdelayX, delayX;
	int outYR, alpYR, maxdelayY, delayY;
	int outZR, alpZR, maxdelayZ, delayZ;

	double savedRoomsize;
	int countdown;
	double peakL;
	double peakR;

	uint32_t fpdL;
	uint32_t fpdR;
} PocketVerbs;

static LV2_Handle instantiate(
	const LV2_Descriptor* descriptor,
	double rate,
	const char* bundle_path,
	const LV2_Feature* const* features)
{
	PocketVerbs* pocketverbs = (PocketVerbs*) calloc(1, sizeof(PocketVerbs));
	return (LV2_Handle) pocketverbs;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PocketVerbs* pocketverbs = (PocketVerbs*) instance;

	switch ((PortIndex) port) {
		case INPUT_L:
			pocketverbs->input[0] = (const float*) data;
			break;
		case INPUT_R:
			pocketverbs->input[1] = (const float*) data;
			break;
		case OUTPUT_L:
			pocketverbs->output[0] = (float*) data;
			break;
		case OUTPUT_R:
			pocketverbs->output[1] = (float*) data;
			break;
		case TYPE:
			pocketverbs->type = (const float*) data;
			break;
		case SIZE:
			pocketverbs->size = (const float*) data;
			break;
		case GATING:
			pocketverbs->gating = (const float*) data;
			break;
		case DRYWET:
			pocketverbs->drywet = (const float*) data;
			break;
	}
}

static void activate(LV2_Handle instance)
{
	PocketVerbs* pocketverbs = (PocketVerbs*) instance;
	int count;

	for (count = 0; count < 6; count++) {
		pocketverbs->dAL[count] = 0.0;
		pocketverbs->dBL[count] = 0.0;
		pocketverbs->dCL[count] = 0.0;
		pocketverbs->dDL[count] = 0.0;
		pocketverbs->dEL[count] = 0.0;
		pocketverbs->dFL[count] = 0.0;
		pocketverbs->dGL[count] = 0.0;
		pocketverbs->dHL[count] = 0.0;
		pocketverbs->dIL[count] = 0.0;
		pocketverbs->dJL[count] = 0.0;
		pocketverbs->dKL[count] = 0.0;
		pocketverbs->dLL[count] = 0.0;
		pocketverbs->dML[count] = 0.0;
		pocketverbs->dNL[count] = 0.0;
		pocketverbs->dOL[count] = 0.0;
		pocketverbs->dPL[count] = 0.0;
		pocketverbs->dQL[count] = 0.0;
		pocketverbs->dRL[count] = 0.0;
		pocketverbs->dSL[count] = 0.0;
		pocketverbs->dTL[count] = 0.0;
		pocketverbs->dUL[count] = 0.0;
		pocketverbs->dVL[count] = 0.0;
		pocketverbs->dWL[count] = 0.0;
		pocketverbs->dXL[count] = 0.0;
		pocketverbs->dYL[count] = 0.0;
		pocketverbs->dZL[count] = 0.0;
	}

	for (count = 0; count < 15149; count++) {
		pocketverbs->aAL[count] = 0.0;
	}
	for (count = 0; count < 15149; count++) {
		pocketverbs->oAL[count] = 0.0;
	}
	for (count = 0; count < 14617; count++) {
		pocketverbs->aBL[count] = 0.0;
	}
	for (count = 0; count < 14617; count++) {
		pocketverbs->oBL[count] = 0.0;
	}
	for (count = 0; count < 14357; count++) {
		pocketverbs->aCL[count] = 0.0;
	}
	for (count = 0; count < 14357; count++) {
		pocketverbs->oCL[count] = 0.0;
	}
	for (count = 0; count < 13817; count++) {
		pocketverbs->aDL[count] = 0.0;
	}
	for (count = 0; count < 13817; count++) {
		pocketverbs->oDL[count] = 0.0;
	}
	for (count = 0; count < 13561; count++) {
		pocketverbs->aEL[count] = 0.0;
	}
	for (count = 0; count < 13561; count++) {
		pocketverbs->oEL[count] = 0.0;
	}
	for (count = 0; count < 13045; count++) {
		pocketverbs->aFL[count] = 0.0;
	}
	for (count = 0; count < 13045; count++) {
		pocketverbs->oFL[count] = 0.0;
	}
	for (count = 0; count < 11965; count++) {
		pocketverbs->aGL[count] = 0.0;
	}
	for (count = 0; count < 11965; count++) {
		pocketverbs->oGL[count] = 0.0;
	}
	for (count = 0; count < 11129; count++) {
		pocketverbs->aHL[count] = 0.0;
	}
	for (count = 0; count < 11129; count++) {
		pocketverbs->oHL[count] = 0.0;
	}
	for (count = 0; count < 10597; count++) {
		pocketverbs->aIL[count] = 0.0;
	}
	for (count = 0; count < 10597; count++) {
		pocketverbs->oIL[count] = 0.0;
	}
	for (count = 0; count < 9809; count++) {
		pocketverbs->aJL[count] = 0.0;
	}
	for (count = 0; count < 9809; count++) {
		pocketverbs->oJL[count] = 0.0;
	}
	for (count = 0; count < 9521; count++) {
		pocketverbs->aKL[count] = 0.0;
	}
	for (count = 0; count < 9521; count++) {
		pocketverbs->oKL[count] = 0.0;
	}
	for (count = 0; count < 8981; count++) {
		pocketverbs->aLL[count] = 0.0;
	}
	for (count = 0; count < 8981; count++) {
		pocketverbs->oLL[count] = 0.0;
	}
	for (count = 0; count < 8785; count++) {
		pocketverbs->aML[count] = 0.0;
	}
	for (count = 0; count < 8785; count++) {
		pocketverbs->oML[count] = 0.0;
	}
	for (count = 0; count < 8461; count++) {
		pocketverbs->aNL[count] = 0.0;
	}
	for (count = 0; count < 8461; count++) {
		pocketverbs->oNL[count] = 0.0;
	}
	for (count = 0; count < 8309; count++) {
		pocketverbs->aOL[count] = 0.0;
	}
	for (count = 0; count < 8309; count++) {
		pocketverbs->oOL[count] = 0.0;
	}
	for (count = 0; count < 7981; count++) {
		pocketverbs->aPL[count] = 0.0;
	}
	for (count = 0; count < 7981; count++) {
		pocketverbs->oPL[count] = 0.0;
	}
	for (count = 0; count < 7321; count++) {
		pocketverbs->aQL[count] = 0.0;
	}
	for (count = 0; count < 7321; count++) {
		pocketverbs->oQL[count] = 0.0;
	}
	for (count = 0; count < 6817; count++) {
		pocketverbs->aRL[count] = 0.0;
	}
	for (count = 0; count < 6817; count++) {
		pocketverbs->oRL[count] = 0.0;
	}
	for (count = 0; count < 6505; count++) {
		pocketverbs->aSL[count] = 0.0;
	}
	for (count = 0; count < 6505; count++) {
		pocketverbs->oSL[count] = 0.0;
	}
	for (count = 0; count < 6001; count++) {
		pocketverbs->aTL[count] = 0.0;
	}
	for (count = 0; count < 6001; count++) {
		pocketverbs->oTL[count] = 0.0;
	}
	for (count = 0; count < 5837; count++) {
		pocketverbs->aUL[count] = 0.0;
	}
	for (count = 0; count < 5837; count++) {
		pocketverbs->oUL[count] = 0.0;
	}
	for (count = 0; count < 5501; count++) {
		pocketverbs->aVL[count] = 0.0;
	}
	for (count = 0; count < 5501; count++) {
		pocketverbs->oVL[count] = 0.0;
	}
	for (count = 0; count < 5009; count++) {
		pocketverbs->aWL[count] = 0.0;
	}
	for (count = 0; count < 5009; count++) {
		pocketverbs->oWL[count] = 0.0;
	}
	for (count = 0; count < 4849; count++) {
		pocketverbs->aXL[count] = 0.0;
	}
	for (count = 0; count < 4849; count++) {
		pocketverbs->oXL[count] = 0.0;
	}
	for (count = 0; count < 4295; count++) {
		pocketverbs->aYL[count] = 0.0;
	}
	for (count = 0; count < 4295; count++) {
		pocketverbs->oYL[count] = 0.0;
	}
	for (count = 0; count < 4179; count++) {
		pocketverbs->aZL[count] = 0.0;
	}
	for (count = 0; count < 4179; count++) {
		pocketverbs->oZL[count] = 0.0;
	}

	for (count = 0; count < 6; count++) {
		pocketverbs->dAR[count] = 0.0;
		pocketverbs->dBR[count] = 0.0;
		pocketverbs->dCR[count] = 0.0;
		pocketverbs->dDR[count] = 0.0;
		pocketverbs->dER[count] = 0.0;
		pocketverbs->dFR[count] = 0.0;
		pocketverbs->dGR[count] = 0.0;
		pocketverbs->dHR[count] = 0.0;
		pocketverbs->dIR[count] = 0.0;
		pocketverbs->dJR[count] = 0.0;
		pocketverbs->dKR[count] = 0.0;
		pocketverbs->dLR[count] = 0.0;
		pocketverbs->dMR[count] = 0.0;
		pocketverbs->dNR[count] = 0.0;
		pocketverbs->dOR[count] = 0.0;
		pocketverbs->dPR[count] = 0.0;
		pocketverbs->dQR[count] = 0.0;
		pocketverbs->dRR[count] = 0.0;
		pocketverbs->dSR[count] = 0.0;
		pocketverbs->dTR[count] = 0.0;
		pocketverbs->dUR[count] = 0.0;
		pocketverbs->dVR[count] = 0.0;
		pocketverbs->dWR[count] = 0.0;
		pocketverbs->dXR[count] = 0.0;
		pocketverbs->dYR[count] = 0.0;
		pocketverbs->dZR[count] = 0.0;
	}

	for (count = 0; count < 15149; count++) {
		pocketverbs->aAR[count] = 0.0;
	}
	for (count = 0; count < 15149; count++) {
		pocketverbs->oAR[count] = 0.0;
	}
	for (count = 0; count < 14617; count++) {
		pocketverbs->aBR[count] = 0.0;
	}
	for (count = 0; count < 14617; count++) {
		pocketverbs->oBR[count] = 0.0;
	}
	for (count = 0; count < 14357; count++) {
		pocketverbs->aCR[count] = 0.0;
	}
	for (count = 0; count < 14357; count++) {
		pocketverbs->oCR[count] = 0.0;
	}
	for (count = 0; count < 13817; count++) {
		pocketverbs->aDR[count] = 0.0;
	}
	for (count = 0; count < 13817; count++) {
		pocketverbs->oDR[count] = 0.0;
	}
	for (count = 0; count < 13561; count++) {
		pocketverbs->aER[count] = 0.0;
	}
	for (count = 0; count < 13561; count++) {
		pocketverbs->oER[count] = 0.0;
	}
	for (count = 0; count < 13045; count++) {
		pocketverbs->aFR[count] = 0.0;
	}
	for (count = 0; count < 13045; count++) {
		pocketverbs->oFR[count] = 0.0;
	}
	for (count = 0; count < 11965; count++) {
		pocketverbs->aGR[count] = 0.0;
	}
	for (count = 0; count < 11965; count++) {
		pocketverbs->oGR[count] = 0.0;
	}
	for (count = 0; count < 11129; count++) {
		pocketverbs->aHR[count] = 0.0;
	}
	for (count = 0; count < 11129; count++) {
		pocketverbs->oHR[count] = 0.0;
	}
	for (count = 0; count < 10597; count++) {
		pocketverbs->aIR[count] = 0.0;
	}
	for (count = 0; count < 10597; count++) {
		pocketverbs->oIR[count] = 0.0;
	}
	for (count = 0; count < 9809; count++) {
		pocketverbs->aJR[count] = 0.0;
	}
	for (count = 0; count < 9809; count++) {
		pocketverbs->oJR[count] = 0.0;
	}
	for (count = 0; count < 9521; count++) {
		pocketverbs->aKR[count] = 0.0;
	}
	for (count = 0; count < 9521; count++) {
		pocketverbs->oKR[count] = 0.0;
	}
	for (count = 0; count < 8981; count++) {
		pocketverbs->aLR[count] = 0.0;
	}
	for (count = 0; count < 8981; count++) {
		pocketverbs->oLR[count] = 0.0;
	}
	for (count = 0; count < 8785; count++) {
		pocketverbs->aMR[count] = 0.0;
	}
	for (count = 0; count < 8785; count++) {
		pocketverbs->oMR[count] = 0.0;
	}
	for (count = 0; count < 8461; count++) {
		pocketverbs->aNR[count] = 0.0;
	}
	for (count = 0; count < 8461; count++) {
		pocketverbs->oNR[count] = 0.0;
	}
	for (count = 0; count < 8309; count++) {
		pocketverbs->aOR[count] = 0.0;
	}
	for (count = 0; count < 8309; count++) {
		pocketverbs->oOR[count] = 0.0;
	}
	for (count = 0; count < 7981; count++) {
		pocketverbs->aPR[count] = 0.0;
	}
	for (count = 0; count < 7981; count++) {
		pocketverbs->oPR[count] = 0.0;
	}
	for (count = 0; count < 7321; count++) {
		pocketverbs->aQR[count] = 0.0;
	}
	for (count = 0; count < 7321; count++) {
		pocketverbs->oQR[count] = 0.0;
	}
	for (count = 0; count < 6817; count++) {
		pocketverbs->aRR[count] = 0.0;
	}
	for (count = 0; count < 6817; count++) {
		pocketverbs->oRR[count] = 0.0;
	}
	for (count = 0; count < 6505; count++) {
		pocketverbs->aSR[count] = 0.0;
	}
	for (count = 0; count < 6505; count++) {
		pocketverbs->oSR[count] = 0.0;
	}
	for (count = 0; count < 6001; count++) {
		pocketverbs->aTR[count] = 0.0;
	}
	for (count = 0; count < 6001; count++) {
		pocketverbs->oTR[count] = 0.0;
	}
	for (count = 0; count < 5837; count++) {
		pocketverbs->aUR[count] = 0.0;
	}
	for (count = 0; count < 5837; count++) {
		pocketverbs->oUR[count] = 0.0;
	}
	for (count = 0; count < 5501; count++) {
		pocketverbs->aVR[count] = 0.0;
	}
	for (count = 0; count < 5501; count++) {
		pocketverbs->oVR[count] = 0.0;
	}
	for (count = 0; count < 5009; count++) {
		pocketverbs->aWR[count] = 0.0;
	}
	for (count = 0; count < 5009; count++) {
		pocketverbs->oWR[count] = 0.0;
	}
	for (count = 0; count < 4849; count++) {
		pocketverbs->aXR[count] = 0.0;
	}
	for (count = 0; count < 4849; count++) {
		pocketverbs->oXR[count] = 0.0;
	}
	for (count = 0; count < 4295; count++) {
		pocketverbs->aYR[count] = 0.0;
	}
	for (count = 0; count < 4295; count++) {
		pocketverbs->oYR[count] = 0.0;
	}
	for (count = 0; count < 4179; count++) {
		pocketverbs->aZR[count] = 0.0;
	}
	for (count = 0; count < 4179; count++) {
		pocketverbs->oZR[count] = 0.0;
	}

	pocketverbs->outAL = 1;
	pocketverbs->alpAL = 1;
	pocketverbs->outBL = 1;
	pocketverbs->alpBL = 1;
	pocketverbs->outCL = 1;
	pocketverbs->alpCL = 1;
	pocketverbs->outDL = 1;
	pocketverbs->alpDL = 1;
	pocketverbs->outEL = 1;
	pocketverbs->alpEL = 1;
	pocketverbs->outFL = 1;
	pocketverbs->alpFL = 1;
	pocketverbs->outGL = 1;
	pocketverbs->alpGL = 1;
	pocketverbs->outHL = 1;
	pocketverbs->alpHL = 1;
	pocketverbs->outIL = 1;
	pocketverbs->alpIL = 1;
	pocketverbs->outJL = 1;
	pocketverbs->alpJL = 1;
	pocketverbs->outKL = 1;
	pocketverbs->alpKL = 1;
	pocketverbs->outLL = 1;
	pocketverbs->alpLL = 1;
	pocketverbs->outML = 1;
	pocketverbs->alpML = 1;
	pocketverbs->outNL = 1;
	pocketverbs->alpNL = 1;
	pocketverbs->outOL = 1;
	pocketverbs->alpOL = 1;
	pocketverbs->outPL = 1;
	pocketverbs->alpPL = 1;
	pocketverbs->outQL = 1;
	pocketverbs->alpQL = 1;
	pocketverbs->outRL = 1;
	pocketverbs->alpRL = 1;
	pocketverbs->outSL = 1;
	pocketverbs->alpSL = 1;
	pocketverbs->outTL = 1;
	pocketverbs->alpTL = 1;
	pocketverbs->outUL = 1;
	pocketverbs->alpUL = 1;
	pocketverbs->outVL = 1;
	pocketverbs->alpVL = 1;
	pocketverbs->outWL = 1;
	pocketverbs->alpWL = 1;
	pocketverbs->outXL = 1;
	pocketverbs->alpXL = 1;
	pocketverbs->outYL = 1;
	pocketverbs->alpYL = 1;
	pocketverbs->outZL = 1;
	pocketverbs->alpZL = 1;

	pocketverbs->outAR = 1;
	pocketverbs->alpAR = 1;
	pocketverbs->delayA = 4;
	pocketverbs->maxdelayA = 7573;
	pocketverbs->outBR = 1;
	pocketverbs->alpBR = 1;
	pocketverbs->delayB = 4;
	pocketverbs->maxdelayB = 7307;
	pocketverbs->outCR = 1;
	pocketverbs->alpCR = 1;
	pocketverbs->delayC = 4;
	pocketverbs->maxdelayC = 7177;
	pocketverbs->outDR = 1;
	pocketverbs->alpDR = 1;
	pocketverbs->delayD = 4;
	pocketverbs->maxdelayD = 6907;
	pocketverbs->outER = 1;
	pocketverbs->alpER = 1;
	pocketverbs->delayE = 4;
	pocketverbs->maxdelayE = 6779;
	pocketverbs->outFR = 1;
	pocketverbs->alpFR = 1;
	pocketverbs->delayF = 4;
	pocketverbs->maxdelayF = 6521;
	pocketverbs->outGR = 1;
	pocketverbs->alpGR = 1;
	pocketverbs->delayG = 4;
	pocketverbs->maxdelayG = 5981;
	pocketverbs->outHR = 1;
	pocketverbs->alpHR = 1;
	pocketverbs->delayH = 4;
	pocketverbs->maxdelayH = 5563;
	pocketverbs->outIR = 1;
	pocketverbs->alpIR = 1;
	pocketverbs->delayI = 4;
	pocketverbs->maxdelayI = 5297;
	pocketverbs->outJR = 1;
	pocketverbs->alpJR = 1;
	pocketverbs->delayJ = 4;
	pocketverbs->maxdelayJ = 4903;
	pocketverbs->outKR = 1;
	pocketverbs->alpKR = 1;
	pocketverbs->delayK = 4;
	pocketverbs->maxdelayK = 4759;
	pocketverbs->outLR = 1;
	pocketverbs->alpLR = 1;
	pocketverbs->delayL = 4;
	pocketverbs->maxdelayL = 4489;
	pocketverbs->outMR = 1;
	pocketverbs->alpMR = 1;
	pocketverbs->delayM = 4;
	pocketverbs->maxdelayM = 4391;
	pocketverbs->outNR = 1;
	pocketverbs->alpNR = 1;
	pocketverbs->delayN = 4;
	pocketverbs->maxdelayN = 4229;
	pocketverbs->outOR = 1;
	pocketverbs->alpOR = 1;
	pocketverbs->delayO = 4;
	pocketverbs->maxdelayO = 4153;
	pocketverbs->outPR = 1;
	pocketverbs->alpPR = 1;
	pocketverbs->delayP = 4;
	pocketverbs->maxdelayP = 3989;
	pocketverbs->outQR = 1;
	pocketverbs->alpQR = 1;
	pocketverbs->delayQ = 4;
	pocketverbs->maxdelayQ = 3659;
	pocketverbs->outRR = 1;
	pocketverbs->alpRR = 1;
	pocketverbs->delayR = 4;
	pocketverbs->maxdelayR = 3407;
	pocketverbs->outSR = 1;
	pocketverbs->alpSR = 1;
	pocketverbs->delayS = 4;
	pocketverbs->maxdelayS = 3251;
	pocketverbs->outTR = 1;
	pocketverbs->alpTR = 1;
	pocketverbs->delayT = 4;
	pocketverbs->maxdelayT = 2999;
	pocketverbs->outUR = 1;
	pocketverbs->alpUR = 1;
	pocketverbs->delayU = 4;
	pocketverbs->maxdelayU = 2917;
	pocketverbs->outVR = 1;
	pocketverbs->alpVR = 1;
	pocketverbs->delayV = 4;
	pocketverbs->maxdelayV = 2749;
	pocketverbs->outWR = 1;
	pocketverbs->alpWR = 1;
	pocketverbs->delayW = 4;
	pocketverbs->maxdelayW = 2503;
	pocketverbs->outXR = 1;
	pocketverbs->alpXR = 1;
	pocketverbs->delayX = 4;
	pocketverbs->maxdelayX = 2423;
	pocketverbs->outYR = 1;
	pocketverbs->alpYR = 1;
	pocketverbs->delayY = 4;
	pocketverbs->maxdelayY = 2146;
	pocketverbs->outZR = 1;
	pocketverbs->alpZR = 1;
	pocketverbs->delayZ = 4;
	pocketverbs->maxdelayZ = 2088;

	pocketverbs->savedRoomsize = -1.0; // force update to begin
	pocketverbs->countdown = -1;
	pocketverbs->peakL = 1.0;
	pocketverbs->peakR = 1.0;
	pocketverbs->fpdL = 1.0;
	while (pocketverbs->fpdL < 16386) pocketverbs->fpdL = rand() * UINT32_MAX;
	pocketverbs->fpdR = 1.0;
	while (pocketverbs->fpdR < 16386) pocketverbs->fpdR = rand() * UINT32_MAX;
}

static void run(LV2_Handle instance, uint32_t sampleFrames)
{
	PocketVerbs* pocketverbs = (PocketVerbs*) instance;

	const float* in1 = pocketverbs->input[0];
	const float* in2 = pocketverbs->input[1];
	float* out1 = pocketverbs->output[0];
	float* out2 = pocketverbs->output[1];

	int verbtype = ((int) *pocketverbs->type) + 1;
	if (verbtype > 6) {
		verbtype = 6;
	}

	double roomsize = (pow(*pocketverbs->size, 2) * 1.9) + 0.1;

	double release = 0.00008 * pow(*pocketverbs->gating, 3);
	if (release == 0.0) {
		pocketverbs->peakL = 1.0;
		pocketverbs->peakR = 1.0;
	}
	double wetnesstarget = *pocketverbs->drywet;
	double dryness = (1.0 - wetnesstarget);
	// verbs use base wetness value internally
	double wetness = wetnesstarget;
	double constallpass = 0.618033988749894848204586; // golden ratio!
	int allpasstemp;
	int count;
	int max = 70; // biggest divisor to test primes against
	double bridgerectifier;
	double gain = 0.5 + (wetnesstarget * 0.5); // dryer for less verb drive
	// used as an aux, saturates when fed high levels

	// remap values to primes input number in question is 'i'
	// max is the largest prime we care about- HF interactions more interesting than the big numbers
	// pushing values larger and larger until we have a result
	// for (primetest=2; primetest <= max; primetest++) {if ( i!=primetest && i % primetest == 0 ) {i += 1; primetest=2;}}

	if (pocketverbs->savedRoomsize != roomsize) {
		pocketverbs->savedRoomsize = roomsize; // kick off the adjustment which will take 26 zippernoise refreshes to complete
		pocketverbs->countdown = 26;
	}

	if (pocketverbs->countdown > 0) {
		switch (pocketverbs->countdown) {
			case 1:
				pocketverbs->delayA = ((int) pocketverbs->maxdelayA * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayA != count && pocketverbs->delayA % count == 0) {
						pocketverbs->delayA += 1; // try for primeish As
						count = 2;
					}
				}
				if (pocketverbs->delayA > pocketverbs->maxdelayA) pocketverbs->delayA = pocketverbs->maxdelayA; // insanitycheck
				for (count = pocketverbs->alpAL; count < 15149; count++) {
					pocketverbs->aAL[count] = 0.0;
				}
				for (count = pocketverbs->outAL; count < 15149; count++) {
					pocketverbs->oAL[count] = 0.0;
				}
				break;

			case 2:
				pocketverbs->delayB = ((int) pocketverbs->maxdelayB * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayB != count && pocketverbs->delayB % count == 0) {
						pocketverbs->delayB += 1; // try for primeish Bs
						count = 2;
					}
				}
				if (pocketverbs->delayB > pocketverbs->maxdelayB) pocketverbs->delayB = pocketverbs->maxdelayB; // insanitycheck
				for (count = pocketverbs->alpBL; count < 14617; count++) {
					pocketverbs->aBL[count] = 0.0;
				}
				for (count = pocketverbs->outBL; count < 14617; count++) {
					pocketverbs->oBL[count] = 0.0;
				}
				break;

			case 3:
				pocketverbs->delayC = ((int) pocketverbs->maxdelayC * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayC != count && pocketverbs->delayC % count == 0) {
						pocketverbs->delayC += 1; // try for primeish Cs
						count = 2;
					}
				}
				if (pocketverbs->delayC > pocketverbs->maxdelayC) pocketverbs->delayC = pocketverbs->maxdelayC; // insanitycheck
				for (count = pocketverbs->alpCL; count < 14357; count++) {
					pocketverbs->aCL[count] = 0.0;
				}
				for (count = pocketverbs->outCL; count < 14357; count++) {
					pocketverbs->oCL[count] = 0.0;
				}
				break;

			case 4:
				pocketverbs->delayD = ((int) pocketverbs->maxdelayD * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayD != count && pocketverbs->delayD % count == 0) {
						pocketverbs->delayD += 1; // try for primeish Ds
						count = 2;
					}
				}
				if (pocketverbs->delayD > pocketverbs->maxdelayD) pocketverbs->delayD = pocketverbs->maxdelayD; // insanitycheck
				for (count = pocketverbs->alpDL; count < 13817; count++) {
					pocketverbs->aDL[count] = 0.0;
				}
				for (count = pocketverbs->outDL; count < 13817; count++) {
					pocketverbs->oDL[count] = 0.0;
				}
				break;

			case 5:
				pocketverbs->delayE = ((int) pocketverbs->maxdelayE * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayE != count && pocketverbs->delayE % count == 0) {
						pocketverbs->delayE += 1; // try for primeish Es
						count = 2;
					}
				}
				if (pocketverbs->delayE > pocketverbs->maxdelayE) pocketverbs->delayE = pocketverbs->maxdelayE; // insanitycheck
				for (count = pocketverbs->alpEL; count < 13561; count++) {
					pocketverbs->aEL[count] = 0.0;
				}
				for (count = pocketverbs->outEL; count < 13561; count++) {
					pocketverbs->oEL[count] = 0.0;
				}
				break;

			case 6:
				pocketverbs->delayF = ((int) pocketverbs->maxdelayF * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayF != count && pocketverbs->delayF % count == 0) {
						pocketverbs->delayF += 1; // try for primeish Fs
						count = 2;
					}
				}
				if (pocketverbs->delayF > pocketverbs->maxdelayF) pocketverbs->delayF = pocketverbs->maxdelayF; // insanitycheck
				for (count = pocketverbs->alpFL; count < 13045; count++) {
					pocketverbs->aFL[count] = 0.0;
				}
				for (count = pocketverbs->outFL; count < 13045; count++) {
					pocketverbs->oFL[count] = 0.0;
				}
				break;

			case 7:
				pocketverbs->delayG = ((int) pocketverbs->maxdelayG * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayG != count && pocketverbs->delayG % count == 0) {
						pocketverbs->delayG += 1; // try for primeish Gs
						count = 2;
					}
				}
				if (pocketverbs->delayG > pocketverbs->maxdelayG) pocketverbs->delayG = pocketverbs->maxdelayG; // insanitycheck
				for (count = pocketverbs->alpGL; count < 11965; count++) {
					pocketverbs->aGL[count] = 0.0;
				}
				for (count = pocketverbs->outGL; count < 11965; count++) {
					pocketverbs->oGL[count] = 0.0;
				}
				break;

			case 8:
				pocketverbs->delayH = ((int) pocketverbs->maxdelayH * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayH != count && pocketverbs->delayH % count == 0) {
						pocketverbs->delayH += 1; // try for primeish Hs
						count = 2;
					}
				}
				if (pocketverbs->delayH > pocketverbs->maxdelayH) pocketverbs->delayH = pocketverbs->maxdelayH; // insanitycheck
				for (count = pocketverbs->alpHL; count < 11129; count++) {
					pocketverbs->aHL[count] = 0.0;
				}
				for (count = pocketverbs->outHL; count < 11129; count++) {
					pocketverbs->oHL[count] = 0.0;
				}
				break;

			case 9:
				pocketverbs->delayI = ((int) pocketverbs->maxdelayI * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayI != count && pocketverbs->delayI % count == 0) {
						pocketverbs->delayI += 1; // try for primeish Is
						count = 2;
					}
				}
				if (pocketverbs->delayI > pocketverbs->maxdelayI) pocketverbs->delayI = pocketverbs->maxdelayI; // insanitycheck
				for (count = pocketverbs->alpIL; count < 10597; count++) {
					pocketverbs->aIL[count] = 0.0;
				}
				for (count = pocketverbs->outIL; count < 10597; count++) {
					pocketverbs->oIL[count] = 0.0;
				}
				break;

			case 10:
				pocketverbs->delayJ = ((int) pocketverbs->maxdelayJ * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayJ != count && pocketverbs->delayJ % count == 0) {
						pocketverbs->delayJ += 1; // try for primeish Js
						count = 2;
					}
				}
				if (pocketverbs->delayJ > pocketverbs->maxdelayJ) pocketverbs->delayJ = pocketverbs->maxdelayJ; // insanitycheck
				for (count = pocketverbs->alpJL; count < 9809; count++) {
					pocketverbs->aJL[count] = 0.0;
				}
				for (count = pocketverbs->outJL; count < 9809; count++) {
					pocketverbs->oJL[count] = 0.0;
				}
				break;

			case 11:
				pocketverbs->delayK = ((int) pocketverbs->maxdelayK * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayK != count && pocketverbs->delayK % count == 0) {
						pocketverbs->delayK += 1; // try for primeish Ks
						count = 2;
					}
				}
				if (pocketverbs->delayK > pocketverbs->maxdelayK) pocketverbs->delayK = pocketverbs->maxdelayK; // insanitycheck
				for (count = pocketverbs->alpKL; count < 9521; count++) {
					pocketverbs->aKL[count] = 0.0;
				}
				for (count = pocketverbs->outKL; count < 9521; count++) {
					pocketverbs->oKL[count] = 0.0;
				}
				break;

			case 12:
				pocketverbs->delayL = ((int) pocketverbs->maxdelayL * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayL != count && pocketverbs->delayL % count == 0) {
						pocketverbs->delayL += 1; // try for primeish Ls
						count = 2;
					}
				}
				if (pocketverbs->delayL > pocketverbs->maxdelayL) pocketverbs->delayL = pocketverbs->maxdelayL; // insanitycheck
				for (count = pocketverbs->alpLL; count < 8981; count++) {
					pocketverbs->aLL[count] = 0.0;
				}
				for (count = pocketverbs->outLL; count < 8981; count++) {
					pocketverbs->oLL[count] = 0.0;
				}
				break;

			case 13:
				pocketverbs->delayM = ((int) pocketverbs->maxdelayM * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayM != count && pocketverbs->delayM % count == 0) {
						pocketverbs->delayM += 1; // try for primeish Ms
						count = 2;
					}
				}
				if (pocketverbs->delayM > pocketverbs->maxdelayM) pocketverbs->delayM = pocketverbs->maxdelayM; // insanitycheck
				for (count = pocketverbs->alpML; count < 8785; count++) {
					pocketverbs->aML[count] = 0.0;
				}
				for (count = pocketverbs->outML; count < 8785; count++) {
					pocketverbs->oML[count] = 0.0;
				}
				break;

			case 14:
				pocketverbs->delayN = ((int) pocketverbs->maxdelayN * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayN != count && pocketverbs->delayN % count == 0) {
						pocketverbs->delayN += 1; // try for primeish Ns
						count = 2;
					}
				}
				if (pocketverbs->delayN > pocketverbs->maxdelayN) pocketverbs->delayN = pocketverbs->maxdelayN; // insanitycheck
				for (count = pocketverbs->alpNL; count < 8461; count++) {
					pocketverbs->aNL[count] = 0.0;
				}
				for (count = pocketverbs->outNL; count < 8461; count++) {
					pocketverbs->oNL[count] = 0.0;
				}
				break;

			case 15:
				pocketverbs->delayO = ((int) pocketverbs->maxdelayO * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayO != count && pocketverbs->delayO % count == 0) {
						pocketverbs->delayO += 1; // try for primeish Os
						count = 2;
					}
				}
				if (pocketverbs->delayO > pocketverbs->maxdelayO) pocketverbs->delayO = pocketverbs->maxdelayO; // insanitycheck
				for (count = pocketverbs->alpOL; count < 8309; count++) {
					pocketverbs->aOL[count] = 0.0;
				}
				for (count = pocketverbs->outOL; count < 8309; count++) {
					pocketverbs->oOL[count] = 0.0;
				}
				break;

			case 16:
				pocketverbs->delayP = ((int) pocketverbs->maxdelayP * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayP != count && pocketverbs->delayP % count == 0) {
						pocketverbs->delayP += 1; // try for primeish Ps
						count = 2;
					}
				}
				if (pocketverbs->delayP > pocketverbs->maxdelayP) pocketverbs->delayP = pocketverbs->maxdelayP; // insanitycheck
				for (count = pocketverbs->alpPL; count < 7981; count++) {
					pocketverbs->aPL[count] = 0.0;
				}
				for (count = pocketverbs->outPL; count < 7981; count++) {
					pocketverbs->oPL[count] = 0.0;
				}
				break;

			case 17:
				pocketverbs->delayQ = ((int) pocketverbs->maxdelayQ * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayQ != count && pocketverbs->delayQ % count == 0) {
						pocketverbs->delayQ += 1; // try for primeish Qs
						count = 2;
					}
				}
				if (pocketverbs->delayQ > pocketverbs->maxdelayQ) pocketverbs->delayQ = pocketverbs->maxdelayQ; // insanitycheck
				for (count = pocketverbs->alpQL; count < 7321; count++) {
					pocketverbs->aQL[count] = 0.0;
				}
				for (count = pocketverbs->outQL; count < 7321; count++) {
					pocketverbs->oQL[count] = 0.0;
				}
				break;

			case 18:
				pocketverbs->delayR = ((int) pocketverbs->maxdelayR * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayR != count && pocketverbs->delayR % count == 0) {
						pocketverbs->delayR += 1; // try for primeish Rs
						count = 2;
					}
				}
				if (pocketverbs->delayR > pocketverbs->maxdelayR) pocketverbs->delayR = pocketverbs->maxdelayR; // insanitycheck
				for (count = pocketverbs->alpRL; count < 6817; count++) {
					pocketverbs->aRL[count] = 0.0;
				}
				for (count = pocketverbs->outRL; count < 6817; count++) {
					pocketverbs->oRL[count] = 0.0;
				}
				break;

			case 19:
				pocketverbs->delayS = ((int) pocketverbs->maxdelayS * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayS != count && pocketverbs->delayS % count == 0) {
						pocketverbs->delayS += 1; // try for primeish Ss
						count = 2;
					}
				}
				if (pocketverbs->delayS > pocketverbs->maxdelayS) pocketverbs->delayS = pocketverbs->maxdelayS; // insanitycheck
				for (count = pocketverbs->alpSL; count < 6505; count++) {
					pocketverbs->aSL[count] = 0.0;
				}
				for (count = pocketverbs->outSL; count < 6505; count++) {
					pocketverbs->oSL[count] = 0.0;
				}
				break;

			case 20:
				pocketverbs->delayT = ((int) pocketverbs->maxdelayT * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayT != count && pocketverbs->delayT % count == 0) {
						pocketverbs->delayT += 1; // try for primeish Ts
						count = 2;
					}
				}
				if (pocketverbs->delayT > pocketverbs->maxdelayT) pocketverbs->delayT = pocketverbs->maxdelayT; // insanitycheck
				for (count = pocketverbs->alpTL; count < 6001; count++) {
					pocketverbs->aTL[count] = 0.0;
				}
				for (count = pocketverbs->outTL; count < 6001; count++) {
					pocketverbs->oTL[count] = 0.0;
				}
				break;

			case 21:
				pocketverbs->delayU = ((int) pocketverbs->maxdelayU * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayU != count && pocketverbs->delayU % count == 0) {
						pocketverbs->delayU += 1; // try for primeish Us
						count = 2;
					}
				}
				if (pocketverbs->delayU > pocketverbs->maxdelayU) pocketverbs->delayU = pocketverbs->maxdelayU; // insanitycheck
				for (count = pocketverbs->alpUL; count < 5837; count++) {
					pocketverbs->aUL[count] = 0.0;
				}
				for (count = pocketverbs->outUL; count < 5837; count++) {
					pocketverbs->oUL[count] = 0.0;
				}
				break;

			case 22:
				pocketverbs->delayV = ((int) pocketverbs->maxdelayV * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayV != count && pocketverbs->delayV % count == 0) {
						pocketverbs->delayV += 1; // try for primeish Vs
						count = 2;
					}
				}
				if (pocketverbs->delayV > pocketverbs->maxdelayV) pocketverbs->delayV = pocketverbs->maxdelayV; // insanitycheck
				for (count = pocketverbs->alpVL; count < 5501; count++) {
					pocketverbs->aVL[count] = 0.0;
				}
				for (count = pocketverbs->outVL; count < 5501; count++) {
					pocketverbs->oVL[count] = 0.0;
				}
				break;

			case 23:
				pocketverbs->delayW = ((int) pocketverbs->maxdelayW * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayW != count && pocketverbs->delayW % count == 0) {
						pocketverbs->delayW += 1; // try for primeish Ws
						count = 2;
					}
				}
				if (pocketverbs->delayW > pocketverbs->maxdelayW) pocketverbs->delayW = pocketverbs->maxdelayW; // insanitycheck
				for (count = pocketverbs->alpWL; count < 5009; count++) {
					pocketverbs->aWL[count] = 0.0;
				}
				for (count = pocketverbs->outWL; count < 5009; count++) {
					pocketverbs->oWL[count] = 0.0;
				}
				break;

			case 24:
				pocketverbs->delayX = ((int) pocketverbs->maxdelayX * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayX != count && pocketverbs->delayX % count == 0) {
						pocketverbs->delayX += 1; // try for primeish Xs
						count = 2;
					}
				}
				if (pocketverbs->delayX > pocketverbs->maxdelayX) pocketverbs->delayX = pocketverbs->maxdelayX; // insanitycheck
				for (count = pocketverbs->alpXL; count < 4849; count++) {
					pocketverbs->aXL[count] = 0.0;
				}
				for (count = pocketverbs->outXL; count < 4849; count++) {
					pocketverbs->oXL[count] = 0.0;
				}
				break;

			case 25:
				pocketverbs->delayY = ((int) pocketverbs->maxdelayY * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayY != count && pocketverbs->delayY % count == 0) {
						pocketverbs->delayY += 1; // try for primeish Ys
						count = 2;
					}
				}
				if (pocketverbs->delayY > pocketverbs->maxdelayY) pocketverbs->delayY = pocketverbs->maxdelayY; // insanitycheck
				for (count = pocketverbs->alpYL; count < 4295; count++) {
					pocketverbs->aYL[count] = 0.0;
				}
				for (count = pocketverbs->outYL; count < 4295; count++) {
					pocketverbs->oYL[count] = 0.0;
				}
				break;

			case 26:
				pocketverbs->delayZ = ((int) pocketverbs->maxdelayZ * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayZ != count && pocketverbs->delayZ % count == 0) {
						pocketverbs->delayZ += 1; // try for primeish Zs
						count = 2;
					}
				}
				if (pocketverbs->delayZ > pocketverbs->maxdelayZ) pocketverbs->delayZ = pocketverbs->maxdelayZ; // insanitycheck
				for (count = pocketverbs->alpZL; count < 4179; count++) {
					pocketverbs->aZL[count] = 0.0;
				}
				for (count = pocketverbs->outZL; count < 4179; count++) {
					pocketverbs->oZL[count] = 0.0;
				}
				break;
		} // end of switch statement
		// pocketverbs->countdown--; we are doing this after the second one
	}

	if (pocketverbs->countdown > 0) {
		switch (pocketverbs->countdown) {
			case 1:
				pocketverbs->delayA = ((int) pocketverbs->maxdelayA * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayA != count && pocketverbs->delayA % count == 0) {
						pocketverbs->delayA += 1; // try for primeish As
						count = 2;
					}
				}
				if (pocketverbs->delayA > pocketverbs->maxdelayA) pocketverbs->delayA = pocketverbs->maxdelayA; // insanitycheck
				for (count = pocketverbs->alpAR; count < 15149; count++) {
					pocketverbs->aAR[count] = 0.0;
				}
				for (count = pocketverbs->outAR; count < 15149; count++) {
					pocketverbs->oAR[count] = 0.0;
				}
				break;

			case 2:
				pocketverbs->delayB = ((int) pocketverbs->maxdelayB * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayB != count && pocketverbs->delayB % count == 0) {
						pocketverbs->delayB += 1; // try for primeish Bs
						count = 2;
					}
				}
				if (pocketverbs->delayB > pocketverbs->maxdelayB) pocketverbs->delayB = pocketverbs->maxdelayB; // insanitycheck
				for (count = pocketverbs->alpBR; count < 14617; count++) {
					pocketverbs->aBR[count] = 0.0;
				}
				for (count = pocketverbs->outBR; count < 14617; count++) {
					pocketverbs->oBR[count] = 0.0;
				}
				break;

			case 3:
				pocketverbs->delayC = ((int) pocketverbs->maxdelayC * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayC != count && pocketverbs->delayC % count == 0) {
						pocketverbs->delayC += 1; // try for primeish Cs
						count = 2;
					}
				}
				if (pocketverbs->delayC > pocketverbs->maxdelayC) pocketverbs->delayC = pocketverbs->maxdelayC; // insanitycheck
				for (count = pocketverbs->alpCR; count < 14357; count++) {
					pocketverbs->aCR[count] = 0.0;
				}
				for (count = pocketverbs->outCR; count < 14357; count++) {
					pocketverbs->oCR[count] = 0.0;
				}
				break;

			case 4:
				pocketverbs->delayD = ((int) pocketverbs->maxdelayD * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayD != count && pocketverbs->delayD % count == 0) {
						pocketverbs->delayD += 1; // try for primeish Ds
						count = 2;
					}
				}
				if (pocketverbs->delayD > pocketverbs->maxdelayD) pocketverbs->delayD = pocketverbs->maxdelayD; // insanitycheck
				for (count = pocketverbs->alpDR; count < 13817; count++) {
					pocketverbs->aDR[count] = 0.0;
				}
				for (count = pocketverbs->outDR; count < 13817; count++) {
					pocketverbs->oDR[count] = 0.0;
				}
				break;

			case 5:
				pocketverbs->delayE = ((int) pocketverbs->maxdelayE * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayE != count && pocketverbs->delayE % count == 0) {
						pocketverbs->delayE += 1; // try for primeish Es
						count = 2;
					}
				}
				if (pocketverbs->delayE > pocketverbs->maxdelayE) pocketverbs->delayE = pocketverbs->maxdelayE; // insanitycheck
				for (count = pocketverbs->alpER; count < 13561; count++) {
					pocketverbs->aER[count] = 0.0;
				}
				for (count = pocketverbs->outER; count < 13561; count++) {
					pocketverbs->oER[count] = 0.0;
				}
				break;

			case 6:
				pocketverbs->delayF = ((int) pocketverbs->maxdelayF * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayF != count && pocketverbs->delayF % count == 0) {
						pocketverbs->delayF += 1; // try for primeish Fs
						count = 2;
					}
				}
				if (pocketverbs->delayF > pocketverbs->maxdelayF) pocketverbs->delayF = pocketverbs->maxdelayF; // insanitycheck
				for (count = pocketverbs->alpFR; count < 13045; count++) {
					pocketverbs->aFR[count] = 0.0;
				}
				for (count = pocketverbs->outFR; count < 13045; count++) {
					pocketverbs->oFR[count] = 0.0;
				}
				break;

			case 7:
				pocketverbs->delayG = ((int) pocketverbs->maxdelayG * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayG != count && pocketverbs->delayG % count == 0) {
						pocketverbs->delayG += 1; // try for primeish Gs
						count = 2;
					}
				}
				if (pocketverbs->delayG > pocketverbs->maxdelayG) pocketverbs->delayG = pocketverbs->maxdelayG; // insanitycheck
				for (count = pocketverbs->alpGR; count < 11965; count++) {
					pocketverbs->aGR[count] = 0.0;
				}
				for (count = pocketverbs->outGR; count < 11965; count++) {
					pocketverbs->oGR[count] = 0.0;
				}
				break;

			case 8:
				pocketverbs->delayH = ((int) pocketverbs->maxdelayH * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayH != count && pocketverbs->delayH % count == 0) {
						pocketverbs->delayH += 1; // try for primeish Hs
						count = 2;
					}
				}
				if (pocketverbs->delayH > pocketverbs->maxdelayH) pocketverbs->delayH = pocketverbs->maxdelayH; // insanitycheck
				for (count = pocketverbs->alpHR; count < 11129; count++) {
					pocketverbs->aHR[count] = 0.0;
				}
				for (count = pocketverbs->outHR; count < 11129; count++) {
					pocketverbs->oHR[count] = 0.0;
				}
				break;

			case 9:
				pocketverbs->delayI = ((int) pocketverbs->maxdelayI * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayI != count && pocketverbs->delayI % count == 0) {
						pocketverbs->delayI += 1; // try for primeish Is
						count = 2;
					}
				}
				if (pocketverbs->delayI > pocketverbs->maxdelayI) pocketverbs->delayI = pocketverbs->maxdelayI; // insanitycheck
				for (count = pocketverbs->alpIR; count < 10597; count++) {
					pocketverbs->aIR[count] = 0.0;
				}
				for (count = pocketverbs->outIR; count < 10597; count++) {
					pocketverbs->oIR[count] = 0.0;
				}
				break;

			case 10:
				pocketverbs->delayJ = ((int) pocketverbs->maxdelayJ * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayJ != count && pocketverbs->delayJ % count == 0) {
						pocketverbs->delayJ += 1; // try for primeish Js
						count = 2;
					}
				}
				if (pocketverbs->delayJ > pocketverbs->maxdelayJ) pocketverbs->delayJ = pocketverbs->maxdelayJ; // insanitycheck
				for (count = pocketverbs->alpJR; count < 9809; count++) {
					pocketverbs->aJR[count] = 0.0;
				}
				for (count = pocketverbs->outJR; count < 9809; count++) {
					pocketverbs->oJR[count] = 0.0;
				}
				break;

			case 11:
				pocketverbs->delayK = ((int) pocketverbs->maxdelayK * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayK != count && pocketverbs->delayK % count == 0) {
						pocketverbs->delayK += 1; // try for primeish Ks
						count = 2;
					}
				}
				if (pocketverbs->delayK > pocketverbs->maxdelayK) pocketverbs->delayK = pocketverbs->maxdelayK; // insanitycheck
				for (count = pocketverbs->alpKR; count < 9521; count++) {
					pocketverbs->aKR[count] = 0.0;
				}
				for (count = pocketverbs->outKR; count < 9521; count++) {
					pocketverbs->oKR[count] = 0.0;
				}
				break;

			case 12:
				pocketverbs->delayL = ((int) pocketverbs->maxdelayL * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayL != count && pocketverbs->delayL % count == 0) {
						pocketverbs->delayL += 1; // try for primeish Ls
						count = 2;
					}
				}
				if (pocketverbs->delayL > pocketverbs->maxdelayL) pocketverbs->delayL = pocketverbs->maxdelayL; // insanitycheck
				for (count = pocketverbs->alpLR; count < 8981; count++) {
					pocketverbs->aLR[count] = 0.0;
				}
				for (count = pocketverbs->outLR; count < 8981; count++) {
					pocketverbs->oLR[count] = 0.0;
				}
				break;

			case 13:
				pocketverbs->delayM = ((int) pocketverbs->maxdelayM * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayM != count && pocketverbs->delayM % count == 0) {
						pocketverbs->delayM += 1; // try for primeish Ms
						count = 2;
					}
				}
				if (pocketverbs->delayM > pocketverbs->maxdelayM) pocketverbs->delayM = pocketverbs->maxdelayM; // insanitycheck
				for (count = pocketverbs->alpMR; count < 8785; count++) {
					pocketverbs->aMR[count] = 0.0;
				}
				for (count = pocketverbs->outMR; count < 8785; count++) {
					pocketverbs->oMR[count] = 0.0;
				}
				break;

			case 14:
				pocketverbs->delayN = ((int) pocketverbs->maxdelayN * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayN != count && pocketverbs->delayN % count == 0) {
						pocketverbs->delayN += 1; // try for primeish Ns
						count = 2;
					}
				}
				if (pocketverbs->delayN > pocketverbs->maxdelayN) pocketverbs->delayN = pocketverbs->maxdelayN; // insanitycheck
				for (count = pocketverbs->alpNR; count < 8461; count++) {
					pocketverbs->aNR[count] = 0.0;
				}
				for (count = pocketverbs->outNR; count < 8461; count++) {
					pocketverbs->oNR[count] = 0.0;
				}
				break;

			case 15:
				pocketverbs->delayO = ((int) pocketverbs->maxdelayO * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayO != count && pocketverbs->delayO % count == 0) {
						pocketverbs->delayO += 1; // try for primeish Os
						count = 2;
					}
				}
				if (pocketverbs->delayO > pocketverbs->maxdelayO) pocketverbs->delayO = pocketverbs->maxdelayO; // insanitycheck
				for (count = pocketverbs->alpOR; count < 8309; count++) {
					pocketverbs->aOR[count] = 0.0;
				}
				for (count = pocketverbs->outOR; count < 8309; count++) {
					pocketverbs->oOR[count] = 0.0;
				}
				break;

			case 16:
				pocketverbs->delayP = ((int) pocketverbs->maxdelayP * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayP != count && pocketverbs->delayP % count == 0) {
						pocketverbs->delayP += 1; // try for primeish Ps
						count = 2;
					}
				}
				if (pocketverbs->delayP > pocketverbs->maxdelayP) pocketverbs->delayP = pocketverbs->maxdelayP; // insanitycheck
				for (count = pocketverbs->alpPR; count < 7981; count++) {
					pocketverbs->aPR[count] = 0.0;
				}
				for (count = pocketverbs->outPR; count < 7981; count++) {
					pocketverbs->oPR[count] = 0.0;
				}
				break;

			case 17:
				pocketverbs->delayQ = ((int) pocketverbs->maxdelayQ * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayQ != count && pocketverbs->delayQ % count == 0) {
						pocketverbs->delayQ += 1; // try for primeish Qs
						count = 2;
					}
				}
				if (pocketverbs->delayQ > pocketverbs->maxdelayQ) pocketverbs->delayQ = pocketverbs->maxdelayQ; // insanitycheck
				for (count = pocketverbs->alpQR; count < 7321; count++) {
					pocketverbs->aQR[count] = 0.0;
				}
				for (count = pocketverbs->outQR; count < 7321; count++) {
					pocketverbs->oQR[count] = 0.0;
				}
				break;

			case 18:
				pocketverbs->delayR = ((int) pocketverbs->maxdelayR * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayR != count && pocketverbs->delayR % count == 0) {
						pocketverbs->delayR += 1; // try for primeish Rs
						count = 2;
					}
				}
				if (pocketverbs->delayR > pocketverbs->maxdelayR) pocketverbs->delayR = pocketverbs->maxdelayR; // insanitycheck
				for (count = pocketverbs->alpRR; count < 6817; count++) {
					pocketverbs->aRR[count] = 0.0;
				}
				for (count = pocketverbs->outRR; count < 6817; count++) {
					pocketverbs->oRR[count] = 0.0;
				}
				break;

			case 19:
				pocketverbs->delayS = ((int) pocketverbs->maxdelayS * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayS != count && pocketverbs->delayS % count == 0) {
						pocketverbs->delayS += 1; // try for primeish Ss
						count = 2;
					}
				}
				if (pocketverbs->delayS > pocketverbs->maxdelayS) pocketverbs->delayS = pocketverbs->maxdelayS; // insanitycheck
				for (count = pocketverbs->alpSR; count < 6505; count++) {
					pocketverbs->aSR[count] = 0.0;
				}
				for (count = pocketverbs->outSR; count < 6505; count++) {
					pocketverbs->oSR[count] = 0.0;
				}
				break;

			case 20:
				pocketverbs->delayT = ((int) pocketverbs->maxdelayT * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayT != count && pocketverbs->delayT % count == 0) {
						pocketverbs->delayT += 1; // try for primeish Ts
						count = 2;
					}
				}
				if (pocketverbs->delayT > pocketverbs->maxdelayT) pocketverbs->delayT = pocketverbs->maxdelayT; // insanitycheck
				for (count = pocketverbs->alpTR; count < 6001; count++) {
					pocketverbs->aTR[count] = 0.0;
				}
				for (count = pocketverbs->outTR; count < 6001; count++) {
					pocketverbs->oTR[count] = 0.0;
				}
				break;

			case 21:
				pocketverbs->delayU = ((int) pocketverbs->maxdelayU * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayU != count && pocketverbs->delayU % count == 0) {
						pocketverbs->delayU += 1; // try for primeish Us
						count = 2;
					}
				}
				if (pocketverbs->delayU > pocketverbs->maxdelayU) pocketverbs->delayU = pocketverbs->maxdelayU; // insanitycheck
				for (count = pocketverbs->alpUR; count < 5837; count++) {
					pocketverbs->aUR[count] = 0.0;
				}
				for (count = pocketverbs->outUR; count < 5837; count++) {
					pocketverbs->oUR[count] = 0.0;
				}
				break;

			case 22:
				pocketverbs->delayV = ((int) pocketverbs->maxdelayV * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayV != count && pocketverbs->delayV % count == 0) {
						pocketverbs->delayV += 1; // try for primeish Vs
						count = 2;
					}
				}
				if (pocketverbs->delayV > pocketverbs->maxdelayV) pocketverbs->delayV = pocketverbs->maxdelayV; // insanitycheck
				for (count = pocketverbs->alpVR; count < 5501; count++) {
					pocketverbs->aVR[count] = 0.0;
				}
				for (count = pocketverbs->outVR; count < 5501; count++) {
					pocketverbs->oVR[count] = 0.0;
				}
				break;

			case 23:
				pocketverbs->delayW = ((int) pocketverbs->maxdelayW * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayW != count && pocketverbs->delayW % count == 0) {
						pocketverbs->delayW += 1; // try for primeish Ws
						count = 2;
					}
				}
				if (pocketverbs->delayW > pocketverbs->maxdelayW) pocketverbs->delayW = pocketverbs->maxdelayW; // insanitycheck
				for (count = pocketverbs->alpWR; count < 5009; count++) {
					pocketverbs->aWR[count] = 0.0;
				}
				for (count = pocketverbs->outWR; count < 5009; count++) {
					pocketverbs->oWR[count] = 0.0;
				}
				break;

			case 24:
				pocketverbs->delayX = ((int) pocketverbs->maxdelayX * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayX != count && pocketverbs->delayX % count == 0) {
						pocketverbs->delayX += 1; // try for primeish Xs
						count = 2;
					}
				}
				if (pocketverbs->delayX > pocketverbs->maxdelayX) pocketverbs->delayX = pocketverbs->maxdelayX; // insanitycheck
				for (count = pocketverbs->alpXR; count < 4849; count++) {
					pocketverbs->aXR[count] = 0.0;
				}
				for (count = pocketverbs->outXR; count < 4849; count++) {
					pocketverbs->oXR[count] = 0.0;
				}
				break;

			case 25:
				pocketverbs->delayY = ((int) pocketverbs->maxdelayY * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayY != count && pocketverbs->delayY % count == 0) {
						pocketverbs->delayY += 1; // try for primeish Ys
						count = 2;
					}
				}
				if (pocketverbs->delayY > pocketverbs->maxdelayY) pocketverbs->delayY = pocketverbs->maxdelayY; // insanitycheck
				for (count = pocketverbs->alpYR; count < 4295; count++) {
					pocketverbs->aYR[count] = 0.0;
				}
				for (count = pocketverbs->outYR; count < 4295; count++) {
					pocketverbs->oYR[count] = 0.0;
				}
				break;

			case 26:
				pocketverbs->delayZ = ((int) pocketverbs->maxdelayZ * roomsize);
				for (count = 2; count <= max; count++) {
					if (pocketverbs->delayZ != count && pocketverbs->delayZ % count == 0) {
						pocketverbs->delayZ += 1; // try for primeish Zs
						count = 2;
					}
				}
				if (pocketverbs->delayZ > pocketverbs->maxdelayZ) pocketverbs->delayZ = pocketverbs->maxdelayZ; // insanitycheck
				for (count = pocketverbs->alpZR; count < 4179; count++) {
					pocketverbs->aZR[count] = 0.0;
				}
				for (count = pocketverbs->outZR; count < 4179; count++) {
					pocketverbs->oZR[count] = 0.0;
				}
				break;
		} // end of switch statement
		pocketverbs->countdown--; // every buffer we'll do one of the recalculations for prime buffer sizes
	}

	while (sampleFrames-- > 0) {
		double inputSampleL = *in1;
		double inputSampleR = *in2;

		if (fabs(inputSampleL) < 1.18e-23) inputSampleL = pocketverbs->fpdL * 1.18e-17;
		if (fabs(inputSampleR) < 1.18e-23) inputSampleR = pocketverbs->fpdR * 1.18e-17;

		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;

		pocketverbs->peakL -= release;
		if (pocketverbs->peakL < fabs(inputSampleL * 2.0)) pocketverbs->peakL = fabs(inputSampleL * 2.0);
		if (pocketverbs->peakL > 1.0) pocketverbs->peakL = 1.0;
		pocketverbs->peakR -= release;
		if (pocketverbs->peakR < fabs(inputSampleR * 2.0)) pocketverbs->peakR = fabs(inputSampleR * 2.0);
		if (pocketverbs->peakR > 1.0) pocketverbs->peakR = 1.0;
		// chase the maximum signal to incorporate the wetter/louder behavior
		// boost for more extreme effect when in use, cap it

		inputSampleL *= gain;
		bridgerectifier = fabs(inputSampleL);
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0) inputSampleL = bridgerectifier;
		else inputSampleL = -bridgerectifier;
		inputSampleR *= gain;
		bridgerectifier = fabs(inputSampleR);
		bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0) inputSampleR = bridgerectifier;
		else inputSampleR = -bridgerectifier;
		// here we apply the ADT2 console-on-steroids trick

		switch (verbtype) {

			case 1: // Chamber
				allpasstemp = pocketverbs->alpAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->aAL[allpasstemp] * constallpass;
				pocketverbs->aAL[pocketverbs->alpAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpAL--;
				if (pocketverbs->alpAL < 0 || pocketverbs->alpAL > pocketverbs->delayA) {
					pocketverbs->alpAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->aAL[pocketverbs->alpAL]);
				// allpass filter A

				pocketverbs->dAL[3] = pocketverbs->dAL[2];
				pocketverbs->dAL[2] = pocketverbs->dAL[1];
				pocketverbs->dAL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dAL[2] + pocketverbs->dAL[3]) / 3.0;

				allpasstemp = pocketverbs->alpBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->aBL[allpasstemp] * constallpass;
				pocketverbs->aBL[pocketverbs->alpBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpBL--;
				if (pocketverbs->alpBL < 0 || pocketverbs->alpBL > pocketverbs->delayB) {
					pocketverbs->alpBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->aBL[pocketverbs->alpBL]);
				// allpass filter B

				pocketverbs->dBL[3] = pocketverbs->dBL[2];
				pocketverbs->dBL[2] = pocketverbs->dBL[1];
				pocketverbs->dBL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dBL[2] + pocketverbs->dBL[3]) / 3.0;

				allpasstemp = pocketverbs->alpCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->aCL[allpasstemp] * constallpass;
				pocketverbs->aCL[pocketverbs->alpCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpCL--;
				if (pocketverbs->alpCL < 0 || pocketverbs->alpCL > pocketverbs->delayC) {
					pocketverbs->alpCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->aCL[pocketverbs->alpCL]);
				// allpass filter C

				pocketverbs->dCL[3] = pocketverbs->dCL[2];
				pocketverbs->dCL[2] = pocketverbs->dCL[1];
				pocketverbs->dCL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dCL[2] + pocketverbs->dCL[3]) / 3.0;

				allpasstemp = pocketverbs->alpDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->aDL[allpasstemp] * constallpass;
				pocketverbs->aDL[pocketverbs->alpDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpDL--;
				if (pocketverbs->alpDL < 0 || pocketverbs->alpDL > pocketverbs->delayD) {
					pocketverbs->alpDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->aDL[pocketverbs->alpDL]);
				// allpass filter D

				pocketverbs->dDL[3] = pocketverbs->dDL[2];
				pocketverbs->dDL[2] = pocketverbs->dDL[1];
				pocketverbs->dDL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dDL[2] + pocketverbs->dDL[3]) / 3.0;

				allpasstemp = pocketverbs->alpEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->aEL[allpasstemp] * constallpass;
				pocketverbs->aEL[pocketverbs->alpEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpEL--;
				if (pocketverbs->alpEL < 0 || pocketverbs->alpEL > pocketverbs->delayE) {
					pocketverbs->alpEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->aEL[pocketverbs->alpEL]);
				// allpass filter E

				pocketverbs->dEL[3] = pocketverbs->dEL[2];
				pocketverbs->dEL[2] = pocketverbs->dEL[1];
				pocketverbs->dEL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dEL[2] + pocketverbs->dEL[3]) / 3.0;

				allpasstemp = pocketverbs->alpFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->aFL[allpasstemp] * constallpass;
				pocketverbs->aFL[pocketverbs->alpFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpFL--;
				if (pocketverbs->alpFL < 0 || pocketverbs->alpFL > pocketverbs->delayF) {
					pocketverbs->alpFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->aFL[pocketverbs->alpFL]);
				// allpass filter F

				pocketverbs->dFL[3] = pocketverbs->dFL[2];
				pocketverbs->dFL[2] = pocketverbs->dFL[1];
				pocketverbs->dFL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dFL[2] + pocketverbs->dFL[3]) / 3.0;

				allpasstemp = pocketverbs->alpGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->aGL[allpasstemp] * constallpass;
				pocketverbs->aGL[pocketverbs->alpGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpGL--;
				if (pocketverbs->alpGL < 0 || pocketverbs->alpGL > pocketverbs->delayG) {
					pocketverbs->alpGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->aGL[pocketverbs->alpGL]);
				// allpass filter G

				pocketverbs->dGL[3] = pocketverbs->dGL[2];
				pocketverbs->dGL[2] = pocketverbs->dGL[1];
				pocketverbs->dGL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dGL[2] + pocketverbs->dGL[3]) / 3.0;

				allpasstemp = pocketverbs->alpHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->aHL[allpasstemp] * constallpass;
				pocketverbs->aHL[pocketverbs->alpHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpHL--;
				if (pocketverbs->alpHL < 0 || pocketverbs->alpHL > pocketverbs->delayH) {
					pocketverbs->alpHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->aHL[pocketverbs->alpHL]);
				// allpass filter H

				pocketverbs->dHL[3] = pocketverbs->dHL[2];
				pocketverbs->dHL[2] = pocketverbs->dHL[1];
				pocketverbs->dHL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dHL[2] + pocketverbs->dHL[3]) / 3.0;

				allpasstemp = pocketverbs->alpIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->aIL[allpasstemp] * constallpass;
				pocketverbs->aIL[pocketverbs->alpIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpIL--;
				if (pocketverbs->alpIL < 0 || pocketverbs->alpIL > pocketverbs->delayI) {
					pocketverbs->alpIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->aIL[pocketverbs->alpIL]);
				// allpass filter I

				pocketverbs->dIL[3] = pocketverbs->dIL[2];
				pocketverbs->dIL[2] = pocketverbs->dIL[1];
				pocketverbs->dIL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dIL[2] + pocketverbs->dIL[3]) / 3.0;

				allpasstemp = pocketverbs->alpJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->aJL[allpasstemp] * constallpass;
				pocketverbs->aJL[pocketverbs->alpJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpJL--;
				if (pocketverbs->alpJL < 0 || pocketverbs->alpJL > pocketverbs->delayJ) {
					pocketverbs->alpJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->aJL[pocketverbs->alpJL]);
				// allpass filter J

				pocketverbs->dJL[3] = pocketverbs->dJL[2];
				pocketverbs->dJL[2] = pocketverbs->dJL[1];
				pocketverbs->dJL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dJL[2] + pocketverbs->dJL[3]) / 3.0;

				allpasstemp = pocketverbs->alpKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->aKL[allpasstemp] * constallpass;
				pocketverbs->aKL[pocketverbs->alpKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpKL--;
				if (pocketverbs->alpKL < 0 || pocketverbs->alpKL > pocketverbs->delayK) {
					pocketverbs->alpKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->aKL[pocketverbs->alpKL]);
				// allpass filter K

				pocketverbs->dKL[3] = pocketverbs->dKL[2];
				pocketverbs->dKL[2] = pocketverbs->dKL[1];
				pocketverbs->dKL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dKL[2] + pocketverbs->dKL[3]) / 3.0;

				allpasstemp = pocketverbs->alpLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->aLL[allpasstemp] * constallpass;
				pocketverbs->aLL[pocketverbs->alpLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpLL--;
				if (pocketverbs->alpLL < 0 || pocketverbs->alpLL > pocketverbs->delayL) {
					pocketverbs->alpLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->aLL[pocketverbs->alpLL]);
				// allpass filter L

				pocketverbs->dLL[3] = pocketverbs->dLL[2];
				pocketverbs->dLL[2] = pocketverbs->dLL[1];
				pocketverbs->dLL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dLL[2] + pocketverbs->dLL[3]) / 3.0;

				allpasstemp = pocketverbs->alpML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->aML[allpasstemp] * constallpass;
				pocketverbs->aML[pocketverbs->alpML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpML--;
				if (pocketverbs->alpML < 0 || pocketverbs->alpML > pocketverbs->delayM) {
					pocketverbs->alpML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->aML[pocketverbs->alpML]);
				// allpass filter M

				pocketverbs->dML[3] = pocketverbs->dML[2];
				pocketverbs->dML[2] = pocketverbs->dML[1];
				pocketverbs->dML[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dML[2] + pocketverbs->dML[3]) / 3.0;

				allpasstemp = pocketverbs->alpNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->aNL[allpasstemp] * constallpass;
				pocketverbs->aNL[pocketverbs->alpNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpNL--;
				if (pocketverbs->alpNL < 0 || pocketverbs->alpNL > pocketverbs->delayN) {
					pocketverbs->alpNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->aNL[pocketverbs->alpNL]);
				// allpass filter N

				pocketverbs->dNL[3] = pocketverbs->dNL[2];
				pocketverbs->dNL[2] = pocketverbs->dNL[1];
				pocketverbs->dNL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dNL[2] + pocketverbs->dNL[3]) / 3.0;

				allpasstemp = pocketverbs->alpOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->aOL[allpasstemp] * constallpass;
				pocketverbs->aOL[pocketverbs->alpOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpOL--;
				if (pocketverbs->alpOL < 0 || pocketverbs->alpOL > pocketverbs->delayO) {
					pocketverbs->alpOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->aOL[pocketverbs->alpOL]);
				// allpass filter O

				pocketverbs->dOL[3] = pocketverbs->dOL[2];
				pocketverbs->dOL[2] = pocketverbs->dOL[1];
				pocketverbs->dOL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dOL[2] + pocketverbs->dOL[3]) / 3.0;

				allpasstemp = pocketverbs->alpPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->aPL[allpasstemp] * constallpass;
				pocketverbs->aPL[pocketverbs->alpPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpPL--;
				if (pocketverbs->alpPL < 0 || pocketverbs->alpPL > pocketverbs->delayP) {
					pocketverbs->alpPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->aPL[pocketverbs->alpPL]);
				// allpass filter P

				pocketverbs->dPL[3] = pocketverbs->dPL[2];
				pocketverbs->dPL[2] = pocketverbs->dPL[1];
				pocketverbs->dPL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dPL[2] + pocketverbs->dPL[3]) / 3.0;

				allpasstemp = pocketverbs->alpQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->aQL[allpasstemp] * constallpass;
				pocketverbs->aQL[pocketverbs->alpQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpQL--;
				if (pocketverbs->alpQL < 0 || pocketverbs->alpQL > pocketverbs->delayQ) {
					pocketverbs->alpQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->aQL[pocketverbs->alpQL]);
				// allpass filter Q

				pocketverbs->dQL[3] = pocketverbs->dQL[2];
				pocketverbs->dQL[2] = pocketverbs->dQL[1];
				pocketverbs->dQL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dQL[2] + pocketverbs->dQL[3]) / 3.0;

				allpasstemp = pocketverbs->alpRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->aRL[allpasstemp] * constallpass;
				pocketverbs->aRL[pocketverbs->alpRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpRL--;
				if (pocketverbs->alpRL < 0 || pocketverbs->alpRL > pocketverbs->delayR) {
					pocketverbs->alpRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->aRL[pocketverbs->alpRL]);
				// allpass filter R

				pocketverbs->dRL[3] = pocketverbs->dRL[2];
				pocketverbs->dRL[2] = pocketverbs->dRL[1];
				pocketverbs->dRL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dRL[2] + pocketverbs->dRL[3]) / 3.0;

				allpasstemp = pocketverbs->alpSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->aSL[allpasstemp] * constallpass;
				pocketverbs->aSL[pocketverbs->alpSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpSL--;
				if (pocketverbs->alpSL < 0 || pocketverbs->alpSL > pocketverbs->delayS) {
					pocketverbs->alpSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->aSL[pocketverbs->alpSL]);
				// allpass filter S

				pocketverbs->dSL[3] = pocketverbs->dSL[2];
				pocketverbs->dSL[2] = pocketverbs->dSL[1];
				pocketverbs->dSL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dSL[2] + pocketverbs->dSL[3]) / 3.0;

				allpasstemp = pocketverbs->alpTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->aTL[allpasstemp] * constallpass;
				pocketverbs->aTL[pocketverbs->alpTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpTL--;
				if (pocketverbs->alpTL < 0 || pocketverbs->alpTL > pocketverbs->delayT) {
					pocketverbs->alpTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->aTL[pocketverbs->alpTL]);
				// allpass filter T

				pocketverbs->dTL[3] = pocketverbs->dTL[2];
				pocketverbs->dTL[2] = pocketverbs->dTL[1];
				pocketverbs->dTL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dTL[2] + pocketverbs->dTL[3]) / 3.0;

				allpasstemp = pocketverbs->alpUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->aUL[allpasstemp] * constallpass;
				pocketverbs->aUL[pocketverbs->alpUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpUL--;
				if (pocketverbs->alpUL < 0 || pocketverbs->alpUL > pocketverbs->delayU) {
					pocketverbs->alpUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->aUL[pocketverbs->alpUL]);
				// allpass filter U

				pocketverbs->dUL[3] = pocketverbs->dUL[2];
				pocketverbs->dUL[2] = pocketverbs->dUL[1];
				pocketverbs->dUL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dUL[2] + pocketverbs->dUL[3]) / 3.0;

				allpasstemp = pocketverbs->alpVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->aVL[allpasstemp] * constallpass;
				pocketverbs->aVL[pocketverbs->alpVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpVL--;
				if (pocketverbs->alpVL < 0 || pocketverbs->alpVL > pocketverbs->delayV) {
					pocketverbs->alpVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->aVL[pocketverbs->alpVL]);
				// allpass filter V

				pocketverbs->dVL[3] = pocketverbs->dVL[2];
				pocketverbs->dVL[2] = pocketverbs->dVL[1];
				pocketverbs->dVL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dVL[2] + pocketverbs->dVL[3]) / 3.0;

				allpasstemp = pocketverbs->alpWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->aWL[allpasstemp] * constallpass;
				pocketverbs->aWL[pocketverbs->alpWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpWL--;
				if (pocketverbs->alpWL < 0 || pocketverbs->alpWL > pocketverbs->delayW) {
					pocketverbs->alpWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->aWL[pocketverbs->alpWL]);
				// allpass filter W

				pocketverbs->dWL[3] = pocketverbs->dWL[2];
				pocketverbs->dWL[2] = pocketverbs->dWL[1];
				pocketverbs->dWL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dWL[2] + pocketverbs->dWL[3]) / 3.0;

				allpasstemp = pocketverbs->alpXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->aXL[allpasstemp] * constallpass;
				pocketverbs->aXL[pocketverbs->alpXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpXL--;
				if (pocketverbs->alpXL < 0 || pocketverbs->alpXL > pocketverbs->delayX) {
					pocketverbs->alpXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->aXL[pocketverbs->alpXL]);
				// allpass filter X

				pocketverbs->dXL[3] = pocketverbs->dXL[2];
				pocketverbs->dXL[2] = pocketverbs->dXL[1];
				pocketverbs->dXL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dXL[2] + pocketverbs->dXL[3]) / 3.0;

				allpasstemp = pocketverbs->alpYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->aYL[allpasstemp] * constallpass;
				pocketverbs->aYL[pocketverbs->alpYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpYL--;
				if (pocketverbs->alpYL < 0 || pocketverbs->alpYL > pocketverbs->delayY) {
					pocketverbs->alpYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->aYL[pocketverbs->alpYL]);
				// allpass filter Y

				pocketverbs->dYL[3] = pocketverbs->dYL[2];
				pocketverbs->dYL[2] = pocketverbs->dYL[1];
				pocketverbs->dYL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dYL[2] + pocketverbs->dYL[3]) / 3.0;

				allpasstemp = pocketverbs->alpZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->aZL[allpasstemp] * constallpass;
				pocketverbs->aZL[pocketverbs->alpZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpZL--;
				if (pocketverbs->alpZL < 0 || pocketverbs->alpZL > pocketverbs->delayZ) {
					pocketverbs->alpZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->aZL[pocketverbs->alpZL]);
				// allpass filter Z

				pocketverbs->dZL[3] = pocketverbs->dZL[2];
				pocketverbs->dZL[2] = pocketverbs->dZL[1];
				pocketverbs->dZL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[1] + pocketverbs->dZL[2] + pocketverbs->dZL[3]) / 3.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->oAL[allpasstemp] * constallpass;
				pocketverbs->oAL[pocketverbs->outAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outAL--;
				if (pocketverbs->outAL < 0 || pocketverbs->outAL > pocketverbs->delayA) {
					pocketverbs->outAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->oAL[pocketverbs->outAL]);
				// allpass filter A

				pocketverbs->dAL[6] = pocketverbs->dAL[5];
				pocketverbs->dAL[5] = pocketverbs->dAL[4];
				pocketverbs->dAL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[4] + pocketverbs->dAL[5] + pocketverbs->dAL[6]) / 3.0;

				allpasstemp = pocketverbs->outBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->oBL[allpasstemp] * constallpass;
				pocketverbs->oBL[pocketverbs->outBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outBL--;
				if (pocketverbs->outBL < 0 || pocketverbs->outBL > pocketverbs->delayB) {
					pocketverbs->outBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->oBL[pocketverbs->outBL]);
				// allpass filter B

				pocketverbs->dBL[6] = pocketverbs->dBL[5];
				pocketverbs->dBL[5] = pocketverbs->dBL[4];
				pocketverbs->dBL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[4] + pocketverbs->dBL[5] + pocketverbs->dBL[6]) / 3.0;

				allpasstemp = pocketverbs->outCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->oCL[allpasstemp] * constallpass;
				pocketverbs->oCL[pocketverbs->outCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outCL--;
				if (pocketverbs->outCL < 0 || pocketverbs->outCL > pocketverbs->delayC) {
					pocketverbs->outCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->oCL[pocketverbs->outCL]);
				// allpass filter C

				pocketverbs->dCL[6] = pocketverbs->dCL[5];
				pocketverbs->dCL[5] = pocketverbs->dCL[4];
				pocketverbs->dCL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dCL[5] + pocketverbs->dCL[6]) / 3.0;

				allpasstemp = pocketverbs->outDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->oDL[allpasstemp] * constallpass;
				pocketverbs->oDL[pocketverbs->outDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outDL--;
				if (pocketverbs->outDL < 0 || pocketverbs->outDL > pocketverbs->delayD) {
					pocketverbs->outDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->oDL[pocketverbs->outDL]);
				// allpass filter D

				pocketverbs->dDL[6] = pocketverbs->dDL[5];
				pocketverbs->dDL[5] = pocketverbs->dDL[4];
				pocketverbs->dDL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[4] + pocketverbs->dDL[5] + pocketverbs->dDL[6]) / 3.0;

				allpasstemp = pocketverbs->outEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->oEL[allpasstemp] * constallpass;
				pocketverbs->oEL[pocketverbs->outEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outEL--;
				if (pocketverbs->outEL < 0 || pocketverbs->outEL > pocketverbs->delayE) {
					pocketverbs->outEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->oEL[pocketverbs->outEL]);
				// allpass filter E

				pocketverbs->dEL[6] = pocketverbs->dEL[5];
				pocketverbs->dEL[5] = pocketverbs->dEL[4];
				pocketverbs->dEL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dEL[5] + pocketverbs->dEL[6]) / 3.0;

				allpasstemp = pocketverbs->outFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->oFL[allpasstemp] * constallpass;
				pocketverbs->oFL[pocketverbs->outFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outFL--;
				if (pocketverbs->outFL < 0 || pocketverbs->outFL > pocketverbs->delayF) {
					pocketverbs->outFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->oFL[pocketverbs->outFL]);
				// allpass filter F

				pocketverbs->dFL[6] = pocketverbs->dFL[5];
				pocketverbs->dFL[5] = pocketverbs->dFL[4];
				pocketverbs->dFL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[4] + pocketverbs->dFL[5] + pocketverbs->dFL[6]) / 3.0;

				allpasstemp = pocketverbs->outGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->oGL[allpasstemp] * constallpass;
				pocketverbs->oGL[pocketverbs->outGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outGL--;
				if (pocketverbs->outGL < 0 || pocketverbs->outGL > pocketverbs->delayG) {
					pocketverbs->outGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->oGL[pocketverbs->outGL]);
				// allpass filter G

				pocketverbs->dGL[6] = pocketverbs->dGL[5];
				pocketverbs->dGL[5] = pocketverbs->dGL[4];
				pocketverbs->dGL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[4] + pocketverbs->dGL[5] + pocketverbs->dGL[6]) / 3.0;

				allpasstemp = pocketverbs->outHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->oHL[allpasstemp] * constallpass;
				pocketverbs->oHL[pocketverbs->outHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outHL--;
				if (pocketverbs->outHL < 0 || pocketverbs->outHL > pocketverbs->delayH) {
					pocketverbs->outHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->oHL[pocketverbs->outHL]);
				// allpass filter H

				pocketverbs->dHL[6] = pocketverbs->dHL[5];
				pocketverbs->dHL[5] = pocketverbs->dHL[4];
				pocketverbs->dHL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[4] + pocketverbs->dHL[5] + pocketverbs->dHL[6]) / 3.0;

				allpasstemp = pocketverbs->outIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->oIL[allpasstemp] * constallpass;
				pocketverbs->oIL[pocketverbs->outIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outIL--;
				if (pocketverbs->outIL < 0 || pocketverbs->outIL > pocketverbs->delayI) {
					pocketverbs->outIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->oIL[pocketverbs->outIL]);
				// allpass filter I

				pocketverbs->dIL[6] = pocketverbs->dIL[5];
				pocketverbs->dIL[5] = pocketverbs->dIL[4];
				pocketverbs->dIL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[4] + pocketverbs->dIL[5] + pocketverbs->dIL[6]) / 3.0;

				allpasstemp = pocketverbs->outJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->oJL[allpasstemp] * constallpass;
				pocketverbs->oJL[pocketverbs->outJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outJL--;
				if (pocketverbs->outJL < 0 || pocketverbs->outJL > pocketverbs->delayJ) {
					pocketverbs->outJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->oJL[pocketverbs->outJL]);
				// allpass filter J

				pocketverbs->dJL[6] = pocketverbs->dJL[5];
				pocketverbs->dJL[5] = pocketverbs->dJL[4];
				pocketverbs->dJL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[4] + pocketverbs->dJL[5] + pocketverbs->dJL[6]) / 3.0;

				allpasstemp = pocketverbs->outKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->oKL[allpasstemp] * constallpass;
				pocketverbs->oKL[pocketverbs->outKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outKL--;
				if (pocketverbs->outKL < 0 || pocketverbs->outKL > pocketverbs->delayK) {
					pocketverbs->outKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->oKL[pocketverbs->outKL]);
				// allpass filter K

				pocketverbs->dKL[6] = pocketverbs->dKL[5];
				pocketverbs->dKL[5] = pocketverbs->dKL[4];
				pocketverbs->dKL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dKL[5] + pocketverbs->dKL[6]) / 3.0;

				allpasstemp = pocketverbs->outLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->oLL[allpasstemp] * constallpass;
				pocketverbs->oLL[pocketverbs->outLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outLL--;
				if (pocketverbs->outLL < 0 || pocketverbs->outLL > pocketverbs->delayL) {
					pocketverbs->outLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->oLL[pocketverbs->outLL]);
				// allpass filter L

				pocketverbs->dLL[6] = pocketverbs->dLL[5];
				pocketverbs->dLL[5] = pocketverbs->dLL[4];
				pocketverbs->dLL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[4] + pocketverbs->dLL[5] + pocketverbs->dLL[6]) / 3.0;

				allpasstemp = pocketverbs->outML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->oML[allpasstemp] * constallpass;
				pocketverbs->oML[pocketverbs->outML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outML--;
				if (pocketverbs->outML < 0 || pocketverbs->outML > pocketverbs->delayM) {
					pocketverbs->outML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->oML[pocketverbs->outML]);
				// allpass filter M

				pocketverbs->dML[6] = pocketverbs->dML[5];
				pocketverbs->dML[5] = pocketverbs->dML[4];
				pocketverbs->dML[4] = inputSampleL;
				inputSampleL = (pocketverbs->dML[4] + pocketverbs->dML[5] + pocketverbs->dML[6]) / 3.0;

				allpasstemp = pocketverbs->outNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->oNL[allpasstemp] * constallpass;
				pocketverbs->oNL[pocketverbs->outNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outNL--;
				if (pocketverbs->outNL < 0 || pocketverbs->outNL > pocketverbs->delayN) {
					pocketverbs->outNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->oNL[pocketverbs->outNL]);
				// allpass filter N

				pocketverbs->dNL[6] = pocketverbs->dNL[5];
				pocketverbs->dNL[5] = pocketverbs->dNL[4];
				pocketverbs->dNL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[4] + pocketverbs->dNL[5] + pocketverbs->dNL[6]) / 3.0;

				allpasstemp = pocketverbs->outOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->oOL[allpasstemp] * constallpass;
				pocketverbs->oOL[pocketverbs->outOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outOL--;
				if (pocketverbs->outOL < 0 || pocketverbs->outOL > pocketverbs->delayO) {
					pocketverbs->outOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->oOL[pocketverbs->outOL]);
				// allpass filter O

				pocketverbs->dOL[6] = pocketverbs->dOL[5];
				pocketverbs->dOL[5] = pocketverbs->dOL[4];
				pocketverbs->dOL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dOL[5] + pocketverbs->dOL[6]) / 3.0;

				allpasstemp = pocketverbs->outPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->oPL[allpasstemp] * constallpass;
				pocketverbs->oPL[pocketverbs->outPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outPL--;
				if (pocketverbs->outPL < 0 || pocketverbs->outPL > pocketverbs->delayP) {
					pocketverbs->outPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->oPL[pocketverbs->outPL]);
				// allpass filter P

				pocketverbs->dPL[6] = pocketverbs->dPL[5];
				pocketverbs->dPL[5] = pocketverbs->dPL[4];
				pocketverbs->dPL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[4] + pocketverbs->dPL[5] + pocketverbs->dPL[6]) / 3.0;

				allpasstemp = pocketverbs->outQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->oQL[allpasstemp] * constallpass;
				pocketverbs->oQL[pocketverbs->outQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outQL--;
				if (pocketverbs->outQL < 0 || pocketverbs->outQL > pocketverbs->delayQ) {
					pocketverbs->outQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->oQL[pocketverbs->outQL]);
				// allpass filter Q

				pocketverbs->dQL[6] = pocketverbs->dQL[5];
				pocketverbs->dQL[5] = pocketverbs->dQL[4];
				pocketverbs->dQL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dQL[5] + pocketverbs->dQL[6]) / 3.0;

				allpasstemp = pocketverbs->outRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->oRL[allpasstemp] * constallpass;
				pocketverbs->oRL[pocketverbs->outRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outRL--;
				if (pocketverbs->outRL < 0 || pocketverbs->outRL > pocketverbs->delayR) {
					pocketverbs->outRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->oRL[pocketverbs->outRL]);
				// allpass filter R

				pocketverbs->dRL[6] = pocketverbs->dRL[5];
				pocketverbs->dRL[5] = pocketverbs->dRL[4];
				pocketverbs->dRL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[4] + pocketverbs->dRL[5] + pocketverbs->dRL[6]) / 3.0;

				allpasstemp = pocketverbs->outSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->oSL[allpasstemp] * constallpass;
				pocketverbs->oSL[pocketverbs->outSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outSL--;
				if (pocketverbs->outSL < 0 || pocketverbs->outSL > pocketverbs->delayS) {
					pocketverbs->outSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->oSL[pocketverbs->outSL]);
				// allpass filter S

				pocketverbs->dSL[6] = pocketverbs->dSL[5];
				pocketverbs->dSL[5] = pocketverbs->dSL[4];
				pocketverbs->dSL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[4] + pocketverbs->dSL[5] + pocketverbs->dSL[6]) / 3.0;

				allpasstemp = pocketverbs->outTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->oTL[allpasstemp] * constallpass;
				pocketverbs->oTL[pocketverbs->outTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outTL--;
				if (pocketverbs->outTL < 0 || pocketverbs->outTL > pocketverbs->delayT) {
					pocketverbs->outTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->oTL[pocketverbs->outTL]);
				// allpass filter T

				pocketverbs->dTL[6] = pocketverbs->dTL[5];
				pocketverbs->dTL[5] = pocketverbs->dTL[4];
				pocketverbs->dTL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[4] + pocketverbs->dTL[5] + pocketverbs->dTL[6]) / 3.0;

				allpasstemp = pocketverbs->outUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->oUL[allpasstemp] * constallpass;
				pocketverbs->oUL[pocketverbs->outUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outUL--;
				if (pocketverbs->outUL < 0 || pocketverbs->outUL > pocketverbs->delayU) {
					pocketverbs->outUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->oUL[pocketverbs->outUL]);
				// allpass filter U

				pocketverbs->dUL[6] = pocketverbs->dUL[5];
				pocketverbs->dUL[5] = pocketverbs->dUL[4];
				pocketverbs->dUL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dUL[5] + pocketverbs->dUL[6]) / 3.0;

				allpasstemp = pocketverbs->outVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->oVL[allpasstemp] * constallpass;
				pocketverbs->oVL[pocketverbs->outVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outVL--;
				if (pocketverbs->outVL < 0 || pocketverbs->outVL > pocketverbs->delayV) {
					pocketverbs->outVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->oVL[pocketverbs->outVL]);
				// allpass filter V

				pocketverbs->dVL[6] = pocketverbs->dVL[5];
				pocketverbs->dVL[5] = pocketverbs->dVL[4];
				pocketverbs->dVL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[4] + pocketverbs->dVL[5] + pocketverbs->dVL[6]) / 3.0;

				allpasstemp = pocketverbs->outWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->oWL[allpasstemp] * constallpass;
				pocketverbs->oWL[pocketverbs->outWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outWL--;
				if (pocketverbs->outWL < 0 || pocketverbs->outWL > pocketverbs->delayW) {
					pocketverbs->outWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->oWL[pocketverbs->outWL]);
				// allpass filter W

				pocketverbs->dWL[6] = pocketverbs->dWL[5];
				pocketverbs->dWL[5] = pocketverbs->dWL[4];
				pocketverbs->dWL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[4] + pocketverbs->dWL[5] + pocketverbs->dWL[6]) / 3.0;

				allpasstemp = pocketverbs->outXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->oXL[allpasstemp] * constallpass;
				pocketverbs->oXL[pocketverbs->outXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outXL--;
				if (pocketverbs->outXL < 0 || pocketverbs->outXL > pocketverbs->delayX) {
					pocketverbs->outXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->oXL[pocketverbs->outXL]);
				// allpass filter X

				pocketverbs->dXL[6] = pocketverbs->dXL[5];
				pocketverbs->dXL[5] = pocketverbs->dXL[4];
				pocketverbs->dXL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[4] + pocketverbs->dXL[5] + pocketverbs->dXL[6]) / 3.0;

				allpasstemp = pocketverbs->outYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->oYL[allpasstemp] * constallpass;
				pocketverbs->oYL[pocketverbs->outYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outYL--;
				if (pocketverbs->outYL < 0 || pocketverbs->outYL > pocketverbs->delayY) {
					pocketverbs->outYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->oYL[pocketverbs->outYL]);
				// allpass filter Y

				pocketverbs->dYL[6] = pocketverbs->dYL[5];
				pocketverbs->dYL[5] = pocketverbs->dYL[4];
				pocketverbs->dYL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[4] + pocketverbs->dYL[5] + pocketverbs->dYL[6]) / 3.0;

				allpasstemp = pocketverbs->outZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->oZL[allpasstemp] * constallpass;
				pocketverbs->oZL[pocketverbs->outZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outZL--;
				if (pocketverbs->outZL < 0 || pocketverbs->outZL > pocketverbs->delayZ) {
					pocketverbs->outZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->oZL[pocketverbs->outZL]);
				// allpass filter Z

				pocketverbs->dZL[6] = pocketverbs->dZL[5];
				pocketverbs->dZL[5] = pocketverbs->dZL[4];
				pocketverbs->dZL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[4] + pocketverbs->dZL[5] + pocketverbs->dZL[6]);
				// output Chamber
				break;

			case 2: // Spring

				allpasstemp = pocketverbs->alpAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->aAL[allpasstemp] * constallpass;
				pocketverbs->aAL[pocketverbs->alpAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpAL--;
				if (pocketverbs->alpAL < 0 || pocketverbs->alpAL > pocketverbs->delayA) {
					pocketverbs->alpAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->aAL[pocketverbs->alpAL]);
				// allpass filter A

				pocketverbs->dAL[3] = pocketverbs->dAL[2];
				pocketverbs->dAL[2] = pocketverbs->dAL[1];
				pocketverbs->dAL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dAL[2] + pocketverbs->dAL[3]) / 3.0;

				allpasstemp = pocketverbs->alpBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->aBL[allpasstemp] * constallpass;
				pocketverbs->aBL[pocketverbs->alpBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpBL--;
				if (pocketverbs->alpBL < 0 || pocketverbs->alpBL > pocketverbs->delayB) {
					pocketverbs->alpBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->aBL[pocketverbs->alpBL]);
				// allpass filter B

				pocketverbs->dBL[3] = pocketverbs->dBL[2];
				pocketverbs->dBL[2] = pocketverbs->dBL[1];
				pocketverbs->dBL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dBL[2] + pocketverbs->dBL[3]) / 3.0;

				allpasstemp = pocketverbs->alpCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->aCL[allpasstemp] * constallpass;
				pocketverbs->aCL[pocketverbs->alpCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpCL--;
				if (pocketverbs->alpCL < 0 || pocketverbs->alpCL > pocketverbs->delayC) {
					pocketverbs->alpCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->aCL[pocketverbs->alpCL]);
				// allpass filter C

				pocketverbs->dCL[3] = pocketverbs->dCL[2];
				pocketverbs->dCL[2] = pocketverbs->dCL[1];
				pocketverbs->dCL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[1] + pocketverbs->dCL[2] + pocketverbs->dCL[3]) / 3.0;

				allpasstemp = pocketverbs->alpDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->aDL[allpasstemp] * constallpass;
				pocketverbs->aDL[pocketverbs->alpDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpDL--;
				if (pocketverbs->alpDL < 0 || pocketverbs->alpDL > pocketverbs->delayD) {
					pocketverbs->alpDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->aDL[pocketverbs->alpDL]);
				// allpass filter D

				pocketverbs->dDL[3] = pocketverbs->dDL[2];
				pocketverbs->dDL[2] = pocketverbs->dDL[1];
				pocketverbs->dDL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dDL[2] + pocketverbs->dDL[3]) / 3.0;

				allpasstemp = pocketverbs->alpEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->aEL[allpasstemp] * constallpass;
				pocketverbs->aEL[pocketverbs->alpEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpEL--;
				if (pocketverbs->alpEL < 0 || pocketverbs->alpEL > pocketverbs->delayE) {
					pocketverbs->alpEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->aEL[pocketverbs->alpEL]);
				// allpass filter E

				pocketverbs->dEL[3] = pocketverbs->dEL[2];
				pocketverbs->dEL[2] = pocketverbs->dEL[1];
				pocketverbs->dEL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[1] + pocketverbs->dEL[2] + pocketverbs->dEL[3]) / 3.0;

				allpasstemp = pocketverbs->alpFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->aFL[allpasstemp] * constallpass;
				pocketverbs->aFL[pocketverbs->alpFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpFL--;
				if (pocketverbs->alpFL < 0 || pocketverbs->alpFL > pocketverbs->delayF) {
					pocketverbs->alpFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->aFL[pocketverbs->alpFL]);
				// allpass filter F

				pocketverbs->dFL[3] = pocketverbs->dFL[2];
				pocketverbs->dFL[2] = pocketverbs->dFL[1];
				pocketverbs->dFL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dFL[2] + pocketverbs->dFL[3]) / 3.0;

				allpasstemp = pocketverbs->alpGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->aGL[allpasstemp] * constallpass;
				pocketverbs->aGL[pocketverbs->alpGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpGL--;
				if (pocketverbs->alpGL < 0 || pocketverbs->alpGL > pocketverbs->delayG) {
					pocketverbs->alpGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->aGL[pocketverbs->alpGL]);
				// allpass filter G

				pocketverbs->dGL[3] = pocketverbs->dGL[2];
				pocketverbs->dGL[2] = pocketverbs->dGL[1];
				pocketverbs->dGL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[1] + pocketverbs->dGL[2] + pocketverbs->dGL[3]) / 3.0;

				allpasstemp = pocketverbs->alpHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->aHL[allpasstemp] * constallpass;
				pocketverbs->aHL[pocketverbs->alpHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpHL--;
				if (pocketverbs->alpHL < 0 || pocketverbs->alpHL > pocketverbs->delayH) {
					pocketverbs->alpHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->aHL[pocketverbs->alpHL]);
				// allpass filter H

				pocketverbs->dHL[3] = pocketverbs->dHL[2];
				pocketverbs->dHL[2] = pocketverbs->dHL[1];
				pocketverbs->dHL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dHL[2] + pocketverbs->dHL[3]) / 3.0;

				allpasstemp = pocketverbs->alpIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->aIL[allpasstemp] * constallpass;
				pocketverbs->aIL[pocketverbs->alpIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpIL--;
				if (pocketverbs->alpIL < 0 || pocketverbs->alpIL > pocketverbs->delayI) {
					pocketverbs->alpIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->aIL[pocketverbs->alpIL]);
				// allpass filter I

				pocketverbs->dIL[3] = pocketverbs->dIL[2];
				pocketverbs->dIL[2] = pocketverbs->dIL[1];
				pocketverbs->dIL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dIL[2] + pocketverbs->dIL[3]) / 3.0;

				allpasstemp = pocketverbs->alpJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->aJL[allpasstemp] * constallpass;
				pocketverbs->aJL[pocketverbs->alpJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpJL--;
				if (pocketverbs->alpJL < 0 || pocketverbs->alpJL > pocketverbs->delayJ) {
					pocketverbs->alpJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->aJL[pocketverbs->alpJL]);
				// allpass filter J

				pocketverbs->dJL[3] = pocketverbs->dJL[2];
				pocketverbs->dJL[2] = pocketverbs->dJL[1];
				pocketverbs->dJL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dJL[2] + pocketverbs->dJL[3]) / 3.0;

				allpasstemp = pocketverbs->alpKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->aKL[allpasstemp] * constallpass;
				pocketverbs->aKL[pocketverbs->alpKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpKL--;
				if (pocketverbs->alpKL < 0 || pocketverbs->alpKL > pocketverbs->delayK) {
					pocketverbs->alpKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->aKL[pocketverbs->alpKL]);
				// allpass filter K

				pocketverbs->dKL[3] = pocketverbs->dKL[2];
				pocketverbs->dKL[2] = pocketverbs->dKL[1];
				pocketverbs->dKL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[1] + pocketverbs->dKL[2] + pocketverbs->dKL[3]) / 3.0;

				allpasstemp = pocketverbs->alpLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->aLL[allpasstemp] * constallpass;
				pocketverbs->aLL[pocketverbs->alpLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpLL--;
				if (pocketverbs->alpLL < 0 || pocketverbs->alpLL > pocketverbs->delayL) {
					pocketverbs->alpLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->aLL[pocketverbs->alpLL]);
				// allpass filter L

				pocketverbs->dLL[3] = pocketverbs->dLL[2];
				pocketverbs->dLL[2] = pocketverbs->dLL[1];
				pocketverbs->dLL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dLL[2] + pocketverbs->dLL[3]) / 3.0;

				allpasstemp = pocketverbs->alpML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->aML[allpasstemp] * constallpass;
				pocketverbs->aML[pocketverbs->alpML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpML--;
				if (pocketverbs->alpML < 0 || pocketverbs->alpML > pocketverbs->delayM) {
					pocketverbs->alpML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->aML[pocketverbs->alpML]);
				// allpass filter M

				pocketverbs->dML[3] = pocketverbs->dML[2];
				pocketverbs->dML[2] = pocketverbs->dML[1];
				pocketverbs->dML[1] = inputSampleL;
				inputSampleL = (pocketverbs->dML[1] + pocketverbs->dML[2] + pocketverbs->dML[3]) / 3.0;

				allpasstemp = pocketverbs->alpNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->aNL[allpasstemp] * constallpass;
				pocketverbs->aNL[pocketverbs->alpNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpNL--;
				if (pocketverbs->alpNL < 0 || pocketverbs->alpNL > pocketverbs->delayN) {
					pocketverbs->alpNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->aNL[pocketverbs->alpNL]);
				// allpass filter N

				pocketverbs->dNL[3] = pocketverbs->dNL[2];
				pocketverbs->dNL[2] = pocketverbs->dNL[1];
				pocketverbs->dNL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dNL[2] + pocketverbs->dNL[3]) / 3.0;

				allpasstemp = pocketverbs->alpOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->aOL[allpasstemp] * constallpass;
				pocketverbs->aOL[pocketverbs->alpOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpOL--;
				if (pocketverbs->alpOL < 0 || pocketverbs->alpOL > pocketverbs->delayO) {
					pocketverbs->alpOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->aOL[pocketverbs->alpOL]);
				// allpass filter O

				pocketverbs->dOL[3] = pocketverbs->dOL[2];
				pocketverbs->dOL[2] = pocketverbs->dOL[1];
				pocketverbs->dOL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dOL[2] + pocketverbs->dOL[3]) / 3.0;

				allpasstemp = pocketverbs->alpPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->aPL[allpasstemp] * constallpass;
				pocketverbs->aPL[pocketverbs->alpPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpPL--;
				if (pocketverbs->alpPL < 0 || pocketverbs->alpPL > pocketverbs->delayP) {
					pocketverbs->alpPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->aPL[pocketverbs->alpPL]);
				// allpass filter P

				pocketverbs->dPL[3] = pocketverbs->dPL[2];
				pocketverbs->dPL[2] = pocketverbs->dPL[1];
				pocketverbs->dPL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dPL[2] + pocketverbs->dPL[3]) / 3.0;

				allpasstemp = pocketverbs->alpQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->aQL[allpasstemp] * constallpass;
				pocketverbs->aQL[pocketverbs->alpQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpQL--;
				if (pocketverbs->alpQL < 0 || pocketverbs->alpQL > pocketverbs->delayQ) {
					pocketverbs->alpQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->aQL[pocketverbs->alpQL]);
				// allpass filter Q

				pocketverbs->dQL[3] = pocketverbs->dQL[2];
				pocketverbs->dQL[2] = pocketverbs->dQL[1];
				pocketverbs->dQL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[1] + pocketverbs->dQL[2] + pocketverbs->dQL[3]) / 3.0;

				allpasstemp = pocketverbs->alpRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->aRL[allpasstemp] * constallpass;
				pocketverbs->aRL[pocketverbs->alpRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpRL--;
				if (pocketverbs->alpRL < 0 || pocketverbs->alpRL > pocketverbs->delayR) {
					pocketverbs->alpRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->aRL[pocketverbs->alpRL]);
				// allpass filter R

				pocketverbs->dRL[3] = pocketverbs->dRL[2];
				pocketverbs->dRL[2] = pocketverbs->dRL[1];
				pocketverbs->dRL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dRL[2] + pocketverbs->dRL[3]) / 3.0;

				allpasstemp = pocketverbs->alpSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->aSL[allpasstemp] * constallpass;
				pocketverbs->aSL[pocketverbs->alpSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpSL--;
				if (pocketverbs->alpSL < 0 || pocketverbs->alpSL > pocketverbs->delayS) {
					pocketverbs->alpSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->aSL[pocketverbs->alpSL]);
				// allpass filter S

				pocketverbs->dSL[3] = pocketverbs->dSL[2];
				pocketverbs->dSL[2] = pocketverbs->dSL[1];
				pocketverbs->dSL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[1] + pocketverbs->dSL[2] + pocketverbs->dSL[3]) / 3.0;

				allpasstemp = pocketverbs->alpTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->aTL[allpasstemp] * constallpass;
				pocketverbs->aTL[pocketverbs->alpTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpTL--;
				if (pocketverbs->alpTL < 0 || pocketverbs->alpTL > pocketverbs->delayT) {
					pocketverbs->alpTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->aTL[pocketverbs->alpTL]);
				// allpass filter T

				pocketverbs->dTL[3] = pocketverbs->dTL[2];
				pocketverbs->dTL[2] = pocketverbs->dTL[1];
				pocketverbs->dTL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dTL[2] + pocketverbs->dTL[3]) / 3.0;

				allpasstemp = pocketverbs->alpUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->aUL[allpasstemp] * constallpass;
				pocketverbs->aUL[pocketverbs->alpUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpUL--;
				if (pocketverbs->alpUL < 0 || pocketverbs->alpUL > pocketverbs->delayU) {
					pocketverbs->alpUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->aUL[pocketverbs->alpUL]);
				// allpass filter U

				pocketverbs->dUL[3] = pocketverbs->dUL[2];
				pocketverbs->dUL[2] = pocketverbs->dUL[1];
				pocketverbs->dUL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dUL[2] + pocketverbs->dUL[3]) / 3.0;

				allpasstemp = pocketverbs->alpVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->aVL[allpasstemp] * constallpass;
				pocketverbs->aVL[pocketverbs->alpVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpVL--;
				if (pocketverbs->alpVL < 0 || pocketverbs->alpVL > pocketverbs->delayV) {
					pocketverbs->alpVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->aVL[pocketverbs->alpVL]);
				// allpass filter V

				pocketverbs->dVL[3] = pocketverbs->dVL[2];
				pocketverbs->dVL[2] = pocketverbs->dVL[1];
				pocketverbs->dVL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dVL[2] + pocketverbs->dVL[3]) / 3.0;

				allpasstemp = pocketverbs->alpWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->aWL[allpasstemp] * constallpass;
				pocketverbs->aWL[pocketverbs->alpWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpWL--;
				if (pocketverbs->alpWL < 0 || pocketverbs->alpWL > pocketverbs->delayW) {
					pocketverbs->alpWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->aWL[pocketverbs->alpWL]);
				// allpass filter W

				pocketverbs->dWL[3] = pocketverbs->dWL[2];
				pocketverbs->dWL[2] = pocketverbs->dWL[1];
				pocketverbs->dWL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[1] + pocketverbs->dWL[2] + pocketverbs->dWL[3]) / 3.0;

				allpasstemp = pocketverbs->alpXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->aXL[allpasstemp] * constallpass;
				pocketverbs->aXL[pocketverbs->alpXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpXL--;
				if (pocketverbs->alpXL < 0 || pocketverbs->alpXL > pocketverbs->delayX) {
					pocketverbs->alpXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->aXL[pocketverbs->alpXL]);
				// allpass filter X

				pocketverbs->dXL[3] = pocketverbs->dXL[2];
				pocketverbs->dXL[2] = pocketverbs->dXL[1];
				pocketverbs->dXL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dXL[2] + pocketverbs->dXL[3]) / 3.0;

				allpasstemp = pocketverbs->alpYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->aYL[allpasstemp] * constallpass;
				pocketverbs->aYL[pocketverbs->alpYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpYL--;
				if (pocketverbs->alpYL < 0 || pocketverbs->alpYL > pocketverbs->delayY) {
					pocketverbs->alpYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->aYL[pocketverbs->alpYL]);
				// allpass filter Y

				pocketverbs->dYL[3] = pocketverbs->dYL[2];
				pocketverbs->dYL[2] = pocketverbs->dYL[1];
				pocketverbs->dYL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dYL[2] + pocketverbs->dYL[3]) / 3.0;

				allpasstemp = pocketverbs->alpZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->aZL[allpasstemp] * constallpass;
				pocketverbs->aZL[pocketverbs->alpZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpZL--;
				if (pocketverbs->alpZL < 0 || pocketverbs->alpZL > pocketverbs->delayZ) {
					pocketverbs->alpZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->aZL[pocketverbs->alpZL]);
				// allpass filter Z

				pocketverbs->dZL[3] = pocketverbs->dZL[2];
				pocketverbs->dZL[2] = pocketverbs->dZL[1];
				pocketverbs->dZL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[1] + pocketverbs->dZL[2] + pocketverbs->dZL[3]) / 3.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->oAL[allpasstemp] * constallpass;
				pocketverbs->oAL[pocketverbs->outAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outAL--;
				if (pocketverbs->outAL < 0 || pocketverbs->outAL > pocketverbs->delayA) {
					pocketverbs->outAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->oAL[pocketverbs->outAL]);
				// allpass filter A

				pocketverbs->dAL[6] = pocketverbs->dAL[5];
				pocketverbs->dAL[5] = pocketverbs->dAL[4];
				pocketverbs->dAL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dAL[5] + pocketverbs->dAL[6]) / 3.0;

				allpasstemp = pocketverbs->outBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->oBL[allpasstemp] * constallpass;
				pocketverbs->oBL[pocketverbs->outBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outBL--;
				if (pocketverbs->outBL < 0 || pocketverbs->outBL > pocketverbs->delayB) {
					pocketverbs->outBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->oBL[pocketverbs->outBL]);
				// allpass filter B

				pocketverbs->dBL[6] = pocketverbs->dBL[5];
				pocketverbs->dBL[5] = pocketverbs->dBL[4];
				pocketverbs->dBL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dBL[5] + pocketverbs->dBL[6]) / 3.0;

				allpasstemp = pocketverbs->outCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->oCL[allpasstemp] * constallpass;
				pocketverbs->oCL[pocketverbs->outCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outCL--;
				if (pocketverbs->outCL < 0 || pocketverbs->outCL > pocketverbs->delayC) {
					pocketverbs->outCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->oCL[pocketverbs->outCL]);
				// allpass filter C

				pocketverbs->dCL[6] = pocketverbs->dCL[5];
				pocketverbs->dCL[5] = pocketverbs->dCL[4];
				pocketverbs->dCL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[1] + pocketverbs->dCL[5] + pocketverbs->dCL[6]) / 3.0;

				allpasstemp = pocketverbs->outDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->oDL[allpasstemp] * constallpass;
				pocketverbs->oDL[pocketverbs->outDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outDL--;
				if (pocketverbs->outDL < 0 || pocketverbs->outDL > pocketverbs->delayD) {
					pocketverbs->outDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->oDL[pocketverbs->outDL]);
				// allpass filter D

				pocketverbs->dDL[6] = pocketverbs->dDL[5];
				pocketverbs->dDL[5] = pocketverbs->dDL[4];
				pocketverbs->dDL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dDL[5] + pocketverbs->dDL[6]) / 3.0;

				allpasstemp = pocketverbs->outEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->oEL[allpasstemp] * constallpass;
				pocketverbs->oEL[pocketverbs->outEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outEL--;
				if (pocketverbs->outEL < 0 || pocketverbs->outEL > pocketverbs->delayE) {
					pocketverbs->outEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->oEL[pocketverbs->outEL]);
				// allpass filter E

				pocketverbs->dEL[6] = pocketverbs->dEL[5];
				pocketverbs->dEL[5] = pocketverbs->dEL[4];
				pocketverbs->dEL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dEL[5] + pocketverbs->dEL[6]) / 3.0;

				allpasstemp = pocketverbs->outFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->oFL[allpasstemp] * constallpass;
				pocketverbs->oFL[pocketverbs->outFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outFL--;
				if (pocketverbs->outFL < 0 || pocketverbs->outFL > pocketverbs->delayF) {
					pocketverbs->outFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->oFL[pocketverbs->outFL]);
				// allpass filter F

				pocketverbs->dFL[6] = pocketverbs->dFL[5];
				pocketverbs->dFL[5] = pocketverbs->dFL[4];
				pocketverbs->dFL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dFL[5] + pocketverbs->dFL[6]) / 3.0;

				allpasstemp = pocketverbs->outGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->oGL[allpasstemp] * constallpass;
				pocketverbs->oGL[pocketverbs->outGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outGL--;
				if (pocketverbs->outGL < 0 || pocketverbs->outGL > pocketverbs->delayG) {
					pocketverbs->outGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->oGL[pocketverbs->outGL]);
				// allpass filter G

				pocketverbs->dGL[6] = pocketverbs->dGL[5];
				pocketverbs->dGL[5] = pocketverbs->dGL[4];
				pocketverbs->dGL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[1] + pocketverbs->dGL[5] + pocketverbs->dGL[6]) / 3.0;

				allpasstemp = pocketverbs->outHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->oHL[allpasstemp] * constallpass;
				pocketverbs->oHL[pocketverbs->outHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outHL--;
				if (pocketverbs->outHL < 0 || pocketverbs->outHL > pocketverbs->delayH) {
					pocketverbs->outHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->oHL[pocketverbs->outHL]);
				// allpass filter H

				pocketverbs->dHL[6] = pocketverbs->dHL[5];
				pocketverbs->dHL[5] = pocketverbs->dHL[4];
				pocketverbs->dHL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dHL[5] + pocketverbs->dHL[6]) / 3.0;

				allpasstemp = pocketverbs->outIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->oIL[allpasstemp] * constallpass;
				pocketverbs->oIL[pocketverbs->outIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outIL--;
				if (pocketverbs->outIL < 0 || pocketverbs->outIL > pocketverbs->delayI) {
					pocketverbs->outIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->oIL[pocketverbs->outIL]);
				// allpass filter I

				pocketverbs->dIL[6] = pocketverbs->dIL[5];
				pocketverbs->dIL[5] = pocketverbs->dIL[4];
				pocketverbs->dIL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[1] + pocketverbs->dIL[5] + pocketverbs->dIL[6]) / 3.0;

				allpasstemp = pocketverbs->outJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->oJL[allpasstemp] * constallpass;
				pocketverbs->oJL[pocketverbs->outJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outJL--;
				if (pocketverbs->outJL < 0 || pocketverbs->outJL > pocketverbs->delayJ) {
					pocketverbs->outJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->oJL[pocketverbs->outJL]);
				// allpass filter J

				pocketverbs->dJL[6] = pocketverbs->dJL[5];
				pocketverbs->dJL[5] = pocketverbs->dJL[4];
				pocketverbs->dJL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dJL[5] + pocketverbs->dJL[6]) / 3.0;

				allpasstemp = pocketverbs->outKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->oKL[allpasstemp] * constallpass;
				pocketverbs->oKL[pocketverbs->outKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outKL--;
				if (pocketverbs->outKL < 0 || pocketverbs->outKL > pocketverbs->delayK) {
					pocketverbs->outKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->oKL[pocketverbs->outKL]);
				// allpass filter K

				pocketverbs->dKL[6] = pocketverbs->dKL[5];
				pocketverbs->dKL[5] = pocketverbs->dKL[4];
				pocketverbs->dKL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dKL[5] + pocketverbs->dKL[6]) / 3.0;

				allpasstemp = pocketverbs->outLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->oLL[allpasstemp] * constallpass;
				pocketverbs->oLL[pocketverbs->outLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outLL--;
				if (pocketverbs->outLL < 0 || pocketverbs->outLL > pocketverbs->delayL) {
					pocketverbs->outLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->oLL[pocketverbs->outLL]);
				// allpass filter L

				pocketverbs->dLL[6] = pocketverbs->dLL[5];
				pocketverbs->dLL[5] = pocketverbs->dLL[4];
				pocketverbs->dLL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dLL[5] + pocketverbs->dLL[6]) / 3.0;

				allpasstemp = pocketverbs->outML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->oML[allpasstemp] * constallpass;
				pocketverbs->oML[pocketverbs->outML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outML--;
				if (pocketverbs->outML < 0 || pocketverbs->outML > pocketverbs->delayM) {
					pocketverbs->outML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->oML[pocketverbs->outML]);
				// allpass filter M

				pocketverbs->dML[6] = pocketverbs->dML[5];
				pocketverbs->dML[5] = pocketverbs->dML[4];
				pocketverbs->dML[4] = inputSampleL;
				inputSampleL = (pocketverbs->dML[1] + pocketverbs->dML[5] + pocketverbs->dML[6]) / 3.0;

				allpasstemp = pocketverbs->outNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->oNL[allpasstemp] * constallpass;
				pocketverbs->oNL[pocketverbs->outNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outNL--;
				if (pocketverbs->outNL < 0 || pocketverbs->outNL > pocketverbs->delayN) {
					pocketverbs->outNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->oNL[pocketverbs->outNL]);
				// allpass filter N

				pocketverbs->dNL[6] = pocketverbs->dNL[5];
				pocketverbs->dNL[5] = pocketverbs->dNL[4];
				pocketverbs->dNL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dNL[5] + pocketverbs->dNL[6]) / 3.0;

				allpasstemp = pocketverbs->outOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->oOL[allpasstemp] * constallpass;
				pocketverbs->oOL[pocketverbs->outOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outOL--;
				if (pocketverbs->outOL < 0 || pocketverbs->outOL > pocketverbs->delayO) {
					pocketverbs->outOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->oOL[pocketverbs->outOL]);
				// allpass filter O

				pocketverbs->dOL[6] = pocketverbs->dOL[5];
				pocketverbs->dOL[5] = pocketverbs->dOL[4];
				pocketverbs->dOL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[1] + pocketverbs->dOL[5] + pocketverbs->dOL[6]) / 3.0;

				allpasstemp = pocketverbs->outPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->oPL[allpasstemp] * constallpass;
				pocketverbs->oPL[pocketverbs->outPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outPL--;
				if (pocketverbs->outPL < 0 || pocketverbs->outPL > pocketverbs->delayP) {
					pocketverbs->outPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->oPL[pocketverbs->outPL]);
				// allpass filter P

				pocketverbs->dPL[6] = pocketverbs->dPL[5];
				pocketverbs->dPL[5] = pocketverbs->dPL[4];
				pocketverbs->dPL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dPL[5] + pocketverbs->dPL[6]) / 3.0;

				allpasstemp = pocketverbs->outQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->oQL[allpasstemp] * constallpass;
				pocketverbs->oQL[pocketverbs->outQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outQL--;
				if (pocketverbs->outQL < 0 || pocketverbs->outQL > pocketverbs->delayQ) {
					pocketverbs->outQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->oQL[pocketverbs->outQL]);
				// allpass filter Q

				pocketverbs->dQL[6] = pocketverbs->dQL[5];
				pocketverbs->dQL[5] = pocketverbs->dQL[4];
				pocketverbs->dQL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dQL[5] + pocketverbs->dQL[6]) / 3.0;

				allpasstemp = pocketverbs->outRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->oRL[allpasstemp] * constallpass;
				pocketverbs->oRL[pocketverbs->outRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outRL--;
				if (pocketverbs->outRL < 0 || pocketverbs->outRL > pocketverbs->delayR) {
					pocketverbs->outRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->oRL[pocketverbs->outRL]);
				// allpass filter R

				pocketverbs->dRL[6] = pocketverbs->dRL[5];
				pocketverbs->dRL[5] = pocketverbs->dRL[4];
				pocketverbs->dRL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dRL[5] + pocketverbs->dRL[6]) / 3.0;

				allpasstemp = pocketverbs->outSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->oSL[allpasstemp] * constallpass;
				pocketverbs->oSL[pocketverbs->outSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outSL--;
				if (pocketverbs->outSL < 0 || pocketverbs->outSL > pocketverbs->delayS) {
					pocketverbs->outSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->oSL[pocketverbs->outSL]);
				// allpass filter S

				pocketverbs->dSL[6] = pocketverbs->dSL[5];
				pocketverbs->dSL[5] = pocketverbs->dSL[4];
				pocketverbs->dSL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[1] + pocketverbs->dSL[5] + pocketverbs->dSL[6]) / 3.0;

				allpasstemp = pocketverbs->outTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->oTL[allpasstemp] * constallpass;
				pocketverbs->oTL[pocketverbs->outTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outTL--;
				if (pocketverbs->outTL < 0 || pocketverbs->outTL > pocketverbs->delayT) {
					pocketverbs->outTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->oTL[pocketverbs->outTL]);
				// allpass filter T

				pocketverbs->dTL[6] = pocketverbs->dTL[5];
				pocketverbs->dTL[5] = pocketverbs->dTL[4];
				pocketverbs->dTL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dTL[5] + pocketverbs->dTL[6]) / 3.0;

				allpasstemp = pocketverbs->outUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->oUL[allpasstemp] * constallpass;
				pocketverbs->oUL[pocketverbs->outUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outUL--;
				if (pocketverbs->outUL < 0 || pocketverbs->outUL > pocketverbs->delayU) {
					pocketverbs->outUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->oUL[pocketverbs->outUL]);
				// allpass filter U

				pocketverbs->dUL[6] = pocketverbs->dUL[5];
				pocketverbs->dUL[5] = pocketverbs->dUL[4];
				pocketverbs->dUL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[1] + pocketverbs->dUL[5] + pocketverbs->dUL[6]) / 3.0;

				allpasstemp = pocketverbs->outVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->oVL[allpasstemp] * constallpass;
				pocketverbs->oVL[pocketverbs->outVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outVL--;
				if (pocketverbs->outVL < 0 || pocketverbs->outVL > pocketverbs->delayV) {
					pocketverbs->outVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->oVL[pocketverbs->outVL]);
				// allpass filter V

				pocketverbs->dVL[6] = pocketverbs->dVL[5];
				pocketverbs->dVL[5] = pocketverbs->dVL[4];
				pocketverbs->dVL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dVL[5] + pocketverbs->dVL[6]) / 3.0;

				allpasstemp = pocketverbs->outWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->oWL[allpasstemp] * constallpass;
				pocketverbs->oWL[pocketverbs->outWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outWL--;
				if (pocketverbs->outWL < 0 || pocketverbs->outWL > pocketverbs->delayW) {
					pocketverbs->outWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->oWL[pocketverbs->outWL]);
				// allpass filter W

				pocketverbs->dWL[6] = pocketverbs->dWL[5];
				pocketverbs->dWL[5] = pocketverbs->dWL[4];
				pocketverbs->dWL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[1] + pocketverbs->dWL[5] + pocketverbs->dWL[6]) / 3.0;

				allpasstemp = pocketverbs->outXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->oXL[allpasstemp] * constallpass;
				pocketverbs->oXL[pocketverbs->outXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outXL--;
				if (pocketverbs->outXL < 0 || pocketverbs->outXL > pocketverbs->delayX) {
					pocketverbs->outXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->oXL[pocketverbs->outXL]);
				// allpass filter X

				pocketverbs->dXL[6] = pocketverbs->dXL[5];
				pocketverbs->dXL[5] = pocketverbs->dXL[4];
				pocketverbs->dXL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dXL[5] + pocketverbs->dXL[6]) / 3.0;

				allpasstemp = pocketverbs->outYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->oYL[allpasstemp] * constallpass;
				pocketverbs->oYL[pocketverbs->outYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outYL--;
				if (pocketverbs->outYL < 0 || pocketverbs->outYL > pocketverbs->delayY) {
					pocketverbs->outYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->oYL[pocketverbs->outYL]);
				// allpass filter Y

				pocketverbs->dYL[6] = pocketverbs->dYL[5];
				pocketverbs->dYL[5] = pocketverbs->dYL[4];
				pocketverbs->dYL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dYL[5] + pocketverbs->dYL[6]) / 3.0;

				allpasstemp = pocketverbs->outZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->oZL[allpasstemp] * constallpass;
				pocketverbs->oZL[pocketverbs->outZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outZL--;
				if (pocketverbs->outZL < 0 || pocketverbs->outZL > pocketverbs->delayZ) {
					pocketverbs->outZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->oZL[pocketverbs->outZL]);
				// allpass filter Z

				pocketverbs->dZL[6] = pocketverbs->dZL[5];
				pocketverbs->dZL[5] = pocketverbs->dZL[4];
				pocketverbs->dZL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[5] + pocketverbs->dZL[6]);
				// output Spring
				break;

			case 3: // Tiled
				allpasstemp = pocketverbs->alpAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->aAL[allpasstemp] * constallpass;
				pocketverbs->aAL[pocketverbs->alpAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpAL--;
				if (pocketverbs->alpAL < 0 || pocketverbs->alpAL > pocketverbs->delayA) {
					pocketverbs->alpAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->aAL[pocketverbs->alpAL]);
				// allpass filter A

				pocketverbs->dAL[2] = pocketverbs->dAL[1];
				pocketverbs->dAL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dAL[2]) / 2.0;

				allpasstemp = pocketverbs->alpBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->aBL[allpasstemp] * constallpass;
				pocketverbs->aBL[pocketverbs->alpBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpBL--;
				if (pocketverbs->alpBL < 0 || pocketverbs->alpBL > pocketverbs->delayB) {
					pocketverbs->alpBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->aBL[pocketverbs->alpBL]);
				// allpass filter B

				pocketverbs->dBL[2] = pocketverbs->dBL[1];
				pocketverbs->dBL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dBL[2]) / 2.0;

				allpasstemp = pocketverbs->alpCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->aCL[allpasstemp] * constallpass;
				pocketverbs->aCL[pocketverbs->alpCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpCL--;
				if (pocketverbs->alpCL < 0 || pocketverbs->alpCL > pocketverbs->delayC) {
					pocketverbs->alpCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->aCL[pocketverbs->alpCL]);
				// allpass filter C

				pocketverbs->dCL[2] = pocketverbs->dCL[1];
				pocketverbs->dCL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dCL[2]) / 2.0;

				allpasstemp = pocketverbs->alpDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->aDL[allpasstemp] * constallpass;
				pocketverbs->aDL[pocketverbs->alpDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpDL--;
				if (pocketverbs->alpDL < 0 || pocketverbs->alpDL > pocketverbs->delayD) {
					pocketverbs->alpDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->aDL[pocketverbs->alpDL]);
				// allpass filter D

				pocketverbs->dDL[2] = pocketverbs->dDL[1];
				pocketverbs->dDL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dDL[2]) / 2.0;

				allpasstemp = pocketverbs->alpEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->aEL[allpasstemp] * constallpass;
				pocketverbs->aEL[pocketverbs->alpEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpEL--;
				if (pocketverbs->alpEL < 0 || pocketverbs->alpEL > pocketverbs->delayE) {
					pocketverbs->alpEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->aEL[pocketverbs->alpEL]);
				// allpass filter E

				pocketverbs->dEL[2] = pocketverbs->dEL[1];
				pocketverbs->dEL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dEL[2]) / 2.0;

				allpasstemp = pocketverbs->alpFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->aFL[allpasstemp] * constallpass;
				pocketverbs->aFL[pocketverbs->alpFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpFL--;
				if (pocketverbs->alpFL < 0 || pocketverbs->alpFL > pocketverbs->delayF) {
					pocketverbs->alpFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->aFL[pocketverbs->alpFL]);
				// allpass filter F

				pocketverbs->dFL[2] = pocketverbs->dFL[1];
				pocketverbs->dFL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dFL[2]) / 2.0;

				allpasstemp = pocketverbs->alpGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->aGL[allpasstemp] * constallpass;
				pocketverbs->aGL[pocketverbs->alpGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpGL--;
				if (pocketverbs->alpGL < 0 || pocketverbs->alpGL > pocketverbs->delayG) {
					pocketverbs->alpGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->aGL[pocketverbs->alpGL]);
				// allpass filter G

				pocketverbs->dGL[2] = pocketverbs->dGL[1];
				pocketverbs->dGL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dGL[2]) / 2.0;

				allpasstemp = pocketverbs->alpHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->aHL[allpasstemp] * constallpass;
				pocketverbs->aHL[pocketverbs->alpHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpHL--;
				if (pocketverbs->alpHL < 0 || pocketverbs->alpHL > pocketverbs->delayH) {
					pocketverbs->alpHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->aHL[pocketverbs->alpHL]);
				// allpass filter H

				pocketverbs->dHL[2] = pocketverbs->dHL[1];
				pocketverbs->dHL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dHL[2]) / 2.0;

				allpasstemp = pocketverbs->alpIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->aIL[allpasstemp] * constallpass;
				pocketverbs->aIL[pocketverbs->alpIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpIL--;
				if (pocketverbs->alpIL < 0 || pocketverbs->alpIL > pocketverbs->delayI) {
					pocketverbs->alpIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->aIL[pocketverbs->alpIL]);
				// allpass filter I

				pocketverbs->dIL[2] = pocketverbs->dIL[1];
				pocketverbs->dIL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dIL[2]) / 2.0;

				allpasstemp = pocketverbs->alpJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->aJL[allpasstemp] * constallpass;
				pocketverbs->aJL[pocketverbs->alpJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpJL--;
				if (pocketverbs->alpJL < 0 || pocketverbs->alpJL > pocketverbs->delayJ) {
					pocketverbs->alpJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->aJL[pocketverbs->alpJL]);
				// allpass filter J

				pocketverbs->dJL[2] = pocketverbs->dJL[1];
				pocketverbs->dJL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dJL[2]) / 2.0;

				allpasstemp = pocketverbs->alpKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->aKL[allpasstemp] * constallpass;
				pocketverbs->aKL[pocketverbs->alpKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpKL--;
				if (pocketverbs->alpKL < 0 || pocketverbs->alpKL > pocketverbs->delayK) {
					pocketverbs->alpKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->aKL[pocketverbs->alpKL]);
				// allpass filter K

				pocketverbs->dKL[2] = pocketverbs->dKL[1];
				pocketverbs->dKL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dKL[2]) / 2.0;

				allpasstemp = pocketverbs->alpLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->aLL[allpasstemp] * constallpass;
				pocketverbs->aLL[pocketverbs->alpLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpLL--;
				if (pocketverbs->alpLL < 0 || pocketverbs->alpLL > pocketverbs->delayL) {
					pocketverbs->alpLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->aLL[pocketverbs->alpLL]);
				// allpass filter L

				pocketverbs->dLL[2] = pocketverbs->dLL[1];
				pocketverbs->dLL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dLL[2]) / 2.0;

				allpasstemp = pocketverbs->alpML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->aML[allpasstemp] * constallpass;
				pocketverbs->aML[pocketverbs->alpML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpML--;
				if (pocketverbs->alpML < 0 || pocketverbs->alpML > pocketverbs->delayM) {
					pocketverbs->alpML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->aML[pocketverbs->alpML]);
				// allpass filter M

				pocketverbs->dML[2] = pocketverbs->dML[1];
				pocketverbs->dML[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dML[2]) / 2.0;

				allpasstemp = pocketverbs->alpNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->aNL[allpasstemp] * constallpass;
				pocketverbs->aNL[pocketverbs->alpNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpNL--;
				if (pocketverbs->alpNL < 0 || pocketverbs->alpNL > pocketverbs->delayN) {
					pocketverbs->alpNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->aNL[pocketverbs->alpNL]);
				// allpass filter N

				pocketverbs->dNL[2] = pocketverbs->dNL[1];
				pocketverbs->dNL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dNL[2]) / 2.0;

				allpasstemp = pocketverbs->alpOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->aOL[allpasstemp] * constallpass;
				pocketverbs->aOL[pocketverbs->alpOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpOL--;
				if (pocketverbs->alpOL < 0 || pocketverbs->alpOL > pocketverbs->delayO) {
					pocketverbs->alpOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->aOL[pocketverbs->alpOL]);
				// allpass filter O

				pocketverbs->dOL[2] = pocketverbs->dOL[1];
				pocketverbs->dOL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dOL[2]) / 2.0;

				allpasstemp = pocketverbs->alpPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->aPL[allpasstemp] * constallpass;
				pocketverbs->aPL[pocketverbs->alpPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpPL--;
				if (pocketverbs->alpPL < 0 || pocketverbs->alpPL > pocketverbs->delayP) {
					pocketverbs->alpPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->aPL[pocketverbs->alpPL]);
				// allpass filter P

				pocketverbs->dPL[2] = pocketverbs->dPL[1];
				pocketverbs->dPL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dPL[2]) / 2.0;

				allpasstemp = pocketverbs->alpQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->aQL[allpasstemp] * constallpass;
				pocketverbs->aQL[pocketverbs->alpQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpQL--;
				if (pocketverbs->alpQL < 0 || pocketverbs->alpQL > pocketverbs->delayQ) {
					pocketverbs->alpQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->aQL[pocketverbs->alpQL]);
				// allpass filter Q

				pocketverbs->dQL[2] = pocketverbs->dQL[1];
				pocketverbs->dQL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dQL[2]) / 2.0;

				allpasstemp = pocketverbs->alpRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->aRL[allpasstemp] * constallpass;
				pocketverbs->aRL[pocketverbs->alpRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpRL--;
				if (pocketverbs->alpRL < 0 || pocketverbs->alpRL > pocketverbs->delayR) {
					pocketverbs->alpRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->aRL[pocketverbs->alpRL]);
				// allpass filter R

				pocketverbs->dRL[2] = pocketverbs->dRL[1];
				pocketverbs->dRL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dRL[2]) / 2.0;

				allpasstemp = pocketverbs->alpSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->aSL[allpasstemp] * constallpass;
				pocketverbs->aSL[pocketverbs->alpSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpSL--;
				if (pocketverbs->alpSL < 0 || pocketverbs->alpSL > pocketverbs->delayS) {
					pocketverbs->alpSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->aSL[pocketverbs->alpSL]);
				// allpass filter S

				pocketverbs->dSL[2] = pocketverbs->dSL[1];
				pocketverbs->dSL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dSL[2]) / 2.0;

				allpasstemp = pocketverbs->alpTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->aTL[allpasstemp] * constallpass;
				pocketverbs->aTL[pocketverbs->alpTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpTL--;
				if (pocketverbs->alpTL < 0 || pocketverbs->alpTL > pocketverbs->delayT) {
					pocketverbs->alpTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->aTL[pocketverbs->alpTL]);
				// allpass filter T

				pocketverbs->dTL[2] = pocketverbs->dTL[1];
				pocketverbs->dTL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dTL[2]) / 2.0;

				allpasstemp = pocketverbs->alpUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->aUL[allpasstemp] * constallpass;
				pocketverbs->aUL[pocketverbs->alpUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpUL--;
				if (pocketverbs->alpUL < 0 || pocketverbs->alpUL > pocketverbs->delayU) {
					pocketverbs->alpUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->aUL[pocketverbs->alpUL]);
				// allpass filter U

				pocketverbs->dUL[2] = pocketverbs->dUL[1];
				pocketverbs->dUL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dUL[2]) / 2.0;

				allpasstemp = pocketverbs->alpVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->aVL[allpasstemp] * constallpass;
				pocketverbs->aVL[pocketverbs->alpVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpVL--;
				if (pocketverbs->alpVL < 0 || pocketverbs->alpVL > pocketverbs->delayV) {
					pocketverbs->alpVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->aVL[pocketverbs->alpVL]);
				// allpass filter V

				pocketverbs->dVL[2] = pocketverbs->dVL[1];
				pocketverbs->dVL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dVL[2]) / 2.0;

				allpasstemp = pocketverbs->alpWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->aWL[allpasstemp] * constallpass;
				pocketverbs->aWL[pocketverbs->alpWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpWL--;
				if (pocketverbs->alpWL < 0 || pocketverbs->alpWL > pocketverbs->delayW) {
					pocketverbs->alpWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->aWL[pocketverbs->alpWL]);
				// allpass filter W

				pocketverbs->dWL[2] = pocketverbs->dWL[1];
				pocketverbs->dWL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dWL[2]) / 2.0;

				allpasstemp = pocketverbs->alpXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->aXL[allpasstemp] * constallpass;
				pocketverbs->aXL[pocketverbs->alpXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpXL--;
				if (pocketverbs->alpXL < 0 || pocketverbs->alpXL > pocketverbs->delayX) {
					pocketverbs->alpXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->aXL[pocketverbs->alpXL]);
				// allpass filter X

				pocketverbs->dXL[2] = pocketverbs->dXL[1];
				pocketverbs->dXL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dXL[2]) / 2.0;

				allpasstemp = pocketverbs->alpYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->aYL[allpasstemp] * constallpass;
				pocketverbs->aYL[pocketverbs->alpYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpYL--;
				if (pocketverbs->alpYL < 0 || pocketverbs->alpYL > pocketverbs->delayY) {
					pocketverbs->alpYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->aYL[pocketverbs->alpYL]);
				// allpass filter Y

				pocketverbs->dYL[2] = pocketverbs->dYL[1];
				pocketverbs->dYL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dYL[2]) / 2.0;

				allpasstemp = pocketverbs->alpZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->aZL[allpasstemp] * constallpass;
				pocketverbs->aZL[pocketverbs->alpZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpZL--;
				if (pocketverbs->alpZL < 0 || pocketverbs->alpZL > pocketverbs->delayZ) {
					pocketverbs->alpZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->aZL[pocketverbs->alpZL]);
				// allpass filter Z

				pocketverbs->dZL[2] = pocketverbs->dZL[1];
				pocketverbs->dZL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[1] + pocketverbs->dZL[2]) / 2.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->oAL[allpasstemp] * constallpass;
				pocketverbs->oAL[pocketverbs->outAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outAL--;
				if (pocketverbs->outAL < 0 || pocketverbs->outAL > pocketverbs->delayA) {
					pocketverbs->outAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->oAL[pocketverbs->outAL]);
				// allpass filter A

				pocketverbs->dAL[5] = pocketverbs->dAL[4];
				pocketverbs->dAL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[4] + pocketverbs->dAL[5]) / 2.0;

				allpasstemp = pocketverbs->outBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->oBL[allpasstemp] * constallpass;
				pocketverbs->oBL[pocketverbs->outBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outBL--;
				if (pocketverbs->outBL < 0 || pocketverbs->outBL > pocketverbs->delayB) {
					pocketverbs->outBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->oBL[pocketverbs->outBL]);
				// allpass filter B

				pocketverbs->dBL[5] = pocketverbs->dBL[4];
				pocketverbs->dBL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[4] + pocketverbs->dBL[5]) / 2.0;

				allpasstemp = pocketverbs->outCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->oCL[allpasstemp] * constallpass;
				pocketverbs->oCL[pocketverbs->outCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outCL--;
				if (pocketverbs->outCL < 0 || pocketverbs->outCL > pocketverbs->delayC) {
					pocketverbs->outCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->oCL[pocketverbs->outCL]);
				// allpass filter C

				pocketverbs->dCL[5] = pocketverbs->dCL[4];
				pocketverbs->dCL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dCL[5]) / 2.0;

				allpasstemp = pocketverbs->outDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->oDL[allpasstemp] * constallpass;
				pocketverbs->oDL[pocketverbs->outDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outDL--;
				if (pocketverbs->outDL < 0 || pocketverbs->outDL > pocketverbs->delayD) {
					pocketverbs->outDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->oDL[pocketverbs->outDL]);
				// allpass filter D

				pocketverbs->dDL[5] = pocketverbs->dDL[4];
				pocketverbs->dDL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[4] + pocketverbs->dDL[5]) / 2.0;

				allpasstemp = pocketverbs->outEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->oEL[allpasstemp] * constallpass;
				pocketverbs->oEL[pocketverbs->outEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outEL--;
				if (pocketverbs->outEL < 0 || pocketverbs->outEL > pocketverbs->delayE) {
					pocketverbs->outEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->oEL[pocketverbs->outEL]);
				// allpass filter E

				pocketverbs->dEL[5] = pocketverbs->dEL[4];
				pocketverbs->dEL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dEL[5]) / 2.0;

				allpasstemp = pocketverbs->outFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->oFL[allpasstemp] * constallpass;
				pocketverbs->oFL[pocketverbs->outFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outFL--;
				if (pocketverbs->outFL < 0 || pocketverbs->outFL > pocketverbs->delayF) {
					pocketverbs->outFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->oFL[pocketverbs->outFL]);
				// allpass filter F

				pocketverbs->dFL[5] = pocketverbs->dFL[4];
				pocketverbs->dFL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[4] + pocketverbs->dFL[5]) / 2.0;

				allpasstemp = pocketverbs->outGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->oGL[allpasstemp] * constallpass;
				pocketverbs->oGL[pocketverbs->outGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outGL--;
				if (pocketverbs->outGL < 0 || pocketverbs->outGL > pocketverbs->delayG) {
					pocketverbs->outGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->oGL[pocketverbs->outGL]);
				// allpass filter G

				pocketverbs->dGL[5] = pocketverbs->dGL[4];
				pocketverbs->dGL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[4] + pocketverbs->dGL[5]) / 2.0;

				allpasstemp = pocketverbs->outHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->oHL[allpasstemp] * constallpass;
				pocketverbs->oHL[pocketverbs->outHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outHL--;
				if (pocketverbs->outHL < 0 || pocketverbs->outHL > pocketverbs->delayH) {
					pocketverbs->outHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->oHL[pocketverbs->outHL]);
				// allpass filter H

				pocketverbs->dHL[5] = pocketverbs->dHL[4];
				pocketverbs->dHL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[4] + pocketverbs->dHL[5]) / 2.0;

				allpasstemp = pocketverbs->outIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->oIL[allpasstemp] * constallpass;
				pocketverbs->oIL[pocketverbs->outIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outIL--;
				if (pocketverbs->outIL < 0 || pocketverbs->outIL > pocketverbs->delayI) {
					pocketverbs->outIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->oIL[pocketverbs->outIL]);
				// allpass filter I

				pocketverbs->dIL[5] = pocketverbs->dIL[4];
				pocketverbs->dIL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[4] + pocketverbs->dIL[5]) / 2.0;

				allpasstemp = pocketverbs->outJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->oJL[allpasstemp] * constallpass;
				pocketverbs->oJL[pocketverbs->outJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outJL--;
				if (pocketverbs->outJL < 0 || pocketverbs->outJL > pocketverbs->delayJ) {
					pocketverbs->outJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->oJL[pocketverbs->outJL]);
				// allpass filter J

				pocketverbs->dJL[5] = pocketverbs->dJL[4];
				pocketverbs->dJL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[4] + pocketverbs->dJL[5]) / 2.0;

				allpasstemp = pocketverbs->outKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->oKL[allpasstemp] * constallpass;
				pocketverbs->oKL[pocketverbs->outKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outKL--;
				if (pocketverbs->outKL < 0 || pocketverbs->outKL > pocketverbs->delayK) {
					pocketverbs->outKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->oKL[pocketverbs->outKL]);
				// allpass filter K

				pocketverbs->dKL[5] = pocketverbs->dKL[4];
				pocketverbs->dKL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dKL[5]) / 2.0;

				allpasstemp = pocketverbs->outLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->oLL[allpasstemp] * constallpass;
				pocketverbs->oLL[pocketverbs->outLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outLL--;
				if (pocketverbs->outLL < 0 || pocketverbs->outLL > pocketverbs->delayL) {
					pocketverbs->outLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->oLL[pocketverbs->outLL]);
				// allpass filter L

				pocketverbs->dLL[5] = pocketverbs->dLL[4];
				pocketverbs->dLL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[4] + pocketverbs->dLL[5]) / 2.0;

				allpasstemp = pocketverbs->outML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->oML[allpasstemp] * constallpass;
				pocketverbs->oML[pocketverbs->outML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outML--;
				if (pocketverbs->outML < 0 || pocketverbs->outML > pocketverbs->delayM) {
					pocketverbs->outML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->oML[pocketverbs->outML]);
				// allpass filter M

				pocketverbs->dML[5] = pocketverbs->dML[4];
				pocketverbs->dML[4] = inputSampleL;
				inputSampleL = (pocketverbs->dML[4] + pocketverbs->dML[5]) / 2.0;

				allpasstemp = pocketverbs->outNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->oNL[allpasstemp] * constallpass;
				pocketverbs->oNL[pocketverbs->outNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outNL--;
				if (pocketverbs->outNL < 0 || pocketverbs->outNL > pocketverbs->delayN) {
					pocketverbs->outNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->oNL[pocketverbs->outNL]);
				// allpass filter N

				pocketverbs->dNL[5] = pocketverbs->dNL[4];
				pocketverbs->dNL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[4] + pocketverbs->dNL[5]) / 2.0;

				allpasstemp = pocketverbs->outOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->oOL[allpasstemp] * constallpass;
				pocketverbs->oOL[pocketverbs->outOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outOL--;
				if (pocketverbs->outOL < 0 || pocketverbs->outOL > pocketverbs->delayO) {
					pocketverbs->outOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->oOL[pocketverbs->outOL]);
				// allpass filter O

				pocketverbs->dOL[5] = pocketverbs->dOL[4];
				pocketverbs->dOL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dOL[5]) / 2.0;

				allpasstemp = pocketverbs->outPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->oPL[allpasstemp] * constallpass;
				pocketverbs->oPL[pocketverbs->outPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outPL--;
				if (pocketverbs->outPL < 0 || pocketverbs->outPL > pocketverbs->delayP) {
					pocketverbs->outPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->oPL[pocketverbs->outPL]);
				// allpass filter P

				pocketverbs->dPL[5] = pocketverbs->dPL[4];
				pocketverbs->dPL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[4] + pocketverbs->dPL[5]) / 2.0;

				allpasstemp = pocketverbs->outQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->oQL[allpasstemp] * constallpass;
				pocketverbs->oQL[pocketverbs->outQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outQL--;
				if (pocketverbs->outQL < 0 || pocketverbs->outQL > pocketverbs->delayQ) {
					pocketverbs->outQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->oQL[pocketverbs->outQL]);
				// allpass filter Q

				pocketverbs->dQL[5] = pocketverbs->dQL[4];
				pocketverbs->dQL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dQL[5]) / 2.0;

				allpasstemp = pocketverbs->outRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->oRL[allpasstemp] * constallpass;
				pocketverbs->oRL[pocketverbs->outRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outRL--;
				if (pocketverbs->outRL < 0 || pocketverbs->outRL > pocketverbs->delayR) {
					pocketverbs->outRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->oRL[pocketverbs->outRL]);
				// allpass filter R

				pocketverbs->dRL[5] = pocketverbs->dRL[4];
				pocketverbs->dRL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[4] + pocketverbs->dRL[5]) / 2.0;

				allpasstemp = pocketverbs->outSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->oSL[allpasstemp] * constallpass;
				pocketverbs->oSL[pocketverbs->outSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outSL--;
				if (pocketverbs->outSL < 0 || pocketverbs->outSL > pocketverbs->delayS) {
					pocketverbs->outSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->oSL[pocketverbs->outSL]);
				// allpass filter S

				pocketverbs->dSL[5] = pocketverbs->dSL[4];
				pocketverbs->dSL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[4] + pocketverbs->dSL[5]) / 2.0;

				allpasstemp = pocketverbs->outTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->oTL[allpasstemp] * constallpass;
				pocketverbs->oTL[pocketverbs->outTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outTL--;
				if (pocketverbs->outTL < 0 || pocketverbs->outTL > pocketverbs->delayT) {
					pocketverbs->outTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->oTL[pocketverbs->outTL]);
				// allpass filter T

				pocketverbs->dTL[5] = pocketverbs->dTL[4];
				pocketverbs->dTL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[4] + pocketverbs->dTL[5]) / 2.0;

				allpasstemp = pocketverbs->outUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->oUL[allpasstemp] * constallpass;
				pocketverbs->oUL[pocketverbs->outUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outUL--;
				if (pocketverbs->outUL < 0 || pocketverbs->outUL > pocketverbs->delayU) {
					pocketverbs->outUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->oUL[pocketverbs->outUL]);
				// allpass filter U

				pocketverbs->dUL[5] = pocketverbs->dUL[4];
				pocketverbs->dUL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dUL[5]) / 2.0;

				allpasstemp = pocketverbs->outVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->oVL[allpasstemp] * constallpass;
				pocketverbs->oVL[pocketverbs->outVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outVL--;
				if (pocketverbs->outVL < 0 || pocketverbs->outVL > pocketverbs->delayV) {
					pocketverbs->outVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->oVL[pocketverbs->outVL]);
				// allpass filter V

				pocketverbs->dVL[5] = pocketverbs->dVL[4];
				pocketverbs->dVL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[4] + pocketverbs->dVL[5]) / 2.0;

				allpasstemp = pocketverbs->outWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->oWL[allpasstemp] * constallpass;
				pocketverbs->oWL[pocketverbs->outWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outWL--;
				if (pocketverbs->outWL < 0 || pocketverbs->outWL > pocketverbs->delayW) {
					pocketverbs->outWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->oWL[pocketverbs->outWL]);
				// allpass filter W

				pocketverbs->dWL[5] = pocketverbs->dWL[4];
				pocketverbs->dWL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[4] + pocketverbs->dWL[5]) / 2.0;

				allpasstemp = pocketverbs->outXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->oXL[allpasstemp] * constallpass;
				pocketverbs->oXL[pocketverbs->outXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outXL--;
				if (pocketverbs->outXL < 0 || pocketverbs->outXL > pocketverbs->delayX) {
					pocketverbs->outXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->oXL[pocketverbs->outXL]);
				// allpass filter X

				pocketverbs->dXL[5] = pocketverbs->dXL[4];
				pocketverbs->dXL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[4] + pocketverbs->dXL[5]) / 2.0;

				allpasstemp = pocketverbs->outYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->oYL[allpasstemp] * constallpass;
				pocketverbs->oYL[pocketverbs->outYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outYL--;
				if (pocketverbs->outYL < 0 || pocketverbs->outYL > pocketverbs->delayY) {
					pocketverbs->outYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->oYL[pocketverbs->outYL]);
				// allpass filter Y

				pocketverbs->dYL[5] = pocketverbs->dYL[4];
				pocketverbs->dYL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[4] + pocketverbs->dYL[5]) / 2.0;

				allpasstemp = pocketverbs->outZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->oZL[allpasstemp] * constallpass;
				pocketverbs->oZL[pocketverbs->outZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outZL--;
				if (pocketverbs->outZL < 0 || pocketverbs->outZL > pocketverbs->delayZ) {
					pocketverbs->outZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->oZL[pocketverbs->outZL]);
				// allpass filter Z

				pocketverbs->dZL[5] = pocketverbs->dZL[4];
				pocketverbs->dZL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[4] + pocketverbs->dZL[5]);
				// output Tiled
				break;

			case 4: // Room
				allpasstemp = pocketverbs->alpAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->aAL[allpasstemp] * constallpass;
				pocketverbs->aAL[pocketverbs->alpAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpAL--;
				if (pocketverbs->alpAL < 0 || pocketverbs->alpAL > pocketverbs->delayA) {
					pocketverbs->alpAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->aAL[pocketverbs->alpAL]);
				// allpass filter A

				pocketverbs->dAL[2] = pocketverbs->dAL[1];
				pocketverbs->dAL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->aBL[allpasstemp] * constallpass;
				pocketverbs->aBL[pocketverbs->alpBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpBL--;
				if (pocketverbs->alpBL < 0 || pocketverbs->alpBL > pocketverbs->delayB) {
					pocketverbs->alpBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->aBL[pocketverbs->alpBL]);
				// allpass filter B

				pocketverbs->dBL[2] = pocketverbs->dBL[1];
				pocketverbs->dBL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->aCL[allpasstemp] * constallpass;
				pocketverbs->aCL[pocketverbs->alpCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpCL--;
				if (pocketverbs->alpCL < 0 || pocketverbs->alpCL > pocketverbs->delayC) {
					pocketverbs->alpCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->aCL[pocketverbs->alpCL]);
				// allpass filter C

				pocketverbs->dCL[2] = pocketverbs->dCL[1];
				pocketverbs->dCL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->aDL[allpasstemp] * constallpass;
				pocketverbs->aDL[pocketverbs->alpDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpDL--;
				if (pocketverbs->alpDL < 0 || pocketverbs->alpDL > pocketverbs->delayD) {
					pocketverbs->alpDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->aDL[pocketverbs->alpDL]);
				// allpass filter D

				pocketverbs->dDL[2] = pocketverbs->dDL[1];
				pocketverbs->dDL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->aEL[allpasstemp] * constallpass;
				pocketverbs->aEL[pocketverbs->alpEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpEL--;
				if (pocketverbs->alpEL < 0 || pocketverbs->alpEL > pocketverbs->delayE) {
					pocketverbs->alpEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->aEL[pocketverbs->alpEL]);
				// allpass filter E

				pocketverbs->dEL[2] = pocketverbs->dEL[1];
				pocketverbs->dEL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->aFL[allpasstemp] * constallpass;
				pocketverbs->aFL[pocketverbs->alpFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpFL--;
				if (pocketverbs->alpFL < 0 || pocketverbs->alpFL > pocketverbs->delayF) {
					pocketverbs->alpFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->aFL[pocketverbs->alpFL]);
				// allpass filter F

				pocketverbs->dFL[2] = pocketverbs->dFL[1];
				pocketverbs->dFL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->aGL[allpasstemp] * constallpass;
				pocketverbs->aGL[pocketverbs->alpGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpGL--;
				if (pocketverbs->alpGL < 0 || pocketverbs->alpGL > pocketverbs->delayG) {
					pocketverbs->alpGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->aGL[pocketverbs->alpGL]);
				// allpass filter G

				pocketverbs->dGL[2] = pocketverbs->dGL[1];
				pocketverbs->dGL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->aHL[allpasstemp] * constallpass;
				pocketverbs->aHL[pocketverbs->alpHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpHL--;
				if (pocketverbs->alpHL < 0 || pocketverbs->alpHL > pocketverbs->delayH) {
					pocketverbs->alpHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->aHL[pocketverbs->alpHL]);
				// allpass filter H

				pocketverbs->dHL[2] = pocketverbs->dHL[1];
				pocketverbs->dHL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->aIL[allpasstemp] * constallpass;
				pocketverbs->aIL[pocketverbs->alpIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpIL--;
				if (pocketverbs->alpIL < 0 || pocketverbs->alpIL > pocketverbs->delayI) {
					pocketverbs->alpIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->aIL[pocketverbs->alpIL]);
				// allpass filter I

				pocketverbs->dIL[2] = pocketverbs->dIL[1];
				pocketverbs->dIL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->aJL[allpasstemp] * constallpass;
				pocketverbs->aJL[pocketverbs->alpJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpJL--;
				if (pocketverbs->alpJL < 0 || pocketverbs->alpJL > pocketverbs->delayJ) {
					pocketverbs->alpJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->aJL[pocketverbs->alpJL]);
				// allpass filter J

				pocketverbs->dJL[2] = pocketverbs->dJL[1];
				pocketverbs->dJL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->aKL[allpasstemp] * constallpass;
				pocketverbs->aKL[pocketverbs->alpKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpKL--;
				if (pocketverbs->alpKL < 0 || pocketverbs->alpKL > pocketverbs->delayK) {
					pocketverbs->alpKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->aKL[pocketverbs->alpKL]);
				// allpass filter K

				pocketverbs->dKL[2] = pocketverbs->dKL[1];
				pocketverbs->dKL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->aLL[allpasstemp] * constallpass;
				pocketverbs->aLL[pocketverbs->alpLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpLL--;
				if (pocketverbs->alpLL < 0 || pocketverbs->alpLL > pocketverbs->delayL) {
					pocketverbs->alpLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->aLL[pocketverbs->alpLL]);
				// allpass filter L

				pocketverbs->dLL[2] = pocketverbs->dLL[1];
				pocketverbs->dLL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->aML[allpasstemp] * constallpass;
				pocketverbs->aML[pocketverbs->alpML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpML--;
				if (pocketverbs->alpML < 0 || pocketverbs->alpML > pocketverbs->delayM) {
					pocketverbs->alpML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->aML[pocketverbs->alpML]);
				// allpass filter M

				pocketverbs->dML[2] = pocketverbs->dML[1];
				pocketverbs->dML[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->aNL[allpasstemp] * constallpass;
				pocketverbs->aNL[pocketverbs->alpNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpNL--;
				if (pocketverbs->alpNL < 0 || pocketverbs->alpNL > pocketverbs->delayN) {
					pocketverbs->alpNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->aNL[pocketverbs->alpNL]);
				// allpass filter N

				pocketverbs->dNL[2] = pocketverbs->dNL[1];
				pocketverbs->dNL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->aOL[allpasstemp] * constallpass;
				pocketverbs->aOL[pocketverbs->alpOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpOL--;
				if (pocketverbs->alpOL < 0 || pocketverbs->alpOL > pocketverbs->delayO) {
					pocketverbs->alpOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->aOL[pocketverbs->alpOL]);
				// allpass filter O

				pocketverbs->dOL[2] = pocketverbs->dOL[1];
				pocketverbs->dOL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->aPL[allpasstemp] * constallpass;
				pocketverbs->aPL[pocketverbs->alpPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpPL--;
				if (pocketverbs->alpPL < 0 || pocketverbs->alpPL > pocketverbs->delayP) {
					pocketverbs->alpPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->aPL[pocketverbs->alpPL]);
				// allpass filter P

				pocketverbs->dPL[2] = pocketverbs->dPL[1];
				pocketverbs->dPL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->aQL[allpasstemp] * constallpass;
				pocketverbs->aQL[pocketverbs->alpQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpQL--;
				if (pocketverbs->alpQL < 0 || pocketverbs->alpQL > pocketverbs->delayQ) {
					pocketverbs->alpQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->aQL[pocketverbs->alpQL]);
				// allpass filter Q

				pocketverbs->dQL[2] = pocketverbs->dQL[1];
				pocketverbs->dQL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->aRL[allpasstemp] * constallpass;
				pocketverbs->aRL[pocketverbs->alpRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpRL--;
				if (pocketverbs->alpRL < 0 || pocketverbs->alpRL > pocketverbs->delayR) {
					pocketverbs->alpRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->aRL[pocketverbs->alpRL]);
				// allpass filter R

				pocketverbs->dRL[2] = pocketverbs->dRL[1];
				pocketverbs->dRL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->aSL[allpasstemp] * constallpass;
				pocketverbs->aSL[pocketverbs->alpSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpSL--;
				if (pocketverbs->alpSL < 0 || pocketverbs->alpSL > pocketverbs->delayS) {
					pocketverbs->alpSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->aSL[pocketverbs->alpSL]);
				// allpass filter S

				pocketverbs->dSL[2] = pocketverbs->dSL[1];
				pocketverbs->dSL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->aTL[allpasstemp] * constallpass;
				pocketverbs->aTL[pocketverbs->alpTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpTL--;
				if (pocketverbs->alpTL < 0 || pocketverbs->alpTL > pocketverbs->delayT) {
					pocketverbs->alpTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->aTL[pocketverbs->alpTL]);
				// allpass filter T

				pocketverbs->dTL[2] = pocketverbs->dTL[1];
				pocketverbs->dTL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->aUL[allpasstemp] * constallpass;
				pocketverbs->aUL[pocketverbs->alpUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpUL--;
				if (pocketverbs->alpUL < 0 || pocketverbs->alpUL > pocketverbs->delayU) {
					pocketverbs->alpUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->aUL[pocketverbs->alpUL]);
				// allpass filter U

				pocketverbs->dUL[2] = pocketverbs->dUL[1];
				pocketverbs->dUL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->aVL[allpasstemp] * constallpass;
				pocketverbs->aVL[pocketverbs->alpVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpVL--;
				if (pocketverbs->alpVL < 0 || pocketverbs->alpVL > pocketverbs->delayV) {
					pocketverbs->alpVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->aVL[pocketverbs->alpVL]);
				// allpass filter V

				pocketverbs->dVL[2] = pocketverbs->dVL[1];
				pocketverbs->dVL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->aWL[allpasstemp] * constallpass;
				pocketverbs->aWL[pocketverbs->alpWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpWL--;
				if (pocketverbs->alpWL < 0 || pocketverbs->alpWL > pocketverbs->delayW) {
					pocketverbs->alpWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->aWL[pocketverbs->alpWL]);
				// allpass filter W

				pocketverbs->dWL[2] = pocketverbs->dWL[1];
				pocketverbs->dWL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->aXL[allpasstemp] * constallpass;
				pocketverbs->aXL[pocketverbs->alpXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpXL--;
				if (pocketverbs->alpXL < 0 || pocketverbs->alpXL > pocketverbs->delayX) {
					pocketverbs->alpXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->aXL[pocketverbs->alpXL]);
				// allpass filter X

				pocketverbs->dXL[2] = pocketverbs->dXL[1];
				pocketverbs->dXL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->aYL[allpasstemp] * constallpass;
				pocketverbs->aYL[pocketverbs->alpYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpYL--;
				if (pocketverbs->alpYL < 0 || pocketverbs->alpYL > pocketverbs->delayY) {
					pocketverbs->alpYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->aYL[pocketverbs->alpYL]);
				// allpass filter Y

				pocketverbs->dYL[2] = pocketverbs->dYL[1];
				pocketverbs->dYL[1] = inputSampleL;
				inputSampleL = drySampleL;

				allpasstemp = pocketverbs->alpZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->aZL[allpasstemp] * constallpass;
				pocketverbs->aZL[pocketverbs->alpZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpZL--;
				if (pocketverbs->alpZL < 0 || pocketverbs->alpZL > pocketverbs->delayZ) {
					pocketverbs->alpZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->aZL[pocketverbs->alpZL]);
				// allpass filter Z

				pocketverbs->dZL[2] = pocketverbs->dZL[1];
				pocketverbs->dZL[1] = inputSampleL;
				inputSampleL = drySampleL;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->oAL[allpasstemp] * constallpass;
				pocketverbs->oAL[pocketverbs->outAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outAL--;
				if (pocketverbs->outAL < 0 || pocketverbs->outAL > pocketverbs->delayA) {
					pocketverbs->outAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->oAL[pocketverbs->outAL]);
				// allpass filter A

				pocketverbs->dAL[5] = pocketverbs->dAL[4];
				pocketverbs->dAL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dAL[2]) / 2.0;

				allpasstemp = pocketverbs->outBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->oBL[allpasstemp] * constallpass;
				pocketverbs->oBL[pocketverbs->outBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outBL--;
				if (pocketverbs->outBL < 0 || pocketverbs->outBL > pocketverbs->delayB) {
					pocketverbs->outBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->oBL[pocketverbs->outBL]);
				// allpass filter B

				pocketverbs->dBL[5] = pocketverbs->dBL[4];
				pocketverbs->dBL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dBL[2]) / 2.0;

				allpasstemp = pocketverbs->outCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->oCL[allpasstemp] * constallpass;
				pocketverbs->oCL[pocketverbs->outCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outCL--;
				if (pocketverbs->outCL < 0 || pocketverbs->outCL > pocketverbs->delayC) {
					pocketverbs->outCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->oCL[pocketverbs->outCL]);
				// allpass filter C

				pocketverbs->dCL[5] = pocketverbs->dCL[4];
				pocketverbs->dCL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[1] + pocketverbs->dCL[2]) / 2.0;

				allpasstemp = pocketverbs->outDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->oDL[allpasstemp] * constallpass;
				pocketverbs->oDL[pocketverbs->outDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outDL--;
				if (pocketverbs->outDL < 0 || pocketverbs->outDL > pocketverbs->delayD) {
					pocketverbs->outDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->oDL[pocketverbs->outDL]);
				// allpass filter D

				pocketverbs->dDL[5] = pocketverbs->dDL[4];
				pocketverbs->dDL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dDL[2]) / 2.0;

				allpasstemp = pocketverbs->outEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->oEL[allpasstemp] * constallpass;
				pocketverbs->oEL[pocketverbs->outEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outEL--;
				if (pocketverbs->outEL < 0 || pocketverbs->outEL > pocketverbs->delayE) {
					pocketverbs->outEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->oEL[pocketverbs->outEL]);
				// allpass filter E

				pocketverbs->dEL[5] = pocketverbs->dEL[4];
				pocketverbs->dEL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[1] + pocketverbs->dEL[2]) / 2.0;

				allpasstemp = pocketverbs->outFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->oFL[allpasstemp] * constallpass;
				pocketverbs->oFL[pocketverbs->outFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outFL--;
				if (pocketverbs->outFL < 0 || pocketverbs->outFL > pocketverbs->delayF) {
					pocketverbs->outFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->oFL[pocketverbs->outFL]);
				// allpass filter F

				pocketverbs->dFL[5] = pocketverbs->dFL[4];
				pocketverbs->dFL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dFL[2]) / 2.0;

				allpasstemp = pocketverbs->outGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->oGL[allpasstemp] * constallpass;
				pocketverbs->oGL[pocketverbs->outGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outGL--;
				if (pocketverbs->outGL < 0 || pocketverbs->outGL > pocketverbs->delayG) {
					pocketverbs->outGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->oGL[pocketverbs->outGL]);
				// allpass filter G

				pocketverbs->dGL[5] = pocketverbs->dGL[4];
				pocketverbs->dGL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[1] + pocketverbs->dGL[2]) / 2.0;

				allpasstemp = pocketverbs->outHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->oHL[allpasstemp] * constallpass;
				pocketverbs->oHL[pocketverbs->outHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outHL--;
				if (pocketverbs->outHL < 0 || pocketverbs->outHL > pocketverbs->delayH) {
					pocketverbs->outHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->oHL[pocketverbs->outHL]);
				// allpass filter H

				pocketverbs->dHL[5] = pocketverbs->dHL[4];
				pocketverbs->dHL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dHL[2]) / 2.0;

				allpasstemp = pocketverbs->outIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->oIL[allpasstemp] * constallpass;
				pocketverbs->oIL[pocketverbs->outIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outIL--;
				if (pocketverbs->outIL < 0 || pocketverbs->outIL > pocketverbs->delayI) {
					pocketverbs->outIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->oIL[pocketverbs->outIL]);
				// allpass filter I

				pocketverbs->dIL[5] = pocketverbs->dIL[4];
				pocketverbs->dIL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dIL[2]) / 2.0;

				allpasstemp = pocketverbs->outJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->oJL[allpasstemp] * constallpass;
				pocketverbs->oJL[pocketverbs->outJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outJL--;
				if (pocketverbs->outJL < 0 || pocketverbs->outJL > pocketverbs->delayJ) {
					pocketverbs->outJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->oJL[pocketverbs->outJL]);
				// allpass filter J

				pocketverbs->dJL[5] = pocketverbs->dJL[4];
				pocketverbs->dJL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dJL[2]) / 2.0;

				allpasstemp = pocketverbs->outKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->oKL[allpasstemp] * constallpass;
				pocketverbs->oKL[pocketverbs->outKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outKL--;
				if (pocketverbs->outKL < 0 || pocketverbs->outKL > pocketverbs->delayK) {
					pocketverbs->outKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->oKL[pocketverbs->outKL]);
				// allpass filter K

				pocketverbs->dKL[5] = pocketverbs->dKL[4];
				pocketverbs->dKL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[1] + pocketverbs->dKL[2]) / 2.0;

				allpasstemp = pocketverbs->outLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->oLL[allpasstemp] * constallpass;
				pocketverbs->oLL[pocketverbs->outLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outLL--;
				if (pocketverbs->outLL < 0 || pocketverbs->outLL > pocketverbs->delayL) {
					pocketverbs->outLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->oLL[pocketverbs->outLL]);
				// allpass filter L

				pocketverbs->dLL[5] = pocketverbs->dLL[4];
				pocketverbs->dLL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dLL[2]) / 2.0;

				allpasstemp = pocketverbs->outML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->oML[allpasstemp] * constallpass;
				pocketverbs->oML[pocketverbs->outML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outML--;
				if (pocketverbs->outML < 0 || pocketverbs->outML > pocketverbs->delayM) {
					pocketverbs->outML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->oML[pocketverbs->outML]);
				// allpass filter M

				pocketverbs->dML[5] = pocketverbs->dML[4];
				pocketverbs->dML[4] = inputSampleL;
				inputSampleL = (pocketverbs->dML[1] + pocketverbs->dML[2]) / 2.0;

				allpasstemp = pocketverbs->outNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->oNL[allpasstemp] * constallpass;
				pocketverbs->oNL[pocketverbs->outNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outNL--;
				if (pocketverbs->outNL < 0 || pocketverbs->outNL > pocketverbs->delayN) {
					pocketverbs->outNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->oNL[pocketverbs->outNL]);
				// allpass filter N

				pocketverbs->dNL[5] = pocketverbs->dNL[4];
				pocketverbs->dNL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dNL[2]) / 2.0;

				allpasstemp = pocketverbs->outOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->oOL[allpasstemp] * constallpass;
				pocketverbs->oOL[pocketverbs->outOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outOL--;
				if (pocketverbs->outOL < 0 || pocketverbs->outOL > pocketverbs->delayO) {
					pocketverbs->outOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->oOL[pocketverbs->outOL]);
				// allpass filter O

				pocketverbs->dOL[5] = pocketverbs->dOL[4];
				pocketverbs->dOL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dOL[2]) / 2.0;

				allpasstemp = pocketverbs->outPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->oPL[allpasstemp] * constallpass;
				pocketverbs->oPL[pocketverbs->outPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outPL--;
				if (pocketverbs->outPL < 0 || pocketverbs->outPL > pocketverbs->delayP) {
					pocketverbs->outPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->oPL[pocketverbs->outPL]);
				// allpass filter P

				pocketverbs->dPL[5] = pocketverbs->dPL[4];
				pocketverbs->dPL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dPL[2]) / 2.0;

				allpasstemp = pocketverbs->outQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->oQL[allpasstemp] * constallpass;
				pocketverbs->oQL[pocketverbs->outQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outQL--;
				if (pocketverbs->outQL < 0 || pocketverbs->outQL > pocketverbs->delayQ) {
					pocketverbs->outQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->oQL[pocketverbs->outQL]);
				// allpass filter Q

				pocketverbs->dQL[5] = pocketverbs->dQL[4];
				pocketverbs->dQL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[1] + pocketverbs->dQL[2]) / 2.0;

				allpasstemp = pocketverbs->outRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->oRL[allpasstemp] * constallpass;
				pocketverbs->oRL[pocketverbs->outRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outRL--;
				if (pocketverbs->outRL < 0 || pocketverbs->outRL > pocketverbs->delayR) {
					pocketverbs->outRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->oRL[pocketverbs->outRL]);
				// allpass filter R

				pocketverbs->dRL[5] = pocketverbs->dRL[4];
				pocketverbs->dRL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dRL[2]) / 2.0;

				allpasstemp = pocketverbs->outSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->oSL[allpasstemp] * constallpass;
				pocketverbs->oSL[pocketverbs->outSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outSL--;
				if (pocketverbs->outSL < 0 || pocketverbs->outSL > pocketverbs->delayS) {
					pocketverbs->outSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->oSL[pocketverbs->outSL]);
				// allpass filter S

				pocketverbs->dSL[5] = pocketverbs->dSL[4];
				pocketverbs->dSL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[1] + pocketverbs->dSL[2]) / 2.0;

				allpasstemp = pocketverbs->outTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->oTL[allpasstemp] * constallpass;
				pocketverbs->oTL[pocketverbs->outTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outTL--;
				if (pocketverbs->outTL < 0 || pocketverbs->outTL > pocketverbs->delayT) {
					pocketverbs->outTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->oTL[pocketverbs->outTL]);
				// allpass filter T

				pocketverbs->dTL[5] = pocketverbs->dTL[4];
				pocketverbs->dTL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dTL[2]) / 2.0;

				allpasstemp = pocketverbs->outUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->oUL[allpasstemp] * constallpass;
				pocketverbs->oUL[pocketverbs->outUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outUL--;
				if (pocketverbs->outUL < 0 || pocketverbs->outUL > pocketverbs->delayU) {
					pocketverbs->outUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->oUL[pocketverbs->outUL]);
				// allpass filter U

				pocketverbs->dUL[5] = pocketverbs->dUL[4];
				pocketverbs->dUL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dUL[2]) / 2.0;

				allpasstemp = pocketverbs->outVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->oVL[allpasstemp] * constallpass;
				pocketverbs->oVL[pocketverbs->outVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outVL--;
				if (pocketverbs->outVL < 0 || pocketverbs->outVL > pocketverbs->delayV) {
					pocketverbs->outVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->oVL[pocketverbs->outVL]);
				// allpass filter V

				pocketverbs->dVL[5] = pocketverbs->dVL[4];
				pocketverbs->dVL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dVL[2]) / 2.0;

				allpasstemp = pocketverbs->outWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->oWL[allpasstemp] * constallpass;
				pocketverbs->oWL[pocketverbs->outWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outWL--;
				if (pocketverbs->outWL < 0 || pocketverbs->outWL > pocketverbs->delayW) {
					pocketverbs->outWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->oWL[pocketverbs->outWL]);
				// allpass filter W

				pocketverbs->dWL[5] = pocketverbs->dWL[4];
				pocketverbs->dWL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[1] + pocketverbs->dWL[2]) / 2.0;

				allpasstemp = pocketverbs->outXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->oXL[allpasstemp] * constallpass;
				pocketverbs->oXL[pocketverbs->outXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outXL--;
				if (pocketverbs->outXL < 0 || pocketverbs->outXL > pocketverbs->delayX) {
					pocketverbs->outXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->oXL[pocketverbs->outXL]);
				// allpass filter X

				pocketverbs->dXL[5] = pocketverbs->dXL[4];
				pocketverbs->dXL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dXL[2]) / 2.0;

				allpasstemp = pocketverbs->outYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->oYL[allpasstemp] * constallpass;
				pocketverbs->oYL[pocketverbs->outYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outYL--;
				if (pocketverbs->outYL < 0 || pocketverbs->outYL > pocketverbs->delayY) {
					pocketverbs->outYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->oYL[pocketverbs->outYL]);
				// allpass filter Y

				pocketverbs->dYL[5] = pocketverbs->dYL[4];
				pocketverbs->dYL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dYL[2]) / 2.0;

				allpasstemp = pocketverbs->outZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->oZL[allpasstemp] * constallpass;
				pocketverbs->oZL[pocketverbs->outZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outZL--;
				if (pocketverbs->outZL < 0 || pocketverbs->outZL > pocketverbs->delayZ) {
					pocketverbs->outZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->oZL[pocketverbs->outZL]);
				// allpass filter Z

				pocketverbs->dZL[5] = pocketverbs->dZL[4];
				pocketverbs->dZL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[4] * dryness);
				inputSampleL += (pocketverbs->dCL[4] * dryness);
				inputSampleL += pocketverbs->dDL[4];
				inputSampleL += pocketverbs->dEL[4];
				inputSampleL += pocketverbs->dFL[4];
				inputSampleL += pocketverbs->dGL[4];
				inputSampleL += pocketverbs->dHL[4];
				inputSampleL += pocketverbs->dIL[4];
				inputSampleL += pocketverbs->dJL[4];
				inputSampleL += pocketverbs->dKL[4];
				inputSampleL += pocketverbs->dLL[4];
				inputSampleL += pocketverbs->dML[4];
				inputSampleL += pocketverbs->dNL[4];
				inputSampleL += pocketverbs->dOL[4];
				inputSampleL += pocketverbs->dPL[4];
				inputSampleL += pocketverbs->dQL[4];
				inputSampleL += pocketverbs->dRL[4];
				inputSampleL += pocketverbs->dSL[4];
				inputSampleL += pocketverbs->dTL[4];
				inputSampleL += pocketverbs->dUL[4];
				inputSampleL += pocketverbs->dVL[4];
				inputSampleL += pocketverbs->dWL[4];
				inputSampleL += pocketverbs->dXL[4];
				inputSampleL += pocketverbs->dYL[4];
				inputSampleL += (pocketverbs->dZL[4] * wetness);

				inputSampleL += (pocketverbs->dBL[5] * dryness);
				inputSampleL += (pocketverbs->dCL[5] * dryness);
				inputSampleL += pocketverbs->dDL[5];
				inputSampleL += pocketverbs->dEL[5];
				inputSampleL += pocketverbs->dFL[5];
				inputSampleL += pocketverbs->dGL[5];
				inputSampleL += pocketverbs->dHL[5];
				inputSampleL += pocketverbs->dIL[5];
				inputSampleL += pocketverbs->dJL[5];
				inputSampleL += pocketverbs->dKL[5];
				inputSampleL += pocketverbs->dLL[5];
				inputSampleL += pocketverbs->dML[5];
				inputSampleL += pocketverbs->dNL[5];
				inputSampleL += pocketverbs->dOL[5];
				inputSampleL += pocketverbs->dPL[5];
				inputSampleL += pocketverbs->dQL[5];
				inputSampleL += pocketverbs->dRL[5];
				inputSampleL += pocketverbs->dSL[5];
				inputSampleL += pocketverbs->dTL[5];
				inputSampleL += pocketverbs->dUL[5];
				inputSampleL += pocketverbs->dVL[5];
				inputSampleL += pocketverbs->dWL[5];
				inputSampleL += pocketverbs->dXL[5];
				inputSampleL += pocketverbs->dYL[5];
				inputSampleL += (pocketverbs->dZL[5] * wetness);

				inputSampleL /= (26.0 + (wetness * 4.0));
				// output Room effect
				break;

			case 5: // Stretch
				allpasstemp = pocketverbs->alpAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->aAL[allpasstemp] * constallpass;
				pocketverbs->aAL[pocketverbs->alpAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpAL--;
				if (pocketverbs->alpAL < 0 || pocketverbs->alpAL > pocketverbs->delayA) {
					pocketverbs->alpAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->aAL[pocketverbs->alpAL]);
				// allpass filter A

				pocketverbs->dAL[2] = pocketverbs->dAL[1];
				pocketverbs->dAL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dAL[2]) / 2.0;

				allpasstemp = pocketverbs->alpBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->aBL[allpasstemp] * constallpass;
				pocketverbs->aBL[pocketverbs->alpBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpBL--;
				if (pocketverbs->alpBL < 0 || pocketverbs->alpBL > pocketverbs->delayB) {
					pocketverbs->alpBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->aBL[pocketverbs->alpBL]);
				// allpass filter B

				pocketverbs->dBL[2] = pocketverbs->dBL[1];
				pocketverbs->dBL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dBL[2]) / 2.0;

				allpasstemp = pocketverbs->alpCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->aCL[allpasstemp] * constallpass;
				pocketverbs->aCL[pocketverbs->alpCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpCL--;
				if (pocketverbs->alpCL < 0 || pocketverbs->alpCL > pocketverbs->delayC) {
					pocketverbs->alpCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->aCL[pocketverbs->alpCL]);
				// allpass filter C

				pocketverbs->dCL[2] = pocketverbs->dCL[1];
				pocketverbs->dCL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[1] + pocketverbs->dCL[2]) / 2.0;

				allpasstemp = pocketverbs->alpDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->aDL[allpasstemp] * constallpass;
				pocketverbs->aDL[pocketverbs->alpDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpDL--;
				if (pocketverbs->alpDL < 0 || pocketverbs->alpDL > pocketverbs->delayD) {
					pocketverbs->alpDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->aDL[pocketverbs->alpDL]);
				// allpass filter D

				pocketverbs->dDL[2] = pocketverbs->dDL[1];
				pocketverbs->dDL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dDL[2]) / 2.0;

				allpasstemp = pocketverbs->alpEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->aEL[allpasstemp] * constallpass;
				pocketverbs->aEL[pocketverbs->alpEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpEL--;
				if (pocketverbs->alpEL < 0 || pocketverbs->alpEL > pocketverbs->delayE) {
					pocketverbs->alpEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->aEL[pocketverbs->alpEL]);
				// allpass filter E

				pocketverbs->dEL[2] = pocketverbs->dEL[1];
				pocketverbs->dEL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[1] + pocketverbs->dEL[2]) / 2.0;

				allpasstemp = pocketverbs->alpFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->aFL[allpasstemp] * constallpass;
				pocketverbs->aFL[pocketverbs->alpFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpFL--;
				if (pocketverbs->alpFL < 0 || pocketverbs->alpFL > pocketverbs->delayF) {
					pocketverbs->alpFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->aFL[pocketverbs->alpFL]);
				// allpass filter F

				pocketverbs->dFL[2] = pocketverbs->dFL[1];
				pocketverbs->dFL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dFL[2]) / 2.0;

				allpasstemp = pocketverbs->alpGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->aGL[allpasstemp] * constallpass;
				pocketverbs->aGL[pocketverbs->alpGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpGL--;
				if (pocketverbs->alpGL < 0 || pocketverbs->alpGL > pocketverbs->delayG) {
					pocketverbs->alpGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->aGL[pocketverbs->alpGL]);
				// allpass filter G

				pocketverbs->dGL[2] = pocketverbs->dGL[1];
				pocketverbs->dGL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[1] + pocketverbs->dGL[2]) / 2.0;

				allpasstemp = pocketverbs->alpHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->aHL[allpasstemp] * constallpass;
				pocketverbs->aHL[pocketverbs->alpHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpHL--;
				if (pocketverbs->alpHL < 0 || pocketverbs->alpHL > pocketverbs->delayH) {
					pocketverbs->alpHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->aHL[pocketverbs->alpHL]);
				// allpass filter H

				pocketverbs->dHL[2] = pocketverbs->dHL[1];
				pocketverbs->dHL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dHL[2]) / 2.0;

				allpasstemp = pocketverbs->alpIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->aIL[allpasstemp] * constallpass;
				pocketverbs->aIL[pocketverbs->alpIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpIL--;
				if (pocketverbs->alpIL < 0 || pocketverbs->alpIL > pocketverbs->delayI) {
					pocketverbs->alpIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->aIL[pocketverbs->alpIL]);
				// allpass filter I

				pocketverbs->dIL[2] = pocketverbs->dIL[1];
				pocketverbs->dIL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dIL[2]) / 2.0;

				allpasstemp = pocketverbs->alpJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->aJL[allpasstemp] * constallpass;
				pocketverbs->aJL[pocketverbs->alpJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpJL--;
				if (pocketverbs->alpJL < 0 || pocketverbs->alpJL > pocketverbs->delayJ) {
					pocketverbs->alpJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->aJL[pocketverbs->alpJL]);
				// allpass filter J

				pocketverbs->dJL[2] = pocketverbs->dJL[1];
				pocketverbs->dJL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dJL[2]) / 2.0;

				allpasstemp = pocketverbs->alpKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->aKL[allpasstemp] * constallpass;
				pocketverbs->aKL[pocketverbs->alpKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpKL--;
				if (pocketverbs->alpKL < 0 || pocketverbs->alpKL > pocketverbs->delayK) {
					pocketverbs->alpKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->aKL[pocketverbs->alpKL]);
				// allpass filter K

				pocketverbs->dKL[2] = pocketverbs->dKL[1];
				pocketverbs->dKL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[1] + pocketverbs->dKL[2]) / 2.0;

				allpasstemp = pocketverbs->alpLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->aLL[allpasstemp] * constallpass;
				pocketverbs->aLL[pocketverbs->alpLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpLL--;
				if (pocketverbs->alpLL < 0 || pocketverbs->alpLL > pocketverbs->delayL) {
					pocketverbs->alpLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->aLL[pocketverbs->alpLL]);
				// allpass filter L

				pocketverbs->dLL[2] = pocketverbs->dLL[1];
				pocketverbs->dLL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dLL[2]) / 2.0;

				allpasstemp = pocketverbs->alpML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->aML[allpasstemp] * constallpass;
				pocketverbs->aML[pocketverbs->alpML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpML--;
				if (pocketverbs->alpML < 0 || pocketverbs->alpML > pocketverbs->delayM) {
					pocketverbs->alpML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->aML[pocketverbs->alpML]);
				// allpass filter M

				pocketverbs->dML[2] = pocketverbs->dML[1];
				pocketverbs->dML[1] = inputSampleL;
				inputSampleL = (pocketverbs->dML[1] + pocketverbs->dML[2]) / 2.0;

				allpasstemp = pocketverbs->alpNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->aNL[allpasstemp] * constallpass;
				pocketverbs->aNL[pocketverbs->alpNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpNL--;
				if (pocketverbs->alpNL < 0 || pocketverbs->alpNL > pocketverbs->delayN) {
					pocketverbs->alpNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->aNL[pocketverbs->alpNL]);
				// allpass filter N

				pocketverbs->dNL[2] = pocketverbs->dNL[1];
				pocketverbs->dNL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dNL[2]) / 2.0;

				allpasstemp = pocketverbs->alpOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->aOL[allpasstemp] * constallpass;
				pocketverbs->aOL[pocketverbs->alpOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpOL--;
				if (pocketverbs->alpOL < 0 || pocketverbs->alpOL > pocketverbs->delayO) {
					pocketverbs->alpOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->aOL[pocketverbs->alpOL]);
				// allpass filter O

				pocketverbs->dOL[2] = pocketverbs->dOL[1];
				pocketverbs->dOL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dOL[2]) / 2.0;

				allpasstemp = pocketverbs->alpPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->aPL[allpasstemp] * constallpass;
				pocketverbs->aPL[pocketverbs->alpPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpPL--;
				if (pocketverbs->alpPL < 0 || pocketverbs->alpPL > pocketverbs->delayP) {
					pocketverbs->alpPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->aPL[pocketverbs->alpPL]);
				// allpass filter P

				pocketverbs->dPL[2] = pocketverbs->dPL[1];
				pocketverbs->dPL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dPL[2]) / 2.0;

				allpasstemp = pocketverbs->alpQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->aQL[allpasstemp] * constallpass;
				pocketverbs->aQL[pocketverbs->alpQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpQL--;
				if (pocketverbs->alpQL < 0 || pocketverbs->alpQL > pocketverbs->delayQ) {
					pocketverbs->alpQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->aQL[pocketverbs->alpQL]);
				// allpass filter Q

				pocketverbs->dQL[2] = pocketverbs->dQL[1];
				pocketverbs->dQL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[1] + pocketverbs->dQL[2]) / 2.0;

				allpasstemp = pocketverbs->alpRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->aRL[allpasstemp] * constallpass;
				pocketverbs->aRL[pocketverbs->alpRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpRL--;
				if (pocketverbs->alpRL < 0 || pocketverbs->alpRL > pocketverbs->delayR) {
					pocketverbs->alpRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->aRL[pocketverbs->alpRL]);
				// allpass filter R

				pocketverbs->dRL[2] = pocketverbs->dRL[1];
				pocketverbs->dRL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dRL[2]) / 2.0;

				allpasstemp = pocketverbs->alpSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->aSL[allpasstemp] * constallpass;
				pocketverbs->aSL[pocketverbs->alpSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpSL--;
				if (pocketverbs->alpSL < 0 || pocketverbs->alpSL > pocketverbs->delayS) {
					pocketverbs->alpSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->aSL[pocketverbs->alpSL]);
				// allpass filter S

				pocketverbs->dSL[2] = pocketverbs->dSL[1];
				pocketverbs->dSL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[1] + pocketverbs->dSL[2]) / 2.0;

				allpasstemp = pocketverbs->alpTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->aTL[allpasstemp] * constallpass;
				pocketverbs->aTL[pocketverbs->alpTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpTL--;
				if (pocketverbs->alpTL < 0 || pocketverbs->alpTL > pocketverbs->delayT) {
					pocketverbs->alpTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->aTL[pocketverbs->alpTL]);
				// allpass filter T

				pocketverbs->dTL[2] = pocketverbs->dTL[1];
				pocketverbs->dTL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dTL[2]) / 2.0;

				allpasstemp = pocketverbs->alpUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->aUL[allpasstemp] * constallpass;
				pocketverbs->aUL[pocketverbs->alpUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpUL--;
				if (pocketverbs->alpUL < 0 || pocketverbs->alpUL > pocketverbs->delayU) {
					pocketverbs->alpUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->aUL[pocketverbs->alpUL]);
				// allpass filter U

				pocketverbs->dUL[2] = pocketverbs->dUL[1];
				pocketverbs->dUL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dUL[2]) / 2.0;

				allpasstemp = pocketverbs->alpVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->aVL[allpasstemp] * constallpass;
				pocketverbs->aVL[pocketverbs->alpVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpVL--;
				if (pocketverbs->alpVL < 0 || pocketverbs->alpVL > pocketverbs->delayV) {
					pocketverbs->alpVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->aVL[pocketverbs->alpVL]);
				// allpass filter V

				pocketverbs->dVL[2] = pocketverbs->dVL[1];
				pocketverbs->dVL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dVL[2]) / 2.0;

				allpasstemp = pocketverbs->alpWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->aWL[allpasstemp] * constallpass;
				pocketverbs->aWL[pocketverbs->alpWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpWL--;
				if (pocketverbs->alpWL < 0 || pocketverbs->alpWL > pocketverbs->delayW) {
					pocketverbs->alpWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->aWL[pocketverbs->alpWL]);
				// allpass filter W

				pocketverbs->dWL[2] = pocketverbs->dWL[1];
				pocketverbs->dWL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[1] + pocketverbs->dWL[2]) / 2.0;

				allpasstemp = pocketverbs->alpXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->aXL[allpasstemp] * constallpass;
				pocketverbs->aXL[pocketverbs->alpXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpXL--;
				if (pocketverbs->alpXL < 0 || pocketverbs->alpXL > pocketverbs->delayX) {
					pocketverbs->alpXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->aXL[pocketverbs->alpXL]);
				// allpass filter X

				pocketverbs->dXL[2] = pocketverbs->dXL[1];
				pocketverbs->dXL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dXL[2]) / 2.0;

				allpasstemp = pocketverbs->alpYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->aYL[allpasstemp] * constallpass;
				pocketverbs->aYL[pocketverbs->alpYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpYL--;
				if (pocketverbs->alpYL < 0 || pocketverbs->alpYL > pocketverbs->delayY) {
					pocketverbs->alpYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->aYL[pocketverbs->alpYL]);
				// allpass filter Y

				pocketverbs->dYL[2] = pocketverbs->dYL[1];
				pocketverbs->dYL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dYL[2]) / 2.0;

				allpasstemp = pocketverbs->alpZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->aZL[allpasstemp] * constallpass;
				pocketverbs->aZL[pocketverbs->alpZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpZL--;
				if (pocketverbs->alpZL < 0 || pocketverbs->alpZL > pocketverbs->delayZ) {
					pocketverbs->alpZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->aZL[pocketverbs->alpZL]);
				// allpass filter Z

				pocketverbs->dZL[2] = pocketverbs->dZL[1];
				pocketverbs->dZL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[1] + pocketverbs->dZL[2]) / 2.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->oAL[allpasstemp] * constallpass;
				pocketverbs->oAL[pocketverbs->outAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outAL--;
				if (pocketverbs->outAL < 0 || pocketverbs->outAL > pocketverbs->delayA) {
					pocketverbs->outAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->oAL[pocketverbs->outAL]);
				// allpass filter A

				pocketverbs->dAL[5] = pocketverbs->dAL[4];
				pocketverbs->dAL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[4] + pocketverbs->dAL[5]) / 2.0;

				allpasstemp = pocketverbs->outBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->oBL[allpasstemp] * constallpass;
				pocketverbs->oBL[pocketverbs->outBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outBL--;
				if (pocketverbs->outBL < 0 || pocketverbs->outBL > pocketverbs->delayB) {
					pocketverbs->outBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->oBL[pocketverbs->outBL]);
				// allpass filter B

				pocketverbs->dBL[5] = pocketverbs->dBL[4];
				pocketverbs->dBL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[4] + pocketverbs->dBL[5]) / 2.0;

				allpasstemp = pocketverbs->outCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->oCL[allpasstemp] * constallpass;
				pocketverbs->oCL[pocketverbs->outCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outCL--;
				if (pocketverbs->outCL < 0 || pocketverbs->outCL > pocketverbs->delayC) {
					pocketverbs->outCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->oCL[pocketverbs->outCL]);
				// allpass filter C

				pocketverbs->dCL[5] = pocketverbs->dCL[4];
				pocketverbs->dCL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[4] + pocketverbs->dCL[5]) / 2.0;

				allpasstemp = pocketverbs->outDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->oDL[allpasstemp] * constallpass;
				pocketverbs->oDL[pocketverbs->outDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outDL--;
				if (pocketverbs->outDL < 0 || pocketverbs->outDL > pocketverbs->delayD) {
					pocketverbs->outDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->oDL[pocketverbs->outDL]);
				// allpass filter D

				pocketverbs->dDL[5] = pocketverbs->dDL[4];
				pocketverbs->dDL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[4] + pocketverbs->dDL[5]) / 2.0;

				allpasstemp = pocketverbs->outEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->oEL[allpasstemp] * constallpass;
				pocketverbs->oEL[pocketverbs->outEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outEL--;
				if (pocketverbs->outEL < 0 || pocketverbs->outEL > pocketverbs->delayE) {
					pocketverbs->outEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->oEL[pocketverbs->outEL]);
				// allpass filter E

				pocketverbs->dEL[5] = pocketverbs->dEL[4];
				pocketverbs->dEL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[4] + pocketverbs->dEL[5]) / 2.0;

				allpasstemp = pocketverbs->outFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->oFL[allpasstemp] * constallpass;
				pocketverbs->oFL[pocketverbs->outFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outFL--;
				if (pocketverbs->outFL < 0 || pocketverbs->outFL > pocketverbs->delayF) {
					pocketverbs->outFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->oFL[pocketverbs->outFL]);
				// allpass filter F

				pocketverbs->dFL[5] = pocketverbs->dFL[4];
				pocketverbs->dFL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[4] + pocketverbs->dFL[5]) / 2.0;

				allpasstemp = pocketverbs->outGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->oGL[allpasstemp] * constallpass;
				pocketverbs->oGL[pocketverbs->outGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outGL--;
				if (pocketverbs->outGL < 0 || pocketverbs->outGL > pocketverbs->delayG) {
					pocketverbs->outGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->oGL[pocketverbs->outGL]);
				// allpass filter G

				pocketverbs->dGL[5] = pocketverbs->dGL[4];
				pocketverbs->dGL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[4] + pocketverbs->dGL[5]) / 2.0;

				allpasstemp = pocketverbs->outHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->oHL[allpasstemp] * constallpass;
				pocketverbs->oHL[pocketverbs->outHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outHL--;
				if (pocketverbs->outHL < 0 || pocketverbs->outHL > pocketverbs->delayH) {
					pocketverbs->outHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->oHL[pocketverbs->outHL]);
				// allpass filter H

				pocketverbs->dHL[5] = pocketverbs->dHL[4];
				pocketverbs->dHL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[4] + pocketverbs->dHL[5]) / 2.0;

				allpasstemp = pocketverbs->outIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->oIL[allpasstemp] * constallpass;
				pocketverbs->oIL[pocketverbs->outIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outIL--;
				if (pocketverbs->outIL < 0 || pocketverbs->outIL > pocketverbs->delayI) {
					pocketverbs->outIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->oIL[pocketverbs->outIL]);
				// allpass filter I

				pocketverbs->dIL[5] = pocketverbs->dIL[4];
				pocketverbs->dIL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[4] + pocketverbs->dIL[5]) / 2.0;

				allpasstemp = pocketverbs->outJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->oJL[allpasstemp] * constallpass;
				pocketverbs->oJL[pocketverbs->outJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outJL--;
				if (pocketverbs->outJL < 0 || pocketverbs->outJL > pocketverbs->delayJ) {
					pocketverbs->outJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->oJL[pocketverbs->outJL]);
				// allpass filter J

				pocketverbs->dJL[5] = pocketverbs->dJL[4];
				pocketverbs->dJL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[4] + pocketverbs->dJL[5]) / 2.0;

				allpasstemp = pocketverbs->outKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->oKL[allpasstemp] * constallpass;
				pocketverbs->oKL[pocketverbs->outKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outKL--;
				if (pocketverbs->outKL < 0 || pocketverbs->outKL > pocketverbs->delayK) {
					pocketverbs->outKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->oKL[pocketverbs->outKL]);
				// allpass filter K

				pocketverbs->dKL[5] = pocketverbs->dKL[4];
				pocketverbs->dKL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[4] + pocketverbs->dKL[5]) / 2.0;

				allpasstemp = pocketverbs->outLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->oLL[allpasstemp] * constallpass;
				pocketverbs->oLL[pocketverbs->outLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outLL--;
				if (pocketverbs->outLL < 0 || pocketverbs->outLL > pocketverbs->delayL) {
					pocketverbs->outLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->oLL[pocketverbs->outLL]);
				// allpass filter L

				pocketverbs->dLL[5] = pocketverbs->dLL[4];
				pocketverbs->dLL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[4] + pocketverbs->dLL[5]) / 2.0;

				allpasstemp = pocketverbs->outML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->oML[allpasstemp] * constallpass;
				pocketverbs->oML[pocketverbs->outML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outML--;
				if (pocketverbs->outML < 0 || pocketverbs->outML > pocketverbs->delayM) {
					pocketverbs->outML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->oML[pocketverbs->outML]);
				// allpass filter M

				pocketverbs->dML[5] = pocketverbs->dML[4];
				pocketverbs->dML[4] = inputSampleL;
				inputSampleL = (pocketverbs->dML[4] + pocketverbs->dML[5]) / 2.0;

				allpasstemp = pocketverbs->outNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->oNL[allpasstemp] * constallpass;
				pocketverbs->oNL[pocketverbs->outNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outNL--;
				if (pocketverbs->outNL < 0 || pocketverbs->outNL > pocketverbs->delayN) {
					pocketverbs->outNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->oNL[pocketverbs->outNL]);
				// allpass filter N

				pocketverbs->dNL[5] = pocketverbs->dNL[4];
				pocketverbs->dNL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[4] + pocketverbs->dNL[5]) / 2.0;

				allpasstemp = pocketverbs->outOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->oOL[allpasstemp] * constallpass;
				pocketverbs->oOL[pocketverbs->outOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outOL--;
				if (pocketverbs->outOL < 0 || pocketverbs->outOL > pocketverbs->delayO) {
					pocketverbs->outOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->oOL[pocketverbs->outOL]);
				// allpass filter O

				pocketverbs->dOL[5] = pocketverbs->dOL[4];
				pocketverbs->dOL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[4] + pocketverbs->dOL[5]) / 2.0;

				allpasstemp = pocketverbs->outPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->oPL[allpasstemp] * constallpass;
				pocketverbs->oPL[pocketverbs->outPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outPL--;
				if (pocketverbs->outPL < 0 || pocketverbs->outPL > pocketverbs->delayP) {
					pocketverbs->outPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->oPL[pocketverbs->outPL]);
				// allpass filter P

				pocketverbs->dPL[5] = pocketverbs->dPL[4];
				pocketverbs->dPL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[4] + pocketverbs->dPL[5]) / 2.0;

				allpasstemp = pocketverbs->outQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->oQL[allpasstemp] * constallpass;
				pocketverbs->oQL[pocketverbs->outQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outQL--;
				if (pocketverbs->outQL < 0 || pocketverbs->outQL > pocketverbs->delayQ) {
					pocketverbs->outQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->oQL[pocketverbs->outQL]);
				// allpass filter Q

				pocketverbs->dQL[5] = pocketverbs->dQL[4];
				pocketverbs->dQL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[4] + pocketverbs->dQL[5]) / 2.0;

				allpasstemp = pocketverbs->outRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->oRL[allpasstemp] * constallpass;
				pocketverbs->oRL[pocketverbs->outRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outRL--;
				if (pocketverbs->outRL < 0 || pocketverbs->outRL > pocketverbs->delayR) {
					pocketverbs->outRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->oRL[pocketverbs->outRL]);
				// allpass filter R

				pocketverbs->dRL[5] = pocketverbs->dRL[4];
				pocketverbs->dRL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[4] + pocketverbs->dRL[5]) / 2.0;

				allpasstemp = pocketverbs->outSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->oSL[allpasstemp] * constallpass;
				pocketverbs->oSL[pocketverbs->outSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outSL--;
				if (pocketverbs->outSL < 0 || pocketverbs->outSL > pocketverbs->delayS) {
					pocketverbs->outSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->oSL[pocketverbs->outSL]);
				// allpass filter S

				pocketverbs->dSL[5] = pocketverbs->dSL[4];
				pocketverbs->dSL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[4] + pocketverbs->dSL[5]) / 2.0;

				allpasstemp = pocketverbs->outTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->oTL[allpasstemp] * constallpass;
				pocketverbs->oTL[pocketverbs->outTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outTL--;
				if (pocketverbs->outTL < 0 || pocketverbs->outTL > pocketverbs->delayT) {
					pocketverbs->outTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->oTL[pocketverbs->outTL]);
				// allpass filter T

				pocketverbs->dTL[5] = pocketverbs->dTL[4];
				pocketverbs->dTL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[4] + pocketverbs->dTL[5]) / 2.0;

				allpasstemp = pocketverbs->outUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->oUL[allpasstemp] * constallpass;
				pocketverbs->oUL[pocketverbs->outUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outUL--;
				if (pocketverbs->outUL < 0 || pocketverbs->outUL > pocketverbs->delayU) {
					pocketverbs->outUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->oUL[pocketverbs->outUL]);
				// allpass filter U

				pocketverbs->dUL[5] = pocketverbs->dUL[4];
				pocketverbs->dUL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[4] + pocketverbs->dUL[5]) / 2.0;

				allpasstemp = pocketverbs->outVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->oVL[allpasstemp] * constallpass;
				pocketverbs->oVL[pocketverbs->outVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outVL--;
				if (pocketverbs->outVL < 0 || pocketverbs->outVL > pocketverbs->delayV) {
					pocketverbs->outVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->oVL[pocketverbs->outVL]);
				// allpass filter V

				pocketverbs->dVL[5] = pocketverbs->dVL[4];
				pocketverbs->dVL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[4] + pocketverbs->dVL[5]) / 2.0;

				allpasstemp = pocketverbs->outWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->oWL[allpasstemp] * constallpass;
				pocketverbs->oWL[pocketverbs->outWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outWL--;
				if (pocketverbs->outWL < 0 || pocketverbs->outWL > pocketverbs->delayW) {
					pocketverbs->outWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->oWL[pocketverbs->outWL]);
				// allpass filter W

				pocketverbs->dWL[5] = pocketverbs->dWL[4];
				pocketverbs->dWL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[4] + pocketverbs->dWL[5]) / 2.0;

				allpasstemp = pocketverbs->outXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->oXL[allpasstemp] * constallpass;
				pocketverbs->oXL[pocketverbs->outXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outXL--;
				if (pocketverbs->outXL < 0 || pocketverbs->outXL > pocketverbs->delayX) {
					pocketverbs->outXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->oXL[pocketverbs->outXL]);
				// allpass filter X

				pocketverbs->dXL[5] = pocketverbs->dXL[4];
				pocketverbs->dXL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[4] + pocketverbs->dXL[5]) / 2.0;

				allpasstemp = pocketverbs->outYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->oYL[allpasstemp] * constallpass;
				pocketverbs->oYL[pocketverbs->outYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outYL--;
				if (pocketverbs->outYL < 0 || pocketverbs->outYL > pocketverbs->delayY) {
					pocketverbs->outYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->oYL[pocketverbs->outYL]);
				// allpass filter Y

				pocketverbs->dYL[5] = pocketverbs->dYL[4];
				pocketverbs->dYL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[4] + pocketverbs->dYL[5]) / 2.0;

				allpasstemp = pocketverbs->outZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->oZL[allpasstemp] * constallpass;
				pocketverbs->oZL[pocketverbs->outZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outZL--;
				if (pocketverbs->outZL < 0 || pocketverbs->outZL > pocketverbs->delayZ) {
					pocketverbs->outZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->oZL[pocketverbs->outZL]);
				// allpass filter Z

				pocketverbs->dZL[5] = pocketverbs->dZL[4];
				pocketverbs->dZL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[4] + pocketverbs->dZL[5]) / 2.0;
				// output Stretch unrealistic but smooth fake Paulstretch
				break;

			case 6: // Zarathustra
				allpasstemp = pocketverbs->alpAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->aAL[allpasstemp] * constallpass;
				pocketverbs->aAL[pocketverbs->alpAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpAL--;
				if (pocketverbs->alpAL < 0 || pocketverbs->alpAL > pocketverbs->delayA) {
					pocketverbs->alpAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->aAL[pocketverbs->alpAL]);
				// allpass filter A

				pocketverbs->dAL[3] = pocketverbs->dAL[2];
				pocketverbs->dAL[2] = pocketverbs->dAL[1];
				pocketverbs->dAL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dAL[1] + pocketverbs->dAL[2] + pocketverbs->dZL[3]) / 3.0; // add feedback

				allpasstemp = pocketverbs->alpBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->aBL[allpasstemp] * constallpass;
				pocketverbs->aBL[pocketverbs->alpBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpBL--;
				if (pocketverbs->alpBL < 0 || pocketverbs->alpBL > pocketverbs->delayB) {
					pocketverbs->alpBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->aBL[pocketverbs->alpBL]);
				// allpass filter B

				pocketverbs->dBL[3] = pocketverbs->dBL[2];
				pocketverbs->dBL[2] = pocketverbs->dBL[1];
				pocketverbs->dBL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[1] + pocketverbs->dBL[2] + pocketverbs->dBL[3]) / 3.0;

				allpasstemp = pocketverbs->alpCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->aCL[allpasstemp] * constallpass;
				pocketverbs->aCL[pocketverbs->alpCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpCL--;
				if (pocketverbs->alpCL < 0 || pocketverbs->alpCL > pocketverbs->delayC) {
					pocketverbs->alpCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->aCL[pocketverbs->alpCL]);
				// allpass filter C

				pocketverbs->dCL[3] = pocketverbs->dCL[2];
				pocketverbs->dCL[2] = pocketverbs->dCL[1];
				pocketverbs->dCL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[1] + pocketverbs->dCL[2] + pocketverbs->dCL[3]) / 3.0;

				allpasstemp = pocketverbs->alpDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->aDL[allpasstemp] * constallpass;
				pocketverbs->aDL[pocketverbs->alpDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpDL--;
				if (pocketverbs->alpDL < 0 || pocketverbs->alpDL > pocketverbs->delayD) {
					pocketverbs->alpDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->aDL[pocketverbs->alpDL]);
				// allpass filter D

				pocketverbs->dDL[3] = pocketverbs->dDL[2];
				pocketverbs->dDL[2] = pocketverbs->dDL[1];
				pocketverbs->dDL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[1] + pocketverbs->dDL[2] + pocketverbs->dDL[3]) / 3.0;

				allpasstemp = pocketverbs->alpEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->aEL[allpasstemp] * constallpass;
				pocketverbs->aEL[pocketverbs->alpEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpEL--;
				if (pocketverbs->alpEL < 0 || pocketverbs->alpEL > pocketverbs->delayE) {
					pocketverbs->alpEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->aEL[pocketverbs->alpEL]);
				// allpass filter E

				pocketverbs->dEL[3] = pocketverbs->dEL[2];
				pocketverbs->dEL[2] = pocketverbs->dEL[1];
				pocketverbs->dEL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[1] + pocketverbs->dEL[2] + pocketverbs->dEL[3]) / 3.0;

				allpasstemp = pocketverbs->alpFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->aFL[allpasstemp] * constallpass;
				pocketverbs->aFL[pocketverbs->alpFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpFL--;
				if (pocketverbs->alpFL < 0 || pocketverbs->alpFL > pocketverbs->delayF) {
					pocketverbs->alpFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->aFL[pocketverbs->alpFL]);
				// allpass filter F

				pocketverbs->dFL[3] = pocketverbs->dFL[2];
				pocketverbs->dFL[2] = pocketverbs->dFL[1];
				pocketverbs->dFL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[1] + pocketverbs->dFL[2] + pocketverbs->dFL[3]) / 3.0;

				allpasstemp = pocketverbs->alpGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->aGL[allpasstemp] * constallpass;
				pocketverbs->aGL[pocketverbs->alpGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpGL--;
				if (pocketverbs->alpGL < 0 || pocketverbs->alpGL > pocketverbs->delayG) {
					pocketverbs->alpGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->aGL[pocketverbs->alpGL]);
				// allpass filter G

				pocketverbs->dGL[3] = pocketverbs->dGL[2];
				pocketverbs->dGL[2] = pocketverbs->dGL[1];
				pocketverbs->dGL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[1] + pocketverbs->dGL[2] + pocketverbs->dGL[3]) / 3.0;

				allpasstemp = pocketverbs->alpHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->aHL[allpasstemp] * constallpass;
				pocketverbs->aHL[pocketverbs->alpHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpHL--;
				if (pocketverbs->alpHL < 0 || pocketverbs->alpHL > pocketverbs->delayH) {
					pocketverbs->alpHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->aHL[pocketverbs->alpHL]);
				// allpass filter H

				pocketverbs->dHL[3] = pocketverbs->dHL[2];
				pocketverbs->dHL[2] = pocketverbs->dHL[1];
				pocketverbs->dHL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[1] + pocketverbs->dHL[2] + pocketverbs->dHL[3]) / 3.0;

				allpasstemp = pocketverbs->alpIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->aIL[allpasstemp] * constallpass;
				pocketverbs->aIL[pocketverbs->alpIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpIL--;
				if (pocketverbs->alpIL < 0 || pocketverbs->alpIL > pocketverbs->delayI) {
					pocketverbs->alpIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->aIL[pocketverbs->alpIL]);
				// allpass filter I

				pocketverbs->dIL[3] = pocketverbs->dIL[2];
				pocketverbs->dIL[2] = pocketverbs->dIL[1];
				pocketverbs->dIL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[1] + pocketverbs->dIL[2] + pocketverbs->dIL[3]) / 3.0;

				allpasstemp = pocketverbs->alpJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->aJL[allpasstemp] * constallpass;
				pocketverbs->aJL[pocketverbs->alpJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpJL--;
				if (pocketverbs->alpJL < 0 || pocketverbs->alpJL > pocketverbs->delayJ) {
					pocketverbs->alpJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->aJL[pocketverbs->alpJL]);
				// allpass filter J

				pocketverbs->dJL[3] = pocketverbs->dJL[2];
				pocketverbs->dJL[2] = pocketverbs->dJL[1];
				pocketverbs->dJL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[1] + pocketverbs->dJL[2] + pocketverbs->dJL[3]) / 3.0;

				allpasstemp = pocketverbs->alpKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->aKL[allpasstemp] * constallpass;
				pocketverbs->aKL[pocketverbs->alpKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpKL--;
				if (pocketverbs->alpKL < 0 || pocketverbs->alpKL > pocketverbs->delayK) {
					pocketverbs->alpKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->aKL[pocketverbs->alpKL]);
				// allpass filter K

				pocketverbs->dKL[3] = pocketverbs->dKL[2];
				pocketverbs->dKL[2] = pocketverbs->dKL[1];
				pocketverbs->dKL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[1] + pocketverbs->dKL[2] + pocketverbs->dKL[3]) / 3.0;

				allpasstemp = pocketverbs->alpLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->aLL[allpasstemp] * constallpass;
				pocketverbs->aLL[pocketverbs->alpLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpLL--;
				if (pocketverbs->alpLL < 0 || pocketverbs->alpLL > pocketverbs->delayL) {
					pocketverbs->alpLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->aLL[pocketverbs->alpLL]);
				// allpass filter L

				pocketverbs->dLL[3] = pocketverbs->dLL[2];
				pocketverbs->dLL[2] = pocketverbs->dLL[1];
				pocketverbs->dLL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[1] + pocketverbs->dLL[2] + pocketverbs->dLL[3]) / 3.0;

				allpasstemp = pocketverbs->alpML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->aML[allpasstemp] * constallpass;
				pocketverbs->aML[pocketverbs->alpML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpML--;
				if (pocketverbs->alpML < 0 || pocketverbs->alpML > pocketverbs->delayM) {
					pocketverbs->alpML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->aML[pocketverbs->alpML]);
				// allpass filter M

				pocketverbs->dML[3] = pocketverbs->dML[2];
				pocketverbs->dML[2] = pocketverbs->dML[1];
				pocketverbs->dML[1] = inputSampleL;
				inputSampleL = (pocketverbs->dML[1] + pocketverbs->dML[2] + pocketverbs->dML[3]) / 3.0;

				allpasstemp = pocketverbs->alpNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->aNL[allpasstemp] * constallpass;
				pocketverbs->aNL[pocketverbs->alpNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpNL--;
				if (pocketverbs->alpNL < 0 || pocketverbs->alpNL > pocketverbs->delayN) {
					pocketverbs->alpNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->aNL[pocketverbs->alpNL]);
				// allpass filter N

				pocketverbs->dNL[3] = pocketverbs->dNL[2];
				pocketverbs->dNL[2] = pocketverbs->dNL[1];
				pocketverbs->dNL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[1] + pocketverbs->dNL[2] + pocketverbs->dNL[3]) / 3.0;

				allpasstemp = pocketverbs->alpOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->aOL[allpasstemp] * constallpass;
				pocketverbs->aOL[pocketverbs->alpOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpOL--;
				if (pocketverbs->alpOL < 0 || pocketverbs->alpOL > pocketverbs->delayO) {
					pocketverbs->alpOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->aOL[pocketverbs->alpOL]);
				// allpass filter O

				pocketverbs->dOL[3] = pocketverbs->dOL[2];
				pocketverbs->dOL[2] = pocketverbs->dOL[1];
				pocketverbs->dOL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[1] + pocketverbs->dOL[2] + pocketverbs->dOL[3]) / 3.0;

				allpasstemp = pocketverbs->alpPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->aPL[allpasstemp] * constallpass;
				pocketverbs->aPL[pocketverbs->alpPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpPL--;
				if (pocketverbs->alpPL < 0 || pocketverbs->alpPL > pocketverbs->delayP) {
					pocketverbs->alpPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->aPL[pocketverbs->alpPL]);
				// allpass filter P

				pocketverbs->dPL[3] = pocketverbs->dPL[2];
				pocketverbs->dPL[2] = pocketverbs->dPL[1];
				pocketverbs->dPL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[1] + pocketverbs->dPL[2] + pocketverbs->dPL[3]) / 3.0;

				allpasstemp = pocketverbs->alpQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->aQL[allpasstemp] * constallpass;
				pocketverbs->aQL[pocketverbs->alpQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpQL--;
				if (pocketverbs->alpQL < 0 || pocketverbs->alpQL > pocketverbs->delayQ) {
					pocketverbs->alpQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->aQL[pocketverbs->alpQL]);
				// allpass filter Q

				pocketverbs->dQL[3] = pocketverbs->dQL[2];
				pocketverbs->dQL[2] = pocketverbs->dQL[1];
				pocketverbs->dQL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[1] + pocketverbs->dQL[2] + pocketverbs->dQL[3]) / 3.0;

				allpasstemp = pocketverbs->alpRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->aRL[allpasstemp] * constallpass;
				pocketverbs->aRL[pocketverbs->alpRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpRL--;
				if (pocketverbs->alpRL < 0 || pocketverbs->alpRL > pocketverbs->delayR) {
					pocketverbs->alpRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->aRL[pocketverbs->alpRL]);
				// allpass filter R

				pocketverbs->dRL[3] = pocketverbs->dRL[2];
				pocketverbs->dRL[2] = pocketverbs->dRL[1];
				pocketverbs->dRL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[1] + pocketverbs->dRL[2] + pocketverbs->dRL[3]) / 3.0;

				allpasstemp = pocketverbs->alpSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->aSL[allpasstemp] * constallpass;
				pocketverbs->aSL[pocketverbs->alpSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpSL--;
				if (pocketverbs->alpSL < 0 || pocketverbs->alpSL > pocketverbs->delayS) {
					pocketverbs->alpSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->aSL[pocketverbs->alpSL]);
				// allpass filter S

				pocketverbs->dSL[3] = pocketverbs->dSL[2];
				pocketverbs->dSL[2] = pocketverbs->dSL[1];
				pocketverbs->dSL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[1] + pocketverbs->dSL[2] + pocketverbs->dSL[3]) / 3.0;

				allpasstemp = pocketverbs->alpTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->aTL[allpasstemp] * constallpass;
				pocketverbs->aTL[pocketverbs->alpTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpTL--;
				if (pocketverbs->alpTL < 0 || pocketverbs->alpTL > pocketverbs->delayT) {
					pocketverbs->alpTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->aTL[pocketverbs->alpTL]);
				// allpass filter T

				pocketverbs->dTL[3] = pocketverbs->dTL[2];
				pocketverbs->dTL[2] = pocketverbs->dTL[1];
				pocketverbs->dTL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[1] + pocketverbs->dTL[2] + pocketverbs->dTL[3]) / 3.0;

				allpasstemp = pocketverbs->alpUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->aUL[allpasstemp] * constallpass;
				pocketverbs->aUL[pocketverbs->alpUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpUL--;
				if (pocketverbs->alpUL < 0 || pocketverbs->alpUL > pocketverbs->delayU) {
					pocketverbs->alpUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->aUL[pocketverbs->alpUL]);
				// allpass filter U

				pocketverbs->dUL[3] = pocketverbs->dUL[2];
				pocketverbs->dUL[2] = pocketverbs->dUL[1];
				pocketverbs->dUL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[1] + pocketverbs->dUL[2] + pocketverbs->dUL[3]) / 3.0;

				allpasstemp = pocketverbs->alpVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->aVL[allpasstemp] * constallpass;
				pocketverbs->aVL[pocketverbs->alpVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpVL--;
				if (pocketverbs->alpVL < 0 || pocketverbs->alpVL > pocketverbs->delayV) {
					pocketverbs->alpVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->aVL[pocketverbs->alpVL]);
				// allpass filter V

				pocketverbs->dVL[3] = pocketverbs->dVL[2];
				pocketverbs->dVL[2] = pocketverbs->dVL[1];
				pocketverbs->dVL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[1] + pocketverbs->dVL[2] + pocketverbs->dVL[3]) / 3.0;

				allpasstemp = pocketverbs->alpWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->aWL[allpasstemp] * constallpass;
				pocketverbs->aWL[pocketverbs->alpWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpWL--;
				if (pocketverbs->alpWL < 0 || pocketverbs->alpWL > pocketverbs->delayW) {
					pocketverbs->alpWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->aWL[pocketverbs->alpWL]);
				// allpass filter W

				pocketverbs->dWL[3] = pocketverbs->dWL[2];
				pocketverbs->dWL[2] = pocketverbs->dWL[1];
				pocketverbs->dWL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[1] + pocketverbs->dWL[2] + pocketverbs->dWL[3]) / 3.0;

				allpasstemp = pocketverbs->alpXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->aXL[allpasstemp] * constallpass;
				pocketverbs->aXL[pocketverbs->alpXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpXL--;
				if (pocketverbs->alpXL < 0 || pocketverbs->alpXL > pocketverbs->delayX) {
					pocketverbs->alpXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->aXL[pocketverbs->alpXL]);
				// allpass filter X

				pocketverbs->dXL[3] = pocketverbs->dXL[2];
				pocketverbs->dXL[2] = pocketverbs->dXL[1];
				pocketverbs->dXL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[1] + pocketverbs->dXL[2] + pocketverbs->dXL[3]) / 3.0;

				allpasstemp = pocketverbs->alpYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->aYL[allpasstemp] * constallpass;
				pocketverbs->aYL[pocketverbs->alpYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpYL--;
				if (pocketverbs->alpYL < 0 || pocketverbs->alpYL > pocketverbs->delayY) {
					pocketverbs->alpYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->aYL[pocketverbs->alpYL]);
				// allpass filter Y

				pocketverbs->dYL[3] = pocketverbs->dYL[2];
				pocketverbs->dYL[2] = pocketverbs->dYL[1];
				pocketverbs->dYL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[1] + pocketverbs->dYL[2] + pocketverbs->dYL[3]) / 3.0;

				allpasstemp = pocketverbs->alpZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->aZL[allpasstemp] * constallpass;
				pocketverbs->aZL[pocketverbs->alpZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->alpZL--;
				if (pocketverbs->alpZL < 0 || pocketverbs->alpZL > pocketverbs->delayZ) {
					pocketverbs->alpZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->aZL[pocketverbs->alpZL]);
				// allpass filter Z

				pocketverbs->dZL[3] = pocketverbs->dZL[2];
				pocketverbs->dZL[2] = pocketverbs->dZL[1];
				pocketverbs->dZL[1] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[1] + pocketverbs->dZL[2] + pocketverbs->dZL[3]) / 3.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleL -= pocketverbs->oAL[allpasstemp] * constallpass;
				pocketverbs->oAL[pocketverbs->outAL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outAL--;
				if (pocketverbs->outAL < 0 || pocketverbs->outAL > pocketverbs->delayA) {
					pocketverbs->outAL = pocketverbs->delayA;
				}
				inputSampleL += (pocketverbs->oAL[pocketverbs->outAL]);
				// allpass filter A

				pocketverbs->dAL[6] = pocketverbs->dAL[5];
				pocketverbs->dAL[5] = pocketverbs->dAL[4];
				pocketverbs->dAL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[1] + pocketverbs->dAL[5] + pocketverbs->dAL[6]) / 3.0; // note, feeding in dry again for a little more clarity!

				allpasstemp = pocketverbs->outBL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleL -= pocketverbs->oBL[allpasstemp] * constallpass;
				pocketverbs->oBL[pocketverbs->outBL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outBL--;
				if (pocketverbs->outBL < 0 || pocketverbs->outBL > pocketverbs->delayB) {
					pocketverbs->outBL = pocketverbs->delayB;
				}
				inputSampleL += (pocketverbs->oBL[pocketverbs->outBL]);
				// allpass filter B

				pocketverbs->dBL[6] = pocketverbs->dBL[5];
				pocketverbs->dBL[5] = pocketverbs->dBL[4];
				pocketverbs->dBL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dBL[4] + pocketverbs->dBL[5] + pocketverbs->dBL[6]) / 3.0;

				allpasstemp = pocketverbs->outCL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleL -= pocketverbs->oCL[allpasstemp] * constallpass;
				pocketverbs->oCL[pocketverbs->outCL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outCL--;
				if (pocketverbs->outCL < 0 || pocketverbs->outCL > pocketverbs->delayC) {
					pocketverbs->outCL = pocketverbs->delayC;
				}
				inputSampleL += (pocketverbs->oCL[pocketverbs->outCL]);
				// allpass filter C

				pocketverbs->dCL[6] = pocketverbs->dCL[5];
				pocketverbs->dCL[5] = pocketverbs->dCL[4];
				pocketverbs->dCL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dCL[4] + pocketverbs->dCL[5] + pocketverbs->dCL[6]) / 3.0;

				allpasstemp = pocketverbs->outDL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleL -= pocketverbs->oDL[allpasstemp] * constallpass;
				pocketverbs->oDL[pocketverbs->outDL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outDL--;
				if (pocketverbs->outDL < 0 || pocketverbs->outDL > pocketverbs->delayD) {
					pocketverbs->outDL = pocketverbs->delayD;
				}
				inputSampleL += (pocketverbs->oDL[pocketverbs->outDL]);
				// allpass filter D

				pocketverbs->dDL[6] = pocketverbs->dDL[5];
				pocketverbs->dDL[5] = pocketverbs->dDL[4];
				pocketverbs->dDL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dDL[4] + pocketverbs->dDL[5] + pocketverbs->dDL[6]) / 3.0;

				allpasstemp = pocketverbs->outEL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleL -= pocketverbs->oEL[allpasstemp] * constallpass;
				pocketverbs->oEL[pocketverbs->outEL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outEL--;
				if (pocketverbs->outEL < 0 || pocketverbs->outEL > pocketverbs->delayE) {
					pocketverbs->outEL = pocketverbs->delayE;
				}
				inputSampleL += (pocketverbs->oEL[pocketverbs->outEL]);
				// allpass filter E

				pocketverbs->dEL[6] = pocketverbs->dEL[5];
				pocketverbs->dEL[5] = pocketverbs->dEL[4];
				pocketverbs->dEL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dEL[4] + pocketverbs->dEL[5] + pocketverbs->dEL[6]) / 3.0;

				allpasstemp = pocketverbs->outFL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleL -= pocketverbs->oFL[allpasstemp] * constallpass;
				pocketverbs->oFL[pocketverbs->outFL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outFL--;
				if (pocketverbs->outFL < 0 || pocketverbs->outFL > pocketverbs->delayF) {
					pocketverbs->outFL = pocketverbs->delayF;
				}
				inputSampleL += (pocketverbs->oFL[pocketverbs->outFL]);
				// allpass filter F

				pocketverbs->dFL[6] = pocketverbs->dFL[5];
				pocketverbs->dFL[5] = pocketverbs->dFL[4];
				pocketverbs->dFL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dFL[4] + pocketverbs->dFL[5] + pocketverbs->dFL[6]) / 3.0;

				allpasstemp = pocketverbs->outGL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleL -= pocketverbs->oGL[allpasstemp] * constallpass;
				pocketverbs->oGL[pocketverbs->outGL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outGL--;
				if (pocketverbs->outGL < 0 || pocketverbs->outGL > pocketverbs->delayG) {
					pocketverbs->outGL = pocketverbs->delayG;
				}
				inputSampleL += (pocketverbs->oGL[pocketverbs->outGL]);
				// allpass filter G

				pocketverbs->dGL[6] = pocketverbs->dGL[5];
				pocketverbs->dGL[5] = pocketverbs->dGL[4];
				pocketverbs->dGL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dGL[4] + pocketverbs->dGL[5] + pocketverbs->dGL[6]) / 3.0;

				allpasstemp = pocketverbs->outHL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleL -= pocketverbs->oHL[allpasstemp] * constallpass;
				pocketverbs->oHL[pocketverbs->outHL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outHL--;
				if (pocketverbs->outHL < 0 || pocketverbs->outHL > pocketverbs->delayH) {
					pocketverbs->outHL = pocketverbs->delayH;
				}
				inputSampleL += (pocketverbs->oHL[pocketverbs->outHL]);
				// allpass filter H

				pocketverbs->dHL[6] = pocketverbs->dHL[5];
				pocketverbs->dHL[5] = pocketverbs->dHL[4];
				pocketverbs->dHL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dHL[4] + pocketverbs->dHL[5] + pocketverbs->dHL[6]) / 3.0;

				allpasstemp = pocketverbs->outIL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleL -= pocketverbs->oIL[allpasstemp] * constallpass;
				pocketverbs->oIL[pocketverbs->outIL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outIL--;
				if (pocketverbs->outIL < 0 || pocketverbs->outIL > pocketverbs->delayI) {
					pocketverbs->outIL = pocketverbs->delayI;
				}
				inputSampleL += (pocketverbs->oIL[pocketverbs->outIL]);
				// allpass filter I

				pocketverbs->dIL[6] = pocketverbs->dIL[5];
				pocketverbs->dIL[5] = pocketverbs->dIL[4];
				pocketverbs->dIL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dIL[4] + pocketverbs->dIL[5] + pocketverbs->dIL[6]) / 3.0;

				allpasstemp = pocketverbs->outJL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleL -= pocketverbs->oJL[allpasstemp] * constallpass;
				pocketverbs->oJL[pocketverbs->outJL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outJL--;
				if (pocketverbs->outJL < 0 || pocketverbs->outJL > pocketverbs->delayJ) {
					pocketverbs->outJL = pocketverbs->delayJ;
				}
				inputSampleL += (pocketverbs->oJL[pocketverbs->outJL]);
				// allpass filter J

				pocketverbs->dJL[6] = pocketverbs->dJL[5];
				pocketverbs->dJL[5] = pocketverbs->dJL[4];
				pocketverbs->dJL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dJL[4] + pocketverbs->dJL[5] + pocketverbs->dJL[6]) / 3.0;

				allpasstemp = pocketverbs->outKL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleL -= pocketverbs->oKL[allpasstemp] * constallpass;
				pocketverbs->oKL[pocketverbs->outKL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outKL--;
				if (pocketverbs->outKL < 0 || pocketverbs->outKL > pocketverbs->delayK) {
					pocketverbs->outKL = pocketverbs->delayK;
				}
				inputSampleL += (pocketverbs->oKL[pocketverbs->outKL]);
				// allpass filter K

				pocketverbs->dKL[6] = pocketverbs->dKL[5];
				pocketverbs->dKL[5] = pocketverbs->dKL[4];
				pocketverbs->dKL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dKL[4] + pocketverbs->dKL[5] + pocketverbs->dKL[6]) / 3.0;

				allpasstemp = pocketverbs->outLL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleL -= pocketverbs->oLL[allpasstemp] * constallpass;
				pocketverbs->oLL[pocketverbs->outLL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outLL--;
				if (pocketverbs->outLL < 0 || pocketverbs->outLL > pocketverbs->delayL) {
					pocketverbs->outLL = pocketverbs->delayL;
				}
				inputSampleL += (pocketverbs->oLL[pocketverbs->outLL]);
				// allpass filter L

				pocketverbs->dLL[6] = pocketverbs->dLL[5];
				pocketverbs->dLL[5] = pocketverbs->dLL[4];
				pocketverbs->dLL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dLL[4] + pocketverbs->dLL[5] + pocketverbs->dLL[6]) / 3.0;

				allpasstemp = pocketverbs->outML - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleL -= pocketverbs->oML[allpasstemp] * constallpass;
				pocketverbs->oML[pocketverbs->outML] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outML--;
				if (pocketverbs->outML < 0 || pocketverbs->outML > pocketverbs->delayM) {
					pocketverbs->outML = pocketverbs->delayM;
				}
				inputSampleL += (pocketverbs->oML[pocketverbs->outML]);
				// allpass filter M

				pocketverbs->dML[6] = pocketverbs->dML[5];
				pocketverbs->dML[5] = pocketverbs->dML[4];
				pocketverbs->dML[4] = inputSampleL;
				inputSampleL = (pocketverbs->dML[4] + pocketverbs->dML[5] + pocketverbs->dML[6]) / 3.0;

				allpasstemp = pocketverbs->outNL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleL -= pocketverbs->oNL[allpasstemp] * constallpass;
				pocketverbs->oNL[pocketverbs->outNL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outNL--;
				if (pocketverbs->outNL < 0 || pocketverbs->outNL > pocketverbs->delayN) {
					pocketverbs->outNL = pocketverbs->delayN;
				}
				inputSampleL += (pocketverbs->oNL[pocketverbs->outNL]);
				// allpass filter N

				pocketverbs->dNL[6] = pocketverbs->dNL[5];
				pocketverbs->dNL[5] = pocketverbs->dNL[4];
				pocketverbs->dNL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dNL[4] + pocketverbs->dNL[5] + pocketverbs->dNL[6]) / 3.0;

				allpasstemp = pocketverbs->outOL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleL -= pocketverbs->oOL[allpasstemp] * constallpass;
				pocketverbs->oOL[pocketverbs->outOL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outOL--;
				if (pocketverbs->outOL < 0 || pocketverbs->outOL > pocketverbs->delayO) {
					pocketverbs->outOL = pocketverbs->delayO;
				}
				inputSampleL += (pocketverbs->oOL[pocketverbs->outOL]);
				// allpass filter O

				pocketverbs->dOL[6] = pocketverbs->dOL[5];
				pocketverbs->dOL[5] = pocketverbs->dOL[4];
				pocketverbs->dOL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dOL[4] + pocketverbs->dOL[5] + pocketverbs->dOL[6]) / 3.0;

				allpasstemp = pocketverbs->outPL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleL -= pocketverbs->oPL[allpasstemp] * constallpass;
				pocketverbs->oPL[pocketverbs->outPL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outPL--;
				if (pocketverbs->outPL < 0 || pocketverbs->outPL > pocketverbs->delayP) {
					pocketverbs->outPL = pocketverbs->delayP;
				}
				inputSampleL += (pocketverbs->oPL[pocketverbs->outPL]);
				// allpass filter P

				pocketverbs->dPL[6] = pocketverbs->dPL[5];
				pocketverbs->dPL[5] = pocketverbs->dPL[4];
				pocketverbs->dPL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dPL[4] + pocketverbs->dPL[5] + pocketverbs->dPL[6]) / 3.0;

				allpasstemp = pocketverbs->outQL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleL -= pocketverbs->oQL[allpasstemp] * constallpass;
				pocketverbs->oQL[pocketverbs->outQL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outQL--;
				if (pocketverbs->outQL < 0 || pocketverbs->outQL > pocketverbs->delayQ) {
					pocketverbs->outQL = pocketverbs->delayQ;
				}
				inputSampleL += (pocketverbs->oQL[pocketverbs->outQL]);
				// allpass filter Q

				pocketverbs->dQL[6] = pocketverbs->dQL[5];
				pocketverbs->dQL[5] = pocketverbs->dQL[4];
				pocketverbs->dQL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dQL[4] + pocketverbs->dQL[5] + pocketverbs->dQL[6]) / 3.0;

				allpasstemp = pocketverbs->outRL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleL -= pocketverbs->oRL[allpasstemp] * constallpass;
				pocketverbs->oRL[pocketverbs->outRL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outRL--;
				if (pocketverbs->outRL < 0 || pocketverbs->outRL > pocketverbs->delayR) {
					pocketverbs->outRL = pocketverbs->delayR;
				}
				inputSampleL += (pocketverbs->oRL[pocketverbs->outRL]);
				// allpass filter R

				pocketverbs->dRL[6] = pocketverbs->dRL[5];
				pocketverbs->dRL[5] = pocketverbs->dRL[4];
				pocketverbs->dRL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dRL[4] + pocketverbs->dRL[5] + pocketverbs->dRL[6]) / 3.0;

				allpasstemp = pocketverbs->outSL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleL -= pocketverbs->oSL[allpasstemp] * constallpass;
				pocketverbs->oSL[pocketverbs->outSL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outSL--;
				if (pocketverbs->outSL < 0 || pocketverbs->outSL > pocketverbs->delayS) {
					pocketverbs->outSL = pocketverbs->delayS;
				}
				inputSampleL += (pocketverbs->oSL[pocketverbs->outSL]);
				// allpass filter S

				pocketverbs->dSL[6] = pocketverbs->dSL[5];
				pocketverbs->dSL[5] = pocketverbs->dSL[4];
				pocketverbs->dSL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dSL[4] + pocketverbs->dSL[5] + pocketverbs->dSL[6]) / 3.0;

				allpasstemp = pocketverbs->outTL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleL -= pocketverbs->oTL[allpasstemp] * constallpass;
				pocketverbs->oTL[pocketverbs->outTL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outTL--;
				if (pocketverbs->outTL < 0 || pocketverbs->outTL > pocketverbs->delayT) {
					pocketverbs->outTL = pocketverbs->delayT;
				}
				inputSampleL += (pocketverbs->oTL[pocketverbs->outTL]);
				// allpass filter T

				pocketverbs->dTL[6] = pocketverbs->dTL[5];
				pocketverbs->dTL[5] = pocketverbs->dTL[4];
				pocketverbs->dTL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dTL[4] + pocketverbs->dTL[5] + pocketverbs->dTL[6]) / 3.0;

				allpasstemp = pocketverbs->outUL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleL -= pocketverbs->oUL[allpasstemp] * constallpass;
				pocketverbs->oUL[pocketverbs->outUL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outUL--;
				if (pocketverbs->outUL < 0 || pocketverbs->outUL > pocketverbs->delayU) {
					pocketverbs->outUL = pocketverbs->delayU;
				}
				inputSampleL += (pocketverbs->oUL[pocketverbs->outUL]);
				// allpass filter U

				pocketverbs->dUL[6] = pocketverbs->dUL[5];
				pocketverbs->dUL[5] = pocketverbs->dUL[4];
				pocketverbs->dUL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dUL[4] + pocketverbs->dUL[5] + pocketverbs->dUL[6]) / 3.0;

				allpasstemp = pocketverbs->outVL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleL -= pocketverbs->oVL[allpasstemp] * constallpass;
				pocketverbs->oVL[pocketverbs->outVL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outVL--;
				if (pocketverbs->outVL < 0 || pocketverbs->outVL > pocketverbs->delayV) {
					pocketverbs->outVL = pocketverbs->delayV;
				}
				inputSampleL += (pocketverbs->oVL[pocketverbs->outVL]);
				// allpass filter V

				pocketverbs->dVL[6] = pocketverbs->dVL[5];
				pocketverbs->dVL[5] = pocketverbs->dVL[4];
				pocketverbs->dVL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dVL[4] + pocketverbs->dVL[5] + pocketverbs->dVL[6]) / 3.0;

				allpasstemp = pocketverbs->outWL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleL -= pocketverbs->oWL[allpasstemp] * constallpass;
				pocketverbs->oWL[pocketverbs->outWL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outWL--;
				if (pocketverbs->outWL < 0 || pocketverbs->outWL > pocketverbs->delayW) {
					pocketverbs->outWL = pocketverbs->delayW;
				}
				inputSampleL += (pocketverbs->oWL[pocketverbs->outWL]);
				// allpass filter W

				pocketverbs->dWL[6] = pocketverbs->dWL[5];
				pocketverbs->dWL[5] = pocketverbs->dWL[4];
				pocketverbs->dWL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dWL[4] + pocketverbs->dWL[5] + pocketverbs->dWL[6]) / 3.0;

				allpasstemp = pocketverbs->outXL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleL -= pocketverbs->oXL[allpasstemp] * constallpass;
				pocketverbs->oXL[pocketverbs->outXL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outXL--;
				if (pocketverbs->outXL < 0 || pocketverbs->outXL > pocketverbs->delayX) {
					pocketverbs->outXL = pocketverbs->delayX;
				}
				inputSampleL += (pocketverbs->oXL[pocketverbs->outXL]);
				// allpass filter X

				pocketverbs->dXL[6] = pocketverbs->dXL[5];
				pocketverbs->dXL[5] = pocketverbs->dXL[4];
				pocketverbs->dXL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dXL[4] + pocketverbs->dXL[5] + pocketverbs->dXL[6]) / 3.0;

				allpasstemp = pocketverbs->outYL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleL -= pocketverbs->oYL[allpasstemp] * constallpass;
				pocketverbs->oYL[pocketverbs->outYL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outYL--;
				if (pocketverbs->outYL < 0 || pocketverbs->outYL > pocketverbs->delayY) {
					pocketverbs->outYL = pocketverbs->delayY;
				}
				inputSampleL += (pocketverbs->oYL[pocketverbs->outYL]);
				// allpass filter Y

				pocketverbs->dYL[6] = pocketverbs->dYL[5];
				pocketverbs->dYL[5] = pocketverbs->dYL[4];
				pocketverbs->dYL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dYL[4] + pocketverbs->dYL[5] + pocketverbs->dYL[6]) / 3.0;

				allpasstemp = pocketverbs->outZL - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleL -= pocketverbs->oZL[allpasstemp] * constallpass;
				pocketverbs->oZL[pocketverbs->outZL] = inputSampleL;
				inputSampleL *= constallpass;
				pocketverbs->outZL--;
				if (pocketverbs->outZL < 0 || pocketverbs->outZL > pocketverbs->delayZ) {
					pocketverbs->outZL = pocketverbs->delayZ;
				}
				inputSampleL += (pocketverbs->oZL[pocketverbs->outZL]);
				// allpass filter Z

				pocketverbs->dZL[6] = pocketverbs->dZL[5];
				pocketverbs->dZL[5] = pocketverbs->dZL[4];
				pocketverbs->dZL[4] = inputSampleL;
				inputSampleL = (pocketverbs->dZL[4] + pocketverbs->dZL[5] + pocketverbs->dZL[6]);
				// output Zarathustra infinite space verb
				break;
		}
		// end big switch for verb type

		switch (verbtype) {

			case 1: // Chamber
				allpasstemp = pocketverbs->alpAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->aAR[allpasstemp] * constallpass;
				pocketverbs->aAR[pocketverbs->alpAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpAR--;
				if (pocketverbs->alpAR < 0 || pocketverbs->alpAR > pocketverbs->delayA) {
					pocketverbs->alpAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->aAR[pocketverbs->alpAR]);
				// allpass filter A

				pocketverbs->dAR[3] = pocketverbs->dAR[2];
				pocketverbs->dAR[2] = pocketverbs->dAR[1];
				pocketverbs->dAR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dAR[2] + pocketverbs->dAR[3]) / 3.0;

				allpasstemp = pocketverbs->alpBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->aBR[allpasstemp] * constallpass;
				pocketverbs->aBR[pocketverbs->alpBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpBR--;
				if (pocketverbs->alpBR < 0 || pocketverbs->alpBR > pocketverbs->delayB) {
					pocketverbs->alpBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->aBR[pocketverbs->alpBR]);
				// allpass filter B

				pocketverbs->dBR[3] = pocketverbs->dBR[2];
				pocketverbs->dBR[2] = pocketverbs->dBR[1];
				pocketverbs->dBR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dBR[2] + pocketverbs->dBR[3]) / 3.0;

				allpasstemp = pocketverbs->alpCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->aCR[allpasstemp] * constallpass;
				pocketverbs->aCR[pocketverbs->alpCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpCR--;
				if (pocketverbs->alpCR < 0 || pocketverbs->alpCR > pocketverbs->delayC) {
					pocketverbs->alpCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->aCR[pocketverbs->alpCR]);
				// allpass filter C

				pocketverbs->dCR[3] = pocketverbs->dCR[2];
				pocketverbs->dCR[2] = pocketverbs->dCR[1];
				pocketverbs->dCR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dCR[2] + pocketverbs->dCR[3]) / 3.0;

				allpasstemp = pocketverbs->alpDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->aDR[allpasstemp] * constallpass;
				pocketverbs->aDR[pocketverbs->alpDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpDR--;
				if (pocketverbs->alpDR < 0 || pocketverbs->alpDR > pocketverbs->delayD) {
					pocketverbs->alpDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->aDR[pocketverbs->alpDR]);
				// allpass filter D

				pocketverbs->dDR[3] = pocketverbs->dDR[2];
				pocketverbs->dDR[2] = pocketverbs->dDR[1];
				pocketverbs->dDR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dDR[2] + pocketverbs->dDR[3]) / 3.0;

				allpasstemp = pocketverbs->alpER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->aER[allpasstemp] * constallpass;
				pocketverbs->aER[pocketverbs->alpER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpER--;
				if (pocketverbs->alpER < 0 || pocketverbs->alpER > pocketverbs->delayE) {
					pocketverbs->alpER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->aER[pocketverbs->alpER]);
				// allpass filter E

				pocketverbs->dER[3] = pocketverbs->dER[2];
				pocketverbs->dER[2] = pocketverbs->dER[1];
				pocketverbs->dER[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dER[2] + pocketverbs->dER[3]) / 3.0;

				allpasstemp = pocketverbs->alpFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->aFR[allpasstemp] * constallpass;
				pocketverbs->aFR[pocketverbs->alpFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpFR--;
				if (pocketverbs->alpFR < 0 || pocketverbs->alpFR > pocketverbs->delayF) {
					pocketverbs->alpFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->aFR[pocketverbs->alpFR]);
				// allpass filter F

				pocketverbs->dFR[3] = pocketverbs->dFR[2];
				pocketverbs->dFR[2] = pocketverbs->dFR[1];
				pocketverbs->dFR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dFR[2] + pocketverbs->dFR[3]) / 3.0;

				allpasstemp = pocketverbs->alpGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->aGR[allpasstemp] * constallpass;
				pocketverbs->aGR[pocketverbs->alpGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpGR--;
				if (pocketverbs->alpGR < 0 || pocketverbs->alpGR > pocketverbs->delayG) {
					pocketverbs->alpGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->aGR[pocketverbs->alpGR]);
				// allpass filter G

				pocketverbs->dGR[3] = pocketverbs->dGR[2];
				pocketverbs->dGR[2] = pocketverbs->dGR[1];
				pocketverbs->dGR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dGR[2] + pocketverbs->dGR[3]) / 3.0;

				allpasstemp = pocketverbs->alpHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->aHR[allpasstemp] * constallpass;
				pocketverbs->aHR[pocketverbs->alpHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpHR--;
				if (pocketverbs->alpHR < 0 || pocketverbs->alpHR > pocketverbs->delayH) {
					pocketverbs->alpHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->aHR[pocketverbs->alpHR]);
				// allpass filter H

				pocketverbs->dHR[3] = pocketverbs->dHR[2];
				pocketverbs->dHR[2] = pocketverbs->dHR[1];
				pocketverbs->dHR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dHR[2] + pocketverbs->dHR[3]) / 3.0;

				allpasstemp = pocketverbs->alpIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->aIR[allpasstemp] * constallpass;
				pocketverbs->aIR[pocketverbs->alpIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpIR--;
				if (pocketverbs->alpIR < 0 || pocketverbs->alpIR > pocketverbs->delayI) {
					pocketverbs->alpIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->aIR[pocketverbs->alpIR]);
				// allpass filter I

				pocketverbs->dIR[3] = pocketverbs->dIR[2];
				pocketverbs->dIR[2] = pocketverbs->dIR[1];
				pocketverbs->dIR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dIR[2] + pocketverbs->dIR[3]) / 3.0;

				allpasstemp = pocketverbs->alpJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->aJR[allpasstemp] * constallpass;
				pocketverbs->aJR[pocketverbs->alpJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpJR--;
				if (pocketverbs->alpJR < 0 || pocketverbs->alpJR > pocketverbs->delayJ) {
					pocketverbs->alpJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->aJR[pocketverbs->alpJR]);
				// allpass filter J

				pocketverbs->dJR[3] = pocketverbs->dJR[2];
				pocketverbs->dJR[2] = pocketverbs->dJR[1];
				pocketverbs->dJR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dJR[2] + pocketverbs->dJR[3]) / 3.0;

				allpasstemp = pocketverbs->alpKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->aKR[allpasstemp] * constallpass;
				pocketverbs->aKR[pocketverbs->alpKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpKR--;
				if (pocketverbs->alpKR < 0 || pocketverbs->alpKR > pocketverbs->delayK) {
					pocketverbs->alpKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->aKR[pocketverbs->alpKR]);
				// allpass filter K

				pocketverbs->dKR[3] = pocketverbs->dKR[2];
				pocketverbs->dKR[2] = pocketverbs->dKR[1];
				pocketverbs->dKR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dKR[2] + pocketverbs->dKR[3]) / 3.0;

				allpasstemp = pocketverbs->alpLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->aLR[allpasstemp] * constallpass;
				pocketverbs->aLR[pocketverbs->alpLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpLR--;
				if (pocketverbs->alpLR < 0 || pocketverbs->alpLR > pocketverbs->delayL) {
					pocketverbs->alpLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->aLR[pocketverbs->alpLR]);
				// allpass filter L

				pocketverbs->dLR[3] = pocketverbs->dLR[2];
				pocketverbs->dLR[2] = pocketverbs->dLR[1];
				pocketverbs->dLR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dLR[2] + pocketverbs->dLR[3]) / 3.0;

				allpasstemp = pocketverbs->alpMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->aMR[allpasstemp] * constallpass;
				pocketverbs->aMR[pocketverbs->alpMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpMR--;
				if (pocketverbs->alpMR < 0 || pocketverbs->alpMR > pocketverbs->delayM) {
					pocketverbs->alpMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->aMR[pocketverbs->alpMR]);
				// allpass filter M

				pocketverbs->dMR[3] = pocketverbs->dMR[2];
				pocketverbs->dMR[2] = pocketverbs->dMR[1];
				pocketverbs->dMR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dMR[2] + pocketverbs->dMR[3]) / 3.0;

				allpasstemp = pocketverbs->alpNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->aNR[allpasstemp] * constallpass;
				pocketverbs->aNR[pocketverbs->alpNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpNR--;
				if (pocketverbs->alpNR < 0 || pocketverbs->alpNR > pocketverbs->delayN) {
					pocketverbs->alpNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->aNR[pocketverbs->alpNR]);
				// allpass filter N

				pocketverbs->dNR[3] = pocketverbs->dNR[2];
				pocketverbs->dNR[2] = pocketverbs->dNR[1];
				pocketverbs->dNR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dNR[2] + pocketverbs->dNR[3]) / 3.0;

				allpasstemp = pocketverbs->alpOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->aOR[allpasstemp] * constallpass;
				pocketverbs->aOR[pocketverbs->alpOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpOR--;
				if (pocketverbs->alpOR < 0 || pocketverbs->alpOR > pocketverbs->delayO) {
					pocketverbs->alpOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->aOR[pocketverbs->alpOR]);
				// allpass filter O

				pocketverbs->dOR[3] = pocketverbs->dOR[2];
				pocketverbs->dOR[2] = pocketverbs->dOR[1];
				pocketverbs->dOR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dOR[2] + pocketverbs->dOR[3]) / 3.0;

				allpasstemp = pocketverbs->alpPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->aPR[allpasstemp] * constallpass;
				pocketverbs->aPR[pocketverbs->alpPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpPR--;
				if (pocketverbs->alpPR < 0 || pocketverbs->alpPR > pocketverbs->delayP) {
					pocketverbs->alpPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->aPR[pocketverbs->alpPR]);
				// allpass filter P

				pocketverbs->dPR[3] = pocketverbs->dPR[2];
				pocketverbs->dPR[2] = pocketverbs->dPR[1];
				pocketverbs->dPR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dPR[2] + pocketverbs->dPR[3]) / 3.0;

				allpasstemp = pocketverbs->alpQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->aQR[allpasstemp] * constallpass;
				pocketverbs->aQR[pocketverbs->alpQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpQR--;
				if (pocketverbs->alpQR < 0 || pocketverbs->alpQR > pocketverbs->delayQ) {
					pocketverbs->alpQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->aQR[pocketverbs->alpQR]);
				// allpass filter Q

				pocketverbs->dQR[3] = pocketverbs->dQR[2];
				pocketverbs->dQR[2] = pocketverbs->dQR[1];
				pocketverbs->dQR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dQR[2] + pocketverbs->dQR[3]) / 3.0;

				allpasstemp = pocketverbs->alpRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->aRR[allpasstemp] * constallpass;
				pocketverbs->aRR[pocketverbs->alpRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpRR--;
				if (pocketverbs->alpRR < 0 || pocketverbs->alpRR > pocketverbs->delayR) {
					pocketverbs->alpRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->aRR[pocketverbs->alpRR]);
				// allpass filter R

				pocketverbs->dRR[3] = pocketverbs->dRR[2];
				pocketverbs->dRR[2] = pocketverbs->dRR[1];
				pocketverbs->dRR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dRR[2] + pocketverbs->dRR[3]) / 3.0;

				allpasstemp = pocketverbs->alpSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->aSR[allpasstemp] * constallpass;
				pocketverbs->aSR[pocketverbs->alpSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpSR--;
				if (pocketverbs->alpSR < 0 || pocketverbs->alpSR > pocketverbs->delayS) {
					pocketverbs->alpSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->aSR[pocketverbs->alpSR]);
				// allpass filter S

				pocketverbs->dSR[3] = pocketverbs->dSR[2];
				pocketverbs->dSR[2] = pocketverbs->dSR[1];
				pocketverbs->dSR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dSR[2] + pocketverbs->dSR[3]) / 3.0;

				allpasstemp = pocketverbs->alpTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->aTR[allpasstemp] * constallpass;
				pocketverbs->aTR[pocketverbs->alpTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpTR--;
				if (pocketverbs->alpTR < 0 || pocketverbs->alpTR > pocketverbs->delayT) {
					pocketverbs->alpTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->aTR[pocketverbs->alpTR]);
				// allpass filter T

				pocketverbs->dTR[3] = pocketverbs->dTR[2];
				pocketverbs->dTR[2] = pocketverbs->dTR[1];
				pocketverbs->dTR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dTR[2] + pocketverbs->dTR[3]) / 3.0;

				allpasstemp = pocketverbs->alpUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->aUR[allpasstemp] * constallpass;
				pocketverbs->aUR[pocketverbs->alpUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpUR--;
				if (pocketverbs->alpUR < 0 || pocketverbs->alpUR > pocketverbs->delayU) {
					pocketverbs->alpUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->aUR[pocketverbs->alpUR]);
				// allpass filter U

				pocketverbs->dUR[3] = pocketverbs->dUR[2];
				pocketverbs->dUR[2] = pocketverbs->dUR[1];
				pocketverbs->dUR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dUR[2] + pocketverbs->dUR[3]) / 3.0;

				allpasstemp = pocketverbs->alpVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->aVR[allpasstemp] * constallpass;
				pocketverbs->aVR[pocketverbs->alpVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpVR--;
				if (pocketverbs->alpVR < 0 || pocketverbs->alpVR > pocketverbs->delayV) {
					pocketverbs->alpVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->aVR[pocketverbs->alpVR]);
				// allpass filter V

				pocketverbs->dVR[3] = pocketverbs->dVR[2];
				pocketverbs->dVR[2] = pocketverbs->dVR[1];
				pocketverbs->dVR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dVR[2] + pocketverbs->dVR[3]) / 3.0;

				allpasstemp = pocketverbs->alpWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->aWR[allpasstemp] * constallpass;
				pocketverbs->aWR[pocketverbs->alpWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpWR--;
				if (pocketverbs->alpWR < 0 || pocketverbs->alpWR > pocketverbs->delayW) {
					pocketverbs->alpWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->aWR[pocketverbs->alpWR]);
				// allpass filter W

				pocketverbs->dWR[3] = pocketverbs->dWR[2];
				pocketverbs->dWR[2] = pocketverbs->dWR[1];
				pocketverbs->dWR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dWR[2] + pocketverbs->dWR[3]) / 3.0;

				allpasstemp = pocketverbs->alpXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->aXR[allpasstemp] * constallpass;
				pocketverbs->aXR[pocketverbs->alpXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpXR--;
				if (pocketverbs->alpXR < 0 || pocketverbs->alpXR > pocketverbs->delayX) {
					pocketverbs->alpXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->aXR[pocketverbs->alpXR]);
				// allpass filter X

				pocketverbs->dXR[3] = pocketverbs->dXR[2];
				pocketverbs->dXR[2] = pocketverbs->dXR[1];
				pocketverbs->dXR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dXR[2] + pocketverbs->dXR[3]) / 3.0;

				allpasstemp = pocketverbs->alpYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->aYR[allpasstemp] * constallpass;
				pocketverbs->aYR[pocketverbs->alpYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpYR--;
				if (pocketverbs->alpYR < 0 || pocketverbs->alpYR > pocketverbs->delayY) {
					pocketverbs->alpYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->aYR[pocketverbs->alpYR]);
				// allpass filter Y

				pocketverbs->dYR[3] = pocketverbs->dYR[2];
				pocketverbs->dYR[2] = pocketverbs->dYR[1];
				pocketverbs->dYR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dYR[2] + pocketverbs->dYR[3]) / 3.0;

				allpasstemp = pocketverbs->alpZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->aZR[allpasstemp] * constallpass;
				pocketverbs->aZR[pocketverbs->alpZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpZR--;
				if (pocketverbs->alpZR < 0 || pocketverbs->alpZR > pocketverbs->delayZ) {
					pocketverbs->alpZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->aZR[pocketverbs->alpZR]);
				// allpass filter Z

				pocketverbs->dZR[3] = pocketverbs->dZR[2];
				pocketverbs->dZR[2] = pocketverbs->dZR[1];
				pocketverbs->dZR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[1] + pocketverbs->dZR[2] + pocketverbs->dZR[3]) / 3.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->oAR[allpasstemp] * constallpass;
				pocketverbs->oAR[pocketverbs->outAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outAR--;
				if (pocketverbs->outAR < 0 || pocketverbs->outAR > pocketverbs->delayA) {
					pocketverbs->outAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->oAR[pocketverbs->outAR]);
				// allpass filter A

				pocketverbs->dAR[6] = pocketverbs->dAR[5];
				pocketverbs->dAR[5] = pocketverbs->dAR[4];
				pocketverbs->dAR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[4] + pocketverbs->dAR[5] + pocketverbs->dAR[6]) / 3.0;

				allpasstemp = pocketverbs->outBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->oBR[allpasstemp] * constallpass;
				pocketverbs->oBR[pocketverbs->outBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outBR--;
				if (pocketverbs->outBR < 0 || pocketverbs->outBR > pocketverbs->delayB) {
					pocketverbs->outBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->oBR[pocketverbs->outBR]);
				// allpass filter B

				pocketverbs->dBR[6] = pocketverbs->dBR[5];
				pocketverbs->dBR[5] = pocketverbs->dBR[4];
				pocketverbs->dBR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[4] + pocketverbs->dBR[5] + pocketverbs->dBR[6]) / 3.0;

				allpasstemp = pocketverbs->outCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->oCR[allpasstemp] * constallpass;
				pocketverbs->oCR[pocketverbs->outCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outCR--;
				if (pocketverbs->outCR < 0 || pocketverbs->outCR > pocketverbs->delayC) {
					pocketverbs->outCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->oCR[pocketverbs->outCR]);
				// allpass filter C

				pocketverbs->dCR[6] = pocketverbs->dCR[5];
				pocketverbs->dCR[5] = pocketverbs->dCR[4];
				pocketverbs->dCR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dCR[5] + pocketverbs->dCR[6]) / 3.0;

				allpasstemp = pocketverbs->outDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->oDR[allpasstemp] * constallpass;
				pocketverbs->oDR[pocketverbs->outDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outDR--;
				if (pocketverbs->outDR < 0 || pocketverbs->outDR > pocketverbs->delayD) {
					pocketverbs->outDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->oDR[pocketverbs->outDR]);
				// allpass filter D

				pocketverbs->dDR[6] = pocketverbs->dDR[5];
				pocketverbs->dDR[5] = pocketverbs->dDR[4];
				pocketverbs->dDR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[4] + pocketverbs->dDR[5] + pocketverbs->dDR[6]) / 3.0;

				allpasstemp = pocketverbs->outER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->oER[allpasstemp] * constallpass;
				pocketverbs->oER[pocketverbs->outER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outER--;
				if (pocketverbs->outER < 0 || pocketverbs->outER > pocketverbs->delayE) {
					pocketverbs->outER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->oER[pocketverbs->outER]);
				// allpass filter E

				pocketverbs->dER[6] = pocketverbs->dER[5];
				pocketverbs->dER[5] = pocketverbs->dER[4];
				pocketverbs->dER[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dER[5] + pocketverbs->dER[6]) / 3.0;

				allpasstemp = pocketverbs->outFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->oFR[allpasstemp] * constallpass;
				pocketverbs->oFR[pocketverbs->outFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outFR--;
				if (pocketverbs->outFR < 0 || pocketverbs->outFR > pocketverbs->delayF) {
					pocketverbs->outFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->oFR[pocketverbs->outFR]);
				// allpass filter F

				pocketverbs->dFR[6] = pocketverbs->dFR[5];
				pocketverbs->dFR[5] = pocketverbs->dFR[4];
				pocketverbs->dFR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[4] + pocketverbs->dFR[5] + pocketverbs->dFR[6]) / 3.0;

				allpasstemp = pocketverbs->outGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->oGR[allpasstemp] * constallpass;
				pocketverbs->oGR[pocketverbs->outGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outGR--;
				if (pocketverbs->outGR < 0 || pocketverbs->outGR > pocketverbs->delayG) {
					pocketverbs->outGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->oGR[pocketverbs->outGR]);
				// allpass filter G

				pocketverbs->dGR[6] = pocketverbs->dGR[5];
				pocketverbs->dGR[5] = pocketverbs->dGR[4];
				pocketverbs->dGR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[4] + pocketverbs->dGR[5] + pocketverbs->dGR[6]) / 3.0;

				allpasstemp = pocketverbs->outHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->oHR[allpasstemp] * constallpass;
				pocketverbs->oHR[pocketverbs->outHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outHR--;
				if (pocketverbs->outHR < 0 || pocketverbs->outHR > pocketverbs->delayH) {
					pocketverbs->outHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->oHR[pocketverbs->outHR]);
				// allpass filter H

				pocketverbs->dHR[6] = pocketverbs->dHR[5];
				pocketverbs->dHR[5] = pocketverbs->dHR[4];
				pocketverbs->dHR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[4] + pocketverbs->dHR[5] + pocketverbs->dHR[6]) / 3.0;

				allpasstemp = pocketverbs->outIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->oIR[allpasstemp] * constallpass;
				pocketverbs->oIR[pocketverbs->outIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outIR--;
				if (pocketverbs->outIR < 0 || pocketverbs->outIR > pocketverbs->delayI) {
					pocketverbs->outIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->oIR[pocketverbs->outIR]);
				// allpass filter I

				pocketverbs->dIR[6] = pocketverbs->dIR[5];
				pocketverbs->dIR[5] = pocketverbs->dIR[4];
				pocketverbs->dIR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[4] + pocketverbs->dIR[5] + pocketverbs->dIR[6]) / 3.0;

				allpasstemp = pocketverbs->outJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->oJR[allpasstemp] * constallpass;
				pocketverbs->oJR[pocketverbs->outJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outJR--;
				if (pocketverbs->outJR < 0 || pocketverbs->outJR > pocketverbs->delayJ) {
					pocketverbs->outJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->oJR[pocketverbs->outJR]);
				// allpass filter J

				pocketverbs->dJR[6] = pocketverbs->dJR[5];
				pocketverbs->dJR[5] = pocketverbs->dJR[4];
				pocketverbs->dJR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[4] + pocketverbs->dJR[5] + pocketverbs->dJR[6]) / 3.0;

				allpasstemp = pocketverbs->outKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->oKR[allpasstemp] * constallpass;
				pocketverbs->oKR[pocketverbs->outKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outKR--;
				if (pocketverbs->outKR < 0 || pocketverbs->outKR > pocketverbs->delayK) {
					pocketverbs->outKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->oKR[pocketverbs->outKR]);
				// allpass filter K

				pocketverbs->dKR[6] = pocketverbs->dKR[5];
				pocketverbs->dKR[5] = pocketverbs->dKR[4];
				pocketverbs->dKR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dKR[5] + pocketverbs->dKR[6]) / 3.0;

				allpasstemp = pocketverbs->outLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->oLR[allpasstemp] * constallpass;
				pocketverbs->oLR[pocketverbs->outLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outLR--;
				if (pocketverbs->outLR < 0 || pocketverbs->outLR > pocketverbs->delayL) {
					pocketverbs->outLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->oLR[pocketverbs->outLR]);
				// allpass filter L

				pocketverbs->dLR[6] = pocketverbs->dLR[5];
				pocketverbs->dLR[5] = pocketverbs->dLR[4];
				pocketverbs->dLR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[4] + pocketverbs->dLR[5] + pocketverbs->dLR[6]) / 3.0;

				allpasstemp = pocketverbs->outMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->oMR[allpasstemp] * constallpass;
				pocketverbs->oMR[pocketverbs->outMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outMR--;
				if (pocketverbs->outMR < 0 || pocketverbs->outMR > pocketverbs->delayM) {
					pocketverbs->outMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->oMR[pocketverbs->outMR]);
				// allpass filter M

				pocketverbs->dMR[6] = pocketverbs->dMR[5];
				pocketverbs->dMR[5] = pocketverbs->dMR[4];
				pocketverbs->dMR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[4] + pocketverbs->dMR[5] + pocketverbs->dMR[6]) / 3.0;

				allpasstemp = pocketverbs->outNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->oNR[allpasstemp] * constallpass;
				pocketverbs->oNR[pocketverbs->outNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outNR--;
				if (pocketverbs->outNR < 0 || pocketverbs->outNR > pocketverbs->delayN) {
					pocketverbs->outNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->oNR[pocketverbs->outNR]);
				// allpass filter N

				pocketverbs->dNR[6] = pocketverbs->dNR[5];
				pocketverbs->dNR[5] = pocketverbs->dNR[4];
				pocketverbs->dNR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[4] + pocketverbs->dNR[5] + pocketverbs->dNR[6]) / 3.0;

				allpasstemp = pocketverbs->outOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->oOR[allpasstemp] * constallpass;
				pocketverbs->oOR[pocketverbs->outOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outOR--;
				if (pocketverbs->outOR < 0 || pocketverbs->outOR > pocketverbs->delayO) {
					pocketverbs->outOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->oOR[pocketverbs->outOR]);
				// allpass filter O

				pocketverbs->dOR[6] = pocketverbs->dOR[5];
				pocketverbs->dOR[5] = pocketverbs->dOR[4];
				pocketverbs->dOR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dOR[5] + pocketverbs->dOR[6]) / 3.0;

				allpasstemp = pocketverbs->outPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->oPR[allpasstemp] * constallpass;
				pocketverbs->oPR[pocketverbs->outPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outPR--;
				if (pocketverbs->outPR < 0 || pocketverbs->outPR > pocketverbs->delayP) {
					pocketverbs->outPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->oPR[pocketverbs->outPR]);
				// allpass filter P

				pocketverbs->dPR[6] = pocketverbs->dPR[5];
				pocketverbs->dPR[5] = pocketverbs->dPR[4];
				pocketverbs->dPR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[4] + pocketverbs->dPR[5] + pocketverbs->dPR[6]) / 3.0;

				allpasstemp = pocketverbs->outQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->oQR[allpasstemp] * constallpass;
				pocketverbs->oQR[pocketverbs->outQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outQR--;
				if (pocketverbs->outQR < 0 || pocketverbs->outQR > pocketverbs->delayQ) {
					pocketverbs->outQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->oQR[pocketverbs->outQR]);
				// allpass filter Q

				pocketverbs->dQR[6] = pocketverbs->dQR[5];
				pocketverbs->dQR[5] = pocketverbs->dQR[4];
				pocketverbs->dQR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dQR[5] + pocketverbs->dQR[6]) / 3.0;

				allpasstemp = pocketverbs->outRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->oRR[allpasstemp] * constallpass;
				pocketverbs->oRR[pocketverbs->outRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outRR--;
				if (pocketverbs->outRR < 0 || pocketverbs->outRR > pocketverbs->delayR) {
					pocketverbs->outRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->oRR[pocketverbs->outRR]);
				// allpass filter R

				pocketverbs->dRR[6] = pocketverbs->dRR[5];
				pocketverbs->dRR[5] = pocketverbs->dRR[4];
				pocketverbs->dRR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[4] + pocketverbs->dRR[5] + pocketverbs->dRR[6]) / 3.0;

				allpasstemp = pocketverbs->outSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->oSR[allpasstemp] * constallpass;
				pocketverbs->oSR[pocketverbs->outSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outSR--;
				if (pocketverbs->outSR < 0 || pocketverbs->outSR > pocketverbs->delayS) {
					pocketverbs->outSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->oSR[pocketverbs->outSR]);
				// allpass filter S

				pocketverbs->dSR[6] = pocketverbs->dSR[5];
				pocketverbs->dSR[5] = pocketverbs->dSR[4];
				pocketverbs->dSR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[4] + pocketverbs->dSR[5] + pocketverbs->dSR[6]) / 3.0;

				allpasstemp = pocketverbs->outTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->oTR[allpasstemp] * constallpass;
				pocketverbs->oTR[pocketverbs->outTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outTR--;
				if (pocketverbs->outTR < 0 || pocketverbs->outTR > pocketverbs->delayT) {
					pocketverbs->outTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->oTR[pocketverbs->outTR]);
				// allpass filter T

				pocketverbs->dTR[6] = pocketverbs->dTR[5];
				pocketverbs->dTR[5] = pocketverbs->dTR[4];
				pocketverbs->dTR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[4] + pocketverbs->dTR[5] + pocketverbs->dTR[6]) / 3.0;

				allpasstemp = pocketverbs->outUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->oUR[allpasstemp] * constallpass;
				pocketverbs->oUR[pocketverbs->outUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outUR--;
				if (pocketverbs->outUR < 0 || pocketverbs->outUR > pocketverbs->delayU) {
					pocketverbs->outUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->oUR[pocketverbs->outUR]);
				// allpass filter U

				pocketverbs->dUR[6] = pocketverbs->dUR[5];
				pocketverbs->dUR[5] = pocketverbs->dUR[4];
				pocketverbs->dUR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dUR[5] + pocketverbs->dUR[6]) / 3.0;

				allpasstemp = pocketverbs->outVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->oVR[allpasstemp] * constallpass;
				pocketverbs->oVR[pocketverbs->outVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outVR--;
				if (pocketverbs->outVR < 0 || pocketverbs->outVR > pocketverbs->delayV) {
					pocketverbs->outVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->oVR[pocketverbs->outVR]);
				// allpass filter V

				pocketverbs->dVR[6] = pocketverbs->dVR[5];
				pocketverbs->dVR[5] = pocketverbs->dVR[4];
				pocketverbs->dVR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[4] + pocketverbs->dVR[5] + pocketverbs->dVR[6]) / 3.0;

				allpasstemp = pocketverbs->outWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->oWR[allpasstemp] * constallpass;
				pocketverbs->oWR[pocketverbs->outWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outWR--;
				if (pocketverbs->outWR < 0 || pocketverbs->outWR > pocketverbs->delayW) {
					pocketverbs->outWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->oWR[pocketverbs->outWR]);
				// allpass filter W

				pocketverbs->dWR[6] = pocketverbs->dWR[5];
				pocketverbs->dWR[5] = pocketverbs->dWR[4];
				pocketverbs->dWR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[4] + pocketverbs->dWR[5] + pocketverbs->dWR[6]) / 3.0;

				allpasstemp = pocketverbs->outXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->oXR[allpasstemp] * constallpass;
				pocketverbs->oXR[pocketverbs->outXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outXR--;
				if (pocketverbs->outXR < 0 || pocketverbs->outXR > pocketverbs->delayX) {
					pocketverbs->outXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->oXR[pocketverbs->outXR]);
				// allpass filter X

				pocketverbs->dXR[6] = pocketverbs->dXR[5];
				pocketverbs->dXR[5] = pocketverbs->dXR[4];
				pocketverbs->dXR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[4] + pocketverbs->dXR[5] + pocketverbs->dXR[6]) / 3.0;

				allpasstemp = pocketverbs->outYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->oYR[allpasstemp] * constallpass;
				pocketverbs->oYR[pocketverbs->outYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outYR--;
				if (pocketverbs->outYR < 0 || pocketverbs->outYR > pocketverbs->delayY) {
					pocketverbs->outYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->oYR[pocketverbs->outYR]);
				// allpass filter Y

				pocketverbs->dYR[6] = pocketverbs->dYR[5];
				pocketverbs->dYR[5] = pocketverbs->dYR[4];
				pocketverbs->dYR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[4] + pocketverbs->dYR[5] + pocketverbs->dYR[6]) / 3.0;

				allpasstemp = pocketverbs->outZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->oZR[allpasstemp] * constallpass;
				pocketverbs->oZR[pocketverbs->outZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outZR--;
				if (pocketverbs->outZR < 0 || pocketverbs->outZR > pocketverbs->delayZ) {
					pocketverbs->outZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->oZR[pocketverbs->outZR]);
				// allpass filter Z

				pocketverbs->dZR[6] = pocketverbs->dZR[5];
				pocketverbs->dZR[5] = pocketverbs->dZR[4];
				pocketverbs->dZR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[4] + pocketverbs->dZR[5] + pocketverbs->dZR[6]);
				// output Chamber
				break;

			case 2: // Spring

				allpasstemp = pocketverbs->alpAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->aAR[allpasstemp] * constallpass;
				pocketverbs->aAR[pocketverbs->alpAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpAR--;
				if (pocketverbs->alpAR < 0 || pocketverbs->alpAR > pocketverbs->delayA) {
					pocketverbs->alpAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->aAR[pocketverbs->alpAR]);
				// allpass filter A

				pocketverbs->dAR[3] = pocketverbs->dAR[2];
				pocketverbs->dAR[2] = pocketverbs->dAR[1];
				pocketverbs->dAR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dAR[2] + pocketverbs->dAR[3]) / 3.0;

				allpasstemp = pocketverbs->alpBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->aBR[allpasstemp] * constallpass;
				pocketverbs->aBR[pocketverbs->alpBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpBR--;
				if (pocketverbs->alpBR < 0 || pocketverbs->alpBR > pocketverbs->delayB) {
					pocketverbs->alpBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->aBR[pocketverbs->alpBR]);
				// allpass filter B

				pocketverbs->dBR[3] = pocketverbs->dBR[2];
				pocketverbs->dBR[2] = pocketverbs->dBR[1];
				pocketverbs->dBR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dBR[2] + pocketverbs->dBR[3]) / 3.0;

				allpasstemp = pocketverbs->alpCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->aCR[allpasstemp] * constallpass;
				pocketverbs->aCR[pocketverbs->alpCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpCR--;
				if (pocketverbs->alpCR < 0 || pocketverbs->alpCR > pocketverbs->delayC) {
					pocketverbs->alpCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->aCR[pocketverbs->alpCR]);
				// allpass filter C

				pocketverbs->dCR[3] = pocketverbs->dCR[2];
				pocketverbs->dCR[2] = pocketverbs->dCR[1];
				pocketverbs->dCR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[1] + pocketverbs->dCR[2] + pocketverbs->dCR[3]) / 3.0;

				allpasstemp = pocketverbs->alpDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->aDR[allpasstemp] * constallpass;
				pocketverbs->aDR[pocketverbs->alpDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpDR--;
				if (pocketverbs->alpDR < 0 || pocketverbs->alpDR > pocketverbs->delayD) {
					pocketverbs->alpDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->aDR[pocketverbs->alpDR]);
				// allpass filter D

				pocketverbs->dDR[3] = pocketverbs->dDR[2];
				pocketverbs->dDR[2] = pocketverbs->dDR[1];
				pocketverbs->dDR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dDR[2] + pocketverbs->dDR[3]) / 3.0;

				allpasstemp = pocketverbs->alpER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->aER[allpasstemp] * constallpass;
				pocketverbs->aER[pocketverbs->alpER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpER--;
				if (pocketverbs->alpER < 0 || pocketverbs->alpER > pocketverbs->delayE) {
					pocketverbs->alpER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->aER[pocketverbs->alpER]);
				// allpass filter E

				pocketverbs->dER[3] = pocketverbs->dER[2];
				pocketverbs->dER[2] = pocketverbs->dER[1];
				pocketverbs->dER[1] = inputSampleR;
				inputSampleR = (pocketverbs->dER[1] + pocketverbs->dER[2] + pocketverbs->dER[3]) / 3.0;

				allpasstemp = pocketverbs->alpFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->aFR[allpasstemp] * constallpass;
				pocketverbs->aFR[pocketverbs->alpFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpFR--;
				if (pocketverbs->alpFR < 0 || pocketverbs->alpFR > pocketverbs->delayF) {
					pocketverbs->alpFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->aFR[pocketverbs->alpFR]);
				// allpass filter F

				pocketverbs->dFR[3] = pocketverbs->dFR[2];
				pocketverbs->dFR[2] = pocketverbs->dFR[1];
				pocketverbs->dFR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dFR[2] + pocketverbs->dFR[3]) / 3.0;

				allpasstemp = pocketverbs->alpGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->aGR[allpasstemp] * constallpass;
				pocketverbs->aGR[pocketverbs->alpGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpGR--;
				if (pocketverbs->alpGR < 0 || pocketverbs->alpGR > pocketverbs->delayG) {
					pocketverbs->alpGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->aGR[pocketverbs->alpGR]);
				// allpass filter G

				pocketverbs->dGR[3] = pocketverbs->dGR[2];
				pocketverbs->dGR[2] = pocketverbs->dGR[1];
				pocketverbs->dGR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[1] + pocketverbs->dGR[2] + pocketverbs->dGR[3]) / 3.0;

				allpasstemp = pocketverbs->alpHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->aHR[allpasstemp] * constallpass;
				pocketverbs->aHR[pocketverbs->alpHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpHR--;
				if (pocketverbs->alpHR < 0 || pocketverbs->alpHR > pocketverbs->delayH) {
					pocketverbs->alpHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->aHR[pocketverbs->alpHR]);
				// allpass filter H

				pocketverbs->dHR[3] = pocketverbs->dHR[2];
				pocketverbs->dHR[2] = pocketverbs->dHR[1];
				pocketverbs->dHR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dHR[2] + pocketverbs->dHR[3]) / 3.0;

				allpasstemp = pocketverbs->alpIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->aIR[allpasstemp] * constallpass;
				pocketverbs->aIR[pocketverbs->alpIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpIR--;
				if (pocketverbs->alpIR < 0 || pocketverbs->alpIR > pocketverbs->delayI) {
					pocketverbs->alpIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->aIR[pocketverbs->alpIR]);
				// allpass filter I

				pocketverbs->dIR[3] = pocketverbs->dIR[2];
				pocketverbs->dIR[2] = pocketverbs->dIR[1];
				pocketverbs->dIR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dIR[2] + pocketverbs->dIR[3]) / 3.0;

				allpasstemp = pocketverbs->alpJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->aJR[allpasstemp] * constallpass;
				pocketverbs->aJR[pocketverbs->alpJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpJR--;
				if (pocketverbs->alpJR < 0 || pocketverbs->alpJR > pocketverbs->delayJ) {
					pocketverbs->alpJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->aJR[pocketverbs->alpJR]);
				// allpass filter J

				pocketverbs->dJR[3] = pocketverbs->dJR[2];
				pocketverbs->dJR[2] = pocketverbs->dJR[1];
				pocketverbs->dJR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dJR[2] + pocketverbs->dJR[3]) / 3.0;

				allpasstemp = pocketverbs->alpKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->aKR[allpasstemp] * constallpass;
				pocketverbs->aKR[pocketverbs->alpKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpKR--;
				if (pocketverbs->alpKR < 0 || pocketverbs->alpKR > pocketverbs->delayK) {
					pocketverbs->alpKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->aKR[pocketverbs->alpKR]);
				// allpass filter K

				pocketverbs->dKR[3] = pocketverbs->dKR[2];
				pocketverbs->dKR[2] = pocketverbs->dKR[1];
				pocketverbs->dKR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[1] + pocketverbs->dKR[2] + pocketverbs->dKR[3]) / 3.0;

				allpasstemp = pocketverbs->alpLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->aLR[allpasstemp] * constallpass;
				pocketverbs->aLR[pocketverbs->alpLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpLR--;
				if (pocketverbs->alpLR < 0 || pocketverbs->alpLR > pocketverbs->delayL) {
					pocketverbs->alpLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->aLR[pocketverbs->alpLR]);
				// allpass filter L

				pocketverbs->dLR[3] = pocketverbs->dLR[2];
				pocketverbs->dLR[2] = pocketverbs->dLR[1];
				pocketverbs->dLR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dLR[2] + pocketverbs->dLR[3]) / 3.0;

				allpasstemp = pocketverbs->alpMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->aMR[allpasstemp] * constallpass;
				pocketverbs->aMR[pocketverbs->alpMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpMR--;
				if (pocketverbs->alpMR < 0 || pocketverbs->alpMR > pocketverbs->delayM) {
					pocketverbs->alpMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->aMR[pocketverbs->alpMR]);
				// allpass filter M

				pocketverbs->dMR[3] = pocketverbs->dMR[2];
				pocketverbs->dMR[2] = pocketverbs->dMR[1];
				pocketverbs->dMR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[1] + pocketverbs->dMR[2] + pocketverbs->dMR[3]) / 3.0;

				allpasstemp = pocketverbs->alpNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->aNR[allpasstemp] * constallpass;
				pocketverbs->aNR[pocketverbs->alpNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpNR--;
				if (pocketverbs->alpNR < 0 || pocketverbs->alpNR > pocketverbs->delayN) {
					pocketverbs->alpNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->aNR[pocketverbs->alpNR]);
				// allpass filter N

				pocketverbs->dNR[3] = pocketverbs->dNR[2];
				pocketverbs->dNR[2] = pocketverbs->dNR[1];
				pocketverbs->dNR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dNR[2] + pocketverbs->dNR[3]) / 3.0;

				allpasstemp = pocketverbs->alpOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->aOR[allpasstemp] * constallpass;
				pocketverbs->aOR[pocketverbs->alpOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpOR--;
				if (pocketverbs->alpOR < 0 || pocketverbs->alpOR > pocketverbs->delayO) {
					pocketverbs->alpOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->aOR[pocketverbs->alpOR]);
				// allpass filter O

				pocketverbs->dOR[3] = pocketverbs->dOR[2];
				pocketverbs->dOR[2] = pocketverbs->dOR[1];
				pocketverbs->dOR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dOR[2] + pocketverbs->dOR[3]) / 3.0;

				allpasstemp = pocketverbs->alpPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->aPR[allpasstemp] * constallpass;
				pocketverbs->aPR[pocketverbs->alpPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpPR--;
				if (pocketverbs->alpPR < 0 || pocketverbs->alpPR > pocketverbs->delayP) {
					pocketverbs->alpPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->aPR[pocketverbs->alpPR]);
				// allpass filter P

				pocketverbs->dPR[3] = pocketverbs->dPR[2];
				pocketverbs->dPR[2] = pocketverbs->dPR[1];
				pocketverbs->dPR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dPR[2] + pocketverbs->dPR[3]) / 3.0;

				allpasstemp = pocketverbs->alpQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->aQR[allpasstemp] * constallpass;
				pocketverbs->aQR[pocketverbs->alpQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpQR--;
				if (pocketverbs->alpQR < 0 || pocketverbs->alpQR > pocketverbs->delayQ) {
					pocketverbs->alpQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->aQR[pocketverbs->alpQR]);
				// allpass filter Q

				pocketverbs->dQR[3] = pocketverbs->dQR[2];
				pocketverbs->dQR[2] = pocketverbs->dQR[1];
				pocketverbs->dQR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[1] + pocketverbs->dQR[2] + pocketverbs->dQR[3]) / 3.0;

				allpasstemp = pocketverbs->alpRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->aRR[allpasstemp] * constallpass;
				pocketverbs->aRR[pocketverbs->alpRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpRR--;
				if (pocketverbs->alpRR < 0 || pocketverbs->alpRR > pocketverbs->delayR) {
					pocketverbs->alpRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->aRR[pocketverbs->alpRR]);
				// allpass filter R

				pocketverbs->dRR[3] = pocketverbs->dRR[2];
				pocketverbs->dRR[2] = pocketverbs->dRR[1];
				pocketverbs->dRR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dRR[2] + pocketverbs->dRR[3]) / 3.0;

				allpasstemp = pocketverbs->alpSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->aSR[allpasstemp] * constallpass;
				pocketverbs->aSR[pocketverbs->alpSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpSR--;
				if (pocketverbs->alpSR < 0 || pocketverbs->alpSR > pocketverbs->delayS) {
					pocketverbs->alpSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->aSR[pocketverbs->alpSR]);
				// allpass filter S

				pocketverbs->dSR[3] = pocketverbs->dSR[2];
				pocketverbs->dSR[2] = pocketverbs->dSR[1];
				pocketverbs->dSR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[1] + pocketverbs->dSR[2] + pocketverbs->dSR[3]) / 3.0;

				allpasstemp = pocketverbs->alpTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->aTR[allpasstemp] * constallpass;
				pocketverbs->aTR[pocketverbs->alpTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpTR--;
				if (pocketverbs->alpTR < 0 || pocketverbs->alpTR > pocketverbs->delayT) {
					pocketverbs->alpTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->aTR[pocketverbs->alpTR]);
				// allpass filter T

				pocketverbs->dTR[3] = pocketverbs->dTR[2];
				pocketverbs->dTR[2] = pocketverbs->dTR[1];
				pocketverbs->dTR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dTR[2] + pocketverbs->dTR[3]) / 3.0;

				allpasstemp = pocketverbs->alpUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->aUR[allpasstemp] * constallpass;
				pocketverbs->aUR[pocketverbs->alpUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpUR--;
				if (pocketverbs->alpUR < 0 || pocketverbs->alpUR > pocketverbs->delayU) {
					pocketverbs->alpUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->aUR[pocketverbs->alpUR]);
				// allpass filter U

				pocketverbs->dUR[3] = pocketverbs->dUR[2];
				pocketverbs->dUR[2] = pocketverbs->dUR[1];
				pocketverbs->dUR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dUR[2] + pocketverbs->dUR[3]) / 3.0;

				allpasstemp = pocketverbs->alpVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->aVR[allpasstemp] * constallpass;
				pocketverbs->aVR[pocketverbs->alpVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpVR--;
				if (pocketverbs->alpVR < 0 || pocketverbs->alpVR > pocketverbs->delayV) {
					pocketverbs->alpVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->aVR[pocketverbs->alpVR]);
				// allpass filter V

				pocketverbs->dVR[3] = pocketverbs->dVR[2];
				pocketverbs->dVR[2] = pocketverbs->dVR[1];
				pocketverbs->dVR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dVR[2] + pocketverbs->dVR[3]) / 3.0;

				allpasstemp = pocketverbs->alpWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->aWR[allpasstemp] * constallpass;
				pocketverbs->aWR[pocketverbs->alpWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpWR--;
				if (pocketverbs->alpWR < 0 || pocketverbs->alpWR > pocketverbs->delayW) {
					pocketverbs->alpWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->aWR[pocketverbs->alpWR]);
				// allpass filter W

				pocketverbs->dWR[3] = pocketverbs->dWR[2];
				pocketverbs->dWR[2] = pocketverbs->dWR[1];
				pocketverbs->dWR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[1] + pocketverbs->dWR[2] + pocketverbs->dWR[3]) / 3.0;

				allpasstemp = pocketverbs->alpXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->aXR[allpasstemp] * constallpass;
				pocketverbs->aXR[pocketverbs->alpXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpXR--;
				if (pocketverbs->alpXR < 0 || pocketverbs->alpXR > pocketverbs->delayX) {
					pocketverbs->alpXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->aXR[pocketverbs->alpXR]);
				// allpass filter X

				pocketverbs->dXR[3] = pocketverbs->dXR[2];
				pocketverbs->dXR[2] = pocketverbs->dXR[1];
				pocketverbs->dXR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dXR[2] + pocketverbs->dXR[3]) / 3.0;

				allpasstemp = pocketverbs->alpYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->aYR[allpasstemp] * constallpass;
				pocketverbs->aYR[pocketverbs->alpYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpYR--;
				if (pocketverbs->alpYR < 0 || pocketverbs->alpYR > pocketverbs->delayY) {
					pocketverbs->alpYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->aYR[pocketverbs->alpYR]);
				// allpass filter Y

				pocketverbs->dYR[3] = pocketverbs->dYR[2];
				pocketverbs->dYR[2] = pocketverbs->dYR[1];
				pocketverbs->dYR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dYR[2] + pocketverbs->dYR[3]) / 3.0;

				allpasstemp = pocketverbs->alpZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->aZR[allpasstemp] * constallpass;
				pocketverbs->aZR[pocketverbs->alpZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpZR--;
				if (pocketverbs->alpZR < 0 || pocketverbs->alpZR > pocketverbs->delayZ) {
					pocketverbs->alpZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->aZR[pocketverbs->alpZR]);
				// allpass filter Z

				pocketverbs->dZR[3] = pocketverbs->dZR[2];
				pocketverbs->dZR[2] = pocketverbs->dZR[1];
				pocketverbs->dZR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[1] + pocketverbs->dZR[2] + pocketverbs->dZR[3]) / 3.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->oAR[allpasstemp] * constallpass;
				pocketverbs->oAR[pocketverbs->outAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outAR--;
				if (pocketverbs->outAR < 0 || pocketverbs->outAR > pocketverbs->delayA) {
					pocketverbs->outAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->oAR[pocketverbs->outAR]);
				// allpass filter A

				pocketverbs->dAR[6] = pocketverbs->dAR[5];
				pocketverbs->dAR[5] = pocketverbs->dAR[4];
				pocketverbs->dAR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dAR[5] + pocketverbs->dAR[6]) / 3.0;

				allpasstemp = pocketverbs->outBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->oBR[allpasstemp] * constallpass;
				pocketverbs->oBR[pocketverbs->outBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outBR--;
				if (pocketverbs->outBR < 0 || pocketverbs->outBR > pocketverbs->delayB) {
					pocketverbs->outBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->oBR[pocketverbs->outBR]);
				// allpass filter B

				pocketverbs->dBR[6] = pocketverbs->dBR[5];
				pocketverbs->dBR[5] = pocketverbs->dBR[4];
				pocketverbs->dBR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dBR[5] + pocketverbs->dBR[6]) / 3.0;

				allpasstemp = pocketverbs->outCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->oCR[allpasstemp] * constallpass;
				pocketverbs->oCR[pocketverbs->outCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outCR--;
				if (pocketverbs->outCR < 0 || pocketverbs->outCR > pocketverbs->delayC) {
					pocketverbs->outCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->oCR[pocketverbs->outCR]);
				// allpass filter C

				pocketverbs->dCR[6] = pocketverbs->dCR[5];
				pocketverbs->dCR[5] = pocketverbs->dCR[4];
				pocketverbs->dCR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[1] + pocketverbs->dCR[5] + pocketverbs->dCR[6]) / 3.0;

				allpasstemp = pocketverbs->outDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->oDR[allpasstemp] * constallpass;
				pocketverbs->oDR[pocketverbs->outDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outDR--;
				if (pocketverbs->outDR < 0 || pocketverbs->outDR > pocketverbs->delayD) {
					pocketverbs->outDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->oDR[pocketverbs->outDR]);
				// allpass filter D

				pocketverbs->dDR[6] = pocketverbs->dDR[5];
				pocketverbs->dDR[5] = pocketverbs->dDR[4];
				pocketverbs->dDR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dDR[5] + pocketverbs->dDR[6]) / 3.0;

				allpasstemp = pocketverbs->outER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->oER[allpasstemp] * constallpass;
				pocketverbs->oER[pocketverbs->outER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outER--;
				if (pocketverbs->outER < 0 || pocketverbs->outER > pocketverbs->delayE) {
					pocketverbs->outER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->oER[pocketverbs->outER]);
				// allpass filter E

				pocketverbs->dER[6] = pocketverbs->dER[5];
				pocketverbs->dER[5] = pocketverbs->dER[4];
				pocketverbs->dER[4] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dER[5] + pocketverbs->dER[6]) / 3.0;

				allpasstemp = pocketverbs->outFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->oFR[allpasstemp] * constallpass;
				pocketverbs->oFR[pocketverbs->outFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outFR--;
				if (pocketverbs->outFR < 0 || pocketverbs->outFR > pocketverbs->delayF) {
					pocketverbs->outFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->oFR[pocketverbs->outFR]);
				// allpass filter F

				pocketverbs->dFR[6] = pocketverbs->dFR[5];
				pocketverbs->dFR[5] = pocketverbs->dFR[4];
				pocketverbs->dFR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dFR[5] + pocketverbs->dFR[6]) / 3.0;

				allpasstemp = pocketverbs->outGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->oGR[allpasstemp] * constallpass;
				pocketverbs->oGR[pocketverbs->outGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outGR--;
				if (pocketverbs->outGR < 0 || pocketverbs->outGR > pocketverbs->delayG) {
					pocketverbs->outGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->oGR[pocketverbs->outGR]);
				// allpass filter G

				pocketverbs->dGR[6] = pocketverbs->dGR[5];
				pocketverbs->dGR[5] = pocketverbs->dGR[4];
				pocketverbs->dGR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[1] + pocketverbs->dGR[5] + pocketverbs->dGR[6]) / 3.0;

				allpasstemp = pocketverbs->outHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->oHR[allpasstemp] * constallpass;
				pocketverbs->oHR[pocketverbs->outHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outHR--;
				if (pocketverbs->outHR < 0 || pocketverbs->outHR > pocketverbs->delayH) {
					pocketverbs->outHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->oHR[pocketverbs->outHR]);
				// allpass filter H

				pocketverbs->dHR[6] = pocketverbs->dHR[5];
				pocketverbs->dHR[5] = pocketverbs->dHR[4];
				pocketverbs->dHR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dHR[5] + pocketverbs->dHR[6]) / 3.0;

				allpasstemp = pocketverbs->outIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->oIR[allpasstemp] * constallpass;
				pocketverbs->oIR[pocketverbs->outIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outIR--;
				if (pocketverbs->outIR < 0 || pocketverbs->outIR > pocketverbs->delayI) {
					pocketverbs->outIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->oIR[pocketverbs->outIR]);
				// allpass filter I

				pocketverbs->dIR[6] = pocketverbs->dIR[5];
				pocketverbs->dIR[5] = pocketverbs->dIR[4];
				pocketverbs->dIR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[1] + pocketverbs->dIR[5] + pocketverbs->dIR[6]) / 3.0;

				allpasstemp = pocketverbs->outJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->oJR[allpasstemp] * constallpass;
				pocketverbs->oJR[pocketverbs->outJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outJR--;
				if (pocketverbs->outJR < 0 || pocketverbs->outJR > pocketverbs->delayJ) {
					pocketverbs->outJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->oJR[pocketverbs->outJR]);
				// allpass filter J

				pocketverbs->dJR[6] = pocketverbs->dJR[5];
				pocketverbs->dJR[5] = pocketverbs->dJR[4];
				pocketverbs->dJR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dJR[5] + pocketverbs->dJR[6]) / 3.0;

				allpasstemp = pocketverbs->outKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->oKR[allpasstemp] * constallpass;
				pocketverbs->oKR[pocketverbs->outKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outKR--;
				if (pocketverbs->outKR < 0 || pocketverbs->outKR > pocketverbs->delayK) {
					pocketverbs->outKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->oKR[pocketverbs->outKR]);
				// allpass filter K

				pocketverbs->dKR[6] = pocketverbs->dKR[5];
				pocketverbs->dKR[5] = pocketverbs->dKR[4];
				pocketverbs->dKR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dKR[5] + pocketverbs->dKR[6]) / 3.0;

				allpasstemp = pocketverbs->outLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->oLR[allpasstemp] * constallpass;
				pocketverbs->oLR[pocketverbs->outLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outLR--;
				if (pocketverbs->outLR < 0 || pocketverbs->outLR > pocketverbs->delayL) {
					pocketverbs->outLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->oLR[pocketverbs->outLR]);
				// allpass filter L

				pocketverbs->dLR[6] = pocketverbs->dLR[5];
				pocketverbs->dLR[5] = pocketverbs->dLR[4];
				pocketverbs->dLR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dLR[5] + pocketverbs->dLR[6]) / 3.0;

				allpasstemp = pocketverbs->outMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->oMR[allpasstemp] * constallpass;
				pocketverbs->oMR[pocketverbs->outMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outMR--;
				if (pocketverbs->outMR < 0 || pocketverbs->outMR > pocketverbs->delayM) {
					pocketverbs->outMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->oMR[pocketverbs->outMR]);
				// allpass filter M

				pocketverbs->dMR[6] = pocketverbs->dMR[5];
				pocketverbs->dMR[5] = pocketverbs->dMR[4];
				pocketverbs->dMR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[1] + pocketverbs->dMR[5] + pocketverbs->dMR[6]) / 3.0;

				allpasstemp = pocketverbs->outNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->oNR[allpasstemp] * constallpass;
				pocketverbs->oNR[pocketverbs->outNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outNR--;
				if (pocketverbs->outNR < 0 || pocketverbs->outNR > pocketverbs->delayN) {
					pocketverbs->outNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->oNR[pocketverbs->outNR]);
				// allpass filter N

				pocketverbs->dNR[6] = pocketverbs->dNR[5];
				pocketverbs->dNR[5] = pocketverbs->dNR[4];
				pocketverbs->dNR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dNR[5] + pocketverbs->dNR[6]) / 3.0;

				allpasstemp = pocketverbs->outOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->oOR[allpasstemp] * constallpass;
				pocketverbs->oOR[pocketverbs->outOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outOR--;
				if (pocketverbs->outOR < 0 || pocketverbs->outOR > pocketverbs->delayO) {
					pocketverbs->outOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->oOR[pocketverbs->outOR]);
				// allpass filter O

				pocketverbs->dOR[6] = pocketverbs->dOR[5];
				pocketverbs->dOR[5] = pocketverbs->dOR[4];
				pocketverbs->dOR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[1] + pocketverbs->dOR[5] + pocketverbs->dOR[6]) / 3.0;

				allpasstemp = pocketverbs->outPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->oPR[allpasstemp] * constallpass;
				pocketverbs->oPR[pocketverbs->outPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outPR--;
				if (pocketverbs->outPR < 0 || pocketverbs->outPR > pocketverbs->delayP) {
					pocketverbs->outPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->oPR[pocketverbs->outPR]);
				// allpass filter P

				pocketverbs->dPR[6] = pocketverbs->dPR[5];
				pocketverbs->dPR[5] = pocketverbs->dPR[4];
				pocketverbs->dPR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dPR[5] + pocketverbs->dPR[6]) / 3.0;

				allpasstemp = pocketverbs->outQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->oQR[allpasstemp] * constallpass;
				pocketverbs->oQR[pocketverbs->outQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outQR--;
				if (pocketverbs->outQR < 0 || pocketverbs->outQR > pocketverbs->delayQ) {
					pocketverbs->outQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->oQR[pocketverbs->outQR]);
				// allpass filter Q

				pocketverbs->dQR[6] = pocketverbs->dQR[5];
				pocketverbs->dQR[5] = pocketverbs->dQR[4];
				pocketverbs->dQR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dQR[5] + pocketverbs->dQR[6]) / 3.0;

				allpasstemp = pocketverbs->outRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->oRR[allpasstemp] * constallpass;
				pocketverbs->oRR[pocketverbs->outRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outRR--;
				if (pocketverbs->outRR < 0 || pocketverbs->outRR > pocketverbs->delayR) {
					pocketverbs->outRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->oRR[pocketverbs->outRR]);
				// allpass filter R

				pocketverbs->dRR[6] = pocketverbs->dRR[5];
				pocketverbs->dRR[5] = pocketverbs->dRR[4];
				pocketverbs->dRR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dRR[5] + pocketverbs->dRR[6]) / 3.0;

				allpasstemp = pocketverbs->outSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->oSR[allpasstemp] * constallpass;
				pocketverbs->oSR[pocketverbs->outSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outSR--;
				if (pocketverbs->outSR < 0 || pocketverbs->outSR > pocketverbs->delayS) {
					pocketverbs->outSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->oSR[pocketverbs->outSR]);
				// allpass filter S

				pocketverbs->dSR[6] = pocketverbs->dSR[5];
				pocketverbs->dSR[5] = pocketverbs->dSR[4];
				pocketverbs->dSR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[1] + pocketverbs->dSR[5] + pocketverbs->dSR[6]) / 3.0;

				allpasstemp = pocketverbs->outTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->oTR[allpasstemp] * constallpass;
				pocketverbs->oTR[pocketverbs->outTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outTR--;
				if (pocketverbs->outTR < 0 || pocketverbs->outTR > pocketverbs->delayT) {
					pocketverbs->outTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->oTR[pocketverbs->outTR]);
				// allpass filter T

				pocketverbs->dTR[6] = pocketverbs->dTR[5];
				pocketverbs->dTR[5] = pocketverbs->dTR[4];
				pocketverbs->dTR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dTR[5] + pocketverbs->dTR[6]) / 3.0;

				allpasstemp = pocketverbs->outUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->oUR[allpasstemp] * constallpass;
				pocketverbs->oUR[pocketverbs->outUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outUR--;
				if (pocketverbs->outUR < 0 || pocketverbs->outUR > pocketverbs->delayU) {
					pocketverbs->outUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->oUR[pocketverbs->outUR]);
				// allpass filter U

				pocketverbs->dUR[6] = pocketverbs->dUR[5];
				pocketverbs->dUR[5] = pocketverbs->dUR[4];
				pocketverbs->dUR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dER[1] + pocketverbs->dUR[5] + pocketverbs->dUR[6]) / 3.0;

				allpasstemp = pocketverbs->outVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->oVR[allpasstemp] * constallpass;
				pocketverbs->oVR[pocketverbs->outVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outVR--;
				if (pocketverbs->outVR < 0 || pocketverbs->outVR > pocketverbs->delayV) {
					pocketverbs->outVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->oVR[pocketverbs->outVR]);
				// allpass filter V

				pocketverbs->dVR[6] = pocketverbs->dVR[5];
				pocketverbs->dVR[5] = pocketverbs->dVR[4];
				pocketverbs->dVR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dVR[5] + pocketverbs->dVR[6]) / 3.0;

				allpasstemp = pocketverbs->outWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->oWR[allpasstemp] * constallpass;
				pocketverbs->oWR[pocketverbs->outWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outWR--;
				if (pocketverbs->outWR < 0 || pocketverbs->outWR > pocketverbs->delayW) {
					pocketverbs->outWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->oWR[pocketverbs->outWR]);
				// allpass filter W

				pocketverbs->dWR[6] = pocketverbs->dWR[5];
				pocketverbs->dWR[5] = pocketverbs->dWR[4];
				pocketverbs->dWR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[1] + pocketverbs->dWR[5] + pocketverbs->dWR[6]) / 3.0;

				allpasstemp = pocketverbs->outXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->oXR[allpasstemp] * constallpass;
				pocketverbs->oXR[pocketverbs->outXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outXR--;
				if (pocketverbs->outXR < 0 || pocketverbs->outXR > pocketverbs->delayX) {
					pocketverbs->outXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->oXR[pocketverbs->outXR]);
				// allpass filter X

				pocketverbs->dXR[6] = pocketverbs->dXR[5];
				pocketverbs->dXR[5] = pocketverbs->dXR[4];
				pocketverbs->dXR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dXR[5] + pocketverbs->dXR[6]) / 3.0;

				allpasstemp = pocketverbs->outYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->oYR[allpasstemp] * constallpass;
				pocketverbs->oYR[pocketverbs->outYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outYR--;
				if (pocketverbs->outYR < 0 || pocketverbs->outYR > pocketverbs->delayY) {
					pocketverbs->outYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->oYR[pocketverbs->outYR]);
				// allpass filter Y

				pocketverbs->dYR[6] = pocketverbs->dYR[5];
				pocketverbs->dYR[5] = pocketverbs->dYR[4];
				pocketverbs->dYR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dYR[5] + pocketverbs->dYR[6]) / 3.0;

				allpasstemp = pocketverbs->outZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->oZR[allpasstemp] * constallpass;
				pocketverbs->oZR[pocketverbs->outZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outZR--;
				if (pocketverbs->outZR < 0 || pocketverbs->outZR > pocketverbs->delayZ) {
					pocketverbs->outZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->oZR[pocketverbs->outZR]);
				// allpass filter Z

				pocketverbs->dZR[6] = pocketverbs->dZR[5];
				pocketverbs->dZR[5] = pocketverbs->dZR[4];
				pocketverbs->dZR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[5] + pocketverbs->dZR[6]);
				// output Spring
				break;

			case 3: // Tiled
				allpasstemp = pocketverbs->alpAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->aAR[allpasstemp] * constallpass;
				pocketverbs->aAR[pocketverbs->alpAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpAR--;
				if (pocketverbs->alpAR < 0 || pocketverbs->alpAR > pocketverbs->delayA) {
					pocketverbs->alpAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->aAR[pocketverbs->alpAR]);
				// allpass filter A

				pocketverbs->dAR[2] = pocketverbs->dAR[1];
				pocketverbs->dAR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dAR[2]) / 2.0;

				allpasstemp = pocketverbs->alpBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->aBR[allpasstemp] * constallpass;
				pocketverbs->aBR[pocketverbs->alpBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpBR--;
				if (pocketverbs->alpBR < 0 || pocketverbs->alpBR > pocketverbs->delayB) {
					pocketverbs->alpBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->aBR[pocketverbs->alpBR]);
				// allpass filter B

				pocketverbs->dBR[2] = pocketverbs->dBR[1];
				pocketverbs->dBR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dBR[2]) / 2.0;

				allpasstemp = pocketverbs->alpCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->aCR[allpasstemp] * constallpass;
				pocketverbs->aCR[pocketverbs->alpCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpCR--;
				if (pocketverbs->alpCR < 0 || pocketverbs->alpCR > pocketverbs->delayC) {
					pocketverbs->alpCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->aCR[pocketverbs->alpCR]);
				// allpass filter C

				pocketverbs->dCR[2] = pocketverbs->dCR[1];
				pocketverbs->dCR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dCR[2]) / 2.0;

				allpasstemp = pocketverbs->alpDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->aDR[allpasstemp] * constallpass;
				pocketverbs->aDR[pocketverbs->alpDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpDR--;
				if (pocketverbs->alpDR < 0 || pocketverbs->alpDR > pocketverbs->delayD) {
					pocketverbs->alpDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->aDR[pocketverbs->alpDR]);
				// allpass filter D

				pocketverbs->dDR[2] = pocketverbs->dDR[1];
				pocketverbs->dDR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dDR[2]) / 2.0;

				allpasstemp = pocketverbs->alpER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->aER[allpasstemp] * constallpass;
				pocketverbs->aER[pocketverbs->alpER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpER--;
				if (pocketverbs->alpER < 0 || pocketverbs->alpER > pocketverbs->delayE) {
					pocketverbs->alpER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->aER[pocketverbs->alpER]);
				// allpass filter E

				pocketverbs->dER[2] = pocketverbs->dER[1];
				pocketverbs->dER[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dER[2]) / 2.0;

				allpasstemp = pocketverbs->alpFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->aFR[allpasstemp] * constallpass;
				pocketverbs->aFR[pocketverbs->alpFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpFR--;
				if (pocketverbs->alpFR < 0 || pocketverbs->alpFR > pocketverbs->delayF) {
					pocketverbs->alpFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->aFR[pocketverbs->alpFR]);
				// allpass filter F

				pocketverbs->dFR[2] = pocketverbs->dFR[1];
				pocketverbs->dFR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dFR[2]) / 2.0;

				allpasstemp = pocketverbs->alpGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->aGR[allpasstemp] * constallpass;
				pocketverbs->aGR[pocketverbs->alpGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpGR--;
				if (pocketverbs->alpGR < 0 || pocketverbs->alpGR > pocketverbs->delayG) {
					pocketverbs->alpGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->aGR[pocketverbs->alpGR]);
				// allpass filter G

				pocketverbs->dGR[2] = pocketverbs->dGR[1];
				pocketverbs->dGR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dGR[2]) / 2.0;

				allpasstemp = pocketverbs->alpHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->aHR[allpasstemp] * constallpass;
				pocketverbs->aHR[pocketverbs->alpHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpHR--;
				if (pocketverbs->alpHR < 0 || pocketverbs->alpHR > pocketverbs->delayH) {
					pocketverbs->alpHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->aHR[pocketverbs->alpHR]);
				// allpass filter H

				pocketverbs->dHR[2] = pocketverbs->dHR[1];
				pocketverbs->dHR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dHR[2]) / 2.0;

				allpasstemp = pocketverbs->alpIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->aIR[allpasstemp] * constallpass;
				pocketverbs->aIR[pocketverbs->alpIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpIR--;
				if (pocketverbs->alpIR < 0 || pocketverbs->alpIR > pocketverbs->delayI) {
					pocketverbs->alpIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->aIR[pocketverbs->alpIR]);
				// allpass filter I

				pocketverbs->dIR[2] = pocketverbs->dIR[1];
				pocketverbs->dIR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dIR[2]) / 2.0;

				allpasstemp = pocketverbs->alpJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->aJR[allpasstemp] * constallpass;
				pocketverbs->aJR[pocketverbs->alpJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpJR--;
				if (pocketverbs->alpJR < 0 || pocketverbs->alpJR > pocketverbs->delayJ) {
					pocketverbs->alpJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->aJR[pocketverbs->alpJR]);
				// allpass filter J

				pocketverbs->dJR[2] = pocketverbs->dJR[1];
				pocketverbs->dJR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dJR[2]) / 2.0;

				allpasstemp = pocketverbs->alpKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->aKR[allpasstemp] * constallpass;
				pocketverbs->aKR[pocketverbs->alpKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpKR--;
				if (pocketverbs->alpKR < 0 || pocketverbs->alpKR > pocketverbs->delayK) {
					pocketverbs->alpKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->aKR[pocketverbs->alpKR]);
				// allpass filter K

				pocketverbs->dKR[2] = pocketverbs->dKR[1];
				pocketverbs->dKR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dKR[2]) / 2.0;

				allpasstemp = pocketverbs->alpLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->aLR[allpasstemp] * constallpass;
				pocketverbs->aLR[pocketverbs->alpLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpLR--;
				if (pocketverbs->alpLR < 0 || pocketverbs->alpLR > pocketverbs->delayL) {
					pocketverbs->alpLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->aLR[pocketverbs->alpLR]);
				// allpass filter L

				pocketverbs->dLR[2] = pocketverbs->dLR[1];
				pocketverbs->dLR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dLR[2]) / 2.0;

				allpasstemp = pocketverbs->alpMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->aMR[allpasstemp] * constallpass;
				pocketverbs->aMR[pocketverbs->alpMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpMR--;
				if (pocketverbs->alpMR < 0 || pocketverbs->alpMR > pocketverbs->delayM) {
					pocketverbs->alpMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->aMR[pocketverbs->alpMR]);
				// allpass filter M

				pocketverbs->dMR[2] = pocketverbs->dMR[1];
				pocketverbs->dMR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dMR[2]) / 2.0;

				allpasstemp = pocketverbs->alpNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->aNR[allpasstemp] * constallpass;
				pocketverbs->aNR[pocketverbs->alpNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpNR--;
				if (pocketverbs->alpNR < 0 || pocketverbs->alpNR > pocketverbs->delayN) {
					pocketverbs->alpNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->aNR[pocketverbs->alpNR]);
				// allpass filter N

				pocketverbs->dNR[2] = pocketverbs->dNR[1];
				pocketverbs->dNR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dNR[2]) / 2.0;

				allpasstemp = pocketverbs->alpOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->aOR[allpasstemp] * constallpass;
				pocketverbs->aOR[pocketverbs->alpOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpOR--;
				if (pocketverbs->alpOR < 0 || pocketverbs->alpOR > pocketverbs->delayO) {
					pocketverbs->alpOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->aOR[pocketverbs->alpOR]);
				// allpass filter O

				pocketverbs->dOR[2] = pocketverbs->dOR[1];
				pocketverbs->dOR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dOR[2]) / 2.0;

				allpasstemp = pocketverbs->alpPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->aPR[allpasstemp] * constallpass;
				pocketverbs->aPR[pocketverbs->alpPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpPR--;
				if (pocketverbs->alpPR < 0 || pocketverbs->alpPR > pocketverbs->delayP) {
					pocketverbs->alpPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->aPR[pocketverbs->alpPR]);
				// allpass filter P

				pocketverbs->dPR[2] = pocketverbs->dPR[1];
				pocketverbs->dPR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dPR[2]) / 2.0;

				allpasstemp = pocketverbs->alpQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->aQR[allpasstemp] * constallpass;
				pocketverbs->aQR[pocketverbs->alpQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpQR--;
				if (pocketverbs->alpQR < 0 || pocketverbs->alpQR > pocketverbs->delayQ) {
					pocketverbs->alpQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->aQR[pocketverbs->alpQR]);
				// allpass filter Q

				pocketverbs->dQR[2] = pocketverbs->dQR[1];
				pocketverbs->dQR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dQR[2]) / 2.0;

				allpasstemp = pocketverbs->alpRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->aRR[allpasstemp] * constallpass;
				pocketverbs->aRR[pocketverbs->alpRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpRR--;
				if (pocketverbs->alpRR < 0 || pocketverbs->alpRR > pocketverbs->delayR) {
					pocketverbs->alpRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->aRR[pocketverbs->alpRR]);
				// allpass filter R

				pocketverbs->dRR[2] = pocketverbs->dRR[1];
				pocketverbs->dRR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dRR[2]) / 2.0;

				allpasstemp = pocketverbs->alpSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->aSR[allpasstemp] * constallpass;
				pocketverbs->aSR[pocketverbs->alpSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpSR--;
				if (pocketverbs->alpSR < 0 || pocketverbs->alpSR > pocketverbs->delayS) {
					pocketverbs->alpSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->aSR[pocketverbs->alpSR]);
				// allpass filter S

				pocketverbs->dSR[2] = pocketverbs->dSR[1];
				pocketverbs->dSR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dSR[2]) / 2.0;

				allpasstemp = pocketverbs->alpTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->aTR[allpasstemp] * constallpass;
				pocketverbs->aTR[pocketverbs->alpTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpTR--;
				if (pocketverbs->alpTR < 0 || pocketverbs->alpTR > pocketverbs->delayT) {
					pocketverbs->alpTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->aTR[pocketverbs->alpTR]);
				// allpass filter T

				pocketverbs->dTR[2] = pocketverbs->dTR[1];
				pocketverbs->dTR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dTR[2]) / 2.0;

				allpasstemp = pocketverbs->alpUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->aUR[allpasstemp] * constallpass;
				pocketverbs->aUR[pocketverbs->alpUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpUR--;
				if (pocketverbs->alpUR < 0 || pocketverbs->alpUR > pocketverbs->delayU) {
					pocketverbs->alpUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->aUR[pocketverbs->alpUR]);
				// allpass filter U

				pocketverbs->dUR[2] = pocketverbs->dUR[1];
				pocketverbs->dUR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dUR[2]) / 2.0;

				allpasstemp = pocketverbs->alpVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->aVR[allpasstemp] * constallpass;
				pocketverbs->aVR[pocketverbs->alpVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpVR--;
				if (pocketverbs->alpVR < 0 || pocketverbs->alpVR > pocketverbs->delayV) {
					pocketverbs->alpVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->aVR[pocketverbs->alpVR]);
				// allpass filter V

				pocketverbs->dVR[2] = pocketverbs->dVR[1];
				pocketverbs->dVR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dVR[2]) / 2.0;

				allpasstemp = pocketverbs->alpWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->aWR[allpasstemp] * constallpass;
				pocketverbs->aWR[pocketverbs->alpWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpWR--;
				if (pocketverbs->alpWR < 0 || pocketverbs->alpWR > pocketverbs->delayW) {
					pocketverbs->alpWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->aWR[pocketverbs->alpWR]);
				// allpass filter W

				pocketverbs->dWR[2] = pocketverbs->dWR[1];
				pocketverbs->dWR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dWR[2]) / 2.0;

				allpasstemp = pocketverbs->alpXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->aXR[allpasstemp] * constallpass;
				pocketverbs->aXR[pocketverbs->alpXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpXR--;
				if (pocketverbs->alpXR < 0 || pocketverbs->alpXR > pocketverbs->delayX) {
					pocketverbs->alpXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->aXR[pocketverbs->alpXR]);
				// allpass filter X

				pocketverbs->dXR[2] = pocketverbs->dXR[1];
				pocketverbs->dXR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dXR[2]) / 2.0;

				allpasstemp = pocketverbs->alpYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->aYR[allpasstemp] * constallpass;
				pocketverbs->aYR[pocketverbs->alpYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpYR--;
				if (pocketverbs->alpYR < 0 || pocketverbs->alpYR > pocketverbs->delayY) {
					pocketverbs->alpYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->aYR[pocketverbs->alpYR]);
				// allpass filter Y

				pocketverbs->dYR[2] = pocketverbs->dYR[1];
				pocketverbs->dYR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dYR[2]) / 2.0;

				allpasstemp = pocketverbs->alpZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->aZR[allpasstemp] * constallpass;
				pocketverbs->aZR[pocketverbs->alpZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpZR--;
				if (pocketverbs->alpZR < 0 || pocketverbs->alpZR > pocketverbs->delayZ) {
					pocketverbs->alpZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->aZR[pocketverbs->alpZR]);
				// allpass filter Z

				pocketverbs->dZR[2] = pocketverbs->dZR[1];
				pocketverbs->dZR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[1] + pocketverbs->dZR[2]) / 2.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->oAR[allpasstemp] * constallpass;
				pocketverbs->oAR[pocketverbs->outAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outAR--;
				if (pocketverbs->outAR < 0 || pocketverbs->outAR > pocketverbs->delayA) {
					pocketverbs->outAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->oAR[pocketverbs->outAR]);
				// allpass filter A

				pocketverbs->dAR[5] = pocketverbs->dAR[4];
				pocketverbs->dAR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[4] + pocketverbs->dAR[5]) / 2.0;

				allpasstemp = pocketverbs->outBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->oBR[allpasstemp] * constallpass;
				pocketverbs->oBR[pocketverbs->outBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outBR--;
				if (pocketverbs->outBR < 0 || pocketverbs->outBR > pocketverbs->delayB) {
					pocketverbs->outBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->oBR[pocketverbs->outBR]);
				// allpass filter B

				pocketverbs->dBR[5] = pocketverbs->dBR[4];
				pocketverbs->dBR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[4] + pocketverbs->dBR[5]) / 2.0;

				allpasstemp = pocketverbs->outCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->oCR[allpasstemp] * constallpass;
				pocketverbs->oCR[pocketverbs->outCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outCR--;
				if (pocketverbs->outCR < 0 || pocketverbs->outCR > pocketverbs->delayC) {
					pocketverbs->outCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->oCR[pocketverbs->outCR]);
				// allpass filter C

				pocketverbs->dCR[5] = pocketverbs->dCR[4];
				pocketverbs->dCR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dCR[5]) / 2.0;

				allpasstemp = pocketverbs->outDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->oDR[allpasstemp] * constallpass;
				pocketverbs->oDR[pocketverbs->outDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outDR--;
				if (pocketverbs->outDR < 0 || pocketverbs->outDR > pocketverbs->delayD) {
					pocketverbs->outDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->oDR[pocketverbs->outDR]);
				// allpass filter D

				pocketverbs->dDR[5] = pocketverbs->dDR[4];
				pocketverbs->dDR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[4] + pocketverbs->dDR[5]) / 2.0;

				allpasstemp = pocketverbs->outER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->oER[allpasstemp] * constallpass;
				pocketverbs->oER[pocketverbs->outER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outER--;
				if (pocketverbs->outER < 0 || pocketverbs->outER > pocketverbs->delayE) {
					pocketverbs->outER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->oER[pocketverbs->outER]);
				// allpass filter E

				pocketverbs->dER[5] = pocketverbs->dER[4];
				pocketverbs->dER[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dER[5]) / 2.0;

				allpasstemp = pocketverbs->outFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->oFR[allpasstemp] * constallpass;
				pocketverbs->oFR[pocketverbs->outFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outFR--;
				if (pocketverbs->outFR < 0 || pocketverbs->outFR > pocketverbs->delayF) {
					pocketverbs->outFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->oFR[pocketverbs->outFR]);
				// allpass filter F

				pocketverbs->dFR[5] = pocketverbs->dFR[4];
				pocketverbs->dFR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[4] + pocketverbs->dFR[5]) / 2.0;

				allpasstemp = pocketverbs->outGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->oGR[allpasstemp] * constallpass;
				pocketverbs->oGR[pocketverbs->outGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outGR--;
				if (pocketverbs->outGR < 0 || pocketverbs->outGR > pocketverbs->delayG) {
					pocketverbs->outGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->oGR[pocketverbs->outGR]);
				// allpass filter G

				pocketverbs->dGR[5] = pocketverbs->dGR[4];
				pocketverbs->dGR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[4] + pocketverbs->dGR[5]) / 2.0;

				allpasstemp = pocketverbs->outHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->oHR[allpasstemp] * constallpass;
				pocketverbs->oHR[pocketverbs->outHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outHR--;
				if (pocketverbs->outHR < 0 || pocketverbs->outHR > pocketverbs->delayH) {
					pocketverbs->outHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->oHR[pocketverbs->outHR]);
				// allpass filter H

				pocketverbs->dHR[5] = pocketverbs->dHR[4];
				pocketverbs->dHR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[4] + pocketverbs->dHR[5]) / 2.0;

				allpasstemp = pocketverbs->outIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->oIR[allpasstemp] * constallpass;
				pocketverbs->oIR[pocketverbs->outIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outIR--;
				if (pocketverbs->outIR < 0 || pocketverbs->outIR > pocketverbs->delayI) {
					pocketverbs->outIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->oIR[pocketverbs->outIR]);
				// allpass filter I

				pocketverbs->dIR[5] = pocketverbs->dIR[4];
				pocketverbs->dIR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[4] + pocketverbs->dIR[5]) / 2.0;

				allpasstemp = pocketverbs->outJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->oJR[allpasstemp] * constallpass;
				pocketverbs->oJR[pocketverbs->outJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outJR--;
				if (pocketverbs->outJR < 0 || pocketverbs->outJR > pocketverbs->delayJ) {
					pocketverbs->outJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->oJR[pocketverbs->outJR]);
				// allpass filter J

				pocketverbs->dJR[5] = pocketverbs->dJR[4];
				pocketverbs->dJR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[4] + pocketverbs->dJR[5]) / 2.0;

				allpasstemp = pocketverbs->outKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->oKR[allpasstemp] * constallpass;
				pocketverbs->oKR[pocketverbs->outKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outKR--;
				if (pocketverbs->outKR < 0 || pocketverbs->outKR > pocketverbs->delayK) {
					pocketverbs->outKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->oKR[pocketverbs->outKR]);
				// allpass filter K

				pocketverbs->dKR[5] = pocketverbs->dKR[4];
				pocketverbs->dKR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dKR[5]) / 2.0;

				allpasstemp = pocketverbs->outLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->oLR[allpasstemp] * constallpass;
				pocketverbs->oLR[pocketverbs->outLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outLR--;
				if (pocketverbs->outLR < 0 || pocketverbs->outLR > pocketverbs->delayL) {
					pocketverbs->outLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->oLR[pocketverbs->outLR]);
				// allpass filter L

				pocketverbs->dLR[5] = pocketverbs->dLR[4];
				pocketverbs->dLR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[4] + pocketverbs->dLR[5]) / 2.0;

				allpasstemp = pocketverbs->outMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->oMR[allpasstemp] * constallpass;
				pocketverbs->oMR[pocketverbs->outMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outMR--;
				if (pocketverbs->outMR < 0 || pocketverbs->outMR > pocketverbs->delayM) {
					pocketverbs->outMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->oMR[pocketverbs->outMR]);
				// allpass filter M

				pocketverbs->dMR[5] = pocketverbs->dMR[4];
				pocketverbs->dMR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[4] + pocketverbs->dMR[5]) / 2.0;

				allpasstemp = pocketverbs->outNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->oNR[allpasstemp] * constallpass;
				pocketverbs->oNR[pocketverbs->outNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outNR--;
				if (pocketverbs->outNR < 0 || pocketverbs->outNR > pocketverbs->delayN) {
					pocketverbs->outNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->oNR[pocketverbs->outNR]);
				// allpass filter N

				pocketverbs->dNR[5] = pocketverbs->dNR[4];
				pocketverbs->dNR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[4] + pocketverbs->dNR[5]) / 2.0;

				allpasstemp = pocketverbs->outOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->oOR[allpasstemp] * constallpass;
				pocketverbs->oOR[pocketverbs->outOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outOR--;
				if (pocketverbs->outOR < 0 || pocketverbs->outOR > pocketverbs->delayO) {
					pocketverbs->outOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->oOR[pocketverbs->outOR]);
				// allpass filter O

				pocketverbs->dOR[5] = pocketverbs->dOR[4];
				pocketverbs->dOR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dOR[5]) / 2.0;

				allpasstemp = pocketverbs->outPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->oPR[allpasstemp] * constallpass;
				pocketverbs->oPR[pocketverbs->outPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outPR--;
				if (pocketverbs->outPR < 0 || pocketverbs->outPR > pocketverbs->delayP) {
					pocketverbs->outPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->oPR[pocketverbs->outPR]);
				// allpass filter P

				pocketverbs->dPR[5] = pocketverbs->dPR[4];
				pocketverbs->dPR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[4] + pocketverbs->dPR[5]) / 2.0;

				allpasstemp = pocketverbs->outQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->oQR[allpasstemp] * constallpass;
				pocketverbs->oQR[pocketverbs->outQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outQR--;
				if (pocketverbs->outQR < 0 || pocketverbs->outQR > pocketverbs->delayQ) {
					pocketverbs->outQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->oQR[pocketverbs->outQR]);
				// allpass filter Q

				pocketverbs->dQR[5] = pocketverbs->dQR[4];
				pocketverbs->dQR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dQR[5]) / 2.0;

				allpasstemp = pocketverbs->outRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->oRR[allpasstemp] * constallpass;
				pocketverbs->oRR[pocketverbs->outRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outRR--;
				if (pocketverbs->outRR < 0 || pocketverbs->outRR > pocketverbs->delayR) {
					pocketverbs->outRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->oRR[pocketverbs->outRR]);
				// allpass filter R

				pocketverbs->dRR[5] = pocketverbs->dRR[4];
				pocketverbs->dRR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[4] + pocketverbs->dRR[5]) / 2.0;

				allpasstemp = pocketverbs->outSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->oSR[allpasstemp] * constallpass;
				pocketverbs->oSR[pocketverbs->outSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outSR--;
				if (pocketverbs->outSR < 0 || pocketverbs->outSR > pocketverbs->delayS) {
					pocketverbs->outSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->oSR[pocketverbs->outSR]);
				// allpass filter S

				pocketverbs->dSR[5] = pocketverbs->dSR[4];
				pocketverbs->dSR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[4] + pocketverbs->dSR[5]) / 2.0;

				allpasstemp = pocketverbs->outTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->oTR[allpasstemp] * constallpass;
				pocketverbs->oTR[pocketverbs->outTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outTR--;
				if (pocketverbs->outTR < 0 || pocketverbs->outTR > pocketverbs->delayT) {
					pocketverbs->outTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->oTR[pocketverbs->outTR]);
				// allpass filter T

				pocketverbs->dTR[5] = pocketverbs->dTR[4];
				pocketverbs->dTR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[4] + pocketverbs->dTR[5]) / 2.0;

				allpasstemp = pocketverbs->outUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->oUR[allpasstemp] * constallpass;
				pocketverbs->oUR[pocketverbs->outUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outUR--;
				if (pocketverbs->outUR < 0 || pocketverbs->outUR > pocketverbs->delayU) {
					pocketverbs->outUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->oUR[pocketverbs->outUR]);
				// allpass filter U

				pocketverbs->dUR[5] = pocketverbs->dUR[4];
				pocketverbs->dUR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dUR[5]) / 2.0;

				allpasstemp = pocketverbs->outVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->oVR[allpasstemp] * constallpass;
				pocketverbs->oVR[pocketverbs->outVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outVR--;
				if (pocketverbs->outVR < 0 || pocketverbs->outVR > pocketverbs->delayV) {
					pocketverbs->outVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->oVR[pocketverbs->outVR]);
				// allpass filter V

				pocketverbs->dVR[5] = pocketverbs->dVR[4];
				pocketverbs->dVR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[4] + pocketverbs->dVR[5]) / 2.0;

				allpasstemp = pocketverbs->outWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->oWR[allpasstemp] * constallpass;
				pocketverbs->oWR[pocketverbs->outWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outWR--;
				if (pocketverbs->outWR < 0 || pocketverbs->outWR > pocketverbs->delayW) {
					pocketverbs->outWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->oWR[pocketverbs->outWR]);
				// allpass filter W

				pocketverbs->dWR[5] = pocketverbs->dWR[4];
				pocketverbs->dWR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[4] + pocketverbs->dWR[5]) / 2.0;

				allpasstemp = pocketverbs->outXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->oXR[allpasstemp] * constallpass;
				pocketverbs->oXR[pocketverbs->outXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outXR--;
				if (pocketverbs->outXR < 0 || pocketverbs->outXR > pocketverbs->delayX) {
					pocketverbs->outXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->oXR[pocketverbs->outXR]);
				// allpass filter X

				pocketverbs->dXR[5] = pocketverbs->dXR[4];
				pocketverbs->dXR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[4] + pocketverbs->dXR[5]) / 2.0;

				allpasstemp = pocketverbs->outYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->oYR[allpasstemp] * constallpass;
				pocketverbs->oYR[pocketverbs->outYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outYR--;
				if (pocketverbs->outYR < 0 || pocketverbs->outYR > pocketverbs->delayY) {
					pocketverbs->outYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->oYR[pocketverbs->outYR]);
				// allpass filter Y

				pocketverbs->dYR[5] = pocketverbs->dYR[4];
				pocketverbs->dYR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[4] + pocketverbs->dYR[5]) / 2.0;

				allpasstemp = pocketverbs->outZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->oZR[allpasstemp] * constallpass;
				pocketverbs->oZR[pocketverbs->outZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outZR--;
				if (pocketverbs->outZR < 0 || pocketverbs->outZR > pocketverbs->delayZ) {
					pocketverbs->outZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->oZR[pocketverbs->outZR]);
				// allpass filter Z

				pocketverbs->dZR[5] = pocketverbs->dZR[4];
				pocketverbs->dZR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[4] + pocketverbs->dZR[5]);
				// output Tiled
				break;

			case 4: // Room
				allpasstemp = pocketverbs->alpAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->aAR[allpasstemp] * constallpass;
				pocketverbs->aAR[pocketverbs->alpAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpAR--;
				if (pocketverbs->alpAR < 0 || pocketverbs->alpAR > pocketverbs->delayA) {
					pocketverbs->alpAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->aAR[pocketverbs->alpAR]);
				// allpass filter A

				pocketverbs->dAR[2] = pocketverbs->dAR[1];
				pocketverbs->dAR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->aBR[allpasstemp] * constallpass;
				pocketverbs->aBR[pocketverbs->alpBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpBR--;
				if (pocketverbs->alpBR < 0 || pocketverbs->alpBR > pocketverbs->delayB) {
					pocketverbs->alpBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->aBR[pocketverbs->alpBR]);
				// allpass filter B

				pocketverbs->dBR[2] = pocketverbs->dBR[1];
				pocketverbs->dBR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->aCR[allpasstemp] * constallpass;
				pocketverbs->aCR[pocketverbs->alpCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpCR--;
				if (pocketverbs->alpCR < 0 || pocketverbs->alpCR > pocketverbs->delayC) {
					pocketverbs->alpCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->aCR[pocketverbs->alpCR]);
				// allpass filter C

				pocketverbs->dCR[2] = pocketverbs->dCR[1];
				pocketverbs->dCR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->aDR[allpasstemp] * constallpass;
				pocketverbs->aDR[pocketverbs->alpDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpDR--;
				if (pocketverbs->alpDR < 0 || pocketverbs->alpDR > pocketverbs->delayD) {
					pocketverbs->alpDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->aDR[pocketverbs->alpDR]);
				// allpass filter D

				pocketverbs->dDR[2] = pocketverbs->dDR[1];
				pocketverbs->dDR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->aER[allpasstemp] * constallpass;
				pocketverbs->aER[pocketverbs->alpER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpER--;
				if (pocketverbs->alpER < 0 || pocketverbs->alpER > pocketverbs->delayE) {
					pocketverbs->alpER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->aER[pocketverbs->alpER]);
				// allpass filter E

				pocketverbs->dER[2] = pocketverbs->dER[1];
				pocketverbs->dER[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->aFR[allpasstemp] * constallpass;
				pocketverbs->aFR[pocketverbs->alpFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpFR--;
				if (pocketverbs->alpFR < 0 || pocketverbs->alpFR > pocketverbs->delayF) {
					pocketverbs->alpFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->aFR[pocketverbs->alpFR]);
				// allpass filter F

				pocketverbs->dFR[2] = pocketverbs->dFR[1];
				pocketverbs->dFR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->aGR[allpasstemp] * constallpass;
				pocketverbs->aGR[pocketverbs->alpGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpGR--;
				if (pocketverbs->alpGR < 0 || pocketverbs->alpGR > pocketverbs->delayG) {
					pocketverbs->alpGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->aGR[pocketverbs->alpGR]);
				// allpass filter G

				pocketverbs->dGR[2] = pocketverbs->dGR[1];
				pocketverbs->dGR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->aHR[allpasstemp] * constallpass;
				pocketverbs->aHR[pocketverbs->alpHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpHR--;
				if (pocketverbs->alpHR < 0 || pocketverbs->alpHR > pocketverbs->delayH) {
					pocketverbs->alpHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->aHR[pocketverbs->alpHR]);
				// allpass filter H

				pocketverbs->dHR[2] = pocketverbs->dHR[1];
				pocketverbs->dHR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->aIR[allpasstemp] * constallpass;
				pocketverbs->aIR[pocketverbs->alpIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpIR--;
				if (pocketverbs->alpIR < 0 || pocketverbs->alpIR > pocketverbs->delayI) {
					pocketverbs->alpIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->aIR[pocketverbs->alpIR]);
				// allpass filter I

				pocketverbs->dIR[2] = pocketverbs->dIR[1];
				pocketverbs->dIR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->aJR[allpasstemp] * constallpass;
				pocketverbs->aJR[pocketverbs->alpJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpJR--;
				if (pocketverbs->alpJR < 0 || pocketverbs->alpJR > pocketverbs->delayJ) {
					pocketverbs->alpJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->aJR[pocketverbs->alpJR]);
				// allpass filter J

				pocketverbs->dJR[2] = pocketverbs->dJR[1];
				pocketverbs->dJR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->aKR[allpasstemp] * constallpass;
				pocketverbs->aKR[pocketverbs->alpKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpKR--;
				if (pocketverbs->alpKR < 0 || pocketverbs->alpKR > pocketverbs->delayK) {
					pocketverbs->alpKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->aKR[pocketverbs->alpKR]);
				// allpass filter K

				pocketverbs->dKR[2] = pocketverbs->dKR[1];
				pocketverbs->dKR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->aLR[allpasstemp] * constallpass;
				pocketverbs->aLR[pocketverbs->alpLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpLR--;
				if (pocketverbs->alpLR < 0 || pocketverbs->alpLR > pocketverbs->delayL) {
					pocketverbs->alpLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->aLR[pocketverbs->alpLR]);
				// allpass filter L

				pocketverbs->dLR[2] = pocketverbs->dLR[1];
				pocketverbs->dLR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->aMR[allpasstemp] * constallpass;
				pocketverbs->aMR[pocketverbs->alpMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpMR--;
				if (pocketverbs->alpMR < 0 || pocketverbs->alpMR > pocketverbs->delayM) {
					pocketverbs->alpMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->aMR[pocketverbs->alpMR]);
				// allpass filter M

				pocketverbs->dMR[2] = pocketverbs->dMR[1];
				pocketverbs->dMR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->aNR[allpasstemp] * constallpass;
				pocketverbs->aNR[pocketverbs->alpNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpNR--;
				if (pocketverbs->alpNR < 0 || pocketverbs->alpNR > pocketverbs->delayN) {
					pocketverbs->alpNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->aNR[pocketverbs->alpNR]);
				// allpass filter N

				pocketverbs->dNR[2] = pocketverbs->dNR[1];
				pocketverbs->dNR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->aOR[allpasstemp] * constallpass;
				pocketverbs->aOR[pocketverbs->alpOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpOR--;
				if (pocketverbs->alpOR < 0 || pocketverbs->alpOR > pocketverbs->delayO) {
					pocketverbs->alpOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->aOR[pocketverbs->alpOR]);
				// allpass filter O

				pocketverbs->dOR[2] = pocketverbs->dOR[1];
				pocketverbs->dOR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->aPR[allpasstemp] * constallpass;
				pocketverbs->aPR[pocketverbs->alpPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpPR--;
				if (pocketverbs->alpPR < 0 || pocketverbs->alpPR > pocketverbs->delayP) {
					pocketverbs->alpPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->aPR[pocketverbs->alpPR]);
				// allpass filter P

				pocketverbs->dPR[2] = pocketverbs->dPR[1];
				pocketverbs->dPR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->aQR[allpasstemp] * constallpass;
				pocketverbs->aQR[pocketverbs->alpQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpQR--;
				if (pocketverbs->alpQR < 0 || pocketverbs->alpQR > pocketverbs->delayQ) {
					pocketverbs->alpQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->aQR[pocketverbs->alpQR]);
				// allpass filter Q

				pocketverbs->dQR[2] = pocketverbs->dQR[1];
				pocketverbs->dQR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->aRR[allpasstemp] * constallpass;
				pocketverbs->aRR[pocketverbs->alpRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpRR--;
				if (pocketverbs->alpRR < 0 || pocketverbs->alpRR > pocketverbs->delayR) {
					pocketverbs->alpRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->aRR[pocketverbs->alpRR]);
				// allpass filter R

				pocketverbs->dRR[2] = pocketverbs->dRR[1];
				pocketverbs->dRR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->aSR[allpasstemp] * constallpass;
				pocketverbs->aSR[pocketverbs->alpSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpSR--;
				if (pocketverbs->alpSR < 0 || pocketverbs->alpSR > pocketverbs->delayS) {
					pocketverbs->alpSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->aSR[pocketverbs->alpSR]);
				// allpass filter S

				pocketverbs->dSR[2] = pocketverbs->dSR[1];
				pocketverbs->dSR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->aTR[allpasstemp] * constallpass;
				pocketverbs->aTR[pocketverbs->alpTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpTR--;
				if (pocketverbs->alpTR < 0 || pocketverbs->alpTR > pocketverbs->delayT) {
					pocketverbs->alpTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->aTR[pocketverbs->alpTR]);
				// allpass filter T

				pocketverbs->dTR[2] = pocketverbs->dTR[1];
				pocketverbs->dTR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->aUR[allpasstemp] * constallpass;
				pocketverbs->aUR[pocketverbs->alpUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpUR--;
				if (pocketverbs->alpUR < 0 || pocketverbs->alpUR > pocketverbs->delayU) {
					pocketverbs->alpUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->aUR[pocketverbs->alpUR]);
				// allpass filter U

				pocketverbs->dUR[2] = pocketverbs->dUR[1];
				pocketverbs->dUR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->aVR[allpasstemp] * constallpass;
				pocketverbs->aVR[pocketverbs->alpVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpVR--;
				if (pocketverbs->alpVR < 0 || pocketverbs->alpVR > pocketverbs->delayV) {
					pocketverbs->alpVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->aVR[pocketverbs->alpVR]);
				// allpass filter V

				pocketverbs->dVR[2] = pocketverbs->dVR[1];
				pocketverbs->dVR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->aWR[allpasstemp] * constallpass;
				pocketverbs->aWR[pocketverbs->alpWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpWR--;
				if (pocketverbs->alpWR < 0 || pocketverbs->alpWR > pocketverbs->delayW) {
					pocketverbs->alpWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->aWR[pocketverbs->alpWR]);
				// allpass filter W

				pocketverbs->dWR[2] = pocketverbs->dWR[1];
				pocketverbs->dWR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->aXR[allpasstemp] * constallpass;
				pocketverbs->aXR[pocketverbs->alpXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpXR--;
				if (pocketverbs->alpXR < 0 || pocketverbs->alpXR > pocketverbs->delayX) {
					pocketverbs->alpXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->aXR[pocketverbs->alpXR]);
				// allpass filter X

				pocketverbs->dXR[2] = pocketverbs->dXR[1];
				pocketverbs->dXR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->aYR[allpasstemp] * constallpass;
				pocketverbs->aYR[pocketverbs->alpYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpYR--;
				if (pocketverbs->alpYR < 0 || pocketverbs->alpYR > pocketverbs->delayY) {
					pocketverbs->alpYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->aYR[pocketverbs->alpYR]);
				// allpass filter Y

				pocketverbs->dYR[2] = pocketverbs->dYR[1];
				pocketverbs->dYR[1] = inputSampleR;
				inputSampleR = drySampleR;

				allpasstemp = pocketverbs->alpZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->aZR[allpasstemp] * constallpass;
				pocketverbs->aZR[pocketverbs->alpZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpZR--;
				if (pocketverbs->alpZR < 0 || pocketverbs->alpZR > pocketverbs->delayZ) {
					pocketverbs->alpZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->aZR[pocketverbs->alpZR]);
				// allpass filter Z

				pocketverbs->dZR[2] = pocketverbs->dZR[1];
				pocketverbs->dZR[1] = inputSampleR;
				inputSampleR = drySampleR;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->oAR[allpasstemp] * constallpass;
				pocketverbs->oAR[pocketverbs->outAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outAR--;
				if (pocketverbs->outAR < 0 || pocketverbs->outAR > pocketverbs->delayA) {
					pocketverbs->outAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->oAR[pocketverbs->outAR]);
				// allpass filter A

				pocketverbs->dAR[5] = pocketverbs->dAR[4];
				pocketverbs->dAR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dAR[2]) / 2.0;

				allpasstemp = pocketverbs->outBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->oBR[allpasstemp] * constallpass;
				pocketverbs->oBR[pocketverbs->outBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outBR--;
				if (pocketverbs->outBR < 0 || pocketverbs->outBR > pocketverbs->delayB) {
					pocketverbs->outBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->oBR[pocketverbs->outBR]);
				// allpass filter B

				pocketverbs->dBR[5] = pocketverbs->dBR[4];
				pocketverbs->dBR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dBR[2]) / 2.0;

				allpasstemp = pocketverbs->outCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->oCR[allpasstemp] * constallpass;
				pocketverbs->oCR[pocketverbs->outCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outCR--;
				if (pocketverbs->outCR < 0 || pocketverbs->outCR > pocketverbs->delayC) {
					pocketverbs->outCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->oCR[pocketverbs->outCR]);
				// allpass filter C

				pocketverbs->dCR[5] = pocketverbs->dCR[4];
				pocketverbs->dCR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[1] + pocketverbs->dCR[2]) / 2.0;

				allpasstemp = pocketverbs->outDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->oDR[allpasstemp] * constallpass;
				pocketverbs->oDR[pocketverbs->outDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outDR--;
				if (pocketverbs->outDR < 0 || pocketverbs->outDR > pocketverbs->delayD) {
					pocketverbs->outDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->oDR[pocketverbs->outDR]);
				// allpass filter D

				pocketverbs->dDR[5] = pocketverbs->dDR[4];
				pocketverbs->dDR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dDR[2]) / 2.0;

				allpasstemp = pocketverbs->outER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->oER[allpasstemp] * constallpass;
				pocketverbs->oER[pocketverbs->outER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outER--;
				if (pocketverbs->outER < 0 || pocketverbs->outER > pocketverbs->delayE) {
					pocketverbs->outER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->oER[pocketverbs->outER]);
				// allpass filter E

				pocketverbs->dER[5] = pocketverbs->dER[4];
				pocketverbs->dER[4] = inputSampleR;
				inputSampleR = (pocketverbs->dER[1] + pocketverbs->dER[2]) / 2.0;

				allpasstemp = pocketverbs->outFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->oFR[allpasstemp] * constallpass;
				pocketverbs->oFR[pocketverbs->outFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outFR--;
				if (pocketverbs->outFR < 0 || pocketverbs->outFR > pocketverbs->delayF) {
					pocketverbs->outFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->oFR[pocketverbs->outFR]);
				// allpass filter F

				pocketverbs->dFR[5] = pocketverbs->dFR[4];
				pocketverbs->dFR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dFR[2]) / 2.0;

				allpasstemp = pocketverbs->outGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->oGR[allpasstemp] * constallpass;
				pocketverbs->oGR[pocketverbs->outGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outGR--;
				if (pocketverbs->outGR < 0 || pocketverbs->outGR > pocketverbs->delayG) {
					pocketverbs->outGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->oGR[pocketverbs->outGR]);
				// allpass filter G

				pocketverbs->dGR[5] = pocketverbs->dGR[4];
				pocketverbs->dGR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[1] + pocketverbs->dGR[2]) / 2.0;

				allpasstemp = pocketverbs->outHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->oHR[allpasstemp] * constallpass;
				pocketverbs->oHR[pocketverbs->outHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outHR--;
				if (pocketverbs->outHR < 0 || pocketverbs->outHR > pocketverbs->delayH) {
					pocketverbs->outHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->oHR[pocketverbs->outHR]);
				// allpass filter H

				pocketverbs->dHR[5] = pocketverbs->dHR[4];
				pocketverbs->dHR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dHR[2]) / 2.0;

				allpasstemp = pocketverbs->outIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->oIR[allpasstemp] * constallpass;
				pocketverbs->oIR[pocketverbs->outIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outIR--;
				if (pocketverbs->outIR < 0 || pocketverbs->outIR > pocketverbs->delayI) {
					pocketverbs->outIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->oIR[pocketverbs->outIR]);
				// allpass filter I

				pocketverbs->dIR[5] = pocketverbs->dIR[4];
				pocketverbs->dIR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dIR[2]) / 2.0;

				allpasstemp = pocketverbs->outJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->oJR[allpasstemp] * constallpass;
				pocketverbs->oJR[pocketverbs->outJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outJR--;
				if (pocketverbs->outJR < 0 || pocketverbs->outJR > pocketverbs->delayJ) {
					pocketverbs->outJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->oJR[pocketverbs->outJR]);
				// allpass filter J

				pocketverbs->dJR[5] = pocketverbs->dJR[4];
				pocketverbs->dJR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dJR[2]) / 2.0;

				allpasstemp = pocketverbs->outKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->oKR[allpasstemp] * constallpass;
				pocketverbs->oKR[pocketverbs->outKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outKR--;
				if (pocketverbs->outKR < 0 || pocketverbs->outKR > pocketverbs->delayK) {
					pocketverbs->outKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->oKR[pocketverbs->outKR]);
				// allpass filter K

				pocketverbs->dKR[5] = pocketverbs->dKR[4];
				pocketverbs->dKR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[1] + pocketverbs->dKR[2]) / 2.0;

				allpasstemp = pocketverbs->outLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->oLR[allpasstemp] * constallpass;
				pocketverbs->oLR[pocketverbs->outLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outLR--;
				if (pocketverbs->outLR < 0 || pocketverbs->outLR > pocketverbs->delayL) {
					pocketverbs->outLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->oLR[pocketverbs->outLR]);
				// allpass filter L

				pocketverbs->dLR[5] = pocketverbs->dLR[4];
				pocketverbs->dLR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dLR[2]) / 2.0;

				allpasstemp = pocketverbs->outMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->oMR[allpasstemp] * constallpass;
				pocketverbs->oMR[pocketverbs->outMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outMR--;
				if (pocketverbs->outMR < 0 || pocketverbs->outMR > pocketverbs->delayM) {
					pocketverbs->outMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->oMR[pocketverbs->outMR]);
				// allpass filter M

				pocketverbs->dMR[5] = pocketverbs->dMR[4];
				pocketverbs->dMR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[1] + pocketverbs->dMR[2]) / 2.0;

				allpasstemp = pocketverbs->outNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->oNR[allpasstemp] * constallpass;
				pocketverbs->oNR[pocketverbs->outNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outNR--;
				if (pocketverbs->outNR < 0 || pocketverbs->outNR > pocketverbs->delayN) {
					pocketverbs->outNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->oNR[pocketverbs->outNR]);
				// allpass filter N

				pocketverbs->dNR[5] = pocketverbs->dNR[4];
				pocketverbs->dNR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dNR[2]) / 2.0;

				allpasstemp = pocketverbs->outOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->oOR[allpasstemp] * constallpass;
				pocketverbs->oOR[pocketverbs->outOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outOR--;
				if (pocketverbs->outOR < 0 || pocketverbs->outOR > pocketverbs->delayO) {
					pocketverbs->outOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->oOR[pocketverbs->outOR]);
				// allpass filter O

				pocketverbs->dOR[5] = pocketverbs->dOR[4];
				pocketverbs->dOR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dOR[2]) / 2.0;

				allpasstemp = pocketverbs->outPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->oPR[allpasstemp] * constallpass;
				pocketverbs->oPR[pocketverbs->outPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outPR--;
				if (pocketverbs->outPR < 0 || pocketverbs->outPR > pocketverbs->delayP) {
					pocketverbs->outPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->oPR[pocketverbs->outPR]);
				// allpass filter P

				pocketverbs->dPR[5] = pocketverbs->dPR[4];
				pocketverbs->dPR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dPR[2]) / 2.0;

				allpasstemp = pocketverbs->outQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->oQR[allpasstemp] * constallpass;
				pocketverbs->oQR[pocketverbs->outQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outQR--;
				if (pocketverbs->outQR < 0 || pocketverbs->outQR > pocketverbs->delayQ) {
					pocketverbs->outQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->oQR[pocketverbs->outQR]);
				// allpass filter Q

				pocketverbs->dQR[5] = pocketverbs->dQR[4];
				pocketverbs->dQR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[1] + pocketverbs->dQR[2]) / 2.0;

				allpasstemp = pocketverbs->outRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->oRR[allpasstemp] * constallpass;
				pocketverbs->oRR[pocketverbs->outRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outRR--;
				if (pocketverbs->outRR < 0 || pocketverbs->outRR > pocketverbs->delayR) {
					pocketverbs->outRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->oRR[pocketverbs->outRR]);
				// allpass filter R

				pocketverbs->dRR[5] = pocketverbs->dRR[4];
				pocketverbs->dRR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dRR[2]) / 2.0;

				allpasstemp = pocketverbs->outSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->oSR[allpasstemp] * constallpass;
				pocketverbs->oSR[pocketverbs->outSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outSR--;
				if (pocketverbs->outSR < 0 || pocketverbs->outSR > pocketverbs->delayS) {
					pocketverbs->outSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->oSR[pocketverbs->outSR]);
				// allpass filter S

				pocketverbs->dSR[5] = pocketverbs->dSR[4];
				pocketverbs->dSR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[1] + pocketverbs->dSR[2]) / 2.0;

				allpasstemp = pocketverbs->outTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->oTR[allpasstemp] * constallpass;
				pocketverbs->oTR[pocketverbs->outTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outTR--;
				if (pocketverbs->outTR < 0 || pocketverbs->outTR > pocketverbs->delayT) {
					pocketverbs->outTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->oTR[pocketverbs->outTR]);
				// allpass filter T

				pocketverbs->dTR[5] = pocketverbs->dTR[4];
				pocketverbs->dTR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dTR[2]) / 2.0;

				allpasstemp = pocketverbs->outUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->oUR[allpasstemp] * constallpass;
				pocketverbs->oUR[pocketverbs->outUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outUR--;
				if (pocketverbs->outUR < 0 || pocketverbs->outUR > pocketverbs->delayU) {
					pocketverbs->outUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->oUR[pocketverbs->outUR]);
				// allpass filter U

				pocketverbs->dUR[5] = pocketverbs->dUR[4];
				pocketverbs->dUR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dUR[2]) / 2.0;

				allpasstemp = pocketverbs->outVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->oVR[allpasstemp] * constallpass;
				pocketverbs->oVR[pocketverbs->outVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outVR--;
				if (pocketverbs->outVR < 0 || pocketverbs->outVR > pocketverbs->delayV) {
					pocketverbs->outVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->oVR[pocketverbs->outVR]);
				// allpass filter V

				pocketverbs->dVR[5] = pocketverbs->dVR[4];
				pocketverbs->dVR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dVR[2]) / 2.0;

				allpasstemp = pocketverbs->outWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->oWR[allpasstemp] * constallpass;
				pocketverbs->oWR[pocketverbs->outWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outWR--;
				if (pocketverbs->outWR < 0 || pocketverbs->outWR > pocketverbs->delayW) {
					pocketverbs->outWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->oWR[pocketverbs->outWR]);
				// allpass filter W

				pocketverbs->dWR[5] = pocketverbs->dWR[4];
				pocketverbs->dWR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[1] + pocketverbs->dWR[2]) / 2.0;

				allpasstemp = pocketverbs->outXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->oXR[allpasstemp] * constallpass;
				pocketverbs->oXR[pocketverbs->outXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outXR--;
				if (pocketverbs->outXR < 0 || pocketverbs->outXR > pocketverbs->delayX) {
					pocketverbs->outXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->oXR[pocketverbs->outXR]);
				// allpass filter X

				pocketverbs->dXR[5] = pocketverbs->dXR[4];
				pocketverbs->dXR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dXR[2]) / 2.0;

				allpasstemp = pocketverbs->outYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->oYR[allpasstemp] * constallpass;
				pocketverbs->oYR[pocketverbs->outYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outYR--;
				if (pocketverbs->outYR < 0 || pocketverbs->outYR > pocketverbs->delayY) {
					pocketverbs->outYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->oYR[pocketverbs->outYR]);
				// allpass filter Y

				pocketverbs->dYR[5] = pocketverbs->dYR[4];
				pocketverbs->dYR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dYR[2]) / 2.0;

				allpasstemp = pocketverbs->outZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->oZR[allpasstemp] * constallpass;
				pocketverbs->oZR[pocketverbs->outZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outZR--;
				if (pocketverbs->outZR < 0 || pocketverbs->outZR > pocketverbs->delayZ) {
					pocketverbs->outZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->oZR[pocketverbs->outZR]);
				// allpass filter Z

				pocketverbs->dZR[5] = pocketverbs->dZR[4];
				pocketverbs->dZR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[4] * dryness);
				inputSampleR += (pocketverbs->dCR[4] * dryness);
				inputSampleR += pocketverbs->dDR[4];
				inputSampleR += pocketverbs->dER[4];
				inputSampleR += pocketverbs->dFR[4];
				inputSampleR += pocketverbs->dGR[4];
				inputSampleR += pocketverbs->dHR[4];
				inputSampleR += pocketverbs->dIR[4];
				inputSampleR += pocketverbs->dJR[4];
				inputSampleR += pocketverbs->dKR[4];
				inputSampleR += pocketverbs->dLR[4];
				inputSampleR += pocketverbs->dMR[4];
				inputSampleR += pocketverbs->dNR[4];
				inputSampleR += pocketverbs->dOR[4];
				inputSampleR += pocketverbs->dPR[4];
				inputSampleR += pocketverbs->dQR[4];
				inputSampleR += pocketverbs->dRR[4];
				inputSampleR += pocketverbs->dSR[4];
				inputSampleR += pocketverbs->dTR[4];
				inputSampleR += pocketverbs->dUR[4];
				inputSampleR += pocketverbs->dVR[4];
				inputSampleR += pocketverbs->dWR[4];
				inputSampleR += pocketverbs->dXR[4];
				inputSampleR += pocketverbs->dYR[4];
				inputSampleR += (pocketverbs->dZR[4] * wetness);

				inputSampleR += (pocketverbs->dBR[5] * dryness);
				inputSampleR += (pocketverbs->dCR[5] * dryness);
				inputSampleR += pocketverbs->dDR[5];
				inputSampleR += pocketverbs->dER[5];
				inputSampleR += pocketverbs->dFR[5];
				inputSampleR += pocketverbs->dGR[5];
				inputSampleR += pocketverbs->dHR[5];
				inputSampleR += pocketverbs->dIR[5];
				inputSampleR += pocketverbs->dJR[5];
				inputSampleR += pocketverbs->dKR[5];
				inputSampleR += pocketverbs->dLR[5];
				inputSampleR += pocketverbs->dMR[5];
				inputSampleR += pocketverbs->dNR[5];
				inputSampleR += pocketverbs->dOR[5];
				inputSampleR += pocketverbs->dPR[5];
				inputSampleR += pocketverbs->dQR[5];
				inputSampleR += pocketverbs->dRR[5];
				inputSampleR += pocketverbs->dSR[5];
				inputSampleR += pocketverbs->dTR[5];
				inputSampleR += pocketverbs->dUR[5];
				inputSampleR += pocketverbs->dVR[5];
				inputSampleR += pocketverbs->dWR[5];
				inputSampleR += pocketverbs->dXR[5];
				inputSampleR += pocketverbs->dYR[5];
				inputSampleR += (pocketverbs->dZR[5] * wetness);

				inputSampleR /= (26.0 + (wetness * 4.0));
				// output Room effect
				break;

			case 5: // Stretch
				allpasstemp = pocketverbs->alpAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->aAR[allpasstemp] * constallpass;
				pocketverbs->aAR[pocketverbs->alpAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpAR--;
				if (pocketverbs->alpAR < 0 || pocketverbs->alpAR > pocketverbs->delayA) {
					pocketverbs->alpAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->aAR[pocketverbs->alpAR]);
				// allpass filter A

				pocketverbs->dAR[2] = pocketverbs->dAR[1];
				pocketverbs->dAR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dAR[2]) / 2.0;

				allpasstemp = pocketverbs->alpBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->aBR[allpasstemp] * constallpass;
				pocketverbs->aBR[pocketverbs->alpBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpBR--;
				if (pocketverbs->alpBR < 0 || pocketverbs->alpBR > pocketverbs->delayB) {
					pocketverbs->alpBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->aBR[pocketverbs->alpBR]);
				// allpass filter B

				pocketverbs->dBR[2] = pocketverbs->dBR[1];
				pocketverbs->dBR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dBR[2]) / 2.0;

				allpasstemp = pocketverbs->alpCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->aCR[allpasstemp] * constallpass;
				pocketverbs->aCR[pocketverbs->alpCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpCR--;
				if (pocketverbs->alpCR < 0 || pocketverbs->alpCR > pocketverbs->delayC) {
					pocketverbs->alpCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->aCR[pocketverbs->alpCR]);
				// allpass filter C

				pocketverbs->dCR[2] = pocketverbs->dCR[1];
				pocketverbs->dCR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[1] + pocketverbs->dCR[2]) / 2.0;

				allpasstemp = pocketverbs->alpDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->aDR[allpasstemp] * constallpass;
				pocketverbs->aDR[pocketverbs->alpDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpDR--;
				if (pocketverbs->alpDR < 0 || pocketverbs->alpDR > pocketverbs->delayD) {
					pocketverbs->alpDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->aDR[pocketverbs->alpDR]);
				// allpass filter D

				pocketverbs->dDR[2] = pocketverbs->dDR[1];
				pocketverbs->dDR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dDR[2]) / 2.0;

				allpasstemp = pocketverbs->alpER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->aER[allpasstemp] * constallpass;
				pocketverbs->aER[pocketverbs->alpER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpER--;
				if (pocketverbs->alpER < 0 || pocketverbs->alpER > pocketverbs->delayE) {
					pocketverbs->alpER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->aER[pocketverbs->alpER]);
				// allpass filter E

				pocketverbs->dER[2] = pocketverbs->dER[1];
				pocketverbs->dER[1] = inputSampleR;
				inputSampleR = (pocketverbs->dER[1] + pocketverbs->dER[2]) / 2.0;

				allpasstemp = pocketverbs->alpFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->aFR[allpasstemp] * constallpass;
				pocketverbs->aFR[pocketverbs->alpFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpFR--;
				if (pocketverbs->alpFR < 0 || pocketverbs->alpFR > pocketverbs->delayF) {
					pocketverbs->alpFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->aFR[pocketverbs->alpFR]);
				// allpass filter F

				pocketverbs->dFR[2] = pocketverbs->dFR[1];
				pocketverbs->dFR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dFR[2]) / 2.0;

				allpasstemp = pocketverbs->alpGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->aGR[allpasstemp] * constallpass;
				pocketverbs->aGR[pocketverbs->alpGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpGR--;
				if (pocketverbs->alpGR < 0 || pocketverbs->alpGR > pocketverbs->delayG) {
					pocketverbs->alpGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->aGR[pocketverbs->alpGR]);
				// allpass filter G

				pocketverbs->dGR[2] = pocketverbs->dGR[1];
				pocketverbs->dGR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[1] + pocketverbs->dGR[2]) / 2.0;

				allpasstemp = pocketverbs->alpHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->aHR[allpasstemp] * constallpass;
				pocketverbs->aHR[pocketverbs->alpHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpHR--;
				if (pocketverbs->alpHR < 0 || pocketverbs->alpHR > pocketverbs->delayH) {
					pocketverbs->alpHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->aHR[pocketverbs->alpHR]);
				// allpass filter H

				pocketverbs->dHR[2] = pocketverbs->dHR[1];
				pocketverbs->dHR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dHR[2]) / 2.0;

				allpasstemp = pocketverbs->alpIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->aIR[allpasstemp] * constallpass;
				pocketverbs->aIR[pocketverbs->alpIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpIR--;
				if (pocketverbs->alpIR < 0 || pocketverbs->alpIR > pocketverbs->delayI) {
					pocketverbs->alpIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->aIR[pocketverbs->alpIR]);
				// allpass filter I

				pocketverbs->dIR[2] = pocketverbs->dIR[1];
				pocketverbs->dIR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dIR[2]) / 2.0;

				allpasstemp = pocketverbs->alpJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->aJR[allpasstemp] * constallpass;
				pocketverbs->aJR[pocketverbs->alpJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpJR--;
				if (pocketverbs->alpJR < 0 || pocketverbs->alpJR > pocketverbs->delayJ) {
					pocketverbs->alpJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->aJR[pocketverbs->alpJR]);
				// allpass filter J

				pocketverbs->dJR[2] = pocketverbs->dJR[1];
				pocketverbs->dJR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dJR[2]) / 2.0;

				allpasstemp = pocketverbs->alpKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->aKR[allpasstemp] * constallpass;
				pocketverbs->aKR[pocketverbs->alpKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpKR--;
				if (pocketverbs->alpKR < 0 || pocketverbs->alpKR > pocketverbs->delayK) {
					pocketverbs->alpKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->aKR[pocketverbs->alpKR]);
				// allpass filter K

				pocketverbs->dKR[2] = pocketverbs->dKR[1];
				pocketverbs->dKR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[1] + pocketverbs->dKR[2]) / 2.0;

				allpasstemp = pocketverbs->alpLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->aLR[allpasstemp] * constallpass;
				pocketverbs->aLR[pocketverbs->alpLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpLR--;
				if (pocketverbs->alpLR < 0 || pocketverbs->alpLR > pocketverbs->delayL) {
					pocketverbs->alpLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->aLR[pocketverbs->alpLR]);
				// allpass filter L

				pocketverbs->dLR[2] = pocketverbs->dLR[1];
				pocketverbs->dLR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dLR[2]) / 2.0;

				allpasstemp = pocketverbs->alpMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->aMR[allpasstemp] * constallpass;
				pocketverbs->aMR[pocketverbs->alpMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpMR--;
				if (pocketverbs->alpMR < 0 || pocketverbs->alpMR > pocketverbs->delayM) {
					pocketverbs->alpMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->aMR[pocketverbs->alpMR]);
				// allpass filter M

				pocketverbs->dMR[2] = pocketverbs->dMR[1];
				pocketverbs->dMR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[1] + pocketverbs->dMR[2]) / 2.0;

				allpasstemp = pocketverbs->alpNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->aNR[allpasstemp] * constallpass;
				pocketverbs->aNR[pocketverbs->alpNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpNR--;
				if (pocketverbs->alpNR < 0 || pocketverbs->alpNR > pocketverbs->delayN) {
					pocketverbs->alpNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->aNR[pocketverbs->alpNR]);
				// allpass filter N

				pocketverbs->dNR[2] = pocketverbs->dNR[1];
				pocketverbs->dNR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dNR[2]) / 2.0;

				allpasstemp = pocketverbs->alpOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->aOR[allpasstemp] * constallpass;
				pocketverbs->aOR[pocketverbs->alpOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpOR--;
				if (pocketverbs->alpOR < 0 || pocketverbs->alpOR > pocketverbs->delayO) {
					pocketverbs->alpOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->aOR[pocketverbs->alpOR]);
				// allpass filter O

				pocketverbs->dOR[2] = pocketverbs->dOR[1];
				pocketverbs->dOR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dOR[2]) / 2.0;

				allpasstemp = pocketverbs->alpPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->aPR[allpasstemp] * constallpass;
				pocketverbs->aPR[pocketverbs->alpPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpPR--;
				if (pocketverbs->alpPR < 0 || pocketverbs->alpPR > pocketverbs->delayP) {
					pocketverbs->alpPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->aPR[pocketverbs->alpPR]);
				// allpass filter P

				pocketverbs->dPR[2] = pocketverbs->dPR[1];
				pocketverbs->dPR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dPR[2]) / 2.0;

				allpasstemp = pocketverbs->alpQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->aQR[allpasstemp] * constallpass;
				pocketverbs->aQR[pocketverbs->alpQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpQR--;
				if (pocketverbs->alpQR < 0 || pocketverbs->alpQR > pocketverbs->delayQ) {
					pocketverbs->alpQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->aQR[pocketverbs->alpQR]);
				// allpass filter Q

				pocketverbs->dQR[2] = pocketverbs->dQR[1];
				pocketverbs->dQR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[1] + pocketverbs->dQR[2]) / 2.0;

				allpasstemp = pocketverbs->alpRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->aRR[allpasstemp] * constallpass;
				pocketverbs->aRR[pocketverbs->alpRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpRR--;
				if (pocketverbs->alpRR < 0 || pocketverbs->alpRR > pocketverbs->delayR) {
					pocketverbs->alpRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->aRR[pocketverbs->alpRR]);
				// allpass filter R

				pocketverbs->dRR[2] = pocketverbs->dRR[1];
				pocketverbs->dRR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dRR[2]) / 2.0;

				allpasstemp = pocketverbs->alpSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->aSR[allpasstemp] * constallpass;
				pocketverbs->aSR[pocketverbs->alpSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpSR--;
				if (pocketverbs->alpSR < 0 || pocketverbs->alpSR > pocketverbs->delayS) {
					pocketverbs->alpSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->aSR[pocketverbs->alpSR]);
				// allpass filter S

				pocketverbs->dSR[2] = pocketverbs->dSR[1];
				pocketverbs->dSR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[1] + pocketverbs->dSR[2]) / 2.0;

				allpasstemp = pocketverbs->alpTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->aTR[allpasstemp] * constallpass;
				pocketverbs->aTR[pocketverbs->alpTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpTR--;
				if (pocketverbs->alpTR < 0 || pocketverbs->alpTR > pocketverbs->delayT) {
					pocketverbs->alpTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->aTR[pocketverbs->alpTR]);
				// allpass filter T

				pocketverbs->dTR[2] = pocketverbs->dTR[1];
				pocketverbs->dTR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dTR[2]) / 2.0;

				allpasstemp = pocketverbs->alpUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->aUR[allpasstemp] * constallpass;
				pocketverbs->aUR[pocketverbs->alpUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpUR--;
				if (pocketverbs->alpUR < 0 || pocketverbs->alpUR > pocketverbs->delayU) {
					pocketverbs->alpUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->aUR[pocketverbs->alpUR]);
				// allpass filter U

				pocketverbs->dUR[2] = pocketverbs->dUR[1];
				pocketverbs->dUR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dUR[2]) / 2.0;

				allpasstemp = pocketverbs->alpVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->aVR[allpasstemp] * constallpass;
				pocketverbs->aVR[pocketverbs->alpVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpVR--;
				if (pocketverbs->alpVR < 0 || pocketverbs->alpVR > pocketverbs->delayV) {
					pocketverbs->alpVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->aVR[pocketverbs->alpVR]);
				// allpass filter V

				pocketverbs->dVR[2] = pocketverbs->dVR[1];
				pocketverbs->dVR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dVR[2]) / 2.0;

				allpasstemp = pocketverbs->alpWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->aWR[allpasstemp] * constallpass;
				pocketverbs->aWR[pocketverbs->alpWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpWR--;
				if (pocketverbs->alpWR < 0 || pocketverbs->alpWR > pocketverbs->delayW) {
					pocketverbs->alpWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->aWR[pocketverbs->alpWR]);
				// allpass filter W

				pocketverbs->dWR[2] = pocketverbs->dWR[1];
				pocketverbs->dWR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[1] + pocketverbs->dWR[2]) / 2.0;

				allpasstemp = pocketverbs->alpXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->aXR[allpasstemp] * constallpass;
				pocketverbs->aXR[pocketverbs->alpXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpXR--;
				if (pocketverbs->alpXR < 0 || pocketverbs->alpXR > pocketverbs->delayX) {
					pocketverbs->alpXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->aXR[pocketverbs->alpXR]);
				// allpass filter X

				pocketverbs->dXR[2] = pocketverbs->dXR[1];
				pocketverbs->dXR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dXR[2]) / 2.0;

				allpasstemp = pocketverbs->alpYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->aYR[allpasstemp] * constallpass;
				pocketverbs->aYR[pocketverbs->alpYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpYR--;
				if (pocketverbs->alpYR < 0 || pocketverbs->alpYR > pocketverbs->delayY) {
					pocketverbs->alpYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->aYR[pocketverbs->alpYR]);
				// allpass filter Y

				pocketverbs->dYR[2] = pocketverbs->dYR[1];
				pocketverbs->dYR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dYR[2]) / 2.0;

				allpasstemp = pocketverbs->alpZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->aZR[allpasstemp] * constallpass;
				pocketverbs->aZR[pocketverbs->alpZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpZR--;
				if (pocketverbs->alpZR < 0 || pocketverbs->alpZR > pocketverbs->delayZ) {
					pocketverbs->alpZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->aZR[pocketverbs->alpZR]);
				// allpass filter Z

				pocketverbs->dZR[2] = pocketverbs->dZR[1];
				pocketverbs->dZR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[1] + pocketverbs->dZR[2]) / 2.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->oAR[allpasstemp] * constallpass;
				pocketverbs->oAR[pocketverbs->outAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outAR--;
				if (pocketverbs->outAR < 0 || pocketverbs->outAR > pocketverbs->delayA) {
					pocketverbs->outAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->oAR[pocketverbs->outAR]);
				// allpass filter A

				pocketverbs->dAR[5] = pocketverbs->dAR[4];
				pocketverbs->dAR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[4] + pocketverbs->dAR[5]) / 2.0;

				allpasstemp = pocketverbs->outBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->oBR[allpasstemp] * constallpass;
				pocketverbs->oBR[pocketverbs->outBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outBR--;
				if (pocketverbs->outBR < 0 || pocketverbs->outBR > pocketverbs->delayB) {
					pocketverbs->outBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->oBR[pocketverbs->outBR]);
				// allpass filter B

				pocketverbs->dBR[5] = pocketverbs->dBR[4];
				pocketverbs->dBR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[4] + pocketverbs->dBR[5]) / 2.0;

				allpasstemp = pocketverbs->outCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->oCR[allpasstemp] * constallpass;
				pocketverbs->oCR[pocketverbs->outCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outCR--;
				if (pocketverbs->outCR < 0 || pocketverbs->outCR > pocketverbs->delayC) {
					pocketverbs->outCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->oCR[pocketverbs->outCR]);
				// allpass filter C

				pocketverbs->dCR[5] = pocketverbs->dCR[4];
				pocketverbs->dCR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[4] + pocketverbs->dCR[5]) / 2.0;

				allpasstemp = pocketverbs->outDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->oDR[allpasstemp] * constallpass;
				pocketverbs->oDR[pocketverbs->outDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outDR--;
				if (pocketverbs->outDR < 0 || pocketverbs->outDR > pocketverbs->delayD) {
					pocketverbs->outDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->oDR[pocketverbs->outDR]);
				// allpass filter D

				pocketverbs->dDR[5] = pocketverbs->dDR[4];
				pocketverbs->dDR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[4] + pocketverbs->dDR[5]) / 2.0;

				allpasstemp = pocketverbs->outER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->oER[allpasstemp] * constallpass;
				pocketverbs->oER[pocketverbs->outER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outER--;
				if (pocketverbs->outER < 0 || pocketverbs->outER > pocketverbs->delayE) {
					pocketverbs->outER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->oER[pocketverbs->outER]);
				// allpass filter E

				pocketverbs->dER[5] = pocketverbs->dER[4];
				pocketverbs->dER[4] = inputSampleR;
				inputSampleR = (pocketverbs->dER[4] + pocketverbs->dER[5]) / 2.0;

				allpasstemp = pocketverbs->outFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->oFR[allpasstemp] * constallpass;
				pocketverbs->oFR[pocketverbs->outFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outFR--;
				if (pocketverbs->outFR < 0 || pocketverbs->outFR > pocketverbs->delayF) {
					pocketverbs->outFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->oFR[pocketverbs->outFR]);
				// allpass filter F

				pocketverbs->dFR[5] = pocketverbs->dFR[4];
				pocketverbs->dFR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[4] + pocketverbs->dFR[5]) / 2.0;

				allpasstemp = pocketverbs->outGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->oGR[allpasstemp] * constallpass;
				pocketverbs->oGR[pocketverbs->outGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outGR--;
				if (pocketverbs->outGR < 0 || pocketverbs->outGR > pocketverbs->delayG) {
					pocketverbs->outGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->oGR[pocketverbs->outGR]);
				// allpass filter G

				pocketverbs->dGR[5] = pocketverbs->dGR[4];
				pocketverbs->dGR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[4] + pocketverbs->dGR[5]) / 2.0;

				allpasstemp = pocketverbs->outHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->oHR[allpasstemp] * constallpass;
				pocketverbs->oHR[pocketverbs->outHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outHR--;
				if (pocketverbs->outHR < 0 || pocketverbs->outHR > pocketverbs->delayH) {
					pocketverbs->outHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->oHR[pocketverbs->outHR]);
				// allpass filter H

				pocketverbs->dHR[5] = pocketverbs->dHR[4];
				pocketverbs->dHR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[4] + pocketverbs->dHR[5]) / 2.0;

				allpasstemp = pocketverbs->outIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->oIR[allpasstemp] * constallpass;
				pocketverbs->oIR[pocketverbs->outIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outIR--;
				if (pocketverbs->outIR < 0 || pocketverbs->outIR > pocketverbs->delayI) {
					pocketverbs->outIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->oIR[pocketverbs->outIR]);
				// allpass filter I

				pocketverbs->dIR[5] = pocketverbs->dIR[4];
				pocketverbs->dIR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[4] + pocketverbs->dIR[5]) / 2.0;

				allpasstemp = pocketverbs->outJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->oJR[allpasstemp] * constallpass;
				pocketverbs->oJR[pocketverbs->outJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outJR--;
				if (pocketverbs->outJR < 0 || pocketverbs->outJR > pocketverbs->delayJ) {
					pocketverbs->outJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->oJR[pocketverbs->outJR]);
				// allpass filter J

				pocketverbs->dJR[5] = pocketverbs->dJR[4];
				pocketverbs->dJR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[4] + pocketverbs->dJR[5]) / 2.0;

				allpasstemp = pocketverbs->outKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->oKR[allpasstemp] * constallpass;
				pocketverbs->oKR[pocketverbs->outKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outKR--;
				if (pocketverbs->outKR < 0 || pocketverbs->outKR > pocketverbs->delayK) {
					pocketverbs->outKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->oKR[pocketverbs->outKR]);
				// allpass filter K

				pocketverbs->dKR[5] = pocketverbs->dKR[4];
				pocketverbs->dKR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[4] + pocketverbs->dKR[5]) / 2.0;

				allpasstemp = pocketverbs->outLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->oLR[allpasstemp] * constallpass;
				pocketverbs->oLR[pocketverbs->outLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outLR--;
				if (pocketverbs->outLR < 0 || pocketverbs->outLR > pocketverbs->delayL) {
					pocketverbs->outLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->oLR[pocketverbs->outLR]);
				// allpass filter L

				pocketverbs->dLR[5] = pocketverbs->dLR[4];
				pocketverbs->dLR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[4] + pocketverbs->dLR[5]) / 2.0;

				allpasstemp = pocketverbs->outMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->oMR[allpasstemp] * constallpass;
				pocketverbs->oMR[pocketverbs->outMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outMR--;
				if (pocketverbs->outMR < 0 || pocketverbs->outMR > pocketverbs->delayM) {
					pocketverbs->outMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->oMR[pocketverbs->outMR]);
				// allpass filter M

				pocketverbs->dMR[5] = pocketverbs->dMR[4];
				pocketverbs->dMR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[4] + pocketverbs->dMR[5]) / 2.0;

				allpasstemp = pocketverbs->outNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->oNR[allpasstemp] * constallpass;
				pocketverbs->oNR[pocketverbs->outNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outNR--;
				if (pocketverbs->outNR < 0 || pocketverbs->outNR > pocketverbs->delayN) {
					pocketverbs->outNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->oNR[pocketverbs->outNR]);
				// allpass filter N

				pocketverbs->dNR[5] = pocketverbs->dNR[4];
				pocketverbs->dNR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[4] + pocketverbs->dNR[5]) / 2.0;

				allpasstemp = pocketverbs->outOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->oOR[allpasstemp] * constallpass;
				pocketverbs->oOR[pocketverbs->outOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outOR--;
				if (pocketverbs->outOR < 0 || pocketverbs->outOR > pocketverbs->delayO) {
					pocketverbs->outOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->oOR[pocketverbs->outOR]);
				// allpass filter O

				pocketverbs->dOR[5] = pocketverbs->dOR[4];
				pocketverbs->dOR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[4] + pocketverbs->dOR[5]) / 2.0;

				allpasstemp = pocketverbs->outPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->oPR[allpasstemp] * constallpass;
				pocketverbs->oPR[pocketverbs->outPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outPR--;
				if (pocketverbs->outPR < 0 || pocketverbs->outPR > pocketverbs->delayP) {
					pocketverbs->outPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->oPR[pocketverbs->outPR]);
				// allpass filter P

				pocketverbs->dPR[5] = pocketverbs->dPR[4];
				pocketverbs->dPR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[4] + pocketverbs->dPR[5]) / 2.0;

				allpasstemp = pocketverbs->outQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->oQR[allpasstemp] * constallpass;
				pocketverbs->oQR[pocketverbs->outQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outQR--;
				if (pocketverbs->outQR < 0 || pocketverbs->outQR > pocketverbs->delayQ) {
					pocketverbs->outQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->oQR[pocketverbs->outQR]);
				// allpass filter Q

				pocketverbs->dQR[5] = pocketverbs->dQR[4];
				pocketverbs->dQR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[4] + pocketverbs->dQR[5]) / 2.0;

				allpasstemp = pocketverbs->outRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->oRR[allpasstemp] * constallpass;
				pocketverbs->oRR[pocketverbs->outRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outRR--;
				if (pocketverbs->outRR < 0 || pocketverbs->outRR > pocketverbs->delayR) {
					pocketverbs->outRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->oRR[pocketverbs->outRR]);
				// allpass filter R

				pocketverbs->dRR[5] = pocketverbs->dRR[4];
				pocketverbs->dRR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[4] + pocketverbs->dRR[5]) / 2.0;

				allpasstemp = pocketverbs->outSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->oSR[allpasstemp] * constallpass;
				pocketverbs->oSR[pocketverbs->outSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outSR--;
				if (pocketverbs->outSR < 0 || pocketverbs->outSR > pocketverbs->delayS) {
					pocketverbs->outSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->oSR[pocketverbs->outSR]);
				// allpass filter S

				pocketverbs->dSR[5] = pocketverbs->dSR[4];
				pocketverbs->dSR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[4] + pocketverbs->dSR[5]) / 2.0;

				allpasstemp = pocketverbs->outTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->oTR[allpasstemp] * constallpass;
				pocketverbs->oTR[pocketverbs->outTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outTR--;
				if (pocketverbs->outTR < 0 || pocketverbs->outTR > pocketverbs->delayT) {
					pocketverbs->outTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->oTR[pocketverbs->outTR]);
				// allpass filter T

				pocketverbs->dTR[5] = pocketverbs->dTR[4];
				pocketverbs->dTR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[4] + pocketverbs->dTR[5]) / 2.0;

				allpasstemp = pocketverbs->outUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->oUR[allpasstemp] * constallpass;
				pocketverbs->oUR[pocketverbs->outUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outUR--;
				if (pocketverbs->outUR < 0 || pocketverbs->outUR > pocketverbs->delayU) {
					pocketverbs->outUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->oUR[pocketverbs->outUR]);
				// allpass filter U

				pocketverbs->dUR[5] = pocketverbs->dUR[4];
				pocketverbs->dUR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[4] + pocketverbs->dUR[5]) / 2.0;

				allpasstemp = pocketverbs->outVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->oVR[allpasstemp] * constallpass;
				pocketverbs->oVR[pocketverbs->outVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outVR--;
				if (pocketverbs->outVR < 0 || pocketverbs->outVR > pocketverbs->delayV) {
					pocketverbs->outVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->oVR[pocketverbs->outVR]);
				// allpass filter V

				pocketverbs->dVR[5] = pocketverbs->dVR[4];
				pocketverbs->dVR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[4] + pocketverbs->dVR[5]) / 2.0;

				allpasstemp = pocketverbs->outWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->oWR[allpasstemp] * constallpass;
				pocketverbs->oWR[pocketverbs->outWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outWR--;
				if (pocketverbs->outWR < 0 || pocketverbs->outWR > pocketverbs->delayW) {
					pocketverbs->outWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->oWR[pocketverbs->outWR]);
				// allpass filter W

				pocketverbs->dWR[5] = pocketverbs->dWR[4];
				pocketverbs->dWR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[4] + pocketverbs->dWR[5]) / 2.0;

				allpasstemp = pocketverbs->outXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->oXR[allpasstemp] * constallpass;
				pocketverbs->oXR[pocketverbs->outXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outXR--;
				if (pocketverbs->outXR < 0 || pocketverbs->outXR > pocketverbs->delayX) {
					pocketverbs->outXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->oXR[pocketverbs->outXR]);
				// allpass filter X

				pocketverbs->dXR[5] = pocketverbs->dXR[4];
				pocketverbs->dXR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[4] + pocketverbs->dXR[5]) / 2.0;

				allpasstemp = pocketverbs->outYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->oYR[allpasstemp] * constallpass;
				pocketverbs->oYR[pocketverbs->outYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outYR--;
				if (pocketverbs->outYR < 0 || pocketverbs->outYR > pocketverbs->delayY) {
					pocketverbs->outYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->oYR[pocketverbs->outYR]);
				// allpass filter Y

				pocketverbs->dYR[5] = pocketverbs->dYR[4];
				pocketverbs->dYR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[4] + pocketverbs->dYR[5]) / 2.0;

				allpasstemp = pocketverbs->outZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->oZR[allpasstemp] * constallpass;
				pocketverbs->oZR[pocketverbs->outZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outZR--;
				if (pocketverbs->outZR < 0 || pocketverbs->outZR > pocketverbs->delayZ) {
					pocketverbs->outZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->oZR[pocketverbs->outZR]);
				// allpass filter Z

				pocketverbs->dZR[5] = pocketverbs->dZR[4];
				pocketverbs->dZR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[4] + pocketverbs->dZR[5]) / 2.0;
				// output Stretch unrealistic but smooth fake Paulstretch
				break;

			case 6: // Zarathustra
				allpasstemp = pocketverbs->alpAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->aAR[allpasstemp] * constallpass;
				pocketverbs->aAR[pocketverbs->alpAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpAR--;
				if (pocketverbs->alpAR < 0 || pocketverbs->alpAR > pocketverbs->delayA) {
					pocketverbs->alpAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->aAR[pocketverbs->alpAR]);
				// allpass filter A

				pocketverbs->dAR[3] = pocketverbs->dAR[2];
				pocketverbs->dAR[2] = pocketverbs->dAR[1];
				pocketverbs->dAR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dAR[1] + pocketverbs->dAR[2] + pocketverbs->dZR[3]) / 3.0; // add feedback

				allpasstemp = pocketverbs->alpBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->aBR[allpasstemp] * constallpass;
				pocketverbs->aBR[pocketverbs->alpBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpBR--;
				if (pocketverbs->alpBR < 0 || pocketverbs->alpBR > pocketverbs->delayB) {
					pocketverbs->alpBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->aBR[pocketverbs->alpBR]);
				// allpass filter B

				pocketverbs->dBR[3] = pocketverbs->dBR[2];
				pocketverbs->dBR[2] = pocketverbs->dBR[1];
				pocketverbs->dBR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[1] + pocketverbs->dBR[2] + pocketverbs->dBR[3]) / 3.0;

				allpasstemp = pocketverbs->alpCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->aCR[allpasstemp] * constallpass;
				pocketverbs->aCR[pocketverbs->alpCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpCR--;
				if (pocketverbs->alpCR < 0 || pocketverbs->alpCR > pocketverbs->delayC) {
					pocketverbs->alpCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->aCR[pocketverbs->alpCR]);
				// allpass filter C

				pocketverbs->dCR[3] = pocketverbs->dCR[2];
				pocketverbs->dCR[2] = pocketverbs->dCR[1];
				pocketverbs->dCR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[1] + pocketverbs->dCR[2] + pocketverbs->dCR[3]) / 3.0;

				allpasstemp = pocketverbs->alpDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->aDR[allpasstemp] * constallpass;
				pocketverbs->aDR[pocketverbs->alpDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpDR--;
				if (pocketverbs->alpDR < 0 || pocketverbs->alpDR > pocketverbs->delayD) {
					pocketverbs->alpDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->aDR[pocketverbs->alpDR]);
				// allpass filter D

				pocketverbs->dDR[3] = pocketverbs->dDR[2];
				pocketverbs->dDR[2] = pocketverbs->dDR[1];
				pocketverbs->dDR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[1] + pocketverbs->dDR[2] + pocketverbs->dDR[3]) / 3.0;

				allpasstemp = pocketverbs->alpER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->aER[allpasstemp] * constallpass;
				pocketverbs->aER[pocketverbs->alpER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpER--;
				if (pocketverbs->alpER < 0 || pocketverbs->alpER > pocketverbs->delayE) {
					pocketverbs->alpER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->aER[pocketverbs->alpER]);
				// allpass filter E

				pocketverbs->dER[3] = pocketverbs->dER[2];
				pocketverbs->dER[2] = pocketverbs->dER[1];
				pocketverbs->dER[1] = inputSampleR;
				inputSampleR = (pocketverbs->dER[1] + pocketverbs->dER[2] + pocketverbs->dER[3]) / 3.0;

				allpasstemp = pocketverbs->alpFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->aFR[allpasstemp] * constallpass;
				pocketverbs->aFR[pocketverbs->alpFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpFR--;
				if (pocketverbs->alpFR < 0 || pocketverbs->alpFR > pocketverbs->delayF) {
					pocketverbs->alpFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->aFR[pocketverbs->alpFR]);
				// allpass filter F

				pocketverbs->dFR[3] = pocketverbs->dFR[2];
				pocketverbs->dFR[2] = pocketverbs->dFR[1];
				pocketverbs->dFR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[1] + pocketverbs->dFR[2] + pocketverbs->dFR[3]) / 3.0;

				allpasstemp = pocketverbs->alpGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->aGR[allpasstemp] * constallpass;
				pocketverbs->aGR[pocketverbs->alpGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpGR--;
				if (pocketverbs->alpGR < 0 || pocketverbs->alpGR > pocketverbs->delayG) {
					pocketverbs->alpGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->aGR[pocketverbs->alpGR]);
				// allpass filter G

				pocketverbs->dGR[3] = pocketverbs->dGR[2];
				pocketverbs->dGR[2] = pocketverbs->dGR[1];
				pocketverbs->dGR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[1] + pocketverbs->dGR[2] + pocketverbs->dGR[3]) / 3.0;

				allpasstemp = pocketverbs->alpHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->aHR[allpasstemp] * constallpass;
				pocketverbs->aHR[pocketverbs->alpHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpHR--;
				if (pocketverbs->alpHR < 0 || pocketverbs->alpHR > pocketverbs->delayH) {
					pocketverbs->alpHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->aHR[pocketverbs->alpHR]);
				// allpass filter H

				pocketverbs->dHR[3] = pocketverbs->dHR[2];
				pocketverbs->dHR[2] = pocketverbs->dHR[1];
				pocketverbs->dHR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[1] + pocketverbs->dHR[2] + pocketverbs->dHR[3]) / 3.0;

				allpasstemp = pocketverbs->alpIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->aIR[allpasstemp] * constallpass;
				pocketverbs->aIR[pocketverbs->alpIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpIR--;
				if (pocketverbs->alpIR < 0 || pocketverbs->alpIR > pocketverbs->delayI) {
					pocketverbs->alpIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->aIR[pocketverbs->alpIR]);
				// allpass filter I

				pocketverbs->dIR[3] = pocketverbs->dIR[2];
				pocketverbs->dIR[2] = pocketverbs->dIR[1];
				pocketverbs->dIR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[1] + pocketverbs->dIR[2] + pocketverbs->dIR[3]) / 3.0;

				allpasstemp = pocketverbs->alpJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->aJR[allpasstemp] * constallpass;
				pocketverbs->aJR[pocketverbs->alpJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpJR--;
				if (pocketverbs->alpJR < 0 || pocketverbs->alpJR > pocketverbs->delayJ) {
					pocketverbs->alpJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->aJR[pocketverbs->alpJR]);
				// allpass filter J

				pocketverbs->dJR[3] = pocketverbs->dJR[2];
				pocketverbs->dJR[2] = pocketverbs->dJR[1];
				pocketverbs->dJR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[1] + pocketverbs->dJR[2] + pocketverbs->dJR[3]) / 3.0;

				allpasstemp = pocketverbs->alpKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->aKR[allpasstemp] * constallpass;
				pocketverbs->aKR[pocketverbs->alpKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpKR--;
				if (pocketverbs->alpKR < 0 || pocketverbs->alpKR > pocketverbs->delayK) {
					pocketverbs->alpKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->aKR[pocketverbs->alpKR]);
				// allpass filter K

				pocketverbs->dKR[3] = pocketverbs->dKR[2];
				pocketverbs->dKR[2] = pocketverbs->dKR[1];
				pocketverbs->dKR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[1] + pocketverbs->dKR[2] + pocketverbs->dKR[3]) / 3.0;

				allpasstemp = pocketverbs->alpLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->aLR[allpasstemp] * constallpass;
				pocketverbs->aLR[pocketverbs->alpLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpLR--;
				if (pocketverbs->alpLR < 0 || pocketverbs->alpLR > pocketverbs->delayL) {
					pocketverbs->alpLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->aLR[pocketverbs->alpLR]);
				// allpass filter L

				pocketverbs->dLR[3] = pocketverbs->dLR[2];
				pocketverbs->dLR[2] = pocketverbs->dLR[1];
				pocketverbs->dLR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[1] + pocketverbs->dLR[2] + pocketverbs->dLR[3]) / 3.0;

				allpasstemp = pocketverbs->alpMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->aMR[allpasstemp] * constallpass;
				pocketverbs->aMR[pocketverbs->alpMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpMR--;
				if (pocketverbs->alpMR < 0 || pocketverbs->alpMR > pocketverbs->delayM) {
					pocketverbs->alpMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->aMR[pocketverbs->alpMR]);
				// allpass filter M

				pocketverbs->dMR[3] = pocketverbs->dMR[2];
				pocketverbs->dMR[2] = pocketverbs->dMR[1];
				pocketverbs->dMR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[1] + pocketverbs->dMR[2] + pocketverbs->dMR[3]) / 3.0;

				allpasstemp = pocketverbs->alpNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->aNR[allpasstemp] * constallpass;
				pocketverbs->aNR[pocketverbs->alpNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpNR--;
				if (pocketverbs->alpNR < 0 || pocketverbs->alpNR > pocketverbs->delayN) {
					pocketverbs->alpNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->aNR[pocketverbs->alpNR]);
				// allpass filter N

				pocketverbs->dNR[3] = pocketverbs->dNR[2];
				pocketverbs->dNR[2] = pocketverbs->dNR[1];
				pocketverbs->dNR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[1] + pocketverbs->dNR[2] + pocketverbs->dNR[3]) / 3.0;

				allpasstemp = pocketverbs->alpOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->aOR[allpasstemp] * constallpass;
				pocketverbs->aOR[pocketverbs->alpOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpOR--;
				if (pocketverbs->alpOR < 0 || pocketverbs->alpOR > pocketverbs->delayO) {
					pocketverbs->alpOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->aOR[pocketverbs->alpOR]);
				// allpass filter O

				pocketverbs->dOR[3] = pocketverbs->dOR[2];
				pocketverbs->dOR[2] = pocketverbs->dOR[1];
				pocketverbs->dOR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[1] + pocketverbs->dOR[2] + pocketverbs->dOR[3]) / 3.0;

				allpasstemp = pocketverbs->alpPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->aPR[allpasstemp] * constallpass;
				pocketverbs->aPR[pocketverbs->alpPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpPR--;
				if (pocketverbs->alpPR < 0 || pocketverbs->alpPR > pocketverbs->delayP) {
					pocketverbs->alpPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->aPR[pocketverbs->alpPR]);
				// allpass filter P

				pocketverbs->dPR[3] = pocketverbs->dPR[2];
				pocketverbs->dPR[2] = pocketverbs->dPR[1];
				pocketverbs->dPR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[1] + pocketverbs->dPR[2] + pocketverbs->dPR[3]) / 3.0;

				allpasstemp = pocketverbs->alpQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->aQR[allpasstemp] * constallpass;
				pocketverbs->aQR[pocketverbs->alpQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpQR--;
				if (pocketverbs->alpQR < 0 || pocketverbs->alpQR > pocketverbs->delayQ) {
					pocketverbs->alpQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->aQR[pocketverbs->alpQR]);
				// allpass filter Q

				pocketverbs->dQR[3] = pocketverbs->dQR[2];
				pocketverbs->dQR[2] = pocketverbs->dQR[1];
				pocketverbs->dQR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[1] + pocketverbs->dQR[2] + pocketverbs->dQR[3]) / 3.0;

				allpasstemp = pocketverbs->alpRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->aRR[allpasstemp] * constallpass;
				pocketverbs->aRR[pocketverbs->alpRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpRR--;
				if (pocketverbs->alpRR < 0 || pocketverbs->alpRR > pocketverbs->delayR) {
					pocketverbs->alpRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->aRR[pocketverbs->alpRR]);
				// allpass filter R

				pocketverbs->dRR[3] = pocketverbs->dRR[2];
				pocketverbs->dRR[2] = pocketverbs->dRR[1];
				pocketverbs->dRR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[1] + pocketverbs->dRR[2] + pocketverbs->dRR[3]) / 3.0;

				allpasstemp = pocketverbs->alpSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->aSR[allpasstemp] * constallpass;
				pocketverbs->aSR[pocketverbs->alpSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpSR--;
				if (pocketverbs->alpSR < 0 || pocketverbs->alpSR > pocketverbs->delayS) {
					pocketverbs->alpSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->aSR[pocketverbs->alpSR]);
				// allpass filter S

				pocketverbs->dSR[3] = pocketverbs->dSR[2];
				pocketverbs->dSR[2] = pocketverbs->dSR[1];
				pocketverbs->dSR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[1] + pocketverbs->dSR[2] + pocketverbs->dSR[3]) / 3.0;

				allpasstemp = pocketverbs->alpTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->aTR[allpasstemp] * constallpass;
				pocketverbs->aTR[pocketverbs->alpTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpTR--;
				if (pocketverbs->alpTR < 0 || pocketverbs->alpTR > pocketverbs->delayT) {
					pocketverbs->alpTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->aTR[pocketverbs->alpTR]);
				// allpass filter T

				pocketverbs->dTR[3] = pocketverbs->dTR[2];
				pocketverbs->dTR[2] = pocketverbs->dTR[1];
				pocketverbs->dTR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[1] + pocketverbs->dTR[2] + pocketverbs->dTR[3]) / 3.0;

				allpasstemp = pocketverbs->alpUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->aUR[allpasstemp] * constallpass;
				pocketverbs->aUR[pocketverbs->alpUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpUR--;
				if (pocketverbs->alpUR < 0 || pocketverbs->alpUR > pocketverbs->delayU) {
					pocketverbs->alpUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->aUR[pocketverbs->alpUR]);
				// allpass filter U

				pocketverbs->dUR[3] = pocketverbs->dUR[2];
				pocketverbs->dUR[2] = pocketverbs->dUR[1];
				pocketverbs->dUR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[1] + pocketverbs->dUR[2] + pocketverbs->dUR[3]) / 3.0;

				allpasstemp = pocketverbs->alpVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->aVR[allpasstemp] * constallpass;
				pocketverbs->aVR[pocketverbs->alpVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpVR--;
				if (pocketverbs->alpVR < 0 || pocketverbs->alpVR > pocketverbs->delayV) {
					pocketverbs->alpVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->aVR[pocketverbs->alpVR]);
				// allpass filter V

				pocketverbs->dVR[3] = pocketverbs->dVR[2];
				pocketverbs->dVR[2] = pocketverbs->dVR[1];
				pocketverbs->dVR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[1] + pocketverbs->dVR[2] + pocketverbs->dVR[3]) / 3.0;

				allpasstemp = pocketverbs->alpWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->aWR[allpasstemp] * constallpass;
				pocketverbs->aWR[pocketverbs->alpWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpWR--;
				if (pocketverbs->alpWR < 0 || pocketverbs->alpWR > pocketverbs->delayW) {
					pocketverbs->alpWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->aWR[pocketverbs->alpWR]);
				// allpass filter W

				pocketverbs->dWR[3] = pocketverbs->dWR[2];
				pocketverbs->dWR[2] = pocketverbs->dWR[1];
				pocketverbs->dWR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[1] + pocketverbs->dWR[2] + pocketverbs->dWR[3]) / 3.0;

				allpasstemp = pocketverbs->alpXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->aXR[allpasstemp] * constallpass;
				pocketverbs->aXR[pocketverbs->alpXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpXR--;
				if (pocketverbs->alpXR < 0 || pocketverbs->alpXR > pocketverbs->delayX) {
					pocketverbs->alpXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->aXR[pocketverbs->alpXR]);
				// allpass filter X

				pocketverbs->dXR[3] = pocketverbs->dXR[2];
				pocketverbs->dXR[2] = pocketverbs->dXR[1];
				pocketverbs->dXR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[1] + pocketverbs->dXR[2] + pocketverbs->dXR[3]) / 3.0;

				allpasstemp = pocketverbs->alpYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->aYR[allpasstemp] * constallpass;
				pocketverbs->aYR[pocketverbs->alpYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpYR--;
				if (pocketverbs->alpYR < 0 || pocketverbs->alpYR > pocketverbs->delayY) {
					pocketverbs->alpYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->aYR[pocketverbs->alpYR]);
				// allpass filter Y

				pocketverbs->dYR[3] = pocketverbs->dYR[2];
				pocketverbs->dYR[2] = pocketverbs->dYR[1];
				pocketverbs->dYR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[1] + pocketverbs->dYR[2] + pocketverbs->dYR[3]) / 3.0;

				allpasstemp = pocketverbs->alpZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->aZR[allpasstemp] * constallpass;
				pocketverbs->aZR[pocketverbs->alpZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->alpZR--;
				if (pocketverbs->alpZR < 0 || pocketverbs->alpZR > pocketverbs->delayZ) {
					pocketverbs->alpZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->aZR[pocketverbs->alpZR]);
				// allpass filter Z

				pocketverbs->dZR[3] = pocketverbs->dZR[2];
				pocketverbs->dZR[2] = pocketverbs->dZR[1];
				pocketverbs->dZR[1] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[1] + pocketverbs->dZR[2] + pocketverbs->dZR[3]) / 3.0;

				// now the second stage using the 'out' bank of allpasses

				allpasstemp = pocketverbs->outAR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayA) {
					allpasstemp = pocketverbs->delayA;
				}
				inputSampleR -= pocketverbs->oAR[allpasstemp] * constallpass;
				pocketverbs->oAR[pocketverbs->outAR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outAR--;
				if (pocketverbs->outAR < 0 || pocketverbs->outAR > pocketverbs->delayA) {
					pocketverbs->outAR = pocketverbs->delayA;
				}
				inputSampleR += (pocketverbs->oAR[pocketverbs->outAR]);
				// allpass filter A

				pocketverbs->dAR[6] = pocketverbs->dAR[5];
				pocketverbs->dAR[5] = pocketverbs->dAR[4];
				pocketverbs->dAR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[1] + pocketverbs->dAR[5] + pocketverbs->dAR[6]) / 3.0; // note, feeding in dry again for a little more clarity!

				allpasstemp = pocketverbs->outBR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayB) {
					allpasstemp = pocketverbs->delayB;
				}
				inputSampleR -= pocketverbs->oBR[allpasstemp] * constallpass;
				pocketverbs->oBR[pocketverbs->outBR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outBR--;
				if (pocketverbs->outBR < 0 || pocketverbs->outBR > pocketverbs->delayB) {
					pocketverbs->outBR = pocketverbs->delayB;
				}
				inputSampleR += (pocketverbs->oBR[pocketverbs->outBR]);
				// allpass filter B

				pocketverbs->dBR[6] = pocketverbs->dBR[5];
				pocketverbs->dBR[5] = pocketverbs->dBR[4];
				pocketverbs->dBR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dBR[4] + pocketverbs->dBR[5] + pocketverbs->dBR[6]) / 3.0;

				allpasstemp = pocketverbs->outCR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayC) {
					allpasstemp = pocketverbs->delayC;
				}
				inputSampleR -= pocketverbs->oCR[allpasstemp] * constallpass;
				pocketverbs->oCR[pocketverbs->outCR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outCR--;
				if (pocketverbs->outCR < 0 || pocketverbs->outCR > pocketverbs->delayC) {
					pocketverbs->outCR = pocketverbs->delayC;
				}
				inputSampleR += (pocketverbs->oCR[pocketverbs->outCR]);
				// allpass filter C

				pocketverbs->dCR[6] = pocketverbs->dCR[5];
				pocketverbs->dCR[5] = pocketverbs->dCR[4];
				pocketverbs->dCR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dCR[4] + pocketverbs->dCR[5] + pocketverbs->dCR[6]) / 3.0;

				allpasstemp = pocketverbs->outDR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayD) {
					allpasstemp = pocketverbs->delayD;
				}
				inputSampleR -= pocketverbs->oDR[allpasstemp] * constallpass;
				pocketverbs->oDR[pocketverbs->outDR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outDR--;
				if (pocketverbs->outDR < 0 || pocketverbs->outDR > pocketverbs->delayD) {
					pocketverbs->outDR = pocketverbs->delayD;
				}
				inputSampleR += (pocketverbs->oDR[pocketverbs->outDR]);
				// allpass filter D

				pocketverbs->dDR[6] = pocketverbs->dDR[5];
				pocketverbs->dDR[5] = pocketverbs->dDR[4];
				pocketverbs->dDR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dDR[4] + pocketverbs->dDR[5] + pocketverbs->dDR[6]) / 3.0;

				allpasstemp = pocketverbs->outER - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayE) {
					allpasstemp = pocketverbs->delayE;
				}
				inputSampleR -= pocketverbs->oER[allpasstemp] * constallpass;
				pocketverbs->oER[pocketverbs->outER] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outER--;
				if (pocketverbs->outER < 0 || pocketverbs->outER > pocketverbs->delayE) {
					pocketverbs->outER = pocketverbs->delayE;
				}
				inputSampleR += (pocketverbs->oER[pocketverbs->outER]);
				// allpass filter E

				pocketverbs->dER[6] = pocketverbs->dER[5];
				pocketverbs->dER[5] = pocketverbs->dER[4];
				pocketverbs->dER[4] = inputSampleR;
				inputSampleR = (pocketverbs->dER[4] + pocketverbs->dER[5] + pocketverbs->dER[6]) / 3.0;

				allpasstemp = pocketverbs->outFR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayF) {
					allpasstemp = pocketverbs->delayF;
				}
				inputSampleR -= pocketverbs->oFR[allpasstemp] * constallpass;
				pocketverbs->oFR[pocketverbs->outFR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outFR--;
				if (pocketverbs->outFR < 0 || pocketverbs->outFR > pocketverbs->delayF) {
					pocketverbs->outFR = pocketverbs->delayF;
				}
				inputSampleR += (pocketverbs->oFR[pocketverbs->outFR]);
				// allpass filter F

				pocketverbs->dFR[6] = pocketverbs->dFR[5];
				pocketverbs->dFR[5] = pocketverbs->dFR[4];
				pocketverbs->dFR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dFR[4] + pocketverbs->dFR[5] + pocketverbs->dFR[6]) / 3.0;

				allpasstemp = pocketverbs->outGR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayG) {
					allpasstemp = pocketverbs->delayG;
				}
				inputSampleR -= pocketverbs->oGR[allpasstemp] * constallpass;
				pocketverbs->oGR[pocketverbs->outGR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outGR--;
				if (pocketverbs->outGR < 0 || pocketverbs->outGR > pocketverbs->delayG) {
					pocketverbs->outGR = pocketverbs->delayG;
				}
				inputSampleR += (pocketverbs->oGR[pocketverbs->outGR]);
				// allpass filter G

				pocketverbs->dGR[6] = pocketverbs->dGR[5];
				pocketverbs->dGR[5] = pocketverbs->dGR[4];
				pocketverbs->dGR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dGR[4] + pocketverbs->dGR[5] + pocketverbs->dGR[6]) / 3.0;

				allpasstemp = pocketverbs->outHR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayH) {
					allpasstemp = pocketverbs->delayH;
				}
				inputSampleR -= pocketverbs->oHR[allpasstemp] * constallpass;
				pocketverbs->oHR[pocketverbs->outHR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outHR--;
				if (pocketverbs->outHR < 0 || pocketverbs->outHR > pocketverbs->delayH) {
					pocketverbs->outHR = pocketverbs->delayH;
				}
				inputSampleR += (pocketverbs->oHR[pocketverbs->outHR]);
				// allpass filter H

				pocketverbs->dHR[6] = pocketverbs->dHR[5];
				pocketverbs->dHR[5] = pocketverbs->dHR[4];
				pocketverbs->dHR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dHR[4] + pocketverbs->dHR[5] + pocketverbs->dHR[6]) / 3.0;

				allpasstemp = pocketverbs->outIR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayI) {
					allpasstemp = pocketverbs->delayI;
				}
				inputSampleR -= pocketverbs->oIR[allpasstemp] * constallpass;
				pocketverbs->oIR[pocketverbs->outIR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outIR--;
				if (pocketverbs->outIR < 0 || pocketverbs->outIR > pocketverbs->delayI) {
					pocketverbs->outIR = pocketverbs->delayI;
				}
				inputSampleR += (pocketverbs->oIR[pocketverbs->outIR]);
				// allpass filter I

				pocketverbs->dIR[6] = pocketverbs->dIR[5];
				pocketverbs->dIR[5] = pocketverbs->dIR[4];
				pocketverbs->dIR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dIR[4] + pocketverbs->dIR[5] + pocketverbs->dIR[6]) / 3.0;

				allpasstemp = pocketverbs->outJR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayJ) {
					allpasstemp = pocketverbs->delayJ;
				}
				inputSampleR -= pocketverbs->oJR[allpasstemp] * constallpass;
				pocketverbs->oJR[pocketverbs->outJR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outJR--;
				if (pocketverbs->outJR < 0 || pocketverbs->outJR > pocketverbs->delayJ) {
					pocketverbs->outJR = pocketverbs->delayJ;
				}
				inputSampleR += (pocketverbs->oJR[pocketverbs->outJR]);
				// allpass filter J

				pocketverbs->dJR[6] = pocketverbs->dJR[5];
				pocketverbs->dJR[5] = pocketverbs->dJR[4];
				pocketverbs->dJR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dJR[4] + pocketverbs->dJR[5] + pocketverbs->dJR[6]) / 3.0;

				allpasstemp = pocketverbs->outKR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayK) {
					allpasstemp = pocketverbs->delayK;
				}
				inputSampleR -= pocketverbs->oKR[allpasstemp] * constallpass;
				pocketverbs->oKR[pocketverbs->outKR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outKR--;
				if (pocketverbs->outKR < 0 || pocketverbs->outKR > pocketverbs->delayK) {
					pocketverbs->outKR = pocketverbs->delayK;
				}
				inputSampleR += (pocketverbs->oKR[pocketverbs->outKR]);
				// allpass filter K

				pocketverbs->dKR[6] = pocketverbs->dKR[5];
				pocketverbs->dKR[5] = pocketverbs->dKR[4];
				pocketverbs->dKR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dKR[4] + pocketverbs->dKR[5] + pocketverbs->dKR[6]) / 3.0;

				allpasstemp = pocketverbs->outLR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayL) {
					allpasstemp = pocketverbs->delayL;
				}
				inputSampleR -= pocketverbs->oLR[allpasstemp] * constallpass;
				pocketverbs->oLR[pocketverbs->outLR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outLR--;
				if (pocketverbs->outLR < 0 || pocketverbs->outLR > pocketverbs->delayL) {
					pocketverbs->outLR = pocketverbs->delayL;
				}
				inputSampleR += (pocketverbs->oLR[pocketverbs->outLR]);
				// allpass filter L

				pocketverbs->dLR[6] = pocketverbs->dLR[5];
				pocketverbs->dLR[5] = pocketverbs->dLR[4];
				pocketverbs->dLR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dLR[4] + pocketverbs->dLR[5] + pocketverbs->dLR[6]) / 3.0;

				allpasstemp = pocketverbs->outMR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayM) {
					allpasstemp = pocketverbs->delayM;
				}
				inputSampleR -= pocketverbs->oMR[allpasstemp] * constallpass;
				pocketverbs->oMR[pocketverbs->outMR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outMR--;
				if (pocketverbs->outMR < 0 || pocketverbs->outMR > pocketverbs->delayM) {
					pocketverbs->outMR = pocketverbs->delayM;
				}
				inputSampleR += (pocketverbs->oMR[pocketverbs->outMR]);
				// allpass filter M

				pocketverbs->dMR[6] = pocketverbs->dMR[5];
				pocketverbs->dMR[5] = pocketverbs->dMR[4];
				pocketverbs->dMR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dMR[4] + pocketverbs->dMR[5] + pocketverbs->dMR[6]) / 3.0;

				allpasstemp = pocketverbs->outNR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayN) {
					allpasstemp = pocketverbs->delayN;
				}
				inputSampleR -= pocketverbs->oNR[allpasstemp] * constallpass;
				pocketverbs->oNR[pocketverbs->outNR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outNR--;
				if (pocketverbs->outNR < 0 || pocketverbs->outNR > pocketverbs->delayN) {
					pocketverbs->outNR = pocketverbs->delayN;
				}
				inputSampleR += (pocketverbs->oNR[pocketverbs->outNR]);
				// allpass filter N

				pocketverbs->dNR[6] = pocketverbs->dNR[5];
				pocketverbs->dNR[5] = pocketverbs->dNR[4];
				pocketverbs->dNR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dNR[4] + pocketverbs->dNR[5] + pocketverbs->dNR[6]) / 3.0;

				allpasstemp = pocketverbs->outOR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayO) {
					allpasstemp = pocketverbs->delayO;
				}
				inputSampleR -= pocketverbs->oOR[allpasstemp] * constallpass;
				pocketverbs->oOR[pocketverbs->outOR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outOR--;
				if (pocketverbs->outOR < 0 || pocketverbs->outOR > pocketverbs->delayO) {
					pocketverbs->outOR = pocketverbs->delayO;
				}
				inputSampleR += (pocketverbs->oOR[pocketverbs->outOR]);
				// allpass filter O

				pocketverbs->dOR[6] = pocketverbs->dOR[5];
				pocketverbs->dOR[5] = pocketverbs->dOR[4];
				pocketverbs->dOR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dOR[4] + pocketverbs->dOR[5] + pocketverbs->dOR[6]) / 3.0;

				allpasstemp = pocketverbs->outPR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayP) {
					allpasstemp = pocketverbs->delayP;
				}
				inputSampleR -= pocketverbs->oPR[allpasstemp] * constallpass;
				pocketverbs->oPR[pocketverbs->outPR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outPR--;
				if (pocketverbs->outPR < 0 || pocketverbs->outPR > pocketverbs->delayP) {
					pocketverbs->outPR = pocketverbs->delayP;
				}
				inputSampleR += (pocketverbs->oPR[pocketverbs->outPR]);
				// allpass filter P

				pocketverbs->dPR[6] = pocketverbs->dPR[5];
				pocketverbs->dPR[5] = pocketverbs->dPR[4];
				pocketverbs->dPR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dPR[4] + pocketverbs->dPR[5] + pocketverbs->dPR[6]) / 3.0;

				allpasstemp = pocketverbs->outQR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayQ) {
					allpasstemp = pocketverbs->delayQ;
				}
				inputSampleR -= pocketverbs->oQR[allpasstemp] * constallpass;
				pocketverbs->oQR[pocketverbs->outQR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outQR--;
				if (pocketverbs->outQR < 0 || pocketverbs->outQR > pocketverbs->delayQ) {
					pocketverbs->outQR = pocketverbs->delayQ;
				}
				inputSampleR += (pocketverbs->oQR[pocketverbs->outQR]);
				// allpass filter Q

				pocketverbs->dQR[6] = pocketverbs->dQR[5];
				pocketverbs->dQR[5] = pocketverbs->dQR[4];
				pocketverbs->dQR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dQR[4] + pocketverbs->dQR[5] + pocketverbs->dQR[6]) / 3.0;

				allpasstemp = pocketverbs->outRR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayR) {
					allpasstemp = pocketverbs->delayR;
				}
				inputSampleR -= pocketverbs->oRR[allpasstemp] * constallpass;
				pocketverbs->oRR[pocketverbs->outRR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outRR--;
				if (pocketverbs->outRR < 0 || pocketverbs->outRR > pocketverbs->delayR) {
					pocketverbs->outRR = pocketverbs->delayR;
				}
				inputSampleR += (pocketverbs->oRR[pocketverbs->outRR]);
				// allpass filter R

				pocketverbs->dRR[6] = pocketverbs->dRR[5];
				pocketverbs->dRR[5] = pocketverbs->dRR[4];
				pocketverbs->dRR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dRR[4] + pocketverbs->dRR[5] + pocketverbs->dRR[6]) / 3.0;

				allpasstemp = pocketverbs->outSR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayS) {
					allpasstemp = pocketverbs->delayS;
				}
				inputSampleR -= pocketverbs->oSR[allpasstemp] * constallpass;
				pocketverbs->oSR[pocketverbs->outSR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outSR--;
				if (pocketverbs->outSR < 0 || pocketverbs->outSR > pocketverbs->delayS) {
					pocketverbs->outSR = pocketverbs->delayS;
				}
				inputSampleR += (pocketverbs->oSR[pocketverbs->outSR]);
				// allpass filter S

				pocketverbs->dSR[6] = pocketverbs->dSR[5];
				pocketverbs->dSR[5] = pocketverbs->dSR[4];
				pocketverbs->dSR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dSR[4] + pocketverbs->dSR[5] + pocketverbs->dSR[6]) / 3.0;

				allpasstemp = pocketverbs->outTR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayT) {
					allpasstemp = pocketverbs->delayT;
				}
				inputSampleR -= pocketverbs->oTR[allpasstemp] * constallpass;
				pocketverbs->oTR[pocketverbs->outTR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outTR--;
				if (pocketverbs->outTR < 0 || pocketverbs->outTR > pocketverbs->delayT) {
					pocketverbs->outTR = pocketverbs->delayT;
				}
				inputSampleR += (pocketverbs->oTR[pocketverbs->outTR]);
				// allpass filter T

				pocketverbs->dTR[6] = pocketverbs->dTR[5];
				pocketverbs->dTR[5] = pocketverbs->dTR[4];
				pocketverbs->dTR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dTR[4] + pocketverbs->dTR[5] + pocketverbs->dTR[6]) / 3.0;

				allpasstemp = pocketverbs->outUR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayU) {
					allpasstemp = pocketverbs->delayU;
				}
				inputSampleR -= pocketverbs->oUR[allpasstemp] * constallpass;
				pocketverbs->oUR[pocketverbs->outUR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outUR--;
				if (pocketverbs->outUR < 0 || pocketverbs->outUR > pocketverbs->delayU) {
					pocketverbs->outUR = pocketverbs->delayU;
				}
				inputSampleR += (pocketverbs->oUR[pocketverbs->outUR]);
				// allpass filter U

				pocketverbs->dUR[6] = pocketverbs->dUR[5];
				pocketverbs->dUR[5] = pocketverbs->dUR[4];
				pocketverbs->dUR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dUR[4] + pocketverbs->dUR[5] + pocketverbs->dUR[6]) / 3.0;

				allpasstemp = pocketverbs->outVR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayV) {
					allpasstemp = pocketverbs->delayV;
				}
				inputSampleR -= pocketverbs->oVR[allpasstemp] * constallpass;
				pocketverbs->oVR[pocketverbs->outVR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outVR--;
				if (pocketverbs->outVR < 0 || pocketverbs->outVR > pocketverbs->delayV) {
					pocketverbs->outVR = pocketverbs->delayV;
				}
				inputSampleR += (pocketverbs->oVR[pocketverbs->outVR]);
				// allpass filter V

				pocketverbs->dVR[6] = pocketverbs->dVR[5];
				pocketverbs->dVR[5] = pocketverbs->dVR[4];
				pocketverbs->dVR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dVR[4] + pocketverbs->dVR[5] + pocketverbs->dVR[6]) / 3.0;

				allpasstemp = pocketverbs->outWR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayW) {
					allpasstemp = pocketverbs->delayW;
				}
				inputSampleR -= pocketverbs->oWR[allpasstemp] * constallpass;
				pocketverbs->oWR[pocketverbs->outWR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outWR--;
				if (pocketverbs->outWR < 0 || pocketverbs->outWR > pocketverbs->delayW) {
					pocketverbs->outWR = pocketverbs->delayW;
				}
				inputSampleR += (pocketverbs->oWR[pocketverbs->outWR]);
				// allpass filter W

				pocketverbs->dWR[6] = pocketverbs->dWR[5];
				pocketverbs->dWR[5] = pocketverbs->dWR[4];
				pocketverbs->dWR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dWR[4] + pocketverbs->dWR[5] + pocketverbs->dWR[6]) / 3.0;

				allpasstemp = pocketverbs->outXR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayX) {
					allpasstemp = pocketverbs->delayX;
				}
				inputSampleR -= pocketverbs->oXR[allpasstemp] * constallpass;
				pocketverbs->oXR[pocketverbs->outXR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outXR--;
				if (pocketverbs->outXR < 0 || pocketverbs->outXR > pocketverbs->delayX) {
					pocketverbs->outXR = pocketverbs->delayX;
				}
				inputSampleR += (pocketverbs->oXR[pocketverbs->outXR]);
				// allpass filter X

				pocketverbs->dXR[6] = pocketverbs->dXR[5];
				pocketverbs->dXR[5] = pocketverbs->dXR[4];
				pocketverbs->dXR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dXR[4] + pocketverbs->dXR[5] + pocketverbs->dXR[6]) / 3.0;

				allpasstemp = pocketverbs->outYR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayY) {
					allpasstemp = pocketverbs->delayY;
				}
				inputSampleR -= pocketverbs->oYR[allpasstemp] * constallpass;
				pocketverbs->oYR[pocketverbs->outYR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outYR--;
				if (pocketverbs->outYR < 0 || pocketverbs->outYR > pocketverbs->delayY) {
					pocketverbs->outYR = pocketverbs->delayY;
				}
				inputSampleR += (pocketverbs->oYR[pocketverbs->outYR]);
				// allpass filter Y

				pocketverbs->dYR[6] = pocketverbs->dYR[5];
				pocketverbs->dYR[5] = pocketverbs->dYR[4];
				pocketverbs->dYR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dYR[4] + pocketverbs->dYR[5] + pocketverbs->dYR[6]) / 3.0;

				allpasstemp = pocketverbs->outZR - 1;
				if (allpasstemp < 0 || allpasstemp > pocketverbs->delayZ) {
					allpasstemp = pocketverbs->delayZ;
				}
				inputSampleR -= pocketverbs->oZR[allpasstemp] * constallpass;
				pocketverbs->oZR[pocketverbs->outZR] = inputSampleR;
				inputSampleR *= constallpass;
				pocketverbs->outZR--;
				if (pocketverbs->outZR < 0 || pocketverbs->outZR > pocketverbs->delayZ) {
					pocketverbs->outZR = pocketverbs->delayZ;
				}
				inputSampleR += (pocketverbs->oZR[pocketverbs->outZR]);
				// allpass filter Z

				pocketverbs->dZR[6] = pocketverbs->dZR[5];
				pocketverbs->dZR[5] = pocketverbs->dZR[4];
				pocketverbs->dZR[4] = inputSampleR;
				inputSampleR = (pocketverbs->dZR[4] + pocketverbs->dZR[5] + pocketverbs->dZR[6]);
				// output Zarathustra infinite space verb
				break;
		}
		// end big switch for verb type

		bridgerectifier = fabs(inputSampleL);
		bridgerectifier = 1.0 - cos(bridgerectifier);
		if (inputSampleL > 0) inputSampleL -= bridgerectifier;
		else inputSampleL += bridgerectifier;
		inputSampleL /= gain;
		bridgerectifier = fabs(inputSampleR);
		bridgerectifier = 1.0 - cos(bridgerectifier);
		if (inputSampleR > 0) inputSampleR -= bridgerectifier;
		else inputSampleR += bridgerectifier;
		inputSampleR /= gain;
		// here we apply the ADT2 'console on steroids' trick

		wetness = wetnesstarget;
		// setting up verb wetness to be manipulated by gate and peak

		wetness *= pocketverbs->peakL;
		// but we only use peak (indirect) to deal with dry/wet, so that it'll manipulate the dry/wet like we want

		drySampleL *= (1.0 - wetness);
		inputSampleL *= wetness;
		inputSampleL += drySampleL;
		// here we combine the tanks with the dry signal

		wetness = wetnesstarget;
		// setting up verb wetness to be manipulated by gate and peak

		wetness *= pocketverbs->peakR;
		// but we only use peak (indirect) to deal with dry/wet, so that it'll manipulate the dry/wet like we want

		drySampleR *= (1.0 - wetness);
		inputSampleR *= wetness;
		inputSampleR += drySampleR;
		// here we combine the tanks with the dry signal

		// begin 32 bit stereo floating point dither
		int expon;
		frexpf((float) inputSampleL, &expon);
		pocketverbs->fpdL ^= pocketverbs->fpdL << 13;
		pocketverbs->fpdL ^= pocketverbs->fpdL >> 17;
		pocketverbs->fpdL ^= pocketverbs->fpdL << 5;
		inputSampleL += (((double) pocketverbs->fpdL - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
		frexpf((float) inputSampleR, &expon);
		pocketverbs->fpdR ^= pocketverbs->fpdR << 13;
		pocketverbs->fpdR ^= pocketverbs->fpdR >> 17;
		pocketverbs->fpdR ^= pocketverbs->fpdR << 5;
		inputSampleR += (((double) pocketverbs->fpdR - (uint32_t) 0x7fffffff) * 5.5e-36l * pow(2, expon + 62));
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
	POCKETVERBS_URI,
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
