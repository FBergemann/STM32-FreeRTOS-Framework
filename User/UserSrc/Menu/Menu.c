/*
 * Menu.c
 *
 *  Created on: Aug 7, 2021
 *      Author: frank
 */

#include "stm32f7xx_hal.h" // TODO: is there no generic wrapper for this?

#include "UserInc/Logging.h"
#include "UserInc/Tasks/TaskPWM.h"
#include "UserInc/Menu.h"

#include <errno.h>
#include <inttypes.h>

typedef enum {
	Menu_Off_c = 0,
	Menu_Top_c,
		Menu_StartStop_c,
		Menu_Overrule_c,
			Menu_Overrule_PWM_c,
				Menu_Overrule_PWM_Enable_c,
				Menu_Overrule_PWM_Disable_c,
	Menu_Get_Value_c,
} MenuState_t;

static MenuState_t sMenuState = Menu_Off_c;
static MenuState_t sMenuStateShadow = Menu_Off_c;
static uint8_t sMenuStateShadowLastKey;

#if 0
static char sKeyMsg[] = "key pressed '###'\r\n";
#endif

static char sEnterValue[] = "Enter value: ";
static char sSingleKey[] = "x";

static char sGetValue[256];
static size_t sGetValuePos=0;

static uint32_t sValueUint32;
static uint16_t sValueUint16;
static uint8_t  sValueUint8;

static void MenuHandle_Top(uint8_t rcvByte);
static void MenuHandle_StartStop(uint8_t rcvByte);
static void MenuHandle_Overrule(uint8_t rcvByte);
static void MenuHandle_Overrule_PWM(uint8_t rcvByte);
static void MenuHandle_Overrule_PWM_Enable(uint8_t rcvByte);
static void MenuHandle_Overrule_PWM_Disable(uint8_t rcvByte);

void MenuHandle_Shadow(uint8_t rcvByte);

static bool Parse_Uint16(char *str, uint16_t *value);

void ShowMenu_Top()
{
	sMenuState = Menu_Top_c;

	Log(LC_Console_c, "********\r\n");
	Log(LC_Console_c, "Top Menu\r\n");
	Log(LC_Console_c, "********\r\n");
	Log(LC_Console_c, "s|S: start/stop tasks\r\n");
	Log(LC_Console_c, "o|O: overrule task\r\n");
	Log(LC_Console_c, "r|R: reset\r\n");
	Log(LC_Console_c, "\r\n");
	Log(LC_Console_c, "x|X: exit\r\n");
}

void ShowMenu_StartStop()
{
	Log(LC_Console_c, "**************\r\n");
	Log(LC_Console_c, "Star/Stop Task\r\n");
	Log(LC_Console_c, "**************\r\n");
	Log(LC_Console_c, "1: ADC\r\n");
	Log(LC_Console_c, "2: PWM\r\n");
	Log(LC_Console_c, "3: USB Log\r\n");
	Log(LC_Console_c, "\r\n");
	Log(LC_Console_c, "x|X: exit\r\n");
}

void ShowMenu_Overrule()
{
	Log(LC_Console_c, "*************\r\n");
	Log(LC_Console_c, "Overrule Task\r\n");
	Log(LC_Console_c, "*************\r\n");
	Log(LC_Console_c, "1: PWM\r\n");
	Log(LC_Console_c, "\r\n");
	Log(LC_Console_c, "x|X: exit\r\n");
}

void ShowMenu_Overrule_PWM()
{
	Log(LC_Console_c, "************\r\n");
	Log(LC_Console_c, "Overrule PWM\r\n");
	Log(LC_Console_c, "************\r\n");
	Log(LC_Console_c, "e|E enable (update)\r\n");
	Log(LC_Console_c, "d|D: disable\r\n");
	Log(LC_Console_c, "\r\n");
	Log(LC_Console_c, "x|X: exit\r\n");
}

static char sPrescaler[]			= "p|P: prescaler      #.....\r\n";			// 16 bit prescaler
static char sCounter[]				= "c|C: counter        #..........\r\n";	// 32 bit counter
static char sDutyCyclePercent[]		= "d:   duty cycle     ... %\r\n";			// duty cycle in %
static char sDutyCycleAbsolute[]	= "D:   duty cycle     #..........\r\n";	// duty cycle absolute

