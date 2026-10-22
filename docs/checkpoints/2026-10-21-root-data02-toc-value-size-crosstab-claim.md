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
