# CPU-01-LEAF-CTOR-BATCH-05 — constructors 0xcc8330, 0xcc5a10, 0xcc07d0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors (manifest positions 105–107); each 17-byte body `c5f81005 <rel32> c5f81105 <rel32> c3` loads `{~0,~0,~0,0}` from rodata, stores to its own BSS cell, `ret`; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xcc8330` | 163 / 105 | `0x12f5fd0` -> `0x3351c10` | `db647c67…` |
| `0xcc5a10` | 164 / 106 | `0x12f5c80` -> `0x3351c00` | `eff8c3a5…` |
| `0xcc07d0` | 165 / 107 | `0x12f57d0` -> `0x3351bf0` | `eb406c01…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 131/131 pass (was 128), all three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Caveats: each checker's optional `--dump` cross-check reads `local/re/work/c<rva>.bin` (template-copied path) which does not exist, so that sub-check is skipped; the main byte/ELF assertions run. Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next candidate: position 108 `0xcbb2c0`. G0 PARTIAL, G1-G6 NOT RUN.
