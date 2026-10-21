# Checkpoint: CPU-01-LEAF-CTOR-BATCH-32 — three unguarded AVX-chain ctors lifted (idx 80/81/105)

- Time / timezone: 2026-10-21 00:37 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent `cpu01-native-batch` (deepseek-v4.1-flash-1017); integrator
  is the orchestrator session. `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `ROADMAP.md`, root `README.md`, `launcher/`, `scripts/`, `.gitignore` were **not** edited here.
- Task / gate / status: CPU-01-LEAF-CTOR-BATCH-32 (idx 80 `0xd8fa40`, idx 81 `0xd86ed0`,
  idx 105 `0xd5a280`). Status: **COMPLETE — 3 lifts, 0 skipped, FIXTURE_VERIFIED**. Not registered
  in `startup_ctors`; G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (asserted by each checker).
- Owned / changed files:
  - Added `native/core/avx_d8fa40.{hpp,cpp}`, `native/core/avx_d86ed0.{hpp,cpp}`,
    `native/core/avx_d5a280.{hpp,cpp}`.
  - Added `native/tests/test_avx_d8fa40.cpp`, `test_avx_d86ed0.cpp`, `test_avx_d5a280.cpp`.
  - Added `tools/lift/check_fd8fa40.py`, `check_fd86ed0.py`, `check_fd5a280.py`.
  - Modified `native/CMakeLists.txt` (3 sources + 3 test targets) and `native/README.md` (3 rows +
    3 checker names).
- Hypothesis and expected result: each RVA is an unguarded constructor that publishes rodata
  constants into BSS. Expected: decode every instruction, recover all sources/destinations and the
  final bytes, and lift only if unambiguous. All three decoded unambiguously; **3 lifts, 0 skipped**.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - Decode: `python3 -I <tmp>/dec.py 0xd8fa40 0xd86ed0` (exact-encoding walker) and
    `python3 -I <tmp>/dossier2.py` (raw-byte target re-derivation) for `0xd5a280`.
  - `cmake -S native -B local/build/native-agent-a -G Ninja` → exit 0
  - `cmake --build local/build/native-agent-a -j 8` → exit 0
  - `ctest --test-dir local/build/native-agent-a --output-on-failure` → **261/261 pass**
    (258 after batches 30/31 + 3 here)
  - `python3 -I tools/lift/check_fd8fa40.py` / `check_fd86ed0.py` / `check_fd5a280.py` → exit 0
  - `python3 -I tools/lift/check_ctor_slots.py` → exit 0
- Exit / observed result:
  - `0xd8fa40` (idx 80): body `0xd8fa40..0xd8fa9d`, 93 bytes, sha256 `f4580a78…60b072f`.
    10 `vmovups` (8 loads, 6 stores) + `vxorps` + 1 zero store + `ret`. Six 16-byte stores into
    `0x3352bb0`, `0x3352bf0`, `0x3352c00`, `0x3352bc0`, `0x3352bd0`, `0x3352be0`
    (contiguous 96-byte region). Sources: `0x130af70` (`{~0,~0,~0,0}`), `0x130af80`
    (`0x80000000`×4, −0.0f), `0x130af90` (`0x3089705f`×4), `0x130afa0` (`0x40000000`×4, 2.0f),
    `0x130afb0` (`0x33d6bf95`×4), plus a `vxorps` zero store (16 zero bytes).
    **No YMM/ZMM-width stores exist in this body** — all six stores are 16 bytes.
  - `0xd86ed0` (idx 81): body `0xd86ed0..0xd86f65`, 149 bytes, sha256 `d217178f…9b9cef66a`.
    Five 32-byte `vmovups ymm` stores, two 4-byte immediate stores and one 16-byte store into
    `0x3352aa0`..`0x3352bc0` (272-byte region; gaps at `0x3352ab0..0x3352ac0`,
    `0x3352b08..0x3352b20`, `0x3352bb0..0x3352bc0` are never written by the original).
    The 32-byte stores are modelled as four exact 8-byte native writes each (all 32 bytes
    emitted), not as 16 bytes. Sources: `0x130acc0`, `0x130ace0`, `0x130aca0`, `0x130acb0`,
    `0x130ad00`, `0x130ad20`, `0x130ad40`, `0x130ad60`; the two dword immediates are `0` and
    `0xbf800000` (−1.0f).
  - `0xd5a280` (idx 105): body `0xd5a280..0xd5a334`, 180 bytes, sha256 `6630dcb8…692e04f4`.
    Publishes the 16-byte `{~0,~0,~0,0}` mask from `0x1302fe0` at `0x3352120`, then runs a loop
    (16 iterations) writing four 32-byte blocks per iteration at `rcx + 0x80*i + {0,0x20,0x40,0x60}`
    with `rcx = 0x3352130`; `add rax,0x20` / `cmp rax,0x200` count **dwords**, so the written table
    is `0x800` bytes (512 int32), not `0x200`. Decoded constants: base lanes `0..7` (`0x13030e0`),
    splats 8/16/24/32 (`0x1302ff0`/`0x1303000`/`0x1303010`/`0x1303020`). The resulting table is the
    int32 identity `table[i] == i`, which the test asserts independently of the implementation.
  - All three are unguarded (no guard cells) and contain no calls.
- Log, capture and REA evidence paths/IDs: temp logs
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/` — `build-a3.log`,
  `verify-final.log`, `d8fa40.bin|.dis`, `d86ed0.bin|.dis`, `d5a280.bin|.dis`, `dossier2.py`.
  No Ghidra/REA session; no ELF execution.
- Observation versus inference / coverage limitations:
  - Observation: exact instruction bytes and RIP-relative sources/destinations for all three bodies;
    body SHA-256 pinned in each checker.
  - Observation (tooling): `scripts/disasm_range.sh` uses `llvm-objdump --adjust-vma`, so its `#`
    comments show section-relative, not absolute, targets. Every target in this batch was
    re-derived from the raw displacement bytes; the checkers do the same.
  - Observation: `0xd5a280`'s table is fully determined by six rodata constants and one loop, so it
    is lifted as ordinary native code (a native loop with 32-bit modular lane adds, mirroring
    `vpaddd`), not as a byte blob. The checker independently simulates the loop in Python from the
    ELF bytes and compares with the native `--dump`.
  - Inference: `0xd8fa40`'s and `0xd86ed0`'s float splats are engine constants; consumers were not
    analysed.
  - Not verified: run-time mutation of the rodata sources; whether any consumer depends on the
    identity table being contiguous with the mask cell.
- Cumulative failed tries and distinct hypotheses: 0 failed decode hypotheses. Two test-harness
  defects were fixed during development: (a) the "second call rewrites everything" assertion was
  wrong for cells with gaps — replaced with per-store coverage checks plus a "gap byte untouched"
  check; (b) the `0xd5a280` checker initially compared rodata lanes against a dict of addresses —
  corrected to compare each splat against its decoded value. Neither affected a decoded value.
- Remaining risks / blockers: unregistered fixtures. `0xd5a280` writes `0x800` bytes; a future
  consumer must not assume the table is `0x200` bytes (the earlier `0x200` reading came from the
  byte-vs-dword confusion documented above).
- Next exact action: integrator merges; next leaf batch, or start `startup_ctors` registration
  work with an explicit single-run decision for the shared-cell clones.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **NOT updated by this worker**.
  `scripts/project_index.py` and trail refresh not run.
