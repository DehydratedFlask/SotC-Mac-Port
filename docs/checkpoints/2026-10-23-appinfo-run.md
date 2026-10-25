# Checkpoint — composed `0x1bdf30` run path

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-APPINFO-RUN-01`
- Gate: **FIXTURE_VERIFIED**; the gate, screenshot query and adoption remain contracts

## Composition

`run_app_info_1bdf30(RegistryOwner&, ReportItem&, AppInfoSnapshotHooks)`:

1. `RegistryOwner::dispatch()` — list-C FIFO, native delegates only, identity-only `0x1290` nodes
   skipped (`0x1bdf66` loads `container+0x40`, `0x1bdf79` follows `node+0x00`).
2. `app_info_snapshot()` — list-A `FieldEntry`, list-B `Attachment`, list-A `flag==1` drain,
   list-B release/free/reset.

A dispatch failure stops before serialization.

## Fixture

- Product delegate + Game delegate: 5 entries installed, then 5 `FieldEntry` children and 5 drained,
  no list-B entries.
- A null-fn delegate fails dispatch and leaves the report empty.
- Delegate nodes are freed through the owner seam.

## Checker

`check_fapp_info_run.py` pins the `0x1bdf30` prologue, the two dispatch bytes, and the native
dispatch-before-snapshot order.

## Limits

The `snapshotBugData` gate `0x25db56`, `0x961a0`, the synthetic `"Command"` entry and
`0xa70c30`/`0xa70d10` root adoption are not modelled. Nothing here runs the game.
