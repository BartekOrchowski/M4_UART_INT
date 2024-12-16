/*
 * main.c
 *
 *  Created on: Sep 20, 2024
 *      Author: Mateusz Salamon
 *		Kurs STM32 Na Rejestrach
 *			https://stm32narejestrach.pl
 *
 *		Blog: https://msalamon.pl
 *		Sklep z elektroniką: https://sklep.msalamon.pl
 *		Kursy Embedded: https://akademiaembedded.pl
 */

#include "main.h"
#include "sys_clocks.h"
#include "tester.h"
#include "led.h"
#include "software_timers.h"
#include "lpuart.h"
#include "stdio.h"

#define BUFFER_SIZE 64

SoftTimer_t TimerLD2;
SoftTimer_t TimerLPUART1;
SoftTimer_t TimerLPUART1Fast;

char ReceivedString[BUFFER_SIZE];
char ToSend[BUFFER_SIZE*2];

void TaskLD2(void);
void TaskLPUART1(void);
void TaskLPUART1Fast(void);

int main(void)
{
	SystemClockSetup();
	TEST1_Init();
	TEST1_Off();

	LED_LD2_Init();

	LPUART1_Init();

	SoftTimerAction(&TimerLD2, TaskLD2);
	SoftTimerStart(&TimerLD2, 500);

	SoftTimerAction(&TimerLPUART1, TaskLPUART1);
	SoftTimerStart(&TimerLPUART1, 2000);

	SoftTimerAction(&TimerLPUART1Fast, TaskLPUART1Fast);
	SoftTimerStart(&TimerLPUART1Fast, 500);


    /* Loop forever */
	while(1)
	{
		SoftTimerEvent(&TimerLD2);
		SoftTimerEvent(&TimerLPUART1);
		SoftTimerEvent(&TimerLPUART1Fast);

		if(LPUART1_IsLineReady())
		{
			LPUART1_GetLine(ReceivedString);

			sprintf(ToSend, "Char Received: %s\r\n", ReceivedString);
			LPUART1_SendString(ToSend);
		}

	}
}

void TaskLD2(void)
{
	LED_LD2_Toggle();
}

void TaskLPUART1(void)
{
	LPUART1_SendString("Test\r\n");
}

void TaskLPUART1Fast(void)
{
	LPUART1_SendString("Fast!\r\n");
}



