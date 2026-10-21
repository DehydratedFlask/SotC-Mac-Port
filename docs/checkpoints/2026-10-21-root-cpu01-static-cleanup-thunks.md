# Checkpoint: CPU-01-STATIC-CLEANUP-THUNKS-01 — 15 constructor-table entries are 33-byte atexit registration thunks

- Time / timezone: 2026-10-21 01:45 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-STATIC-CLEANUP-THUNKS-01. **STATIC_ANALYSIS_VERIFIED**; no lift,
  no registration (the destructors' semantics are unknown). G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  `0x2dd70e0` (573 entries); CPU-01-CTOR-FAMILY-MAP-01 (`local/re/work/ctor-classes.json`), which
  left these slots inside the `other` class.
- Owned / changed files: NEW `tools/lift/verify_cleanup_thunks.py`; `docs/CTOR_CLASSES.md`
  (new family section); this checkpoint.
- Hypothesis and expected result: of the 54 unlifted call-free `other` bodies, the sixteen 33-byte
  entries are not constant writers but static-destructor registrations: zero a `.bss` cell and
  register its destructor through `__cxa_atexit`. Expected: one exact five-instruction shape with
  no call and no return.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/analyze_slot_struct.py <rva>` per slot
  - `python3 -I tools/lift/verify_cleanup_thunks.py` → **exit 0**
- Exit / observed result: **15 slots** match (one of the sixteen 33-byte bodies was a different
  shape and is excluded):
  `515 0x1c00e0`, `522 0x16e960`, `523 0x16cd50`, `525 0x166ef0`, `527 0x157e60`,
  `528 0x156fb0`, `531 0x1442c0`, `533 0x143300`, `542 0x0fb3d0`, `546 0x0a25f0`,
  `547 0x09a970`, `549 0x096650`, `551 0x08d740`, `564 0x031220`, `565 0x02fcf0`.
  Each body is 33 bytes: `48 8d 35 <d32>` (lea rsi, cell), `48 8d 3d <d32>` (lea rdi, destructor),
  `48 8d 15 <d32>` (lea rdx, `0x1604218`), `48 c7 06 00 00 00 00` (zero the cell), `e9 <d32>`
  (tail `jmp` to the `__cxa_atexit` thunk `0xf8f390`, GOT `0x15fffb8`). No calls, no returns, no
  indirect transfers, one branch edge each. Cell/destructor pairs and body digests are pinned in the
  verifier and summarised in `docs/CTOR_CLASSES.md`.
- Observation versus inference / coverage limitations:
  - Observation: the exact body shape, digests, `.bss` cells (all `>= 0x2dd7100`) and destructor
    addresses (all code) for all 15 slots.
  - Inference: these are static-destructor registrations (`__cxa_atexit` with a zeroed slot) — the
    argument order (`rsi` = cell, `rdi` = destructor, `rdx` = dso) is the ABI, so this is a strong
    reading, not a guess.
  - Not verified: what each destructor does, whether the zeroed cell is later repopulated by other
    constructors, ordering, and runtime effect. Nothing is lifted or registered.
- Cumulative failed tries and distinct hypotheses: 0 failed hypotheses. The 33-byte span group
  contained one non-member, which the verifier excludes by shape rather than by size — a size-only
  classification would have over-claimed 16.
- Remaining risks / blockers: slot 11 still blocks the runner. Lifting these would require the
  destructor bodies (they are ordinary cleanup functions, several of which are outside the
  recovered prefix).
- Next exact action: characterise one destructor (e.g. `0x1bdae0`, which serves slot 515) to decide
  whether the native model is "zeroed cell + destructor" or a typed owner.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
