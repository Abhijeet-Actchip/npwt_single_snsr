#include "ui_helper.h"
#include "utils_c/zmq_restapi_common.h"
#include "fogpass/fogpass_ui.h"
#include "lvgl/lvgl.h"
#include "utils_c/cJSON.h"
#include "utils_c/log_pub.h"
#include "hal/hal.h"
#include <fogpass/fogpass_types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define TAG                 "UI-HELPER"

static void *LvglThread(void *arg)
{
    uimgr_ctx_t *ctx = (uimgr_ctx_t *) arg;
    assert(ctx != NULL);

    lv_init();
    hal_init(FOGPASS_DISP_W, FOGPASS_DISP_H);
    FogpassUIInit(ctx);

    while(ctx->uiMgrActive)
    {
        uint32_t sleep_time_ms = lv_timer_handler();
        if(sleep_time_ms == LV_NO_TIMER_READY)
        {
            sleep_time_ms =  LV_DEF_REFR_PERIOD;
        }
        /* Reduce CPU hog. */
        if(sleep_time_ms < 10)
        {
            sleep_time_ms = 10;
        }
        usleep(sleep_time_ms * 1000);
    }
    return NULL;
}

int8_t UIHelperInitLvgl(uimgr_ctx_t *ctx)
{
    if(ctx == NULL)
    {
        return APP_ERR_INVALID_PARAMS;
    }
    
    int8_t ret;
    ret = pthread_create(&ctx->lvglThreadHandle, NULL, LvglThread, ctx);
    if(ret != APP_ERR_SUCCESS)
    {
        return ret;
    }

    return APP_ERR_SUCCESS;
}

