# Checkpoint: CPU-01-SLOT10-47-FAMILY-01 — all 34 descriptor+mutex+atexit constructors verified as one contract

- Time / timezone: 2026-10-21 00:40 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-SLOT10-47-FAMILY-01. **STATIC_ANALYSIS_VERIFIED**. No native code
  emitted or registered; slot 11 remains the blocker. G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  `0x2dd70e0` (573 entries, sha256 `5917cb85…3ab28`); private `local/re/work/ctor-classes.json`
  from CPU-01-CTOR-FAMILY-MAP-01; CPU-01-SLOT12/13-STRUCT-01 and CPU-01-SLOT14-19-FAMILY-01.
- Owned / changed files: NEW `tools/lift/verify_ctor_family.py`; NEW `docs/CTOR_CLASSES.md`
  (publishable summary); this checkpoint.
- Hypothesis and expected result: the family identified in slots 10–19 and 24–47 is uniform, so a
  single native helper type can model all 34 slots once slot 11 is resolved. Expected for every
  slot: two adjacent `.bss` globals, one `malloc`, one mutex init, exactly one `__cxa_atexit`
  registration (call or tail call) and a reference to the shared cleanup.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/verify_ctor_family.py` → **exit 0**
  - Earlier: `python3 -I tools/lift/classify_ctors.py` (writes the manifest this verifier replays)
- Exit / observed result:
  - 34/34 slots pass. **Every slot publishes exactly two `.bss` globals 0x10 bytes apart**, makes
    exactly one `malloc` call and one call to the mutex initializer `0xb30950`, registers exactly
    one `__cxa_atexit(0xe44a10, …)`, and references only in-image addresses.
  - `atexit` is a normal call in 29 slots and a tail call in the five 79-byte slots
    (13, 19, 24, 25, 27).
  - Global pairs by slot (private but non-payload addresses, for future consumer work):
    10 `0x3356678/0x3356688`, 11 `0x3356068/0x3356078`, 12 `0x3355f88/0x3355f98`,
    13 `0x3355470/0x3355480`, 14 `0x3355028/0x3355038`, 15 `0x3354e00/0x3354e10`,
    16 `0x3354b58/0x3354b68`, 17 `0x33549f0/0x3354a00`, 18 `0x3354660/0x3354670`,
    19 `0x3354190/0x33541a0`, 24 `0x3353ec0/0x3353ed0`, 25 `0x3353d90/0x3353da0`,
    26 `0x3353ce8/0x3353cf8`, 27 `0x3353bf0/0x3353c00`, 28 `0x3353b20/0x3353b30`,
    29 `0x3353ac8/0x3353ad8`, 30 `0x3353a80/0x3353a90`, 31 `0x3353a40/0x3353a50`,
    32 `0x3353a10/0x3353a20`, 33 `0x33539b8/0x33539c8`, 34 `0x3353970/0x3353980`,
    35 `0x3353930/0x3353940`, 36 `0x3353900/0x3353910`, 37 `0x33538a8/0x33538b8`,
    38 `0x3353860/0x3353870`, 39 `0x3353820/0x3353830`, 40 `0x33537f0/0x3353800`,
    41 `0x33537a0/0x33537b0`, 42 `0x3353770/0x3353780`, 43 `0x3353730/0x3353740`,
    44 `0x3353700/0x3353710`, 45 `0x33535c0/0x33535d0`, 46 `0x3353478/0x3353488`,
    47 `0x33533c8/0x33533d8`.
- Observation versus inference / coverage limitations:
  - Observation: all of the above, re-derived per slot from pinned bytes and replayed by one
    verifier; spans and body digests are pinned to the manifest.
  - Inference: the two globals hold (object/descriptor pointer, heap-mutex pointer); the published
    vtable address points seen at slots 10–12 suggest per-class descriptors. Class identities are
    still unknown for all 34 slots.
  - Not verified: consumers, runtime ordering, once-guard semantics, ABI of the mutex block, and
    whether a shared helper is faithful for the sub-shapes that also dispatch a virtual call.
- Cumulative failed tries and distinct hypotheses: 0 failed. The family check initially required a
  `__stack_chk_fail` guard for every slot and failed on the 79-byte members; the guard is correctly
  optional (those bodies have no local array).
- Remaining risks / blockers: slot 11 `0xf09160` still blocks the runner; registering the family
  before that would violate the strict-order contract. A shared helper must not silently accept a
  slot whose shape differs (the verifier is the guard against that).
- Next exact action: implement one native `DescriptorMutexRegistration` fixture behind the existing
  `StartupState` ownership model, with tests for the two sub-shapes (with/without the virtual
  dispatch, with/without the stack canary), and keep it unregistered until slot 11 passes.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
