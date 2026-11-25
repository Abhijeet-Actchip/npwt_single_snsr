/*
 * app_main.c
 *
 *  Created on: 24-Nov-2026
 *      Author: Abhijeet M
 */

#include "bsp/board.h"
#include "manualtests/testsuite.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_hal/app_hal_wdt.h"
//#include "thpy_mgr.h"
#include "app_mgr.h"
#include "diag_svc.h"

#include "../app_utils/assert/app_assert.h"

int main(void)
{
	int8_t retVal;

	retVal = Board_InitPeripherals();
	/* If peripheral initialization failed. */
	APP_ASSERT(retVal == APP_ERR_NONE);

	/* Run manual tests if any enabled. */
	ManualTestRunner();

	retVal = AppMgrInit();
	APP_ASSERT(retVal == APP_ERR_NONE);
//	retVal = ThpyMgrInit();
//	APP_ASSERT(retVal == APP_ERR_NONE);

	retVal = DiagSvcInit();
	APP_ASSERT(retVal == APP_ERR_NONE);

	/* BIST */

	APPHAL_WDT_Clear();

	/* Start scheduler */
	vTaskStartScheduler();
	/* We should never get here as control is now taken by the scheduler */
	APP_ASSERT(0);
}
