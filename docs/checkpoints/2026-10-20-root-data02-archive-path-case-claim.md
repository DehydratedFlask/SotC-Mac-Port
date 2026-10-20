# DATA-02 all-archive path/case audit claim

- Time: 2026-10-20 04:24 CDT (America/Chicago).
- Agent / integrator: `/root`; task `DATA-02-ARCHIVE-PATH-CASE-01`.
- Status: `ACTIVE`; 0 failed hypotheses/attempts before this claim.
- Bounded outcome: verify exact duplicate path records and Unicode `casefold()` collision groups within each of the seven extracted PSARC member manifests, plus differing spellings that fold together across archive manifests. This extends the existing cachedFiles-only collision count in `docs/ASSETS.md`.

## Inputs and boundaries

- Inputs are the seven existing `local/manifests/assets/CUSA08809-*.members.jsonl` files: base cachedFiles, patch cachedfiles, pak0, pak1, pak2, pak3 and pakN.
- The audit reads path/locator metadata only. It does not open original PKGs, extracted archive payloads, or write any member data. It does not change `scripts/psarc.py`, `docs/ASSETS.md`, or any source/reference checkout.
- This establishes only the contents of the current manifests. A collision count does not prove filesystem behavior, Fios behavior, title loader policy, overlay precedence, or game use.
- Private worker-owned outputs: `local/validation/data02-archive-path-case-01/` containing a bounded checker, two deterministic JSON results, command/status logs, input hashes and a checksum manifest. Root owns all shared documentation.

## Check and next action

- Claim-time command `python3 scripts/project_index.py`: exit 0; 4,156 folders indexed; output in `logs/root-data02-path-case-claim-project-index.log`.
- Claim-time command `graft check`: exit 0; wiring graph in sync, deep layer unbuilt with 1,225 nodes pending; output in `logs/root-data02-path-case-claim-graft-check.log`.
- Claim-time command `git diff --check`: exit 0; no whitespace errors.
- Planned targeted check: run `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/data02-archive-path-case-01/check.py` twice against the seven exact manifests, compare JSON outputs with `cmp`, verify private checksums, then rerun the checker from root.
- Targeted check result/logs: `NOT RUN` at claim; worker `/root/data02_archive_path_case` was dispatched after publication and confirmed seven inputs plus Python 3.15.0 before implementing the private checker. One command-capture wrapper error occurred: `python3 --version` ran, then zsh rejected assignment to its reserved `status` variable. The worker is preserving the wrapper error under the private dossier and correcting the capture variable. No analysis hypothesis or targeted audit failed; the first manifest scan remains pending.
- Observation: `docs/ASSETS.md` records 0 casefold collisions within/across the cachedFiles base/patch pair. The remaining five manifests are not covered by that statement.
- Inference: none about runtime path resolution or archive order.
- Risks: manifests may not represent every archive version on disk; report will hash its exact inputs. No change to the acceptance gates.
- Publication checks at claim: `git diff --cached --check` exit 0 (`logs/root-data02-path-case-claim-staged-diff-check.log`); `python3 -I scripts/audit_publish_paths.py` exit 0 for 4 allowlisted paths (`logs/root-data02-path-case-claim-staged-audit.log`); `python3 -I scripts/audit_publish_paths.py --all` exit 0 for 623 tracked paths (`logs/root-data02-path-case-claim-all-audit.log`); `git fetch origin main` exit 0 and `git rev-list --left-right --count HEAD...origin/main` returned `0 0` (`logs/root-data02-path-case-claim-fetch.log`, `...divergence.log`). `git status --short` showed exactly four staged docs paths plus untracked `tools/graft/` (`...status.log`). Claim commit `7560229` was pushed to `origin/main`; no audit failed.

### Worker wrapper correction checkpoint — 2026-10-20 04:28 CDT, `/root`

