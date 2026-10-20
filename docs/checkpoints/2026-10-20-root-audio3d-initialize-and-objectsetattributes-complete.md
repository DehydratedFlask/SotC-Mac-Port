# AUD-02-AUDIO3D-INITIALIZE-01 and AUD-02-AUDIO3D-OBJECTSETATTRIBUTES-01 — 2026-10-20, `/root`

One new REA 6.7.0 / Ghidra 12.1.4 `function` query (exit 0, 366.24 s, hash-verified pristine snapshot copy, no retry; 1 success, 0 failures); the Initialize task reused an existing Ghidra record by hash. Root replayed both verifiers (exit 0) and all four `shasum -c` runs (all OK). Static path only.

## AUD-02-AUDIO3D-INITIALIZE-01 — `STATIC_PATH_BYTE_VERIFIED`
- `sceAudio3dInitialize`: symbol 4, NID `UmCvjSmuZIw`, slot `0x1601060`; thunk `0xf914e0` (`ff257afb6600`). Exactly one raw `E8 rel32` candidate: `0x948db4` (`e827876400`), inside `FUN_009485a0` (`0x9485a0..0x949b70`) — the same function as the `sceAudio3dPortOpen` callsite (`0x948e09`) and six `sceNgs2RackGetVoiceHandle` callsites. No new query; reused `ev_e75da1125582b7ce66119aa29c7564079127de388e1187d6c604104ed90fed5b` by hash.
- Neighbouring bytes only: `xor edi,edi` before the call; `mov r12d,eax; test r12d,r12d; jnz 0x9488d2` after it.
- Dir `local/re/evidence/aud02-audio3d-initialize-01/`; verifier `a8f963ed…de09`, `verification.json` `27e4cd46…5f4b`, `function-callsite/SHA256SUMS` `72558cc6…df5d`, top-level `SHA256SUMS` `8db5d0e6…99f6`.

## AUD-02-AUDIO3D-OBJECTSETATTRIBUTES-01 — `STATIC_PATH_BYTE_VERIFIED` (one of three callsites)
- `sceAudio3dObjectSetAttributes`: symbol 6, NID `4uyHN9q4ZeU`, slot `0x1601070`; thunk `0xf91500` (`ff256afb6600`). Three raw candidates: `0x94da39, 0x94dc21, 0x999e8c`. Chosen `0x94da39` (`e8c23a6400`) in `FUN_0094d900` (`0x94d900..0x94da54`, 341 B, contiguous). Query `ev_33a74e313f06c3d196e54af12de1cd411e41195f52cada781b245cf221d59191`. Ghidra reports no callers.
- Dir `local/re/evidence/aud02-audio3d-objectsetattributes-01/`; verifier `f04b8f03…697e`, `verification.json` `67efe325…a27d`, `function-callsite/SHA256SUMS` `5a0d853f…`, top-level `SHA256SUMS` `a693a154…ccf3`.
- `0x94dc21` and `0x999e8c` unattributed.

Not established (both): reachability, runtime binding, ABI/argument/return semantics, ordering relative to PortOpen/PortPush, indirect/tail-jump uses of the thunks, function-boundary completeness. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
