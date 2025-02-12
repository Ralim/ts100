/*
 * GUIThread.cpp
 *
 *  Created on: 19 Aug 2019
 *      Author: ralim
 */
extern "C" {
#include "FreeRTOSConfig.h"
}
#include "BootLogo.h"
#include "Buttons.hpp"
#include "I2CBB2.hpp"
#include "LIS2DH12.hpp"
#include "MMA8652FC.hpp"
#include "OLED.hpp"
#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#include "Settings.h"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "settingsGUI.hpp"
#include "stdlib.h"
#include "string.h"
#include "ui_drawing.hpp"
#ifdef POW_PD
#include "USBPD.h"
#include "pd.h"
#endif

// File local variables
#define MOVEMENT_INACTIVITY_TIME (60 * configTICK_RATE_HZ)
#define BUTTON_INACTIVITY_TIME   (60 * configTICK_RATE_HZ)

ButtonState   buttonsAtDeviceBoot;                                      // We record button state at startup, incase of jumping to debug modes
OperatingMode currentOperatingMode = OperatingMode::InitialisationDone; // Current mode we are rendering
guiContext    context;                                                  // Context passed to functions to aid in state during render passes

OperatingMode handle_post_init_state();
OperatingMode guiHandleDraw(void) {
  OLED::clearScreen(); // Clear ready for render pass
  bool d = getSettingValue(SettingsOptions::ReverseButtonNavEnabled);
  bool e = getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled);
#ifdef REVERSE_NAV_EVERYWHERE
  bool f = currentOperatingMode == OperatingMode::TemperatureAdjust;
  // Read button state
  ButtonState buttons = getButtonState((e && f) || (d && !e && !f) || (d && e && !f));
#else
  ButtonState buttons = getButtonState();
#endif
  // Enforce screen on if buttons pressed, movement, hot tip etc
  if (buttons != BUTTON_NONE) {
    OLED::setDisplayState(OLED::DisplayState::ON);
  } else {
    // Buttons are none; check if we can sleep display
    uint32_t tipTemp = TipThermoModel::getTipInC();
    if ((tipTemp < 50) && getSettingValue(SettingsOptions::Sensitivity) &&
        (((xTaskGetTickCount() - lastMovementTime) > MOVEMENT_INACTIVITY_TIME) && ((xTaskGetTickCount() - lastButtonTime) > BUTTON_INACTIVITY_TIME))) {
      OLED::setDisplayState(OLED::DisplayState::OFF);
      setStatusLED(LED_OFF);
    } else {
      OLED::setDisplayState(OLED::DisplayState::ON);
      if (tipTemp > 55) {
        setStatusLED(LED_COOLING_STILL_HOT);
      } else {
        setStatusLED(LED_STANDBY);
      }
    }
  }
  // Dispatch button state to gui mode
  OperatingMode newMode = currentOperatingMode;
  switch (currentOperatingMode) {
  case OperatingMode::StartupWarnings:
    newMode = showWarnings(buttons, &context);
    break;
  case OperatingMode::UsbPDDebug:
#ifdef HAS_POWER_DEBUG_MENU
    newMode = showPDDebug(buttons, &context);
    break;
#else
    newMode = OperatingMode::InitialisationDone;
#endif
  case OperatingMode::StartupLogo:
    showBootLogo();

    if (getSettingValue(SettingsOptions::AutoStartMode) == autoStartMode_t::SLEEP) {
      lastMovementTime = lastButtonTime = 0; // We mask the values so that sleep goes until user moves again or presses a button
      newMode                           = OperatingMode::Sleeping;
    } else if (getSettingValue(SettingsOptions::AutoStartMode) == autoStartMode_t::SOLDER) {
      lastMovementTime = lastButtonTime = xTaskGetTickCount(); // Move forward so we dont go to sleep
      newMode                           = OperatingMode::Soldering;
    } else if (getSettingValue(SettingsOptions::AutoStartMode) == autoStartMode_t::ZERO) {
      lastMovementTime = lastButtonTime = 0; // We mask the values so that sleep goes until user moves again or presses a button
      newMode                           = OperatingMode::Hibernating;
    } else {
      newMode = OperatingMode::HomeScreen;
    }

    break;
  default:
    /* Fallthrough */
  case OperatingMode::HomeScreen:
    newMode = drawHomeScreen(buttons, &context);
    break;
  case OperatingMode::Soldering:
    context.scratch_state.state4 = 0;
    newMode                      = gui_solderingMode(buttons, &context);
    break;
  case OperatingMode::SolderingProfile:
    newMode = gui_solderingProfileMode(buttons, &context);
    break;
  case OperatingMode::Sleeping:
    newMode = gui_SolderingSleepingMode(buttons, &context);
    break;
  case OperatingMode::TemperatureAdjust:
#ifdef REVERSE_NAV_EVERYWHERE
    newMode = gui_solderingTempAdjust(getButtonState(), &context);
#else
    newMode = gui_solderingTempAdjust(getButtonState(e), &context);
#endif
    break;
  case OperatingMode::DebugMenuReadout:
    newMode = showDebugMenu(buttons, &context);
    break;
  case OperatingMode::CJCCalibration:
    newMode = performCJCC(buttons, &context);
    break;
  case OperatingMode::SettingsMenu:
#ifdef REVERSE_NAV_EVERYWHERE
    newMode = gui_SettingsMenu(getButtonState(), &context);
#else
    newMode = gui_SettingsMenu(getButtonState(d), &context);
#endif
    break;
  case OperatingMode::InitialisationDone:
    newMode = handle_post_init_state();
    break;
  case OperatingMode::Hibernating:
    context.scratch_state.state4 = 1;
    gui_SolderingSleepingMode(buttons, &context);
    if (lastButtonTime > 0 || lastMovementTime > 0) {
      newMode = OperatingMode::Soldering;
    }
    break;
  case OperatingMode::ThermalRunaway:
    /*TODO*/
    newMode = OperatingMode::HomeScreen;
    break;
  };
  return newMode;
}
void guiRenderLoop(void) {
  OperatingMode newMode = guiHandleDraw(); // This does the screen drawing

  // Post draw we handle any state transitions

  if (newMode != currentOperatingMode) {
    context.viewEnterTime = xTaskGetTickCount();
    context.previousMode  = currentOperatingMode;
    // If the previous mode is the startup logo; we dont want to return to it, but instead dispatch out to either home or soldering
    if (currentOperatingMode == OperatingMode::StartupLogo) {
      if (getSettingValue(SettingsOptions::AutoStartMode)) {
        context.previousMode = OperatingMode::Soldering;
      } else {
        newMode = OperatingMode::HomeScreen;
      }
    }
    memset(&context.scratch_state, 0, sizeof(context.scratch_state));
    currentOperatingMode = newMode;
  }
  // If the transition marker is set, we need to make the next draw occur to the secondary buffer so we have something to transition to
  if (context.transitionMode != TransitionAnimation::None) {
    OLED::useSecondaryFramebuffer(true);
    // Now we need to fill the secondary buffer with the _next_ frame to transistion to
    guiHandleDraw();
    OLED::useSecondaryFramebuffer(false);
    // Now dispatch the transition
    switch (context.transitionMode) {
    case TransitionAnimation::Down:
      OLED::transitionScrollDown(context.viewEnterTime);
      break;
    case TransitionAnimation::Left:
      OLED::transitionSecondaryFramebuffer(false, context.viewEnterTime);
      break;
    case TransitionAnimation::Right:
      OLED::transitionSecondaryFramebuffer(true, context.viewEnterTime);
      break;
    case TransitionAnimation::Up:
      OLED::transitionScrollUp(context.viewEnterTime);

    case TransitionAnimation::None:
    default:
      break; // Do nothing on unknown
    }

    context.transitionMode = TransitionAnimation::None; // Clear transition flag
  }
  // Render done, draw it out
  OLED::refresh();
}

