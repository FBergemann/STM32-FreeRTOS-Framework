/*
 * Init_Task_Led2.c
 *
 *  Created on: Jun 3, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

#include "ErrorHandler.h"

#include "Init/Tasks/Init_Task_Led2.h"

#ifndef TASK_LED2_USE_NEW_VERSION
static GPIO_InitTypeDef GPIO_InitStruct;

void Init_Task_Led2()
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = LD2_Pin;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

#else

static GPIO_InitTypeDef GPIO_InitStruct;
static TIM_HandleTypeDef TimHandle;
static __IO uint32_t uwPrescalerValue = 0;

void Init_Task_Led2()
{
  // init LED ------------------------------------------------
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin	= LD2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  // init Timer ----------------------------------------------
  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  uwPrescalerValue = (uint32_t)((SystemCoreClock/ 2) / 10000) - 1;

  /* Set TIMx instance */
  TimHandle.Instance = TIMx;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = ((SystemCoreClock / 2)/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 10000 - 1;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }

}
#endif
