# Checkpoint: CPU-01-LEAF-CTOR-BATCH-25..29 — 27 remaining mask-copy leaf ctors lifted

- Time / timezone: 2026-10-20 22:35 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent (CPU-01 leaf-ctor batch 25..29). `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md` NOT edited by this worker; content below
  is for the integrator to merge.
- Task / gate / status: CPU-01-LEAF-CTOR-BATCH-25..29 (one combined bounded task). Status:
  COMPLETE for the offline static lift + native tests + binary ties. Gate status unchanged:
  fixture only, not wired into `startup_ctors`, G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf`
  sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (re-verified by every
  new checker); leads from `local/re/ctor-vec-copies.json` (227 entries) and
  `local/re/work/ctor-survey.json` (573 ctors). Manifest values were treated as leads only and
  re-derived from ELF bytes.
- Owned / changed files:
  - Added 108 files (27 × hpp/cpp/test/checker):
    `native/core/vec_{bb82d0,bb7990,bacfb0,bace80,bace50,bacb30,bab950,ba3ef0,b950e0,b8d170,
    b89eb0,b84580,b7b270,b77e90,b77910,b77740,b71f80,b6e6d0,b67a70,b5ecf0,b58690,b57360,
    b4ef70,b494d0,b47100,b44580,b33bd0}.{hpp,cpp}`,
    `native/tests/test_vec_<same>.cpp`, `tools/lift/check_f<same>.py`.
  - Modified `native/CMakeLists.txt` (27 `sotc_core` sources + 27 add_executable/link/add_test).
  - Modified `native/README.md` (27 table rows + 27 checker names in the binary-tie command list).
- Hypothesis and expected result: each of the 27 RVAs is an unguarded leaf ctor of the form
  `vmovups xmm0,[rip+d]` (c5 f8 10 05) / `vmovups [rip+d],xmm0` (c5 f8 11 05) / `ret` (c3),
  17 bytes, copying 16 bytes from a rodata cell into its own BSS cell, lanes `{~0,~0,~0,0}`.
  Expected: all 27 byte-match, 0 skipped; test count 224 → 251.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - ELF decode/verification (read-only, no execution of the ELF):
    `python3 -I <tmp>/analyze27.py` (uses `scripts/ps4_dynlink.py` `program_headers`/`Image.read`)
  - `cmake -S native -B local/build/native -G Ninja` — **failed** (pre-existing
    `local/build/native` was configured with Unix Makefiles; generator mismatch). Not deleted.
  - `cmake -S native -B local/build/native-cpu01-2529 -G Ninja` (clean, documented command form)
  - `cmake --build local/build/native-cpu01-2529`
  - `ctest --test-dir local/build/native-cpu01-2529 --output-on-failure`
  - `cmake --build local/build/native` (existing Makefiles dir; auto-reconfigured)
  - `ctest --test-dir local/build/native --output-on-failure`
  - `python3 -I tools/lift/check_f<rva>.py` for all 27 new RVAs
  - Negative control: temporarily corrupt one README dest token, rerun `check_fbb82d0.py`,
    restore README.
- Exit / observed result:
  - ELF decode: all 27 RVAs match the 17-byte idiom exactly; derived source/dest equal the task
    manifest for all 27; all lane quads `{0xffffffff,0xffffffff,0xffffffff,0}`; each dest is
    unique in the manifest and not referenced by any pre-existing `native/core/vec_*.hpp`.
    **27 lifted, 0 skipped.**
  - Clean Ninja build: configure 0, build 0, ctest 0 — `100% tests passed out of 251`.
  - Existing `local/build/native` (Makefiles): build 0, ctest 0 — `100% tests passed out of 251`.
  - All 27 checkers exit 0 (`OK vec_<rva> binary tie: body_sha256=... src=... dst=...`).
  - Negative control: checker exit 1 with "README row disagrees: missing `0x3350f10`", then 0
    after restore.
- Log, capture and REA evidence paths/IDs: temp logs under
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/`:
  `cmake.log` (expected generator error), `fresh-config.log`, `fresh-build.log`, `fresh-ctest.log`,
  `ctest.log`, `build.log`, `analyze27.py`, `gather.py`, `data27.json`, `gen.py`, `reg.py`.
  No Ghidra/REA session used; no ELF execution, no emulator. Fresh build dir
  `local/build/native-cpu01-2529` was removed after the run (private, regenerable); the canonical
  `local/build/native` remains.
- Observation versus inference / coverage limitations:
  - Observation: exact instruction bytes, RIP-relative displacements, source/dest cells and lane
    bits for all 27, re-derived from the hash-pinned ELF and tied to the native binaries.
  - Observation: these 27 form the tail of the manifest (indices 196–226). The gaps in this set
    (manifest 212–214 = `0xb76950`, `0xb747f0`, `0xb743c0`; manifest 221 = `0xb52110`) were
    already lifted by earlier batches and are present in `native/core`.
  - Observation: after this batch every one of the 227 `ctor-vec-copies.json` entries has a
    native lift or a recorded disposition — `0xf30460`/`0xf50d70` via `xyz_mask_copy`
    (`check_ff30460.py`, `check_ff50d70.py`), and `0xd71f60` remains NEEDS_PRINCIPAL. The
    manifest itself is therefore exhausted; further leaf work must come from a new survey, not
    from this file.
  - Inference: the cells are plain 16-byte constant globals consumed by later game code; no
    caller analysis was performed in this task, so consumers remain unverified.
  - Not verified: runtime effect of the values, whether any consumer mutates the cell, and
    ordering constraints against other ctors. Not registered in `startup_ctors`, so none of this
    runs at startup yet.
- Cumulative failed tries and distinct hypotheses: 0 failed tries on this task. One environment
  mismatch (Ninja vs Makefiles in the pre-existing build dir) was resolved by using a clean
  build dir for the documented command and the existing dir for the canonical suite; no
  implementation hypothesis was falsified.
- Remaining risks / blockers: `local/build/native` is a Makefiles tree, so the README's
  `-G Ninja` line cannot be re-run against it without deleting the cache (not done — another
  worker may own it). The 27 new ctors are unregistered fixtures; G1 still requires startup
  ordering and real consumers.
- Next exact action: integrator merges this record into `HANDOFF.md` / `docs/TASKS.md` /
  `docs/EVIDENCE.md`, then the next leaf-ctor batch should either (a) confirm no remaining
  manifest entries are unlifted, or (b) move to registering a contiguous run of these mask-copy
  ctors in `startup_ctors` behind a test that proves original slot order.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: NOT updated by this worker
  (orchestrator-owned). `python3 scripts/project_index.py` and trail refresh were not run; the
  new `native/` scope is outside trail's persisted `onlyDirs`, so graph coverage for these files
  is absent by design and must be handled by the integrator if required.
