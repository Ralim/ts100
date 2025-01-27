/*
 * Settings.c
 *
 *  Created on: 29 Sep 2016
 *      Author: Ralim
 *
 *      This file holds the users settings and saves / restores them to the
 * devices flash
 */

#include "Settings.h"
#include "BSP.h"
#include "Setup.h"
#include "Translation.h"
#include "configuration.h"
#include <string.h> // for memset
bool sanitiseSettings();

/*
 * Used to constrain the QC 3.0 Voltage selection to suit hardware.
 * We allow a little overvoltage for users who want to push it
 */
#ifdef POW_QC_20V
#define QC_VOLTAGE_MAX 220
#else
#define QC_VOLTAGE_MAX 140
#endif /* POW_QC_20V */

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from
 * flash in uint16_t chunks
 */
typedef struct {
  uint16_t versionMarker;
  uint16_t length; // Length of valid bytes following
  uint16_t settingsValues[SettingsOptionsLength];
  // used to make this nicely "good enough" aligned to 32 bytes to make driver code trivial
  uint32_t padding;

} systemSettingsType;

//~1024 is common programming size, setting threshold to be lower so we have warning
static_assert(sizeof(systemSettingsType) < 512);

// char (*__kaboom)[sizeof(systemSettingsType)] = 1; // Uncomment to print size at compile time
volatile systemSettingsType systemSettings;

// For every setting we need to store the min/max/increment values
typedef struct {
  const uint16_t min;          // Inclusive minimum value
  const uint16_t max;          // Inclusive maximum value
  const uint16_t increment;    // Standard increment
  const uint16_t defaultValue; // Default vaue after reset
} SettingConstants;

