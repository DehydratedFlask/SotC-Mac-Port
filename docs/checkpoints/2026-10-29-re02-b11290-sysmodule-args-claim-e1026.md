# E1026 — claim system-module loader argument map

Date: 2026-10-29 22:07 CDT
Agent: `/root`
Task: `RE-02-APPMAIN-B11290-SYSMODULE-ARGS-01`
Status: `ACTIVE` — no analysis started

## Scope

Resolve the 15 direct `sceSysmoduleLoadModule` call sites in app-main Init callee `0xb11290`. Record each call-site address and its immediate integer module-ID argument from the pinned ELF. Cross-reference IDs against attributable module-ID tables, naming a module only when a source supports the mapping. Preserve numeric-only rows where names cannot be verified.

Do not infer API effects, initialization dependencies, engine/world-init semantics, runtime order, or boot success. Do not execute original game code. The existing import map E1025 establishes only that `0xf90d30` is the `sceSysmoduleLoadModule` import; it does not establish its arguments or effects.

## Inputs and prerequisites

- Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Existing structural dossier: E847, app-main Init callee `0xb11290`, 173 bytes / 37 reachable instructions / 16 direct calls and one direct jump, with no indirect transfer in that body.
- E1025 private report: `local/validation/orch-20261030/RE-02-APPMAIN-B11290-IMPORT-MAP-01/REPORT.md`, SHA-256 `c9510ce5238da29caca92cbc54303406f7bc1c6f4af032afa0c8bab44641fed4`.

## Acceptance

1. Run Trail before searching source/reference for module-ID definitions.
2. Verify all 15 call sites against the pinned ELF and existing disassembly profile; record exact IDs and byte/address evidence.
3. Cite the exact file/table or primary source for every human-readable module name. Keep unsupported names unknown.
4. Keep the task `STATIC_ANALYSIS_VERIFIED` for bytes/IDs only. Do not claim runtime effects, boot necessity, or an engine-init path.

No source files are owned by this task. Private report should go under `local/validation/orch-20261029/RE-02-APPMAIN-B11290-SYSMODULE-ARGS-01/`.
