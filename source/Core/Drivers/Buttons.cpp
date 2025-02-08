/*
 * Buttons.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */
#include "FreeRTOS.h"
#include "OperatingModeUtilities.h"
#include "settingsGUI.hpp"
#include "task.h"
#include <Buttons.hpp>
TickType_t lastButtonTime = 0;

ButtonState getButtonState(bool revert) {
  /*
   * Read in the buttons and then determine if a state change needs to occur
   */

  /*
   * If the previous state was  00 Then we want to latch the new state if
   * different & update time
   * If the previous state was !00 Then we want to search if we trigger long
   * press (buttons still down), or if release we trigger press
   * (downtime>filter)
   */
  static uint8_t    previousState       = 0;
  static bool       longPressed         = false;
  static TickType_t previousStateChange = 0;
  const TickType_t  timeout             = TICKS_100MS * 4;
  uint8_t           currentState;
  currentState = (getButtonA()) << (0 xor revert);
  currentState |= (getButtonB()) << (1 xor revert);

  if (currentState) {
    lastButtonTime = xTaskGetTickCount();
  }
  if (currentState == previousState) {
    if (currentState == 0) {
      return BUTTON_NONE;
    }
    if ((xTaskGetTickCount() - previousStateChange) >= timeout) {
      // User has been holding the button down
      // We want to send a button is held message
      longPressed = true;
      if (currentState == 0x01) {
        return BUTTON_F_LONG;
      } else if (currentState == 0x02) {
        return BUTTON_B_LONG;
      } else {
        return BUTTON_BOTH_LONG; // Both being held case
      }
    } else {
      return BUTTON_NONE;
    }
  } else {
    // A change in button state has occurred
    ButtonState retVal = BUTTON_NONE;
    if (currentState) {
      // User has pressed a button down (nothing done on down)
      // If there is a rising edge on one of the buttons from double press we
      // want to mask that out As users are having issues with not release
      // both at once
      previousState |= currentState;
    } else {
      // User has released buttons
      // If they previously had the buttons down we want to check if they were <
      // long hold and trigger a press
      if (!longPressed) {
        // The user didn't hold the button for long
        // So we send button press

        if (previousState == 0x01) {
          retVal = BUTTON_F_SHORT;
        } else if (previousState == 0x02) {
          retVal = BUTTON_B_SHORT;
        } else {
          retVal = BUTTON_BOTH; // Both being held case
        }
      }
      previousState = 0;
      longPressed   = false;
    }
    previousStateChange = xTaskGetTickCount();
    return retVal;
  }
  return BUTTON_NONE;
}

void waitForButtonPress() {
  // we are just lazy and sleep until user confirms button press
  // This also eats the button press event!
  ButtonState buttons = getButtonState();
  while (buttons) {
    buttons = getButtonState();
    GUIDelay();
  }
  while (!buttons) {
    buttons = getButtonState();
    GUIDelay();
  }
}

void waitForButtonPressOrTimeout(TickType_t timeout) {
  timeout += xTaskGetTickCount();
  // calculate the exit point

  ButtonState buttons = getButtonState();
  while (buttons) {
    buttons = getButtonState();
    GUIDelay();
    if (xTaskGetTickCount() > timeout) {
      return;
    }
  }
  while (!buttons) {
    buttons = getButtonState();
    GUIDelay();
    if (xTaskGetTickCount() > timeout) {
      return;
    }
  }
}
