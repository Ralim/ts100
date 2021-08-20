/*
 * Debug.cpp
 *
 *  Created on: 26 Jan. 2021
 *      Author: Ralim
 */
#include "Debug.h"
#include "FreeRTOS.h"
#include "Pins.h"

extern "C" {

#include "gd32vf103_usart.h"
}
char                    uartOutputBuffer[uartOutputBufferLength];
volatile uint32_t       currentOutputPos = 0xFF;
volatile uint32_t       outputLength     = 0;
extern volatile uint8_t pendingPWM;
void                    log_system_state(int32_t PWMWattsx10) {
  if (currentOutputPos == 0xFF) {

    // Want to print a CSV log out the uart
    // Tip_Temp_C,Handle_Temp_C,Output_Power_Wattx10,PWM,Tip_Raw\r\n
    // 3+1+3+1+3+1+3+1+5+2 = 23, so sizing at 32 for now

    outputLength = snprintf(uartOutputBuffer, uartOutputBufferLength, "%lu,%u,%li,%u,%lu\r\n", //
                            TipThermoModel::getTipInC(false),                                  // Tip temp in C
                            getHandleTemperature(),                                            // Handle temp in C X10
                            PWMWattsx10,                                                       // Output Wattage
                            pendingPWM,                                                        // PWM
                            TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true)       // Tip temp in uV
    );

    // Now print this out the uart via IRQ (DMA cant be used as oled has it)
    currentOutputPos = 0;
    /* enable USART1 Transmit Buffer Empty interrupt */
    usart_interrupt_enable(UART_PERIF, USART_INT_TBE);
  }
}
ssize_t _write(int fd, const void *ptr, size_t len) {
  if (len > uartOutputBufferLength) {
    len = uartOutputBufferLength;
  }
  outputLength     = len;
  currentOutputPos = 0;
  memcpy(uartOutputBuffer, ptr, len);
  /* enable USART1 Transmit Buffer Empty interrupt */
  usart_interrupt_enable(UART_PERIF, USART_INT_TBE);
  delay_ms(1);
  return len;
}
void USART1_IRQHandler(void) {
  if (RESET != usart_interrupt_flag_get(UART_PERIF, USART_INT_FLAG_TBE)) {
    /* write one byte to the transmit data register */
    usart_data_transmit(UART_PERIF, uartOutputBuffer[currentOutputPos++]);
    if (currentOutputPos >= outputLength) {
      currentOutputPos = 0xFF; // Mark done
      usart_interrupt_disable(UART_PERIF, USART_INT_TBE);
    }
  }
}
