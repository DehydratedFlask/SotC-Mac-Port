# SaveData dialog-helper imports — 2026-10-20, `/root`

Five probes (Agent subagent). One new REA 6.7.0 / Ghidra 12.1.4 `function` query (GetStatus: exit 0, 362.58 s, hash-verified pristine snapshot copy, no retry; 1 success, 0 failures); four reuse the existing `FUN_00a48280` record `ev_51166d47…` (copied into each dir's `source-record/` and hash-pinned). The worker was interrupted by a session limit and resumed; it reported all five dirs were already complete. Root replayed all five verifiers (exit 0) and every `shasum -c` (0 bad lines). Static path only. With these, every imported `libSceSaveData` function has a byte-verified callsite. Dirs: `local/re/evidence/sav01-savedata-<name>-01/`.

| Symbol (NID, slot, thunk) | Candidates | Verified callsite | Function (Ghidra) | Basis |
|---|---|---|---|---|
| DialogClose (`fH46Lag88XY`, `0x1600e08`, `0xf91030`) | 2 | `0xa48e42` | `FUN_00a48280` (`0xa48da0..0xa48f5c`) | reuse |
| DialogGetStatus (`ERKzksauAJA`, `0x1600e18`, `0xf91050`) | 1 | `0xa4d404` | `FUN_00a4d400` (`0xa4d400..0xa4d40f`, 16 B) | new query `ev_a61008eaa592d8f3c2b5faec4f20c88d268e3b482a0914e6487c41d56f726049` |
| DialogIsReadyToDisplay (`en7gNVnh878`, `0x1600e28`, `0xf91070`) | 3 | `0xa48cd8` | `FUN_00a48280` | reuse |
| DialogProgressBarSetValue (`hay1CfTmLyA`, `0x1600e38`, `0xf91090`) | 12 | `0xa48fb5` | `FUN_00a48280` | reuse |
| DialogTerminate (`YuH2FA7azqQ`, `0x1600e40`, `0xf910a0`) | 2 | `0xa4a33e` | `FUN_00a48280` (range `0xa4a230..0xa4a36a`) | reuse |

Reuse verifiers also check Ghidra's edges to each thunk equal exactly the candidates inside `FUN_00a48280`'s ranges; candidates outside it are labelled by range but not independently attributed here. Provider caller of `FUN_00a4d400` (`0x25bb30`) unverified.

Not established: reachability, runtime binding, ABI/return codes, dialog behavior, save format, indirect/tail-jump uses of the thunks, function-boundary completeness beyond Ghidra. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
