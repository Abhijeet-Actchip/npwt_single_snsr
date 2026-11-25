/*
 * board_v1_0.h
 *
 *  Created on: 24-Nov-2026
 *      Author: Abhijeet M
 */

#ifndef BOARD_V1_0_H_
#define BOARD_V1_0_H_


#include "main.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

#if (SELECT_BOARD == BOARD_ID_V1_0)

/* SPI port identifier. */
#define BOARD_SPI1_PID					0

#endif /* SELECT_BOARD */

#ifdef __cplusplus
}
#endif


#endif /* BOARD_V1_0_H_ */
