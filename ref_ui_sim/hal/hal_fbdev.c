#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include "lvgl/src/drivers/evdev/lv_evdev.h"

lv_display_t * hal_init(int32_t w, int32_t h)
{
    (void)w; (void)h;  /* fbdev uses hardware resolution */

    lv_group_set_default(lv_group_create());

    lv_display_t *disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, LV_EVDEV_DEVICE_NAME);
    if (touch)
    {
        lv_indev_set_display(touch, disp);
        lv_indev_set_group(touch, lv_group_get_default());
        /* Touch calibration is only required on the prod board. */
        #if (USE_FOGPASS_DEVBOARD == 0)
            /* NOTE: Actual range was somewhere around
             * min = (200, 200) and max = (9600, 9600).
             * Edit this if you notice any touch issues.
             */
            lv_evdev_set_calibration(touch, 200, 200, 9600, 9600);
        #endif
    }

    lv_display_set_default(disp);
    return disp;
}