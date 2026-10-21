# Claim: RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01

- Local time: 2026-10-21 11:10 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `launcher_review`.
- Task status / attempts: ACTIVE; 0 failed implementation tries.
- Bounded scope: add one synthetic temporary-text-tree fixture to `scripts/test_ps4_dynlink.py` that checks `name_by_hash()` resolves a requested known NID to the corresponding identifier token and omits a requested NID absent from the tree. Edit only the test file. No parser/build code change, live source-tree claim, game executable, loader, or runtime behavior claim.
- Prerequisite: `RE-02-DYNLINK-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command `trail grep "name_by_hash" --in scripts/` found the definition at `scripts/ps4_dynlink.py:155-170` and caller in `build()` at line 212. The function hashes identifier-like tokens from selected text file suffixes; its documented evidentiary limit is spelling only, not semantics, module version, or signature.
- Live test inspection (`sed -n '1,240p' scripts/test_ps4_dynlink.py`) shows synthetic ELF/parser fixtures and known `nid()` values but no `name_by_hash()` assertion. Registry search confirms this task is not already claimed.
- Live source observation: `name_by_hash()` iterates text roots, filters a fixed source/document suffix set, tokenizes identifier-like strings, and returns only matched requested NIDs. Inference: an isolated temporary tree can exercise the lookup without game data or environment-specific files.
- Checkout at claim: branch `main`, HEAD `219f261ce102aabe6050a3ffa63ff1e4f7114856`, matching `origin/main`. Only pre-existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/` are dirty; exclude them from this task and all commits.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Baseline status: NOT RUN for this new fixture. The suite currently has no direct lookup test. This bounded test demonstrates only synthetic token-to-NID spelling lookup and omission of an absent requested NID. It does not validate NID collision resistance, identifiers in real trees, import semantics, module/version/signature metadata, or loader behavior.
- Private evidence path: `local/validation/re02-dynlink-name-by-hash/`; retain the test log and source/test hashes there.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this checkpoint. No test or production source changed at claim time.
- Publication gates: staged audit, full tracked-path audit, graph/index checks, and cached whitespace check are pending claim publication. Pre-existing escalated host-protocol paths remain unstaged and excluded.

## Claim publication and assignment checkpoint

- Local date/time: 2026-10-21 11:10 CDT (America/Chicago). Integrator: `/root`; worker: `launcher_review` after publication.
- Exact claim publication command/result and commit: pending.
- Observation: worker scope is only `scripts/test_ps4_dynlink.py`; root owns task and handoff records. No fixture has run yet for this task.
- Remaining risks: NID-to-name is a spelling hint only; no real ELF or import behavior is validated.
- Next exact action: publish the claim, assign the test-only file, review its diff and private log/hash, replay the acceptance command, then record and publish completion.
