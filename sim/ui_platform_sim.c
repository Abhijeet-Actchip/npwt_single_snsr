/*
 * ui_platform_sim.c
 *
 * pthread implementation of the ui_platform abstraction layer.
 * Compiled only for the WSL2 simulator target.
 */

#include "app/ui_platform.h"
#include "app_middlewares/lvgl/lvgl.h"
#include "config/appconfig.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

/* --------------------------------------------------------------------------
 * Task
 * -------------------------------------------------------------------------- */

typedef struct
{
    void (*fn)(void *);
    void *arg;
} task_wrapper_t;

static void *TaskEntry(void *arg)
{
    task_wrapper_t *w = (task_wrapper_t *)arg;
    w->fn(w->arg);
    free(w);
    return NULL;
}

int8_t UIPlatform_CreateTask(void (*fn)(void *), const char *name,
                             uint32_t stackSize, void *arg,
                             uint32_t prio, ui_platform_task_t *handle)
{
    (void)name;
    (void)stackSize;
    (void)prio;

    task_wrapper_t *w = malloc(sizeof(task_wrapper_t));
    if(w == NULL)
    {
        return APP_ERR_NO_MEM;
    }

    w->fn  = fn;
    w->arg = arg;

    pthread_t tid;
    if(pthread_create(&tid, NULL, TaskEntry, w) != 0)
    {
        free(w);
        return APP_ERR_NO_MEM;
    }
    pthread_detach(tid);

    if(handle != NULL)
    {
        *handle = (ui_platform_task_t)(uintptr_t)tid;
    }

    return APP_ERR_NONE;
}

uint32_t UIPlatform_TaskGetStackWatermark(ui_platform_task_t handle)
{
    /* Stack high-water mark is not measurable on Linux pthreads. */
    (void)handle;
    return 0U;
}

/* --------------------------------------------------------------------------
 * Mutex
 * -------------------------------------------------------------------------- */

ui_platform_mutex_t UIPlatform_MutexCreate(void)
{
    pthread_mutex_t *m = malloc(sizeof(pthread_mutex_t));
    if(m == NULL)
    {
        return NULL;
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);

    return (ui_platform_mutex_t)m;
}

void UIPlatform_MutexLock(ui_platform_mutex_t m)
{
    pthread_mutex_lock((pthread_mutex_t *)m);
}

void UIPlatform_MutexUnlock(ui_platform_mutex_t m)
{
    pthread_mutex_unlock((pthread_mutex_t *)m);
}

/* --------------------------------------------------------------------------
 * Time
 * -------------------------------------------------------------------------- */

void UIPlatform_Delay(uint32_t ms)
{
    usleep((useconds_t)ms * 1000U);
}

void UIPlatform_TickInc(uint32_t ms)
{
    lv_tick_inc(ms);
}
