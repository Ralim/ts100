/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "BSP.h"
#include "IRQ.h"
#include "Setup.h"
extern "C" {
#include "bflb_platform.h"
#include "bl702_glb.h"
#include "bl702_i2c.h"
}
#include <I2C_Wrapper.hpp>

SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;
#define I2C_TIME_OUT (uint16_t)(12000)
void FRToSI2C::CpltCallback() {} // Not used

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) { return Mem_Write(address, reg, &data, 1); }

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
  uint8_t temp = 0;
  Mem_Read(add, reg, &temp, 1);
  return temp;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t read_address, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock()) {
    return false;
  }
  I2C_Transfer_Cfg i2cCfg = {0, DISABLE, 0, 0, 0, 0};
  BL_Err_Type      err    = ERROR;
  i2cCfg.slaveAddr        = DevAddress >> 1;
  i2cCfg.stopEveryByte    = DISABLE;
  i2cCfg.subAddr          = read_address;
  i2cCfg.dataSize         = number_of_byte;
  i2cCfg.data             = p_buffer;
  i2cCfg.subAddrSize      = 1; // one byte address

  vTaskSuspendAll();
  /* --------------- */
  err = I2C_MasterReceiveBlocking(I2C0_ID, &i2cCfg);
  xTaskResumeAll();
  bool res = err == SUCCESS;
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock()) {
    return false;
  }
  I2C_Transfer_Cfg i2cCfg = {0, DISABLE, 0, 0, 0, 0};
  BL_Err_Type      err    = ERROR;
  i2cCfg.slaveAddr        = DevAddress >> 1;
  i2cCfg.stopEveryByte    = DISABLE;
  i2cCfg.subAddr          = MemAddress;
  i2cCfg.dataSize         = number_of_byte;
  i2cCfg.data             = p_buffer;
  i2cCfg.subAddrSize      = 1; // one byte address

  vTaskSuspendAll();
  /* --------------- */
  err = I2C_MasterSendBlocking(I2C0_ID, &i2cCfg);
  xTaskResumeAll();
  bool res = err == SUCCESS;
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}

bool FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) { return Mem_Write(DevAddress, pData[0], pData + 1, Size - 1); }

bool FRToSI2C::probe(uint16_t DevAddress) {
  uint8_t temp[1];
  return Mem_Read(DevAddress, 0x00, temp, sizeof(temp));
}

void FRToSI2C::I2C_Unstick() { unstick_I2C(); }

bool FRToSI2C::lock() {
  if (I2CSemaphore == nullptr) {
    return false;
  }
  return xSemaphoreTake(I2CSemaphore, TICKS_SECOND) == pdTRUE;
}

void FRToSI2C::unlock() { xSemaphoreGive(I2CSemaphore); }

bool FRToSI2C::writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength) {
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

bool FRToSI2C::wakePart(uint16_t DevAddress) {
  // wakepart is a special case  where only the device address is sent

  if (!lock()) {
    return false;
  }
  uint8_t          temp[1] = {0};
  I2C_Transfer_Cfg i2cCfg  = {0, DISABLE, 0, 0, 0, 0};
  BL_Err_Type      err     = ERROR;
  i2cCfg.slaveAddr         = DevAddress >> 1;
  i2cCfg.stopEveryByte     = DISABLE;
  i2cCfg.subAddr           = 0;
  i2cCfg.dataSize          = 1;
  i2cCfg.data              = temp;
  i2cCfg.subAddrSize       = 0;

  err      = I2C_MasterReceiveBlocking(I2C0_ID, &i2cCfg);
  bool res = err == SUCCESS;
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}
