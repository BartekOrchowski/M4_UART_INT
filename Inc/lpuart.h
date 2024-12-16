/*
 * lpuart.h
 *
 *  Created on: Oct 15, 2024
 *      Author: Mateusz Salamon
 *		Kurs STM32 Na Rejestrach
 *			https://stm32narejestrach.pl
 *
 *		Blog: https://msalamon.pl
 *		Sklep z elektroniką: https://sklep.msalamon.pl
 *		Kursy Embedded: https://akademiaembedded.pl
 */
#ifndef LPUART_H_
#define LPUART_H_

#define EOF_CHAR '\n'

void LPUART1_Init(void);

uint8_t LPUART1_IsLineReady(void);
uint8_t LPUART1_GetLine(char *Line);

void LPUART1_SendString(char *str);

// Interrupts handlers
void LPUART1_RxInterrupt(void);
void LPUART1_TxInterrupt(void);

#endif /* LPUART_H_ */
