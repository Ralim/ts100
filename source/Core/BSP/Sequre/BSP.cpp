// BSP mapping functions

#include "BSP.h"
#include "BootLogo.h"
#include "FS2711.hpp"
#include "HUB238.hpp"
#include "I2C_Wrapper.hpp"
#include "Pins.h"
#include "Settings.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include <IRQ.h>

volatile uint16_t    PWMSafetyTimer   = 0;
volatile uint8_t     pendingPWM       = 0;
const uint16_t       powerPWM         = 255;
static const uint8_t holdoffTicks     = 15; // delay of 8 ish ms
static const uint8_t tempMeasureTicks = 15;

uint16_t totalPWM = powerPWM + tempMeasureTicks + holdoffTicks; // htim2.Init.Period, the full PWM cycle

void resetWatchdog() { HAL_IWDG_Refresh(&hiwdg); }
// Lookup table for the NTC
// We dont know exact specs, but it loooks to be roughly a 10K B=4000 NTC
// Stored as ADCReading,Temp in degC
static const uint16_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C
    23931, 0,  //
    23210, 2,  //
    22466, 4,  //
    21703, 6,  //
    20924, 8,  //
    20135, 10, //
    19338, 12, //
    18538, 14, //
    17738, 16, //
    16943, 18, //
    16156, 20, //
    15381, 22, //
    14621, 24, //
    13878, 26, //
    13155, 28, //
    12455, 30, //
    11778, 32, //
    11126, 34, //
    10501, 36, //
    9902,  38, //
    9330,  40, //
    8786,  42, //
    8269,  44, //
};

uint16_t getHandleTemperature(uint8_t sample) {
#ifdef TMP36_ADC1_CHANNEL
  int32_t result = getADCHandleTemp(sample);
  // S60 uses 10k NTC resistor
  // For now not doing interpolation
  for (uint32_t i = 0; i < (sizeof(NTCHandleLookup) / (2 * sizeof(uint16_t))); i++) {
    if (result > NTCHandleLookup[(i * 2) + 0]) {
      return NTCHandleLookup[(i * 2) + 1] * 10;
    }
  }
  return 45 * 10;
#else
  return 0; // Not implemented
#endif
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  // ADC maximum is 32767 == 3.3V at input == 28.05V at VIN
  // Therefore we can divide down from there
  // Multiplying ADC max by 4 for additional calibration options,
  // ideal term is 467
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

static void switchToFastPWM(void) {
  // 20Hz
  totalPWM             = powerPWM + tempMeasureTicks + holdoffTicks;
  htim2.Instance->ARR  = totalPWM;
  htim2.Instance->CCR1 = powerPWM + holdoffTicks;
  htim2.Instance->CCR4 = powerPWM;
  htim2.Instance->PSC  = 1500;
}

void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  PWMSafetyTimer = 20; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.
  pendingPWM = pulse;
}
// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Period has elapsed
  if (htim->Instance == TIM2) {
    // we want to turn on the output again
    PWMSafetyTimer--;
    // We decrement this safety value so that lockups in the
    // scheduler will not cause the PWM to become locked in an
    // active driving state.
    // While we could assume this could never happen, its a small price for
    // increased safety
    if (PWMSafetyTimer == 0) {
      htim4.Instance->CCR3 = 0;
    } else {
      htim4.Instance->CCR3 = pendingPWM / 4;
    }
  } else if (htim->Instance == TIM1) {
    // STM uses this for internal functions as a counter for timeouts
    HAL_IncTick();
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // This was a when the PWM for the output has timed out
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
    // HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    htim4.Instance->CCR3 = 0;
  }
}

void unstick_I2C() {}

uint8_t getButtonA() { return HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET ? 1 : 0; }
uint8_t getButtonB() { return HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin) == GPIO_PIN_RESET ? 1 : 0; }

void BSPInit(void) { switchToFastPWM(); }

void reboot() { NVIC_SystemReset(); }

void delay_ms(uint16_t count) { HAL_Delay(count); }

bool isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void    setStatusLED(const enum StatusLED state) {}
uint8_t preStartChecks() {
#if POW_PD_EXT == 1
  if (!hub238_has_run_selection() && (xTaskGetTickCount() < TICKS_SECOND * 5)) {
    return 0;
  }
  // We check if we are in a "Limited" mode; where we have to run the PWM really fast
  // Where as if we are on 9V for example, the tip resistance is enough
  uint16_t voltage     = hub238_source_voltage();
  uint16_t currentx100 = hub238_source_currentX100();
#endif
#if POW_PD_EXT == 2
  if (!FS2711::has_run_selection() && (xTaskGetTickCount() < TICKS_SECOND * 5)) {
    return 0;
  }
  uint16_t voltage     = FS2711::source_voltage();
  uint16_t currentx100 = FS2711::source_currentx100();
#endif

  uint16_t thresholdResistancex10 = ((voltage * 1000) / currentx100) + 5;

  if (getTipResistanceX10() <= thresholdResistancex10) {
    // We are limited by resistance, not our current limiting, we can slow down PWM to avoid audible noise
    htim4.Instance->PSC = 50; // 10 -> 500 removes audible noise
  }

  return 1; // We are done now
}
uint64_t getDeviceID() {
  //
  return HAL_GetUIDw0() | ((uint64_t)HAL_GetUIDw1() << 32);
}

uint8_t getTipResistanceX10() {
#ifdef COPPER_HEATER_COIL

  // TODO
  //! Warning, must never return 0.
  TemperatureType_t measuredTemperature = TipThermoModel::getTipInC(false);
  if (measuredTemperature < 25) {
    return 50; // Start assuming under spec to soft-start
  }

  // Assuming a temperature rise of 0.00393 per deg c over 20C

  uint32_t scaler = 393 * (measuredTemperature - 20);

  return TIP_RESISTANCE + ((TIP_RESISTANCE * scaler) / 100000);
#else
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return TIP_RESISTANCE; // Auto mode
  }
  return user_selected_tip;
#endif
}
bool    isTipShorted() { return false; }
uint8_t preStartChecksDone() { return 1; }

uint16_t getTipThermalMass() { return TIP_THERMAL_MASS; }
uint16_t getTipInertia() { return TIP_THERMAL_INERTIA; }

void setBuzzer(bool on) {}

void showBootLogo(void) { BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); }

#ifdef CUSTOM_MAX_TEMP_C
TemperatureType_t getCustomTipMaxInC() { return MAX_TEMP_C; }
#endif
