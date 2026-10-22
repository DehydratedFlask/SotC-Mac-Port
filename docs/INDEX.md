# Project documentation index

Workspace: `/Volumes/Extra/SoC Mac Native`. Start with [HANDOFF](../HANDOFF.md),
[AGENTS](../AGENTS.md), and [ROADMAP](../ROADMAP.md). The live files are implementation truth.

## Authoritative navigation

| Document | Purpose / owner |
|---|---|
| [TASKS](TASKS.md) | Dependency-ordered task register and detailed work packets; integrator |
| [EVIDENCE](EVIDENCE.md) | Verified artifact/source/tool facts and unresolved questions; integrator |
| [POINTERS](POINTERS.md) | Address classes, tagged 31-bit fields, evidence labels (CPU-02 partial); integrator |
| [ASSETS](ASSETS.md) | PSARC archive findings and audio-asset leads (DATA-02/AUD-01 partial); integrator |
| [GPU01_PREP](GPU01_PREP.md) | Offline `.csdr`/`.cmat` member-name inventory from manifests (GPU-01 prep); GPU worker |
| [CTOR_CLASSES](CTOR_CLASSES.md) | 573-entry constructor-table classification by reachable-body idiom plus the slot 10–19/24–47 family contract; CPU-01 |
| [INPUTTABLE_551140](INPUTTABLE_551140.md) | Numeric-only deepen of Init-hash 275-entry table (`0x551140`); CPU-01 |
| [INPUTTABLE_551140_NAMES_FIXTURE](INPUTTABLE_551140_NAMES_FIXTURE.md) | Private 275-name fixture recipe for registry fill `0x52a220`; CPU-01 |
| [EXTRACTION](EXTRACTION.md) | Repeatable native extractor, inputs, SELF reconstruction; artifact worker |
| [REVERSE_ENGINEERING](REVERSE_ENGINEERING.md) | Ghidra procedure, symbol manifests, boot/audio/save recovery; RE worker |
| [REFERENCE_AUDIT](REFERENCE_AUDIT.md) | Pinned SoTC/Bloodborne/shadPS4 source anchors; source-audit worker |
| [PLATFORM](PLATFORM.md) | Native architecture and subsystem requirements; native-core owner |
| [VALIDATION](VALIDATION.md) | Measurable gates, failures, real-game proof; validation owner |
| [G1 PARTIAL pack (2026-10-22)](checkpoints/2026-10-22-g1-partial-evidence-pack.md) | Phase C evidence; G1 PARTIAL not PASSED; E404 |
| [App-main Init registry slice (2026-10-22)](checkpoints/2026-10-22-appmain-init-registry-slice.md) | Measured Init-hash edge via tip AllocatorFront; PR5 merge gap; E405; G1 PARTIAL |
| [Init trivial remainder leaves (2026-10-22)](checkpoints/2026-10-22-init-531d60-trivial-leaves.md) | Init remainder beyond fragment; E406; G1 PARTIAL |
| [Base Initialize 0x255130 (2026-10-22)](checkpoints/2026-10-22-base-initialize-255130.md) | Orchestrator + 0x357f0 leaf; E407; G1 PARTIAL |
| [Prologue 0x255180 early (2026-10-22)](checkpoints/2026-10-22-prologue-255180-early.md) | Early fragment + 0x335a0; E408; G1 PARTIAL |
| [Prologue 0x255180 tail (2026-10-22)](checkpoints/2026-10-22-prologue-255180-tail.md) | 0x98a90 / app+0x3b58/+0x3b40; E409; G1 PARTIAL |
| [Ctor 0x3be050 header (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050.md) | Nested 0x48 + hooks; E410; G1 PARTIAL |
| [FOLDERS](FOLDERS.md) | Generated public/source folder inventory; `scripts/project_index.py` |
| [INSTALL_LOG](../INSTALL_LOG.md) | Existing and newly installed tools, downloads and removals |
| [Checkpoint template](checkpoints/TEMPLATE.md) | Required record at each checkpoint |
| [Escalation template](escalations/TEMPLATE.md) | Three-try handback to principal |
| [Function schema](schemas/function.example.json) | Address-to-native-symbol recovery record |

## Folder contracts

| Folder | What belongs here | Index / protection |
|---|---|---|
| Root | Agent rules, current handoff, roadmap, README, dependency log | This index; root initially has no `.git` |
| `docs/` | Reviewable plans/evidence summaries, task status, schemas, checkpoint/escalation records | This file + generated FOLDERS |
| `scripts/` | Small stdlib project/analysis helpers; never production game code | FOLDERS + source docstrings; tests |
| `native/` | Recovered ARM64 C++ (core/), tests/, README with build/test command; no original bytes or guest state | FOLDERS + native/README; Graft scope added |
| `launcher/` | SwiftPM SwiftUI launcher shell (LauncherCore + app + tests); builds go to local/build/launcher | launcher/README; Graft scope added |
| `tools/` | Reproducible tool build adapters and analysis recipes | FOLDERS + INSTALL_LOG |
| `reference/ShadowOfTheColossus-PC/` | SoTC PC source, unchanged pinned checkout | Graft; REFERENCE_AUDIT |
| `reference/bloodborne_pc/` | Bloodborne performance/architecture reference, unchanged | Graft; REFERENCE_AUDIT |
| `reference/shadPS4/` | Service/GPU semantics reference, unchanged; its AGENTS applies there | Graft; REFERENCE_AUDIT |
| `reference/shadPS4Plus-pkg/` | Standalone extractor source, unchanged | Graft + EXTRACTION |
| `Shadow of the Colossus/` | Original base/update/DLC PKGs, metadata, images; READ ONLY | Private inventory; excluded from publication |
| `local/` | Extracted copyrighted data, reconstructed ELF, raw RE evidence, Ghidra state, builds | `local/indexes/folders.json`; excluded |
| `logs/` | Raw install/build/extraction/verification logs | INSTALL_LOG + evidence links; excluded |
| `graft/` | Regenerable source graph; not canonical documentation | `graft check`; excluded |

Every existing subfolder is listed in generated indexes. Build/cache internals and `.git`
administration and Finder metadata are deliberately summarized, not treated as source documentation. Original
game/private analysis folders have a local-only inventory. Do not hand-author thousands of
duplicate source summaries; Graft is the source-level navigation layer.

## Source identities

| Source | Commit | Role |
|---|---|---|
| [AjncSystem/ShadowOfTheColossus-PC](https://github.com/AjncSystem/ShadowOfTheColossus-PC/tree/70bb43859cd7ff181a6d409540b757866184d60d) | `70bb43859cd7ff181a6d409540b757866184d60d` | Main behavior reference |
| [Supermedo/bloodborne_pc](https://github.com/Supermedo/bloodborne_pc/tree/de2e38af055f09f819946951075c6bb119e62ae6) | `de2e38af055f09f819946951075c6bb119e62ae6` | Optimization methodology; inspect subtree licenses |
| [shadps4-emu/shadPS4](https://github.com/shadps4-emu/shadPS4/tree/0ebea15c24253b60f514fc37a348dabc8d0bcb90) | `0ebea15c24253b60f514fc37a348dabc8d0bcb90` | PS4 interface/format reference |
| [AzaharPlus/shadPS4Plus](https://github.com/AzaharPlus/shadPS4Plus/tree/771a8d58aff540448c7cbace7c40a66a5daf010d) | `771a8d58aff540448c7cbace7c40a66a5daf010d` | PKG_EXTRACTOR_1_1 extraction source |

Verify with `git -C reference/<name> rev-parse HEAD` and `git -C reference/<name> status --short`.
Do not fetch or update a pin silently. Uninitialized submodules are not installed dependencies.
Pinned source URLs support auditability; live upstream pages are separately dated evidence.

## Source / binary indexing workflow

1. Source question: `graft ask "specific question" --source --in reference/ShadowOfTheColossus-PC/`.
2. Need all uses: `graft grep "symbol" --in reference/ShadowOfTheColossus-PC/`.
3. Before changes: `graft callers Symbol --depth all`; respect all connected platform files.
4. Ghidra finding: append its original RVA, file offset, artifact SHA, evidence ID, native
   symbol/path, confidence and unresolved edges to the local function manifest.
5. New/moved folders or handoff: `python3 scripts/project_index.py`; then `graft check`.
6. If Graft is stale: rebuild and report status. Current persisted scope recipe is
   `graft build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir native --only-dir launcher --only-dir reference`.
   When first creating native/launcher/tests, append their scopes while retaining all current
   ones. `graft check` only checks configured scopes; prove new files queryable with skeleton/grep.
   Never index extracted binaries, original game archives or generated pseudocode as source.

Future implementation directories (`native/`, `launcher/`, `tests/`) are proposed, not created
as fake completed components. Their first task must add a folder contract and update the indexes.

## Skills/tools to use for each workstream

| Workstream | Available skill / tool | Boundary |
|---|---|---|
| Binary recovery | `reverse-engineer-anything`, REA Ghidra provider / headless Ghidra | One shared MCP binary-session owner; export evidence; static is not runtime proof |
| Source exploration | `graft` CLI / graph tools | Query before source search; map callers before changes; private binary index stays separate |
| Native core/tool utilities | `ponytail`, Apple Clang/CMake/Ninja | Reuse stdlib/native facilities, keep scripts bounded; no guest-state shortcuts |
| Swift launcher | `macos-design-guidelines`, `swiftui-pro:swiftui-pro`, Xcode/Swift tools | Read skills at implementation time; native macOS lifecycle, not an iOS simulator test |
| Native UI validation | `computer-use` by main agent, native REA observations, Xcode/Instruments | Actual native app/Finder behavior; browser-only testing cannot certify Mac UI |
| Independent bounded work | Collaboration subagents / existing agent workflows | Separate ownership, one HANDOFF integrator, preserved failed-try counts |
| Shared project knowledge | `@wiki`, config-resolved Projects topic | Read index first; durable promotion only when explicitly requested |

Other tools may be installed only when a concrete task needs them. Record version/source/path,
command, architecture, purpose and removal boundary in INSTALL_LOG, then verify the live result.
Do not install a Sony SDK, an emulator or a second analysis engine just to avoid the native task.
