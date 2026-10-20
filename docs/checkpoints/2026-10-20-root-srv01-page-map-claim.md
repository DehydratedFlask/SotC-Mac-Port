# SRV-01-PAGE-MAP-19660-01 — offline characterization claim

- Date/time: 2026-10-20 00:01 CDT (America/Chicago).
- Integrator: `/root`; assigned analyst: `/root/srv01_page_map_19660`.
- Task ID: SRV-01-PAGE-MAP-19660-01; parent SRV-01 / RE-02.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this checkpoint.
  No native implementation or private analysis artifact has been changed by the claim.
- Failed hypotheses/tries: 0.

## Bounded scope

Characterize only the pinned local ELF function target at `0x19660` and enough direct callers,
callees, data references, and byte-level context to describe its observed control/data flow. This is
the next open SRV-01 page-map target. Nearby allocator comments and prior fixture lifts are clues,
not proof of this function's role. Do not label a bitmap/page allocator operation unless the bytes
and references support it. Do not write replacement source or change any Ghidra/REA baseline.

## Acceptance and limits

- Use the pinned `local/re/eboot.elf`; record its SHA-256, image base and REA/Ghidra versions.
- Use REA/Ghidra at `0x19660` and a distinct snapshot at `local/re/rea-19660-snapshot.json`.
- Preserve the exact REA command, analysis JSON/stderr, Evidence ID, function range/body hash,
  direct calls, data refs, unresolved flows, relationship coverage, and any independent raw-byte or
  disassembly verification under the existing ignored `local/re/evidence/` directory.
- Keep all binary-derived outputs private. Do not run the game, install or download tools, or alter
  `local/re/rea-snapshot.json`.
- A function dossier is static evidence only; it does not establish runtime behavior, reachability,
  service semantics, a native ABI, game startup, or G1.

## Claim checkpoint

- Exact target: `local/re/eboot.elf`, address `0x19660`.
- Exact command shape: pinned REA CLI `function local/re/eboot.elf 0x19660 --provider ghidra --snapshot local/re/rea-19660-snapshot.json --format json`; capture stdout/stderr and the exact environment in private command files.
- Exit/result: pending.
- Evidence/log paths: pending under ignored `local/re/evidence/`.
- Remaining risks: target boundary, function semantics, and relationship coverage are unknown.
- Next exact action: run one hash-bound REA/Ghidra inspection, export evidence, independently check raw bytes when available, and report limitations to root.
- Status: CLAIMED; 0 failed hypotheses/tries. Root owns shared docs, index, audits, commit, and push.
