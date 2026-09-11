# Bill of materials

`bom.csv` is the working BOM. Columns:

- **status** — `have` / `to buy` / `to make` / `conditional` / `phase 2`
- **notes** — anything that would otherwise be lost between sessions

## Sourcing: Brazil

Parts are chosen to be buyable locally (Mercado Livre, national distributors)
rather than imported. This is a selection criterion, not an afterthought — a part
that is optimal on paper but import-only costs weeks and customs hassle.

**Consequences already baked into this BOM:**

- **MAX7219 in DIP-24**, not SOIC. The DIP is the common hobby part sold
  everywhere here; the SMD version is the awkward one to find. (DIP was already
  chosen for soldering ease — the two criteria agree.)
- **IS31FL3731 rejected as a fallback.** Technically the better driver, but
  SMD-only and effectively import-only. If the MAX7219 struggles at 3.3 V, add a
  level shifter instead of changing driver.
- **Level shifter named explicitly** — TXS0108E or a BSS138 breakout, both sold
  locally as ready-made modules.

**Watch out for:** the 26 white 0805 LEDs. Buying locally often means loose parts
from mixed bins rather than one reel, and bin-to-bin variation is visible when the
LEDs sit side by side on a panel. Buy 40–50 from a **single seller and lot**, then
pick the 26 most consistent. The spares also cover soldering mishaps.

## Things to resolve before ordering

1. **LED forward voltage.** Sets `RSET` (R1) and therefore panel brightness. Red
   and green differ, and the MAX7219 uses one `RSET` for all — so the two colours
   will not match in brightness by default. Check Vf for both and decide whether
   the mismatch is acceptable or needs compensating.
2. **Total current with all 26 lit.** Sets battery capacity. Not yet estimated.
3. **MAX7219 package.** DIP-24 is far easier to fiber-laser than SOIC-24. Confirm
   the minimum pitch the laser holds before committing.
4. **Logic level.** If the MAX7219 runs at 5 V, the level shifter becomes
   required, not conditional. See `docs/hardware.md`.

## Phase 1 vs phase 2

Phase 1 (trainer) needs everything except U3 (CC1101) and J1 (antenna). The radio
footprint is on the board from the start and simply stays unpopulated, so phase 1
is buildable without settling any regulatory question.
