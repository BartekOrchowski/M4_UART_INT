/*
 * interrupts.c
 *
 *  Created on: Oct 19, 2024
 *      Author: Mateusz Salamon
 *		Kurs STM32 Na Rejestrach
 *			https://stm32narejestrach.pl
 *
 *		Blog: https://msalamon.pl
 *		Sklep z elektroniką: https://sklep.msalamon.pl
 *		Kursy Embedded: https://akademiaembedded.pl
 */
#include "main.h"
#include "lpuart.h"
#include "tester.h"

void LPUART_IRQHandler(void)
{
	LPUART1_RxInterrupt();

	TEST1_On();
	LPUART1_TxInterrupt();
	TEST1_Off();
}
