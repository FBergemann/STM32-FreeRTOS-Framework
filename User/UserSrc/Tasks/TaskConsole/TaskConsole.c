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
extern RTC_HandleTypeDef hrtc;

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
		char *writePtrCopy = writePtr;
		if (writePtrCopy != readPtr) {
			HAL_UART_StateTypeDef x;
			if (writePtrCopy > readPtr) {
				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)readPtr, writePtrCopy - readPtr);
			}
			else {
				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)readPtr, endPtr - readPtr + 1);
				while ( (x = HAL_UART_GetState(&huart3)) != HAL_UART_STATE_READY); // TODO
				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)logBuffer, writePtrCopy - logBuffer);
			}
			while ( (x = HAL_UART_GetState(&huart3)) != HAL_UART_STATE_READY); // TODO
			readPtr = writePtrCopy;
			continue; // check immediately again, no timeout
		}
		osDelay(1);
	}
}

/*
 *  TODO:
 *  1) currently, this just overwrites not yet flushed data
 *     there should be at least an overwrite indicator in the output
 */
static void AddLogCore(const char* str, const size_t len)
{
	size_t noWrapLen = endPtr - writePtr + 1;
	if (noWrapLen >= len) {
		memcpy(writePtr, str, len);
		char *writePtrCopy = writePtr + len;
		if (writePtrCopy > endPtr) writePtrCopy = logBuffer;
		writePtr = writePtrCopy; // atomic
	}
	else {
		memcpy(writePtr, str, noWrapLen);
		size_t restLen = len - noWrapLen;
		memcpy(logBuffer, str + noWrapLen, restLen);
		char *writePtrCopy = logBuffer + restLen;
		writePtr = writePtrCopy; // atomic
	}
}

void TaskConsole_AddLog(const LogClient_t logClient, const char* str)
{
	if (str == NULL) return;
	size_t lenStr = strlen(str);

	size_t lenPrefix;
	char* prefix = LogMakePrefix(logClient, &lenPrefix);

	size_t len = lenPrefix + lenStr + 1;
	if (len > LOG_BUFF_SIZE) return;

	osSemaphoreWait(osSemaphore, 0);
		AddLogCore(prefix, lenPrefix);
		AddLogCore(str, lenStr + 1);
	osSemaphoreRelease(osSemaphore);
}
