# CPU-01-LEAF-CTOR-BATCH-07 — constructors 0xcae740, 0xcae240, 0xcad450 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors (manifest positions 111–113); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xcae740` | 169 / 111 | `0x12f4490` -> `0x3351bb0` | `4d75509b…` |
| `0xcae240` | 170 / 112 | `0x12f4270` -> `0x3351b90` | `7eac439a…` |
| `0xcad450` | 171 / 113 | `0x12f4010` -> `0x3351b80` | `b9307db8…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 137/137 pass (was 134), all three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next candidate: position 114 `0xca4dc0`. G0 PARTIAL, G1-G6 NOT RUN.
