# Roadmap

Ordered so that each step produces something testable, and so the parts that are
still undecided stay off the critical path.

## Phase 0 — settle the unknowns (no hardware needed)

- [ ] Pick the exact white LEDs; record Vf. Buy all 26 from one reel/batch.
      **White Vf is high (~2.8–3.4 V) — check MAX7219 supply headroom.**
- [ ] Compute `RSET` from Vf and target current.
- [ ] Design the engraved arms: short mark = dot, long bar = dash, ≥3:1 ratio.
      With white LEDs this engraving is the *only* dot/dash cue.
- [x] Estimate worst-case current → see `docs/hardware.md` § Power budget.
      Trainer ~139 mA, TX ~194 mA worst case; any cell ≥1000 mAh is comfortable.
- [ ] Decide MAX7219 supply voltage → level shifter required or not.
- [x] MAX7219 package → **DIP-24**, chosen for hand-soldering ease. 2.54 mm pitch
      is within any fiber laser's capability, so the minimum trace/space question
      no longer blocks the design.

## Phase 1 — breadboard the panel

- [ ] MAX7219 + a handful of LEDs on a breadboard, driven by the ESP32-C3.
- [ ] Confirm 3.3 V logic actually clocks the chip reliably (this is the open risk).
- [ ] Firmware: light an arbitrary LED by letter — proves the SEG/DIG map.

## Phase 2 — the trainer firmware

- [ ] Key timing: debounce, classify dot vs dash by duration.
- [ ] Walk the tree as symbols arrive; light the current node live.
- [ ] Letter gap → commit the letter. Word gap → space.
- [ ] Buzzer sidetone keyed with the input.
- [ ] Adaptive timing, so the dot/dash threshold follows the user's speed.

At this point Sparks is fully useful without a PCB or a radio.

## Phase 3 — PCB

- [ ] Schematic in KiCad.
- [ ] Panel layout matching `media/morse_code_guide.png` — this is the hard part;
      the LED positions are fixed by the tree, routing works around them.
- [ ] Route; keep trace count low for the laser.
- [ ] Engrave, populate, test.

## Phase 4 — case

- [ ] Onshape model around the finished board.
- [ ] Front panel with the tree engraved — arms connecting parent to child.
- [ ] Export STEP/STL to `mechanical/`.

## Phase 5 — radio

- [ ] Research ANATEL limits for 433 MHz short-range in Brazil. **Blocking.**
- [ ] Populate the CC1101; test into a dummy load first.
- [ ] Key the radio in step with the LED panel and sidetone.
- [ ] Decide on a receive mode — decoding incoming Morse onto the same panel would
      be a natural extension.

## Ideas, unscheduled

- Playback mode: the device sends a letter, you copy it — reverse training.
- Speed/WPM readout using the spare DIG lines as a small status row.
- Store practice statistics; which letters you get wrong most.
