# SAV-01-SAVEDATA-UMOUNTWITHBACKUP-01 — 2026-10-20, `/root`

`STATIC_PATH_BYTE_VERIFIED` (one of two callsites). Worker (Agent subagent) used one REA 6.7.0 / Ghidra 12.1.4 `function` query (exit 0, 376.16 s, private snapshot copy, no retry). Root replayed `verify_callsite_function.py` (exit 0, status `PASS_STATIC_CALLSITE_FUNCTION_ATTRIBUTION_PARTIAL_TASK`) and both `shasum -c` runs (all OK).
- `sceSaveDataUmountWithBackup`: symbol 278, NID `VwadwBBBJ80`, slot `0x1600e00`, `libSceSaveData`; thunk `0xf91020` (`ff25dafd6600`). Two raw `E8 rel32` candidates: `0xa48db9` and `0xa4bcdb` (the latter inside the already-attributed `FUN_00a4ab90`; re-decoded only).
- `0xa48db9` (`e862825400`) is in `FUN_00a48280`, 15 ranges `0xa48280..0xa4a36a` (8276 B of an 8427 B span), in range `0xa48da0..0xa48f5c`. `ev_51166d4706e9b186e21a38419d6485a831ad6e3b42fdf86d284c9ca04399e0e7`.
- Dir `local/re/evidence/sav01-savedata-umountwithbackup-01/`; verifier `a6ef70d6…5411`, `verification.json` `edf89851…0bfda`, `function-callsite/SHA256SUMS` `9ec096a4…3400`, top-level `SHA256SUMS` `d11b54de…1f85`.
- Not established: reachability, runtime binding, ABI/return codes, save/backup/mount semantics, boundary completeness (gaps between the 15 ranges unchecked), provider caller `0xa4d330`, thunk indirect/tail-jump uses. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
