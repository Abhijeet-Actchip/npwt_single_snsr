/**
  ******************************************************************************
  * @file    eeprom_emulation/interface_time/eeprom_itftime_gettick.c
  * @brief   This file provides an implementation of the time interface based on
  *          the HAL tick (HAL_GetTick).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "eeprom_itf_time.h"

/** @addtogroup EEPROM_EMULATION
  * @{
  */

/** @addtogroup EEPROM_EMULATION_INTERFACE
  * @{
  */

/** @addtogroup EEPROM_EMULATION_INTERFACE_TIME
  * @{
  */

/** @defgroup EEPROM_EMULATION_INTERFACE_TIME_GETTICK Interface TIME HAL Tick
  * @note this file contains the implementation based on the HAL tick (HAL_GetTick)
  * @{
  */

/* Exported functions --------------------------------------------------------*/

/**
  * @addtogroup EEPROM_EMULATION_INTERFACE_TIME_FUNCTIONS
  * @{
  */

/** @defgroup EEPROM_EMULATION_INTERFACE_TIME_GETTICK_APIS Interface TIME HAL Tick APIs
  * @{
  */

/**
  * @brief  Initialize the time interface context.
  * @param  time_object Time base instance used by the interface (unused with the HAL tick time base).
  * @retval Possible return values: see @ref ee_itf_time_status.
  */
ee_itf_time_status EE_ITF_TIME_Init(void *time_object)
{
  /* The HAL tick time base does not require any instance */
  (void)(time_object);
  return EE_ITF_TIME_OK;
}

/**
  * @brief  Return the current time base value, in milliseconds.
  * @retval uint32_t free-running millisecond counter provided by HAL_GetTick().
  */
uint32_t EE_ITF_TIME_Get(void)
{
  return HAL_GetTick();
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
