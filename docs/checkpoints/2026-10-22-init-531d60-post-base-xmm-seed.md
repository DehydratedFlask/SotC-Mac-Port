# Checkpoint — Init continue xmm BSS seed

- Local: 2026-10-22 ~15:15 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-XMM-SEED-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E458 ymm inter-cell.
OBS xmm seed at `0x531f07`: load 16 B from `0x1014be0` (`0,1,0,0` f32),
dword @ `0x16042d8`, overwrite head of `0x160430c` (+0..+0x23). Stop before
`mov ecx,4` / ymm reload→`0x160433c` / CP10 stack / `0x26800`/`0xf590`
tourism. Prefer this XMM-class Init continue over deepen `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370` and over staffroll `0x7f7220` body.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531f07..0x531f3c` (53 B) sha256 `97400c25…` |
| Seed | 16 B `0x1014be0` → `0x16042d8` + head of `0x160430c` |
| Stop | before `mov ecx,4` @ `0x531f3c` |

## Not modelled

Ymm reload→`0x160433c`; CP10/stack; `0x26800` / `0xf590` bodies;
`0x7f7220` body; helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` /
`0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_xmm_seed.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-xmm-seed-20261022/` (manifest `cdbc6477…` @ `7597ced`; capture `4249ce0a…`) |
| Pairs | still **155** |

## Measured

`pbxmm=2` + `pbic=2` + `pbymm=4` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

Ymm reload→`0x160433c` / CP10 call-site, or bounded `0x1290` / `0x5c64c0` /
`0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370` fragments.
Goal open.
