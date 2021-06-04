/*
 * TaskConsole.h
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#ifndef USERINC_TASKS_TASKCONSOLE_H_
#define USERINC_TASKS_TASKCONSOLE_H_

void TaskConsole_PrepareRTOS();
void TaskConsole_Run(void const * argument);

void TaskConsole_AddLog(const char* str);

#endif /* USERINC_TASKS_TASKCONSOLE_H_ */
