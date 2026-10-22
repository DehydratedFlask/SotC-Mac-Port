# Checkpoint — App vslot `0x533100` shadowstreams load-attempt

- Local: 2026-10-22 ~13:15 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-SHADOWSTREAMS-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why shadowstreams)

Always-on sequential after E433 banks attach ends at `0x5339e9`. Prefer over
success atexit (flag-true only; measured fail) and over deepening already-hooked
`0x14c2f0` / `0x6a5370`. Prefer over empty-path migrate/non-null.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x5339e9..0x533a4c` (99 B) sha256 `435ef1b3…` |
| Notify | `0x95580(*f08 + 0x40)` before load |
| Path | `$/sound/shadow/****/shadowstreams.crmp` @ `0x1014ee8` |
| Cells | loader `0x2ee2cf8`; f08 `0x2efc9e0` |
| Callees | `0x14c2f0` (hooked), vcall +0x18 (hooked), `0x14ccb0` (lifted) |
| Stop | before success-flag branch @ `0x533a4c` |

## Not modelled

Success atexit cluster; streams attach into `f08+0x50`; bodies of `0x14c2f0` /
`0x95580` / vcall; asset I/O.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_shadowstreams.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-shadowstreams-20261022/` (manifest `a48c5309…` @ `9b0ecb1`; capture `63dad34f…`) |
| Pairs | still **155** |

## Measured

`streams_refc=1` + notify + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

Streams attach / success atexit past `0x533a4c`, or bounded `0x14c2f0`/`0x6a5370`.
Goal open.
