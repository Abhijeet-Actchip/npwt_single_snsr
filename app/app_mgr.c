/*
 * app_mgr.c
 *
 *  Created on: Feb 17, 2025
 *      Author: Mahesh Murty
 */

#include "app_mgr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp/board.h"
#include "snsr_mgr.h"
#include "thpy_mgr.h"
#include "app_hal/app_hal_delay.h"

#include "../app_utils/trice/trice.h"
#include "../utilities/eeprom_emulation/core/eeprom_emul_core.h"
#include "generated/utilities/mx_eeprom_emulation_app.h"

/* Enable this macro to clear the uvCutoff Flag everytime on boot */
#define ENABLE_FLASH_BIT_CLEAR			0
/* Enable the macro to Disable the power sequencing */
#define DISABLE_POWER_SEQUENCING		0

/* Note: Do Not Modify this address */
#define FLASH_VAR_ADDRESS			0x0001

#define POWER_SW_PRESS_THRESH			30			// 3 secs
#define POWER_JACK_DET_THRESH			20			// 2 secs

#define BATTERY_VOLTAGE_THRESH			9.2f
#define BATTERY_NOT_PRESENT_THRESH_MS	120

/* Calculated ADC Offset for Battery voltage */
#define BATTERY_ADC_OFFSET				0.055f
#define BATTERY_MAX_VOLTAGE				12.0
#define GET_BATTERY_PERCENT(x)			((x / BATTERY_MAX_VOLTAGE) * 100)

static TaskHandle_t taskHandle;
static uint8_t enFlashRead = 0;

static ee_object_t eeObj = {};
static int8_t Board_FlashRead(uint32_t *data);
static int8_t Board_FlashWrite(uint32_t data);

static int8_t BoardPowerCtrl(bool en)
{
	static int8_t state = -1;

	if(state == en)
		return APP_ERR_NONE;

	if(en)
	{
		trice(iD(4944), "dbg: APP-MGR : Board Power ON\n");
		/* Turn ON Device */
		/* Enable Display power */
//		Board_EnableDisplayPwr(1);
		Board_CtrlDisplayBacklight(1);
		APPHAL_Delay(100);
		Board_LedPwrOnCtrl(1);
	}
	else
	{
		trice(iD(5326), "dbg: APP-MGR : Board Power OFF\n");
		/* Turn OFF Device */
		Board_LedPwrOnCtrl(0);
		/* Disable Power to Display board */
		Board_EnableBoostConv(0);
//		Board_EnableDisplayPwr(0);
		Board_CtrlDisplayBacklight(0);
	}

	state = en;
	return APP_ERR_NONE;
}

//static int8_t PollPowerSw(uint8_t *deviceSt)
//{
//	static uint16_t swCtr;
//	uint8_t swPressed = 0;
//	uint32_t uvCutoffSensed = 1;
//
//	if(Board_PowerSwSensed())
//	{
//		if(swCtr < POWER_SW_PRESS_THRESH)
//		{
//			swCtr++;
//		}
//		else
//		{
//			swPressed = 1;
//			swCtr = 0;
//		}
//	}
//	else
//	{
//		swCtr = 0;
//	}
//
//	if(swPressed)
//	{
//		/* Read Flash and Allow Board Power Ctrl */
//		Board_FlashRead(&uvCutoffSensed);
//
//		if(uvCutoffSensed)
//		{
//			*deviceSt = !(*deviceSt);
//			BoardPowerCtrl(*deviceSt);
//		}
//
//		/* Wait till Switch is released */
//		while(Board_PowerSwSensed())
//			APPHAL_Delay(100);
//	}
//
//	return APP_ERR_NONE;
//}


static int8_t BatteryMonitoring(bool en)
{
	float batV = 0;

	/* Read Flash Here */
	uint32_t uvCutoffSensed = 1;
	Board_FlashRead(&uvCutoffSensed);
	trice32(iD(2337), "dbg: APP-MGR : uvCutoff State : %d\n", uvCutoffSensed);

	if(en && uvCutoffSensed)
	{
		/* Read Battery Voltage from ADC */
		SnsrMgrGetBatV(&batV);
		/* Subtract ADC offset from batV */
		batV -= BATTERY_ADC_OFFSET;

		if(batV < BATTERY_VOLTAGE_THRESH)
		{
			trice(iD(6275), "dbg: APP-MGR : Battery under cutOff voltage\n");
			Board_EnableBoostConv(0);
		    /* Store state(clear flag) in flash
			 * Do not turnON device until jack is plugged in again */
			Board_FlashWrite(0);
			enFlashRead = 1;
		}
		else
		{
			trice(iD(3714), "dbg: APP-MGR : Battery greater than cutOff voltage \n");
			/* Read Flash and Allow boost converter enable */
			Board_EnableBoostConv(1);
		}
	}

	return APP_ERR_NONE;
}

