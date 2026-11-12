# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH47 — slot 554 batch 11 landed

- Time / timezone: 2026-11-08, local (America/Los_Angeles).
- Agent and integrator: Buffy (single integrator; no concurrent worker edits to these files).
- Task / gate / status: CPU-01-AOT-CFG-SCALE-01-BATCH47 — PASSED /
  STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Slot 554 remains PARTIAL (batch 12 is next).
- Prerequisite evidence / input and source hashes: pinned private ELF `local/re/eboot.elf`,
  sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  word `0x6efe0` at `0x2dd70e0 - 554*8`; body `[0x83ad1,0x86052)` sha256
  `0f7577aea5c97031cbb980639f39465d633ed3ddba4e3667a043da524873e848`; object block
  `0x2eb9ee8..0x2ebbbc8` (`0x1ce0` B).
- Owned / changed files: `native/core/ctor_slot_554_batch11_83ad1.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch11.cpp`, `tools/lift/check_ctor_slot_554_batch11.py`,
  `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}`,
  `native/tests/test_startup_ctor_coverage.cpp`, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/STARTUP_SLOT_554_PLAN.md`, `docs/EVIDENCE.md`, `docs/G1_EVIDENCE.md`,
  `docs/entry_map.json`, `docs/FOLDERS.md`, `local/indexes/folders.json`, this checkpoint.
- Hypothesis and expected result: the body is 36 objects in four families (6 bools, 26 `0xc300`
  bounded floats, 3 `0xc580` page-cell dwords, 1 `0x6e4d0`/`0x6e660` wrapper), byte map 6,126
  written / 1,266 holes; the model, fixture and oracle reproduce it fail-closed.
- Exact command, CWD and nonsecret environment: CWD
  `/Volumes/Extra/SoC Mac Native`.
  `cmake -S native -B local/build/b11`; `cmake --build local/build/b11 -j 12`;
  `./local/build/b11/test_ctor_slot_554_batch11`;
  `python3 -I tools/lift/check_ctor_slot_554_batch11.py`;
  `python3 -I tools/lift/check_ctor_slot_554_batch11.py --tamper elf`;
  `python3 -I tools/lift/check_ctor_slot_554_batch11.py --tamper native`;
  `ctest --test-dir local/build/b11 -j 12 --output-on-failure`;
  `SOTC_NATIVE_BUILD=local/build/b11 tools/lift/run_all_checkers.sh`;
  `python3 tools/lift/g1_evidence_pack.py --write`;
  `python3 tools/lift/gen_entry_map.py`;
  `python3 scripts/project_index.py`;
  `python3 -I scripts/audit_publish_paths.py --all`;
  `python3 -I scripts/check_commit_attribution.py`.
- Exit / observed result: configure 0, build 0, fixture 0 (`ok (36 objects, 6126/1266 bytes)`),
  oracle 0 (484 pins; 1,067 ELF + 84 native mutants rejected), both tamper modes 1, CTest
  **696/696**, sweep **904 passed / 0 skipped / 0 failed** (exit 0), G1 OK **573/573**, entry map **2,835 -> 1,812 RVAs / 570
  headers (431 duplicates)**, project index **111,567**, path audit 0, attribution 0, staged paths 0.
- Log, capture and REA evidence paths/IDs: `/tmp/b11_cfg.log`, `/tmp/b11_build.log`,
  `/tmp/b11_ctest.log`, `/tmp/b11_sweep.log`, `/tmp/b11_oracle.log`, `/tmp/b11_te.log`,
  `/tmp/b11_tn.log`, `/tmp/b11_g1.log`, `/tmp/b11_map.log`, `/tmp/b11_index.log`,
  `/tmp/b11_paths.log`, `/tmp/b11_attr.log`; E1415; hermes `/tmp/b11.dis`, `/tmp/b11_rows.json`,
  `/tmp/b11_pins.txt`.
- Observation versus inference / coverage limitations: bytes, object starts, families, names,
  bounds, wrapper tags and callee hashes are measured from the pinned image. The `.bss` object
  block has no file-backed initial contents; the hole map is derived from the body stores plus the
  established family masks. The strict store definition (mov-family memory writes + all `vmovups`)
  recounts batch 10 at 497/119/378; that batch's pinned artifacts and records were corrected from
  the landed 481/119/362 during the batch-12 pass. Batch 11 records the strict counts. No guest byte was executed; no boot,
  audio, Metal or save/load claim is made.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries for this batch. The
  inherited `0x2ebbaf8` object end, 209 calls and 64 "direct stores" were measurement errors
  corrected before code was written (measured `0x2ebbbc8`, 179 direct + 30 indirect, 112 object /
  508 total); no failed implementation attempt.
- Remaining risks / blockers: batch 12 (body `0x86052..`, objects from `0x2ebbbc8`) still open;
  slot 554 remains PARTIAL. The fixture asserts the modelled store sequence, not a running game.
- Next exact action: measure batch 12's true body end and object block (`0x86052..`, start
  `0x2ebbbc8`; inherited body end `0x8849d`, last object start `0x2ebd728`, 199 calls and 148 stores
  unverified), then repeat the same model/fixture/oracle gate.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF top section,
  `docs/TASKS.md` BATCH47 row, E1415, plan status/row/result/gaps, G1 and entry map regenerated,
  project index rewritten. No install or dependency change, so INSTALL_LOG is unchanged.
