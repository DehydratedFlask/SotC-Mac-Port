# Completion: RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01

- Local time: 2026-10-21 11:13 CDT (America/Chicago).
- Agent / integrator: worker `launcher_review`; integrator `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task status / attempt count: FIXTURE_VERIFIED; 0 failed implementation tries.
- Files changed: worker changed only `scripts/test_ps4_dynlink.py`; root updated `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Production `scripts/ps4_dynlink.py` is unchanged.

## Result

- Synthetic fixture writes `fixture.cpp` under a temporary directory with the identifier `synthetic_export_name`. It requests that token's NID plus the NID for an absent token and asserts the result contains only the expected NID/token mapping.
- Observation: `name_by_hash()` performs spelling lookup for identifier-like tokens in selected text files. Inference: the temporary source proves the function can identify a spelling from a NID under this fixture; it does not establish collision resistance or symbol meaning.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Worker result: exit 0, 19 tests passed. Log `local/validation/re02-dynlink-name-by-hash/attempt-1.log`, SHA256 `2156b828ab1e4244256993eddce37def4171ccc6bae53a3ddd3647fbdca6ef8e`.
- Root replay: exit 0, 19 tests passed. Log `local/validation/re02-dynlink-name-by-hash/root-replay.log`, SHA256 `ee3fea9cad4104c8e872ddbef1ca170f599bab5c14727c4e734b3343aafb0298`.
- Test SHA256 `aef6a26b2ec7c40184ba0b6755522c6c94ed7a8907f347a4f34235fe19324a05`; production source SHA256 `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9` (unchanged). Worker and root `git diff --check` passed.
- Source graph: `trail build --check` exited 0 and reports the wiring graph in sync with 3,528 nodes. The deep meaning layer remains unbuilt.

## Limits and checkpoint

- This is synthetic spelling lookup only. It does not validate real source trees, NID collisions, imports, module/version/signature metadata, an executable loader, or game behavior.
- No code attempt failed. No production implementation changed. No game data was read.
- Remaining risk: the truncated NID is only a strong spelling hint as documented by the source; this fixture says nothing about whether a symbol's semantics or ABI match.
- Exact publication command/result: `git push origin main` exited 0 and advanced
  `3a9c553..cfebea9`. Commit `cfebea9848583ed1df9124a869e99379e65f988a` is on `origin/main`, and
  root verified HEAD equals `origin/main`.
- Publication checks: staged audit accepted six allowlisted files; full tracked-path audit accepted
  1,514 files; `git diff --cached --check` exited 0. `python3 scripts/project_index.py` indexed
  14,219 folders, and `python3 scripts/project_index.py --check` reports current. `trail build
  --check` reports the graph in sync at 3,528 nodes (deep meaning layer remains unbuilt). Command
  logs are in `local/checkpoints/root-round12-dynlink-claim/`.
- Next exact action: select and claim another independent bounded task while preserving the escalated
  and blocked work boundaries.
