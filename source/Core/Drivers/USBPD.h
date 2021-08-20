
#ifndef DRIVERS_USBPD_H_
#define DRIVERS_USBPD_H_
#include "configuration.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#ifdef POW_PD
class USBPowerDelivery {
public:
  static bool    start();                 // Start the PD stack
  static bool    negotiationComplete();   // Has negotiation completed to a voltage > 5v
  static bool    negotiationInProgress(); // Is negotiation ongoing
  static bool    fusbPresent();           // Is the FUSB302 present on the bus
  static void    PPSTimerCallback();      // PPS Timer
  static void    IRQOccured();            // Thread callback that an irq occured
  static void    step();                  // Iterate the step machine
  static bool    negotiationHasWorked();  //
  static uint8_t getStateNumber();        //

private:
  //
  static int detectionState;
};
#endif

#endif
#endif