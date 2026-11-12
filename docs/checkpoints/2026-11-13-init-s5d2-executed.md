# 2026-11-13 — `0x5b9f90` S5.2 executed

Task `CPU-01-INIT-S5-2-01`, branch `cursor/init-hash-551140-6aa93e8`, resumed at `0x5bc78c`.

## What changed

`0x5bc78c..0x5c1196` (18,954 B, 3,085 instructions) is no longer "blocked on item extraction". The
"Dynamic 3D" page and the "Shadow Boss Worm Hole Effect" page execute as **56 byte-pinned items**:
2 groups (`0x5bc78c`, `0x5c0daf`), 9 `0x20af0` bool variables, 28 `0xc300` bounded floats with
`0x27120` sliders, 7 `0xc580` ranged ints (incl. 16..256, -1..INT32_MAX, 0..INT32_MAX), 7 `0x26b60`
separators and 3 `0x26f90` keyframe commands targeting `0x2dfcbd0`. The `c580` path clamps
float-encoded bounds to `INT32_MIN/MAX`; item/bool/ranged/record capacities are 192/32/64/16.
Resume offset **`0x5c1196`**.

## Measured

Body extent **10,173 -> 29,127 of 46,881 B (62.1 %)**; native total **23,232 -> 42,186 B (90.0 %)**.
Counters: slices 11, groups 12, commands 41, separators 13, sliders 45, int_sliders 9, bools 28,
registered 160, releases 279, destroys 120, records 11, items 160, s5_items 59, s5_bytes 19,843.

## Verification

| gate | result |
|---|---|
| build | exit 0 |
| full CTest | **704/704** |
| `check_init_5b9f90_slices.py` | OK `executed=29127B executed_total=42186B` |
| map generator | `executed_bytes=29127`, `total_executed_bytes=42186`, resume `0x5c1196`, S5.2 executed |
| init runtime | exit 0, stderr 0 B, `ok=true`, `post_calls_executed=27`, blocked 1, `post_call_count=28` |
| sweep | **914 passed / 0 skipped / 0 failed** |
| entry map | 3,042 constants -> 1,950 RVAs / 584 headers (460 duplicates) |

## Not claimed / open

Cold boot, audio, save/reload, Metal validation: **NOT RUN**. S4.2 (`0x7f8930` -> `0x7f8610` plus
host globals `0x3288478`/`0x3288480`), S6..S9 and `0x2170` remain; `post_calls_complete=false`;
G1 **PARTIAL**.
