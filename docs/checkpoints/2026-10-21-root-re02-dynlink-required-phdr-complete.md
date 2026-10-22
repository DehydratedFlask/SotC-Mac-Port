# RE-02 dynlink required program headers — completion checkpoint

- Time / timezone: 2026-10-21 23:01 CDT (America/Chicago); owner/integrator `/root`.
- Task: `RE-02-DYNLINK-REQUIRED-PHDR-01`, child of `RE-02-DYNLINK-01`.
- Files changed by worker: `scripts/ps4_dynlink.py`, `scripts/test_ps4_dynlink.py`. Parser now raises
  `ValueError('missing PT_DYNAMIC program header')` or
  `ValueError('missing PT_SCE_DYNLIBDATA program header')` instead of leaking `StopIteration`.
  Tests mutate the two synthetic program-header type fields and assert the exact messages.
- Final SHA256: parser `c0fcf34b5b9a4ad626a5b88cd2a100be37312e8038c26fb230337bc98bf5ba95`; tests
  `6681021f7cb4d66d2b23411d43c84e884309dca4898b3ccc7459ecb3bda97ece`.
- Exact suite command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`;
  worker and root exit 0 with 29 tests. Worker stderr log
  `local/validation/re02-dynlink-required-phdr-01/worker.stderr` SHA256
  `7f6d674dc38c83e5df2985921ab33658727cdf69955265a530601defdf4dcbeb`; root stderr log
  `root-replay.stderr` SHA256
  `71846c3ad14dcb97da46cdc1a06524a497a103b3d2af5a38324a314bbcf91e66`; both stdout logs are
  empty. Baseline had 27 tests and passed before the claim.
- Mutant checks: the two guards were independently removed in temporary copies, then the associated
  new test was run. Both commands exited 1 with the original `StopIteration`, as expected. The
  captured error logs are `mutation-pt_dynamic.stderr` SHA256
  `309b022ecee85516cfe4c2693f0a327694704b2449deb57d97e62289986d4972` and
  `mutation-pt_sce_dynlibdata.stderr` SHA256
  `4e21a81dc7bb9bbe2fc8f6af471c707cee9637644473007f5f8c9f57b0603aeb`.
- Duplicate-header check: exact command `python3 -I local/validation/re02-dynlink-required-phdr-01/root_duplicate_header_check.py`, exit 0. The
  synthetic fixture adds a later same-type header with an out-of-range file offset; parsed table
  outputs remain equal to the original fixture for both header kinds, showing first-match behavior.
  Script SHA256 `84beb0c3c57ebb40ee9ff3c0f6ac6fa78dfb0109f687cdb793c2b4a0d2bad733`; stdout SHA256
  `d3c41f2bc1199590eb4c56a85286e17014707b3ce436c6a821e1537b5bce18ba`, stderr empty.
- Failed verification-harness experiment: the first root duplicate check exited 1 because it
  compared the full parser result, including `ph`, which necessarily differs when the fixture adds
  a duplicate header. The corrected comparison excludes only `ph` and passes. Preserved stderr is
  `root-duplicate-header-attempt1.stderr`, SHA256
  `93ef5a5e1e67c00c6fcb00c2fae4248109a50610a060c5956fa8414cd9de286c`; this was not an
  implementation failure and does not count as a failed implementation try.
- Root `git diff --check -- scripts/ps4_dynlink.py scripts/test_ps4_dynlink.py` exited 0.
- Index and source-graph checks after adding this checkpoint: `python3 scripts/project_index.py`
  exit 0, 19,275 folders indexed; `python3 scripts/project_index.py --check` exit 0, current;
  `trail build --check` exit 0, graph in sync with 3,599 nodes (deep semantic layer remains
  unbuilt). Logs: `/tmp/round78-dynlink-project-index.log` SHA256
  `be383a9ca396b168dc2b07fd43c859eebbc366320ef05de31997c0775952b2a2`,
  `/tmp/round78-dynlink-project-index-check.log` SHA256
  `aad69692b772bf23bd5a7ec150587b537c633cb4c270c3b34101e9cbde87a82d`, and
  `/tmp/round78-dynlink-trail-check.log` SHA256
  `4b0aecb55c6f7700bf0861a2fa1def317d581b34ef946c1b50dfe1405c2da70b`. The folder inventory
  now lists 401 checkpoint files.
- Observed: explicit parser errors, all 29 synthetic tests pass, both reversion mutations kill the
  original defect, and duplicate-header first-match behavior passes in the synthetic replay.
  Inference: change hardens offline fixture parser input handling only. No live ELF, dynamic loader,
  runtime, or game behavior is established.
- Remaining risks: no live ELF/runtime-loader validation; intentionally outside this bounded task.
  Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` changes remain unstaged and
  untouched. Attempts: 0 failed implementation tries.
- Task status: `FIXTURE_VERIFIED`.
- Exact next action: delegate a read-only scout for a bounded task outside the PSARC reader files;
  keep negative-limit behavior unclaimed until the user chooses invalid-error versus empty-read
  behavior.
