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

#include "UserInc/Tasks/TaskPWM.h"

typedef struct {
	uint16_t prescaler;			// prescaler
	uint32_t counterPeriod;		// counter period
	uint8_t  dutyCyclePercent;	// percentage (will be re-calculated)
	uint32_t dutyCycleAbolute;	// absolute value
} PWMSettings_t;

static PWMSettings_t sFixedPWMSettings = { 2687, 31249, 50, 0};
static UseFixedSettings_t sUseFixedSettings = UFS_Off_c;

/*
 * demo PWM settings to test varying over time
 */
static PWMSettings_t pwmSettings[] = {
	// 1) updating counter period, only
	{ 2687,	31249, 	50, 0 },
	{ 2687,	31249, 	50, 0 },
	{ 2687,	31249, 	50, 0 },
	{ 2687,	31249, 	50, 0 },
	{ 2687,	31249, 	50, 0 },

	{ 2687,	3124, 	50, 0 },
	{ 2687,	3124, 	50, 0 },
	{ 2687,	3124, 	50, 0 },
	{ 2687,	3124, 	50, 0 },
	{ 2687,	3124, 	50, 0 },

	{ 2687,	312, 	50, 0 },
	{ 2687,	312, 	50, 0 },
	{ 2687,	312, 	50, 0 },
	{ 2687,	312, 	50, 0 },
	{ 2687,	312, 	50, 0 },

	{ 2687,	31, 	50, 0 },
	{ 2687,	31, 	50, 0 },
	{ 2687,	31, 	50, 0 },
	{ 2687,	31, 	50, 0 },
	{ 2687,	31, 	50, 0 },

	// 2) updating prescaler, only

	{ 2000,	31, 	50, 0 },
	{ 2000,	31, 	50, 0 },
	{ 2000,	31, 	50, 0 },
	{ 2000,	31, 	50, 0 },
	{ 2000,	31, 	50, 0 },

	{ 100,	31, 	50, 0 },
	{ 100,	31, 	50, 0 },
	{ 100,	31, 	50, 0 },
	{ 100,	31, 	50, 0 },
	{ 100,	31, 	50, 0 },

	{ 20,	31, 	50, 0 },
	{ 20,	31, 	50, 0 },
	{ 20,	31, 	50, 0 },
	{ 20,	31, 	50, 0 },
	{ 20,	31, 	50, 0 },

	// 1) updating counter period, again
	{ 20,	17, 	50, 0 },
	{ 20,	17, 	50, 0 },
	{ 20,	17, 	50, 0 },
	{ 20,	17, 	50, 0 },
	{ 20,	17, 	50, 0 },

#if 0 // 1,272,723 Hz
	// and get higher
	{ 5,	10, 	50, 0 },
	{ 5,	10, 	50, 0 },
	{ 5,	10, 	50, 0 },
	{ 5,	10, 	50, 0 },
	{ 5,	10, 	50, 0 },
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
	uint64_t tmpVal;
	for (int i = 0; i < PWMSettingsNo; ++i) {
		tmpVal = pwmSettings[i].dutyCyclePercent; tmpVal *= pwmSettings[i].counterPeriod; tmpVal /= 100;
		pwmSettings[i].dutyCycleAbolute = tmpVal;
	}

	tmpVal = sFixedPWMSettings.dutyCyclePercent; tmpVal *= sFixedPWMSettings.counterPeriod; tmpVal /= 100;
	sFixedPWMSettings.dutyCycleAbolute = tmpVal;
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
		switch (sUseFixedSettings) {
		case UFS_Enable_c:
			TIM2->PSC  = sFixedPWMSettings.prescaler;
			TIM2->ARR  = sFixedPWMSettings.counterPeriod;
			TIM2->CCR1 = sFixedPWMSettings.dutyCycleAbolute;
			sUseFixedSettings = UFS_Enabled_c;
			break;
		case UFS_Enabled_c:
			break; // just continue w/o changes
		case UFS_Disable_c:
			// update unconditionally
			TIM2->PSC  = pwmSettings[PWMSettingsIndex].prescaler;
			TIM2->ARR  = pwmSettings[PWMSettingsIndex].counterPeriod;
			TIM2->CCR1 = pwmSettings[PWMSettingsIndex].dutyCycleAbolute;
			sUseFixedSettings = UFS_Off_c;
			break;
		case UFS_Off_c:
			{
				// update conditionally

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

				if (pwmSettings[oldPWMSettingsIndex].dutyCycleAbolute != pwmSettings[PWMSettingsIndex].dutyCycleAbolute) {
					TIM2->CCR1 = pwmSettings[PWMSettingsIndex].dutyCycleAbolute;
				}
			}
			break;
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


// getter
uint16_t TaskPWM_GetFixedPrescaler()
{
	return sFixedPWMSettings.prescaler;
}

uint32_t TaskPWM_GetFixedCounter()
{
	return sFixedPWMSettings.counterPeriod;
}

uint8_t  TaskPWM_GetFixedDutyCyclePercent()
{
	return sFixedPWMSettings.dutyCyclePercent;
}

uint32_t  TaskPWM_GetFixedDutyCycleAbsolute()
{
	return sFixedPWMSettings.dutyCycleAbolute;
}

// setter
void TaskPWM_SetFixedPrescaler(uint16_t value)
{
	sFixedPWMSettings.prescaler = value;
}

void TaskPWM_SetFixedCounter(uint32_t value)
{
	sFixedPWMSettings.counterPeriod = value;
	uint64_t tmpVal = sFixedPWMSettings.dutyCyclePercent; tmpVal *= value; tmpVal /= 100;
	sFixedPWMSettings.dutyCycleAbolute = tmpVal;

}

void TaskPWM_SetFixedDutyCyclePercent(uint8_t value)
{
	sFixedPWMSettings.dutyCyclePercent = value;
	uint64_t tmpVal = value; tmpVal *= sFixedPWMSettings.counterPeriod ; tmpVal /= 100;
	sFixedPWMSettings.dutyCycleAbolute = tmpVal;
}

void TaskPWM_SetFixedDutyCycleAbsolute(uint32_t value)
{
	sFixedPWMSettings.dutyCycleAbolute = value;
	uint64_t tmpVal = 100; tmpVal *= value; tmpVal /= sFixedPWMSettings.counterPeriod;
	sFixedPWMSettings.dutyCyclePercent = tmpVal;
}

void TaskPWM_UseFixedSettings(UseFixedSettings_t value)
{
	sUseFixedSettings = value;
}

#endif // ENABLE_PWM
