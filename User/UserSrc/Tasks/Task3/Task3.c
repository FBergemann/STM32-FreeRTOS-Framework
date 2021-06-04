/*
 * Task3.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>
#include <cmsis_os.h>

#include <stdio.h> // TODO

void Task3_Run(void const * argument)
{
  printf("start Task3...\r\n"); // TODO

  while (1)
  {
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    osDelay(125);
  }
}
