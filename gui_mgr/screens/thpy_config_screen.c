/*
 * thpy_config_screen.c
 *
 * Therapy configuration screen using simple LVGL button widgets.
 *
 * Navigation:
 *   UP / DOWN    : Move focus between configurable fields, START, and BACK.
 *   LEFT / RIGHT : Decrement / increment the focused pressure or time field.
 *   ENTER        : On START -> start therapy (UI_THPY_RUN_SCREEN).
 *                  On BACK  -> return to UI_THPY_SELECT_SCREEN.
 */

#include "thpy_config_screen.h"
#include "screenhandler.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(mono_16);
LV_FONT_DECLARE(mono_24);

#define THPY_PRESS_MAX_VAL_MMHG     (-40)
#define THPY_PRESS_MIN_VAL_MMHG     (-300)
#define THPY_PRESS_STEP_MMHG        5

#define THPY_TIME_MIN_SEC           30U
#define THPY_TIME_MAX_SEC           900U
#define THPY_TIME_STEP_SEC          30U

#define CFG_MAX_ITEMS               6U
#define CFG_ROW_X_PX                14
#define CFG_ROW_W_PX                (HORIZONTAL_RES - 28)
#define CFG_ROW_H_PX                26
#define CFG_ROW_START_Y_PX          50
#define CFG_ROW_STEP_Y_PX           32
#define CFG_ACTION_W_PX             116
#define CFG_ACTION_H_PX             38
#define CFG_ACTION_Y_PX             190

typedef enum
{
    CFG_ITEM_PRESSURE = 0,
    CFG_ITEM_MAX_PRESS,
    CFG_ITEM_MIN_PRESS_FIXED,
    CFG_ITEM_MIN_PRESS,
    CFG_ITEM_ON_TIME,
    CFG_ITEM_OFF_TIME,
    CFG_ITEM_START,
    CFG_ITEM_BACK,
} cfg_item_id_t;

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *titleLabel;
    lv_obj_t *dividerLine;
    lv_obj_t *itemObjs[CFG_MAX_ITEMS];
    lv_obj_t *itemLabels[CFG_MAX_ITEMS];
    cfg_item_id_t items[CFG_MAX_ITEMS];
    uint8_t itemCount;
} thpy_config_components_t;

static thpy_config_components_t cfgUi = { 0 };

static thpy_config_t thpyConfig = {
    .mode            = THPY_SEL_CONTINUOUS,
    .pressureMmHg    = -125,
    .maxPressureMmHg = -125,
    .minPressureMmHg = -80,
    .onTimeSec       = 300U,
    .offTimeSec      = 120U,
};

static const char *ModeTitle(thpy_sel_mode_t mode)
{
    switch(mode)
    {
        case THPY_SEL_CONTINUOUS: return "CONFIG: CONTINUOUS";
        case THPY_SEL_CYCLIC:     return "CONFIG: CYCLIC";
        case THPY_SEL_VARIABLE:   return "CONFIG: VARIABLE";
        default:                  return "CONFIG";
    }
}

static void FormatTime(uint16_t sec, char *buf, size_t len)
{
    uint16_t m = sec / 60U;
    uint16_t s = sec % 60U;
    snprintf(buf, len, "%02u:%02u", (unsigned)m, (unsigned)s);
}

static void RefreshItemText(uint8_t idx)
{
    char buf[40];
    char timeBuf[16];

    switch(cfgUi.items[idx])
    {
        case CFG_ITEM_PRESSURE:
            snprintf(buf, sizeof(buf), "Pressure    : %d mmHg", (int)thpyConfig.pressureMmHg);
            break;

        case CFG_ITEM_MAX_PRESS:
            snprintf(buf, sizeof(buf), "Max Press   : %d mmHg", (int)thpyConfig.maxPressureMmHg);
            break;

        case CFG_ITEM_MIN_PRESS_FIXED:
            snprintf(buf, sizeof(buf), "Min Press   : 0 mmHg (fixed)");
            break;

        case CFG_ITEM_MIN_PRESS:
            snprintf(buf, sizeof(buf), "Min Press   : %d mmHg", (int)thpyConfig.minPressureMmHg);
            break;

        case CFG_ITEM_ON_TIME:
            FormatTime(thpyConfig.onTimeSec, timeBuf, sizeof(timeBuf));
            snprintf(buf, sizeof(buf), "ON Time     : %s", timeBuf);
            break;

        case CFG_ITEM_OFF_TIME:
            FormatTime(thpyConfig.offTimeSec, timeBuf, sizeof(timeBuf));
            snprintf(buf, sizeof(buf), "OFF Time    : %s", timeBuf);
            break;

        case CFG_ITEM_START:
            snprintf(buf, sizeof(buf), "START");
            break;

        case CFG_ITEM_BACK:
            snprintf(buf, sizeof(buf), "BACK");
            break;

        default:
            buf[0] = '\0';
            break;
    }

    lv_label_set_text(cfgUi.itemLabels[idx], buf);
}