void ShowMenu_Overrule_PWM_Enable()
{
	LogUInt16ToStr(sPrescaler			+ 21, TaskPWM_GetFixedPrescaler(), 5);
	LogUInt32ToStr(sCounter				+ 21, TaskPWM_GetFixedCounter(), 10);
	LogUInt8ToStr (sDutyCyclePercent	+ 20, TaskPWM_GetFixedDutyCyclePercent(), 3);
	LogUInt32ToStr(sDutyCycleAbsolute	+ 21, TaskPWM_GetFixedDutyCycleAbsolute(), 10);

	Log(LC_Console_c, "*******************\r\n");
	Log(LC_Console_c, "Enable Overrule PWM\r\n");
	Log(LC_Console_c, "*******************\r\n");
	Log(LC_Console_c, sPrescaler);
	Log(LC_Console_c, sCounter);
	Log(LC_Console_c, sDutyCyclePercent);
	Log(LC_Console_c, sDutyCycleAbsolute);
	Log(LC_Console_c, "t|T: take over\r\n");
	Log(LC_Console_c, "\r\n");
	Log(LC_Console_c, "x|X: exit\r\n");
}


void MenuHandle(uint8_t rcvByte)
{
#if 0
    LogIntToStr(sKeyMsg+13, rcvByte, 3);
    Log(LC_Console_c, sKeyMsg);
#endif

	switch (sMenuState)
	{
	case Menu_Off_c:
	case Menu_Top_c:
		MenuHandle_Top(rcvByte);
		break;

		case Menu_StartStop_c:
			MenuHandle_StartStop(rcvByte);
			if (sMenuState == Menu_Off_c) {
				MenuHandle_Top(rcvByte);
			}
			break;

		case Menu_Overrule_c:
			MenuHandle_Overrule(rcvByte);
			if (sMenuState == Menu_Off_c) {
				MenuHandle_Top(rcvByte);
			}
			break;

			case Menu_Overrule_PWM_c:
				MenuHandle_Overrule_PWM(rcvByte);
				if (sMenuState == Menu_Off_c) {
					MenuHandle_Overrule(rcvByte);
				}
				break;

				case Menu_Overrule_PWM_Enable_c:
					MenuHandle_Overrule_PWM_Enable(rcvByte);
					if (sMenuState == Menu_Off_c) {
						MenuHandle_Overrule_PWM(rcvByte);
					}
					break;

				case Menu_Overrule_PWM_Disable_c:
					MenuHandle_Overrule_PWM_Disable(rcvByte);
					if (sMenuState == Menu_Off_c) {
						MenuHandle_Overrule_PWM(rcvByte);
					}
					break;
		case Menu_Get_Value_c:
			if (rcvByte == '\r') {
				LogNoPrefix(LC_Console_c, "\r\n");
				sGetValue[sGetValuePos] = '\0';
				MenuHandle_Shadow(rcvByte);
				sGetValuePos = 0;
			}
			else {
				sSingleKey[0] = rcvByte;
				LogNoPrefix(LC_Console_c, sSingleKey);
				sGetValue[sGetValuePos] = rcvByte;
				sGetValuePos++;
				sGetValuePos %= sizeof(sGetValue) - 1;
			}
	}
}

void MenuHandle_Top(uint8_t rcvByte)
{
	if (sMenuState == Menu_Off_c) {
		LogSetSingleMode(LC_Console_c);
	}

	if (sMenuState != Menu_Top_c) {
		ShowMenu_Top();
		sMenuState = Menu_Top_c;
		return; // early exit
	}

	sMenuState = Menu_Top_c;

	switch(rcvByte) {
	case 's':
	case 'S':
		MenuHandle_StartStop(rcvByte);
		break;
	case 'o':
	case 'O':
		MenuHandle_Overrule(rcvByte);
		break;
	case 'r':
	case 'R':
		HAL_NVIC_SystemReset();
		break;
	case 'x':
	case 'X':
		LogSetSingleMode(LC_EOL_c);
		sMenuState = Menu_Off_c;
		break;
	}
}

void MenuHandle_StartStop(uint8_t rcvByte)
{
	if (sMenuState != Menu_StartStop_c) {
		ShowMenu_StartStop();
		sMenuState = Menu_StartStop_c;
		return; // early exit
	}

	sMenuState = Menu_StartStop_c;

	switch(rcvByte) {
	case '1': /* ADC */
		break;
	case '2': /* PWM */
		break;
	case '3': /* USB Log */
		break;
	case 'x':
	case 'X':
		sMenuState = Menu_Off_c; // THIS menu off, leave up to the caller to reset properly
		break;
	}
}

