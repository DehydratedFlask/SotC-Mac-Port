# Checkpoint: CPU-01-NAMEHASH-E71990-01 — name-hash algorithm decoded; vtable 0x15f9ef0 mapped

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64. Agent B (worker). Integrator
  files (`HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md`) were **not** edited; nothing committed.
- Task / status: CPU-01-NAMEHASH-E71990-01 **STATIC_ANALYSIS_VERIFIED + native hash function
  tested** (0 failed tries, first hypothesis confirmed). Bonus: CPU-01-VTABLE-15F9EF0-01
  **STATIC_ANALYSIS_VERIFIED** (contents and slot offsets only, no registration).
- Input: `local/re/eboot.elf` sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
  Offline only (python + `scripts/disasm_range.sh`). **No Ghidra/REA session was opened**; the
  persistent project was not touched.

## Findings (observation)

1. Ctor `0xe71990` (337 B, sha256 `3932fb85…beaf774`) contains **no call**. Its 14 hashes are
   immediates; the premise "it calls a hash function" is wrong. The runtime routine that produces
   the same values is the 67-byte function at **RVA `0xe70aa0`** (sha256
   `85516bb8f64e82a12498c2f395a6d3442101961154e95d5fdfa679b6e0e0459d`), called directly from
   `0xe5cd78` and `0xe5ce2a`, and reached by `0xe70af0` through a vtable slot at offset `+0x38`.
2. Algorithm (djb2 with XOR, not FNV): `h = 5381; for each byte c: h = (h*33) ^ sign_extend32(c)`;
   null pointer returns 0; a final 0 is remapped to 1 (`test ecx,ecx; cmovne`). Bytes are `movsx`.
3. Recomputing it over the 13 original `.rodata` strings matches **13/13** stored constants:
   `0xf1e58049 0x257da323 0x47a2c413 0xed7e6ae8 0xb2d47dcd 0x4941f51c..1f 0x10ef7257 0x7cae59c9
   0x330bbb24 0x2b1488e7`. The 14th record's hash is the literal `0` (sentinel; its string would hash
   to `0x8f2ebec1`, so 0 is not computed). FNV-1a and additive djb2 match 0/13 (negative controls).
4. Record table `0x3353f40`, 16-byte stride (`+0` string pointer, `+8` u32 hash); accessors
   `0xe70a60` (pointer by index) and `0xe70a80` (hash by index) both bound the index at `<= 12`
   (13 records; the sentinel at index 13 is unreachable through them).
5. The same loop appears **inlined with zero-extended bytes, no 0→1 remap** in hash-table lookups
   at `0xc0c58b 0xc0cfe3 0xc0d0d9 0xc0d19c 0xe5fd08 0xe67d37 0xe6802a 0xe6810e` (masked by
   bucket count). Pure-ASCII keys give identical values.

## Native code (clean, tested)

- `native/core/name_hash.{hpp,cpp}`: `name_hash()` (dedicated routine semantics) and
  `name_hash_inline()` (hash-table form). Test `native/tests/test_name_hash.cpp` uses synthetic
  vectors only (incl. a constructed raw-hash-0 string and high-byte cases). No game strings.
- `native/CMakeLists.txt`: additive — `core/name_hash.cpp` in `sotc_core`, plus `test_name_hash`.
- Not done on purpose: the 14-record table is not lifted/registered (the pointers address original
  strings, i.e. game content); the hash function removes the "unknown algorithm" blocker only.
  A lift would need a documented decision to import names as data or to regenerate the table.

## Vtable `0x15f9ef0` (bonus)

- Header 16 zero bytes (no RTTI), address point `0x15f9f00`, **222** contiguous code-pointer slots to
  `0x15fa5ef` (the qword at `0x15fa5f0` is `2`; terminator is a heuristic, not proven).
- Referenced from `0xf05952` (class ctor), `0xf059e0` (slot 0 dtor), `0xf05a36` (slot 1 deleting
  dtor), `0xf0639a` (the slot-12 registration thunk).
- Class overrides (all in `0xf05900..0xf06340`): slot 0 `0xf059e0`, 1 `0xf05a30`, 2 `0xf05a60`,
  3 `0xf062c0` (11 default stores `+0x44..+0x68`), 5 `0xf06310` (returns a 24-char class-name
  string at `0x13277b8`), 9 `0xf06320` (returns 1), **12 `0xf06330`** (returns RVA `0x1327840` and
  stores `0x80` to `*rsi`), 17 `0xf06000`, 18 `0xf06070`, 124 `0xf060e0`, 125 `0xf05ab0`, 126
  `0xf05fb0`. Remaining slots point at shared base code in `0xe1b000..0xe36000`.
- Definition-tree builder `0xf05b00` (singleton `0x24506a0`, once-flag `0x2450690`, mutex pointer
  `0x2450698`) allocates `0x720` bytes. The class name is withheld here (game string); it is in the
  private packet. **Inference:** an APEX NvParameterized-style parameters class unrelated to the
  slot-11 mesh class; slot 12 behaves as a 128-bit checksum accessor.

## Evidence and commands

- `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/cpu01-namehash-e71990-01/verify.py`
  exit 0, 58 checks. Packet: `report.md` (private), `records.private.json` (private strings),
  `verification.json` (hashes/addresses only), `SHA256SUMS`.
- `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/cpu01-vtable-15f9ef0-01/verify.py`
  exit 0, 36 checks; packet has the same layout.
- `cmake -S native -B local/build/native-agent-b -DSOTC_SANITIZE=ON` exit 0;
  `cmake --build … --target test_name_hash` exit 0; `ctest … -R '^name_hash$'` 1/1 passed
  (ASan+UBSan, arm64). The full suite was not re-run here.

## Risks / next action

- Consumers of the name table (`0xe5cd78` area, `0xe715d0`) are unanalysed; the sentinel's role is unknown.
- Next: decide the data policy for original name strings before any lift of `0xe71990`; register
  nothing until slot 11 is unblocked.
