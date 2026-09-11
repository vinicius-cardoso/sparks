# Bill of materials

`bom.csv` is the working BOM. Columns:

- **status** — `have` / `to buy` / `to make` / `conditional` / `phase 2`
- **notes** — anything that would otherwise be lost between sessions

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
