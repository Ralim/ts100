//
// Created by laura on 24.04.23.
//

#include "SolderingCommon.h"
#include "OperatingModes.h"
#include "Types.h"
#include "configuration.h"
#include "history.hpp"
#include "ui_drawing.hpp"

extern uint8_t heaterThermalRunawayCounter;

bool checkExitSoldering(void) {
#ifdef POW_DC
  // Undervoltage test
  if (checkForUnderVoltage()) {
    lastButtonTime = xTaskGetTickCount();
    return true;
  }
#endif

#ifdef ACCEL_EXITS_ON_MOVEMENT
  // If the accel works in reverse where movement will cause exiting the soldering mode
  if (getSettingValue(Sensitivity)) {
    if (lastMovementTime) {
      if (lastMovementTime > TICKS_SECOND * 10) {
        // If we have moved recently; in the last second
        // Then exit soldering mode

        // Movement occurred in last update
        if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) < (TickType_t)(TICKS_SECOND / 5)) {
          currentTempTargetDegC = 0;
          lastMovementTime      = 0;
          return true;
        }
      }
    }
  }
#endif

  // If we have tripped thermal runaway, turn off heater and show warning

  return false;
}

int8_t getPowerSourceNumber(void) {
  int8_t sourceNumber = 0;
  if (getIsPoweredByDCIN()) {
    sourceNumber = 0;
  } else {
    // We are not powered via DC, so want to display the appropriate state for PD or QC
    bool poweredbyPD        = false;
    bool pdHasVBUSConnected = false;
#ifdef POW_PD
    if (USBPowerDelivery::fusbPresent()) {
      // We are PD capable
      if (USBPowerDelivery::negotiationComplete()) {
        // We are powered via PD
        poweredbyPD = true;
#ifdef VBUS_MOD_TEST
        pdHasVBUSConnected = USBPowerDelivery::isVBUSConnected();
#endif
      }
    }
#endif
    if (poweredbyPD) {
      if (pdHasVBUSConnected) {
        sourceNumber = 2;
      } else {
        sourceNumber = 3;
      }
    } else {
      sourceNumber = 1;
    }
  }
  return sourceNumber;
}

// Returns temperature of the tip in *C/*F (based on user settings)
TemperatureType_t getTipTemp(void) {
#ifdef FILTER_DISPLAYED_TIP_TEMP
  static history<TemperatureType_t, FILTER_DISPLAYED_TIP_TEMP> Filter_Temp;
  TemperatureType_t                                            reading = getSettingValue(SettingsOptions::TemperatureInF) ? TipThermoModel::getTipInF() : TipThermoModel::getTipInC();
  Filter_Temp.update(reading);
  return Filter_Temp.average();

#else
  return getSettingValue(SettingsOptions::TemperatureInF) ? TipThermoModel::getTipInF() : TipThermoModel::getTipInC();
#endif
}
