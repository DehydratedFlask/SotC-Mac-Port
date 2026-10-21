# CPU-01-LEAF-CTOR-BATCH-22 — constructors 0xbd32f0, 0xbd3190, 0xbd06c0, 0xbceda0, 0xbccc00, 0xbcc960 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted six leaf vector-copy constructors (manifest positions 178–183, 0-based); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey idx / manifest pos | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xbd32f0` | 246 / 178 | `0x12e14c0` -> `0x3351180` | `190a4273…` |
| `0xbd3190` | 247 / 179 | `0x12e12c0` -> `0x3351170` | `e7abeabf…` |
| `0xbd06c0` | 248 / 180 | `0x12e0ee0` -> `0x3351160` | `92c1622a…` |
| `0xbceda0` | 249 / 181 | `0x12e0850` -> `0x3351150` | `89e6b03b…` |
| `0xbccc00` | 250 / 182 | `0x12e0680` -> `0x3351140` | `c5e0e6ea…` |
| `0xbcc960` | 251 / 183 | `0x12e04e0` -> `0x3351130` | `c97bb895…` |

Root replay: rebuilt `local/build/native` (the checkers' hard-coded path), build exit 0, `ctest -j8` 210/210 pass (was 204); the worker's separate fresh-build run also reported 210/210; all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy of `native/`) failed all six tests. README rows verified distinct per ctor.

Note: the checkers hard-code `local/build/native/test_vec_*`; that directory must be rebuilt (`cmake -S native -B local/build/native && cmake --build local/build/native`) before running them.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest position: 184 `0xbcc4e0`. G0 PARTIAL, G1-G6 NOT RUN.
