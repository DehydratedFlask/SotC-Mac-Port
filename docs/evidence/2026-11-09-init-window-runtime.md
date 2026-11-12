# Init window runtime execution — 2026-11-09

**Task:** `CPU-01-INIT-RUNTIME-ENTRY-01`  
**Artifact status:** `PASSED / ARTIFACT_VERIFIED` for the Init control-flow window. This is not a game
boot, audio, save/load or Metal gate.

## Entry and command

The native entry is `sotc::run_init_window_runtime`, exposed by the ARM64 Mach-O executable
`local/build/orch-r22/sotc_init_window`. It calls `init_531d60_entry_gaps` first; the entry frame's
base callback runs the existing measured `run_appmain_init_ctor_98a90_slice`. The driver then
requires the native registry body and remainder to install all 275 entries at the `0x532c1c`
checkpoint.

```sh
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
```

Observed output (also `local/validation/2026-11-09-init-window-runtime.log`):

```json
{"schema":"sotc.init_window.v1","ok":true,"entry_rva":"0x531d60","window_end_rva":"0x532b0c","checkpoint_rva":"0x532c1c","entry_units":17,"entry_base_calls":1,"early":true,"tail":true,"ctor":true,"base":true,"post_base":true,"continue_e":true,"registry_tail":true,"registry_body":true,"registry_body_units":64,"registry_entries":275,"registry_status":0,"remainder":true,"boss_flag_after":167,"stub_hooks":["0xa473b0","0xa54920","0x54b250"],"stub_calls":{"0xa473b0":1,"0xa54920":3,"0x54b250":1},"unmodelled_callee_bodies":"call-site hooks are modelled; the invoked 0xa473b0, 0xa54920 and 0x54b250 bodies are not executed; see native/core/appmain_init_ctor_98a90_slice.hpp NOT modelled list","first_failed_stage":""}
```

Observed state:

- entry frame units **17**, base call **1**;
- prologue early/tail, lifted `0x3be050`, base `0x255130`, the post-base span through continue
  C/D/E, registry tail, registry body and remainder all returned true;
- registry body unit count **64**, entries **275**, status **0** (`ok`);
- `0x2dcdf14` was stamped **0xA7** (`boss_flag_after` 167), matching the `0x1e4c0` value `0xA6`
  through the recovered `xor 1` route;
- no stage failed: `first_failed_stage` is empty.

## Explicit boundaries

The following caller-owned bodies are **not** executed as game code. The runtime counts the
invocations and says so in the JSON:

| seam | calls observed | status |
|---|---:|---|
| `0xa473b0` | 1 | explicit host hook; body not executed |
| `0xa54920` | 3 | explicit host hook; body not executed |
| `0x54b250` | 1 | explicit host hook; body not executed |

The post-base modules also model call sites whose callee bodies remain in the slice header's
`NOT modelled` list (for example `0x499f80`, `0x56dcb0`, `0x5c64c0`, `0x7f7220`, `0xf90310`,
`0x26800`, `0xf590`, the `0x532ca0`/`0x532ed0` vcalls, and the `0x2eff450`/`0x2eff448` destroy
paths). Their **control flow, arguments and state transitions** are executed by the native slice;
the callee bodies are not claimed. This is why the artifact is a control-flow/runtime entry proof,
not a completed engine.

## Verification

```sh
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
```

- build exit **0**;
- CTest **701/701 passed** (`100% tests passed out of 701`), including
  `init_window_runtime` as test 391;
- full logs: `local/validation/2026-11-09-full-build.log`,
  `local/validation/2026-11-09-full-ctest.log`.

## Still open

G0/G1/G3/G4 remain **PARTIAL**; G2/G5/G6 remain **NOT RUN**. There is still no cold game boot,
audible game content, save/reload after relaunch, Metal validation, or forbidden-runtime/module
absence proof. The new result replaces the old “3,500 claimed bytes” framing only for the
**control-flow window execution** claim; it does not claim the unmodelled engine bodies.
