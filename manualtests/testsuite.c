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

static inline void StartTestTask(TaskFunction_t taskFn, const char *taskName)
{
	int8_t retVal;
	BaseType_t status;

	/* Required to push logs on UART using trice. */
	retVal = DiagSvcInit();
	APP_ASSERT(retVal == APP_ERR_NONE);

	/* Create test thread task. */
	status = xTaskCreate(taskFn, taskName,
			DIAG_SVC_STACK_SIZE, NULL,
			DIAG_SVC_PRIORITY, NULL);
	APP_ASSERT(status == pdPASS);

	/* Start scheduler */
	vTaskStartScheduler();
	APP_ASSERT(0);
}

#if(RUN_VACUUM_MOTOR_TEST == 1)

	static void TestThread(void *args)
	{
		uint8_t duty = 0;

		Board_EnableBatVDiv(1);
		trice0(iD(4744), "dbg: Battery Divider Enabled\n");

		Board_EnableBoostConv(1);
		trice0(iD(4424), "dbg: Boost Converter Enabled\n");

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

			APPHAL_Delay(5000);
		}
	}

	static void Test_VacuumMotor(void)
	{
		StartTestTask(TestThread, "TEST-VACUMOTOR");
	}

#endif

#if(RUN_SENSOR_TEST == 1)

	static void TestThread(void *args)
	{
		float batV = 0.0f;
		float vacuumPres = 0.0f;

		APPHAL_Delay(1000);

		Board_EnableBatVDiv(1);
		trice0(iD(2058), "dbg: Battery Divider Enabled\n");

		Board_EnableBoostConv(1);
		trice0(iD(3426), "dbg: Boost Converter Enabled\n");

		while(1)
		{
			if(Board_GetBatV(&batV) == APP_ERR_NONE)
			{
				trice32(iD(5104), "dbg: Battery Voltage = %.3f V\n", aFloat(batV));
			}
			else
			{
				trice0(iD(5105), "err: Failed to read Battery Voltage\n");
			}

			if(Board_GetVacuumPres(&vacuumPres) == APP_ERR_NONE)
			{
				trice32(iD(5017), "dbg: Vacuum Pressure = %.2f mmHg\n", aFloat(vacuumPres));
			}
			else
			{
				trice0(iD(5018), "err: Failed to read Vacuum Pressure\n");
			}

			APPHAL_Delay(2000);
		}
	}

	static void Test_Sensor(void)
	{
		StartTestTask(TestThread, "TEST-SENSOR");
	}

#endif

#if(RUN_SWITCHES_TEST == 1)

	static void TestThread(void *args)
	{
		bool di1, di2, di3, di4, di5;

		while(1)
		{
			di1 = Board_GetDIN1();
			di2 = Board_GetDIN2();
			di3 = Board_GetDIN3();
			di4 = Board_GetDIN4();
			di5 = Board_GetDIN5();

			trice8(iD(6309), "dbg: Switch DI1 State: %d\n", di1);
			trice8(iD(1672), "dbg: Switch DI2 State: %d\n", di2);
			trice8(iD(2983), "dbg: Switch DI3 State: %d\n", di3);
			trice8(iD(2984), "dbg: Switch DI4 State: %d\n", di4);
			trice8(iD(2985), "dbg: Switch DI5 State: %d\n", di5);

			APPHAL_Delay(500);
		}
	}

	static void Test_Switches(void)
	{
		StartTestTask(TestThread, "TEST-SWITCHES");
	}

#endif

#if(RUN_GPIO_OUTPUT_TEST == 1)

	static void SetEnablePins(bool en)
	{
		Board_EnableBatVDiv(en);
		Board_EnableVacuumMot(en);
		Board_EnableBoostConv(en);
		Board_EnableDisplayPwr(en);
		Board_CtrlDisplayBacklight(en);
		Board_CtrlValve(en);
		Board_LedPwrOnCtrl(en);
	}

	static void TestThread(void *args)
	{
		while(1)
		{
			SetEnablePins(true);
			trice0(iD(7870), "dbg: Enable pins ON (BatVDiv, VacMot, Boost, DispPwr, Backlight, Valve, LedPwrOn)\n");
			APPHAL_Delay(10000);

			SetEnablePins(false);
			trice0(iD(1950), "dbg: Enable pins OFF (BatVDiv, VacMot, Boost, DispPwr, Backlight, Valve, LedPwrOn)\n");
			APPHAL_Delay(10000);
		}
	}

	static void Test_GpioOutput(void)
	{
		StartTestTask(TestThread, "TEST-GPIO-OUTPUT");
	}

