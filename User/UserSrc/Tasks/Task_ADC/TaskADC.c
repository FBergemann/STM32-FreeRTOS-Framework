/*
 * TaskADC.c
 *
 *  Created on: Jun 5, 2021
 *      Author: frank
 */

#include "UserInc/Features.h"

#if ( ENABLE_ADC == 1 )

#include "main.h"
#include <cmsis_os.h>

#include "UserInc/Logging.h"

#define SIMULATION_ADC

static char logLine[] = "### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ###\r\n";
static char logElem = 0;

#ifdef SIMULATION_ADC

void TaskADC_Run(void * argument)
{
	LogWait4Ready();

	Log(LC_ADC_c, "start TaskADC...\r\n");

	int32_t value = 0;
	while (1) {

		LogIntToStr(logLine + (logElem<<2), value, 3);
		if (logElem == 15) {
			Log(LC_ADC_c, logLine);
		}

		logElem = (logElem + 1) % 16;
		value = (value + 1) % 1000;

		osDelay(100);
	}
}

#else

void TaskADC_Run(void const * argument)
{
  Log(LC_ADC_c, "start TaskADC...\r\n");

  while (1) {
    osDelay(500);
  }
}

#endif

#endif
