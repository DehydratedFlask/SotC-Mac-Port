# Checkpoint — Init continue Shadow Boss Outro Jump call-sites

- Local: 2026-10-22 ~15:36 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-SBOJ-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E462 Shadow Boss Jump.
OBS "Shadow Boss Outro Jump" string-pair at `0x531fe2..0x532031` (79 B):
stack/string prep then `call 0x26800` @ `0x532024` and `call 0xf590` @
`0x53202c`. Claim the **call-sites**, not helper bodies or following
`call 0x52eb50`. Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
`0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370` and over staffroll/PLT
body tourism.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531fe2..0x532031` (79 B) sha256 `f91d9940…` |
| Hooks | `call_26800` + `call_f590` (1 each) |
| Stop | before `call 0x52eb50` @ `0x532031` |

## Not modelled

Stack/string prep; `0x26800` / `0xf590` bodies; `0x52eb50`; staffroll/PLT
bodies; helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
`0x140d90` / `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_sboj.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-sboj-20261022/` (manifest pending; capture `7c23acdc…`) |
| Pairs | still **155** |

## Measured

`pbsboj268=1` + `pbsbojf590=1` + `pbsbj268=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`call 0x52eb50` call-site (not body) after `0x532031`, or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
Prefer call-site continue over bodies. Goal open.
