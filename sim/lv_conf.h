/**
 * @file lv_conf.h
 *
 * Simulator-specific LVGL configuration.
 * Derived from config/lv_conf.h (embedded) with the following overrides:
 *   - LV_USE_OS:            LV_OS_PTHREAD    (instead of LV_OS_NONE)
 *   - LV_COLOR_FORMAT:      XRGB8888         (SDL2 requires 32-bit)
 *   - LV_USE_SDL:           1                (SDL2 display + input driver)
 *   - LV_MEM_SIZE:          4 MB             (embedded uses 64 KB)
 *   - LV_USE_LOG:           1, LV_LOG_PRINTF (enable console logging)
 *
 * All other settings are identical to the embedded config.
 */

/* clang-format off */
#if 1 /* Set this to "1" to enable content */

#ifndef LV_CONF_H
#define LV_CONF_H

#if 0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif

/*============================================================================
 * MEMORY AND STANDARD LIBRARY
 *============================================================================*/

#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
/* 4 MB — ample headroom for a desktop simulator. */
#define LV_MEM_SIZE             (4U * 1024U * 1024U)
#define LV_MEM_ADR              0x0
#endif

#define LV_STDINT_INCLUDE       "stdint.h"
#define LV_STDDEF_INCLUDE       "stddef.h"
#define LV_STDBOOL_INCLUDE      "stdbool.h"
#define LV_INTTYPES_INCLUDE     "inttypes.h"
#define LV_LIMITS_INCLUDE       "limits.h"
#define LV_STDARG_INCLUDE       "stdarg.h"

/*============================================================================
 * OPERATING SYSTEM (OS)
 *============================================================================*/

/* Use pthreads on the simulator. */
#define LV_USE_OS               LV_OS_PTHREAD

/*============================================================================
 * RENDERING CONFIGURATION
 *============================================================================*/

/* SDL2 requires 32-bit XRGB8888. The ST7789 uses RGB565 on hardware;
 * gui_mgr.c does not reference the color format directly, so this is safe. */
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_XRGB8888

#define LV_COLOR_MIX_ROUND_OFS     0
#define LV_DEF_REFR_PERIOD         33
#define LV_DPI_DEF                 130
#define LV_DRAW_BUF_STRIDE_ALIGN   1
#define LV_DRAW_BUF_ALIGN          4
#define LV_USE_MATRIX              0
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE   24576
#define LV_DRAW_LAYER_MAX_MEMORY        0

#if LV_USE_OS != LV_OS_NONE
#define LV_DRAW_THREAD_STACK_SIZE   (32U * 1024U)
#define LV_DRAW_THREAD_PRIO         2   /* LV_THREAD_PRIO_MID */
#endif

/*============================================================================
 * FEATURE CONFIGURATION
 *============================================================================*/

/* Software renderer */
#define LV_USE_DRAW_SW      1
#if LV_USE_DRAW_SW
    #define LV_DRAW_SW_DRAW_UNIT_CNT    1
    #define LV_DRAW_SW_COMPLEX          1
    #if LV_DRAW_SW_COMPLEX == 1
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE    0
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE    4
    #endif
    #define LV_USE_DRAW_SW_ASM      LV_DRAW_SW_ASM_NONE
    #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    1
    #define LV_DRAW_SW_SUPPORT_RGB565           1
    #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED   1
    #define LV_DRAW_SW_SUPPORT_RGB888           1
    #define LV_DRAW_SW_SUPPORT_XRGB8888         1
    #define LV_DRAW_SW_SUPPORT_ARGB8888         1
    #define LV_DRAW_SW_SUPPORT_L8               1
    #define LV_DRAW_SW_SUPPORT_AL88             1
    #define LV_DRAW_SW_SUPPORT_A8               1
    #define LV_DRAW_SW_SUPPORT_I1               1
    #define LV_DRAW_SW_I1_LUM_THRESHOLD         127
#endif

/*============================================================================
 * LOGGING
 *============================================================================*/

/* Enable LVGL log output to stdout — useful during simulator development. */
#define LV_USE_LOG              1
#if LV_USE_LOG
    #define LV_LOG_LEVEL        LV_LOG_LEVEL_WARN
    #define LV_LOG_PRINTF       1
    #define LV_LOG_USE_TIMESTAMP    1
    #define LV_LOG_USE_FILE_LINE    1
    #define LV_LOG_TRACE_MEM        0
    #define LV_LOG_TRACE_TIMER      0
    #define LV_LOG_TRACE_INDEV      0
    #define LV_LOG_TRACE_DISP_REFR  0
    #define LV_LOG_TRACE_EVENT      0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT     0
    #define LV_LOG_TRACE_ANIM       0
    #define LV_LOG_TRACE_CACHE      0
