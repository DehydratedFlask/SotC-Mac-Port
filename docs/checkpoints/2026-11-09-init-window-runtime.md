# 2026-11-09 — Init window runtime entry

**Task:** `CPU-01-INIT-RUNTIME-ENTRY-01`  
**Status:** `PASSED / ARTIFACT_VERIFIED` (Init control-flow window only)

- Added `native/core/init_window_runtime.{hpp,cpp}`: starts at `0x531d60` through
  `init_531d60_entry_gaps`, runs the measured full Init slice, then requires the native registry
  body/remainder to install **275** entries at `0x532c1c`.
- Added `native/app/sotc_init_window_main.cpp` and the `sotc_init_window` ARM64 executable plus
  CTest `init_window_runtime`.
- Observed JSON: entry units **17**, entry base calls **1**, all stage booleans true, registry body
  units **64**, registry entries **275**, registry status **0**, boss flag **0xA7**.
- Explicit non-executed seams: `0xa473b0` ×1, `0xa54920` ×3, `0x54b250` ×1; post-base callee bodies
  remain in the slice header's `NOT modelled` list. They are named in the JSON, not hidden.
- Full suite: **701/701** passed. Logs under `local/validation/`.
- No commit/tag/push; 0 staged. See `docs/evidence/2026-11-09-init-window-runtime.md`.