static void UpdateStateFromJSON(fogpass_state_t *state, cJSON *msgJson)
{
    cJSON *isFoggy = cJSON_GetObjectItem(msgJson, JSON_KEY_IS_FOGGY);
    if(cJSON_IsBool(isFoggy))
    {
        if(cJSON_IsTrue(isFoggy))
        {
            state->weather = FOGPASS_WEATHER_FOGGY;
        }
        else
        {
            state->weather = FOGPASS_WEATHER_NON_FOGGY;
        }
    }
    state->route_mode = FOGPASS_MODE_MANUAL;
    cJSON *isUp = cJSON_GetObjectItem(msgJson, JSON_KEY_IS_UP);
    if(cJSON_IsBool(isUp))
    {
        if(cJSON_IsTrue(isUp))
        {
            state->track = FOGPASS_TRACK_UP;
        }
        else
        {
            state->track = FOGPASS_TRACK_DOWN;
        }
    }

    cJSON *batPct = cJSON_GetObjectItem(msgJson, JSON_KEY_BAT_PCT);
    if(cJSON_IsNumber(batPct))
    {
        state->battery_pct = batPct->valueint;
    }
    cJSON *batCharging = cJSON_GetObjectItem(msgJson, JSON_KEY_BAT_CHARGING);
    state->battery_charging = cJSON_IsTrue(batCharging);
    cJSON *batLow = cJSON_GetObjectItem(msgJson, JSON_KEY_BAT_LOW);
    state->battery_low = cJSON_IsTrue(batLow);
   
    cJSON *snoozeActive = cJSON_GetObjectItem(msgJson, JSON_KEY_SNOOZE_ACTIVE);
    state->snooze_active = cJSON_IsTrue(snoozeActive);
    cJSON *pdConnected = cJSON_GetObjectItem(msgJson, JSON_KEY_PD_CONNECTED);
    state->pen_drive_connected = cJSON_IsTrue(pdConnected);
    
    cJSON *gpsFix = cJSON_GetObjectItem(msgJson, JSON_KEY_GPS_FIX);
    if(cJSON_IsNumber(gpsFix))
    {
        state->gps_fix_mode = gpsFix->valueint;
        if(gpsFix->valueint < 2)
        {
            state->gps_state = FOGPASS_GPS_ERROR;
        }
        else
        {
            state->gps_state = FOGPASS_GPS_OK;
        }
    }
    cJSON *gpsExtant = cJSON_GetObjectItem(msgJson, JSON_KEY_GPS_EXTANT);
    if(cJSON_IsTrue(gpsExtant))
    {
        state->antenna = FOGPASS_ANTENNA_EXTERNAL;
    }
    else
    {
        state->antenna = FOGPASS_ANTENNA_INTERNAL;
    }
    cJSON *gpsLat = cJSON_GetObjectItem(msgJson, JSON_KEY_GPS_LAT);
    if(cJSON_IsNumber(gpsLat))
    {
        state->gps_lat = gpsLat->valuedouble;
    }
    cJSON *gpsLon = cJSON_GetObjectItem(msgJson, JSON_KEY_GPS_LON);
    if(cJSON_IsNumber(gpsLon))
    {
        state->gps_lon = gpsLon->valuedouble;
    }

    cJSON *priText = cJSON_GetObjectItem(msgJson, JSON_KEY_PRI_TEXT);
    if(cJSON_IsString(priText))
    {
        strncpy(state->landmarks[0].description, 
            priText->valuestring,
            sizeof(state->landmarks[0].description));
        state->landmarks[0].description[sizeof(state->landmarks[0].description) - 1] = '\0';
    }
    cJSON *priMd = cJSON_GetObjectItem(msgJson, JSON_KEY_PRI_MD);
    if(cJSON_IsString(priMd))
    {
        strncpy(state->landmarks[0].mast_km,
            priMd->valuestring,
            sizeof(state->landmarks[0].mast_km));
        state->landmarks[0].mast_km[sizeof(state->landmarks[0].mast_km) - 1] = '\0';
    }
    cJSON *priDist = cJSON_GetObjectItem(msgJson, JSON_KEY_PRI_DIST);
    if(cJSON_IsNumber(priDist))
    {
        state->landmarks[0].distance_m = priDist->valueint;
    }
    cJSON *priColor = cJSON_GetObjectItem(msgJson, JSON_KEY_PRI_COLOR);
    if(cJSON_IsNumber(priColor))
    {
        state->landmarks[0].signal_class = (fogpass_landmark_class_t)priColor->valueint;
    }
    state->landmarks[0].active = true;
    cJSON *sec1Valid = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC1_VALID);
    if(cJSON_IsTrue(sec1Valid))
    {
        state->landmarks[1].active = true;

        cJSON *sec1Text = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC1_TEXT);
        if(cJSON_IsString(sec1Text))
        {
            strncpy(state->landmarks[1].description, 
                sec1Text->valuestring,
                sizeof(state->landmarks[1].description));
            state->landmarks[1].description[sizeof(state->landmarks[1].description) - 1] = '\0';
        }
        state->landmarks[1].mast_km[0] = '\0';
        cJSON *sec1Dist = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC1_DIST);
        if(cJSON_IsNumber(sec1Dist))
        {
            state->landmarks[1].distance_m = sec1Dist->valueint;
        }
        cJSON *sec1Color = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC1_COLOR);
        if(cJSON_IsNumber(sec1Color))
        {
            state->landmarks[1].signal_class = (fogpass_landmark_class_t)sec1Color->valueint;
        }
    }
    else
    {
        state->landmarks[1].active = false;
    }

    cJSON *sec2Valid = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC2_VALID);
    if(cJSON_IsTrue(sec2Valid))
    {
        state->landmarks[2].active = true;

        cJSON *sec2Text = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC2_TEXT);
        if(cJSON_IsString(sec2Text))
        {
            strncpy(state->landmarks[2].description, 
                sec2Text->valuestring,
                sizeof(state->landmarks[2].description));
            state->landmarks[2].description[sizeof(state->landmarks[2].description) - 1] = '\0';
        }
        state->landmarks[2].mast_km[0] = '\0';
        cJSON *sec2Dist = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC2_DIST);
        if(cJSON_IsNumber(sec2Dist))
        {
            state->landmarks[2].distance_m = sec2Dist->valueint;
        }
        cJSON *sec2Color = cJSON_GetObjectItem(msgJson, JSON_KEY_SEC2_COLOR);
        if(cJSON_IsNumber(sec2Color))
        {
            state->landmarks[2].signal_class = (fogpass_landmark_class_t)sec2Color->valueint;
        }
    }
    else
    {
        state->landmarks[2].active = false;
    }

    cJSON *speed = cJSON_GetObjectItem(msgJson, JSON_KEY_SPEED);
    if(cJSON_IsNumber(speed))
    {
        /* Convert m/s to km/h. */
        state->speed_kph = (int32_t)(speed->valuedouble * 3.6);
    }
    cJSON *gradient = cJSON_GetObjectItem(msgJson, JSON_KEY_GRADIENT);
    if(cJSON_IsString(gradient))
    {
        strncpy(state->gradient_text,
            gradient->valuestring,
            sizeof(state->gradient_text));
        state->gradient_text[sizeof(state->gradient_text) - 1] = '\0';
    }

    cJSON *isDiverted = cJSON_GetObjectItem(msgJson, JSON_KEY_IS_DIVERTED);
    state->isDiverted = cJSON_IsTrue(isDiverted);
    cJSON *autoEnabled = cJSON_GetObjectItem(msgJson, JSON_KEY_AUTO_ENABLED);
    state->isAutoRouteEnabled = cJSON_IsTrue(autoEnabled);
    cJSON *routeExec = cJSON_GetObjectItem(msgJson, JSON_KEY_ROUTE_EXEC);
    state->isExecutingRoute = cJSON_IsTrue(routeExec);
    if(state->isExecutingRoute)
    {
        cJSON *zone = cJSON_GetObjectItem(msgJson, JSON_KEY_ZONE);
        if(cJSON_IsString(zone) && zone->valuestring[0] != '\0')
        {
            strncpy(state->route_zone, zone->valuestring, sizeof(state->route_zone) - 1);
            state->route_zone[sizeof(state->route_zone) - 1] = '\0';
            for(size_t i = 0; state->route_zone[i] != '\0'; i++)
            {
                state->route_zone[i] = (char)toupper((unsigned char)state->route_zone[i]);
            }
        }
        cJSON *division = cJSON_GetObjectItem(msgJson, JSON_KEY_DIVISION);
        if(cJSON_IsString(division) && division->valuestring[0] != '\0')
        {
            strncpy(state->route_division, division->valuestring, sizeof(state->route_division) - 1);
            state->route_division[sizeof(state->route_division) - 1] = '\0';
            for(size_t i = 0; state->route_division[i] != '\0'; i++)
            {
                state->route_division[i] = (char)toupper((unsigned char)state->route_division[i]);
            }
        }
        cJSON *route = cJSON_GetObjectItem(msgJson, JSON_KEY_ROUTE);
        if(cJSON_IsString(route) && route->valuestring[0] != '\0')
        {
            strncpy(state->route_section, route->valuestring, sizeof(state->route_section) - 1);
            state->route_section[sizeof(state->route_section) - 1] = '\0';
            for(size_t i = 0; state->route_section[i] != '\0'; i++)
            {
                state->route_section[i] = (char)toupper((unsigned char)state->route_section[i]);
            }
        }
    }
}

int8_t UIHelperProcessNotifyMsgs(pub_sub_pkt_t *msg)
{
    if(msg == NULL)
    {
        return APP_ERR_INVALID_PARAMS;
    }

    cJSON *msgJson = cJSON_Parse((char *)msg->data);
    if(msgJson == NULL)
    {
        LogPubPrintF(TAG, "Err: UIHelperProcessNotifyMsgs: Failed to parse message.");
        return APP_ERR_MEM_ALLOC_FAILED;
    }

    fogpass_state_t state;
    
    /* NOTE: Lock gets acquired even inside the FogpassUIxx() APIs. 
     * Calling it here to make the update atomic. 
     * The mutex is recursive, so should be safe to call here.
     */
    lv_lock();
    {
        FogpassUIGetState(&state);
        UpdateStateFromJSON(&state, msgJson);
        FogpassUISetState(&state);
        FogpassUIProcessDiversion();
        FogpassUIProcessAutoRoute();
        FogpassUIProcessRouteExec();
        FogpassUIShowPowerOffDialog(msgJson);
    }
    lv_unlock();

    cJSON_Delete(msgJson);
    return APP_ERR_SUCCESS;
}