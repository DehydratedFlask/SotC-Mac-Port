# E830 — Slot-309 nonzero `+0x10` consumer trace

- **Date / local time:** 2026-10-27 15:57 CDT (America/Chicago).
- **Integrator:** `/root`.
- **Task:** `CPU-01-STARTUP-SLOT-309-NONZERO-CONSUMER-01`.
- **Status:** **PARTIAL / STATIC_ANALYSIS_VERIFIED** for exact vtable/method bytes and the bounded saved-project reference result. No concrete nonzero producer, semantic reader, or release path was established. Parent startup characterization remains ACTIVE; slot 309 stays unregistered, `+0x10` ownership UNKNOWN, G1 PARTIAL.
- **Attempts:** 0 failed semantic hypotheses; one bounded saved-project reference query/replay and one exact-ELF method profile. A verifier assertion was corrected to match the recorded table edge; this was not a failed analysis hypothesis.
- **Source pin:** `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, matching E821.

## Findings

The four qwords at vtable `0x15d1cc8` map through PH1 to file offset `0x15d5cc8` and point to
`0xa50220`, `0xa50240`, `0xa50260`, and `0xa50270`. Exact PH0 method bytes show the first three
helpers copy or propagate the qword at object offset `+0x10`; they do not dereference it, use it as
a call target, change a reference count, or free it in the inspected bodies. The fourth method is a
one-byte `ret`. This repeats E821's no-op cleanup finding and does not establish a general lifetime
contract.

The saved Ghidra ReferenceManager query and read-only replay agree for targets
`0xa50240`, `0xa50260`, `0x15d1cc8`, `0x15d1cd0`, and `0x15d1cd8`. In the saved reference graph,
the slot methods are table entries, while the table head has 12 incoming references including two
within the known slot-309 helper path. No recorded edge establishes a nonzero value producer or
owning release. Unresolved indirect data/control flow and runtime field values remain outside this
query's coverage. Keep the field's semantic use/ownership UNKNOWN; the slot-309 hook boundary stands.

## Evidence and commands

Private worker report: `local/validation/orch-20261027/e829/slot309-nonzero-consumer/REPORT.md`,
SHA-256 `8aedcb5e678e8bc46e760b81e3184edea986476fe977f89dd03aef8044021586`. It contains the exact
ELF bytes, the saved Ghidra command and all log paths/hashes. The byte-profile command exited 0:

```sh
python3 -I local/validation/orch-20261027/e829/slot309-nonzero-consumer/profile_vtable.py > local/validation/orch-20261027/e829/slot309-nonzero-consumer/vtable-profile.log 2>&1
```

The saved-project command used Ghidra 12.1.4 / OpenJDK 25.0.4.1 and existing
`local/ghidra/persistent-20261020/SotC` with `-noanalysis -readOnly`; it did not import or mutate the
project. The first invocation's shell exit was not retained, so its process status is **UNKNOWN**;
the complete second replay captured exit **0** in `headless-replay.exit`. Both output sets passed
the root verifier:

```sh
python3 -I local/validation/orch-20261027/e829/slot309-nonzero-consumer/verify_refs.py
python3 -I local/validation/orch-20261027/e829/slot309-nonzero-consumer/verify_refs.py local/validation/orch-20261027/e829/slot309-nonzero-consumer/ghidra-replay-script.log
```

Both verifier commands exited 0. `/root/slot309_nonzero_consumer_review` approved the result,
verified all 12 listed artifact hashes, and independently replayed the byte profile. No source or
dependency changed; no game code executed and no acceptance gate advanced.

## Repository verification

From the workspace root:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
python3 scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed
**516 / 0 skipped / 0 failed**; Trail graph check passed at **9,877 nodes / 13,470 edges** (deep
layer not built); project index regenerated and checked current at **31,944 folders**. Logs are
private under `local/validation/orch-20261027/e830/`.

## Publication baseline

E829 claim commit `3f8cf1bc0f5e1340d53d51e1b544873268c720bc` was pushed and fetched at `origin/main`.
Post-E829 public-ref scan: 9 branches, 12 PR heads, 0 tags, 15 unique tips, **2,442 reachable
commits**, zero Claude-family author/committer identities or credit lines. Private stash
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` remains unchanged per owner instruction.
