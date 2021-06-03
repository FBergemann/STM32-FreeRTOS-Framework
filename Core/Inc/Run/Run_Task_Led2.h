/*
 * Task_Led2.h
 *
 *  Created on: Jun 1, 2021
 *      Author: frank
 */

#ifndef INC_RUN_RUN_TASK_LED2_H_
#define INC_RUN_RUN_TASK_LED2_H_

#include "Init/Init_Global.h"

#include "stm32f7xx_hal_tim.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Run_Task_Led2(void const * argument);

#endif /* INC_RUN_RUN_TASK_LED2_H_ */
