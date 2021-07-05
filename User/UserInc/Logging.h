/*
 * Loggin.h
 *
 *  Created on: Jun 4, 2021
 *      Author: frank
 */

#ifndef USERINC_LOGGING_H_
#define USERINC_LOGGING_H_

typedef enum {
	LC_Main_c = 0,
	LC_Console_c,
	LC_USB_c,
	LC_LED1_c,
	LC_LED2_c,
	LC_LED3_c,
	LC_ADC_c,
	LC_PWM_c,
	LC_EOL_c, 	// end marker
} LogClient_t;

void	Log(const LogClient_t logClient, const char* str);
char*	LogClientID2String(const LogClient_t logClient);
char* 	LogMakePrefix(const LogClient_t logClient, size_t* lenPrefix);

// conversions
void	LogIntToStr(char *dest, int value, int digits);

#endif /* USERINC_LOGGING_H_ */
