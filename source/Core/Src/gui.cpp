/*
 * gui.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "gui.hpp"
#include "Buttons.hpp"
#include "ScrollMessage.hpp"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "main.hpp"

void gui_Menu(const menuitem *menu);

#ifdef POW_DC
static bool settings_setInputVRange(void);
static bool settings_displayInputVRange(void);
static bool settings_setInputMinVRange(void);
static bool settings_displayInputMinVRange(void);
#endif
#ifdef POW_QC
static bool settings_setQCInputV(void);
static bool settings_displayQCInputV(void);
#endif
#ifdef POW_PD
static bool settings_setPDNegTimeout(void);
static bool settings_displayPDNegTimeout(void);
#endif
#ifndef NO_SLEEP_MODE
static bool settings_setSleepTemp(void);
static bool settings_displaySleepTemp(void);
static bool settings_setSleepTime(void);
static bool settings_displaySleepTime(void);
#endif
static bool settings_setShutdownTime(void);
static bool settings_displayShutdownTime(void);
static bool settings_setSensitivity(void);
static bool settings_displaySensitivity(void);
static bool settings_setTempF(void);
static bool settings_displayTempF(void);
static bool settings_setAdvancedSolderingScreens(void);
static bool settings_displayAdvancedSolderingScreens(void);
static bool settings_setAdvancedIDLEScreens(void);
static bool settings_displayAdvancedIDLEScreens(void);
static bool settings_setScrollSpeed(void);
static bool settings_displayScrollSpeed(void);
static bool settings_setPowerLimit(void);
static bool settings_displayPowerLimit(void);
#ifndef NO_DISPLAY_ROTATE
static bool settings_setDisplayRotation(void);
static bool settings_displayDisplayRotation(void);
#endif
static bool settings_setBoostTemp(void);
static bool settings_displayBoostTemp(void);
static bool settings_setAutomaticStartMode(void);
static bool settings_displayAutomaticStartMode(void);
static bool settings_setLockingMode(void);
static bool settings_displayLockingMode(void);
static bool settings_setCoolingBlinkEnabled(void);
static bool settings_displayCoolingBlinkEnabled(void);
static bool settings_setResetSettings(void);
static bool settings_displayResetSettings(void);
static bool settings_setCalibrate(void);
static bool settings_displayCalibrate(void);
static bool settings_setCalibrateVIN(void);
static bool settings_displayCalibrateVIN(void);
static bool settings_displayReverseButtonTempChangeEnabled(void);
static bool settings_setReverseButtonTempChangeEnabled(void);
static bool settings_displayTempChangeShortStep(void);
static bool settings_setTempChangeShortStep(void);
static bool settings_displayTempChangeLongStep(void);
static bool settings_setTempChangeLongStep(void);
static bool settings_displayPowerPulse(void);
static bool settings_setPowerPulse(void);
static bool settings_displayAnimationSpeed(void);
static bool settings_setAnimationSpeed(void);
static bool settings_displayAnimationLoop(void);
static bool settings_setAnimationLoop(void);
static bool settings_displayPowerPulseWait(void);
static bool settings_setPowerPulseWait(void);
static bool settings_displayPowerPulseDuration(void);
static bool settings_setPowerPulseDuration(void);
static bool settings_displayBrightnessLevel(void);
static bool settings_setBrightnessLevel(void);
static bool settings_displayInvertColor(void);
static bool settings_setInvertColor(void);
#ifdef HALL_SENSOR
static bool settings_displayHallEffect(void);
static bool settings_setHallEffect(void);
#endif
// Menu functions

#if defined(POW_DC) || defined(POW_QC)
static bool settings_displayPowerMenu(void);
static bool settings_enterPowerMenu(void);
#endif
static bool settings_displaySolderingMenu(void);
static bool settings_enterSolderingMenu(void);
static bool settings_displayPowerSavingMenu(void);
static bool settings_enterPowerSavingMenu(void);
static bool settings_displayUIMenu(void);
static bool settings_enterUIMenu(void);
static bool settings_displayAdvancedMenu(void);
static bool settings_enterAdvancedMenu(void);
/*
 * Root Settings Menu
 *
 * Power Menu
 *  Power Source
 *
 * Soldering
 * 	Boost Mode Enabled
 * 	Boost Mode Temp
 * 	Auto Start
 *  Temp change short step
 *  Temp change long step
 *	Locking Mode

 *
 * Power Saving
 * 	Sleep Temp
 * 	Sleep Time
 * 	Shutdown Time
 * 	Motion Sensitivity
 *
 * UI
 *  // Language
 *  Scrolling Speed
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 *  Reverse Temp change buttons + -
 *
 * Advanced
 *  Enable Power Limit
 *  Power Limit
 *  Detailed IDLE
 *  Detailed Soldering
 *  Logo Time
 *  Calibrate Temperature
 *  Calibrate Input V
 *  Reset Settings
 *
 */
const menuitem rootSettingsMenu[] {
  /*
   * Power Menu
   * Soldering Menu
   * Power Saving Menu
   * UI Menu
   * Advanced Menu
   * Exit
   */

#if defined(POW_DC) || defined(POW_QC)
  {0, settings_enterPowerMenu, settings_displayPowerMenu}, /*Power*/
#endif
      {0, settings_enterSolderingMenu, settings_displaySolderingMenu},     /*Soldering*/
      {0, settings_enterPowerSavingMenu, settings_displayPowerSavingMenu}, /*Sleep Options Menu*/
      {0, settings_enterUIMenu, settings_displayUIMenu},                   /*UI Menu*/
      {0, settings_enterAdvancedMenu, settings_displayAdvancedMenu},       /*Advanced Menu*/
      {0, settings_setLanguageSwitch, settings_displayLanguageSwitch},     /*Language Switch*/
  {
    0, nullptr, nullptr
  } // end of menu marker. DO NOT REMOVE
};

