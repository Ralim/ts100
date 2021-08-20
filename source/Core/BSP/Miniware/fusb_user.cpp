#include "Model_Config.h"
#ifdef POW_PD
#include "BSP.h"
#include "I2CBB.hpp"
#include "Setup.h"

bool fusb_read_buf(const uint8_t deviceAddr, const uint8_t registerAdd, const uint8_t size, uint8_t *buf) { return I2CBB::Mem_Read(deviceAddr, registerAdd, buf, size); }

bool fusb_write_buf(const uint8_t deviceAddr, const uint8_t registerAdd, const uint8_t size, uint8_t *buf) { return I2CBB::Mem_Write(deviceAddr, registerAdd, buf, size); }

void setupFUSBIRQ() {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin   = GPIO_PIN_9;
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
#endif
