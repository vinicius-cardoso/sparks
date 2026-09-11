# Local KiCad libraries

Parts not present in the KiCad stock libraries, kept here so the project builds
on any machine without hunting them down again.

| Part | Symbol | Footprint | Source licence |
|---|---|---|---|
| ESP32-C3 SuperMini | ✅ local | ✅ local | none stated |
| TP4056 breakout | ✅ local | ⚠️ local, unverified | none stated |
| CC1101 module | ✅ local (converted) | ❌ use stock header | MIT |

---

## ESP32-C3-SuperMini

| File | What it is |
|---|---|
| `ESP32-C3-SuperMini.kicad_sym` | schematic symbol |
| `ESP32-C3-SuperMini.pretty/ESP32-C3-SuperMini.kicad_mod` | footprint, 2×8 through-hole, 2.54 mm |
| `ESP32-C3-SuperMini.pretty/ESP32-C3-SuperMini.stp` | 3D model |

**Source:** <https://github.com/mrtnvgr/KiCad_ESP32-C3-SuperMini>
(credits Ulf Hille's GrabCAD model and RabchikEngineer).

### Unmodified

Exactly as published upstream — no local changes.

```
        ┌──────────────┐
 GPIO5 ─┤              ├─ GPIO4
 GPIO6 ─┤              ├─ GPIO3
 GPIO7 ─┤   ESP32-C3   ├─ GPIO2
 GPIO8 ─┤   SuperMini  ├─ GPIO1
 GPIO9 ─┤              ├─ GPIO0
GPIO10 ─┤              │
GPIO20 ─┤              │
GPIO21 ─┤              │
        └──────────────┘
         5V/3V3 top, GND bottom
```

> [!NOTE]
> Some ESP32-C3 SuperMini pinout diagrams in circulation are **x-mirrored**
> (GPIO0–GPIO4 drawn on the left). This symbol follows the correct orientation.
> A disagreeing reference image is not evidence the symbol is wrong.

---

## TP4056-Breakout

| File | What it is |
|---|---|
| `TP4056-Breakout.kicad_sym` | 6-pin symbol: IN+ IN− B+ B− OUT+ OUT− |
| `TP4056-Breakout.pretty/TP4056-Breakout.kicad_mod` | through-hole footprint |

**Source:** <https://github.com/kyriosaa/tp4056-breakout>

This is the symbol for the **module on headers**, which is what Sparks uses — not
the bare TP4056 IC. Pin names match the labels silkscreened on the common red
charging boards.

> [!WARNING]
> **The footprint's pad spacing is irregular and has not been verified against a
> real module.** Pads sit at x = −10.85 (2 pads, 14.3 mm apart) and x = +11.15
> (4 pads, spaced 3.0 / 8.3 / 3.0 mm), with 1.2 mm drills. That is plausible for
> a specific board revision but is **not** a standard 2.54 mm grid, so TP4056
> modules from a different seller may not fit. **Measure your actual module
> before committing the PCB to the laser.**
>
> Safer alternative: use a generic `Connector_PinHeader_2.54mm` footprint and wire
> the module with pin headers, which tolerates board-to-board variation.

---

## CC1101-Module

| File | What it is |
|---|---|
| `CC1101-Module.kicad_sym` | 8-pin symbol: VCC GND MOSI SCLK MISO GDO2 GDO0 CSN |

**Source:** <https://github.com/olliiiver/kicad_homeatic_minimal> (MIT), converted
locally from the legacy KiCad 5 `.lib` format to modern `.kicad_sym`.

Pin order matches the V2.0 SMA module used in this project, and matches
`electronics/pinouts/cc1101.png`.

### No footprint here — use a stock pin header

The upstream footprint was **deliberately not installed**: it is a single column
of 8 pads at **2.0 mm pitch**, which belongs to a bare Homematic-style board. The
module Sparks uses has **8 pins in 2 rows of 4 at 2.54 mm**. Using the upstream
footprint would produce a board the module cannot physically plug into.

Use instead:

```
Connector_PinHeader_2.54mm:PinHeader_2x04_P2.54mm_Vertical
```

Assign it to the CC1101 symbol in the footprint field. Confirm the row order
against your own module before routing — pin 1 is marked on the board.

---

## Licensing

> [!IMPORTANT]
> The ESP32-C3 SuperMini and TP4056 repositories carry **no licence file**, so
> that material is under default copyright with no explicit grant to redistribute.
> It is vendored here for personal use on this project — ordinary practice for
> hobby KiCad libraries, but not a licence. The CC1101 symbol derives from an
> MIT-licensed repository and is fine to redistribute with attribution.
>
> If Sparks is ever published as a kit or the hardware files are redistributed,
> either obtain permission for the unlicensed parts or redraw them from the
> datasheets.

## Adding these to a KiCad project

**Symbols:** Preferences → Manage Symbol Libraries → *Project Specific Libraries*
→ add each `.kicad_sym` from this directory.

**Footprints:** Preferences → Manage Footprint Libraries → *Project Specific
Libraries* → add each `.pretty` directory.

Use *Project Specific* rather than Global, so the paths travel with the repo.
