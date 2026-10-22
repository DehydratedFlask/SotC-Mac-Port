# Checkpoint — Init continue cell-install 0x2eff450

- Local: 2026-10-22 ~16:58 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-CELL-2EFF450-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init continue after E481 first `0x544fe0`.
OBS `0x53238c..0x5323a9` (29 B): `lea r14,[0x2eff450]` / load / cmp /
optional vcall `[rax+0x70]` / store rbx. Prefer this lea/cell-install
over `0x544fe0` / `0x17070` / `0x532ed0` / vcall-callee / stack-prep
tourism, over trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll
bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
`0x140d90` / `0x14c2f0` / `0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x53238c..0x5323a9` (29 B) sha256 `ebee4eef…` |
| Cell | `0x2eff450` |
| Hooks | `dtor_vslot70` (optional; measured empty-cell skips) |
| Stop | before `mov edi,0x490` / call `0x17070` @ `0x5323a9` |

## Not modelled

Vcall `[rax+0x70]` callee body; following second `0x17070` / `0x53fd10`;
trampoline / string-helper / staffroll / PLT bodies; helper bodies
`0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_cell_2eff450.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-cell-2eff450-20261022/` (manifest `833b7154…` @ `74cb221`; capture `14f5f463…`) |
| Pairs | still **155** |

## Measured

`pb450=1` + `pb544=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`mov edi,0x490` / second `0x17070` / always-on after `0x5323a9`, or
bounded `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`. Prefer call-site continue over bodies. Goal open.
