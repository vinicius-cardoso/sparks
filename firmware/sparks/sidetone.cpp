#include "sidetone.h"

Sidetone sidetone;

void Sidetone::begin() {
  pinMode(SIDETONE_PIN, OUTPUT);
  digitalWrite(SIDETONE_PIN, LOW);
  sounding_ = false;
}

void Sidetone::on() {
  if (!enabled_ || sounding_) return;
  tone(SIDETONE_PIN, SIDETONE_FREQ_HZ);
  sounding_ = true;
}

void Sidetone::off() {
  if (!sounding_) return;
  noTone(SIDETONE_PIN);
  digitalWrite(SIDETONE_PIN, LOW);
  sounding_ = false;
}

void Sidetone::setEnabled(bool on) {
  enabled_ = on;
  if (!enabled_) off();
}

void Sidetone::blip(uint16_t freqHz, uint16_t ms) {
  if (!enabled_) return;

  bool wasSounding = sounding_;
  off();

  tone(SIDETONE_PIN, freqHz, ms);
  delay(ms);
  noTone(SIDETONE_PIN);
  digitalWrite(SIDETONE_PIN, LOW);

  sounding_ = false;
  if (wasSounding) on();
}
