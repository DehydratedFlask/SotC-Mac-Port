# CPU-01-LEAF-CTOR-BATCH-17 — constructors 0xc37020, 0xc36a80, 0xc23650, 0xc22300, 0xc20710 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted five leaf vector-copy constructors (manifest positions 152–156); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc37020` | 217 / 152 | `0x12ec290` -> `0x33515e0` | `ebe8205f…` |
| `0xc36a80` | 218 / 153 | `0x12eaf90` -> `0x3351590` | `c5f7ba0b…` |
| `0xc23650` | 219 / 154 | `0x12eaae0` -> `0x3351580` | `b599de72…` |
| `0xc22300` | 220 / 155 | `0x12ea490` -> `0x3351570` | `90a479f7…` |
| `0xc20710` | 221 / 156 | `0x12e9d10` -> `0x3351560` | `940407fd…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 180/180 pass (was 175), all five checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 157 `0xc1a9a0`. G0 PARTIAL, G1-G6 NOT RUN.
