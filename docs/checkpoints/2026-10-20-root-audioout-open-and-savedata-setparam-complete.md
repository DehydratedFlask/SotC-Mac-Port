# AUD-02-AUDIOOUT-OPEN-01 and SAV-01-SAVEDATA-SETPARAM-01 — 2026-10-20, `/root`

Two parallel REA 6.7.0 / Ghidra 12.1.4 `function` queries on private snapshot copies (both exit 0, no retry; 2 successes, 0 failures). Root replayed both verifiers (exit 0) and all four `shasum -c` runs (all OK). Static path only.

## AUD-02-AUDIOOUT-OPEN-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceAudioOutOpen`: symbol 17, NID `ekNvsT22rsY`, slot `0x1600d20`, `libSceAudioOut`; thunk `0xf90e60` (`ff25bafe6600`), only FF25 thunk to that slot. Exactly one raw `E8 rel32` candidate: `0x9ac2dd` (`e87e4b5e00`), outside `FUN_009ab360`.
- `FUN_009ac1f0`, `0x9ac1f0..0x9ac38f` (416 B, contiguous); body ends with `jmp 0x9ac110` at `0x9ac38b`. Query 374.37 s, `ev_a81cc14510214df76df6655526de152a3d21686de709beb2d7ffe74ff60737d1`.
- Dir `local/re/evidence/aud02-audioout-open-01/`; verifier `0bfe1678…e0fe9`, `verification.json` `c0210d10…bc9b7`, `function-callsite/SHA256SUMS` `d09c9f94…91c3`, top-level `SHA256SUMS` `58c86215…a7de`.
- Provider callers `0x9ab360`, `0x9ac720`, `0x9ac8d0` unverified (`0x9ab360` is the already-attributed `FUN_009ab360`, which contains two `sceAudioOutOutput` callsites).

## SAV-01-SAVEDATA-SETPARAM-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceSaveDataSetParam`: symbol 276, NID `85zul--eGXs`, slot `0x1600df0`, `libSceSaveData`; thunk `0xf91000` (`ff25eafd6600`). Exactly one raw candidate: `0xa4bcb1` (`e84a535400`), outside `FUN_00a4a940`.
- `FUN_00a4ab90`, 20 ranges, `0xa4ab90..0xa4cd1a` (6125 B in an 8587 B span); callsite in range 11 (`0xa4bb90..0xa4bd57`). Query 374.12 s, `ev_83f636d1fe2fd1507c6d735ee8d8ff025f84f293584e0f0fd099f74e5c34c77f`.
- Dir `local/re/evidence/sav01-savedata-setparam-01/`; verifier `4a27febe…7780`, `verification.json` `d3d696f0…40ce`, `function-00a4bcb1/SHA256SUMS` `7a5286aa…62c8`, top-level `SHA256SUMS` `0d7e1ef7…362c`.
- Gaps between the 20 ranges are not byte-verified; provider caller `0xa4d330` unverified.

Not established (both): reachability, runtime binding, ABI/return codes, audio/port semantics, save format, function-boundary completeness, indirect/tail-jump uses of the thunks. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
