# Checkpoint — Init continue vcall [rax+0x38]

- Local: 2026-10-22 ~17:20 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-VCALL38-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init continue after E485 cell-install `0x2eff448`.
OBS `0x5323db..0x5323e5` (10 B): `mov rax,[r12]` / `mov rdi,r12` /
`call [rax+0x38]`. Prefer this virtual-call continue over vcall-callee /
`0x53fd10` / `0x17070` / stack-prep tourism, over trampoline/`0x52eb50` /
`0x26800`/`0xf590` / staffroll bodies, and over deepen `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x5323db..0x5323e5` (10 B) sha256 `a7107e2d…` |
| Site | `call [rax+0x38]` @ `0x5323e2` |
| Hooks | `call_vslot38` (callee body NOT lifted) |
| Stop | before `lea r13` @ `0x5323e5` |

## Not modelled

Vcall `[rax+0x38]` callee body; r12/vtable producers; following `lea r13`;
trampoline / string-helper / staffroll / PLT bodies; helper bodies `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_vcall38.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-vcall38-20261022/` (manifest `75533939…` @ `333043c`; capture `a669aba2…`) |
| Pairs | still **155** |

## Measured

`pbv38=1` + `pb448=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`lea r13` / always-on after `0x5323e5`, or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`. Prefer call-site continue over bodies. Goal open.
