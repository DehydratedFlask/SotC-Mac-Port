# AUD-02-NGS2-VOICEHANDLE-INTERVALS-05 — 2026-10-20, `/root`

Two sequential REA 6.7.0 / Ghidra 12.1.4 `function` queries (both exit 0, 366.49 s and 365.17 s, private snapshot copy, no retry). Root replayed `verify_intervals.py` (exit 0, `PASS_STATIC_INTERVALS_ATTRIBUTION`) and `shasum -c SHA256SUMS` (all OK). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates-6/`.

- `FUN_009549e0` (query `0x954b1a`): `0x9549e0..0x954bb3` + `0x954bc0..0x954c55`, 618 B; exact match; upgraded `0x954b1a, 0x954bcd`. `ev_c773c641f0d8f139df0edf855fcd4d684dcbb6e61ed6b628bbf59a19f0bba5b7`.
- `FUN_0094e3f0` (query `0x94e44b`): `0x94e3f0..0x94e9ff`, 1552 B; exact match; upgraded `0x94e44b`. `ev_e58afd5b76af34c34f55257531afb767d639b41d79db8829eb9428c33d79bd19`.
- No mismatches. Tally: 95 of 104 candidates Ghidra-confirmed; 9 heuristic-only across 9 intervals (`0x949df4, 0x949f34, 0x94e0a9, 0x94e2bd, 0x952f0a, 0x9532da, 0x95338e, 0x95341e, 0x9534be`); 0 unattributed.
- Hashes: `verify_intervals.py` `8a6fdba2…3652`, `verification.json` `cd05a96b…a7be`, `SHA256SUMS` `cfa60af5…ba99`.
- Not established: reachability, runtime binding, ABI/NGS2 semantics; provider callers are observations only; 9 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
