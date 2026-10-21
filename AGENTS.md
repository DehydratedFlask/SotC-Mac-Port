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

## Public GitHub remote (durable)

- Remote: https://github.com/DehydratedFlask/SotC-Mac-Port (`main`).
- At every checkpoint / completed task claim: commit **allowlisted** changes only, clear message,
  then `git push origin main`. Pull/rebase if others pushed; never force-push.
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
