#include <SPI.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// OLED
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ============================================================
// BUTTON
// ============================================================

#define BUTTON_PIN 10

// ============================================================
// CC1101
// ============================================================

#define CC1101_SCK   3
#define CC1101_MISO  4
#define CC1101_MOSI  2
#define CC1101_CS    1
#define CC1101_GDO0  0
#define CC1101_GDO2  7

SPIClass radioSPI(FSPI);

CC1101 radio = new Module(
  CC1101_CS,
  CC1101_GDO0,
  RADIOLIB_NC,
  CC1101_GDO2,
  radioSPI,
  SPISettings(
    2000000,
    MSBFIRST,
    SPI_MODE0
  )
);

// ============================================================
// OLED MESSAGE
// ============================================================

void showMessage(const char* line1, const char* line2) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(line1);

  display.setCursor(0, 20);
  display.println(line2);

  display.display();
}

// ============================================================
// SEND ONE FSK TONE
// ============================================================
//
// 0xAA = 10101010...
//
// With 2-FSK at 4.8 kbps this produces an alternating
// frequency pattern which SDR++ NFM turns into an audible tone.
//
// 180 bytes:
// 180 * 8 / 4800 = ~300 ms
//
// ============================================================

void sendTone(int durationMs) {
  uint8_t pattern[180];

  for (int i = 0; i < sizeof(pattern); i++) {
    pattern[i] = 0xAA;
  }

  int state = radio.transmit(
    pattern,
    sizeof(pattern)
  );

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[CC1101] Tone TX failed, code ");
    Serial.println(state);
  }

  delay(durationMs);
}

// ============================================================
// SEND MORSE HELLO
// ============================================================
//
// H = ....
// E = .
// L = .-..
// L = .-..
// O = ---
//
// ============================================================

void sendMorseHello() {

  Serial.println();
  Serial.println("[CC1101] Sending Morse: HELLO");

  showMessage(
    "CC1101",
    "Sending HELLO..."
  );

  // Morse timing in milliseconds
  const int DOT = 100;
  const int DASH = 300;

  const int SYMBOL_GAP = 100;
  const int LETTER_GAP = 300;

  // ----------------------------------------------------------
  // H = ....
  // ----------------------------------------------------------

  Serial.println("[MORSE] H = ....");

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(LETTER_GAP);


  // ----------------------------------------------------------
  // E = .
  // ----------------------------------------------------------

  Serial.println("[MORSE] E = .");

  sendTone(DOT);
  delay(LETTER_GAP);


  // ----------------------------------------------------------
  // L = .-..
  // ----------------------------------------------------------

  Serial.println("[MORSE] L = .-..");

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DASH);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(LETTER_GAP);


  // ----------------------------------------------------------
  // L = .-..
  // ----------------------------------------------------------

  Serial.println("[MORSE] L = .-..");

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DASH);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(SYMBOL_GAP);

  sendTone(DOT);
  delay(LETTER_GAP);


  // ----------------------------------------------------------
  // O = ---
  // ----------------------------------------------------------

  Serial.println("[MORSE] O = ---");

  sendTone(DASH);
  delay(SYMBOL_GAP);

  sendTone(DASH);
  delay(SYMBOL_GAP);

  sendTone(DASH);


  // ----------------------------------------------------------
  // DONE
  // ----------------------------------------------------------

  Serial.println("[CC1101] Morse HELLO transmission complete!");

  showMessage(
    "CC1101",
    "HELLO sent!"
  );
}

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" ESP32-C3 CC1101 Morse Test");
  Serial.println("================================");

  // ----------------------------------------------------------
  // Button
  // ----------------------------------------------------------

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  // ----------------------------------------------------------
  // OLED
  // ----------------------------------------------------------

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("[OLED] Initialization failed!");

  } else {

    Serial.println("[OLED] Initialization successful!");

    showMessage(
      "CC1101 433.92MHz",
      "Press button"
    );
  }

  // ----------------------------------------------------------
  // CC1101 SPI
  // ----------------------------------------------------------

  Serial.println("[CC1101] Starting SPI...");

  radioSPI.begin(
    CC1101_SCK,
    CC1101_MISO,
    CC1101_MOSI,
    CC1101_CS
  );

  // ----------------------------------------------------------
  // CC1101 initialization
  // ----------------------------------------------------------

  Serial.println("[CC1101] Initializing...");

  int state = radio.begin(
    433.92,   // Frequency MHz
    4.8,      // Bit rate kbps
    47.6,     // Frequency deviation kHz
    135.0,    // RX bandwidth kHz
    10        // TX power dBm
  );

  if (state == RADIOLIB_ERR_NONE) {

    Serial.println(
      "[CC1101] Initialization successful!"
    );

  } else {

    Serial.print(
      "[CC1101] Initialization failed, code "
    );

    Serial.println(state);

    showMessage(
      "CC1101 ERROR",
      "Initialization"
    );

    while (true) {
      delay(1000);
    }
  }

  Serial.println();
  Serial.println("Ready!");
  Serial.println("Press the button to transmit HELLO.");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  // Button pressed
  if (digitalRead(BUTTON_PIN) == LOW) {

    // Transmit HELLO
    sendMorseHello();

    // Wait until button is released
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }

    // Debounce
    delay(200);

    // Return to idle screen
    showMessage(
      "CC1101 433.92MHz",
      "Press button"
    );
  }
}