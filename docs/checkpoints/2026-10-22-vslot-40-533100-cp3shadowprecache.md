# Checkpoint — App vslot `0x533100` cp3shadowprecache load-attempt

- Local: 2026-10-22 ~13:50 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-CP3SHADOWPRECACHE-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why cp3shadowprecache load)

Always-on sequential after E441 cp3Characters attach ends at `0x533f51`. Prefer
over success atexit (flag-true only) and over deepening already-hooked
`0x14c2f0` / `0x6a5370`. Prefer over empty-path migrate/non-null. Prefer load
over attach (attach into `self+0x3cc8` is the next edge).

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x533f51..0x533f97` (70 B) sha256 `77f7b95a…` |
| Notify | `0x95580(self+0x3cb8)` (post-install of cp3Characters) |
| Path | `$/cp3shadow/****/cp3shadowprecache.calt` |
| Cells | loader `0x2ee2cf8`; self from `[rbp-0x300]` |
| Callees | hooks `0x14c2f0` / vcall+0x18 (no `0x14ccb0` in this fragment) |
| Stop | before attach-into `self+0x3cc8` @ `0x533f97` |

## Not modelled

Attach/install into `self+0x3cc8`; `0x14ccb0` (in attach); success atexit;
bodies of `0x14c2f0` / `0x95580` / vcall; asset I/O.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_cp3shadowprecache.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-cp3shadowprecache-20261022/` (manifest `54d200a4…` @ `0c79bf1`; capture `8b018ad3…`) |
| Pairs | still **155** |

## Measured

`csp=` stand-in + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

cp3shadowprecache attach into `self+0x3cc8` / success atexit past `0x533f97`,
or bounded `0x14c2f0`/`0x6a5370`. Goal open.
