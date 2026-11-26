/*
 * test_macros.h
 *
 *  Created on: 24-Nov-2026
 *      Author: Abhijeet M
 */

#ifndef __TEST_MACROS_H__
#define __TEST_MACROS_H__				1

#ifdef __cplusplus
extern "C" {
#endif

/* Enable these macros to run manual tests. */
#define RUN_VACUUM_MOTOR_TEST			0
#define RUN_SENSOR_TEST					0
#define RUN_SWITCHES_TEST				0
#define RUN_GPIO_OUTPUT_TEST			0
#define RUN_GPIO_INPUT_TEST				0
#define RUN_PRESS_REGULATOR_TEST		0
#define RUN_PRESSURE_PER_VOLTAGE_TEST	0
#define RUN_FLASH_WRITE_TEST			0
#define RUN_FLASH_READ_TEST				0
#define RUN_BATTERY_CHARGING_TEST		0
#define RUN_DISPLAY_TEXT_TEST			0
#define RUN_DISPLAY_BUTTONS_TEST		0


#ifdef __cplusplus
}
#endif

#endif /* __TEST_MACROS_H__ */
