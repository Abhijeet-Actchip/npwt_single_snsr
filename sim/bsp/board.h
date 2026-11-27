/*
 * board.h  (simulator redirect)
 *
 * Shadows bsp/board.h for the simulator build so gui_mgr.c can
 * include "bsp/board.h" unchanged on both embedded and simulator targets.
 */

#ifndef BOARD_H_
#define BOARD_H_				1

#include "config/appconfig.h"
#include "sim/bsp/board_sim.h"

#endif /* BOARD_H_ */
