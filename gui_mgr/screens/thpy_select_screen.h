/*
 * thpy_select_screen.h
 */

#ifndef __THPY_SELECT_SCREEN_H__
#define __THPY_SELECT_SCREEN_H__		1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
    THPY_SEL_NONE = 0,
    THPY_SEL_CONTINUOUS,
    THPY_SEL_CYCLIC,
    THPY_SEL_VARIABLE,
    THPY_SEL_MAX
} thpy_sel_mode_t;

void ShowThpySelectScreen(void);
void ClearThpySelectStateVar(void);
thpy_sel_mode_t ThpySelectGetSelectedMode(void);

#ifdef __cplusplus
}
#endif

#endif /* __THPY_SELECT_SCREEN_H__ */
