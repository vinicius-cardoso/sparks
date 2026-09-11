# Sparks

A handheld Morse code trainer and radio transmitter.

Press the key. The device times your dots and dashes, walks the Morse tree, and
lights the letter you just formed on a 26-LED panel shaped like the tree itself.
Optionally, it keys a CC1101 and sends the same characters over the air.

The panel is the point. Instead of showing you a decoded letter as text, Sparks
shows you *where that letter lives* in the Morse tree — so the structure of the
code becomes something you read off the board rather than something you memorize
from a table.

## The LED panel

The 26 LEDs are not an arbitrary count — they are the International Morse tree for
the English alphabet, one LED per letter. All **white**, so the panel lights
uniformly.

The dot/dash distinction is carried by the **engraved panel**, not by LED colour:
the arm joining each letter to its parent is etched as a **short mark for a dot**
or a **long bar for a dash**, exactly as in `media/morse_code_guide.png`. The code
for any letter is therefore readable by tracing its path from the centre — and
stays readable with the device switched off.

Layout also follows that image: **E** and **T** sit at the centre, **dots branch
right**, **dashes branch left**, and each deeper level branches perpendicular from
its parent.

```
level 1:  E  T                                     2 LEDs
level 2:  I  A  N  M                               4 LEDs
level 3:  S  U  R  W  D  K  G  O                   8 LEDs
level 4:  H  V  F  ·  L  ·  P  J  B  X  C  Y  Z  Q  ·  ·   12 LEDs
                                                  ──────────
                                                   26 LEDs
```

A full four-level tree has 30 slots. The four marked `·` are the non-English
letters (`..--` Ü, `.-.-` Ä, `---.` Ö, `----` CH) and are left empty — deliberate
gaps in the engraved panel, not mistakes.

## Hardware

| Part | Role |
|---|---|
| ESP32-C3 Supermini | the brain — timing, decoding, radio, everything |
| MAX7219 | LED driver — constant current, 3 pins, no series resistors |
| 26 SMD LEDs | the tree panel — all white |
| CC1101 | 433 MHz transceiver (phase 2) |
| Momentary key | Morse input |
| Buzzer | sidetone, so you hear what you send |
| Li-ion cell + TP4056 | power and charging |

The MAX7219 is **not** programmable — it has no CPU. It holds which LEDs are on
and feeds them regulated current; all decisions happen on the ESP32-C3. It exists
so the panel costs 3 GPIOs instead of 26, and so the board needs **one** current-
setting resistor instead of 26 series resistors.

### Pin budget — 11 of 13 GPIOs

| Function | Pins |
|---|---|
| MAX7219 (DIN, CLK, LOAD) | 3 |
| CC1101 (SCK, MISO, MOSI, CS, GDO0) | 5 |
| Morse key | 1 |
| Buzzer | 1 |
| Battery sense (ADC) | 1 |
| **Total** | **11 — 2 spare** |

See `docs/hardware.md` for the wiring detail and the LED addressing map.

> [!NOTE]
> The MAX7219 is a 5 V part and wants ~3.5 V for a logic high. Driving it from a
> 3.3 V ESP32-C3 often works but is marginal. See `docs/hardware.md` for the two
> fixes (level shifter, or run the chip at ~4 V).

## Build phases

1. **Trainer** — LEDs + key + buzzer. No radio. Fully useful on its own, and
   nothing about it raises regulatory questions.
2. **Radio** — populate the CC1101 once the band and power limits are settled.

The CC1101 footprint is on the board from the start; phase 1 simply leaves it
unpopulated.

## Repository layout

```text
.
├── bom/          # bill of materials
├── docs/         # design decisions, hardware notes
├── firmware/     # Arduino sketch (firmware/sparks/sparks.ino)
├── hardware/     # KiCad project, datasheets
├── mechanical/   # Onshape exports, case STEP/STL
└── media/        # reference images, photos
```

## Toolchain

- **KiCad** for the PCB (`hardware/kicad/`)
- **Onshape** for the case, exported to `mechanical/`
- **Fiber laser** to engrave the copper
- **Arduino** for firmware (`firmware/sparks/`)

Keep editable sources as the source of truth: `.kicad_pro`, `.kicad_sch`,
`.kicad_pcb`, `.ino`, native CAD. Generated manufacturing output belongs in
`exports/` directories.

## Status

Early. The existing sketch (`firmware/sparks/sparks.ino`) is a CC1101 bench test
that sends a hardcoded "HELLO" — it predates the panel design and does not yet
decode input or drive the MAX7219.

## Licence

GPL-3.0 — see `LICENSE`.
