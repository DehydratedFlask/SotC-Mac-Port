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
- A fresh audit found 9 GitHub branch heads and no remote tags. Two stale
  `cursor/parallel-foundations-*` refs exposed 70 Claude/Anthropic trailers across their shared
  history; no PR was open for either branch. Both were atomically updated with exact
  `--force-with-lease` expectations after a disposable review verified the message-only rewrite.
- The rewrite preserves trees, author/committer headers, and mapped parents for all 1,074 commits
  in the WIP ancestry; it removes 70 trailer lines and changes 708 commit IDs. No signed or
  mergetag commits were found. A post-push scan of 2,341 commits reachable from the remote heads
  and tags found zero Claude/Anthropic credit trailers and zero Claude/Anthropic author/committer
  identities. The exact tips and evidence are in [EVIDENCE](EVIDENCE.md) and
  [the publication checkpoint](checkpoints/2026-10-26-attribution-cleanup-published.md).
- Refreshed after `d0faf04e3758d1fa6c9843fb453eb16bfc4af837`: all 9 fetched GitHub branch heads,
  no remote tag refs, and **2,350 unique reachable commits** were parsed. The remote-ref scan found
  zero Claude/Anthropic author or committer identities and zero Claude/Anthropic co-author or
  generated-by credit lines. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-d0faf04e.json`;
  see E720 and the [refreshed scan checkpoint](checkpoints/2026-10-27-post-d0faf04-attribution-scan.md).
- Refreshed after main commit `209ff474d3d9d7727084076a6f27ec9c0eacdc39`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,351 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-209ff474.json`;
  see E722 and the [latest scan checkpoint](checkpoints/2026-10-27-post-209ff474-attribution-scan.md).
- Refreshed after main commit `d0b454b607c0799a6f5b03ffe51196e93d91279c`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,352 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-d0b454b6.json`;
  see E724 and the [latest scan checkpoint](checkpoints/2026-10-27-post-d0b454b6-attribution-scan.md).
- Refreshed after main commit `e6c8d33ed93867253f397600684dd1e8d59c2fa5`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,353 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-e6c8d33e.json`;
  see E725 and the [latest scan checkpoint](checkpoints/2026-10-27-post-e6c8d33e-attribution-scan.md).
- Refreshed after main commit `9e61358b9f6f81e6b17208911364a14ab0852609`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,354 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-9e61358b.json`;
  see E727 and the [latest scan checkpoint](checkpoints/2026-10-27-post-9e61358b-attribution-scan.md).
- Refreshed after main commit `53634a156b064d8c78630f64240241d58059ee0b`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,355 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-53634a15.json`;
  see E729 and the [latest scan checkpoint](checkpoints/2026-10-27-post-53634a15-attribution-scan.md).
- Refreshed after main commit `4682a33743688d36ced7fec5e00401addceef11c`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,356 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-4682a337.json`;
  see E730 and the [latest scan checkpoint](checkpoints/2026-10-27-post-4682a337-attribution-scan.md).
- Refreshed after main commit `c8f23500f36d030729752ab9a1fbc1a754bd3a0d`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,357 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-c8f23500.json`;
  see E732 and the [latest scan checkpoint](checkpoints/2026-10-27-post-c8f23500-attribution-scan.md).
- Refreshed after main commit `cf94e831aea6b2ec60826334bb04206535e7d7d3`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,358 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-cf94e831.json`;
  see E733 and the [latest scan checkpoint](checkpoints/2026-10-27-post-cf94e831-attribution-scan.md).
- Refreshed after main commit `5315489cf9706b3713945c51fe0267a970614003`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,359 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-5315489c.json`;
  see E734 and the [latest scan checkpoint](checkpoints/2026-10-27-post-5315489c-attribution-scan.md).
- Refreshed after main commit `690eda9335f506e8f881bf3493429538d97b4bdb`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,360 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-690eda93.json`;
  see E735 and the [latest scan checkpoint](checkpoints/2026-10-27-post-690eda93-attribution-scan.md).
- Refreshed after main commit `0fec9e9ede5db82b221034cfbbb2603d34b15a85`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,361 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-0fec9e9e.json`;
  see E737 and the [latest scan checkpoint](checkpoints/2026-10-27-post-0fec9e9e-attribution-scan.md).
- Refreshed after main commit `07e2055f4978e0d18208bf4bf4e5edffb2f1b3f7`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,362 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-07e2055f.json`;
  see E738 and the [latest scan checkpoint](checkpoints/2026-10-27-post-07e2055f-attribution-scan.md).
- Refreshed after main commit `3d9f58b165579012bc1b6c8ae06621e2b1278f41`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,363 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-3d9f58b1.json`;
  see E739 and the [latest scan checkpoint](checkpoints/2026-10-27-post-3d9f58b1-attribution-scan.md).
- Refreshed after main commit `447557f5746d1550fbae6c0e655c80165aa2cd02`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,364 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-447557f5.json`;
  see E741 and the [latest scan checkpoint](checkpoints/2026-10-27-post-447557f5-attribution-scan.md).
- Refreshed after main commit `ff90a38919d6f199e560b9d74107f8c2d292e100`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,365 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-ff90a389.json`;
  see E742 and the [latest scan checkpoint](checkpoints/2026-10-27-post-ff90a389-attribution-scan.md).
- Refreshed after main commit `ca87a357490b72877799b6ac589b837fac4b386c`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,366 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-ca87a357.json`;
  see E743 and the [latest scan checkpoint](checkpoints/2026-10-27-post-ca87a357-attribution-scan.md).
- The historical `f6604a86e60e` `orchestrator@local` author/committer remains the one separate
  `--all` gate finding. It is not a Claude/Anthropic attribution and was not changed by this task.

## Why the host-derived identity matters

Git writes `<login>@<hostname>` when `user.email` is unset. Those commits cannot be associated
with the owner's GitHub account and appear as a separate contributor in the repository graph.
Setting the repository-local identity to the owner's GitHub noreply address, as this repository
does, prevents that.
