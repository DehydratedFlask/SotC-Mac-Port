# E829 — Claim slot-309 nonzero `+0x10` consumer trace

- **Date / local time:** 2026-10-27 15:46 CDT (America/Chicago).
- **Integrator:** `/root`.
- **Task:** `CPU-01-STARTUP-SLOT-309-NONZERO-CONSUMER-01`, a bounded read-only continuation of `CPU-01-STARTUP-SLOT-309-PAYLOAD-OWNERSHIP-01`.
- **Starting tree:** E828 commit `4f3d9af2d17a66a46afb1af636c6d59286235d87` is pushed and fetched; `HEAD == origin/main` before this claim.
- **Claimed scope:** Use only existing hash-pinned saved Ghidra/export evidence and exact pinned ELF bytes to find and verify one concrete nonzero reader or destruction path for the qword copied between source and destination offset `+0x10` by helper `0xa50220` on the slot-309 path. The prior E821 observation remains narrow: the inspected cleanup resolves to `0xa50270` (`ret`) and does not access/release the copied qword.
- **Acceptance:** preserve exact addresses, byte/RELA mapping, evidence hashes and command/log paths; distinguish observation from inference. If the existing saved evidence cannot establish a concrete nonzero consumer/release path, return `PARTIAL` and identify the coverage boundary. A broader ownership contract requires positive evidence and independent review.
- **Limits:** no fresh binary import, game execution, payload extraction, slot-309 registration, or ownership/liftability claim from absent evidence. Keep slot 309 unregistered and G1 **PARTIAL**.
- **Attempts:** 0 failed hypotheses; 0 tries on this bounded continuation. Prior E748–E821 history remains recorded in `docs/TASKS.md` and `HANDOFF.md`.

## E828 publication and attribution closeout

Owner commit `4f3d9af2d17a66a46afb1af636c6d59286235d87` (parent `763c9178c7b12123eb432ea48a9aa76b3a2b7d15`) was pushed to `origin main` and fetched. The worktree was clean after publication. The commit changes eight allowlisted documentation paths and passes exact-range attribution, owner identity, committed path, and diff checks.

A fresh fetch of GitHub branches and PR heads found 9 branch refs, 12 PR refs, 0 tags, 15 unique tips, and **2,441 reachable commits**. A Claude-family scan of author/committer names and emails, assistant credit trailers, and generated-with/by signatures found zero matches. The private stash remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`, unchanged per owner instruction. The broader historical attribution policy still reports unrelated hostname-derived and assistant-alias identity fields in 1,070 existing commits; the new commit-range audit passes and unrelated history was not rewritten.

The prior E828 result remains **PARTIAL**. This claim advances no game gate and changes no source or dependency.

## Prepublication repository verification

These commands were run from the workspace root:

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
**516 / 0 skipped / 0 failed**; Trail wiring graph check passed at **9,877 nodes / 13,470 edges**
(deep/meaning layer not built); project index regenerated and checked current at **31,942 folders**.
These repository checks do not establish the `+0x10` ownership contract or advance a game gate.
Logs are private under `local/validation/orch-20261027/e829/`.
