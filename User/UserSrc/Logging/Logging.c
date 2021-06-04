/*
 * Logging.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include <main.h>

#include <stdio.h>

#include "UserInc/Tasks/TaskConsole.h"

void Log(const char* str)
{
	TaskConsole_AddLog(str);
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
