/*
 * lpuart.c
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
#include "main.h"
#include "lpuart.h"
#include "ring_buffer.h"

static RingBuffer RxBuffer;
static RingBuffer TxBuffer;
volatile uint8_t LineReadyFlag;

void LPUART1_GPIO_Init(void)
{
	// TX - PA2
	// RX - PA3

	// Enable clock for GPIO pins
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

/*
	// TX - PA2
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_0);
	// AF12
	GPIOA->AFR[0] |= (12 << GPIO_AFRL_AFSEL2_Pos);

	// RX - PA3
	GPIOA->MODER &= ~(GPIO_MODER_MODE3_0);
	GPIOA->AFR[0] |= (12 << GPIO_AFRL_AFSEL3_Pos);
	GPIOA->OTYPER |= GPIO_OTYPER_OT3;
*/
	// TX and RX once
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0); // AF Mode
	GPIOA->AFR[0] |= ((12 << GPIO_AFRL_AFSEL2_Pos) | (12 << GPIO_AFRL_AFSEL3_Pos)); // Select AF number
	GPIOA->OTYPER |= GPIO_OTYPER_OT3; // Set push-pull output type for TX pin
}

void LPUART1_InterruptsConfig(void)
{
	LPUART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE; // Enable Receive interrupt

	// Set NVIC for UART Interrupt
	NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUP_SELECTED, 0, 0)); // Highest priority - 0,0
	NVIC_ClearPendingIRQ(LPUART1_IRQn); // Clear pending to avoid misshandling
	NVIC_EnableIRQ(LPUART1_IRQn);
}

void LPUART1_Config(void)
{
	// Enable UART Clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;

	// BRR = 256 * (48000000 / 115200) = 106666,6666666667

	// Desired baudrate: 115200
	// ROUND DOWN
	// LPUARTDIV = 256 * (48000000 / 115200) = 106666
	// Calculated baudrate: (256*48M)/106666 = 115200,7200045
	// Error = 100 - ((11520,07200045 / 115200)*100) = 0,00062500390625 %

	// ROUND UP
	// LPUARTDIV = 256 * (48000000 / 115200) = 106667
	// Calculated baudrate: (256*48M)/106666 = 115199,640001125
	// Error = 100 - ((115199,640001125 / 115200)*100) = 0,0003124990234375 %

	uint32_t BRR_Value = 106666; // 256 * (48000000 / 115200);
	if(BRR_Value < 0x300 || BRR_Value > 0xFFFFF) while(1);  // Check constraints

	LPUART1->BRR = BRR_Value & USART_BRR_LPUART; // Set calculated baudrate

	//	LPUART1->CR1 |= USART_CR1_UE; // Enable UART
	//	LPUART1->CR1 |= USART_CR1_TE; // Enable UART Transmitter
	//	LPUART1->CR1 |= USART_CR1_RE; // Enable UART Receiver

	LPUART1_InterruptsConfig(); // Config interrupts

	LPUART1->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE); // Enable all in one time
}

void LPUART1_Init(void)
{
	LPUART1_GPIO_Init();
	LPUART1_Config();
}

void LPUART1_SendChar(char ch)
{
	RB_Write(&TxBuffer, ch); // Put char to round-buffer
}

void LPUART1_SendString(char *str)
{
	while(*str != '\0') // If there is something to send
	{
		LPUART1_SendChar(*str); // Send char
		str++; // Next char
	}

	// Enable TXE interrupt - transmit will start immediately
	LPUART1->CR1 |=	USART_CR1_TXEIE_TXFNFIE;
}

void LPUART1_GetNextCharToSend(void)
{
	uint8_t ToTransmit;

	// Try to read from TX Buffer
	if(RB_OK == RB_Read(&TxBuffer, &ToTransmit))
	{
		// If there is something to send - put into UART TX buffer
		LPUART1->TDR = ToTransmit;
	}
	else
	{
		// If nothing to send - disable TX interrupt
		LPUART1->CR1 &= ~(USART_CR1_TXEIE_TXFNFIE);
	}
}

void LPUART1_TxInterrupt(void)
{
	// Check if TX Interrupt occured
	if(LPUART1->ISR & USART_ISR_TXE_TXFNF)
	{
		// Take next char to send from round-buffer
		LPUART1_GetNextCharToSend();
	}
}

void LPUART1_RxInterrupt(void)
{
	// Check if RX Interrupt occured
	if(LPUART1->ISR & USART_ISR_RXNE_RXFNE)
	{
		uint8_t Recevied = LPUART1->RDR; // Receive char from UART RX buffer
		RB_Write(&RxBuffer, Recevied); // Store into RX round-buffer

		// Check if we got end of frame sign
		if(Recevied == EOF_CHAR)
		{
			LineReadyFlag++; // Increase ready to parse line counter
		}
	}
}

uint8_t LPUART1_IsLineReady(void)
{
	return LineReadyFlag; // Return line counter
}

uint8_t LPUART1_GetLine(char *Line)
{
	uint8_t i;
	Line[0] = '\0'; // For safety

	// For whole RX round-buffer
	for(i = 0; i < RING_BUFFER_SIZE; i++)
	{
		// Try to read from RX round-buffer to line pointer
		if(RB_OK != RB_Read(&RxBuffer, (uint8_t*)&(Line[i])))
		{
			// return error if nothing more to read
			return 1;
		}

		// Check if end of frame sign is met
		if(Line[i] == EOF_CHAR)
		{
			Line[i] = '\0'; // change EOF to end of c-string sign
			LineReadyFlag--; // Decrease line counter
			return 0; // no error
		}
	}
	return 1; // unspecified error - should never get there.
}
