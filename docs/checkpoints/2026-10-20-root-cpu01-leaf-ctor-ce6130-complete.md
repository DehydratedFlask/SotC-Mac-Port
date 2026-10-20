# CPU-01-LEAF-CTOR-NEXT-01 — constructor RVA 0xce6130 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. Worker (Agent subagent) lifted original ctor index 152 (vector-copy manifest position 94): range `0xce6130..0xce6140` (17 bytes) `c5f8100528246100c5f8110580bb6602c3`, sha256 `7977e36a…c32c`: one 16-byte vector load from rodata `0x12f8560` (`{0xffffffff ×3, 0}`) and one store to BSS `0x3351cc0`, then `ret`; leaf, no calls.
- Added `native/core/vec_ce6130.{hpp,cpp}` (`init_vec_ce6130`), `native/tests/test_vec_ce6130.cpp`, `tools/lift/check_fce6130.py`; changed `native/CMakeLists.txt`, `native/README.md`.
- Root replay: `cmake -S native -B local/build/native && cmake --build local/build/native -j8` exit 0; `ctest --test-dir local/build/native -j8` 121/121 pass (was 120); `python3 -I tools/lift/check_fce6130.py` exit 0, output SHA-256 `7c366382…f239`. Worker negative check: lane 2 changed to `0xfffffffe` in a scratch copy fails the test.
- Not done: slot not registered in `startup_ctors` (siblings `0xce6fb0`, `0xce67b0` are also unregistered); semantics of the global unknown. Ordinary native function only; no runtime/boot claim. G0 PARTIAL, G1-G6 NOT RUN.
