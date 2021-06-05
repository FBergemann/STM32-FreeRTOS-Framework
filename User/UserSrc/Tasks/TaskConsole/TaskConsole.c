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

#include "UserInc/Logging.h"
#include "UserInc/Tasks/TaskConsole.h"

extern UART_HandleTypeDef huart3;

#define LOG_BUFF_SIZE 10240 // 10 KByte

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
	Log(LC_Console_c, "start TaskConsole...\r\n");

	while (1) {
		if (writePtr != readPtr) {
			HAL_UART_Transmit(&huart3, (uint8_t *)readPtr, 1, 0xFFFF);
			readPtr += 1;
			if (readPtr > endPtr) readPtr = logBuffer;
		}
		osDelay(2); // TODO don't use timeout, but interrupt
	}
}

/*
 *  TODO:
 *  1) currently, this just overwrites not yet flushed data
 *     there should be at least an overwrite indicator in the output
 *  2) use atomic updates for writePtr acess
 */
static void AddLogCore(const char* str, const size_t len)
{
	size_t noWrapLen = endPtr - writePtr + 1;
	if (noWrapLen >= len) {
		memcpy(writePtr, str, len);
		writePtr += len;
		if (writePtr > endPtr) writePtr = logBuffer;
	}
	else {
		memcpy(writePtr, str, noWrapLen);
		size_t restLen = len - noWrapLen;
		memcpy(logBuffer, str + noWrapLen, restLen);
		writePtr = logBuffer + restLen;
	}
}

static char prefix[] = "2021-05-01 12:30:00.123:ADC:";
static size_t lenPrefix = sizeof(prefix) - 1;

void TaskConsole_AddLog(const LogClient_t logClient, const char* str)
{
	if (str == NULL) return;
	size_t lenStr = strlen(str);
	size_t len = lenPrefix + lenStr + 1;
	if (len > LOG_BUFF_SIZE) return;

	osSemaphoreWait(osSemaphore, 0);
		memcpy(prefix+24, LogClientID2String(logClient), 3);
		AddLogCore(prefix, lenPrefix);
		AddLogCore(str, lenStr + 1);

	osSemaphoreRelease(osSemaphore);
}
