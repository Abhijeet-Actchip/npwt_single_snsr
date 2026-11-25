/*
 * thpy_algo_cont.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Mahesh Murty
 */

#include "thpy_algo_cont.h"
#include "bsp/board.h"
#include "app_hal/app_hal_delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "press_regulator.h"
#include "utils/assert/app_assert.h"

/* Pressure Range */
#define THPY_PRESSURE_MAX					-300.0f
#define THPY_PRESSURE_MIN					-25.0f

/* Max Pressure difference b/w reference and vacuum pressure,
 * when no leak or block */
//#define PRESSURE_BLOCK_DET_THRESH			50.0f
//#define PRESSURE_BLOCK_DET_TIME_S			10
#define PRESSURE_BLOCK_DET_THRESH			100.0f
#define PRESSURE_BLOCK_DET_TIME_S			15

/* Max Pressure Leakage percent allowed */
#define PRESSURE_LEAK_DET_THRESH			10.0f
#define PRESSURE_LEAK_DET_TIME_S			30

typedef struct _cThpyState_t
{
	float pSetPt;
	bool thpyEnable;
	thpy_cmd_t cmd;
	thpy_fault_t fault;
	float leakPercent;
	bool isThpyPaused;
	int32_t remTime;
	TickType_t timerTicks, faultDetTicks;
	uint8_t blockCtr, leakCtr;
}cThpyState_t;

static cThpyState_t state;

static void AcquireLock(struct thpy_class_t *this)
{
	BaseType_t status;
	status = xSemaphoreTake(this->lock, pdMS_TO_TICKS(2000));
	APP_ASSERT(status == pdTRUE);
}

static void ReleaseLock(struct thpy_class_t *this)
{
	xSemaphoreGive(this->lock);
}

static void DetectFaults(struct thpy_class_t *this, plantData_t *snsrData)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	/* Time Based Implementation */
	TickType_t currTicks = xTaskGetTickCount();
	if((currTicks - ptrState->faultDetTicks) >= pdMS_TO_TICKS(1000))
	{
		ptrState->faultDetTicks = currTicks;
		ptrState->leakPercent = ((ptrState->pSetPt - snsrData->refPressure)/ptrState->pSetPt) * 100;
		if((snsrData->refPressure - snsrData->vacuumPressure) > PRESSURE_BLOCK_DET_THRESH)
		{
			ptrState->leakCtr = 0;
			if(ptrState->blockCtr < PRESSURE_BLOCK_DET_TIME_S)
			{
				ptrState->blockCtr++;
			}
			else
			{
				ptrState->fault = COMM_THERAPY_FAULT_PIPE_BLOCKED;
				ptrState->cmd = COMM_THERAPY_CMD_STOP;
				ptrState->isThpyPaused = 1;
				/* Request TherapyStop from comm */
			}
		}
		else if(ptrState->leakPercent > PRESSURE_LEAK_DET_THRESH)
		{
			ptrState->blockCtr = 0;
			if(ptrState->leakCtr < PRESSURE_LEAK_DET_TIME_S)
			{
				ptrState->leakCtr++;
			}
			else
			{
				ptrState->fault = COMM_THERAPY_FAULT_PIPE_LEAKED;
				/* Send leakPercent */
			}
		}
		else
		{
			ptrState->blockCtr = 0;
			ptrState->leakCtr = 0;
			ptrState->fault = COMM_THERAPY_FAULT_NONE;
		}
	}
}


static int8_t RunCtrlLoop(struct thpy_class_t *this, plantData_t *snsrData)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	int8_t retVal;

	AcquireLock(this);

	if(!this->isInit)
	{
		/* No action is required in this case. */
		goto no_act;
	}

	/* Check is Therapy Paused */
	if(ptrState->isThpyPaused)
	{
		 /* Disable Thpy */
		 if(ptrState->thpyEnable)
		 {
			 this->DisableThpy(this);
		 }
	}
	else
	{
		 /* Enable Thpy */
		 if(!ptrState->thpyEnable)
		 {
			 this->EnableThpy(this);
		 }
	}

	/* Check Timer */
	if(ptrState->remTime < 0)
	{
		/* Therapy Runs for Infinite time */
	}
	else									// Timer prsnt, run for thpy time
	{
		if((ptrState->thpyEnable) & (ptrState->remTime > 0))
		{
			TickType_t currTicks = xTaskGetTickCount();
			if((currTicks - ptrState->timerTicks) > pdMS_TO_TICKS(1000))
			{
				ptrState->timerTicks = currTicks;
				ptrState->remTime--;
				if(ptrState->remTime == 0)
				{
					/* Therapy Completed */
					ptrState->fault = COMM_THERAPY_FAULT_NONE;
					ptrState->cmd = COMM_THERAPY_CMD_STOP;
					ptrState->isThpyPaused = 1;
				}
			}
		}
	}

	if(ptrState->thpyEnable)
	{
		retVal = PressRegRunCtrlLoop(snsrData, ptrState->pSetPt);
		DetectFaults(this, snsrData);
	}
	else
	{
		retVal = APP_ERR_NONE;
	}

