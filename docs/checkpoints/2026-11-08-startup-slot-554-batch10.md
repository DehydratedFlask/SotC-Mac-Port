# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH46 — slot 554 batch 10 landed

- Time / timezone: 2026-11-08, local (America/Los_Angeles).
- Agent and integrator: Buffy (single integrator; no concurrent worker edits to these files).
- Task / gate / status: CPU-01-AOT-CFG-SCALE-01-BATCH46 — PASSED /
  STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Slot 554 remains PARTIAL (batch 11 is next).
- Prerequisite evidence / input and source hashes: pinned private ELF `local/re/eboot.elf`,
  sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  word `0x6efe0` at `0x2dd70e0 - 554*8`; body `[0x8160c,0x83ad1)` sha256
  `94191f4eb264bb0c1b9230a3ed65b266b3e45b8d8eb8a97ca2c5cb49059dd27b`; object block
  `0x2eb8218..0x2eb9ee8` (`0x1cd0` B).
- Owned / changed files: `native/core/ctor_slot_554_batch10_8160c.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch10.cpp`, `tools/lift/check_ctor_slot_554_batch10.py`,
  `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}`,
  `native/tests/test_startup_ctor_coverage.cpp`, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/STARTUP_SLOT_554_PLAN.md`, `docs/EVIDENCE.md`, `docs/G1_EVIDENCE.md`,
  `docs/entry_map.json`, `docs/FOLDERS.md`, `local/indexes/folders.json`, this checkpoint.
- Hypothesis and expected result: the body is 36 objects in five families (7 bools, 22 `0xc300`
  bounded floats, 4 `0x6aa00` unbounded floats, 2 `0x3a980` unbounded ints, 1 `0x6e140` wrapper),
  byte map 6,081 written / 1,295 holes; the model, fixture and oracle reproduce it fail-closed.
- Exact command, CWD and nonsecret environment: CWD
  `/Volumes/Extra/SoC Mac Native`.
  `cmake -S native -B local/build/b10`; `cmake --build local/build/b10 -j 12`;
  `./local/build/b10/test_ctor_slot_554_batch10`;
  `python3 tools/lift/check_ctor_slot_554_batch10.py`;
  `python3 tools/lift/check_ctor_slot_554_batch10.py --tamper elf`;
  `python3 tools/lift/check_ctor_slot_554_batch10.py --tamper native`;
  `ctest --test-dir local/build/b10 -j 12 --output-on-failure`;
  `SOTC_NATIVE_BUILD=local/build/b10 tools/lift/run_all_checkers.sh`;
  `python3 tools/lift/g1_evidence_pack.py --write`;
  `python3 tools/lift/gen_entry_map.py`;
  `python3 scripts/project_index.py`;
  `python3 -I scripts/audit_publish_paths.py --all`;
  `python3 -I scripts/check_commit_attribution.py`.
- Exit / observed result: configure 0, build 0, fixture 0 (`ok (36 objects, 6081/1295 bytes)`),
  oracle 0 (475 pins; 1,051 ELF + 87 native mutants rejected), both tamper modes 1, CTest
  **695/695**, sweep **903 passed / 0 skipped / 0 failed** (exit 0), G1 OK **573/573**, entry map
  **2,816 -> 1,807 RVAs / 569 headers (430 duplicates)**, project index **110,081**, path audit 0,
  attribution 0, staged paths 0.
- Log, capture and REA evidence paths/IDs: `/tmp/b10_cfg.log`, `/tmp/b10_build.log`,
  `/tmp/b10_ctest.log`, `/tmp/b10_sweep.log`, `/tmp/b10_te.log`, `/tmp/b10_tn.log`,
  `/tmp/b10_g1.log`, `/tmp/b10_map.log`, `/tmp/b10_index.log`, `/tmp/b10_paths.log`,
  `/tmp/b10_attr.log`; E1414; hermes `/tmp/b10.dis`, `/tmp/b10_wrap.dis`,
  `/tmp/b10_inner.dis`, `/tmp/b10_rows.json`, `/tmp/b10_pins.txt`.
- Observation versus inference / coverage limitations: bytes, object starts, families, names,
  bounds, wrapper tags and callee hashes are measured from the pinned image. The `.bss` object
  block has no file-backed initial contents; the hole map is derived from the body stores plus
  the established family masks. No guest byte was executed; no boot, audio, Metal or save/load
  claim is made.
- Cumulative failed tries and distinct hypotheses: 0 for this batch. The inherited `0x1ac5` body
  size and `d89df573…` hash were arithmetic errors in the earlier summary, corrected by direct
  measurement before any code was written (no failed implementation try).
- Remaining risks / blockers: batch 11 (body `0x83ad1..`, objects from `0x2eb9ee8`) still open;
  slot 554 remains PARTIAL. The fixture asserts the modelled store sequence, not a running game.
- Next exact action: measure batch 11's true body end and object block (`0x83ad1..`, start
  `0x2eb9ee8`; inherited last start `0x2ebbaf8`), then repeat the same model/fixture/oracle gate.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF top section,
  `docs/TASKS.md` BATCH46 row, E1414, plan row/result/gaps, G1 and entry map regenerated, project
  index rewritten. No install or dependency change, so INSTALL_LOG is unchanged.

## Correction — strict store recount (2026-11-08, batch-12 pass)

- The landed batch-10 store total `481` (119 object + 362 stack) was 16 stack writes short. The
  strict rule used by batches 11/12 — every mov-family memory write plus every `vmovups` write —
  measures **497 total / 119 object / 378 stack**.
- Updated pinned artifacts: `native/core/ctor_slot_554_batch10_8160c.hpp`
  (`kSlot554B10StoreCount = 497`, `kSlot554B10StackStoreCount = 378`),
  `native/tests/test_ctor_slot_554_batch10.cpp` (assertions 497/119/378) and
  `tools/lift/check_ctor_slot_554_batch10.py` (`0x1f1` / `0x17a` header constants). The body hash,
  object map, byte map and operand pins are unchanged.
- Verification: fixture **0** (`ok (36 objects, 6081/1295 bytes)`); oracle **0** (475 pins; 1,051 ELF
  + 87 native mutants rejected); `--tamper elf` and `--tamper native` both exit **1**. Active docs
  (`HANDOFF.md`, `docs/TASKS.md` BATCH46, E1414, plan row/result) now state 497/119/378.
