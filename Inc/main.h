/*
 * main.h
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

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include "stm32g4xx.h"
#include "sys_clocks.h"

// NVIC PRIOTY GROUPPING DEFINITIONS
#define NVIC_PRIORITYGROUP_0         0x00000007U /*!< 0 bit  for pre-emption priority,
                                                      4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         0x00000006U /*!< 1 bit  for pre-emption priority,
                                                      3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         0x00000005U /*!< 2 bits for pre-emption priority,
                                                      2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         0x00000004U /*!< 3 bits for pre-emption priority,
                                                      1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         0x00000003U /*!< 4 bits for pre-emption priority,
                                                      0 bit  for subpriority */
// Set what you want
#define NVIC_PRIORITYGROUP_SELECTED	NVIC_PRIORITYGROUP_4

#endif /* MAIN_H_ */
