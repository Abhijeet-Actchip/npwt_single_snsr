/*
 * thpy_select_screen.c
 *
 * Character-display style therapy mode selection screen.
 * Displays 3 therapy modes (CONTINUOUS, CYCLIC, VARIABLE) with '>' beside
 * the active selection. Up/Down (or Left/Right) moves '>', Enter advances
 * to thpy_config.
 */

#include "thpy_select_screen.h"
#include "screenhandler.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <string.h>

LV_FONT_DECLARE(dot_matrix_26);
LV_FONT_DECLARE(dot_matrix);

#define THPY_MODE_COUNT             3U
#define THPY_ROW_START_Y_PX         65
#define THPY_ROW_STEP_Y_PX          35
#define THPY_CURSOR_X_PX            16
#define THPY_TEXT_X_PX              34

typedef struct
{
    const char *label;
    thpy_sel_mode_t mode;
} thpy_mode_item_t;

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *titleLabel;
    lv_obj_t *cursorLabel;
    lv_obj_t *modeLabels[THPY_MODE_COUNT];
    uint8_t activeIdx;
} thpy_select_components_t;

static const thpy_mode_item_t thpyModeItems[THPY_MODE_COUNT] = {
    { "CONTINUOUS", THPY_SEL_CONTINUOUS },
    { "CYCLIC",     THPY_SEL_CYCLIC     },
    { "VARIABLE",   THPY_SEL_VARIABLE   },
};

static thpy_select_components_t thpySelectComponents = { 0 };
static thpy_sel_mode_t lastSelectedMode = THPY_SEL_CONTINUOUS;

static void UpdateCursorPosition(void)
{
    int32_t yPos = THPY_ROW_START_Y_PX + ((int32_t)thpySelectComponents.activeIdx * THPY_ROW_STEP_Y_PX);
    lv_obj_set_pos(thpySelectComponents.cursorLabel, THPY_CURSOR_X_PX, yPos);
}

static void SelectCurrentMode(void)
{
    lastSelectedMode = thpyModeItems[thpySelectComponents.activeIdx].mode;
    UISelectScreen(UI_THPY_CONFIG_SCREEN);
}

static void ScreenInputCb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED)
    {
        SelectCurrentMode();
        return;
    }

    if(code == LV_EVENT_KEY)
    {
        uint32_t key = lv_event_get_key(e);

        if(key == LV_KEY_DOWN)
        {
            thpySelectComponents.activeIdx = (uint8_t)((thpySelectComponents.activeIdx + 1U) % THPY_MODE_COUNT);
            UpdateCursorPosition();
        }
        else if(key == LV_KEY_UP)
        {
            thpySelectComponents.activeIdx = (uint8_t)((thpySelectComponents.activeIdx + THPY_MODE_COUNT - 1U) % THPY_MODE_COUNT);
            UpdateCursorPosition();
        }
    }
}

static void UICreateCharacterMenu(void)
{
    lv_obj_t *scr = thpySelectComponents.screenHandle;

    thpySelectComponents.titleLabel = lv_label_create(scr);
    lv_label_set_text(thpySelectComponents.titleLabel, "SELECT THERAPY");
    lv_obj_set_style_text_font(thpySelectComponents.titleLabel, &dot_matrix, LV_PART_MAIN);
    lv_obj_set_pos(thpySelectComponents.titleLabel, 16, 14);

    thpySelectComponents.cursorLabel = lv_label_create(scr);
    lv_label_set_text(thpySelectComponents.cursorLabel, ">");
    lv_obj_set_style_text_font(thpySelectComponents.cursorLabel, &dot_matrix_26, LV_PART_MAIN);

    for(uint8_t i = 0U; i < THPY_MODE_COUNT; i++)
    {
        int32_t yPos = THPY_ROW_START_Y_PX + ((int32_t)i * THPY_ROW_STEP_Y_PX);
        lv_obj_t *lbl = lv_label_create(scr);

        lv_label_set_text(lbl, thpyModeItems[i].label);
        lv_obj_set_style_text_font(lbl, &dot_matrix_26, LV_PART_MAIN);
        lv_obj_set_pos(lbl, THPY_TEXT_X_PX, yPos);

        thpySelectComponents.modeLabels[i] = lbl;
    }

    thpySelectComponents.activeIdx = 0U;
    UpdateCursorPosition();

    lv_obj_add_event_cb(scr, ScreenInputCb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(scr, ScreenInputCb, LV_EVENT_CLICKED, NULL);

    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_add_obj(grp, scr);
        lv_group_focus_obj(scr);
    }
}

void ShowThpySelectScreen(void)
{
    CreateScreen();
    thpySelectComponents.screenHandle = GetActiveScreenHandle();
    UICreateCharacterMenu();
}

void ClearThpySelectStateVar(void)
{
    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_remove_all_objs(grp);
    }
    memset(&thpySelectComponents, 0, sizeof(thpy_select_components_t));
}

thpy_sel_mode_t ThpySelectGetSelectedMode(void)
{
    return lastSelectedMode;
}
