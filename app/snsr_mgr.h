/*
 * snsr_mgr.h
 *
 *  Created on: Mar 10, 2025
 *      Author: Mahesh Murty
 */

#ifndef __SNSR_MGR_H__
#define __SNSR_MGR_H__				1

#include <stdint.h>

typedef struct _plantData_t
{
	float batV;
	float refPressure;
	float vacuumPressure;
	float vacuumCurrent;
	float irigCurrent;
	float valveCurrent;
	float vaccumMotFb;
	uint8_t canDet;

	/* Temporary variable */
	uint8_t tempCanDet;
	uint8_t tempCanDetCtr;
} plantData_t;

/* \brief: Starts ADC conversion stores the converted result into arg.
 *
 * NOTE: This function should be called periodically so that all the
 * analog measurements are updated in RAM.
 */
int8_t SnsrMgrReadPlantData(plantData_t *snsrData);
int8_t SnsrMgrGetBatV(float *ptrBatV);
int8_t SnsrMgrGetBatChrgSts(uint8_t *ptrBatSts);

#endif /* __SNSR_MGR_H__ */
