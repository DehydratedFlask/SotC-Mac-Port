# Checkpoint — bug-report Frame root

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-FRAME-ROOT-01`
- Gate: **FIXTURE_VERIFIED**

## Bodies

| function | span | sha256 |
|---|---|---|
| `0xa70c30` Frame get/create | 220 B | `33074135a3486645057378fb6041f670fe5d9318eb24781fbeec6b0a417f4e24` |
| `0xa70d10` adopt | 32 B | `b1ed5e7e72a5ac845f06663de16d8f45ce895f4ad8d6d71873ab682eac25f09c` |

- `0xa70c30` lazily creates a `"Frame"` report node (`0xab1b20` class) and stores the integer
  property `"frameNum"` from `[owner+0x70]`, or -1 when the owner is null.
- `0xa70d10` calls `0xa70c30` at `0xa70d19`, then tail-jumps to `0xab0d40` at `0xa70d2a` to append
  the root under the Frame node. Ownership transfers; no copy.

## Native

- `ReportItem` gained an integer-property map (`set_int_property`, `int_property`), because
  `"frameNum"` is not stored as text.
- `FrameRoot::node(frame_num)` creates once and ignores later `frame_num`; `adopt` appends in
  order and returns the raw child. A null root is rejected before creating the node (native guard).
- Fixture: lazy creation, `frameNum` only on creation, two ordered children, parent/sibling links,
  null-root behavior.

## Checker

`check_fframe_root.py` pins both body hashes, the call and tail targets, and the
`"Frame"`/`"frameNum"` literals.

## Limits

The live frame counter `[owner+0x70]` and the 0x20 B holder `ctrl` (vtable `0x15d22f0`) are not
lifted. `snapshot_bug_data`'s `adopt_root` hook still links a caller-owned root; a future owner API
should transfer ownership into `FrameRoot`.
