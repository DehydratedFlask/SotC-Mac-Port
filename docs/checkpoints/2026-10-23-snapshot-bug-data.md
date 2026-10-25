# Checkpoint — `snapshotBugData` BugData block

- Local: 2026-10-23 (America/Chicago)
- Agent: static-RE worker; integrator
- Task: `CPU-01-SNAPSHOT-BUGDATA-01`
- Gate: **FIXTURE_VERIFIED**; the GPU readback, file sink, `frameNum` and poll frequency are
  contracts

## Block

`0x25db56` is **not a function**: it is the BugData block of `0x25d8b0` (`0x25d8b0..0x25dcee`,
1086 B, sha `e3eeb549…`), reached by `je 0x25da58`. Its only caller is `0x25c913` inside
`0x25c080`, a per-tick virtual present in three vtables; "once per frame" is inferred, not proven.

- Gate: the byte at `[*(0x2eed670)]` (the `"snapshotBugData"` variable), cleared through `0xba40`
  at `0x25db7b` **before** `0x1bdf30` is called at `0x25db9e`.
- `0x1bdf30(*(0x2ee68c8), *(0x2ed3670), "", "")`; both strings are the empty literal.
- Screenshot: `0x961a0` is GPU readback (fence wait, target creation, size query, readback,
  memcpy); it cannot be modelled offline. When it succeeds, list B receives
  `("Screenshot", "bug_screenshot.png")`; otherwise a synthetic `"Command"` node with
  `name="screenshot"` and value `"bug_screenshot"` is emitted at `0x1be236..0x1be256`.
- Adoption: `0xa70d10(&ctx+0x48, root)` → `0xa70c30` creates the `"Frame"` node (with `frameNum`)
  and `0xab0d40` links the root by pointer; ownership transfers, no copy.

## Native seam

`snapshot_bug_data(RegistryOwner&, ReportItem&, SnapshotBugDataHooks)`:

- `armed`, `arg2_present`, `query_screenshot`, `adopt_root`, `frame_num`, list-B insert seam and
  the snapshot hooks.
- `armed == false` does nothing; when armed the gate is cleared first.
- The synthetic `"Command"` is appended between the list-A and list-B phases, matching
  `0x1be236`; the snapshot hook flag carries that decision.
- A list-B insert failure stops before the report build; a missing adopt hook leaves `adopted`
  false but reports `built`.

## Fixture

1. `armed=false`: no nodes, no query, no adoption.
2. `armed=true, arg2=false`: one `"Command"`, list B empty, adopted.
3. `armed=true, arg2=true, query=false`: one `"Command"`.
4. `armed=true, arg2=true, query=true`: no `"Command"`, one `"Attachment"` with the Screenshot
   name/value, list B cleared.
5. Missing adopt hook: built but not adopted.
6. Screenshot insert without an allocator: stops before the build.

## Checker

`check_fsnapshot_bug_data.py` pins the three body hashes, the gate-clear target `0xba40`, the
build target `0x1bdf30`, the synthetic literals/stack text, and the native
query → insert → build → adopt order.

## Limits

The GPU readback, the file sink, the `frameNum` source and the `0x25c080` poll frequency cannot be
modelled offline. The Frame holder's upper bytes are treated as zero ("no frame yet").
