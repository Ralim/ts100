/*
 * OLED.cpp
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include <OLED.hpp>
#include <stdlib.h>
#include <string.h>

// rendering to the buffer
uint8_t *OLED::firstStripPtr; // Pointers to the strips to allow for buffer
// having extra content
uint8_t *OLED::secondStripPtr;   // Pointers to the strips
bool     OLED::inLeftHandedMode; // Whether the screen is in left or not (used for
// offsets in GRAM)
OLED::DisplayState OLED::displayState;
int16_t            OLED::cursor_x, OLED::cursor_y;
bool               OLED::initDone = false;
uint8_t            OLED::displayOffset;
uint8_t            OLED::screenBuffer[16 + (OLED_WIDTH * 2) + 10]; // The data buffer
uint8_t            OLED::secondFrameBuffer[OLED_WIDTH * 2];

/*Setup params for the OLED screen*/
/*http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf*/
/*All commands are prefixed with 0x80*/
/*Data packets are prefixed with 0x40*/
I2C_CLASS::I2C_REG OLED_Setup_Array[] = {
    /**/
    {0x80, 0xAE, 0}, /*Display off*/
    {0x80, 0xD5, 0}, /*Set display clock divide ratio / osc freq*/
    {0x80, 0x52, 0}, /*Divide ratios*/
    {0x80, 0xA8, 0}, /*Set Multiplex Ratio*/
    {0x80, 0x0F, 0}, /*16 == max brightness,39==dimmest*/
    {0x80, 0xC0, 0}, /*Set COM Scan direction*/
    {0x80, 0xD3, 0}, /*Set vertical Display offset*/
    {0x80, 0x00, 0}, /*0 Offset*/
    {0x80, 0x40, 0}, /*Set Display start line to 0*/
    {0x80, 0xA0, 0}, /*Set Segment remap to normal*/
    {0x80, 0x8D, 0}, /*Charge Pump*/
    {0x80, 0x14, 0}, /*Charge Pump settings*/
    {0x80, 0xDA, 0}, /*Set VCOM Pins hardware config*/
    {0x80, 0x02, 0}, /*Combination 2*/
    {0x80, 0x81, 0}, /*Contrast*/
    {0x80, 0x00, 0}, /*^0*/
    {0x80, 0xD9, 0}, /*Set pre-charge period*/
    {0x80, 0xF1, 0}, /*Pre charge period*/
    {0x80, 0xDB, 0}, /*Adjust VCOMH regulator ouput*/
    {0x80, 0x30, 0}, /*VCOM level*/
    {0x80, 0xA4, 0}, /*Enable the display GDDR*/
    {0x80, 0XA6, 0}, /*Normal display*/
    {0x80, 0x20, 0}, /*Memory Mode*/
    {0x80, 0x00, 0}, /*Wrap memory*/
    {0x80, 0xAF, 0}, /*Display on*/
};
// Setup based on the SSD1307 and modified for the SSD1306

const uint8_t REFRESH_COMMANDS[17] = {
    // Set display ON:
    0x80,
    0xAF, // cmd

    // Set column address:
    //  A[6:0] - Column start address = 0x20
    //  B[6:0] - Column end address = 0x7F
    0x80,
    0x21, // cmd
    0x80,
    0x20, // A
    0x80,
    0x7F, // B

    // Set COM output scan direction (normal mode, COM0 to COM[N-1])
    0x80,
    0xC0,

    // Set page address:
    //  A[2:0] - Page start address = 0
    //  B[2:0] - Page end address = 1
    0x80,
    0x22, // cmd
    0x80,
    0x00, // A
    0x80,
    0x01, // B

    // Start of data
    0x40,
};

/*
 * Animation timing function that follows a bezier curve.
 * @param t A given percentage value [0..<100]
 * Returns a new percentage value with ease in and ease out.
 * Original floating point formula: t * t * (3.0f - 2.0f * t);
 */
static uint8_t easeInOutTiming(uint8_t t) { return t * t * (300 - 2 * t) / 10000; }

/*
 * Returns the value between a and b, using a percentage value t.
 * @param a The value associated with 0%
 * @param b The value associated with 100%
 * @param t The percentage [0..<100]
 */
