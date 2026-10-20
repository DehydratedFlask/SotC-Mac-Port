# AUD-02-NGS2-VOICEHANDLE-INTERVALS-07A/07B — 2026-10-20, `/root`

Four REA 6.7.0 / Ghidra 12.1.4 `function` queries, run as two parallel sequential pairs on private snapshot copies (all exit 0, 365.9–372.3 s, no retry). Root replayed both `verify_intervals.py` (exit 0, `PASS_STATIC_INTERVALS_ATTRIBUTION`) and both `shasum -c SHA256SUMS` (all OK). Provider: 4 successes, 0 failures. The two verifiers each count against the prior baseline (`remaining-candidates-7`: 97); their counts are merged here.

## 07A — `remaining-candidates-8a/`
- `FUN_00949dc0` `0x949dc0..0x949efb` (316 B, contiguous), exact match; upgraded `0x949df4`. `ev_71d26fef9228daaa1fbf008a6dc3d807456b225987d1627d4ec0c5a03e965d9a`.
- `FUN_00949f00` `0x949f00..0x949f7e` (127 B, contiguous), exact match; upgraded `0x949f34`. `ev_e463b2c3149e42a1c4440d1a2c0d840ddb1c55a4eb9fab9e2b32e21c42893cc5`.
- Hashes: verifier `6376888b…c23d`, `verification.json` `b3ce6f56…5134`, `SHA256SUMS` `cc11cfbd…7781`.

## 07B — `remaining-candidates-8b/`
- `FUN_00953220` `0x953220..0x953330` (273 B, contiguous), exact match; upgraded `0x9532da`. `ev_60174d476523761275731a363c2c6adfa2a5f58e04b796b92206a8d17c2f0a16`.
- `FUN_00952e10` `0x952e10..0x952f74` (357 B, contiguous), exact match; upgraded `0x952f0a`. `ev_4937fda55f2ae195ac8269c71123d007992998ce6e8775b4572e22fab4f9220b`.
- `0x95338e`, `0x95341e`, `0x9534be` lie in separate functions, not in either body.
- Hashes: verifier `e40e0931…fbb6`, `verification.json` `2faba32e…5883`, `SHA256SUMS` `51dd06e9…6753`.

## Merged tally
101 of 104 candidates Ghidra-confirmed (97 + 2 + 2); 3 heuristic-only (`0x95338e, 0x95341e, 0x9534be`); 0 unattributed. No mismatches anywhere in the heuristic-vs-Ghidra comparisons (every queried body matched exactly).

Not established: reachability, runtime binding, ABI/NGS2 semantics; provider callers are observations only; 3 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
