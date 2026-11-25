/**
  ******************************************************************************
  * @file           : mx_gpio_default.h
  * @brief          : Header for mx_gpio_default.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the mx_stm32c5xx_hal_drivers_license.md file
  * in the same directory as the generated code.
  * If no mx_stm32c5xx_hal_drivers_license.md file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX_GPIO_DEFAULT_H
#define MX_GPIO_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"
#include "mx_def.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/******************************************************************************/
/* Exported defines for gpio_default in HAL layer                             */
/******************************************************************************/

/* Primary aliases for GPIO PA2 pin */
#define VBAT_DIV_EN_PORT                                HAL_GPIOA
#define VBAT_DIV_EN_PIN                                 HAL_GPIO_PIN_2

/* Primary aliases for GPIO PA3 pin */
#define BOOST_13V_EN_PORT                               HAL_GPIOA
#define BOOST_13V_EN_PIN                                HAL_GPIO_PIN_3

/* Primary aliases for GPIO PA4 pin */
#define BACKLIGHT_CTRL_PORT                             HAL_GPIOA
#define BACKLIGHT_CTRL_PIN                              HAL_GPIO_PIN_4

/* Primary aliases for GPIO PA8 pin */
#define DI_2_PORT                                       HAL_GPIOA
#define DI_2_PIN                                        HAL_GPIO_PIN_8

/* Primary aliases for GPIO PA9 pin */
#define VALVE_CTRL_EN_PORT                              HAL_GPIOA
#define VALVE_CTRL_EN_PIN                               HAL_GPIO_PIN_9

/* Primary aliases for GPIO PA10 pin */
#define VACCUM_CTRL_EN_PORT                             HAL_GPIOA
#define VACCUM_CTRL_EN_PIN                              HAL_GPIO_PIN_10
#define VACCUM_CTRL_EN_INIT_STATE                       HAL_GPIO_PIN_RESET
#define VACCUM_CTRL_EN_ACTIVE_STATE                     HAL_GPIO_PIN_SET
#define VACCUM_CTRL_EN_INACTIVE_STATE                   HAL_GPIO_PIN_RESET

/* Primary aliases for GPIO PB0 pin */
#define DISPLAY_CS_PORT                                 HAL_GPIOB
#define DISPLAY_CS_PIN                                  HAL_GPIO_PIN_0
#define DISPLAY_CS_INIT_STATE                           HAL_GPIO_PIN_RESET
#define DISPLAY_CS_ACTIVE_STATE                         HAL_GPIO_PIN_SET
#define DISPLAY_CS_INACTIVE_STATE                       HAL_GPIO_PIN_RESET

/* Primary aliases for GPIO PB1 pin */
#define DISPLAY_RESET_PORT                              HAL_GPIOB
#define DISPLAY_RESET_PIN                               HAL_GPIO_PIN_1
#define DISPLAY_RESET_INIT_STATE                        HAL_GPIO_PIN_RESET
#define DISPLAY_RESET_ACTIVE_STATE                      HAL_GPIO_PIN_SET
#define DISPLAY_RESET_INACTIVE_STATE                    HAL_GPIO_PIN_RESET

/* Primary aliases for GPIO PB2 pin */
#define DISPLAY_RS_PORT                                 HAL_GPIOB
#define DISPLAY_RS_PIN                                  HAL_GPIO_PIN_2

/* Primary aliases for GPIO PB4(NJTRST) pin */
#define DI_4_PORT                                       HAL_GPIOB
#define DI_4_PIN                                        HAL_GPIO_PIN_4

/* Primary aliases for GPIO PB5 pin */
#define DI_3_PORT                                       HAL_GPIOB
#define DI_3_PIN                                        HAL_GPIO_PIN_5

/* Primary aliases for GPIO PB7 pin */
#define DI_1_PORT                                       HAL_GPIOB
#define DI_1_PIN                                        HAL_GPIO_PIN_7

