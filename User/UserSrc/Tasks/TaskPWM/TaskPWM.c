/*
 * Task2.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>

#include "UserInc/Logging.h"

void TaskPWM_Interrupt()
{
//	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	Log(LC_PWM_c, "tick...\r\n");
}
