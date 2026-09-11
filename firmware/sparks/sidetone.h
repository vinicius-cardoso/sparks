// ============================================================
// sidetone.h — the buzzer
// ============================================================
//
// You cannot learn Morse silently. The rhythm is the code, and
// the ear learns it long before the eye does — so the buzzer
// follows the key directly, on while it is down, off when it
// is up.
//
// ============================================================

#ifndef SIDETONE_H
#define SIDETONE_H

#include <Arduino.h>

#define SIDETONE_PIN      20
#define SIDETONE_FREQ_HZ  700   // classic CW pitch, easy on the ear

class Sidetone {
 public:
  void begin();

  void on();
  void off();

  void setEnabled(bool on);
  bool enabled() const { return enabled_; }

  // A short blip, used for feedback that is not the key itself
  // (letter committed, mode changed).
  void blip(uint16_t freqHz, uint16_t ms);

 private:
  bool enabled_  = true;
  bool sounding_ = false;
};

extern Sidetone sidetone;

#endif  // SIDETONE_H