static int8_t PollPowerJack(uint8_t *prevState, uint8_t *status)
{
	static uint8_t jackCtr = 0;
	uint8_t curState = 0; //= Board_PowerJackDet();
	uint32_t uvCutoffSensed = 1;

	if(curState == *prevState)
	{
		jackCtr++;
	}
	else
	{
		jackCtr = 1;
		*prevState = curState;
	}

	if(jackCtr > POWER_JACK_DET_THRESH)
	{
		jackCtr = 0;
		/* Jack Detected */

		/* Read flash uvCutoffState */
		Board_FlashRead(&uvCutoffSensed);

		if(curState)
		{
			if(*status || (!uvCutoffSensed))
			{
				trice(iD(1183), "dbg: APP-MGR : Jack Detected\n");
				*status = 0;
				Board_EnableBoostConv(0);

				/* Set Flash variable */
				Board_FlashWrite(1);
			}
		}
		else		/* Jack Not Detected */
		{
			if(!(*status))
			{
				trice(iD(7697), "dbg: APP-MGR : Jack not Detected\n");
				*status = 1;
			}
		}
	}
	return APP_ERR_NONE;
}

static int8_t Board_EEInit(void)
{
	ee_status eeStatus = EE_OK;
	eeObj.crc_object = EEPROM_EMULATION_CRC_HANDLE();
	eeObj.f_object = EEPROM_EMULATION_FLASH_HANDLE();
	eeObj.time_object = NULL;

	/* Unlock the Flash Program Erase controller */
	if(HAL_FLASH_ITF_Unlock(HAL_FLASH) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	eeStatus = EE_Init(&eeObj, EE_FORCED_ERASE);

	/* Lock the Flash Program Erase controller */
	if(HAL_FLASH_ITF_Lock(HAL_FLASH) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	if(eeStatus != EE_OK)
	{
		return APP_ERR_STHAL;
	}

	return APP_ERR_NONE;
}

static int8_t Board_FlashRead(uint32_t *data)
{
	ee_status eeStatus = EE_OK;
	uint16_t readAddress = FLASH_VAR_ADDRESS;

	if(data == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	if(HAL_FLASH_ITF_Unlock(HAL_FLASH) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	eeStatus = EE_ReadVariable32bits(readAddress, data);

	if(HAL_FLASH_ITF_Lock(HAL_FLASH) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	if(eeStatus != EE_OK)
	{
		return APP_ERR_STHAL;
	}

	return APP_ERR_NONE;
}

static int8_t Board_FlashWrite(uint32_t data)
{
	ee_status eeStatus = EE_OK;
	uint16_t writeAddress = FLASH_VAR_ADDRESS;

	if(HAL_FLASH_ITF_Unlock(HAL_FLASH) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	eeStatus = EE_WriteVariable32bits(writeAddress, data);

	if(HAL_FLASH_ITF_Lock(HAL_FLASH) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	if(eeStatus != EE_OK)
	{
		return APP_ERR_STHAL;
	}

	return APP_ERR_NONE;
}

/*
 * Take one flag, store it in flash
 * If flag == 1,
 * * Enable power Ctrl, on Switch press.
 * * Flag is cleared(flag = 0) when, batV < Threshold.
 * If flag == 0
 * * Disable board power ctrl, on switch press.
 * * Don't Turn ON device
 * * Poll for jack detection, to set(flag = 1) the flag, again.
 */
static void AppMgrTask(void *args)
{
	uint8_t isDeviceOn = 0;
	uint8_t isRunningOnBat = 0;
	uint8_t tempJackSt = 0;			// To Store Temp jack state

	/* Enable battery Voltage divider */
	Board_EnableBatVDiv(1);

	/* Initialize EEPROM Emulation */
	Board_EEInit();

	/* Read once during boot */
	uint32_t uvCutoffSensed = 1;

#if(ENABLE_FLASH_BIT_CLEAR == 1)
	Board_FlashWrite(0);
#endif

#if(DISABLE_POWER_SEQUENCING == 1)
	bool doOnce = true;
#endif

	while(1)
	{
#if(DISABLE_POWER_SEQUENCING == 1)
		if(doOnce)
		{
			doOnce = false;
			BoardPowerCtrl(1);
		}
#else
		if(enFlashRead)
		{
			Board_FlashRead(&uvCutoffSensed);
			isDeviceOn = uvCutoffSensed;
			trice32(iD(3951), "dbg: APP-MGR : Flash flag update, uvCutOff state : %d\n",isDeviceOn);
			enFlashRead = 0;
			BoardPowerCtrl(isDeviceOn);
		}
//		PollPowerSw(&isDeviceOn);
		PollPowerJack(&tempJackSt, &isRunningOnBat);
		BatteryMonitoring(isRunningOnBat);
#endif
		APPHAL_Delay(100);
	}
}

int8_t AppMgrInit(void)
{
	BaseType_t status;

	/* Create the therapy manager task. */
	status = xTaskCreate(AppMgrTask, "APPMGR",
			APP_MGR_STACK_SIZE, NULL,
			APP_MGR_PRIORITY, &taskHandle);
	if(status != pdPASS)
	{
		return APP_ERR_NO_MEM;
	}

	return APP_ERR_NONE;
}

void AppMgrPrintDiags(void)
{
	if(taskHandle != NULL)
	{
		uint32_t space = uxTaskGetStackHighWaterMark(taskHandle) * sizeof(StackType_t);
		trice32(iD(2570), "dbg: diag: APPMGR stack rem = %d \n", space);
	}
}
