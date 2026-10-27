# Claim: check SELF program-header 10 recoverability

Date: 2026-10-27 15:01 CDT
Integrator: Codex `/root`
Task: `GPU-03-CMSH-TEX0-SEGMENT10-RECOVERABILITY-01`
Status: **CLAIMED**, 0 attempts; parser replay NOT RUN

## Scope

Determine whether the existing pinned offline plaintext SELF parser can map source bytes to ELF
program-header 10, which E662 left out of the reconstructed ELF. This is a read-only coverage check.
It does not reopen the E662 TEX0 search, analyze segment contents, change the parser, alter the
synthetic UV harness, or establish TEX0 semantics.

## Exact input identity and current evidence

- Original local SELF: `local/extracted/CUSA08809/eboot.bin`, 65,314,052 bytes, SHA-256
  `21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0` (rechecked before claim).
- Immutable reference checkout: `reference/ShadowOfTheColossus-PC` at
  `70bb43859cd7ff181a6d409540b757866184d60d`; `scripts/prepare.py` SHA-256
  `9459d439056e9ca61758ad8f3023f0c1e46b085dd1524644446af00374856890`.
- Local reader wrapper: `scripts/unwrap_self.py` SHA-256
  `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`.
- E662's admitted reconstructed ELF is 65,214,308 bytes, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. E662 records PHDR 10
  (`type 0x6fffff01`, `filesz 2928`, `memsz 0`) in `missing_segments`; its semantics are
  UNCLASSIFIED. The parser copies only segments with the plaintext flag, rejects encrypted or
  compressed segments, rejects unsupported blocked layouts, and requires executable/dynamic/
  relocation segments.
- Prior task checkpoint: [E662 format-code-6 investigation](2026-10-25-gpu03-cmsh-tex0-format-6.md).

## Acceptance and stop conditions

Using `parse_self` in memory against the hash-pinned local SELF, record the parser source identity,
SELF descriptor rows, exact PHDR 10 fields, whether any admitted plaintext descriptor maps bytes to
PHDR 10, and the resulting `missing`/recoverability state. Keep any byte-derived output private in
`local/validation/orch-20261027/gpu03-tex0-seg10/`.

Stop after this exact check. Do not write a new ELF, use alternate decrypt/unpack tooling, modify
`reference/` or `scripts/unwrap_self.py`, disassemble or interpret segment contents, or change a
fixture. If the admitted reader cannot recover bytes for PHDR 10, record that limitation and retain
E662's UNKNOWN result. If bytes are present, this task still does not analyze their contents; a
separate bounded claim is required before any further code analysis.

## Claim evidence and status

- Starting branch: clean `main == origin/main == b815af341a61b0bdf4a0dbc2a918a78fa420abae`.
- Fresh remote scan after E824: 9 branch refs, 12 pull-request refs, 0 tags, 15 unique tips,
  2,437 reachable commits; zero Claude/Anthropic author/committer identity and credit-line hits.
  Private `refs/stash` remains unchanged by owner choice.
- Input hashes, pinned checkout and source paths were verified. The parser replay is NOT RUN.
- Required precommit gates: CMake configure exit 0; build exit 0 (`ninja: no work to do`); CTest
  **431/431**; lift sweep **516 passed / 0 skipped / 0 failed**; Trail graph **9,877 nodes / 13,470
  edges**, deep layer not built; project index **31,934 folders**, final `--check` current.
- The staged path audit passed **6 paths**; full path audit passed **3,042 paths**. Precommit
  attribution passed for `origin/main..HEAD`; `git diff --check` passed. Remaining publication
  checks (exact commit range, postcommit audit, push/fetch and fresh remote scan) are pending. Initial
  `python3 scripts/project_index.py --check` exited 1 with `Stale indexes:
  local/indexes/folders.json` after new validation log files appeared. Once all expected E825 log
  paths existed, project-index regeneration and a serial check passed. Exact logs are under
  `local/validation/orch-20261027/e825/`.
- The first staged `git diff --cached --check` found three trailing whitespace lines in checkpoint
  metadata. They were removed and the final staged check passed. That initial check was not redirected
  to a separate log; the failure and correction are recorded here.
- All exact gate commands are the workspace-mandated commands in `AGENTS.md`; the command logs are
  `cmake-configure.log`, `cmake-build.log`, `ctest.log`, `lift-checkers.log`, `trail-build-check.log`,
  `project-index-generate.log`, `project-index-check.log`, `full-path-audit.log`,
  `attribution-precommit.log`, and `diff-check.log` in that private directory.
- Root owns shared docs and publication. Worker execution may begin only after the E825 claim is
  published. Proposed private report path:
  `local/validation/orch-20261027/gpu03-tex0-seg10/report.json`.
- Observation: E662's reconstructed ELF omits PHDR 10. Inference awaiting this task: current reader
  may not have a plaintext descriptor for it; that mapping has not been inspected.
- Next exact action: after publication, run one in-memory `parse_self` check, record descriptors,
  PHDR 10 fields and missing/recoverability state, then request independent report review.
- Task status: CLAIMED; 0 failed tries; no dependency or source edit.
