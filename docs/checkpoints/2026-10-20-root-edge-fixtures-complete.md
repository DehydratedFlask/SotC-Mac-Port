# Three edge-case fixtures — 2026-10-20, `/root` (workers via Agent subagents; root re-ran every suite)

| Task | Result | Files | Evidence |
|---|---|---|---|
| AUD-PUBLISH-EDGE-01 | **Defect fixed**: `violations()` accepted `allowlisted/../forbidden` paths (first-component check only). Now rejects any `..` component as `path traversal`. New test fails (5 subtests) without the fix; 4 tests pass with it; `--all` audit exits 0. | `scripts/audit_publish_paths.py` `a9a40ad0…3b24`, `scripts/test_audit_publish_paths.py` `7fe706e0…b3ba` | log `local/checkpoints/aud-publish-edge-test.log` `e6ec1827…c1` |
| DATA-02-PSARC-EDGE-01 | **Defect fixed**: zero-entry archive made `Psarc.paths()` raise bare `IndexError`; `_load` now raises `PsarcError('archive has no manifest entry')`. 14→15 tests pass; new test failed with IndexError before fix. | `scripts/psarc.py` `5f44b206…a771`, `scripts/test_psarc.py` `ca67ee3e…4669` | log `local/checkpoints/data02-psarc-edge-test.log` `edecde55…0cc0` |
| APP-01-ASSET-VALIDATOR-EDGE-01 | Fixture only (no defect): required archive name present as a directory is reported `not found`. `swift test` 18→19 tests pass (root re-ran: 19 tests/3 suites). | `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift` `77ee2181…feab` | log `local/checkpoints/app01-asset-validator-edge-test.log` `6c3702c9…f65c` |

Known remaining (not fixed): a forbidden dir nested under an allowlisted top dir (e.g. `docs/Shadow of the Colossus/x`) passes the audit; a corrupt zlib block raises `zlib.error` rather than `PsarcError`. Synthetic inputs only; no live-game claim. G0 PARTIAL, G1-G6 NOT RUN. `.build/` added to `.gitignore`.
