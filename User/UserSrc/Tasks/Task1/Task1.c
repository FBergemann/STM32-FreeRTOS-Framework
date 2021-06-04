/*
 * Task1.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>
#include <cmsis_os.h>

#include <stdio.h> // TODO

void Task1_Run(void const * argument)
{
  printf("start Task1...\r\n"); // TODO

  while (1)
  {
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    osDelay(500);
  }
}
