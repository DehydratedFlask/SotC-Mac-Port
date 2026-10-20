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
- Targeted check result/logs: `NOT RUN` at claim; worker has not yet been dispatched. Staged/full audits, fetch/divergence, commit and push are also pending.
- Observation: `docs/ASSETS.md` records 0 casefold collisions within/across the cachedFiles base/patch pair. The remaining five manifests are not covered by that statement.
- Inference: none about runtime path resolution or archive order.
- Risks: manifests may not represent every archive version on disk; report will hash its exact inputs. No change to the acceptance gates.
- Publication preparation: `git diff --cached --check` exit 0 (`logs/root-data02-path-case-claim-staged-diff-check.log`); `python3 -I scripts/audit_publish_paths.py` exit 0 for 4 allowlisted paths (`logs/root-data02-path-case-claim-staged-audit.log`); `python3 -I scripts/audit_publish_paths.py --all` exit 0 for 623 tracked paths (`logs/root-data02-path-case-claim-all-audit.log`); `git fetch origin main` exit 0 and `git rev-list --left-right --count HEAD...origin/main` returned `0 0` (`logs/root-data02-path-case-claim-fetch.log`, `...divergence.log`). `git status --short` showed exactly four staged docs paths plus ignored/untracked `tools/graft/` (`...status.log`).
- Next exact action: commit and push the four allowlisted staged documentation paths; then dispatch one offline manifest-only worker and independently replay its result.
