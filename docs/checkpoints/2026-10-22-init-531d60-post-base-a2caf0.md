# Checkpoint — Init continue first 0xa2caf0 call-site

- Local: 2026-10-22 ~17:28 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-A2CAF0-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init continue after E487 lea r13 pin `0x32f4fe0`.
OBS `0x5323f0..0x5323f5` (5 B): `call 0xa2caf0`. Prep `0x5323ec..0x5323f0`
(4 B: `mov rdi,[r13]`) demoted. Prefer call-site over mov-rdi prep /
`0xa2caf0` body tourism, over trampoline/`0x52eb50` / `0x26800`/`0xf590` /
staffroll bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
`0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x5323f0..0x5323f5` (5 B) sha256 `8ca56826…` |
| Target | `0xa2caf0` |
| Prep demoted | `0x5323ec..0x5323f0` (4 B) `mov rdi,[r13]` |
| Stop | before second `mov rdi,[r13]` @ `0x5323f5` |

## Not modelled

`mov rdi,[r13]` prep; `0xa2caf0` body; following stack prep; trampoline /
string-helper / staffroll / PLT bodies; helper bodies `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_a2caf0.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-a2caf0-20261022/` (manifest `69e1b21f…` @ `096cc41`; capture `505ffc04…`) |
| Pairs | still **155** |

## Measured

`pba2=1` + `pblea=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`mov rdi,[r13]` / stack prep / always-on after `0x5323f5`, or bounded
`0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`. Prefer call-site continue over bodies. Goal open.
