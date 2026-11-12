# Checkpoint — startup slot 554 batch 1

- Date/time: 2026-11-08 (local); agent: Buffy.
- Task: CPU-01-AOT-CFG-SCALE-01-BATCH38.
- Files added: `native/core/ctor_slot_554_6efe0.{hpp,cpp}`,
  `native/core/ctor_slot_554_page_init.inc`, `native/tests/test_ctor_slot_554.cpp`,
  `tools/lift/check_ctor_slot_554.py`, `docs/STARTUP_SLOT_554_PLAN.md`, this checkpoint.
- Files edited: `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}`,
  `native/tests/test_startup_ctors.cpp`, `native/tests/test_startup_ctor_coverage.cpp`,
  `tools/lift/check_ctor_slots.py`, `tools/lift/check_console_var_ctors.py`,
  `tools/lift/check_fctor_slots_296_297_300.py`, `ROADMAP.md`, `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`.
- Exact checks: `cmake --build local/build/b38 -j 12` exit 0; focused CTest 3/3; full `ctest
  --test-dir local/build/b38 -j 12 --output-on-failure` **684/684**; `check_ctor_slot_554.py`
  exit 0 (129 ELF + 32 native mutants); `check_ctor_slots.py` 573 rows; `check_console_var_ctors.py`
  exit 0; full `SOTC_NATIVE_BUILD=local/build/b38 tools/lift/run_all_checkers.sh` **892 passed /
  0 skipped / 0 failed**; `check_g1_evidence_pack.py` OK after regenerating 573/573 with 554
  PARTIAL; entry map 2,615 -> 1,742 RVAs / 558 headers; index 107,093; path audit OK 4,110;
  attribution OK; HEAD `6ef42d20…`, staged 0.
- Observations: batch 1 models 0xac8 page + 0x660 objects; 187 page stores; 3427 written /
  965 holes; 8 registrations and 8 atexit rows. The full slot is 112,184 B and remains PARTIAL.
- Inference: the twelve remaining batches' hole counts cannot be claimed before their
  constructors are modelled; only measured object/body extents and direct-store counts are given.
- Remaining risk: later batches encounter the 17 x 397 B wrappers and four twins; none are modelled
  in this batch.
- Next exact action: lift batch 2 (`0x2eaa010..0x2eabac0`, body `0x6f793..0x718bb`), classify its
  constructors, close its byte map, and add `check_ctor_slot_554_batch2.py`.
- Status: PASSED (batch 1 only) / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Failed tries: 0.