#if defined(POW_DC) || defined(POW_QC) || defined(POW_PD)
const menuitem powerMenu[] = {
/*
 * Power Source
 */
#ifdef POW_DC
    {SETTINGS_DESC(SettingsItemIndex::DCInCutoff), settings_setInputVRange, settings_displayInputVRange},       /*Voltage input*/
    {SETTINGS_DESC(SettingsItemIndex::MinVolCell), settings_setInputMinVRange, settings_displayInputMinVRange}, /*Minimum voltage input*/
#endif
#ifdef POW_QC
    {SETTINGS_DESC(SettingsItemIndex::QCMaxVoltage), settings_setQCInputV, settings_displayQCInputV}, /*Voltage input*/
#endif
#ifdef POW_PD
    {SETTINGS_DESC(SettingsItemIndex::PDNegTimeout), settings_setPDNegTimeout, settings_displayPDNegTimeout}, /*PD timeout setup*/
#endif
    {0, nullptr, nullptr} // end of menu marker. DO NOT REMOVE
};
#endif
const menuitem solderingMenu[] = {
    /*
     * Boost Mode Enabled
     * 	Boost Mode Temp
     * 	Auto Start
     *  Temp change short step
     *  Temp change long step
     */
    {SETTINGS_DESC(SettingsItemIndex::BoostTemperature), settings_setBoostTemp, settings_displayBoostTemp},                        /*Boost Temp*/
    {SETTINGS_DESC(SettingsItemIndex::AutoStart), settings_setAutomaticStartMode, settings_displayAutomaticStartMode},             /*Auto start*/
    {SETTINGS_DESC(SettingsItemIndex::TempChangeShortStep), settings_setTempChangeShortStep, settings_displayTempChangeShortStep}, /*Temp change short step*/
    {SETTINGS_DESC(SettingsItemIndex::TempChangeLongStep), settings_setTempChangeLongStep, settings_displayTempChangeLongStep},    /*Temp change long step*/
    {SETTINGS_DESC(SettingsItemIndex::LockingMode), settings_setLockingMode, settings_displayLockingMode},                         /*Locking Mode*/
    {0, nullptr, nullptr}                                                                                                          // end of menu marker. DO NOT REMOVE
};
const menuitem UIMenu[] = {
    /*
     // Language
     *  Scrolling Speed
     *  Temperature Unit
     *  Display orientation
     *  Cooldown blink
     *  Reverse Temp change buttons + -
     */
    {SETTINGS_DESC(SettingsItemIndex::TemperatureUnit), settings_setTempF,
     settings_displayTempF}, /* Temperature units, this has to be the first element in the array to work with the logic in settings_enterUIMenu() */
#ifndef NO_DISPLAY_ROTATE
    {SETTINGS_DESC(SettingsItemIndex::DisplayRotation), settings_setDisplayRotation, settings_displayDisplayRotation}, /*Display Rotation*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::CooldownBlink), settings_setCoolingBlinkEnabled, settings_displayCoolingBlinkEnabled},                                 /*Cooling blink warning*/
    {SETTINGS_DESC(SettingsItemIndex::ScrollingSpeed), settings_setScrollSpeed, settings_displayScrollSpeed},                                                /*Scroll Speed for descriptions*/
    {SETTINGS_DESC(SettingsItemIndex::ReverseButtonTempChange), settings_setReverseButtonTempChangeEnabled, settings_displayReverseButtonTempChangeEnabled}, /* Reverse Temp change buttons + - */
    {SETTINGS_DESC(SettingsItemIndex::AnimSpeed), settings_setAnimationSpeed, settings_displayAnimationSpeed},                                               /*Animation Speed adjustment */
    {SETTINGS_DESC(SettingsItemIndex::AnimLoop), settings_setAnimationLoop, settings_displayAnimationLoop},                                                  /*Animation Loop switch */
    {0, settings_setBrightnessLevel, settings_displayBrightnessLevel},                                                                                       /*Brightness Level*/
    {0, settings_setInvertColor, settings_displayInvertColor},                                                                                               /*Invert screen colour*/
    {0, nullptr, nullptr}                                                                                                                                    // end of menu marker. DO NOT REMOVE
};
const menuitem PowerSavingMenu[] = {
/*
 * Sleep Temp
 * 	Sleep Time
 * 	Shutdown Time
 * 	Motion Sensitivity
 */
#ifndef NO_SLEEP_MODE
    {SETTINGS_DESC(SettingsItemIndex::SleepTemperature), settings_setSleepTemp, settings_displaySleepTemp}, /*Sleep Temp*/
    {SETTINGS_DESC(SettingsItemIndex::SleepTimeout), settings_setSleepTime, settings_displaySleepTime},     /*Sleep Time*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::ShutdownTimeout), settings_setShutdownTime, settings_displayShutdownTime}, /*Shutdown Time*/
    {SETTINGS_DESC(SettingsItemIndex::MotionSensitivity), settings_setSensitivity, settings_displaySensitivity}, /* Motion Sensitivity*/
#ifdef HALL_SENSOR
    {SETTINGS_DESC(SettingsItemIndex::HallEffSensitivity), settings_setHallEffect, settings_displayHallEffect}, /* HallEffect Sensitivity*/
#endif
    {0, nullptr, nullptr} // end of menu marker. DO NOT REMOVE
};
const menuitem advancedMenu[] = {

    /*
     *  Power limit
     *  Detailed IDLE
     *  Detailed Soldering
     *  Calibrate Temperature
     *  Calibrate Input V
     *  Reset Settings
     *  Power Pulse
     *  Animation Loop
     *  Animation Speed
     *  Power Pulse Wait
     *  Power Pulse Duration
     */
    {SETTINGS_DESC(SettingsItemIndex::PowerLimit), settings_setPowerLimit, settings_displayPowerLimit},                                    /*Power limit*/
    {SETTINGS_DESC(SettingsItemIndex::AdvancedIdle), settings_setAdvancedIDLEScreens, settings_displayAdvancedIDLEScreens},                /* Advanced idle screen*/
    {SETTINGS_DESC(SettingsItemIndex::AdvancedSoldering), settings_setAdvancedSolderingScreens, settings_displayAdvancedSolderingScreens}, /* Advanced soldering screen*/
    {SETTINGS_DESC(SettingsItemIndex::SettingsReset), settings_setResetSettings, settings_displayResetSettings},                           /*Resets settings*/
    {SETTINGS_DESC(SettingsItemIndex::TemperatureCalibration), settings_setCalibrate, settings_displayCalibrate},                          /*Calibrate tip*/
    {SETTINGS_DESC(SettingsItemIndex::VoltageCalibration), settings_setCalibrateVIN, settings_displayCalibrateVIN},                        /*Voltage input cal*/
    {SETTINGS_DESC(SettingsItemIndex::PowerPulsePower), settings_setPowerPulse, settings_displayPowerPulse},                               /*Power Pulse adjustment */
    {SETTINGS_DESC(SettingsItemIndex::PowerPulseWait), settings_setPowerPulseWait, settings_displayPowerPulseWait},                        /*Power Pulse Wait adjustment*/
    {SETTINGS_DESC(SettingsItemIndex::PowerPulseDuration), settings_setPowerPulseDuration, settings_displayPowerPulseDuration},            /*Power Pulse Duration adjustment*/
    {0, nullptr, nullptr}                                                                                                                  // end of menu marker. DO NOT REMOVE
};

