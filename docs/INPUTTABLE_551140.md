# Input table `0x551140` — numeric deepen (recipe)

**Policy:** publishable numeric immediates only. Never commit original name strings,
callback bodies, `eboot.bin`, or `local/re/eboot.elf`.

## Purpose

Ctor `0x551140` (exec index **424**, not a production `recovered_constructors` slot)
fills a 275-entry BSS table at `0x2eff8e0` (stride `0x60`). After three pre-objects,
store region `0x55141d..0x557fe4` writes 2088 stores with 0 calls/branches.

This deepen recovers only the immediate numeric fields:

| Offset | Type | Range |
|---|---|---|
| `+0x10` | `u32 a` | 0..5 |
| `+0x14` | `u32 b` | 0..3 |
| `+0x18` | `u32 c` | 0..3 |
| `+0x50` | `u64 f50` | 0..15 |

Name pointers (`+0x00`/`+0x08`) and callback/vtable state (`+0x20`/`+0x28`/`+0x40`)
stay **null** in native. Init `0x531d60` / registry `0x2efd160` still need keys later
via a private fixture — not this slice.

## Regenerate (owner machine with private ELF)

```bash
python3 -I scripts/extract_inputtable_551140_numeric.py
python3 -I tools/lift/check_f551140.py
```

Pinned digests (ELF sha256 `d0723afb…5861d3`):

- Region `0x55141d..0x557fe4` → `5ecf2186b5a7f35b384a8ec65b69ea2304ace415a19de9b1429212f78e9a3068`
- Numeric payload (275 × `{a,b,c,f50}` LE) → `61c85093797bb62b47f6299c320a3d05d7cf17577434f25360b5ba2c298b9dcb`

## Runtime / tests

- Native: `native/core/input_table_551140.{hpp,cpp}` + generated
  `native/core/input_table_551140_numeric.inc.hpp`
- Accessors modelled: `input_table_551140_base` (`0x54dc10`),
  `input_table_551140_count` (`0x54dc20` → `0x113`)
- CTest: `input_table_551140`

## Related

- Analysis: `CPU-01-INPUTTABLE-551140-ANALYSIS-01` / E391
- Registry context: `CPU-01-REGISTRY-2EFD160-ANALYSIS-01` / E389
- Private layout notes: `local/validation/inputtable-agent/inputtable_layout_PUBLIC.json`
  (local only; not required to build)
