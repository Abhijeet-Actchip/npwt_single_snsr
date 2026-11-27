/*
 * boot_screen.c
 *
 * Displays the Capture Meditech boot splash screen with brand colors.
 */

#include "boot_screen.h"
#include "screenhandler.h"
#include "app_middlewares/lvgl/lvgl.h"
#include <string.h>

LV_FONT_DECLARE(mono_16);
LV_FONT_DECLARE(mono_cm_96);

typedef struct
{
    lv_obj_t *screenHandle;
    lv_obj_t *brandLabel;
    lv_obj_t *subLabel;
} boot_screen_components_t;

static boot_screen_components_t bootScreenComponents = { 0 };

static void UISetContent(void)
{
    lv_obj_t *scr = GetActiveScreenHandle();
    bootScreenComponents.screenHandle = scr;

    /* Light ice-blue background matching the Capture Meditech boot screen */
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xCBEBF7), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    bootScreenComponents.brandLabel = lv_label_create(scr);
    lv_label_set_text(bootScreenComponents.brandLabel, "CM");
    lv_obj_set_style_text_font(bootScreenComponents.brandLabel, &mono_cm_96, LV_PART_MAIN);
    lv_obj_set_style_text_color(bootScreenComponents.brandLabel, lv_color_hex(0x005A9C), LV_PART_MAIN);
    lv_obj_align(bootScreenComponents.brandLabel, LV_ALIGN_CENTER, 0, -24);

    bootScreenComponents.subLabel = lv_label_create(scr);
    lv_label_set_text(bootScreenComponents.subLabel, "Capture Meditech Pvt Ltd");
    lv_obj_set_style_text_font(bootScreenComponents.subLabel, &mono_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(bootScreenComponents.subLabel, lv_color_hex(0xE1251B), LV_PART_MAIN);
    lv_obj_align(bootScreenComponents.subLabel, LV_ALIGN_CENTER, 0, 62);
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
