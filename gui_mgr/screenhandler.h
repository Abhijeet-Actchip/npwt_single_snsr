/*
 * screenhandler.h
 *
 * Core screen management APIs for the NPWT UI.
 */

#ifndef __SCREENHANDLER_H__
#define __SCREENHANDLER_H__		1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "app_middlewares/lvgl/lvgl.h"
#include "config/appconfig.h"

#define HORIZONTAL_RES      DISP_HOR_RES
#define VERTICAL_RES        DISP_VER_RES

typedef enum
{
    UI_NO_SCREEN = 0,
    UI_BOOT_SCREEN,
    UI_THPY_SELECT_SCREEN,
    UI_THPY_CONFIG_SCREEN,
    UI_THPY_RUN_SCREEN,
    UI_MAX_SCREENS
} ui_screens_t;

typedef struct
{
    lv_obj_t *screenHandle;
} screen_handle_t;

void UIInit(void);
void UISelectScreen(ui_screens_t screenId);
void CreateScreen(void);
lv_obj_t *GetActiveScreenHandle(void);
ui_screens_t GetActiveScreenId(void);

#ifdef __cplusplus
}
#endif

#endif /* __SCREENHANDLER_H__ */
