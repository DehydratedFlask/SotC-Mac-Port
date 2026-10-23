# SoC Mac Native — current handoff

Updated: 2026-10-23 (America/Chicago). Integrator: OpenCode orchestrator (goal resumed by explicit
user command). Branch `cursor/init-hash-551140-9dfc`; published tip `9a85650` (round 24).
Older rounds, the history-rewrite record and the full pre-tidy handoff are archived verbatim in
[docs/checkpoints/HANDOFF-archive-2026-10-23.md](docs/checkpoints/HANDOFF-archive-2026-10-23.md).
The rounds that follow the archive are in chronological order.

## Where things stand (measured at this tip)

- Full native suite: **345/345** (`ctest --test-dir local/build/orch-r22`), ASan+UBSan, Mach-O arm64.
- ELF checker sweep: **442 passed / 0 skipped / 0 failed** with
  `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`.
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
## 2026-10-23 Orchestrator round 24 — app-info registry path (E562–E567)

- Agent: integrator plus six workers (three static-RE, one integration, one f590 census, one f590
  wiring).
- **E562** — the `0x15ff060..0x15ff0a0` window is eight `R_X86_64_RELATIVE` slots in `.data.rel.ro`,
  not a vtable: the base is never taken, nothing writes it, slots 6/7 are data. `0x2569d0` is a
  consumer; it constructs the registry and registers slot 4 (`0x25a4d0`) as a list-3 delegate.
- **E563** — `0x25a4d0` is 1014 B (`12a2d0d7…`): four product entries (Build Version, Platform Type,
  Is Debug Memory Available, Camera Location), all flag 1. `"Game"` is a separate 139 B writer
  `0xb10`. E561's `"ps4 - STA"` and functor claims are corrected; platform is `ps4 - STANDARD`, the
  debug predicate is always false. Byte check also corrects list-3 dispatch to **FIFO**.
- **E564** — native registry path landed: `app_info_registry`, `app_info_install`, `registry_owner`
  (FIFO list-3 dispatch; identity-only `fn!=0` nodes are skipped, never executed) and
  `container_dtor_1bede0` (208 B `632451ff…`, list-C vtable slot 3 = `0x19f0` bare ret). Four new
  fixtures plus `check_fapp_info_install.py` and `check_fcontainer_dtor_1bede0.py`.
- **E565** — 10,168 direct `0xf590` sites classified by holder; the 17 Init hooks are all zeroed
  32 B temporaries, so the real release is an exact no-op there. All 17 modules now call
  `shared_release_f590` with a holder and assert `had_control == false`; mutation removing one lift
  fails the slice test.
- **E566** — preserved parallel-foundations commit `a49fd2a` integrated: exclusive save temp,
  `shared_str_release_f590`, rackquery CFG tool + 20 tests, `check_ff590.py`.
- **E567** — `0x2569d0` mapped: 12,621 B (`4ce6b5d8…`), verified `sub rsp,0x1118`, 87 reachable
  blocks, 369 calls. Its registry block `0x256f98..0x257021` is modelled by `registry_slot`
  (construct replacement → tear down previous → replace); the four callback installs and ordering
  are recorded.
- Verification: **345/345 ctest**, sweep **442/0/0**, publish and attribution audits exit 0.

## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.
- The shared descriptor helper models the 34-slot family; slots 24–47 remain in a test-only ahead
  table outside `recovered_constructors()` until production reaches and resolves slot 23.
- Native app-info entries are **borrowed string views** (`control == nullptr`) by default; callers
  keep the text alive for the registry's lifetime. This is a stated native contract, not a claim
  that the original never owns formatted values.
- The native list-3 dispatcher never calls an original RVA: native delegates are tagged `fn == 0`;
  nodes with non-zero `fn` (the identity-only `0x1290` handlers) are counted and skipped.

## Next exact work

1. Continue the `0x2569d0` lift beyond the registry block: model the 19 constructor rows, the
   `[this+0x48]` / `[this+0x58]` call bodies (`0xb111e0`/`0xb11210`), the mode/gate dispatch and the
   `0xac5260` property table as data. The registry slot, container ctor/dtor and list-3 insert are
   already native (E564/E567).
2. Resolve the list-A reader and dispatch timing (E563 unresolved 1–2): no read path for the
   app-info entries has been found, and a second `0x1bdf30` dispatch would duplicate all entries.
3. Make the camera value native: characterize `0x1c70b0`/`0x1c0f40`/`0x216480`/`0x50ba0` and the
   `[owner+0x2668]` handle, then replace the host-supplied `camera_location` text.
4. Convert the 8,194 P2 `0xf590` frame/global sites only after the `ctrl_slot` + callee-body check
   from E565; the 17 P1 modules are done.
5. `0xa56560` (stat engine) and `0xa54920` (ConVar registrar) remain contracts; do not claim them.
6. Round end: fresh build, full `ctest`, `SOTC_NATIVE_BUILD=local/build/orch-r22
   tools/lift/run_all_checkers.sh`, `python3 -I scripts/audit_publish_paths.py --all`,
   `python3 -I scripts/check_commit_attribution.py`.

## Parallel-agent state

- No workers are running. The preserved `origin/cursor/parallel-foundations-wip-20261023` content
  (shared_str_release_f590, save_stream exclusive temp, aud02 rackquery scripts, check_ff590) is now
  **integrated** (E566); `origin/cursor/parallel-foundations-86ac55b` was its base.
- Round 24 used six workers on disjoint files: table owner, installer semantics, 0x2569d0 mapping,
  f590 census, f590 wiring, and the wip integration. No worker touched `HANDOFF.md` or committed.
- `local/build/native` is pinned by several checkers and is stale (313 tests); keep it.
  `local/build/orch-r22` is the current full-suite tree.
- The persistent Ghidra project `local/ghidra/persistent-20261020` must never be re-imported or
  overwritten while in use; read-only `-noanalysis` reopening is the supported path.

## Private evidence pointers

- Family manifest: `local/re/work/ctor-classes.json` (per-slot span, digest, calls, class).
- Consumer packet: `local/re/evidence/cpu01-ctor10-descriptor-consumer-01/`
  (`report.md`, `verify.py`, `verification.json` 45/45, `ghidra-*.txt`, `SHA256SUMS`).
- Round-24 evidence: `local/re/evidence/appinfo-table-owner-15ff060/`,
  `local/re/evidence/appinfo-installer-25a4d0/`, `local/re/evidence/owner-installer-2569d0/`,
  `local/re/evidence/f590-callsite-holders/`, `local/re/evidence/f590-wiring/`,
  `local/re/evidence/parallel-foundations-integration/`.
- Reopen log: `local/checkpoints/root-20261020-reopen-console.log`.

