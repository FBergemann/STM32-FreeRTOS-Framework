/*
 * Menu.c
 *
 *  Created on: Aug 7, 2021
 *      Author: frank
 */

#include "UserInc/Logging.h"
#include "UserInc/Menu.h"

void MenuStart()
{
	Log(LC_Console_c, "****\r\n");
	Log(LC_Console_c, "Menu\r\n");
	Log(LC_Console_c, "****\r\n");
	Log(LC_Console_c, "\r\n");
    Log(LC_Console_c, "x|X: quit menu \r\n");
}

bool MenuHandle(uint8_t rcvByte)
{
	bool quitMenu = false;
	switch(rcvByte) {
	case 'x':
	case 'X':
		quitMenu = true;
		break;
	}

	return quitMenu;
}
