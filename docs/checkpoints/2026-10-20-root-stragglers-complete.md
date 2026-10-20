# STRAGGLERS-01 — 2026-10-20, `/root`

Three unattributed raw `E8 rel32` candidates; one REA 6.7.0 / Ghidra 12.1.4 `function` query each, sequential, hash-verified pristine snapshot copies, no retries. **2 successes, 1 provider failure.** Root replayed all three verifiers (exit 0) and every `shasum -c` (0 bad lines). None of the three addresses lay in any of 35 existing Ghidra function records, so no query could be skipped. Static path only. Dirs under `local/re/evidence/`.

| Candidate | Thunk | Result | Evidence |
|---|---|---|---|
| `0x954726` (`stragglers-954726-01/`) | `0xf91570` `sceAudio3dPortSetAttribute` | `FUN_00954680`, `0x954680..0x954773` (244 B); query 369.78 s | `ev_c5b1475a…`; verifier `82b635ae…`, `verification.json` `b78e0ae7…` |
| `0xa4a903` (`stragglers-a4a903-01/`) | `0xf91080` `sceSaveDataDialogOpen` | `FUN_00a4a600`, `0xa4a600..0xa4a92f` (816 B), falls between `FUN_00a48280` and `FUN_00a4a940`; query 367.00 s | `ev_22ac97aa…`; verifier `83bc2199…`, `verification.json` `8089174e…` |
| `0xa4bddb` (`stragglers-a4bddb-01/`) | `0xf90f90` `sceSaveDataDelete` | **No Ghidra attribution.** Query exit 1 (364.72 s): "No procedure exists at the requested address" — the address is in the gap `0xa4bd94..0xa4be81` between two ranges of `FUN_00a4ab90`. Not retried. | verifier `c7922d53…` (`verify_callsite_offline.py`), `verification.json` `1ab2e88f…` |

Offline fallback for `0xa4bddb` (inference only): no aligned `55 48 89 e5` prologue between `0xa4ab90` and the callsite, and a `jmp rel32` at `0xa4be7d` inside the gap lands at `0xa4af93`, inside `FUN_00a4ab90`'s ranges — consistent with the gap belonging to `FUN_00a4ab90`, but unproven.

Established for all three: call bytes decode to the stated thunk; each thunk is the only FF25 thunk for its slot; slot/symbol/NID match `local/re/imports.json`; for the first two, Ghidra's edge, containing function and body hash. Not established: reachability, runtime binding, ABI, indirect/tail-jump uses, function-boundary completeness; no Ghidra attribution for `0xa4bddb`. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
