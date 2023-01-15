#include "OperatingModes.h"
extern osThreadId GUITaskHandle;
extern osThreadId MOVTaskHandle;
extern osThreadId PIDTaskHandle;

#ifdef MODEL_Pinecilv2
#include "bl702_adc.h"
extern ADC_Gain_Coeff_Type adcGainCoeffCal;
#endif

void showDebugMenu(void) {
  uint8_t     screen = 0;
  ButtonState b;
  for (;;) {
    OLED::clearScreen();                                // Ensure the buffer starts clean
    OLED::setCursor(0, 0);                              // Position the cursor at the 0,0 (top left)
    OLED::print(SymbolVersionNumber, FontStyle::SMALL); // Print version number
    OLED::setCursor(0, 8);                              // second line
    OLED::print(DebugMenu[screen], FontStyle::SMALL);
    switch (screen) {
    case 0: // Build Date
      break;
    case 1: // Device ID
    {
      uint64_t id = getDeviceID();
#ifdef DEVICE_HAS_VALIDATION_CODE
      // If device has validation code; then we want to take over both lines of the screen
      OLED::clearScreen();   // Ensure the buffer starts clean
      OLED::setCursor(0, 0); // Position the cursor at the 0,0 (top left)
      OLED::print(DebugMenu[screen], FontStyle::SMALL);
      OLED::drawHex(getDeviceValidation(), FontStyle::SMALL, 8);
      OLED::setCursor(0, 8); // second line
#endif
      OLED::drawHex((uint32_t)(id >> 32), FontStyle::SMALL, 8);
      OLED::drawHex((uint32_t)(id & 0xFFFFFFFF), FontStyle::SMALL, 8);
    } break;
    case 2: // ACC Type
      OLED::print(AccelTypeNames[(int)DetectedAccelerometerVersion], FontStyle::SMALL);
      break;
    case 3: // Power Negotiation Status
    {
      int sourceNumber = 0;
      if (getIsPoweredByDCIN()) {
        sourceNumber = 0;
      } else {
        // We are not powered via DC, so want to display the appropriate state for PD or QC
        bool poweredbyPD        = false;
        bool pdHasVBUSConnected = false;
#if POW_PD
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
      OLED::print(PowerSourceNames[sourceNumber], FontStyle::SMALL);
    } break;
    case 4: // Input Voltage
      printVoltage();
      break;
    case 5: // Temp in °C
      OLED::printNumber(TipThermoModel::getTipInC(), 6, FontStyle::SMALL);
      break;
    case 6: // Handle Temp in °C
      OLED::printNumber(getHandleTemperature(0) / 10, 6, FontStyle::SMALL);
      OLED::print(SymbolDot, FontStyle::SMALL);
      OLED::printNumber(getHandleTemperature(0) % 10, 1, FontStyle::SMALL);
      break;
    case 7: // Max Temp Limit in °C
      OLED::printNumber(TipThermoModel::getTipMaxInC(), 6, FontStyle::SMALL);
      break;
    case 8: // System Uptime
      OLED::printNumber(xTaskGetTickCount() / TICKS_100MS, 8, FontStyle::SMALL);
      break;
    case 9: // Movement Timestamp
      OLED::printNumber(lastMovementTime / TICKS_100MS, 8, FontStyle::SMALL);
      break;
    case 10:                                                              // Tip Resistance in Ω
      OLED::printNumber(getTipResistanceX10() / 10, 6, FontStyle::SMALL); // large to pad over so that we cover ID left overs
      OLED::print(SymbolDot, FontStyle::SMALL);
      OLED::printNumber(getTipResistanceX10() % 10, 1, FontStyle::SMALL);
      break;
    case 11: // Raw Tip in µV
      OLED::printNumber(TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true), 8, FontStyle::SMALL);
      break;
    case 12: // Tip Cold Junction Compensation Offset in µV
      OLED::printNumber(getSettingValue(SettingsOptions::CalibrationOffset), 8, FontStyle::SMALL);
      break;
    case 13: // High Water Mark for GUI
      OLED::printNumber(uxTaskGetStackHighWaterMark(GUITaskHandle), 8, FontStyle::SMALL);
      break;
    case 14: // High Water Mark for Movement Task
      OLED::printNumber(uxTaskGetStackHighWaterMark(MOVTaskHandle), 8, FontStyle::SMALL);
      break;
    case 15: // High Water Mark for PID Task
      OLED::printNumber(uxTaskGetStackHighWaterMark(PIDTaskHandle), 8, FontStyle::SMALL);
      break;
      break;
#ifdef HALL_SENSOR
    case 16: // Raw Hall Effect Value
    {
      int16_t hallEffectStrength = getRawHallEffect();
      if (hallEffectStrength < 0)
        hallEffectStrength = -hallEffectStrength;
      OLED::printNumber(hallEffectStrength, 6, FontStyle::SMALL);
    } break;
#endif
#ifdef MODEL_Pinecilv2
    case 17:
    {
        if (adcGainCoeffCal.adcGainCoeffEnable) {
            const int32_t coe_x10000 = (int)(adcGainCoeffCal.coe * 10000 + 0.5);
            OLED::printNumber(coe_x10000 / 10000, 3, FontStyle::SMALL);
            OLED::print(SymbolDot, FontStyle::SMALL);
            OLED::printNumber(coe_x10000 % 10000, 4, FontStyle::SMALL, false);
        } else {
            OLED::print(translatedString(Tr->OffString), FontStyle::SMALL);
        }
    } break;
#endif


    default:
      break;
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT)
      return;
    else if (b == BUTTON_F_SHORT) {
      screen++;
#ifndef HALL_SENSOR
      if (screen == 16) screen = 17;
#endif
#ifndef MODEL_Pinecilv2
      if (screen == 17) screen = 18;
#endif
      screen = screen % 18;
    }
    GUIDelay();
  }
}
