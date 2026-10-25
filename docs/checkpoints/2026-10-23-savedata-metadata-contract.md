# Checkpoint — SaveData metadata / icon / delete contract

- Local: 2026-10-23 (America/Chicago)
- Agent: static-analysis and design worker
- Task: `SAV-03-METADATA-CONTRACT-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**; not implemented

## `0x530` metadata block

At `0xa4b56d` (state 9) the original `memset`s `0x530` bytes, then `strcpy` (not `strncpy`) into
`+0x000`, `+0x080`, `+0x100`, writes a `u32` at `+0x500`, and leaves the `0x2C` bytes at `+0x504`
never written. `sceSaveDataSetParam(&mount, 0, block, 0x530)` runs at `0xa4bcb1` and its result is
**never read** (`0xa4bcb6` goes to state 12 unconditionally).

- Title is a localized constant (`0x52eb70(3)`); subtitle/detail compose
  `SAVEINFO_PROGRESS_TITLE/DESC[_PLUS|_COMPLETE]` with `{SHRINE}{BOSS}{PLAY_TIME}{MODE}{PLAY_COUNT}{MIRROR}`.
- `userParam` is the slot-flags dword via the `0xa51580` setter.
- `0x530` is also the platform's per-slot stride (`imul …,0x530` at `0xa47ff4`).
- Only `+0x500` is ever read back (`0xa48003`); the texts are platform-dialog-only.

## Icon

The icon is a **shared resource copy**, not a render: `$`-gated name (`0xa51510`) -> FNV id
(`0x14c2f0`) -> `GetResource` (`0xa4cff0`) -> `{data,size,size}` -> `SaveIcon(mount,&desc)` at
`0xa4b84d` (two args, result ignored; a missing icon is not an error; no guest-side size check).
Five regional keys each for `ProgressIcon`/`ProfileIcon`; the level-1 FNV is pinned.

## Delete and dialog status

- Save-state 6 delete at `0xa4badb` (before mount/write), load corrupt-confirm at `0xa48a6e`,
  duplicate at `0xa4bddb` (state 18, unreachable). Wrapper `{u32 user, 0, char* dirName}`; **no
  BUSY retry**; `0x12c3370` maps `0x809f0004->5, 0xa->3, 0xe->4, 0xf->6, else->7`.
- Slot/confirm dialogs need status **3** before `GetResult`; the progress dialog closes on **2**. The
  earlier "3 = finished" simplification is corrected.

## Proposed slice (PROPOSED)

`native/core/save_meta.{hpp,cpp}`: `SaveMeta` `0x530` encode/decode with length checks, `USR-META`
wrapper `SOTCMETA/1/descriptor/data_hash/0x530`, `SaveIcon` with a 1 MiB native cap. Transaction
additions: `SaveStep::{write_meta,write_icon,delete_slot}`, `SaveStatus::data_committed`,
`SaveSlotInfo::{meta,has_meta,meta_matches_data}`, a `SaveAttachments` overload of `save_store`, and
`save_delete` (bak -> meta -> icon -> USR-DATA, no retry), plus pure `map_sce_result` /
`classify_dialog_status`. No `SaveIoOps` change.

15 fixtures planned (M01 golden sha256 `5717bb09…da88`, text/reserved bounds, container, attachments,
invalid bundle, meta/icon failures, delete order/missing/busy/atomicity, pinned tables).

## Corrections filed

`[obj+0x58]` is `+0x500`; `SaveIcon` takes two args; dialog `2` vs `3`; `CheckBackupData`'s result
is discarded; `0x809f0008` is not "internal only" in the mapper; a second delete site exists;
`save_store` does not call `save_check_backup`; F28 (`USR-META`) is now applicable.

## Limits

No real PS4 save or platform metadata exists; the localization text is absent from the ELF; the
level-2 icon id and PNG-ness are unknown; the `0x2C` tail, the SDK struct layouts and state 18
remain open. **No save/load works claim.**
