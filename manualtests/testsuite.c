/*
 * testsuite.c
 *
 *  Created on: 05-Oct-2023
 *      Author: Mahesh Murty
 */

#include "testsuite.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_hal/app_hal_delay.h"
#include "app/diag_svc.h"
#include "../app_utils/assert/app_assert.h"
#include "../app_utils/trice/trice.h"


#if(RUN_POWER_SWITCH_SENSE_TEST == 1)

#include "bsp/board.h"
	static void TestThread(void *args)
	{
		uint16_t swCtr = 0;
		Board_Enable5VBuck(1);

		Board_LedPwrOnCtrl(1);
		while(1)
		{
			if(Board_PowerSwSensed())
			{
				swCtr++;
				trice32(iD(4759), "dbg: SwCounter = %d", aFloat(swCtr));
				if(swCtr > 5)
				{
					trice(iD(3816), "Switch Press sensed\n");
					Board_LedPwrOnCtrl(0);
					Board_Enable5VBuck(0);
				}
			}
			else
			{
				swCtr = 0;
			}
			APPHAL_Delay(500);
		}
	}

	static void Test_PowerSwitch(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-PWR-SWITCH",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}

#endif


#if(RUN_VACUUM_MOTOR_TEST == 1)

#include "bsp/board.h"
	static void TestThread(void *args)
	{
		uint8_t duty = 0;
		float current = 0;

		if(!Board_PowerJackDet())
		{
			Board_EnableBatVDiv(1);
			trice0(iD(4744), "dbg: Battery Divider Enabled\n");

			Board_EnableBoostConv(1);
			trice0(iD(4424), "dbg: Boost Converter Enabled\n");
		}

		Board_EnableVacuumMot(1);
		trice0(iD(1887), "dbg: Vacuum Motor Enabled\n");

		Board_CtrlVacuumMot(duty);
		trice8(iD(3327), "dbg: Vacuum Motor Duty Cycle: %d\n", duty);

		while(1)
		{
			duty += 10;
			if(duty > 100)
			{
				duty = 0;
			}

			Board_CtrlVacuumMot(duty);
			trice8(iD(6338), "dbg: Vacuum Motor Duty Cycle: %d\n", duty);

//			Board_GetVacuumMotCurr(&current);
//			trice32(iD(1154), "dbg: Vacuum Motor Current: %.2f A \n", aFloat(current));
			APPHAL_Delay(5000);
		}
	}

	static void Test_VacuumMotor(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-VACUMOTOR",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}

#endif

#if(RUN_IRRIGATION_MOTOR_TEST == 1)

#include "bsp/board.h"
	static void TestThread(void *args)
	{
		uint8_t duty = 0;
		float current = 0;

		if(!Board_PowerJackDet())
		{
			Board_EnableBatVDiv(1);
			trice0(iD(5537), "dbg: Battery Divider Enabled\n");

			Board_EnableBoostConv(1);
			trice0(iD(5390), "dbg: Boost Converter Enabled\n");
		}

		while(1)
		{
			duty += 20;
			if(duty > 100)
			{
				duty = 0;
			}
			trice8(iD(1813), "dbg: Irrigation Motor Duty Cycle: %d\n", duty);
			Board_CtrlIrriMot(duty);

			Board_GetIrriMotCurr(&current);
			trice32(iD(3181), "dbg: Irrigation Motor Current: %.2f A\n", aFloat(current));

			APPHAL_Delay(5000);
		}
	}

	static void Test_IrrigationMotor(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-IRIGMOTOR",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}

#endif

#if(RUN_ADC_TEST == 1)

#include "bsp/board.h"

	static void TestThread(void *args)
	{
		uint8_t vacMotSpeed = 0, irigMotSpeed = 0;
		float vacuumMotCurr = 0, irigMotCurr = 0;//, valveCurr = 0;
		float vacuumPres = 0, refPres = 0;
		float batV = 0;

		APPHAL_Delay(1000);

		Board_EnableBatVDiv(1);
		trice0(iD(2058), "dbg: Battery Divider Enabled\n");

		if(!Board_PowerJackDet())
		{
			Board_EnableBoostConv(1);
			trice0(iD(3426), "dbg: Boost Converter Enabled\n");
		}

		Board_EnableVacuumMot(1);
		trice0(iD(7147), "dbg: Vacuum Motor Enabled \n");

		while(1)
		{
			vacMotSpeed += 20;
			irigMotSpeed += 20;

			if(vacMotSpeed > 100)
			{
				irigMotSpeed = 0;
			}

			if(irigMotSpeed > 100)
			{
				irigMotSpeed = 0;
			}

			Board_CtrlVacuumMot(vacMotSpeed);
			trice8(iD(6017), "dbg: Vacuum Motor Duty: %d\n", vacMotSpeed);

			Board_CtrlIrriMot(irigMotSpeed);
			trice8(iD(3974), "dbg: Irrigation Motor Duty: %d\n", irigMotSpeed);


			Board_GetIrriMotCurr(&irigMotCurr);
			trice32(iD(7492), "dbg: Irrigation Motor Current = %.3f A\n", aFloat(irigMotCurr));

			Board_GetVacuumMotCurr(&vacuumMotCurr);
			trice32(iD(5500), "dbg: Vacuum Motor Current = %.3f A\n", aFloat(vacuumMotCurr));

//			Board_GetValveCurr(&valveCurr);
//			trice32(iD(1825), "dbg: Valve Current = %.3f A\n", aFloat(valveCurr));

			Board_GetBatV(&batV);
			trice32(iD(5104), "dbg: Battery Voltage = %.3f V\n", aFloat(batV));

			Board_GetVacuumPres(&vacuumPres);
			trice32(iD(5017), "dbg: Vacuum Motor Pressure = %.2f mmHg\n", aFloat(vacuumPres));

			Board_GetRefPres(&refPres);
			trice32(iD(2900), "dbg: Reference Pressure = %.2f mmHg\n", aFloat(refPres));

			APPHAL_Delay(5000);
		}
	}

	static void Test_ADC(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-ADC",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}


#endif

#if(RUN_GPIO_OUTPUT_TEST == 1)

#include "bsp/board.h"

	static void TestThread(void *args)
	{
		Board_Enable5VBuck(1);
		trice0(iD(6913), "dbg: Enabled 5V Buck Converter\n");

		while(1)
		{
			Board_EnableBoostConv(1);
			trice0(iD(7870), "dbg: Enable Boost Converter\n");
			Board_EnableAuxPwr(1);
			trice0(iD(4719), "dbg: Enable Power to Display Board\n");
			Board_CtrlValve(1);
			trice0(iD(5817), "dbg: Valve Open\n");
			Board_LedPwrOffCtrl(1);
			trice0(iD(6817), "dbg: PowerOn led ON\n");
			Board_LedPwrOnCtrl(1);
			trice0(iD(2194), "dbg: PowerOff led ON\n");
			Board_CtrlIrriMot(100);
			trice0(iD(3463), "dbg: Irrigation motor ON\n");


			APPHAL_Delay(10000);

//			Board_Enable5VBuck(0);
//			trice0(iD(2441), "dbg: Disabled 5V Buck Converter\n");
			Board_EnableBoostConv(0);
			trice0(iD(1950), "dbg: Disable Boost Converter\n");
			Board_EnableAuxPwr(0);
			trice0(iD(5645), "dbg: Disabled Power to Display Board\n");
			Board_CtrlValve(0);
			trice0(iD(6988), "dbg: Valve Close\n");
			Board_LedPwrOffCtrl(0);
			trice0(iD(7007), "dbg: PowerOn led OFF\n");
			Board_LedPwrOnCtrl(0);
			trice0(iD(5956), "dbg: PowerOff led OFF\n");
			Board_CtrlIrriMot(0);
			trice0(iD(6600), "dbg: Irrigation motor OFF\n");

			APPHAL_Delay(10000);
		}
	}

	static void Test_GpioOutput(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-GPIO-OUTPUT",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if(RUN_GPIO_INPUT_TEST == 1)

#include "bsp/board.h"

	static void TestThread(void *args)
	{
		int8_t adapterDet, pwrSw, in1, in2, in3;
		Board_Enable5VBuck(1);

		while(1)
		{
			adapterDet = Board_PowerJackDet();
			trice8(iD(7419), "dbg: Adapter State: %d\n", adapterDet);

			pwrSw = Board_PowerSwSensed();
			trice8(iD(5603), "dbg: Power Switch State: %d\n", pwrSw);

			in1 = Board_GetIN1();
			trice8(iD(6309), "dbg: Digital Input 1 State: %d\n", in1);
			in2 = Board_GetIN2();
			trice8(iD(1672), "dbg: Digital Input 2 State: %d\n", in2);
			in3 = Board_GetIN3();
			trice8(iD(2983), "dbg: Digital Input 3 State: %d\n", in3);

			APPHAL_Delay(2000);
		}
	}

	static void Test_GpioInput(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-GPIO-INPUT",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if(RUN_PRESS_REGULATOR_TEST == 1)

#include "bsp/board.h"
#include "app/press_regulator.h"
#include "app/snsr_mgr.h"

#define PRESS_REGULATOR_SETPOINT		-125.0f

	static plantData_t snsrData;

	static void TestThread(void *args)
	{
		float setpoint = PRESS_REGULATOR_SETPOINT;
		trice32(iD(3620), "dbg: Pressure SP = %.2f mmHg\n", aFloat(setpoint));


		if(!Board_PowerJackDet())
		{
			Board_EnableBatVDiv(1);
			trice0(iD(6320), "dbg: Battery Divider Enabled...\n");

			Board_EnableBoostConv(1);
			trice0(iD(5846), "dbg: Boost Converter Enabled\n");
		}

		Board_EnableVacuumMot(1);
		trice0(iD(1246), "dbg: Vacuum Motor Enabled\n");

		PressRegEnable(1);
		trice0(iD(1766), "dbg : Pressure Regulator Enabled...\n");

		while(1)
		{
			SnsrMgrReadPlantData(&snsrData);
			trice32(iD(2763), "dbg: Ref Pressure = %0.2f mmHg\n", aFloat(snsrData.refPressure));
			trice32(iD(3120), "dbg: Vac Pressure = %0.2f mmHg\n", aFloat(snsrData.vacuumPressure));
			trice32(iD(1106), "dbg: BatV = %0.2f V\n", aFloat(snsrData.batV));
			PressRegRunCtrlLoop(&snsrData, setpoint);
			APPHAL_Delay(100);
		}
	}

	static void Test_PressRegulator(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-PRESS-REGULATOR",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if(RUN_PRESSURE_PER_VOLTAGE_TEST == 1)

#include "bsp/board.h"

#define VACUUM_MOTOR_SPEED				100

	static void TestThread(void *args)
	{
		uint8_t isMotorOn = 1;
		uint8_t vacMotSpeed = VACUUM_MOTOR_SPEED;
		float vacuumMotCurr = 0;
		float vacuumPres = 0, refPres = 0;

		APPHAL_Delay(1000);

		Board_CtrlValve(0);
		Board_EnableVacuumMot(1);
		trice0(iD(5694), "dbg: Vacuum Motor Enabled \n");
		Board_CtrlVacuumMot(vacMotSpeed);
		trice8(iD(4099), "dbg: Vacuum Motor Duty: %d\n", vacMotSpeed);

		while(1)
		{
			Board_GetVacuumMotCurr(&vacuumMotCurr);
			trice32(iD(1891), "dbg: Vacuum Motor Current = %.3f A\n", aFloat(vacuumMotCurr));

			Board_GetVacuumPres(&vacuumPres);
			trice32(iD(4066), "dbg: Vacuum Motor Pressure = %.2f mmHg\n", aFloat(vacuumPres));

			Board_GetRefPres(&refPres);
			trice32(iD(7523), "dbg: Reference Pressure = %.2f mmHg\n", aFloat(refPres));

			APPHAL_Delay(5000);
			if(isMotorOn)
			{
				Board_CtrlVacuumMot(0);
				trice0(iD(6568), "dbg: Vacuum Motor Stopped\n");

				Board_EnableVacuumMot(0);
				isMotorOn = 0;
			}
		}
	}

	static void Test_PressPerVoltage(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-PRESS-PER-VOLTAGE",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}

#endif

#if(RUN_THERAPY_FAULT_DET_TEST == 1)

#include "bsp/board.h"
#include "app/press_regulator.h"

#define PRESS_REGULATOR_SETPOINT			-200.0f

#define PRESSURE_BLOCK_DET_THRESH			30.0f
#define PRESSURE_BLOCK_DET_TIME_S			5

#define PRESSURE_LEAK_DET_THRESH			15.0f
#define PRESSURE_LEAK_DET_TIME_S			20

	float rfP, vacP, er, erP;
	float pSp = PRESS_REGULATOR_SETPOINT;

	static void TestThread(void *args)
	{
		static TickType_t startTicks = 0;
		static uint8_t blockCtr = 0, leakCtr = 0;
		float leakRate = 0;


		/* Close Valve */
		Board_CtrlValve(0);
		/* Enable Vacuum Motor */
		Board_EnableVacuumMot(1);
		/* Set Motor Speed to 100 */
		Board_CtrlVacuumMot(60);

		while(1)
		{
			Board_CtrlVacuumMot(60);

			/* Get Sensor data */
			Board_GetRefPres(&rfP);
			Board_GetVacuumPres(&vacP);
			er = pSp - rfP;
			erP = (er/pSp) * 100;
			/* Monitor Pressure sensor readings every 1 sec */
			TickType_t currTicks = xTaskGetTickCount();
			if((currTicks - startTicks) >= pdMS_TO_TICKS(1000))
			{
				startTicks = currTicks;
				trice32(iD(4142), "dbg: Reference Pressure: %.3f\n", aFloat(rfP));
				trice32(iD(6310), "dbg: Vacuum Pressure: %.3f\n", aFloat(vacP));
				/* Leak Detection */
				if((rfP - vacP) > PRESSURE_BLOCK_DET_THRESH)
				{
					leakCtr = 0;
					if(blockCtr < PRESSURE_BLOCK_DET_TIME_S)
					{
						blockCtr++;
					}
					else
					{
						trice(iD(3670), "dbg: Block detected\n");
					}
				}
				else if((erP) > PRESSURE_LEAK_DET_THRESH)
				{
					blockCtr = 0;
					if(leakCtr < PRESSURE_LEAK_DET_TIME_S)
					{
						leakCtr++;
					}
					else
					{
						/* Calculate Leakage Rate */
						trice(iD(3619), "dbg: Leak detected, Check for leaks\n");
						trice32(iD(3981), "dbg: Leakage Rate: %.2f\n", aFloat(erP));
					}
				}	/* Block detection */
				else	/* No fault */
				{
					blockCtr = 0;
					leakCtr = 0;
					trice(iD(7848), "dbg: No fault Detected\n");
				}
			}

			APPHAL_Delay(100);
		}
	}

	static void Test_TherapyBlockDet(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-BLOCK-DET",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if (RUN_FLASH_WRITE_TEST == 1)

#include "bsp/board.h"
#include "eeprom_emul.h"

#define FLASH_VAR_ADDRESS			0x0001
#define FLASH_VAR_VALUE				1

static uint16_t VarAddress = FLASH_VAR_ADDRESS;
static uint32_t VarValue = FLASH_VAR_VALUE, readValue;

	static void TestThread(void *args)
	{
		EE_Status ee_status = EE_OK;

		/* Unlock the Flash Program Erase controller */
		HAL_FLASH_Unlock();

		trice(iD(2036), "dbg: Flash Unlock\n");

	    ee_status = EE_Init(EE_FORCED_ERASE);
	    if(ee_status != EE_OK)
		{
			Error_Handler();
		}

	    trice(iD(3089), "dbg: EE Init\n");

	    ee_status = EE_WriteVariable32bits(VarAddress, VarValue);
	    ee_status = EE_ReadVariable32bits(VarAddress, &readValue);
	    if(ee_status != EE_OK)
	    {
	    	Error_Handler();
	    }

	    if(VarValue != readValue)
		{
			Error_Handler();
		}

	    /* Lock the Flash Program Erase controller */
	    HAL_FLASH_Lock();

		while(1)
		{
			trice32(iD(7204), "dbg: Value Written to Flash : %X\n", VarValue);
			trice32(iD(3835), "dbg: Value Read from Flash : %X\n", readValue);
			APPHAL_Delay(2000);
		}
	}

	static void Test_FlashWrite(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-FLASH-WRITE",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if (RUN_FLASH_READ_TEST == 1)

#include "bsp/board.h"
#include "eeprom_emul.h"

#define FLASH_VAR_ADDRESS			0x0001

static uint16_t VarAddress = FLASH_VAR_ADDRESS;

	static void TestThread(void *args)
	{
		uint32_t readValue;

		EE_Status ee_status = EE_OK;

		/* Unlock the Flash Program Erase controller */
		HAL_FLASH_Unlock();

	    ee_status = EE_Init(EE_FORCED_ERASE);
	    if(ee_status != EE_OK)
		{
			Error_Handler();
		}

	    trice(iD(1487), "dbg: EE Initialized...\n");

	    ee_status = EE_ReadVariable32bits(VarAddress, &readValue);
	    if(ee_status != EE_OK)
	    {
	    	Error_Handler();
	    }

	    /* Lock the Flash Program Erase controller */
	    HAL_FLASH_Lock();

		while(1)
		{
			trice32(iD(4120), "dbg: Value Read from Flash : %X\n", readValue);
			APPHAL_Delay(2000);
		}
	}


	static void Test_FlashRead(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-FLASH-READ",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if(RUN_BATTERY_CHARGING_TEST == 1)

#include "bsp/board.h"

	static void TestThread(void *args)
	{
		float batV;
		uint8_t jackDet, chrgSts, doneSts;

		Board_EnableBatVDiv(1);
		trice(iD(7339), "dbg: Battery Divider Enable\n");

		Board_DisableBatChrg(0);
		trice(iD(4382), "dbg: Battery Charging Enabled\n");

		while(1)
		{
			jackDet = Board_PowerJackDet();
			chrgSts = Board_GetBatChrgSts();
			doneSts = Board_GetBatDoneSts();
			Board_GetBatV(&batV);

			trice32(iD(1296), "dbg: Power Jack State : %d\n", jackDet);
			trice32(iD(3843), "Battery voltage : %.3f", aFloat(batV));
			trice32(iD(1586), "Battery Charging status : %d\n", ((!chrgSts) & doneSts));

			APPHAL_Delay(3000);
		}
	}

	static void Test_BatteryCharging(void)
	{
		int8_t retVal;
		BaseType_t status;

		/* Required to push logs on UART using trice. */
		retVal = DiagSvcInit();
		APP_ASSERT(retVal == APP_ERR_NONE);

		/* Create test thread task. */
		status = xTaskCreate(TestThread, "TEST-BAT-CHARGING",
				DIAG_SVC_STACK_SIZE, NULL,
				DIAG_SVC_PRIORITY, NULL);
		APP_ASSERT(status == pdPASS);

		/* Start scheduler */
		vTaskStartScheduler();
		APP_ASSERT(0);
	}
#endif

#if (RUN_IRRIGATION_THPY_TEST == 1)
#include "app/thpy_mgr.h"

/* Modify irrigation volume (mL) here */
#define INSTILL_ML				100

static void TestThread(void *args)
{
	int8_t retVal;

	// Turn off actuators
	Board_CtrlVacuumMot(0);
	Board_CtrlValve(0);
	Board_CtrlIrriMot(0);

	COMM_IrrigationReqMsg irigReq;
	COMM_IrrigationStatMsg irigStatus = COMM_IrrigationStatMsg_init_zero;

	irigReq.cmd = COMM_THERAPY_CMD_START;
	irigReq.instillmL = INSTILL_ML;

	retVal = ThpyMgrRunIrrigation(&irigReq);
	if(retVal != APP_ERR_NONE)
	{
		trice8(iD(5655), "dbg: Failed to Start Irrigation: %d\n", retVal);
		APP_ASSERT(0);
	}

	TickType_t startTicks = xTaskGetTickCount(), diffTicks = 0;

	while(1)
	{
		/* Read Irrigation Status Msg periodically */
		retVal = ThpyMgrGetIrrigationStatus(&irigStatus);
		if(retVal != APP_ERR_NONE)
		{
			trice8(iD(4441), "dbg: Failed to read Irrigation Status msg: %d\n", retVal);
		}
		else
		{
			if(irigStatus.dispensedmL == (INSTILL_ML / 2))
			{
				/* Pause */
				retVal = ThpyMgrPauseIrrigation();
				if(retVal != APP_ERR_NONE)
				{
					trice8(iD(6053), "dbg: Failed to pause irrigation: %d\n", retVal);
				}
				else
				{
					trice(iD(1646), "dbg: Successfully paused irrigation\n");

					APPHAL_Delay(5000);

					/* Resume */
					retVal = ThpyMgrResumeIrrigation();
					if(retVal != APP_ERR_NONE)
					{
						trice8(iD(4540), "dbg: Failed to resume irrigation: %d\n", retVal);
					}
					else
					{
						trice(iD(3575), "dbg: Successfully resumed irrigation\n");
					}
				}
			}
			else
			{
				diffTicks = startTicks - xTaskGetTickCount();
				if(diffTicks >= pdMS_TO_TICKS(1000))
				{
					startTicks = xTaskGetTickCount();
					trice32(iD(1461), "dbg: Dispensed mL :%d\n", irigStatus.dispensedmL);
				}
			}
		}

		APPHAL_Delay(200);
	}
}

static void Test_IrrigationThpy(void)
{
	int8_t retVal;
	BaseType_t status;

	/* Required to push logs on UART using trice. */
	retVal = DiagSvcInit();
	APP_ASSERT(retVal == APP_ERR_NONE);

	/* Required to test irrigation therapy */
	retVal = ThpyMgrInit();
	APP_ASSERT(retVal == APP_ERR_NONE);

	/* Create test thread task. */
	status = xTaskCreate(TestThread, "TEST-IRRIGATION",
			DIAG_SVC_STACK_SIZE, NULL,
			DIAG_SVC_PRIORITY, NULL);
	APP_ASSERT(status == pdPASS);

	/* Start scheduler */
	vTaskStartScheduler();
	APP_ASSERT(0);

}
#endif

void ManualTestRunner(void)
{
	#if (RUN_VACUUM_MOTOR_TEST == 1)
		Test_VacuumMotor();
	#endif

	#if (RUN_IRRIGATION_MOTOR_TEST == 1)
		Test_IrrigationMotor();
	#endif

	#if(RUN_ADC_TEST == 1)
		Test_ADC();
	#endif

	#if(RUN_GPIO_OUTPUT_TEST == 1)
		Test_GpioOutput();
	#endif

	#if(RUN_GPIO_INPUT_TEST == 1)
		Test_GpioInput();
	#endif

	#if(RUN_PRESS_REGULATOR_TEST == 1)
		Test_PressRegulator();
	#endif

		#if (RUN_PRESSURE_PER_VOLTAGE_TEST == 1)
			Test_PressPerVoltage();
		#endif

	#if(RUN_FLASH_WRITE_TEST == 1)
			Test_FlashWrite();
	#endif

	#if(RUN_FLASH_READ_TEST == 1)
			Test_FlashRead();
	#endif

	#if(RUN_BATTERY_CHARGING_TEST == 1)
			Test_BatteryCharging();
	#endif

	#if(RUN_THERAPY_FAULT_DET_TEST == 1)
		Test_TherapyBlockDet();
	#endif

	#if (RUN_IRRIGATION_THPY_TEST == 1)
		Test_IrrigationThpy();
	#endif

	#if(RUN_POWER_SWITCH_SENSE_TEST == 1)
		Test_PowerSwitch();
	#endif
}
