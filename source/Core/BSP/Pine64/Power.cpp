#include "BSP.h"
#include "BSP_Power.h"
#include "Model_Config.h"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"
#include "USBPD.h"

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
#ifdef POW_QC
  QC_resync();
#endif
}

bool getIsPoweredByDCIN() {
#ifdef POW_PD
  if (!USBPowerDelivery::negotiationComplete()) {
    return false; // We are assuming not dc while negotiating
  }
  if (USBPowerDelivery::negotiationHasWorked()) {
    return false; // We are using PD
  }
#endif

#ifdef POW_QC
  if (hasQCNegotiated()) {
    return false; // We are using QC
  }
#endif
  return true;
}