/**
 * Prints two small lines (or one line for CJK) of short description for
 * setting items and prepares cursor after it.
 * @param settingsItemIndex Index of the setting item.
 * @param cursorCharPosition Custom cursor char position to set after printing
 * description.
 */
static void printShortDescription(SettingsItemIndex settingsItemIndex, uint16_t cursorCharPosition) {
  // print short description (default single line, explicit double line)
  uint8_t shortDescIndex = static_cast<uint8_t>(settingsItemIndex);
  OLED::printWholeScreen(translatedString(Tr->SettingsShortNames[shortDescIndex]));

  // prepare cursor for value
  // make room for scroll indicator
  OLED::setCursor(cursorCharPosition * FONT_12_WIDTH - 2, 0);
}

static int userConfirmation(const char *message) {
  ScrollMessage scrollMessage;

  for (;;) {
    bool lcdRefresh = scrollMessage.drawUpdate(message, xTaskGetTickCount());

    ButtonState buttons = getButtonState();
    switch (buttons) {
    case BUTTON_F_SHORT:
      // User confirmed
      return 1;

    case BUTTON_NONE:
      break;
    default:
    case BUTTON_BOTH:
    case BUTTON_B_SHORT:
    case BUTTON_F_LONG:
    case BUTTON_B_LONG:
      return 0;
    }

    if (lcdRefresh) {
      OLED::refresh();
      osDelay(40);
    }
  }
  return 0;
}
#ifdef POW_DC
static bool settings_setInputVRange(void) {
  systemSettings.minDCVoltageCells = (systemSettings.minDCVoltageCells + 1) % 5;
  if (systemSettings.minDCVoltageCells == 1 && systemSettings.minVoltageCells < 30)
    systemSettings.minVoltageCells = 30;
  return systemSettings.minDCVoltageCells == 4;
}

static bool settings_displayInputVRange(void) {
  printShortDescription(SettingsItemIndex::DCInCutoff, 6);

  if (systemSettings.minDCVoltageCells) {
    OLED::printNumber(2 + systemSettings.minDCVoltageCells, 1, FontStyle::LARGE);
    OLED::print(SymbolCellCount, FontStyle::LARGE);
  } else {
    OLED::print(SymbolDC, FontStyle::LARGE);
  }
  return false;
}

static bool settings_setInputMinVRange(void) {
  systemSettings.minVoltageCells = (systemSettings.minVoltageCells + 1) % 38;
  if (systemSettings.minDCVoltageCells == 1 && systemSettings.minVoltageCells < 30)
    systemSettings.minVoltageCells = 30;
  else if (systemSettings.minVoltageCells < 24)
    systemSettings.minVoltageCells = 24;
  return systemSettings.minVoltageCells == 37;
}

static bool settings_displayInputMinVRange(void) {
  if (systemSettings.minDCVoltageCells) {
    printShortDescription(SettingsItemIndex::MinVolCell, 4);
    OLED::printNumber(systemSettings.minVoltageCells / 10, 2, FontStyle::LARGE);
    OLED::print(SymbolDot, FontStyle::LARGE);
    OLED::printNumber(systemSettings.minVoltageCells % 10, 1, FontStyle::LARGE);
  } else {
    printShortDescription(SettingsItemIndex::MinVolCell, 5);
    OLED::print(translatedString(Tr->SettingNAChar), FontStyle::LARGE);
  }
  return false;
}
#endif
#ifdef POW_QC
static bool settings_setQCInputV(void) {
#ifdef POW_QC_20V
  systemSettings.QCIdealVoltage = (systemSettings.QCIdealVoltage + 1) % 3;

  return systemSettings.QCIdealVoltage == 2;
#else
  systemSettings.QCIdealVoltage = (systemSettings.QCIdealVoltage + 1) % 2;
  return systemSettings.QCIdealVoltage == 1;
#endif
}

static bool settings_displayQCInputV(void) {
  printShortDescription(SettingsItemIndex::QCMaxVoltage, 5);
  // 0 = 9V, 1=12V, 2=20V (Fixed Voltages)
  // These are only used in QC modes
  switch (systemSettings.QCIdealVoltage) {
  case 0:
    OLED::printNumber(9, 2, FontStyle::LARGE);
    OLED::print(SymbolVolts, FontStyle::LARGE);
    break;
  case 1:
    OLED::printNumber(12, 2, FontStyle::LARGE);
    OLED::print(SymbolVolts, FontStyle::LARGE);
    break;
  case 2:
    OLED::printNumber(20, 2, FontStyle::LARGE);
    OLED::print(SymbolVolts, FontStyle::LARGE);
    break;
  default:
    break;
  }
  return false;
}

#endif

#ifdef POW_PD
static bool settings_setPDNegTimeout(void) {
  systemSettings.PDNegTimeout = (systemSettings.PDNegTimeout + 1) % 50;

  return systemSettings.PDNegTimeout == 49;
}

static bool settings_displayPDNegTimeout(void) {
  printShortDescription(SettingsItemIndex::PDNegTimeout, 5);
  OLED::printNumber(systemSettings.PDNegTimeout, 2, FontStyle::LARGE);

  return systemSettings.PDNegTimeout == 49;
}
#endif

#ifndef NO_SLEEP_MODE
static bool settings_setSleepTemp(void) {
  // If in C, 10 deg, if in F 20 deg
  if (systemSettings.temperatureInF) {
    systemSettings.SleepTemp += 20;
    if (systemSettings.SleepTemp > 580)
      systemSettings.SleepTemp = 60;
    return systemSettings.SleepTemp == 580;
  } else {
    systemSettings.SleepTemp += 10;
    if (systemSettings.SleepTemp > 300)
      systemSettings.SleepTemp = 10;
    return systemSettings.SleepTemp == 300;
  }
}

