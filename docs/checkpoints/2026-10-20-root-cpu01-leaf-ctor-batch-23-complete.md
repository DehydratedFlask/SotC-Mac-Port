# CPU-01-LEAF-CTOR-BATCH-23 — constructors 0xbcc4e0, 0xbcc3f0, 0xbc8210, 0xbc7280, 0xbc5b00, 0xbc2800 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted six leaf vector-copy constructors (manifest positions 184–189, 0-based); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey idx / manifest pos | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xbcc4e0` | 252 / 184 | `0x12e0350` -> `0x3351120` | `1ea1b86d…` |
| `0xbcc3f0` | 253 / 185 | `0x12dfe60` -> `0x3351110` | `37086ba7…` |
| `0xbc8210` | 254 / 186 | `0x12dfc60` -> `0x3351100` | `75de6f83…` |
| `0xbc7280` | 255 / 187 | `0x12df790` -> `0x3350ff0` | `d3af6cf0…` |
| `0xbc5b00` | 256 / 188 | `0x12df360` -> `0x3350fe0` | `5ccd1d18…` |
| `0xbc2800` | 257 / 189 | `0x12deef0` -> `0x3350fd0` | `9702fa20…` |

Root replay: rebuilt `local/build/native` (checkers' hard-coded path), build exit 0, `ctest -j8` 216/216 pass (was 210); the worker's separate fresh-build run also reported 216/216; all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed all six tests.

**Second README defect found and fixed:** the worker noticed five README rows sharing one source/destination. Root verified by decoding every `vec_*` row's RIP-relative operands directly from the pinned ELF and comparing with the README text: exactly four rows were wrong — batch 14's `0xc5a400`, `0xc5a100`, `0xc555c0` and `0xc54d20` had carried `0xc5c040`'s `0x12ef040 -> 0x33516d0` (same generator error class as the batch-18 rows fixed in `1443b27`). Their code, tests and checkers were correct; only README prose was wrong. Rows now corrected to the ELF-derived values (which match the batch-14 checkpoint table). No other parseable row mismatches the ELF.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest position: 190 `0xbc07f0`. G0 PARTIAL, G1-G6 NOT RUN.
