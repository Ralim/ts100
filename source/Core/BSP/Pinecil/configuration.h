#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <stdint.h>
/**
 * Configuration.h
 * Define here your default pre settings for Pinecil
 *
 */

//===========================================================================
//============================= Default Settings ============================
//===========================================================================
/**
 * Default soldering temp is 320.0 C
 * Temperature the iron sleeps at - default 150.0 C
 */

#define SLEEP_TEMP         150 // Default sleep temperature
#define BOOST_TEMP         420 // Default boost temp.
#define BOOST_MODE_ENABLED 1   // 0: Disable 1: Enable

/**
 * Blink the temperature on the cooling screen when its > 50C
 */
#define COOLING_TEMP_BLINK 0 // 0: Disable 1: Enable

/**
 * How many seconds/minutes we wait until going to sleep/shutdown.
 * Values -> SLEEP_TIME * 10; i.e. 5*10 = 50 Seconds!
 */
#define SLEEP_TIME    5  // x10 Seconds
#define SHUTDOWN_TIME 10 // Minutes

/**
 * Auto start off for safety.
 * Pissible values are:
 *  0 - none
 *  1 - Soldering Temperature
 *  2 - Sleep Temperature
 *  3 - Sleep Off Temperature
 */
#define AUTO_START_MODE 0 // Default to none

/**
 * Locking Mode
 * When in soldering mode a long press on both keys toggle the lock of the buttons
 * Possible values are:
 *  0 - Desactivated
 *  1 - Lock except boost
 *  2 - Full lock
 */
#define LOCKING_MODE 0 // Default to desactivated for safety

/**
 * OLED Orientation
 */
#define ORIENTATION_MODE           2 // 0: Right 1:Left 2:Automatic - Default Automatic
#define MAX_ORIENTATION_MODE       2 // Up to auto

/**
 * Buttons handling
 */
#define REVERSE_BUTTON_TEMP_CHANGE 0 // 0:Default 1:Reverse - Reverse the plus and minus button assignment for temperature change
#define REVERSE_BUTTON_MENU        0 // 0:Default 1:Reverse - Reverse the A and B button assignment for Settings menu only

/**
 * OLED Brightness
 */
#define MIN_BRIGHTNESS     0   // Min OLED brightness selectable
#define MAX_BRIGHTNESS     100 // Max OLED brightness selectable
#define BRIGHTNESS_STEP    25  // OLED brightness increment
#define DEFAULT_BRIGHTNESS 25  // default OLED brightness

/**
 * Temp change settings
 */
#define TEMP_CHANGE_SHORT_STEP     1  // Default temp change short step +1
#define TEMP_CHANGE_LONG_STEP      10 // Default temp change long step +10
#define TEMP_CHANGE_SHORT_STEP_MAX 50 // Temp change short step MAX value
#define TEMP_CHANGE_LONG_STEP_MAX  90 // Temp change long step MAX value

/* Power pulse for keeping power banks awake*/
#define POWER_PULSE_INCREMENT    1
#define POWER_PULSE_MAX          100 // x10 max watts
#define POWER_PULSE_WAIT_MAX     9   // 9*2.5s = 22.5 seconds
#define POWER_PULSE_DURATION_MAX 9   // 9*250ms = 2.25 seconds

#ifdef MODEL_Pinecil
#define POWER_PULSE_DEFAULT 0
#else
#define POWER_PULSE_DEFAULT 5
#endif                                 /* Pinecil */
#define POWER_PULSE_WAIT_DEFAULT     4 // Default rate of the power pulse: 4*2500 = 10000 ms = 10 s
#define POWER_PULSE_DURATION_DEFAULT 1 // Default duration of the power pulse: 1*250 = 250 ms

/**
 * OLED Orientation Sensitivity on Automatic mode!
 * Motion Sensitivity <0=Off 1=Least Sensitive 9=Most Sensitive>
 */
#define SENSITIVITY 7 // Default 7

/**
 * Detailed soldering screen
 * Detailed idle screen (off for first time users)
 */
#define DETAILED_SOLDERING 0 // 0: Disable 1: Enable - Default 0
#define DETAILED_IDLE      0 // 0: Disable 1: Enable - Default 0

