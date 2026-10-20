# CPU-01-LEAF-CTOR-BATCH-14 — constructors 0xc5c040, 0xc5a400, 0xc5a100, 0xc555c0, 0xc54d20 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted five leaf vector-copy constructors (manifest positions 137–141); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc5c040` | 202 / 137 | `0x12ef040` -> `0x33516d0` | `11371c6a…` |
| `0xc5a400` | 203 / 138 | `0x12eee30` -> `0x33516c0` | `18bc243e…` |
| `0xc5a100` | 204 / 139 | `0x12eec70` -> `0x33516b0` | `cb19fb92…` |
| `0xc555c0` | 205 / 140 | `0x12ee940` -> `0x33516a0` | `a0a1096c…` |
| `0xc54d20` | 206 / 141 | `0x12ee790` -> `0x3351690` | `95b8c9b0…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 165/165 pass (was 160), all five checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 142 `0xc54830`. G0 PARTIAL, G1-G6 NOT RUN.
