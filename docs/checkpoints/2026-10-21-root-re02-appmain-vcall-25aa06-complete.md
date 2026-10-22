# RE-02 app-main +0x20 callsite — bounded static path verified

- Time / timezone: 2026-10-21 22:42 CDT (America/Chicago); worker `/root/re02_vcall_25aa06`,
  shared-document integrator `/root` (Codex desktop task
  `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `RE-02-APPMAIN-VCALL-25AA06-01`, bounded complete as
  `STATIC_PATH_BYTE_VERIFIED`. The runtime call edge remains unresolved. No G1 claim.
- Files changed: worker created `tools/lift/verify_appmain_vcall_25aa06.py` and private evidence
  under `local/re/evidence/re02-appmain-vcall-25aa06/`; root updated `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/FOLDERS.md`, and this checkpoint. Existing user-owned
  `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and excluded.
- Pinned input: `local/re/eboot.elf`, size 65,214,308, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- REA attempt 1: `mcp__rea__analyze_function` for `0x25a8d0` timed out after 330,000 ms during
  Ghidra 12.1.4 startup; run ID `1d30f13b-1bf7-4897-a088-b59b8e01e6d4`. The target SHA matched.
  No REA records were produced; an empty export was retained and the session closed. Temporary
  provider logs and raw response were unavailable after cleanup and were not reconstructed. No
  same-profile retry was made. The offline analysis completed as a separate byte-verification path.
- Private evidence: report
  `local/re/evidence/re02-appmain-vcall-25aa06/report.md` SHA-256
  `37678cdf2bcc535ab610b73a7a0d89f10ebe6568b271dc0ebaabadb700410157`; verifier SHA-256
  `9aab3ab6c7b46a7dbdd203c5fe100121cc3cf29c0c3300c4a70187cd8a8680a4`; checksum manifest SHA-256
  `f2edcda793e145d31e928745e3e45cf00efd63ff508516fae0e88afe16a7c429`. The worker verifier output
  SHA-256 is `00ed6182b87b9860cc1eee41f58cba7966b70dd660e32b113cfc789c752adf86`.
- Root replay command: `python3 -I tools/lift/verify_appmain_vcall_25aa06.py`; exit 0. Root stdout
  `local/checkpoints/round78-re02-25aa06-root.stdout` is 19,089 bytes and SHA-256
  `00ed6182b87b9860cc1eee41f58cba7966b70dd660e32b113cfc789c752adf86`, matching worker output.
  Root stderr `local/checkpoints/round78-re02-25aa06-root.stderr` is empty with SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; exit file
  `local/checkpoints/round78-re02-25aa06-root.exit` contains `0`.
- Static observations: callsite `0x25aa06` reads RAX from `[R15]` and encodes `call [RAX+0x20]`.
  Constructor code at `0x536ac7` forms address point `0x15c8cd8` and stores it at `0x536afa`;
  its `+0x18` word at `0x15c8cf0` points to app-main `0x25a8d0`, while `+0x20` at `0x15c8cf8`
  contains `0x531d60`, mapped executable in PT_LOAD #0 and beginning with byte `0x55`. Two other
  file-backed address-point windows have `0x25a8d0` at `+0x18` but `0x255130` at `+0x20`:
  `0x159c590` / `0x159c598` and `0x15d5b98` / `0x15d5ba0`.
- Observation versus inference: instruction/data bytes and file mappings are verified. The current
  receiver's table at the call is not established across all paths. `0x531d60` and `0x255130` are
  static table values in separate windows; neither is claimed as a unique runtime target. No class,
  method, ABI, semantics, reachability, runtime, or game behavior is inferred.
- Attempt count: one bounded offline analysis; zero failed analysis hypotheses. The REA startup
  timeout is recorded as failed try 1. Verifier result is `PASS_UNRESOLVED_EDGE`; unresolved means
  the verifier did not manufacture a unique call target.
- Remaining risk: resolving the indirect edge requires proving which address point the receiver
  holds at the call. Any runtime-vptr investigation needs a distinct claim and a separate REA-use
  reassessment.
- Index/graph check time: 2026-10-21 22:45 CDT. `python3 scripts/project_index.py` exits 0 with
  `19273 folders indexed; written`; `python3 scripts/project_index.py --check` exits 0 with
  `19273 folders indexed; current`. Captured command logs outside the indexed workspace:
  `/tmp/round78-re02-25aa06-project-index-final.log` SHA-256
  `239aa785397fb5e09903ca5d4bd239cd908623441b2bdc05af07795d1da626ee`, and
  `/tmp/round78-re02-25aa06-project-index-final-check.log` SHA-256
  `dac802dbcc2170c6afc6c76ba90d758ce2a191785c6592c57a11df38781b043e`. Earlier checks were
  stale because additional private checkpoint logs were created after the inventory was written;
  after all such files existed, the final regeneration/check pair passed.
- `trail build --check` exits 0: wiring graph in sync, 3,597 nodes; deep semantic layer remains
  unbuilt. Captured output `local/checkpoints/round78-re02-25aa06-trail-check.log` SHA-256
  `9583fa2b05846d0bd3d3e3b8396ac5d45e57dfa97d95278f6ee5943dc4b2c0f3`.
- Publication preflight time: 2026-10-21 22:45 CDT. Staged allowlist: `HANDOFF.md`,
  `docs/EVIDENCE.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, this checkpoint, the 25ac9f post-push
  checkpoint sync, the DATA-02 claim checkpoint, and `tools/lift/verify_appmain_vcall_25aa06.py`.
  User-owned native API files remain unstaged. `git diff --cached --check` exits 0, empty output
  SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; log
  `/tmp/round78-re02-25aa06-data02-claim-diff-check.log`. The staged audit exits 0, accepts 8
  paths, SHA-256 `8cae57e01b60218ad671ad4f23c7ab7c7bd8856d969328d6479bc004e4e55338`; log
  `/tmp/round78-re02-25aa06-data02-claim-staged-audit.log`. The full audit exits 0, accepts 1,667
  paths, SHA-256 `a24d31532815c136fad3ea12efcb696b95197d87b38e447db5f4d1ec84d393db`; log
  `/tmp/round78-re02-25aa06-data02-claim-full-audit.log`.
- Commit/push and fetch verification will be recorded in the publication checkpoint below after
  those operations complete.
- Task status: `STATIC_PATH_BYTE_VERIFIED`; G0 remains PARTIAL and G1–G6 remain NOT RUN.

## Completion and claim publication checkpoint

- Time / timezone: 2026-10-21 22:47 CDT (America/Chicago); integrator `/root`.
- Commit: `f9fc4302f1702af6677e90121a6040becc533d5e`, message
  `verify unresolved app-main call and claim TOC crosstab`. `git push origin main` succeeded.
- Fetch verification: `git fetch origin`, `git rev-parse HEAD`, `git rev-parse origin/main`, and
  `git rev-list --left-right --count HEAD...origin/main` show matching `f9fc4302f1702af6677e90121a6040becc533d5e`,
  divergence `0 0`. Private post-push record `local/checkpoints/round78-re02-25aa06-data02-claim-postpush.log`
  SHA-256 `7c434d2e6dff50eecac5ac952fdd14be140e42e041a373377dc9bcc8d2bb485d`.
- Final preflight accepted 8 staged paths and 1,667 full-tree paths; `git diff --cached --check`
  passed. Logs under `/tmp/round78-25aa06-data02-final-*.log`; exact staged/full audit hashes are
  `8cae57e01b60218ad671ad4f23c7ab7c7bd8856d969328d6479bc004e4e55338` and
  `a24d31532815c136fad3ea12efcb696b95197d87b38e447db5f4d1ec84d393db`. User-owned native API
  files were not staged.
- Project index was regenerated after adding the post-push log; `python3 scripts/project_index.py`
  and `python3 scripts/project_index.py --check` both exit 0 at 19,273 folders. The wiring graph
  check passes at 3,597 nodes; deep layer remains unbuilt.
- The DATA-02 claim was published in the same commit. Only after push/fetch verification, root
  assigned `/root/round78_next_task_scout` to implement the private aggregate-only analyzer. No
  manifest data was accessed before claim publication.
- Observation versus inference: publication and fetch state are verified; this does not advance any
  game acceptance gate. The `0x25aa06` call edge remains unresolved.
