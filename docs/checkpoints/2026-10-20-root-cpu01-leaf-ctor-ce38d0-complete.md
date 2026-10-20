# CPU-01-LEAF-CTOR-NEXT-02 — constructor RVA 0xce38d0 lifted, 2026-10-20, `/root`

`FIXTURE_VERIFIED`. The assigned `0xce6fb0`/`0xce67b0` were already lifted (`vec_ce6fb0.*`, `vec_ce67b0.*`; root re-ran both checkers, exit 0), so the worker lifted the next unlifted vector-copy ctor: `0xce38d0` (survey index 154, manifest position 96), range `0xce38d0..0xce38e1` (17 bytes) `c5f81005f8466100c5f81105c0e36602c3`, sha256 `0d0620cf…ada0`: one 16-byte load from rodata `0x12f7fd0` (`{0xffffffff ×3, 0}`), one store to own BSS `0x3351ca0`, `ret`; leaf, no calls.
- Added `native/core/vec_ce38d0.{hpp,cpp}` (`init_vec_ce38d0`), `native/tests/test_vec_ce38d0.cpp`, `tools/lift/check_fce38d0.py`; changed `native/CMakeLists.txt`, `native/README.md` (also added the missing table rows and checker-list entries for `0xce6fb0` and `0xce67b0`).
- Root replay: build exit 0; `ctest --test-dir local/build/native -j8` 122/122 pass (was 121); `check_fce38d0.py` exit 0, output SHA-256 `51160f8b…cbccf`. Worker mutation (lane 2 -> `0xfffffffe`, scratch copy) fails the test.
- Not done: not registered in `startup_ctors`; global's meaning unknown. No runtime/boot claim. G0 PARTIAL, G1-G6 NOT RUN.
