# AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01 callsite 0x94ebc2 + AUD-01-XFF-EDGE-01 — 2026-10-20, `/root`

## AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01 — one callsite attributed (STATIC_PATH_BYTE_VERIFIED for that callsite)
Worker (Agent subagent) used one REA 6.7.0 / Ghidra 12.1.4 function query (exit 0, 361.9 s, 1 success / 0 failures, Evidence ID `ev_0fd925bf55ace3db6ec209c0b8b14dc5ed51ac17a80e18f6352aa8534aac4f3c`). Root re-ran `python3 -I verify_callsite_function.py` (exit 0) and `shasum -c SHA256SUMS` (all OK).
- Callsite `0x94ebc2` bytes `e8 59 28 64 00` -> thunk `0xf91420` (`jmp [rip+disp]` -> slot `0x1601000`, imports symbol 239). Containing function `FUN_0094ea10`, entry `0x94ea10`, 7554 bytes in 3 ranges to `0x9507a6`.
- Evidence dir: `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/function-0094ebc2/`; verifier SHA-256 `44f2cf93…6ded7`, `verification.json` `4851ffcc…f372`, `SHA256SUMS` `bb1132d1…66c4`.
- NOT established: reachability, runtime slot binding, ABI/arg meaning, NGS2 behavior, attribution of the other 14 candidates (13 lie in `FUN_0094ea10`'s byte ranges but Ghidra did not attribute them), provider's caller `0x947920` unverified; function boundary comes from Ghidra only.

## AUD-01-XFF-EDGE-01 — FIXTURE_VERIFIED
New test: `str` input of minimum member length raises `TypeError('member data must be bytes-like')`. No defect; mutation (guard removed in scratch copy) fails the test. 8->9 tests pass (root re-ran). `scripts/test_audio_bank_header.py` `ed6d6ec9…3bdd`; log `local/checkpoints/aud01-xff-edge-test.log`.

No install changes. G0 PARTIAL, G1-G6 NOT RUN.
