# AUD-02-NGS2-VOICEHANDLE-REMAINING-01 — 2026-10-20, `/root`

Worker (Agent subagent) attributed the 98 remaining raw `E8 rel32` candidates for thunk `0xf91410`; root replayed `verify_remaining.py` (exit 0) and `shasum -c SHA256SUMS` (all OK). Provider: 2 successes, 0 failures (REA 6.7.0 / Ghidra 12.1.4, private snapshot copies). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates/`.

| Class | Count | Basis |
|---|---|---|
| Ghidra-confirmed, existing evidence | 23 | 22 edges in `FUN_0094ea10` (`ev_0fd925bf…`), `0x99fbfc` in `FUN_0099f9d0` (`ev_8f3c6ca3…`) |
| Ghidra-confirmed, new queries | 13 | `FUN_0099b400` (`0x99b400..0x99b764`, 869 B, holds `0x99b50a`, `ev_7b1a3e90faf591ab33335e5daa3ea69e2e667ec76cfc29ebf3ca6349a3a0a11e`, 360.77 s); `FUN_0094b2c0` (`0x94b2c0..0x94d8f0`, 9759 B, 3 ranges, holds 12, `ev_bd980829ab91e66bdb697a072a9e5ed034684c28731efb45283747443e873b1a`, 366.97 s) |
| Heuristic-only | 62 | 18 scanner intervals; largest `0x950850..0x951deb` (5532 B) holds 27, not queried |
| Unattributed | 0 | — |

Scanner (HEURISTIC, `scan_function_starts.py`): 16-aligned `55 48 89 e5` after ret/jmp/call terminator and optional padding. Calibration 6/6 known starts, 3/3 ends, 0 interior false hits; all 4 heuristic intervals matched the new Ghidra bodies exactly. Precision beyond these few calibration points is unmeasured.

Hashes: `verify_remaining.py` `00d00f94…79b3`, `verification.json` `2bb28873…1aa2`, `SHA256SUMS` `5f7e0fe6…679f`.

Observations only (not claims): provider-reported callers `0x9485a0` for `FUN_0099b400`; `0x981aa0` and `0x9838b0` for `FUN_0094b2c0`. Not established: reachability, runtime binding, ABI/semantics, boundaries of the 62 heuristic-only candidates, completeness of Ghidra bodies, instruction boundaries beyond the objdump sweep. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
