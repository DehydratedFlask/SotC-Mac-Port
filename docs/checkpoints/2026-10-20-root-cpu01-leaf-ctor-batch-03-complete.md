# CPU-01-LEAF-CTOR-BATCH-03 — constructors 0xcde740, 0xcd4d60, 0xcd0420 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors; each body is 17 bytes `c5f81005 <rel32> c5f81105 <rel32> c3` (16-byte load from rodata `{~0,~0,~0,0}`, store to own BSS cell, `ret`; no calls).

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xcde740` | 156 / 98 | `0x12f77e0` -> `0x3351c80` | `aae36bc6…` |
| `0xcd4d60` | 158 / 100 | `0x12f6e70` -> `0x3351c60` | `4eabca36…` |
| `0xcd0420` | 159 / 101 | `0x12f6a20` -> `0x3351c50` | `2674d167…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py` (also asserts no other manifest entry shares the source/dest pair or dest cell); `native/CMakeLists.txt` and `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 125/125 pass (was 122), all three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each new test.

Not done: none registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. G0 PARTIAL, G1-G6 NOT RUN.
