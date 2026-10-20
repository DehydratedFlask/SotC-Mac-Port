# SAV-01-SAVEDATA-INITIALIZE3-01 — 2026-10-20, `/root`

`STATIC_PATH_BYTE_VERIFIED` (one callsite). Worker (Agent subagent) used one REA 6.7.0 / Ghidra 12.1.4 `function` query (exit 0, 372.42 s, private snapshot copy, no retry). Root replayed `verify_callsite_function.py` (exit 0) and both `shasum -c` runs (all OK).
- `sceSaveDataInitialize3`: symbol 268, NID `TywrFKCoLGY`, slot `0x1600db0`, `libSceSaveData`; thunk `0xf90f80` (`ff252afe6600`). Exactly one raw `E8 rel32` candidate: `0xa4d295` (`e8e63c5400`), in `FUN_00a4d250` (`0xa4d250..0xa4d2b1`, 98 B, contiguous). `ev_0295feceb47fa6f8fae073e22899cd46c6a159131dcc6181fb1ce1e0c75e0c83`.
- Dir `local/re/evidence/sav01-savedata-initialize3-01/`; verifier `c8e34cf6…ddc0`, `verification.json` `61a53795…d30d`, `function-00a4d295/SHA256SUMS` `5217e7c2…d600`, top-level `SHA256SUMS` `b4f3fe9f…885d`.
- Not established: reachability, runtime binding, ABI/arguments, save format; provider caller `0xa52b80` unverified; thunk indirect/tail-jump uses not excluded. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
