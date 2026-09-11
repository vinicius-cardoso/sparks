// ============================================================
// morse_tree.h — the Morse tree, and where each letter lives
// ============================================================
//
// Decoding is a walk, not a table lookup. Start at the root;
// a dot steps left, a dash steps right. Wherever you stop is
// the letter.
//
// The panel is that same tree made physical, so the LED to
// light while decoding is simply "the node you are standing
// on" — no separate lookup between decode state and display.
//
// ============================================================

#ifndef MORSE_TREE_H
#define MORSE_TREE_H

#include <Arduino.h>

// ------------------------------------------------------------
// Tree geometry
// ------------------------------------------------------------
//
// Nodes are numbered breadth-first, root = 0:
//
//            0 (root, no letter)
//           .   -
//         1       2        <- E T
//        . -     . -
//       3   4   5   6      <- I A N M
//       ...
//
// (edges labelled with the symbol that takes you down them)
//
// For node n:  dot child  = 2n + 1
//              dash child = 2n + 2
//
// Four levels of letters need nodes 1..30.
//
// ------------------------------------------------------------

#define MORSE_ROOT        0
#define MORSE_NODE_COUNT  31   // 0..30
#define MORSE_NO_LETTER   '\0'

// Node -> letter. Index is the node number above.
// '\0' marks the four slots that carry no English letter
// (..-- .-.- ---. ----), plus the root itself.
static const char MORSE_NODE_LETTER[MORSE_NODE_COUNT] = {
  MORSE_NO_LETTER,            // 0  root
  'E', 'T',                   // 1..2    level 1
  'I', 'A', 'N', 'M',         // 3..6    level 2
  'S', 'U', 'R', 'W',         // 7..10   level 3
  'D', 'K', 'G', 'O',         // 11..14
  'H', 'V', 'F', MORSE_NO_LETTER,   // 15..18  level 4
  'L', MORSE_NO_LETTER, 'P', 'J',   // 19..22
  'B', 'X', 'C', 'Y',               // 23..26
  'Z', 'Q', MORSE_NO_LETTER, MORSE_NO_LETTER  // 27..30
};

// ------------------------------------------------------------
// Panel addressing
// ------------------------------------------------------------
//
// Which (DIG, SEG) pair lights the LED for a given node.
// Matches the table in docs/hardware.md.
//
// Physical position on the panel is irrelevant here — the
// MAX7219 drives a pin pair and the PCB decides the geography.
//
// 0xFF = no LED at this node.
//
// ------------------------------------------------------------

#define MORSE_NO_LED 0xFF

struct LedAddr {
  uint8_t dig;
  uint8_t seg;
};

static const LedAddr MORSE_NODE_LED[MORSE_NODE_COUNT] = {
  {MORSE_NO_LED, MORSE_NO_LED},          // 0  root — no LED

  {0, 0}, {0, 1},                        // E T

  {1, 0}, {1, 1}, {1, 2}, {1, 3},        // I A N M

  {2, 0}, {2, 1}, {2, 2}, {2, 3},        // S U R W
  {2, 4}, {2, 5}, {2, 6}, {2, 7},        // D K G O

  {3, 0}, {3, 1}, {3, 2},                // H V F
  {MORSE_NO_LED, MORSE_NO_LED},          //   (..--)
  {3, 4},                                // L
  {MORSE_NO_LED, MORSE_NO_LED},          //   (.-.-)
  {3, 6}, {3, 7},                        // P J

  {4, 0}, {4, 1}, {4, 2}, {4, 3},        // B X C Y
  {4, 4}, {4, 5},                        // Z Q
  {MORSE_NO_LED, MORSE_NO_LED},          //   (---.)
  {MORSE_NO_LED, MORSE_NO_LED}           //   (----)
};

// ------------------------------------------------------------
// Walking the tree
// ------------------------------------------------------------

inline uint8_t morseStepDot(uint8_t node)  { return 2 * node + 1; }
inline uint8_t morseStepDash(uint8_t node) { return 2 * node + 2; }

inline bool morseNodeValid(uint8_t node) {
  return node < MORSE_NODE_COUNT;
}

inline char morseLetterAt(uint8_t node) {
  if (!morseNodeValid(node)) return MORSE_NO_LETTER;
  return MORSE_NODE_LETTER[node];
}

// ------------------------------------------------------------
// Letter -> code, for transmitting and for the serial display
// ------------------------------------------------------------

// Returns the Morse code for an uppercase letter, or nullptr.
inline const char* morseCodeFor(char c) {
  static const char* const CODES[26] = {
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.",
    "--.",  "....", "..",   ".---", "-.-",  ".-..",
    "--",   "-.",   "---",  ".--.", "--.-", ".-.",
    "...",  "-",    "..-",  "...-", ".--",  "-..-",
    "-.--", "--.."
  };
  if (c < 'A' || c > 'Z') return nullptr;
  return CODES[c - 'A'];
}

#endif  // MORSE_TREE_H
