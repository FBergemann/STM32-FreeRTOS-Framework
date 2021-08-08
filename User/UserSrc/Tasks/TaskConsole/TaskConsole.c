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
#include <stdbool.h>

#include "UserInc/Logging.h"
#include "UserInc/Menu.h"
#include "UserInc/Tasks/TaskConsole.h"

extern UART_HandleTypeDef huart3;
extern RTC_HandleTypeDef hrtc;

#define LOG_BUFF_SIZE 10240 // 10 KByte

static char logBuffer[LOG_BUFF_SIZE];
static char* readPtr  = logBuffer;
static char* writePtr = logBuffer;
static char* endPtr = logBuffer + LOG_BUFF_SIZE - 1;

bool rtosInitialized = false;
static bool ready = false;

osSemaphoreId_t LogSemInstance;
osSemaphoreId_t UsartDMAInstance;

void TaskConsole_PrepareRTOS()
{
	LogSemInstance		= osSemaphoreNew(1, 1, NULL);
	UsartDMAInstance	= osSemaphoreNew(1, 1, NULL);
	rtosInitialized		= true;
}

void TaskConsole_USART3_DMA_IRQ()
{
	if (rtosInitialized) {
		osStatus_t status;
		while ((status = osSemaphoreRelease(UsartDMAInstance)) != osOK) { // TODO: error handling
			;
		}
	}
}

/*
 * input handling
 */
static uint8_t rcvByte;

/*
 * TOOD it's not good design to have this generic function here, because we are dealing witrh huart3 here, only
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart3) {
		HAL_UART_Receive(&huart3, &rcvByte, 1, 10);
	}
	else {
		// TODO: error handling?
	}
}

void TaskConsole_Run(void * argument)
{
	ready = true;

	Log(LC_Console_c, "start TaskConsole...\r\n");

	while (1) {

		/*
		 * handle input
		 * TODO: we don't want to poll, but use callback
		 */
		HAL_StatusTypeDef keyPress = HAL_UART_Receive(&huart3, &rcvByte, 1, 10);
		if (keyPress == HAL_OK) {
			MenuHandle(rcvByte);
		}

		/*
		 * transmit logs
		 */
		char *writePtrCopy = writePtr;
		if (writePtrCopy != readPtr) {
			if (writePtrCopy > readPtr) {
				while (osSemaphoreAcquire(UsartDMAInstance, 0) != osOK) { ; }
				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)readPtr, writePtrCopy - readPtr);
			}
			else {
				// TODO: error handling
				while (osSemaphoreAcquire(UsartDMAInstance, 0) != osOK) { ; }
				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)readPtr, endPtr - readPtr + 1);
				while (osSemaphoreAcquire(UsartDMAInstance, 0) != osOK) { ; }
				HAL_UART_Transmit_DMA(&huart3, (uint8_t*)logBuffer, writePtrCopy - logBuffer);
			}
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

	osSemaphoreAcquire(LogSemInstance, 0);
		AddLogCore(prefix, lenPrefix);
		AddLogCore(str, lenStr + 1);
	osSemaphoreRelease(LogSemInstance);
}

void TaskConsole_WaitReady()
{
	while (1) {
		if (ready) break;
		osDelay(100);
	}
}
