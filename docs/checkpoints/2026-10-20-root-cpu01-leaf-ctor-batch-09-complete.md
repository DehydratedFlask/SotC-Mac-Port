# CPU-01-LEAF-CTOR-BATCH-09 — constructors 0xc891e0, 0xc7de60, 0xc7c310 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted three leaf vector-copy constructors (manifest positions 117–119; survey index 179 has no manifest entry); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc891e0` | 178 / 117 | `0x12f2cd0` -> `0x3351a20` | `71a9819f…` |
| `0xc7de60` | 180 / 118 | `0x12f2520` -> `0x33519f0` | `3922683a…` |
| `0xc7c310` | 181 / 119 | `0x12f2250` -> `0x33519e0` | `3f383ac3…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 143/143 pass (was 140), all three checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidates: 120 `0xc786c0`, 121 `0xc77070`. G0 PARTIAL, G1-G6 NOT RUN.
