/*
 * BSP_PD.h
 *
 *  Created on: 21 Jul 2020
 *      Author: Ralim
 */

#ifndef USER_BSP_PD_H_
#define USER_BSP_PD_H_
#include "BSP.h"
bool fusb_write_buf(const uint8_t deviceAddr, const uint8_t registerAdd, const uint8_t size, uint8_t *buf);
bool fusb_read_buf(const uint8_t deviceAddr, const uint8_t registerAdd, const uint8_t size, uint8_t *buf);
void setupFUSBIRQ();
bool getFUS302IRQLow();
#endif /* USER_BSP_PD_H_ */
