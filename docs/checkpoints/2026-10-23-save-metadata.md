# Checkpoint — SaveData metadata / icon / delete slice (`SAV-03`)

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator (implementation from E604)
- Task: `SAV-03-METADATA-01`
- Gate: **FIXTURE_VERIFIED**; synthetic metadata + shared-icon copy + delete; no real save,
  no platform metadata, G2/G5 NOT RUN

## Native

- `native/core/save_meta.*`: the `0x530` block (title/subtitle/detail at `+0x000/+0x080/+0x100`,
  `user_param` at `+0x500`, `0x2C` reserved at `+0x504`), length-checked before any I/O (the guest
  uses unbounded `strcpy`); the native `USR-META` container
  `{SOTCMETA, u32 1, u32 descriptor, u64 data_hash, u64 0x530}` + block; the shared-icon payload
  with a 1 MiB native cap.
- `save_store` overload with `SaveAttachments{meta, icon}`: the data write remains the commit;
  advisory `USR-META`/`icon0.png` failures return `failed_generic` + step but keep
  `data_committed = true`.
- `save_delete`: removes `USR-META`, `icon0.png`, `USR-DATA.bak`, `USR-META.bak`, then `USR-DATA`
  (the commit); no BUSY retry; missing slot -> 4 + `0x809f000e`.
- `save_enumerate` reads `USR-META` best-effort and reports `has_meta`/`meta_matches_data`.
- `map_sce_result` (`0x12c3370`) and `classify_dialog_status` (2 vs 3) are pure native observers.

## Fixture

`test_save_metadata` 102 checks: golden block FNV `0x6bd7d637` / container `0x57959728`
(independently reproduced), text bounds and NUL rejection, reserved/userParam, container
strictness, attachments present/absent, invalid bundle writes nothing, meta/icon failure keeps the
data, delete order/missing/busy/atomicity, the result/dialog tables, and metadata mismatch.

## Checker

`check_fsave_metadata.py` pins the save body (`217f8559…`), the mount2 body (`3cdeeb68…`), the
12-entry result table (`0xff789452` generic + indices 0/6/10/11) and the native `0x530`/delete API.

## Limits

No real platform `USR-META`/icon exists; the text is caller data; `userParam` bit meanings, the
icon resource id/PNG-ness, the `0x2C` tail, the SDK struct layouts and the unreachable state 18
remain open. **No save/load works claim.**
