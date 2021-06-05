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

void Log(const LogClient_t logClient, const char* str)
{
	TaskConsole_AddLog(logClient, str);
}

char* LogClientID2String(const LogClient_t logClient)
{
	switch (logClient) {
	case LC_Main_c :	return "M  "; break;
	case LC_Console_c:	return "CON"; break;
	case LC_LED1_c :	return "L1 "; break;
	case LC_LED2_c :	return "L2 "; break;
	case LC_LED3_c :	return "L3 "; break;
	case LC_ADC_c  :	return "ADC"; break;
	}
	return "?  ";
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
