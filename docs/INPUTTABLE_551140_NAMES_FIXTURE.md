# Input table `0x551140` name-string fixture — recipe only

**Policy:** private asset-only / fixture import (Project store `docs/roadmap-revised.md`).  
**Never** commit the fixture binary, original strings, `eboot.bin`, or `local/re/eboot.elf`.

## Purpose

Registry ctor `0x52a220` hashes each of the 275 input-table name pairs and inserts into
sorted tables A/B. Native fill (`registry_fill_52a220`) needs process pointers for
`name_a` / `name_b`. Strings come from a **private** fixture; hashes are verified with
`fnv_cstr_52a990` / `fnv_cstr_a2ddc0`.

## Regenerate (owner machine with private ELF + decode cache)

```bash
python3 -I scripts/extract_inputtable_551140_names_fixture.py
```

Inputs (private, gitignored):

- `local/re/eboot.elf` — pinned sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`
- `local/validation/inputtable-agent/inputtable_image_PRIVATE.bin` — local pointer-VA cache

Outputs (private, gitignored):

- `local/fixtures/inputtable_551140_names.bin`
- `local/fixtures/inputtable_551140_names.meta.json` (lengths / RVAs / hashes / per-string sha256 only)

## Binary format (`I275` v1)

| Field | Size | Notes |
|---|---|---|
| magic | 4 | `I275` |
| version | u32 LE | `1` |
| count | u32 LE | `275` |
| per record | u32 len_a + bytes; u32 len_b + bytes | **no** trailing NUL; loader appends NUL |

Pinned hash-pair payload sha256 (275 × `{u32 hash_a, u32 hash_b}` LE):

`a1aa059e47b80f1d450d46f47d0885858d04323299c8e01b6a3bd2425f329258`

## Runtime

- Env override: `SOTC_INPUTTABLE_NAMES_FIXTURE=/abs/path/to/inputtable_551140_names.bin`
- If unset, the loader probes a few repo-relative paths under `local/fixtures/`.
- Missing / corrupt / hash-mismatch → loaders return false; fill returns `missing_names` (fail-visible).

Focused CTest sets the env var via CMake when the private fixture exists.

## Related

- Numeric deepen: `docs/INPUTTABLE_551140.md`
- Fill: `native/core/registry_fill_52a220.*`
- Checker: `tools/lift/check_f52a220_fill.py`
