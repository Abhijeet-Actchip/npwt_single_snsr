/*
 * board_v1_0.c
 *
 *  Created on: 24-Nov-2026
 *      Author: Abhijeet M
 */


#include "board_v1_0.h"

#if(SELECT_BOARD == BOARD_ID_V1_0)

#include <stdio.h>
#include "app_hal/app_hal_spi.h"
#include "app_hal/app_hal_wdt.h"
#include "../app_utils/trice/trice.h"

/* ADC Constants */
#define ADC_RESOLUTION							4095.0f
#define ADC_REFERENCE_V							3.3f
/* Voltage reference pin is given with 3.3 V */
//#define ADC_REFERENCE_V							3.36667f
#define ADC_STEPSIZE							(ADC_REFERENCE_V / ADC_RESOLUTION)
#define ADC_CONVTIMEOUT_MS						1

/* Voltage Divide ratios According to Schematic */
#define VOLTAGE_DIV_BATTERY						(1.0f / 4.65f)

/* Voltage Multiplier ratios to get Voltage without division */
#define VOLTAGE_MUL_BATTERY						(1.0f / VOLTAGE_DIV_BATTERY)


static spi_config_t spi1Config;

int8_t Board_InitPeripherals(void)
{
	int8_t retVal = APP_ERR_INV_STATE;

	/* Enable DWT CYCCNT for Trice timestamps (Cortex-M33 / STM32C5).
	 * ARMv8-M ITM has no LAR; TRCENA is sufficient to access DWT. */
	DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0U;
	DWT->CTRL &= ~DWT_CTRL_CYCDISS_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	TriceInit();

	if (mx_system_init() != SYSTEM_OK)
	{
		return retVal;
	}

	spi1Config.hspi = mx_spi1_gethandle();
	retVal = APPHAL_SPI_Init(BOARD_SPI1_PID, &spi1Config);
	if(retVal != APP_ERR_NONE)
	{
		return retVal;
	}

	/* Keep the valve closed */
	Board_CtrlValve(0);

	#if (APP_WDT_EN == 1)
		/** TODO: Init IWDT and set it as non callable, so that, it can be manually called here,
		 *        instead of being called in mx_system_init	*/
	#endif

	return retVal;
}

static float PresKPaTommHG(float presKPa)
{
	return (presKPa / 0.1333223684);
}

/**
 * @brief: Converts the ADC value to Output voltage of Pressure sensor,
 * 		   and converts the voltage int pressure (in KPa)
 * @param: ADC value
 * @return: Pressure (in KPa).
 */
static float ADCtoPressureKPa(uint32_t adcVal)
{
	float Vpres = (adcVal * ADC_STEPSIZE); // Vout of Pressure sensor.
	/* for VDD 3V3
	 * Pressure = (Vout - 2.7) / K, where K = 0.025 (for Pressure range -100 to 0)
	 */
	return ((Vpres - 2.7) / 0.025);
}

/**
 * @brief: Converts the ADC value to Actual Battery Voltage, with divider.
 * @param: ADC value
 * @return: Battery (in V)
 */
static float ADCtoBatteryV(uint32_t adcVal)
{
	return (adcVal * ADC_STEPSIZE) * VOLTAGE_MUL_BATTERY;
}

int8_t Board_LedPwrOnCtrl(bool en)
{
//	HAL_GPIO_WritePin(LED_PWR_ON_GPIO_Port, LED_PWR_ON_Pin, en);
	/** NOTE: Directly connected to power rail */
	return APP_ERR_NONE;
}

int8_t Board_EnableBatVDiv(bool en)
{
	HAL_GPIO_WritePin(VBAT_DIV_EN_PORT, VBAT_DIV_EN_PIN, en);
	return APP_ERR_NONE;
}

int8_t Board_EnableBoostConv(bool en)
{
	HAL_GPIO_WritePin(BOOST_13V_EN_PORT, BOOST_13V_EN_PIN, !en);
	return APP_ERR_NONE;
}

int8_t Board_EnableDisplayPwr(bool en)
{
	HAL_GPIO_WritePin(DISPLAY_PWR_EN_PORT, DISPLAY_PWR_EN_PIN, en);
	return APP_ERR_NONE;
}

int8_t Board_CtrlDisplayBacklight(bool en)
{
	HAL_GPIO_WritePin(BACKLIGHT_CTRL_PORT, BACKLIGHT_CTRL_PIN, en);
	return APP_ERR_NONE;
}

int8_t Board_EnableVacuumMot(bool en)
{
	HAL_GPIO_WritePin(VACCUM_CTRL_EN_PORT, VACCUM_CTRL_EN_PIN, en);
	return APP_ERR_NONE;
}

bool Board_GetBatChrgSts(void)
{
	return HAL_GPIO_ReadPin(CHRG_STS_PORT, CHRG_STS_PIN);
}

bool Board_GetBatDoneSts(void)
{
	return HAL_GPIO_ReadPin(DONE_STS_PORT, DONE_STS_PIN);
}

bool Board_GetDIN1(void)
{
	return HAL_GPIO_ReadPin(DI_1_PORT, DI_1_PIN);
}

bool Board_GetDIN2(void)
{
	return HAL_GPIO_ReadPin(DI_2_PORT, DI_2_PIN);
}

bool Board_GetDIN3(void)
{
	return HAL_GPIO_ReadPin(DI_3_PORT, DI_3_PIN);
}

bool Board_GetDIN4(void)
{
	return HAL_GPIO_ReadPin(DI_4_PORT, DI_4_PIN);
}

