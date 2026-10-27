# Startup slot 316 preliminary analyzer dossier

Local date/time: 2026-10-27 00:06 CDT (America/Chicago). Agent: `/root`.

Task: `CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01`.

## Source and ELF checks

The live table row at `native/core/ctor_original_table.cpp:330` is `{316, 0xa33280}`. The pinned
`local/re/eboot.elf` SHA-256 is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Worker and root runs of
`python3 -I tools/lift/check_ctor_slots.py` exited 0 and produced the same checker log SHA-256:
`0178d083ee30c8e60af051e72223a084ba9aeb2d161ae1e0cc060c4b70a06a75`. The checker verifies all
573 table rows and 397 registered RVA/index pairs; 176 `other` rows remain unregistered, with
index 309 / `0xa505e0` still the first gap.

## Analyzer result

The worker ran:

```sh
python3 -I tools/lift/analyze_slot_struct.py 0xa33280 --label slot316 > local/validation/orch-20261027/slot316/analyzer.json 2>&1
```

It exited 0. Root replay imported the same analyzer, pointed `WORK` to the task-private
`local/validation/orch-20261027/slot316/root-scratch/`, and wrote
`local/validation/orch-20261027/slot316/root-analyzer.json`. Full parsed JSON values were equal;
both files have SHA-256
`99c1a3bac98714513d437202409ca936d027e4e7f757695835fd199e026d1108`.

The analyzer reports `[0xa33280, 0xa33c80)` / **2,560 bytes** with body SHA-256
`0ade15657b51ff758d29d61901ad38405241ab87ce9b455a4af6b7fa905dc84e`; **473 reachable
instructions**, **2** conditional edges (`0xa338a7 -> 0xa338b1` and `0xa33c6a -> 0xa33c7b`),
**54 direct-call sites across 9 targets**, **5 unresolved indirect calls**, and **88 RIP-relative
references / 11 stores**, with zero outside-image references. Detailed sites and targets are in the
private analyzer JSON and
[`REPORT.md`](../../local/validation/orch-20261027/slot316/REPORT.md).

Root replay commands, both exit 0:

```sh
python3 -I tools/lift/check_ctor_slots.py > local/validation/orch-20261027/slot316/root-check_ctor_slots.log 2>&1
python3 -I -c 'import importlib.util,json; from pathlib import Path; root=Path.cwd(); spec=importlib.util.spec_from_file_location("slot_struct", root/"tools/lift/analyze_slot_struct.py"); m=importlib.util.module_from_spec(spec); spec.loader.exec_module(m); m.WORK=root/"local/validation/orch-20261027/slot316/root-scratch"; _,image,names,nids=m.load_image(); print(json.dumps(m.build_report(0xa33280,image,names,nids,label="slot316"),indent=1))' > local/validation/orch-20261027/slot316/root-analyzer.json 2> local/validation/orch-20261027/slot316/root-analyzer.stderr.log
```

The analyzer uses a fixed temporary decode cache at
`/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/slot_struct`; the worker's
command may refresh generated `.bin` windows there. Root replay used the task-private cache and did
not touch that shared scratch directory.

## Limits and status

This is preliminary analyzer-only evidence. It does not prove call semantics, complete control-flow
coverage, native-lift feasibility, or game behavior. No REA/Ghidra, original executable, emulator,
or Sony runtime was used. Slot 316 remains unregistered behind gaps 309, 312, and 314. **STATIC_ANALYSIS_VERIFIED**
for this task's bounded analyzer scope; **G1 remains PARTIAL**.
