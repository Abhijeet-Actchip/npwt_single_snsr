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
int8_t Board_LedPwrOnCtrl(bool en);
//bool Board_PowerJackDet(void);
int8_t Board_EnableBatVDiv(bool en);
int8_t Board_EnableVacuumMot(bool en);
int8_t Board_EnableBoostConv(bool en);
int8_t Board_EnableDisplayPwr(bool en);
int8_t Board_EnableDisplayCS(bool en);
int8_t Board_CtrlDisplayReset(bool en);
int8_t Board_CtrlDisplayRS(bool en);

int8_t Board_CtrlDisplayBacklight(bool en);
int8_t Board_CtrlVacuumMot(uint8_t speedPer);
int8_t Board_CtrlValve(bool en);

int8_t Board_GetBatV(float *ptrBatV);
int8_t Board_GetVacuumPres(float *ptrPres);
bool Board_GetBatDoneSts(void);
bool Board_GetBatChrgSts(void);

bool Board_GetDIN1(void);
bool Board_GetDIN2(void);
bool Board_GetDIN3(void);
bool Board_GetDIN4(void);
bool Board_GetDIN5(void);

bool Board_ReadSwUP(void);
bool Board_ReadSwDOWN(void);
bool Board_ReadSwENTER(void);
bool Board_ReadSwRIGHT(void);
bool Board_ReadSwLEFT(void);


#endif /* BOARD_H_ */
