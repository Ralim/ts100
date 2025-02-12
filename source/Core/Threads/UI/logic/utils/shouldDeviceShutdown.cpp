#include "OperatingModeUtilities.h"

extern TickType_t lastMovementTime;
extern TickType_t lastHallEffectSleepStart;
#include "Buttons.hpp"

bool shouldShutdown(void) {
  if (getSettingValue(SettingsOptions::ShutdownTime)) { // only allow shutdown exit if time > 0
    if (lastMovementTime) {
      if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) > (TickType_t)(getSettingValue(SettingsOptions::ShutdownTime) * TICKS_MIN)) {
        return true;
      }
    }
    if (lastHallEffectSleepStart) {
      if (((TickType_t)(xTaskGetTickCount() - lastHallEffectSleepStart)) > (TickType_t)(getSettingValue(SettingsOptions::ShutdownTime) * TICKS_MIN)) {
        return true;
      }
    }
  }
#ifdef REVERSE_NAV_EVERYWHERE
  if (getButtonState(getSettingValue(SettingsOptions::ReverseButtonNavEnabled) == BUTTON_B_LONG) { // allow also if back button is pressed long
#else
  if (getButtonState() == BUTTON_B_LONG) { // allow also if back button is pressed long
#endif
    return true;
  }
  return false;
}