/* Primary aliases for GPIO PB8 pin */
#define CHRG_STS_PORT                                   HAL_GPIOB
#define CHRG_STS_PIN                                    HAL_GPIO_PIN_8

/* Primary aliases for GPIO PB9 pin */
#define DONE_STS_PORT                                   HAL_GPIOB
#define DONE_STS_PIN                                    HAL_GPIO_PIN_9
#define DONE_STS_INIT_STATE                             HAL_GPIO_PIN_RESET
#define DONE_STS_ACTIVE_STATE                           HAL_GPIO_PIN_SET
#define DONE_STS_INACTIVE_STATE                         HAL_GPIO_PIN_RESET

/* Primary aliases for GPIO PB10 pin */
#define DISPLAY_PWR_EN_PORT                             HAL_GPIOB
#define DISPLAY_PWR_EN_PIN                              HAL_GPIO_PIN_10
#define DISPLAY_PWR_EN_INIT_STATE                       HAL_GPIO_PIN_RESET
#define DISPLAY_PWR_EN_ACTIVE_STATE                     HAL_GPIO_PIN_SET
#define DISPLAY_PWR_EN_INACTIVE_STATE                   HAL_GPIO_PIN_RESET

/* Primary aliases for GPIO PB15 pin */
#define DI_5_PORT                                       HAL_GPIOB
#define DI_5_PIN                                        HAL_GPIO_PIN_15

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/******************************************************************************/
/* Exported functions for gpio_default in HAL layer                           */
/******************************************************************************/
/**
  * @brief mx_gpio_default init function
  * This function configures the hardware resources used in this example
  * @retval 0  GPIO group correctly initialized
  * @retval -1 Issue detected during GPIO group initialization
  */
system_status_t mx_gpio_default_init(void);

/**
  * @brief  De-initialize gpio_default instance.
  */
system_status_t mx_gpio_default_deinit(void);

/**
  * @brief  Get the EXTI8 object.
  * @retval Pointer on the EXTI8 Handle
  */
hal_exti_handle_t *mx_gpio_default_exti8_gethandle(void);

/**
  * @brief  Get the EXTI9 object.
  * @retval Pointer on the EXTI9 Handle
  */
hal_exti_handle_t *mx_gpio_default_exti9_gethandle(void);

/**
  * @brief  Get the EXTI4 object.
  * @retval Pointer on the EXTI4 Handle
  */
hal_exti_handle_t *mx_gpio_default_exti4_gethandle(void);

/**
  * @brief  Get the EXTI5 object.
  * @retval Pointer on the EXTI5 Handle
  */
hal_exti_handle_t *mx_gpio_default_exti5_gethandle(void);

/**
  * @brief  Get the EXTI7 object.
  * @retval Pointer on the EXTI7 Handle
  */
hal_exti_handle_t *mx_gpio_default_exti7_gethandle(void);

/**
  * @brief  Get the EXTI15 object.
  * @retval Pointer on the EXTI15 Handle
  */
hal_exti_handle_t *mx_gpio_default_exti15_gethandle(void);

/******************************************************************************/
/*                            EXTI Line8 interrupt                            */
/******************************************************************************/
void EXTI8_IRQHandler(void);

/******************************************************************************/
/*                            EXTI Line9 interrupt                            */
/******************************************************************************/
void EXTI9_IRQHandler(void);

/******************************************************************************/
/*                            EXTI Line4 interrupt                            */
/******************************************************************************/
void EXTI4_IRQHandler(void);

/******************************************************************************/
/*                            EXTI Line5 interrupt                            */
/******************************************************************************/
void EXTI5_IRQHandler(void);

/******************************************************************************/
/*                            EXTI Line7 interrupt                            */
/******************************************************************************/
void EXTI7_IRQHandler(void);

/******************************************************************************/
/*                           EXTI Line15 interrupt                            */
/******************************************************************************/
void EXTI15_IRQHandler(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MX_GPIO_DEFAULT_H */
