/*
 * TaskUSB.c
 *
 *  Created on: Jun 21, 2021
 *      Author: frank
 */

#include "main.h"
#include <cmsis_os.h>

#include "ff_gen_drv.h"

#include "UserInc/Logging.h"

#undef ACTIVATE

typedef enum {
	Start_c = 0,
	Ready_c,
	Disconnect_c,
} State_t;

static State_t sState = Disconnect_c;

static FATFS USBDISKFatFs;		/* File system object for USB disk logical drive */
static char USBDISKPath[4];		/* USB Host logical drive path */
static FIL MyFile;				/* File object */

/* forward declarations */
static void Start();
static void Ready();
static void Disconnect();

void TaskUSB_ApplicationStart()
{
	// Log(LC_USB_c, "ApplicationStart\r\n");
	sState = Start_c;
}

void TaskUSB_ApplicationReady()
{
	// Log(LC_USB_c, "ApplicationReady\r\n");
	sState = Ready_c;
}

void TaskUSB_ApplicationDisconnect()
{
	// Log(LC_USB_c, "ApplicationDisconnect\r\n");
	sState = Disconnect_c;
}


void TaskUSB_PrepareRTOS()
{
}

void TaskUSB_Run(void * argument)
{
	Log(LC_Console_c, "start TaskUSB...\r\n");
	State_t lastState = sState;

	while (1) {
		if (sState != lastState) {
			switch (sState) {
			case Start_c:
				Start();
				break;
			case Ready_c:
				Ready();
				break;
			case Disconnect_c:
				Disconnect();
				break;
			}
			lastState = sState;
		}
		osDelay(1);
	}
}

static void Start()
{
	Log(LC_USB_c, "start...\r\n");
}

static void Ready()
{
#if defined(ACTIVATE)
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */
#endif
	Log(LC_USB_c, "ready...\r\n");

#if defined(ACTIVATE)
	/* Register the file system object to the FatFs module */
	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
	{
		/* FatFs Initialization Error */
		Error_Handler();
		return;
	}

    /* Create and Open a new text file object with write access */
    if(f_open(&MyFile, "STM32FBE.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
      /* 'STM32.TXT' file Open for write Error */
      Error_Handler();
      return;
    }

    /* Write data to the text file */
    res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

      if((byteswritten == 0) || (res != FR_OK))
      {
        /* 'STM32.TXT' file Write or EOF Error */
        Error_Handler();
      }
      else
      {
        /* Close the open text file */
        f_close(&MyFile);

#if 0
        /* Open the text file object with read access */
        if(f_open(&MyFile, "STM32FBE.TXT", FA_READ) != FR_OK)
        {
          /* 'STM32.TXT' file Open for read Error */
          Error_Handler();
        }
        else
        {
          /* Read data from the text file */
          res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

          if((bytesread == 0) || (res != FR_OK))
          {
            /* 'STM32.TXT' file Read or EOF Error */
            Error_Handler();
          }
          else
          {
            /* Close the open text file */
            f_close(&MyFile);

            /* Compare read data with the expected data */
            if((bytesread != byteswritten))
            {
              /* Read data is different from the expected data */
              Error_Handler();
            }
          }
        }
#endif
      }
#endif
}

static void Disconnect()
{
	Log(LC_USB_c, "disconnect...\r\n");
}
