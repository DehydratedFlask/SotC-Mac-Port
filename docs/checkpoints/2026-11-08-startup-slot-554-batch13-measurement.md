> **Superseded by** `docs/checkpoints/2026-11-08-startup-slot-554-batch13-implementation.md`
> and the mask store trace checkpoint. This file preserves the measurement-only point in time.

# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH49 — slot 554 batch 13 measured

- Time / timezone: 2026-11-08, local (America/Los_Angeles).
- Agent and integrator: Buffy (single integrator).
- Task / gate / status: CPU-01-AOT-CFG-SCALE-01-BATCH49 — **ACTIVE /
  STATIC_ANALYSIS_VERIFIED (measurement only)**. Model, fixture, oracle, CMake wiring, build,
  CTest and checker sweep are **NOT RUN**. No completion claim is made.
- Prerequisite evidence / input: pinned private ELF `local/re/eboot.elf`, sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; body
  `[0x8849d,0x8a618)` = `0x217b` B; object block `0x2ebd7f8..0x2ebf0d0` = `0x18d8` B;
  dummy `0x2ebf0d0..0x2ebf1a0` = `0xd0` B.
- Owned / changed files: this checkpoint, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/STARTUP_SLOT_554_PLAN.md`, `docs/EVIDENCE.md`. No native, fixture, oracle or build file
  was changed.
- Hypothesis and expected result: the final batch is 31 block variables plus a separate dummy,
  not the inherited 32-object `0x1af8` span; body call/store counts and name pointer must be
  measured rather than inherited.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`.
  `python3 /tmp/b13_measure.py`; `objdump -d --start-address=0x8849d --stop-address=0x8a700
  local/re/eboot.elf`; the derived row table is `/tmp/b13_rows_raw.json`, disassembly
  `/tmp/b13.dis`, log `/tmp/b13_measure.log`.
- Exit / observed result: `b13_measure.py` exit 0. Body 1,501 instructions; 161 direct + 25
  indirect calls; 457 stores (360 mov + 97 vmov; 120 object = 115 block + 5 dummy; 337 stack);
  two branches (`0x8a59b je`, `0x8a601 jne`). Call histogram: `0xf590` 64, `0x20af0` 32,
  `0x20d60` 31, `0xf8f390` 31, `0xc580` 6, `0xc300` 14, `0x6ebe0` 2, `0x6ede0` 2, `0xf2f0` 1,
  `0xf8f3a0` 1. 32 object starts; 31 block variables + dummy. Block families: 7 bool, 14 c300,
  6 c580 (3 page-cell d0 + 3 owned d8), 4 wrappers. Block byte map: bool 1,134/210; c300
  2,394/518; c580 page 513/111; c580 owned 525/123; wrapper 780/52; block 5,346/1,014.
  Names: `r_furWaterResist` at `0xf9ca33` through `option_unavailable` at `0xf9ccd8`; dummy
  description at `0xf9cceb` (0x39 B).
- Log, capture and REA evidence paths/IDs: `/tmp/b13_measure.py`, `/tmp/b13_measure.log`,
  `/tmp/b13.dis`, `/tmp/b13_rows_raw.json`; E1417. The full disassembly was read from the pinned
  ELF; no guest instruction was executed.
- Observation versus inference / coverage limitations: starts, names, page cells, bounds, wrapper
  final/tag/atexit pins and c580 owned rows are measured from the pinned image. The dummy and
  owned-dword byte masks are inferred from the command/c580 shapes and are not fixture-verified.
  The body has 8 direct `0xc580` calls for 6 c580 objects and 31 direct `0xf8f390` calls for 32
  starts; both deltas are unresolved and must be explained before a model is written. The
  inherited 186 calls / 76 stores and `0x1af8` span are wrong. No boot, audio, Metal or save/load
  claim is made.
- Remaining risks / blockers: batch 13 model, fixture, oracle and wiring are unwritten; slot 554
  remains PARTIAL. The earlier c580/f8f390 delta note was a script artifact; the corrected counts reconcile.
- Next exact action: resolve the c580 and f8f390 call-count deltas, pin the dummy/owned-dword
  masks, then implement `ctor_slot_554_batch13_8849d` with the same model/fixture/oracle gates.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF top section, TASKS
  BATCH49, E1417, plan status/table/result/gaps, this checkpoint. No install or dependency change,
  so INSTALL_LOG is unchanged. No commit, stage or push; HEAD `6ef42d20`.
