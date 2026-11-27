/*
 * thpy_run_screen.c
 *
 * Therapy running screen using simple LVGL button widgets for PAUSE/RESUME and STOP.
 * UP/DOWN moves focus between PAUSE/RESUME and STOP, ENTER executes.
 */

#include "thpy_run_screen.h"
#include "thpy_config_screen.h"
#include "screenhandler.h"
#include "app/gui_mgr.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(mono_16);
LV_FONT_DECLARE(mono_24);

#define RUN_TIMER_PERIOD_MS         1000U
#define RUN_BTN_W_PX                116
#define RUN_BTN_H_PX                38
#define RUN_ACTION_Y_PX             186
#define RUN_PAUSE_X_PX              28
#define RUN_STOP_X_PX               176

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *statusLabel;
    lv_obj_t *dividerLine;
    lv_obj_t *setPressLabel;
    lv_obj_t *achPressLabel;
    lv_obj_t *elapsedLabel;
    lv_obj_t *pauseBtn;
    lv_obj_t *pauseLabel;
    lv_obj_t *stopBtn;
    lv_timer_t *elapsedTimer;
    bool isPaused;
    uint32_t elapsedSec;
    int16_t achievedPressureMmHg;
} thpy_run_components_t;

static thpy_run_components_t runUi = { 0 };

static const char *ModeName(thpy_sel_mode_t mode)
{
    switch(mode)
    {
        case THPY_SEL_CONTINUOUS: return "CONTINUOUS";
        case THPY_SEL_CYCLIC:     return "CYCLIC";
        case THPY_SEL_VARIABLE:   return "VARIABLE";
        default:                  return "THERAPY";
    }
}

static void RefreshDynamicLabels(void)
{
    const thpy_config_t *cfg = ThpyConfigGet();
    char buf[48];

    snprintf(buf, sizeof(buf), "%s : %s",
             ModeName(cfg->mode),
             runUi.isPaused ? "PAUSED" : "RUNNING");
    lv_label_set_text(runUi.statusLabel, buf);

    if(cfg->mode == THPY_SEL_CONTINUOUS)
    {
        snprintf(buf, sizeof(buf), "Set Press   : %d mmHg", (int)cfg->pressureMmHg);
    }
    else
    {
        snprintf(buf, sizeof(buf), "Set Press   : %d / %d mmHg",
                 (int)cfg->maxPressureMmHg, (int)cfg->minPressureMmHg);
    }
    lv_label_set_text(runUi.setPressLabel, buf);

    snprintf(buf, sizeof(buf), "Achieved    : %d mmHg", (int)runUi.achievedPressureMmHg);
    lv_label_set_text(runUi.achPressLabel, buf);

    uint32_t mins = runUi.elapsedSec / 60U;
    uint32_t secs = runUi.elapsedSec % 60U;
    snprintf(buf, sizeof(buf), "Elapsed     : %02u:%02u", (unsigned)mins, (unsigned)secs);
    lv_label_set_text(runUi.elapsedLabel, buf);

    lv_label_set_text(runUi.pauseLabel, runUi.isPaused ? "RESUME" : "PAUSE");
}

static void ElapsedTimerCb(lv_timer_t *timer)
{
    (void)timer;
    if(!runUi.isPaused)
    {
        runUi.elapsedSec++;
        RefreshDynamicLabels();
    }
}

static void PauseBtnClickCb(lv_event_t *e)
{
    (void)e;
    runUi.isPaused = !runUi.isPaused;
    RefreshDynamicLabels();
}

static void StopBtnClickCb(lv_event_t *e)
{
    (void)e;
    UISelectScreen(UI_THPY_CONFIG_SCREEN);
}

static void ActionBtnKeyCb(lv_event_t *e)
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

