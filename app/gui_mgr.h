/*
 * gui_mgr.h
 *
 *  Created on: Mar 5, 2025
 *      Author: Mahesh Murty
 */

#ifndef __GUI_MGR_H__
#define __GUI_MGR_H__				1

#include <stdint.h>
//#include "app_types.h"

typedef enum _screen_id_t
{
	UI_NO_SCREEN = 0,
	UI_DO_SCREEN,
	UI_CO2_SCREEN,
	UI_TEMP_SCREEN,
	UI_MAX_SCREENS
}screen_id_t;

int8_t GuiMgrInit(void);

int8_t GuiMgrShowScr(screen_id_t id);
int8_t GuiMgrShowNextScr(void);
int8_t GuiMgrShowPrevScr(void);
//int8_t GuiMgrUpdateData(app_data_t *pData);

void GuiMgrPrintDiags(void);


#endif /* __GUI_MGR_H__ */
