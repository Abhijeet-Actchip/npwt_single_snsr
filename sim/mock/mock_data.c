/*
 * mock_data.c
 *
 * Simulator mock sensor data feed.
 *
 * Generates slowly varying synthetic sensor values and calls
 * GuiMgrUpdateData() so the UI screens update like they would on hardware.
 *
 * Platform-decoupled: uses UIPlatform_Delay and UIPlatform_CreateTask
 * rather than any OS-specific API.
 *
 * NOTE: GuiMgrUpdateData() and app_data_t are currently commented out in
 * gui_mgr.c. The skeleton below is ready to activate once the data type
 * is finalised. Screen navigation (GuiMgrShowNextScr / Prev) is exercised
 * in the meantime via key presses.
 */

#include "mock_data.h"
#include "app/gui_mgr.h"
#include "app/ui_platform.h"
#include "config/appconfig.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define MOCK_UPDATE_INTERVAL_MS     500U

static void MockDataTask(void *arg)
{
    (void)arg;
    float t = 0.0f;

    while(1)
    {
        /*
         * Uncomment and populate app_data_t once GuiMgrUpdateData()
         * is re-enabled in gui_mgr.c:
         *
         * app_data_t data = {
         *     .dOxygen     = 7.5f  + sinf(t)        * 1.5f,
         *     .ph          = 7.0f  + cosf(t * 0.5f) * 0.5f,
         *     .salinity    = 35.0f + sinf(t * 0.3f),
         *     .co2         = 400U  + (uint32_t)(fabsf(sinf(t * 0.7f)) * 50.0f),
         *     .turbidity   = 10U,
         *     .barPressure = 1013.0f + sinf(t * 0.2f) * 5.0f,
         *     .temp        = 25.0f   + sinf(t * 0.4f) * 3.0f,
         * };
         * GuiMgrUpdateData(&data);
         */

        t += 0.05f;
        UIPlatform_Delay(MOCK_UPDATE_INTERVAL_MS);
    }
}

void MockDataStart(void)
{
    UIPlatform_CreateTask(MockDataTask, "MOCK", 256U, NULL, 1U, NULL);
}
