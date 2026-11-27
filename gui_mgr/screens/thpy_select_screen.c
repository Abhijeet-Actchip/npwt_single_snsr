/*
 * thpy_select_screen.c
 *
 * Displays 3 therapy mode buttons (CONTINUOUS, CYCLIC, VARIABLE).
 * UP/DOWN navigates between options, ENTER selects and opens thpy_config.
 */

#include "thpy_select_screen.h"
#include "screenhandler.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <string.h>

LV_FONT_DECLARE(mono_16);
LV_FONT_DECLARE(mono_24);

#define THPY_MODE_COUNT             3U
#define THPY_BTN_W_PX               240
#define THPY_BTN_H_PX               44
#define THPY_BTN_START_Y_PX         58
#define THPY_BTN_STEP_Y_PX          56

typedef struct
{
    const char *label;
    thpy_sel_mode_t mode;
} thpy_mode_item_t;

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *titleLabel;
    lv_obj_t *dividerLine;
    lv_obj_t *modeBtns[THPY_MODE_COUNT];
} thpy_select_components_t;

static const thpy_mode_item_t thpyModeItems[THPY_MODE_COUNT] = {
    { "CONTINUOUS", THPY_SEL_CONTINUOUS },
    { "CYCLIC",     THPY_SEL_CYCLIC     },
    { "VARIABLE",   THPY_SEL_VARIABLE   },
};

static thpy_select_components_t thpySelectComponents = { 0 };
static thpy_sel_mode_t lastSelectedMode = THPY_SEL_CONTINUOUS;

static void ModeBtnClickCb(lv_event_t *e)
{
    thpy_sel_mode_t mode = (thpy_sel_mode_t)(uintptr_t)lv_event_get_user_data(e);

    lastSelectedMode = mode;
    UISelectScreen(UI_THPY_CONFIG_SCREEN);
}

static void ModeBtnKeyCb(lv_event_t *e)
{
    uint32_t key = lv_event_get_key(e);
    lv_group_t *grp = lv_group_get_default();

    if(grp == NULL)
    {
        return;
    }

    if(key == LV_KEY_DOWN)
    {
        lv_group_focus_next(grp);
    }
    else if(key == LV_KEY_UP)
    {
        lv_group_focus_prev(grp);
    }
}

static void StyleActionButton(lv_obj_t *btn)
{
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 6, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(btn, lv_color_hex(0x1A1A1A), LV_PART_MAIN);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x005A9C), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x003D6B), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN | LV_STATE_FOCUSED);
}

static void UICreateContent(void)
{
    lv_obj_t *scr = thpySelectComponents.screenHandle;
    lv_group_t *grp = lv_group_get_default();

    thpySelectComponents.titleLabel = lv_label_create(scr);
    lv_label_set_text(thpySelectComponents.titleLabel, "SELECT THERAPY");
    lv_obj_set_style_text_font(thpySelectComponents.titleLabel, &mono_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(thpySelectComponents.titleLabel, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_pos(thpySelectComponents.titleLabel, 14, 6);

    thpySelectComponents.dividerLine = lv_obj_create(scr);
    lv_obj_set_size(thpySelectComponents.dividerLine, HORIZONTAL_RES - 28, 2);
    lv_obj_set_pos(thpySelectComponents.dividerLine, 14, 38);
    lv_obj_set_style_bg_color(thpySelectComponents.dividerLine, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thpySelectComponents.dividerLine, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(thpySelectComponents.dividerLine, 0, LV_PART_MAIN);

    for(uint8_t i = 0U; i < THPY_MODE_COUNT; i++)
    {
        int32_t yPos = THPY_BTN_START_Y_PX + ((int32_t)i * THPY_BTN_STEP_Y_PX);
        lv_obj_t *btn = lv_button_create(scr);

        lv_obj_set_size(btn, THPY_BTN_W_PX, THPY_BTN_H_PX);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, yPos);
        StyleActionButton(btn);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, thpyModeItems[i].label);
        lv_obj_set_style_text_font(lbl, &mono_16, LV_PART_MAIN);
        lv_obj_center(lbl);

        lv_obj_add_event_cb(btn, ModeBtnClickCb, LV_EVENT_CLICKED,
                            (void *)(uintptr_t)thpyModeItems[i].mode);
        lv_obj_add_event_cb(btn, ModeBtnKeyCb, LV_EVENT_KEY, NULL);

        if(grp != NULL)
        {
            lv_group_add_obj(grp, btn);
        }

        thpySelectComponents.modeBtns[i] = btn;
    }

    if(grp != NULL)
    {
        lv_group_focus_obj(thpySelectComponents.modeBtns[0]);
    }
}

void ShowThpySelectScreen(void)
{
    CreateScreen();
    thpySelectComponents.screenHandle = GetActiveScreenHandle();
    UICreateContent();
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
