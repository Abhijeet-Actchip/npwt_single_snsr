/*
 * board_sim.h
 *
 * Simulator BSP declarations.
 */

#ifndef BOARD_SIM_H_
#define BOARD_SIM_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Keypad — implemented in board_sim.c via SDL keyboard state. */
bool Board_ReadSwUP(void);
bool Board_ReadSwDOWN(void);
bool Board_ReadSwENTER(void);
bool Board_ReadSwRIGHT(void);
bool Board_ReadSwLEFT(void);

/* No-op hardware control stubs. */
int8_t Board_InitPeripherals(void);
int8_t Board_EnableDisplayCS(bool en);
int8_t Board_CtrlDisplayReset(bool en);
int8_t Board_CtrlDisplayRS(bool en);
int8_t Board_CtrlDisplayBacklight(bool en);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_SIM_H_ */
