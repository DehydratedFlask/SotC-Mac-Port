# CPU-01-LEAF-CTOR-BATCH-13 — constructors 0xc64060, 0xc61670, 0xc5f6f0, 0xc5c320, 0xc5c1a0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted five leaf vector-copy constructors (manifest positions 132–136); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc64060` | 197 / 132 | `0x12efbf0` -> `0x3351720` | `e6c6c1a1…` |
| `0xc61670` | 198 / 133 | `0x12ef8c0` -> `0x3351710` | `47d3447b…` |
| `0xc5f6f0` | 199 / 134 | `0x12ef650` -> `0x3351700` | `7a0149c0…` |
| `0xc5c320` | 200 / 135 | `0x12ef3a0` -> `0x33516f0` | `0815d3c0…` |
| `0xc5c1a0` | 201 / 136 | `0x12ef200` -> `0x33516e0` | `1a8073c1…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 160/160 pass (was 155), all five checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 137 `0xc5c040`. G0 PARTIAL, G1-G6 NOT RUN.
