# CPU-01-LEAF-CTOR-BATCH-19 — constructors 0xc0bea0, 0xc0bbd0, 0xc0bb00, 0xc0baa0, 0xc0aae0, 0xc09070 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted six leaf vector-copy constructors (manifest positions 163–168); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | manifest / survey idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc0bea0` | 163 / 230 | `0x12e7b50` -> `0x3351310` | `965303c7…` |
| `0xc0bbd0` | 164 / 231 | `0x12e79c0` -> `0x3351300` | `1e7b4ea9…` |
| `0xc0bb00` | 165 / 232 | `0x12e7830` -> `0x33512f0` | `fd9817b5…` |
| `0xc0baa0` | 166 / 233 | `0x12e7520` -> `0x33512e0` | `ec992bcc…` |
| `0xc0aae0` | 167 / 234 | `0x12e7210` -> `0x33512d0` | `bec5fabf…` |
| `0xc09070` | 168 / 235 | `0x12e6f40` -> `0x33512c0` | `7660a936…` |

Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 192/192 pass (was 186), all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test (only via the explicit lane check, since the first assertion shares the header constant).

**Documentation defect found and fixed:** the batch-18 README rows for `0xc191a0`, `0xc17a70`, `0xc152c0`, `0xc14f60` and `0xc0d720` (pushed in `4320b2e`) had carried `0xc1a9a0`'s ctor index/position/addresses (222 / 157 / `0x12e9a70` / `0x3351550`) — a generator error in the batch-18 worker's README step. The code, tests and checkers for those ctors were correct (checkers passed and the batch-18 checkpoint table lists the right values); only the README prose was wrong. Corrected here to the batch-18 checkpoint values.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 169 `0xbe5410`. G0 PARTIAL, G1-G6 NOT RUN.
