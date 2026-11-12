# Init post-base call chain executes native bodies

**Date:** 2026-11-09
**Task:** `CPU-01-INIT-POST-CHAIN-01`
**Agent:** Buffy
**Status:** PASSED / ARTIFACT_VERIFIED
**Gate effect:** G1 remains **PARTIAL** (the post-base Init closure is measured; whole-program
CFG/entry map and a real game boot remain open). No other gate changes.

## Change

The runtime previously supplied `{}` for the `0x56dcb0`, `0x5c64c0` and `0x7f7220` call sites
and passed a call-site wrapper for `0x1290`, so those sites were pins. This change executes
native bodies at every one of them and reports the result in the runtime JSON.

- `native/core/init_post_bodies.{hpp,cpp}` adds ordinary native bodies for
  `0x56dcb0` (132 B), `0x5c64c0` (291 B) and `0x7f7220` (54 B). Each un-lifted callee is an
  explicit hook; a missing hook/input returns a named `blocked` stage, not a fake success.
- `native/core/init_window_runtime.{hpp,cpp}` owns the host cells/hooks, passes the real
  `Init1290Hooks` with a `Container1beeb0` (list-3) and a `Container21cd0` (4-entry vector),
  and executes the three bodies through their existing call-site hook slots.
- `native/app/sotc_init_window_main.cpp` emits schema `sotc.init_window.v4` with per-site
  native/blocked state, call counts, descriptor words, BSS hashes and the selected staffroll
  strings. The five one-byte-`ret` sites (`0x56dca0` plus four others) are native empty
  bodies; `0xf90310` is a named splash-hide host service.
- `native/tests/test_init_post_bodies.cpp` covers the three bodies' real stores/call order,
  the descriptor `{0x101e7ac, 0x030a}`, the AVX/BSS copy result, both `0x7f7220` branches and
  the named blocked stages. CMake registers `init_post_bodies`; the full suite is now 702.

No original RVA/guest byte is executed. `0x1290` is the existing native model; its singleton
owners remain host-owned. The runtime supplies stand-in global cells for the other bodies.

## Exact command and observed output

```sh
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
```

Exit **0**, stderr empty. Observed JSON (selected fields; full object in
`local/validation/2026-11-09-init-post-chain-runtime.json`):

```json
{
  "schema": "sotc.init_window.v4",
  "ok": true,
  "entry_units": 17,
  "entry_base_calls": 1,
  "post_chain_complete": true,
  "post_chain_blocked": "",
  "post_1290": {"native": true, "a": true, "b": true, "a_count": 1, "b_count": 1, "vcall": 1},
  "post_56dcb0": {"native": true, "calls": 5, "a5cd90": 1, "946880": 2, "a5d990": 1,
                   "560b60": 1, "desc0": "0x101e7ac", "desc1": "0x30a"},
  "post_56dca0": {"native": true, "bytes": 1, "calls": 1},
  "post_5c64c0": {"native": true, "bss_a": "0x19edac14", "bss_b": "0x75a104c5",
                   "bss_c": "0x8428e931", "cell": "0x5c64c0", "calls": 9, "tail": true},
  "post_7f7220": {"native": true, "branch": false, "sets": 2, "mode0": 0,
                   "str0": "0x128b8ac", "mode1": 1, "str1": "0x128b892"},
  "post_56db70": {"native": true, "bytes": 1, "calls": 1},
  "post_5b3d00": {"native": true, "bytes": 1, "calls": 1},
  "post_5b3cf0": {"native": true, "bytes": 1, "calls": 1},
  "post_56fad0": {"native": true, "bytes": 1, "calls": 1},
  "post_f90310": {"host_service": true, "calls": 1, "provider": "splash_hide"},
  "post_base_sites": ["0x56dcb0", "0x56dca0", "0x5c64c0", "0x1290", "0x56db70",
                       "0x5b3d00", "0x5b3cf0", "0x7f7220", "0x56fad0", "0xf90310"],
  "registry_entries": 275,
  "registry_status": 0,
  "boss_flag_after": 167,
  "first_failed_stage": ""
}
```

