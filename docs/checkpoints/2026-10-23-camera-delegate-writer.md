# Checkpoint — camera bound-delegate writer

- Local: 2026-10-23 (America/Chicago)
- Agent: static-RE worker; integrator
- Task: `CPU-01-CAMERA-DELEGATE-WRITER-01`
- Gate: **STATIC_ANALYSIS_VERIFIED (negative result)**; the pose remains blocked

## Findings

- `D = [element+0x18]` is the camera-scene `_Root` node: vtable `0x15992b8`, ctor `0x1d6230`
  (486 B), object size `0x3c8`, allocated at `0x1c5904`, named `"_Root"` at `0x1c58a5`.
- The ctor **zeroes** `+0x2e8..+0x307`. A full opcode-aware census of the RX segment (760 disp32
  hits at `+0x2e8`, 324 at `+0x2f0`, 542 at `+0x2f8`) plus four independent cross-checks — three
  adjacent stores, `vmovups` + scalar pair, 94 `lea rdi,[rX+0x2e8]` sites, and a 7×7×7
  register-permutation search — produced **zero** static writers.
- The 24 B Itanium bound PMF is `{object +0x2e8, pfn +0x2f0, adjust +0x2f8}` (plus an unread
  `+0x300`). `pfn` bit 0 selects a vtable byte offset; otherwise it is a code address. `adjust` is
  added to `object`. Exactly one delegate is reachable from `0x1c70b0`/`0x1c0f40`; its target
  returns `const CameraTransform*` (0x40 B).
- Slot 0 is populated by `0x1c0b40`, driven from `0x1c6e80` (`CameraObject::Update` →
  `0x1c0c90`/`0x1c1010`) and from the `snapshotBugData` chain's `0x1c0f40` call. `override_flag`
  `+0x264` is never written by camera code, so the forced-object fast path is dead in this build.

## Side results

- `bpe::Camera`: hash `0x3cb0eb33`, vtable `0x159ac50`, ctor `0x212150`, size `0xb0`.
- `bpe::DebugCamera`: hash `0x6f9e1a98`, vtable `0x159abf0`, ctor `0x211c00`, clone `0x211cf0`.
- The prior program-header map dropped `phdr 1` (`va 0x1594000`, filesz `0x6d190`), exactly where
  `.data.rel.ro` lives. The worker's parser reproduces the existing `0x1c0f40`/`0x1c70b0` hashes
  byte-for-byte, so this is a verified correction, not an assumption.

## Consequence

There is no static pose to lift. The pure `0x50ba0` Euler conversion and the `%.2f` formatter are
already native; the live six values need the unbound delegate, which this build never writes.

## Exact next action

Full-decode `0x1c0000..0x1da000` and `0x20c000..0x216800` with rizin; find the function taking a
`BoundMemberFn*` in `rdi` that writes `[rdi]`, `[rdi+8]`, `[rdi+0x10]`, then census its callers and
keep the one reached from `[rX+0x290]` or from `D`. The disp32 scan cannot see `mod=00/01` stores,
so a real instruction decode is required.
