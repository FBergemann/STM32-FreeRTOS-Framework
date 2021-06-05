/*
 * Logging.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>

#include <stdio.h>

#include "UserInc/Tasks/TaskConsole.h"

#include "UserInc/Logging.h"

// log prefix definition
static char prefixPrototype[] = "2021-05-01 12:30:00.123:ADC:";
// each client has it's own log prefix, to update that lock-free
static char logPrefix[LC_EOL_c + 1][sizeof(prefixPrototype)];

void Log(const LogClient_t logClient, const char* str)
{
	TaskConsole_AddLog(logClient, str);
}

char* LogClientID2String(const LogClient_t logClient)
{
	switch (logClient) {
	case LC_Main_c:		return "M  "; break;
	case LC_Console_c:	return "CON"; break;
	case LC_LED1_c:		return "L1 "; break;
	case LC_LED2_c:		return "L2 "; break;
	case LC_LED3_c:		return "L3 "; break;
	case LC_ADC_c:		return "ADC"; break;
	case LC_EOL_c:		return "EOL"; break;
	}
	return "?  ";
}

 char* LogMakePrefix(const LogClient_t logClient, size_t* prefixLen)
{
	if (prefixLen != NULL) *prefixLen = sizeof(prefixPrototype);
//	HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);
	uint32_t ticks = HAL_GetTick();
	sprintf(	logPrefix[logClient],
				"2021-05-01 %02lu:%02lu:%02lu.%03lu:%s:",
				(ticks / (1000 * 60 * 60)) % 24,	// hours
				(ticks / (1000 * 60)) % 60, 		// minutes
				(ticks / 1000) % 60, 				// seconds
				(ticks % 1000), 					// milliseconds
				LogClientID2String(logClient));

	return logPrefix[logClient];
}

void LogIntToStr(char *dest, int value, int digits)
{
	if (dest == NULL) return;
	digits = digits - 1;
	while (digits >= 0) {
		dest[digits] = (value % 10) + '0';
		value = value / 10;
		digits = digits -1;
	}
}
