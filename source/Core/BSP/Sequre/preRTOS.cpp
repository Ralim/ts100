/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "I2CBB1.hpp"
#include "I2CBB2.hpp"
#include "Pins.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>
#if defined(I2C_PROBE_POW_PD) && POW_PD_EXT == 2
#include "FS2711.hpp"
#endif

void preRToSInit() {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();
  Setup_HAL(); // Setup all the HAL objects
  BSPInit();
#ifdef I2C_SOFT_BUS_1
  I2CBB1::init();
#endif /* I2C_SOFT_BUS_1 */
#ifdef I2C_SOFT_BUS_2
  I2CBB2::init();
#endif /* I2C_SOFT_BUS_2 */
#if defined(I2C_PROBE_POW_PD) && POW_PD_EXT == 2
  // Detect and set FS2711 I2C bus num
  FS2711::detect_i2c_bus_num();
#endif /* defined(I2C_PROBE_POW_PD) && POW_PD_EXT == 2 */
}
