/*
 * I2CBB2.cpp
 *
 *  Created on: 12 Jun 2020
 *      Author: Ralim
 */
#include "configuration.h"
#ifdef I2C_SOFT_BUS_2
#include "FreeRTOS.h"
#include <I2CBB2.hpp>
SemaphoreHandle_t I2CBB2::I2CSemaphore = NULL;
StaticSemaphore_t I2CBB2::xSemaphoreBuffer;
void              I2CBB2::init() {
  // Set GPIO's to output open drain
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Pin   = SDA2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(SDA2_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Pin   = SCL2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(SCL2_GPIO_Port, &GPIO_InitStruct);
  SOFT_SDA2_HIGH();
  SOFT_SCL2_HIGH();
  // To ensure bus is unlocked; we toggle the Clock a bunch of times to make things error out
  for (int i = 0; i < 128; i++) {
    SOFT_SCL2_LOW();
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    SOFT_SCL2_HIGH();
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
  I2CSemaphore = xSemaphoreCreateMutexStatic(&xSemaphoreBuffer);
  unlock();
}

bool I2CBB2::probe(uint8_t address) {
  if (!lock()) {
    return false;
  }
  start();
  bool ack = send(address);
  stop();
  unlock();
  return ack;
}

bool I2CBB2::Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return false;
  }
  start();
  bool ack = send(DevAddress);
  if (!ack) {
    stop();
    unlock();
    return false;
  }
  ack = send(MemAddress);
  if (!ack) {
    stop();
    unlock();
    return false;
  }
  SOFT_SCL2_LOW();
  SOFT_I2C_DELAY();
  //	stop();
  start();
  ack = send(DevAddress | 1);
  if (!ack) {
    stop();
    unlock();
    return false;
  }
  while (Size) {
    pData[0] = read(Size > 1);
    pData++;
    Size--;
  }
  stop();
  unlock();
  return true;
}

bool I2CBB2::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, const uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return false;
  }
  start();
  bool ack = send(DevAddress);
  if (!ack) {
    stop();
    unlock();
    return false;
  }
  ack = send(MemAddress);
  if (!ack) {
    stop();
    unlock();
    return false;
  }
  while (Size) {
    resetWatchdog();
    ack = send(pData[0]);
    if (!ack) {
      stop();
      unlock();
      return false;
    }
    pData++;
    Size--;
  }
  stop();
  unlock();
  return true;
}

void I2CBB2::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return;
  }
  start();
  bool ack = send(DevAddress);
  if (!ack) {
    stop();
    unlock();
    return;
  }
  while (Size) {
    ack = send(pData[0]);
    if (!ack) {
      stop();
      unlock();
      return;
    }
    pData++;
    Size--;
  }
  stop();
  unlock();
}

void I2CBB2::Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return;
  }
  start();
  bool ack = send(DevAddress | 1);
  if (!ack) {
    stop();
    unlock();
    return;
  }
  while (Size) {
    pData[0] = read(Size > 1);
    pData++;
    Size--;
  }
  stop();
  unlock();
}

void I2CBB2::TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx) {
  if (Size_tx == 0 && Size_rx == 0) {
    return;
  }
  if (lock() == false) {
    return;
  }
  if (Size_tx) {
    start();
    bool ack = send(DevAddress);
    if (!ack) {
      stop();
      unlock();
      return;
    }
    while (Size_tx) {
      ack = send(pData_tx[0]);
      if (!ack) {
        stop();
        unlock();
        return;
      }
      pData_tx++;
      Size_tx--;
    }
  }
  if (Size_rx) {
    start();
    bool ack = send(DevAddress | 1);
    if (!ack) {
      stop();
      unlock();
      return;
    }
    while (Size_rx) {
      pData_rx[0] = read(Size_rx > 1);
      pData_rx++;
      Size_rx--;
    }
  }
  stop();
  unlock();
}

void I2CBB2::start() {
  /* I2C Start condition, data line goes low when clock is high */
  SOFT_SCL2_HIGH();
  SOFT_SDA2_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SDA2_LOW();
  SOFT_I2C_DELAY();
  SOFT_SCL2_LOW();
  SOFT_I2C_DELAY();
  SOFT_SDA2_HIGH();
}

void I2CBB2::stop() {
  /* I2C Stop condition, clock goes high when data is low */
  SOFT_SDA2_LOW();
  SOFT_I2C_DELAY();
  SOFT_SCL2_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SDA2_HIGH();
  SOFT_I2C_DELAY();
}

bool I2CBB2::send(uint8_t value) {

  for (uint8_t i = 0; i < 8; i++) {
    write_bit(value & 0x80); // write the most-significant bit
    value <<= 1;
  }

  SOFT_SDA2_HIGH();
  bool ack = (read_bit() == 0);
  return ack;
}

uint8_t I2CBB2::read(bool ack) {
  uint8_t B = 0;

  uint8_t i;
  for (i = 0; i < 8; i++) {
    B <<= 1;
    B |= read_bit();
  }

  SOFT_SDA2_HIGH();
  if (ack) {
    write_bit(0);
  } else {
    write_bit(1);
  }
  return B;
}

uint8_t I2CBB2::read_bit() {
  uint8_t b;

  SOFT_SDA2_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SCL2_HIGH();
  SOFT_I2C_DELAY();

  if (SOFT_SDA2_READ()) {
    b = 1;
  } else {
    b = 0;
  }

  SOFT_SCL2_LOW();
  return b;
}

void I2CBB2::unlock() { xSemaphoreGive(I2CSemaphore); }

bool I2CBB2::lock() {
  if (I2CSemaphore == NULL) {
  }
  bool a = xSemaphoreTake(I2CSemaphore, (TickType_t)100) == pdTRUE;
  return a;
}

bool I2CBB2::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) { return Mem_Write(address, reg, &data, 1); }

uint8_t I2CBB2::I2C_RegisterRead(uint8_t address, uint8_t reg) {
  uint8_t temp = 0;
  Mem_Read(address, reg, &temp, 1);
  return temp;
}

void I2CBB2::write_bit(uint8_t val) {
  if (val) {
    SOFT_SDA2_HIGH();
  } else {
    SOFT_SDA2_LOW();
  }

  SOFT_I2C_DELAY();
  SOFT_SCL2_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SCL2_LOW();
}

bool I2CBB2::writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength) {
  for (int index = 0; index < registersLength; index++) {
    if (!I2C_RegisterWrite(address, registers[index].reg, registers[index].val)) {
      return false;
    }
    if (registers[index].pause_ms) {
      delay_ms(registers[index].pause_ms);
    }
  }
  return true;
}

bool I2CBB2::wakePart(uint16_t DevAddress) {
  // wakepart is a special case  where only the device address is sent
  if (!lock()) {
    return false;
  }
  start();
  bool ack = send(DevAddress);
  stop();
  unlock();
  return ack;
}
#endif
