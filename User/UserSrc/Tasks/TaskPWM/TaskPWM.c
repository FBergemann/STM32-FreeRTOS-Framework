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

static TickType_t sInterval = TASK_DELAY_S(5);
static TickType_t xLastWakeTime = 0;

void TaskPWM_Interrupt(TIM_HandleTypeDef *htim5)
{
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
}

void TaskPWM_Run(void * argument)
{
	Log(LC_PWM_c, "start TaskPWM...\r\n");

	while (1) {
		xLastWakeTime = xTaskGetTickCount();				// get timer tick timestamp
		Log(LC_PWM_c, "TaskPWM loop\r\n");

		// TODO: dummy
		osDelay(300);

		vTaskDelayUntil( &xLastWakeTime, sInterval);		// wait for remaining #sInterval ticks for next cycle
	};
}
