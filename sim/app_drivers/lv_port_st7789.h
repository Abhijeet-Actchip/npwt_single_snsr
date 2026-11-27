/*
 * lv_port_st7789.h  (simulator redirect)
 *
 * The simulator provides lv_disp_st7789_init() via sim/port/lv_port_sim.c.
 * This redirect header sits at the same path as the embedded driver header
 * so gui_mgr.c can include "app_drivers/lv_port_st7789.h" unchanged on
 * both targets.
 */

#ifndef LV_PORT_ST7789_H_
#define LV_PORT_ST7789_H_

#include "sim/port/lv_port_sim.h"

#endif /* LV_PORT_ST7789_H_ */
