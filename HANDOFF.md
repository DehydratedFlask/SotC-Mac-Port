# SoC Mac Native — current handoff

Updated: 2026-10-23 (America/Chicago). Integrator: OpenCode orchestrator (goal resumed by explicit
user command). Branch `cursor/init-hash-551140-9dfc`; published tip `2e4dfc1`.
Older rounds, the history-rewrite record and the full pre-tidy handoff are archived verbatim in
[docs/checkpoints/HANDOFF-archive-2026-10-23.md](docs/checkpoints/HANDOFF-archive-2026-10-23.md).
The rounds that follow the archive are in chronological order.

## Where things stand (measured at this tip)

- Full native suite: **339/339** (`ctest --test-dir local/build/orch-r22`), ASan+UBSan, Mach-O arm64.
- ELF checker sweep: **419 passed / 20 skipped (need a build dir) / 0 failed**
  (`tools/lift/run_all_checkers.sh`).
- Init `0x531d60..0x532b0c`: 100% of bytes claimed by native modules
  (`tools/lift/report_init_coverage.py`), with the recorded gaps and limits. **Claimed bytes are not
  semantics** — most of it is call-site pinning with callee hooks.
- Publish gate: `python3 -I scripts/audit_publish_paths.py` and
  `python3 -I scripts/check_commit_attribution.py` both exit 0 at this tip.
- **G1 PARTIAL. G2–G6 NOT RUN.** No boot, no game audio, no save/load, no Metal frame.

## Housekeeping 2026-10-23 (no code change)

- Removed 4 stale git worktrees. The two that held unpushed work were preserved first on
  `origin/cursor/parallel-foundations-86ac55b` (detached tip) and
  `origin/cursor/parallel-foundations-wip-20261023` (10 uncommitted files).
- Pruned 242 stale `local/build/*` trees, keeping `native` (several `tools/lift/check_*.py` pin it)
  and `orch-r22` (current suite); ~9.5 GB reclaimed. Re-verified after pruning: 339/339 and 419/0.
- Deleted 10 root `0x*.s` incbin scratch stubs, stray `.DS_Store` files, and a 0-byte
  `/Volumes/Extra/SoC`.

## 2026-10-23 Orchestrator round 21 — 0x1bdeb0 blocker resolved (E558)

- Agent: integrator (goal resumed by explicit user command).
- Task: `CPU-01-LIST-DTOR-1BDEB0-RESOLVE-01` (E558).
- Observation: `0x1bdeb0` has exactly one caller, `0x25dc76`, which passes `*(0x2ee68c8)` — the same
  singleton `0x1290` uses as the A-insert owner. `0x1bdb50` is that list's inserter and allocates
  **0x58-byte nodes**, writing a dword flag at `+0x50` and two 0x20 B callables at `+0x10`/`+0x30`.
  The container therefore holds two node types (0x58 flag nodes and 0x60 callable nodes), which is
  exactly why the `+0x50` test looked contradictory.
- Correction: E557's "count is seeded at construction" is **withdrawn** — 33 indirect
  `lock incq (%reg)` acquire sites exist (including `0x1bdb9d`/`0x1bdbce` in `0x1bdb50`); only the
  narrower "`0x14ccb0` is not the add side" claim stands.
- Next exact action: lift `0x1bdeb0` over an explicitly typed 0x58 node (prev `+0x00`, next `+0x08`,
  callables `+0x10`/`+0x30`, dword flag `+0x50`), releasing via `shared_release_f590` and freeing
  via tag 2, with mixed-flag fixtures.

## 2026-10-23 Orchestrator round 22 — 0x1bdb50 inserter lifted (E560)

- Agent: integrator (goal resumed by explicit user command).
- Task: `CPU-01-INSERT-1BDB50-NATIVE-01`.
- Observation: `0x1bdb50` is 304 B (`add72de8…`), signature sentinel/A/B/flag. It acquires each live
  control block, allocates 0x58 through `0x17090`, stores the callables at `+0x10`/`+0x30` and the
  flag at `+0x50`, links, `incq [sentinel+0x10]`, then **zeroes and releases both temporaries**.
