# Checkpoint — Init continue lea rbx / vmovq

- Local: 2026-10-22 ~15:55 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-LEA-VMOVQ-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E464 `0x52eb50` call-site.
OBS `lea rbx, [0x32f79a8]` + `vmovq xmm0, [0x15ffa38]` at
`0x532036..0x532045` (15 B). Claim this seed continue — not stack prep /
`call 0x532ca0`, not trampoline/`0x52eb50` / `0x26800`/`0xf590` /
staffroll body tourism, and not deepen `0x1290` / `0x5c64c0` /
`0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x532036..0x532045` (15 B) sha256 `97b753ed…` |
| Effects | rbx cell-address pin `0x32f79a8`; 8 B load from `0x15ffa38` |
| Stop | before `lea rsi` stack prep @ `0x532045` |

## Not modelled

Stack object materialization; `call 0x532ca0`; trampoline/`0x52eb50` body;
`0x26800` / `0xf590` bodies; staffroll/PLT bodies; helper bodies
`0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_lea_vmovq.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-lea-vmovq-20261022/` (manifest `e154ecfd…` @ `e8e32dd`; capture `9a9b3ee4…`) |
| Pairs | still **155** |

## Measured

`pblv=1` + `pb52=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

Stack prep / `call 0x532ca0` call-site after `0x532045`, or bounded
`0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`. Prefer call-site continue over bodies. Goal open.
