#include "ui_drawing.hpp"

#ifdef OLED_96x16
extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_simplified(TemperatureType_t tipTemp) {
   bool tipDisconnected = isTipDisconnected();
   bool isFlipped       = OLED::getRotation();
   bool isReverse       = getSettingValue(SettingsOptions::ReverseButtonNavEnabled);
   // Flip and switch buttons accordingly
   OLED::drawArea(isFlipped ? 54 : 0, 0, 42, 16, isFlipped ? (isReverse ? buttonBF : buttonAF) : (isReverse ? buttonB : buttonA));
   OLED::drawArea(isFlipped ? 12 : 42, 0, 42, 16, isFlipped ? (isReverse ? buttonAF : buttonBF) : (isReverse ? buttonA : buttonB));
 
   if ((tipTemp > 55) || tipDisconnected) {
	   // draw temp over the start soldering button
	   // Location changes on screen rotation
	   // in right handed mode we want to draw over the first part
	   OLED::fillArea(isReverse ? (isFlipped ? 14 : 42) : (isFlipped ? 55 : 0), 0, 41, 16, 0); // clear the area
	   OLED::setCursor(isReverse ? (isFlipped ? 15 : 43) : (isFlipped ? 56 : 0), 0);
	   // If tip is connected draw the temp, otherwise - the notification
	   if (tipDisconnected) {
		   // Draw-in the missing tip symbol
		   if (isReverse) {
			   OLED::drawArea(isFlipped ? 12 : 42, 0, 42, 16, isFlipped ? disconnectedTipF : disconnectedTip);
		   } else {
			   OLED::drawArea(isFlipped ? 54 : 0, 0, 42, 16, isFlipped ? disconnectedTipF : disconnectedTip);
		   }
	   } else if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (xTaskGetTickCount() % 1000 < 300))) {
		   ui_draw_tip_temperature(false, FontStyle::LARGE);  // Draw-in the temp
	   }
   }
   OLED::setCursor(isFlipped ? 0 : 84, 0);
   ui_draw_power_source_icon();
}

#endif