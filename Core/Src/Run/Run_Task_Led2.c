/*
 * Task_Led2.c
 *
 *  Created on: Jun 1, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

#include "cmsis_os.h"

#include "stdio.h"

#include "Run/Run_Task_Led2.h"

#ifndef TASK_LED2_USE_NEW_VERSION
void Run_Task_Led2(void const * argument)
{
  printf("start Task_Led2...\n\t");

  while (1)
  {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    osDelay(250);
  }
}

#else
/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void Run_Task_Led2(void const * argument)
{
  printf("start Task_Led2...\n\t");

  while (1)
  {
    osDelay(1000);
  }
}
#endif
