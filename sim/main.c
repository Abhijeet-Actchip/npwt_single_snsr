/*
 * main.c  (simulator)
 *
 * Entry point for the WSL2 LVGL simulator.
 * Starts the GUI manager (which spawns tick + render pthreads internally),
 * shows the first screen, starts the mock data feed, then idles.
 *
 * All OS work is done through the ui_platform abstraction layer so this
 * file remains platform-agnostic.
 */

#include "app/gui_mgr.h"
#include "sim/mock/mock_data.h"
#include "app/ui_platform.h"
#include "config/appconfig.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    int8_t ret;

    printf("NPWT UI Simulator — %dx%d\n", DISP_HOR_RES, DISP_VER_RES);

    ret = GuiMgrInit();
    assert(ret == APP_ERR_NONE);

    /* Wait for GuiMgrTask to finish lv_init() and SDL display creation. */
    while(!GuiMgrIsReady())
    {
        UIPlatform_Delay(10U);
    }

    ret = GuiMgrShowScr(UI_DO_SCREEN);
    assert(ret == APP_ERR_NONE);

    /* Start mock sensor data feed. */
    MockDataStart();

    /* Idle — all rendering and input handling run in pthreads. */
    while(1)
    {
        UIPlatform_Delay(100U);
    }

    return 0;
}
