# Historical planning checkpoints — 2026-10-19

This preserves the earlier in-progress states. Current authority is ../../HANDOFF.md.

# SoC Mac Native — current agent handoff

Updated: 2026-10-19 (America/Chicago). Principal: this chat's root agent.
Current checkpoint: planning and local artifact investigation in progress.

## Start here

Read [AGENTS.md](../../AGENTS.md), [project index](../../docs/INDEX.md), and [roadmap](../../ROADMAP.md).
Every agent MUST update this handoff at every checkpoint and stop/escalate a task after three
failed tries. See AGENTS.md for the required checkpoint and escalation fields.

## Settled requirements

ARM64 native game, direct Metal, Swift native Mac launcher, actual boot/audio/save/load, zero
emulation, no runtime eboot.bin or Sony modules. User confirmed offline eboot analysis is allowed
and every project folder must be documented/indexed. Do not request another upload.

## Verified current facts

- Root: `/Volumes/Extra/SoC Mac Native`; initially no root `.git`.
- Source references already present: ShadowOfTheColossus-PC, bloodborne_pc, shadPS4.
- Local inputs are PS4 PKGs: base CUSA08809 (13,567,262,720 bytes), v1.01 update
  (16,842,752 bytes), four DLCs (1,048,576 bytes each), metadata and images.
- Xcode 27.1 (27A9275), `xcrun --find metal` succeeds.
- REA 6.7.0 is connected. Its Ghidra 12.1.4 ARM64 provider is available with Java 25.0.4.1.
- REA `open_binary` rejects PS4 PKG containers as unsupported. This is NOT an extraction result.
- PC source executes original x86-64 entry and module initializers; its NGS2 fallback is silence.
  Native game-code recovery and real audio replacement are critical-path work.

## Current work ownership

- Principal: PKG extraction, REA/Ghidra seed evidence, roadmap/handoff/index/install record.
- `reference_audit`: completed `docs/REFERENCE_AUDIT.md`; returned to integrator.
- `native_plan_review`: owns only `docs/PLATFORM.md` and `docs/VALIDATION.md`; integrator
  records its DOC-PLATFORM / DOC-VALIDATION checkpoints here.

## Checkpoint 2026-10-19 — setup investigation

Observed: upstream source pins and old install log exist. The user suggested ShadPS4Plus
extractor. Its PKG_EXTRACTOR_1_1 release has Windows/Linux assets; source cloned separately at
`reference/shadPS4Plus-pkg`, commit `771a8d58aff540448c7cbace7c40a66a5daf010d`.
Crypto++ ARM64 build dependency installation is in progress; log
`logs/brew-cryptopp-2026-10-19.log`. No game execution has occurred.

Next: finish an out-of-tree ARM64 extractor build, extract into private `local/`, identify the
effective v1.01 executable, save its hash/format, then run REA/Ghidra on it. If extraction or
analysis fails, retain exact errors and mark the corresponding gate unresolved.

## Checkpoint 2026-10-19 — extractor build attempt 1

Task SETUP-01: Crypto++ installation passed. Configure passed. First native build failed because
upstream uses Linux `fseeko64`, absent on Darwin; see `logs/pkg-extractor-build.log`.
Distinct failed tries: 1. Out-of-tree CMake adapter now maps `fseeko64` to Darwin's 64-bit
`fseeko`; no upstream source modified. Next: rebuild and validate an actual PKG extraction.

## Checkpoint 2026-10-19 00:26 CDT — source audit integrated

DOC-REFERENCE-AUDIT, agent `/root/reference_audit`: DONE_DOC, failed tries 0.
Changed only `docs/REFERENCE_AUDIT.md` (253 lines). Worker used read-only Python validation
of line bound, reference-label resolution and 23 immutable source URL paths/ranges: PASSED.
Exact source observations include direct x86 execution, zero-filled NGS2 fallback, x86
fibers/TLS, packed pointers, save-service limits and SoTC regressions from copied Bloodborne
GPU scheduling. No game or build verification is implied. Linked from docs/INDEX and ROADMAP.
Next: integrator cross-checks docs, completes task/evidence registers and generated indexes.

## Checkpoint 2026-10-19 — plaintext executable recovered

SETUP-01 attempt 2 PASSED: native ARM64 extractor built, update extracted to a separate
`local/extracted/CUSA08809-patch` (no eboot in update). Base extraction still runs separately.
`scripts/unwrap_self.py` reused pinned `prepare.py::parse_self` successfully on base eboot.
SELF SHA256 `21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0`;
ELF SHA256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`;
entry RVA `0xf120`, 11 program headers, machine 62 (x86-64), type `0xfe10`.
Required segments admitted by the reader; segment 10 remains missing and needs explicit
classification. Identity JSON: `local/re/eboot.identity.json`; log: `logs/unwrap-self.log`.
REA opened this ELF with Ghidra; default auto-analysis/overview is currently running.
Next: retain overview evidence, inspect entry function and relevant strings/call edges, export
evidence. None of this executes the game or proves the port can boot.

## Checkpoint 2026-10-19 00:27:18 CDT — platform and validation specs integrated

DOC-PLATFORM and DOC-VALIDATION, agent `/root/native_plan_review`: DONE_DOC, failed tries 0.
Changed only `docs/PLATFORM.md` (221 lines) and `docs/VALIDATION.md` (228 lines). Worker ran
a read-only Python audit of line bounds, defined references, whitespace, escalation terms,
gate rows G0-G6 and Metal diagnostic variables: PASSED (tool chunk `944c01`). No runtime
checks or installs. Specs prohibit runtime PM4/GCN processing and guest CPU state under AOT;
define ordinary native functions, direct Metal, real audio/save behavior and Swift UI.
Linked from ROADMAP/docs/INDEX; task statuses recorded. Gameplay gates remain NOT RUN.
Next: principal review/integration, artifact evidence and final folder/source index validation.

## Checkpoint 2026-10-19 — Ghidra MCP startup limit

RE-01 default MCP analysis failed to complete within REA's 330,000 ms startup deadline.
The provider deleted its ephemeral project after timeout; no overview/function result exists
from that attempt. Error and diagnostic tail retained in `local/re/evidence/rea-mcp-timeout.json`.
Ghidra imported x86:LE:64:default:gcc and reported stripped dynamic-table interpretation,
function overlaps and unresolved instruction/decompile warnings. Do not claim complete analysis.
Next: use the same installed REA public CLI with a process-local longer startup limit and larger
heap; preserve a snapshot/evidence file. This does not change MCP/client configuration.
Failed attempt count: 1 (startup resource limit); raising a limit is not a new semantic hypothesis.
