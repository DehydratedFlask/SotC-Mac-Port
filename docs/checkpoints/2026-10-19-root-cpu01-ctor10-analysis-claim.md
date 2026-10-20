# CPU-01-CTOR10-ANALYSIS-01 — first unresolved constructor analysis claim

- Date/time: claim 2026-10-19 23:29 CDT; completion 2026-10-19 23:44 CDT (America/Chicago).
- Integrator: `/root`; assigned analyst: `/root/cpu_slot10_analysis`.
- Task ID: CPU-01-CTOR10-ANALYSIS-01; parent CPU-01 / RE-02.
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No analysis source
  or native code has been changed.
- Failed tries: 0.

## Prerequisite evidence and bounded scope

The CPU-01-CTOR-PREFIX-01 fixture stops after ten recovered callbacks and reports original
constructor index 10, RVA `0xf09e60`, as the first unresolved slot. Its exact ELF identity is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; the 573-entry original table
starts at `0x2dd70e0` and has SHA-256
`5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`.

Graft `grep "f09e60"` finds the null registration, test assertion, and table checker only. Earlier
static notes record a 255-byte body hash
`fde1ac223a4c084da0a1a9d04804bee8ba8e12be0ac46e1a8e98a925ac4ffcc7`, but complete call/data
semantics have not been corroborated. The task is read-only: use REA/Ghidra on the local ELF at
image base zero, save an exported evidence record and reproducible recipe, and classify direct/
indirect call and data-reference coverage. Do not implement a replacement, run the game, download
files, or modify the baseline `local/re/rea-snapshot.json`.

## Acceptance and limits

Exact REA command:

```sh
rea function local/re/eboot.elf 0xf09e60 --provider ghidra --snapshot local/re/rea-f09e60-snapshot.json --format json
```

Retain logs, Evidence ID, source digest, tool versions, image base, function range and byte hash,
coverage, direct calls/import identities, data references, and unresolved computed calls under
private `local/re/evidence/` or `local/validation/`. Independently verify the input hash and any
byte/range claims. A missing function or incomplete references are valid documented outcomes, not
permission to infer semantics. This task can identify the blocker and required contracts only; it
cannot pass G1 or replace the runtime constructor.

## Completion checkpoint

- Agent ID: `/root/cpu_slot10_analysis`; integrator and independent verifier: `/root`.
- Shared files updated: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint.
  Analysis artifacts and game bytes remain private under `local/`; no native implementation changed.
- Exact worker command (process-local environment; exit 0; REA stderr empty):

```sh
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xf09e60 --provider ghidra --snapshot local/re/rea-f09e60-snapshot.json --format json > local/re/evidence/rea-f09e60-analysis.json 2> local/re/evidence/rea-f09e60-analysis.stderr.log
```

- Tool identity: REA 6.7.0, Ghidra 12.1.4, OpenJDK 25.0.4.1, Ghidra language
  `x86:LE:64:default:gcc`, profile digest
  `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. Evidence ID:
  `ev_054b2f190852ca75b5f92898208d4f8e554fbcfe6ab6416b0c018242902d7921`.
- Identity: ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image-base observation 0.
  New snapshot SHA-256
  `cb35935424cae5121c34741b73453d545a49bd61c65b68727ffcf6b9c8333352`. Baseline snapshot was
  `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354` both before and after.
- Observation: Ghidra assigns `FUN_00f09e60` inclusive range `0xf09e60..0xf09f63` (260 bytes),
  SHA-256 `22204129cd9b4997ae893d5d078a9bf8b0409114bb122fab2324956b5039a3c4`. The previous
  255-byte body hash matches bytes through RET and excludes a terminal five-byte direct call.
  Independent disassembly verifies seven direct calls:
  `f09ec7→e1eda0`, `f09f04→e1ede0`, `f09f13→b30a10`, `f09f1a→f90210`, `f09f2c→b30950`,
  `f09f42→f8f390`, `f09f5f→f8f3a0`; the computed call `0xf09efb` through `[RAX+0xc0]` is unresolved.
  Three thunk GOT slots match the same-ELF import table: `malloc` and `__cxa_atexit` are
  `name-by-nid-hash-only`; `__stack_chk_fail` is `contract-declared`. These are evidence labels, not
  ABI proof. Non-stack data targets are recorded in E43 and the private verifier report.
- Root independently ran
  `python3 -I local/re/evidence/rea-f09e60-verify.py > local/re/evidence/rea-f09e60-root-verification.json`
  (exit 0). `cmp local/re/evidence/rea-f09e60-root-verification.json local/re/evidence/rea-f09e60-verification.json`
  passed. Root hashes: ELF matches the digest above; baseline and new snapshots match their hashes;
  analysis JSON SHA-256 `49d7f43778017ab4b7aa2fd38f1a0fdd94a25118bc157451875fd9e141be484c`; worker and
  root verifier outputs both SHA-256
  `f606e0aed3da8a64a94ff1835a3033096cfd7fff3c639866c5f4c71e1d0391d3`.
- Observation vs inference: byte ranges, call destinations, import slots, and data references are
  cross-checked against the pinned ELF. Code flow suggests global allocation and cleanup
  registration, but helpers/callback are unnamed. Global Ghidra logs contain unrelated function
  overlap repairs at `0xb30a00`/`0xb30c20` near `0xb30a10`; preserve caution about that boundary.
  The indirect call, service contracts, runtime reachability, and ABI remain unknown; G1 is NOT
  PASSED.
- Failed attempts: REA invocation 0. The private verifier's first run failed because it expected
  the snapshot field `id` instead of `evidence_id`; correction was verified successfully. Count one
  task checker correction, not an REA failure; no three-try escalation.
- Private files include `rea-f09e60-summary.md`, analysis JSON/stderr, separate snapshot, exact
  command, progress log, objdump/thunk/reference checks, checker script and worker/root outputs.
- Task status: STATIC_ANALYSIS_VERIFIED for the bounded dossier. Exact next action: keep CPU-01 and
  G1 open; any attempt to resolve the indirect target/helper contracts must be separately scoped,
  hash-bound and evidence-backed.
