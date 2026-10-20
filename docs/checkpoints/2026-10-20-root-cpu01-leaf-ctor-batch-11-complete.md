# CPU-01-LEAF-CTOR-BATCH-11 — constructors 0xc6fdf0, 0xc6d910, 0xc6bad0, 0xc69f70 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted four leaf vector-copy constructors (manifest positions 124–127); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc6fdf0` | 186 / 124 | `0x12f14e0` -> `0x3351990` | `0bb6dc95…` |
| `0xc6d910` | 189 / 125 | `0x12f0ea0` -> `0x3351820` | `4b7e7f63…` |
| `0xc6bad0` | 190 / 126 | `0x12f0c80` -> `0x3351810` | `92579c8f…` |
| `0xc69f70` | 191 / 127 | `0x12f0a30` -> `0x3351800` | `43f07370…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 151/151 pass (was 147), all four checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Note: survey entry 189 (`0xc6d910`) carries a stale `.text`-based destination in its disassembly text; the checker reads the ELF directly and agrees with the manifest. Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidates: 128 `0xc686e0`, 129 `0xc66580`, 130 `0xc65f80`. G0 PARTIAL, G1-G6 NOT RUN.
