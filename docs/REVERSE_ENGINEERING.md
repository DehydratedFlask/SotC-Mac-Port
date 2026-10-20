# REA / Ghidra recovery playbook

Purpose: convert evidence about this exact PS4 remake into conventional ARM64 functions,
typed data and native services. Decompilation is a starting point, not recovered original source.
Use [task packets](TASKS.md), [source audit](REFERENCE_AUDIT.md), and [native contract](PLATFORM.md).

## Current binary boundary

- Input: local base CUSA08809 SELF, 65,314,052 bytes, SHA256
  `21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0`.
- Admitted plaintext ELF: `local/re/eboot.elf`, 65,214,308 bytes, SHA256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Entry ELF RVA `0xf120`; code PT_LOAD begins at virtual zero, file offset 16,384.
  Derive other file offsets from actual segment mappings; do not apply one universal subtraction.
- PS4 dynamic executable type `0xfe10`, machine x86-64, 11 program headers; segment 10 missing
  under the pinned plaintext reader. See identity JSON and [EXTRACTION](EXTRACTION.md).
- Update SFO is 01.01 but contains no replacement eboot. Keep executable/content versions separate.

The PS4 loader's nonstandard dynamic tables may not be interpreted by a generic ELF importer.
Default Ghidra import logged “ELF Dynamic table appears to have been stripped from binary.”
This is an importer observation, not proof that the game has no dynamic imports or relocations.
Use PS4-specific table readers and verify their byte spans against the exact input.

## Tool ownership and preserved evidence

One agent owns a shared REA MCP active binary session at a time. Other workers must not open,
close or annotate another target in that session. They can read exported evidence or use a
separate process-local CLI/persistent Ghidra project. Never open the same writable Ghidra project
from two processes. Keep original inputs immutable and work only under `local/`.

MCP calls actually made:

```text
open_binary({path: "/Volumes/Extra/SoC Mac Native/Shadow of the Colossus/Shadow of the Colossus.pkg"})
  -> unsupported format (no extraction)
open_binary({path: "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf", provider_id: "ghidra"})
  -> ELF x86_64, exact hash admitted
binary_overview({})
  -> startup timeout after 330,000 ms, no completed overview
```

