# CPU-01-LEAF-CTOR-BATCH-20 — constructors 0xc060a0, 0xbe5410, 0xbe5270, 0xbe50a0, 0xbe4ff0, 0xbd7a80 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted six leaf vector-copy constructors (manifest positions 169–174); each 17-byte body `vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret` copies `{~0,~0,~0,0}` from rodata to its own BSS cell; no calls.

| RVA | survey idx / manifest pos | src -> dst | checker stdout SHA-256 (root replay) |
|---|---|---|---|
| `0xc060a0` | 236 / 169 | `0x12e6d30` -> `0x33512b0` | `3ee96cde…` |
| `0xbe5410` | 238 / 170 | `0x12e35c0` -> `0x3351280` | `fce4419b…` |
| `0xbe5270` | 239 / 171 | `0x12e3430` -> `0x3351270` | `72406539…` |
| `0xbe50a0` | 240 / 172 | `0x12e3290` -> `0x33511e0` | `cbf85f4f…` |
| `0xbe4ff0` | 241 / 173 | `0x12e2490` -> `0x33511d0` | `007db26b…` |
| `0xbd7a80` | 242 / 174 | `0x12e2290` -> `0x33511c0` | `203699d7…` |

Root replay on a fresh build directory (`cmake -S native -B /tmp/nb`): build exit 0, `ctest -j8` 198/198 pass (was 192), all six checkers exit 0. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) failed each test. README rows were checked per ctor against its checker constants; no duplicate ctor rows in `native/README.md` (the two repeated "position" numbers are one survey-position and one manifest-position reference, not duplicates).

Correction of a prior start point: batch 19's "next candidate is 169 `0xbe5410`" was wrong — manifest position 169 is `0xc060a0` (lifted here) and `0xbe5410` is 170. Process note: the worker's per-ctor in-place mutation loop left stale test binaries mid-batch; the final state was rebuilt clean (the per-step ctest counts 194–197 it quoted are not clean pass counts; the final 198/198 is, and I reproduced it from scratch).

Not registered in `startup_ctors`; global meanings unknown; no runtime/boot claim. Next manifest candidate: position 175 `0xbd7950`. G0 PARTIAL, G1-G6 NOT RUN.
