# E1015 — slot-309 app-main receiver-provenance route review

- Date/time: 2026-10-29 20:33 CDT.
- Agent: `/root` (integrator), with read-only review from `/root/slot309_route_review`.
- Task ID: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`; resumed after principal reassessment.
- Status: `PARTIAL / SAVED_EXPORT_REVIEWED`; one bounded route reviewed; 0 failed semantic hypotheses.

## Scope and inputs

Reviewed only existing saved disassembly for the `0x25aa06` indirect call and its first direct helper at `0x33390`. The pinned local ELF SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. No REA/Ghidra operation, game execution, source change, or VoiceOver action occurred.

Exact read-only commands:

```sh
sed -n '1,74p' local/re/evidence/re02-appmain-vcall-25aa06/callsite.objdump.txt
sed -n '52483,52498p' local/validation/registry-analysis-agent/all.dis
```

Both exited 0. Output excerpts, the existing private report, and SHA-256 values are retained under `local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-RECEIVER-PROVENANCE-01/analysis/attempt-01/`.

## Observations

- At `0x25a8f2`, the saved callsite assigns `RDI` to `R15`. It calls `0x33390` at `0x25a901`, reloads `[R15]` at `0x25a9fd`, then calls through `[RAX+0x20]` at `0x25aa06`.
- The saved `0x33390` disassembly reads the thread pointer, increments a global counter, stores a TLS slot, and returns. It contains no visible receiver access through `RDI`/`R15` and no nested call.
- A conditional branch at `0x25a920` can skip the intervening calls and reach the vptr reload, or enter a path with additional callees before the reload. This review did not close those callees' alias or object effects.
- The existing app-main report has three candidate pointer windows. The constructor-linked window has `+0x20 = 0x531d60`; two other windows have `+0x20 = 0x255130`. No saved evidence selects the active receiver table.

## Conclusion and limits

The first direct helper does not show a receiver mutation in the saved export. The current `[R15]` value, unique indirect target, later-call effects, and an execution/order edge relative to startup constructor slot `0xa505e0` remain unknown. This route establishes no native-lift boundary and does not advance G1. Slot 309 remains unregistered. No runtime or game behavior is inferred.

Next exact action: do not repeat this receiver slice or generic xref/literal scans. Continue boot work only from a distinct concrete producer/consumer/order edge or newly available evidence; check whether any independent task can directly advance real startup without bypassing slot 309.

No repository build, test, lift-checker, or publication command is part of the analysis result; those checks are recorded in the publication closeout.
