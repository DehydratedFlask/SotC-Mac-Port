# AUD-PUBLISH-NESTED-01 and DATA-02-PSARC-ZLIB-CORRUPT-01 — 2026-10-20, `/root`

Both `FIXTURE_VERIFIED`; workers via Agent subagents, root re-ran both suites and `audit_publish_paths.py --all` (exit 0).

- **AUD-PUBLISH-NESTED-01**: audit now rejects directory components `Shadow of the Colossus`, `local`, `AgentStores`, `.graft` (case-insensitive) at any depth; nested eboot/sprx/self/elf/pkg/psarc were already caught by the name regex. 4→6 tests pass; nested-rejection test failed on 6 cases before the rule. `scripts/audit_publish_paths.py` `2ea984cf…bba3`, test `57e5dd22…7f44`, log `local/checkpoints/aud-publish-nested-test.log`. `reference/`, `logs/`, `graft/` deliberately not forbidden when nested.
- **DATA-02-PSARC-ZLIB-CORRUPT-01**: `Psarc.read()` wraps `zlib.error` as `PsarcError('block N has corrupt zlib data')`. 15→16 tests pass; test failed with raw `zlib.error` before the fix. `scripts/psarc.py` `fcaf423b…4007`, test `47c23b74…62c0`, log `local/checkpoints/data02-psarc-zlib-corrupt-test.log` (`ad39657e…3119`).

Synthetic inputs only; G0 PARTIAL, G1-G6 NOT RUN; failed tries 0.
