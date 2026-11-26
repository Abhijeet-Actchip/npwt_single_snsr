/*
 * thpy_mgr.h
 *
 *  Created on: Feb 17, 2025
 *      Author: Mahesh Murty
 */

#ifndef __THPY_MGR_H__
#define __THPY_MGR_H__				1

#include <stdint.h>

/** TODO: Refactor ThpyMgr */

int8_t ThpyMgrInit(void);
//int8_t ThpyMgrRunTherapy(COMM_TherapyReqMsg *req);
int8_t ThpyMgrStopTherapy(void);
int8_t ThpyMgrPauseTherapy(void);
int8_t ThpyMgrResumeTherapy(void);
//int8_t ThpyMgrGetTherapyStatus(COMM_TherapyStatMsg *msg);
int8_t ThpyMgrStopIrrigation(void);
int8_t ThpyMgrPauseIrrigation(void);
int8_t ThpyMgrResumeIrrigation(void);
void ThpyMgrPrintDiags(void);

#endif /* __THPY_MGR_H__ */
