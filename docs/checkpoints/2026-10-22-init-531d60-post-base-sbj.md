# Checkpoint — Init continue Shadow Boss Jump call-sites

- Local: 2026-10-22 ~15:31 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-SBJ-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E461 `0x26800`/`0xf590` call-sites.
OBS "Shadow Boss Jump" string-pair at `0x531f93..0x531fe2` (79 B): stack/string
prep then `call 0x26800` @ `0x531fd5` and `call 0xf590` @ `0x531fdd`. Claim the
**call-sites**, not helper bodies. Prefer over deepen `0x1290` / `0x5c64c0` /
`0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370` and over
staffroll/PLT body tourism.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531f93..0x531fe2` (79 B) sha256 `80bc46b9…` |
| Hooks | `call_26800` + `call_f590` (1 each) |
| Stop | before next string setup @ `0x531fe2` ("Shadow Boss Outro Jump") |

## Not modelled

Stack/string prep; `0x26800` / `0xf590` bodies; Outro Jump pair; `0x52eb50`;
`0x7f7220` body; helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` /
`0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_sbj.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-sbj-20261022/` (manifest `a920e869…` @ `44d7474`; capture `9647eceb…`) |
| Pairs | still **155** |

## Measured

`pbsbj268=1` + `pbsbjf590=1` + `pb268=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

"Shadow Boss Outro Jump" call-sites after `0x531fe2`, or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
Prefer call-site continue over `0x26800`/`0xf590` bodies. Goal open.