#endif

#if(RUN_GPIO_INPUT_TEST == 1)

	static void TestThread(void *args)
	{
		bool di1, di2, di3, di4, di5;
		bool chrgSts, doneSts;

		while(1)
		{
			di1 = Board_GetDIN1();
			di2 = Board_GetDIN2();
			di3 = Board_GetDIN3();
			di4 = Board_GetDIN4();
			di5 = Board_GetDIN5();
			chrgSts = Board_GetBatChrgSts();
			doneSts = Board_GetBatDoneSts();

			trice8(iD(6309), "dbg: Digital Input 1 State: %d\n", di1);
			trice8(iD(1672), "dbg: Digital Input 2 State: %d\n", di2);
			trice8(iD(2983), "dbg: Digital Input 3 State: %d\n", di3);
			trice8(iD(2984), "dbg: Digital Input 4 State: %d\n", di4);
			trice8(iD(2985), "dbg: Digital Input 5 State: %d\n", di5);
			trice8(iD(2986), "dbg: Battery Charge Status: %d\n", chrgSts);
			trice8(iD(2987), "dbg: Battery Done Status: %d\n", doneSts);

			APPHAL_Delay(2000);
		}
	}

	static void Test_GpioInput(void)
	{
		StartTestTask(TestThread, "TEST-GPIO-INPUT");
	}

#endif

#if(RUN_PRESS_REGULATOR_TEST == 1)

#include "app/press_regulator.h"
#include "app/snsr_mgr.h"

#define PRESS_REGULATOR_SETPOINT		-125.0f

	static plantData_t snsrData;

	static void TestThread(void *args)
	{
		float setpoint = PRESS_REGULATOR_SETPOINT;
		trice32(iD(3620), "dbg: Pressure SP = %.2f mmHg\n", aFloat(setpoint));

		Board_EnableBatVDiv(1);
		trice0(iD(6320), "dbg: Battery Divider Enabled...\n");

		Board_EnableBoostConv(1);
		trice0(iD(5846), "dbg: Boost Converter Enabled\n");

		Board_EnableVacuumMot(1);
		trice0(iD(1246), "dbg: Vacuum Motor Enabled\n");

		PressRegEnable(1);
		trice0(iD(1766), "dbg : Pressure Regulator Enabled...\n");

		while(1)
		{
			SnsrMgrReadPlantData(&snsrData);
			trice32(iD(3120), "dbg: Vac Pressure = %0.2f mmHg\n", aFloat(snsrData.vacuumPressure));
			trice32(iD(1106), "dbg: BatV = %0.2f V\n", aFloat(snsrData.batV));
			PressRegRunCtrlLoop(&snsrData, setpoint);
			APPHAL_Delay(100);
		}
	}

	static void Test_PressRegulator(void)
	{
		StartTestTask(TestThread, "TEST-PRESS-REGULATOR");
	}

#endif

#if(RUN_PRESSURE_PER_VOLTAGE_TEST == 1)

