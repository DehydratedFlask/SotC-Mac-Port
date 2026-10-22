# APP-01 core-client internal error copy — completion checkpoint

- Time / timezone: 2026-10-21 23:06 CDT (America/Chicago); owner/integrator `/root`.
- Task: `APP-01-CORECLIENT-INTERNAL-ERROR-COPY-FIXTURE-01`, child of `APP-01`.
- Files changed: `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`, `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint. Production source was unchanged.
- Test edit: `failureTextCoversTheErrorsTheLauncherCanSee` now asserts that both
  `.internalError` and `.unknown(5)` return the exact fallback `The game core reported an error.`
  The existing equality assertion remains.
- Final test SHA256 `7dfd2f4d50479d0cfb208643b8d0cac0dd91a1f2742f4e4a1c0371ad84548c92`; unchanged
  `CoreClient.swift` SHA256 `9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5`.
- Exact command: `swift test --package-path launcher --scratch-path
  local/build/app01-internal-error-copy --filter CoreClientTests`; exit 0, 22 tests across two
  suites (9 `NativeCoreClientTests`, 13 `CoreClientTests`). Baseline log SHA256
  `22286765f4a240ea0890103ca94d9503186383e9635c1e245aa5e5739d38c647`; root replay log SHA256
  `bf2010b1150e5b5578cd4fe8c9ec65d7153d7ce96e88dbdc3cb1e5bf19eeb8bb`; both logs are under
  `local/validation/`.
- Root `git diff --check -- launcher/Tests/LauncherCoreTests/CoreClientTests.swift` exited 0.
- Index and source-graph checks: after adding the completion checkpoint,
  `python3 scripts/project_index.py` exited 0 with 20,472 folders indexed;
  `python3 scripts/project_index.py --check` exited 0, current. Logs
  `/tmp/round78-app01-coreclient-complete-project-index.log` SHA256
  `e2c570d4b9a79915f37c1110d3adf04752abddeb1797817424ed690f17cbc66b` and
  `/tmp/round78-app01-coreclient-complete-project-index-check.log` SHA256
  `bb426595c14eb0f30dcb10b46bc5b6f5f411f8e7d29a70217ad554cb25996232`; `docs/FOLDERS.md` lists
  403 checkpoint files.
- The first `trail build --check` exited 1 and reported the changed test file, test type, and test
  method as stale. `trail build` exited 0 and rebuilt the wiring graph at 3,599 nodes; the final
  `trail build --check` exited 0 and reported the graph in sync. Logs:
  `/tmp/round78-app01-coreclient-trail-build.log` SHA256
  `8d52b0108b34389fc1f2c465b2a8d77a660d78cbcbf159c537fda3c7fe4b4b61` and
  `/tmp/round78-app01-coreclient-trail-check-final.log` SHA256
  `4b0aecb55c6f7700bf0861a2fa1def317d581b34ef946c1b50dfe1405c2da70b`. The deep semantic layer
  remains unbuilt.
- Claim publication: commit `8c33c317cc5cb6863b3ab618ebfb28d36557570a` was pushed to `origin/main`
  and fetch-verified with 0/0 divergence. Its preflight `git diff --cached --check` passed;
  staged audit passed 4 paths (log SHA256
  `4cacc72cf1a3521de490acd2890b4edbdf90f6a84b10b91eafadec93b4811719`), full audit passed 1,671
  paths (log SHA256 `b1a04a3d5a324285e6f084e6f181a1505f2d9ef090a57b09d3b070993995ff88`).
- Completion publication: `git diff --cached --check` passed; staged
  `python3 -I scripts/audit_publish_paths.py` passed 6 allowlisted paths (log SHA256
  `5634be08b800d8892037718063d942af309bcee44fef5cf4bf1482903de4312f`); full audit passed 1,672
  allowlisted paths (log SHA256 `876030b78a5d01b3b07393b278cb8823e6bcec0c27557f9541f5bbe9df19d62d`).
  Before commit, fetch exited 0 and divergence was 0/0. Completion commit
  `2152dfbbe90699d550b07d26b90d3bfd7d18394b` was pushed; post-push fetch verified `HEAD ==
  origin/main` and 0/0 divergence. Existing native API edits remained unstaged.
- Observed: exact fallback mapping is asserted for both `.internalError` and `.unknown(5)`;
  the focused suite passes. Inference: this locks down the current fallback string in the test
  suite. No UI presentation or core/game behavior is inferred.
- Remaining risks: no runtime UI or core behavior is tested; this was intentionally test-only.
  User-owned `native/api/CMakeLists.txt` and `native/api/host/` edits remain untouched. Attempts: 0
  failed implementation tries.
- Task status: `FIXTURE_VERIFIED`.
- Exact next action: scout another independent bounded task outside the PSARC and CoreClient files;
  leave negative-limit behavior unclaimed pending the user's answer.
