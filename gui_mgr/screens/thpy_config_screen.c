/*
 * thpy_config_screen.c
 *
 * Character-display style therapy configuration screen.
 *
 * Navigation:
 *   Up / Down    : Move '>' between configurable fields, START, and BACK.
 *   Left / Right : Decrement / increment the active pressure or time field.
 *   Enter        : On START -> start therapy (UI_THPY_RUN_SCREEN).
 *                  On BACK  -> return to UI_THPY_SELECT_SCREEN.
 */

#include "thpy_config_screen.h"
#include "screenhandler.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(dot_matrix_26);
LV_FONT_DECLARE(dot_matrix);

#define THPY_PRESS_MAX_VAL_MMHG     (-40)
#define THPY_PRESS_MIN_VAL_MMHG     (-300)
#define THPY_PRESS_STEP_MMHG        5

#define THPY_TIME_MIN_SEC           30U
#define THPY_TIME_MAX_SEC           900U
#define THPY_TIME_STEP_SEC          30U

#define CFG_MAX_ITEMS               6U
#define CFG_CURSOR_X_PX             12
#define CFG_TEXT_X_PX               28
#define CFG_ROW_START_Y_PX          56
#define CFG_ROW_STEP_Y_PX           30
#define CFG_ACTION_Y_PX             202

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
    lv_obj_t *cursorLabel;
    lv_obj_t *itemLabels[CFG_MAX_ITEMS];
    cfg_item_id_t items[CFG_MAX_ITEMS];
    bool editable[CFG_MAX_ITEMS];
    int16_t posX[CFG_MAX_ITEMS];
    int16_t posY[CFG_MAX_ITEMS];
    uint8_t itemCount;
    uint8_t activeIdx;
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

static void UpdateCursor(void)
{
    uint8_t idx = cfgUi.activeIdx;
    lv_obj_set_pos(cfgUi.cursorLabel, cfgUi.posX[idx] - 16, cfgUi.posY[idx]);
}

static void MoveSelection(int8_t dir)
{
    uint8_t n = cfgUi.itemCount;
    for(uint8_t step = 0U; step < n; step++)
    {
        if(dir > 0)
        {
            cfgUi.activeIdx = (uint8_t)((cfgUi.activeIdx + 1U) % n);
        }
        else
        {
            cfgUi.activeIdx = (uint8_t)((cfgUi.activeIdx + n - 1U) % n);
        }

        if(cfgUi.editable[cfgUi.activeIdx])
        {
            break;
        }
    }
    UpdateCursor();
}

static int16_t StepPressure(int16_t val, int8_t dir)
{
    /* dir > 0 (Up): increase vacuum magnitude (-40 -> -300).
     * dir < 0 (Down): decrease vacuum magnitude (-300 -> -40). */
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

static void AdjustActiveValue(int8_t dir)
{
    uint8_t idx = cfgUi.activeIdx;

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

static void HandleEnter(void)
{
    cfg_item_id_t active = cfgUi.items[cfgUi.activeIdx];

    if(active == CFG_ITEM_START)
    {
        UISelectScreen(UI_THPY_RUN_SCREEN);
    }
    else if(active == CFG_ITEM_BACK)
    {
        UISelectScreen(UI_THPY_SELECT_SCREEN);
    }
    else
    {
        /* Advance cursor toward START when pressing Enter on a parameter */
        MoveSelection(1);
    }
}

static void ScreenInputCb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED)
    {
        HandleEnter();
        return;
    }

    if(code == LV_EVENT_KEY)
    {
        uint32_t key = lv_event_get_key(e);

        if(key == LV_KEY_DOWN)
        {
            MoveSelection(1);
        }
        else if(key == LV_KEY_UP)
        {
            MoveSelection(-1);
        }
        else if(key == LV_KEY_RIGHT)
        {
            AdjustActiveValue(1);
        }
        else if(key == LV_KEY_LEFT)
        {
            AdjustActiveValue(-1);
        }
    }
}

