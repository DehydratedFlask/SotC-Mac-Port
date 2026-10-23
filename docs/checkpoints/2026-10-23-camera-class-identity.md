# Checkpoint — camera object class identity

- Local: 2026-10-23 (America/Chicago)
- Agent: static-analysis worker
- Task: `CPU-01-CAMERA-CLASS-IDENTITY-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**

## Verdict

`D` is **not** the `_Root` class (`vtable 0x15992b8`, ctor `0x1d6230`, 0x3c8 B). It is a
**Camera-typed registry object**: `0x20e890(D)` is `is_type<0x3cb0eb33>`, and `0x3cb0eb33` is
`"Camera"` (name getter `0x2121b0`; classId `0x6f9e1a98` -> `new(0xb0)` + ctor `0x211c00`,
vtable `0x159abf0`), with a 16-byte handle at `+0x20` and a parent object at `+0x18`.

## Decisive bytes

1. `0xa68030` returns `slot->value` from an inline 65536-entry array at `owner+0x1041b0`, stride
   `0x10`; slot = `{u32 version; u32 pad; void* value}`. There is no `element+0x18` field:
   `element+0x18` is the next slot's `value`. Exactly one instruction writes `slot->value`
   (`0xa688e5`).
2. `0x216480` starts `mov rcx,[rdi+0x18]; mov rdi,[rcx+0x2e8]; ...` — the 24-byte block is at
   `[D+0x18]+0x2e8`, one indirection away from `D`. E599's census target label was wrong.
3. `D` cannot be `_Root`: `0x2165d0` and `0x20e890` read `[D+0x18]`/`[D+0x30]` without null tests,
   and the `_Root` ctor chain writes no `+0x18` and explicitly zeroes `+0x30`.

## Pose path

The `+0x2e8` census reproduces E599: 57 disp-`0x2f0` stores image-wide, 0 immediates, 0
`lea`-of-code. The path is debug-only: the TU is `bpe::ToolRemoteCommandMgr`, the string is
`"SyncCameraAcrossTools %s %f %f %f %f %f %f"` sent as `"SendConCommand"`, rate-limited and gated
on a live tool connection.

## Limits and next action

`D`'s exact concrete class and `[D+0x18]`'s class remain open. Next: decode `0x1d64c0..0x1d6508`
and the `_Root` vtable `+0x10` dispatch sites to decide whether handle registration ever happens,
and therefore whether `D` can be non-null from gameplay. No native `BoundMemberFn` is modelled.
