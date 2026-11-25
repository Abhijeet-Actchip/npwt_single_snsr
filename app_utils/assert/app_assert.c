/*
 * app_assert.c
 *
 *  Created on: 20-Sep-2023
 *      Author: Mahesh Murty
 */

#include "../../app_utils/assert/app_assert.h"

#include "FreeRTOS.h"
#include "task.h"
#include "bsp/board.h"
#include "app_middlewares/trice_deferred_sink.h"

void APP_ASSERT_DUMP_LOGS(void)
{
	if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
		vTaskSuspendAll();
	}
	/* DMA complete IRQs not required for polling TX, but keep IRQs on so
	 * SysTick/HAL_GetTick still advance for WriteBlocking timeouts. */
	portENABLE_INTERRUPTS();

//	TriceDeferredSinkPanicFlush(BOARD_PCCOMM_UART_PID);

	/* Hang until WDT reset (or debugger halt). */
	while (1) {
		TriceTransfer();
	}
}
