# Round 71 fixture claims

Claim checkpoint time: 2026-10-21 16:27 CDT (America/Chicago). Integrator: `/root` (Codex
desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claim base: commit
`73f9a979fffa1e5f87942a2db547994708714c91` (`origin/main`).

Claim publication update: 2026-10-21 16:28 CDT. Commit
`bb1f58628a85dbea0331e8195d792a49193d02d8` (`docs: claim Round 71 fixture tasks`) is pushed to
`origin/main`; after `git fetch origin`, `HEAD == origin/main` and divergence is `0 0`. The only
remaining worktree changes are the pre-existing user-owned `native/api/CMakeLists.txt` and
untracked `native/api/host/`, both excluded from the commit. The commit and push logs are
`local/validation/root-round71-scout/claim-commit.log` SHA-256
`3576f69429ef94e088c33e2b785b0374865a3116aa823063f1977981fe6f999a` and `claim-push.log`
SHA-256 `786cb3ee024ac313d78e970b7e556f18602707cbf26f172c51848eadc9c9f716`; fetch log SHA-256 is
the empty-output hash `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; post-push
state log SHA-256 is `e700f3d1a45ff2ecd8c7070635e7b561d4cbba773f9add8563317e2e31536ffe`.

Three independent bounded tasks are claimed before implementation. No code or test file has been
edited. `docs/TASKS.md` assigns one test file to each worker; `/root` is the sole integrator for
`HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated indexes, and checkpoints.

## Claims

| Task | Owned file | Hypothesis and acceptance | Status / attempts |
|---|---|---|---|
| `APP-01-SYSTEM-CHECKS-CORE-MISSING-PATH-FIXTURE-01` | `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift` | A unique temporary parent with absent `SoCGameCore` should return exact `.notBuilt` copy: `NOT BUILT: SoCGameCore not found — native recovery incomplete. Play disabled until a native SoCGameCore exists`. This is path classification/copy only. | ACTIVE; 0 failed tries |
| `RE-02-DYNLINK-MULTI-NEEDED-MODULE-FIXTURE-01` | `scripts/test_ps4_dynlink.py` | A synthetic ELF fixture with two distinct needed-module records should parse both IDs and the second version. This pins parser list handling only; it says nothing about loader acceptance or module runtime semantics. | ACTIVE; 0 failed tries |
| `SRV-01-REGION-SPLIT-OVERSIZE-LEAD-NO-MUTATION-FIXTURE-01` | `native/tests/test_region_split.cpp` | With `free.size() == 0x10000`, a lead of `0x10001` should be rejected and both node byte arrays preserved. This pins the existing helper guard only. | ACTIVE; 0 failed tries |

No matching task IDs were found in `docs/TASKS.md` or `docs/EVIDENCE.md` before these claims. Graft
scouts identified each exact helper and current gap; root verified the live source and test spans,
current hashes, and scope. The parser candidate repurposes the unused synthetic export-library
dynamic record as the second needed-module record, without changing production parser code.

## Baselines and claim-time identities

| Scope | Exact command | Result / log |
|---|---|---|
| Launcher | `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidatorTests` | Exit 0; 10 tests in 1 suite. `local/validation/root-round71-scout/app-baseline.log`, SHA-256 `8b8b60f4c57d08cc9f4c66aee2f6d5bd8e11525cc1ecdcd72ec002a651a1cdd2`. |
| Dynlink parser | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` | Exit 0; 25 tests. `local/validation/root-round71-scout/dynlink-baseline.log`, SHA-256 `e88eaf6e50984f6551445a6c9e77e4ca9e63f0e93ed442e493d2682e576a4f19`. |
| Native region split | `cmake --build local/build/native --target test_region_split`; then `ctest --test-dir local/build/native -R '^region_split$'` | Build exit 0; focused CTest exit 0, 1/1 passed. Logs `region-split-baseline-build.log` SHA-256 `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748eea0a75d2448d65bf0ab` and `region-split-baseline-ctest.log` SHA-256 `b375c10499380371acc89985e8d8caeb36895dba02f44cd4f17b02350fcadcb4`. |

Claim-time hashes:

- `launcher/Sources/LauncherCore/SystemChecks.swift`: `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`.
- `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`: `f86ad0e6e85a6a33b5a5a19e57a5d14e45942fa278ce2452e6682050651f7cd4`.
- `scripts/ps4_dynlink.py`: `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`.
- `scripts/test_ps4_dynlink.py`: `ca04e6fa132262c90643731ac607b9a3873303adeab6978443efd9132bbb6f95`.
- `native/core/region_split.cpp`: `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`.
- `native/tests/test_region_split.cpp`: `929f85ae00e44b9ed5bf8d50de6aed6fed1ab25765f4da61fbf92210ca5b1141`.

## Limits, risks, and next action

The candidate assertions cover temporary filesystem classification, a synthetic ELF data structure,
and helper-level node arithmetic. They do not establish native ELF loading, game startup, game audio,
save/reload, or Metal behavior. Existing user-owned `native/api/CMakeLists.txt` and untracked
`native/api/host/` are unchanged and excluded from publication.

At claim time, G0 remains PARTIAL and G1–G6 remain NOT RUN. No install/dependency fact changed.
Implementation has not started. Claim publication checks: `python3 -I scripts/project_index.py`
and `--check` exited 0 at 14,466 folders (log hashes `7a011c9c98b920104871e7cee6a0c371391892a025c329d853d8c830c2ca0d99` and
`cfda4d504c8a996ed5e632aa4f3299497abf52178d627dc583bd192c496e3886`); staged audit accepted 4
paths (log SHA-256 `fc3f0976acf9a7c706f563af96e66a47fc29eb33d6eaeb413f6c9a9c40fd5f0b`); full audit
accepted 1,646 paths (log SHA-256 `0fa611c7c1b08cf42e9741cfc5ae40739623789425835c717691c4440aff658a`);
`git diff --cached --check` exited 0 (empty log hash
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Next exact action: dispatch
the three single-file edits, independently review and replay them, record E213–E215 and a completion
checkpoint, refresh Trail after source changes, audit allowlisted paths, and push completion.
