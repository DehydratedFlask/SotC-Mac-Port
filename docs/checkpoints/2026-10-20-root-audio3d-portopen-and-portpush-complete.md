# AUD-02-AUDIO3D-PORTOPEN-01 and AUD-02-AUDIO3D-PORTPUSH-01 — 2026-10-20, `/root`

Two parallel REA 6.7.0 / Ghidra 12.1.4 `function` queries, each on a hash-verified pristine snapshot copy (`32f2c278…`; both exit 0, no retry; 2 successes, 0 failures). Root replayed both verifiers (exit 0) and all four `shasum -c` runs (all OK). Static path only; first probes of the `libSceAudio3d` imports.

## AUD-02-AUDIO3D-PORTOPEN-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceAudio3dPortOpen`: symbol 11, NID `XeDDK0xJWQA`, slot `0x1601098`; thunk `0xf91550` (`ff2542fb6600`), only FF25 thunk to that slot. Exactly one raw `E8 rel32` candidate: `0x948e09` (`e842876400`).
- In `FUN_009485a0` (`0x9485a0..0x949b70`, 5585 B) — the same function that holds six `sceNgs2RackGetVoiceHandle` callsites (`0x9496a8` etc.), so this function uses both the Audio3d and NGS2 imports (static co-location only; says nothing about runtime role). Query 385.99 s, `ev_e75da1125582b7ce66119aa29c7564079127de388e1187d6c604104ed90fed5b`.
- Dir `local/re/evidence/aud02-audio3d-portopen-01/`; verifier `b34dd103…22cf`, `verification.json` `7ad197e8…0511`, `function-callsite/SHA256SUMS` `193d5810…bfb40`, top-level `SHA256SUMS` `a2f597b6…3aae`.

## AUD-02-AUDIO3D-PORTPUSH-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceAudio3dPortPush`: symbol 12, NID `VEVhZ9qd4ZY`, slot `0x16010a0`; thunk `0xf91560` (`ff253afb6600`). Exactly one raw candidate: `0x949f95` (`e8c6756400`).
- `FUN_00949f80`, ranges `0x949f80..0x949f8a` + `0x949f90..0x949fd0` (76 B in an 81 B span); a small loop (entry `jmp` to `0x949f9a`, `jnz` back to `0x949f90`, `ret` at `0x949fd0`). Query 380.57 s, `ev_7db34c475585e7de59f6e1ee5ae1262ce288cfa8dc059ee27adc37a1ce9ff01a`. Ghidra reports no callers.
- Dir `local/re/evidence/aud02-audio3d-portpush-01/`; verifier `2bf289a0…d198`, `verification.json` `93a20c42…be783`, `function-callsite/SHA256SUMS` `90e02999…e519aa`, top-level `SHA256SUMS` `35d8a15f…4457ea`.

Not established (both): reachability, runtime binding, ABI/return codes, port/audio semantics, loop/global meanings, indirect/tail-jump uses of the thunks, function-boundary completeness. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
