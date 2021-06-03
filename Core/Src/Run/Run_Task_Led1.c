/*
 * Task_Led1.c
 *
 *  Created on: Jun 1, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

#include "cmsis_os.h"

#include "stdio.h"

#include "Run/Run_Task_Led1.h"

void Run_Task_Led1(void const * argument)
{
  printf("start Task_Led1...\r\n");

  while (1)
  {
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    osDelay(500);
  }
}
