/**
  ******************************************************************************
  * @file    eeprom_emulation/interface_time/eeprom_itf_time.h
  * @brief   This file contains all the functions prototypes for the time interface
  *          of the eeprom emulation.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EEPROM_ITF_TIME_H
#define EEPROM_ITF_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"

/** @addtogroup EEPROM_EMULATION
  * @{
  */

/** @addtogroup EEPROM_EMULATION_INTERFACE
  * @{
  */

/** @defgroup EEPROM_EMULATION_INTERFACE_TIME Interface TIME
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup EEPROM_EMULATION_INTERFACE_TIME_TYPEDEF Interface TIME Types Definition
  * @{
  */

/**
  * @brief Define the status values returned by the TIME interface.
  */
typedef enum
{
  EE_ITF_TIME_OK    = 0,    /*!< TIME interface operation successful              */
  EE_ITF_TIME_ERROR = -1    /*!< An error has been detected in the TIME interface */
} ee_itf_time_status;

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @defgroup EEPROM_EMULATION_INTERFACE_TIME_FUNCTIONS Interface TIME Functions
  * @{
  */

ee_itf_time_status EE_ITF_TIME_Init(void *time_object);
uint32_t EE_ITF_TIME_Get(void);

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
#ifdef __cplusplus
}
#endif
#endif /* EEPROM_ITF_TIME_H */
