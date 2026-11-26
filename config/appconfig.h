/*
 * appconfig.h
 *
 *  Created on: 24-Nov-2026
 *      Author: Abhijeet M
 */

#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__             1

#ifdef __cplusplus
extern "C" {
#endif

#include "test_macros.h"

/* Define Motor Polarity */
#define VAC_MOTOR_POL_HIGH			0

/* Define Board-ID macros here. */
#define BOARD_ID_V1_0				1

/* Define application specific settings macros here. */
#define SELECT_BOARD                BOARD_ID_V1_0

/* Display resolution. */
#define DISP_HOR_RES				320
#define DISP_VER_RES				240

/* Task priorities and stack sizes. */
#define GUI_TICK_PRIORITY			(tskIDLE_PRIORITY + 5)
#define GUI_TICK_STACK_SIZE			128

#define THERAPY_MGR_PRIORITY		(tskIDLE_PRIORITY + 3)
#define THERAPY_MGR_STACK_SIZE		512

#define APP_MGR_PRIORITY			(tskIDLE_PRIORITY + 2)
#define APP_MGR_STACK_SIZE			256

#define GUI_MGR_PRIORITY			(tskIDLE_PRIORITY + 1)
#define GUI_MGR_STACK_SIZE			1024

#define DIAG_SVC_PRIORITY			(tskIDLE_PRIORITY + 1)
#define DIAG_SVC_STACK_SIZE			512

/*!< Enable Watchdog timer. (Should be enabled on production builds
 * disabled when debugging.) */
#define APP_WDT_EN					0

/* Error codes returned by functions. */
#define APP_ERR_NONE                0
#define APP_ERR_INV_CMDID           -1
#define APP_ERR_UNKNOWN             -2
#define APP_ERR_DUP_VAL             -3
#define APP_ERR_INV_LEN             -4
#define APP_ERR_INV_ARG             -5
#define APP_ERR_NOT_FOUND			-6
#define APP_ERR_TIMEOUT             -7
#define APP_ERR_INV_VAL        		-8
#define APP_ERR_NO_MEM              -9
#define APP_ERR_RESP_INV			-12
#define APP_ERR_NO_ACK				-13
#define APP_ERR_STHAL				-14
#define APP_ERR_INV_STATE			-15
#define APP_ERR_BUS_BUSY			-16
#define APP_ERR_NO_RESP				-17

/******************** Advanced user settings below (DO NOT EDIT) ********************/

#define ARRAY_SIZE(foo)             (sizeof(foo)/sizeof(foo[0]))

/* Set a bit in a variable */
#define SBI(val, bit)               val |= (1 << bit)
/* Clear a bit in a variable */
#define CBI(val, bit)               val &= ~(1 << bit)
/* Check if a bit is set in a variable */
#define BIT_IS_SET(val, bit)         ((val) & (1 << (bit)))


#ifdef TEST
    #define STATIC
#else
    #define STATIC static
#endif

#ifndef SELECT_BOARD
    #error "Board not selected."
#endif

#ifdef __cplusplus
}
#endif

#endif /* __APPCONFIG_H__ */
