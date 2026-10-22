# Checkpoint — App vslot `0x533100` cp3shadowprecache attach

- Local: 2026-10-22 ~13:55 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-CP3SHADOWPRECACHE-ATTACH-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why cp3shadowprecache attach)

Always-on sequential after E442 cp3shadowprecache load ends at `0x533f97`. Prefer
over success atexit (flag-true only at `0x533fec`; measured fail) and over
deepening already-hooked `0x14c2f0` / `0x6a5370`. Prefer over empty-path
migrate/non-null. Host is `self`; slot `*[self+0x3cc8]`; flag `self+0x3cd0`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x533f97..0x53403c` (165 B) sha256 `da6b2404…` |
| Fail path | measured `success_flag=false` → skip notify cluster |
| Install | loaded into `*[self+0x3cc8]`; flag byte `self+0x3cd0` |
| Callees | lift `0x14ccb0`; hooks `0x14ccc0` / `0xf590` (bodies not modelled) |
| Stop | before next (`0x140d90` esi=9) @ `0x53403c` |

## Not modelled

Success atexit cluster; bodies of `0x14d880` / `0x14d2d0` / `0x14ccc0` / `0x95580`;
`0x140d90`; deepen `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_cp3shadowprecache_attach.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-cp3shadowprecache-attach-20261022/` (manifest `bd8349ca…` @ `4d22fd2`; capture `cc94fde5…`) |
| Pairs | still **155** |

## Measured

`csp_fail=1` + install into `self+0x3cc8` + mid_reg=4 → 275 registry;
alloc_ops=13; tip AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

`0x140d90` / success atexit past `0x53403c`, or bounded
`0x14c2f0`/`0x6a5370`. Goal open.
