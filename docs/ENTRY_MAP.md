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
| Declared RVA constants | **1,414** |
| Distinct guest RVAs | **966** |
| Headers contributing | **373** |
| RVAs claimed by more than one header | **299** |

## What the numbers mean

- **951 distinct addresses is small against the program.** The CFG census
  ([CFG_CENSUS](CFG_CENSUS.md)) finds **55,790** unwind ranges in the same binary, so the current
  native entry map covers on the order of 1–2 % of the executable. That is the honest scale of the
  lifting work done so far.
- **Duplicates are expected and healthy.** Shared helpers are referenced by many modules: `0xf590`
  is named by 24 headers, `0x17070` (allocation wrapper) by 11, and the `0x2ee2cf8` global cell by
  11 `vslot_40_533100_*` modules. The one vtable slot resolved by the indirect-frontier card
  (`0x20d60`) was already named by the ConCmd modules, which corroborates that it is a
  "register once" entry. A duplicate means "many units call this address", not "conflict".
- **The table mixes code and data targets.** It records the literal constant each header uses; a
  `…Target` constant is usually a call target, a `…Cell`/`…Slot` constant is usually a global
  address. Only 62 RVAs are named by a `Target`/`Rva`-style constant. Treating an entry as callable
  requires the owning module's checker, not this table.
- **Concentration:** the Init body window `0x531d60..0x534100` alone contributes **273** distinct
  entries, so the per-instruction Init lifting dominates the map.

## What is still missing

1. **Entry point type.** Nothing here says whether an address starts a function, is called
   mid-function, or is data. The CFG census gives function starts for 55,790 ranges; intersecting
   the two tables is the next step (`CPU-01-AOT-CFG-SCALE-01` continuation).
2. **ABI.** A native entry today is `bool f(const Cells&) noexcept`-style with explicit hooks, not a
   retail ABI call. Calling one from lifted code needs an address → callable-thunk map.
3. **Indirect targets.** 105 indirect call sites are in the `0x531d60` closure and remain
   unresolved (see CFG_CENSUS). Those are exactly the sites this map cannot help until their target
   set is known.

## Limits

- Generated from header text; a constant that is declared but never used still appears.
- No semantic or behavioural claim: presence in the map means a module names the address, nothing
  more.
- It cannot be regenerated from a clean checkout without the headers it scans (those are public),
  but any claim about the mapped addresses still needs the private ELF checkers.
