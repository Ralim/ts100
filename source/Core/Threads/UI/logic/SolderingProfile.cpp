
#include "OperatingModes.h"
#include "SolderingCommon.h"
#include "ui_drawing.hpp"

OperatingMode gui_solderingProfileMode(const ButtonState buttons, guiContext *cxt) {
  /*
   * * Soldering
   * -> Main loop where we draw temp, and animations
   * --> Long hold back button to exit
   * --> Double button to exit
   */

  uint16_t tipTemp = 0;

  // If this is during init, start at preheat
  if (cxt->scratch_state.state1 == 0) {
    cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePreheatTemp);
  }
  uint16_t phaseTicksPerDegree      = TICKS_SECOND / getSettingValue(SettingsOptions::ProfilePreheatSpeed);
  uint16_t profileCurrentTargetTemp = 0;

  switch (buttons) {
  case BUTTON_BOTH:
  case BUTTON_B_LONG:
    cxt->transitionMode = TransitionAnimation::Right;
    return OperatingMode::HomeScreen; // exit on back long hold
  case BUTTON_F_LONG:
  case BUTTON_F_SHORT:
  case BUTTON_B_SHORT:
  case BUTTON_NONE:
    // Not used yet
    break;
  default:
    break;
  }

  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    tipTemp = TipThermoModel::getTipInF();
  } else {
    tipTemp = TipThermoModel::getTipInC();
  }
  // If time of entering is unknown; then we start now
  if (cxt->scratch_state.state3 == 0) {
    cxt->scratch_state.state3 = xTaskGetTickCount();
  }

  // if start temp is unknown (preheat), we're setting it now
  if (cxt->scratch_state.state6 == 0) {
    cxt->scratch_state.state6 = tipTemp;
    // if this is hotter than the preheat temperature, we should fail
    if (cxt->scratch_state.state6 >= cxt->scratch_state.state5) {
      warnUser(translatedString(Tr->TooHotToStartProfileWarning), buttons);
      return OperatingMode::HomeScreen;
    }
  }
  uint16_t phaseElapsedSeconds = (xTaskGetTickCount() - cxt->scratch_state.state3) / TICKS_SECOND;

  // Have we finished this phase?
  // Check if we have hit our temperature target in either direction.
  bool phaseTargetReached = false;
  if (cxt->scratch_state.state6 < cxt->scratch_state.state5 && tipTemp >= cxt->scratch_state.state5) {
    phaseTargetReached = true;
  } else if (cxt->scratch_state.state6 > cxt->scratch_state.state5 && tipTemp <= cxt->scratch_state.state5) {
    phaseTargetReached = true;
  } else if (tipTemp == cxt->scratch_state.state5) {
    phaseTargetReached = true;
  }

  // If we both hit the temperature target and enough time has passed, phase complete.
  if (phaseElapsedSeconds >= cxt->scratch_state.state2 && phaseTargetReached) {
    cxt->scratch_state.state1++;
    cxt->scratch_state.state6 = cxt->scratch_state.state5;
    cxt->scratch_state.state3 = xTaskGetTickCount();
    phaseElapsedSeconds       = 0;
    if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
      // done with all phases, lets go to cooldown
      cxt->scratch_state.state2 = 0;
      cxt->scratch_state.state5 = 0;
      phaseTicksPerDegree       = TICKS_SECOND / getSettingValue(SettingsOptions::ProfileCooldownSpeed);
    } else {
      // set up next phase
      switch (cxt->scratch_state.state1) {
      case 1:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase1Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase1Temp);
        break;
      case 2:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase2Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase2Temp);
        break;
      case 3:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase3Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase3Temp);
        break;
      case 4:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase4Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase4Temp);
        break;
      case 5:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase5Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase5Temp);
        break;
      default:
        break;
      }
      if (cxt->scratch_state.state6 < cxt->scratch_state.state5) {
        phaseTicksPerDegree = (cxt->scratch_state.state2 * TICKS_SECOND) / (cxt->scratch_state.state5 - cxt->scratch_state.state6);
      } else {
        phaseTicksPerDegree = (cxt->scratch_state.state2 * TICKS_SECOND) / (cxt->scratch_state.state6 - cxt->scratch_state.state5);
      }
    }
  }

  // cooldown phase done?
  if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
    if (TipThermoModel::getTipInC() < 55) {
      // we're done, let the buzzer beep too
      setStatusLED(LED_STANDBY);
      if (cxt->scratch_state.state4 == 0) {
        setBuzzer(true);
        cxt->scratch_state.state4 = xTaskGetTickCount() + TICKS_SECOND / 3;
      }
    }
  }

  // determine current target temp
  if (cxt->scratch_state.state6 < cxt->scratch_state.state5) {
    profileCurrentTargetTemp = cxt->scratch_state.state6 + ((xTaskGetTickCount() - cxt->viewEnterTime) / phaseTicksPerDegree);
    if (profileCurrentTargetTemp > cxt->scratch_state.state5) {
      profileCurrentTargetTemp = cxt->scratch_state.state5;
    }
  } else if (cxt->scratch_state.state6 > cxt->scratch_state.state5) {
    profileCurrentTargetTemp = cxt->scratch_state.state6 - ((xTaskGetTickCount() - cxt->viewEnterTime) / phaseTicksPerDegree);
    // Chance of an overflow when ramping up is basically zero, but chance of an underflow here is quite high. If the target underflowed, snap it back.
    if (profileCurrentTargetTemp < cxt->scratch_state.state5 || profileCurrentTargetTemp > cxt->scratch_state.state6) {
      profileCurrentTargetTemp = cxt->scratch_state.state5;
    }
  } else {
    profileCurrentTargetTemp = cxt->scratch_state.state5;
  }

  // Draw in the screen details
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
    ui_draw_soldering_profile_advanced(tipTemp, profileCurrentTargetTemp, phaseElapsedSeconds, cxt->scratch_state.state1, cxt->scratch_state.state2);
    ui_draw_soldering_power_status(false);
  } else {
    ui_draw_soldering_basic_status(false);
  }

  // Update the setpoints for the temperature
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    currentTempTargetDegC = TipThermoModel::convertFtoC(profileCurrentTargetTemp);
  } else {
    currentTempTargetDegC = profileCurrentTargetTemp;
  }

  if (checkExitSoldering() || (cxt->scratch_state.state4 != 0 && xTaskGetTickCount() >= cxt->scratch_state.state4)) {
    setBuzzer(false);
    return OperatingMode::HomeScreen;
  }
  if (heaterThermalRunawayCounter > 8) {
    currentTempTargetDegC       = 0; // heater control off
    heaterThermalRunawayCounter = 0;
    return OperatingMode::ThermalRunaway;
  }

  // Update LED status
  if (cxt->scratch_state.state1 == 0) {
    setStatusLED(LED_HEATING);
  } else if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
    setStatusLED(LED_COOLING_STILL_HOT);
  } else {
    setStatusLED(LED_HOT);
  }
  return OperatingMode::SolderingProfile;
}
