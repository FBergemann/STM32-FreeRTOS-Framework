/*
 * TaskConsole.c
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#include "main.h"
#include <cmsis_os.h>

#include <stdio.h>
#include <string.h>

#include "UserInc/Tasks/TaskConsole.h"

extern UART_HandleTypeDef huart3;

#define LOG_BUFF_SIZE 10240 // 10KByte

static char logBuffer[LOG_BUFF_SIZE];
static char* readPtr  = logBuffer;
static char* writePtr = logBuffer;
static char* endPtr = logBuffer + LOG_BUFF_SIZE - 1;

osSemaphoreDef(LogSem);
osSemaphoreId osSemaphore;

void TaskConsole_PrepareRTOS()
{
	osSemaphore = osSemaphoreCreate(osSemaphore(LogSem), 1);
}

void TaskConsole_Run(void const * argument)
{
	TaskConsole_AddLog("start TaskConsole...\r\n");

	while (1) {
		if (readPtr != writePtr) {
			HAL_UART_Transmit(&huart3, (uint8_t *)writePtr, 1, 0xFFFF);
			writePtr = writePtr + 1;
			if (writePtr >= endPtr) writePtr = logBuffer;
		}
		osDelay(5); // TODO don't use timeout, but interrupt
	}
}

// TODO: currently, this just overwrites not yet flushed data
void TaskConsole_AddLog(const char* str)
{
	if (str == NULL) return;
	size_t len = strlen(str) + 1;
	if (len > LOG_BUFF_SIZE) return;

	osSemaphoreWait(osSemaphore, 0);

		size_t noWrapLen = endPtr - readPtr;
		if (noWrapLen > len) {
			memcpy(readPtr, str, len);
			readPtr += len;
		}
		else {
			memcpy(readPtr, str, noWrapLen);
			size_t restLen = len - noWrapLen;
			memcpy(logBuffer, str + noWrapLen, restLen);
			readPtr = logBuffer + restLen;
		}

	osSemaphoreRelease(osSemaphore);
}
