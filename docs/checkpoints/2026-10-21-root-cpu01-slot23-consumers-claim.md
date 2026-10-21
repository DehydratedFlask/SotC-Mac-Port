# Claim: CPU-01-SLOT23-CONSUMERS-01 — trace the slot-23 name/hash table

- Time / timezone: 2026-10-21 09:03 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `CPU-01-SLOT23-CONSUMERS-01`, ACTIVE, attempt 0. This is offline
  static analysis only; no native code, game data, or policy change is included in the claim.
- Prerequisites: `CPU-01-LEAF-CTOR-BATCH-33` and `CPU-01-NAMEHASH-E71990-01`; source image
  `local/re/eboot.elf`, SHA256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Question / acceptance: establish which consumers use the slot-23 table at `0x3353f40`, whether
  the 13 non-sentinel string pointers or only their hashes affect required behavior, and whether
  the 14th hash-zero record is used. Bound each observed edge by original address, source digest,
  image base and REA Evidence; preserve indirect-call and coverage limits. Do not add names to public
  code or docs, register slot 23, or advance G1.
- Existing prior evidence: ctor 0xe71990 is a 337-byte store-only body with 14 16-byte
  `{string pointer, hash}` records. The runtime hash routine at 0xe70aa0 is already verified as
  signed-byte djb2-XOR with zero remapped to one; accessors 0xe70a60/0xe70a80 bound indices to
  0–12, and record 13 is not reached by those accessors. Prior checkpoints:
  `docs/checkpoints/2026-10-21-root-native-batch-33.md` and
  `docs/checkpoints/2026-10-21-agentB-namehash-e71990.md`.
- Exact next action: inspect the current REA target state; reuse a matching `eboot.elf` session if
  present, otherwise open the exact hashed artifact read-only. Query focused references and analyze
  consumers around the table and accessors, then export the required Evidence before closing.
- Owner boundaries: root is the only owner of `HANDOFF.md`, task/evidence ledgers and indexes. Any
  parallel worker is read-only and returns exact addresses, commands, Evidence IDs and limitations.
- Remaining risk / policy boundary: previous analysis explicitly left a principal decision on
  importing the original name strings as ordinary native data. This task will first determine
  whether consumers make them necessary; it does not decide or implement that import.
- Checkpoint status: task claim recorded in `HANDOFF.md` and `docs/TASKS.md`; no new evidence or
  installation facts yet. Update indexes after this claim checkpoint is created.
