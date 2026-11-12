# Init window seams wired: `0xa473b0` / `0xa54920` / `0x54b250` execute natively

- **Date:** 2026-11-09
- **Task:** `CPU-01-INIT-SEAMS-WIRED-01`
- **Agent:** Buffy
- **Runtime:** `sotc_init_window` (arm64), build `local/build/orch-r22`
- **Supersedes:** the seam count in E1423. The entry/checkpoint result is unchanged; the three bodies
  that E1423 counted as caller-owned hooks now execute native models.

## Command

```sh
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
```

Exit status **0**. Observed JSON:

```json
{"schema":"sotc.init_window.v2","ok":true,"entry_rva":"0x531d60","window_end_rva":"0x532b0c","checkpoint_rva":"0x532c1c","entry_units":17,"entry_base_calls":1,"early":true,"tail":true,"ctor":true,"base":true,"post_base":true,"continue_e":true,"registry_tail":true,"registry_body":true,"registry_body_units":64,"registry_entries":275,"registry_status":0,"remainder":true,"boss_flag_after":167,"executed_bodies":["0xa473b0","0xa54920","0x54b250"],"executed_body_calls":{"0xa473b0":1,"0xa54920":3,"0x54b250":1},"executed_body_ok":{"0xa473b0":true,"0xa54920":true,"0x54b250":true},"capture_step":5,"capture_video_0d":1,"capture_video_a007":0,"savedata_refusal":0,"savedata_counters":9,"savedata_items":9,"stat_engine_calls":1,"stat_engine_arg2":2,"stub_hooks":[],"stub_calls":{},"contract_boundaries":"0xa473b0, 0xa54920 and 0x54b250 execute native bodies; 0x54b250's engine 0xa56560 and the capture platform services are explicit host contracts; the remaining post-base callee bodies stay listed in native/core/appmain_init_ctor_98a90_slice.hpp","first_failed_stage":""}
```

## What now executes

- **`0xa473b0` (capture / streaming enable-state setter), 1 call.** The runtime installs
  `HostCaptureServices` and `continue_d` calls `capture_a473b0_set_enabled(false, ...)`. The lifted
  body reaches step 5, writes the `0x0d` video-info value **1** and the `0xa007` value **0**, and
  reports `ok`. This is the real 134 B body, not a counting hook.
- **`0xa54920` (save-data/statistics registry), 3 calls.** The runtime supplies the native
  `savedata_stats_a54920` model with a 0x700 B parent object, its eight-slot allocator and a valid
  `0x5e8` deque map. The three call sites use the real slot-428 save-name records at
  `0x2efd9a0`/`0x2efd9f8`/`0x2efda50` — `CollectionProfile`, `ShadowProfile`, `ShadowProgress` from
  `kSlot428SaveSpecs` — not invented host strings. Observed: **9** registered counters, **9**
  registered menu items, `savedata_refusal=0`, three distinct sibling slots (cursor 1/2/3), and the
  deque append. This is the real 1,840 B body.
- **`0x54b250` (stat-query wrapper), 1 call.** The runtime supplies `StatsEngine54b250`; the lifted
  119 B wrapper builds its 0x60 B descriptor, pins traits `0x15c9920`, dispatches the pinned bare-ret
  slot, and calls the engine once with `arg1=0`, `arg2=2` (the Init call site's `xor esi,esi` /
  `mov edx,2`). Observed `stat_engine_calls=1`, `stat_engine_arg2=2`.

The runtime still reaches the named checkpoint: entry units **17**, entry base calls **1**, registry
body **64** units, registry entries **275**, registry status **0**, boss flag **0xA7**,
`first_failed_stage=""`.

## Explicit boundaries (not hidden stubs)

- **`0xa56560` (stat engine inside `0x54b250`) is not lifted.** It is a host contract that records
  the call and returns accepted. `0x54b250`'s wrapper body itself executes.
- **Capture platform services are host no-ops** returning `ok`; no Sony live-streaming, screenshot
  or video-recording module runs. The lifted body's control flow and state writes execute.
- **`0xa54920`'s five descriptor vcalls per call** (`[in.vtable40+8]`, `[tmp.vptr]`,
  `[item.vptr]`, `[desc+0x90]`, `[tmp.vptr+0x18]`) and the old parent-slot destructor remain
  caller-owned hooks. They are counted (15 descriptor vcalls) and not dispatched; they are not
  claimed as executed bodies.
- **Post-base call-site callee bodies** listed in `native/core/appmain_init_ctor_98a90_slice.hpp`
  (`ensure migrate`, heavy menu helpers, `0xf8f860`, `0x499f80`/`0x56dcb0`/`0x5c64c0`/`0x1290`
  bodies when not supplied, `0x7f7220`, later `0x14c2f0`/`0x6a5370`/`0x6abcc0`/`0x21620`/`0x2779a0`/
  `0x42390`, and `0x523bd0`/`0x525b40`/`0x493230`/`0x525080`/`0x2569d0`) remain unexecuted.

No guest byte is executed. RVAs are identity tags.

## Verification

- Build: `cmake --build local/build/orch-r22 -j 12` — exit **0**.
- Runtime: exit **0**, JSON above.
- Full suite: `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — **701/701
  passed**, exit **0**; log `local/validation/2026-11-09-init-seams-full-ctest.log`.
- Focused CTest `init_window_runtime` is part of that total and passes.

## Not claimed

No cold game boot, audible game audio, save/reload after relaunch, Metal validation, or
forbidden-runtime/module absence. G0/G1/G3/G4 remain **PARTIAL**; G2/G5/G6 remain **NOT RUN**.
Nothing is staged, committed, tagged or pushed.
