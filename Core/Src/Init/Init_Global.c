/*
 * Init_Global.c
 *
 *  Created on: Jun 3, 2021
 *      Author: frank
 */

#include "Init/Init_Global.h"

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

void Init_Global(void)
{
	CPU_CACHE_Enable();
}
