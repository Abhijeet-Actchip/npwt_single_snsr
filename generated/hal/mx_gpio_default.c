/**
  ******************************************************************************
  * @file           : mx_gpio_default.c
  * @brief          : gpio_default Peripheral initialization
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

/* Includes ------------------------------------------------------------------*/
#include "mx_gpio_default.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported variables by reference -------------------------------------------*/
static hal_exti_handle_t hEXTI8;
static hal_exti_handle_t hEXTI9;
static hal_exti_handle_t hEXTI4;
static hal_exti_handle_t hEXTI5;
static hal_exti_handle_t hEXTI7;
static hal_exti_handle_t hEXTI15;

/******************************************************************************/
/* Exported functions for GPIO in HAL layer                                   */
/******************************************************************************/
system_status_t mx_gpio_default_init(void)
{
  hal_gpio_config_t  gpio_config;

  HAL_RCC_GPIOA_EnableClock();

  HAL_RCC_GPIOB_EnableClock();

  /*
    GPIO pin labels :
    PA2   ---------> VBAT_DIV_EN
    PA3   ---------> BOOST_13V_EN
    PA4   ---------> BACKLIGHT_CTRL
    PA8   ---------> DI_2
    PA9   ---------> VALVE_CTRL_EN
    */
  /* Configure PA2, PA3, PA4, PA8, PA9 GPIO pins in input mode */
  gpio_config.mode            = HAL_GPIO_MODE_INPUT;
  gpio_config.pull            = HAL_GPIO_PULL_NO;
  if (HAL_GPIO_Init(HAL_GPIOA, VBAT_DIV_EN_PIN | BOOST_13V_EN_PIN | BACKLIGHT_CTRL_PIN | DI_2_PIN | VALVE_CTRL_EN_PIN, &gpio_config) != HAL_OK)
  {
    return SYSTEM_PERIPHERAL_ERROR;
  }

  hal_exti_config_t exti_config;

  /* Initialize the EXTI for line 8 */
  HAL_EXTI_Init(&hEXTI8, HAL_EXTI_LINE_8);

  /* Set the trigger as RISING for the GPIOA */
  exti_config.trigger   = HAL_EXTI_TRIGGER_RISING;
  exti_config.gpio_port = HAL_EXTI_GPIOA;
  HAL_EXTI_SetConfig(&hEXTI8, &exti_config);

  /* Set line 8 Interrupt priority */
  HAL_CORTEX_NVIC_SetPriority(EXTI8_IRQn, HAL_CORTEX_NVIC_PREEMP_PRIORITY_5, HAL_CORTEX_NVIC_SUB_PRIORITY_0);
  HAL_CORTEX_NVIC_EnableIRQ(EXTI8_IRQn);

  /* Initialize the EXTI for line 9 */
  HAL_EXTI_Init(&hEXTI9, HAL_EXTI_LINE_9);

  /* Set the trigger as RISING for the GPIOA */
  exti_config.trigger   = HAL_EXTI_TRIGGER_RISING;
  exti_config.gpio_port = HAL_EXTI_GPIOA;
  HAL_EXTI_SetConfig(&hEXTI9, &exti_config);

  /* Set line 9 Interrupt priority */
  HAL_CORTEX_NVIC_SetPriority(EXTI9_IRQn, HAL_CORTEX_NVIC_PREEMP_PRIORITY_5, HAL_CORTEX_NVIC_SUB_PRIORITY_0);
  HAL_CORTEX_NVIC_EnableIRQ(EXTI9_IRQn);

  /*
    GPIO pin labels :
    PA10  ---------> VACCUM_CTRL_EN
    */
  /* Configure PA10 GPIO pin in output mode */
  gpio_config.mode            = HAL_GPIO_MODE_OUTPUT;
  gpio_config.speed           = HAL_GPIO_SPEED_FREQ_LOW;
  gpio_config.pull            = HAL_GPIO_PULL_NO;
  gpio_config.output_type     = HAL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.init_state      = VACCUM_CTRL_EN_INIT_STATE;
  if (HAL_GPIO_Init(VACCUM_CTRL_EN_PORT, VACCUM_CTRL_EN_PIN, &gpio_config) != HAL_OK)
  {
    return SYSTEM_PERIPHERAL_ERROR;
  }

  /*
    GPIO pin labels :
    PB0   ---------> DISPLAY_CS
    PB1   ---------> DISPLAY_RESET
    PB9   ---------> DONE_STS
    PB10  ---------> DISPLAY_PWR_EN
    */
  /* Configure PB0, PB1, PB9, PB10 GPIO pins in output mode */
  gpio_config.mode            = HAL_GPIO_MODE_OUTPUT;
  gpio_config.speed           = HAL_GPIO_SPEED_FREQ_LOW;
  gpio_config.pull            = HAL_GPIO_PULL_NO;
  gpio_config.output_type     = HAL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.init_state      = HAL_GPIO_PIN_RESET;
  if (HAL_GPIO_Init(HAL_GPIOB, DISPLAY_CS_PIN | DISPLAY_RESET_PIN | DONE_STS_PIN | DISPLAY_PWR_EN_PIN, &gpio_config) != HAL_OK)
  {
    return SYSTEM_PERIPHERAL_ERROR;
  }

  /*
    GPIO pin labels :
    PB2   ---------> DISPLAY_RS
    PB4   ---------> DI_4
    PB5   ---------> DI_3
    PB7   ---------> DI_1
    PB8   ---------> CHRG_STS
    PB15  ---------> DI_5
    */
  /* Configure PB2, PB4, PB5, PB7, PB8, PB15 GPIO pins in input mode */
  gpio_config.mode            = HAL_GPIO_MODE_INPUT;
  gpio_config.pull            = HAL_GPIO_PULL_NO;
  if (HAL_GPIO_Init(HAL_GPIOB, DISPLAY_RS_PIN | DI_4_PIN | DI_3_PIN | DI_1_PIN | CHRG_STS_PIN | DI_5_PIN, &gpio_config) != HAL_OK)
  {
    return SYSTEM_PERIPHERAL_ERROR;
  }

  /* Initialize the EXTI for line 4 */
  HAL_EXTI_Init(&hEXTI4, HAL_EXTI_LINE_4);

  /* Set the trigger as RISING for the GPIOB */
  exti_config.trigger   = HAL_EXTI_TRIGGER_RISING;
  exti_config.gpio_port = HAL_EXTI_GPIOB;
  HAL_EXTI_SetConfig(&hEXTI4, &exti_config);

  /* Set line 4 Interrupt priority */
  HAL_CORTEX_NVIC_SetPriority(EXTI4_IRQn, HAL_CORTEX_NVIC_PREEMP_PRIORITY_5, HAL_CORTEX_NVIC_SUB_PRIORITY_0);
  HAL_CORTEX_NVIC_EnableIRQ(EXTI4_IRQn);

  /* Initialize the EXTI for line 5 */
  HAL_EXTI_Init(&hEXTI5, HAL_EXTI_LINE_5);

  /* Set the trigger as RISING for the GPIOB */
  exti_config.trigger   = HAL_EXTI_TRIGGER_RISING;
  exti_config.gpio_port = HAL_EXTI_GPIOB;
  HAL_EXTI_SetConfig(&hEXTI5, &exti_config);

  /* Set line 5 Interrupt priority */
  HAL_CORTEX_NVIC_SetPriority(EXTI5_IRQn, HAL_CORTEX_NVIC_PREEMP_PRIORITY_5, HAL_CORTEX_NVIC_SUB_PRIORITY_0);
  HAL_CORTEX_NVIC_EnableIRQ(EXTI5_IRQn);

  /* Initialize the EXTI for line 7 */
  HAL_EXTI_Init(&hEXTI7, HAL_EXTI_LINE_7);

  /* Set the trigger as RISING for the GPIOB */
  exti_config.trigger   = HAL_EXTI_TRIGGER_RISING;
  exti_config.gpio_port = HAL_EXTI_GPIOB;
  HAL_EXTI_SetConfig(&hEXTI7, &exti_config);

  /* Set line 7 Interrupt priority */
  HAL_CORTEX_NVIC_SetPriority(EXTI7_IRQn, HAL_CORTEX_NVIC_PREEMP_PRIORITY_5, HAL_CORTEX_NVIC_SUB_PRIORITY_0);
  HAL_CORTEX_NVIC_EnableIRQ(EXTI7_IRQn);

  /* Initialize the EXTI for line 15 */
  HAL_EXTI_Init(&hEXTI15, HAL_EXTI_LINE_15);

  /* Set the trigger as RISING for the GPIOB */
  exti_config.trigger   = HAL_EXTI_TRIGGER_RISING;
  exti_config.gpio_port = HAL_EXTI_GPIOB;
  HAL_EXTI_SetConfig(&hEXTI15, &exti_config);

  /* Set line 15 Interrupt priority */
  HAL_CORTEX_NVIC_SetPriority(EXTI15_IRQn, HAL_CORTEX_NVIC_PREEMP_PRIORITY_5, HAL_CORTEX_NVIC_SUB_PRIORITY_0);
  HAL_CORTEX_NVIC_EnableIRQ(EXTI15_IRQn);

  return SYSTEM_OK;
}