The timeout diagnostic is in `local/re/evidence/rea-mcp-timeout.json`. REA removed its ephemeral
database. It reported Ghidra 12.1.4, profile digest
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`, overlap/decompiler warnings,
and a 2G heap. These warnings must not be converted into claims about valid recovered functions.

Recovery uses the existing public CLI with a larger process-local startup allowance and heap,
without editing client/MCP configuration or installing another engine. Command used:

```sh
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xf120 --provider ghidra --snapshot local/re/rea-snapshot.json --format json > local/re/evidence/entry.json 2> logs/rea-cli-entry.stderr
```

Inspect the current result in HANDOFF/EVIDENCE before re-running: a second CLI invocation may
repeat costly auto-analysis. Do not mistake an empty in-progress JSON for completed evidence.
The version-pinned public launcher `npx -y rea-agents@6.7.0` is a fallback only if the existing
launcher disappears; record any acquisition in INSTALL_LOG. Confirm installed CLI `function
--help`/schema; MCP names and CLI subcommands differ (`function`, not `analyze-function`).

The CLI recovery COMPLETED with exit 0 on 2026-10-19. Canonical dossier:
`local/re/evidence/entry.json`, Evidence ID
`ev_51f10a737f14992cc6215e62b361b86a073fdce4a36455feb6d18635bc8a78b4`.
It observes FUN_0000f120 (81-byte body) calling six unique targets at seven sites; see EVIDENCE.
Typed function semantics and native implementations remain unverified. Raw pseudocode stays private.

Portable evidence projection was also executed successfully, without a fresh Ghidra import:

```sh
/Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea inspect-analysis-view local/re/evidence/entry-view-request.json --format json
```

The request embeds the completed canonical Evidence and selects native/pseudocode, offset 0,
limit 4096. Result saved at `local/re/evidence/entry-view.json`. To inspect another supported
facet, prepare a new request against the same parent, following the installed tool's schema.
Do not modify the parent Evidence or call the producer again to page its existing results.

The existing pinned preparation script was also executed as OFFLINE INSPECTION only:

```sh
python3 reference/ShadowOfTheColossus-PC/scripts/prepare.py local/extracted/CUSA08809 --out local/re/pc-loader-reference
```

Its analysis.json gives 588 imports, 712,333 relocations, 49 module declarations and some name
hints. Its boot.bin/entry.bin remain x86 reference-loader artifacts: never execute or ship them.
This parser output is useful for RE-02, but it does not resolve all NIDs or native call targets.

## Persistent Ghidra continuation recipe

REA's Ghidra database/annotations are ephemeral; CLI snapshots/evidence do not imply a saved .gpr.
Before long recovery batches, create one persistent private project so every agent need not
repeat auto-analysis. This recipe is a planned follow-up, not a claim that a project exists:

```sh
mkdir -p local/ghidra
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless "/Volumes/Extra/SoC Mac Native/local/ghidra" SotC -import "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf" -analysisTimeoutPerFile 1200 -log "/Volumes/Extra/SoC Mac Native/logs/ghidra-persistent.log"
```

Do not use `-readOnly`/`-deleteProject` for the persistent import. Review completion logs and
timeout state; a saved partial project is still partial. Record language/compiler, image base,
memory blocks, analyzers/options, warnings and tool identity. Reopen with `-process eboot.elf`
or GUI only after verifying the actual saved program name and project lock state. No implicit
`-overwrite`, image-base patch, forced function boundary or analyzer disabling to fake success.

If an analyzer is disabled for a targeted experiment, give the profile a distinct identity,
list lost coverage and do not merge it with default-profile evidence. Preserve names/comments
as replayable address/hash-bound annotations, not only in a transient GUI database.

## Stage 1 — layout, linkage and startup index

1. Verify SELF/ELF hashes. Read every program header and classify code/data/TLS/exception,
   PS4-specific dynamic/relocation segments and unavailable segments. Record loaded versus
   file-backed ranges, zero-fill, permissions, alignment and base assumptions.
2. Enumerate dynamic tags, imports/exports, NIDs with library/module identity and versions,
   relocations and initialization records. Use pinned `prepare.py`/`link_modules.py` as format
   references, not as a runtime implementation. Verify lengths/offsets and reject malformed data.
3. Build a NID-to-contract table from open reference declarations, exact export metadata and
   game call sites. Track collisions/ambiguity. A source spelling inferred from a hash stays
   inferred until the module/version/signature matches.
4. Seed at actual Ghidra entry corresponding to RVA 0xf120. Record original bytes/assembly,
   call/data references and decompilation limitations. Confirm the function boundary manually
   where auto-analysis overlaps or invents a no-return path.
5. Traverse entry -> runtime initialization -> global constructors -> engine initialization ->
   job system -> asset mounts -> scene load -> main update/render/audio/save callbacks.
   Do not assume compiler entry equals the gameplay main function.
6. Resolve callback/vtable registration and jump tables. Include error/cleanup edges, dynamic
   dispatch targets, exception/unwind and static TLS dependencies. Keep unresolved edges explicit.

Deliver `local/re/functions.jsonl`, `local/re/imports.json`, `local/re/unresolved.jsonl`,
and a small reviewed boot graph (original RVA -> native task -> required contracts).
Use [function record schema](schemas/function.example.json); never populate it with guessed
native symbols or pretend all discovered functions are reachable game code.

## Stage 2 — per-function recovery protocol

For each bounded native function task:

1. Record exact ELF hash, original RVA, Ghidra image base, source-byte mapping and evidence ID.
2. Review original assembly alongside pseudocode, callers, callees, referenced constants/data,
   input/output widths, ownership, side effects and error cases. Pseudocode types are hypotheses.
3. Recover the interface and data schemas first. Resolve pointer packing, bitfields, virtual calls,
   signed/unsigned operations, FP rounding/NaNs, SIMD operations, aliasing and alignment.
4. Write ordinary native functions with typed parameters/native call targets. Replace imported
   services by their actually used contracts. No original address can remain an executable pointer.
5. Derive independent fixtures: arithmetic boundary cases, known data invariants or separately
   established observations. Define FP tolerance before testing. Never compare two copies of the
   same erroneous pseudocode and call that an independent oracle.
6. Build ARM64, check disassembly and run the fixture with ASan/UBSan as appropriate. TSan is
   separate for concurrency. Keep real-game integration status distinct from fixture status.
7. Add original-to-native symbol mapping, unresolved behavior, test command/results and next
   callers in the manifest. Update TASKS/EVIDENCE/HANDOFF at the checkpoint.

If an offline lifter is considered, test a representative small slice before adoption: indirect
calls, stack objects, floats/SIMD, exceptions and packed pointers. Inspect generated runtime
dependencies. A persistent guest-register/flags machine, guest memory executor or dispatch loop
fails the native contract even if its executable is ARM64. Exported Ghidra C is not drop-in C++.

## Stage 3 — focused audio recovery without firmware

Do not wait for libSceNgs2.sprx or libSceUlt.sprx; they are not supplied as a requirement and
must never become runtime dependencies. Work from this game's import call sites, command
buffers, asset readers and native service behavior. Source stubs provide names, not mixer logic.

- Seed from NGS2 system/rack/voice/render imports and their callers, using the import manifest.
  Follow each parameter-list builder to its concrete bytes; identify opcodes, lengths, alignment,
  rack IDs, voice IDs, patch endpoints, sample/config pointers and callback/event state.
- Trace bank/stream lookup and decompression into sample buffers. Connect asset IDs to the
  supplied archive member and language/scene. Separate streaming codec frames from PCM buffers.
- Recover voice lifecycle: allocation, start/stop, loop/seek, gain/rate/pan, graph patches,
  render blocks, finished callbacks, virtualization and teardown. Determine required effects
  from command values and call sites; do not implement guessed generic NGS2 behavior.
- Determine actual Audio3D use and required ULT task semantics. Native recovery may eliminate
  the need to reproduce unused middleware internals; prove that with reachability evidence.
- Deliver a command/API matrix, typed bank schema, deterministic decoder and mix fixtures,
  then live event integration. Every unsupported used command must fail visibly.

Only one package audio asset is currently known to decode as ATRAC9. Music, dialogue and
effects may have different containers/configurations. Keep the codec inventory open until
representative assets from each class are verified. Never use snd0 as the gameplay soundtrack.

## Stage 4 — save serializer and persistence recovery

Follow imported SaveData open/mount/read/write/unmount/memory/dialog calls into the game's own
serializer. Identify slot selection, safe simulation boundary, state fields, checksums, schema,
load order, migration and asynchronous completion. Map write buffers back to their producers;
a host SaveData shim only answers where bytes go, not what the game state means.

For each observed save payload record format bounds, file grouping and validation rules.
Native save state must encode logical state/IDs, not host addresses. Distinguish imported legacy
saves from the native save format. Replaying a native save after a new process launch is required;
reading a buffer in the same process does not prove restoration.

## Stage 5 — native renderer boundary recovery

Use GPU imports/callers and original engine data to recover render intent: object/material,
resource binding, pass dependencies, compute work, feedback/readbacks and presentation.
Original packets/shaders may inform offline fixtures and converters. The final engine must
call native render operations directly instead of feeding a retained console command processor.

Inventory shaders/material variants offline with hashes and stage/resource layouts. Cross-check
SoTC-specific decoder fixes (WQM/water, indexed textures, vertex fetch, streaming and readbacks).
For dynamic packet/shader creation, recover the creating game function and replace the intent
with a native operation/variant. An unhandled dynamic path remains a blocker, not runtime emulation.

## Coverage, checkpoints and three-try escalation

Track at least: discovered functions, reviewed functions, native implementations, fixture-tested
functions, actually exercised functions, unresolved calls and known unsupported behavior.
None of those counts alone proves completeness. Static under-discovery and data-driven paths
must stay explicit; do not report a percentage whose denominator is unknown.

At every checkpoint update HANDOFF and preserve a small evidence ledger: observation/inference,
hash/address/profile, exact command, result, logs, limitations and next action. One integrator
merges worker reports. After three failed distinct tries, mark NEEDS_PRINCIPAL and attach
the smallest byte/hash-bound reproducer using the escalation template. No fourth blind retry.

The plan is complete only as a plan. Game boot, native Metal output, real audio and durable
save/load remain the implementation gates in [VALIDATION](VALIDATION.md).
