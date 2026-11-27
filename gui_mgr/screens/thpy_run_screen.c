/*
 * thpy_run_screen.c
 *
 * Character-display style therapy running screen.
 * Displays therapy status, set pressure, achieved pressure, elapsed time,
 * and PAUSE/RESUME + STOP actions navigated with Left/Right and Enter.
 */

#include "thpy_run_screen.h"
#include "thpy_config_screen.h"
#include "screenhandler.h"
#include "app/gui_mgr.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(dot_matrix_26);
LV_FONT_DECLARE(dot_matrix);

#define RUN_TIMER_PERIOD_MS         1000U
#define RUN_ACTION_Y_PX             205
#define RUN_PAUSE_X_PX              36
#define RUN_STOP_X_PX               190

typedef enum
{
    RUN_ACTION_PAUSE_RESUME = 0,
    RUN_ACTION_STOP,
    RUN_ACTION_COUNT
} run_action_t;

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *statusLabel;
    lv_obj_t *setPressLabel;
    lv_obj_t *achPressLabel;
    lv_obj_t *elapsedLabel;
    lv_obj_t *pauseLabel;
    lv_obj_t *stopLabel;
    lv_obj_t *cursorLabel;
    lv_timer_t *elapsedTimer;
    bool isPaused;
    run_action_t activeAction;
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

static void UpdateActionCursor(void)
{
    int32_t xPos = (runUi.activeAction == RUN_ACTION_PAUSE_RESUME)
                   ? (RUN_PAUSE_X_PX - 16)
                   : (RUN_STOP_X_PX - 16);
    lv_obj_set_pos(runUi.cursorLabel, xPos, RUN_ACTION_Y_PX);
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

static void HandleEnter(void)
{
    if(runUi.activeAction == RUN_ACTION_PAUSE_RESUME)
    {
        runUi.isPaused = !runUi.isPaused;
        RefreshDynamicLabels();
    }
    else
    {
        UISelectScreen(UI_THPY_CONFIG_SCREEN);
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

        if((key == LV_KEY_UP) || (key == LV_KEY_DOWN))
        {
            runUi.activeAction = (runUi.activeAction == RUN_ACTION_PAUSE_RESUME)
                                 ? RUN_ACTION_STOP
                                 : RUN_ACTION_PAUSE_RESUME;
            UpdateActionCursor();
        }
    }
}

void ShowThpyRunScreen(void)
{
    CreateScreen();
    lv_obj_t *scr = GetActiveScreenHandle();
    runUi.screenHandle         = scr;
    runUi.isPaused             = false;
    runUi.activeAction         = RUN_ACTION_PAUSE_RESUME;
    runUi.elapsedSec           = 0U;
    runUi.achievedPressureMmHg = 0;

    runUi.statusLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.statusLabel, &dot_matrix, LV_PART_MAIN);
    lv_obj_set_pos(runUi.statusLabel, 16, 12);

    runUi.setPressLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.setPressLabel, &dot_matrix_26, LV_PART_MAIN);
    lv_obj_set_pos(runUi.setPressLabel, 16, 60);

    runUi.achPressLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.achPressLabel, &dot_matrix_26, LV_PART_MAIN);
    lv_obj_set_pos(runUi.achPressLabel, 16, 95);

    runUi.elapsedLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.elapsedLabel, &dot_matrix_26, LV_PART_MAIN);
    lv_obj_set_pos(runUi.elapsedLabel, 16, 130);

    runUi.pauseLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(runUi.pauseLabel, &dot_matrix_26, LV_PART_MAIN);
    lv_obj_set_pos(runUi.pauseLabel, RUN_PAUSE_X_PX, RUN_ACTION_Y_PX);

    runUi.stopLabel = lv_label_create(scr);
    lv_label_set_text(runUi.stopLabel, "STOP");
    lv_obj_set_style_text_font(runUi.stopLabel, &dot_matrix_26, LV_PART_MAIN);
    lv_obj_set_pos(runUi.stopLabel, RUN_STOP_X_PX, RUN_ACTION_Y_PX);

    runUi.cursorLabel = lv_label_create(scr);
    lv_label_set_text(runUi.cursorLabel, ">");
    lv_obj_set_style_text_font(runUi.cursorLabel, &dot_matrix_26, LV_PART_MAIN);

    RefreshDynamicLabels();
    UpdateActionCursor();

    runUi.elapsedTimer = lv_timer_create(ElapsedTimerCb, RUN_TIMER_PERIOD_MS, NULL);

    lv_obj_add_event_cb(scr, ScreenInputCb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(scr, ScreenInputCb, LV_EVENT_CLICKED, NULL);

    lv_group_t *grp = lv_group_get_default();
    if(grp != NULL)
    {
        lv_group_add_obj(grp, scr);
        lv_group_focus_obj(scr);
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
