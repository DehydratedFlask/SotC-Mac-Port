# 2026-11-13 — Callable entry shapes and the indirect frontier

Task `CPU-01-AOT-CFG-CALLABLE-01`, successor of `CPU-01-ENTRY-MAP-CLOSURE-01` (M1: guest-address →
native-entry map / callable program). Branch `cursor/init-hash-551140-6aa93e8`, base `62f10629`.

## What was missing

The map typed every address but did not say whether an FDE start is a normal SysV entry, a thunk or a
ret stub, and its indirect frontier was a count (105 functions) with no site-level evidence.

## What was built

- `tools/lift/entry_closure.py` now decodes every mapped function start from the pinned image and
  classifies `retail_entry` / `jmp_thunk` / `ret_stub`, recording prologue shape, pushes, `sub rsp`,
  canary and static call alignment. It also walks the closure's 105 frontier functions, and for every
  indirect site performs a same-block backward provenance scan (max 70 instructions, 3 register
  hops), resolving a target only when the base is absolute and the slot contents are an exact FDE
  start; otherwise it records a verbatim reason.
- `GuestEntry` carries `GuestEntryCallable`; the generated `guest_entry_table.inc` has three fields
  plus `kGuestIndirectSummary`. The runtime JSON and `test_guest_entry_table` expose and pin them.
- `check_entry_closure.py` re-derives everything (including objdump disassembly of the pinned ELF),
  compares JSON and `.inc` exactly, and `--mutate` proves both a tampered row and a tampered total are
  detected. An external file tamper was also run (exit 1) and reverted.

## Measured

| metric | value |
| --- | ---: |
| function starts | 830 |
| retail entries | **786** |
| jmp thunks | **10** |
| one-byte ret stubs | **34** |
| frame_pointer / leaf | 626 / 160 |
| aligned / misaligned / dynamic / n/a | 494 / 1 / 131 / 160 |
| frontier functions / indirect sites | 105 / **499** |
| resolved / blocked | **180 / 319** |
| blocked reason kinds | 60 |

Resolved sites go through 15 distinct vtable targets; the top blocker is
`stack_block_boundary` (109), then `block_boundary` (45) and indexed-register arithmetic (44).

## Real bugs fixed

1. The RIP-relative operand helper matched the full two-operand string instead of the source operand,
   so the first resolution pass returned **0/499**.
2. Stack spills were invalidated by an intervening `call`; a callee cannot modify the caller's frame,
   so stack definitions now survive calls (registers do not). Resolved sites rose 0 → 180.

## Gates

build 0; full CTest **705/705**; entry checker **OK indirect=180/499** plus mutation probe; sweep
**915/0/0**; runtime exit 0, stderr 0 B, `ok=true`, `entry_map.indirect={105,499,180,319}`; init
slice totals unchanged at **46,876 / 108,325 B**; entry map 3,052 → 1,957 / 584 / 462; project index
114,665.

## Limits / not claimed

No native ABI thunk body was written; the 786 retail entries prove ELF entry shape, not a callable
native signature. 319 indirect sites remain blocked with exact offsets/reasons. Cold boot, audio,
save/reload and Metal validation stay **NOT RUN**; G1 **PARTIAL**.
