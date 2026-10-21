# Checkpoint: CPU-01-CTOR21-CONST-TABLE-01 — slot 21 (`0xee1130`) lifted as a byte-exact 128-byte constant table

- Time / timezone: 2026-10-21 01:25 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-CTOR21-CONST-TABLE-01. **FIXTURE_VERIFIED** (byte-exact data lift;
  semantics of the cells are NOT recovered). Not registered in `startup_ctors`; G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  `0x2dd70e0` (573 entries); CPU-01-CTOR-FAMILY-MAP-01 marked slot 21 as `other` and unlifted.
- Owned / changed files: NEW `native/core/const_table_ee1130.{hpp,cpp}`,
  NEW `native/tests/test_const_table_ee1130.cpp`, NEW `tools/lift/check_fee1130.py`,
  `native/CMakeLists.txt`, `native/README.md`, this checkpoint.
- Hypothesis and expected result: slot 21 is a call-free straight-line constant writer (182 bytes,
  no indirect transfers) that fills one 128-byte `.bss` block from eight rodata cells. Expected: the
  exact store map can be re-derived from the pinned bytes, including the four register/vector
  reloads, and reproduced by an ordinary native function.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/analyze_slot_struct.py 0xee1130`
  - `cmake --build local/build/native -j 8` → 0
  - `ctest --test-dir local/build/native` → **265/265 pass** (ASan+UBSan, arm64)
  - `python3 -I tools/lift/check_fee1130.py` → exit 0
  - Negative controls (second attempt, see below): corrupting one stored constant byte in
    `native/core/const_table_ee1130.cpp` → native test exit 1 **and** checker exit 1; restored →
    both pass again.
- Exit / observed result: body `0xee1130..0xee11e6`, sha256
  `0c983636dcbe529cd2852757a9368123db371ce189a777562cab666de172349d`, 26 instructions, one return.
  15 stores cover `0x33540f0..0x3354170` exactly (no gaps):

  | offset | 8/16 B value | source |
  |---|---|---|
  | +0x00 | `0000000000000000` | `vxorps` xmm0 |
  | +0x08 | `00000000ffffffff` | rax = `[0x1323b50]` |
  | +0x10 | `ffffffffffffffff` | `vpcmpeqd` xmm1 |
  | +0x18 | `0000000000000000` | xmm0 |
  | +0x20 | `ffffffffffffffff` | xmm1 |
  | +0x28 | `ffffffff00000000` | `vmovss [0x1323bdc] = 0xffffffff`, zero-extended |
  | +0x30 | `00000000ffffffff` | rax (unchanged) |
  | +0x38 | `ffffffffffffffff` | xmm1 |
  | +0x40 | `34000000`×4 | `vmovups [0x1323b60]` |
  | +0x50 | `bf800000bf800000` | rdx = `[0x1323b70]` (−1.0f, −1.0f) |
  | +0x58 | `3f800000bf800000` | rcx = `[0x1323b80]` (1.0f, −1.0f) |
  | +0x60 | `0000000000000000` | xmm0 |
  | +0x68 | `7f7fffff00000000` | rdx = `[0x1323b90]` (FLT_MAX, 0) |
  | +0x70 | `ff7fffffff7fffff` | rcx = `[0x1323ba0]` (−FLT_MAX, −FLT_MAX) |
  | +0x78 | `ff7fffff00000000` | `vmovss [0x1323be0] = 0xff7fffff`, zero-extended |

  The checker verifies each instruction's opcode template and decodes its raw `disp32` to the
  expected target, then simulates rax/rdx/rcx/xmm0/xmm1/xmm2 in original order and compares the
  resulting image with the native dump.
- Observation versus inference / coverage limitations:
  - Observation: body span/digest, every store address, width and value, and the reload order.
  - **Register-order trap:** using the initial `rdx`/`rcx`/`xmm0`/`xmm1` values for all stores
    produces wrong bytes — `xmm0` is reloaded at `0xee117d`, `xmm1` at `0xee119b`, `rdx` at
    `0xee11aa` and `rcx` at `0xee11b1`. The checker models this explicitly.
  - Inference: none about meaning. The values look like float/mask defaults for a 128-byte
    structure, but no consumer was traced, so the block is an untyped byte table.
  - Not verified: which code reads `0x33540f0..0x3354170`; whether the cells are independent or a
    struct with padding; runtime effect. Not registered.
- Cumulative failed tries and distinct hypotheses: 0 failed lifts. One **invalid negative control**
  was detected and corrected: the first attempt replaced a `constexpr` table that then became
  unused, so the build failed under `-Werror` and the stale binary still passed. The accepted
  control perturbs a stored byte inside a still-used constant instead. Only the corrected result is
  reported.
- Remaining risks / blockers: slot 11 still blocks the constructor runner, so this table is
  unreachable at runtime. The lift is a data image, not a typed model; do not attach field names
  without a consumer.
- Next exact action: after slot 11, find the reader(s) of `0x33540f0..0x3354170` and only then name
  the fields; until then leave the table untyped and unregistered.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
