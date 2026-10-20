# CPU-01-LEAF-CTOR-BATCH-18 — constructors 0xc1a9a0, 0xc191a0, 0xc17a70, 0xc152c0, 0xc14f60, 0xc0d720 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted six leaf vector-copy constructors (manifest positions 157–162; survey index 225 is not a manifest entry); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc1a9a0` | 222 / 157 | `0x12e9a70` -> `0x3351550` | `8e96502d…` |
| `0xc191a0` | 223 / 158 | `0x12e97a0` -> `0x3351540` | `a1dd5d2c…` |
| `0xc17a70` | 224 / 159 | `0x12e9230` -> `0x3351530` | `73ad494f…` |
| `0xc152c0` | 226 / 160 | `0x12e8c90` -> `0x33513e0` | `23018bad…` |
| `0xc14f60` | 227 / 161 | `0x12e84f0` -> `0x33513d0` | `b0217abe…` |
| `0xc0d720` | 228 / 162 | `0x12e8260` -> `0x33513c0` | `c7182c6e…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 186/186 pass (was 180), all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 163 `0xc0bea0`. G0 PARTIAL, G1-G6 NOT RUN.
