/*
 * thpy_common.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Mahesh Murty
 */

#ifndef __THPY_COMMON_H__
#define __THPY_COMMON_H__				1

#include "msgs/npwt_msgs.pb.h"
#include "snsr_mgr.h"
#include "FreeRTOS.h"
#include "semphr.h"

typedef COMM_THERAPY_TYPE therapy_t;
typedef COMM_THERAPY_CMD thpy_cmd_t;
typedef COMM_THERAPY_FAULT thpy_fault_t;

typedef struct thpy_class_t
{
	therapy_t type;
	int32_t thpyDurationS;

	int8_t (*RunCtrlLoop)(struct thpy_class_t *this, plantData_t *snsrData);
	int8_t (*EnableThpy)(struct thpy_class_t *this);
	int8_t (*DisableThpy)(struct thpy_class_t *this);
	int8_t (*PauseThpy)(struct thpy_class_t *this);
	int8_t (*ResumeThpy)(struct thpy_class_t *this);
	int8_t (*SetPressureSp)(struct thpy_class_t *this, float sp);
	thpy_fault_t (*GetFaults)(struct thpy_class_t *this);
	bool (*IsThpyRunning)(struct thpy_class_t *this);
	int32_t (*GetRemTime)(struct thpy_class_t *this);
	int32_t (*GetDutyTime)(struct thpy_class_t *this);
	float (*GetPressureSp)(struct thpy_class_t *this);
	thpy_cmd_t (*GetThpyCmd)(struct thpy_class_t *this);
	float (*GetLeakPercent)(struct thpy_class_t *this);
	int8_t (*DeInit)(struct thpy_class_t *this);

	/* Private variables. */
	bool isInit;
	void *thpyData;
	SemaphoreHandle_t lock;
}thpy_class_t;


typedef struct irig_class_t
{
	int32_t instillmL; // Volume (mL) of liquid to be irrigated.

	int8_t (*RunCtrlLoop)(struct irig_class_t *this);
	int8_t (*EnableIrig)(struct irig_class_t *this);
	int8_t (*DisableIrig)(struct irig_class_t *this);
	int8_t (*PauseIrig)(struct irig_class_t *this);
	int8_t (*ResumeIrig)(struct irig_class_t *this);
	thpy_cmd_t (*GetIrigCmd)(struct irig_class_t *this);
	int32_t (*GetDispensedmL)(struct irig_class_t *this);
	thpy_fault_t (*GetFaults)(struct irig_class_t *this);
	int8_t (*DeInit)(struct irig_class_t *this);
	bool (*IsIrigRunning)(struct irig_class_t *this);

	/* Private variables. */
	bool isInit;
	void *irigData;
	SemaphoreHandle_t lock;
}irig_class_t;

#endif /* __THPY_COMMON_H__ */