#define THERMAL_RUNAWAY_TIME_SEC 20
#define THERMAL_RUNAWAY_TEMP_C   3

#define CUT_OUT_SETTING          0  // default to no cut-off voltage
#define RECOM_VOL_CELL           33 // Minimum voltage per cell (Recommended 3.3V (33))
#define TEMPERATURE_INF          0  // default to 0
#define DESCRIPTION_SCROLL_SPEED 0  // 0: Slow 1: Fast - default to slow
#define ANIMATION_LOOP           1  // 0: off 1: on
#define ANIMATION_SPEED          settingOffSpeed_t::MEDIUM

#define OP_AMP_Rf_Pinecil  750 * 1000 // 750  Kilo-ohms -> From schematic, R1
#define OP_AMP_Rin_Pinecil 2370       // 2.37 Kilo-ohms -> From schematic, R2

#define OP_AMP_GAIN_STAGE_PINECIL (1 + (OP_AMP_Rf_Pinecil / OP_AMP_Rin_Pinecil))

#define ADC_MAX_READING (4096 * 8) // Maximum reading of the adc
#define ADC_VDD_MV      3300       // ADC max reading millivolts

#if defined(MODEL_Pinecil) == 0
#error "No model defined!"
#endif

#ifdef MODEL_Pinecil
#define SOLDERING_TEMP         320                       // Default soldering temp is 320.0 °C
#define VOLTAGE_DIV            467                       // 467 - Default divider from schematic
#define CALIBRATION_OFFSET     900                       // 900 - Default adc offset in uV
#define MIN_CALIBRATION_OFFSET 100                       // Min value for calibration
#define PID_POWER_LIMIT        70                        // Sets the max pwm power limit
#define POWER_LIMIT            0                         // 0 watts default limit
#define MAX_POWER_LIMIT        70                        //
#define POWER_LIMIT_STEPS      5                         //
#define OP_AMP_GAIN_STAGE      OP_AMP_GAIN_STAGE_PINECIL // Uses TS100 resistors
#define TEMP_uV_LOOKUP_HAKKO                             // Use Hakko lookup table
#define USB_PD_VMAX            20                        // Maximum voltage for PD to negotiate
#define PID_TIM_HZ             (8)                       // Tick rate of the PID loop
#define MAX_TEMP_C             450                       // Max soldering temp selectable °C
#define MAX_TEMP_F             850                       // Max soldering temp selectable °F
#define MIN_TEMP_C             10                        // Min soldering temp selectable °C
#define MIN_TEMP_F             50                        // Min soldering temp selectable °F
#define MIN_BOOST_TEMP_C       250                       // The min settable temp for boost mode °C
#define MIN_BOOST_TEMP_F       480                       // The min settable temp for boost mode °F

#define OLED_96x16         1
#define POW_PD             1
#define USB_PD_EPR_WATTAGE 0 /*No EPR (Yet?) */
#define POW_PD_EXT         0
#define POW_QC             1
#define POW_DC             1
#define POW_QC_20V         1
#define ENABLE_QC2         1
#define MAG_SLEEP_SUPPORT  1
#define TIPTYPE_T12        1 // Can manually pick a T12 tip
#define TEMP_TMP36
#define ACCEL_BMA
#define ACCEL_SC7
#define HALL_SENSOR
#define VBUS_MOD_TEST
#define HALL_SI7210
#define DEBUG_UART_OUTPUT
#define NEEDS_VBUS_PROBE 1

#define HARDWARE_MAX_WATTAGE_X10 750
#define TIP_THERMAL_MASS         65 // X10 watts to raise 1 deg C in 1 second
#define TIP_RESISTANCE           75 // x10 ohms, 7.5 typical for Pinecil tips
#define CANT_DIRECT_READ_SETTINGS
#endif /* Pinecil */

#define FLASH_LOGOADDR      (0x08000000 + (126 * 1024))
#define SETTINGS_START_PAGE (0x08000000 + (127 * 1024))

#define HAS_POWER_DEBUG_MENU

#endif /* CONFIGURATION_H_ */
