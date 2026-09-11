// ============================================================
// Sparks — a Morse code trainer and radio transmitter
// ============================================================
//
// Press the key. Sparks times what you send, walks the Morse
// tree, and lights the path on the 26-LED panel so you can see
// where the letter lives rather than just being told what it
// was.
//
// Hardware: ESP32-C3 Supermini + MAX7219 + 26 white LEDs.
// See docs/hardware.md for wiring.
//
// ------------------------------------------------------------
// RUNNING WITHOUT HARDWARE
// ------------------------------------------------------------
//
// The MAX7219 is not required. panel.h defaults to
// PANEL_SIMULATE 1, which draws the panel to the serial monitor
// instead of driving the chip. Everything else — key timing,
// decoding, adaptive speed — works identically.
//
// You can also drive it entirely from the serial monitor with
// no key wired: send '.' and '-' for symbols, space for a
// letter gap. Type `help` for the rest.
//
// Set PANEL_SIMULATE to 0 once the chip is on the board.
//
// ============================================================

#include "morse_tree.h"
#include "panel.h"
#include "key.h"
#include "sidetone.h"

// ------------------------------------------------------------
// Decoder state
// ------------------------------------------------------------

static uint8_t currentNode = MORSE_ROOT;
static bool    overrun     = false;   // more than 4 symbols sent

static char    lastLetter  = MORSE_NO_LETTER;
static String  message     = "";

// ------------------------------------------------------------

static void resetLetter() {
  currentNode = MORSE_ROOT;
  overrun     = false;
}

// ------------------------------------------------------------
// A symbol arrived: step down the tree and light the path.
// ------------------------------------------------------------

static void applySymbol(bool isDash) {
  uint8_t next = isDash ? morseStepDash(currentNode)
                        : morseStepDot(currentNode);

  if (!morseNodeValid(next)) {
    // Past the fourth level — no English letter lives here.
    // Keep accepting symbols so the letter gap still commits,
    // but remember that the result is invalid.
    overrun = true;
    Serial.println(F("  (too many symbols for a letter)"));
    return;
  }

  currentNode = next;
  panel.showPath(currentNode);

  Serial.print(F("  "));
  Serial.print(isDash ? '-' : '.');
  Serial.print(F("  -> node "));
  Serial.print(currentNode);

  char c = morseLetterAt(currentNode);
  if (c != MORSE_NO_LETTER) {
    Serial.print(F("  ("));
    Serial.print(c);
    Serial.print(')');
  } else {
    Serial.print(F("  (no letter here)"));
  }
  Serial.println();
}

// ------------------------------------------------------------
// The letter gap closed: commit whatever we landed on.
// ------------------------------------------------------------

static void commitLetter() {
  if (currentNode == MORSE_ROOT) return;   // nothing was sent

  char c = overrun ? MORSE_NO_LETTER : morseLetterAt(currentNode);

  if (c != MORSE_NO_LETTER) {
    lastLetter = c;
    message += c;

    Serial.print(F("LETTER: "));
    Serial.print(c);
    Serial.print(F("   ["));
    Serial.print(morseCodeFor(c));
    Serial.print(F("]   "));
    Serial.print(key.wpm());
    Serial.print(F(" wpm (unit "));
    Serial.print(key.unitMs());
    Serial.println(F(" ms)"));

    // Hold the finished letter lit for a moment so it registers
    // visually before the panel clears.
    panel.showPath(currentNode);
  } else {
    Serial.println(F("LETTER: ? (not a valid Morse letter)"));
    panel.clear();
    panel.show();
  }

  Serial.print(F("MESSAGE: "));
  Serial.println(message);
  Serial.println();

  resetLetter();
}

// ------------------------------------------------------------

static void commitWord() {
  if (message.length() == 0) return;
  if (message.endsWith(" "))  return;

  message += ' ';

  Serial.println(F("--- word gap ---"));
  Serial.print(F("MESSAGE: "));
  Serial.println(message);
  Serial.println();

  panel.clear();
  panel.show();
}

