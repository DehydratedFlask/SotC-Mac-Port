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
[HANDOFF](HANDOFF.md) / [TASKS](docs/TASKS.md)). Constructor coverage is **573/573 registered rows and
573/573 fully lifted** (measured by `tools/lift/g1_evidence_pack.py`, not remembered); slot 554
(`0x6efe0`), the last open row, closed with batch 13 on 2026-11-08 (E1417,
`docs/STARTUP_SLOT_554_PLAN.md`). The recovered set was **not a prefix** (18 rows above the former first
gap were registered early), so registration was never startup-order or semantic coverage — track it, but
do not treat greedy per-callsite Init lifting as G1 progress. With no constructor row left, the next work
is the Init chain (`0x531d60` closure / post-body slices) and the runtime gates, not another slot batch.
First post-constructor slice landed 2026-11-09: `0xa54920` (1,840 B, the largest unmodelled body on
the Init path) is now a native model composed from the `0x3a980` worker, `0x26ea0` item ctor and
deferred `0x272d0` records, with `0xa55050` as an explicit contract (E1419). It adds no constructor
row; 573/573 remains and G1 stays PARTIAL.
The follow-up savedata-family pass (E1420) replaced that contract with a full 347 B `0xa55050`
slot allocator, lifted the reachable 303 B `0xa57c10` sentinel cleanup (documenting its dead
`a57c45..a57cea` traversal), and converted the 367 B `0xa586a0` grow placeholder into a precise
fail-closed contract. It also adds no constructor row. The tree-helper follow-up (E1421) closed that boundary: `0xa57d40` (1,201 B) is a full lift by
delegation to the byte-identical admitted `0x482a90` erase, and `0xa58490` (76 B) is a real
recursive release, so `0xa57c10` no longer releases through a host seam. Its `a57c45..a57cea`
traversal stays dead (no entry from any body), not an unlifted hook. The savedata-grow follow-up
(E1422) then replaced the 367 B `0xa586a0` fail-closed contract with a full block-map model:
0x50 B object, 0x100 B blocks, front/back in-place shifts and the allocate/copy/free path, with
`0x17090`/`0x170c0` reused and the `0xf8f470`/`0xf8f480` imports proven as `memcpy`/`memmove`.
No constructor row changed. The runtime-entry follow-up (E1423) then executed the lifted Init
control-flow window from `init_531d60_entry_gaps` through the `0x532c1c` registry checkpoint:
entry units 17, registry body 64 units, 275 entries, status 0, with the full suite 701/701.
The seam-replacement follow-up (E1424) then executed all three bodies natively: `0xa473b0`
(capture, 1 call), `0xa54920` (save-data/statistics, 3 calls, 9 counters + 9 menu items) and
`0x54b250` (stat-query wrapper, 1 call), leaving `0xa56560` and the platform capture services as
explicit host contracts. The first post-base callee closure (E1425) then executed the already-lifted
193 B `0x499f80` body in that runtime: it receives `self+0x3c70`, commits one 0x50 B object with two
0xa00 B buffers and a live CritSec (1 call, cleanup failures 0), with the allocator dispatch and
CritSec supply still explicit host contracts. The post-base closure (E1426) then executed `0x56dcb0`, `0x5c64c0`, `0x1290` and `0x7f7220`
natively, plus the five one-byte-ret sites (`0x56dca0`/`0x56db70`/`0x5b3d00`/`0x5b3cf0`/`0x56fad0`), and
observed `0xf90310` as a named splash-hide host service. The runtime JSON reports all ten
sites, `post_chain_complete=true`, 275 registry entries and status 0; full CTest **702/702**.
The per-call follow-up (E1427) then made the post-base call surface explicit: the three bodies
emit **17** records (7 native leaves executed, 10 named blockers with observed arguments and
runtime diagnostics), while the anchored run still reaches the 275-entry checkpoint with
`post_chain_complete=true` and `post_calls_complete=false`. Executed are five one-byte `ret`
targets, the 174 B `0x5c91e0` copy and the 14 B `0x5d06f0` query; `0xa5cd90`, `0x946880`,
`0xa5d990`, `0x560b60`, `0x5b9f90`, `0x96f20`/`0x97a50`, `0x5d5410` and `0x7f6f90` remain
unlifted. The post-call checker gate was repaired separately (the first sweep exposed ten stale
`NOT modelled` assertions and a stale entry map); the full **908/0/0** checker sweep and
**2,929 → 1,861 RVA / 576 header** entry map now agree with the executed bodies. This is a
startup-runtime proof, not a game boot and not callee closure; G1 stays
**PARTIAL**. Next leverage is the next runtime gate or a named seam, not another constructor
batch.
The `0x946880` follow-up (E1428) then lifted the smallest blocked chain: `0x946880` (353 B)
and `0x9469f0` (360 B) are native, with explicit hooks for `0x98a450`, `0x986f30`,
`0x987720`, `0x9882e0`, `0x988400`, `0x9556a0`, `0x9878d0`, `0x987260` and `0x984700`.
Both observed values use the invalid-handle `0x9469f0` branch and return 0; the runtime now
reports **9 executed / 8 blocked** post-calls, still with 275 registry entries and status 0.
The valid-handle `0x944de0` (399 B) branch and the remaining eight invocations stay named
blocks; `post_calls_complete=false`. The focused fixture, extended 56dcb0 checker and entry
map agree with the bodies. G1 stays **PARTIAL**.
The `0x5d5410` follow-up (E1429) then executed that group natively: `0x5d5410` (420 B),
`0x8be60` (547 B), `0x8f520` (269 B) and `0x8f1d0` (171 B) configure both 0x30 B objects and
write the nine-qword state block. The real run now reports **10 executed / 7 blocked**
post-calls with 275 registry entries/status 0; the measured path reaches neither the bounded
host allocator (`0x12820`) nor the refusing platform tail (`0x87f3e0 -> 0xf906d0`). Seven
invocations remain named blockers, led by `0x96f20`/`0x97a50` (2,835 B, 10 args, 607
instructions, 55 direct calls). `post_calls_complete=false`; this is not callee closure or a
game boot, and G1 stays **PARTIAL**.
The `0x7f6f90` follow-up (E1430) then lifted the staffroll loader: 656 B, 19 direct call
sites, a 37-token table, stdio plus the four-call fallback host contract, and retained
text buffers so the absolute rest pointers stay valid. Both real `0x7f7220` invocations
execute; mode 0 stores `CENTER_FULL_SCREEN` at index 19 (19 B) and mode 1 stores `TARGET`
at index 36 (7 B), both through the fallback (`fopen=false`). The real run now reports
**12 executed / 5 blocked** post-calls with 275 registry entries/status 0; `post_7f7220`
reports two sets and zero blocked. The remaining five invocations are `0xa5cd90`,
`0xa5d990`, `0x560b60`, `0x5b9f90` and `0x96f20`/`0x97a50`. `post_calls_complete=false`;
this is not callee closure or a game boot, and G1 stays **PARTIAL**.
The `0x560b60`/`0x96f20` follow-up (E1431) then moved two more post-call entries: the
803 B `0x560b60` body executes its object/buffer/global stores and the 41 B `0x96f20`
wrapper returns the host's observed 448-byte `0x97a50` object. The real run now reports
**14 executed / 3 blocked** post-calls with 275 registry entries/status 0; `0x5c64c0.cell`
is non-null. The three remaining blocked records are `0xa5cd90`, `0xa5d990` and
`0x5b9f90`; `0x97a50` and the thirteen `0x560b60` helpers remain explicit host seams, so
`post_calls_complete=false` and this is not callee closure or a game boot. G1 stays
**PARTIAL**.
The 2026-11-12 pass (E1432) first repaired a red handover tree: the native `0xa5cd90` body
passed the selected config-string **original-image RVA** to its `0x994e70` hook as a
`const char*`, so the host's `%s` read address `0x12c4f51` and the Init runtime aborted under
ASan (CTest **701/702**). The hook now takes the RVA as an identity tag. The same pass then
executed the 2,709 B `0xa5d990` body natively: region 1 reuses the existing
`build_sndx_category_vars` loop (32 Mute + 32 Filter vars with the engine-span write-back),
region 2 is the "Unnamed Categories" block proven dead behind a gate byte that is only written
from a zero register, and region 3 builds the two separators and four `Mute All`/`Unmute All`/
`Filter All`/`Unfilter All` command items through the existing `fmenu_ctors_slot306` models,
registers them and destroys them through a caller-owned `0x272d0` hook. The run now reports
**16 executed / 1 blocked** post-calls with 275 registry entries and status 0; only
`0x5b9f90` (46,881 B) remains, so `post_calls_complete=false` still holds and G1 stays
**PARTIAL**. Full CTest **703/703**; two new ELF checkers pin both bodies.
The remaining blocker was then decomposed instead of attempted whole. `0x5b9f90`
(`0x5b9f90..0x5c56b1`, 46,881 B) has exactly one `ret` and one backward branch: it is a
straight-line debug-menu page tree of 1,136 direct calls over 25 callees, 491 of them the
already-modelled menu constructors. It is now mapped into S0–S9 at its four rare callees and its
one loop nest, with S3 split into six pages, and the map is committed
(`docs/5B9F90_SLICES.md`, `docs/5b9f90_slices.json`). **5,412 B execute natively**: S0 (canary +
group "Shadow" under "CP10") and S3.1/S3.2 ("Shadow Boss Jump" and "Shadow Boss Outro Jump",
groups plus 32 command items, targets `0x2fed5b8`/`0x2fed6f8`, commands `"0".."15"`). The two
intervening page builders `0x2170`/`0x5cf140` are named host contracts and are not counted. The
run reports **19 executed / 1 blocked** post-calls (from 16/1) with `post_calls_complete=false`,
resume offset `0x5bb4be` and 41,469 B still refused; 275 entries/status 0. Full CTest
**704/704**, sweep **914/0/0**, G1 **PARTIAL**.
S3 was then finished from that exact resume point. `0x5bb4be..0x5bc3d4` (9,097 B) is now also
native: "Shadow Cheat" (8 toggles, 5 game-mode commands, 4 float ranges with the measured
0.01f/0.1f pair), "Shadow Trophy" (4 guarded bool variables built through `0x20af0`, the bool-class
stores, `__cxa_atexit(0xacf0)` and `0x26db0`) and the "Shadow Render"/"MLAA" groups, plus the
23 `0x272d0` destroys. All six S3 pages execute, so the measured extent is **9,274 of 46,881 B
(19.8 %)**, 60 menu items and 106 releases; the run reports **23 executed / 1 blocked** post-calls
with `post_calls_complete=false` and resume `0x5bc3d4` (S4). The two page builders stay contracts:
`0x2170` is ≥ 28,288 B with no `ret` before `0x9000`, and `0x5cf140` (5,526 B, single `ret`) was
liftable once the tracer learned stack-slot views, xmm arguments and `mem_f32` constants. That
tracer then executed **S2** (`0x5cf140..0x5d06d6`, 5,526 B): "Shadow Credits" with its command,
six separators, the external `cp10_creditsUseNewList` variable, two guarded bool variables and ten
bounded float sliders. The body extent is now **9,279 of 46,881 B (19.8 %)** and the native code
executed by the same run is **14,805 B (31.6 %)**; 8 slice records, 1 contract (`0x2170`).
S4 was then decomposed at callee boundaries instead of being left whole: **S4.1** executes the
`0x5dbf40..0x5ddcad` "Particle" page (7,533 B, 1,243 instructions, 177 calls, single `ret`) with
its 20 byte-pinned items (2 `0xc580` ranged ints, 5 bounded floats, 12 bool variables), and **S5.1**
executes the `0x5bc413..0x5bc78c` "Stereo 3D" page (889 B). S4.2 (`0x7f8930`) stays blocked
on its measured reason. S5.2 then turned out to be liftable with the same page machinery and now
executes `0x5bc78c..0x5c1196` ("Dynamic 3D" + "Shadow Boss Worm Hole Effect", 18,954 B, 3,085
instructions) as 56 byte-pinned items: 2 groups, 9 bool variables, 28 bounded floats, 7 ranged ints,
7 separators and 3 keyframe commands. The body extent is now **29,127 of 46,881 B (62.1 %)** and the
native code executed by the same run is **42,186 B (90.0 %)**; 11 slice records, resume
**`0x5c1196`**, `post_calls_executed` **27**, 1 blocked. Full CTest **704/704**, G1 **PARTIAL**.
Slot **23** (`0xe71990`) is registered via a **private fixture**; consumer/string necessity
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
