# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH48 — slot 554 batch 12 landed

- Time / timezone: 2026-11-08, local (America/Los_Angeles).
- Agent and integrator: Buffy (single integrator; no concurrent worker edits to these files).
- Task / gate / status: CPU-01-AOT-CFG-SCALE-01-BATCH48 — PASSED /
  STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Slot 554 remains PARTIAL (batch 13 is next).
- Prerequisite evidence / input and source hashes: pinned private ELF `local/re/eboot.elf`,
  sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  word `0x6efe0` at `0x2dd70e0 - 554*8`; body `[0x86052,0x8849d)` sha256
  `634603b5c627d875d067453f32a541cd83a789b32a310c3eae071d8e210ec9ba`; object block
  `0x2ebbbc8..0x2ebd7f8` (`0x1c30` B).
- Owned / changed files: `native/core/ctor_slot_554_batch12_86052.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch12.cpp`, `tools/lift/check_ctor_slot_554_batch12.py`,
  `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}`,
  `native/tests/test_startup_ctor_coverage.cpp`, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/STARTUP_SLOT_554_PLAN.md`, `docs/EVIDENCE.md`, `docs/G1_EVIDENCE.md`,
  `docs/entry_map.json`, `docs/FOLDERS.md`, `local/indexes/folders.json`, this checkpoint; batch-10
  correction in `native/core/ctor_slot_554_batch10_8160c.hpp`,
  `native/tests/test_ctor_slot_554_batch10.cpp`, `tools/lift/check_ctor_slot_554_batch10.py`,
  `docs/checkpoints/2026-11-08-startup-slot-554-batch10.md`.
- Hypothesis and expected result: the body is 36 objects in three families (17 bools, 17 `0xc300`
  bounded floats, 2 unbounded wrappers `0x6e800`/`0x6ebe0` and `0x6e9f0`/`0x6ede0`), byte map 6,051
  written / 1,165 holes; the model, fixture and oracle reproduce it fail-closed.
- Exact command, CWD and nonsecret environment: CWD
  `/Volumes/Extra/SoC Mac Native`.
  `cmake -S native -B local/build/b12`; `cmake --build local/build/b12 -j 12`;
  `./local/build/b12/test_ctor_slot_554_batch12`;
  `./local/build/b12/test_ctor_slot_554_batch10`;
  `python3 -I tools/lift/check_ctor_slot_554_batch12.py`;
  `python3 -I tools/lift/check_ctor_slot_554_batch12.py --tamper elf`;
  `python3 -I tools/lift/check_ctor_slot_554_batch12.py --tamper native`;
  `python3 -I tools/lift/check_ctor_slot_554_batch10.py` (and both tamper modes);
  `ctest --test-dir local/build/b12 -j 12 --output-on-failure`;
  `SOTC_NATIVE_BUILD=local/build/b12 tools/lift/run_all_checkers.sh`;
  `python3 tools/lift/g1_evidence_pack.py --write`;
  `python3 tools/lift/gen_entry_map.py`;
  `python3 scripts/project_index.py`;
  `python3 -I scripts/audit_publish_paths.py --all`;
  `python3 -I scripts/check_commit_attribution.py`.
- Exit / observed result: build 0, fixture 0 (`ok (36 objects, 6051/1165 bytes)`), batch-10 fixture 0
  (`ok (36 objects, 6081/1295 bytes)`), both oracles 0 (batch 12: 459 pins, 1,027 ELF + 99 native
  mutants; batch 10: 475 pins, 1,051 ELF + 87 native mutants), all tamper modes 1, CTest
  **697/697**, sweep **905 passed / 0 skipped / 0 failed**, G1 **OK 573/573**, entry map **2,861 -> 1,824 RVAs / 571 headers (432 duplicates)**, project index
  **113,055**, path audit **OK**, attribution **OK**, staged paths 0.
- Log, capture and REA evidence paths/IDs: `/tmp/b12_build3.log`, `/tmp/b12_ctest.log`,
  `/tmp/b12_fixture.log`, `/tmp/b12_oracle_default.log`, `/tmp/b12_oracle_elf.log`,
  `/tmp/b12_oracle_native.log`, `/tmp/b10_fixture.log`, `/tmp/b10_oracle_default.log`,
  `/tmp/b12_sweep.log`, `/tmp/b12_g1.log`, `/tmp/b12_map.log`, `/tmp/b12_index.log`,
  `/tmp/b12_paths.log`, `/tmp/b12_attr.log`; E1416; `/tmp/b12.dis`, `/tmp/b12_rows.json`,
  `/tmp/b12_pins.txt`, `/tmp/b12_table.log`, `/tmp/b12_measure.log`.
- Observation versus inference / coverage limitations: bytes, body extent, object starts, families,
  names, bounds, wrapper tags, getter/setter identities and callee hashes are measured from the
  pinned image. The `.bss` object block has no file-backed initial contents; the hole map is derived
  from body stores plus the established family masks. The inherited 199 calls / 148 stores were
  wrong; measured 180 direct + 19 indirect calls and 518 strict stores (191 object + 327 stack).
  The inherited body end `0x8849d` and last object start `0x2ebd728` were confirmed; the block ends
  at `0x2ebd7f8`, which batch 13's `leaq` names. No guest byte was executed; no boot, audio, Metal
  or save/load claim is made.
- Cumulative failed tries and distinct hypotheses: 0 analysis hypotheses for this batch. Two
  mechanical copy-label build failures (stale `_83ad1.hpp` include, then `Slot554Batch11*` type
  names) were corrected without changing the model, fixture or pins; no failed analysis try.
- Remaining risks / blockers: batch 13 (body starts `0x8849d`, objects from `0x2ebd7f8`) still open;
  slot 554 remains PARTIAL. The fixture asserts the modelled store sequence, not a running game.
- Next exact action: measure batch 13's true body end, object count and block end (start `0x8849d`;
  first object `0x2ebd7f8`; second `0x2ebd8c8` at `0x885ab`; inherited 32 objects through
  `0x2ebf0d0` and body end `0x8a618`), then repeat the same model/fixture/oracle gate.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF top section,
  `docs/TASKS.md` BATCH48 row (and BATCH46 correction), E1416 (and E1414/E1415 correction), plan
  status/row/result/gaps, G1 and entry map regenerated, project index rewritten. No install or
  dependency change, so INSTALL_LOG is unchanged.

## Final closeout at stopping point

- The first closing build was interrupted by the user after `cmake` had started rebuilding
  (`/tmp/b12_build_final.log` ends with `Terminated: 15`). It was re-run, not assumed: the final
  build is **0** (`/tmp/b12_build_final2.log`).
- Final focused results: batch-12 fixture **0** (`ok (36 objects, 6051/1165 bytes)`), batch-10
  fixture **0** (`ok (36 objects, 6081/1295 bytes)`); batch-12 oracle **0** (459 pins, 1,027 ELF +
  99 native mutants); batch-10 oracle **0** (475 pins, 1,051 ELF + 87 native mutants); all four
  tamper modes **1**; CTest **697/697**; checker sweep **905 passed / 0 skipped / 0 failed**.
- Final paths: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/STARTUP_SLOT_554_PLAN.md`,
  `docs/G1_EVIDENCE.md`, `docs/entry_map.json`, `docs/FOLDERS.md`, `local/indexes/folders.json` and
  this checkpoint. `git diff --cached` is empty and HEAD is `6ef42d20`; no commit, stage or push was
  made.
- State at stop: batch 12 is closed as STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; slot 554 remains
  PARTIAL. Next open unit is batch 13, body start `0x8849d`, first object `0x2ebd7f8`, second
  `0x2ebd8c8` at `0x885ab`; its 32-object count, block end and body end are still inherited.
