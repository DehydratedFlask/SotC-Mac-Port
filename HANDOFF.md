# SoC Mac Native — current handoff

Updated: 2026-10-23 (America/Chicago). Integrator: OpenCode orchestrator (goal resumed by explicit
user command). Branch `cursor/init-hash-551140-9dfc`; round-25 tips `f394fc8` (E569–E573), `1eebca2` (E574), and the commit carrying this file (E575); round-24 tip `9a85650`.
Older rounds, the history-rewrite record and the full pre-tidy handoff are archived verbatim in
[docs/checkpoints/HANDOFF-archive-2026-10-23.md](docs/checkpoints/HANDOFF-archive-2026-10-23.md).
The rounds that follow the archive are in chronological order.

## Where things stand (measured at this tip)

- Full native suite: **355/355** (`ctest --test-dir local/build/orch-r22`), ASan+UBSan, Mach-O arm64.
- ELF checker sweep: **452 passed / 0 skipped / 0 failed** with
  `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`.
- Init prefix `0x531d60..0x532b0c` (the full body ends at `0x532c8b`): 100% of prefix bytes claimed by
  native modules (`tools/lift/report_init_coverage.py`), with the recorded gaps and limits. **Claimed
  bytes are not semantics** — most of it is call-site pinning with callee hooks.
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

## 2026-10-23 Orchestrator round 25 — list A reader/drain, list B, owner rows/properties (E569–E572)

- Agent: integrator plus five workers (list reader, camera chain, owner rows, Init f590 remainder,
  0x533100 f590 sites).
- **E569** — the list-A reader is the second half of `0x1bdf30` (`0x1be14d..0x1be222`); it serializes
  every entry with no lookup, then drains `flag == 1` nodes (`0x1be3f4..0x1be456`). The gate is the
  `snapshotBugData` command, so dispatch is not per-frame and re-entry cannot duplicate. `0x1bdeb0`
  handles `flag == 0` (none produced). Native `app_info_collect` models the serialization/drain;
  checker `check_fapp_info_collect.py` pins the bytes.
- **E570** — `0x2569d0` non-registry rows: 20 `0x17070` sites = 18 slot stores + 2 self-registered +
  1 factory; three destroy idioms; gate = **RetailBuild**; mode provably **1** (mode-2 block
  unreachable); 259 constant property rows (map A `{i,f=1,tag}`, map B `{pair,v,tag}`), 139
  reachable. Native `owner_properties_2569d0` + 260-row generated table + `classify_owner_mode` and
  `apply_owner_properties`; checker pins the gate/mode/record bytes and table hash.
- **E571** — list-B inserter `0x1bdc80` lifted (417 B, `0x50` nodes, container+0x20 sentinel, count
  +0x30, four `0xf590` hand-offs); `container_dtor_1bede0` now uses the 0x50 `Node1bdc80` for list B.
- **E572** — round verification: **349/349 ctest**, sweep **446 passed / 0 skipped / 0 failed**,
  publish/attribution audits exit 0.
- **E573** — camera chain characterized; pure `0x50ba0` Euler math and the six-double `setCameraPos`
  formatter are native (`camera_location_25a4d0`). The live pose remains blocked on engine state.
- **E574** — the four provably-zero `0xf590` sites in app vslot `0x533100` now run the lifted
  `shared_release_f590` with real holders (`"r_fps"` ×2, `"Position"` ×2). The nine rank-2
  key-holder sites remain real refcount releases and are untouched; `0x5333e3` stays a per-element
  hook. Mutation checks fail on a non-null control and on a skipped release.
- **E575** — list B is the screenshot-attachment channel and is **serialized, not counted**: the
  reader (`0x1be2f7`, 239 B) walks FIFO and emits one `"Attachment"` node plus a kind-4 value child
  per entry, copying value and name. The clear (`0x1be458`, 69 B) releases value then name, frees
  with tag 2, and zeroes both links and the 8-byte count. Native `report_tree` + `appinfo_listb`;
  list-B fields renamed to the recovered next/prev order and the node allocation corrected to the
  `0x17090` flags-2 wrapper. Checkers 447/0/0, ctest 350/350.
