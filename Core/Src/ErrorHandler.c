/*
 * ErrorHandler.c
 *
 *  Created on: May 31, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

#include "cmsis_os.h"

#include "ErrorHandler.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
