/*
 * Task2.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>
#include <cmsis_os.h>

#include "UserInc/Logging.h"
#include "UserInc/TimerTick.h"

static TickType_t sInterval = TASK_DELAY_S(1);
static TickType_t xLastWakeTime = 0;
static uint32_t sCounter = 0;
static char sBuff[] = "TaskPWM: pulses = #.....\r\n";

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

	LogWait4Ready();

	Log(LC_PWM_c, "start TaskPWM...\r\n");

	while (1) {
		xLastWakeTime = xTaskGetTickCount();				// get timer tick timestamp

		copyCounter = sCounter;
		counterDiff = copyCounter - lastCounter;
		lastCounter = copyCounter;

		LogUInt32ToStr(sBuff + 19, counterDiff, 5);
		Log(LC_PWM_c, sBuff);

		vTaskDelayUntil( &xLastWakeTime, sInterval);		// wait for remaining #sInterval ticks for next cycle
	};
}