static bool settings_displaySleepTemp(void) {
  printShortDescription(SettingsItemIndex::SleepTemperature, 5);
  OLED::printNumber(systemSettings.SleepTemp, 3, FontStyle::LARGE);
  return false;
}

static bool settings_setSleepTime(void) {
  systemSettings.SleepTime++; // Go up 1 minute at a time
  if (systemSettings.SleepTime >= 16) {
    systemSettings.SleepTime = 0; // can't set time over 10 mins
  }
  // Remember that ^ is the time of no movement
  if (DetectedAccelerometerVersion == NO_DETECTED_ACCELEROMETER)
    systemSettings.SleepTime = 0; // Disable sleep on no accel
  return systemSettings.SleepTime == 15;
}

static bool settings_displaySleepTime(void) {
  printShortDescription(SettingsItemIndex::SleepTimeout, 5);
  if (systemSettings.SleepTime == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else if (systemSettings.SleepTime < 6) {
    OLED::printNumber(systemSettings.SleepTime * 10, 2, FontStyle::LARGE);
    OLED::print(SymbolSeconds, FontStyle::LARGE);
  } else {
    OLED::printNumber(systemSettings.SleepTime - 5, 2, FontStyle::LARGE);
    OLED::print(SymbolMinutes, FontStyle::LARGE);
  }
  return false;
}
#endif
static bool settings_setShutdownTime(void) {
  systemSettings.ShutdownTime++;
  if (systemSettings.ShutdownTime > 60) {
    systemSettings.ShutdownTime = 0; // wrap to off
  }
  if (DetectedAccelerometerVersion == NO_DETECTED_ACCELEROMETER)
    systemSettings.ShutdownTime = 0; // Disable shutdown on no accel
  return systemSettings.ShutdownTime == 60;
}

static bool settings_displayShutdownTime(void) {
  printShortDescription(SettingsItemIndex::ShutdownTimeout, 5);
  if (systemSettings.ShutdownTime == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(systemSettings.ShutdownTime, 2, FontStyle::LARGE);
    OLED::print(SymbolMinutes, FontStyle::LARGE);
  }
  return false;
}
static bool settings_setTempF(void) {
  systemSettings.temperatureInF = !systemSettings.temperatureInF;
  if (systemSettings.temperatureInF) {
    // Change sleep, boost and soldering temps to the F equiv
    // C to F == F= ( (C*9) +160)/5
    systemSettings.BoostTemp     = ((systemSettings.BoostTemp * 9) + 160) / 5;
    systemSettings.SolderingTemp = ((systemSettings.SolderingTemp * 9) + 160) / 5;
    systemSettings.SleepTemp     = ((systemSettings.SleepTemp * 9) + 160) / 5;
  } else {
    // Change sleep, boost and soldering temps to the C equiv
    // F->C == C = ((F-32)*5)/9
    systemSettings.BoostTemp     = ((systemSettings.BoostTemp - 32) * 5) / 9;
    systemSettings.SolderingTemp = ((systemSettings.SolderingTemp - 32) * 5) / 9;
    systemSettings.SleepTemp     = ((systemSettings.SleepTemp - 32) * 5) / 9;
  }
  // Rescale both to be multiples of 10
  systemSettings.BoostTemp = systemSettings.BoostTemp / 10;
  systemSettings.BoostTemp *= 10;
  systemSettings.SolderingTemp = systemSettings.SolderingTemp / 10;
  systemSettings.SolderingTemp *= 10;
  systemSettings.SleepTemp = systemSettings.SleepTemp / 10;
  systemSettings.SleepTemp *= 10;
  return false;
}

static bool settings_displayTempF(void) {
  printShortDescription(SettingsItemIndex::TemperatureUnit, 7);

  OLED::print((systemSettings.temperatureInF) ? SymbolDegF : SymbolDegC, FontStyle::LARGE);
  return false;
}

static bool settings_setSensitivity(void) {
  systemSettings.sensitivity++;
  systemSettings.sensitivity = systemSettings.sensitivity % 10;
  return systemSettings.sensitivity == 9;
}

static bool settings_displaySensitivity(void) {
  printShortDescription(SettingsItemIndex::MotionSensitivity, 7);
  OLED::printNumber(systemSettings.sensitivity, 1, FontStyle::LARGE, false);
  return false;
}

static bool settings_setAdvancedSolderingScreens(void) {
  systemSettings.detailedSoldering = !systemSettings.detailedSoldering;
  return false;
}

static bool settings_displayAdvancedSolderingScreens(void) {
  printShortDescription(SettingsItemIndex::AdvancedSoldering, 7);

  OLED::drawCheckbox(systemSettings.detailedSoldering);
  return false;
}

static bool settings_setAdvancedIDLEScreens(void) {
  systemSettings.detailedIDLE = !systemSettings.detailedIDLE;
  return false;
}

static bool settings_displayAdvancedIDLEScreens(void) {
  printShortDescription(SettingsItemIndex::AdvancedIdle, 7);

  OLED::drawCheckbox(systemSettings.detailedIDLE);
  return false;
}

static bool settings_setPowerLimit(void) {
  systemSettings.powerLimit += POWER_LIMIT_STEPS;
  if (systemSettings.powerLimit > MAX_POWER_LIMIT)
    systemSettings.powerLimit = 0;
  return systemSettings.powerLimit + POWER_LIMIT_STEPS > MAX_POWER_LIMIT;
}

static bool settings_displayPowerLimit(void) {
  printShortDescription(SettingsItemIndex::PowerLimit, 5);
  if (systemSettings.powerLimit == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(systemSettings.powerLimit, 2, FontStyle::LARGE);
    OLED::print(SymbolWatts, FontStyle::LARGE);
  }
  return false;
}

static bool settings_setScrollSpeed(void) {
  if (systemSettings.descriptionScrollSpeed == 0)
    systemSettings.descriptionScrollSpeed = 1;
  else
    systemSettings.descriptionScrollSpeed = 0;
  return false;
}

static bool settings_displayScrollSpeed(void) {
  printShortDescription(SettingsItemIndex::ScrollingSpeed, 7);
  OLED::print(translatedString((systemSettings.descriptionScrollSpeed) ? Tr->SettingFastChar : Tr->SettingSlowChar), FontStyle::LARGE);
  return false;
}

#ifndef NO_DISPLAY_ROTATE
static bool settings_setDisplayRotation(void) {
  systemSettings.OrientationMode++;
  systemSettings.OrientationMode = systemSettings.OrientationMode % 3;
  switch (systemSettings.OrientationMode) {
  case 0:
    OLED::setRotation(false);
    break;
  case 1:
    OLED::setRotation(true);
    break;
  case 2:
    // do nothing on auto
    break;
  default:
    break;
  }
  return systemSettings.OrientationMode == 2;
}

static bool settings_displayDisplayRotation(void) {
  printShortDescription(SettingsItemIndex::DisplayRotation, 7);

  switch (systemSettings.OrientationMode) {
  case 0:
    OLED::print(translatedString(Tr->SettingRightChar), FontStyle::LARGE);
    break;
  case 1:
    OLED::print(translatedString(Tr->SettingLeftChar), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingAutoChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingRightChar), FontStyle::LARGE);
    break;
  }
  return false;
}
#endif
static bool settings_setBoostTemp(void) {
  if (systemSettings.temperatureInF) {
    if (systemSettings.BoostTemp == 0) {
      systemSettings.BoostTemp = MIN_BOOST_TEMP_F; // loop back at 480
    } else {
      systemSettings.BoostTemp += 20; // Go up 20F at a time
    }

    if (systemSettings.BoostTemp > MAX_TEMP_F) {
      systemSettings.BoostTemp = 0; // jump to off
    }
    return systemSettings.BoostTemp == MAX_TEMP_F - 10;
  } else {
    if (systemSettings.BoostTemp == 0) {
      systemSettings.BoostTemp = MIN_BOOST_TEMP_C; // loop back at 250
    } else {
      systemSettings.BoostTemp += 10; // Go up 10C at a time
    }
    if (systemSettings.BoostTemp > MAX_TEMP_C) {
      systemSettings.BoostTemp = 0; // Go to off state
    }
    return systemSettings.BoostTemp == MAX_TEMP_C;
  }
}

