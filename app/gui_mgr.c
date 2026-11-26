/*
 * gui_mgr.c
 *
 *  Created on: Mar 5, 2025
 *      Author: Mahesh Murty
 */

#include "gui_mgr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "bsp/board.h"
#include "app_hal/app_hal_delay.h"
#include "app_middlewares/lvgl/lvgl.h"
#include "app_drivers/lv_port_st7789.h"
#include "app_utils/trice/trice.h"
#include "app_utils/assert/app_assert.h"
#include <stdio.h>

#define GUI_REFRESH_DEL_MS			20
#define GUI_TICK_DEL_MS				10
/* Adjust if switches are active-high on hardware */
#define KEYPAD_SW_ACTIVE_STATE		false

typedef struct _gui_state_t
{
	SemaphoreHandle_t lock;
	screen_id_t currScr;
	
	lv_obj_t *labTitle1;
	lv_obj_t *labTitle2;
	lv_obj_t *labTitle3;
	
	lv_obj_t *labVal1;
	lv_obj_t *labVal2;
	lv_obj_t *labVal3;
}gui_state_t;

static TaskHandle_t uiTaskHandle = NULL;
static TaskHandle_t tickTaskHandle = NULL;

static gui_state_t state = {
		.currScr = UI_NO_SCREEN
};

typedef struct _keypad_map_t
{
	bool (*readFn)(void);
	uint32_t lvKey;
} keypad_map_t;

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
	while(1)
	{
		lv_tick_inc(GUI_TICK_DEL_MS);
		APPHAL_Delay(GUI_TICK_DEL_MS);
	}
}

static void GuiMgrTask(void *args)
{
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

	while(1)
	{
		/* NOTE: lv_lock/lv_unlock is called internally */
		lv_timer_handler();
		APPHAL_Delay(GUI_REFRESH_DEL_MS);
	}
}

int8_t GuiMgrInit(void)
{
	BaseType_t status;

	state.lock = xSemaphoreCreateRecursiveMutex();
	if(state.lock == NULL)
	{
		return APP_ERR_NO_MEM;
	}

	/* Create the GUI tick task. */
	status = xTaskCreate(GuiTickTask, "GUITICK",
			GUI_TICK_STACK_SIZE, NULL,
			GUI_TICK_PRIORITY, &tickTaskHandle);
	if(status != pdPASS)
	{
		return APP_ERR_NO_MEM;
	}

	/* Create the GUI manager task. */
	status = xTaskCreate(GuiMgrTask, "GUIMGR",
			GUI_MGR_STACK_SIZE, NULL,
			GUI_MGR_PRIORITY, &uiTaskHandle);
	if(status != pdPASS)
	{
		return APP_ERR_NO_MEM;
	}

	return APP_ERR_NONE;
}

static void AcquireLock(void)
{
	BaseType_t status;
	status = xSemaphoreTake(state.lock, pdMS_TO_TICKS(1000));
	APP_ASSERT(status == pdPASS);
}

static void ReleaseLock(void)
{
	xSemaphoreGive(state.lock);
}

int8_t GuiMgrShowScr(screen_id_t id)
{
	AcquireLock();
	
	if((id == UI_NO_SCREEN) || (id >= UI_MAX_SCREENS))
	{
		ReleaseLock();
		return APP_ERR_INV_ARG;
	}

	if(state.currScr != id)
	{
		lv_lock();

		/* Should enter only first time here. */
		if(state.currScr == UI_NO_SCREEN)
		{
			/* Allocate labels. */
			state.labTitle1 = lv_label_create(lv_scr_act());
			state.labTitle2 = lv_label_create(lv_scr_act());
			state.labTitle3 = lv_label_create(lv_scr_act());
			state.labVal1 = lv_label_create(lv_scr_act());
			state.labVal2 = lv_label_create(lv_scr_act());
			state.labVal3 = lv_label_create(lv_scr_act());

			lv_obj_set_style_text_font(state.labTitle1, LV_FONT_DEFAULT_MONTSERRAT_14, LV_PART_MAIN);
			lv_obj_set_style_text_font(state.labTitle2, LV_FONT_DEFAULT_MONTSERRAT_14, LV_PART_MAIN);
			lv_obj_set_style_text_font(state.labTitle3, LV_FONT_DEFAULT_MONTSERRAT_14, LV_PART_MAIN);
			lv_obj_set_style_text_font(state.labVal1, LV_FONT_DEFAULT_MONTSERRAT_14, LV_PART_MAIN);
			lv_obj_set_style_text_font(state.labVal2, LV_FONT_DEFAULT_MONTSERRAT_14, LV_PART_MAIN);
			lv_obj_set_style_text_font(state.labVal3, LV_FONT_DEFAULT_MONTSERRAT_14, LV_PART_MAIN);
		}
		
		switch (id)
		{
			case UI_DO_SCREEN:
				/* Show hidden labels. */
//				lv_obj_clear_flag(state.labTitle3, LV_OBJ_FLAG_HIDDEN);
//				lv_obj_clear_flag(state.labVal3, LV_OBJ_FLAG_HIDDEN);
				lv_label_set_text(state.labTitle1, "DO");
				lv_label_set_text(state.labTitle2, "PH");
				lv_label_set_text(state.labTitle3, "SALINITY");
				lv_obj_set_pos(state.labTitle1, 2, 30);
				lv_obj_set_pos(state.labTitle2, 2, 50);
				lv_obj_set_pos(state.labTitle3, 2, 70);
				break;
			case UI_CO2_SCREEN:
				/* Show hidden labels. */
//				lv_obj_clear_flag(state.labTitle3, LV_OBJ_FLAG_HIDDEN);
//				lv_obj_clear_flag(state.labVal3, LV_OBJ_FLAG_HIDDEN);
				lv_label_set_text(state.labTitle1, "CO2");
				lv_label_set_text(state.labTitle2, "TURBIDITY");
				lv_label_set_text(state.labTitle3, "CURRENT");
				lv_obj_set_pos(state.labTitle1, 2, 30);
				lv_obj_set_pos(state.labTitle2, 2, 50);
				lv_obj_set_pos(state.labTitle3, 2, 70);
				break;
			case UI_TEMP_SCREEN:
				/* Hide 2 labels. */
//				lv_obj_add_flag(state.labTitle3, LV_OBJ_FLAG_HIDDEN);
//				lv_obj_add_flag(state.labVal3, LV_OBJ_FLAG_HIDDEN);

				lv_label_set_text(state.labTitle1, "PRESSURE");
				lv_label_set_text(state.labTitle2, "TEMP");
				lv_obj_set_pos(state.labTitle1, 2, 30);
				lv_obj_set_pos(state.labTitle2, 2, 50);
				break;
			default:
				APP_ASSERT(0);
				break;
		}

		lv_unlock();

		state.currScr = id;
	}

	ReleaseLock();

	return APP_ERR_NONE;
}

