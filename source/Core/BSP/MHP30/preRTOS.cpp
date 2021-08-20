/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "I2CBB.hpp"
#include "Model_Config.h"
#include "Pins.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>
void preRToSInit() {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();
  Setup_HAL(); // Setup all the HAL objects
  BSPInit();
  I2CBB::init();
  /* Init the IPC objects */
  FRToSI2C::FRToSInit();
}
