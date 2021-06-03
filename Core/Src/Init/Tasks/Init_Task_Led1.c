/*
 * Init_Task_Led1.c
 *
 *  Created on: Jun 3, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

#include "Init/Tasks/Init_Task_Led1.h"

static GPIO_InitTypeDef GPIO_InitStruct;

void Init_Task_Led1()
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin	= LD1_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);
}

