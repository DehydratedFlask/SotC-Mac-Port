# Checkpoint — Init continue cell-install 0x2eff448

- Local: 2026-10-22 ~17:12 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-CELL-2EFF448-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init continue after E484 first `0x53fd10`.
OBS `0x5323be..0x5323db` (29 B): `lea r14,[0x2eff448]` / load / cmp /
optional vcall `[rax+0x70]` / store rbx. Prefer this lea/cell-install
over `0x53fd10` / `0x17070` / `0x544fe0` / vcall-callee / stack-prep
tourism, over trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll
bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
`0x140d90` / `0x14c2f0` / `0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x5323be..0x5323db` (29 B) sha256 `cd7c96f3…` |
| Cell | `0x2eff448` |
| Hooks | `dtor_vslot70` (optional; measured empty-cell skips) |
| Stop | before `mov rax,[r12]` @ `0x5323db` |

## Not modelled

Vcall `[rax+0x70]` callee body; following vcall `[rax+0x38]`; trampoline /
string-helper / staffroll / PLT bodies; helper bodies `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_cell_2eff448.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-cell-2eff448-20261022/` (manifest `8727a4a8…` @ `ae80da5`; capture `f0018976…`) |
| Pairs | still **155** |

## Measured

`pb448=1` + `pb53=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

Vcall `[rax+0x38]` / always-on after `0x5323db`, or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`. Prefer call-site continue over bodies. Goal open.