- **E576** — `0xac5260`/`0xac5320` are `insert_equal` consumers, not tables: 236 + 23 unrolled
  call sites, no loop, duplicate keys allowed (8 keys appear twice). Map A is
  `{i32 i, f32 f=1.0, u32 tag}`; map B is `{f32 lo, f32 hi, f32 v, u32 tag}` — the old "u64 pair"
  reading was a float range. The sink/table now carry `lo/hi`; the fixture pins 122 A + 12 B
  inserts, 114 distinct A keys and two `0x7b` rows. The bag and insert bodies remain contracts.
- **E577** — the bag is now a native model: `OwnerPropBag` (0x178 B factory/base ctor, four
  sentinels, `*(0x3305568)` republish) uses ordered multimaps so `insert_equal` duplicates survive,
  and `OwnerPropBagSink` lets `apply_owner_properties` populate it. Gate 1 yields 122 A + 12 B
  (114 distinct A keys, two `0x7b` rows); gate 0 yields 125 + 12. The exact RB nodes/fixups and
  trees 0/1 remain contracts.
- **E578** — the full `0x1bdf30` report path is composed natively in the recovered order:
  list-A `"FieldEntry"` serialization, list-B `"Attachment"` serialization, list-A `flag==1`
  drain, list-B release/free/reset. `drain_app_info` is public; `app_info_snapshot` owns the
  composition. The fixture covers order, counts, the flag-0 survivor, deep copy and both failure
  paths. The `snapshotBugData` gate and root adoption remain contracts.
- **E579** — the 21 `0x2569d0` install rows are now a native driver: 19 slot stores + 2
  self-registered, the three destroy idioms (virtual dtor, dtor+free, row 7 no replace), row 3/17/18
  arg dependencies, and visible stops for a missing ctor/allocator/dtor. All 18 non-trivial ctor
  bodies remain hooks.
- **E580** — the three trivial `0x2569d0` ctors are native: `0xa1cc0` (no-op), `0xfafb0`
  (0x82028-byte layout) and `0x26f820` (base/vptr/+0x18/sub-object). The guest vptrs are host
  values, never executed.
- **E581** — the 0x20 B `SharedStr` ctor `0xf1a0` is native (align4 capacity, data tag 0, control
  tag 1, `*control=1`), and the key blocker is resolved: `[[0x2ee2cf8]]+0x18` is
  `ResourcePool::GetResource`, not a destructor, so the nine `0x533100` sites need a real release
  with `previous ∈ {1,2}`. The existing `shared_release_f590` is unchanged; the nine sites are the
  next wiring step.

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
- The owner allows downloading/using pre-existing tools for concrete tasks. Follow the
  `AGENTS.md` tool-acquisition policy: official source, checksum, signature/architecture smoke test,
  and an `INSTALL_LOG.md` record. Never install guest runtimes, Sony SDKs, firmware or game files.

## Next exact work

1. Continue the `0x2569d0` lift beyond the registry/property slices: implement the
   `init_owner_members` driver (19 slot rows + 2 self-registered; three destroy idioms) and the
   simple ctors `0xa1cc0`/`0xfafb0`/`0x26f820`/`0xa2b960`, then the `[this+0x48]`/`[this+0x58]`
   bodies (`0xb111e0`/`0xb11210`). The map sink (`0xac5260`/`0xac5320`) is the next dependency for
   `apply_owner_properties` (E570).
2. Finish the registry lifecycle: model list B's reader/clear (`0x1be2f7..0x1be49d`) and the
   report/FieldEntry construction (`0xab1b20`, `0xab0d40`). List A serialization/drain is native
   (E569); list B's 0x50 B inserter is native (E571).
3. Make the camera value native: characterize `0x1c70b0`/`0x1c0f40`/`0x216480`/`0x50ba0` and the
   `[owner+0x2668]` handle, then replace the host-supplied `camera_location` text.
4. Wire the safe `0x533100` `f590` sites only after the extended `+0x18` dominance check; the 17 P1
   Init modules are done and Init has no remaining `f590` site (E565 follow-up).
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

