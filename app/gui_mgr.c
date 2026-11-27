/*
 * gui_mgr.c
 *
 *  Created on: Mar 5, 2025
 *      Author: Mahesh Murty
 */

#include "gui_mgr.h"
#include "ui_platform.h"
#include "app_middlewares/lvgl/lvgl.h"
#include "app_drivers/lv_port_st7789.h"
#include "bsp/board.h"
#include "app_utils/assert/app_assert.h"
#include <stdio.h>

#define GUI_REFRESH_DEL_MS			10
#define GUI_TICK_DEL_MS				10
/* Adjust if switches are active-high on hardware */
#define KEYPAD_SW_ACTIVE_STATE		false

typedef struct
{
	ui_platform_mutex_t lock;
	volatile bool isReady;
} gui_state_t;

typedef struct
{
	bool (*readFn)(void);
	uint32_t lvKey;
} keypad_map_t;

static ui_platform_task_t uiTaskHandle  = NULL;
static ui_platform_task_t tickTaskHandle = NULL;
static gui_state_t state = { 0 };

static const keypad_map_t keypadMap[] = {
	{ Board_ReadSwUP,    LV_KEY_UP    },
	{ Board_ReadSwDOWN,  LV_KEY_DOWN  },
	{ Board_ReadSwENTER, LV_KEY_ENTER },
	{ Board_ReadSwRIGHT, LV_KEY_RIGHT },
	{ Board_ReadSwLEFT,  LV_KEY_LEFT  },
};

static void KeypadReadCb(lv_indev_t *indev, lv_indev_data_t *data)
{
	static uint32_t lastKey = 0U;
	(void)indev;

	for(size_t i = 0U; i < (sizeof(keypadMap) / sizeof(keypadMap[0])); i++)
	{
		if(keypadMap[i].readFn() == KEYPAD_SW_ACTIVE_STATE)
		{
			lastKey = keypadMap[i].lvKey;
			data->key = lastKey;
			data->state = LV_INDEV_STATE_PRESSED;
			return;
		}
	}

	data->key = lastKey;
	data->state = LV_INDEV_STATE_RELEASED;
}

static void GuiTickTask(void *args)
{
	(void)args;
	while(1)
	{
		UIPlatform_TickInc(GUI_TICK_DEL_MS);
		UIPlatform_Delay(GUI_TICK_DEL_MS);
	}
}

static void GuiMgrTask(void *args)
{
	(void)args;
	int8_t retVal;

	lv_init();

	/* Initialize the display driver (creates display, buffers, and flush callbacks). */
	retVal = lv_disp_st7789_init();
	APP_ASSERT(retVal == APP_ERR_NONE);

	/* Configure DI1..DI5 switches as an LVGL keypad input device. */
	lv_indev_t *keypadIndev = lv_indev_create();
	APP_ASSERT(keypadIndev != NULL);
	lv_indev_set_type(keypadIndev, LV_INDEV_TYPE_KEYPAD);
	lv_indev_set_read_cb(keypadIndev, KeypadReadCb);

	lv_group_t *defaultGroup = lv_group_create();
	APP_ASSERT(defaultGroup != NULL);
	lv_group_set_default(defaultGroup);
	lv_indev_set_group(keypadIndev, defaultGroup);

	state.isReady = true;

	while(1)
	{
		GuiAcquireLock();
		lv_timer_handler();
		GuiReleaseLock();
		UIPlatform_Delay(GUI_REFRESH_DEL_MS);
	}
}

int8_t GuiMgrInit(void)
{
	int8_t status;

	state.lock = UIPlatform_MutexCreate();
	if(state.lock == NULL)
	{
		return APP_ERR_NO_MEM;
	}

	/* Create the GUI tick task. */
	status = UIPlatform_CreateTask(GuiTickTask, "GUITICK",
			GUI_TICK_STACK_SIZE, NULL,
			UI_PLATFORM_PRIO_HIGH, &tickTaskHandle);
	if(status != APP_ERR_NONE)
	{
		return APP_ERR_NO_MEM;
	}

	/* Create the GUI manager task. */
	status = UIPlatform_CreateTask(GuiMgrTask, "GUIMGR",
			GUI_MGR_STACK_SIZE, NULL,
			UI_PLATFORM_PRIO_LOW, &uiTaskHandle);
	if(status != APP_ERR_NONE)
	{
		return APP_ERR_NO_MEM;
	}

	return APP_ERR_NONE;
}

bool GuiMgrIsReady(void)
{
	return state.isReady;
}

void GuiAcquireLock(void)
{
	UIPlatform_MutexLock(state.lock);
}

void GuiReleaseLock(void)
{
	UIPlatform_MutexUnlock(state.lock);
}

void GuiMgrPrintDiags(void)
{
	uint32_t space;

	if(uiTaskHandle != NULL)
	{
		space = UIPlatform_TaskGetStackWatermark(uiTaskHandle);
		(void)space;
	}

	if(tickTaskHandle != NULL)
	{
		space = UIPlatform_TaskGetStackWatermark(tickTaskHandle);
		(void)space;
	}
}