static bool settings_displayBoostTemp(void) {
  printShortDescription(SettingsItemIndex::BoostTemperature, 5);
  if (systemSettings.BoostTemp) {
    OLED::printNumber(systemSettings.BoostTemp, 3, FontStyle::LARGE);
  } else {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  }
  return false;
}

static bool settings_setAutomaticStartMode(void) {
  systemSettings.autoStartMode++;
  systemSettings.autoStartMode %= 4;
  return systemSettings.autoStartMode == 3;
}

static bool settings_displayAutomaticStartMode(void) {
  printShortDescription(SettingsItemIndex::AutoStart, 7);

  switch (systemSettings.autoStartMode) {
  case 0:
    OLED::print(translatedString(Tr->SettingStartNoneChar), FontStyle::LARGE);
    break;
  case 1:
    OLED::print(translatedString(Tr->SettingStartSolderingChar), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingStartSleepChar), FontStyle::LARGE);
    break;
  case 3:
    OLED::print(translatedString(Tr->SettingStartSleepOffChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingStartNoneChar), FontStyle::LARGE);
    break;
  }
  return false;
}

static bool settings_setLockingMode(void) {
  systemSettings.lockingMode++;
  systemSettings.lockingMode %= 3;
  return systemSettings.lockingMode == 2;
}

static bool settings_displayLockingMode(void) {
  printShortDescription(SettingsItemIndex::LockingMode, 7);

  switch (systemSettings.lockingMode) {
  case 0:
    OLED::print(translatedString(Tr->SettingLockDisableChar), FontStyle::LARGE);
    break;
  case 1:
    OLED::print(translatedString(Tr->SettingLockBoostChar), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingLockFullChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingLockDisableChar), FontStyle::LARGE);
    break;
  }
  return false;
}

static bool settings_setCoolingBlinkEnabled(void) {
  systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;
  return false;
}

static bool settings_displayCoolingBlinkEnabled(void) {
  printShortDescription(SettingsItemIndex::CooldownBlink, 7);
  OLED::drawCheckbox(systemSettings.coolingTempBlink);
  return false;
}

static bool settings_setResetSettings(void) {
  if (userConfirmation(translatedString(Tr->SettingsResetWarning))) {
    resetSettings();
    warnUser(translatedString(Tr->ResetOKMessage), 2 * TICKS_SECOND);
  }
  return false;
}

static bool settings_displayResetSettings(void) {
  printShortDescription(SettingsItemIndex::SettingsReset, 7);
  return false;
}

static void setTipOffset() {
  systemSettings.CalibrationOffset = 0;

  // If the thermo-couple at the end of the tip, and the handle are at
  // equilibrium, then the output should be zero, as there is no temperature
  // differential.
  while (systemSettings.CalibrationOffset == 0) {
    uint32_t offset = 0;
    for (uint8_t i = 0; i < 16; i++) {
      offset += getTipRawTemp(1);
      // cycle through the filter a fair bit to ensure we're stable.
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      OLED::print(SymbolDot, FontStyle::LARGE);
      for (uint8_t x = 0; x < (i / 4); x++)
        OLED::print(SymbolDot, FontStyle::LARGE);
      OLED::refresh();
      osDelay(100);
    }
    systemSettings.CalibrationOffset = TipThermoModel::convertTipRawADCTouV(offset / 16);
  }
  OLED::clearScreen();
  OLED::setCursor(0, 0);
  OLED::drawCheckbox(true);
  OLED::printNumber(systemSettings.CalibrationOffset, 4, FontStyle::LARGE);
  OLED::refresh();
  osDelay(1200);
}

// Provide the user the option to tune their own tip if custom is selected
// If not only do single point tuning as per usual
static bool settings_setCalibrate(void) {

  if (userConfirmation(translatedString(Tr->SettingsCalibrationWarning))) {
    // User confirmed
    // So we now perform the actual calculation
    setTipOffset();
  }
  return false;
}

static bool settings_displayCalibrate(void) {
  printShortDescription(SettingsItemIndex::TemperatureCalibration, 5);
  return false;
}