static uint8_t lerp(uint8_t a, uint8_t b, uint8_t t) { return a + t * (b - a) / 100; }

void OLED::initialize() {
  cursor_x = cursor_y = 0;
  inLeftHandedMode    = false;
  firstStripPtr       = &screenBuffer[FRAMEBUFFER_START];
  secondStripPtr      = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];
  displayOffset       = 0;
  memcpy(&screenBuffer[0], &REFRESH_COMMANDS[0], sizeof(REFRESH_COMMANDS));

  // Set the display to be ON once the settings block is sent and send the
  // initialisation data to the OLED.

  for (int tries = 0; tries < 10; tries++) {
    if (I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, OLED_Setup_Array, sizeof(OLED_Setup_Array) / sizeof(OLED_Setup_Array[0]))) {
      return;
    }
  }
  setDisplayState(DisplayState::ON);
  initDone = true;
}
void OLED::setFramebuffer(uint8_t *buffer) {
  if (buffer == NULL) {
    firstStripPtr  = &screenBuffer[FRAMEBUFFER_START];
    secondStripPtr = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];
    return;
  }

  firstStripPtr  = &buffer[0];
  secondStripPtr = &buffer[OLED_WIDTH];
}

/*
 * Prints a char to the screen.
 * UTF font handling is done using the two input chars.
 * Precursor is the command char that is used to select the table.
 */
void OLED::drawChar(const uint16_t charCode, const FontStyle fontStyle) {

  const uint8_t *currentFont;
  static uint8_t fontWidth, fontHeight;
  uint16_t       index;
  switch (fontStyle) {
  case FontStyle::EXTRAS:
    currentFont = ExtraFontChars;
    index       = charCode;
    fontHeight  = 16;
    fontWidth   = 12;
    break;
  case FontStyle::SMALL:
  case FontStyle::LARGE:
  default:
    if (charCode == '\x01' && cursor_y == 0) { // 0x01 is used as new line char
      setCursor(0, 8);
      return;
    } else if (charCode <= 0x01) {
      return;
    }
    currentFont = nullptr;
    index       = 0;
    switch (fontStyle) {
    case FontStyle::SMALL:
      fontHeight = 8;
      fontWidth  = 6;
      break;
    case FontStyle::LARGE:
    default:
      fontHeight = 16;
      fontWidth  = 12;
      break;
    }
    for (uint32_t i = 0; i < FontSectionsCount; i++) {
      const auto &section = FontSections[i];
      if (charCode >= section.symbol_start && charCode < section.symbol_end) {
        currentFont = fontStyle == FontStyle::SMALL ? section.font06_start_ptr : section.font12_start_ptr;
        index       = charCode - section.symbol_start;
        break;
      }
    }
    break;
  }
  const uint8_t *charPointer = currentFont + ((fontWidth * (fontHeight / 8)) * index);
  drawArea(cursor_x, cursor_y, fontWidth, fontHeight, charPointer);
  cursor_x += fontWidth;
}

/*
 * Draws a one pixel wide scrolling indicator. y is the upper vertical position
 * of the indicator in pixels (0..<16).
 */
void OLED::drawScrollIndicator(uint8_t y, uint8_t height) {
  union u_type {
    uint16_t whole;
    uint8_t  strips[2];
  } column;

  column.whole = (1 << height) - 1;
  column.whole <<= y;

  // Draw a one pixel wide bar to the left with a single pixel as
  // the scroll indicator.
  fillArea(OLED_WIDTH - 1, 0, 1, 8, column.strips[0]);
  fillArea(OLED_WIDTH - 1, 8, 1, 8, column.strips[1]);
}

/**
 * Masks (removes) the scrolling indicator, i.e. clears the rightmost column
 * on the screen. This operates directly on the OLED graphics RAM, as this
 * is intended to be used before calling `OLED::transitionScrollDown()`.
 */
