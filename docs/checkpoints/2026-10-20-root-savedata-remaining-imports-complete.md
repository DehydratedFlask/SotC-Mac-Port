# SaveData remaining imports — 2026-10-20, `/root`

Ten SaveData import probes (Agent subagents). One new REA 6.7.0 / Ghidra 12.1.4 `function` query (`sceSaveDataTerminate`: exit 0, 370.64 s, hash-verified pristine snapshot copy, no retry); nine reused existing Ghidra records by hash (copied into each evidence dir and hash-pinned). Root replayed all ten verifiers (exit 0) and every `shasum -c` (0 bad lines in all dirs). Static path only. With these, every imported `libSceSaveData` function except the dialog-progress/close helpers (`sceSaveDataDialogClose/GetStatus/IsReadyToDisplay/ProgressBarSetValue/Terminate`) has a byte-verified callsite.

Dirs are `local/re/evidence/sav01-savedata-<name>-01/`. All thunks are the only FF25 site for their slot.

| Symbol (NID, slot, thunk) | Raw candidates | Verified callsite | Function (Ghidra) | Basis |
|---|---|---|---|---|
| DialogInitialize (`s9e3+YpRnzw`, `0x1600e20`, `0xf91060`) | 7 | `0xa488ea` | `FUN_00a48280` | reuse `ev_51166d47…` |
| DialogOpen (`4tPhsP6FpDI`, `0x1600e30`, `0xf91080`) | 10 | `0xa48caf` | `FUN_00a48280` | reuse; `0xa4a903` in no known body |
| DialogGetResult (`yEiJ-qqr6Cg`, `0x1600e10`, `0xf91040`) | 7 | `0xa48a14` | `FUN_00a48280` | reuse |
| DialogUpdateStatus (`KK3Bdg1RWK0`, `0x1600e48`, `0xf910b0`) | 18 | `0xa4893c` | `FUN_00a48280` | reuse |
| GetMountInfo (`65VH0Qaaz6s`, `0x1600dd0`, `0xf90fc0`) | 1 | `0xa4aa93` | `FUN_00a4a940` (`0xa4aa50..0xa4ab4e`) | reuse `ev_58084b2d…` |
| SaveIcon (`c88Yy54Mx0w`, `0x1600de8`, `0xf90ff0`) | 1 | `0xa4b84d` | `FUN_00a4ab90` (`0xa4b6f0..0xa4b903`) | reuse `ev_83f636d1…` |
| Delete (`S1GkePI17zQ`, `0x1600db8`, `0xf90f90`) | 3 | `0xa48a6e` (`0xa4badb` in `FUN_00a4ab90`; `0xa4bddb` unattributed) | `FUN_00a48280` | reuse |
| Terminate (`yKDy8S5yLA0`, `0x1600df8`, `0xf91010`) | 1 | `0xa4d304` | `FUN_00a4d300` (`0xa4d300..0xa4d326`, 39 B) | new query `ev_f7e906c4829dfc8d02b676f6d8bc5a77025de7039635e2f216903b7f7b3c7173` |
| CheckBackupData (`RQOqDbk3bSU`, `0x1600dc8`, `0xf90fb0`) | 2 | `0xa49da2` (`0xa4bd6d` in `FUN_00a4ab90`) | `FUN_00a48280` | reuse |
| RestoreBackupData (`lU9YRFsgwSU`, `0x1600de0`, `0xf90fe0`) | 1 | `0xa48f72` | `FUN_00a48280` | reuse |

Notes: the dialog probes' verifiers also check that Ghidra's edge set to the thunk equals exactly the candidates inside `FUN_00a48280`'s ranges (4 Initialize, 6 Open, 4 GetResult, 11 UpdateStatus). The `Terminate` verifier checks for a final RET rather than the `0xf8f3a0` call used by siblings.

Not established: reachability, runtime binding, ABI/return codes, dialog behavior, save format, indirect/tail-jump uses of the thunks, function-boundary completeness beyond Ghidra, candidates outside the named bodies (`0xa4a903`, `0xa4bddb`). G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
