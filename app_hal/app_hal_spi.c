/*
 * app_hal_spi.c
 *
 *  Created on: Sep 16, 2024
 *      Author: Mahesh Murty
 */

#include "app_hal_spi.h"
#include "task.h"

/*!
 *  \brief Maximum number of SPI ports supported by this driver.
 */
#define MAX_SPIS				1

static spi_config_t *perConfig[MAX_SPIS] = { NULL };

/*!
 * \brief Retrieve the configuration structure associated with a SPI handle.
 * \param hspi Pointer to the SPI handle.
 * \return Pointer to the configuration structure or NULL if not found.
 */
static spi_config_t *GetConfig(hal_spi_handle_t *hspi)
{
	spi_config_t *config = NULL;

	for(uint8_t i = 0; i < MAX_SPIS; i++)
	{
		if(perConfig[i]->hspi == hspi)
		{
			config = perConfig[i];
			break;
		}
	}

	return config;
}

void HAL_SPI_TxCpltCallback(hal_spi_handle_t *hspi)
{
	spi_config_t *config = GetConfig(hspi);
	BaseType_t taskWoken = pdFALSE;

	if(config != NULL)
	{
		xSemaphoreGiveFromISR(config->waitSem, &taskWoken);
		portYIELD_FROM_ISR(taskWoken);
	}
}

void HAL_SPI_RxCpltCallback(hal_spi_handle_t *hspi)
{
	spi_config_t *config = GetConfig(hspi);
	BaseType_t taskWoken = pdFALSE;

	if(config != NULL)
	{
		xSemaphoreGiveFromISR(config->waitSem, &taskWoken);
		portYIELD_FROM_ISR(taskWoken);
	}
}

void HAL_SPI_TxRxCpltCallback(hal_spi_handle_t *hspi)
{
	spi_config_t *config = GetConfig(hspi);
	BaseType_t taskWoken = pdFALSE;

	if(config != NULL)
	{
		xSemaphoreGiveFromISR(config->waitSem, &taskWoken);
		portYIELD_FROM_ISR(taskWoken);
	}
}

static void ClearSem(const spi_config_t* const config)
{
	/* Clear the semaphore state to 0 so that the next
	 * call t0 xSemaphoreTake() blocks. */
	xSemaphoreTake(config->waitSem, 0);
}

static void WaitTxnComp(const spi_config_t* const config, uint32_t timeoutMs)
{
	BaseType_t status;

	/* Wait for the transaction to complete. */
	status = xSemaphoreTake(config->waitSem, pdMS_TO_TICKS(timeoutMs));
	configASSERT(status == pdTRUE);
}


int8_t APPHAL_SPI_Init(const uint8_t pid, spi_config_t* const config)
{
	int8_t retVal = APP_ERR_INV_ARG;

		/* Validate params. */
		if((pid > MAX_SPIS) || (config == NULL) ||
		   (config->hspi == NULL))
		{
			return retVal;
		}

		if((config->rxDmaHandle == NULL) || (config->txDmaHandle == NULL))
		{
			return retVal;
		}

		retVal = APP_ERR_NO_MEM;
		config->waitSem = xSemaphoreCreateBinary();
		if(config->waitSem == NULL)
		{
			return retVal;
		}

		/* Save config variable for future reference. */
		perConfig[pid] = config;
		retVal = APP_ERR_NONE;

		return retVal;
}

int8_t APPHAL_SPI_Write(const uint8_t pid, uint8_t *write_buf,
						uint16_t transfer_len, uint32_t timeoutMs)
{
	int8_t retVal = APP_ERR_INV_STATE;

	if((pid > MAX_SPIS) || (perConfig[pid] == NULL))
	{
		return retVal;
	}

	retVal = APP_ERR_INV_ARG;

	if((write_buf == NULL) || (transfer_len == 0))
	{
		return retVal;
	}

	ClearSem(perConfig[pid]);
	retVal = APP_ERR_NONE;
	if(HAL_SPI_Transmit_DMA(perConfig[pid]->hspi, write_buf,
						transfer_len) != HAL_OK)
	{
		retVal = APP_ERR_STHAL;
	}

	WaitTxnComp(perConfig[pid], timeoutMs);

	return retVal;
}

int8_t APPHAL_SPI_Read(const uint8_t pid, uint8_t *read_buf,
						uint16_t transfer_len, uint32_t timeoutMs)
{
	int8_t retVal = APP_ERR_INV_STATE;

	if((pid > MAX_SPIS) || (perConfig[pid] == NULL))
	{
		return retVal;
	}

	retVal = APP_ERR_INV_ARG;

	if((read_buf == NULL) || (transfer_len == 0))
	{
		return retVal;
	}

	ClearSem(perConfig[pid]);

	retVal = APP_ERR_NONE;
	if(HAL_SPI_Receive_DMA(perConfig[pid]->hspi, read_buf,
					transfer_len) != HAL_OK)
	{
		retVal = APP_ERR_STHAL;
	}

	WaitTxnComp(perConfig[pid], timeoutMs);

	return retVal;
}

int8_t APPHAL_SPI_Write_Read(const uint8_t pid, uint8_t *write_buf,
						uint8_t *read_buf, uint16_t transfer_len,
						uint32_t timeoutMs)
{
	int8_t retVal = APP_ERR_INV_STATE;

	if((pid > MAX_SPIS) || (perConfig[pid] == NULL))
	{
		return retVal;
	}

	retVal = APP_ERR_INV_ARG;

	if((write_buf == NULL) ||(read_buf == NULL) || (transfer_len == 0))
	{
		return retVal;
	}

	ClearSem(perConfig[pid]);

	retVal = APP_ERR_NONE;
	if(HAL_SPI_TransmitReceive_DMA(perConfig[pid]->hspi, write_buf,
							  read_buf, transfer_len) != HAL_OK)
	{
		retVal = APP_ERR_STHAL;
	}

	WaitTxnComp(perConfig[pid], timeoutMs);

	return retVal;
}
