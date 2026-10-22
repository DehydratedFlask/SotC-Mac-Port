# DATA-02 TOC-value / declared-size crosstab — claim

- Time / timezone: 2026-10-21 22:42 CDT (America/Chicago); owner/integrator `/root`.
- Task: `DATA-02-TOC-VALUE-SIZE-CROSSTAB-01`, parent `DATA-02-ARCHIVE-OVERLAP-MAP-01`.
- Scope: over all 40,216 exact-path archive-pair occurrences from the seven private
  `local/manifests/assets/*.members.jsonl` inputs, count the four combinations of equality for
  opaque TOC values and declared sizes. Report only four counts and reconciliation margins; keep
  member paths and raw TOC values out of generated outputs.
- Baselines: E77 records 4,704 equal-size and 35,512 different-size pair occurrences. E123 records
  4,479 equal opaque-TOC-value and 35,737 different-value occurrences. The four cells must sum to
  40,216 and reconcile to both margins.
- Boundaries: metadata-only. Do not read payload bytes or infer digest/TOC semantics, archive
  overlay precedence, selected-scene resolution, loader behavior, or runtime behavior. No REA/Ghidra
  use. Preserve the seven inputs as private user data.
- Files changed for claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No manifest was read
  and no analyzer was created before the claim.
- Exact next action: after claim publication and fetch verification, assign
  `/root/round78_next_task_scout` a private deterministic analyzer and independent rerun; root will
  review its code/output and reconcile all margins before status promotion.
- Task status: `CLAIMED`; attempt count 0.

## Claim publication checkpoint

- Time / timezone: 2026-10-21 22:47 CDT (America/Chicago); owner/integrator `/root`.
- Claim documentation and the separate `0x25aa06` static verifier completion were published in
  commit `f9fc4302f1702af6677e90121a6040becc533d5e` (`verify unresolved app-main call and claim TOC
  crosstab`). `git push origin main` succeeded; fetch confirmed `HEAD == origin/main` with 0/0
  divergence. Post-push log:
  `local/checkpoints/round78-re02-25aa06-data02-claim-postpush.log`, SHA-256
  `7c434d2e6dff50eecac5ac952fdd14be140e42e041a373377dc9bcc8d2bb485d`.
- Preflight: staged publication audit accepted 8 paths and full-tree audit accepted 1,667 paths;
  both exited 0. `git diff --cached --check` exited 0. Native API user files were not staged. Full
  audit SHA-256 `a24d31532815c136fad3ea12efcb696b95197d87b38e447db5f4d1ec84d393db`.
- Worker `/root/round78_next_task_scout` was assigned only after publication verification. It owns
  private analyzer/report/log files under a task-specific `local/` directory; it must not edit shared
  documentation, publish, read payloads, or use REA/Ghidra. Root remains the shared-document
  integrator.
- Claim status remains `CLAIMED`; attempt count 0 until the bounded analysis begins. Exact next
  action: independently review the worker's deterministic four-cell output and verify both E77/E123
  margins before changing task status.
