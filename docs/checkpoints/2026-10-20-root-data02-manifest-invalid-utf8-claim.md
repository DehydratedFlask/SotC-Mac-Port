# Checkpoint: DATA-02-ARCHIVE-MANIFEST-INVALID-UTF8-01 — claim

- Time / timezone: 2026-10-20 05:17 CDT
- Agent and integrator: `/root`; root owns shared docs and Git
- Task / gate / status: `DATA-02-ARCHIVE-MANIFEST-INVALID-UTF8-01`; claimed, `ACTIVE`; no game gate implication
- Prerequisite evidence / input and source hashes: parent `DATA-02-ARCHIVE-OVERLAP-MAP-01` is `ARTIFACT_VERIFIED`; parser SHA-256 `28330049faf0c7366159e7439ce80b8f4354413908c98bd3d7c95c6addbc7bf7`; test SHA-256 `6f13a329f1572716107639691b36f9f488ab66b665e59ed37e4074f8377110a1`
- Owned / changed files: worker owns only `scripts/test_archive_overlap_map.py`; root owns `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint
- Hypothesis and expected result: `parse_manifest_bytes("synthetic", b"\xff")` raises `ManifestError` with `manifest is not UTF-8`; no parser change expected
- Exact command, CWD and nonsecret environment: from `/Volumes/Extra/SoC Mac Native`, `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`
- Exit / observed result: baseline exit 0; 11 existing tests pass
- Log, capture and REA evidence paths/IDs: `local/checkpoints/root-2026-10-20-manifest-invalid-utf8-claim-baseline.log`, SHA-256 `125933e1e7e2e18e36f6fded21c56b4de02add30203872c9228d401855150e90`
- Observation versus inference / coverage limitations: Graft and live source show `UnicodeDecodeError` is caught and reported as `ManifestError`; existing tests do not exercise invalid UTF-8. Any test pass establishes only synthetic manifest-byte handling, not live archives or game behavior
- Cumulative failed tries and distinct hypotheses: 0; baseline is not an implementation try
- Remaining risks / blockers: archive member semantics and overlay/runtime use remain open; G1 NOT PASSED; G2 NOT RUN
- Publication checks: `python3 -I scripts/audit_publish_paths.py` exited 0 for 4 staged paths; log `local/checkpoints/root-2026-10-20-manifest-invalid-utf8-claim-publish-audit-staged.log`, SHA-256 `68a38548f7309cae385cfc0f9f886da8f91f1cfca31112350e1942a7a9ada8a9`; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 637 tracked paths; log `local/checkpoints/root-2026-10-20-manifest-invalid-utf8-claim-publish-audit-all.log`, SHA-256 `33c82ebbb734b4bd2b852d06590f3dac5391afb8b026eb032fc5f536c79c57d3`; `git fetch origin` showed 0/0 divergence
- Next exact action: commit and push this claim, then dispatch the worker
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF and TASKS updated; no EVIDENCE or INSTALL_LOG fact changed; `python3 scripts/project_index.py` exited 0 at 4,166 folders (`local/checkpoints/root-2026-10-20-manifest-invalid-utf8-claim-project-index.log`, SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`); generated `docs/FOLDERS.md` lists 79 checkpoint files; `graft check` exited 0 at 1,257 nodes / 530 source files, optional deep meaning layer unbuilt (`local/checkpoints/root-2026-10-20-manifest-invalid-utf8-claim-graft-check.log`, SHA-256 `618ddc43b260a735660b1be6e0de2fa68cbdc86bfe54031aaaae53b1d09287ab`); `git diff --check` passed
