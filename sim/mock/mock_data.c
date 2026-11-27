/*
 * mock_data.c
 *
 * Simulator mock sensor data feed.
 *
 * When UI_THPY_RUN_SCREEN is active, simulates achieved pressure converging
 * toward the configured target pressure and updates the run screen via
 * ThpyRunSetAchievedPressure().
 *
 * Platform-decoupled: uses UIPlatform_Delay and UIPlatform_CreateTask.
 */

#include "mock_data.h"
#include "gui_mgr/screenhandler.h"
#include "gui_mgr/screens/thpy_config_screen.h"
#include "gui_mgr/screens/thpy_run_screen.h"
#include "app/ui_platform.h"
#include "config/appconfig.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define MOCK_UPDATE_INTERVAL_MS     500U

static void MockDataTask(void *arg)
{
    (void)arg;
    float currentPress = 0.0f;
    float t = 0.0f;

    while(1)
    {
        if(GetActiveScreenId() == UI_THPY_RUN_SCREEN)
        {
            const thpy_config_t *cfg = ThpyConfigGet();
            float target = (cfg->mode == THPY_SEL_CONTINUOUS)
                           ? (float)cfg->pressureMmHg
                           : (float)cfg->maxPressureMmHg;

            /* Smoothly approach target pressure with small ripple */
            currentPress += (target - currentPress) * 0.25f;
            int16_t measured = (int16_t)(currentPress + (sinf(t) * 2.0f));
            ThpyRunSetAchievedPressure(measured);
        }
        else
        {
            currentPress = 0.0f;
        }

        t += 0.3f;
        UIPlatform_Delay(MOCK_UPDATE_INTERVAL_MS);
    }
}

void MockDataStart(void)
{
    UIPlatform_CreateTask(MockDataTask, "MOCK", 256U, NULL, 1U, NULL);
}
