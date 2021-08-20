/*
 * Setup.h
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef PINE_SETUP_H_
#define PINE_SETUP_H_
#include "gd32vf103_libopt.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uint16_t getADC(uint8_t channel);
void     hardware_init();
#ifdef __cplusplus
}
#endif
void                 setupFUSBIRQ();
extern const uint8_t holdoffTicks;
extern const uint8_t tempMeasureTicks;
#endif /* PINE_SETUP_H_ */
