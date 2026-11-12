# 2026-11-08 — startup slot 554 batch 2b continuation (23 objects)

- **Task:** `CPU-01-STARTUP-SLOT-554-BATCH-02B-CONT` (`BATCH38C`).
- **Agent:** Buffy.
- **Status:** `PASSED / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED`. Slot 554 remains
  `PARTIAL`; no boot/audio/Metal/save gate is claimed.
- **Scope:** the remaining body `[0x7036d,0x718bb)` = `0x154e` B and the 23 objects
  `0x2eaa9e0..0x2eabb80` (`0x11a0` B). Batch 2b is now complete (8-object prefix + 23-object
  continuation).
- **Body digest:** `5d14edc8d2d0985e0a523e6fa8b25f313b4924c338d75dc1e82f05f740421ca0`.
  Measured 955 instructions, 111 direct calls, six indirect `call [reg+0x18]` functor destroys,
  332 stores (157 object + 175 stack), no conditional branch.
- **Families:** four `0xd0` wrappers (`0x6af60`/`0x6b0f0` x2, `0x6b2f0`/`0x6b480`,
  `0x6b680`/`0x6b810`), one `0x3a980` int (`RenderTargetDisplayMip`), one `0xc300` bounded float
  (`miscDebugShaderFloat0`), and seventeen `0xc0` bools.
- **Byte map:** wrapper **195 written / 13 holes**; int **165 / 43**; float **171 / 37**; bool
  **162 / 30**; batch **3870 written / 642 holes**. Batches 1 + 2a + 2b accumulate `0x28db` body
  bytes, `0x2c98` object bytes, **9436 written / 1980 holes**, 44 registrations and 44 atexit rows.
- **Corrections:** `Show Ambient RenderTarget` bounds are `0xf..0xf`; the `0x6b2f0` and `0x6b680`
  spec rows publish final vptrs `0x1596a18` / `0x1596ab0` (table + `0x10`); the bounded float's
  arg-fn alias `0x1596878` resolves through the same `0x15d57e0`/`0x1594318` helpers as the
  canonical float family.
- **Native files:** `native/core/ctor_slot_554_2b_cont_7036d.{hpp,cpp}`,
  `native/tests/test_ctor_slot_554_batch2b_cont.cpp`, `tools/lift/check_ctor_slot_554_batch2b_cont.py`,
  `native/CMakeLists.txt`, and the `StartupState` wiring in `native/core/startup_ctors.{hpp,cpp}`.
- **Checks run:**
  - `cmake --build local/build/b38 -j 12` → exit 0.
  - `./local/build/b38/test_ctor_slot_554_batch2b_cont` → exit 0; 274-pin oracle companion covers
    all 23 objects, 23 registrations/atexit rows, 642-hole sentinel sweep, fail-closed hooks and
    byte-identical replay.
  - `python3 tools/lift/check_ctor_slot_554_batch2b_cont.py` → exit 0; body/callee hashes,
    **274 operand pins**, **621 ELF + 48 native mutants rejected**.
  - `--tamper elf` and `--tamper native` → both exit 1 (whole-body hash / missing token), proving
    the checker fails closed.
  - `ctest --test-dir local/build/b38 -j 12 --output-on-failure` → **692/692 passed**.
  - `python3 tools/lift/g1_evidence_pack.py --write` → **573/573 registered**, first gap `-1/0x0`,
    no remaining.
  - `python3 scripts/project_index.py` → index **107,113**.
  - `python3 tools/lift/check_entry_map.py` → **2,756 constants -> 1,789 distinct RVAs in 566
    headers** (424 duplicates).
  - `python3 -I scripts/audit_publish_paths.py --all` → **OK 4,110 paths**.
  - `python3 -I scripts/check_commit_attribution.py` → exit 0.
  - `git diff --check` → exit 0.
  - `SOTC_NATIVE_BUILD=local/build/b38 tools/lift/run_all_checkers.sh` → exit 0;
    **900 passed / 0 skipped / 0 failed**.
- **Observation vs inference:** the body extent, digest, object starts, page cells, wrapper vptrs,
  call/store counts and byte masks are measured from the pinned ELF; hook transfer/view behavior is
  a native model and never executes a guest RVA.
- **Repository state:** HEAD unchanged; nothing staged by this task. All edits are uncommitted.
- **Next exact action:** none for this unit. The next open slot-554 unit is batch 8
  (`0x7cb5b..0x7f098`) per `docs/TASKS.md`.
