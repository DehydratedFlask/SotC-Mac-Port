# Checkpoint: CPU-01-LEAF-CTOR-BATCH-30 — three guarded-constant ctors lifted (idx 64/65/66)

- Time / timezone: 2026-10-21 00:37 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent `cpu01-native-batch` (deepseek-v4.1-flash-1017). Integrator
  is the orchestrator session. This worker did **not** edit `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/INDEX.md`, `ROADMAP.md`, `README.md`, `launcher/`, `scripts/`, or
  `.gitignore`; the content below is for the integrator to merge.
- Task / gate / status: CPU-01-LEAF-CTOR-BATCH-30 (idx 64 `0xdd91b0`, idx 65 `0xdd7e30`,
  idx 66 `0xdd4210`). Status: **COMPLETE — 3 lifts, 0 skipped, FIXTURE_VERIFIED** (native fixtures +
  tests + binary ties pass). Gate status unchanged: these ctors are **not** registered in
  `startup_ctors`; G1 not passed, G0 still PARTIAL.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (re-asserted by all three new
  checkers). Index/survey positions come from `local/re/work/ctor-survey.json` (573 entries); the
  task manifest indices 64/65/66 were treated as leads and re-derived from ELF bytes.
- Owned / changed files:
  - Added `native/core/guarded_dd91b0.{hpp,cpp}`, `native/core/guarded_dd7e30.{hpp,cpp}`,
    `native/core/guarded_dd4210.{hpp,cpp}`.
  - Added `native/tests/test_guarded_dd91b0.cpp`, `test_guarded_dd7e30.cpp`,
    `test_guarded_dd4210.cpp`.
  - Added `tools/lift/check_fdd91b0.py`, `check_fdd7e30.py`, `check_fdd4210.py`.
  - Modified `native/CMakeLists.txt` (3 `sotc_core` sources + 3 add_executable/link/add_test),
    `native/README.md` (3 table rows + 3 checker names in the binary-tie list).
  - Modified `tools/lift/guarded_const_ctor.py`: added an exact-encoding whole-body store decoder
    (`step`/`decode_body`/`DecodeError`) for the larger members of the family. The existing
    `match`/`match_tail`/`lift` API is unchanged, so `check_f6cad0.py`, `check_fdd41e0.py`,
    `check_ff4b510.py` and `check_math_ctor_clones.py` are unaffected (re-run: all still pass).
- Hypothesis and expected result: each RVA is a guarded static-constant constructor of the
  `0xdd...` family — 32-byte guarded repetitions (`lea rax,[rip+G]; cmp byte [rax],0; jne next;
  lea rcx,[rip+V]; mov qword [rax],1; mov dword [rcx],IMM`), then a guarded `{~0,0,0,0}` lane
  mask (`vmovd`+`vmovdqu`), a guarded 16-byte `{~0,~0,~0,0}` mask copy, a guarded u32 `1`, and an
  unguarded straight-line tail. Expected: all three bodies decode byte-exactly, 3 lifts, 0 skipped.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - Decode (read-only, no execution of the ELF):
    `python3 -I <tmp>/dec.py` (imports `tools/lift/guarded_const_ctor.py` + `scripts/ps4_dynlink.py`)
  - `cmake -S native -B local/build/native-agent-a -G Ninja` → exit 0
  - `cmake --build local/build/native-agent-a -j 8` → exit 0
  - `ctest --test-dir local/build/native-agent-a --output-on-failure` → **258/258 pass** (A1+A2
    were built and measured together; baseline was 252, +6 = 258)
  - `python3 -I tools/lift/check_fdd91b0.py` / `check_fdd7e30.py` / `check_fdd4210.py` → exit 0
  - `python3 -I tools/lift/check_ctor_slots.py` → exit 0 (108 registered RVA/index pairs; file is
    owned by another worker, `startup_ctors.cpp` untouched)
  - Negative control on `check_fdd91b0.py`: corrupted one expected constant
    (`0x40490fdb` → `0x40490fdc`) in the `EXPECTED_CELLS` literal, re-ran → exit 1
    (`FAIL guarded cells changed`), restored the file → exit 0.
