/*
 * app_hal_spi.h
 *
 *  Created on: Sep 16, 2024
 *      Author: Mahesh Murty
 */

#ifndef __APP_HAL_SPI_H__
#define __APP_HAL_SPI_H__				1

#include "bsp/board.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct _spi_config_t
{
	hal_spi_handle_t *hspi;
	hal_dma_handle_t *rxDmaHandle;
	hal_dma_handle_t *txDmaHandle;
	/* Private variables, not be be accessed by app code. */
	SemaphoreHandle_t waitSem;
}spi_config_t;

int8_t APPHAL_SPI_Init(const uint8_t pid, spi_config_t* const config);

int8_t APPHAL_SPI_Write(const uint8_t pid, uint8_t *write_buf,
						uint16_t transfer_len, uint32_t timeoutMs);

int8_t APPHAL_SPI_Read(const uint8_t pid, uint8_t *read_buf,
						uint16_t transfer_len, uint32_t timeoutMs);

int8_t APPHAL_SPI_Write_Read(const uint8_t pid, uint8_t *write_buf,
						uint8_t *read_buf, uint16_t transfer_len,
						uint32_t timeoutMs);

#endif /* __APP_HAL_SPI_H__ */
