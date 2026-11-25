/*
 * trice_deferred_sink.c
 *
 * Single TRICE_DEFERRED_AUXILIARY8 callback: gated memcpy into a UART byte
 * queue and a copy into the log file spool (log_spool.c). No peripheral
 * I/O here.
 */

#include "../app_middlewares/trice_deferred_sink.h"

#include "log_spool.h"
#include "app_hal/app_hal_uart.h"
#include "app_hal/app_hal_delay.h"
#include "config/appconfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "../app_utils/trice/trice.h"

#define TRICE_UART_Q_SIZE			1024u
#define TRICE_UART_TX_CHUNK			256u
#define TRICE_UART_RX_PAUSE_MS		5000u

static uint8_t s_uartQ[TRICE_UART_Q_SIZE];
static volatile size_t s_uartQHead;
static volatile size_t s_uartQTail;
static volatile size_t s_uartQCount;

static uint8_t s_txScratch[TRICE_UART_TX_CHUNK];

static volatile bool s_uartLogsEnabled;
static volatile bool s_rxPauseActive;
static volatile TickType_t s_lastRxTick;

static size_t UartQFree(void)
{
	return TRICE_UART_Q_SIZE - s_uartQCount;
}

static void UartQPush(const uint8_t *data, size_t len)
{
	size_t i;

	taskENTER_CRITICAL();
	if (len > UartQFree()) {
		/* Drop whole chunk rather than split a TCOBS frame mid-push. */
		taskEXIT_CRITICAL();
		return;
	}
	for (i = 0; i < len; i++) {
		s_uartQ[s_uartQHead] = data[i];
		s_uartQHead++;
		if (s_uartQHead >= TRICE_UART_Q_SIZE) {
			s_uartQHead = 0;
		}
	}
	s_uartQCount += len;
	taskEXIT_CRITICAL();
}

static size_t UartQPopContiguous(uint8_t *dst, size_t maxLen)
{
	size_t n = 0;

	taskENTER_CRITICAL();
	if (s_uartQCount == 0u) {
		taskEXIT_CRITICAL();
		return 0;
	}
	n = s_uartQCount;
	if (n > maxLen) {
		n = maxLen;
	}
	/* Limit to contiguous bytes before wrap. */
	{
		size_t toEnd = TRICE_UART_Q_SIZE - s_uartQTail;
		if (n > toEnd) {
			n = toEnd;
		}
	}
	memcpy(dst, &s_uartQ[s_uartQTail], n);
	s_uartQTail += n;
	if (s_uartQTail >= TRICE_UART_Q_SIZE) {
		s_uartQTail = 0;
	}
	s_uartQCount -= n;
	taskEXIT_CRITICAL();
	return n;
}

static void TriceDeferredSinkWrite(const uint8_t *enc, size_t encLen)
{
	if ((enc == NULL) || (encLen == 0u)) {
		return;
	}

	/* UART sink: enqueue only while streaming is allowed. */
	if (TriceDeferredSinkUartMayStream()) {
		UartQPush(enc, encLen);
	}

	/* File sink: always, from boot, independent of the UART gate. The log
	 * file task (app/log_file.c) appends the spool to /log/trice.bin. */
	LogSpoolWrite(enc, encLen);
}

void TriceDeferredSinkInit(void)
{
	s_uartQHead = 0;
	s_uartQTail = 0;
	s_uartQCount = 0;
	s_uartLogsEnabled = false;
	s_rxPauseActive = false;
	s_lastRxTick = 0;
	LogSpoolInit();
	UserNonBlockingDeferredWrite8AuxiliaryFn = TriceDeferredSinkWrite;
}

void TriceDeferredSinkSetUartLogsEnabled(bool enabled)
{
	s_uartLogsEnabled = enabled;
	if (enabled) {
		/* After enable response, stream without waiting out a prior RX pause. */
		s_rxPauseActive = false;
	} else {
		/* Discard queued UART bytes; the file spool keeps the history. */
		taskENTER_CRITICAL();
		s_uartQHead = 0;
		s_uartQTail = 0;
		s_uartQCount = 0;
		taskEXIT_CRITICAL();
	}
}

bool TriceDeferredSinkUartLogsEnabled(void)
{
	return s_uartLogsEnabled;
}

void TriceDeferredSinkNoteRxActivity(void)
{
	s_lastRxTick = xTaskGetTickCount();
	s_rxPauseActive = true;
}

bool TriceDeferredSinkUartMayStream(void)
{
	if (!s_uartLogsEnabled) {
		return false;
	}
	if (!s_rxPauseActive) {
		return true;
	}
	if ((xTaskGetTickCount() - s_lastRxTick) >= pdMS_TO_TICKS(TRICE_UART_RX_PAUSE_MS)) {
		s_rxPauseActive = false;
		return true;
	}
	return false;
}

void TriceDeferredSinkDrainUart(uint8_t uartPid)
{
	size_t n;
	int8_t txStatus;

	if (!TriceDeferredSinkUartMayStream()) {
		return;
	}
	if (APPHAL_UART_Get_WriteStatus(uartPid) != APP_ERR_NONE) {
		return;
	}

	n = UartQPopContiguous(s_txScratch, sizeof(s_txScratch));
	if (n == 0u) {
		return;
	}

	txStatus = APPHAL_UART_Write(uartPid, s_txScratch, (uint16_t)n);
	if (txStatus != APP_ERR_NONE) {
		return;
	}

	/* Hold CommSvc until DMA completes so MIN never races this TX. */
	do {
		APPHAL_Delay(1);
		txStatus = APPHAL_UART_Get_WriteStatus(uartPid);
	} while (txStatus != APP_ERR_NONE);
}

void TriceDeferredSinkPanicFlush(uint8_t uartPid)
{
	size_t n;
	unsigned i;

	/* Assert lines must reach the UART queue even if PCCOMM logs were off
	 * or paused for RX. Scheduler is typically suspended; only this path runs. */
	s_uartLogsEnabled = true;
	s_rxPauseActive = false;

	/* Encode deferred Trice (double buffer) into AUXILIARY8 → UART queue.
	 * Two passes cover a half that was mid-transfer if OutDepth ever blocks. */
	for (i = 0u; i < 2u; i++) {
		TriceTransfer();
	}

	(void)APPHAL_UART_AbortTx(uartPid);

	for (;;) {
		n = UartQPopContiguous(s_txScratch, sizeof(s_txScratch));
		if (n == 0u) {
			break;
		}
		(void)APPHAL_UART_WriteBlocking(uartPid, s_txScratch, (uint16_t)n);
	}
}