OperatingMode handle_post_init_state() {
#ifdef HAS_POWER_DEBUG_MENU
#ifdef DEBUG_POWER_MENU_BUTTON_B
  if (buttonsAtDeviceBoot == BUTTON_B_LONG || buttonsAtDeviceBoot == BUTTON_B_SHORT) {
#else
  if (buttonsAtDeviceBoot == BUTTON_F_LONG || buttonsAtDeviceBoot == BUTTON_F_SHORT) {
#endif
    buttonsAtDeviceBoot = BUTTON_NONE;
    return OperatingMode::UsbPDDebug;
  }
#endif

  if (getSettingValue(SettingsOptions::CalibrateCJC) > 0) {
    return OperatingMode::CJCCalibration;
  }

  return OperatingMode::StartupWarnings;
}

/* StartGUITask function */
void startGUITask(void const *argument) {
  (void)argument;
  prepareTranslations();

  OLED::initialize(); // start up the LCD
  OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
  OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));

  bool buttonLockout = false;
  ui_pre_render_assets();
  getTipRawTemp(1); // reset filter
  memset(&context, 0, sizeof(context));

  OLED::setRotation(getSettingValue(SettingsOptions::OrientationMode) & 1);

  // Read boot button state
  if (getButtonA()) {
    buttonsAtDeviceBoot = BUTTON_F_LONG;
  }
  if (getButtonB()) {
    buttonsAtDeviceBoot = BUTTON_B_LONG;
  }

  TickType_t startRender = xTaskGetTickCount();
  for (;;) {
    guiRenderLoop();
    resetWatchdog();
    vTaskDelayUntil(&startRender, TICKS_100MS * 4 / 10); // Try and maintain 20-25fps ish update rate, way to fast but if we can its nice
  }
}
