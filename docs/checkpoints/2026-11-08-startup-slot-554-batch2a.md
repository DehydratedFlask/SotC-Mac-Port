# Checkpoint — startup slot 554 batch 2a

- Date/time: 2026-11-08 04:41 UTC (2026-11-07 22:41 CST); agent: Buffy.
- Task: CPU-01-AOT-CFG-SCALE-01-BATCH38A.
- Files added: `native/core/ctor_slot_554_2a_6f793.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch2a.cpp`,
  `tools/lift/check_ctor_slot_554_batch2a.py`, this checkpoint.
- Files edited: `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}`,
  `native/tests/test_startup_ctor_coverage.cpp`, `tools/lift/l2_slot_check.py`,
  `docs/STARTUP_SLOT_554_PLAN.md`, `docs/ENTRY_MAP.md`, `docs/G1_EVIDENCE.md`,
  `ROADMAP.md`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`.
- Exact checks: `cmake --build local/build/b38 -j 12` exit 0; focused CTest 4/4
  (`startup_ctors`, `startup_ctor_coverage`, `ctor_slot_554`,
  `ctor_slot_554_batch2a`); full `ctest --test-dir local/build/b38 -j 12
  --output-on-failure` **685/685** (`local/build/b38/ctest_b2a2.log`);
  `check_ctor_slot_554_batch2a.py` exit 0 (30 operand pins, body sha
  `adfcc16736d8440b779af9cf4b80b44e1acbf380aba41184fdd0d5faab94ec6c`, 81 ELF +
  29 native mutants rejected); `--tamper elf` and `--tamper native` each exit 1;
  `gen_entry_map.py`/`--check` exit 0, **2,624 constants -> 1,743 distinct RVAs /
  559 headers / 417 duplicates**; G1 pack regenerated and
  `check_g1_evidence_pack.py` exit 0 (**573/573 registered, 572 fully lifted,
  554 PARTIAL**); the generator's hardcoded `batch 1/13` label was corrected to
  `batches 1 and 2a of 13`, then `g1_evidence_pack.py --write`,
  `check_g1_evidence_pack.py` and `check_ctor_slots.py` all exit 0; full `SOTC_NATIVE_BUILD=local/build/b38
  tools/lift/run_all_checkers.sh` **893 passed / 0 skipped / 0 failed** (exit 0, `local/build/b38/sweep_b2a_final.log`); `python3 scripts/project_index.py`
  **107,095 folders**; `python3 -I scripts/audit_publish_paths.py --all` **OK 4,110 paths**;
  `python3 -I scripts/check_commit_attribution.py` **OK**; `git diff --check` exit 0.
- Observations: the clean five-object prefix at `0x2eaa010..0x2eaa3d0` is five
  `0xc0` bools, body `[0x6f793,0x6fc29)` = `0x496` B. The body has 209
  instructions, 25 direct calls (`0x20af0` x5, `0xf590` x10, `0x20d60` x5,
  `0xf8f390` x5) and 40 direct stores, with no indirect call and no canary.
  Per object: **162 written / 30 holes**; batch: **810 written / 150 holes**.
  Batch 1 + 2a is `0x14e8` B, **4237 written / 1115 holes**, 13 registrations and
  13 atexit rows. The next object (`0x2eaa3d0`, `XPR Occluder Debug Mode`) is a
  `0xd0` `0x6abb0`-family wrapper, so batch 2 was split: batch 2b remains 31
  objects `0x2eaa3d0..0x2eabb80`, body `0x6fc29..0x718bb`.
- Inference: the seven following bools at `0x2eaa4a0..0x2eaa9e0` likely form a
  clean `0x744`-byte eight-object prefix once the `0x6abb0` wrapper is
  first-class. This is reconnaissance only; it is **not** modelled and no byte
  map is claimed.
- Remaining risk: the wrapper family's vtable/tag immediates must be pinned
  entry by entry; the batch-2a store map cannot be reused for `0x6abb0`
  objects. Slot 554 stays PARTIAL until batch 13 closes.
- Next exact action: model object 5 (`0x2eaa3d0`, `0x6abb0`, page `+0x18`,
  bounds `0..2`, atexit `0x54b60`), then objects 6..12 as the seven-bool prefix,
  and add `check_ctor_slot_554_batch2b.py`.
- Status: PASSED (batch 2a only) / STATIC_ANALYSIS_VERIFIED +
  FIXTURE_VERIFIED. Failed tries: 0. Full slot still PARTIAL.
