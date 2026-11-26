/*
 * ui_platform.h
 *
 * Platform-agnostic OS abstraction for the UI layer.
 * Provides a common API for tasks, mutexes, delay, and LVGL tick.
 *
 * Implementations:
 *   - app/ui_platform_embedded.c  : FreeRTOS (STM32 target)
 *   - sim/ui_platform_sim.c       : pthreads  (WSL2 simulator)
 */

#ifndef __UI_PLATFORM_H__
#define __UI_PLATFORM_H__		1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Platform-neutral task priority offsets (relative to idle priority).
 */
#define UI_PLATFORM_PRIO_LOW        1U
#define UI_PLATFORM_PRIO_HIGH       5U

/*
 * Opaque handle types. Each implementation casts to its native type
 * internally; callers treat them as void pointers.
 */
typedef void *ui_platform_task_t;
typedef void *ui_platform_mutex_t;

/* -------------------------------------------------------------------------
 * Task
 * ------------------------------------------------------------------------- */

/*!
 * \brief Creates a platform task (FreeRTOS task or pthread).
 *
 * \param fn        Task entry function.
 * \param name      Human-readable task name.
 * \param stackSize Stack size in words (embedded) / bytes (sim, unused).
 * \param arg       Argument passed to fn.
 * \param prio      Task priority.
 * \param handle    Output handle, may be NULL.
 * \return APP_ERR_NONE on success, APP_ERR_NO_MEM on failure.
 */
int8_t UIPlatform_CreateTask(void (*fn)(void *), const char *name,
                             uint32_t stackSize, void *arg,
                             uint32_t prio, ui_platform_task_t *handle);

/*!
 * \brief Returns the remaining stack high-water mark in bytes.
 *        Returns 0 on platforms that do not support this measurement.
 */
uint32_t UIPlatform_TaskGetStackWatermark(ui_platform_task_t handle);

/* -------------------------------------------------------------------------
 * Mutex (recursive)
 * ------------------------------------------------------------------------- */

/*!
 * \brief Creates a recursive mutex.
 * \return Mutex handle, or NULL on failure.
 */
ui_platform_mutex_t UIPlatform_MutexCreate(void);

/*!
 * \brief Acquires the mutex (blocks until available).
 */
void UIPlatform_MutexLock(ui_platform_mutex_t m);

/*!
 * \brief Releases the mutex.
 */
void UIPlatform_MutexUnlock(ui_platform_mutex_t m);

/* -------------------------------------------------------------------------
 * Time
 * ------------------------------------------------------------------------- */

/*!
 * \brief Blocks the calling task for the specified duration.
 * \param ms Duration in milliseconds.
 */
void UIPlatform_Delay(uint32_t ms);

/*!
 * \brief Advances the LVGL software timer by the given number of milliseconds.
 *        Called from the platform tick task/thread.
 * \param ms Elapsed time in milliseconds.
 */
void UIPlatform_TickInc(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __UI_PLATFORM_H__ */
