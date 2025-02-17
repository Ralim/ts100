/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_

#include <stdbool.h>
#include <stdint.h>

extern const bool HasFahrenheit;

extern const char *SmallSymbolPlus;
extern const char *LargeSymbolPlus;
extern const char *SmallSymbolMinus;
extern const char *LargeSymbolMinus;
extern const char *SmallSymbolSpace;
extern const char *LargeSymbolSpace;
extern const char *SmallSymbolAmps;
extern const char *LargeSymbolAmps;
extern const char *SmallSymbolDot;
extern const char *LargeSymbolDot;
extern const char *SmallSymbolSlash;
extern const char *SmallSymbolColon;
extern const char *SmallSymbolDegC;
extern const char *LargeSymbolDegC;
extern const char *SmallSymbolDegF;
extern const char *LargeSymbolDegF;
extern const char *LargeSymbolMinutes;
extern const char *SmallSymbolMinutes;
extern const char *LargeSymbolSeconds;
extern const char *SmallSymbolSeconds;
extern const char *LargeSymbolWatts;
extern const char *SmallSymbolWatts;
extern const char *LargeSymbolVolts;
extern const char *SmallSymbolVolts;
extern const char *LargeSymbolDC;
extern const char *SmallSymbolDC;
extern const char *LargeSymbolCellCount;
extern const char *SmallSymbolCellCount;
//
extern const char *SmallSymbolVersionNumber;
extern const char *SmallSymbolPDDebug;
extern const char *SmallSymbolState;
extern const char *SmallSymbolNoVBus;
extern const char *SmallSymbolVBus;

extern const char *LargeSymbolSleep;

extern const char *DebugMenu[];
extern const char *AccelTypeNames[];
extern const char *PowerSourceNames[];

/* !
 * The order of the items inside this enum class SettingsItemIndex
 * must be the same as the order of the related items
 * in the "menuOptions" section of translations_definitions.json file.
 */
enum class SettingsItemIndex : uint8_t {
  DCInCutoff,
  MinVolCell,
  QCMaxVoltage,
  PDNegTimeout,
  USBPDMode,
  BoostTemperature,
  AutoStart,
  TempChangeShortStep,
  TempChangeLongStep,
  LockingMode,
  ProfilePhases,
  ProfilePreheatTemp,
  ProfilePreheatSpeed,
  ProfilePhase1Temp,
  ProfilePhase1Duration,
  ProfilePhase2Temp,
  ProfilePhase2Duration,
  ProfilePhase3Temp,
  ProfilePhase3Duration,
  ProfilePhase4Temp,
  ProfilePhase4Duration,
  ProfilePhase5Temp,
  ProfilePhase5Duration,
  ProfileCooldownSpeed,
  MotionSensitivity,
  SleepTemperature,
  SleepTimeout,
  ShutdownTimeout,
  HallEffSensitivity,
  HallEffSleepTimeout,
  TemperatureUnit,
  DisplayRotation,
  CooldownBlink,
  ScrollingSpeed,
  ReverseButtonTempChange,
  ReverseButtonMenu,
  AnimSpeed,
  AnimLoop,
  Brightness,
  ColourInversion,
  LOGOTime,
  AdvancedIdle,
  AdvancedSoldering,
  BluetoothLE,
  PowerLimit,
  CalibrateCJC,
  VoltageCalibration,
  PowerPulsePower,
  PowerPulseWait,
  PowerPulseDuration,
  SettingsReset,
  LanguageSwitch,
  SolderingTipType,
  NUM_ITEMS,
};

struct TranslationIndexTable {
  uint16_t CalibrationDone;
  uint16_t ResetOKMessage;
  uint16_t SettingsResetMessage;
  uint16_t NoAccelerometerMessage;
  uint16_t NoPowerDeliveryMessage;
  uint16_t LockingKeysString;
  uint16_t UnlockingKeysString;
  uint16_t WarningKeysLockedString;
  uint16_t WarningThermalRunaway;
  uint16_t WarningTipShorted;

  uint16_t SettingsCalibrationWarning;
  uint16_t CJCCalibrating;
  uint16_t SettingsResetWarning;
  uint16_t UVLOWarningString;
  uint16_t UndervoltageString;
  uint16_t InputVoltageString;
  uint16_t ProfilePreheatString;
  uint16_t ProfileCooldownString;

  uint16_t SleepingAdvancedString;
  uint16_t SleepingTipAdvancedString;
  uint16_t DeviceFailedValidationWarning;
  uint16_t TooHotToStartProfileWarning;

  uint16_t SettingRightChar;
  uint16_t SettingLeftChar;
  uint16_t SettingAutoChar;
  uint16_t SettingSlowChar;
  uint16_t SettingMediumChar;
  uint16_t SettingFastChar;
  uint16_t SettingStartSolderingChar;
  uint16_t SettingStartSleepChar;
  uint16_t SettingStartSleepOffChar;
  uint16_t SettingLockBoostChar;
  uint16_t SettingLockFullChar;
  uint16_t USBPDModeDefault;
  uint16_t USBPDModeNoDynamic;
  uint16_t USBPDModeSafe;
  uint16_t TipTypeAuto;
  uint16_t TipTypeT12Long;
  uint16_t TipTypeT12Short;
  uint16_t TipTypeT12PTS;
  uint16_t TipTypeTS80;
  uint16_t TipTypeJBCC210;

  uint16_t SettingsDescriptions[static_cast<uint32_t>(SettingsItemIndex::NUM_ITEMS)];
  uint16_t SettingsShortNames[static_cast<uint32_t>(SettingsItemIndex::NUM_ITEMS)];
  uint16_t SettingsMenuEntriesDescriptions[5]; // unused
  uint16_t SettingsMenuEntries[5];
};

extern const TranslationIndexTable *Tr;

extern const char *TranslationStrings;

struct TranslationData {
  TranslationIndexTable indices;
  // Translation strings follows the translation index table.
  // C++ does not support flexible array member as in C, so we use a 1-element
  // array as a placeholder.
  char strings[1];
};

struct FontSection {
  const uint8_t *font12_start_ptr;
  const uint8_t *font06_start_ptr;
  uint16_t       font12_decompressed_size;
  uint16_t       font06_decompressed_size;
  const uint8_t *font12_compressed_source; // Pointer to compressed data or null
  const uint8_t *font06_compressed_source; // Pointer to compressed data or null
};

extern const FontSection FontSectionInfo;

constexpr uint8_t settings_item_index(const SettingsItemIndex i) { return static_cast<uint8_t>(i); }
// Use a constexpr function for type-checking.
#define SETTINGS_DESC(i) (settings_item_index(i) + 1)

const char *translatedString(uint16_t index);

void prepareTranslations();
void settings_displayLanguageSwitch(void);
bool settings_showLanguageSwitch(void);
void settings_setLanguageSwitch(void);
bool isLastLanguageOption(void);

#endif /* TRANSLATION_H_ */
