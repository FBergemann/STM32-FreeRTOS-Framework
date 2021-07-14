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

#endif /* USERINC_TASKS_TASKPWM_H_ */

#endif // ENABLE_PWM