- Observation: worker reported `python3 --version` ran, then its zsh wrapper failed while assigning to the reserved variable `status`. It is preserving the wrapper error and correcting the capture variable. No manifest audit/hypothesis failed; no failure-count increment. Exact wrapper evidence is private under `local/validation/data02-archive-path-case-01/`.
- Root checks: `python3 scripts/project_index.py` exit 0, 4,157 folders indexed (`logs/root-data02-case-wrapper-project-index.log`); `graft check` exit 0, wiring synced with deep layer unbuilt and 1,225 nodes pending (`logs/root-data02-case-wrapper-graft-check.log`); `git diff --check` exit 0 (`logs/root-data02-case-wrapper-diff-check.log`). The private worker folder changed the generated folder index.
- Status: worker is correcting command capture; targeted path audit remains NOT RUN. No source, payload, reference or provider operation was involved in this correction.
- Pre-push commands: `git diff --cached --check` exit 0 (`logs/root-data02-case-wrapper-staged-diff-check.log`); `python3 -I scripts/audit_publish_paths.py` exit 0 for 3 allowlisted documentation paths (`logs/root-data02-case-wrapper-staged-audit.log`); `python3 -I scripts/audit_publish_paths.py --all` exit 0 for 623 tracked paths (`logs/root-data02-case-wrapper-all-audit.log`); `git fetch origin main` exit 0 and divergence `0 0` (`logs/root-data02-case-wrapper-fetch.log`, `...divergence.log`). Status lists the 3 intended paths plus `tools/graft/` (`...status.log`).
- Next exact action: commit and push this wrapper-correction checkpoint, then await the DATA-02 manifest audit and separately serialized RackQuery query.
- Next exact action: await the worker, then independently verify private hashes and rerun its deterministic checker before updating shared evidence.


### Root replay and completion — 2026-10-20 04:34 CDT, `/root`

- Final task status: `ARTIFACT_VERIFIED`; failed analysis hypotheses: 0. The worker wrapper correction and the root's first checksum command from the private dossier directory were command-capture/working-directory corrections only.
- Worker checksum replay: from the repository root, `shasum -a 256 -c local/validation/data02-archive-path-case-01/SHA256SUMS` exited 0 (32 entries, all `OK`). Root log: `local/validation/data02-archive-path-case-01-root-review/worker-checksum-verify.log`. The earlier command `cd local/validation/data02-archive-path-case-01 && shasum -a 256 -c SHA256SUMS` exited 1 because the manifest stores repository-relative paths; all entries appeared missing from that working directory. It was immediately corrected with the repository-root command; no audit input changed.
- Root deterministic rerun: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/data02-archive-path-case-01/check.py` exited 0, Python 3.15.0; root JSON matched both worker run outputs (`cmp` exit 0 for each) and SHA-256 `7b01a6762362c0cc49e07759b3ba19cbbb174e415a3db9d9711308b9b654128d`. Root output is `local/validation/data02-archive-path-case-01-root-review/rerun.json`; aggregate root record is `verification.json`, SHA-256 `b706b935c28ed3915ac9c860d3fd2160a63c23ddba93ed765f499e3a5c054019`.
- Observed result: seven exact manifests, 170,451 records, 21 archive pairs, zero exact duplicate groups, zero within-manifest casefold groups, and zero differently spelled cross-archive equivalents. Metadata paths only. No payload, filesystem, Fios, archive precedence, mount order, title-loader, runtime, or game-use claim.
- Shared docs updated: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/ASSETS.md`, `docs/FOLDERS.md`, and this checkpoint. `INSTALL_LOG.md` unchanged.
- Root ran `python3 scripts/project_index.py` after creating the root-review folder: exit 0, 4,158 folders indexed (`logs/root-data02-archive-case-complete-project-index.log`); `graft check` exit 0, wiring synced, deep layer unbuilt with 1,225 nodes pending (`logs/root-data02-archive-case-complete-graft-check.log`). `git diff --check` passed.
- Next exact action: publish the allowlisted documentation checkpoint after staged/full path audits and zero-divergence check; then continue independent task work.
