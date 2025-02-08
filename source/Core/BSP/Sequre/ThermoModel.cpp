/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "Utils.hpp"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_PT1000
// Use https://br.flukecal.com/pt100-table-generator to make table for resistance to temp
const int32_t ohmsToDegC[] = {
    //
    //  Resistance (ohms x10)	Temperature (Celsius)

    10000, 0,   //
    10390, 10,  //
    10779, 20,  //
    11167, 30,  //
    11554, 40,  //
    11940, 50,  //
    12324, 60,  //
    12708, 70,  //
    13090, 80,  //
    13471, 90,  //
    13851, 100, //
    14229, 110, //
    14607, 120, //
    14983, 130, //
    15358, 140, //
    15733, 150, //
    16105, 160, //
    16477, 170, //
    16848, 180, //
    17217, 190, //
    17586, 200, //
    17953, 210, //
    18319, 220, //
    18684, 230, //
    19047, 240, //
    19410, 250, //
    19771, 260, //
    20131, 270, //
    20490, 280, //
    20848, 290, //
    21205, 300, //
    21561, 310, //
    21915, 320, //
    22268, 330, //
    22621, 340, //
    22972, 350, //
    23321, 360, //
    23670, 370, //
    24018, 380, //
    24364, 390, //
    24709, 400, //
    25053, 410, //
    25396, 420, //
    25738, 430, //
    26078, 440, //
    26418, 450, //
    26756, 460, //
    27093, 470, //
    27429, 480, //
    27764, 490, //
    28098, 500, //

};

TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) {

  // 3.3V -> 1K ->(ADC) <- PT1000 <- GND
  // PT100 = (adc*r1)/(3.3V-adc)
  uint32_t reading_mv     = tipuVDelta / 1000;
  uint32_t resistance_x10 = (reading_mv * 10000) / (3300 - reading_mv);

  return Utils::InterpolateLookupTable(ohmsToDegC, sizeof(ohmsToDegC) / (2 * sizeof(int32_t)), resistance_x10);
}

#endif // TEMP_uV_LOOKUP_PT1000

#ifdef TEMP_uV_LOOKUP_S60
TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 50) / 485; }
#endif // TEMP_uV_LOOKUP_S60

#ifdef TEMP_uV_LOOKUP_S99
// 42.85 uV / K -> 1/42.85 K/uV = 20/857
// TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 20) / 857; }
// Measurement is probably with heating element in series, thats why the reading differs from the approx 42.85 uV/K
TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 1) / 30; }
#endif // TEMP_uV_LOOKUP_S99
