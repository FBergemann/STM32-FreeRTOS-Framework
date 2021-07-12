/*
 * Task3.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include "main.h"
#include <cmsis_os.h>

#include "UserInc/Logging.h"

void Task3_Run(void * argument)
{
	LogWait4Ready();

	Log(LC_LED3_c, "start Task3...\r\n");

	while (1) {
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		osDelay(125);
	}
}
