# Checkpoint — composed `0x1bdf30` report path

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-APPINFO-SNAPSHOT-01`
- Gate: **FIXTURE_VERIFIED**; the screenshot query and root adoption remain contracts

## Phase order (byte-verified)

| phase | span | action |
|---|---|---|
| 2 | `0x1be14d..0x1be222` | list A → `"FieldEntry"` nodes, `"name"` property, value child; all flags |
| 4 | `0x1be2f7..0x1be3e5` | list B → `"Attachment"` nodes, same shape |
| 5 | `0x1be3f4..0x1be456` | drain list-A `flag == 1` |
| 6 | `0x1be458..0x1be4b6` | release/free list B, reset links and count |

The address order is the execution order. The list-B read precedes the list-A drain even though the
report copies text, so the native composition keeps that order rather than relying on
copy-before-release.

## Native change

- `drain_app_info` is now a separate public operation (phase 5); `collect_app_info` reuses it after
  its optional visitor pass, so the old API and behaviour are unchanged.
- `app_info_snapshot` composes `serialize_list_a` → `appinfo_listb_serialize` → `drain_app_info`
  → `appinfo_listb_clear`.
- Both list-A and list-B entries use the `report_tree` model, which copies text; the registry and
  list-B strings are released immediately afterwards.

## Fixture

- Four list-A entries (flags 1, 1, 0, 1) and two list-B entries:
  - 4 `"FieldEntry"` then 2 `"Attachment"` children, in insertion order;
  - 3 list-A nodes drained, 2 list-B nodes freed, 10 releases total;
  - the flag-0 survivor remains and is the only entry on a second snapshot;
  - mutating the source buffers after the snapshot does not change the report;
  - a missing list-B free hook or a value root fails before any list is serialized, drained or
    cleared.

## Checker

`check_fapp_info_snapshot.py` pins the four phase start bytes, their increasing order, the
`"FieldEntry"`/`"Attachment"`/`"name"` literals, and the native composition order.

## Limits

- The list-C dispatch before phase 2, the `snapshotBugData` gate `0x25db56`, the screenshot query
  `0x961a0`, the synthetic `"Command"` entry, and root adoption `0xa70c30`/`0xa70d10` are not
  modelled. Nothing here runs the game.
