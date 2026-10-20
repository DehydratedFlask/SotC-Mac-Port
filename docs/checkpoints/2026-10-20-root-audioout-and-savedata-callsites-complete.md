# AUD-02-AUDIOOUT-OUTPUT-01 and SAV-01-SAVEDATA-MOUNT2-01 — 2026-10-20, `/root`

Two parallel REA 6.7.0 / Ghidra 12.1.4 `function` queries on private snapshot copies (both exit 0, no retry; 2 successes, 0 failures). Root replayed both verifiers (exit 0) and all four `shasum -c` runs (all OK). Static path only; these are the first probes of the AudioOut and SaveData imports.

## AUD-02-AUDIOOUT-OUTPUT-01 — `STATIC_PATH_BYTE_VERIFIED` (one callsite)
- `sceAudioOutOutput`: symbol 19, NID `QOQtbeDqsT4`, slot `0x1600d30`, `libSceAudioOut`; thunk `0xf90e80` (`ff25aafe6600`). 5 raw `E8 rel32` candidates: `0x9abdc2, 0x9abe30, 0x9abfc4, 0x9ac030, 0x9acb5e`.
- Chosen `0x9abdc2` (`e8b9505e00`) in `FUN_009ab360` (`0x9ab360..0x9abeca`, 2923 B, contiguous); `0x9abe30` also lies inside (two of five). Ghidra query 373.73 s, `ev_0b6ffc5f48aab81a351bc2971582ccc6c04274057dad07ad4b31e225246850cb`.
- Dir `local/re/evidence/aud02-audioout-output-01/`; verifier `87ccf5f1…d2d4`, `verification.json` `1f9f2631…cd48`, `function-callsite/SHA256SUMS` `243f53d3…3f84`, top-level `SHA256SUMS` `65a3c94a…daeb3`.
- Unattributed: `0x9abfc4, 0x9ac030, 0x9acb5e`. Provider callers `0x994ee0`, `0x9ad000` unverified.

## SAV-01-SAVEDATA-MOUNT2-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceSaveDataMount2`: symbol 273, NID `0z45PIH+SNI`, slot `0x1600dd8`, `libSceSaveData`; thunk `0xf90fd0` (`ff2502fe6600`), the only FF25 thunk to that slot. Exactly one raw `E8 rel32` candidate: `0xa4aa60` (`e86b655400`).
- In `FUN_00a4a940`, ranges `0xa4a940..0xa4aa42` + `0xa4aa50..0xa4ab4e` (514 B, span 527); callsite in the second range. Ghidra query 368.19 s, `ev_58084b2dd1e4e3c0d66d7d3b14aa7101a6451f02f55ca96ad23a3c9666a3ba3e`.
- Dir `local/re/evidence/sav01-savedata-mount2-01/`; verifier `ec168eee…32f5`, `verification.json` `68eb639d…41e2`, `SHA256SUMS` `d9a6444c…417e`.
- Provider callers `0xa48280`, `0xa4ab90` unverified; indirect/tail-jump uses of the thunk not excluded.

Not established (both): reachability, runtime binding, ABI/return codes, audio output semantics, save format/serializer, function-boundary completeness. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
