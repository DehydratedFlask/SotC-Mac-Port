# Checkpoint — repo-health round: full-suite green + publish-audit gitlink gap

- Local: 2026-10-22 ~23:40 CDT (America/Chicago)
- Agents: two orchestrator round-2 worker subagents (non-Claude model) + integrator replay
- Tasks: `CPU-01-CTOR-STALE-EXPECTATION-01` (E518), `AUD-PUBLISH-GITLINK-01` (E517)
- Gate: no gate promoted; tooling/repo health only. G1 stays PARTIAL.

## 1. Full `ctest` was red at the inherited tip

At tip `ff7b186` a fresh full build failed **2 of 326** tests: `registration_thunks`
(`test_registration_thunks.cpp:64`) and `clothing_physical_mesh_parameters`
(`test_clothing_physical_mesh_parameters.cpp:205`). Both still asserted that
`run_constructors(recovered_constructors(), st)` stops at slot 23 / `0xe71990`, although
`native/core/startup_ctors.cpp` now registers slots 24–47 and indices 282/290/520/538.
Earlier agents had only re-run the focused `appmain_init_ctor_98a90_slice` test, so the drift
was invisible.

Measured ground truth (temporary diagnostic, removed afterwards):

```
completed=155 blocked=0x0 cb_failed=0 ok=1 table=155
```

`completed` equals `recovered_constructors().size()`, and no row has a null runner. No
implementation defect: `run_constructors` and the table ordering are unchanged and
`startup_ctors.cpp/.hpp` were not touched.

Fix (test-only, two files): both tests now derive the expected stop from the table (first row
with `run == nullptr`) and assert `completed == first_unresolved` plus `ok`/`blocked_on_rva`
consistency; every descriptor-identity, isolation, idempotence, mutex, out-of-range and
foreign-descriptor assertion is retained. `test_startup_ctors.cpp` still pins the explicit index
list and the forced-missing slot-23 callback failure, so a silently removed registration is
still caught.

Replay: fresh configure, `329/329` build, **326/326 pass** (twice).

## 2. Publish-path audit let a bare gitlink/directory name through

`scripts/audit_publish_paths.py` checked only `p.split("/")[:-1]` against
`FORBIDDEN_DIR_COMPONENTS`, so `tools/local` and `docs/reference` passed. Fixed by normalising
empty components (trailing slash, doubled separator) and checking **every** component including
the last; `reference`/`logs` added to the component set to mirror the prefix roots. The code
comment records the tradeoff: a path string cannot distinguish a file from a gitlink, so a file
whose basename is exactly a reserved directory name is also rejected; names with an extension or
extra word (`tools/local.md`, `native/local_tests/a.c`, `docs/reference-notes.md`) still pass.

Correction to the earlier note: `tools/local/` (trailing slash) was already rejected before the
fix; the lax inputs were the no-slash forms plus `docs/reference`/`tools/logs`.

Replay: 12/12 tests (was 10); the new tests fail 6/6 against `git show HEAD:` logic; `--all`
exit 0 over the identical 2334-path set; nothing-staged exit 2 unchanged; all 8 `scripts/test_*.py`
files pass (180 tests).

## Next exact action

Keep every future round ending with a full `ctest` (not only the focused test) and
`python3 -I scripts/audit_publish_paths.py --all`; both are cheap compared with the drift they
exposed.
