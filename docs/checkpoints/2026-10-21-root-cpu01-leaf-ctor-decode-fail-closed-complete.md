# Checkpoint: CPU-01-LEAF-CTOR-DECODER-FAIL-CLOSED-FIXTURE-01 — complete

- Local date/time: 2026-10-21 12:29 CDT.
- Agent ID: `/root` (integrator), with implementation by `/root/review_host_proto`.
- Task ID: `CPU-01-LEAF-CTOR-DECODER-FAIL-CLOSED-FIXTURE-01`.
- Status: `FIXTURE_VERIFIED`; failed tries: 0.
- Files changed for implementation: only the new `tools/lift/test_leaf_ctor_decode.py`.
  Production decoder is unchanged.
- Verification commands and results:
  - Worker `PYTHONDONTWRITEBYTECODE=1 python3 -I tools/lift/test_leaf_ctor_decode.py` — exit 0;
    1 unittest passed. Log `local/validation/cpu01-leaf-ctor-decode-fail-closed/attempt-1.log`,
    SHA256 `fb184ea57734512b1f088f9181209901b9cd46df3eb7eaa2500ea9e48dde20b5`.
  - Root replay `PYTHONDONTWRITEBYTECODE=1 python3 -I tools/lift/test_leaf_ctor_decode.py` — exit 0;
    1 unittest passed. Log `local/validation/cpu01-leaf-ctor-decode-fail-closed/root-replay.log`,
    SHA256 `fb184ea57734512b1f088f9181209901b9cd46df3eb7eaa2500ea9e48dde20b5`.
  - `git diff --check` — exit 0; the new untracked file was checked against `/dev/null` with
    `git diff --no-index --check` and had no whitespace diagnostics.
  - Test source SHA256: `477e29614c6c71e7d7b9bbe16e61e423078382131b3f2eb3292fea4fc4025d37`.
  - Unchanged decoder SHA256: `82b74b8278e0859882f6cd6a8fdcbe14e1d2892f86cd1fb0ff4df1639c6d48cc`.
- Observation: the synthetic image returns 16 NOP bytes at RVA `0x4000`; decode raises the
  required `DecodeError` prefix `unrecognised encoding at 0x4000:`.
- Inference limit: one unsupported-opcode case only; no general decoder correctness or game
  behavior is established.
- Remaining risks: the native executable remains unimplemented and all gameplay gates remain open.
- Next exact action: publish this fixture with the launcher-tone and region-split completions.
