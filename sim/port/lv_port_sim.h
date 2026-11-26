/*
 * lv_port_sim.h
 *
 * Simulator display port — exposes the same API as lv_port_st7789.h so
 * gui_mgr.c can call lv_disp_st7789_init() unchanged on both targets.
 */

#ifndef LV_PORT_SIM_H_
#define LV_PORT_SIM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int8_t lv_disp_st7789_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_SIM_H_ */
