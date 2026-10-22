# Checkpoint — App vslot `0x533100` shadowbossmusic load-attempt

- Local: 2026-10-22 ~13:20 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-SHADOWBOSSMUSIC-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why shadowbossmusic load)

Always-on sequential after E437 randomevents attach ends at `0x533cdb`. Prefer
over success atexit (flag-true only; measured fail continues) and over deepening
already-hooked `0x14c2f0` / `0x6a5370`. Prefer over empty-path migrate/non-null.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x533cdb..0x533d3e` (99 B) sha256 `5898927c…` |
| Notify | `0x95580(f08+0x60)` |
| Path | `$/sound/shadow/****/shadowbossmusic.crmp` |
| Cells | loader `0x2ee2cf8`; f08 `0x2efc9e0`; r15=`f08+0x70` (attach later) |
| Callees | hooks `0x14c2f0` / vcall+0x18; lift `0x14ccb0` |
| Stop | before success-flag branch @ `0x533d3e` |

## Not modelled

Success atexit cluster; bossmusic attach/install; bodies of `0x14c2f0` / `0x95580` /
vcall; asset I/O.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_shadowbossmusic.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-shadowbossmusic-20261022/` (manifest `bc789fce…` @ `f5b0079`; capture `ab3efceb…`) |
| Pairs | still **155** |

## Measured

`boss_refc=1` + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

Bossmusic attach / success atexit past `0x533d3e`, or bounded
`0x14c2f0`/`0x6a5370`. Goal open.