int8_t GuiMgrShowNextScr(void)
{
	AcquireLock();
	uint8_t nextScr = state.currScr + 1;
	if(nextScr >= UI_MAX_SCREENS)
	{
		nextScr = UI_MAX_SCREENS - 1;
	}
	ReleaseLock();

	return GuiMgrShowScr(nextScr);
}

int8_t GuiMgrShowPrevScr(void)
{
	AcquireLock();
	int8_t prevScr = state.currScr - 1;
	if(prevScr <= UI_NO_SCREEN)
	{
		prevScr = UI_NO_SCREEN + 1;
	}
	ReleaseLock();

	return GuiMgrShowScr(prevScr);
}

//int8_t GuiMgrUpdateData(app_data_t *pData)
//{
//	if(pData == NULL)
//	{
//		return APP_ERR_INV_ARG;
//	}
//
//	AcquireLock();
//
//	char printBuff[16];
//
//	switch(state.currScr)
//	{
//		case UI_DO_SCREEN:
//			snprintf(printBuff, sizeof(printBuff), "%0.1f", pData->dOxygen);
//			lv_label_set_text(state.labVal1, printBuff);
//			snprintf(printBuff, sizeof(printBuff), "%0.1f", pData->ph);
//			lv_label_set_text(state.labVal2, printBuff);
//			snprintf(printBuff, sizeof(printBuff), "%0.1f", pData->salinity);
//			lv_label_set_text(state.labVal3, printBuff);
//			lv_obj_set_pos(state.labVal1, 85, 30);
//			lv_obj_set_pos(state.labVal2, 85, 50);
//			lv_obj_set_pos(state.labVal3, 85, 70);
//			break;
//		case UI_CO2_SCREEN:
//			snprintf(printBuff, sizeof(printBuff), "%d", (int)pData->co2);
//			lv_label_set_text(state.labVal1, printBuff);
//			snprintf(printBuff, sizeof(printBuff), "%d", (int)pData->turbidity);
//			lv_label_set_text(state.labVal2, printBuff);
//			snprintf(printBuff, sizeof(printBuff), "%d", (int)pData->current);
//			lv_label_set_text(state.labVal3, printBuff);
//			lv_obj_set_pos(state.labVal1, 90, 30);
//			lv_obj_set_pos(state.labVal2, 90, 50);
//			lv_obj_set_pos(state.labVal3, 90, 70);
//			break;
//		case UI_TEMP_SCREEN:
//			snprintf(printBuff, sizeof(printBuff), "%0.1f", pData->barPressure);
//			lv_label_set_text(state.labVal1, printBuff);
//			snprintf(printBuff, sizeof(printBuff), "%0.1f", pData->temp);
//			lv_label_set_text(state.labVal2, printBuff);
//			lv_obj_set_pos(state.labVal1, 85, 30);
//			lv_obj_set_pos(state.labVal2, 85, 50);
//			break;
//		default:
//			/* Invalid screen. */
//			break;
//	}
//
//	ReleaseLock();
//
//	return APP_ERR_NONE;
//}

void GuiMgrPrintDiags(void)
{
	if(uiTaskHandle != NULL)
	{
		uint32_t space = uxTaskGetStackHighWaterMark(uiTaskHandle) * sizeof(StackType_t);
		trice32(iD(7110), "dbg: diag: GUIMGR stack rem = %d \n", space);
	}

	if(tickTaskHandle != NULL)
	{
		uint32_t space = uxTaskGetStackHighWaterMark(tickTaskHandle) * sizeof(StackType_t);
		trice32(iD(6806), "dbg: diag: GUITICK stack rem = %d \n", space);
	}

	/* TODO: Monitor UI heap memory. */

//	lv_mem_monitor_t mon;
//	GuiAcquireLock();
//	lv_mem_monitor(&mon);
//	GuiReleaseLock();
//
//	ESP_LOGI(TAG, "used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)mon.total_size - mon.free_size,
//		mon.used_pct,
//		mon.frag_pct,
//		(int)mon.free_biggest_size);
}
