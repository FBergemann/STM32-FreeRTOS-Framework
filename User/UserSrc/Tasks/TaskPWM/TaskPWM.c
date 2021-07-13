/*
 * Task2.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include "UserInc/Features.h"

#if ( ENABLE_PWM == 1 )

#include <main.h>
#include <cmsis_os.h>

#include <stdint.h> // UINT32_MAX

#include "UserInc/Logging.h"
#include "UserInc/TimerTick.h"

extern TIM_HandleTypeDef htim5;

typedef struct {
uint16_t prescaler;
uint32_t counterPeriod;
uint32_t pulseLength;
} PWMSettings_t;

/*
 * demo PWM settings to test varying over time
 */
static PWMSettings_t pwmSettings[] = {
// 1) updating counter period, only
{ 2687,	31249, 	1000 },
{ 2687,	31249, 	1000 },
{ 2687,	31249, 	1000 },
{ 2687,	31249, 	1000 },
{ 2687,	31249, 	1000 },

{ 2687,	3124, 	1000 },
{ 2687,	3124, 	1000 },
{ 2687,	3124, 	1000 },
{ 2687,	3124, 	1000 },
{ 2687,	3124, 	1000 },

{ 2687,	312, 	1000 },
{ 2687,	312, 	1000 },
{ 2687,	312, 	1000 },
{ 2687,	312, 	1000 },
{ 2687,	312, 	1000 },

{ 2687,	31, 	1000 },
{ 2687,	31, 	1000 },
{ 2687,	31, 	1000 },
{ 2687,	31, 	1000 },
{ 2687,	31, 	1000 },

// 2) updating prescaler, only

{ 2000,	31, 	1000 },
{ 2000,	31, 	1000 },
{ 2000,	31, 	1000 },
{ 2000,	31, 	1000 },
{ 2000,	31, 	1000 },

{ 100,	31, 	1000 },
{ 100,	31, 	1000 },
{ 100,	31, 	1000 },
{ 100,	31, 	1000 },
{ 100,	31, 	1000 },

// 3) updating both: prescaler and counter period
};

/*
 * number of settings in pwmSettings
 */
static const int PWMSettingsNo = sizeof(pwmSettings) / sizeof(PWMSettings_t);

/*
 * indexing pwmSettings
 */
static int PWMSettingsIndex = 0;

/*
 * main routine interval
 */
static TickType_t sInterval = TASK_DELAY_S(1);

/*
 * main routine interval wake-up time
 */
static TickType_t xLastWakeTime = 0;

/*
 * pulse counter updated in IRQ (wrap-around)
 */
static uint32_t sCounter = 0;

/*
 * log message for the main routine
 */
static char sBuff[] = "pulses = #.....\r\n";

void TaskPWM_Interrupt(TIM_HandleTypeDef *htim5)
{
#if 0
//	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	if (htim5 == NULL) {
		Log(LC_PWM_c, "tick...\r\n");
	}
	else if (htim5->Channel == HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
		Log(LC_PWM_c, "tick - channel cleared\r\n");
	}
	else {
		if (htim5->Channel & HAL_TIM_ACTIVE_CHANNEL_1) {
			Log(LC_PWM_c, "tick - channel #1\r\n");
		}
		if (htim5->Channel & HAL_TIM_ACTIVE_CHANNEL_2) {
			Log(LC_PWM_c, "tick - channel #2\r\n");
		}
	}
#endif
	sCounter += 1;
}

void TaskPWM_Run(void * argument)
{
	uint32_t lastCounter = 0;
	uint32_t copyCounter;
	uint32_t counterDiff;
	int oldPWMSettingsIndex;

	LogWait4Ready();

	Log(LC_PWM_c, "start TaskPWM...\r\n");

	while (1) {
		xLastWakeTime = xTaskGetTickCount();				// get timer tick timestamp

		/*
		 * calculate IRQ counter difference for interval
		 */
		copyCounter = sCounter;
		if (lastCounter <= copyCounter) {
			counterDiff = copyCounter - lastCounter;
		}
		else {
			counterDiff = UINT32_MAX - lastCounter + copyCounter + 1;
		}

		lastCounter = copyCounter;

		/*
		 * update PWM settings
		 */

		// old settings
		oldPWMSettingsIndex = PWMSettingsIndex;
		// index for next settings (wrap around)
		PWMSettingsIndex += 1;
		PWMSettingsIndex %= PWMSettingsNo;

		if (pwmSettings[oldPWMSettingsIndex].prescaler != pwmSettings[PWMSettingsIndex].prescaler) {
			TIM5->PSC = pwmSettings[PWMSettingsIndex].prescaler;
		}

		if (pwmSettings[oldPWMSettingsIndex].counterPeriod != pwmSettings[PWMSettingsIndex].counterPeriod) {
			TIM5->ARR = pwmSettings[PWMSettingsIndex].counterPeriod;
		}

		/*
		 * log info message
		 */
		LogUInt32ToStr(sBuff + 10, counterDiff, 5);
		Log(LC_PWM_c, sBuff);

		vTaskDelayUntil( &xLastWakeTime, sInterval);		// wait for remaining #sInterval ticks for next cycle
	};
}

#endif
