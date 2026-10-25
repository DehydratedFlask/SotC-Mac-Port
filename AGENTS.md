# Agent operating instructions

## Read before doing work

1. Read `HANDOFF.md`, `docs/INDEX.md`, `ROADMAP.md`, then your task's linked specification.
2. Check `docs/TASKS.md` and claim a bounded task in `HANDOFF.md` before editing shared files.
3. Use trail before searching source: `trail ask "question" --source --in reference/ShadowOfTheColossus-PC/`.
   Known symbol: `trail grep "symbol"`; before a symbol change: `trail callers symbol --depth all`.
4. Verify live files and pinned commits; indexes, upstream READMEs, and previous agents are evidence, not proof of execution.

## Non-negotiable target

- Native Apple Silicon ARM64 Mach-O game executable and a SwiftUI/AppKit Mac launcher.
- Direct Metal rendering. No Rosetta, Wine, CrossOver, QEMU, Unicorn, guest CPU interpreter/JIT,
  guest-register dispatch runtime, x86 executable loading, or MoltenVK/Vulkan runtime in the final product.
- The finished app must boot the actual game, produce actual game audio, and save/reload progress.
- No runtime dependency on `eboot.bin`, any `libSce*.sprx`, other Sony executable module, or firmware.
- The user explicitly allows local `eboot.bin` offline analysis. Offline lifting is acceptable only
  when the result is ordinary native functions and data; preserving a guest CPU or PS4 execution
  environment under an AOT label does not satisfy the target.
- Original local game assets may be read through a documented asset-only import. Do not embed the
  original executable as a blob or disguise executable sections as assets.
- Do not download game/firmware files, request uploads, or silently substitute a PS2 version.
- Do not claim boot/audio/saving from a window, triangle, tone, mock save, or source/build success.

## Current architecture contracts (round 24–25; do not re-derive)

- **App-info registry.** `*(0x2ee68c8)` is the 0x60 B three-list container built by `0x1bdb10`:
  list A at `+0x00` (0x58 B named entries, dword flag at `+0x50`), list B at `+0x20` (0x50 B,
  no flag), list C at `+0x40` (0x60 B callable nodes). `0x25a4d0(app,registry)` writes four product
  entries; `0xb10(registry)` writes `"Game"`; every observed write uses `flag = 1`.
- **Dispatch and lifecycle.** `0x1bdf30` walks list C **FIFO** (start at `container+0x40`, follow
  `node+0x00`), then serializes every list-A entry (`0x1be14d..0x1be222`) and drains `flag == 1`
  nodes (`0x1be3f4..0x1be456`). `0x1bdeb0` purges `flag == 0`; `0x1bede0` destroys all three lists.
  The only normal gate is the `snapshotBugData` command at `0x25db56`, so dispatch is not per-frame
  and cannot duplicate entries. The old "LIFO" and "flag 0 producer unknown" claims are withdrawn.
- **Native modules.** `app_info_registry`, `app_info_install`, `app_info_collect`,
  `app_info_snapshot`, `appinfo_listb`, `report_tree`, `frame_root`, `registry_owner`, `registry_slot`,
  `container_dtor_1bede0`, `insert_1bdc80`, `owner_properties_2569d0`, `owner_prop_bag`. Their
  headers state what is modelled and what is a hook.
- **Native conventions.** App-info text is a **borrowed view** (`control == nullptr`) unless a module
  explicitly owns it; callers keep it alive. Native list-C delegates are tagged `fn == 0`; nodes with
  non-zero `fn` are identity-only `0x1290` handlers and are skipped, never called. Never execute an
  original RVA.
- **`0x2569d0`** is 12,621 B (`sub rsp,0x1118`, 87 reachable blocks). Only its registry block,
  `RetailBuild`/mode classification and the constant property table are native. `0xac5260`/
  `0xac5320` are `insert_equal` consumers, not tables; map A is `{i32,f32=1,u32}`, map B is
  `{f32 lo,f32 hi,f32 v,u32}`, and 8 keys are inserted twice (the sink must allow duplicates).
  `owner_prop_bag` models the 0x178 B bag with ordered multimaps (E577); the exact RB nodes/fixups
  and trees 0/1 remain contracts. The four 0xc88 objects in `0xa2afb0` are per-user **input
  devices**, not language tables (E590), and do not affect the property path; `owner_bag_construct`
  models the observable sequence with hooks (E592). The 20 constructor
  rows (19 slot rows + 2 self-registered) remain open as ctor hooks in `owner_members` (the 21-row
  driver, E579); the large bodies are blocked subsystems.
