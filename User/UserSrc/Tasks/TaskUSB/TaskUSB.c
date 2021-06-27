/*
 * TaskUSB.c
 *
 *  Created on: Jun 21, 2021
 *      Author: frank
 */

#include "main.h"
#include <cmsis_os.h>

#include "UserInc/Logging.h"

typedef enum {
	Disconnected_c = 0,
	Connected_c,
} State_t;

static State_t sState = Disconnected_c;

static void InitConnection();

void TaskUSB_ApplicationStart()
{
	// Log(LC_USB_c, "ApplicationStart\r\n");
	sState = Connected_c;
}

void TaskUSB_ApplicationReady()
{
	// Log(LC_USB_c, "ApplicationReady\r\n");
}

void TaskUSB_ApplicationDisconnect()
{
	// Log(LC_USB_c, "ApplicationDisconnect\r\n");
	sState = Disconnected_c;
}


void TaskUSB_PrepareRTOS()
{
}

void TaskUSB_Run(void * argument)
{
	Log(LC_Console_c, "start TaskUSB...\r\n");
	State_t lastState = sState;

	while (1) {
		if (sState != lastState) {
			switch (sState) {
			case Disconnected_c:
				Log(LC_USB_c, "Disconnected\r\n");
				break;
			case Connected_c:
				Log(LC_USB_c, "Connected\r\n");
				InitConnection();
				break;
			}
			lastState = sState;
		}
	}
}

static void InitConnection()
{
	// Log(LC_USB_c, "set-up...\r\n");
}
