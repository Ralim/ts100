#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <stdint.h>
/**
 * Configuration.h
 * Define here your default pre settings for MHP30
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
#define SLEEP_TIME    5 // x10 Seconds
#define SHUTDOWN_TIME 0 // Minutes -- Default shutdown to being off

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
 *
 */
#define ORIENTATION_MODE           0 // 0: Right 1:Left 2:Automatic - Default right
#define MAX_ORIENTATION_MODE       1 // Unlikely to ever change
#define REVERSE_BUTTON_TEMP_CHANGE 0 // 0:Default 1:Reverse - Reverse the plus and minus button assigment for temperature change

/**
 * OLED Brightness
 *
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

#define ADC_MAX_READING (4096 * 8) // Maximum reading of the adc
#define ADC_VDD_MV      3300       // ADC max reading millivolts

#define POWER_PULSE_WAIT_DEFAULT     4 // Default rate of the power pulse: 4*2500 = 10000 ms = 10 s
#define POWER_PULSE_DURATION_DEFAULT 1 // Default duration of the power pulse: 1*250 = 250 ms
#define POWER_PULSE_DEFAULT          5

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

// Due to large thermal mass of the PCB being heated we need to pull this back a bit
#define THERMAL_RUNAWAY_TIME_SEC 20
#define THERMAL_RUNAWAY_TEMP_C   2

#define CUT_OUT_SETTING          0  // default to no cut-off voltage
#define RECOM_VOL_CELL           33 // Minimum voltage per cell (Recommended 3.3V (33))
#define TEMPERATURE_INF          0  // default to 0
#define DESCRIPTION_SCROLL_SPEED 0  // 0: Slow 1: Fast - default to slow
#define ANIMATION_LOOP           1  // 0: off 1: on
#define ANIMATION_SPEED          settingOffSpeed_t::MEDIUM

#define OP_AMP_Rf_MHP30  268500 //  268.5  Kilo-ohms -> Measured
#define OP_AMP_Rin_MHP30 1600   //  1.6  Kilo-ohms -> Measured

#define OP_AMP_GAIN_STAGE_MHP30 (1 + (OP_AMP_Rf_MHP30 / OP_AMP_Rin_MHP30))
// Deriving the Voltage div:
// Vin_max = (3.3*(r1+r2))/(r2)
// vdiv = (32768*4)/(vin_max*10)

#ifndef MODEL_MHP30
#error "No model defined!"
#endif

#ifdef MODEL_MHP30
#define SOLDERING_TEMP         200                     // Default soldering temp is 200.0 °C
#define VOLTAGE_DIV            360                     // Default for MHP30
#define PID_POWER_LIMIT        65                      // Sets the max pwm power limit
#define CALIBRATION_OFFSET     0                       // the adc offset in uV - MHP compensates automagically
#define MIN_CALIBRATION_OFFSET 0                       // Min value for calibration
#define POWER_LIMIT            65                      // 65 watts default power limit
#define MAX_POWER_LIMIT        65                      //
#define POWER_LIMIT_STEPS      1                       //
#define OP_AMP_GAIN_STAGE      OP_AMP_GAIN_STAGE_MHP30 //
#define USB_PD_VMAX            20                      // Maximum voltage for PD to negotiate
#define MODEL_HAS_DCDC                                 // Has inductor to current filter
#define PID_TIM_HZ             (16)                    //
#define MAX_TEMP_C             350                     // Max soldering temp selectable °C
#define MAX_TEMP_F             660                     // Max soldering temp selectable °F
#define MIN_TEMP_C             10                      // Min soldering temp selectable °C
#define MIN_TEMP_F             50                      // Min soldering temp selectable °F
#define MIN_BOOST_TEMP_C       150                     // The min settable temp for boost mode °C
#define MIN_BOOST_TEMP_F       300                     // The min settable temp for boost mode °F
#define NO_DISPLAY_ROTATE                              // Disable OLED rotation by accel
#define SLEW_LIMIT             50                      // Limit to 3.0 Watts per 64ms pid loop update rate slew rate
#define TIPTYPE_MHP30          1                       // It's own special tip
#define ACCEL_SC7
#define ACCEL_MSA

#define PROFILE_SUPPORT
#define OLED_96x16         1
#define POW_PD             1
#define USB_PD_TIMEOUT     20 // Default Timeout for USB-PD Protocol negotiation in x100ms
#define POW_PD_EXT         0
#define USB_PD_EPR_WATTAGE 0 /*No EPR*/
#define TEMP_NTC
#define I2C_SOFT_BUS_2  1
#define I2C_SOFT_BUS_1  1
#define OLED_I2CBB1     1
#define ACCEL_I2CBB1    1
#define BATTFILTERDEPTH 8
#define OLED_I2CBB2
#define ACCEL_EXITS_ON_MOVEMENT
#define NEEDS_VBUS_PROBE 0

#define HARDWARE_MAX_WATTAGE_X10 650
#define TIP_THERMAL_MASS         65 // TODO, needs refinement
#define TIP_RESISTANCE           60 // x10 ohms, ~6 typical
#endif                              /* MHP30 */

#ifdef ACCEL_EXITS_ON_MOVEMENT
#define NO_SLEEP_MODE
#endif

#define FLASH_LOGOADDR      (0x08000000 + (126 * 1024))
#define SETTINGS_START_PAGE (0x08000000 + (127 * 1024))

#endif /* CONFIGURATION_H_ */
