# CPU-01-LEAF-CTOR-BATCH-21 — constructors 0xe094e0, 0xdbfa60, 0xd85490, 0xbd7950, 0xbd60a0, 0xbd4dd0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent, resumed once after a session-limit interruption) lifted six leaf vector-copy constructors; each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls. Three are earlier manifest gaps (positions 4, 18, 28) found by the worker's gap audit; three continue at 175–177.

| RVA | survey idx / manifest pos (0-based) | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xe094e0` | 50 / 4 | `0x1313e20` -> `0x3353380` | `78e6e12d…` |
| `0xdbfa60` | 70 / 18 | `0x130de20` -> `0x3352ca0` | `884a9705…` |
| `0xd85490` | 82 / 28 | `0x130a970` -> `0x3352a90` | `63e07f8d…` |
| `0xbd7950` | 243 / 175 | `0x12e1e30` -> `0x33511b0` | `3235122d…` |
| `0xbd60a0` | 244 / 176 | `0x12e1a30` -> `0x33511a0` | `517db74e…` |
| `0xbd4dd0` | 245 / 177 | `0x12e1660` -> `0x3351190` | `c3b33d94…` |

Root replay on a fresh build directory (`cmake -S native -B /tmp/nb`): build exit 0, `ctest -j8` 204/204 pass (was 198), all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test; nothing mutated in place. The worker's intermediate per-ctor ctest counts are not clean fresh counts (an early CMake edit left some libraries unlinked mid-loop); only the fresh 204 is.

Gap audit: manifest positions lacking a `vec_*.cpp` were 0, 1, 4, 18, 28, 41. 4/18/28 are lifted here; 0 (`0xf50d70`) and 1 (`0xf30460`) are covered by `init_xyz_mask_copy` / `startup_ctors`; 41 (`0xd71f60`) is `NEEDS_PRINCIPAL` (`docs/escalations/CPU-01d-d71f60.md`) and was not retried. Positions 178 onward remain; next is 178 `0xbd32f0`. This corrects the earlier "next candidate" notes that assumed no earlier gaps.

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. G0 PARTIAL, G1-G6 NOT RUN.
