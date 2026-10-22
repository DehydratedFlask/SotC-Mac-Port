# Checkpoint — Init continue ymm reload→`0x160433c`

- Local: 2026-10-22 ~15:20 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-YMM-RELOAD-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E459 xmm BSS seed.
OBS ymm reload at `0x531f3c`: 0x30 B copy `0x160430c`→`0x160433c`. Fragment
bytes include intervening CP10/stack prep for later `0x26800`, but claim the
**AVX BSS copy**, not CP10/stack or helper bodies. Prefer over deepen
`0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370` and over staffroll `0x7f7220` body.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x531f3c..0x531f71` (53 B) sha256 `afdfaa6c…` |
| Copy | 0x30 B `0x160430c`→`0x160433c` |
| Stop | before `vxorps` / `call 0x26800` @ `0x531f86` |

## Not modelled

CP10/stack materialization; `0x26800` / `0xf590` bodies; `0x7f7220` body;
helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
`0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_ymm_reload.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-ymm-reload-20261022/` (manifest `a5240c88…` @ `48cea50`; capture `a4a62b1b…`) |
| Pairs | still **155** |

## Measured

`pbyr=1` + `pbxmm=2` + `pbic=2` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`call 0x26800` / `0xf590` (claim call-site not body), or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
Goal open.