// ------------------------------------------------------------
// Serial commands — lets the whole thing be driven without a
// key or a panel attached.
// ------------------------------------------------------------

static void printHelp() {
  Serial.println();
  Serial.println(F("commands:"));
  Serial.println(F("  .        send a dot"));
  Serial.println(F("  -        send a dash"));
  Serial.println(F("  <space>  letter gap (commit the letter)"));
  Serial.println(F("  /        word gap"));
  Serial.println(F("  clear    clear the message"));
  Serial.println(F("  test     light every LED"));
  Serial.println(F("  wpm N    set speed, fixed (disables adaptive)"));
  Serial.println(F("  auto     re-enable adaptive speed"));
  Serial.println(F("  tone     toggle the buzzer"));
  Serial.println(F("  help     this list"));
  Serial.println();
}

static void handleSerial() {
  static String line = "";

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      line.trim();
      if (line.length() == 0) { line = ""; continue; }

      if (line == "help") {
        printHelp();
      } else if (line == "clear") {
        message = "";
        resetLetter();
        panel.clear();
        panel.show();
        Serial.println(F("message cleared"));
      } else if (line == "test") {
        panel.selfTest();
      } else if (line == "auto") {
        key.setAdaptive(true);
        Serial.println(F("adaptive speed on"));
      } else if (line == "tone") {
        sidetone.setEnabled(!sidetone.enabled());
        Serial.print(F("buzzer "));
        Serial.println(sidetone.enabled() ? F("on") : F("off"));
      } else if (line.startsWith("wpm ")) {
        int w = line.substring(4).toInt();
        if (w >= 3 && w <= 30) {
          key.setAdaptive(false);
          key.setUnitMs(1200 / w);
          Serial.print(F("fixed at "));
          Serial.print(w);
          Serial.println(F(" wpm"));
        } else {
          Serial.println(F("wpm must be 3..30"));
        }
      } else {
        Serial.print(F("unknown: "));
        Serial.println(line);
      }

      line = "";
      continue;
    }

    // Single-character symbol input, acted on immediately.
    if (c == '.') {
      applySymbol(false);
      continue;
    }
    if (c == '-') {
      applySymbol(true);
      continue;
    }
    if (c == ' ') {
      commitLetter();
      continue;
    }
    if (c == '/') {
      commitLetter();
      commitWord();
      continue;
    }

    line += c;
  }
}

// ------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println(F("================================"));
  Serial.println(F(" Sparks — Morse trainer"));
  Serial.println(F("================================"));

#if PANEL_SIMULATE
  Serial.println(F("panel: SIMULATED (no MAX7219 needed)"));
  Serial.println(F("       set PANEL_SIMULATE 0 in panel.h"));
  Serial.println(F("       once the chip is wired"));
#else
  Serial.println(F("panel: MAX7219"));
#endif

  key.begin();
  panel.begin();
  sidetone.begin();

  resetLetter();

  Serial.print(F("speed: "));
  Serial.print(key.wpm());
  Serial.print(F(" wpm (unit "));
  Serial.print(key.unitMs());
  Serial.println(F(" ms), adaptive"));

  printHelp();
  Serial.println(F("ready — press the key, or type . and -"));
  Serial.println();
}

// ------------------------------------------------------------

void loop() {
  handleSerial();

  KeyEvent ev = key.poll();

  // Buzzer tracks the key directly.
  if (key.isDown()) {
    sidetone.on();
  } else {
    sidetone.off();
  }

  switch (ev) {
    case KEY_DOT:        applySymbol(false); break;
    case KEY_DASH:       applySymbol(true);  break;
    case KEY_LETTER_GAP: commitLetter();     break;
    case KEY_WORD_GAP:   commitWord();       break;
    case KEY_NONE:       break;
  }
}