static bool settings_setCalibrateVIN(void) {
  // Jump to the voltage calibration subscreen
  OLED::clearScreen();

  for (;;) {
    OLED::setCursor(0, 0);
    OLED::printNumber(getInputVoltageX10(systemSettings.voltageDiv, 0) / 10, 2, FontStyle::LARGE);
    OLED::print(SymbolDot, FontStyle::LARGE);
    OLED::printNumber(getInputVoltageX10(systemSettings.voltageDiv, 0) % 10, 1, FontStyle::LARGE, false);
    OLED::print(SymbolVolts, FontStyle::LARGE);

    ButtonState buttons = getButtonState();
    switch (buttons) {
    case BUTTON_F_SHORT:
      systemSettings.voltageDiv++;
      break;

    case BUTTON_B_SHORT:
      systemSettings.voltageDiv--;
      break;

    case BUTTON_BOTH:
    case BUTTON_F_LONG:
    case BUTTON_B_LONG:
      saveSettings();
      OLED::setCursor(0, 0);
      OLED::printNumber(systemSettings.voltageDiv, 3, FontStyle::LARGE);
      OLED::refresh();
      waitForButtonPressOrTimeout(1 * TICKS_SECOND);
      return false;
    case BUTTON_NONE:
    default:
      break;
    }

    OLED::refresh();
    osDelay(40);

    // Cap to sensible values
#if defined(MODEL_TS80) + defined(MODEL_TS80P) > 0
    if (systemSettings.voltageDiv < 500) {
      systemSettings.voltageDiv = 500;
    } else if (systemSettings.voltageDiv > 900) {
      systemSettings.voltageDiv = 900;
    }
#else
    if (systemSettings.voltageDiv < 360) {
      systemSettings.voltageDiv = 360;
    } else if (systemSettings.voltageDiv > 520) {
      systemSettings.voltageDiv = 520;
    }
#endif
  }
  return false;
}

static bool settings_setReverseButtonTempChangeEnabled(void) {
  systemSettings.ReverseButtonTempChangeEnabled = !systemSettings.ReverseButtonTempChangeEnabled;
  return false;
}

static bool settings_displayReverseButtonTempChangeEnabled(void) {
  printShortDescription(SettingsItemIndex::ReverseButtonTempChange, 7);
  OLED::drawCheckbox(systemSettings.ReverseButtonTempChangeEnabled);
  return false;
}

static bool settings_setTempChangeShortStep(void) {
  systemSettings.TempChangeShortStep += TEMP_CHANGE_SHORT_STEP;
  if (systemSettings.TempChangeShortStep > TEMP_CHANGE_SHORT_STEP_MAX) {
    systemSettings.TempChangeShortStep = TEMP_CHANGE_SHORT_STEP; // loop back at TEMP_CHANGE_SHORT_STEP_MAX
  }
  return systemSettings.TempChangeShortStep == TEMP_CHANGE_SHORT_STEP_MAX;
}

static bool settings_displayTempChangeShortStep(void) {
  printShortDescription(SettingsItemIndex::TempChangeShortStep, 6);
  OLED::printNumber(systemSettings.TempChangeShortStep, 2, FontStyle::LARGE);
  return false;
}

static bool settings_setTempChangeLongStep(void) {
  if (systemSettings.TempChangeLongStep == TEMP_CHANGE_SHORT_STEP) {
    systemSettings.TempChangeLongStep = TEMP_CHANGE_LONG_STEP / 2;
  } else if (systemSettings.TempChangeLongStep == TEMP_CHANGE_LONG_STEP / 2) {
    systemSettings.TempChangeLongStep = TEMP_CHANGE_LONG_STEP;
  } else {
    systemSettings.TempChangeLongStep += TEMP_CHANGE_LONG_STEP;
  }
  if (systemSettings.TempChangeLongStep > TEMP_CHANGE_LONG_STEP_MAX) {
    systemSettings.TempChangeLongStep = TEMP_CHANGE_SHORT_STEP; // loop back at TEMP_CHANGE_LONG_STEP_MAX
  }
  return systemSettings.TempChangeLongStep == TEMP_CHANGE_LONG_STEP_MAX;
}

static bool settings_displayTempChangeLongStep(void) {
  printShortDescription(SettingsItemIndex::TempChangeLongStep, 6);
  OLED::printNumber(systemSettings.TempChangeLongStep, 2, FontStyle::LARGE);
  return false;
}

static bool settings_setPowerPulse(void) {
  systemSettings.KeepAwakePulse += POWER_PULSE_INCREMENT;
  systemSettings.KeepAwakePulse %= POWER_PULSE_MAX;

  return systemSettings.KeepAwakePulse == POWER_PULSE_MAX - 1;
}
static bool settings_displayPowerPulse(void) {
  printShortDescription(SettingsItemIndex::PowerPulsePower, 5);
  if (systemSettings.KeepAwakePulse) {
    OLED::printNumber(systemSettings.KeepAwakePulse / 10, 1, FontStyle::LARGE);
    OLED::print(SymbolDot, FontStyle::LARGE);
    OLED::printNumber(systemSettings.KeepAwakePulse % 10, 1, FontStyle::LARGE);
  } else {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  }
  return false;
}

static bool settings_setAnimationLoop(void) {
  systemSettings.animationLoop = !systemSettings.animationLoop;
  return false;
}

static bool settings_displayAnimationLoop(void) {
  printShortDescription(SettingsItemIndex::AnimLoop, 7);
  OLED::drawCheckbox(systemSettings.animationLoop);
  return false;
}

static bool settings_setAnimationSpeed(void) {
  systemSettings.animationSpeed++;
  systemSettings.animationSpeed %= settingOffSpeed_t::MAX_VALUE;
  return systemSettings.animationSpeed == (uint8_t)settingOffSpeed_t::FAST;
}

static bool settings_displayAnimationSpeed(void) {
  printShortDescription(SettingsItemIndex::AnimSpeed, 7);
  switch (systemSettings.animationSpeed) {
  case settingOffSpeed_t::SLOW:
    OLED::print(translatedString(Tr->SettingSlowChar), FontStyle::LARGE);
    break;
  case settingOffSpeed_t::MEDIUM:
    OLED::print(translatedString(Tr->SettingMediumChar), FontStyle::LARGE);
    break;
  case settingOffSpeed_t::FAST:
    OLED::print(translatedString(Tr->SettingFastChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingOffChar), FontStyle::LARGE);
    break;
  }
  return false;
}

