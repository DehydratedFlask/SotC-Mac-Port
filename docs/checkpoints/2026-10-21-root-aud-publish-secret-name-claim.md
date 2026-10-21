# Claim: AUD-PUBLISH-SECRET-NAME-FIXTURE-01

- Local time: 2026-10-21 10:40 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `launcher_review`.
- Task status / attempts: ACTIVE; zero failed tries.
- Bounded scope: add synthetic cases to `scripts/test_audit_publish_paths.py` proving nested case-insensitive `.env` and `credentials` filenames are denied, for example `tools/build/.ENV.local` and `docs/private/CREDENTIALS.txt`. No change to the production audit rules. Worker owns only the test file; root owns task/handoff/evidence documentation.
- Prerequisite: `AUD-PUBLISH-EDGE-01` and `AUD-PUBLISH-NESTED-01` are FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command `trail ask "How does audit_publish_paths deny .env and credentials filenames, and which tests cover nested or case-variant names?" --source --in scripts/` located `FORBIDDEN_NAME_RE` at `scripts/audit_publish_paths.py:30-42`. Its `(?i)` flag makes the `credentials` prefix and `.env` component rules case-insensitive. Existing `scripts/test_audit_publish_paths.py:55-75` covers nested forbidden directories/extensions; it has no `.env` or `credentials` filename case.
- The candidate paths stay under allowlisted `tools/` and `docs/` roots, isolating the filename rule from prefix rejection. The test should assert each path produces a violation and avoid inspecting file contents or symlink targets.
- Prerequisite status verified in `docs/TASKS.md`: `AUD-PUBLISH-EDGE-01` and `AUD-PUBLISH-NESTED-01` are FIXTURE_VERIFIED.
- Checkout at claim: `main`, HEAD `de942787325c4c25e9c8f4bd49cc96494b284429`, matching `origin/main`. `git status --short` shows only pre-existing escalated `native/api/CMakeLists.txt` and `native/api/host/`; both are excluded.
- Baseline hashes: `scripts/audit_publish_paths.py` `2ea984cfb38329f4a9ec0b46300b0513998a1a642d224572906cee531653bba3`; `scripts/test_audit_publish_paths.py` `57e5dd223ce8486a9b41a07f7702e2613fbb41d4ffeecee5c88983611d377f44`.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.
- Exact commands/results so far: no task test has run; status NOT RUN. No source or test changes have been made.
- Observation: the live deny regex already contains both filename rules and uses case-insensitive matching. Inference: explicit path fixtures can pin nested and uppercase filename variants without altering production behavior.
- Remaining limits: synthetic path-list inputs only; no file contents, symlink targets, credentials, game data, or live publication is inspected by the new cases. Keep worker logs under `local/validation/aud-publish-secret-name/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this claim checkpoint. No source/test code changes.
- Next exact action after claim publication: assign only `scripts/test_audit_publish_paths.py` to `launcher_review`; review its diff and independently run the acceptance command.

- Claim maintenance: initial trail build --check exited 1 while Task A's worker was adding symbols to scripts/unwrap_self.py and scripts/test_project_tools.py; this was a parallel-edit stale graph, not a Task B source change. Root then ran trail build (exit 0) and trail build --check (exit 0, wiring graph in sync). Recovery logs: local/checkpoints/root-re01-unwrap-input-sidecar-collision-claim/trail-build-after-worker.log and trail-check-after-worker.log; final check log: local/checkpoints/root-re01-unwrap-input-sidecar-collision-complete/trail-check.log. The checkpoint filename matches .gitignore:76 pattern *secret*. The checkpoint was reviewed and contains no credentials or sensitive values; force-add only this allowlisted documentation file for publication.

- Claim publication and dispatch (2026-10-21 10:48 CDT): Task remains ACTIVE, zero failed tries; owner is root plus launcher_review. Exact push command git push origin main exited 0 and advanced de94278..c8dd0b3. git rev-parse HEAD and git rev-parse origin/main both returned c8dd0b3a8f2498709fe0b6d7579b31d4c5367dfa. Pre-commit staged audit passed 9 allowlisted paths and --all passed 1,505; post-commit explicit-path audit also passed all 9 committed paths. The checkpoint itself was force-added because .gitignore:76 ignores filenames containing secret; no credential material is present. Worker assignment: root used collaboration.followup_task for launcher_review to edit only scripts/test_audit_publish_paths.py, run PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py, and save output at local/validation/aud-publish-secret-name/attempt-1.log. Worker and root replay now each pass 7/7; E133 and the completion checkpoint record test/file hashes and limits. Completion status: FIXTURE_VERIFIED, zero failed tries. Commit 8d463abf295b7d761f6e16f4589346f24a7e3c29 published the test, E133, and completion checkpoint; git push origin main exited 0 and advanced c8dd0b3..8d463ab. HEAD and origin/main match. Next exact action: select another bounded task.
