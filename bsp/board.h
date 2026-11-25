/*
 * board.h
 *
 *  Created on: 24-Nov-2026
 *      Author: Abhijeet M
 */

#ifndef BOARD_H_
#define BOARD_H_				1


#include "config/appconfig.h"
#include "main.h"
#include <stdbool.h>

#if (SELECT_BOARD == BOARD_ID_V1_0)
	#include "board_v1_0.h"
#endif

int8_t Board_InitPeripherals(void);
//bool Board_PowerJackDet(void);
int8_t Board_EnableBatVDiv(bool en);
int8_t Board_GetBatV(float *ptrBatV);
//int8_t  Board_Enable5VBuck(bool en);
int8_t Board_EnableBoostConv(bool en);
int8_t Board_CtrlVacuumMot(uint8_t speedPer);
//int8_t Board_CtrlIrriMot(uint8_t speedPer);
int8_t Board_CtrlValve(bool en);
//bool Board_PowerSwSensed(void);
//int8_t Board_GetVacuumMotCurr(float *ptrCurr);
//int8_t Board_GetIrriMotCurr(float *ptrCurr);
//int8_t Board_GetValveCurr(float *ptrCurr);
int8_t Board_GetVacuumPres(float *ptrPres);
//int8_t Board_GetRefPres(float *ptrPres);
//int8_t Board_LedPwrOnCtrl(bool en);
//int8_t Board_LedPwrOffCtrl(bool en);
//int8_t Board_EnableAuxPwr(bool en);
int8_t Board_EnableVacuumMot(bool en);
//int8_t Board_DisableBatChrg(bool en);
bool Board_GetBatDoneSts(void);
bool Board_GetBatChrgSts(void);
//void Board_SetTime(uint32_t epoch);
//uint32_t Board_GetTime(void);
//bool Board_GetCannisterDetStatus(void);
int8_t Board_EnableDisplayPwr(bool en);
int8_t Board_CtrlDisplayBacklight(bool en);
bool Board_GetDIN1(void);
bool Board_GetDIN2(void);
bool Board_GetDIN3(void);
bool Board_GetDIN4(void);
bool Board_GetDIN5(void);

//bool Board_GetIN3(void);
//bool Board_GetIN2(void);
//bool Board_GetIN1(void);



#endif /* BOARD_H_ */
