# Checkpoint — App vslot `0x533100` cp3Characters attach

- Local: 2026-10-22 ~13:40 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-VSLOT-40-533100-CP3CHARACTERS-ATTACH-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why cp3Characters attach)

Always-on sequential after E440 cp3Characters load ends at `0x533eac`. Prefer
over success atexit (flag-true only at `0x533f01`; measured fail) and over
deepening already-hooked `0x14c2f0` / `0x6a5370`. Prefer over empty-path
migrate/non-null. Host is `self` (rdi → `[rbp-0x300]`), not f08.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x533eac..0x533f51` (165 B) sha256 `386566fe…` |
| Fail path | measured `success_flag=false` → skip notify cluster |
| Install | loaded into `*[self+0x3cb8]`; flag byte `self+0x3cc0` |
| Callees | lift `0x14ccb0`; hooks `0x14ccc0` / `0xf590` (bodies not modelled) |
| Stop | before next path (cp3shadowprecache) @ `0x533f51` |

## Not modelled

Success atexit cluster; bodies of `0x14d880` / `0x14d2d0` / `0x14ccc0` / `0x95580`;
next path (`$/cp3shadow/****/cp3shadowprecache.calt`); deepen `0x14c2f0` /
`0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f533100_vslot40_cp3characters_attach.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-vslot40-cp3characters-attach-20261022/` (re-pinned after feat tip) |
| Pairs | still **155** |

## Measured

`cp3_fail=1` + install into `self+0x3cb8` + mid_reg=4 → 275 registry;
alloc_ops=13; tip AllocatorFront+FakeHeap; PR #5 merge gap.

## Risks / next

cp3shadowprecache load / attach past `0x533f51`, or bounded
`0x14c2f0`/`0x6a5370`. Goal open.
