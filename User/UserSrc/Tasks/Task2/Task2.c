/*
 * Task2.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>

void Task2_Interrupt()
{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
