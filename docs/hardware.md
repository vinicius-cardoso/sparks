# Hardware notes

## Block diagram

```
   [KEY] ──────▶ ┌──────────────┐ ──SPI──▶ ┌─────────┐ ──▶ 26 LEDs
                 │  ESP32-C3    │          │ MAX7219 │     (tree panel)
   [BUZZER] ◀─── │  Supermini   │          └─────────┘
                 │              │               │
   [CC1101] ◀──▶ │              │            [RSET]
                 └──────────────┘
                        │
                 [battery sense]
```

The ESP32-C3 does all decision-making: key timing, dot/dash classification, tree
walking, letter identification, gap detection, radio keying, sidetone. The MAX7219
only holds LED state and regulates current — it has no CPU and cannot be
programmed.

## Pin assignment (ESP32-C3 Supermini)

The Supermini breaks out GPIO 0–10, 20, 21 — 13 usable pins.

| GPIO | Function | Notes |
|---|---|---|
| 2 | CC1101 MOSI | |
| 3 | CC1101 SCK | |
| 4 | CC1101 MISO | |
| 1 | CC1101 CS | |
| 0 | CC1101 GDO0 | |
| 5 | MAX7219 DIN | |
| 6 | MAX7219 CLK | |
| 7 | MAX7219 LOAD/CS | |
| 10 | Morse key | `INPUT_PULLUP`, active low |
| 20 | Buzzer | sidetone |
| 21 | Battery sense | ADC via divider |

**Total: 11 of 13 — 2 spare (GPIO8, GPIO9).**

GPIO8/GPIO9 are the strapping pins and the I²C defaults. They are deliberately
left free: they are the least convenient pins to commit, and keeping them open
leaves room for an I²C peripheral later.

Pin numbers above are a starting proposal — adjust during KiCad layout if routing
prefers a different assignment. Only the CC1101 SPI grouping is semi-fixed.

## MAX7219 logic level — unresolved

The MAX7219 runs at 5 V and specifies `V_IH` ≈ 3.5 V. An ESP32-C3 drives 3.3 V,
which is **below spec**. It frequently works, but it is not a designed-in margin.

Two fixes, both cheap:

1. **Level shifter** on DIN / CLK / LOAD (3 lines, one small part). Keeps the chip
   at 5 V for full LED headroom.
2. **Run the MAX7219 at ~4 V**, lowering `V_IH` to ~2.8 V so 3.3 V logic is
   comfortably valid. Costs some LED drive headroom.

Option 2 is fewer parts; option 1 is more robust. Decide with the board in hand.

The **IS31FL3731** avoids the question entirely (native 3.3 V, I²C, constant
current, same part count) and is the fallback if the MAX7219 proves troublesome.

## LED addressing map

All LEDs are white; the **Final** column records each letter's last symbol, which
is what the engraved arm must show ( · short mark / ▬ long bar ).

The MAX7219 lights whatever LED sits between a SEG and a DIG pin. Physical
position on the panel is **irrelevant** to the chip — the mapping below is
electrical only, and the panel layout follows `media/morse_code_guide.png`.

Rows are grouped by tree level, which keeps the wiring comprehensible:

| Letter | Morse | Final | Level | DIG | SEG |
|---|---|---|---|---|---|
| E | `.` | dot · | 1 | 0 | 0 |
| T | `-` | dash ▬ | 1 | 0 | 1 |
| I | `..` | dot · | 2 | 1 | 0 |
| A | `.-` | dash ▬ | 2 | 1 | 1 |
| N | `-.` | dot · | 2 | 1 | 2 |
| M | `--` | dash ▬ | 2 | 1 | 3 |
| S | `...` | dot · | 3 | 2 | 0 |
| U | `..-` | dash ▬ | 3 | 2 | 1 |
| R | `.-.` | dot · | 3 | 2 | 2 |
| W | `.--` | dash ▬ | 3 | 2 | 3 |
| D | `-..` | dot · | 3 | 2 | 4 |
| K | `-.-` | dash ▬ | 3 | 2 | 5 |
| G | `--.` | dot · | 3 | 2 | 6 |
| O | `---` | dash ▬ | 3 | 2 | 7 |
| H | `....` | dot · | 4 | 3 | 0 |
| V | `...-` | dash ▬ | 4 | 3 | 1 |
| F | `..-.` | dot · | 4 | 3 | 2 |
| L | `.-..` | dot · | 4 | 3 | 4 |
| P | `.--.` | dot · | 4 | 3 | 6 |
| J | `.---` | dash ▬ | 4 | 3 | 7 |
| B | `-...` | dot · | 4 | 4 | 0 |
| X | `-..-` | dash ▬ | 4 | 4 | 1 |
| C | `-.-.` | dot · | 4 | 4 | 2 |
| Y | `-.--` | dash ▬ | 4 | 4 | 3 |
| Z | `--..` | dot · | 4 | 4 | 4 |
| Q | `--.-` | dash ▬ | 4 | 4 | 5 |

Uses DIG0–DIG4 (5 of 8) and SEG0–SEG7. Gaps at DIG3/SEG3, DIG3/SEG5, DIG4/SEG6,
DIG4/SEG7 are the four non-English letters — left unpopulated.

Three DIG lines remain free, so status LEDs (power, TX, charge) can be added later
with no driver change.

## Panel layout and engraving

From `media/morse_code_guide.png`:

- **E** and **T** at the centre, flanking the root.
- **Dots branch right**, **dashes branch left**.
- Each deeper level branches perpendicular from its parent, producing the
  cross/grid arrangement rather than a conventional top-down tree.
