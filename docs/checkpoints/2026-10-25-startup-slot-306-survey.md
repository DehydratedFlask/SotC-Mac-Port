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
