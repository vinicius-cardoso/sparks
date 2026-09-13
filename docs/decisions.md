# Design decisions

A record of what was chosen, what was rejected, and why. Written so the reasoning
survives — if a decision gets revisited, the tradeoff is here rather than being
re-derived from scratch.

## 1. The 26 LEDs are the Morse tree

**Decision:** one LED per letter, arranged as the dichotomic Morse tree from
`media/morse_code_guide.png`. **All LEDs white** (revised — see §1a).

A full four-level tree holds 30 letters; the 4 unused slots (`..--`, `.-.-`,
`---.`, `----`) are non-English letters and stay empty on the panel.

### Where the number 26 came from

The original specification was 14 red + 12 green, and those counts turned out to
be exact rather than arbitrary. Colouring each letter by its **final** symbol
splits the English alphabet precisely:

- dot-final, 14: `B C D E F G H I L N P R S Z`
- dash-final, 12: `A J K M O Q T U V W X Y`

This is what identified the panel as the Morse tree in the first place, and it
still explains why there are 26 LEDs. The relationship is real; only its *visual
expression* changed in §1a.

**Why the shape matters:** the panel's arrangement is the teaching mechanism. Any
option that forced a different physical layout was rejected on that basis alone.

## 1a. Revised: all LEDs white, dot/dash engraved instead

**Decision:** all 26 LEDs white; the dot/dash distinction moves from LED colour
to the **engraved arms** — short mark for a dot, long bar for a dash.

**Why:**

- **One colour, one `RSET`.** The MAX7219 allows a single current-setting resistor
  for all LEDs. Red (~2.0 V) and green (~2.0–3.2 V) differ in Vf and efficiency, so
  under one `RSET` they would not have matched in brightness — and per-digit
  intensity control could not fix it, since the colours were mixed across rows.
  A single colour eliminates the problem entirely.
- **Simpler sourcing.** One part number, one reel, uniform output.
- **The information is not lost.** Engraving carries it permanently, and unlike LED
  colour it is readable with the device switched off — so the panel doubles as a
  static reference chart.

**Tradeoff accepted:** no at-a-glance colour cue for "this letter ends long". The
engraved arm lengths convey it instead, which requires tracing rather than
glancing.

**Consequence to watch:** white LEDs have a high forward voltage (~2.8–3.4 V),
which pushes toward running the MAX7219 at 5 V for headroom — and therefore toward
needing the level shifter. See `docs/hardware.md`.

## 2. Some LED driver is mandatory — this is arithmetic, not preference

Fixed peripheral cost is 8 pins (CC1101 SPI ×4 + GDO0, key, buzzer, battery ADC).
Against that:

| Board | GPIO | Free after peripherals | Direct 26 LEDs? |
|---|---|---|---|
| ESP32-C3 Supermini | 13 | 5 | no — short 21 |
| Arduino Nano | 20 | 12 | no — short 14 |
| Raspberry Pi Pico | 26 | 18 | no — short 8 |
| ESP32 DevKit V1 | 25 | 17 | no — short 9 |

**No board on hand can drive 26 LEDs directly**, not even the Pico. Switching
boards does not dissolve the constraint, which is why the MCU choice was freed to
be decided on other merits.

## 3. Driver: MAX7219

Ranked by **physical build work** (firmware cost excluded — it's written, not
soldered):

| Option | Parts to place | Joints | Brightness |
|---|---|---|---|
| **MAX7219** | **4** | 82 | full, dimmable |
| Pico charlieplex | 6 | 64 | dim — 3.8% duty |
| 4× 74HC595 | 34 | 176 | full |
| WS2812B | 52 | 156 | full |

**Chosen: MAX7219.** Constant-current drive means **one** `RSET` resistor sets
brightness for all 26 LEDs, eliminating 26 series resistors — 26 fewer parts to
place *and* 26 fewer traces to route and laser-etch.

**Rejected:**

- **74HC595 ×4** — was the initial recommendation while firmware simplicity still
  counted as a cost. Once firmware became free, 34 parts vs 4 ended it. Still a
  reasonable fallback: easier to source, and already stocked locally.
- **Charlieplexing** — 6 pins for 30 LEDs and the fewest parts, but lights one LED
  at a time: 3.8% duty. Matching a static 5 mA LED would need ~130 mA peak against
  an ~8 mA/pin RP2040 spec. For a device whose whole purpose is *being read*, a dim
  panel is disqualifying.
- **WS2812B** — one pin and per-LED colour in software, but abandons the specified
  red/green SMD LEDs, needs a decoupling cap per LED (52 parts), and draws far more
  from the battery.

