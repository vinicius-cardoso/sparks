#include "panel.h"

Panel panel;

// ------------------------------------------------------------
// MAX7219 registers
// ------------------------------------------------------------

#define MAX7219_REG_NOOP         0x00
#define MAX7219_REG_DIGIT0       0x01
#define MAX7219_REG_DECODE_MODE  0x09
#define MAX7219_REG_INTENSITY    0x0A
#define MAX7219_REG_SCAN_LIMIT   0x0B
#define MAX7219_REG_SHUTDOWN     0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

// ------------------------------------------------------------

void Panel::begin() {
  clear();

#if !PANEL_SIMULATE
  pinMode(PANEL_PIN_DIN,  OUTPUT);
  pinMode(PANEL_PIN_CLK,  OUTPUT);
  pinMode(PANEL_PIN_LOAD, OUTPUT);

  digitalWrite(PANEL_PIN_CLK,  LOW);
  digitalWrite(PANEL_PIN_LOAD, HIGH);

  // Decode mode off: we drive raw segments, not BCD digits.
  // This is essential — the default would try to render numerals.
  sendRaw_(MAX7219_REG_DECODE_MODE, 0x00);

  // Scan only the rows we actually use. Fewer rows scanned means
  // a higher duty cycle per LED, so a brighter panel.
  sendRaw_(MAX7219_REG_SCAN_LIMIT, PANEL_DIGITS - 1);

  sendRaw_(MAX7219_REG_INTENSITY, brightness_);
  sendRaw_(MAX7219_REG_DISPLAY_TEST, 0x00);
  sendRaw_(MAX7219_REG_SHUTDOWN, 0x01);   // 1 = normal operation
#endif

  show();
}

// ------------------------------------------------------------

void Panel::clear() {
  for (uint8_t i = 0; i < PANEL_DIGITS; i++) {
    buffer_[i] = 0;
  }
}

// ------------------------------------------------------------

void Panel::setNode(uint8_t node, bool on) {
  if (!morseNodeValid(node)) return;

  LedAddr a = MORSE_NODE_LED[node];
  if (a.dig == MORSE_NO_LED) return;      // root, or an empty slot
  if (a.dig >= PANEL_DIGITS) return;

  if (on) {
    buffer_[a.dig] |= (1 << a.seg);
  } else {
    buffer_[a.dig] &= ~(1 << a.seg);
  }
}

// ------------------------------------------------------------

void Panel::showNodeOnly(uint8_t node) {
  clear();
  setNode(node, true);
  show();
}

// ------------------------------------------------------------
// Walk back up to the root, lighting every node on the way.
//
// Parent of node n is (n - 1) / 2, which is why breadth-first
// numbering was worth the small awkwardness in the table.
// ------------------------------------------------------------

void Panel::showPath(uint8_t node) {
  clear();

  uint8_t n = node;
  while (morseNodeValid(n) && n != MORSE_ROOT) {
    setNode(n, true);
    n = (n - 1) / 2;
  }

  show();
}

// ------------------------------------------------------------

void Panel::setBrightness(uint8_t value) {
  if (value > 15) value = 15;
  brightness_ = value;

#if !PANEL_SIMULATE
  sendRaw_(MAX7219_REG_INTENSITY, brightness_);
#endif
}

// ------------------------------------------------------------

void Panel::show() {
#if PANEL_SIMULATE
  renderSerial_();
#else
  for (uint8_t d = 0; d < PANEL_DIGITS; d++) {
    sendRaw_(MAX7219_REG_DIGIT0 + d, buffer_[d]);
  }
#endif
}

// ------------------------------------------------------------

void Panel::selfTest() {
  Serial.println(F("[PANEL] Self test — all LEDs on"));

  for (uint8_t i = 0; i < PANEL_DIGITS; i++) {
    buffer_[i] = 0xFF;
  }
  show();

  delay(1000);

  clear();
  show();
}

// ------------------------------------------------------------
// Bit-banged SPI. The MAX7219 wants 16 bits: address then data,
// MSB first, latched on a rising edge of LOAD.
//
// Not using the SPI peripheral here because the CC1101 owns it,
// and bit-banging three pins costs nothing at this rate.
// ------------------------------------------------------------

void Panel::sendRaw_(uint8_t reg, uint8_t data) {
#if !PANEL_SIMULATE
  digitalWrite(PANEL_PIN_LOAD, LOW);

  shiftOut(PANEL_PIN_DIN, PANEL_PIN_CLK, MSBFIRST, reg);
  shiftOut(PANEL_PIN_DIN, PANEL_PIN_CLK, MSBFIRST, data);

  digitalWrite(PANEL_PIN_LOAD, HIGH);
#else
  (void)reg;
  (void)data;
#endif
}

// ------------------------------------------------------------
// Simulation: draw the tree to the serial monitor.
//
// Laid out like the physical panel — dots to the right, dashes
// to the left — so what you read here matches what the board
// will look like.
// ------------------------------------------------------------

void Panel::renderSerial_() {
#if PANEL_SIMULATE
  auto lit = [&](uint8_t node) -> bool {
    if (!morseNodeValid(node)) return false;
    LedAddr a = MORSE_NODE_LED[node];
    if (a.dig == MORSE_NO_LED || a.dig >= PANEL_DIGITS) return false;
    return buffer_[a.dig] & (1 << a.seg);
  };

  // A lit letter is shown in brackets, an unlit one as a dot.
  auto cell = [&](uint8_t node) {
    char c = morseLetterAt(node);
    if (c == MORSE_NO_LETTER) {
      Serial.print(F("    "));
      return;
    }
    if (lit(node)) {
      Serial.print('[');
      Serial.print(c);
      Serial.print(']');
    } else {
      Serial.print(' ');
      Serial.print(c);
      Serial.print(' ');
    }
    Serial.print(' ');
  };

  Serial.println();
  Serial.println(F("  ---- panel ----"));

  // Level 1: E T
  Serial.print(F("   L1: "));
  cell(1); cell(2);
  Serial.println();

  // Level 2: I A N M
  Serial.print(F("   L2: "));
  for (uint8_t n = 3; n <= 6; n++) cell(n);
  Serial.println();

  // Level 3: S U R W D K G O
  Serial.print(F("   L3: "));
  for (uint8_t n = 7; n <= 14; n++) cell(n);
  Serial.println();

  // Level 4
  Serial.print(F("   L4: "));
  for (uint8_t n = 15; n <= 30; n++) cell(n);
  Serial.println();
  Serial.println();
#endif
}
