# CPU-01-LEAF-CTOR-BATCH-10 — constructors 0xc786c0, 0xc77070, 0xc741c0, 0xc70e20 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted four leaf vector-copy constructors (manifest positions 120–123); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc786c0` | 182 / 120 | `0x12f1ee0` -> `0x33519d0` | `9c7b3bc8…` |
| `0xc77070` | 183 / 121 | `0x12f1c00` -> `0x33519c0` | `abaae0a1…` |
| `0xc741c0` | 184 / 122 | `0x12f1970` -> `0x33519b0` | `68069fb9…` |
| `0xc70e20` | 185 / 123 | `0x12f16a0` -> `0x33519a0` | `627619fb…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 147/147 pass (was 143), all four checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidates: 124 `0xc6fdf0`, 125 `0xc6d910`, 126 `0xc6bad0`. G0 PARTIAL, G1-G6 NOT RUN.