- Key detail: those releases are no-ops by construction (zeroed holder + null-control early return),
  so exactly one reference survives per callable — the acquire is transferred to the node.
- Second detail: the original increments the count 64-bit and the destructor decrements 32-bit; equal
  below 2^32, now documented instead of glossed.
- Verification: 339/339 ctest, checker OK, sweep 419 passed / 0 failed.
- Next exact action: find the caller of `0x1bdb50` (same `E8` scan shape as E558) and determine
  whether it sits inside `0x2569d0`, the owner installer that still gates real singletons.

## 2026-10-23 Orchestrator round 23 — the 0x58 list is an application info registry (E561)

- Agent: integrator.
- Task: `CPU-01-REGISTRY-SEMANTICS-01`.
- Observation: `0x1bdb50` has five call sites (`0xb72`, `0x25a538`, `0x25a604`, `0x25a68a`,
  `0x25a88f`), all passing string views and `ecx = 1`. The strings name the registry:
  `"Game"` → `"Shadow of the Colossus"`, `"Build Version"`, `"Platform Type"` →
  `"ps4 - NEO"`/`"ps4 - STA"`, `"Is Debug Memory Available"` → `"True"`/`"False"`.
- Container: `*(0x2ee68c8)`; its installer `0x25a4d0` has no direct caller and is installed through
  the function-pointer slot `0x15ff080`, the fifth entry of an eight-slot table.
- Negative: `0xa54920` never references `0x2ee68c8`, so this registry is **not** the ConVar path.
- Flag: all observed inserts pass 1; no writer of 0 found, so the destructor's `flag == 0` purge
  targets a state not yet located. Headers updated to say that instead of "unknown".
- Next exact action: identify the owner of the `0x15ff060..0x15ff0a0` table, which is what constructs
  `0x2ee68c8`.
## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.
- The shared descriptor helper models the 34-slot family; slots 24–47 remain in a test-only ahead
  table outside `recovered_constructors()` until production reaches and resolves slot 23.

## Next exact work

1. Identify the owner of the `0x15ff060..0x15ff0a0` function-pointer table — the object that
   constructs `0x2ee68c8` (E561). The app-info registry cannot run natively until its container
   exists.
2. Lift `0x2569d0` (owner installer, 0x1118-byte frame) once its frame layout is mapped, so the
   `0x2ee68c8` / `0x2dfcbc8` singletons exist natively.
3. Wire `shared_release_f590` into the modules still taking `call_f590` hooks — their signatures
   need a holder parameter before they can call the lifted release.
4. `0xa56560` (stat engine) and `0xa54920` (ConVar registrar) remain contracts; do not claim them.
5. Round end: fresh build, full `ctest`, `tools/lift/run_all_checkers.sh`,
   `python3 -I scripts/audit_publish_paths.py --all`,
   `python3 -I scripts/check_commit_attribution.py`.

## Parallel-agent state

- No workers are running; all four stale worktrees were removed after their work was pushed.
- Preserved but **not integrated**: `origin/cursor/parallel-foundations-86ac55b` and
  `origin/cursor/parallel-foundations-wip-20261023` (shared_str_release_f590, save_stream, aud02
  rackquery scripts, check_ff590).
- `local/build/native` is pinned by several checkers and is stale (313 tests); keep it.
  `local/build/orch-r22` is the current full-suite tree.
- The persistent Ghidra project `local/ghidra/persistent-20261020` must never be re-imported or
  overwritten while in use; read-only `-noanalysis` reopening is the supported path.

## Private evidence pointers

- Family manifest: `local/re/work/ctor-classes.json` (per-slot span, digest, calls, class).
- Consumer packet: `local/re/evidence/cpu01-ctor10-descriptor-consumer-01/`
  (`report.md`, `verify.py`, `verification.json` 45/45, `ghidra-*.txt`, `SHA256SUMS`).
- Reopen log: `local/checkpoints/root-20261020-reopen-console.log`.

