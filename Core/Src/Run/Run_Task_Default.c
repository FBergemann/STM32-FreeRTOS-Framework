/*
 * Task_Default.c
 *
 *  Created on: May 31, 2021
 *      Author: frank
 */

#include <Run/Run_Task_Default.h>
#include "Init/Init_Global.h"

#include "cmsis_os.h"

#include "stdio.h"


/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void Run_Task_Default(void const * argument)
{
  /* USER CODE BEGIN 5 */
  printf("enter run loop...\n\t");

  /* endless loop */
  int idx = 0;
  for(;;)
  {
    printf("#%03d:in run loop...\n\r", idx);
    idx = idx + 1;
    osDelay(1000);
  }
  /* USER CODE END 5 */
}
