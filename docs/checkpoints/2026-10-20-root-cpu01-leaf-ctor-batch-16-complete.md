# CPU-01-LEAF-CTOR-BATCH-16 — constructors 0xc451d0, 0xc44830, 0xc42b00, 0xc3eaa0, 0xc3cf00 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted five leaf vector-copy constructors (manifest positions 147–151); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey/manifest idx | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc451d0` | 212 / 147 | `0x12ed6e0` -> `0x3351630` | `2b578128…` |
| `0xc44830` | 213 / 148 | `0x12ed4f0` -> `0x3351620` | `74ccbe25…` |
| `0xc42b00` | 214 / 149 | `0x12ed210` -> `0x3351610` | `200e465a…` |
| `0xc3eaa0` | 215 / 150 | `0x12ecc40` -> `0x3351600` | `b081cafe…` |
| `0xc3cf00` | 216 / 151 | `0x12ec5b0` -> `0x33515f0` | `8e968905…` |

Files per ctor: `native/core/vec_<rva>.{hpp,cpp}`, `native/tests/test_vec_<rva>.cpp`, `tools/lift/check_f<rva>.py`; `native/CMakeLists.txt`, `native/README.md` updated. Root replay: build exit 0, `ctest --test-dir local/build/native -j8` 175/175 pass (was 170), all five checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 152 `0xc37020`. G0 PARTIAL, G1-G6 NOT RUN.
