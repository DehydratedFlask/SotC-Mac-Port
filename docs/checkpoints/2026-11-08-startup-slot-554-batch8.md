# 2026-11-08 — startup slot 554 batch 8 (36 objects)

- **Task:** `CPU-01-STARTUP-SLOT-554-BATCH-08` (`BATCH44`).
- **Agent:** Buffy.
- **Status:** `PASSED / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED`. Slot 554 remains
  `PARTIAL`; no boot/audio/Metal/save gate is claimed.
- **Scope:** body `[0x7cb5b,0x7f098)` = `0x253d` B, objects `0x2eb4868..0x2eb6538`
  (`0x1cd0` B).
- **Body digest:** `ff7cbbd941da544771e5212252595b1a0be8477d85ac8d3914b11455ea79d9bb`.
  Measured 1,659 instructions, 177 direct calls, 29 indirect `call [rax+0x18]` functor
  destroys and 505 stores (115 object + 390 stack); no branch of any kind. Call histogram:
  `0xf590` 72, `0xf8f390` 36, `0x20d60` 33, `0xc300` 26, `0x20af0` 7, `0x6d690` 2,
  `0x6c850` 1.
- **Families:** seven `0xc0` bools; twenty-six `0xd0` `0xc300` bounded floats; three `0xd0`
  wrappers: `r_waterDebugDraw` (`0x6c850` outer / `0x6c9e0` inner, inner vptr `0x1596250`,
  final `0x1596da8`, tag `0x1596d78`, atexit `0x54da0`, page+`0x270`, bounds `0..5`) and
  `r_tonemapMethodSDR` / `r_tonemapMethodHDR` (`0x6d690` outer / `0x6d820` inner, inner vptr
  `0x1596430`, final `0x1597008`, tag `0x1596fd8`, atexit `0x54ea0`, page+`0x290` / `0x294`,
  bounds `0..5`).
- **Corrections:** the inherited object end `0x2eb6468` is the last object's start; the real
  block ends at `0x2eb6538` (batch 9's `leaq`). The inherited 206 calls are 177 direct + 29
  indirect; the inherited 62 "direct stores" is not a store count (115 object stores, 505
  including stack). The inherited body size `0x1cd0` is the object byte count; the measured
  body is `0x253d`.
- **Byte map:** bool **162 / 30**, float **171 / 37**, wrapper **195 / 13**; batch
  **6,165 written / 1,211 holes** of `0x1cd0`. Batches 1 + 2a + 2b + 2b-cont + 3 + 4 + 5 +
  6 + 7 + 8: `0xfabb` body bytes / `0xd650` object bytes, **45,778 written / 9,086 holes**,
  260 registrations and 260 atexit rows.
- **Native files:** `native/core/ctor_slot_554_batch8_7cb5b.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch8.cpp`,
  `tools/lift/check_ctor_slot_554_batch8.py`, `native/CMakeLists.txt`, and the
  `StartupState` wiring in `native/core/startup_ctors.{hpp,cpp}` plus the coverage counters in
  `native/tests/test_startup_ctor_coverage.cpp`.
- **Checks run:**
  - `cmake --build local/build/b38 -j 12 --target test_ctor_slot_554_batch8 test_startup_ctor_coverage`
    → exit 0.
  - `./local/build/b38/test_ctor_slot_554_batch8` → exit 0; 36 fields, 36 registrations/atexit
    rows, 72 inert views, three wrapper calls with their own RVAs, three view destroys, the
    1,211-hole sentinel sweep, fail-closed hooks and byte-identical replay.
  - `python3 tools/lift/check_ctor_slot_554_batch8.py` → exit 0; body/callee hashes,
    **423 operand pins**, **939 ELF + 78 native mutants rejected**.
  - `--tamper elf` → exit 1 (whole-body hash); `--tamper native` → exit 1 (missing constant).
  - `ctest --test-dir local/build/b38 -j 12 --output-on-failure` → **693/693 passed**.
  - `python3 tools/lift/g1_evidence_pack.py --write` → **573/573 registered**, first gap
    `-1/0x0`, 0 remaining.
  - `python3 scripts/project_index.py` → index **107,115**.
  - `python3 tools/lift/gen_entry_map.py` then `check_entry_map.py` → **2,776 constants ->
    1,794 distinct RVAs in 567 headers** (428 duplicates).
  - `python3 -I scripts/audit_publish_paths.py --all` → **OK 4,110 paths**.
  - `python3 -I scripts/check_commit_attribution.py` → exit 0.
  - `git diff --check` → exit 0.
  - `SOTC_NATIVE_BUILD=local/build/b38 tools/lift/run_all_checkers.sh` → **901 passed /
    0 skipped / 0 failed**.
- **Observation vs inference:** body extent, digest, object starts, page cells, wrapper
  vptrs/call counts/store counts and byte masks are measured from the pinned ELF; hook
  transfer/view behavior is a native model and never executes a guest RVA.
- **Repository state:** HEAD unchanged; nothing staged by this task. All edits are uncommitted.
- **Next exact action:** none for this unit. The next open slot-554 unit is batch 9 at body
  `0x7f098..` and object `0x2eb6538`, whose inherited object-row end must be treated as the last
  object's start until its body is measured.
