// ============================================================
// panel.h — the 26-LED Morse tree panel
// ============================================================
//
// Wraps the MAX7219. The chip is addressed as 5 digit rows of
// 8 segments; which physical LED that lights is decided by the
// PCB, not here (see docs/hardware.md).
//
// The MAX7219 need not be present. With PANEL_SIMULATE set,
// every panel operation is drawn to the serial monitor instead,
// so the whole firmware can be developed and tested before the
// chip arrives. Nothing else in the codebase changes.
//
// ============================================================

#ifndef PANEL_H
#define PANEL_H

#include <Arduino.h>
#include "morse_tree.h"

// ------------------------------------------------------------
// Set to 1 to run without a MAX7219 (prints the panel to serial)
// Set to 0 once the chip is wired up
// ------------------------------------------------------------
#ifndef PANEL_SIMULATE
#define PANEL_SIMULATE 1
#endif

// Pins — see docs/hardware.md
#define PANEL_PIN_DIN   5
#define PANEL_PIN_CLK   6
#define PANEL_PIN_LOAD  7

// The panel uses 5 of the 8 digit rows
#define PANEL_DIGITS 5

// Brightness, 0..15 (MAX7219 intensity register)
#define PANEL_BRIGHTNESS_DEFAULT 7

class Panel {
 public:
  void begin();

  // Clear every LED (buffer only — call show() to push).
  void clear();

  // Light / unlight the LED belonging to a tree node.
  // Nodes with no LED (the root, the four empty slots) are ignored.
  void setNode(uint8_t node, bool on);

  // Light exactly one node and nothing else.
  void showNodeOnly(uint8_t node);

  // Light the whole path from the root down to `node`, so the
  // letter being formed is visible as a trail rather than a
  // single point. This is the mode that teaches.
  void showPath(uint8_t node);

  // Push the buffer to the hardware (or to serial when simulating).
  void show();

  void setBrightness(uint8_t value);   // 0..15

  // Light every LED briefly — useful to check for dead pixels
  // and bad joints after assembly.
  void selfTest();

 private:
  uint8_t buffer_[PANEL_DIGITS];       // one bitmask per digit row
  uint8_t brightness_ = PANEL_BRIGHTNESS_DEFAULT;

  void sendRaw_(uint8_t reg, uint8_t data);
  void renderSerial_();                // simulation output
};

extern Panel panel;

#endif  // PANEL_H
