# Checkpoint — Init continue ymm inter-cell copy

- Local: 2026-10-22 ~15:10 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-YMM-INTERCELL-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E457 ymm BSS seed.
OBS inter-cell ymm copies at `0x531ece`: 0x30 B from `0x1604278`→`0x16042a8`
and `0x16042dc`→`0x160430c`. Stop before xmm seed @ `0x531f07` and before
later `0x26800` / `0xf590` string-helper tourism. Prefer this AVX-class Init
continue over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
`0x140d90` / `0x14c2f0` / `0x6a5370` and over staffroll `0x7f7220` body.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531ece..0x531f07` (57 B) sha256 `7ea46663…` |
| Copies | 0x30 B `0x1604278`→`0x16042a8`; `0x16042dc`→`0x160430c` |
| Stop | before xmm seed @ `0x531f07` |

## Not modelled

Xmm seed @ `0x531f07+`; `0x26800` / `0xf590` bodies; `0x7f7220` body;
helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_ymm_intercell.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-ymm-intercell-20261022/` (manifest `6ec43158…` @ `530b980`; capture `5a95a2b8…`) |
| Pairs | still **155** |

## Measured

`pbic=2` + `pbymm=4` + `pbf9=1` + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

Xmm seed continue @ `0x531f07`, or bounded `0x1290` / `0x5c64c0` /
`0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370` fragments.
Goal open.
