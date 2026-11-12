# 2026-11-08 — startup slot 554 batch 9 (36 objects)

- **Task:** `CPU-01-STARTUP-SLOT-554-BATCH-09` (`BATCH45`).
- **Agent:** Buffy.
- **Status:** `PASSED / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED`. Slot 554 remains
  `PARTIAL`; no boot/audio/Metal/save gate is claimed.
- **Scope:** body `[0x7f098,0x8160c)` = `0x2574` B, objects `0x2eb6538..0x2eb8218`
  (`0x1ce0` B).
- **Body digest:** `d3b8db529400729e574b4dc3d1e000b515123b58439efdaa5c41b5160fcbc6f6`.
  Measured 1,660 instructions, 178 direct calls, 30 indirect `call [rax+0x18]` functor
  destroys and 506 stores (110 object + 396 stack); no branch of any kind. Call histogram:
  `0xf590` 72, `0xf8f390` 36, `0x20d60` 34, `0xc300` 28, `0x20af0` 6, `0x6da20` 1,
  `0x6ddb0` 1.
- **Families:** six `0xc0` bools; twenty-eight `0xd0` `0xc300` bounded floats; two `0xd0`
  wrappers: `r_showRenderSettings` (`0x6da20` outer / `0x6dbb0` inner, inner vptr `0x15964a8`,
  final `0x15970a0`, tag `0x1597070`, atexit `0x54ee0`, page+`0x2ec`, bounds `0..2`) and
  `r_showRenderSettingsValues` (`0x6ddb0` outer / `0x6df40` inner, inner vptr `0x1596520`,
  final `0x1597138`, tag `0x1597108`, atexit `0x54f20`, page+`0x2f0`, bounds `0..1`).
- **Corrections:** the inherited object-row end `0x2eb8158` is the last object's start; the real
  block ends at `0x2eb8218` (batch 10's first `leaq`). The body end `0x8160c` is confirmed by that
  same `leaq`. The inherited 208 calls are 178 direct + 30 indirect; the inherited 58
  "direct stores" is not a store count (110 object stores, 506 including stack).
- **Byte map:** bool **162 / 30**, float **171 / 37**, wrapper **195 / 13**; batch
  **6,150 written / 1,242 holes** of `0x1ce0`. Batches 1 + 2a + 2b + 2b-cont + 3 + 4 + 5 +
  6 + 7 + 8 + 9: `0x1202f` body bytes / `0xf330` object bytes, **51,928 written / 10,328 holes**,
  296 registrations and 296 atexit rows.
- **Native files:** `native/core/ctor_slot_554_batch9_7f098.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch9.cpp`,
  `tools/lift/check_ctor_slot_554_batch9.py`, `native/CMakeLists.txt`, and the
  `StartupState` wiring in `native/core/startup_ctors.{hpp,cpp}` plus the coverage counters in
  `native/tests/test_startup_ctor_coverage.cpp`.
- **Checks run:**
  - `cmake --build local/build/b9 -j 12` → exit 0 (full build, including both focused targets).
  - `./local/build/b9/test_ctor_slot_554_batch9` → exit 0; 36 fields, 36 registrations/atexit
    rows, 72 inert views, two wrapper calls with their own RVAs, two view destroys, the
    1,242-hole sentinel sweep, fail-closed hooks and byte-identical replay.
  - `python3 tools/lift/check_ctor_slot_554_batch9.py` → exit 0; body/callee hashes,
    **428 operand pins**, **949 ELF + 78 native mutants rejected**.
  - `--tamper elf` → exit 1 (whole-body hash); `--tamper native` → exit 1 (missing constant).
  - `ctest --test-dir local/build/b9 -j 12 --output-on-failure` → **694/694 passed**.
  - `python3 tools/lift/g1_evidence_pack.py --write` → **573/573 registered**, first gap
    `-1/0x0`, 0 remaining.
  - `python3 tools/lift/gen_entry_map.py` then `check_entry_map.py` → **2,796 constants ->
    1,802 distinct RVAs in 568 headers** (429 duplicates).
  - `python3 scripts/project_index.py` → index **108,597**.
  - `python3 -I scripts/audit_publish_paths.py --all` → **OK 4,110 paths**.
  - `python3 -I scripts/check_commit_attribution.py` → exit 0.
  - `SOTC_NATIVE_BUILD=local/build/b9 tools/lift/run_all_checkers.sh` → **902 passed,
    0 skipped, 0 failed**.
- **Remaining:** batch 10 starts at body `0x8160c`, objects from `0x2eb8218`; its object/body
  extents must be measured rather than inherited. Batches 11–13 remain, with 119 registrations and
  140 atexit rows still open. No runtime claim.