no_act:
	ReleaseLock(this);

	return retVal;
}

static int8_t EnableThpy(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	PressRegEnable(1);
	ptrState->thpyEnable = true;

	return APP_ERR_NONE;
}

static int8_t DisableThpy(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	PressRegEnable(0);
	APPHAL_Delay(10);
	Board_CtrlValve(1);
	APPHAL_Delay(500);
	Board_CtrlValve(0);
	ptrState->thpyEnable = false;

	return APP_ERR_NONE;
}

static int8_t PauseThpy(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;
	ptrState->cmd = COMM_THERAPY_CMD_PAUSE;
	ptrState->isThpyPaused = 1;

	return APP_ERR_NONE;
}

static int8_t ResumeThpy(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;
	ptrState->cmd = COMM_THERAPY_CMD_START;
	ptrState->isThpyPaused = 0;

	return APP_ERR_NONE;
}

static int8_t SetPressureSp(struct thpy_class_t *this, float sp)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	/* Limit Target pressure b/t THPY_PRESSURE_MIN and THPY_PRESSURE_MAX */
	if(sp < THPY_PRESSURE_MAX)
	{
		sp = THPY_PRESSURE_MAX;
	}
	else if(sp > THPY_PRESSURE_MIN)
	{
		sp = THPY_PRESSURE_MIN;
	}

	ptrState->pSetPt = sp;
	return APP_ERR_NONE;
}

static thpy_fault_t GetFaults(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	return ptrState->fault;
}

static bool IsThpyRunning(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	return ptrState->thpyEnable;
}

static int32_t GetRemTime(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	return ptrState->remTime;
}

static float GetPressureSp(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	return ptrState->pSetPt;
}

static thpy_cmd_t GetThpyCmd(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	return ptrState->cmd;
}

static float GetLeakPercent(struct thpy_class_t *this)
{
	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	return ((ptrState->fault == COMM_THERAPY_FAULT_PIPE_LEAKED) ? ptrState->leakPercent : 0);
}

static void ClearAPIs(thpy_class_t *thpyObj)
{
	thpyObj->RunCtrlLoop = NULL;
	thpyObj->EnableThpy = NULL;
	thpyObj->DisableThpy = NULL;
	thpyObj->PauseThpy = NULL;
	thpyObj->ResumeThpy = NULL;
	thpyObj->SetPressureSp = NULL;
	thpyObj->GetPressureSp = NULL;
	thpyObj->GetFaults = NULL;
	thpyObj->IsThpyRunning = NULL;
	thpyObj->GetRemTime = NULL;
	thpyObj->GetThpyCmd = NULL;
	thpyObj->GetLeakPercent = NULL;
	thpyObj->DeInit = NULL;
}

static int8_t DeInit(struct thpy_class_t *this)
{
	AcquireLock(this);

	cThpyState_t *ptrState = (cThpyState_t *)this->thpyData;

	int8_t retVal;

	/* Turn OFF pressure regulator */
	retVal = PressRegEnable(0);
	/* Turn OFF the Vacuum Motor */
	retVal = Board_CtrlVacuumMot(0);
	/* Disable Vacuum Motor */
	retVal = Board_EnableVacuumMot(0);
	/* Open the Valve, until refPressure is 1 atm */
	retVal = Board_CtrlValve(1);

	ptrState->thpyEnable = 0;

	DisableThpy(this);
	ClearAPIs(this);
	this->isInit = false;

	ReleaseLock(this);

	return retVal;
}

static void SetAPIs(thpy_class_t *thpyObj)
{
	thpyObj->RunCtrlLoop = RunCtrlLoop;
	thpyObj->EnableThpy = EnableThpy;
	thpyObj->DisableThpy = DisableThpy;
	thpyObj->PauseThpy = PauseThpy;
	thpyObj->ResumeThpy = ResumeThpy;
	thpyObj->SetPressureSp = SetPressureSp;
	thpyObj->GetPressureSp = GetPressureSp;
	thpyObj->GetFaults = GetFaults;
	thpyObj->IsThpyRunning = IsThpyRunning;
	thpyObj->GetRemTime = GetRemTime;
	thpyObj->GetThpyCmd = GetThpyCmd;
	thpyObj->GetLeakPercent = GetLeakPercent;
	thpyObj->DeInit = DeInit;
}

int8_t ThpyContInit(thpy_class_t *thpyObj)
{
	SetAPIs(thpyObj);
	thpyObj->isInit = true;

	/* Reset Therapy state */
	state.remTime = thpyObj->thpyDurationS;
	state.isThpyPaused = 0;
	state.fault = COMM_THERAPY_FAULT_NONE;
	state.cmd = COMM_THERAPY_CMD_START;
	state.timerTicks = 0;
	state.faultDetTicks = 0;
	state.leakCtr = 0;
	state.blockCtr = 0;

	thpyObj->thpyData = (void*)&state;
	EnableThpy(thpyObj);

	return APP_ERR_NONE;
}
