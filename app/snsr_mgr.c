/*
 * snsr_mgr.c
 *
 *  Created on: Mar 10, 2025
 *      Author: Mahesh Murty
 */

#include "snsr_mgr.h"
#include "bsp/board.h"

#define CANNISTER_DET_THRESH_CTR		10

static float batV;

/* \brief: Starts ADC conversion stores the converted result into arg.
 *
 * NOTE: This function should be called periodically so that all the
 * analog measurements are updated in RAM.
 */
int8_t SnsrMgrReadPlantData(plantData_t *snsrData)
{
	Board_GetBatV(&snsrData->batV);
	/* Store battery voltage value for future use. */
	batV = snsrData->batV;
//	Board_GetIrriMotCurr(&snsrData->irigCurrent);
//	Board_GetVacuumMotCurr(&snsrData->vacuumCurrent);
//	Board_GetValveCurr(&snsrData->valveCurrent);
	Board_GetRefPres(&snsrData->refPressure);
	Board_GetVacuumPres(&snsrData->vacuumPressure);

	/* Implement cannister detection */
	uint8_t currCanSt = Board_GetCannisterDetStatus();

	if(currCanSt == snsrData->tempCanDet)
	{
		snsrData->tempCanDetCtr++;
	}
	else
	{
		snsrData->tempCanDetCtr = 1;
		snsrData->tempCanDet = currCanSt;
	}

	if(snsrData->tempCanDetCtr >= CANNISTER_DET_THRESH_CTR)
	{
		snsrData->tempCanDetCtr = 0;
		snsrData->canDet = snsrData->tempCanDet;
	}

	return APP_ERR_NONE;
}

int8_t SnsrMgrGetBatV(float *ptrBatV)
{
	if(ptrBatV == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	*ptrBatV = batV;

	return APP_ERR_NONE;
}

int8_t SnsrMgrGetBatChrgSts(uint8_t *ptrBatSts)
{
	if(ptrBatSts == NULL)
		return APP_ERR_INV_ARG;

	/* Output base on charger IC states */
//	uint8_t chrgSts = 0, doneSts = 0;
//
//	chrgSts = Board_GetBatChrgSts();
//	doneSts = Board_GetBatDoneSts();

//	*ptrBatSts = ((!chrgSts) & doneSts);

	/* Ouput based on jack state */
	*ptrBatSts = Board_PowerJackDet();

	return APP_ERR_NONE;
}