bool Board_GetDIN5(void)
{
	return HAL_GPIO_ReadPin(DI_5_PORT, DI_5_PIN);
}

static int8_t SetPWMChannel(hal_tim_handle_t *htim,
                            hal_tim_channel_t channel,
                            hal_tim_oc_compare_unit_t compareUnit,
                            uint8_t dutyPer)
{
	if(htim == NULL)
	{
		return APP_ERR_INV_STATE;
	}

	if(!dutyPer)
	{
		if(HAL_TIM_GetChannelState(htim, channel) == HAL_TIM_OC_CHANNEL_STATE_ACTIVE)
		{
			if(HAL_TIM_OC_StopChannel(htim, channel) != HAL_OK)
			{
				return APP_ERR_STHAL;
			}
		}
		return APP_ERR_NONE;
	}
	else if(dutyPer > 100U)
	{
		dutyPer = 100U;
	}

	uint32_t pulse = ((uint32_t)dutyPer * (HAL_TIM_GetPeriod(htim) + 1U)) / 100U;
	if(HAL_TIM_OC_SetCompareUnitPulse(htim, compareUnit, pulse) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	if(HAL_TIM_GetState(htim) == HAL_TIM_STATE_IDLE)
	{
		if(HAL_TIM_Start(htim) != HAL_OK)
		{
			return APP_ERR_STHAL;
		}
	}

	if(HAL_TIM_GetChannelState(htim, channel) != HAL_TIM_OC_CHANNEL_STATE_ACTIVE)
	{
		if(HAL_TIM_OC_StartChannel(htim, channel) != HAL_OK)
		{
			return APP_ERR_STHAL;
		}
	}

	return APP_ERR_NONE;
}

static int8_t ReadADCChannel(hal_adc_channel_t channel, int32_t *ptrAdcVal)
{
	hal_adc_handle_t *hadc = mx_adc1_gethandle();
	if(hadc == NULL)
	{
		return APP_ERR_INV_STATE;
	}

	hal_adc_reg_config_t regConfig = {
		.trigger_src       = HAL_ADC_REG_TRIG_SOFTWARE,
		.sequencer_length  = 1U,
		.sequencer_discont = HAL_ADC_REG_SEQ_DISCONT_DISABLE,
		.continuous        = HAL_ADC_REG_CONV_SINGLE,
		.overrun           = HAL_ADC_REG_OVR_DATA_OVERWRITTEN
	};

	if(HAL_ADC_REG_SetConfig(hadc, &regConfig) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	hal_adc_channel_config_t chConfig = {
		.group          = HAL_ADC_GROUP_REGULAR,
		.sequencer_rank = 1U,
		.sampling_time  = HAL_ADC_SAMPLING_TIME_289CYCLES,
		.input_mode     = HAL_ADC_IN_SINGLE_ENDED
	};

	if(HAL_ADC_SetConfigChannel(hadc, channel, &chConfig) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	if(HAL_ADC_Start(hadc) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	if(HAL_ADC_REG_StartConv(hadc) != HAL_OK)
	{
		(void)HAL_ADC_Stop(hadc);
		return APP_ERR_STHAL;
	}

	if(HAL_ADC_REG_PollForConv(hadc, ADC_CONVTIMEOUT_MS) != HAL_OK)
	{
		(void)HAL_ADC_REG_StopConv(hadc);
		(void)HAL_ADC_Stop(hadc);
		return APP_ERR_STHAL;
	}

	*ptrAdcVal = HAL_ADC_REG_ReadConversionData(hadc);

	if(HAL_ADC_Stop(hadc) != HAL_OK)
	{
		return APP_ERR_STHAL;
	}

	return APP_ERR_NONE;
}

int8_t Board_CtrlBuzzerPWM(uint8_t pwm)
{
	/* TIM5_CH3 */
	return SetPWMChannel(mx_tim5_gethandle(),
	                     HAL_TIM_CHANNEL_3,
	                     HAL_TIM_OC_COMPARE_UNIT_3,
	                     pwm);
}

int8_t Board_CtrlVacuumMot(uint8_t speedPer)
{
	/* TIM1_CH4 */
	return SetPWMChannel(mx_tim1_gethandle(),
	                     HAL_TIM_CHANNEL_4,
	                     HAL_TIM_OC_COMPARE_UNIT_4,
	                     speedPer);
}

int8_t Board_CtrlValve(bool en)
{
	HAL_GPIO_WritePin(VALVE_CTRL_EN_PORT,VALVE_CTRL_EN_PIN, en);
	return APP_ERR_NONE;
}


int8_t Board_GetVacuumPres(float *ptrPres)
{
	if(ptrPres == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	/* Channel : ADC1_IN1 */
	int32_t adcVal = 0;
	int8_t retVal = ReadADCChannel(HAL_ADC_CHANNEL_1, &adcVal);
	if(retVal != APP_ERR_NONE)
	{
		return retVal;
	}

	*ptrPres = PresKPaTommHG(ADCtoPressureKPa((uint32_t)adcVal));

	return APP_ERR_NONE;
}

int8_t Board_GetBatV(float *ptrBatV)
{
	if(ptrBatV == NULL)
	{
		return APP_ERR_INV_ARG;
	}

	/* Channel : ADC1_IN0 */
	int32_t adcVal = 0;
	int8_t retVal = ReadADCChannel(HAL_ADC_CHANNEL_0, &adcVal);
	if(retVal != APP_ERR_NONE)
	{
		return retVal;
	}

	*ptrBatV = ADCtoBatteryV((uint32_t)adcVal);

	return APP_ERR_NONE;
}


#endif
