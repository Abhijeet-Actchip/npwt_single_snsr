/*
 * thpy_mgr.c
 *
 *  Created on: Feb 17, 2025
 *      Author: Mahesh Murty
 */

#include "thpy_mgr.h"
#include "bsp/board.h"
#include "app_hal/app_hal_delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "app_utils/trice/trice.h"
#include "app_utils/assert/app_assert.h"
#include "thpy_base.h"

#define PER_TIMER_INT_MS				100

/* NOTE: This is a private data structure, not to be exposed to the rest of the application. */
typedef struct _thpy_mgr_state_t
{
	thpy_class_t thpyObj;
	irig_class_t irigObj;
	plantData_t snsrData;
	SemaphoreHandle_t timerOvfSem;
	TimerHandle_t perTimer;
	TaskHandle_t taskHandle;
}thpy_mgr_state_t;

static thpy_mgr_state_t svcState;

static void PerTimerOvfEvt( TimerHandle_t xTimer )
{
	xSemaphoreGive(svcState.timerOvfSem);
}

static void ThpyMgrTask(void *args)
{
	/* Start the therapy timer. */
	BaseType_t status = xTimerStart(svcState.perTimer, pdMS_TO_TICKS(100));
	APP_ASSERT(status == pdPASS);

	/* Worst case time in which the timer must expire. */
	const uint32_t semWaitTicks = pdMS_TO_TICKS(PER_TIMER_INT_MS + 10);

	while(1)
	{
		/* Wait on sem for the timer to expire. */
		BaseType_t status = xSemaphoreTake(svcState.timerOvfSem, semWaitTicks);
		APP_ASSERT(status == pdTRUE);

		SnsrMgrReadPlantData(&svcState.snsrData);
		if(IsThpyInit(&svcState.thpyObj))
		{
			/* Run pressure regulation control loop for different therapies. */
			svcState.thpyObj.RunCtrlLoop(&svcState.thpyObj, &svcState.snsrData);
		}

		if(IsIrigInit(&svcState.irigObj))
		{
			/* Run Irrigation control loop */
			svcState.irigObj.RunCtrlLoop(&svcState.irigObj);
		}
	}
}

int8_t ThpyMgrInit(void)
{
	BaseType_t status;

	svcState.timerOvfSem = xSemaphoreCreateBinary();
	if(svcState.timerOvfSem == NULL)
	{
		return APP_ERR_NO_MEM;
	}

	svcState.thpyObj.lock = xSemaphoreCreateMutex();
	if(svcState.thpyObj.lock == NULL)
	{
		return APP_ERR_NO_MEM;
	}

	svcState.irigObj.lock = xSemaphoreCreateMutex();
	if(svcState.irigObj.lock == NULL)
	{
		return APP_ERR_NO_MEM;
	}

	svcState.perTimer = xTimerCreate("THPY-TIMER",
						pdMS_TO_TICKS(PER_TIMER_INT_MS),
						pdTRUE, NULL, PerTimerOvfEvt);
	if(svcState.perTimer == NULL)
	{
		return APP_ERR_NO_MEM;
	}

	/* Create the therapy manager task. */
	status = xTaskCreate(ThpyMgrTask, "THPYMGR",
			THERAPY_MGR_STACK_SIZE, NULL,
			THERAPY_MGR_PRIORITY, &svcState.taskHandle);
	if(status != pdPASS)
	{
		return APP_ERR_NO_MEM;
	}

	return APP_ERR_NONE;
}

int8_t ThpyMgrIsCannisterPresent(COMM_CannisterStateMsg *req)
{
	if(req == NULL)
		return APP_ERR_INV_ARG;

	req->det = (svcState.snsrData.canDet) ? COMM_CANNISTER_DET_PRESENT : COMM_CANNISTER_DET_ABSENT;

	return APP_ERR_NONE;
}

int8_t ThpyMgrRunTherapy(COMM_TherapyReqMsg *req)
{
	int8_t retVal = APP_ERR_INV_ARG;

	if(req == NULL)
	{
		return retVal;
	}

	/* Stop therapy if it is already running. */
	retVal = ThpyMgrStopTherapy();

	if(req->has_type)
	{
		svcState.thpyObj.type = req->type;
	}
	else
	{
		svcState.thpyObj.type = COMM_THERAPY_TYPE_THPY_NONE;
	}

	if(req->has_therapyTimeS)
	{
		svcState.thpyObj.thpyDurationS = req->therapyTimeS;
	}
	else
	{
		svcState.thpyObj.thpyDurationS = -1;
	}

	retVal = ThpyInit(&svcState.thpyObj);

	if(retVal == APP_ERR_NONE)
	{
		svcState.thpyObj.SetPressureSp(&svcState.thpyObj, req->targetPressure);
	}

	return retVal;
}

int8_t ThpyMgrStopTherapy(void)
{
	int8_t retVal;

	if(IsThpyInit(&svcState.thpyObj))
	{
		retVal = svcState.thpyObj.DeInit(&svcState.thpyObj);
	}

	return retVal;
}

