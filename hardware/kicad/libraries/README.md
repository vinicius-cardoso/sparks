# Local KiCad libraries

Parts not present in the KiCad stock libraries, kept here so the project builds
on any machine without hunting them down again.

## ESP32-C3-SuperMini

| File | What it is |
|---|---|
| `ESP32-C3-SuperMini.kicad_sym` | schematic symbol |
| `ESP32-C3-SuperMini.pretty/ESP32-C3-SuperMini.kicad_mod` | footprint, 2×8 through-hole, 2.54 mm pitch |
| `ESP32-C3-SuperMini.pretty/ESP32-C3-SuperMini.stp` | 3D model |

**Source:** <https://github.com/mrtnvgr/KiCad_ESP32-C3-SuperMini>
(which credits Ulf Hille's GrabCAD model and RabchikEngineer).

> [!IMPORTANT]
> That repository carries **no licence file**, so the material is under default
> copyright — there is no explicit grant to redistribute it. It is vendored here
> for personal use on this project, which is ordinary practice for hobby KiCad
> libraries but is not a licence. If Sparks is ever published as a kit or the
> hardware files are redistributed, either get permission from the author or
> redraw the symbol and footprint from the datasheet.

### Unmodified

These files are exactly as published upstream — no local changes.

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
> Some ESP32-C3 SuperMini pinout diagrams circulating online are **x-mirrored**
> (GPIO0–GPIO4 drawn on the left, GPIO5–GPIO21 on the right). This symbol follows
> the correct orientation. If a reference image disagrees, check the image before
> assuming the symbol is wrong.

## Adding these to a KiCad project

Symbols: **Preferences → Manage Symbol Libraries → Project Specific Libraries**,
add `ESP32-C3-SuperMini.kicad_sym` from this directory.

Footprints: **Preferences → Manage Footprint Libraries → Project Specific
Libraries**, add the `ESP32-C3-SuperMini.pretty` directory.

Use *project specific* rather than global, so the paths travel with the repo.
