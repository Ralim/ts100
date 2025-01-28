#ifndef OPERATING_MODES_H_
#define OPERATING_MODES_H_

extern "C" {
#include "FreeRTOSConfig.h"
}
#include "Buttons.hpp"
#include "OLED.hpp"
#include "OperatingModeUtilities.h"
#include "Settings.h"
#include "TipThermoModel.h"
#include "Translation.h"
#include "Types.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "settingsGUI.hpp"
#include "stdlib.h"
#include "string.h"
#ifdef POW_PD
#include "USBPD.h"
#include "pd.h"
#endif

enum class OperatingMode {
  StartupLogo=10,        // Showing the startup logo
  CJCCalibration=11,     // Cold Junction Calibration
  StartupWarnings=12,    // Startup checks and warnings
  InitialisationDone=13, // Special state we use just before we to home screen at first startup. Allows jumping to extra startup states
  HomeScreen=0,         // Home/Idle screen that is the main launchpad to other modes
  Soldering=1,          // Main soldering operating mode
  SolderingProfile=6,   // Soldering by following a profile, used for reflow for example
  Sleeping=3,           // Sleep state holds iron at lower sleep temp
  Hibernating=14,       // Like sleeping but keeps heater fully off until woken
  SettingsMenu=4,       // Settings Menu
  DebugMenuReadout=5,   // Debug metrics
  TemperatureAdjust=7,  // Set point temperature adjustment
  UsbPDDebug=8,         // USB PD debugging information
  ThermalRunaway=9,     // Thermal Runaway warning state.
};

enum class TransitionAnimation {
  None  = 0,
  Right = 1,
  Left  = 2,
  Down  = 3,
  Up    = 4,
};

// Generic context struct used for gui functions to be able to retain state
struct guiContext {
  TickType_t          viewEnterTime; // Set to ticks when this view state was first entered
  OperatingMode       previousMode;
  TransitionAnimation transitionMode;
  // Below is scratch state, this is retained over re-draws but blown away on state change
  struct scratch {
    uint16_t state1; // 16 bit state scratch
    uint16_t state2; // 16 bit state scratch
    uint32_t state3; // 32 bit state scratch
    uint32_t state4; // 32 bit state scratch
    uint16_t state5; // 16 bit state scratch
    uint16_t state6; // 16 bit state scratch
    uint32_t state7; // 32 bit state scratch

  } scratch_state;
};

// Main functions
OperatingMode gui_SolderingSleepingMode(const ButtonState buttons, guiContext *cxt); // Sleep mode
OperatingMode gui_solderingMode(const ButtonState buttons, guiContext *cxt);         // Main mode for hot pointy tool
OperatingMode gui_solderingTempAdjust(const ButtonState buttons, guiContext *cxt);   // For adjusting the setpoint temperature of the iron
OperatingMode drawHomeScreen(const ButtonState buttons, guiContext *cxt);            // IDLE / Home screen
OperatingMode gui_SettingsMenu(const ButtonState buttons, guiContext *cxt);          //

OperatingMode gui_solderingProfileMode(const ButtonState buttons, guiContext *cxt); // Profile mode for hot likely-not-so-pointy tool
OperatingMode performCJCC(const ButtonState buttons, guiContext *cxt);              // Used to calibrate the Cold Junction offset
OperatingMode showDebugMenu(const ButtonState buttons, guiContext *cxt);            // Debugging values
OperatingMode showPDDebug(const ButtonState buttons, guiContext *cxt);              // Debugging menu that shows PD adaptor info
OperatingMode showWarnings(const ButtonState buttons, guiContext *cxt);             // Shows user warnings if required

// Common helpers
int8_t getPowerSourceNumber(void); // Returns number ID of power source

extern uint8_t heaterThermalRunawayCounter;
#endif
