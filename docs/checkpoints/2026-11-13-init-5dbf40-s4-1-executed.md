# 2026-11-13 — `0x5b9f90` S4.1 + S5.1 executed

Task: `CPU-01-INIT-5DBF40-S4-1-01`. Branch `cursor/init-hash-551140-6aa93e8`, resumed from
`7619a143` at the recorded offset `0x5bc3d4` (S4).

## What changed

- `native/core/init_5b9f90_slices.{hpp,cpp}`: S4's call pair is decomposed at callee boundaries.
  **S4.1** executes the `0x5dbf40..0x5ddcad` "Particle" page (7,533 B, 1,243 instructions, 177
  direct calls, single `ret` `0x5ddcad`, no backward branch) with 20 byte-pinned items: one group
  under "Shadow Render", two `0xc580` ranged ints (`0x15c8450` caller tag; Soft Particle Mode
  -1..2, Debug Particle Index -1..100), five `0xc300` bounded floats (functor `0x15cbda0`;
  0.05..5, 0..50, 0.01..50, 0..50, 0.01..50) and twelve `0x20af0` bool variables (final vptr
  `0x1594348`, functor `0x15cbdc0`, atexit `0xacf0`). The bool builder now takes its functor vptr
  per page.
- **S4.2** (`0x7f8930..0x7f898a`, 26 instructions) is recorded `blocked` with its measured reason:
  single call `0x7f895b -> 0x7f8610` (688 instructions to the first `ret` `0x7f87d9`, no model) and
  the host globals `0x3288478`/`0x3288480` (plus a `0x15fe570` touch).
- **S5.1** executes `0x5bc413..0x5bc78c` (889 B, 149 instructions): group "Stereo 3D" under "CP10"
  and bounded floats Auto Focal Distance (cell `0x2fedb58`, guard `0x2fedc28`, 0..500) and Auto
  Distortion (cell `0x2fedc30`, guard `0x2fedd00`, 0..1), slider ranges 1.0/1.0, 4 releases,
  2 destroys. **S5.2** (`0x5bc78c..0x5c1196`, 18,954 B, plus the 53 B prologue at `0x5bc3de`) is
  blocked on item extraction.
- Resume offset is now **`0x5bc78c`**. Body extent **10,173 of 46,881 B (21.7 %)**; native total
  **23,232 B (49.6 %)**.

## Verification

| gate | command | result |
|---|---|---|
| build | `cmake --build local/build/orch-r22 -j 12` | exit 0 |
| CTest | `ctest --test-dir local/build/orch-r22 -j 12` | **704/704** |
| slice checker | `python3 -I tools/lift/check_init_5b9f90_slices.py` | OK `executed=10173B items=53 executed_total=23232B` |
| slice map | `python3 -I tools/lift/slice_5b9f90.py` | 10 slices, S5.1 executed / S5.2 blocked |
| runtime | `./local/build/orch-r22/sotc_init_window` | exit 0, `ok=true`, `post_calls_executed=26`, blocked 1, count 27, resume `0x5bc78c`, boss `0xA7` |
| sweep | `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | see HANDOFF |

Runtime counters: `slices=10 groups=10 commands=38 toggles=8 floats=4 named_vars=19 registered=104
releases=178 destroys=66 separators=6 sliders=17 int_sliders=2 contracts=2 records=10 atexit=38
s4_items=20 s4_bytes=7533 s5_items=3 s5_bytes=889`.

## Not claimed

Cold boot, audio, save/reload and Metal validation stay **NOT RUN**. `0x2170` extent is still not
closed, so S1 stays a contract and `post_calls_complete=false`. G1 **PARTIAL**.
