# 2026-11-09 — Init window seams execute natively

**State:** green, uncommitted. E1423's three counted seams are now real executions inside
`run_init_window_runtime`.

- `0xa473b0`: 1 call through `capture_a473b0_set_enabled(false, HostCaptureServices)`, step 5,
  video values 1/0, `ok`.
- `0xa54920`: 3 calls through `savedata_stats_a54920` with the real slot-428 names
  `CollectionProfile`/`ShadowProfile`/`ShadowProgress`; 9 counters + 9 menu items, refusal 0.
- `0x54b250`: 1 call through `stat_query_54b250`; engine `0xa56560` contract called once with
  `arg2=2`.
- Checkpoint unchanged: 17 entry units, 275 registry entries, status 0, boss flag `0xA7`.
- Full CTest **701/701**, exit 0. Build exit 0. Runtime exit 0.
- Boundaries: `0xa56560`, host capture services, 15 descriptor vcalls and the post-base callee
  bodies are explicit and listed in `docs/evidence/2026-11-09-init-window-seams-wired.md`.
- G0/G1/G3/G4 **PARTIAL**; G2/G5/G6 **NOT RUN**. 0 staged; no commit/tag/push.

**Next exact action:** next runtime gate (cold boot path) or lift `0xa56560`; no new constructor
batch.
