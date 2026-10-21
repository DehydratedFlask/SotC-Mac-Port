# Checkpoint: CPU-01-LEAF-CTOR-BATCH-33 — three ctors classified; byte-exact dossiers, 0 lifts

- Time / timezone: 2026-10-21 00:37 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent `cpu01-native-batch` (deepseek-v4.1-flash-1017); integrator
  is the orchestrator session. `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `ROADMAP.md`, root `README.md`, `launcher/`, `scripts/`, `.gitignore` were **not** edited here.
- Task / gate / status: CPU-01-LEAF-CTOR-BATCH-33 (idx 23 `0xe71990`, idx 55 `0xdf94c0`,
  idx 59 `0xde0160`). Status: **COMPLETE as analysis-only — STATIC_ANALYSIS_VERIFIED, 0 lifts.**
  No native code was written for these three; nothing is registered in `startup_ctors`. G1 not
  passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; ctor table/survey
  `local/re/work/ctor-survey.json` (573 entries, positions 23/55/59). All targets below were
  re-derived from the raw instruction bytes (see the tooling note).
- Owned / changed files: **none** under `native/` or `tools/lift/` for this task (this record is the
  deliverable). No CMake, README or test changes. Nothing added to `native/core`.
- Hypothesis and expected result: the three are not simple constant initialisers — `0xe71990` is a
  straight-line store sequence, `0xdf94c0` and `0xde0160` call other functions. Expected outcome:
  classify each and lift only those whose semantics are fully determined from bytes alone. Result:
  **none of the three is liftable without inventing semantics**, so all three get a dossier.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - Raw dump: `python3 -I <tmp>/dump.py 0x<RVA> 0x600 <tmp>/<rva>.bin` (read-only)
  - Disassembly aid: `sh scripts/disasm_range.sh <tmp>/<rva>.bin 0x<RVA>`
  - Target/hash extraction: `python3 -I <tmp>/dossier2.py` (parses the raw instruction bytes; does
    not trust objdump's `#` comments)
  - `python3 -I tools/lift/check_ctor_slots.py` → exit 0 (unchanged; no registration work here)
- Exit / observed result: three dossiers, no lift. Details:

  **1) idx 23, RVA `0xe71990` — store-only registry initialiser, 337 bytes**
  - Body `0xe71990..0xe71ae1` (ends at `ret` `0xe71ae0`), sha256
    `3932fb85b759ae53dd1675142ffda4b449be725f241cfeefdb40a07a2beaf774`, 43 instructions.
  - Direct calls: **none**. Tail jumps: none. No guards, no loops, no branches except the `ret`.
  - 14 `lea` of pointers into file-backed `.rodata` at `0x131c990`..`0x131caa4`
    (NUL-terminated printable ASCII; lengths 17/15/16/17/14/18/18/18/18/19/20/30/27/16 — **contents
    withheld, they are original game strings**), and 14 `{qword pointer, dword hash}` records
    written to `0x3353f40`..`0x3354020` (16-byte stride: pointer at +0, u32 hash at +8, +12 pad).
  - Hash immediates in order: `0xf1e58049`, `0x257da323`, `0x47a2c413`, `0xed7e6ae8`,
    `0xb2d47dcd`, `0x4941f51c`, `0x4941f51d`, `0x4941f51e`, `0x4941f51f`, `0x10ef7257`,
    `0x7cae59c9`, `0x330bbb24`, `0x2b1488e7`, then `0x0`.
  - Why not lifted: the only faithful lift would have to publish the original rodata string
    addresses (i.e. import game strings) into a global table; that is a game-content import, not an
    ordinary native function, and the hash algorithm behind the immediates is not determined.
  - Unresolved edges: the table's consumers and the hash function are unknown.

  **2) idx 55, RVA `0xdf94c0` — calls a function and stores its float result, 34 bytes**
  - Body `0xdf94c0..0xdf94e2` (ends at `ret` `0xdf94e1`), sha256
    `4fa743f1e109cbe0b3884cf10fe5c521cfa6f24463d7150d9f9b0cb4a6a1c0fe`, 9 instructions:
    `push rbp; mov rbp,rsp; vmovss xmm0,[0x1311c3c]; mov edi,1; xor esi,esi;
    call 0xf8fd10; vmovss [0x3353330],xmm0; pop rbp; ret`.
  - Input float read from `0x1311c3c` (bits `0x3d567750`); result stored to `0x3353330`.
  - Direct call `0xf8fd10` is **not** in the 573-entry ctor table; its semantics are unknown, so the
    returned float cannot be predicted from these bytes.
  - Why not lifted: a native function that calls an unimplemented 0xf8fd10 and stores its result
    would be a stub, not a lift.
  - Unresolved edges: `0xf8fd10` (callee semantics, argument meaning `edi=1`, `esi=0`), and the
    consumer of `0x3353330`.

  **3) idx 59, RVA `0xde0160` — allocate + zero + tail-call registration, 60 bytes**
  - Body `0xde0160..0xde019c`, sha256
    `4824cb9bf8f87100209cf7a7b59015b18d87f527bead3bfd43f77e6df6da597b`, 13 instructions:
    `push rbp; mov rbp,rsp; lea rdi,[0x3352ee0]; mov esi,1; mov edx,0x400; call 0xf8f4f0;
    lea rdx,[0x1604218]; vxorps xmm0,xmm0,xmm0; lea rdi,[0xddf980]; xor esi,esi;
    vmovups [0x33532e0],xmm0; pop rbp; jmp 0xf8f390`.
  - Body end is the `jmp 0xf8f390` (tail call) at `0xde0197`; there is **no** `ret` in this body.
    The `ret` at `0xde021e` belongs to the *next* function at `0xde01a0` (a 0x3f-dword fill loop),
    which is not part of this ctor.
  - Direct call `0xf8f4f0` (`rdi=0x3352ee0`, `esi=1`, `edx=0x400`) and tail call `0xf8f390`
    (`rdi=0xddf980`, `esi=0`, `rdx=0x1604218`); neither target is in the ctor table. `0xddf980` is
    a one-byte `ret` stub followed by padding. `0x1604218` holds a 16-byte record whose first qword
    is its own address (`0x01604218`); role unknown (vtable/descriptor-shaped).
  - Why not lifted: two unresolved calls plus a 16-byte zero store to `0x33532e0` whose purpose
    depends on the callees.
  - Unresolved edges: `0xf8f4f0`, `0xf8f390`, `0xddf980` (stub), the `0x1604218` record, and
    `0x3352ee0`/`0x33532e0`.