system_status_t mx_gpio_default_deinit(void)
{
  /* De-initialize the EXTI for GPIOA line8 */
  HAL_EXTI_DeInit(&hEXTI8);

  /* set line 8 Interrupt priority */
  HAL_CORTEX_NVIC_DisableIRQ(EXTI8_IRQn);
  /* De-initialize the EXTI for GPIOA line9 */
  HAL_EXTI_DeInit(&hEXTI9);

  /* set line 9 Interrupt priority */
  HAL_CORTEX_NVIC_DisableIRQ(EXTI9_IRQn);

  /* De-initialize the EXTI for GPIOB line4 */
  HAL_EXTI_DeInit(&hEXTI4);

  /* set line 4 Interrupt priority */
  HAL_CORTEX_NVIC_DisableIRQ(EXTI4_IRQn);
  /* De-initialize the EXTI for GPIOB line5 */
  HAL_EXTI_DeInit(&hEXTI5);

  /* set line 5 Interrupt priority */
  HAL_CORTEX_NVIC_DisableIRQ(EXTI5_IRQn);
  /* De-initialize the EXTI for GPIOB line7 */
  HAL_EXTI_DeInit(&hEXTI7);

  /* set line 7 Interrupt priority */
  HAL_CORTEX_NVIC_DisableIRQ(EXTI7_IRQn);
  /* De-initialize the EXTI for GPIOB line15 */
  HAL_EXTI_DeInit(&hEXTI15);

  /* set line 15 Interrupt priority */
  HAL_CORTEX_NVIC_DisableIRQ(EXTI15_IRQn);

  /* De-initialize pins of GPIOA port */
  HAL_GPIO_DeInit(HAL_GPIOA, VBAT_DIV_EN_PIN | BOOST_13V_EN_PIN | BACKLIGHT_CTRL_PIN | DI_2_PIN | VALVE_CTRL_EN_PIN | VACCUM_CTRL_EN_PIN);

  /* De-initialize pins of GPIOB port */
  HAL_GPIO_DeInit(HAL_GPIOB, DISPLAY_CS_PIN | DISPLAY_RESET_PIN | DISPLAY_RS_PIN | DI_4_PIN | DI_3_PIN | DI_1_PIN | CHRG_STS_PIN | DONE_STS_PIN | DISPLAY_PWR_EN_PIN | DI_5_PIN);

  return SYSTEM_OK;
}

