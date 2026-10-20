# CPU-01-LEAF-CTOR-BATCH-15 — constructors 0xc54830, 0xc53db0, 0xc4e5e0, 0xc4aa80, 0xc47e00 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted five leaf vector-copy constructors (manifest positions 142–146); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc54830` | 207 / 142 | `0x12ee5f0` -> `0x3351680` | `c998167e…` |
| `0xc53db0` | 208 / 143 | `0x12ee370` -> `0x3351670` | `b6792dde…` |
| `0xc4e5e0` | 209 / 144 | `0x12edf70` -> `0x3351660` | `a9f59573…` |
| `0xc4aa80` | 210 / 145 | `0x12edcc0` -> `0x3351650` | `6d0ce4fe…` |
| `0xc47e00` | 211 / 146 | `0x12ed9a0` -> `0x3351640` | `7c21db8d…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 170/170 pass (was 165), all five checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 147 `0xc451d0`. G0 PARTIAL, G1-G6 NOT RUN.
