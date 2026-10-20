# AUD-02-AUDIO3D-OBJECTRESERVE-02 and AUD-02-AUDIO3D-OBJECTUNRESERVE-01 — 2026-10-20, `/root`

One new REA 6.7.0 / Ghidra 12.1.4 `function` query (ObjectUnreserve: exit 0, 361.71 s, hash-verified pristine snapshot copy, no retry; 1 success, 0 failures); ObjectReserve-02 used no provider operation. Root replayed both verifiers (exit 0) and all `shasum -c` runs (all OK). Static path only.

## AUD-02-AUDIO3D-OBJECTRESERVE-02 — `STATIC_PATH_BYTE_VERIFIED` (all nine candidates now in Ghidra bodies)
- The eight remaining `sceAudio3dObjectReserve` candidates (`0x99b589, 0x99b5b6, 0x99b5e3, 0x99b610, 0x99b63d, 0x99b66a, 0x99b697, 0x99b6c4`, constant `0x2d` stride) all lie in `FUN_0099b400` (`0x99b400..0x99b764`, 869 B), previously attributed by `ev_7b1a3e90faf591ab33335e5daa3ea69e2e667ec76cfc29ebf3ca6349a3a0a11e` (prior query at `0x99b50a`, which already listed eight extra UNCONDITIONAL_CALL edges to thunk `0xf914f0`). Each re-decodes from ELF bytes as `E8 rel32` → `0xf914f0`. With `0x94a6b8` in `FUN_0094a510`, 9 of 9 candidates are attributed. Edges are restated from existing evidence, not re-derived.
- Dir `local/re/evidence/aud02-audio3d-objectreserve-02/`; verifier `efb352b2…a4da`, `verification.json` `ea9cea8e…f51a`, `SHA256SUMS` `f2c1ff03…d834`.

## AUD-02-AUDIO3D-OBJECTUNRESERVE-01 — `STATIC_PATH_BYTE_VERIFIED` (one of two callsites)
- `sceAudio3dObjectUnreserve`: symbol 7, NID `1HXxo-+1qCw`, slot `0x1601078`; thunk `0xf91510` (`ff2562fb6600`). Two raw candidates: `0x949e58` (chosen, `e8b3766400`) and `0x99b880`. `0x949e58` is in `FUN_00949dc0` (`0x949dc0..0x949efb`, 316 B, contiguous) — the same function that holds the `sceNgs2RackGetVoiceHandle` callsite `0x949df4` (static co-location only). Query `ev_7f0379bfc51a374f9ea82faf9d524ae2366c328a3148e0d8921b4101059b19fd`. Ghidra reports no callers.
- Dir `local/re/evidence/aud02-audio3d-objectunreserve-01/`; verifier `8457ed36…1136`, `verification.json` `0c829c8c…3fb9`, `function-callsite/SHA256SUMS` `553ef41d…3f3e`, top-level `SHA256SUMS` `d87822bf…7a0b`. `0x99b880` unattributed (likely near `FUN_0099b400`'s end; unverified).

Not established (both): reachability, runtime binding, ABI/argument semantics, audio behavior, indirect/tail-jump uses of the thunks, function-boundary completeness beyond Ghidra. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
