# E1052 — Ghidra MCP timeout, attempt 1

**Date:** 2026-10-31 02:10 CDT. **Agent:** `/root`. **Task:**
`CPU-01-SLOT309-SAVE-DISPATCH-JUMPTABLES-01`. **Status:** ACTIVE. **Attempts:** 1.

## Exact action and result

The opened target was `/Volumes/Extra/SoC Mac Native/local/re/eboot.elf`, provider `ghidra`,
snapshot `/Volumes/Extra/SoC Mac Native/local/re/rea-snapshot.json`. The calls were:

```text
mcp__rea__open_binary({path:"/Volumes/Extra/SoC Mac Native/local/re/eboot.elf",provider_id:"ghidra",snapshot_path:"/Volumes/Extra/SoC Mac Native/local/re/rea-snapshot.json"})
Promise.all([mcp__rea__binary_overview({}),mcp__rea__binary_session({})])
```

`binary_overview` returned `provider_timeout` for operation `read_binary_inventory_summary` at
330,000 ms. Diagnostics identify Ghidra 12.1.4, ELF SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, analysis-profile digest
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`, configured JVM max heap
`2G`, and run ID `2fc03235-bdac-4b11-95f3-ba5553b234c4`. The session query then reported the
same target open, `analysis_activity.status = not_observed`, and `cleanup_pending = false`. The
matching Evidence-bundle summary contains only a previous function record at `0xf120`
(`ev_51f10a737f14992cc6215e62b361b86a073fdce4a36455feb6d18635bc8a78b4`); there is no E1052
Ghidra Evidence ID. The provider-reported temporary log paths no longer existed when checked.

## Observations and limits

- This MCP inventory route did not produce a function dossier. It does not establish a Ghidra
  view of either dispatcher or its cases.
- The private ELF parser and Rizin have separately verified the two PIC-relative table spans,
  signed targets, dispatcher bounds, and selected direct-call paths. These are static observations;
  branch conditions, runtime selection, and invocation are not claimed.
- No evidence links these save-dispatch consumers to slot-309 initializer `0xa505e0`. Slot 309
  remains unregistered, and G1 remains PARTIAL.

## Next action

Use the already-installed REA CLI and the repository's documented Ghidra recipe with
`GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000` to query dispatcher
`0xa48280`. Preserve JSON, stderr, and snapshot under the ignored directory
`local/validation/slot309-save-dispatch-e1052/`. If successful, query `0xa4ab90` with the same
bounded setup, then stop if the evidence still cannot establish a concrete edge/order to
`0xa505e0`.
