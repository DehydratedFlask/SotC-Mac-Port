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
- Claim publication: `python3 scripts/project_index.py` exits 0 with 24,078 folders indexed (log SHA-256 `a79b0d25fced0d9c18dda3386413644761318dd915da578a4bbe36f239469b0c`) and `python3 scripts/project_index.py --check` exits 0/current at 24,078 (log SHA-256 `2e24f86275b84b7efc28ec198e984a6698bc3584a72c960f1b280bdaa45c1b67`). `git diff --cached --check` exits 0 with empty output (SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).
- Staged `python3 -I scripts/audit_publish_paths.py` accepts 4 paths (log SHA-256 `2e1abe5c5f3ea8c7ba5ff5a9ee90bf6a551fe710bd20aa7ef711333e7d5f56d7`); full `python3 -I scripts/audit_publish_paths.py --all` accepts 1,693 paths (log SHA-256 `3b51c7a732f7c68621520eda5fc5b8768d5acf53226055f808e6cd4acc1b3062`).
- Claim commit `8020974b0d77aca07143026ab717a2c44c9f9740` (`docs: claim unwrap idempotence fixture`) and `git push origin main` both exit 0. Pre-push and post-push fetch exit 0 and divergence remains `0 0`. Commit/push log SHA-256 are `0e36d467008852cc82b5892f625710af459dbe9c5223b75966ab90187c1d5d99` / `b67781d2193808fcf46eb860227f69c442ed5691dfe9b0b654263b79d83ec25c`; pre/post fetch SHA-256 `58e4bb2ebd9bd713bf3332d4cad1251aeb0b51fb26a8b06f30b3aaa431df79b3`; pre/post divergence SHA-256 `4b14c7b5549e560a68e9c4daaae3fdb8a28fe32d0247050325e7c5a3b2b3271c`; final status SHA-256 `a144c9090dcdd8b43e16e12783ea2e5ba135dee09a85a74793a7b3e9c58e8764`. Logs are under `local/validation/20261022-root-fixture-baselines/`.
- After the claim was pushed, the one-test implementation was assigned to `/root/next_bounded_scout`. Root remains the sole integrator.
- Observation: `unwrap()` allows a matching existing ELF and writes the same reconstructed bytes again. The sidecar record is derived from stable source/destination paths and content for this fixture.
- Inference/task boundary: add a test that makes the same synthetic `unwrap()` call twice and asserts the second returned record, ELF output, and sidecar bytes match the first call. This pins wrapper repeatability only; it proves no real SELF behavior, executable loading, or code execution.
- Remaining risks: no caller concurrency, crash recovery, real package, loader, runtime, or game behavior is covered. Production helper changes are out of scope.
- Implementation and root replay are complete; detailed results are in [completion checkpoint](2026-10-22-root-re01-unwrap-repeat-complete.md). The next independent action is to review `/root/next_independent_task_scout`'s read-only candidate and claim it only after live verification.
