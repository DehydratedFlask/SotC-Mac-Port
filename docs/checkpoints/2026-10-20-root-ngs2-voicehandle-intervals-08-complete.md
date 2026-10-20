# AUD-02-NGS2-VOICEHANDLE-INTERVALS-08A/08B — final candidates, 2026-10-20, `/root`

Three REA 6.7.0 / Ghidra 12.1.4 `function` queries (08A: 378.63 s and 364.64 s sequential; 08B: 374.69 s in parallel; all exit 0, private snapshot copies, no retry). Root replayed both `verify_intervals.py` (exit 0, `PASS_STATIC_INTERVALS_ATTRIBUTION`) and both `shasum -c SHA256SUMS` (all OK). Provider: 3 successes, 0 failures. Counts below are merged from the two verifiers (each baseline = the 3 remaining after intervals-07).

- 08A (`remaining-candidates-9a/`): `FUN_00953340` `0x953340..0x9533ca` upgraded `0x95338e`, `ev_b699f20a36eabd1032f9ba2f1ee629f132da9a04e92f4ff2b64f996019c0acd1`; `FUN_009533d0` `0x9533d0..0x953460` upgraded `0x95341e`, `ev_f8b1d934e69afeef730316ed911ea7ebe3ba921f7921bdc6ab71d4e199058f27`. Hashes: verifier `ff4aacd7…48b7`, `verification.json` `2d105b0d…195d`, `SHA256SUMS` `7f4eb665…fdfc`.
- 08B (`remaining-candidates-9b/`): `FUN_00953470` `0x953470..0x9534ed` (126 B) upgraded `0x9534be`, `ev_66aeb4479158960526f941107aff244da6149c9ce33375c3db029827398c90c4`. Hashes: verifier `35d3baff…ba75`, `verification.json` `c0a031f7…8e80`, `SHA256SUMS` `1bfdde1a…2d9e`.
- All bodies are contiguous and exactly match the heuristic intervals; no mismatches in any heuristic-vs-Ghidra comparison across the whole series.

## Final tally for `sceNgs2RackGetVoiceHandle` thunk `0xf91410`
All 104 raw `E8 rel32` candidates are now Ghidra-confirmed (7 + 97 -> 104; 0 heuristic-only, 0 unattributed). The earlier 8-function heuristic scanner never disagreed with Ghidra in 30+ intervals.

Not established: reachability, runtime slot binding, ABI/NGS2 semantics; provider callers (`0x945a80`, `0x945c80`, `0x945e30` etc.) are observations only; Ghidra body completeness is the provider's result. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