#endif

/*============================================================================
 * ASSERT / CHECK
 *============================================================================*/

#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/* On sim, assert failures abort with a trace rather than spinning. */
#include <assert.h>
#define LV_ASSERT_HANDLER           assert(0);

#define LV_USE_CHECK_ARG            1
#if LV_USE_CHECK_ARG
    #define LV_CHECK_ARG_ASSERT_ON_FAIL 0
    #if LV_USE_LOG
        #define LV_CHECK_ARG_LOG_MODE LV_CHECK_ARG_LOG_MODE_VERBOSE
    #endif
#endif
#define LV_USE_CHECK_OBJ_CLASSTYPE  0
#define LV_USE_CHECK_OBJ_VALIDITY   0

/*============================================================================
 * OTHERS
 *============================================================================*/

#define LV_ENABLE_GLOBAL_CUSTOM         0
#define LV_CACHE_DEF_SIZE               0
#define LV_IMAGE_HEADER_CACHE_DEF_CNT   0
#define LV_GRADIENT_MAX_STOPS           2
#define LV_OBJ_STYLE_CACHE              1
#define LV_USE_OBJ_ID                   1
#define LV_USE_OBJ_NAME                 1
#define LV_OBJ_ID_AUTO_ASSIGN           LV_USE_OBJ_ID
#define LV_USE_OBJ_ID_BUILTIN           1
#define LV_USE_OBJ_PROPERTY             0
#define LV_USE_OBJ_PROPERTY_NAME        1
#define LV_USE_GESTURE_RECOGNITION      0

/*============================================================================
 * COMPILER SETTINGS
 *============================================================================*/

#define LV_BIG_ENDIAN_SYSTEM            0
#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_SYNC_READY
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE     1
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_FAST_MEM
#define LV_EXPORT_CONST_INT(int_value)  struct _silence_gcc_warning
#define LV_ATTRIBUTE_EXTERN_DATA
#define LV_USE_FLOAT                    1
#define LV_USE_MATRIX                   0
#ifndef LV_USE_PRIVATE_API
    #define LV_USE_PRIVATE_API          0
#endif

/*============================================================================
 * FONT USAGE  (matches embedded config — same fonts available)
 *============================================================================*/

#define LV_FONT_MONTSERRAT_8    0
#define LV_FONT_MONTSERRAT_10   0
#define LV_FONT_MONTSERRAT_12   0
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_22   0
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_MONTSERRAT_26   0
#define LV_FONT_MONTSERRAT_28   1
#define LV_FONT_MONTSERRAT_30   0
#define LV_FONT_MONTSERRAT_32   0
#define LV_FONT_MONTSERRAT_34   0
#define LV_FONT_MONTSERRAT_36   1
#define LV_FONT_MONTSERRAT_38   0
#define LV_FONT_MONTSERRAT_40   0
#define LV_FONT_MONTSERRAT_42   0
#define LV_FONT_MONTSERRAT_44   0
#define LV_FONT_MONTSERRAT_46   0
#define LV_FONT_MONTSERRAT_48   1
#define LV_FONT_MONTSERRAT_28_COMPRESSED    0
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    0
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK   0
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK   0
#define LV_FONT_UNSCII_8        1
#define LV_FONT_UNSCII_16       0
#define LV_FONT_CUSTOM_DECLARE
#define LV_FONT_DEFAULT         LV_FONT_DEFAULT_MONTSERRAT_14
#define LV_FONT_FMT_TXT_LARGE   0
#define LV_USE_FONT_COMPRESSED  0
#define LV_USE_FONT_PLACEHOLDER 1

/*============================================================================
 * TEXT SETTINGS
 *============================================================================*/

#define LV_TXT_ENC                      LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS              " ,.;:-_)]}"
#define LV_TXT_LINE_BREAK_LONG_LEN      0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
#define LV_USE_BIDI                     0
#define LV_USE_ARABIC_PERSIAN_CHARS     0
#define LV_TXT_COLOR_CMD                "#"

/*============================================================================
 * WIDGETS  (enable all — mirrors the embedded config)
 *============================================================================*/

#define LV_WIDGETS_HAS_DEFAULT_VALUE    1
#define LV_USE_ANIMIMG      1
#define LV_USE_ARC          1
#define LV_USE_BAR          1
#define LV_USE_BUTTON       1
#define LV_USE_BUTTONMATRIX 1
#define LV_USE_CALENDAR     1
#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY  0
    #define LV_CALENDAR_DEFAULT_DAY_NAMES   {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January","February","March","April","May","June","July","August","September","October","November","December"}
    #define LV_USE_CALENDAR_HEADER_ARROW    1
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
    #define LV_USE_CALENDAR_CHINESE         0
