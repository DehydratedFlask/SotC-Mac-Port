# Checkpoint — Init continue ymm BSS seed

- Local: 2026-10-22 ~15:05 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-YMM-BSS-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E456 continue `0xf90310` call-site.
OBS `vmovups ymm` seed at `0x531e4b`: load 32 B from `0x1014a80`
(`-1,0,0,0,1,0,0,0` f32) into BSS cells `0x1604278` / `0x160426c` /
`0x16042dc` / `0x1604268` with scalar fills. Stop before inter-cell ymm
copies @ `0x531ece` and before later `0x26800` / `0xf590` string-helper
tourism. Prefer this AVX-class Init continue over deepen `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370` and over staffroll `0x7f7220` body.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531e4b..0x531ece` (131 B) sha256 `293909e2…` |
| Seed | 32 B `0x1014a80` → four BSS regions + scalar fills |
| Stop | before inter-cell ymm @ `0x531ece` |

## Not modelled

Inter-cell ymm @ `0x531ece+`; `0x26800` / `0xf590` bodies; `0x7f7220` body;
helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_ymm_bss.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-ymm-bss-20261022/` (see EVIDENCE for tip shas) |
| Pairs | still **155** |

## Measured

`pbymm=4` + `pbf9=1` + mid_reg=4 → 275 registry; alloc_ops=13; tip
AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

Ymm inter-cell continue @ `0x531ece`, or bounded `0x1290` / `0x5c64c0` /
`0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370` fragments.
Goal open.