void OLED::maskScrollIndicatorOnOLED() {
  // The right-most column depends on the screen rotation, so just take
  // it from the screen buffer which is updated by `OLED::setRotation`.
  uint8_t rightmostColumn = screenBuffer[7];
  uint8_t maskCommands[]  = {
      // Set column address:
      //  A[6:0] - Column start address = rightmost column
      //  B[6:0] - Column end address = rightmost column
      0x80,
      0x21, // cmd
      0x80,
      rightmostColumn, // A
      0x80,
      rightmostColumn, // B

      // Start of data
      0x40,

      // Clears two 8px strips
      0x00,
      0x00,
  };
  I2C_CLASS::Transmit(DEVICEADDR_OLED, maskCommands, sizeof(maskCommands));
}

/**
 * Plays a transition animation between two framebuffers.
 * @param forwardNavigation Direction of the navigation animation.
 *
 * If forward is true, this displays a forward navigation to the second framebuffer contents.
 * Otherwise a rewinding navigation animation is shown to the second framebuffer contents.
 */
void OLED::transitionSecondaryFramebuffer(bool forwardNavigation) {
  uint8_t *firstBackStripPtr  = &secondFrameBuffer[0];
  uint8_t *secondBackStripPtr = &secondFrameBuffer[OLED_WIDTH];

  uint32_t totalDuration = TICKS_100MS * 5; // 500ms
  uint32_t duration      = 0;
  uint32_t start         = xTaskGetTickCount();
  uint8_t  offset        = 0;

  while (duration <= totalDuration) {
    duration         = xTaskGetTickCount() - start;
    uint8_t progress = ((duration * 100) / totalDuration); // Percentage of the period we are through for animation
    progress         = easeInOutTiming(progress);
    progress         = lerp(0, OLED_WIDTH, progress);
    if (progress > OLED_WIDTH) {
      progress = OLED_WIDTH;
    }

    // When forward, current contents move to the left out.
    // Otherwise the contents move to the right out.
    uint8_t oldStart    = forwardNavigation ? 0 : progress;
    uint8_t oldPrevious = forwardNavigation ? progress - offset : offset;

    // Content from the second framebuffer moves in from the right (forward)
    // or from the left (not forward).
    uint8_t newStart = forwardNavigation ? OLED_WIDTH - progress : 0;
    uint8_t newEnd   = forwardNavigation ? 0 : OLED_WIDTH - progress;

    offset = progress;

    memmove(&firstStripPtr[oldStart], &firstStripPtr[oldPrevious], OLED_WIDTH - progress);
    memmove(&secondStripPtr[oldStart], &secondStripPtr[oldPrevious], OLED_WIDTH - progress);

    memmove(&firstStripPtr[newStart], &firstBackStripPtr[newEnd], progress);
    memmove(&secondStripPtr[newStart], &secondBackStripPtr[newEnd], progress);

    refresh();
    osDelay(TICKS_100MS / 5);
  }
}

void OLED::useSecondaryFramebuffer(bool useSecondary) {
  if (useSecondary) {
    setFramebuffer(secondFrameBuffer);
  } else {
    setFramebuffer(NULL);
  }
}
/**
 * Plays a transition animation of scrolling downward. Note this does *not*
 * use the secondary framebuffer.
 *
 * This transition relies on the previous screen data already in the OLED
 * RAM. The caller shall not call `OLED::refresh()` before calling this
 * method, as doing so will overwrite the previous screen data. The caller
 * does not need to call `OLED::refresh()` after this function returns.
 *
 * **This function blocks until the transition has completed.**
 */
void OLED::transitionScrollDown() {
  // We want to draw the updated framebuffer to the next page downward.
  uint8_t const pageStart = screenBuffer[13];
  uint8_t const nextPage  = (pageStart + 2) % 8;
  // Change page start address:
  screenBuffer[13] = nextPage;
  // Change page end address:
  screenBuffer[15] = nextPage + 1;

  refresh();
  osDelay(TICKS_100MS / 5);

  uint8_t const startLine = pageStart * 8 + 1;
  uint8_t const scrollTo  = (pageStart + 2) * 8;

  // Scroll the screen by changing display start line.
  for (uint8_t current = startLine; current <= scrollTo; current++) {
    // Set display start line (0x40~0x7F):
    //  X[5:0] - display start line value
    uint8_t scrollCommandByte = 0b01000000 | (current & 0b00111111);

    // Also update setup command for "set display start line":
    OLED_Setup_Array[8].val = scrollCommandByte;

    I2C_CLASS::I2C_RegisterWrite(DEVICEADDR_OLED, 0x80, scrollCommandByte);
    osDelay(TICKS_100MS / 5);
  }
}

