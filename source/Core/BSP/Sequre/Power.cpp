#include "BSP.h"
#include "BSP_Power.h"
#include "FS2711.hpp"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"
#include "USBPD.h"
#include "configuration.h"

void power_check() {
#ifdef POW_PD
  // Cant start QC until either PD works or fails
  if (!USBPowerDelivery::negotiationComplete()) {
    return;
  }
  if (USBPowerDelivery::negotiationHasWorked()) {
    return; // We are using PD
  }
#endif
#if POW_PD_EXT == 2
  if (FS2711::has_run_selection()) {
    return;
  }
#endif
#ifdef POW_QC
  QC_resync();
#endif
}

bool getIsPoweredByDCIN() {
#if POW_PD_EXT == 2 && defined(POW_DC)
  if (!FS2711::has_run_selection()) {
    return true;
  } else if (FS2711::debug_get_state().source_voltage > 0) {
    return false;
  } else {
    return true;
  }
#else
  return false;
#endif
}
