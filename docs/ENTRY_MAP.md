# Guest-address → native-entry map (CPU-01-AOT-CFG-SCALE-01)

Which guest RVAs our native modules claim to model, and which header owns each. This is the table a
call dispatcher needs before lifted code can call lifted code by guest address.

- Generated: [`entry_map.json`](entry_map.json) — every `inline constexpr std::uint64_t k…Rva`
  constant in `native/core/` and `native/platform/`.
- Regenerate/verify: `python3 tools/lift/gen_entry_map.py` / `--check` (the latter runs as
  `tools/lift/check_entry_map.py` in the checker sweep).
- Static only: no address is resolved at run time, and nothing here was executed.

## Totals

| Metric | Value |
|---|---|
| Declared RVA constants | **3,052** |
| Distinct guest RVAs | **1,957** |
| Headers contributing | **584** |
| RVAs claimed by more than one header | **462** |

## What the numbers mean

- **1,957 distinct addresses is small against the program.** The CFG census
  ([CFG_CENSUS](CFG_CENSUS.md)) finds **55,790** unwind ranges in the same binary, so the current
  native entry map covers about 3.5 % of the executable. That is the honest scale of the
  lifting work done so far.
- **Duplicates are expected and healthy.** Shared helpers are referenced by many modules: `0xf590`
  is named by 54 headers, `0x17070` (allocation wrapper) by 13, and the `0x2ee2cf8` global cell by
  11 `vslot_40_533100_*` modules. The one vtable slot resolved by the indirect-frontier card
  (`0x20d60`) was already named by the ConCmd modules, which corroborates that it is a
  "register once" entry. A duplicate means "many units call this address", not "conflict".
- **Every address is now typed against the ELF** ([ENTRY_CLOSURE](ENTRY_CLOSURE.md)): **830** are
  exact FDE starts, **381** are branch labels inside a function, **12** are PLT stubs, **203** are
  data in the R+X segment, **531** are non-executable image data and **0** are outside the image.
  Only a `function_start` is a dispatchable call entry; the name convention alone is not the
  contract (of 115 `Target`/`Rva`-named addresses, 31 are interior labels and 8 are data).
- **Concentration:** the Init body window `0x531d60..0x534100` alone contributes **273** distinct
  entries, so the per-instruction Init lifting dominates the map.

## What is still missing

1. ~~**Entry point type.**~~ Done 2026-11-13 (`CPU-01-ENTRY-MAP-CLOSURE-01`): every mapped RVA is
   typed against the 55,790 FDE ranges and the PT_LOAD map, and the native runtime answers it by
   binary search (`guest_entry_find`). See [ENTRY_CLOSURE](ENTRY_CLOSURE.md); 141 of the 608
   functions in the `0x531d60` closure have an exact native entry (23.2 %).
2. ~~**ABI shape.**~~ Done 2026-11-13 (`CPU-01-AOT-CFG-CALLABLE-01`): every function start is
   classified from its decoded entry as `retail_entry` (786), `jmp_thunk` (10) or `ret_stub` (34),
   with prologue frame and call-alignment evidence. **93** entries now have real SysV bodies
   (23 noop, 28 constants, 4 zero, 3 byte setters, 2 setters, 31 vtable-pair ctors, 1 ConCmd custom, 1 printf custom), all
   re-derived from complete FDE bodies; stubs are registered only after per-site `rax` liveness
   over indirect and full-FDE direct callers, and the 23 that remain non-callable keep exact RVA,
   measured relocation-slot/image references and a specific blocker. One CFG-resolved site
   (`0x28471` -> vtable `0x1594dc0` slot `0x20` -> `0x20d60`) executes through the host vtable image
   and asserts the invocation result. The rest still need a native signature and host state.
3. ~~**Indirect targets.**~~ Measured and partially resolved 2026-11-13: the `0x531d60` closure has
   **105 frontier functions containing 499 indirect sites** (not 105 sites). A forward basic-block
   CFG now resolves **222** to exact FDE starts and leaves **277 blocked** with a verbatim reason
   (top: 85 runtime stack objects, 138 object-field loads, 42 indexed arithmetic, 9 no reaching
   definition). All 499 sites are listed with offsets in
   [ENTRY_CLOSURE](ENTRY_CLOSURE.md) / `entry_closure.json`.

## Limits

- Generated from header text; a constant that is declared but never used still appears.
- No semantic or behavioural claim: presence in the map means a module names the address, nothing
  more.
- It cannot be regenerated from a clean checkout without the headers it scans (those are public),
  but any claim about the mapped addresses still needs the private ELF checkers.
