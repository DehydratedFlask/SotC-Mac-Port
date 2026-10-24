# Checkpoint — `_Root::vtable[+0x10]` registration question

- Local: 2026-10-23 (America/Chicago)
- Agent: static-analysis worker
- Task: `CPU-01-CAMERA-ROOT-REGISTRATION-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**

## Verdict

`_Root::vtable[+0x10]` (`0x1d64e0`) **is** invoked, exactly once at `0xa670ff` inside the world base
constructor `0xa66dc0` (`[world+0x88]` is the `_Root`). It is a **5-argument pass-through** to
`0xa6c7d0`; E602 §8.2's "partial call" is corrected — `rsi/rdx/rcx/r8/r9` are the function's own
incoming arguments.

It does **not** install a parent/handle on a Camera. It writes only `_Root`'s own `+0x38`, `+0x50`,
`+0x60`, then forwards to `component->vtable[+0x10]`. The Camera class (`vptr 0x159abf0`) has
`+0x10 = 0x211d80`, a deserializer; the attach `0xa6e780` exists on exactly one vtable
(`0x15d2208`) plus the Touch override `0x216420`. No Camera field is touched.

## Consequence

The registration path is a world-construction component funnel and is silent about cameras. The pose
**consumer** remains the debug-only `SyncCameraAcrossTools` command (E602 §6); nothing found makes
it reachable from gameplay. A native `BoundMemberFn` is therefore **not** a gameplay requirement and
should not be modelled.

## Coverage

- `0x1d64c0..0x1d6508` and `0xa6c7d0` decoded end to end; FDE-derived boundaries.
- 58,205 indirect-transfer records, 3,407 `call [reg+0x10]` sites, 6,682 `call reg`; 10,089 sites
  sliced, 4,459 virtual-slot sites, 2,840 unresolved hops, 3 chains with `[X+0x88]` provenance.
- Vtable `0x15992b8` has 11 code slots; slot 2 is `0x1d64e0`, slot 4 is `0x1d6670`.

## Limits

Gameplay reachability is `INFERRED` from the absence of any path; the world funnel's dynamic
dispatch remains outside a static census. The camera math (`0x50ba0`) stays native and valid.
