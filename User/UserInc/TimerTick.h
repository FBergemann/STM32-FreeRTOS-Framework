/*
 * TimerTick.h
 *
 *  Created on: Jul 7, 2021
 *      Author: frank
 */

#ifndef USERINC_TIMERTICK_H_
#define USERINC_TIMERTICK_H_

#define TASK_DELAY_MS(x)	(( TickType_t ) ((x) * (configTICK_RATE_HZ / 1000)))
#define TASK_DELAY_S(x)		(( TickType_t ) ((x) * configTICK_RATE_HZ))

#endif /* USERINC_TIMERTICK_H_ */