void ShowThpyRunScreen(void)
{
    CreateScreen();
    lv_obj_t *scr = GetActiveScreenHandle();
    lv_group_t *grp = lv_group_get_default();

    runUi.screenHandle         = scr;
    runUi.isPaused             = false;
    runUi.elapsedSec           = 0U;
    runUi.achievedPressureMmHg = 0;

    runUi.statusLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.statusLabel, &mono_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(runUi.statusLabel, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_pos(runUi.statusLabel, 14, 6);

    runUi.dividerLine = lv_obj_create(scr);
    lv_obj_set_size(runUi.dividerLine, HORIZONTAL_RES - 28, 2);
    lv_obj_set_pos(runUi.dividerLine, 14, 38);
    lv_obj_set_style_bg_color(runUi.dividerLine, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(runUi.dividerLine, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(runUi.dividerLine, 0, LV_PART_MAIN);

    runUi.setPressLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.setPressLabel, &mono_16, LV_PART_MAIN);
    lv_obj_set_pos(runUi.setPressLabel, 16, 58);

    runUi.achPressLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.achPressLabel, &mono_16, LV_PART_MAIN);
    lv_obj_set_pos(runUi.achPressLabel, 16, 92);

    runUi.elapsedLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.elapsedLabel, &mono_16, LV_PART_MAIN);
    lv_obj_set_pos(runUi.elapsedLabel, 16, 126);

    runUi.pauseBtn = lv_button_create(scr);
    lv_obj_set_size(runUi.pauseBtn, RUN_BTN_W_PX, RUN_BTN_H_PX);
    lv_obj_set_pos(runUi.pauseBtn, RUN_PAUSE_X_PX, RUN_ACTION_Y_PX);
    StyleActionButton(runUi.pauseBtn, 0x005A9C);
    lv_obj_add_event_cb(runUi.pauseBtn, PauseBtnClickCb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(runUi.pauseBtn, ActionBtnKeyCb, LV_EVENT_KEY, NULL);

    runUi.pauseLabel = lv_label_create(runUi.pauseBtn);
    lv_obj_set_style_text_font(runUi.pauseLabel, &mono_16, LV_PART_MAIN);
    lv_obj_center(runUi.pauseLabel);

    runUi.stopBtn = lv_button_create(scr);
    lv_obj_set_size(runUi.stopBtn, RUN_BTN_W_PX, RUN_BTN_H_PX);
    lv_obj_set_pos(runUi.stopBtn, RUN_STOP_X_PX, RUN_ACTION_Y_PX);
    StyleActionButton(runUi.stopBtn, 0xD93025);
    lv_obj_add_event_cb(runUi.stopBtn, StopBtnClickCb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(runUi.stopBtn, ActionBtnKeyCb, LV_EVENT_KEY, NULL);

    lv_obj_t *stopLbl = lv_label_create(runUi.stopBtn);
    lv_label_set_text(stopLbl, "STOP");
    lv_obj_set_style_text_font(stopLbl, &mono_16, LV_PART_MAIN);
    lv_obj_center(stopLbl);

    RefreshDynamicLabels();

    runUi.elapsedTimer = lv_timer_create(ElapsedTimerCb, RUN_TIMER_PERIOD_MS, NULL);

    if(grp != NULL)
    {
        lv_group_add_obj(grp, runUi.pauseBtn);
        lv_group_add_obj(grp, runUi.stopBtn);
        lv_group_focus_obj(runUi.pauseBtn);
    }
}

void ClearThpyRunStateVar(void)
{
    if(runUi.elapsedTimer != NULL)
    {
        lv_timer_delete(runUi.elapsedTimer);
        runUi.elapsedTimer = NULL;
    }

    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_remove_all_objs(grp);
    }

    memset(&runUi, 0, sizeof(thpy_run_components_t));
}

void ThpyRunSetAchievedPressure(int16_t pressureMmHg)
{
    GuiAcquireLock();
    if(GetActiveScreenId() == UI_THPY_RUN_SCREEN)
    {
        runUi.achievedPressureMmHg = pressureMmHg;
        RefreshDynamicLabels();
    }
    GuiReleaseLock();
}
