/*
 * ui_platform_embedded.c
 *
 * FreeRTOS implementation of the ui_platform abstraction layer.
 * Compiled only for the embedded STM32 target.
 */

#include "ui_platform.h"
#include "config/appconfig.h"
#include "app_hal/app_hal_delay.h"
#include "app_middlewares/lvgl/lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* --------------------------------------------------------------------------
 * Task
 * -------------------------------------------------------------------------- */

int8_t UIPlatform_CreateTask(void (*fn)(void *), const char *name,
                             uint32_t stackSize, void *arg,
                             uint32_t prio, ui_platform_task_t *handle)
{
    BaseType_t status = xTaskCreate((TaskFunction_t)fn, name,
                                    (uint16_t)stackSize, arg,
                                    (UBaseType_t)(tskIDLE_PRIORITY + prio),
                                    (TaskHandle_t *)handle);
    return (status == pdPASS) ? APP_ERR_NONE : APP_ERR_NO_MEM;
}

uint32_t UIPlatform_TaskGetStackWatermark(ui_platform_task_t handle)
{
    return (uint32_t)uxTaskGetStackHighWaterMark((TaskHandle_t)handle)
           * sizeof(StackType_t);
}

/* --------------------------------------------------------------------------
 * Mutex
 * -------------------------------------------------------------------------- */

ui_platform_mutex_t UIPlatform_MutexCreate(void)
{
    return (ui_platform_mutex_t)xSemaphoreCreateRecursiveMutex();
}

void UIPlatform_MutexLock(ui_platform_mutex_t m)
{
    BaseType_t status = xSemaphoreTakeRecursive((SemaphoreHandle_t)m,
                                                portMAX_DELAY);
    (void)status;
}

void UIPlatform_MutexUnlock(ui_platform_mutex_t m)
{
    xSemaphoreGiveRecursive((SemaphoreHandle_t)m);
}

/* --------------------------------------------------------------------------
 * Time
 * -------------------------------------------------------------------------- */

void UIPlatform_Delay(uint32_t ms)
{
    APPHAL_Delay(ms);
}

void UIPlatform_TickInc(uint32_t ms)
{
    lv_tick_inc(ms);
}
