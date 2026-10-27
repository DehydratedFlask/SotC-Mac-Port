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
- Refreshed after main commit `4cb0afcd4f5dfed883858abc73980a93649f4228`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,367 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-4cb0afcd.json`;
  see E744 and the [latest scan checkpoint](checkpoints/2026-10-27-post-4cb0afcd-attribution-scan.md).
- Refreshed after main commit `f033ed8489b839a5fafc16029e717ad266d3e141`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,368 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-f033ed84.json`;
  see E746 and the [latest scan checkpoint](checkpoints/2026-10-27-post-f033ed84-attribution-scan.md).
- Refreshed after main commit `c4582888ec2bc13f3f5f09b17a9a0178c2e24981`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,369 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-c4582888.json`;
  see E747 and the [latest scan checkpoint](checkpoints/2026-10-27-post-c4582888-attribution-scan.md).
- Refreshed after main commit `fc522ee73c0f01e4c5e0a90abdb00ea1c696bf24`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,371 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-fc522ee7.json`;
  see E749 and the [latest scan checkpoint](checkpoints/2026-10-27-post-fc522ee7-attribution-scan.md).
- Refreshed after main commit `cf1d019a145492c3164f98ef9a9fdec54051e42b`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,374 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-cf1d019a.json`;
  see E752 and the [latest scan checkpoint](checkpoints/2026-10-27-post-cf1d019a-attribution-scan.md).
- Refreshed after main commit `5c5c2758a4c6cd8c3891365f149c8f0d91baf5ab`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,375 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-5c5c2758.json`;
  see E753 and the [latest scan checkpoint](checkpoints/2026-10-27-post-5c5c2758-attribution-scan.md).
- Refreshed after main commit `7068f06a33c069420445b9c40279beb6bba71fc3`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,376 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261026/claude-credit-cleanup-review/remote-scan-after-7068f06a.json`;
  see E755 and the [latest scan checkpoint](checkpoints/2026-10-27-slot309-helper-contracts-post7068-scan.md).
- Refreshed after main commit `f914bcb26fb201e372dcc9c772b84baf6ad82eb4`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,377 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-f914bcb2.json`;
  see E756 and the [latest scan checkpoint](checkpoints/2026-10-27-slot309-lifecycle-callbacks-post-f914.md).
- Refreshed after main commit `329046c759199cb3e7a69919bcf76417de933b95`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,378 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-329046c7.json`;
  see E758 and the [latest scan checkpoint](checkpoints/2026-10-27-post-329046c7-attribution-scan.md).
- Refreshed after main commit `d71c3ccff1dd2eced3aac42aa61d1b949cbfd357`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,379 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-d71c3ccf.json`;
  see E759 and the [latest scan checkpoint](checkpoints/2026-10-27-slot309-rbp38-writer-order-post-d71.md).
- Refreshed after main commit `58e37ed335e2013fa450af8cfcb280a4818a93eb`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,380 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-58e37ed3-refresh.json`;
  see E762 and the [latest scan checkpoint](checkpoints/2026-10-27-slot309-stash-attribution-audit.md).
- Refreshed after main commit `1a44ce550f10045989de544142b80501dc929d2f`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,381 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-1a44ce55.json`;
  see E763 and the [latest scan checkpoint](checkpoints/2026-10-27-post-1a44ce55-attribution-scan.md).
- Refreshed after main commit `60b5947ca80695424d7fb2ecfacc269958b301ce`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,382 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-60b5947c.json`;
  see E764 and the [latest scan checkpoint](checkpoints/2026-10-27-post-60b5947c-attribution-scan.md).
- Refreshed after main commit `32f507e033a6a374b03bf0109aefdfececfb4b42`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,383 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-32f507e0.json`;
  see E766 and the [latest scan checkpoint](checkpoints/2026-10-27-slot309-table-consumer-post-32f-scan.md).
