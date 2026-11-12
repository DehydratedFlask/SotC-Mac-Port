# Checkpoint — startup slot 554 batch 2b prefix

- Date/time: 2026-11-08 (local session); agent: Buffy.
- Task: CPU-01-AOT-CFG-SCALE-01-BATCH38B.
- Files added: `native/core/ctor_slot_554_2b_6fc29.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch2b.cpp`,
  `tools/lift/check_ctor_slot_554_batch2b.py`, this checkpoint.
- Files edited: `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}`,
  `native/tests/test_startup_ctor_coverage.cpp`, `tools/lift/g1_evidence_pack.py`,
  `docs/STARTUP_SLOT_554_PLAN.md`, `docs/ENTRY_MAP.md`, `docs/G1_EVIDENCE.md`,
  `ROADMAP.md`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
  `docs/STARTUP_GAP_SURVEY.md`.
- Exact checks: `cmake --build local/build/b38 -j 12` exit 0; focused CTest 5/5
  (`startup_ctors`, `startup_ctor_coverage`, `ctor_slot_554`,
  `ctor_slot_554_batch2a`, `ctor_slot_554_batch2b`); full
  `ctest --test-dir local/build/b38 -j 12 --output-on-failure` **686/686**
  (`local/build/b38/ctest_b2b.log`); `check_ctor_slot_554_batch2b.py` exit 0
  (81 operand pins, body sha
  `9244a7d9ba3a205fcbe095c324106ac32c3dc2ab9f362257a1d2def3f4066d9b`, 193
  ELF + 49 native mutants rejected); `--tamper elf` and `--tamper native` each
  exit 1; `gen_entry_map.py`/`--check` exit 0, **2,636 constants -> 1,748
  distinct RVAs / 560 headers / 417 duplicates**; G1 generator strings
  corrected to `batches 1, 2a and the 2b eight-object prefix of 13`, then
  `g1_evidence_pack.py --write` exit 0 and `check_g1_evidence_pack.py` exit 0
  (**573/573 registered, 572 fully lifted, 554 PARTIAL**); project index
  **107,098**; full checker sweep on the final tree **894 passed / 0 skipped /
  0 failed** (exit 0, `local/build/b38/sweep_b2b_final.log`), also recorded in
  `docs/EVIDENCE.md` and `HANDOFF.md`.
- Observations: object 0 `0x2eaa3d0` is the 0xd0 `XPR Occluder Debug Mode`
  entry built by `0x6abb0` / `0x6ad40`; its pins are tag `0x15968b9`, inner
  table `0x1595e80` (point `0x1595e90`), final table `0x15968d8` (point
  `0x15968e8`), page cell `+0x18`, low 0 / high 2, register `0x20d60`,
  atexit `0x54b60`. The seven following bools `0x2eaa4a0..0x2eaa9e0` are
  `enableNonPower2DepthOccluders`, `dumpXPRInfoStats`, `useSunShadowIndices`,
  `doIndexBufferCulling`, `doSunShadowIndexBufferCulling`,
  `enableSmallPrimitiveIndexBufferCulling`, `forceUseLastIndexBufferCullingResults`.
  The body prefix `0x6fc29..0x7036d` is 328 instructions, 39 direct calls and
  one indirect view destroy, 63 object stores and 54 stack stores; no
  conditional branch or canary. **Object 0 195/13; each bool 162/30; prefix
  1329/223.** Cumulative with batches 1 and 2a: `0x138d` body bytes,
  `0x1af8` object bytes, **5566/1338**, 21 registrations and 21 atexit rows.
- Inference: none beyond the measured wrapper-family sharing; vtable/tag
  immediates are read from this entry, not copied from `0x6af60`.
- Remaining risk: the other 15 wrapper-family entries and the four twins are
  still unmodelled, and the remaining 23 objects of batch 2b use their own
  tags; slot 554 stays PARTIAL until batch 13 closes.
- Next exact action: classify and model the next object at `0x2eaa9e0`
  (`leaq` at `0x7036d`), then continue the remaining 23-object prefix
  `0x2eaa9e0..0x2eabb80`, body `0x7036d..0x718bb`.
- Status: PASSED (2b eight-object prefix only) / STATIC_ANALYSIS_VERIFIED +
  FIXTURE_VERIFIED. Failed tries: 0. Full slot still PARTIAL.
