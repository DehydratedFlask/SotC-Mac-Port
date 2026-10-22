# Checkpoint — Init continue lea r13 pin 0x32f4fe0

- Local: 2026-10-22 ~17:25 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-LEA-32F4FE0-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init continue after E486 vcall `[rax+0x38]`.
OBS `0x5323e5..0x5323ec` (7 B): `lea r13,[0x32f4fe0]`. Prefer this lea pin
over vcall+0x38 callee / `0x53fd10` / `0x17070` / stack-prep tourism, over
trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll bodies, and over
deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`. Prefer over following `mov rdi,[r13]` /
`call 0xa2caf0` body tourism.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x5323e5..0x5323ec` (7 B) sha256 `b0ec4308…` |
| Cell | `0x32f4fe0` |
| Hooks | host `r13_cell` / `r13_out` pin |
| Stop | before `mov rdi,[r13]` @ `0x5323ec` |

## Not modelled

`mov rdi,[r13]`; `call 0xa2caf0` body; following stack prep; trampoline /
string-helper / staffroll / PLT bodies; helper bodies `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_lea_32f4fe0.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-lea-32f4fe0-20261022/` (manifest `5dc17310…` @ `49925b3`; capture `4ed13cda…`) |
| Pairs | still **155** |

## Measured

`pblea=1` + `pbv38=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`mov rdi,[r13]` / `call 0xa2caf0` / always-on after `0x5323ec`, or
bounded `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`. Prefer call-site continue over bodies. Goal open.
