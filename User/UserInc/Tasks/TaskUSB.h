/*
 * TaskUSB.h
 *
 *  Created on: Jun 21, 2021
 *      Author: frank
 */

#include "UserInc/Features.h"
// #if ( ENABLE_USB == 1 )

#ifndef USERINC_TASKS_TASKUSB_H_
#define USERINC_TASKS_TASKUSB_H_

void TaskUSB_ApplicationStart();
void TaskUSB_ApplicationReady();
void TaskUSB_ApplicationDisconnect();

void TaskUSB_PrepareRTOS();
void TaskUSB_Run(void * argument);

#endif /* USERINC_TASKS_TASKUSB_H_ */

// #endif // ENABLE_USB
