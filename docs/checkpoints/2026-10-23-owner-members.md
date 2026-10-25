# Checkpoint — native `init_owner_members` driver

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-OWNER-MEMBERS-01`
- Gate: **FIXTURE_VERIFIED**; all 18 non-trivial ctor bodies remain hooks

## Row table

21 rows in execution order. 19 slot stores + 2 self-registered:

| kind | rows | action |
|---|---|---|
| global stores | 1,2,3,5,6,7,9,10,12,13,14,15,19,20,21 | `state.globals[rva] = object` |
| this fields | 11 `+0x3b48`, 16 `+0x3b50`, 17 `+0x3b68`, 18 `+0x3b70` | `state.this_fields[off] = object` |
| self-registered | 4 `&0x2eec4d8`, 8 `&0x2eec4e0` | ctor receives the cell address; no store |

Row 11 allocates inside the `0xa2b960` bag factory; row 19 is the `0x1bdb10` registry. Row 3 takes
`[0x2ee2d00]` as its ctor arg; rows 17/18 take `[0x2edf140]`.

## Three destroy idioms

| idiom | rows | native |
|---|---|---|
| virtual deleting dtor `[old_vptr+8]` | 1,2,3,5,6,11,16 | `hooks.virtual_dtor(old)` |
| direct dtor + `0x170b0(old)` | 9,10,12,13,14,15,17,18,19,20,21 | `hooks.dtor(old)` then `deallocate(old)` |
| no replace check | 7 | store unconditionally; the old object is not destroyed |

## Fixture

- First install: 19 stored + 2 self-registered, no destruction.
- Second install: 18 replaced (7 virtual, 11 dtor+free, 11 frees); row 7's old object is
  deliberately not destroyed and remains live, matching the bytes.
- Args: row 3 receives row 1's object; rows 17/18 receive row 9's; rows 4/8 receive their cells.
- Failure surfaces: missing ctor at row 5 stops after four rows and frees the new object;
  allocation failure at row 3 stops after two; a missing virtual-dtor hook fails before storing
  row 1; missing allocator/deallocator fails before row 1.

## Checker

`check_fowner_members.py` pins the 21 new sites (20 × `0x17070`, row 11 → `0xa2b960`), both destroy
idiom byte shapes, row 7's unconditional store, and the native table tokens.

## Limits

- Every ctor body is a host hook; none is synthesised. The 18 blocked bodies from E570 are
  unchanged.
- The post-construct wiring between rows 9 and 12, the mode/gate dispatch, and the registry/property
  blocks are separate modules.
- Nothing here runs the game.
