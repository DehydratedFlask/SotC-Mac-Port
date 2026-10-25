# Checkpoint — list-B reader/clear and the bug-data report tree

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-APPINFO-LISTB-READER-01`
- Gate: **FIXTURE_VERIFIED**; no runtime claim

## Reader (`0x1be2f7..0x1be3e5`, 239 B, sha `af6ee6af…`)

- FIFO from `container+0x20` (`sentinel.next`) via `node+0x00` (`node.next`). The insert at
  `0x1bdc80` is a tail insert, so this is insertion order.
- Reads exactly two node fields: `node+0x30` (value `String.data`) and `node+0x10` (name
  `String.data`). Size/capacity/control are never read; text is re-measured with `strlen`, so
  embedded NULs truncate.
- Allocates a `0x1a0` node and a `0xd0` value (`0x17070`, flags 1) per entry, constructs
  `"Attachment"` (`0xfb3e4c`) and kind-4 value, copies the value text into the value object, sets
  property `"name"` (`0xfb3e34`) to the name text, links value under node and node under root.
- `container+0x30` is never read anywhere in `0x1bdf30`; the walk terminates at the sentinel.

## Clear (`0x1be458..0x1be49c`, 69 B) and reset (`0x1be4a4..0x1be4b6`, 19 B)

- Per node: release value (`+0x30`) then name (`+0x10`) through `0xf590`, free through `0x170c0`
  (tag 2). The next pointer is saved before the free.
- After the loop: both sentinel links point at the sentinel and the full 8-byte count is stored 0.
  The reset runs for an empty list too, so the operation is idempotent.
- `0x1be3f4..0x1be457` is the list-A drain, not part of this slice.

## Report object model

- `0xab1b20` (317 B) node ctor: no allocation inside; vptr `0x15d4ac0`, map vptr `0x15d4e58`,
  tail-call to the deep-copy assign.
- `0xab0ad0` (121 B) value ctor, kind 4; `0xab0d40` (61 B) is an intrusive child-list append
  (parent `+0x80`/`+0x88`, child `+0x70`/`+0xb8`/`+0xc0`), not a vector.
- `0xab2ac0` (688 B) copies key and value, last-write-wins by `memcmp`; `0x1beaa0` (349 B) is the
  deep-copy assign. The report must not borrow registry text: the source strings are released by
  the next list phase.

## Native work

- `native/core/report_tree.{hpp,cpp}`: `ReportItem` with copied text, `std::map<std::less<>>`
  last-write-wins properties, and owned children via `std::unique_ptr` plus the intrusive sibling
  links. `report_node_new`, `report_value_new`, `report_link_child`, `report_set_property`.
- `native/core/appinfo_listb.{hpp,cpp}`: FIFO serialize and idempotent clear over the existing
  `Container1beeb0`/`List1bdc80` layout. A value root or a null free hook is a visible failure and
  leaves the list untouched.
- List-B field semantics corrected to `next` at `+0x00` / `prev` at `+0x08` (the old names were
  inverted even though the bytes were right). `insert_1bdc80` now allocates through the recovered
  `0x17090` wrapper (flags 2), pairing with the `0x170c0` free, instead of `0x17070` (flags 1).
- Fixture `appinfo_listb`: order, `"Attachment"`/`"name"`/value shape, deep-copy proof, last-write-
  wins, clear counts/order/reset, idempotence, null free hook, a real shared refcount, and a value
  root rejection.
- Checker `check_flistb_report.py`: 10 function hashes, 11 byte patterns, both literals, three
  vtable address points, and the native constants.

## Verification

| command | result |
|---|---|
| `cmake --build local/build/orch-r22 -j 12` | 359/359 targets |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **350/350 passed** |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **447 passed / 0 skipped / 0 failed** |

## Limits

- The screenshot query `0x961a0` that decides whether list B is populated, and the root adoption
  `0xa70c30`/`0xa70d10` into `[game+0x48]`, remain contracts.
- The native report is a behaviour model, not the guest 0x1a0/0xd0 ABI.
- Nothing here demonstrates a running game.
