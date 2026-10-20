# CPU-01-LEAF-CTOR-BATCH-04 — constructors 0xcceaa0, 0xccd0f0, 0xcc8ac0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors; each 17-byte body `c5f81005 <rel32> c5f81105 <rel32> c3` loads 16 bytes `{~0,~0,~0,0}` from rodata, stores to its own BSS cell, `ret`; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xcceaa0` | 160 / 102 | `0x12f67d0` -> `0x3351c40` | `a60df653…` |
| `0xccd0f0` | 161 / 103 | `0x12f6560` -> `0x3351c30` | `6a654fd3…` |
| `0xcc8ac0` | 162 / 104 | `0x12f61d0` -> `0x3351c20` | `76d4cbe8…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt` and `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 128/128 pass (was 125), three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not done: none registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next consecutive manifest candidates: positions 105 (`0xcc8330`) and 106 (`0xcc5a10`). G0 PARTIAL, G1-G6 NOT RUN.
