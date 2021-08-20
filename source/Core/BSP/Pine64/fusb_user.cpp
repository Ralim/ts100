#include "Model_Config.h"
#ifdef POW_PD
#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Setup.h"

/*
 * Read multiple bytes from the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address from which to read
 * size: The number of bytes to read
 * buf: The buffer into which data will be read
 */
bool fusb_read_buf(const uint8_t deviceAddr, const uint8_t registerAdd, const uint8_t size, uint8_t *buf) { return FRToSI2C::Mem_Read(deviceAddr, registerAdd, buf, size); }

/*
 * Write multiple bytes to the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address to which we will write
 * size: The number of bytes to write
 * buf: The buffer to write
 */
bool fusb_write_buf(const uint8_t deviceAddr, const uint8_t registerAdd, const uint8_t size, uint8_t *buf) { return FRToSI2C::Mem_Write(deviceAddr, registerAdd, (uint8_t *)buf, size); }

#endif