#endif
#define LV_USE_CANVAS       1
#define LV_USE_CHART        1
#define LV_USE_CHECKBOX     1
#define LV_USE_DROPDOWN     1
#define LV_USE_IMAGE        1
#define LV_USE_IMAGEBUTTON  1
#define LV_USE_KEYBOARD     1
#define LV_USE_LABEL        1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION     1
    #define LV_LABEL_LONG_TXT_HINT      1
    #define LV_LABEL_WAIT_CHAR_COUNT    3
#endif
#define LV_USE_LED          1
#define LV_USE_LINE         1
#define LV_USE_LIST         1
#define LV_USE_LOTTIE       0
#define LV_USE_MENU         1
#define LV_USE_MSGBOX       1
#define LV_USE_ROLLER       1
#define LV_USE_SCALE        1
#define LV_USE_SLIDER       1
#define LV_USE_SPAN         1
#if LV_USE_SPAN
    #define LV_SPAN_SNIPPET_STACK_SIZE  64
#endif
#define LV_USE_SPINBOX      1
#define LV_USE_SPINNER      1
#define LV_USE_SWITCH       1
#define LV_USE_TABLE        1
#define LV_USE_TABVIEW      1
#define LV_USE_TEXTAREA     1
#if LV_USE_TEXTAREA
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME   1500
#endif
#define LV_USE_TILEVIEW     1
#define LV_USE_WIN          1
#define LV_USE_3DTEXTURE    0

/*============================================================================
 * THEMES
 *============================================================================*/

#define LV_USE_THEME_DEFAULT    1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK               0
    #define LV_THEME_DEFAULT_GROW               1
    #define LV_THEME_DEFAULT_TRANSITION_TIME    80
#endif
#define LV_USE_THEME_SIMPLE     1
#define LV_USE_THEME_MONO       0

/*============================================================================
 * LAYOUTS
 *============================================================================*/

#define LV_USE_FLEX     1
#define LV_USE_GRID     1

/*============================================================================
 * FILE SYSTEM
 *============================================================================*/

#define LV_FS_DEFAULT_DRIVER_LETTER '\0'
#define LV_USE_FS_STDIO     1
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER      'A'
    #define LV_FS_STDIO_PATH        ""
    #define LV_FS_STDIO_CACHE_SIZE  0
#endif
#define LV_USE_FS_POSIX     0
#define LV_USE_FS_WIN32     0
#define LV_USE_FS_FATFS     0
#define LV_USE_FS_MEMFS     0
#define LV_USE_FS_LITTLEFS  0

/*============================================================================
 * SDL2 DISPLAY + INPUT DRIVER
 *============================================================================*/

#define LV_USE_SDL              1
#if LV_USE_SDL
    #define LV_SDL_INCLUDE_PATH         "SDL2/SDL.h"
    #define LV_SDL_BACKEND              LV_SDL_BACKEND_SW
    #define LV_SDL_RENDER_MODE          LV_DISPLAY_RENDER_MODE_DIRECT
    #define LV_SDL_BUF_COUNT            1
    #define LV_SDL_ACCELERATED          0   /* SW renderer for portability */
    #define LV_SDL_FULLSCREEN           0
    #define LV_SDL_DIRECT_EXIT          1
    #define LV_SDL_MOUSEWHEEL_MODE      LV_SDL_MOUSEWHEEL_MODE_ENCODER
#endif

/*============================================================================
 * MISC DRIVERS (all off)
 *============================================================================*/

#define LV_USE_LODEPNG          0
#define LV_USE_SJPG             0
#define LV_USE_TJPG             0
#define LV_USE_GIF              0
#define LV_USE_QRCODE           0
#define LV_USE_BARCODE          0
#define LV_USE_FREETYPE         0
#define LV_USE_TINY_TTF         0
#define LV_USE_RLOTTIE          0
#define LV_USE_FFMPEG           0
#define LV_USE_THORVG           0
#define LV_USE_LZ4              0
#define LV_USE_SNAPSHOT         0
#define LV_USE_SYSMON           0
#define LV_USE_PROFILER         0
#define LV_USE_MONKEY           0
#define LV_USE_GRIDNAV          0
#define LV_USE_MSGBOX           1
#define LV_USE_IME_PINYIN       0
#define LV_USE_ARABIC_PERSIAN_CHARS 0
#define LV_USE_OBSERVER         0
#define LV_USE_XML              0
#define LV_USE_DRAW_SDL         0

#endif /* LV_CONF_H */
#endif /* if 1 */
