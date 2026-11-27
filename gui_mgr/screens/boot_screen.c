/*
 * boot_screen.c
 *
 * Displays the "CAPTURE MEDITECH" boot splash screen.
 */

#include "boot_screen.h"
#include "screenhandler.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <string.h>

LV_FONT_DECLARE(dot_matrix);

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *titleLabel;
} boot_screen_components_t;

static boot_screen_components_t bootScreenComponents = { 0 };

static void UISetContent(void)
{
    bootScreenComponents.screenHandle = GetActiveScreenHandle();

    bootScreenComponents.titleLabel = lv_label_create(bootScreenComponents.screenHandle);
    lv_label_set_text(bootScreenComponents.titleLabel, "CAPTURE MEDITECH");
    lv_obj_set_width(bootScreenComponents.titleLabel, HORIZONTAL_RES - 16);
    lv_label_set_long_mode(bootScreenComponents.titleLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(bootScreenComponents.titleLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(bootScreenComponents.titleLabel, &dot_matrix, LV_PART_MAIN);
    lv_obj_center(bootScreenComponents.titleLabel);
}

void ShowBootScreen(void)
{
    CreateScreen();
    UISetContent();
}

void ClearBootStateVar(void)
{
    memset(&bootScreenComponents, 0, sizeof(boot_screen_components_t));
}
