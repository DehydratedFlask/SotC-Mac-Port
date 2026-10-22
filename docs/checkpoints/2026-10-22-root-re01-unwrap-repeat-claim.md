# Checkpoint: RE-01 identical-output SELF unwrap repeat fixture claim

- Local date/time: 2026-10-22 00:21 CDT (America/Chicago)
- Integrator / agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
- Task ID: `RE-01-UNWRAP-IDENTICAL-ELF-REPEAT-FIXTURE-01`
- Parent: `RE-01-UNWRAP-IDENTITY-SIDECAR-ROUNDTRIP-FIXTURE-01`
- Status: `CLAIMED`; 0 implementation attempts and 0 failed tries.
- Files changed for claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and generated `docs/FOLDERS.md`. Test and production source are unchanged.

## Candidate and evidence

- Exact Graft command: `trail ask "existing unwrap tests for identical already-written ELF destination and identity sidecar" --source --in scripts/` (exit 0). It surfaced `scripts/unwrap_self.py:10-31` and `scripts/test_project_tools.py:68-95` as source nodes. The query was refreshed against live files; the live sources below were read afterward.
- Live specification: `docs/EXTRACTION.md:105` says the wrapper refuses to overwrite an existing different ELF. It does not specify behavior for a matching output.
- Live implementation: `scripts/unwrap_self.py:10-31` parses the plaintext SELF, refuses a different existing destination, writes the reconstructed ELF, emits the identity sidecar, and returns a record. Same-path repeated calls are not rejected by these guards.
- Existing tests: `scripts/test_project_tools.py::test_plaintext_self_roundtrip_and_identity` covers first-write success and a later invalid-source call; `test_refuse_overwrite_of_other_artifact` covers rejecting different destination bytes; the identity-collision test covers a sidecar path aliasing the input. No existing task row covers a second identical successful call.
- Baseline command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`: exit 0, 10 tests. Log `local/validation/20261022-root-fixture-baselines/re01-unwrap-repeat-baseline.log`, SHA-256 `811eabda5dc298da232b3c7953fbe433089645edff6f53063d91016c4136c0a2`.
- Baseline test hash: `scripts/test_project_tools.py` SHA-256 `dcf161b6994f01ac92b6abbf261298456017dd7bdfcc0daa5d26fb298aee5280`.
- Current helper hash: `scripts/unwrap_self.py` SHA-256 `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`.
- Pinned reference checkout `reference/ShadowOfTheColossus-PC` is clean at commit `70bb43859cd7ff181a6d409540b757866184d60d` (`Release 0.57: AMD 32-lane path, launcher switch for direct memory access`). It remains untouched.
- Observation: `unwrap()` allows a matching existing ELF and writes the same reconstructed bytes again. The sidecar record is derived from stable source/destination paths and content for this fixture.
- Inference/task boundary: add a test that makes the same synthetic `unwrap()` call twice and asserts the second returned record, ELF output, and sidecar bytes match the first call. This pins wrapper repeatability only; it proves no real SELF behavior, executable loading, or code execution.
- Remaining risks: no caller concurrency, crash recovery, real package, loader, runtime, or game behavior is covered. Production helper changes are out of scope.
- Exact next action: after this bounded claim is committed and pushed, assign one test-only method to `/root/next_bounded_scout`; root will replay `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`, record E256, refresh project/Trail indexes, update HANDOFF, and publish completion.
