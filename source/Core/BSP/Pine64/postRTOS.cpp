#include "BSP.h"
#include "FreeRTOS.h"
#include "I2C_Wrapper.hpp"
#include "QC3.h"
#include "Settings.h"
#include "Si7210.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"

bool hall_effect_present = false;
void postRToSInit() {
  // Any after RTos setup
#ifdef HALL_SI7210
  if (Si7210::detect()) {
    hall_effect_present = Si7210::init();
  }
#endif
}
int16_t getRawHallEffect() {
  if (hall_effect_present) {
    return Si7210::read();
  }
  return 0;
}

bool getHallSensorFitted() { return hall_effect_present; }
