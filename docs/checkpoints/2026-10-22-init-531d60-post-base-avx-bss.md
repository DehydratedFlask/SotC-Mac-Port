# Checkpoint — Init continue AVX BSS copy

- Local: 2026-10-22 ~14:40 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-AVX-BSS-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E452 continue `0x5b3cf0` call-site.
OBS AVX BSS xmm copy at `0x531e17..0x531e3c` (37 B): load 16 B from
`0x1b1dd40`, store to `0x1b1dd00` and `0x1b1dd20`. Prefer this always-on Init
continue over staffroll helper body `0x7f7220` (~108 B to first `ret`) and over
deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`. Prefer over empty-path migrate/non-null.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531e17..0x531e3c` (37 B) sha256 `8d66f325…` |
| Behavior | 16 B copy `0x1b1dd40` → `0x1b1dd00` and `0x1b1dd20` |
| Stop | before `call 0x7f7220` @ `0x531e3c` |

## Not modelled

`0x7f7220` body; helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` /
`0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_avx_bss.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-avx-bss-20261022/` (manifest `67b6755a…` @ `0f2e840`; capture `13f94f73…`) |
| Pairs | still **155** |

## Measured

`pbavx=2` + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`0x7f7220` Init continue, or bounded `0x1290` / `0x5c64c0` / `0x56dcb0` /
`0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370` fragments.
Goal open.
