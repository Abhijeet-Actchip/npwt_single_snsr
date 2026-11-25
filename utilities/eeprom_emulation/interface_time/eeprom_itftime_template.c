/**
  ******************************************************************************
  * @file    eeprom_emulation/interface_time/eeprom_itftime_template.c
  * @brief   This file provides a template implementation of the time interface.
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

/** @defgroup EEPROM_EMULATION_INTERFACE_TIME_TEMPLATE Interface TIME Template
  * @note this file is a template to build a custom time interface
  * @{
  */

/* Exported functions --------------------------------------------------------*/

/**
  * @addtogroup EEPROM_EMULATION_INTERFACE_TIME_FUNCTIONS
  * @{
  */

/** @defgroup EEPROM_EMULATION_INTERFACE_TIME_TEMPLATE_APIS Interface TIME Template APIs
  * @{
  */

/**
  * @brief  Initialize the time interface context (template).
  * @param  time_object Time base instance used by the interface.
  * @retval Possible return values: see @ref ee_itf_time_status.
  */
ee_itf_time_status EE_ITF_TIME_Init(void *time_object)
{
  (void)(time_object);
  return EE_ITF_TIME_ERROR;
}

/**
  * @brief  Return the current time base value, in milliseconds (template).
  * @retval uint32_t free-running millisecond counter.
  */
uint32_t EE_ITF_TIME_Get(void)
{
  uint32_t time_value = 0U;
  return time_value;
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
