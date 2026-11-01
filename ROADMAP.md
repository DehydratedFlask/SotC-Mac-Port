# Shadow of the Colossus — native Apple Silicon / Metal roadmap

Baseline: 2026-10-19, MacBook Pro M4 Pro (12 CPU cores, 48 GB), macOS 27.0.1,
Xcode 27.1 (27A9275). This deliverable is an evidence-backed implementation plan, not a
completed port. The game has not yet been booted on this Mac.

## What success means

A SwiftUI/AppKit launcher starts an ARM64 Mach-O game core. The core renders through Metal,
plays the actual game's music/dialogue/effects, accepts input, and saves/reloads meaningful
progress after process exit. It uses the locally owned game's assets but never loads its
original executable or Sony executable modules. No guest CPU, PS4 GPU execution environment,
Rosetta, Wine, JIT/interpreter, Unicorn, QEMU, or Vulkan/MoltenVK runtime is accepted.

The user explicitly approved offline analysis of local eboot.bin and requested documentation
and indexing for every project folder. Keep these constraints across all agents and sessions.

## The critical finding

The [SoTC reference](https://github.com/AjncSystem/ShadowOfTheColossus-PC) and
[Bloodborne reference](https://github.com/Supermedo/bloodborne_pc) directly execute PS4 x86-64
machine code on an x86 PC and translate graphics into Vulkan. Their architecture does not
supply native ARM64 game logic. The SoTC fallback NGS2 implementation renders zeros. A
Metal backend or Swift launcher alone cannot produce the requested game.

This requires recovering/recompiling game and used middleware semantics into ordinary native
functions, replacing platform services and audio processing, and building a native rendering
boundary. There is no demonstrated automated full-game conversion pipeline here. Treat CPU
recovery, used NGS2 behavior, and graphics wave semantics as feasibility gates, not chores
with assumed solutions. Exact pinned-source evidence is in [REFERENCE_AUDIT](docs/REFERENCE_AUDIT.md).

## Prior art on Apple Silicon (assessed 2026-11-01)

Three public Mac ports were reviewed against this repository's constraints; the full assessment
with paths, licences and per-component verdicts is in [PRIOR_ART](docs/PRIOR_ART.md).

- **bmy/bbport-mac** (GPL-2.0-or-later) runs the game's x86-64 code under **Rosetta 2** and still
  bundles a Vulkan runtime. Its CPU work is the relevant part: a **static recompiler** pipeline
  (plaintext SELF → ELF → FDE/CFG → LLVM IR → x86-64 **and AArch64** objects) is documented with
  verification numbers (308,420 comparisons, 0 mismatches; 46,340 under QEMU for arm64), but the
  `recompiler/` source is **not published** and covers only 140 memory-free integer leaves. Its
  own `gpu/cp/` PM4 decoder and shadow self-test are the best module boundary in the set.
- **Wooloomooloo2/bloodborne_mac_port** (GPL-2.0-or-later) also uses Rosetta for the CPU, but ships
  a **direct Metal renderer** (`renderer_metal/`, 60 files) for PS4 command streams with **no
  Vulkan** at run time (removed in 0.5.3), a pipeline archive with background shader compilation,
  and a classified map of the **327** HLE functions it registers (only 77 imported by that game).
- **truwa/nathan-drake-collection-macos** is a signed, notarised Swift launcher around a third-party
  PC port. It has **no licence file**, so its code is unusable; only its packaging chain is a
  reference.

Decisions taken from this:

1. **Execution model unchanged.** Rosetta/guest-CPU execution remains forbidden, so our
   byte-checked native lifting is the only compliant route; the Bloodborne ports show the GPU and
   service boundary, not the CPU, is what dominates their engineering.
2. **Adopt the lifting method, not the code.** Add whole-program CFG closure and a guest-address →
   native-entry map to M1, following the published method and its stated gaps.
3. **Port the direct-Metal design.** Our M3 renderer should be our own PS4 command decoder feeding
   Metal encoders with render-pass coalescing and an `MTLBinaryArchive`, plus an offline
   GCN → SPIR-V → MSL step (there is no single packaged GCN → MSL tool) — never a Vulkan runtime.
   First slice landed: `native/platform/pm4_decoder.*`, `test_pm4_decoder`, and an independent
   Python oracle `tools/lift/check_pm4_decoder.py`.
4. **Use the HLE map as a specification** for our native service layer.
5. **Licence decided (2026-11-01): GPL-2.0-or-later.** The owner chose GPL-2.0-or-later for this
   repository ([LICENSE](LICENSE)) so GPL-2.0-or-later prior-art code may be ported or vendored with
   attribution. `docs/ATTRIBUTION.md` records the rules; unlicensed code is still never copied.

## Read by role

| Need | Document |
|---|---|
| Resume work safely | [HANDOFF](HANDOFF.md), [AGENTS](AGENTS.md) |
| Ordered, assignable task cards | [TASKS](docs/TASKS.md) |
| Package extraction / executable identity | [EXTRACTION](docs/EXTRACTION.md) |
| Ghidra recovery method, manifests and boot graph | [REVERSE_ENGINEERING](docs/REVERSE_ENGINEERING.md) |
| ARM64, Metal, audio, saving, Swift design | [PLATFORM](docs/PLATFORM.md) |
| Pass/fail acceptance gates | [VALIDATION](docs/VALIDATION.md) |
| Current evidence and explicit unknowns | [EVIDENCE](docs/EVIDENCE.md) |
| Navigation, source pins, folder ownership | [INDEX](docs/INDEX.md), [FOLDERS](docs/FOLDERS.md) |
| Every dependency/download/build | [INSTALL_LOG](INSTALL_LOG.md) |

## Operating plan (current)

M0–M6 below remain the outer frame. Day-to-day work prioritizes **shared platform contracts**
(`CritSec` `0x3b310`/`0x3b380`, allocator dispatch behind `alloc_wrappers_17070`) that unblock many
lifts, then **leverage-ranked** native bodies (see `CPU-01-INIT-531D60-HOOK-INVENTORY-01` in
[HANDOFF](HANDOFF.md) / [TASKS](docs/TASKS.md)). Constructor coverage remains **449/573** with first
gap index **309** / `0xa505e0` — track it, but do not treat greedy per-callsite Init lifting as G1
progress. Slot **23** (`0xe71990`) is registered via a **private fixture**; consumer/string necessity
remains unresolved (G1 caveat). Operating plan lives in [HANDOFF](HANDOFF.md) only. Prefer
session-sized code + checker acceptance over characterization-only docs churn.

Since the 2026-11-01 prior-art assessment ([PRIOR_ART](docs/PRIOR_ART.md)) the plan adds three
leverage items on top of unit lifting: (a) **whole-program CFG closure + guest-address map** so
lifted functions become a callable program rather than isolated units (`CPU-01-AOT-CFG-SCALE-01`);
(b) a **PS4 command-stream → Metal** decoder/encoder of our own with an `MTLBinaryArchive`
(`GPU-01-PM4-TRANSLATOR-01`, `GPU-01-MSL-OFFLINE-01`); (c) a **native service map** derived from the
PS4 services a real game imports (`PLAT-01-SERVICE-MAP-01`). Vendoring GPL-2.0 prior-art code stays
blocked on an explicit owner licence decision.

## Milestones in dependency order

| Milestone | Required deliverable | Exit gate | Parallel work allowed |
|---|---|---|---|
| M0: reproducible inputs | Base/update/DLC inventory; plaintext ELF identity; repeatable extraction and REA evidence | G0 | Environment/tool docs |
| M1: native-code feasibility | Recovered startup slice with native calls/data, relocations and required service contracts; no original CPU execution. Whole-program CFG closure and a guest-address → native-entry map make the lifted units callable as a program | G1 | Asset/audio format inventory, Swift shell, Metal fixtures |
| M2: working native foundations | Job/TLS/fiber semantics, checked pointer schemas, assets, save transactions, decoded/mixed audio fixtures. Native service map derived from the PS4 services the game actually imports | G2 | Independent services, decoding, render fixtures |
| M3: native graphics slice | Offline material/shader manifests; our own PS4 command-stream decoder feeding Metal encoders (render-pass coalescing, `MTLBinaryArchive`, background shader compile), plus an offline GCN → MSL step; direct Metal frame from a real recovered scene path; validated resource ordering | G3 | Launcher, audio event integration |
| M4: native launcher | Finder-launchable Swift UI with actual core lifecycle and honest readiness/error states | G4 | Input and packaging checks |
| M5: actual game vertical slice | Original title/menu to controllable world; authentic audio events; save, quit, reopen, load | G5 | Scene coverage after shared semantics stabilized |
| M6: completion / performance | Gameplay coverage, all required effects/codecs/saves, robust restart; signed staging audit and measured optimization | G6 | Profile-driven optimizations |

Gates are defined in [VALIDATION](docs/VALIDATION.md). A synthetic fixture passing does not
promote a real-game gate. Every milestone retains separate implemented / build / fixture /
real-game evidence. No frame-rate or completion-date promise is justified at this stage.

## First executable work sequence

1. Finish extraction and record hashes, file counts, base/update SFO identities, archive index,
   and module list. Base and update stay separate. Never assume updating means replacing eboot.
2. Reconstruct the plaintext SELF with the pinned `prepare.py` reader, retain missing-segment
   declarations, and analyze the unchanged reconstructed ELF using REA's Ghidra provider.
3. Export REA evidence and a reproducible persistent Ghidra recipe. Recover the entry and
   initialization chain, dynamic imports/relocations, native call targets, vtables, TLS and
   global state. Produce function and unresolved-edge manifests.
4. Prove one small nontrivial native recovered function, then a startup call chain. Match its
   behavior against independent input/output fixtures and exact binary semantics. Do not
   start a wholesale pseudocode rewrite before this feasibility experiment works.
5. Implement only services reached by that chain. In parallel, identify PSARC asset readers,
   used audio banks/codecs, save serializer entry points and shaders/materials.
6. Extend to actual scene initialization and one frame, then native input/update, game-driven
   audio, save/relaunch/load. Maintain explicit coverage for every missing subsystem.

## Bloodborne lessons to use carefully

Study its command preparation, pipeline caches, batching, readback precision and A/B profiling.
SoTC's own notes record black boot, stale state/device loss and hangs when particular
Bloodborne synchronization changes were copied. Preserve a correct SoTC baseline, then
change one measured variable. Existing PC FPS reports use different hardware/scenes and are
not evidence of Mac performance. Do not copy an x86 loader, Windows UI, Vulkan dependency,
or unwanted network subsystem into the native target.

## Public remote (durable)

- **GitHub repository:** https://github.com/DehydratedFlask/SotC-Mac-Port
- Display name: SotC Mac Port. This remote holds the Mac native port + launcher + port docs/tooling only.
- Users must supply their own legally obtained game files. No Sony assets ship in the repo.
- At **every** HANDOFF checkpoint / completed task claim: commit allowlisted changes with a clear
  message and push `origin main`. The owner explicitly returned ongoing work to `main` on 2026-10-25
  after merging PR #4 and deleting `cursor/init-hash-551140-9dfc`; this supersedes the earlier
  draft-branch default. Never force-push or merge another draft branch without new explicit owner
  approval; inspect remote changes before integrating peer work.
- Never stage forbidden paths (`Shadow of the Colossus/`, `local/`, `reference/`, `logs/`, game
  PKG/PSARC/eboot/SELF/ELF dumps, secrets). Before push: `git status`, then
  `python3 -I scripts/audit_publish_paths.py` on the staged set and `--all` on tracked files.
- See `.gitignore` and [AGENTS](AGENTS.md) for the full allow/deny lists, and
  [ATTRIBUTION](docs/ATTRIBUTION.md) for the contributor-identity gate.

### Commit attribution — Claude is never a contributor

Repository contributors must be human. On every commit, before pushing:

1. Author and committer must be the project owner identity (the configured local `user.name` /
   `user.email`); never commit as Claude, Anthropic, "Claude Code", or any agent identity.
2. Never add assistant attribution trailers: no `Co-Authored-By: Claude ...`, no
   `Co-authored-by: <assistant>`, no `Generated with [Claude Code]`, no robot/agent sign-off.
   If an assistant or subagent produced the change, its work is attributed to the owner only.
3. Never write Claude/assistant names into `CONTRIBUTORS`, release notes, changelogs, PR bodies,
   or issue text. Do not let a GitHub contributor graph entry be created for an assistant.
4. Audit before every push: `python3 -I scripts/check_commit_attribution.py` must exit 0
   (identity plus every unpublished commit), and
   `git log -1 --format='%an <%ae> | %cn <%ce>'` must show the owner identity. The full policy
   and audit state are in [ATTRIBUTION](docs/ATTRIBUTION.md).
   Fix the message (or `git commit --amend` before pushing an unpushed commit) instead of pushing
   an assistant-attributed commit. Already-pushed history is never rewritten, except an owner-approved attribution cleanup performed
   with `--force-with-lease` on the agreed branch (see the PUB-01 cleanup task).
5. Subagents that edit code are tooling, not authors: they must not run `git commit`, `git tag`,
   or `git push`; the integrator commits under the owner identity.

## Agent execution contract

Worker subagents are spawned on a non-Claude model; the orchestrator keeps the integrator role
(docs, evidence numbers, index regeneration, commits, pushes) and replays every worker claim.

Use one task card with a bounded output, prerequisites, owned files and acceptance check.
Update [HANDOFF](HANDOFF.md) at every checkpoint, including failed experiments and exit.
Record distinct failed tries across agents. At three failures mark `NEEDS_PRINCIPAL`, attach
the [escalation template](docs/escalations/TEMPLATE.md), stop that task, and return it to the
principal. Do not relax the target, fabricate successful imports, or substitute silence.

Shared documents have one integrator. Workers provide evidence and checkpoint records; the
integrator merges them before the worker exits. Refresh folder indexes and Graft, retain
source pins, and leave exact next commands so the next agent can resume without chat history.
After each checkpoint merge, push allowlisted tree changes to the public remote above.
