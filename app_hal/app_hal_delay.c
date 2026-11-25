/*
 * app_hal_delay.c
 *
 *  Created on: Oct 12, 2023
 *      Author: Mahesh Murty
 */

#include "app_hal_delay.h"
#include "bsp/board.h"
#include "FreeRTOS.h"
#include "task.h"

/*!
 * \brief Retrieves the current system tick count.
 *
 * This function returns the system tick count based on the current operating
 * system state. If the RTOS is running, it retrieves the tick count from
 * FreeRTOS. Otherwise, it uses the HAL tick count.
 *
 * \return Current tick count in milliseconds.
 */
uint32_t APPHAL_GetTick(void)
{
	uint32_t ticks;

	if(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
	{
		ticks = (uint32_t)pdTICKS_TO_MS(xTaskGetTickCount());
	}
	else
	{
		ticks = HAL_GetTick();
	}

	return ticks;
}

/*!
 * \brief Delays the execution for a specified duration in milliseconds.
 *
 * This function introduces a delay in the execution based on the current state
 * of the operating system. If the RTOS is running, it uses FreeRTOS vTaskDelay.
 * Otherwise, it relies on the HAL delay function.
 *
 * \param delayMs The duration of the delay in milliseconds.
 */
void APPHAL_Delay(uint32_t delayMs)
{
	/* taskSCHEDULER_SUSPENDED / NOT_STARTED → busy HAL delay (no vTaskDelay). */
	if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
	{
		vTaskDelay(pdMS_TO_TICKS(delayMs));
	}
	else
	{
		HAL_Delay(delayMs);
	}
}
