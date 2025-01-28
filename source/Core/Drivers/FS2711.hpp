#include "configuration.h"
#ifndef _DRIVERS_FS2711_HPP_
#define _DRIVERS_FS2711_HPP_
// #define POW_PD_EXT 2
#if POW_PD_EXT == 2
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t  pdo_num; // Nums of USB-PD Objects max of 7
  uint16_t source_current;
  uint16_t source_voltage;
  uint16_t req_pdo_num;
  uint16_t pdo_type[7];
  uint16_t pdo_min_volt[7];
  uint16_t pdo_max_volt[7];
  uint16_t pdo_max_curr[7];
} fs2711_state_t;

class FS2711 {
public:
  static uint8_t detect_i2c_bus_num();
  
  static bool probe();

  static void start();

  static bool open_pps(uint8_t PDOID, uint16_t volt, uint16_t max_curr);

  static bool open_pd(uint8_t PDOID);

  static void negotiate();

  static bool has_run_selection();

  static uint16_t source_voltage();

  static uint16_t source_currentx100();

  static uint8_t selected_protocol();

  static void pdo_update();

  static uint8_t        debug_protocol();
  static uint16_t       debug_pdo_max_voltage(uint8_t pdoid);
  static uint16_t       debug_pdo_min_voltage(uint8_t pdoid);
  static uint16_t       debug_pdo_source_current(uint8_t pdoid);
  static uint16_t       debug_pdo_type(uint8_t pdoid);
  static fs2711_state_t debug_get_state();

private:
  // Internal state of IC
  static fs2711_state_t state;

  static void enable_protocol(bool enable);

  static void select_protocol(uint8_t protocol);

  static void enable_voltage();
};

#endif
#endif