#define VACUUM_MOTOR_SPEED				100

	static void TestThread(void *args)
	{
		uint8_t isMotorOn = 1;
		uint8_t vacMotSpeed = VACUUM_MOTOR_SPEED;
		float vacuumPres = 0.0f;

		APPHAL_Delay(1000);

		Board_CtrlValve(0);
		Board_EnableVacuumMot(1);
		trice0(iD(5694), "dbg: Vacuum Motor Enabled \n");
		Board_CtrlVacuumMot(vacMotSpeed);
		trice8(iD(4099), "dbg: Vacuum Motor Duty: %d\n", vacMotSpeed);

		while(1)
		{
			Board_GetVacuumPres(&vacuumPres);
			trice32(iD(4066), "dbg: Vacuum Motor Pressure = %.2f mmHg\n", aFloat(vacuumPres));

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
		StartTestTask(TestThread, "TEST-PRESS-PER-VOLTAGE");
	}

#endif

#if (RUN_FLASH_WRITE_TEST == 1)

#include "utilities/eeprom_emulation/core/eeprom_emul_core.h"
#include "generated/utilities/mx_eeprom_emulation_app.h"

#define FLASH_VAR_ADDRESS			0x0001
#define FLASH_VAR_VALUE				1

	static uint16_t VarAddress = FLASH_VAR_ADDRESS;
	static uint32_t VarValue = FLASH_VAR_VALUE, readValue;
	static ee_object_t testEeObj;

	static void TestThread(void *args)
	{
		ee_status eeStatus = EE_OK;

		testEeObj.crc_object = EEPROM_EMULATION_CRC_HANDLE();
		testEeObj.f_object = EEPROM_EMULATION_FLASH_HANDLE();
		testEeObj.time_object = NULL;

		/* Unlock the Flash Program Erase controller */
		if(HAL_FLASH_ITF_Unlock(HAL_FLASH) != HAL_OK)
		{
			APP_ASSERT(0);
		}

		trice(iD(2036), "dbg: Flash Unlock\n");

		eeStatus = EE_Init(&testEeObj, EE_FORCED_ERASE);
		if(eeStatus != EE_OK)
		{
			(void)HAL_FLASH_ITF_Lock(HAL_FLASH);
			APP_ASSERT(0);
		}

		trice(iD(3089), "dbg: EE Init\n");

		eeStatus = EE_WriteVariable32bits(VarAddress, VarValue);
		if(eeStatus == EE_OK)
		{
			eeStatus = EE_ReadVariable32bits(VarAddress, &readValue);
		}

		/* Lock the Flash Program Erase controller */
		(void)HAL_FLASH_ITF_Lock(HAL_FLASH);

		if((eeStatus != EE_OK) || (VarValue != readValue))
		{
			APP_ASSERT(0);
		}

		while(1)
		{
			trice32(iD(7204), "dbg: Value Written to Flash : %X\n", VarValue);
			trice32(iD(3835), "dbg: Value Read from Flash : %X\n", readValue);
			APPHAL_Delay(2000);
		}
	}

	static void Test_FlashWrite(void)
	{
		StartTestTask(TestThread, "TEST-FLASH-WRITE");
	}

#endif

#if (RUN_FLASH_READ_TEST == 1)

#include "utilities/eeprom_emulation/core/eeprom_emul_core.h"
#include "generated/utilities/mx_eeprom_emulation_app.h"

#define FLASH_VAR_ADDRESS			0x0001

	static uint16_t VarAddress = FLASH_VAR_ADDRESS;
	static ee_object_t testEeObj;

	static void TestThread(void *args)
	{
		uint32_t readValue = 0;
		ee_status eeStatus = EE_OK;

		testEeObj.crc_object = EEPROM_EMULATION_CRC_HANDLE();
		testEeObj.f_object = EEPROM_EMULATION_FLASH_HANDLE();
		testEeObj.time_object = NULL;

		/* Unlock the Flash Program Erase controller */
		if(HAL_FLASH_ITF_Unlock(HAL_FLASH) != HAL_OK)
		{
			APP_ASSERT(0);
		}

		eeStatus = EE_Init(&testEeObj, EE_FORCED_ERASE);
		if(eeStatus != EE_OK)
		{
			(void)HAL_FLASH_ITF_Lock(HAL_FLASH);
			APP_ASSERT(0);
		}

		trice(iD(1487), "dbg: EE Initialized...\n");

		eeStatus = EE_ReadVariable32bits(VarAddress, &readValue);

		/* Lock the Flash Program Erase controller */
		(void)HAL_FLASH_ITF_Lock(HAL_FLASH);

		if(eeStatus != EE_OK)
		{
			APP_ASSERT(0);
		}

		while(1)
		{
			trice32(iD(4120), "dbg: Value Read from Flash : %X\n", readValue);
			APPHAL_Delay(2000);
		}
	}

	static void Test_FlashRead(void)
	{
		StartTestTask(TestThread, "TEST-FLASH-READ");
	}

#endif

#if(RUN_BATTERY_CHARGING_TEST == 1)

	static void TestThread(void *args)
	{
		float batV = 0.0f;
		bool chrgSts, doneSts;

		Board_EnableBatVDiv(1);
		trice(iD(7339), "dbg: Battery Divider Enable\n");

		while(1)
		{
			chrgSts = Board_GetBatChrgSts();
			doneSts = Board_GetBatDoneSts();
			Board_GetBatV(&batV);

			trice32(iD(3843), "Battery voltage : %.3f V\n", aFloat(batV));
			trice32(iD(1586), "Battery Charging status : %d\n", ((!chrgSts) & doneSts));

			APPHAL_Delay(3000);
		}
	}

	static void Test_BatteryCharging(void)
	{
		StartTestTask(TestThread, "TEST-BAT-CHARGING");
	}

#endif

#if (RUN_DISPLAY_TEXT_TEST == 1) || (RUN_DISPLAY_BUTTONS_TEST == 1)

#include "app_middlewares/lvgl/lvgl.h"
#include "app_drivers/lv_port_st7789.h"

#define DISP_TEST_LOOP_MS			20U

	static void InitDisplayForTest(void)
	{
		Board_EnableDisplayPwr(1);
		Board_CtrlDisplayBacklight(1);
		lv_init();
		APP_ASSERT(lv_disp_st7789_init() == APP_ERR_NONE);
	}

#endif

#if (RUN_DISPLAY_TEXT_TEST == 1)

	static void TestThread(void *args)
	{
		(void)args;
		InitDisplayForTest();

		lv_obj_t *label = lv_label_create(lv_screen_active());
		lv_label_set_text(label, "NPWT_TEST_DISPLAY");
		lv_obj_center(label);

		while(1)
		{
			lv_timer_handler();
			APPHAL_Delay(DISP_TEST_LOOP_MS);
			lv_tick_inc(DISP_TEST_LOOP_MS);
		}
	}

	static void Test_DisplayText(void)
	{
		StartTestTask(TestThread, "TEST-DISP-TEXT");
	}

#endif

#if (RUN_DISPLAY_BUTTONS_TEST == 1)

/* Adjust if switches are active-high on hardware */
#define DISP_BTN_ACTIVE_STATE		false

	typedef struct
	{
		const char *name;
		bool (*readFn)(void);
		lv_obj_t *label;
		bool prevPressed;
	} disp_btn_state_t;

	static void TestThread(void *args)
	{
		(void)args;
		InitDisplayForTest();

		disp_btn_state_t buttons[] = {
			{ "UP",    Board_ReadSwUP,    NULL, false },
			{ "DOWN",  Board_ReadSwDOWN,  NULL, false },
			{ "ENTER", Board_ReadSwENTER, NULL, false },
			{ "LEFT",  Board_ReadSwLEFT,  NULL, false },
			{ "RIGHT", Board_ReadSwRIGHT, NULL, false },
		};
		const size_t btnCount = sizeof(buttons) / sizeof(buttons[0]);

		lv_obj_t *cont = lv_obj_create(lv_screen_active());
		lv_obj_set_size(cont, LV_PCT(90), LV_PCT(90));
		lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_center(cont);

		for(size_t i = 0U; i < btnCount; i++)
		{
			buttons[i].label = lv_label_create(cont);
			buttons[i].prevPressed = (buttons[i].readFn() == DISP_BTN_ACTIVE_STATE);
			lv_label_set_text_fmt(buttons[i].label, "%s : %s",
			                      buttons[i].name,
			                      buttons[i].prevPressed ? "PRESSED" : "RELEASED");
		}

		while(1)
		{
			for(size_t i = 0U; i < btnCount; i++)
			{
				bool pressed = (buttons[i].readFn() == DISP_BTN_ACTIVE_STATE);
				if(pressed != buttons[i].prevPressed)
				{
					buttons[i].prevPressed = pressed;
					lv_label_set_text_fmt(buttons[i].label, "%s : %s",
					                      buttons[i].name,
					                      pressed ? "PRESSED" : "RELEASED");
				}
			}

			lv_timer_handler();
			APPHAL_Delay(DISP_TEST_LOOP_MS);
			lv_tick_inc(DISP_TEST_LOOP_MS);
		}
	}

	static void Test_DisplayButtons(void)
	{
		StartTestTask(TestThread, "TEST-DISP-BTNS");
	}

#endif

void ManualTestRunner(void)
{
	#if (RUN_VACUUM_MOTOR_TEST == 1)
		Test_VacuumMotor();
	#endif

	#if (RUN_SENSOR_TEST == 1)
		Test_Sensor();
	#endif

	#if (RUN_SWITCHES_TEST == 1)
		Test_Switches();
	#endif

	#if (RUN_GPIO_OUTPUT_TEST == 1)
		Test_GpioOutput();
	#endif

	#if (RUN_GPIO_INPUT_TEST == 1)
		Test_GpioInput();
	#endif

	#if (RUN_PRESS_REGULATOR_TEST == 1)
		Test_PressRegulator();
	#endif

	#if (RUN_PRESSURE_PER_VOLTAGE_TEST == 1)
		Test_PressPerVoltage();
	#endif

	#if (RUN_FLASH_WRITE_TEST == 1)
		Test_FlashWrite();
	#endif

	#if (RUN_FLASH_READ_TEST == 1)
		Test_FlashRead();
	#endif

	#if (RUN_BATTERY_CHARGING_TEST == 1)
		Test_BatteryCharging();
	#endif

	#if (RUN_DISPLAY_TEXT_TEST == 1)
		Test_DisplayText();
	#endif

	#if (RUN_DISPLAY_BUTTONS_TEST == 1)
		Test_DisplayButtons();
	#endif
}
