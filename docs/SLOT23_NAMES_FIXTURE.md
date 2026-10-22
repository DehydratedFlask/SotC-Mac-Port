# Slot 23 (`0xe71990`) name-string fixture — recipe only

**Policy:** private asset-only / fixture import (Project store `docs/roadmap-revised.md`).  
**Never** commit the fixture binary, original strings, `eboot.bin`, or `local/re/eboot.elf`.

## Purpose

Ctor index 23 fills a 14-record name/hash table. Hashes are recovered immediates in
`native/core/leaf_e71990.hpp` (`kLeafE71990Hash`). The 14 name pointers are original image
addresses; native code loads the strings from a **private** fixture and supplies process
pointers at run time.

## Regenerate (owner machine with private ELF)

```bash
python3 -I scripts/extract_slot23_names_fixture.py
```

Inputs (private, gitignored):

- `local/re/eboot.elf` — pinned sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`

Outputs (private, gitignored):

- `local/fixtures/slot23_e71990_names.bin`
- `local/fixtures/slot23_e71990_names.meta.json` (lengths / RVAs / hashes / per-string sha256 only)

## Binary format (`S23N` v1)

| Field | Size | Notes |
|---|---|---|
| magic | 4 | `S23N` |
| version | u32 LE | `1` |
| count | u32 LE | `14` |
| per record | u32 LE length + raw bytes | **no** trailing NUL in file; loader appends NUL |

Records 0–12: loader requires `name_hash(string) == kLeafE71990Hash[i]`.  
Record 13: sentinel hash immediate is literal `0` (not computed from the string).

## Runtime

- Env override: `SOTC_SLOT23_NAMES_FIXTURE=/abs/path/to/slot23_e71990_names.bin`
- If unset, the loader probes a few repo-relative paths under `local/fixtures/`.
- Missing / corrupt / hash-mismatch → slot-23 callback sets `StartupState::init_failed` (fail-visible).

Focused CTest sets the env var via CMake so CI/local runs that have the private fixture pass.

## Related

- Lift + hash: `docs/checkpoints/2026-10-21-agentB-namehash-e71990.md`
- Byte checker: `tools/lift/check_fe71990.py` (private ELF)
- Native: `native/core/slot23_names_fixture.*`, `leaf_e71990.*`, `name_hash.*`
