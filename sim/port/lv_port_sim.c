/*
 * lv_port_sim.c
 *
 * Simulator display port.
 * Implements lv_disp_st7789_init() using the SDL2 HAL so gui_mgr.c
 * calls the same function name on the simulator as on hardware.
 */

#include "lv_port_sim.h"
#include "config/appconfig.h"       /* DISP_HOR_RES, DISP_VER_RES */
#include "sim/hal/hal.h"

int8_t lv_disp_st7789_init(void)
{
    lv_display_t *disp = hal_init(DISP_HOR_RES, DISP_VER_RES);
    if(disp == NULL)
    {
        return APP_ERR_INV_STATE;
    }

    return APP_ERR_NONE;
}