hal_exti_handle_t *mx_gpio_default_exti8_gethandle(void)
{
  return &hEXTI8;
}

hal_exti_handle_t *mx_gpio_default_exti9_gethandle(void)
{
  return &hEXTI9;
}

hal_exti_handle_t *mx_gpio_default_exti4_gethandle(void)
{
  return &hEXTI4;
}

hal_exti_handle_t *mx_gpio_default_exti5_gethandle(void)
{
  return &hEXTI5;
}

hal_exti_handle_t *mx_gpio_default_exti7_gethandle(void)
{
  return &hEXTI7;
}

hal_exti_handle_t *mx_gpio_default_exti15_gethandle(void)
{
  return &hEXTI15;
}

/******************************************************************************/
/*                            EXTI Line8 interrupt                            */
/******************************************************************************/
void EXTI8_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hEXTI8);
}

/******************************************************************************/
/*                            EXTI Line9 interrupt                            */
/******************************************************************************/
void EXTI9_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hEXTI9);
}

/******************************************************************************/
/*                            EXTI Line4 interrupt                            */
/******************************************************************************/
void EXTI4_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hEXTI4);
}

/******************************************************************************/
/*                            EXTI Line5 interrupt                            */
/******************************************************************************/
void EXTI5_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hEXTI5);
}

/******************************************************************************/
/*                            EXTI Line7 interrupt                            */
/******************************************************************************/
void EXTI7_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hEXTI7);
}

/******************************************************************************/
/*                           EXTI Line15 interrupt                            */
/******************************************************************************/
void EXTI15_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hEXTI15);
}