static const SettingConstants settingsConstants[(int)SettingsOptions::SettingsOptionsLength] = {
  //{                   min,                                                                   max,         increment,                      default}
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,               SOLDERING_TEMP}, // SolderingTemp
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                          150}, // SleepTemp
    {                     0,                                                                    15,                 1,                   SLEEP_TIME}, // SleepTime
    {                     0,                                                                     4,                 1,              CUT_OUT_SETTING}, // MinDCVoltageCells
    {                    24,                                                                    38,                 1,               RECOM_VOL_CELL}, // MinVoltageCells
    {                    90,                                                        QC_VOLTAGE_MAX,                 2,                           90}, // QCIdealVoltage
    {                     0,                                                  MAX_ORIENTATION_MODE,                 1,             ORIENTATION_MODE}, // OrientationMode
    {                     0,                                                                     9,                 1,                  SENSITIVITY}, // Sensitivity
    {                     0,                                                                     1,                 1,               ANIMATION_LOOP}, // AnimationLoop
    {                     0,                                      settingOffSpeed_t::MAX_VALUE - 1,                 1,              ANIMATION_SPEED}, // AnimationSpeed
    {                     0,                                                                     3,                 1,              AUTO_START_MODE}, // AutoStartMode
    {                     0,                                                                    60,                 1,                SHUTDOWN_TIME}, // ShutdownTime
    {                     0,                                                                     1,                 1,           COOLING_TEMP_BLINK}, // CoolingTempBlink
    {                     0,                                                                     1,                 1,                DETAILED_IDLE}, // DetailedIDLE
    {                     0,                                                                     1,                 1,           DETAILED_SOLDERING}, // DetailedSoldering
    {                     0,                                     (uint16_t)(HasFahrenheit ? 1 : 0),                 1,              TEMPERATURE_INF}, // TemperatureInF
    {                     0,                                                                     1,                 1,     DESCRIPTION_SCROLL_SPEED}, // DescriptionScrollSpeed
    {                     0,                                                                     3,                 1,                 LOCKING_MODE}, // LockingMode
    {                     0,                                                                    99,                 1,          POWER_PULSE_DEFAULT}, // KeepAwakePulse
    {                     1,                                                  POWER_PULSE_WAIT_MAX,                 1,     POWER_PULSE_WAIT_DEFAULT}, // KeepAwakePulseWait
    {                     1,                                              POWER_PULSE_DURATION_MAX,                 1, POWER_PULSE_DURATION_DEFAULT}, // KeepAwakePulseDuration
    {                   360,                                                                   900,                 1,                  VOLTAGE_DIV}, // VoltageDiv
    {                     0,                                                            MAX_TEMP_F,                10,                   BOOST_TEMP}, // BoostTemp
    {MIN_CALIBRATION_OFFSET,                                                                  2500,                 1,           CALIBRATION_OFFSET}, // CalibrationOffset
    {                     0,                                                       MAX_POWER_LIMIT, POWER_LIMIT_STEPS,                  POWER_LIMIT}, // PowerLimit
    {                     0,                                                                     1,                 1,   REVERSE_BUTTON_TEMP_CHANGE}, // ReverseButtonTempChangeEnabled
    {                     5,                                             TEMP_CHANGE_LONG_STEP_MAX,                 5,        TEMP_CHANGE_LONG_STEP}, // TempChangeLongStep
    {                     1,                                            TEMP_CHANGE_SHORT_STEP_MAX,                 1,       TEMP_CHANGE_SHORT_STEP}, // TempChangeShortStep
    {                     0,                                                                     9,                 1,                            7}, // HallEffectSensitivity
    {                     0,                                                                     9,                 1,                            0}, // AccelMissingWarningCounter
    {                     0,                                                                     9,                 1,                            0}, // PDMissingWarningCounter
    {                     0,                                                                0xFFFF,                 0,                 41431 /*EN*/}, // UILanguage
    {                     0,                                                                    50,                 1,                           20}, // PDNegTimeout
    {                     0,                                                                     1,                 1,                            0}, // OLEDInversion
    {        MIN_BRIGHTNESS,                                                        MAX_BRIGHTNESS,   BRIGHTNESS_STEP,           DEFAULT_BRIGHTNESS}, // OLEDBrightness
    {                     0,                                                                     6,                 1,                            1}, // LOGOTime
    {                     0,                                                                     1,                 1,                            0}, // CalibrateCJC
    {                     0,                                                                     1,                 1,                            0}, // BluetoothLE
    {                     0,                                                                     2,                 1,                            1}, // USBPDMode
    {                     1,                                                                     5,                 1,                            4}, // ProfilePhases
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                           90}, // ProfilePreheatTemp
    {                     1,                                                                    10,                 1,                            1}, // ProfilePreheatSpeed
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                          130}, // ProfilePhase1Temp
    {                    10,                                                                   180,                 5,                           90}, // ProfilePhase1Duration
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                          140}, // ProfilePhase2Temp
    {                    10,                                                                   180,                 5,                           30}, // ProfilePhase2Duration
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                          165}, // ProfilePhase3Temp
    {                    10,                                                                   180,                 5,                           30}, // ProfilePhase3Duration
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                          140}, // ProfilePhase4Temp
    {                    10,                                                                   180,                 5,                           30}, // ProfilePhase4Duration
    {            MIN_TEMP_C,                                                            MAX_TEMP_F,                 5,                           90}, // ProfilePhase5Temp
    {                    10,                                                                   180,                 5,                           30}, // ProfilePhase5Duration
    {                     1,                                                                    10,                 1,                            2}, // ProfileCooldownSpeed
    {                     0,                                                                    12,                 1,                            0}, // HallEffectSleepTime
    {                     0, (tipType_t::TIP_TYPE_MAX - 1) > 0 ? (tipType_t::TIP_TYPE_MAX - 1) : 0,                 1,                            0}, // SolderingTipType
};
static_assert((sizeof(settingsConstants) / sizeof(SettingConstants)) == ((int)SettingsOptions::SettingsOptionsLength));

void saveSettings() {
#ifdef CANT_DIRECT_READ_SETTINGS
  // For these devices flash is not 1:1 mapped, so need to read into staging buffer
  systemSettingsType settings;
  flash_read_buffer((uint8_t *)&settings, sizeof(systemSettingsType));
  if (memcmp((void *)&settings, (void *)&systemSettings, sizeof(systemSettingsType))) {
    flash_save_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
  }

#else
  if (memcmp((void *)SETTINGS_START_PAGE, (void *)&systemSettings, sizeof(systemSettingsType))) {
    flash_save_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
  }

#endif /* CANT_DIRECT_READ_SETTINGS */
}