static bool settings_setPowerPulseWait(void) {
  // Constrain to range 1 to POWER_PULSE_WAIT_MAX inclusive
  auto &wait = systemSettings.KeepAwakePulseWait;
  if (++wait > POWER_PULSE_WAIT_MAX) {
    wait = 1;
  }

  return wait == POWER_PULSE_WAIT_MAX;
}

static bool settings_displayPowerPulseWait(void) {
  if (systemSettings.KeepAwakePulse) {
    printShortDescription(SettingsItemIndex::PowerPulseWait, 7);
    OLED::printNumber(systemSettings.KeepAwakePulseWait, 1, FontStyle::LARGE);
    return false;
  } else {
    return true; // skip
  }
}

static bool settings_setPowerPulseDuration(void) {
  // Constrain to range 1 to POWER_PULSE_DURATION_MAX inclusive
  auto &duration = systemSettings.KeepAwakePulseDuration;
  if (++duration > POWER_PULSE_DURATION_MAX) {
    duration = 1;
  }

  return duration == POWER_PULSE_DURATION_MAX;
}

static bool settings_displayPowerPulseDuration(void) {
  if (systemSettings.KeepAwakePulse) {
    printShortDescription(SettingsItemIndex::PowerPulseDuration, 7);
    OLED::printNumber(systemSettings.KeepAwakePulseDuration, 1, FontStyle::LARGE);
    return false;
  } else {
    return true; // skip
  }
}

static uint8_t brightness = 0x00;

static bool settings_displayBrightnessLevel(void) {
  OLED::drawArea(0, 0, 16, 16, brightnessIcon);
  OLED::setCursor(5 * FONT_12_WIDTH - 2, 0);
  OLED::printNumber(brightness, 3, FontStyle::LARGE);
  return false;
}

static bool settings_setBrightnessLevel(void) {
  if (brightness > (0xFF - 0x08 + 1)) {
    brightness = 0;
  } else if (brightness == 0) {
    brightness = 7;
  } else {
    brightness += 0x08;
  }
  OLED::setContrast(brightness);
  return brightness == 0xFF;
}

static bool invertColor = false;

static bool settings_displayInvertColor(void) {
  OLED::drawArea(0, 0, 24, 16, invertDisplayIcon);
  OLED::setCursor(7 * FONT_12_WIDTH - 2, 0);
  OLED::drawCheckbox(invertColor);
  return false;
}
static bool settings_setInvertColor(void) {
  invertColor = !invertColor;
  OLED::setInverseDisplay(invertColor);
  return invertColor;
}

#ifdef HALL_SENSOR
static bool settings_displayHallEffect(void) {
  printShortDescription(SettingsItemIndex::HallEffSensitivity, 7);
  switch (systemSettings.hallEffectSensitivity) {
  case 1:
    OLED::print(translatedString(Tr->SettingSensitivityLow), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingSensitivityMedium), FontStyle::LARGE);
    break;
  case 3:
    OLED::print(translatedString(Tr->SettingSensitivityHigh), FontStyle::LARGE);
    break;
  case 0:
  default:
    OLED::print(translatedString(Tr->SettingSensitivityOff), FontStyle::LARGE);
    break;
  }
  return false;
}
static bool settings_setHallEffect(void) {
  // To keep life simpler for now, we have a few preset sensitivity levels
  // Off, Low, Medium, High
  systemSettings.hallEffectSensitivity++;
  systemSettings.hallEffectSensitivity %= 4;
  return systemSettings.hallEffectSensitivity == 3;
}
#endif

// Indicates whether a menu transition is in progress, so that the menu icon
// animation is paused during the transition.
static bool animOpenState = false;

static void displayMenu(size_t index) {
  // Call into the menu
  // Draw title
  OLED::printWholeScreen(translatedString(Tr->SettingsMenuEntries[index]));
  // Draw symbol
  // 16 pixel wide image
  // 2 pixel wide scrolling indicator
  static TickType_t menuSwitchLoopTick = 0;
  static size_t     menuCurrentIndex   = sizeof(rootSettingsMenu) + 1;
  TickType_t        step               = TICKS_100MS * 5;
  switch (systemSettings.animationSpeed) {
  case settingOffSpeed_t::FAST:
    step = TICKS_100MS * 3;
    break;
  case settingOffSpeed_t::MEDIUM:
    step = TICKS_100MS * 4;
    break;
  default: // SLOW or off - defaulted above
    break;
  }
  size_t currentFrame;
  if (!animOpenState && systemSettings.animationSpeed != settingOffSpeed_t::OFF) {
    if (menuCurrentIndex != index) {
      menuCurrentIndex   = index;
      menuSwitchLoopTick = xTaskGetTickCount();
    }
    currentFrame = ((xTaskGetTickCount() - menuSwitchLoopTick) / step);
    if (systemSettings.animationLoop) {
      currentFrame %= 3;
    } else if (currentFrame > 2) {
      currentFrame = 2;
    }
  } else {
    // We want the animation to restart after completing the transition.
    menuCurrentIndex = sizeof(rootSettingsMenu) + 1;
    // Always draw the last frame if icon animation is disabled.
    currentFrame = systemSettings.animationSpeed == settingOffSpeed_t::OFF ? 2 : 0;
  }
  OLED::drawArea(OLED_WIDTH - 16 - 2, 0, 16, 16, (&SettingsMenuIcons[index][(16 * 2) * currentFrame]));
}

static bool settings_displayCalibrateVIN(void) {
  printShortDescription(SettingsItemIndex::VoltageCalibration, 5);
  return false;
}

#if defined(POW_DC) || defined(POW_QC)
static bool settings_displayPowerMenu(void) {
  displayMenu(0);
  return false;
}
static bool settings_enterPowerMenu(void) {
  gui_Menu(powerMenu);
  return false;
}
#endif
static bool settings_displaySolderingMenu(void) {
  displayMenu(1);
  return false;
}
static bool settings_enterSolderingMenu(void) {
  gui_Menu(solderingMenu);
  return false;
}
static bool settings_displayPowerSavingMenu(void) {
  displayMenu(2);
  return false;
}
static bool settings_enterPowerSavingMenu(void) {
  gui_Menu(PowerSavingMenu);
  return false;
}
static bool settings_displayUIMenu(void) {
  displayMenu(3);
  return false;
}
static bool settings_enterUIMenu(void) {
  gui_Menu(HasFahrenheit ? UIMenu : UIMenu + 1);
  return false;
}
static bool settings_displayAdvancedMenu(void) {
  displayMenu(4);
  return false;
}
static bool settings_enterAdvancedMenu(void) {
  gui_Menu(advancedMenu);
  return false;
}

