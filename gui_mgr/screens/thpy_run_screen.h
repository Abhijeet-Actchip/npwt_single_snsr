/*
 * thpy_run_screen.h
 */

#ifndef __THPY_RUN_SCREEN_H__
#define __THPY_RUN_SCREEN_H__		1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void ShowThpyRunScreen(void);
void ClearThpyRunStateVar(void);
void ThpyRunSetAchievedPressure(int16_t pressureMmHg);

#ifdef __cplusplus
}
#endif

#endif /* __THPY_RUN_SCREEN_H__ */
