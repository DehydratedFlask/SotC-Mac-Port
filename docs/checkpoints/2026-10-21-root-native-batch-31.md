# Checkpoint: CPU-01-LEAF-CTOR-BATCH-31 — three guarded-constant ctors lifted (idx 115/116/117)

- Time / timezone: 2026-10-21 00:37 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent `cpu01-native-batch` (deepseek-v4.1-flash-1017). Integrator
  is the orchestrator session. `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `ROADMAP.md`, root `README.md`, `launcher/`, `scripts/` and `.gitignore` were **not** edited by
  this worker.
- Task / gate / status: CPU-01-LEAF-CTOR-BATCH-31 (idx 115 `0xd39b00`, idx 116 `0xd366d0`,
  idx 117 `0xd33b50`). Status: **COMPLETE — 3 lifts, 0 skipped, FIXTURE_VERIFIED**. Not registered
  in `startup_ctors`; G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (asserted by each new
  checker). Survey positions from `local/re/work/ctor-survey.json` (573 entries) re-checked by the
  checkers; manifest indices were leads only.
- Owned / changed files:
  - Added `native/core/guarded_d39b00.{hpp,cpp}`, `native/core/guarded_d366d0.{hpp,cpp}`,
    `native/core/guarded_d33b50.{hpp,cpp}`.
  - Added `native/tests/test_guarded_d39b00.cpp`, `test_guarded_d366d0.cpp`,
    `test_guarded_d33b50.cpp`.
  - Added `tools/lift/check_fd39b00.py`, `check_fd366d0.py`, `check_fd33b50.py`.
  - Modified `native/CMakeLists.txt` (3 sources + 3 test targets), `native/README.md` (3 rows +
    3 checker names). `tools/lift/guarded_const_ctor.py` was extended once for A1 and is shared.
- Hypothesis and expected result: same guarded-constant family as batch 30 (9 guarded cells:
  6 float constants, `{~0,0,0,0}` lane mask, `{~0,~0,~0,0}` mask copy, u32 `1`), with
  `0xd366d0`/`0xd33b50` ending immediately after the guarded `1` and `0xd39b00` carrying an
  unguarded tail. Expected: 3 byte-exact decodes, 3 lifts, 0 skipped.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - Decode: `python3 -I <tmp>/dec.py 0xd39b00 0xd366d0 0xd33b50` (read-only)
  - `cmake -S native -B local/build/native-agent-a -G Ninja` → exit 0
  - `cmake --build local/build/native-agent-a -j 8` → exit 0
  - `ctest --test-dir local/build/native-agent-a --output-on-failure` → **258/258 pass**
    (252 baseline + batch-30 3 + batch-31 3; both batches measured in the same run)
  - `python3 -I tools/lift/check_fd39b00.py` / `check_fd366d0.py` / `check_fd33b50.py` → exit 0
  - `python3 -I tools/lift/check_ctor_slots.py` → exit 0
  - Negative control (shared with batch 30, same checker template): corrupting one expected
    constant in `check_fdd91b0.py` produced exit 1; restoring it produced exit 0.
- Exit / observed result:
  - `0xd39b00`: body `0xd39b00..0xd39cd3` (467 bytes, sha256 `efab66ad…cbf1e63`), 9 guarded cells,
    tail `0x3352020..0x3352090` (112 bytes, 14 qword stores from immediates and rodata qwords).
  - `0xd366d0`: body `0xd366d0..0xd367fe` (302 bytes, sha256 `94fb3729…e1d617`), 9 guarded cells,
    **no** unguarded tail (the guarded `1` store is the last write before `ret`).
  - `0xd33b50`: body `0xd33b50..0xd33c7e` (302 bytes, sha256 `24ded7ed…b35e34`), 9 guarded cells,
    **no** unguarded tail.
  - All three guard the same cells as the batch-30 clones (`0x3351ef4`…`0x3351f90`), so the same
    "first clone wins" observation applies. `0xd366d0`/`0xd33b50` differ only in the rodata address
    of the xyz-mask copy (`0x13010d0` / `0x1301090`) — the 16 bytes are identical.
- Log, capture and REA evidence paths/IDs: temp logs
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/` — `build-a.log`,
  `ctest-a1.log`, `verify-final.log`, `d39b00.bin|.dis`, `d366d0.bin|.dis`, `d33b50.bin|.dis`.
  Ephemeral; the checkers re-derive every value from the pinned ELF. No Ghidra/REA session, no ELF
  execution.
- Observation versus inference / coverage limitations:
  - Observation: exact bytes, guard/value cells, immediates, body lengths and body SHA-256; native
    `--dump` matches the decoded cells (and the empty tail for the two shorter clones).
  - Observation: `0xd39b00`'s tail mixes immediates with `mov r64,[rip+d]` rodata loads
    (e.g. `0xffff7fffffff7fff`, `0x000080bf000080bf`, `0x00000000ffff7f7f`), all decoded from the
    instruction stream.
  - Inference: same engine math-constant set as `core/math_constants`; tail tables are static data.
  - Not verified: consumers, run-time mutation of the rodata sources, ordering between clones.
- Cumulative failed tries and distinct hypotheses: 0 failed tries on this task.
- Remaining risks / blockers: unregistered fixtures; shared guarded cells require a single-clone
  registration decision.
- Next exact action: integrator merges; next leaf batch or `startup_ctors` registration work.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **NOT updated by this worker**.
  `scripts/project_index.py` and trail refresh not run (new `native/` scope outside trail's
  persisted `onlyDirs`).