static void AddRowItem(cfg_item_id_t id, bool editable, int16_t x, int16_t y)
{
    uint8_t idx = cfgUi.itemCount;
    lv_obj_t *lbl = lv_label_create(cfgUi.screenHandle);

    lv_obj_set_style_text_font(lbl, &dot_matrix_26, LV_PART_MAIN);
    lv_obj_set_pos(lbl, x, y);

    cfgUi.items[idx]      = id;
    cfgUi.editable[idx]   = editable;
    cfgUi.posX[idx]       = x;
    cfgUi.posY[idx]       = y;
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
    cfgUi.activeIdx    = 0U;

    thpyConfig.mode = ThpySelectGetSelectedMode();
    if(thpyConfig.mode == THPY_SEL_CYCLIC)
    {
        thpyConfig.minPressureMmHg = 0;
    }

    cfgUi.titleLabel = lv_label_create(scr);
    lv_label_set_text(cfgUi.titleLabel, ModeTitle(thpyConfig.mode));
    lv_obj_set_style_text_font(cfgUi.titleLabel, &dot_matrix, LV_PART_MAIN);
    lv_obj_set_pos(cfgUi.titleLabel, 16, 12);

    cfgUi.cursorLabel = lv_label_create(scr);
    lv_label_set_text(cfgUi.cursorLabel, ">");
    lv_obj_set_style_text_font(cfgUi.cursorLabel, &dot_matrix_26, LV_PART_MAIN);

    if(thpyConfig.mode == THPY_SEL_CONTINUOUS)
    {
        AddRowItem(CFG_ITEM_PRESSURE, true, CFG_TEXT_X_PX, CFG_ROW_START_Y_PX);
    }
    else if(thpyConfig.mode == THPY_SEL_CYCLIC)
    {
        AddRowItem(CFG_ITEM_MAX_PRESS,       true,  CFG_TEXT_X_PX, CFG_ROW_START_Y_PX);
        AddRowItem(CFG_ITEM_MIN_PRESS_FIXED, false, CFG_TEXT_X_PX, CFG_ROW_START_Y_PX + CFG_ROW_STEP_Y_PX);
        AddRowItem(CFG_ITEM_ON_TIME,         true,  CFG_TEXT_X_PX, CFG_ROW_START_Y_PX + (2 * CFG_ROW_STEP_Y_PX));
        AddRowItem(CFG_ITEM_OFF_TIME,        true,  CFG_TEXT_X_PX, CFG_ROW_START_Y_PX + (3 * CFG_ROW_STEP_Y_PX));
    }
    else
    {
        AddRowItem(CFG_ITEM_MAX_PRESS, true, CFG_TEXT_X_PX, CFG_ROW_START_Y_PX);
        AddRowItem(CFG_ITEM_MIN_PRESS, true, CFG_TEXT_X_PX, CFG_ROW_START_Y_PX + CFG_ROW_STEP_Y_PX);
        AddRowItem(CFG_ITEM_ON_TIME,   true, CFG_TEXT_X_PX, CFG_ROW_START_Y_PX + (2 * CFG_ROW_STEP_Y_PX));
        AddRowItem(CFG_ITEM_OFF_TIME,  true, CFG_TEXT_X_PX, CFG_ROW_START_Y_PX + (3 * CFG_ROW_STEP_Y_PX));
    }

    AddRowItem(CFG_ITEM_START, true, 40,  CFG_ACTION_Y_PX);
    AddRowItem(CFG_ITEM_BACK,  true, 180, CFG_ACTION_Y_PX);

    UpdateCursor();

    lv_obj_add_event_cb(scr, ScreenInputCb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(scr, ScreenInputCb, LV_EVENT_CLICKED, NULL);

    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_add_obj(grp, scr);
        lv_group_focus_obj(scr);
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
