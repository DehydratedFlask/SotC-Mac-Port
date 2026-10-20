# AUD-02-NGS2-VOICEHANDLE-INTERVALS-02 — 2026-10-20, `/root`

Two sequential REA 6.7.0 / Ghidra 12.1.4 `function` queries (both exit 0, 373.9 s and 362.02 s, private snapshot copy, no retry). Root replayed `verify_intervals.py` (exit 0, status `PASS_STATIC_INTERVALS_ATTRIBUTION`) and `shasum -c SHA256SUMS` (all OK). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates-3/`.

- `FUN_00951df0` (query `0x952664`): `0x951df0..0x952e01`, 4114 B, 5 ranges totalling 4070 B; exact match to heuristic interval; 7 candidates upgraded. `ev_1c2f97a05b1bd22369fe19bf5f62796016cf1ef80d0ddeb9cb2a67d74e238617`.
- `FUN_0094aaf0` (query `0x94ac4a`): `0x94aaf0..0x94b2b9`, 1994 B, one range; exact match; 6 candidates upgraded. `ev_f2a184ead5d425a1aa7af40addd077ce8cb178763a9f46d54a684a96dc1dae63`.
- Tally for the 98 candidates beyond the first callsite function: 76 Ghidra-confirmed (23 + 13 + 27 + 13), 22 heuristic-only across 15 intervals, 0 unattributed. Across all 104: 82 Ghidra-confirmed (including 6 in `FUN_009485a0`).
- Hashes: `verify_intervals.py` `5ba5e581…123a`, `verification.json` `640eff66…ed0e`, `SHA256SUMS` `f567b07b…c56a`.
- Not established: reachability, runtime binding, ABI/NGS2 semantics; provider callers are observations only; 22 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
