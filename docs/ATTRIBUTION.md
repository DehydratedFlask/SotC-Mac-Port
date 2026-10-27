# Commit attribution policy

The repository owner is the only contributor. Assistants, agents and models are tooling, never
authors, co-authors or contributors. This policy applies to every commit, tag, PR body, issue,
release note and contributor list, and it is enforced by
`scripts/check_commit_attribution.py` before every push.

## Rules

1. Author and committer must be the owner identity (`Johnny Wang
   <dehydratedflask@users.noreply.github.com>`), which is what maps to the owner's GitHub
   account. A host-derived address such as `<login>@<host>.local` is rejected: it creates an
   unlinked ghost contributor.
2. No assistant credit trailers or signatures: `Co-Authored-By: Claude ...`,
   `Co-authored-by: <assistant>`, `Generated with [Claude Code]`, robot sign-offs, or any
   equivalent wording.
3. Agent aliases (`claude`, `anthropic`, `sonnet`, `opus`, `haiku`, `codex`, `opencode`,
   `cursor agent`, `orchestrator`, `subagent`, `assistant`, `chatgpt`, `openai`, `deepseek`) may
   not appear as an author/committer name or e-mail.
4. Commits may *discuss* this policy in their message body; the gate only rejects credit trailers
   and generated-by signatures, not prose.
5. Subagents that edit code must not run `git commit`, `git tag` or `git push`. The integrator
   commits under the owner identity.
6. Published history is not rewritten, except an owner-approved attribution cleanup performed
   with `--force-with-lease` on the agreed branch.

## Gate

```
python3 -I scripts/check_commit_attribution.py            # identity + commits not yet on upstream
python3 -I scripts/check_commit_attribution.py --all      # every commit reachable from HEAD
python3 -I scripts/check_commit_attribution.py --range A..B
python3 -I scripts/check_commit_attribution.py --identity # effective git identity only
python3 -I scripts/test_check_commit_attribution.py       # 13 fixtures
```

Exit 0 means clean; exit 1 lists every violation. Add it next to the publish-path audit in the
pre-push sequence.

## Audit state (2026-10-26)

- Effective identity: owner, clean.
- `--all` over `main`: **one** violation, the historical commit
  `f6604a86e60e` authored and committed as `orchestrator <orchestrator@local>`.
  It is not an Anthropic/Claude attribution and is not linked to any GitHub account, so it does
  not add a Claude contributor. Rewriting it is a separate owner decision and is outside the
  requested Claude-trailer cleanup.
- The 2026-10-23 audit covered the then-current main/Init refs and found zero Claude/Anthropic
  credit trailers across 1,078 reachable commits. A fresh all-remote-ref audit found that
  `origin/main` and seven other fetched branches are clean, while the stale
  `cursor/parallel-foundations-86ac55b` branch and its one-commit WIP descendant still expose 70
  of those trailers. No PR is open for either branch.
- A disposable cleanup review now verifies the exact message-only rewrite: all 1,074 commits in
  the WIP ancestry preserve their trees, author/committer headers, and mapped parents; 70 trailer
  lines are removed, 708 commit IDs change, and there are no signed/mergetag commits. Candidate
  tips are recorded in [EVIDENCE](EVIDENCE.md) and
  [the checkpoint](checkpoints/2026-10-26-resume-integration-attribution-review.md). The original
  remote refs remain unchanged pending the exact-lease push authorized by the owner.

## Why the host-derived identity matters

Git writes `<login>@<hostname>` when `user.email` is unset. Those commits cannot be associated
with the owner's GitHub account and appear as a separate contributor in the repository graph.
Setting the repository-local identity to the owner's GitHub noreply address, as this repository
does, prevents that.
