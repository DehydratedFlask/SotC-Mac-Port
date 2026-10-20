# CPU-01-LEAF-CTOR-BATCH-12 — constructors 0xc686e0, 0xc66580, 0xc65f80, 0xc65da0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted four leaf vector-copy constructors (manifest positions 128–131); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc686e0` | 192 / 128 | `0x12f0720` -> `0x33517f0` | `20530f1c…` |
| `0xc66580` | 194 / 129 | `0x12f0160` -> `0x3351750` | `07ce5c82…` |
| `0xc65f80` | 195 / 130 | `0x12effa0` -> `0x3351740` | `a65f86ba…` |
| `0xc65da0` | 196 / 131 | `0x12efdd0` -> `0x3351730` | `1608daf2…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 155/155 pass (was 151), all four checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Survey entries 194/195 have `bytes: None`; checkers read the ELF directly. Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 132 `0xc64060`. G0 PARTIAL, G1-G6 NOT RUN.
