/*
 * Menu.c
 *
 *  Created on: Aug 7, 2021
 *      Author: frank
 */

#include "stm32f7xx_hal.h" // TODO: is there no generic wrapper for this?

#include "UserInc/Logging.h"
#include "UserInc/Menu.h"

typedef enum {
	Menu_Off_c = 0,
	Menu_Top_c,
		Menu_StartStop_c,
		Menu_Overrule_c,
			Menu_Overrule_PWM_c,
				Menu_Overrule_PWM_Enable_c,
				Menu_Overrule_PWM_Disable_c,
} MenuState_t;

static MenuState_t sMenuState = Menu_Off_c;

static char sKeyMsg[] = "key pressed '###'\r\n";

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

void ShowMenu_Overrule_PWM_Enable()
{
	Log(LC_Console_c, "*******************\r\n");
	Log(LC_Console_c, "Enable Overrule PWM\r\n");
	Log(LC_Console_c, "*******************\r\n");
	Log(LC_Console_c, "p|P: prescaler\r\n");
	Log(LC_Console_c, "c|C: counter period\r\n");
	Log(LC_Console_c, "d|D: duty cycle [%]\r\n");
	Log(LC_Console_c, "\r\n");
	Log(LC_Console_c, "x|X: exit\r\n");
}

void MenuHandle_Top(uint8_t rcvByte);
void MenuHandle_StartStop(uint8_t rcvByte);
void MenuHandle_Overrule(uint8_t rcvByte);
void MenuHandle_Overrule_PWM(uint8_t rcvByte);
void MenuHandle_Overrule_PWM_Enable(uint8_t rcvByte);
void MenuHandle_Overrule_PWM_Disable(uint8_t rcvByte);

void MenuHandle(uint8_t rcvByte)
{
    LogIntToStr(sKeyMsg+13, rcvByte, 3);
    Log(LC_Console_c, sKeyMsg);

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
		break;
	case 'c': /* counter */
	case 'C':
		break;
	case 'd': /* duty cycle [%] */
	case 'D':
		break;
	case 'x':
	case 'X':
		sMenuState = Menu_Off_c; // THIS menu  off, leave up to the caller to reset properly
		break;
	}
}

void MenuHandle_Overrule_PWM_Disable(uint8_t rcvByte)
{
	sMenuState = Menu_Overrule_PWM_Disable_c;

	// TODO: instruct PWM module to use normal mode again

#if 0 // no own menu -> no evaluation for keys
	switch(rcvByte) {
	case 'p': /* prescaler */
	case 'P':
		break;
	case 'c': /* counter */
	case 'C':
		break;
	case 'd': /* duty cycle [%] */
	case 'D':
		break;
	case 'x':
	case 'X':
		sMenuState = Menu_Off_c; // THIS menu  off, leave up to the caller to reset properly
		break;
	}
#endif

	sMenuState = Menu_Off_c; // unconditionally back to parent menu
}
