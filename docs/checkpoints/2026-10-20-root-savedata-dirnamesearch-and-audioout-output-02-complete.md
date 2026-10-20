# SAV-01-SAVEDATA-DIRNAMESEARCH-01 and AUD-02-AUDIOOUT-OUTPUT-02 — 2026-10-20, `/root`

Three REA 6.7.0 / Ghidra 12.1.4 `function` queries (all exit 0, no retry; 3 successes, 0 failures). Root replayed both verifiers (exit 0) and all `shasum -c` runs (all OK). Static path only.

## SAV-01-SAVEDATA-DIRNAMESEARCH-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceSaveDataDirNameSearch`: symbol 270, NID `dyIhnXq-0SM`, slot `0x1600dc0`, `libSceSaveData`; thunk `0xf90fa0` (`ff251afe6600`), only FF25 thunk to that slot. Exactly one raw `E8 rel32` candidate: `0xa47f58` (`e843905400`).
- `FUN_00a47d60`, 3 ranges `0xa47d60..0xa47fa9`, `0xa47fb0..0xa48091`, `0xa480a0..0xa4823e` (1227 B of a 1247 B span); callsite in the first range. Query 381.29 s, `ev_8b08975e7844084c4e1d1b0eb7bd15c84ebc39e734f7b566a1205d4e20df7e62`.
- Dir `local/re/evidence/sav01-savedata-dirnamesearch-01/`; verifier `cf0f7c57…d2d0f`, `verification.json` `487555ab…b1905`, `function-callsite/SHA256SUMS` `1c46cd03…c5eb`, top-level `SHA256SUMS` `14a6c9ac…7748`.
- Caveat (recorded honestly): this worker copied the snapshot left by the preceding UmountWithBackup run, whose hash (`d6f3b0fd…8bd51`) differs from the pristine template snapshot (`32f2c278…`); the verifier asserts the hash actually used. The Ghidra body matches the pattern of its sibling attributions, but the pristine-snapshot provenance is weaker for this one. Provider caller `0xa4d330` unverified; gaps between the 3 ranges unchecked.

## AUD-02-AUDIOOUT-OUTPUT-02 — all 5 `sceAudioOutOutput` candidates attributed
- Thunk `0xf90e80` (slot `0x1600d30`). `FUN_009ab360` (`0x9ab360..0x9abeca`): `0x9abdc2, 0x9abe30`. `FUN_009abed0` (`0x9abed0..0x9ac105`, 566 B): `0x9abfc4, 0x9ac030`, query 380.83 s, `ev_f01e569d7a869f19b1a0791392c2c92805b9fc1b7f55a685e136cac2a845948a`. `FUN_009aca40` (`0x9aca40..0x9acc01`, 450 B): `0x9acb5e`, query 369.72 s, `ev_ff429d91a5f2adc4acd299672948fe9a3c273d398247d6e7e7bb91088af9edb8`. Both new queries used pristine `32f2c278…` snapshot copies.
- No Ghidra edge to the thunk outside the 5 candidates; the three bodies do not overlap. Ghidra lists `0x9ab360` as a caller of `FUN_009abed0` (provider observation only).
- Dir `local/re/evidence/aud02-audioout-output-02/`; verifier `5de50b3e…e537`, `verification.json` `a94ceab1…5f83`, `SHA256SUMS` `53da03a4…c889`.

Not established (both): reachability, runtime binding, ABI/return codes, audio/save semantics, thunk indirect/tail-jump uses, function-boundary completeness. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
