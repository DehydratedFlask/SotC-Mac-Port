# Checkpoint: RE-01-UNWRAP-IDENTITY-SIDECAR-ROUNDTRIP-FIXTURE-01 — claim

- Local date/time: 2026-10-21 12:40 CDT.
- Agent ID: `/root` (integrator); planned implementation owner `/root/review_host_proto`.
- Task ID: `RE-01-UNWRAP-IDENTITY-SIDECAR-ROUNDTRIP-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Planned file: `scripts/test_project_tools.py` only.
- Bounded change: in the existing synthetic plaintext-SELF success case, load
  `output.with_suffix(".identity.json")` and assert its JSON value equals `record` returned by
  `unwrap()`. Leave the unwrap helper unchanged.
- Evidence reviewed: `unwrap_self.py` writes the JSON record to that exact sidecar path and returns
  the same record; the existing test checks output bytes and selected record fields but not a full
  persisted-record round trip. Related truncation and input-collision fixtures are complete.
- Verification command after implementation:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Observation: the synthetic success fixture already provides all values needed to assert equality.
- Inference limit: this covers synthetic plaintext fixture data only; no original SELF, extraction
  corpus, runtime execution, or game behavior is established.
- Remaining risks: real input analysis remains subject to its existing evidence limits; all game
  gates remain open.
- Next exact action: push the claim commit, then ask the assigned worker to add the one sidecar
  equality assertion and run the project-tools suite.
