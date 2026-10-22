# Checkpoint — App vslot `0x533100` shadowmodels attach

- Local: 2026-10-22 ~12:20 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-SHADOW-ATTACH-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why attach)

Always-on sequential after E426 load-attempt ends at `0x53347f`. Prefer over
shadowskels (later path past `0x533582`) and over deepening already-hooked
`0x14c2f0` / `0x6a5370` (earlier callees, not next edge). Prefer over empty-path
migrate/non-null.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x53347f..0x533582` (259 B) sha256 `debc8b328100…` |
| Fail path | success-flag false → `r14=0` @ `0x5334f6` (measured default) |
| Install | loaded into `*[f08+0]`; lift `0x14ccb0` / hook `0x14ccc0` / `0xf590` |
| Stop | before shadowskels @ `0x533582` |

## Not modelled

Success atexit cluster; bodies of `0x14d880`/`0x14d2d0`/`0x14ccc0`/`0x95580`;
shadowskels; `0x14c2f0` / `0x6a5370` bodies.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_shadow_attach.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-shadow-attach-20261022/` (manifest `d0e3702e…` @ `e473659`; capture `b1eb543e…`) |
| Pairs | still **155** |

## Measured

`fail=1` + install into f08+0; marker + refc=1 + mid_reg=4 → 275 registry;
alloc_ops=13; tip AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

Shadowskels past `0x533582`, success atexit, or bounded `0x14c2f0`/`0x6a5370`.
Goal open.