### Clarification worth keeping

"Matrix" here means the **electrical addressing scheme** (which SEG/DIG pin pair
addresses each LED), *not* a physical square. The MAX7219 exposes 8 SEG + 8 DIG
pins and lights whatever LED sits between a pair — it has no knowledge of where
that LED physically sits. The tree layout is therefore completely unconstrained by
the driver choice.

The off-the-shelf "8×8 MAX7219 module" is a *product built around* the chip, with
LEDs pre-soldered in a grid. Not what is used here — Sparks uses the bare IC.

## 4. Board: ESP32-C3 Supermini

Every candidate fits once a driver is used, so this was decided on other grounds:

- **3.3 V native** — wires directly to the CC1101. The Arduino Nano is 5 V and
  would need level shifting on every SPI line to avoid damaging the radio.
- **Smallest footprint** for a handheld case.
- **USB-C**, good deep sleep for Li-ion.
- The existing sketch already targets it.

**Rejected:** Pico (only compelling for charlieplexing, which lost on brightness;
larger board), ESP32 DevKit V1 (largest, worst idle current), Nano (5 V mismatch).

## 5. No OLED

The existing bench sketch drives an SSD1306, but the 26-LED tree *is* the display.
Dropping it removes a part, a case cutout, and frees GPIO8/GPIO9 — the strapping
pins, which are the least convenient pins to spend anyway.

**Tradeoff accepted:** no text readout, so no WPM, battery percentage, or decoded
letter as an actual character. Reading the tree is the intended skill.

## 6. Radio: trainer first

Phase 1 ships without RF. The CC1101 footprint is on the board but unpopulated,
so the trainer is immediately buildable and usable while band and power limits are
settled separately.

**Open:** the legal position for 433 MHz short-range transmission in Brazil
(ANATEL) has not been researched yet. Do that before populating the radio.

## 7. CC1101 antenna is detachable, not built in

**Finding, not a decision:** the module on hand is the **V2.0 SMA variant** (gold
SMA socket on the board edge, `433M` silkscreen). It ships with a short whip
carrying a male SMA plug, which screws on. The two arrive in the same bag, which
is easy to read as "the antenna is built in" — it is not.

**Consequences:** nothing to buy, but the whip is a rigid protruding part that
constrains where the CC1101 can sit in the case, and antennas work poorly inside a
sealed enclosure next to a copper ground plane. A panel-mount SMA pigtail is the
usual fix. See `docs/hardware.md`.

**Safety:** never key the transmitter with the whip unscrewed — power reflects back
into the PA. Use a 50 Ω dummy load for bench testing.

## 8. The panel needs a double-sided board

**Finding.** Routing the 26-LED matrix on a single layer requires roughly **56
jumper wires**, measured by taking each net's minimum spanning tree and counting
straight-line crossings between nets.

**Why.** The matrix is organised by tree *level* — DIG0 is level 1, DIG3 holds
six of the level-4 letters — while the panel is organised by tree *geometry*,
where those same six letters (J, P, F, L, V, H) sit scattered across four rows on
opposite sides of the board. The electrical grouping and the visual arrangement
are different organisations of the same 26 parts, and they do not reconcile on
one layer.

Total net length is about 823 mm across 13 nets.

**Decision:** route double-sided. DIG nets on the back under the panel, SEG nets
on the front between the LEDs, crossings resolved with vias rather than soldered
jumpers. The MAX7219 mounts on the back, centred beneath the panel, which also
shortens its 16 traces.

**Rejected:** single-sided with jumpers — 25-30 hand-soldered wires even after
optimisation, on the visible face of a handheld device. Rejected on both build
effort and appearance.

**Also rejected:** rearranging the panel so electrical rows match visual rows.
That would route trivially on one layer but abandons the tree shape, which is the
device's entire purpose.

**Consequence for fabrication:** the fiber laser must etch both faces with
registration between them. Add alignment holes to the board outline before
etching.

## Open items

- **MAX7219 at 3.3 V.** The chip is a 5 V part wanting ~3.5 V for a logic high.
  Often works directly from a 3.3 V MCU; not reliable enough to design around. Fix
  with a level shifter on DIN/CLK/LOAD, or run the chip at ~4 V. Decide with the
  real board in hand. The **IS31FL3731** was the considered alternative — natively
  3.3 V, I²C, same part count — but MAX7219 was chosen for availability.
- **ANATEL limits** for 433 MHz (see §6).
- **LED forward voltage** vs MAX7219 headroom — check once the exact LEDs are
  picked, since it sets `RSET`.
