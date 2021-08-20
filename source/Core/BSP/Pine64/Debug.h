/*
 * Debug.h
 *
 *  Created on: 26 Jan. 2021
 *      Author: Ralim
 */

#ifndef CORE_BSP_PINE64_DEBUG_H_
#define CORE_BSP_PINE64_DEBUG_H_

#include "BSP.h"
#include "TipThermoModel.h"
#include <stdio.h>
#include <string.h>

const unsigned int uartOutputBufferLength = 32;
extern char        uartOutputBuffer[uartOutputBufferLength];
extern "C" {
ssize_t _write(int fd, const void *ptr, size_t len);
void    USART1_IRQHandler(void);
}
#endif /* CORE_BSP_PINE64_DEBUG_H_ */