- Exit / observed result: all three bodies decoded with 9 guarded cells each plus an unguarded tail;
  every store target and immediate re-derived from the RIP-relative displacements. No skipped ctor.
  - `0xdd91b0`: body `0xdd91b0..0xdd92fa` (330 bytes, sha256 `4fd21ef2…cef41`), 9 guarded cells,
    tail `0x3352e90..0x3352ea0` (16 bytes).
  - `0xdd7e30`: body `0xdd7e30..0xdd7fbc` (396 bytes, sha256 `8b735158…7a814`), 9 guarded cells,
    tail `0x3352e00..0x3352e40` (64 bytes).
  - `0xdd4210`: body `0xdd4210..0xdd43f6` (486 bytes, sha256 `fef6825b…3db09`), 9 guarded cells,
    tail `0x3352d30..0x3352db0` (128 bytes; two 16-byte mask copies and scalar cells).
  - All three guard the **same** six float cells (`0x3351ef4`…`0x3351f40`, guards
    `0x3351ef8`…`0x3351f48`), the same lane0/xyz mask cells (`0x3351f50`/`0x3351f70`, guards
    `0x3351f60`/`0x3351f80`) and the same u32 `1` cell (`0x3351f88`/`0x3351f90`); only the
    unguarded tails differ. In the original the first clone to run initialises the guarded cells and
    the siblings skip them.
  - Stored immediates: `0xbf800000` (−1.0f), `0x3f000000` (0.5f), `0x40000000` (2.0f),
    `0x40490fdb` (pi), `0x34000000` (FLT_EPSILON), `0x7f7fffff` (FLT_MAX), masks
    `{~0,0,0,0}` / `{~0,~0,~0,0}`, and `1`.
- Log, capture and REA evidence paths/IDs: temp logs under
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/`: `cmake-a.log`,
  `build-a.log`, `ctest-a1.log`, `verify-final.log`, `dec.py`, `dump.py`, `dd91b0.bin|.dis`,
  `dd7e30.bin|.dis`, `dd4210.bin|.dis`. These are ephemeral; the checkers reproduce every claim
  from `local/re/eboot.elf`. No Ghidra/REA session was used and no ELF was executed.
- Observation versus inference / coverage limitations:
  - Observation: exact instruction bytes, RIP-relative guard/value cells, stored immediates, body
    lengths and body SHA-256 for all three; the native `--dump` matches the decoded cells and tail.
  - Observation: the unguarded tail bytes come from immediates and from `mov`/`vmovss` loads out of
    file-backed `.rodata` (`0x130ee40`, `0x130eca0`, `0x130ead0` for the xyz mask; `0x130ee54` for a
    NaN-pattern float `0xffffffff` in `0xdd91b0`). Treating those bytes as constants assumes the
    rodata is not patched at run time — not verified.
  - Inference: the six floats are engine math constants (identical set to `core/math_constants`);
    the tails are further static tables whose consumers were not analysed.
  - Not verified: runtime effect, consumer behaviour, ordering against sibling clones, and whether
    the original ever runs more than one clone. Not registered in `startup_ctors`.
- Cumulative failed tries and distinct hypotheses: 0 failed tries on this task. One tooling defect
  was found and fixed during development (the new decoder initially mis-computed VEX
  rip-relative store targets by 4 bytes; corrected before any fixture was generated, and the
  final decode was re-verified against objdump text and raw rodata reads).
- Remaining risks / blockers: fixtures are unregistered and run no engine code. The guarded cells
  are shared between clones, so a future `startup_ctors` registration must run exactly one clone
  per cell set (or rely on the guard), which is a design decision for the integrator.
- Next exact action: integrator merges this record; then either register one of these clones in
  `startup_ctors` behind a test that proves original slot order, or continue with the next leaf
  batch. Do not claim G1 from these fixtures.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **NOT updated by this worker**
  (integrator-owned). `python3 scripts/project_index.py` and a trail refresh were not run; the new
  `native/` files are outside trail's persisted `onlyDirs`.
