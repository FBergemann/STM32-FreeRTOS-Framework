/*
 * TaskUSB.c
 *
 *  Created on: Jun 21, 2021
 *      Author: frank
 */

#include "main.h"
#include <cmsis_os.h>

#include "UserInc/Logging.h"

void TaskUSB_ApplicationStart()
{
	Log(LC_USB_c, "ApplicationStart\r\n");
}

void TaskUSB_ApplicationReady()
{
	Log(LC_USB_c, "ApplicationReady\r\n");
}

void TaskUSB_ApplicationDisconnect()
{
	Log(LC_USB_c, "ApplicationDisconnect\r\n");
}
