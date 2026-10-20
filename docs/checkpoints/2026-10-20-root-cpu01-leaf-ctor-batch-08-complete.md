# CPU-01-LEAF-CTOR-BATCH-08 — constructors 0xca4dc0, 0xc9e8e0, 0xc91b10 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors (manifest positions 114–116); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls. Survey indices 172/174/176 are not in the vector-copy manifest and were skipped.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xca4dc0` | 173 / 114 | `0x12f3ac0` -> `0x3351b10` | `38c20c88…` |
| `0xc9e8e0` | 175 / 115 | `0x12f3550` -> `0x3351aa0` | `4cc47727…` |
| `0xc91b10` | 177 / 116 | `0x12f2fd0` -> `0x3351a30` | `94c175b1…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 140/140 pass (was 137), all three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidates: positions 117–121 (`0xc891e0, 0xc7de60, 0xc7c310, 0xc786c0, 0xc77070`). G0 PARTIAL, G1-G6 NOT RUN.
