# AUD-02-NGS2-VOICEHANDLE-INTERVALS-06 — 2026-10-20, `/root`

Two sequential REA 6.7.0 / Ghidra 12.1.4 `function` queries (both exit 0, 361.66 s and 362.13 s, private snapshot copy, no retry). Root replayed `verify_intervals.py` (exit 0, `PASS_STATIC_INTERVALS_ATTRIBUTION`) and `shasum -c SHA256SUMS` (all OK). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates-7/`.

- `FUN_0094dfe0` (query `0x94e0a9`): `0x94dfe0..0x94e19b`, 444 B, contiguous; exact match; upgraded `0x94e0a9`. `ev_06760f3948b1c8661925851f5d5d2ee8fda4352203918052415b5e3023265066`.
- `FUN_0094e270` (query `0x94e2bd`): `0x94e270..0x94e3eb`, 380 B, contiguous; exact match; upgraded `0x94e2bd`. `ev_94e7e242ba42a205d6960d1224ccada2e2e00a10b1b0d6b5b3267bdeaf6bcf66`.
- No mismatches. Tally: 97 of 104 candidates Ghidra-confirmed; 7 heuristic-only (`0x949df4, 0x949f34, 0x952f0a, 0x9532da, 0x95338e, 0x95341e, 0x9534be`); 0 unattributed.
- Hashes: `verify_intervals.py` `543ae90c…66fa`, `verification.json` `90b3db0f…2313`, `SHA256SUMS` `4e6ef50f…e0cb`.
- Not established: reachability, runtime binding, ABI/NGS2 semantics; provider callers are observations only; 7 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
