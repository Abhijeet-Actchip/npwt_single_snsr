/*
 * trice_deferred_sink.h
 *
 * Deferred Trice output: RAM queues only. CommSvc drains the UART queue via
 * APPHAL_UART_Write (DMA). Every byte is also copied into the log file
 * spool (log_spool.h), which app/log_file.c appends to /log/trice.bin.
 */

#ifndef __TRICE_DEFERRED_SINK_H__
#define __TRICE_DEFERRED_SINK_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Register the Trice AUXILIARY8 callback. Call once before TriceTransfer runs. */
void TriceDeferredSinkInit(void);

/** UART log stream enable (PCCOMM). Boot default is disabled. */
void TriceDeferredSinkSetUartLogsEnabled(bool enabled);
bool TriceDeferredSinkUartLogsEnabled(void);

/** Any LPUART1 RX byte restarts the 5 s UART-log pause. */
void TriceDeferredSinkNoteRxActivity(void);

/**
 * True when the enable flag is set and the RX pause has elapsed.
 * Enqueue and drain both consult this.
 */
bool TriceDeferredSinkUartMayStream(void);

/**
 * If streaming is allowed and the UART is idle, copy one chunk from the
 * Trice UART queue and start DMA (waits for completion). Normal TX path for
 * Trice on LPUART1 — call only from CommSvc.
 */
void TriceDeferredSinkDrainUart(uint8_t uartPid);

/**
 * Assert/panic path: force-enqueue pending Trice into the UART queue, abort
 * any in-flight DMA TX, then drain with blocking polling UART. Does not use
 * FreeRTOS tasks or CommSvc. Safe after vTaskSuspendAll() with IRQs enabled.
 */
void TriceDeferredSinkPanicFlush(uint8_t uartPid);

#ifdef __cplusplus
}
#endif

#endif /* __TRICE_DEFERRED_SINK_H__ */