int8_t ThpyMgrPauseTherapy(void)
{
	int8_t retVal;

	if(IsThpyInit(&svcState.thpyObj))
	{
		retVal = svcState.thpyObj.PauseThpy(&svcState.thpyObj);
	}

	return retVal;
}

int8_t ThpyMgrResumeTherapy(void)
{
	int8_t retVal;

	if(IsThpyInit(&svcState.thpyObj))
	{
		retVal = svcState.thpyObj.ResumeThpy(&svcState.thpyObj);
	}

	return retVal;
}

int8_t ThpyMgrGetTherapyStatus(COMM_TherapyStatMsg *msg)
{
	if(msg == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	msg->refPressure = svcState.snsrData.refPressure;
	msg->vacumCurrent = svcState.snsrData.vacuumCurrent;
	msg->vacumPressure = svcState.snsrData.vacuumPressure;
	msg->batteryV = svcState.snsrData.batV;
	SnsrMgrGetBatChrgSts(&msg->batteryChrgSts);
	msg->det = (svcState.snsrData.canDet) ? COMM_CANNISTER_DET_PRESENT : COMM_CANNISTER_DET_ABSENT;

	if(IsThpyInit(&svcState.thpyObj))
	{
		msg->type = svcState.thpyObj.type;
		msg->cmd = svcState.thpyObj.GetThpyCmd(&svcState.thpyObj);
		msg->targetPressure = svcState.thpyObj.GetPressureSp(&svcState.thpyObj);
		msg->fault = svcState.thpyObj.GetFaults(&svcState.thpyObj);
		msg->remTimeS = svcState.thpyObj.GetRemTime(&svcState.thpyObj);
		msg->leakPercent = svcState.thpyObj.GetLeakPercent(&svcState.thpyObj);
	}
	else
	{
		msg->type = COMM_THERAPY_TYPE_THPY_NONE;
		msg->cmd = COMM_THERAPY_CMD_STOP;
		msg->targetPressure = 0;
		msg->fault = COMM_THERAPY_FAULT_NONE;
		msg->remTimeS = 0;
		msg->leakPercent = 0;
	}

	return APP_ERR_NONE;
}

int8_t ThpyMgrRunIrrigation(COMM_IrrigationReqMsg *req)
{
	int8_t retVal = APP_ERR_INV_ARG;

	if(req == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	retVal = ThpyMgrStopIrrigation();

	svcState.irigObj.instillmL = req->instillmL;
	if(svcState.irigObj.instillmL > 500)
	{
		svcState.irigObj.instillmL = 500;
	}
	else if(svcState.irigObj.instillmL < 10)
	{
		svcState.irigObj.instillmL = 10;
	}

	/* TODO: Check if cannister is present or not */
	retVal = IrigMgrInit(&svcState.irigObj);

	return retVal;
}

int8_t ThpyMgrStopIrrigation(void)
{
	int8_t retVal = APP_ERR_NONE;

	if(IsIrigInit(&svcState.irigObj))
	{
		retVal = svcState.irigObj.DeInit(&svcState.irigObj);
	}

	return retVal;
}

int8_t ThpyMgrPauseIrrigation(void)
{
	int8_t retVal;

	if(IsIrigInit(&svcState.irigObj))
	{
		retVal = svcState.irigObj.PauseIrig(&svcState.irigObj);
	}

	return retVal;
}

int8_t ThpyMgrResumeIrrigation(void)
{
	int8_t retVal;

	if(IsIrigInit(&svcState.irigObj))
	{
		retVal = svcState.irigObj.ResumeIrig(&svcState.irigObj);
	}

	return retVal;
}

int8_t ThpyMgrGetIrrigationStatus(COMM_IrrigationStatMsg *msg)
{
	if(msg == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	msg->det = (svcState.snsrData.canDet) ? COMM_CANNISTER_DET_PRESENT : COMM_CANNISTER_DET_ABSENT;

	if(IsIrigInit(&svcState.irigObj))
	{
		msg->cmd = svcState.irigObj.GetIrigCmd(&svcState.irigObj);
		msg->dispensedmL = svcState.irigObj.GetDispensedmL(&svcState.irigObj);
		msg->fault = svcState.irigObj.GetFaults(&svcState.irigObj);
		msg->instillmL = svcState.irigObj.instillmL;
	}
	else
	{
		msg->cmd = COMM_THERAPY_CMD_STOP;
		msg->dispensedmL = 0;
		msg->fault = COMM_THERAPY_FAULT_NONE;
		msg->instillmL = 0;
	}

	return APP_ERR_NONE;
}

void ThpyMgrPrintDiags(void)
{
	if(svcState.taskHandle != NULL)
	{
		uint32_t space = uxTaskGetStackHighWaterMark(svcState.taskHandle) * sizeof(StackType_t);
		trice32(iD(5102), "dbg: diag: THPYMGR stack rem = %d \n", space);
	}
}
