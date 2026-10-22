# Checkpoint — App vslot `0x533100` shadowrandomevents attach

- Local: 2026-10-22 ~13:14 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-RANDOMEVENTS-ATTACH-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why randomevents attach)

Always-on sequential after E436 shadowrandomevents load ends at `0x533bc5`. Prefer
over success atexit (flag-true only; measured fail) and over deepening
already-hooked `0x14c2f0` / `0x6a5370`. Prefer over empty-path migrate/non-null.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x533bc5..0x533cdb` (278 B) sha256 `f0a8fbd3…` |
| Fail path | measured `success_flag=false` → join @ `0x533c3f` |
| Install | loaded into `*[f08+0x60]`; flag byte `f08+0x68` |
| Callees | lift `0x14ccb0`; hooks `0x14ccc0` / `0xf590` (bodies not modelled) |
| Stop | before next load @ `0x533cdb` |

## Not modelled

Success atexit cluster; bodies of `0x14d880` / `0x14d2d0` / `0x14ccc0` / `0x95580`;
next path; deepen `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_randomevents_attach.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-randomevents-attach-20261022/` (manifest `e22d8d3c…` @ `f36ff9c`; capture `0133a43a…`) |
| Pairs | still **155** |

## Measured

`rand_fail=1` + install + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

Next load / success atexit past `0x533cdb`, or bounded `0x14c2f0`/`0x6a5370`.
Goal open.
