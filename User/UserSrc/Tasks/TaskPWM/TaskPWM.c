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

typedef struct {
	uint16_t prescaler;			// prescaler
	uint32_t counterPeriod;		// counter period
	uint32_t dutyCycle;			// percentage (will be re-calculated)
} PWMSettings_t;

/*
 * demo PWM settings to test varying over time
 */
static PWMSettings_t pwmSettings[] = {
	// 1) updating counter period, only
	{ 2687,	31249, 	50 },
	{ 2687,	31249, 	50 },
	{ 2687,	31249, 	50 },
	{ 2687,	31249, 	50 },
	{ 2687,	31249, 	50 },

	{ 2687,	3124, 	50 },
	{ 2687,	3124, 	50 },
	{ 2687,	3124, 	50 },
	{ 2687,	3124, 	50 },
	{ 2687,	3124, 	50 },

	{ 2687,	312, 	50 },
	{ 2687,	312, 	50 },
	{ 2687,	312, 	50 },
	{ 2687,	312, 	50 },
	{ 2687,	312, 	50 },

	{ 2687,	31, 	50 },
	{ 2687,	31, 	50 },
	{ 2687,	31, 	50 },
	{ 2687,	31, 	50 },
	{ 2687,	31, 	50 },

	// 2) updating prescaler, only

	{ 2000,	31, 	50 },
	{ 2000,	31, 	50 },
	{ 2000,	31, 	50 },
	{ 2000,	31, 	50 },
	{ 2000,	31, 	50 },

	{ 100,	31, 	50 },
	{ 100,	31, 	50 },
	{ 100,	31, 	50 },
	{ 100,	31, 	50 },
	{ 100,	31, 	50 },

	{ 20,	31, 	50 },
	{ 20,	31, 	50 },
	{ 20,	31, 	50 },
	{ 20,	31, 	50 },
	{ 20,	31, 	50 },

	// 1) updating counter period, again
	{ 20,	17, 	50 },
	{ 20,	17, 	50 },
	{ 20,	17, 	50 },
	{ 20,	17, 	50 },
	{ 20,	17, 	50 },

#if 0 // 1,272,723 Hz
	// and get higher
	{ 5,	10, 	50 },
	{ 5,	10, 	50 },
	{ 5,	10, 	50 },
	{ 5,	10, 	50 },
	{ 5,	10, 	50 },
#endif
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

#ifdef PWM_LOG_TIM2
/*
 * pulse counter updated in IRQ (wrap-around)
 */
static uint32_t sCounter = 0;

/*
 * log message for the main routine
 */
static char sBuff[] = "TIM2 pulses/sec = #.........., TIM5 ctr = #.........., diff = #..........\r\n";
#else
static char sBuff[] = "TIM5 ctr = #.........., diff = #..........\r\n";
#endif

#ifdef PWM_LOG_TIM2
void TaskPWM_Interrupt(TIM_HandleTypeDef *htim2)
{
#if 0
//	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	if (htim2 == NULL) {
		Log(LC_PWM_c, "tick...\r\n");
	}
	else if (htim2->Channel == HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
		Log(LC_PWM_c, "tick - channel cleared\r\n");
	}
	else {
		if (htim2->Channel & HAL_TIM_ACTIVE_CHANNEL_1) {
			Log(LC_PWM_c, "tick - channel #1\r\n");
		}
		if (htim2->Channel & HAL_TIM_ACTIVE_CHANNEL_2) {
			Log(LC_PWM_c, "tick - channel #2\r\n");
		}
	}
#endif
	sCounter += 1;
}
#endif
/*
 * calculate absolute values for dutyCycle from percentage
 */
static void UpdateSettings()
{
	for (int i = 0; i < PWMSettingsNo; ++i) {
		pwmSettings[i].dutyCycle = pwmSettings[i].dutyCycle * pwmSettings[i].counterPeriod / 100;
	}
}

void TaskPWM_Run(void * argument)
{
#ifdef PWM_LOG_TIM2
	uint32_t lastCounterTIM2 = 0;
	uint32_t copyCounterTIM2;
	uint32_t counterDiffTIM2;
#endif
	uint32_t lastCounterTIM5 = 0;
	uint32_t copyCounterTIM5;
	uint32_t counterDiffTIM5;

	int oldPWMSettingsIndex;

	LogWait4Ready();

	Log(LC_PWM_c, "start TaskPWM...\r\n");

	UpdateSettings();

	while (1) {
		xLastWakeTime = xTaskGetTickCount();				// get timer tick timestamp

		/*
		 * get TIM5 (slave) counter value and diff to previous value
		 */
		copyCounterTIM5 = TIM5->CNT;

		if (lastCounterTIM5 <= copyCounterTIM5) {
			counterDiffTIM5 = copyCounterTIM5 - lastCounterTIM5;
		}
		else {
			counterDiffTIM5 = UINT32_MAX - lastCounterTIM5 + copyCounterTIM5 + 1;
		}

		lastCounterTIM5 = copyCounterTIM5;

#ifdef PWM_LOG_TIM2
		/*
		 * calculate IRQ counter difference for interval
		 */
		copyCounterTIM2 = sCounter;

		if (lastCounterTIM2 <= copyCounterTIM2) {
			counterDiffTIM2 = copyCounterTIM2 - lastCounterTIM2;
		}
		else {
			counterDiffTIM2 = UINT32_MAX - lastCounterTIM2 + copyCounterTIM2 + 1;
		}

		lastCounterTIM2 = copyCounterTIM2;
#endif

		/*
		 * update PWM settings
		 */

		// old settings
		oldPWMSettingsIndex = PWMSettingsIndex;

		// index for new settings (wrap around)
		PWMSettingsIndex += 1;
		PWMSettingsIndex %= PWMSettingsNo;

		// updates:
		if (pwmSettings[oldPWMSettingsIndex].prescaler != pwmSettings[PWMSettingsIndex].prescaler) {
			TIM2->PSC = pwmSettings[PWMSettingsIndex].prescaler;
		}

		if (pwmSettings[oldPWMSettingsIndex].counterPeriod != pwmSettings[PWMSettingsIndex].counterPeriod) {
			TIM2->ARR = pwmSettings[PWMSettingsIndex].counterPeriod;
		}

		if (pwmSettings[oldPWMSettingsIndex].dutyCycle != pwmSettings[PWMSettingsIndex].dutyCycle) {
			TIM2->CCR1 = pwmSettings[PWMSettingsIndex].dutyCycle;
		}

		/*
		 * log info message
		 */
#ifdef PWM_LOG_TIM2
		LogUInt32ToStr(sBuff + 19, counterDiffTIM2, 10);
		LogUInt32ToStr(sBuff + 43, copyCounterTIM5, 10);
		LogUInt32ToStr(sBuff + 63, counterDiffTIM5, 10);
		Log(LC_PWM_c, sBuff);
#else
		LogUInt32ToStr(sBuff + 12, copyCounterTIM5, 10);
		LogUInt32ToStr(sBuff + 32, counterDiffTIM5, 10);
		Log(LC_PWM_c, sBuff);
#endif

		vTaskDelayUntil( &xLastWakeTime, sInterval);		// wait for remaining #sInterval ticks for next cycle
	};
}

#endif // ENABLE_PWM