- **`0xf590`** is a 73 B shared release. The 17 Init-slice modules and the four provably-zero
  `0x533100` sites (miss `0x5332f4`/`0x533300`, join `0x533399`, tail `0x53404d`; E574) are wired as
  exact inert zero-holder releases. The nine `0x533100` key-holder sites are real releases: the
  0x20 B `SharedStr` ctor `0xf1a0` is native (E581) and `[[0x2ee2cf8]]+0x18` is
  `ResourcePool::GetResource`, not a destructor, so release with `previous ∈ {1,2}`. The key is
  built by `resource_id_ctor_14c2f0` (E582): `****` is patched with the runtime tag before FNV.
  `0xf3c0` resize/COW and `0xf6b0` terminate are native (E587); only the capacity-short path
  returns true. The `"****"` tag is `_cmn` at the nine sites (`0x147e20`, E589), not `_ps4`.
  The nine sites are now wired (E586): each builds the real key and releases it; a host must supply
  `key_alloc.alloc.allocate` in every reachable attach detail, otherwise the fragment reports
  `ok == false`. `platform_tag` defaults to the native `_cmn` table (E589). `0x5333e3` stays a per-element hook. Do not wire P2
  frame/global sites without the `ctrl_slot` + callee-body check; a last reference with a missing
  destroy/free contract must report `ok == false`.
- **`0x15ff060..0x15ff0a0`** is an eight-slot `.data.rel.ro` pointer slice, not a vtable and not an
  owned object. Do not restore the "table owner" reading; `0x2569d0` is a consumer.

## ELF facts (verified)

- The reconstructed ELF has **three** program headers; the older two-header map dropped `phdr 1`
  (`vaddr 0x1594000`, filesz `0x6d190`), which carries `.data.rel.ro` (the vtables). Do not assume
  the RELRO window is absent because it is outside `phdr 0`.
- The camera `D` is a Camera-typed registry object (`is_type<0x3cb0eb33>`, ctor `0x211c00`, 0xb0 B),
  **not** `_Root` (E602). The 24-byte block is at `[D+0x18]+0x2e8`, and the only path that touches it
  is the debug-only `bpe::ToolRemoteCommandMgr` `"SyncCameraAcrossTools"` command. `0xa68030` returns
  `slot->value` from a 65536-entry array; there is no `element+0x18` field. `_Root` vtable `+0x10`
  is a 5-argument pass-through invoked once in the world ctor; it attaches no Camera (E605). The
  pose consumer is debug-tool-only and not gameplay-reachable, so a native `BoundMemberFn` is not a
  gameplay requirement.

## Tool acquisition (owner-approved)

- The owner explicitly allows downloading and using pre-existing tools when a concrete task needs
  them. Prefer already-installed Apple/Xcode tools; add a dependency only when it removes real work.
- Before installing: identify the official source/publisher, prefer signed/notarized or widely used
  releases, record the exact version/commit and SHA-256, and verify the checksum after download.
- Safety check before use: `spctl`/`codesign` where applicable, `file`/`lipo` for architecture, and
  a smoke test in a private temp directory. Do not run an untrusted tool against game files,
  credentials, or private evidence until it has passed.
- Record in `INSTALL_LOG.md`: local date, purpose/task, source URL, version/commit, checksum, install
  path, exact install/run command, safety result, and removal boundary.
- Never install emulators, guest CPU runtimes, Sony SDKs, firmware, game files, or anything that
  reconstructs/executes the original executable. Existing installs are not permission to use them.
- Do not commit downloaded tool binaries. `tools/` holds recipes/adapters and attribution only.

## Checkpoints are mandatory

Update `HANDOFF.md` at EVERY checkpoint: completed task or subtask, passed/failed gate,
meaningful failed experiment, dependency change, blocked state, before context compaction,
before switching agent, and before ending a turn. This applies even when no code changed.

For each checkpoint record: local date/time and agent ID; task ID; files changed; exact command;
exit/result and log/evidence paths; observations versus inference; remaining risks; next exact action;
task status and attempt count. Update `docs/TASKS.md`, `docs/EVIDENCE.md`, and `INSTALL_LOG.md`
when their facts change. Preserve earlier checkpoint records under `docs/checkpoints/` if the
current handoff grows too large; link the archive and keep the current state at the top.

Concurrent workers own different files/tasks. One integrator owns `HANDOFF.md` edits at a time;
workers send checkpoint content to that integrator and verify that it was recorded before exiting.
Do not overwrite another worker's checkpoint or edit the reference clones.

## Verification before publication

