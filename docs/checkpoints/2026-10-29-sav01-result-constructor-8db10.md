# SAV-01 nested result constructor `0x8db10` — E952

- Time: 2026-10-29 06:10 CDT (America/Chicago).
- Agent / integrator: `/root`; task `SAV-01-CHUNK-RESULT-CONSTRUCTOR-8DB10-01`.
- Status: `STATIC_ANALYSIS_VERIFIED` within the constructor and direct call-chain boundary; one verifier setup correction; zero failed semantic hypotheses.

## Scope and observations

Read-only Ghidra 12.1.4 analysis of `0x8db10`, reached from `0x4d470`, and the existing `0xa81180 -> 0x4d470` argument setup. The pinned ELF SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; the saved project was reopened with `-noanalysis -readOnly`, at image base 0 and language `x86:LE:64:default`.

On a chunk match, `0xa81180` passes result object, name constant `0x12c6b80`, payload pointer `record + 12`, payload length, and zero flag in `RDI`, `RSI`, `RDX`, `RCX`, and `R8D`. On a miss it passes null pointer, zero length, and zero flag. The 35-byte `0x4d470` wrapper saves only `RDI` before calling `0x8db10`, so the other argument registers pass through unchanged; it then installs outer vtable value `0x1595dc0`.

The 106-byte `0x8db10` body writes the incoming payload length to raw offsets `+0x110` and `+0x118`, payload pointer to `+0x120`, zero to `+0x108`, `(R8D == 1)` to `+0x128`, clears byte `+0x106`, and zeroes 32 bytes at `+0x130..+0x14f`. It constructs a separate subobject at `this + 8` through `0xf8f630` using the name constant and `0xfe`; that helper was not characterized.

These pointer/length/state writes support an empty-position bounded-view layout, but do not prove read semantics, bounds enforcement, or input ownership. The older profile-chunks checkpoint's memory-stream interpretation is plausible, but should be treated as inference pending consumer-method evidence. No runtime or game gate changes.

## Verification

- Ghidra command used the saved project and `-noanalysis -readOnly`; exit 0. Full command and script are recorded in the private report directory.
- `python3 -I local/validation/orch-20261029/SAV-01-CHUNK-RESULT-CONSTRUCTOR-8DB10-01/verify_constructor.py` — exit 0. All instruction bytes in `0xa81180` (238 B), `0x4d470` (35 B), and `0x8db10` (106 B) matched the pinned ELF. All decoded direct control edges matched references. The four chain edges were byte-verified.
- Exact Ghidra command: `env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless local/ghidra/persistent-20261020 SotC -process eboot.elf -noanalysis -readOnly -scriptPath local/validation/orch-20261029/SAV-01-CHUNK-RESULT-CONSTRUCTOR-8DB10-01/attempt-01 -postScript ChunkResultConstructor8DB10.java > local/validation/orch-20261029/SAV-01-CHUNK-RESULT-CONSTRUCTOR-8DB10-01/attempt-01/ghidra.log 2>&1` — exit 0.
- The verifier's first run had an incomplete expected-call list for the full reader body; it was corrected to include all six direct calls and replayed successfully. No semantic conclusion changed.
- Exact report, scripts, logs, verifier output, and checksums are private under `local/validation/orch-20261029/SAV-01-CHUNK-RESULT-CONSTRUCTOR-8DB10-01/`.

## Remaining work

- No native lift or wiring. The helper `0xf8f630`, object-field names/ownership, consumer bounds behavior, computed callers, full schema, and authentic save/reload remain unresolved.
- Next bounded action: claim and characterize the direct payload-reading consumer identified in the older profile-chunks checkpoint (`0x8eba0`) against the same pinned ELF, then update the stream interpretation only if the consumer corroborates it.

## E953 publication closeout

- Published in owner-authored commit `6abd3501ff0713fe66e430e43072def652e3b90f`; pushed and fetch-verified on `main`, with a clean worktree.
- CMake configure/build passed; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed; Trail check passed with 10,779 deep-layer nodes pending; project index 43,062/current.
- Staged and explicit committed-path audits passed 6/6; all-path audit passed 3,364/3,364; identity, attribution and diff checks passed. Exact logs are in `local/validation/orch-20261029/SAV-01-CHUNK-RESULT-CONSTRUCTOR-8DB10-01/result-publication/attempt-01/`.
- Successor task `SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01` is claimed as E954 in `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md`; it remains ACTIVE with zero analysis attempts.