void OLED::setRotation(bool leftHanded) {
#ifdef OLED_FLIP
  leftHanded = !leftHanded;
#endif
  if (inLeftHandedMode == leftHanded) {
    return;
  }

  // send command struct again with changes
  if (leftHanded) {
    OLED_Setup_Array[5].val = 0xC8; // c1?
    OLED_Setup_Array[9].val = 0xA1;
  } else {
    OLED_Setup_Array[5].val = 0xC0;
    OLED_Setup_Array[9].val = 0xA0;
  }
  I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, OLED_Setup_Array, sizeof(OLED_Setup_Array) / sizeof(OLED_Setup_Array[0]));

  inLeftHandedMode = leftHanded;

  screenBuffer[5] = inLeftHandedMode ? 0 : 32;    // display is shifted by 32 in left handed
                                                  // mode as driver ram is 128 wide
  screenBuffer[7] = inLeftHandedMode ? 95 : 0x7F; // End address of the ram segment we are writing to (96 wide)
  screenBuffer[9] = inLeftHandedMode ? 0xC8 : 0xC0;
}

void OLED::setContrast(uint8_t contrast) {
  OLED_Setup_Array[15].val = contrast;
  I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, &OLED_Setup_Array[14], 2);
}

void OLED::setInverseDisplay(bool inverse) {
  uint8_t normalInverseCmd = inverse ? 0xA7 : 0xA6;
  OLED_Setup_Array[21].val = normalInverseCmd;
  I2C_CLASS::I2C_RegisterWrite(DEVICEADDR_OLED, 0x80, normalInverseCmd);
}

// print a string to the current cursor location
void OLED::print(const char *const str, FontStyle fontStyle) {
  const uint8_t *next = reinterpret_cast<const uint8_t *>(str);
  while (next[0]) {
    uint16_t index;
    if (next[0] <= 0xF0) {
      index = next[0];
      next++;
    } else {
      if (!next[1]) {
        return;
      }
      index = (next[0] - 0xF0) * 0xFF - 15 + next[1];
      next += 2;
    }
    drawChar(index, fontStyle);
  }
}

/**
 * Prints a static string message designed to use the whole screen, starting
 * from the top-left corner.
 *
 * If the message starts with a newline (`\\x01`), the string starting from
 * after the newline is printed in the large font. Otherwise, the message
 * is printed in the small font.
 *
 * @param string The string message to be printed
 */
void OLED::printWholeScreen(const char *string) {
  setCursor(0, 0);
  if (string[0] == '\x01') {
    // Empty first line means that this uses large font (for CJK).
    OLED::print(string + 1, FontStyle::LARGE);
  } else {
    OLED::print(string, FontStyle::SMALL);
  }
}

inline void stripLeaderZeros(char *buffer, uint8_t places) {
  // Removing the leading zero's by swapping them to SymbolSpace
  // Stop 1 short so that we dont blank entire number if its zero
  for (int i = 0; i < (places - 1); i++) {
    if (buffer[i] == 2) {
      buffer[i] = SymbolSpace[0];
    } else {
      return;
    }
  }
}
// maximum places is 5
void OLED::printNumber(uint16_t number, uint8_t places, FontStyle fontStyle, bool noLeaderZeros) {
  char buffer[7] = {0};

  if (places >= 5) {
    buffer[5] = 2 + number % 10;
    number /= 10;
  }
  if (places > 4) {
    buffer[4] = 2 + number % 10;
    number /= 10;
  }

  if (places > 3) {
    buffer[3] = 2 + number % 10;
    number /= 10;
  }

  if (places > 2) {
    buffer[2] = 2 + number % 10;
    number /= 10;
  }

  if (places > 1) {
    buffer[1] = 2 + number % 10;
    number /= 10;
  }

  buffer[0] = 2 + number % 10;
  if (noLeaderZeros)
    stripLeaderZeros(buffer, places);
  print(buffer, fontStyle);
}

