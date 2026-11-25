/*
 * thpy_base.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Mahesh Murty
 */

#include "thpy_base.h"
#include "config/appconfig.h"
#include "thpy_algo_cont.h"
#include "thpy_algo_dynm.h"
#include "thpy_algo_intr.h"
#include "utils/assert/app_assert.h"

static int8_t BaseRunCtrlLoop(struct thpy_class_t *this, plantData_t *snsrData)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static int8_t BasePauseThpy(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static int8_t BaseResumeThpy(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static int8_t BaseEnableThpy(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static int8_t BaseDisableThpy(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static int8_t BaseSetPressureSp(struct thpy_class_t *this, float sp)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static thpy_fault_t BaseGetFaults(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return -1;
}

static bool BaseIsThpyRunning(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return false;
}

static int32_t BaseGetRemTime(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return 0;
}

static int32_t BaseGetDutyTime(struct thpy_class_t *this)
{
	/* GetDutyTime() implementation is only valid for intermittent therapy. */
	if(this->type != COMM_THERAPY_TYPE_INTERMITTENT)
	{
		return 0;
	}
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return 0;
}

static float BaseGetPressureSp(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return 0;
}

static thpy_cmd_t BaseGetThpyCmd(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return 0;
}

static int8_t BaseDeInit(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static float BaseGetLeakPercent(struct thpy_class_t *this)
{
	/* This function is to be implemented by the thpy algo module. */
	APP_ASSERT(0);
	return APP_ERR_INV_STATE;
}

static void SetBaseAPIs(thpy_class_t *thpyObj)
{
	/* Initialize the object with the base APIs so that if the
	 * algorithm init() function fails to implement these APIs it will
	 * result in an assert.
	 */
	thpyObj->RunCtrlLoop = BaseRunCtrlLoop;
	thpyObj->EnableThpy = BaseEnableThpy;
	thpyObj->DisableThpy = BaseDisableThpy;
	thpyObj->PauseThpy = BasePauseThpy;
	thpyObj->ResumeThpy = BaseResumeThpy;
	thpyObj->SetPressureSp = BaseSetPressureSp;
	thpyObj->GetFaults = BaseGetFaults;
	thpyObj->IsThpyRunning = BaseIsThpyRunning;
	thpyObj->GetRemTime = BaseGetRemTime;
	thpyObj->GetDutyTime = BaseGetDutyTime;
	thpyObj->GetPressureSp = BaseGetPressureSp;
	thpyObj->GetThpyCmd = BaseGetThpyCmd;
	thpyObj->GetLeakPercent = BaseGetLeakPercent;
	thpyObj->DeInit = BaseDeInit;
}

int8_t ThpyInit(thpy_class_t *thpyObj)
{
	int8_t retVal = APP_ERR_INV_ARG;

	if(thpyObj == NULL)
	{
		return retVal;
	}

	SetBaseAPIs(thpyObj);

	switch(thpyObj->type)
	{
		case COMM_THERAPY_TYPE_CONTINUOUS:
			retVal = ThpyContInit(thpyObj);
			break;
		case COMM_THERAPY_TYPE_DYNAMIC:
			retVal = ThpyDynmInit(thpyObj);
			break;
		case COMM_THERAPY_TYPE_INTERMITTENT:
			retVal = ThpyIntrInit(thpyObj);
			break;
		default:
			retVal = APP_ERR_INV_ARG;
			break;
	}

	return retVal;
}

bool IsThpyInit(thpy_class_t *thpyObj)
{
	return thpyObj->isInit;
}
