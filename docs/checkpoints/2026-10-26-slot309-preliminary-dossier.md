# Startup slot 309 preliminary static dossier

Local date/time: 2026-10-26 23:50 CDT (America/Chicago). Integrator: `/root`; analysis: `/root/trail_scope_audit`.

Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.

## Verified artifacts

The pinned local ELF is `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The project reachability
analyzer reports the slot 309 root span `[0xa505e0,0xa50d43)` (**1,891 bytes**), 353 reachable
instruction starts, 40 direct-call sites / 10 unique targets, 73 RIP-relative references / 18
stores, one conditional branch, and three root indirect calls. The prior size estimate is
reproduced against this ELF.

Root replay used the repository parser and a private verification script:

```sh
python3 -I local/validation/orch-20261026/slot309/verify_indirect_rela.py > local/validation/orch-20261026/slot309/indirect_rela_root_replay.json
```

It exited 0. The three callsite instruction encodings are `ff 50 18` at `0xa5073c`, `0xa50af4`,
and `0xa50c73`. Their file-backed descriptor slots have type-8, symbol-zero RELA rows with
addends `0xa4f4c0`, `0xa4feb0`, and `0xa50270`. The matching slot bytes are `c0 f4 a4 00 00 00 00 00`,
`b0 fe a4 00 00 00 00 00`, and `70 02 a5 00 00 00 00 00`. For each destination, the project
analyzer reports a one-byte span containing `ret` (`c3`) and one reachable instruction. Target
function boundaries and no-op effects are not Ghidra-confirmed.

The full private dossier is `local/validation/orch-20261026/slot309/REPORT.md`, SHA-256
`9e2ba296e5da8299644e6bd831de7be341a6edfd9fd2e1ced1bc8ffa1c40220d`. It contains the 40-call
census, all first-hop target measurements, nested helper measurements, disassembly, CFG, and
artifact inventory. No shared source file changed.

## Limits and status

The REA overview, targeted function analysis, and focused indirect-call resolver each reached the
330,000 ms Ghidra startup deadline (version 12.1.4). The resolver's run ID was
`5d058cf8-d93f-4b82-b998-65a9342b551d`; no REA Evidence ID, Ghidra function coverage, or Ghidra
image base was returned. The static measurements are not Ghidra-confirmed function boundaries.
The three custom helper paths, their nested indirect calls, two additional indirect edges in
`0x26ea0`, and host lifetime effects remain unresolved. A fail-closed scaffold could be explored,
but a successful behaviorally complete lift is not supported by this evidence.

Status: **ACTIVE / STATIC_ANALYSIS_VERIFIED** for the private analyzer report and root byte replay;
failed semantic hypotheses: **0**; REA/Ghidra provider timeouts: **3**. G1 remains **PARTIAL**.

Exact next action: continue the separate read-only slot 312 preliminary analyzer claim. Do not
implement slot 309 until the unresolved helper and lifetime contracts are recovered or explicitly
defined and independently checked.