Run these from the workspace root and record exact counts in `HANDOFF.md`/`docs/EVIDENCE.md`:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
python3 -I scripts/audit_publish_paths.py --all
python3 -I scripts/check_commit_attribution.py
python3 scripts/project_index.py
```

A green build is not a gate result. Label every claim `NOT RUN`, `FAILED`, `PARTIAL`, `PASSED`,
`FIXTURE_VERIFIED`, `STATIC_ANALYSIS_VERIFIED`, or `ARTIFACT_VERIFIED` as appropriate. Workers do
not run `git commit`, `git tag`, or `git push`; the integrator commits under the owner identity.

## Public GitHub remote (durable)

- Remote: https://github.com/DehydratedFlask/SotC-Mac-Port.
- The owner explicitly directed on 2026-10-25 that ongoing work return to `main`, after merging PR #4
  and deleting `cursor/init-hash-551140-9dfc`. `main` is the active integration branch in
  `HANDOFF.md`; commit and push allowlisted checkpoints there after the required audits. This
  supersedes the earlier draft-branch default. Never force-push or merge other draft PR branches
  without new explicit owner approval.
- At every checkpoint / completed task claim: commit **allowlisted** changes only with a clear
  message, then push `origin main`. Fetch and inspect remote changes before integrating peer work.
- Allow: `native/`, `launcher/`, port docs (`README.md`, `ROADMAP.md`, `HANDOFF.md`, `AGENTS.md`,
  `docs/*` policy/spec), `scripts/` port tooling, `tools/` without game binaries, `.gitignore`.
- Deny (also in `.gitignore`): `Shadow of the Colossus/`, PKG/PSARC/eboot/SELF/ELF/sprx dumps,
  `local/`, `reference/`, `logs/`, secrets, AgentStores, anything that reconstructs or runs the
  game without owning it. Skip huge regenerable `graft/` cache unless explicitly republished.
- Before push: `git status` and `python3 -I scripts/audit_publish_paths.py` (staged), then
  `python3 -I scripts/audit_publish_paths.py --all`. Users need their own game files to run.

## Three-try escalation rule

A try means a distinct documented hypothesis/change plus its targeted check. Count failed tries
per task across agents and sessions; changing agent, prompt, timeout, or model does not reset it.
At the THIRD unsuccessful try, stop that task, mark `NEEDS_PRINCIPAL` in `docs/TASKS.md` and
`HANDOFF.md`, and create `docs/escalations/<task-id>.md` from the template. Include all three
hypotheses, exact changes/commands/logs, expected/actual results, current tree state, binary hashes
and addresses when relevant, and the smallest reproducer. Assign back to the principal agent.
Do not hide a failure with a stub, silence, fake success, emulation, or a weaker acceptance test.
Continue an independent unblocked task if useful. A fourth try requires principal reassessment,
recorded in the escalation; it must not happen automatically.

## Data, tools, and recovery

- `reference/` contains pinned upstream source. Never pull or modify it as part of an implementation.
  Put adaptations/patches in `tools/` or future `native/`; record upstream identity and attribution.
- `Shadow of the Colossus/` is original user input: read-only. Extract to `local/`, never over it.
- `local/`, binaries, extracted assets, Ghidra databases, evidence containing game code, caches,
  and build products are private and excluded from publication. Do not store keys or credentials.
- Use REA and Ghidra for shipped-code claims. Save evidence IDs, source digest, image base,
  tool versions, coverage, and unresolved indirect calls. REA's Ghidra project is ephemeral;
  export evidence before closing and retain a reproducible analysis recipe.
- Record every install/download/build, version/commit, location, command, purpose, and removal
  boundary in `INSTALL_LOG.md`. Existing Unicorn/MoltenVK installs are not permission to use them.
- Run `python3 scripts/project_index.py` after creating/moving files and before handoff.
  Use `trail build --check` for the source graph; document stale or missing scopes explicitly.
- trail currently persists `onlyDirs = [scripts, tools, reference]`. Its check can pass while
  a newly created directory is excluded. Add future native/launcher/tests scopes explicitly
  without dropping the current ones, rebuild, and prove the new file is queryable.
- Workspace root is the public Git repo for SotC-Mac-Port; each `reference/` checkout stays
  separate and gitignored. Never run broad `git add .`, delete user inputs, stage forbidden
  paths, or replace upstream pins implicitly. Add only allowlisted paths, then path-audit.
- For coding choices with materially different approaches, ask the user as required by their
  workspace instructions while continuing independent work. Do not re-ask settled constraints.
- LLM Wiki hub: resolve `~/.config/llm-wiki/config.json`, use `@wiki`, start at
  `topics/projects/_index.md`. Do not promote session material or modify durable memory implicitly.

## Acceptance and honesty

Follow the gates in `ROADMAP.md` and `docs/VALIDATION.md`. Tests must fail on unimplemented
required behavior. Record `NOT RUN`, `FAILED`, `PARTIAL`, or `PASSED` accurately. No completion
claim until cold boot, audible game content, real save/load after relaunch, Metal validation,
and forbidden-runtime/module absence have been demonstrated. Report hardware and scene for FPS.
