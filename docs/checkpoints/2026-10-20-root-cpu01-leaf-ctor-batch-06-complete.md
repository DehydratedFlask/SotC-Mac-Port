# CPU-01-LEAF-CTOR-BATCH-06 — constructors 0xcbb2c0, 0xcb5cb0, 0xcaeed0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors (manifest positions 108–110); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xcbb2c0` | 166 / 108 | `0x12f5330` -> `0x3351be0` | `8a4afd8e…` |
| `0xcb5cb0` | 167 / 109 | `0x12f4c80` -> `0x3351bd0` | `8fab2040…` |
| `0xcaeed0` | 168 / 110 | `0x12f4660` -> `0x3351bc0` | `2ce9630f…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py` (survey+manifest uniqueness; the nonexistent-file dump sub-check was dropped); `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 134/134 pass (was 131), all three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Note: survey records `bytes: None` for these ctors; checkers assert the body straight from the ELF. Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next candidate: position 111 `0xcae740`. G0 PARTIAL, G1-G6 NOT RUN.
