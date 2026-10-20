# Audio3d remaining imports (PortSetAttribute, Terminate, PortGetQueueLevel, PortClose, ObjectUnreserve-02) — 2026-10-20, `/root`

Two new REA 6.7.0 / Ghidra 12.1.4 `function` queries (PortClose 367.48 s, ObjectUnreserve-02 360.73 s; both exit 0, hash-verified pristine snapshot copies, no retry; 2 successes, 0 failures). Three tasks reused existing Ghidra records by hash (no provider operation). Root replayed all five verifiers (exit 0) and every `shasum -c` (all OK). Static path only. With these, every imported `libSceAudio3d` symbol (Initialize, PortOpen, PortPush, PortAdvance, PortGetQueueLevel, PortClose, PortSetAttribute, ObjectReserve, ObjectUnreserve, ObjectSetAttributes, Terminate) has at least one byte-verified callsite.

| Task | Symbol / NID / slot / thunk | Verified callsite | Function (Ghidra) | Query / reuse | Dir |
|---|---|---|---|---|---|
| AUD-02-AUDIO3D-PORTSETATTRIBUTE-01 | 13 / `Yq9bfUQ0uJg` / `0x16010a8` / `0xf91570` | `0x948e79` (2 candidates; `0x954726` unattributed) | `FUN_009485a0` `0x9485a0..0x949b70` | reuse `ev_e75da112…` (0 queries) | `aud02-audio3d-portsetattribute-01/` |
| AUD-02-AUDIO3D-TERMINATE-01 | 14 / `WW1TS2iz5yc` / `0x16010b0` / `0xf91580` | `0x948e16` (2 candidates; `0x94a12d` in `FUN_00949fe0` per existing evidence) | `FUN_009485a0` | reuse `ev_e75da112…` (0 queries) | `aud02-audio3d-terminate-01/` |
| AUD-02-AUDIO3D-PORTGETQUEUELEVEL-01 | 10 / `YaaDbDwKpFM` / `0x1601090` / `0xf91540` | `0x953a68` (sole) | `FUN_009536d0` `0x9536d0..0x953abf` | reuse `ev_a6c4e39c…` (PortAdvance) (0 queries) | `aud02-audio3d-portgetqueuelevel-01/` |
| AUD-02-AUDIO3D-PORTCLOSE-01 | 9 / `OyVqOeVNtSk` / `0x1601088` / `0xf91530` | `0x94a11e` (sole) | `FUN_00949fe0` `0x949fe0..0x94a1d4` (501 B) | `ev_175a9e742517cae163b5ef305c5764099ccda3385d476e9533e9f106a76a685d` | `aud02-audio3d-portclose-01/` |
| AUD-02-AUDIO3D-OBJECTUNRESERVE-02 | 7 / `1HXxo-+1qCw` / `0x1601078` / `0xf91510` | `0x99b880` (second candidate; first `0x949e58` done earlier) | `FUN_0099b850` `0x99b850..0x99b8dd` (142 B) | `ev_e848a3c0d509d29843e31f8f3a9900823c198a99015506b0fd079bf7d0394114` | `aud02-audio3d-objectunreserve-02/` |

Verifier / `verification.json` SHA-256 prefixes (root-observed): PortSetAttribute `9d8fbd8d…`/`5cb0768b…`; Terminate `89c87650…`/`0883f500…`; PortGetQueueLevel `2ba60a19…`/`6697062a…`; PortClose `af158db0…`/`964469d3…`; ObjectUnreserve-02 `ec990c68…`/`6f224bf4…`. Both ObjectUnreserve candidates are now attributed. Provider observation only: Ghidra lists `FUN_00949fe0` (PortClose function) as caller of `FUN_0099b850`.

Not established: reachability, runtime slot binding, ABI/argument semantics, audio behavior, indirect/tail-jump uses of the thunks, function-boundary completeness beyond Ghidra, the unattributed `0x954726`. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN. No install changes.
