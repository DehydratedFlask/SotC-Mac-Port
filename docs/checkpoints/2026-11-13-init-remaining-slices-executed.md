# 2026-11-13 — `0x5b9f90` remaining slices executed

Task `CPU-01-INIT-REMAINING-01`, branch `cursor/init-hash-551140-6aa93e8`, resumed at `0x5bc78c`
then `0x5c1196`.

## Executed

- S5 prologue `0x5bc3de..0x5bc413` (53 B) folded into S5.1; S5 now complete (19,896 B).
- S4.2 `0x7f8930` wrapper + `0x7f8610` label prefix. The callee is 457 B straight-line (462
  instructions to the first `ret` `0x7f87d9`) and copies `{0,0,0,1.0f}` from `0x16061a0` after the
  `"--"` prefix. The geometry block `0x7f8664..0x7f87d8` is gated on live debug-tool singletons
  `0x3288478`/`0x3288480` and stays a contract with that offset.
- S6: `0x5c94b0..0x5cf12a` (23,673 B, 3,885 insns, 76 items: Shadow UI/HUD/Map + bounds) and
  `0xb25840..0xb2b6aa` (24,169 B, 60 items).
- S7: `0x5c11a0..0x5c1d50` (2,992 B, 10 items: Shadow Sound / Shadow Cursor / Layers).
- S8: `0x5c1d50..0x5c1e61` (273 B, 54 insns), the 33-iteration loop over the `0x2dcaa90` name table
  building 33 layer bools.
- S9: `0x5c1e61..0x5c56b1` (14,416 B, 2,356 insns, 40 items).

## Measured

body extent **46,876 / 46,881 B (99.99 %)**; native total **108,325 B**; counters slices 18, groups
26, commands 41, separators 29, sliders 132, int_sliders 23, named_vars 116, registered 379,
releases 671, destroys 339, records 18, items 379; resume **`0x5ba041`**; runtime
`post_calls_executed=34`, blocked 1, `post_call_count=35`.

## Gates

build 0; full CTest **704/704**; checker `executed=46876B executed_total=108325B`; sweep
**914/0/0**; runtime exit 0, stderr 0 B, `ok=true`; entry map 3,052 -> 1,957 RVAs / 584 headers
(462 duplicates); project index 114,663.

## Open / not claimed

S1 `0x2170` (>= 28,288 B, no `ret` before `0x9000`) stays a contract; S4.2's geometry block needs
live debug-tool state. `post_calls_complete=false` for those two reasons only. Cold boot, audio,
save/reload and Metal validation stay **NOT RUN**; G1 **PARTIAL**.