- Refreshed after main commit `4b21a234f127852910cf5252672ae6a5c6f8be09`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,384 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-4b21a234.json`;
  see E767 and the [latest scan checkpoint](checkpoints/2026-10-27-post-4b21a234-attribution-scan.md).
- Refreshed after main commit `9d87ef5baf13fec06979693ff07316bfe034830d`: all 9 fetched GitHub
  branch heads, no remote tag refs, and **2,385 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic author or committer identities and zero Claude/Anthropic credit-line
  matches. The private report is
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-9d87ef5b.json`;
  see E768 and the [latest scan checkpoint](checkpoints/2026-10-27-post-9d87ef5b-attribution-scan.md).
- Refreshed after owner-authored main commit `7e67df61e58eb766d8bdf6e0b88f3029285b60ea`: all 9
  fetched GitHub branch heads, no remote tag refs, and **2,386 unique reachable commits** were
  parsed. The scan found zero Claude/Anthropic identity or credit-line matches. Private report:
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-7e67df61.json`;
  see E769 and the [latest scan checkpoint](checkpoints/2026-10-27-post-7e67df61-attribution-scope-closure.md).
- Refreshed against current `origin/main == 83f78f4fc43b2ec5cbd6b7dab0eab81d46af53d3`: all 9
  fetched GitHub branch heads, no remote tag refs, and **2,387 unique reachable commits** were
  parsed. The scan found zero Claude/Anthropic identity or credit-line matches. Private report:
  `local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-resume-after-83f78f4f.json`;
  see E771 and [the latest checkpoint](checkpoints/2026-10-27-slot309-saved-project-xrefs-post-83f.md).
- A separate read-only audit of all 26 local refs parsed **2,783 unique commits** and found 70
  Claude/Anthropic attribution matches, all reachable only from `refs/stash`; none are reachable
  from a local branch or remote ref. The private audit script and JSON are under
  `local/validation/orch-20261027/claude-credit-cleanup-review/` and are hash-recorded in the E762
  checkpoint. On 2026-10-27, the owner directed leaving these private recovery refs unchanged;
  they are excluded from the published-history cleanup.
- The historical `f6604a86e60e` `orchestrator@local` author/committer remains the one separate
  `--all` gate finding. It is not a Claude/Anthropic attribution and was not changed by this task.

## Why the host-derived identity matters

Git writes `<login>@<hostname>` when `user.email` is unset. Those commits cannot be associated
with the owner's GitHub account and appear as a separate contributor in the repository graph.
Setting the repository-local identity to the owner's GitHub noreply address, as this repository
does, prevents that.
- Refreshed after owner commit `30399e60770fa708c64f29e14d17e3b17669a04d`: all 9 fetched GitHub
  branch heads, 0 remote tags, and **2,388 unique reachable commits** were parsed. The scan found
  zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `24e1ff658d181362ed9bc02fe0137d784190d0fefbd5b82632c9d6e6e637fdc5`; see E773 and the [latest
  checkpoint](checkpoints/2026-10-27-app01-shutdown-term-escalation.md).
- Refreshed after owner commit `46a759003caba59b68a1fd421c07a401272c97fe`: all 9 fetched GitHub
  branch heads, 0 remote tags, and **2,389 unique reachable commits** were parsed. The scan found
  zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `5c18b1e7c6a24c8d2e2adbf9a449414093a86ad7a44a1e84f0dddb4b4de2dff7`; see E776 and [the latest
  checkpoint](checkpoints/2026-10-27-app01-shutdown-term-escalation.md).
- Refreshed after owner commit `91c23ecf7dc252769fb51364741631b710204beb`: all 9 fetched GitHub
  branch heads, 0 remote tags, and **2,390 unique reachable commits** were parsed. The scan found
  zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `bcaf081813afcf053c0789b8f7f151e14fc59e2fcebc8bdee0c5b93b85400fd2`; see E779 and [the latest
  checkpoint](checkpoints/2026-10-27-app01-shutdown-term-escalation.md).
- Refreshed after owner commit `ba6184529e921dfb3aafb01bb4a9520d6ff0238d`: all 9 fetched GitHub
  branch heads, 0 remote tags, and **2,391 unique reachable commits** were parsed. The scan found
  zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `3e6e679f906c75c0fa30c00040e724807176e6999ce72684029870b3ddea0561`; see E780 and [the latest
  checkpoint](checkpoints/2026-10-27-app01-shutdown-term-escalation.md).
- Refreshed after owner commit `98b3cc10d12d742d7456181ab97dfe3ea2696e79`: all 9 fetched GitHub
  branch heads, 0 remote tags, and **2,393 unique reachable commits** were parsed. The scan found
  zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `c780eebbf53e86c543d45a3b266fd3333ea22a6d088b5bd63ba876c1a54059d0`; see E782 and [the latest
  checkpoint](checkpoints/2026-10-27-app01-shutdown-term-escalation.md).
- Refreshed after owner commit `109c9f9f56bdfcf986a9ebd36e1f0aa7066c2fd2`: all 9 fetched GitHub
  branch heads, 0 remote tags, and **2,395 unique reachable commits** were parsed. The scan found
  zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `424eb281d5207c823f37b8191858424b1b96c466fad1a83121a5533372654e3d`; see E784 and [the latest
  checkpoint](checkpoints/2026-10-27-post-109c9f9-attribution-scan.md).
- Refreshed after owner checkpoint `6fcef564c8732c644ec2e4dac903596b8abfe38c`: all 9 fetched
  GitHub branch heads, 0 remote tags, and **2,396 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic identity or credit-line matches. Private report SHA-256
  `bb1a1a19850a051e71f0149801d03a810ba566c8eddd0ad38899a1887ba45f76`; see E785 and [the latest
  checkpoint](checkpoints/2026-10-27-post-6fcef564-attribution-scan.md).
- The subsequent owner closeout commit `8aa90e39d678a32cbaa49a880cfdf83d5bddeb39` passed the
  post-commit attribution audit for `6fcef564..HEAD`, and fetch verified `HEAD == origin/main`.
  Thus the E785 all-ref scan covers the public history through `6fcef564`; the new closeout commit
  is separately checked and contains no assistant attribution. The all-history audit still has
  only the unrelated historical `orchestrator@local` identity. See E786.
- Refreshed after owner checkpoint `e9be553af6f0136c69701501dc38d535fbe46bbe`: all 9 fetched
  GitHub branch heads, 0 remote tags, and **2,400 unique reachable commits** were parsed. The scan
  found zero Claude/Anthropic identity or credit-line matches. Report SHA-256
  `2844551c2cd9b7f5061a20a66672b31810bcc6c116f0d5d7494297ac74fda48c`; see E789 and the
  [music/BGM path census checkpoint](checkpoints/2026-10-27-aud01-music-bgm-census.md). The 70
  stash-only matches remain untouched by the user's explicit choice. E789 was subsequently
  range/identity/path-audited and included in the next fetched-ref scan below.
- After publishing E789 `9ff7cf7b8d7a70569e27306f35b603ca064643bb`, the scanner parsed all 9 fetched
  GitHub branch heads, 0 tags, and **2,401 unique reachable commits**. It found zero Claude/
  Anthropic identity or credit-line matches. Report SHA-256
  `3de89259b7df6516eeb8ef66881df3a98024cccc3d5cf0eb9f7b21354e0b1c00`; see E790 and the
  [publication closeout checkpoint](checkpoints/2026-10-27-e789-publication-closeout.md). The 70
  stash-only matches remain untouched per the user's choice. E789 itself passed a separate exact
  range/identity/path audit after publication.
- After publishing E791 `e7921208b541d03a517a8fbb7ade38c98ae77eda`, the scanner parsed all 9 fetched
  GitHub branch heads, 0 tags, and **2,403 unique reachable commits**. It found zero Claude/
  Anthropic identity or credit-line matches. Report SHA-256
  `b87445237e6364b2a6baaab72483c1d6c5de94170a6641f50c95af02ed4173ae`; see E792 and the
  [E791 publication closeout](checkpoints/2026-10-27-e791-publication-closeout.md). The 70
  stash-only matches remain untouched per the user's choice. E791 itself passed a separate exact
  range/identity/path audit after publication.
- The E793 local-ref census extended the scan to every named ref except `refs/stash`, including
  local/remote branches and Codex turn-diff refs: 25 refs and **2,404 unique commits**, with zero
  Claude/Anthropic identities or assistant credit/signature matches. It separately found the
  already-known 70 stash matches in 768 reachable stash commits and left the ref unchanged by user
  direction. Private report SHA-256 `2d2c80f19d83e8bc4d5c1032fbfefed2501599396db1bb5d18480db20b56c5ef`;
  see the [E793 local-ref census](checkpoints/2026-10-27-local-ref-attribution-census.md).
- After E793 `3797ef7ea3e69439214c7c933ba3a63b55c3abef` was pushed, a fresh scan covered 9 fetched
  GitHub heads, 0 tags, and **2,405 unique commits**, with zero Claude/Anthropic identity or
  credit-line matches. The full local named-ref scan also covered 25 non-stash refs / 2,405
  commits, with zero matches; the 70 stash matches remain unchanged. Remote report SHA-256
  `084a8baa01de2c454d00510c1a4613eef88e410ae5cc1c3c96c893fe55fa15df`; local report SHA-256
  `2d3d6b6f4f91da048efa66c9c545ce6950836786fc52e95304182359ca651449`; see E794 and its
  [publication closeout](checkpoints/2026-10-27-local-ref-attribution-push-closeout.md).
- After E794 `e4e4d20d4f5eb78bd589940a026f3997ea3934e5` was pushed, a fresh scan covered 9 fetched
  GitHub heads, 0 tags, and **2,406 unique commits**, with zero Claude/Anthropic identity or
  credit-line matches. The local scan covered 25 named non-stash refs / 2,406 commits, also zero;
  the 70 stash matches remain unchanged. Remote report SHA-256
  `d32c88e68e0cc06dedcdf2f72f328f56f64160c752e1c929d70b04a35428674d`; local report SHA-256
  `599048a166fc879a59b81b2f137963ce1eb89991fda0dff246545befe5ccf158`; see E795 and its
  [closeout checkpoint](checkpoints/2026-10-27-e794-final-push-closeout.md).
- After E795 `5325daaa1eb5c98f76c0180afe57158f3a1db4cf`, a fresh scan covered 9 fetched GitHub
  heads, 0 tags, and **2,407 unique reachable commits**, with zero Claude/Anthropic identity or
  credit-line matches. The local scan covered 25 named non-stash refs / 2,407 commits, also zero;
  the 70 stash matches remain unchanged by user choice. Remote report SHA-256
  `0aa7b27d0d85524ac30a69b60b0633ca034e7f9507aa285a8476b1f02925dc6a`; local report SHA-256
  `0ccde4532f8ef1649c8f9e64bf58f1c38edca1dfddf8b09bf1a3a9e7a651e7da`; see E796 and its
  [closeout checkpoint](checkpoints/2026-10-27-e795-final-publication-closeout.md). Scans cover
  named refs and fetched heads, not unreachable Git objects.
- After E796 `896a920ebaeec86f11ecde984b3959dea8fcf64e` was pushed, the fetched-ref scan covered 9
  GitHub heads, 0 tags, and **2,408 reachable commits**, with zero Claude/Anthropic identities or
  credit lines. The local scan covered 25 named non-stash refs / 2,408 commits, also zero; the 70
  stash matches remain unchanged by user choice. Remote report SHA-256
  `23940c2ac2b55614f7de4f59f9b01c4d34490acef2f261a42130a33f7af4ed4d`; local report SHA-256
  `72965f534828888f396704a91df413cce2d1591c7616143713e070f60e936d84`; see E797 and its
  [closeout checkpoint](checkpoints/2026-10-27-aud01-seven-archive-sound-path-claim.md). The scan
  covers fetched GitHub heads and named local refs, not unreachable Git objects.
- After E797 `931e0394a47c03fdf4268727b30975f4431bf728`, the fetched-ref scan covered 9 GitHub
  heads, 0 tags, and **2,409 reachable commits**, with zero Claude/Anthropic identity or credit-line
  matches. The local scan covered 25 named non-stash refs / 2,409 commits, also zero. The 70
  stash-only matches remain unchanged by user choice. Remote report SHA-256
  `0bd1a8114a7f300a94c2989e5624edbbb18f493ab87ff1256c53151583cbb1b5`; local report SHA-256
  `316c5b290a5ce95d00331d6203e111373ea8b9f7fe5129e97154df41fe3f5902`; see E798 and its
  [checkpoint](checkpoints/2026-10-27-aud01-seven-archive-sound-path-complete.md). These scans
  cover fetched heads and named local refs, not unreachable Git objects.
- After E798 `f52b09fee8cc39b592d141bc8615f0aa171c6442` was pushed, a fetched-ref scan covered 9
  GitHub heads, 0 tags, and **2,410 reachable commits**, with zero Claude/Anthropic identity or
  credit-line matches. The local scan covered 25 named non-stash refs / 2,410 commits, also zero;
  the 70 stash matches remain unchanged by user choice. Remote report SHA-256
  `62fcd7a56118888fef756d7d806654856046d2295df13b25c08609b7b08714dd`; local report SHA-256
  `2d9ffc0674397dec55bad848f6a08eeb8460ec433cc3362b6b5e70d4a690c1a8`; see E799 and its
  [closeout checkpoint](checkpoints/2026-10-27-e798-publication-closeout.md). These scans cover
  fetched heads and named local refs, not unreachable Git objects.
- After E799 `e86337375bbf5584bdc1bef05d655967089367d4` was pushed, the fetched-ref scan covered 9
  GitHub heads, 0 tags, and **2,411 reachable commits**, with zero Claude/Anthropic identity or
  credit-line matches. The local scan covered 25 named non-stash refs / 2,411 commits, also zero;
  the 70 stash matches remain unchanged by user choice. Remote report SHA-256
  `3a10d59ff78e9fdb3416a1b3035b193090d53bdfafd21d3455cde440e25c47fd`; local report SHA-256
  `074fd12d4732da3012bd54dcb7d7719108d1fb216203ba926b5ada645968976e`; see E800 and its
  [closeout checkpoint](checkpoints/2026-10-27-e799-publication-closeout.md). These scans cover
  fetched heads and named local refs, not unreachable Git objects.
- After E800 `c1536e18d2ca21fde4fe3566a2bbd6c435519e7f` was pushed, the fetched-ref scan covered 9
  GitHub heads, 0 tags, and **2,412 reachable commits**, with zero Claude/Anthropic identity or
  credit-line matches. The local scan covered 25 named non-stash refs / 2,412 commits, also zero;
  the 70 stash matches remain unchanged by user choice. Remote report SHA-256
  `66ec442fcc6fee665421cb1ded67eb9e5acde5daf599512bf1727b8dc47d0ec7`; local report SHA-256
  `4e23b5ea630bce6473e139e992110af4f021be6cc9820d3dda01eb59032629be`; see E801 and its
  [closeout checkpoint](checkpoints/2026-10-27-e800-publication-closeout.md). These scans cover
  fetched heads and named local refs, not unreachable Git objects.
- Expanded after E802 `29329cad4433cb8cf7fdc414b68e0adc14ded7e4`: `git ls-remote --refs origin`
  advertised 21 refs (9 branch heads, 12 pull-request heads, 0 tags). Their combined ancestry
  comprised **2,414 unique commits** with zero Claude/Anthropic identities or credit lines. The
  all-advertised-ref report SHA-256 is
  `315e397624ed2585f923c9984290867422848924020260f96c961b36e8bc953e`. The named local scan
  covered 25 non-stash refs / 2,414 commits, zero matches (SHA-256
  `ba669248c243f7fe82676327fad3b531beec75d370f9cc89e41f38b8ad07435a`). Current stash ancestry
  remains 768 commits / 70 matches; a read-only union of its 21 reflog tips reaches 826 commits
  with the same 70 matches, leaving 58 additional reflog-only commits clear. The user directed that
  all private stash state remain unchanged. See E803 and its
  [closeout checkpoint](checkpoints/2026-10-27-e802-attribution-census-closeout.md). Unreachable
  Git objects and GitHub contributor-graph rendering are not covered.
- After E803 `13ce186fd4762f66ec8f6715e38b67b86a673411` was pushed, a new scan of all advertised
  GitHub refs covered 9 branch heads, 12 pull-request heads, 0 tags, and **2,415 unique commits**,
  with zero Claude/Anthropic matches (SHA-256
  `3ed3c86360093f97f82fc9d9f60d504326fa45c337603e159682e2c309486579`). The local census covered
  25 non-stash refs / 2,415 commits, also zero (SHA-256
  `d62aaff0b55aecc32d4fa46dfdab7354d647f067eb83f59a91e2e8c5fe73968f`). The 21 current stash
  reflog tips reach 826 commits / 70 message-credit matches, 0 identity hits; stash state remains
  unchanged. Independent review reproduced both scans. See E804 and the
  [closeout checkpoint](checkpoints/2026-10-27-e802-attribution-census-closeout.md). Unreachable
  objects and contributor-graph rendering are outside this evidence.
- After E804 `421a096803f78b8a2e4ba21eb72d85cadf578494` was pushed, the all-advertised-ref scan
  covered 21 GitHub refs (9 branches, 12 pull-request heads, 0 tags), **2,416 unique commits**, and
  zero Claude/Anthropic identities or credit lines (SHA-256
  `6be3e8f00b577d7b0b5c6b79685ebe6dff5aafb790d4d829dbb16e61337ab73d`). The local scan covered 25
  non-stash refs / 2,416 commits, also zero (SHA-256
  `36c52992625baf4a0bede24fa1d252dcefeb89a2ef5df496c02976a82544698a`). Stash remained 768/70;
  its 21-tip reflog union was 826/70, all message-credit matches with zero identity hits, and was
  left unchanged. Independent review reproduced both reports. See E805 and the
  [closeout checkpoint](checkpoints/2026-10-27-e804-publication-closeout.md). Unreachable objects
  and contributor-graph rendering are outside this evidence.
- After E805 `e3821edda7685e24899020a28d225c398644e711` was pushed, the all-advertised-ref scan
  covered 21 GitHub refs (9 branches, 12 pull-request heads, 0 tags), **2,417 unique commits**, with
  zero Claude/Anthropic identity or credit-line matches (SHA-256
  `cddda7c867b82e1530f498dc814eee50944f23c6ff85e6716215ee1edc30bf37`). Local scan covered 25
  non-stash refs / 2,417 commits, also zero (SHA-256
  `9b386897a1a65b0ec45e4a4c6974e59320d9880257e8018150ddf9a0b3121c86`). Stash remains 768/70;
  its current 21-tip reflog union remains 826/70, unchanged. See E806 and the
  [closeout checkpoint](checkpoints/2026-10-27-e804-publication-closeout.md). Unreachable Git
  objects and contributor-graph rendering are not covered.
- After E806 `70ba991e2093211d68ac4ca70134535dca8e75d4` was pushed, the all-advertised-ref scan
  covered 21 GitHub refs (9 branches, 12 pull-request heads, 0 tags), **2,418 unique commits**,
  and zero Claude/Anthropic matches (SHA-256
  `14edf72ac7a2f83d5d683271ac1475b6a9d7bd8556831587464e351b6cdb9f83`). Local scan covered 25
  non-stash refs / 2,418 commits, also zero (SHA-256
  `021e1210ca0af54826dad23d0bf16a9b8c0d7a4a88c89aa72685bf073f5f8fba`). Stash remains 768/70;
  its current 21-tip reflog union remains 826/70, unchanged per user choice. Independent review
  reproduced both reports. See E807 and the
  [closeout checkpoint](checkpoints/2026-10-27-e804-publication-closeout.md). Unreachable Git
  objects and contributor-graph rendering are not covered.
- After E807 `c25e48abb06ce645ea04f193a1af2af1686fd79a` was pushed, the live advertised-ref census
  covered 21 GitHub refs (9 branches, 12 pull-request heads, 0 tags), **2,419 unique commits**,
  and zero Claude/Anthropic matches (summary report SHA-256
  `7d3ba7e139b929316aa489002403561d14c799d9b167228077a61e7fe411af80`). Local scan covered 25
  non-stash refs / 2,419 commits, also zero (summary report SHA-256
  `402b13304691bf1fd952bd8cc214188be4d5e71752328bb285840c6c10cf3e7a`). The scan reports contain
  summary counters; independent review separately verified the live ref listing. Stash remains
  768/70 and its 21-tip reflog union 826/70, unchanged per user choice. See E808 and the
  [closeout checkpoint](checkpoints/2026-10-27-e804-publication-closeout.md). Unreachable Git
  objects and contributor-graph rendering are not covered.
- After E808 `968717d483bdb4b412b08751c7525d042f42c8ec` was pushed, the live advertised-ref scan
  covered 21 GitHub refs (9 branches, 12 pull-request heads, 0 tags), **2,420 unique commits**,
  and zero Claude/Anthropic matches (scan log SHA-256
  `cb58878d86ea33afe421f8d4088e6a4023015344e2527151bb744312811c2ddf`). Local scan covered 25
  non-stash refs / 2,420 commits, also zero (summary report SHA-256
  `308d3b250e8ee402004d4ac67e9d51b003782c5a494e32b342eba6969d0091ea`). Independent review
  reproduced the counts and match classes. Stash remains 768/70; the 21-tip reflog union remains
  826/70, unchanged by user choice. See E809 and the
  [closeout checkpoint](checkpoints/2026-10-27-e804-publication-closeout.md). Unreachable Git
  objects and contributor-graph rendering are not covered.
- After E833 commit `9869c4c667e4c56975fa6bc2163c862ca475bb3e` was pushed, a live scan of all
  advertised GitHub branch and PR refs covered 9 branches, 12 PR heads, 0 tags, 15 unique tips,
  and **2,446 reachable commits**. It found zero Claude/Anthropic author or committer identities
  and zero coauthor/generated-by credit lines; remote `main` matched the fetched local tip. The
  owner-selected private stash boundary remains unchanged at `refs/stash` `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`.
  The captured audit log SHA-256 is `bd4a485f3f07564091c763f8d8aca293f62395eae005941f720aea96965f5959`.
  See E834 and the [publication closeout](checkpoints/2026-10-27-e833-publication-closeout-e834.md).
- After E834 commit `5ca52a7f6389b9eae7d927afb0ba5aa064b79a42` was pushed, the live advertised-ref
  scan covered 9 branch refs, 12 PR heads, 0 tags, 15 unique tips, and **2,447 reachable commits**.
  It found zero Claude/Anthropic author or committer identities and zero coauthor/generated-by
  credit lines; live remote `main` matched local. `refs/stash` remains at the owner-selected
  unchanged tip. The scan log SHA-256 is
  `efbc7a4ad369203f55f30b38fea49d42a98d13fd83c16c978a647ab67f065ec8`; see E835 and the
  [publication closeout](checkpoints/2026-10-27-e834-publication-closeout-e835.md).
