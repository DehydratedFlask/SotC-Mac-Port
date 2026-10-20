# SAV-01-REGION-DOSSIER-01 — 2026-10-20, `/root`

`STATIC_ANALYSIS_VERIFIED`. First function-level characterization of the save region toward SAV-01 (serializer/checksum recovery). Worker (Agent subagent) built a hash-bound dossier from stored Ghidra records plus two new REA 6.7.0 / Ghidra 12.1.4 queries (`0x161f50`: exit 0, 366.8 s, `ev_34c436d7…`; `0xa4d330`: exit 0, 363.9 s, `ev_8bbfdf35…`; hash-verified pristine snapshot copies, no retry). Root replayed `verify_dossier.py` (exit 0) and `shasum -c` (0 bad lines). Private dir `local/re/evidence/sav01-region-dossier-01/` (`DOSSIER.md`, `dossier.json`, verifier `5e4b4bd1…18b0`, `verification.json` `0066161e…6b30`, `SHA256SUMS` `e1d78e5d…6401`). The literal strings read from the game binary stay in that private dir and are intentionally not reproduced here.

## Observations (byte/record verified)
- The save region contains a task dispatcher `FUN_00a4d330` that switches on a task dword at `+0x10`: 1 → `FUN_00a47d60`, 2 → `FUN_00a4ab90`, 3 or 4 → `FUN_00a48280`. (`FUN_00a4d330` is also the provider-reported caller seen in earlier SetParam/UmountWithBackup attributions.)
- Per-function SaveData thunk call order is recorded in `dossier.json`; a raw `E8` byte scan agreed exactly with Ghidra's call sites (0 extra, 0 missing).
- The functions reference path/mode/format literals consistent with a `USR-DATA` file read (`rb`) and write (`wb`) under a mounted save directory, plus source/function-name literals and header/body error messages. Load path call sequence: `snprintf`, `fopen`, `fread`, `fread`, `fclose`, then a hash helper. Save path: hash helper first, then `snprintf`, `fopen`, `fwrite`, `fwrite`, `fclose`.
- Hash helper `FUN_00161f50`: `h = (byte ^ h) * 0x01000193` over the buffer, i.e. the public FNV-1a 32-bit form; both Load and Save pass the standard FNV-1a offset basis `0x811c9dc5` as the third argument. 14 callers are listed, so it is a general-purpose helper.

## Inference (not verified)
- The `USR-DATA` file header is 16 bytes: dword 0 == 1, dword 1 a hash, dword 2 a size ≤ `0x6400000`, dword 3 unreferenced. Neither the algorithm's coverage (which bytes are hashed) nor the header layout has been tested against a real or synthetic save.

## Caveats
- The stored `FUN_00a47d60` record came from a modified snapshot (`d6f3b0fd…`, noted in the DirNameSearch checkpoint); its facts were re-verified against the ELF.
- Unknown: body serialization format, SaveTaskInfo/object struct layouts, who sets `[obj+8]`, `[obj+0x10]` and the buffer at `[obj+0x60]`, the param.sfo field layout, indirect targets (`CALL RAX`, jump tables).

## Next bounded step
Find the producer of the body and header: one `rea function` query on a candidate caller of the buffer-object constructor `FUN_00a51180`, then byte-verify the writes; offline, test the 38 verified hash bytes against any sample save blob (none is available locally — do not download or request one). Not claimed: any save/load works, any checksum matches a real save. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
