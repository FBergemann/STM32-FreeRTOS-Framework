/*
 * TaskPWM.h
 *
 *  Created on: Jul 5, 2021
 *      Author: frank
 */

#include "UserInc/Features.h"

#if ( ENABLE_PWM == 1 )

#ifndef USERINC_TASKS_TASKPWM_H_
#define USERINC_TASKS_TASKPWM_H_

void TaskPWM_Interrupt(TIM_HandleTypeDef * htim5);
void TaskPWM_Run(void * argument);

uint16_t TaskPWM_GetFixedPrescaler();
uint32_t TaskPWM_GetFixedCounter();
uint8_t  TaskPWM_GetFixedDutyCyclePercent();
uint32_t TaskPWM_GetFixedDutyCycleAbsolute();

void TaskPWM_SetFixedPrescaler(uint16_t value);
void TaskPWM_SetFixedCounter(uint32_t value);
void TaskPWM_SetFixedDutyCyclePercent(uint8_t value);
void TaskPWM_SetFixedDutyCycleAbolute(uint32_t value);

#endif /* USERINC_TASKS_TASKPWM_H_ */

#endif // ENABLE_PWM

