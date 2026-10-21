# CPU-01-LEAF-CTOR-BATCH-24 — constructors 0xbc07f0, 0xbc0670, 0xbbf580, 0xbbd5e0, 0xbbba10, 0xbb9740 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted six leaf vector-copy constructors (manifest positions 190–195, 0-based); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey idx / manifest pos | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xbc07f0` | 258 / 190 | `0x12ded00` -> `0x3350fc0` | `b33d5bff…` |
| `0xbc0670` | 259 / 191 | `0x12de6c0` -> `0x3350f60` | `bafe610a…` |
| `0xbbf580` | 260 / 192 | `0x12de530` -> `0x3350f50` | `0ede73fc…` |
| `0xbbd5e0` | 261 / 193 | `0x12de320` -> `0x3350f40` | `a2a30ea4…` |
| `0xbbba10` | 262 / 194 | `0x12de090` -> `0x3350f30` | `475538bb…` |
| `0xbb9740` | 263 / 195 | `0x12dde70` -> `0x3350f20` | `c1c11c00…` |

Root replay: rebuilt `local/build/native`, build exit 0, `ctest -j8` 222/222 pass (was 216); the worker's separate fresh-build run also reported 222/222; all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copies only) failed all six tests. README src/dst for the new rows were decoded from the ELF. Root re-ran the README-vs-ELF cross-check over all `vec_*` rows: 103 rows state addresses, **0 mismatches**; five older rows (`0xce7300`, `0xcdc760`, `0xce57b0`, `0xce0890`, `0xce6130`) state no src/dst text (all decode as clean ctors; ELF-decoded values for four: `0xce7300` `0x12f8bb0`->`0x3351cf0`, `0xcdc760` `0x12f7440`->`0x3351c70`, `0xce57b0` `0x12f8330`->`0x3351cb0`, `0xce0890` `0x12f7c00`->`0x3351c90`).

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest position: 196 `0xbb82d0`. G0 PARTIAL, G1-G6 NOT RUN.
