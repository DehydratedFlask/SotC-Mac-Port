# Startup slot 306 — read-only survey claim

## 2026-10-25 10:06 CDT — claimed on `main`

- Integrator: Codex `/root`; task `CPU-01-STARTUP-SLOT-306-SURVEY-01` is **ACTIVE**, attempt 0.
  The checkout is clean on `main` at `2ac26e2076f5541bc918151c83235fb8ee74ee61` before this claim.
- Bounded output: characterize constructor table index **306**, target RVA **`0xa633b0`**, by
  read-only analysis of the pinned local ELF. Confirm table mapping and ELF identity; establish exact
  body extent/hash, decode/call structure, direct callers and callees, global/side-effect footprint,
  and unresolved indirect edges. Do not add a native constructor or register the row in this task.
- Baseline: E623 reports **317/573** registered constructor entries and the first missing index
  306, registered-index-set SHA-256
  `ee1bf21bc35e9baddf2d4a54f8296379786ab103b630c92779d2f2ae53d976e0`. Pinned ELF is
  `local/re/eboot.elf`, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base zero.
- Existing observations are only leads: `local/re/evidence/startup-cliff-296/out/facts.json` maps
  index 306 to table word `0x2dd6750` -> `0xa633b0`; the R30 Ghidra command covered 302, 303, 305,
  and their helpers, not 306. Separate evidence includes a `full.dis` prologue, a `funcs.tsv` range
  `0xa633b0..0xa654be` (reported size `0x210e`), and a 209-row outgoing-call-edge listing, but these
  were produced for other tasks and do not independently establish a complete body or call contract.
  Recheck every fact against the pinned ELF before accepting it.
- Observation versus inference: the table gap and referenced address are established in earlier
  table evidence. The function’s behavior, exact extent, caller reachability, and native-lift safety
  are **UNKNOWN** until this task's verification. Do not infer runtime startup or boot behavior.
- Risks: a reported 0x210e-byte function may be a large subsystem and can contain indirect calls or
  unresolved external effects; keep those visible. If the body/call contract remains incomplete,
  record UNKNOWN and preserve the gap. G1 remains PARTIAL.
- Exact next action: inspect existing private ranges and call-edge exports, validate relevant bytes
  and address mappings against `local/re/eboot.elf`, then create a reproducible private verification
  record under `local/re/evidence/` with source digest, tool/version, coverage, commands, hashes, and
  unresolved edges. Use only the existing read-only Ghidra/REA path if byte evidence leaves material
  questions.
- Files changed for claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and
  this checkpoint. No native/source implementation, installation, or binary execution.
- Status: **ACTIVE**, attempt 0, no failed hypotheses. Next step is private read-only evidence review.

## 2026-10-25 10:34 CDT — read-only characterization completed and independently audited

- Integrator: Codex `/root`; reviewers `/root/next_task_triage`, `/root/slot306_callers`, and
  independent read-only reviewer `/root/audit_slot306`. Branch is `main`; no source, constructor,
  registration, or runtime behavior was changed.
- Exact private verification command set is preserved in
  `local/re/evidence/startup-slot-306-survey-20261025/body/commands.txt`. Core commands were:
  `file`, `shasum -a 256`, and `stat` on `local/re/eboot.elf`; `xcrun llvm-objdump -p` for ELF headers;
  `xcrun llvm-objdump -d --start-address=0xa633b0 --stop-address=0xa654c0` for linear disassembly;
  `env PYTHONDONTWRITEBYTECODE=1 python3 -I tools/lift/analyze_slot_struct.py 0xa633b0`; then
  `verify_slot306.py` and the read-only scratch inventory. The pinned ELF SHA-256 is
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Result: index 306 maps from file offset `0x2dda750`, raw bytes `b033a60000000000`, to RVA
  `0xa633b0`. Reachability body `[0xa633b0,0xa654bd)` is 8,461 B, hash
  `f8f707c7f80129cb79d2d54a61b6fd53caa62bf920c36f18a5b93152ff9933c8`; the existing FDE/function
  inventory row spans 8,462 B and includes a trailing NOP. Ghidra's body count is 1,568; LLVM's
  requested range covers 1,571 instruction rows including three trailing NOPs; reachability reports
  1,582. LLVM and the prior Ghidra edge export agree on 198 direct calls/15 targets; reachability
  reports 196 and omits `0xa637ae -> 0xf8f390` and `0xa643ad -> 0xc300`. Eleven indirect calls remain
  unresolved. The Ghidra reference query reports one DATA reference from the table word and no
  incoming code/call reference; this is not proof against indirect dispatch.
- Independent audit checked the table word, body span/hash, all 18 body-manifest entries and all
  caller checksums. It found `body/REPORT.md` incorrectly says three `PT_LOAD` headers. The pinned
  ELF has 11 program headers and **two** `PT_LOAD` headers (phdrs 0 and 2), as shown by the raw parse,
  machine-readable verifier, and table-word record; public evidence uses the corrected value while
  the original private report remains unchanged. All 61 direct RIP-relative store destinations are
  in RW zero-fill, but the analyzer footprint omits pointer-derived/transitive callee effects and
  does not establish ownership/semantics.
- Side effects/risks remain explicit: the fixed-path analyzer wrote 27 scratch intermediates under
  its pre-existing system temp directory without a before-inventory, so an older same-named file
  replacement cannot be excluded. Ghidra -readOnly changed four index-cache mtimes; project/program
  metadata mtimes stayed unchanged, but byte-for-byte database identity is unverified. No cleanup or
  Ghidra/REA rerun occurred.
- Files for public closeout: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this
  checkpoint. Status: **STATIC_ANALYSIS_VERIFIED** for address/table/body/call-footprint only;
  constructor semantic contract stays UNKNOWN, no implementation/runtime claim; G1 remains PARTIAL.
- Exact next action: publish the evidence closeout through the allowlisted main-branch process, then
  pursue another independently READY task without reopening the persistent Ghidra project.

## 2026-10-25 10:07 CDT — claim published

- Commit `c521168202de3e43fbe459af5ffb46cb73ddf214` was pushed to GitHub `main`; post-push fetch
  confirmed `HEAD == origin/main` and clean status on `main`.
- Staged/all path audits passed 6/2,796; attribution passed. Author and committer are Johnny Wang;
  no Claude/Anthropic/co-author trailer. This records publication of the task claim only, not analysis
  of slot 306.
