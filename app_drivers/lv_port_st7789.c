/*
 * lv_port_st7789.c
 *
 *  Created on: 26-Nov-2026
 *      Author: Abhijeet M
 */

#include "lv_port_st7789.h"
#include "bsp/board.h"
#include "app_hal/app_hal_delay.h"
#include "app_hal/app_hal_spi.h"
#include "app_middlewares/lvgl/lvgl.h"

#define LCD_H_RES               DISP_HOR_RES
#define LCD_V_RES               DISP_VER_RES
#define LCD_BUF_LINES           60U
#define LCD_SPI_TIMEOUT_MS      100U
#define LCD_RESET_PULSE_MS      10U
#define LCD_RESET_WAIT_MS       120U

static lv_display_t *myDisp = NULL;

static void DisplayCSCtrl(bool select)
{
    (void)Board_EnableDisplayCS(!select);
}

static void DisplayReset(bool en)
{
	(void)Board_CtrlDisplayReset(en);
}

static void DisplayRSCtrl(bool isData)
{
	(void)Board_CtrlDisplayRS(isData);
}

static void SendSpiChunks(uint8_t *data, size_t len)
{
    size_t offset = 0U;

    while(offset < len)
    {
        size_t remaining = len - offset;
        uint16_t chunk = (remaining > UINT16_MAX) ? UINT16_MAX : (uint16_t)remaining;
        (void)APPHAL_SPI_Write(BOARD_SPI1_PID, &data[offset], chunk, LCD_SPI_TIMEOUT_MS);
        offset += chunk;
    }
}

/* Send command to the LCD controller */
static void LcdSendCmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                       const uint8_t *param, size_t param_size)
{
    (void)disp;

    DisplayCSCtrl(true);

    if((cmd != NULL) && (cmd_size > 0U))
    {
    	DisplayRSCtrl(0);
        SendSpiChunks((uint8_t *)cmd, cmd_size);
    }

    if((param != NULL) && (param_size > 0U))
    {
    	DisplayRSCtrl(1);
        SendSpiChunks((uint8_t *)param, param_size);
    }

    DisplayCSCtrl(false);
}

/* Send pixel data to the LCD controller */
static void LcdSendColor(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                         uint8_t *param, size_t param_size)
{
    DisplayCSCtrl(true);

    if((cmd != NULL) && (cmd_size > 0U))
    {
    	DisplayRSCtrl(0);
        SendSpiChunks((uint8_t *)cmd, cmd_size);
    }

    if((param != NULL) && (param_size > 0U))
    {
        if(lv_display_get_color_format(disp) == LV_COLOR_FORMAT_RGB565)
        {
            lv_draw_sw_rgb565_swap(param, param_size / 2U);
        }

        DisplayRSCtrl(1);
        SendSpiChunks(param, param_size);
    }

    DisplayCSCtrl(false);
    lv_display_flush_ready(disp);
}

int8_t lv_disp_st7789_init(void)
{
    /* Hardware reset pulse for ST7789 */
    DisplayCSCtrl(false);
    DisplayReset(0);
    APPHAL_Delay(LCD_RESET_PULSE_MS);
    DisplayReset(1);
    APPHAL_Delay(LCD_RESET_WAIT_MS);

    /* Create the LVGL display object and the ST7789 display driver */
    myDisp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE,
                              LcdSendCmd, LcdSendColor);
    if(myDisp == NULL)
    {
        return APP_ERR_INV_STATE;
    }

    /* Set display orientation to landscape */
    lv_display_set_rotation(myDisp, LV_DISPLAY_ROTATION_90);

    /* Configure draw buffer */
    uint32_t bufSize = LCD_H_RES * LCD_BUF_LINES
            * lv_color_format_get_size(lv_display_get_color_format(myDisp));

    uint8_t *buf1 = lv_malloc(bufSize);
    if(buf1 == NULL)
    {
    	/* TODO: Replace with TRICE logs */
        LV_LOG_ERROR("display draw buffer malloc failed");
        return APP_ERR_NO_MEM;
    }

    lv_display_set_buffers(myDisp, buf1, NULL, bufSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /** NOTE: We are turning the display backlight pot bsp initialization. */

    return APP_ERR_NONE;
}