bool loadSettings() {
  // We read the flash
  flash_read_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
  // Then ensure all values are valid
  return sanitiseSettings();
}

bool sanitiseSettings() {
  // For all settings, need to ensure settings are in a valid range
  // First for any not know about due to array growth, reset them and update the length value
  bool dirty = false;
  if (systemSettings.versionMarker != SETTINGSVERSION) {
    memset((void *)&systemSettings, 0xFF, sizeof(systemSettings));
    systemSettings.versionMarker = SETTINGSVERSION;
    dirty                        = true;
  }
  if (systemSettings.padding != 0xFFFFFFFF) {
    systemSettings.padding = 0xFFFFFFFF; // Force padding to 0xFFFFFFFF so that rolling forwards / back should be easier
    dirty                  = true;
  }
  if (systemSettings.length < (int)SettingsOptions::SettingsOptionsLength) {
    dirty = true;
    for (int i = systemSettings.length; i < (int)SettingsOptions::SettingsOptionsLength; i++) {
      systemSettings.settingsValues[i] = 0xFFFF; // Ensure its as if it was erased
    }
    systemSettings.length = (int)SettingsOptions::SettingsOptionsLength;
  }
  for (int i = 0; i < (int)SettingsOptions::SettingsOptionsLength; i++) {
    // Check min max for all settings, if outside the range, move to default
    if (systemSettings.settingsValues[i] < settingsConstants[i].min || systemSettings.settingsValues[i] > settingsConstants[i].max) {
      systemSettings.settingsValues[i] = settingsConstants[i].defaultValue;
      dirty                            = true;
    }
  }
  if (dirty) {
    saveSettings();
  }
  return dirty;
}

void resetSettings() {
  memset((void *)&systemSettings, 0xFF, sizeof(systemSettingsType));
  sanitiseSettings();
  saveSettings(); // Save defaults
}

void setSettingValue(const enum SettingsOptions option, const uint16_t newValue) {
  const auto constants        = settingsConstants[(int)option];
  uint16_t   constrainedValue = newValue;
  if (constrainedValue < constants.min) {
    // If less than min, constrain
    constrainedValue = constants.min;
  } else if (constrainedValue > constants.max) {
    // If hit max, constrain
    constrainedValue = constants.max;
  }
  systemSettings.settingsValues[(int)option] = constrainedValue;
}

// Lookup wrapper for ease of use (with typing)
uint16_t getSettingValue(const enum SettingsOptions option) { return systemSettings.settingsValues[(int)option]; }

// Increment by the step size to the next value. If past the end wrap to the minimum
// Returns true if we are on the _last_ value
void nextSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  if (systemSettings.settingsValues[(int)option] == (constants.max)) {
    // Already at max, wrap to the start
    systemSettings.settingsValues[(int)option] = constants.min;
  } else if (systemSettings.settingsValues[(int)option] >= (constants.max - constants.increment)) {
    // If within one increment of the end, constrain to the end
    systemSettings.settingsValues[(int)option] = constants.max;
  } else {
    // Otherwise increment
    systemSettings.settingsValues[(int)option] += constants.increment;
  }
}

bool isLastSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  uint16_t   max       = constants.max;
  // handle temp unit limitations
  if (option == SettingsOptions::SolderingTemp) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      max = MAX_TEMP_F;
    } else {
      max = MAX_TEMP_C;
    }
  } else if (option == SettingsOptions::BoostTemp) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      max = MAX_TEMP_F;
    } else {
      max = MAX_TEMP_C;
    }
  } else if (option == SettingsOptions::SleepTemp) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      max = 580;
    } else {
      max = 300;
    }
  } else if (option == SettingsOptions::UILanguage) {
    return isLastLanguageOption();
  }
  return systemSettings.settingsValues[(int)option] > (max - constants.increment);
}
// Step backwards on the settings item
// Return true if we are at the end (min)
void prevSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  if (systemSettings.settingsValues[(int)option] == (constants.min)) {
    // Already at min, wrap to the max
    systemSettings.settingsValues[(int)option] = constants.max;
  } else if (systemSettings.settingsValues[(int)option] <= (constants.min + constants.increment)) {
    // If within one increment of the start, constrain to the start
    systemSettings.settingsValues[(int)option] = constants.min;
  } else {
    // Otherwise decrement
    systemSettings.settingsValues[(int)option] -= constants.increment;
  }
}

