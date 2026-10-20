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
