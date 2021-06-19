/*
 * Task1.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include "main.h"
#include <cmsis_os.h>

#include "UserInc/Logging.h"

void Task1_Run(void * argument)
{
  Log(LC_LED1_c, "start Task1...\r\n");

  while (1) {
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    osDelay(500);
  }
}