static int16_t StepPressure(int16_t val, int8_t dir)
{
    int16_t next = val - (dir * THPY_PRESS_STEP_MMHG);
    if(next < THPY_PRESS_MIN_VAL_MMHG)
    {
        next = THPY_PRESS_MIN_VAL_MMHG;
    }
    if(next > THPY_PRESS_MAX_VAL_MMHG)
    {
        next = THPY_PRESS_MAX_VAL_MMHG;
    }
    return next;
}

static uint16_t StepTime(uint16_t val, int8_t dir)
{
    int32_t next = (int32_t)val + (dir * (int32_t)THPY_TIME_STEP_SEC);
    if(next < (int32_t)THPY_TIME_MIN_SEC)
    {
        next = (int32_t)THPY_TIME_MIN_SEC;
    }
    if(next > (int32_t)THPY_TIME_MAX_SEC)
    {
        next = (int32_t)THPY_TIME_MAX_SEC;
    }
    return (uint16_t)next;
}

static void AdjustItemValue(uint8_t idx, int8_t dir)
{
    switch(cfgUi.items[idx])
    {
        case CFG_ITEM_PRESSURE:
            thpyConfig.pressureMmHg = StepPressure(thpyConfig.pressureMmHg, dir);
            RefreshItemText(idx);
            break;

        case CFG_ITEM_MAX_PRESS:
            thpyConfig.maxPressureMmHg = StepPressure(thpyConfig.maxPressureMmHg, dir);
            RefreshItemText(idx);
            break;

        case CFG_ITEM_MIN_PRESS:
            thpyConfig.minPressureMmHg = StepPressure(thpyConfig.minPressureMmHg, dir);
            RefreshItemText(idx);
            break;

        case CFG_ITEM_ON_TIME:
            thpyConfig.onTimeSec = StepTime(thpyConfig.onTimeSec, dir);
            RefreshItemText(idx);
            break;

        case CFG_ITEM_OFF_TIME:
            thpyConfig.offTimeSec = StepTime(thpyConfig.offTimeSec, dir);
            RefreshItemText(idx);
            break;

        default:
            break;
    }
}

static void ItemClickCb(lv_event_t *e)
{
    uint8_t idx = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    cfg_item_id_t id = cfgUi.items[idx];

    if(id == CFG_ITEM_START)
    {
        UISelectScreen(UI_THPY_RUN_SCREEN);
    }
    else if(id == CFG_ITEM_BACK)
    {
        UISelectScreen(UI_THPY_SELECT_SCREEN);
    }
    else
    {
        lv_group_t *grp = lv_group_get_default();
        if(grp != NULL)
        {
            lv_group_focus_next(grp);
        }
    }
}

static void ItemKeyCb(lv_event_t *e)
{
    uint8_t idx = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    uint32_t key = lv_event_get_key(e);
    lv_group_t *grp = lv_group_get_default();

    if(key == LV_KEY_DOWN)
    {
        if(grp != NULL)
        {
            lv_group_focus_next(grp);
        }
    }
    else if(key == LV_KEY_UP)
    {
        if(grp != NULL)
        {
            lv_group_focus_prev(grp);
        }
    }
    else if(key == LV_KEY_RIGHT)
    {
        AdjustItemValue(idx, 1);
    }
    else if(key == LV_KEY_LEFT)
    {
        AdjustItemValue(idx, -1);
    }
}

static void StyleFieldButton(lv_obj_t *btn)
{
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(btn, 6, LV_PART_MAIN);
    lv_obj_set_style_text_color(btn, lv_color_hex(0x1A1A1A), LV_PART_MAIN);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x005A9C), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x003D6B), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN | LV_STATE_FOCUSED);
}

static void StyleActionButton(lv_obj_t *btn, uint32_t focusColorHex)
{
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(focusColorHex), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 6, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(btn, lv_color_hex(0x1A1A1A), LV_PART_MAIN);

    lv_obj_set_style_bg_color(btn, lv_color_hex(focusColorHex), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN | LV_STATE_FOCUSED);
}

static void AddFieldRow(cfg_item_id_t id, bool editable, int16_t y)
{
    uint8_t idx = cfgUi.itemCount;
    lv_group_t *grp = lv_group_get_default();

    cfgUi.items[idx] = id;

    if(editable)
    {
        lv_obj_t *btn = lv_button_create(cfgUi.screenHandle);
        lv_obj_set_size(btn, CFG_ROW_W_PX, CFG_ROW_H_PX);
        lv_obj_set_pos(btn, CFG_ROW_X_PX, y);
        StyleFieldButton(btn);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_obj_set_style_text_font(lbl, &mono_16, LV_PART_MAIN);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);

        lv_obj_add_event_cb(btn, ItemClickCb, LV_EVENT_CLICKED, (void *)(uintptr_t)idx);
        lv_obj_add_event_cb(btn, ItemKeyCb, LV_EVENT_KEY, (void *)(uintptr_t)idx);

        if(grp != NULL)
        {
            lv_group_add_obj(grp, btn);
        }

        cfgUi.itemObjs[idx]   = btn;
        cfgUi.itemLabels[idx] = lbl;
    }
    else
    {
        lv_obj_t *lbl = lv_label_create(cfgUi.screenHandle);
        lv_obj_set_style_text_font(lbl, &mono_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_pos(lbl, CFG_ROW_X_PX + 6, y + 3);

        cfgUi.itemObjs[idx]   = lbl;
        cfgUi.itemLabels[idx] = lbl;
    }

    cfgUi.itemCount++;
    RefreshItemText(idx);
}