- Log, capture and REA evidence paths/IDs: temp logs
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/` — `e71990.bin|.dis`,
  `df94c0.bin|.dis`, `de0160.bin|.dis`, `dossier2.py`. No Ghidra/REA session was opened (not needed
  for a byte dossier); no ELF was executed. These logs are ephemeral; the commands above reproduce
  them exactly.
- Observation versus inference / coverage limitations:
  - Observation: body ranges, body SHA-256, every instruction byte, every direct call/tail-jump
    target and every RIP-relative memory target.
  - Observation (tooling): `scripts/disasm_range.sh` runs `llvm-objdump --adjust-vma`, so its `#`
    comment targets are section-relative and wrong (e.g. it prints `0x4ab000` for a target that is
    really `0x131c990`). Every target in this record was recomputed from the raw displacement bytes;
    do not reuse the objdump comments.
  - Inference: `0xe71990` looks like a name→hash registry table (13 named entries + one hash-0
    record); `0xde0160` looks like an allocate-and-register pattern (allocation call + zeroing +
    tail call with a `ret`-stub destructor). Both are labelled as inference, not decoded semantics.
  - Not verified: any run-time behaviour, any consumer, the hash function, and the two callees.
- Cumulative failed tries and distinct hypotheses: 0 failed tries. One correction was made while
  building the dossier: the initial body range for `0xde0160` was taken from the first `ret` in the
  disassembly (which belongs to the adjacent function at `0xde01a0`); the body was re-bounded at the
  tail `jmp`, and the corrected 60-byte body hash is the one above.
- Remaining risks / blockers: these three remain unlifted; a future attempt needs the callees
  (`0xf8fd10`, `0xf8f4f0`, `0xf8f390`) analysed first, and — for `0xe71990` — an explicit decision
  on whether original game strings may be imported as native data.
- Next exact action: integrator merges; next worker should analyse `0xf8fd10` and `0xf8f4f0`
  (callee semantics) before retrying `0xdf94c0`/`0xde0160`, and decide the string-import policy
  before retrying `0xe71990`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **NOT updated by this worker**.
  `scripts/project_index.py` and trail refresh not run.
