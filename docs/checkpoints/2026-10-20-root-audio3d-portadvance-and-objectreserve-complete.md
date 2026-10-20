# AUD-02-AUDIO3D-PORTADVANCE-01 and AUD-02-AUDIO3D-OBJECTRESERVE-01 — 2026-10-20, `/root`

Two parallel REA 6.7.0 / Ghidra 12.1.4 `function` queries, each on a hash-verified pristine snapshot copy (`32f2c278…`; both exit 0, no retry; 2 successes, 0 failures). Root replayed both verifiers (exit 0) and all four `shasum -c` runs (all OK). Static path only.

## AUD-02-AUDIO3D-PORTADVANCE-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceAudio3dPortAdvance`: symbol 8, NID `lw0qrdSjZt8`, slot `0x1601080`; thunk `0xf91520` (`ff255afb6600`). Exactly one raw `E8 rel32` candidate: `0x953a7c` (`e89fda6300`), in `FUN_009536d0` (`0x9536d0..0x953abf`, 1008 B, contiguous). Query 379.57 s, `ev_a6c4e39c135e68d44987d748b5b51e6dfd4c785cb133500e9feb6573e35f595a`.
- Dir `local/re/evidence/aud02-audio3d-portadvance-01/`; verifier `ff10c8d6…a8cd`, `verification.json` `54be5c3b…fdf5`, top-level `SHA256SUMS` `258c1702…7d39`.
- Provider callers `FUN_00997700`, `FUN_00997970`, `FUN_009ac390` unverified.

## AUD-02-AUDIO3D-OBJECTRESERVE-01 — `STATIC_PATH_BYTE_VERIFIED` (one of nine callsites)
- `sceAudio3dObjectReserve`: symbol 5, NID `jO2tec4dJ2M`, slot `0x1601068`; thunk `0xf914f0` (`ff2572fb6600`). Nine raw candidates: `0x94a6b8` (chosen), `0x99b589, 0x99b5b6, 0x99b5e3, 0x99b610, 0x99b63d, 0x99b66a, 0x99b697, 0x99b6c4` (the eight `0x99b5xx..0x99b6xx` ones look like one tight run, likely one function — unverified).
- `0x94a6b8` (`e8336e6400`) in `FUN_0094a510` (`0x94a510..0x94a84d`, 830 B, contiguous). Query 376.07 s, `ev_b592c1780e95ca7a2abbaf0f25e7874a4c119a99288a3af614289b531c2d3250`.
- Dir `local/re/evidence/aud02-audio3d-objectreserve-01/`; verifier `733f0979…3fd9`, `verification.json` `0824296c…da08`, `function-callsite/SHA256SUMS` `4d904b84…3a20`, top-level `SHA256SUMS` `365d1dba…de1f`.
- Provider caller `FUN_00981aa0` unverified. Eight candidates unattributed.

Not established (both): reachability, runtime binding, ABI/argument semantics, audio behavior, indirect/tail-jump uses of the thunks, function-boundary completeness. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
