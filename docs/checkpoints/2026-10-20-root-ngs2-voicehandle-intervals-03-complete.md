# AUD-02-NGS2-VOICEHANDLE-INTERVALS-03 — 2026-10-20, `/root`

Two sequential REA 6.7.0 / Ghidra 12.1.4 `function` queries (both exit 0, 361.72 s and 360.49 s, private snapshot copy, no retry). Root replayed `verify_intervals.py` (exit 0, `PASS_STATIC_INTERVALS_ATTRIBUTION`) and `shasum -c SHA256SUMS` (all OK). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates-4/`.

- `FUN_009530c0` (query `0x953134`): `0x9530c0..0x9531f9`, 314 B, one range; exact match; upgraded `0x953134, 0x953184, 0x953192`. `ev_495e67562703f16fef129188ece110ef83cc22dcfd9ac0e6785e4d5df648da91`.
- `FUN_00953ad0` (query `0x953c2d`): `0x953ad0..0x953b17` + `0x953b20..0x953e6c`, 917 B in a 925 B span; exact match; upgraded `0x953c2d, 0x953ced, 0x953dc7`. `ev_de4962697ecd845b26f126de4d560f1e623b2425dc34084239eac8c890574a13`.
- Tally: 88 of 104 candidates Ghidra-confirmed; 16 heuristic-only across 13 intervals; 0 unattributed.
- Hashes: `verify_intervals.py` `c9e32249…9926`, `verification.json` `904a8f01…5aba`, `SHA256SUMS` `64f79ee1…f8e7`.
- Not established: reachability, runtime binding, ABI/NGS2 semantics; provider callers are observations only; 16 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
