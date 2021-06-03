/*
 * Task_Led3.c
 *
 *  Created on: Jun 1, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

#include "cmsis_os.h"

#include "stdio.h"

#include "Run/Run_Task_Led3.h"

void Run_Task_Led3(void const * argument)
{
  printf("start Task_Led3...\n\t");

  while (1)
  {
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    osDelay(125);
  }
}

