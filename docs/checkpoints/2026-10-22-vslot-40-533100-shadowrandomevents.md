# Checkpoint — App vslot `0x533100` shadowrandomevents load-attempt

- Local: 2026-10-22 ~13:09 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-SHADOWRANDOMEVENTS-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why shadowrandomevents)

Always-on sequential after E435 streams attach ends at `0x533b62`. Prefer over
success atexit (flag-true only; measured fail) and over deepening already-hooked
`0x14c2f0` / `0x6a5370`. Prefer over empty-path migrate/non-null.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x533b62..0x533bc5` (99 B) sha256 `7d993116…` |
| Notify | `0x95580(*f08 + 0x50)` before load |
| Path | `$/sound/shadow/****/shadowrandomevents.crmp` @ `0x1014f0f` |
| Cells | loader `0x2ee2cf8`; f08 `0x2efc9e0`; future attach `f08+0x60` |
| Callees | `0x14c2f0` (hooked), vcall +0x18 (hooked), `0x14ccb0` (lifted) |
| Stop | before success-flag branch @ `0x533bc5` |

## Not modelled

Success atexit cluster; randomevents attach into `f08+0x60`; bodies of `0x14c2f0` /
`0x95580` / vcall; asset I/O.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_shadowrandomevents.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-shadowrandomevents-20261022/` (manifest `232affbe…` @ `5988cb6`; capture `d0b9a0f6…`) |
| Pairs | still **155** |

## Measured

`rand_refc=1` + notify + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

Randomevents attach / success atexit past `0x533bc5`, or bounded
`0x14c2f0`/`0x6a5370`. Goal open.
