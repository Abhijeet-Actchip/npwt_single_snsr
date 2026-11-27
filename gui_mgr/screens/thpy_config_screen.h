/*
 * thpy_config_screen.h
 */

#ifndef __THPY_CONFIG_SCREEN_H__
#define __THPY_CONFIG_SCREEN_H__		1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "thpy_select_screen.h"

typedef struct
{
    thpy_sel_mode_t mode;
    int16_t pressureMmHg;       /* Continuous: -40 to -300 */
    int16_t maxPressureMmHg;    /* Cyclic / Variable: -40 to -300 */
    int16_t minPressureMmHg;    /* Cyclic: 0 (fixed), Variable: -40 to -300 */
    uint16_t onTimeSec;         /* Cyclic / Variable: 30 to 900 (step 30) */
    uint16_t offTimeSec;        /* Cyclic / Variable: 30 to 900 (step 30) */
} thpy_config_t;

void ShowThpyConfigScreen(void);
void ClearThpyConfigStateVar(void);
const thpy_config_t *ThpyConfigGet(void);

#ifdef __cplusplus
}
#endif

#endif /* __THPY_CONFIG_SCREEN_H__ */