void gui_Menu(const menuitem *menu) {
  // Draw the settings menu and provide iteration support etc

  // This is used to detect whether a menu-exit transition should be played.
  static bool wasInGuiMenu;
  wasInGuiMenu = true;

  enum class NavState {
    Idle,
    Entering,
    ScrollingDown,
    Exiting,
  };

  uint8_t     currentScreen          = 0;
  TickType_t  autoRepeatTimer        = 0;
  TickType_t  autoRepeatAcceleration = 0;
  bool        earlyExit              = false;
  bool        lcdRefresh             = true;
  ButtonState lastButtonState        = BUTTON_NONE;
  uint8_t     scrollContentSize      = 0;
  bool        scrollBlink            = false;
  bool        lastValue              = false;
  NavState    navState               = NavState::Entering;

  ScrollMessage scrollMessage;

  for (uint8_t i = 0; menu[i].draw != nullptr; i++) {
    scrollContentSize += 1;
  }

  while ((menu[currentScreen].draw != nullptr) && earlyExit == false) {

    // Handle menu transition:
    if (navState != NavState::Idle) {
      // Check if this menu item shall be skipped. If it shall be skipped,
      // `draw()` returns true. Draw on the secondary framebuffer as we want
      // to keep the primary framebuffer intact for the upcoming transition
      // animation.
      OLED::useSecondaryFramebuffer(true);
      if (menu[currentScreen].draw()) {
        currentScreen++;
        OLED::useSecondaryFramebuffer(false);
        continue;
      }

      animOpenState = true;
      // The menu entering/exiting transition uses the secondary framebuffer,
      // but the scroll down transition does not.
      if (navState == NavState::ScrollingDown) {
        OLED::useSecondaryFramebuffer(false);
      }
      OLED::setCursor(0, 0);
      OLED::clearScreen();
      menu[currentScreen].draw();
      if (navState == NavState::ScrollingDown) {
        // Play the scroll down animation.
        OLED::maskScrollIndicatorOnOLED();
        OLED::transitionScrollDown();
      } else {
        // The menu was drawn in a secondary framebuffer.
        // Now we play a transition from the pre-drawn primary
        // framebuffer to the new buffer.
        // The extra buffer is discarded at the end of the transition.
        OLED::useSecondaryFramebuffer(false);
        OLED::transitionSecondaryFramebuffer(navState == NavState::Entering);
      }
      animOpenState = false;
      navState      = NavState::Idle;
    }

    // If the user has hesitated for >=3 seconds, show the long text
    // Otherwise "draw" the option
    if ((xTaskGetTickCount() - lastButtonTime < (TICKS_SECOND * 3)) || menu[currentScreen].description == 0) {
      lcdRefresh = true;
      OLED::setCursor(0, 0);
      OLED::clearScreen();
      menu[currentScreen].draw();
      uint8_t indicatorHeight = OLED_HEIGHT / scrollContentSize;
      uint8_t position        = OLED_HEIGHT * currentScreen / scrollContentSize;
      if (lastValue)
        scrollBlink = !scrollBlink;
      if (!lastValue || !scrollBlink)
        OLED::drawScrollIndicator(position, indicatorHeight);
    } else {
      // Draw description
      const char *description = translatedString(Tr->SettingsDescriptions[menu[currentScreen].description - 1]);
      lcdRefresh |= scrollMessage.drawUpdate(description, xTaskGetTickCount());
    }

    if (lcdRefresh) {
      OLED::refresh(); // update the LCD
      osDelay(40);
      lcdRefresh = false;
    }

    ButtonState buttons = getButtonState();

    if (buttons != lastButtonState) {
      autoRepeatAcceleration = 0;
      lastButtonState        = buttons;
    }

    auto callIncrementHandler = [&]() {
      wasInGuiMenu = false;
      bool res     = menu[currentScreen].incrementHandler();
      if (wasInGuiMenu) {
        navState = NavState::Exiting;
      }
      wasInGuiMenu = true;
      return res;
    };

    switch (buttons) {
    case BUTTON_BOTH:
      earlyExit = true; // will make us exit next loop
      scrollMessage.reset();
      break;
    case BUTTON_F_SHORT:
      // increment
      if (scrollMessage.isReset()) {
        if (menu[currentScreen].incrementHandler != nullptr) {
          lastValue = callIncrementHandler();
        } else {
          earlyExit = true;
        }
      } else
        scrollMessage.reset();
      break;
    case BUTTON_B_SHORT:
      if (scrollMessage.isReset()) {
        currentScreen++;
        navState  = NavState::ScrollingDown;
        lastValue = false;
      } else
        scrollMessage.reset();
      break;
    case BUTTON_F_LONG:
      if (xTaskGetTickCount() + autoRepeatAcceleration > autoRepeatTimer + PRESS_ACCEL_INTERVAL_MAX) {
        if ((lastValue = callIncrementHandler()))
          autoRepeatTimer = 1000;
        else
          autoRepeatTimer = 0;

        autoRepeatTimer += xTaskGetTickCount();

        scrollMessage.reset();

        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_B_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        currentScreen++;
        navState        = NavState::ScrollingDown;
        autoRepeatTimer = xTaskGetTickCount();
        scrollMessage.reset();

        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_NONE:
    default:
      break;
    }

    if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration) < PRESS_ACCEL_INTERVAL_MIN) {
      autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
    }

    if ((xTaskGetTickCount() - lastButtonTime) > (TICKS_SECOND * 30)) {
      // If user has not pressed any buttons in 30 seconds, exit back a menu layer
      // This will trickle the user back to the main screen eventually
      earlyExit = true;
      scrollMessage.reset();
    }
  }
}

void enterSettingsMenu() {
  gui_Menu(rootSettingsMenu); // Call the root menu
  saveSettings();
}
