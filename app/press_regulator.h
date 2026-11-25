/*
 * press_regulator.h
 *
 *  Created on: Feb 17, 2025
 *      Author: Mahesh Murty
 */

#ifndef __PRESS_REGULATOR_H__
#define __PRESS_REGULATOR_H__				1

#include <stdint.h>
#include <stdbool.h>
#include "snsr_mgr.h"

int8_t PressRegEnable(bool en);
int8_t PressRegRunCtrlLoop(plantData_t *snsrData, float setPressure);

#endif /* __PRESS_REGULATOR_H__ */
