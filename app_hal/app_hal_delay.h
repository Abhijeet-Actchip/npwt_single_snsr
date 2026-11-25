/*
 * app_hal_delay.h
 *
 *  Created on: Oct 12, 2023
 *      Author: Mahesh Murty
 */

#ifndef __APP_HAL_DELAY_H__
#define __APP_HAL_DELAY_H__				1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/*!
 * \brief Retrieves the current system tick count.
 *
 * Fetches the current time in milliseconds from FreeRTOS when the scheduler
 * is running, otherwise from the HAL tick.
 *
 * \return The current system tick count in milliseconds.
 */
uint32_t APPHAL_GetTick(void);
/*!
 * \brief Delays the execution for a specified duration in milliseconds.
 *
 * Uses FreeRTOS vTaskDelay when the scheduler is running, otherwise HAL_Delay.
 *
 * \param delayMs The duration of the delay in milliseconds.
 */
void APPHAL_Delay(uint32_t delayMs);

#ifdef __cplusplus
}
#endif

#endif /* __APP_HAL_DELAY_H__ */
