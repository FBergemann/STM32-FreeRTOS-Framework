/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include <Init/Init_Global.h>
#include <Init/Init_SystemClock.h>
#include <Init/Init_Usart3Uart.h>
#include <Init/Init_MxGpio.h>
#include <Init/Init_MxUsbOtgFsPcdInit.h>
// #include <Init/Init_MxEthernet.h>

#include <Init/Tasks/Init_Task_Default.h>
#include <Init/Tasks/Init_Task_Led1.h>
#include <Init/Tasks/Init_Task_Led2.h>
#include <Init/Tasks/Init_Task_Led3.h>

#include <Run/Run_Task_Default.h>
#include <Run/Run_Task_Led1.h>
#include <Run/Run_Task_Led2.h>
#include <Run/Run_Task_Led3.h>

#include <ErrorHandler.h>

#include <cmsis_os.h>
#include <stdio.h>


static osThreadId thr_default;
static osThreadId thr_led1;
static osThreadId thr_led2;
static osThreadId thr_led3;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  Init_Global();
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
//  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();

  Init_Task_Led1();
  Init_Task_Led2();
  Init_Task_Led3();
  Init_Task_Default();

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, Run_Task_Default, osPriorityNormal, 0, 128);
  thr_default = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(led1Task, Run_Task_Led1, osPriorityNormal, 0, 128);
  thr_led1 = osThreadCreate(osThread(led1Task), NULL);

  osThreadDef(led2Task, Run_Task_Led2, osPriorityNormal, 0, 128);
  thr_led2 = osThreadCreate(osThread(led2Task), NULL);

  osThreadDef(led3Task, Run_Task_Led3, osPriorityNormal, 0, 128);
  thr_led3 = osThreadCreate(osThread(led3Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  osDelay(1000);
  }
  /* USER CODE END 3 */
}

/* Private function prototypes -----------------------------------------------*/

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
