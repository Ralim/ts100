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
  if (USBPowerDelivery::negotiationComplete()) {
    return;
  }
#endif
#ifdef POW_QC
  QC_resync();
#endif
}

bool getIsPoweredByDCIN() {
#ifdef MODEL_TS80
  return false;
#endif

#ifdef MODEL_TS80P
  return false;
#endif

#ifdef MODEL_TS100
  return true;
#endif
}
