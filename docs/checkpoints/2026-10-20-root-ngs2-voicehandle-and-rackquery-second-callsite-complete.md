# NGS2 VoiceHandle callsite + RackQuery second callsite — 2026-10-20, `/root`

Two REA 6.7.0 / Ghidra 12.1.4 `function` queries ran in parallel on private snapshot copies (Agent subagents); root replayed both verifiers (exit 0) and both `shasum -c` (all OK). Provider: 2 successes, 0 failures this round. Root released the VoiceHandle operation as a *new* hypothesis (function query on a callsite) rather than a retry of the timed-out slot-xrefs query.

## AUD-02-NGS2-RACK-VOICEHANDLE-01 — one callsite STATIC_PATH_BYTE_VERIFIED
- Callsite `0x9496a8` (`e8 63 7d 64 00`) -> thunk `0xf91410` (`ff25 e2fb6600` -> slot `0x1600ff8`, symbol 238, `sceNgs2RackGetVoiceHandle`, NID `MwmHz8pAdAo`). Containing function `FUN_009485a0`, `0x9485a0..0x949b70` (5585 bytes). 6 of 104 raw candidates lie inside it (`0x9496a8, 0x94970e, 0x9497d2, 0x9498a2, 0x94996c, 0x949a21`), each a Ghidra UNCONDITIONAL_CALL edge.
- Evidence ID `ev_7cd1fd53a368738d773628f4c4df821b0612640a832abe1c31afbeca14f29fd0` (375.68 s). Dir `local/re/evidence/aud02-ngs2-rack-voicehandle-01/function-009496a8/`; verifier `ad99b44f…9661`, `verification.json` `166a669e…94fe`, `SHA256SUMS` `1ce335ca…cb44`.
- Not established: reachability, runtime binding, ABI/semantics, boundary completeness beyond Ghidra, the other 98 candidates (Ghidra callers list empty).

## AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01 — second callsite and range check
- `0x99fb7b` (`e8 a0 18 5f 00`) -> thunk `0xf91420`, the only one of 15 candidates outside `FUN_0094ea10`; it lies in `FUN_0099f9d0` (`0x99f9d0..0x99fc70`, 673 bytes), whose only caller is `FUN_0094ea10` (call at `0x94f0cf`). The earlier guess that `0x947920` explains the outlier is not supported.
- Offline check: all 14 other candidates decode as E8 rel32 to `0xf91420` and fall inside `FUN_0094ea10`'s three byte ranges. Ghidra did not attribute the 13 extra in-range ones individually.
- Evidence ID `ev_8f3c6ca338b959fdd9e08902049161dd3287b94091271481b99f0962068432fd` (369.2 s). Dir `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/function-0099fb7b/`; verifier `e7ea30a8…aaa8`, `verification.json` `bec69501…1895`, `SHA256SUMS` `b90bdb32…4f31`.
- Not established: reachability, runtime binding, argument meaning, NGS2 contract, instruction-boundary alignment of the 13.

Static analysis only. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
