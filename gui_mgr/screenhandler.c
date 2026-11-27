/*
 * screenhandler.c
 *
 * Manages active screen lifecycle and transitions across the 4 NPWT screens:
 * boot, thpy_select, thpy_config, and thpy_run.
 */

#include "screenhandler.h"
#include "app/gui_mgr.h"
#include "app_utils/assert/app_assert.h"
#include "screens/boot_screen.h"
#include "screens/thpy_select_screen.h"
#include "screens/thpy_config_screen.h"
#include "screens/thpy_run_screen.h"
#include <stddef.h>

static screen_handle_t activeScreenHandles = { 0 };
static ui_screens_t activeScreen = UI_NO_SCREEN;
static lv_style_t styleScreen;

static void DeleteActiveScreen(void);

static void DeleteActiveScreen(void)
{
    if(activeScreenHandles.screenHandle != NULL)
    {
        /* Delete asynchronously so transitions triggered inside button
         * callbacks do not free the active object mid-event. */
        lv_obj_delete_async(activeScreenHandles.screenHandle);
        activeScreenHandles.screenHandle = NULL;
    }
}

void UIInit(void)
{
    GuiAcquireLock();

    lv_style_init(&styleScreen);
    lv_style_set_radius(&styleScreen, 0);
    lv_style_set_border_width(&styleScreen, 0);
    lv_style_set_bg_color(&styleScreen, lv_color_white());
    lv_style_set_bg_opa(&styleScreen, LV_OPA_COVER);

    GuiReleaseLock();
}

void UISelectScreen(ui_screens_t screenId)
{
    if((activeScreen == screenId) || (screenId == UI_NO_SCREEN) || (screenId >= UI_MAX_SCREENS))
    {
        return;
    }

    GuiAcquireLock();

    if(activeScreen != UI_NO_SCREEN)
    {
        DeleteActiveScreen();

        switch(activeScreen)
        {
            case UI_BOOT_SCREEN:
                ClearBootStateVar();
                break;

            case UI_THPY_SELECT_SCREEN:
                ClearThpySelectStateVar();
                break;

            case UI_THPY_CONFIG_SCREEN:
                ClearThpyConfigStateVar();
                break;

            case UI_THPY_RUN_SCREEN:
                ClearThpyRunStateVar();
                break;

            default:
                APP_ASSERT(0);
                break;
        }
    }

    switch(screenId)
    {
        case UI_BOOT_SCREEN:
            ShowBootScreen();
            activeScreen = UI_BOOT_SCREEN;
            break;

        case UI_THPY_SELECT_SCREEN:
            ShowThpySelectScreen();
            activeScreen = UI_THPY_SELECT_SCREEN;
            break;

        case UI_THPY_CONFIG_SCREEN:
            ShowThpyConfigScreen();
            activeScreen = UI_THPY_CONFIG_SCREEN;
            break;

        case UI_THPY_RUN_SCREEN:
            ShowThpyRunScreen();
            activeScreen = UI_THPY_RUN_SCREEN;
            break;

        default:
            APP_ASSERT(0);
            break;
    }

    GuiReleaseLock();
}

lv_obj_t *GetActiveScreenHandle(void)
{
    return activeScreenHandles.screenHandle;
}

ui_screens_t GetActiveScreenId(void)
{
    ui_screens_t screenId;

    GuiAcquireLock();
    screenId = activeScreen;
    GuiReleaseLock();

    return screenId;
}

void CreateScreen(void)
{
    activeScreenHandles.screenHandle = lv_obj_create(NULL);
    APP_ASSERT(activeScreenHandles.screenHandle != NULL);

    lv_obj_add_style(activeScreenHandles.screenHandle, &styleScreen, LV_PART_MAIN);
    lv_obj_set_size(activeScreenHandles.screenHandle, HORIZONTAL_RES, VERTICAL_RES);
    lv_screen_load(activeScreenHandles.screenHandle);
}