Logs: full JSON `local/validation/2026-11-09-init-post-chain-runtime.json`;
stderr `local/validation/2026-11-09-init-post-chain-runtime.err` (0 B).

## What the observed fields prove

- **`0x1290` ran its native body at `0x531e03`:** the A insert into `Container1beeb0` and the
  B insert into `Container21cd0` both executed (`a_count=1`, `b_count=1`), and the table slot-3
  bare-ret call was made once. The runtime passed both containers; it did not take the
  call-site fallback.
- **`0x56dcb0` ran its 132 B body:** all four helper paths were called in order
  (`a5cd90=1`, `946880=2` with args 1 and 0, `a5d990=1`, `560b60=1`) and the descriptor words
  are exactly `0x101e7ac` / `0x030a`.
- **`0x5c64c0` ran its 291 B body:** the first BSS buffer has the expected constant pattern,
  the second was zeroed over `0xc0` B, the source copy landed in the third, and all nine
  calls/tail executed. The three printed hashes are the runtime's FNV-1a hashes of those
  buffers, not game hashes. `cell=0x5c64c0` is the host hook's sentinel return, not a game
  pointer.
- **`0x7f7220` ran its 54 B body:** the query returned false, so the body selected
  `(0, 0x128b8ac)` and `(1, 0x128b892)`. Those RVAs are
  `staffroll_remaster_us.txt` and `staffroll_original_us.txt`; the true branch's
  `cp10_shadow_staffroll.txt` (`0x128b878`) was not selected.
- **The five one-byte targets execute native empty bodies:** `0x56dca0` (entry-gap span C),
  `0x56db70`, `0x5b3d00`, `0x5b3cf0` and `0x56fad0` each made one call and have no
  stores/inputs. They are the exact observable behavior of a one-byte `ret`, not an
  unlabelled pin.
- **`0xf90310` is explicitly a host service:** one splash-hide call was observed. Its Sony PLT
  body is not lifted and is not claimed as game code.
- **The required checkpoint is unchanged:** 275 registry entries, status 0, boss flag `0xA7`,
  `first_failed_stage=""`.

## Boundaries (named, not silently stubbed)

- `0x56dcb0`'s `0xa5cd90`, `0x946880`, `0xa5d990` and `0x560b60` are host hooks; their bodies
  are not lifted. The body's descriptor/argument sequence is native.
- `0x5c64c0`'s eight helpers are host hooks; its source is a deterministic host pattern and
  its BSS cells are host-owned. The body's stores/copy/call order are native.
- `0x7f7220`'s `0x5d06f0` and `0x7f6f90` are host hooks; no staffroll consumer is exercised.
- `0x1290`'s two singleton owners are host-owned; the list/vector containers are native.
- `0xf90310` is the Sony splash-hide PLT; the runtime supplies a named host service. It is not
  a lifted native game body.
- `0x499f80` remains as recorded in E1425 (allocator/CritSec contracts; first-commit branch).
  `0xa56560`, capture platform services and the `0xa54920` descriptor vcalls remain explicit
  boundaries.
- No game boot, audio, save/reload, Metal validation or forbidden-runtime absence is claimed.

## Verification

```sh
cmake --build local/build/orch-r22 -j 12 --target sotc_init_window test_init_post_bodies
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
git diff --check
python3 scripts/project_index.py
python3 -I scripts/audit_publish_paths.py --all
```

- Build exit **0**; final log `local/validation/2026-11-09-init-post-chain-build3.log`.
- Runtime exit **0**, stderr 0 B; `post_chain_complete=true`.
- Full CTest **702/702 passed**, exit **0**, including `init_post_bodies` and
  `init_window_runtime`; final log `local/validation/2026-11-09-init-post-chain-ctest-final.log`.
  The count is 701 baseline + the new focused fixture; no regression.
- Final incremental build exit **0** (`...-final-build.log`).
- `git diff --check` exit **0**; project index **114,651 folders** exit **0**
  (`...-index.log`); publish-path audit **OK (4,110 paths)** exit **0** (`...-audit.log`).
- Binary: `local/build/orch-r22/sotc_init_window` is a `Mach-O 64-bit executable arm64`.
- Working tree: **0 staged**; no commit, tag, or push.
