# Checkpoint: CPU-01-SLOT23-CONSUMERS-01 — partial static trace

- Time / timezone: 2026-10-21 09:19 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / status / attempts: `CPU-01-SLOT23-CONSUMERS-01`, PARTIAL; 0 hypothesis failures and 1
  Ghidra provider timeout. No code or name strings were added.
- Files changed: this checkpoint, `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md`.
- Source artifact: `local/re/eboot.elf`, SHA256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; ELF64 little-endian
  x86-64. Root parsed the ELF program headers; image base is `0`, with PT_LOAD mappings recorded
  in private `local/checkpoints/root-slot23-elf-layout.json`.

## Observations

- REA `inspect_with_objdump(operation=file_headers)` returned Evidence
  `ev_0df62c09c752ca6cf1f9a8b233c35b91dcc7af3605ecaa15498e08324129fd1c`, exit 0, and the same
  artifact digest. Provider: GNU objdump; version was not probed. This evidence covers identity and
  file headers only, not function relationships.
- The slot-23 initializer at `0xe71990` publishes 14 `{pointer, u32 hash}` rows at `0x3353f40`.
  Existing evidence/checkers establish that accessors `0xe70a60` and `0xe70a80` accept indices
  0–12; the zero-hash row 13 at `0x3354010` is outside those accessor bounds. The pointed-to
  names remain private. The row-by-row hash values from the worker's raw scan were not independently
  verified or persisted in this checkpoint.
- Independent root disassembly at `0xe5c3f5` confirms a call to the pointer accessor `0xe70a60`;
  its returned pointer is passed in `rsi` to `0xe70af0` at `0xe5c400`. The returned value from
  `0xe70af0` is then passed to `0xe71310`. This is an observed instruction sequence, but its
  containing function ownership and reachability are unresolved: bounded traversal from
  `0xe5c330` encounters an indirect `jmp rax` at `0xe5c365` and does not prove this block reachable.
- The bounded root traversal from `0xe5c330` reaches `0xe5cd78`: it loads a pointer from a
  0x28-byte object row at `+0x10`, hashes it with `0xe70aa0`, and passes the hash to `0xe715d0`.
  Root disassembly of `0xe715d0` shows an object subobject at `+8`, count at `+0x60`, row pointer at
  `+0x50`, a hash comparison at row `+0x14`, and a `0x28` stride. It returns the matching index or
  `-1`; the observed body has no calls or indirect transfers and no direct reference to the static
  slot-23 table.
- The existing private byte decoder reports `0xe70af0` over `[0xe70af0, 0xe710ff)`, 326 reachable
  instructions, 7 direct calls, and 31 indirect transfers. Those indirect targets and their runtime
  object/vtable identity are unresolved, so the pointer consumer's semantic purpose is unknown.

## Failed experiment and coverage

- REA opened the exact source with Ghidra 12.1.4, but default whole-program auto-analysis exceeded
  the 330,000 ms startup deadline while servicing `read_binary_inventory_summary`. Error:
  `provider_timeout`; run ID `52b7b008-dc88-4616-9c12-20076854357b`; analysis profile digest
  `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. The provider reported
  incomplete output, so no function evidence or evidence ID was returned. A subsequent focused
  function request also returned an error without retained evidence and was not retried.
- REA export produced 0 records at private
  `local/checkpoints/slot23-consumers-rea.bundle.json`; closing the ephemeral session produced a
  1,817-byte snapshot with 0 evidence records at
  `local/checkpoints/slot23-consumers-ghidra.snapshot`. These preserve no code-analysis result.
- Root used Apple LLVM 21.0.0 `llvm-objdump` to inspect selected ranges. It exited 0 and emitted
  instructions, while warning that no section overlaps each requested range; output is from
  `PT_LOAD#0` and does not establish Ghidra function boundaries. Private captures:
  `local/checkpoints/root-slot23-e5c3.txt`, `root-slot23-e5cd.txt`, and
  `root-slot23-e715d0.txt`.
- Root's bounded offline decoder outputs are private:
  `root-slot23-e70af0.json`, `root-slot23-e715d0.json`, `root-slot23-e5c330.json`, and
  `root-slot23-e5c3f5.json` under `local/checkpoints/`. No original strings were printed or
  included in public docs. No tests or builds were run; this checkpoint changes no implementation.

## Conclusion, risks, and next action

- The evidence establishes two different static paths: one decoded call sequence passes a
  name-table pointer to `0xe70af0`, while the reachable `0xe5cd78` path hashes an object-row pointer
  and searches a per-object hash list. This does not establish whether the names are required for
  the target behavior or what the 31 unresolved indirect transfers do.
- Do not register slot 23, copy strings, or claim G1. The remaining risk is function ownership and
  runtime dispatch semantics. Resolve the pointer consumer's indirect targets with a Ghidra session
  that can complete analysis, or a reviewed bounded alternative; then decide the narrow data-only
  lift with the principal before editing native code.
- Exact next action: continue only with a bounded, name-free trace of `0xe70af0`'s indirect calls and
  its object/vtable source; preserve all unresolved targets. The Ghidra startup timeout remains
  recorded and must not be hidden by treating the local decoder as equivalent evidence.
- Task remains PARTIAL. `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md` updated; tests NOT RUN.
