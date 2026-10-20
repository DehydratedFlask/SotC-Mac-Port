# AUD-02 RackQueryBufferSize provider slot release

- Time: 2026-10-20 04:21 CDT (America/Chicago).
- Agent / integrator: `/root`; task `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01` (already claimed in the earlier three-probe batch).
- Status: `ACTIVE`; 0 failed analysis hypotheses and 0 failed provider operations before this release.
- Bounded outcome: one serialized REA/Ghidra xrefs query on import slot `0x1601000` for symbol 239 / NID `0eFLVCfWVds` / `sceNgs2RackQueryBufferSize`. No second provider operation may start until it returns and root reviews it.

## Prior evidence and inputs

- Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; import manifest `local/re/imports.json`, SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- The root-replayed offline probe records 15 raw `E8` patterns and an `objdump` candidate at `0x94fc2d` calling `0xf91420`. The selected disassembly emitted a section-range warning, so no instruction boundary, function or caller is accepted yet. Details and hashes are in `docs/checkpoints/2026-10-20-root-aud02-next-three-provider-replay.md` and the private task dossier.
- The previous serialized RackCreateWithAllocator query completed and its root byte path was published. VoiceGetStateFlags query also completed and was root replayed. No provider call is currently in flight.

## Released operation

- Owner: `/root/aud02_rackquery_buffersize`; the worker owns only its private dossier and will not edit shared docs, source, reference clones or Git.
- Operation: exactly one REA CLI `xrefs` query for slot `0x1601000`, provider `ghidra`, against the pinned ELF, using the established Ghidra 12.1.4 / REA 6.7.0 CLI setup with `GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`.
- Use a private snapshot copied from the shared baseline before the query. Capture raw stdout/stderr, exact argv and redacted environment/hash, exit/status, duration, resource samples and before/after snapshot/input hashes. Do not issue another xrefs or function query.
- The worker reports only private artifacts and stops. Root independently checksums/replays the dossier and decides whether a follow-up is warranted.

## Remaining limits and next action

- An xrefs address alone may not identify the reference kind or caller. No path, reachability, ABI, runtime binding, command meaning, NGS2 semantics or audio use is accepted until root replays the exact bytes.
- Root is publishing this release checkpoint before dispatch. G1 remains NOT PASSED; G2 remains NOT RUN. `tools/graft/` remains unstaged.
- Next exact action: after the checkpoint is pushed, send the worker the single-operation instruction above and await its result.
