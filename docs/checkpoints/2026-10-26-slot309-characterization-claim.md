# Startup slot 309 characterization claim

Local date/time: 2026-10-26 23:30 CDT (America/Chicago). Agent: `/root`.

Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.

## Bounded objective

Characterize the first remaining production constructor gap, index **309**, RVA **`0xa505e0`**.
The prior constructor census reports a body size of 1,891 bytes; re-check that size against the
pinned local ELF before relying on it. Determine complete reachable control flow, branch targets,
direct calls, indirect-call resolution, required data/side effects, and which callees have an exact
native model versus a hook-only boundary. Recommend whether a native lift can be built without
guessing or hiding an unsupported path.

This is a read-only static-analysis task. Do not edit native source, the pinned reference checkout,
the original game input, or shared task/evidence docs. Keep any disassembly, reports, hashes, and tool
logs private under `local/validation/`. Do not execute the original executable or use an emulator,
guest CPU interpreter, JIT, or a Sony runtime.

## Required evidence and next action

Start with Graft/Trail navigation and verify the live table/checker source. Use the existing pinned
ELF and project analysis tools; for a shipped-code claim, preserve the ELF digest, image base, tool
version, function coverage, direct-call list, and unresolved indirect calls. If REA/Ghidra is used,
retain the evidence ID and exported evidence before closing the analysis session.

No body semantics, callees, native implementation, or acceptance status are inferred from the old
1,891-byte estimate. Status is **ACTIVE / read-only analysis**; failed hypotheses: **0**. Next exact
action: `/root/trail_scope_audit` produces a private report and sends the exact commands, hashes,
coverage, unresolved edges, and recommendation to the integrator.