- Engraved arms connect parent to child, so the path spelling each letter is
  physically traceable on the panel.

### The arms carry the dot/dash information

Since the LEDs are all white, the engraving is now the **only** thing encoding
dot vs dash — so it has to be unambiguous:

- a **dot** arm is engraved as a short mark ( · )
- a **dash** arm is engraved as a long bar ( ▬ )

This matches the reference image, where the arms are visibly different lengths.
Make the long bar at least 3× the short mark, mirroring Morse timing itself, so
the difference is obvious at a glance rather than something to measure.

Each letter is also labelled. Tracing from the centre outward and reading the arms
gives the letter's code directly — and unlike LED colour, it stays readable with
the device switched off, which makes the panel useful as a static reference chart.

## CC1101 module and antenna

The module on hand is the **V2.0 SMA variant**, marked `433M`, with a gold SMA
female socket on the board edge. It ships with a short black whip antenna carrying
a male SMA plug, which **screws onto** that socket.

The antenna is therefore **detachable, not built in** — the two simply come in the
same bag. Nothing to buy, but the mechanical design has to account for it:

- The SMA socket sticks out from the module's edge and is rigid. It sets a hard
  constraint on where the CC1101 can sit inside the case.
- The case needs either an opening for the whip, or an internal antenna position
  with the whip removed. Antennas do not work well inside a sealed enclosure,
  especially near a copper ground plane.
- A panel-mount SMA extension (pigtail) is the usual fix: socket on the case wall,
  short coax to the module. Adds a part but frees the module's placement.
- The whip is removable, so it can be unscrewed for storage — worth designing for
  if the device is meant to be pocketable.

**Never key the transmitter with the antenna unscrewed.** With nothing to radiate
into, power reflects back into the PA and can damage it. For bench testing without
emitting, use a 50 Ω dummy load in place of the whip.

## LEDs — all white

All 26 LEDs are white. This was a deliberate simplification: a single colour means
a single Vf and a single efficiency, so the one `RSET` the MAX7219 allows suits
every LED and the panel lights uniformly.

(The earlier design used 14 red + 12 green to encode each letter's final symbol.
That mapping is still *true* — see `docs/decisions.md` §1 for why the count is 26
— but it is now expressed by the engraved panel rather than by LED colour.)

**Sourcing:** buy all 26 from a single reel or batch. Bin-to-bin variation within
one colour is small but visible on a panel where LEDs sit side by side.

> [!IMPORTANT]
> White LEDs have a **high forward voltage: ~2.8–3.4 V**, considerably more than a
> red LED's ~2.0 V. This bears directly on the MAX7219 supply question in
> *MAX7219 logic level* above — the chip needs headroom above Vf to drive current
> properly. It argues for running the MAX7219 at **5 V** rather than the ~4 V
> option, which in turn makes the level shifter on DIN/CLK/LOAD the likely choice.
> Verify on the breadboard before committing to either.

## Power budget

Estimated worst case. The MAX7219 multiplexes one digit row at a time, so the
instantaneous draw is set by the **largest row**, not by all 26 LEDs at once.

| Digit row | LEDs | Peak @20 mA/LED |
|---|---|---|
| DIG0 | 2 | 40 mA |
| DIG1 | 4 | 80 mA |
| DIG2 | 8 | **160 mA** ← worst row |
| DIG3 | 6 | 120 mA |
| DIG4 | 6 | 120 mA |

- **Worst instantaneous:** ~160 mA (DIG2, the level-3 row — all 8 letters)
- **Average panel draw, everything "lit":** ~104 mA at 20% duty per row

Adding the rest:

| Load | Draw |
|---|---|
| Panel (average) | ~104 mA |
| ESP32-C3 active | ~30 mA |
| MAX7219 quiescent | ~5 mA |
| CC1101 TX @ 10 dBm | ~35 mA |
| Buzzer | ~20 mA |

- **Trainer mode (no radio): ~139 mA**
- **TX mode (worst case): ~194 mA**

### Battery sizing

| Cell | Trainer | TX |
|---|---|---|
| 500 mAh pouch | 3.6 h | 2.6 h |
| 1000 mAh pouch | 7.2 h | 5.2 h |
| 18650 2000 mAh | 14.4 h | 10.3 h |
| 18650 3000 mAh | 21.6 h | 15.5 h |

These are pessimistic — they assume the panel is fully lit continuously, which it
never is. In real use only a few LEDs light at a time and the device sleeps
between key presses, so expect substantially better. **Any cell ≥1000 mAh is
comfortable**; pick on physical size for the case rather than on capacity.

Lowering `RSET` to run the LEDs at 10 mA instead of 20 mA roughly halves the panel
draw and is likely still plenty bright indoors.

## Power

- Li-ion cell (18650 or pouch) + TP4056 charge module with protection.
- ESP32-C3 Supermini has an onboard regulator and accepts battery voltage at 5V/VIN.
- MAX7219 supply depends on the logic-level decision above.
- Battery sense: resistor divider into an ADC pin. Size it so the divider current
  is negligible, and consider a MOSFET to disconnect it during sleep.

## Fabrication

Copper engraved with a fiber laser. Relevant consequences:

- Trace count matters more than usual — every trace is engraving time and a
  potential defect. The MAX7219 keeps this to ~16 panel traces.
- Check minimum trace/space achievable on the laser before committing to the
  MAX7219's 24-pin package pitch (DIP is far more forgiving than SOIC here).
