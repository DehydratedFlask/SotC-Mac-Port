# Startup slot 312 preliminary static dossier

Local date/time: 2026-10-26 23:56 CDT (America/Chicago). Integrator: `/root`; analysis: `/root/history_audit`.

Task: `CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01`.

## Verified artifacts

The pinned ELF is `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The live native table row
is `{312, 0xa46130}`; the table checker reports table start `0x2dd70e0`, and the table word at
`0x2dd6720` is `30 61 a4 00 00 00 00 00`, matching the row.

Root replay commands:

```sh
python3 -I tools/lift/check_ctor_slots.py > local/validation/orch-20261026/slot312/root-check_ctor_slots.log 2>&1
python3 -I tools/lift/analyze_slot_struct.py 0xa46130 --label slot312 > local/validation/orch-20261026/slot312/root-analyzer.json 2> local/validation/orch-20261026/slot312/root-analyzer.stderr.log
```

Both exited 0. Root and worker analyzer JSON SHA-256 values match:
`c189376641eba8b10634c397026bab8110cb420b87cd1270489ecacba17d4c38`. The structural summary is
body `[0xa46130,0xa46611)` (**1,249 bytes**), body SHA-256
`327a08fc102adb871698e7d2bd36c62e6ec16749bcd228ac93411c21b4aa1ad3`, **238** reachable
instructions, **30** direct-call sites, one conditional edge `0xa465f8 -> 0xa4660c`, two unresolved
indirect calls at `0xa4626c` and `0xa46320`, **49** file-backed RIP-relative references (**8**
stores), and zero outside-image references. The entire call and data-reference arrays match between
worker and root outputs.

The worker's exact commands, table-row proof, analyzer JSON, and summary remain private under
`local/validation/orch-20261026/slot312/`. No source edits were made.

## Limits and status

This is analyzer-only static evidence. It does not establish CFG completeness, indirect targets,
callee semantics, branch outcomes, native-lift feasibility, or game behavior. No REA/Ghidra or
runtime was used. G1 remains **PARTIAL**.

Status: **STATIC_ANALYSIS_VERIFIED** for the bounded table and analyzer characterization; failed
semantic hypotheses: **0**. The two indirect calls and all semantic/liftability requirements remain
open.

Exact next action: proceed with the separately claimed read-only analyzer task for slot 314. Do not
implement or register slot 312 from this report alone.