void OLED::debugNumber(int32_t val, FontStyle fontStyle) {
  if (abs(val) > 99999) {
    OLED::print(SymbolSpace, fontStyle); // out of bounds
    return;
  }
  if (val >= 0) {
    OLED::print(SymbolSpace, fontStyle);
    OLED::printNumber(val, 5, fontStyle);
  } else {
    OLED::print(SymbolMinus, fontStyle);
    OLED::printNumber(-val, 5, fontStyle);
  }
}

void OLED::drawSymbol(uint8_t symbolID) {
  // draw a symbol to the current cursor location
  drawChar(symbolID, FontStyle::EXTRAS);
}

// Draw an area, but y must be aligned on 0/8 offset
void OLED::drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide)
    return; // cutoffleft
  if (x > 96)
    return; // cutoff right

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > 96) {
    visibleEnd = 96 - x;
  }

  if (y == 0) {
    // Splat first line of data
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      firstStripPtr[xx + x] = ptr[xx];
    }
  }
  if (y == 8 || height == 16) {
    // Splat the second line
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      secondStripPtr[x + xx] = ptr[xx + (height == 16 ? wide : 0)];
    }
  }
}

// Draw an area, but y must be aligned on 0/8 offset
// For data which has octets swapped in a 16-bit word.
void OLED::drawAreaSwapped(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide)
    return; // cutoffleft
  if (x > 96)
    return; // cutoff right

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > 96) {
    visibleEnd = 96 - x;
  }

  if (y == 0) {
    // Splat first line of data
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx += 2) {
      firstStripPtr[xx + x]     = ptr[xx + 1];
      firstStripPtr[xx + x + 1] = ptr[xx];
    }
  }
  if (y == 8 || height == 16) {
    // Splat the second line
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx += 2) {
      secondStripPtr[x + xx]     = ptr[xx + 1 + (height == 16 ? wide : 0)];
      secondStripPtr[x + xx + 1] = ptr[xx + (height == 16 ? wide : 0)];
    }
  }
}

void OLED::fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide)
    return; // cutoffleft
  if (x > 96)
    return; // cutoff right

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > 96) {
    visibleEnd = 96 - x;
  }

  if (y == 0) {
    // Splat first line of data
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      firstStripPtr[xx + x] = value;
    }
  }
  if (y == 8 || height == 16) {
    // Splat the second line
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      secondStripPtr[x + xx] = value;
    }
  }
}

void OLED::drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear) {
  // Draw this in 3 sections
  // This is basically a N wide version of vertical line

  // Step 1 : Draw in the top few pixels that are not /8 aligned
  // LSB is at the top of the screen
  uint8_t mask = 0xFF;
  if (y0) {
    mask = mask << (y0 % 8);
    for (uint8_t col = x0; col < x1; col++)
      if (clear)
        firstStripPtr[(y0 / 8) * 96 + col] &= ~mask;
      else
        firstStripPtr[(y0 / 8) * 96 + col] |= mask;
  }
  // Next loop down the line the total number of solids
  if (y0 / 8 != y1 / 8)
    for (uint8_t col = x0; col < x1; col++)
      for (uint8_t r = (y0 / 8); r < (y1 / 8); r++) {
        // This gives us the row index r
        if (clear)
          firstStripPtr[(r * 96) + col] = 0;
        else
          firstStripPtr[(r * 96) + col] = 0xFF;
      }

  // Finally draw the tail
  mask = ~(mask << (y1 % 8));
  for (uint8_t col = x0; col < x1; col++)
    if (clear)
      firstStripPtr[(y1 / 8) * 96 + col] &= ~mask;
    else
      firstStripPtr[(y1 / 8) * 96 + col] |= mask;
}

void OLED::drawHeatSymbol(uint8_t state) {
  // Draw symbol 14
  // Then draw over it, the bottom 5 pixels always stay. 8 pixels above that are
  // the levels masks the symbol nicely
  state /= 31; // 0-> 8 range
  // Then we want to draw down (16-(5+state)
  uint8_t cursor_x_temp = cursor_x;
  drawSymbol(14);
  drawFilledRect(cursor_x_temp, 0, cursor_x_temp + 12, 2 + (8 - state), true);
}

bool OLED::isInitDone() { return initDone; }