static void AddActionButton(cfg_item_id_t id, int16_t x, uint32_t focusColorHex)
{
    uint8_t idx = cfgUi.itemCount;
    lv_group_t *grp = lv_group_get_default();

    lv_obj_t *btn = lv_button_create(cfgUi.screenHandle);
    lv_obj_set_size(btn, CFG_ACTION_W_PX, CFG_ACTION_H_PX);
    lv_obj_set_pos(btn, x, CFG_ACTION_Y_PX);
    StyleActionButton(btn, focusColorHex);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_obj_set_style_text_font(lbl, &mono_16, LV_PART_MAIN);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, ItemClickCb, LV_EVENT_CLICKED, (void *)(uintptr_t)idx);
    lv_obj_add_event_cb(btn, ItemKeyCb, LV_EVENT_KEY, (void *)(uintptr_t)idx);

    if(grp != NULL)
    {
        lv_group_add_obj(grp, btn);
    }

    cfgUi.items[idx]      = id;
    cfgUi.itemObjs[idx]   = btn;
    cfgUi.itemLabels[idx] = lbl;
    cfgUi.itemCount++;

    RefreshItemText(idx);
}

void ShowThpyConfigScreen(void)
{
    CreateScreen();
    lv_obj_t *scr = GetActiveScreenHandle();
    cfgUi.screenHandle = scr;
    cfgUi.itemCount    = 0U;

    thpyConfig.mode = ThpySelectGetSelectedMode();
    if(thpyConfig.mode == THPY_SEL_CYCLIC)
    {
        thpyConfig.minPressureMmHg = 0;
    }

    cfgUi.titleLabel = lv_label_create(scr);
    lv_label_set_text(cfgUi.titleLabel, ModeTitle(thpyConfig.mode));
    lv_obj_set_style_text_font(cfgUi.titleLabel, &mono_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(cfgUi.titleLabel, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_pos(cfgUi.titleLabel, 14, 6);

    cfgUi.dividerLine = lv_obj_create(scr);
    lv_obj_set_size(cfgUi.dividerLine, HORIZONTAL_RES - 28, 2);
    lv_obj_set_pos(cfgUi.dividerLine, 14, 38);
    lv_obj_set_style_bg_color(cfgUi.dividerLine, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cfgUi.dividerLine, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(cfgUi.dividerLine, 0, LV_PART_MAIN);

    if(thpyConfig.mode == THPY_SEL_CONTINUOUS)
    {
        AddFieldRow(CFG_ITEM_PRESSURE, true, CFG_ROW_START_Y_PX);
    }
    else if(thpyConfig.mode == THPY_SEL_CYCLIC)
    {
        AddFieldRow(CFG_ITEM_MAX_PRESS,       true,  CFG_ROW_START_Y_PX);
        AddFieldRow(CFG_ITEM_MIN_PRESS_FIXED, false, CFG_ROW_START_Y_PX + CFG_ROW_STEP_Y_PX);
        AddFieldRow(CFG_ITEM_ON_TIME,         true,  CFG_ROW_START_Y_PX + (2 * CFG_ROW_STEP_Y_PX));
        AddFieldRow(CFG_ITEM_OFF_TIME,        true,  CFG_ROW_START_Y_PX + (3 * CFG_ROW_STEP_Y_PX));
    }
    else
    {
        AddFieldRow(CFG_ITEM_MAX_PRESS, true, CFG_ROW_START_Y_PX);
        AddFieldRow(CFG_ITEM_MIN_PRESS, true, CFG_ROW_START_Y_PX + CFG_ROW_STEP_Y_PX);
        AddFieldRow(CFG_ITEM_ON_TIME,   true, CFG_ROW_START_Y_PX + (2 * CFG_ROW_STEP_Y_PX));
        AddFieldRow(CFG_ITEM_OFF_TIME,  true, CFG_ROW_START_Y_PX + (3 * CFG_ROW_STEP_Y_PX));
    }

    AddActionButton(CFG_ITEM_START, 28,  0x1E8E3E);
    AddActionButton(CFG_ITEM_BACK,  176, 0x005A9C);

    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_focus_obj(cfgUi.itemObjs[0]);
    }
}

void ClearThpyConfigStateVar(void)
{
    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_remove_all_objs(grp);
    }
    memset(&cfgUi, 0, sizeof(thpy_config_components_t));
}

const thpy_config_t *ThpyConfigGet(void)
{
    return &thpyConfig;
}