void MenuHandle_Overrule(uint8_t rcvByte)
{
	if (sMenuState != Menu_Overrule_c) {
		ShowMenu_Overrule();
		sMenuState = Menu_Overrule_c;
		return; // early exit
	}

	sMenuState = Menu_Overrule_c;

	switch(rcvByte) {
	case '1': /* PWM */
		MenuHandle_Overrule_PWM(rcvByte);
		break;
	case 'x':
	case 'X':
		sMenuState = Menu_Off_c; // THIS menu off, leave up to the caller to reset properly
		break;
	}
}

void MenuHandle_Overrule_PWM(uint8_t rcvByte)
{
	if (sMenuState != Menu_Overrule_PWM_c) {
		ShowMenu_Overrule_PWM();
		sMenuState = Menu_Overrule_PWM_c;
		return; // early exit
	}

	sMenuState = Menu_Overrule_PWM_c;

	switch(rcvByte) {
	case 'e': /* enable (update) */
	case 'E':
		MenuHandle_Overrule_PWM_Enable(rcvByte);
		break;
	case 'd': /* disable */
	case 'D':
		MenuHandle_Overrule_PWM_Disable(rcvByte);
		break;
	case 'x':
	case 'X':
		sMenuState = Menu_Off_c; // THIS menu  off, leave up to the caller to reset properly
		break;
	}
}

void MenuHandle_Overrule_PWM_Enable(uint8_t rcvByte)
{

	if (sMenuState != Menu_Overrule_PWM_Enable_c) {
		ShowMenu_Overrule_PWM_Enable();
		sMenuState = Menu_Overrule_PWM_Enable_c;
		return; // early exit
	}

	sMenuState = Menu_Overrule_PWM_Enable_c;

	switch(rcvByte) {
	case 'p': /* prescaler */
	case 'P':
	case 'c': /* counter */
	case 'C':
	case 'd': /* duty cycle in percent */
	case 'D': /* duty cycle absolute */
		Log(LC_Console_c, sEnterValue);
		sMenuStateShadow = sMenuState;
		sMenuStateShadowLastKey = rcvByte;
		sMenuState = Menu_Get_Value_c;
		break;
	case 't':
	case 'T':
		TaskPWM_UseFixedSettings(UFS_Enable_c);
		sMenuState = Menu_Off_c; // THIS menu  off, leave up to the caller to reset properly
		break;
	case 'x':
	case 'X':
		sMenuState = Menu_Off_c; // THIS menu  off, leave up to the caller to reset properly
		break;
	}
}

void MenuHandle_Shadow(uint8_t rcvByte)
{
	sMenuState = sMenuStateShadow; // restore

	switch (sMenuState) {
	case Menu_Off_c:
	case Menu_Top_c:
	case Menu_StartStop_c:
	case Menu_Overrule_c:
	case Menu_Overrule_PWM_c:
	case Menu_Overrule_PWM_Disable_c:
	case Menu_Get_Value_c:
		break;

	case Menu_Overrule_PWM_Enable_c:
		{
			switch (sMenuStateShadowLastKey) {
			case 'p':
			case 'P':
				if (Parse_Uint16(sGetValue, &sValueUint16)) {
					TaskPWM_SetFixedPrescaler(sValueUint16);
				}
				break;
			}
		}
		ShowMenu_Overrule_PWM_Enable();
		sMenuState = Menu_Overrule_PWM_Enable_c;
		break;
	}
}

void MenuHandle_Overrule_PWM_Disable(uint8_t rcvByte)
{
	sMenuState = Menu_Overrule_PWM_Disable_c;

	TaskPWM_UseFixedSettings(UFS_Disable_c);

	sMenuState = Menu_Off_c; // unconditionally back to parent menu
}

bool Parse_Uint16(char *str, uint16_t *value)
{
	if (str == NULL) return false;
	if (value == NULL) return false;

	char *end;
	errno = 0;
	intmax_t val = strtoumax(str, &end, 10);
	if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0')
		return false;
	*value = (uint16_t) val;

	return true;
}
