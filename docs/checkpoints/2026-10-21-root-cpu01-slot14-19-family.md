# Checkpoint: CPU-01-SLOT14-19-FAMILY-01 — slots 14–19 share the slot-12/13 descriptor + heap-mutex + atexit contract

- Time / timezone: 2026-10-21 00:20 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-SLOT14-19-FAMILY-01. **STATIC_ANALYSIS_VERIFIED**; nothing lifted or
  registered. G1 not passed; slot 11 remains the first unresolved slot.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  `0x2dd70e0` (573 entries, sha256 `5917cb85…3ab28`); CPU-01-SLOT12-STRUCT-01 /
  CPU-01-SLOT13-STRUCT-01.
- Owned / changed files: NEW `tools/lift/verify_slot_family_14_19.py`; shared
  `tools/lift/analyze_slot_struct.py` (indirect *call* now follows its fallthrough, which was
  required to see the real body end of slots 14/16/17/18); this checkpoint.
- Hypothesis and expected result: the slot-12/13 pattern is not a two-off. Slots 14–19 should each
  (a) obtain or construct one object/descriptor, (b) publish two adjacent `.bss` globals,
  (c) allocate a 16-byte block and initialize a recursive+priority-inherit pthread mutex in it, and
  (d) register the shared destroy+free cleanup through `__cxa_atexit`.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/analyze_slot_struct.py <rva>` for each of the six slots
  - `python3 -I tools/lift/verify_slot_family_14_19.py` → **exit 0** (re-derives every value)
- Exit / observed result (all asserted by the verifier):

  | slot | RVA | span | digest (first 16) | obtain object via | `.bss` pair | virtual call |
  |---|---|---|---|---|---|---|
  | 14 | `0xf02ff0` | 239 B | `0c803e076d6f24f8` | `0xf011f0` | `0x3355028` / `0x3355038` | `call [rbx]` at `0xf03082` |
  | 15 | `0xf01100` | 226 B | `006d8ef333df1b54` | stack ctor `0xe1eda0`+`0xe1ede0` | `0x3354e00` / `0x3354e10` | — |
  | 16 | `0xf00090` | 284 B | `aed062395020b9ce` | `0xefe410` | `0x3354b58` / `0x3354b68` | `call [rbx]` at `0xf0014f` |
  | 17 | `0xefe320` | 239 B | `d157e4bf9c9e7f39` | `0xefd140` | `0x33549f0` / `0x3354a00` | `call [rbx]` at `0xefe3b2` |
  | 18 | `0xefd020` | 281 B | `a14f6aa8db166697` | `0xefad20` | `0x3354660` / `0x3354670` | `call [rbx]` at `0xefd0dc` |
  | 19 | `0xefacd0` | 79 B | `2750b1530f9edd2b` | `0xef8600` | `0x3354190` / `0x33541a0` | — |

  Shared, byte-verified facts for all six (and for slots 12/13):
  - `0xb30a10` is `lea rax,[rip]; ret` returning `0x12d2d1c`, whose dword is `0x10`; that value is
    passed to `malloc` in every registration thunk.
  - `0xb30950` initializes the mutex strictly through `pthread_mutexattr_init` →
    `settype` → `setprotocol` → `pthread_mutex_init` → `pthread_mutexattr_destroy`.
  - `0xe44a10` is the shared destructor: `pthread_mutex_destroy` stub `0xb309d0` then tail `free`.
  - `__cxa_atexit` is called/tail-called with `rdi = 0xe44a10`, `rsi = &<second .bss global>`,
    `rdx = 0x1604218` (GOT slot `0x15fffb8`, NID `tsvEmnenz48`).
  - Slots 14/16/17/18 and 12/15 carry a `__stack_chk_fail` guard; slots 13 and 19 do not (they
    have no local array). Every `.bss` target lies at or above `0x2dd7100` (first non-file-backed
    byte) and below `0x3956aa4`.
- Observation versus inference / coverage limitations:
  - Observation: every span, digest, call, indirect edge and global above.
  - Inference: the two published globals are (object/descriptor pointer, heap-mutex pointer); the
    virtual `call [rbx]` in slots 14/16/17/18 is an interface method invoked right after the
    descriptor is published (target not resolvable statically here). The classes behind
    `0x15f9ef0`, `0xf030e0`, `0xf011f0`, `0xefe410`, `0xefd140`, `0xefad20`, `0xef8600` are
    **not** identified.
  - Not verified: runtime order, once-guard semantics, who consumes the globals, and whether any
    of these classes is reached on the observed startup path. No native code emitted.
- Cumulative failed tries and distinct hypotheses: 0 failed. One tool correction (indirect call
  fallthrough) changed the reported spans of slots 14/16/17/18 from 148/193/148/190 bytes (truncated
  at the indirect call) to the correct 239/284/239/281 bytes; the truncated values are not used.
- Remaining risks / blockers: slot 11 (`0xf09160`) still blocks the runner, so this family is
  unreachable at runtime. Eight constructors now share one structural contract, which is a strong
  candidate for a single native helper type, but that helper must not be written until the slot-11
  blocker is resolved or the family is registered behind an explicit test.
- Next exact action: identify the class/interface behind the `0x15f9ef0` vtable and the four
  forwarding callees, or extend the same verifier to slots 20–29 to see where the family ends.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