uint16_t lookupHallEffectThreshold() {
  // Return the threshold above which the hall effect sensor is "activated"
  // We want this to be roughly exponentially mapped from 0-1000
  switch (getSettingValue(SettingsOptions::HallEffectSensitivity)) {
  case 0:
    return 0;
  case 1:
    return 1000;
  case 2:
    return 750;
  case 3:
    return 500;
  case 4:
    return 250;
  case 5:
    return 150;
  case 6:
    return 100;
  case 7:
    return 75;
  case 8:
    return 50;
  case 9:
    return 25;
  default:
    return 0; // Off
  }
}

// Lookup function for cutoff setting -> X10 voltage
/*
 * 0=DC
 * 1=3S
 * 2=4S
 * 3=5S
 * 4=6S
 */
uint8_t lookupVoltageLevel() {
  auto minVoltageOnCell    = getSettingValue(SettingsOptions::MinDCVoltageCells);
  auto minVoltageCellCount = getSettingValue(SettingsOptions::MinVoltageCells);
  if (minVoltageOnCell == 0) {
    return 90; // 9V since iron does not function effectively below this
  } else {
    return (minVoltageOnCell * minVoltageCellCount) + (minVoltageCellCount * 2);
  }
}

#ifdef TIP_TYPE_SUPPORT
const char *lookupTipName() {
  // Get the name string for the current soldering tip
  tipType_t value = (tipType_t)getSettingValue(SettingsOptions::SolderingTipType);

  switch (value) {
#ifdef TIPTYPE_T12
  case tipType_t::T12_8_OHM:
    return translatedString(Tr->TipTypeT12Long);
    break;
  case tipType_t::T12_6_2_OHM:
    return translatedString(Tr->TipTypeT12Short);
    break;
  case tipType_t::T12_4_OHM:
    return translatedString(Tr->TipTypeT12PTS);
    break;
#endif
#ifdef TIPTYPE_TS80
  case tipType_t::TS80_4_5_OHM:
    return translatedString(Tr->TipTypeTS80);
    break;
#endif
#ifdef TIPTYPE_JBC
  case tipType_t::JBC_210_2_5_OHM:
    return translatedString(Tr->TipTypeJBCC210);
    break;
#endif
#ifdef AUTO_TIP_SELECTION
  case tipType_t::TIP_TYPE_AUTO:
#endif
  default:
    return translatedString(Tr->TipTypeAuto);
    break;
  }
}
#endif /* TIP_TYPE_SUPPORT */

// Returns the resistance for the current tip selected by the user or 0 for auto
#ifdef TIP_TYPE_SUPPORT
uint8_t getUserSelectedTipResistance() {
  tipType_t value = (tipType_t)getSettingValue(SettingsOptions::SolderingTipType);

  switch (value) {
#ifdef AUTO_TIP_SELECTION
  case tipType_t::TIP_TYPE_AUTO:
    return 0;
    break;
#endif
#ifdef TIPTYPE_T12
  case tipType_t::T12_8_OHM:
    return 80;
    break;
  case tipType_t::T12_6_2_OHM:
    return 62;
    break;
  case tipType_t::T12_4_OHM:
    return 40;
    break;
#endif
#ifdef TIPTYE_TS80
  case tipType_t::TS80_4_5_OHM:
    return 45;
    break;
#endif
#ifdef TIPTYPE_JBC
  case tipType_t::JBC_210_2_5_OHM:
    return 25;
    break;
#endif
  default:
    return 0;
    break;
  }
}
#else
uint8_t getUserSelectedTipResistance() { return tipType_t::TIP_TYPE_AUTO; }
#endif /* TIP_TYPE_SUPPORT */
