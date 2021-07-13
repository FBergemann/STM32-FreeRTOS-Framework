/*
 * TaskADC.h
 *
 *  Created on: Jun 5, 2021
 *      Author: frank
 */

#include "UserInc/Features.h"

#if ( ENABLE_ADC == 1 )

#ifndef USERINC_TASKS_TASKADC_H_
#define USERINC_TASKS_TASKADC_H_

void TaskADC_Run(void * argument);

#endif /* USERINC_TASKS_TASKADC_H_ */

#endif
