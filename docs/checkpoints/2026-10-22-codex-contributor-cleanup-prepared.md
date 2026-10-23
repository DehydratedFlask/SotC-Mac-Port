# Contributor attribution cleanup — prepared review

Local date/time: 2026-10-22 21:53 CDT (America/Chicago). Agent: /root.
Task PUB-01-CLAUDE-CREDIT-CLEANUP-01. Metadata preparation attempt 1 successful;
publication awaits explicit resolution of AGENTS.md no-force-push rule.

User requested continuing the Init branch and removing Claude as a contributor.
The Init-only 299 commits had no matching co-author trailers. The shared published ancestry
contains 70 Claude co-author trailers and no matching author/committer identities.

Exact command: `python3 local/validation/claude-credit-cleanup-20261022/prepare.py --source
<managed-worktree> --output local/validation/claude-credit-cleanup-20261022/review-1531d0f`.
It exited 0. All paths in that command are rooted at the original project checkout except the
explicit managed source path. The script only wrote an isolated private bare review repository.
Source references and remote branches were not changed.

Private report: `local/validation/claude-credit-cleanup-20261022/review-1531d0f/report.json`.
Source Init 1531d0ff9ba8efdd6611c7b3de94bbeb03a50e85; source main
4c83b4df4bca69dfdb4a428912f112c10496bf48. Prepared clean review Init
a35ff3bbeee1da3959c0f3bc1dd6f3de7459454c; clean main
f7a3687b45f53b3c96b669a55e844f054b333c7e.

Observation: removed 70 trailer lines from 70 messages, changed 702 descendant commit IDs,
and independently checked all 1068 tree IDs, original authors, original committers and mapped
parent lists. No signed commits or mergetags were present. Every source tree is identical.
Inference: this is a verified commit-metadata rewrite, not source change or gameplay evidence.
GitHub documents that contributor statistics use default-branch ancestry and may take about
24 hours to refresh after history changes:
https://docs.github.com/en/repositories/viewing-activity-and-data-for-your-repository/viewing-a-projects-contributors

Remaining risk: existing branch/PR descendants retain the old ancestry and must be rebased
before later integration; branch-only cleanup does not clean main's contributor ancestry.
AGENTS.md explicitly forbids force-push, so publication requires a user exception. A scoped
force-with-lease would protect the exact observed remote tips. No force push has occurred.

Next action: continue the independent implementation tasks. After their ordinary publication,
regenerate this verified cleanup from the final source tip. Apply only if the user explicitly
approves the scoped exception; otherwise retain the review artifact and report the constraint.
