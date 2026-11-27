/*
 * gui_mgr.h
 *
 *  Created on: Mar 5, 2025
 *      Author: Mahesh Murty
 */

#ifndef __GUI_MGR_H__
#define __GUI_MGR_H__				1

#include <stdint.h>
#include <stdbool.h>

int8_t GuiMgrInit(void);
bool GuiMgrIsReady(void);

void GuiAcquireLock(void);
void GuiReleaseLock(void);

void GuiMgrPrintDiags(void);

#endif /* __GUI_MGR_H__ */
