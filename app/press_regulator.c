/*
 * press_regulator.c
 *
 *  Created on: Feb 17, 2025
 *      Author: Mahesh Murty
 */

#include "press_regulator.h"
#include "bsp/board.h"

#include "../app_utils/assert/app_assert.h"
#include "../app_utils/trice/trice.h"


/*!< Acceptable +/- error in Regulation */
#define PRESSURE_TOLERANCE			5
#define PRESSURE_MIN_TOLERANCE		0
#define PRESSURE_MAX_TOLERANCE		10

static bool regEn = false;
static bool holdPress = false;
/* Kept here for debugging. */
float setPoint = 0;
float err = 0, refPress = 0, vacPres = 0;

int8_t PressRegEnable(bool en)
{
	/* Do not repeat this logic if this function is called multiple times. */
	if(regEn == en)
		return APP_ERR_NONE;

	if(!en)
	{
		Board_CtrlVacuumMot(0);
		Board_EnableVacuumMot(0);
		Board_CtrlValve(0);
	}
	else
	{
		Board_EnableVacuumMot(1);
		holdPress = false;
	}
	regEn = en;

	return APP_ERR_NONE;
}

int8_t PressRegRunCtrlLoop(plantData_t *snsrData, float setPressure)
{
	int8_t retVal = APP_ERR_INV_ARG;

	if(snsrData == NULL)
		return retVal;

	float speed;

	setPoint = setPressure;

	retVal = APP_ERR_NONE;
	/* If the Pressure Regulation is enabled, */
	if(regEn)
	{
		/* Calculate error. */
		err = setPressure - snsrData->refPressure;

		refPress = snsrData->refPressure;
		vacPres = snsrData->vacuumPressure;

		trice32(iD(7678), "dbg: PRESSURE-REG:Set Pressure = %0.2f mmHg, Ref Pressure = %0.2f mmHg, Error = %0.2f\n",
				aFloat(setPressure), aFloat(snsrData->refPressure), aFloat(err));

		if(!holdPress)
		{
			if(err >= PRESSURE_MAX_TOLERANCE + 5)
			{
				/* Overshoot */
				Board_EnableVacuumMot(0);
				Board_CtrlVacuumMot(0);
				Board_CtrlValve(1);
			}
			else if(err <= PRESSURE_MAX_TOLERANCE)
			{
				/* NOTE: Reverted reduced motor speed */
				speed = (setPressure > -75.0) ? 75 : 100;
//				speed = 75;
				trice(iD(5738), "dbg: PRESSURE-REG: Neg Pressure increasing...\n");
				Board_EnableVacuumMot(1);
				Board_CtrlVacuumMot(speed);
				Board_CtrlValve(0);
				retVal = APP_ERR_NONE;
			}
			else
			{
				holdPress = true;
				Board_EnableVacuumMot(0);
				Board_CtrlVacuumMot(0);
				Board_CtrlValve(0);
			}
		}
		else
		{
			if((err >= PRESSURE_MIN_TOLERANCE - 15) && (err <= PRESSURE_MAX_TOLERANCE + 5))
			{
				/* Hold pressure */
				Board_EnableVacuumMot(0);
				Board_CtrlVacuumMot(0);
				Board_CtrlValve(0);
			}
			else if(err < (PRESSURE_MIN_TOLERANCE - 15))
			{
				holdPress = false;
			}
		}
	}

	return retVal;
}
