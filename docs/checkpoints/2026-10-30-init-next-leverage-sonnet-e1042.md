# E1042 — Init next-leverage ranking after E1041 (CPU-01-INIT-NEXT-LEVERAGE-01)

Date: 2026-10-30 CDT. Author: Sonnet 5.5 research delegate (read-only; this file is the only edit).
Status: **STATIC_ANALYSIS_VERIFIED** for the byte/disassembly facts below; source-reading claims are labelled
OBS (observed in repo) or INF (inference). Build, CTest, checkers and publication audits: **NOT RUN** (read-only brief).
Gate: no movement. G1 PARTIAL; `engine_ready=0`; cold boot / audio / save-reload / Metal frame all **NOT RUN**.
Constraints honoured: slot 309 excluded (principal-gated); no engine_ready claim; incremental lifts only.

## Bottom line

1. **The "successor blocks" candidate is mis-framed.** Both successors already exist as lifted, checked modules:
   `0x53299d` = `init_531d60_post_base_continue_d` (+ `continue_c_after_merge`, E535) and `0x5329ae..0x532b0c` =
   `continue_e` (E536). What is wrong is how the **driver composes them**. That is the top task and needs no new RE.
2. `0x21620` is worth lifting (shared, 39 direct callers) but it will **not** remove the `0x91210` vcall hooks,
   and it is blocked on one unanswered question (who registers the real `shadowDemoMode` ConVar). Run it in parallel,
   not first.
3. Cancel the in-flight Composer worker `CPU-01-INIT-53299D-PIN-01`: the span is already pinned
   (`check_f531d60_post_base_continue_d.py`, `..._continue_c_after_merge.py`, `..._continue_c.py` post-end head pin).

## Ordered list

### 1. `CPU-01-INIT-DRIVER-JOIN-01` — fix control-flow composition in `appmain_init_ctor_98a90_slice.cpp` (replaces candidate 2)

OBS (driver lines 530–604; disassembly `local/validation/appmain-agent/init_531d60.dis` 0x53299b–0x532c8c):

- Real CFG: `0x53299b je 0x5329ae`. Taken → `0x5329ae`. Not taken → `0x53299d` (17 B, `call 0xa473b0`) then **falls into
  `0x5329ae`**. So `continue_e` runs on **every** path; `continue_d` only when not taken.
- Driver today: taken → E → remainder (OK). **Not taken or unknown → D → remainder, skipping E** (`...cpp:598–602`).
  The default test run (outcome unknown) therefore never executes E's three `0xa54920` save-data ctors and
  `0x54b250` call. Wrong for the path the default fixture claims to model.
- Alt route (`je 0x5327bd`, driver `...cpp:541–553`): after the alt module the driver does `return out`, so the route
  never reaches C/D/E/remainder. INF (needs one check, see below): `0x1fc80` looks up the config row `"nointro"`;
  null → alt. A retail launch with no such row would take **alt, not B**, so the alt→merge continuation is the likely
  default path and the B route (E1039/E1041 work) is the secondary one.
- Preamble duplication hazard: the alt module models the 36 B preamble `0x5328e0..0x532904` (units 64–69,
  `reached_via_preamble`, `r14b_out`) **and** continue-C models it via `enter_via_5328e0`. When wiring alt→C, enter C at
  `0x532904` with `incoming_r14b = alt.r14b_out`; do **not** set `enter_via_5328e0`, or slot 0 / lazy bind runs twice.
- E's `mov [rax],r12b` (0x532a04) needs the route's r12b: B route `r12b = bl ^ 1`; alt route `alt.r12b_out`.

Bounded split (each step independently testable):
- 1a. D-path continues into E, then remainder. ~10 lines + new CHECKs in `test_appmain_init_ctor_98a90_slice.cpp`
  (E with empty hooks is `ok`, test line 2529, so default expectations survive). Do first; smallest, fixes a visible lie.
- 1b. Alt route continues into C (`incoming_r14b` from alt), then D/E/remainder; thread `r12b`. Also route alt's
  `call [rax]` slot-0 through `run_lazy_bind_91210` like C's four sites.
- 1c. Per-route `r12b` into E.
Checker: no new ELF pins; add a driver-level CFG test asserting unit counts per route
(taken: C,E; not-taken: C,D,E; alt: alt,C,…).

Dependencies: E1041 only. Risk: the 2,900-line slice test; keep default-hook outputs stable.
Value: removes false confidence; first time the default fixture would execute E and the alt route through the merge.
**Does not move a gate.**

### 2. `CPU-01-INIT-21620-LOOKUP-01` — native `0x21620` name→ConCmd lookup (candidate 1)

STATIC_ANALYSIS_VERIFIED (ELF prologue bytes at `0x21620` match the saved `all.dis`; `all.dis` shows **39** direct
`callq 0x21620` sites, indirect not counted; callers include `0x91227`, `0x53327d`, and the `0x1f9d0/0x1fb40/0x1ff40`
family near the config/ConVar helpers):

- Input `rdi → {const char* name, u64 second}`. Output = found object pointer or null.
- Path 1, `*(0x2dfcbc8)` (console manager) non-null: lock `0x3b390(mgr+0x58, &{0xf95d1f,0xf95c06,0x9b})`,
  libc++-style tree descent comparing `[node+0x20]` with `0xf8f500`, result `node+0x40`, unlock `0x3b3c0`.
- Path 2, manager null: static-init guard, then linear scan of the pending list at `0x2dfce48/0x2dfce50`, matching
  `[node+0x28] == name.second` **and** `0xf8f500([node+0x20], name) == 0`; returns the list node.
- Reuse that already exists (OBS): `FConCmdManagerModel::find` + `ConCmdRegistry` pending list
  (`startup_ctors.hpp:645`), `CritSec` (E1034), `insert_21cd0` lock-thunk convention.
- **Correction (bytes):** `0xf8f500` is a `jmp *[rip+0x670b6a]` to GOT `0x1600070` = `strcasecmp`
  (NID `AV6ipCNa4Rw`, `imports.json`), not `strcmp`. `owner_ctor_4a690.hpp:12,55,92,160` and `lookup_1fc80.hpp:6,10` say
  "strcmp PLT" and `lookup_1fc80.cpp:43` defaults to `std::strcmp`; `FConCmdManagerModel::StrcmpLess`
  (`fconcmd_object_slot306.cpp:234`) uses `std::strcmp`. Continue-A's `strcasecmp_plt` injection is right. INF: if
  `0x20d80`/`0x213b0` also call `0xf8f500`, the manager key order is case-insensitive and the slot-306 model is wrong
  for names differing only in case. Verify before building 21620 on that model.

What it does and does not buy: it only supplies `sub` in `run_lazy_bind_91210`. `vcall_18`, `vcall_30` and the
`sub+0x60` value pointer stay hooks, so the `je 0x5329ae` outcome still comes from a fixture byte. And the lookup must
succeed for real: the original dereferences `[0x2efe340]+0x60` unchecked (0x53292f–0x532994), so a null lookup is a
crash, not a branch. Native `shadowDemoMode` rows (slots 372/433) are *handles* (vtable `0x15976b0`); the registering
ConVar is a different, unidentified ctor. If it is outside the 449 lifted slots the lookup has nothing to find.

Dependencies: Luna `CPU-01-INIT-21620-SPEC-01` (in flight; file `...init-21620-luna-spec-e1042.md` absent at write time).
Ask Luna to answer: (a) who registers `shadowDemoMode` and `r_fps`; (b) install order of `*(0x2dfcbc8)` (`0x242c0`)
relative to Init; (c) `0xf8f500` identity (answered above); (d) whether `0x20d80`/`0x213b0` use the same comparator;
(e) `0x3b390` arg 2 (`{func, file, line}` source-location) and whether it is CritSec lock.
Implement as a host-model lookup returning `ConCmdBase*` (borrowed view), never walking guest RB-node offsets
(the guest layout is not reproduced); fail closed (`ok=false`) on missing registrar. New files only
(`console_lookup_21620.*`, test, checker), so it is parallel-safe against item 1.

### 3. `CPU-01-INIT-AB-NATIVE-WIRE-01` — let A's modelled outcome pick the route (candidate 3)

OBS: the driver picks alt vs B from `post_base_alt_5327bd_hooks.gate_check_supplied || gate_2dfce70 != nullptr`
(`...cpp:541`), a test-setup flag, **not** from `post_base_continue_a`'s modelled `je 0x5327bd` outcome (E1039).
Wire: if `continue_a` reports a modelled outcome, use it to choose alt or B; on a hit call
`run_init_continue_ab_native_lookups` so B runs on the real `0x1e4c0` result. Keep caller-supplied override for tests.

Dependency: after item 1 (same file, same function). Needs a host provenance decision for the `0x98`-stride config table
and the `"nointro"` row (INF: absent on a normal launch, so alt is default; confirm what populates the table before
claiming this).
Value: removes a test-only branch selector. Modest. Not a gate mover.

### 4. `CPU-01-INIT-REMAINDER-DEDUP-01` — stop double-running leaves (new)

OBS: `run_appmain_init_remainder_slice` (`appmain_init_remainder_slice.cpp:9–23`) re-runs `ret_nop_56db70/5b3d00/5b3cf0/56fad0`,
`hide_splash_screen_f90310`, `set_init_flag_pair_4c4950` and the three `0xa52b70/0xa599e0/0xa431e0` setters, all of which the
post-base OBS modules (E450–E456, B, C, alt) already pin as call-sites. The continue-C call hooks default to null, so
the remainder is what performs the stores today. Result: per-call counts in a full run are not "executed once".
Fix: wire C/B call hooks to the native leaves, then shrink the remainder to the registry fragment (`0x532b0c..`).
Depends on item 1 (driver) and conflicts with 3/5 on the same files, so serialize. Medium test churn, low leverage.

### 5. `CPU-01-INIT-499F80-BODY-WIRE-01` — plumb `body_globals/body_hooks` through the driver (candidate 4)

OBS: E1036 added the optional wiring inside `init_531d60_post_base_499f80`; the slice driver never passes it, so the
193 B native body (`run_init_499f80_body`) is dead in the Init path. Add driver params, default native hooks, one test.
Per E1034/E1033-B: **do not** chain `0x56dcb0`; this is a small step, not boot critical path. Lowest leverage,
smallest cost, but it edits the driver signature, so schedule after 1 and 3.

## Not ranked (and why)

- Slot 309 (`0xa505e0`): excluded by the constraint; remains the coverage metric (449/573) only.
- `0xa54920` save-data cluster (~1,840 B; three sites in E, strings `save_%s_max_count` etc.) matters for G5 save/reload
  but is large. First check whether it inserts into the same console manager as `0x21620` (shared contract) before sizing it.
- `0xa52530` pthread create: slot-309 ordering still open (E1035 rank 3).

## Sonnet A/B-style conflict handling (E1033 parallel)

- **A-position:** lift `0x21620` first (HANDOFF E1041 "next exact action", 39 callers, shared contract).
- **B-position:** compose the already-lifted blocks correctly first (zero RE cost, fixes a defect, unblocks the
  default path) and keep `0x21620` behind Luna's spec.
- **Resolution:** B for ordering, A in parallel. They touch disjoint files (driver vs new module), so there is no
  real sequencing conflict. A's value case is real; B's is cheaper and correct today. Neither moves G1.
- Slot 309 conflict from E1033 is unchanged: stay with the integrator merge (B), no pivot without principal.

## Doc/claim conflicts found (for integrator to correct, not edited here)

| Claim | Where | Reality (OBS/bytes) |
|---|---|---|
| Successors `0x53299d`/`0x5329ae` "stay unmodelled" | HANDOFF E1041; `continue_c.hpp` NOT-modelled list | Lifted as `continue_d` (E535) and `continue_e` (E536) with checkers; only driver composition is wrong |
| `0xf8f500` is the `strcmp` PLT | `owner_ctor_4a690.hpp`, `lookup_1fc80.*`, `StrcmpLess` | GOT `0x1600070` = `strcasecmp` (static bytes + `imports.json`) |
| `CPU-01-INIT-53299D-PIN-01` ACTIVE | `docs/TASKS.md:19` | Redundant; recommend cancel/close |
| Default fixture covers the "unknown" path | `test_appmain_init_ctor_98a90_slice.cpp:2427–2438` | It checks D only; E is skipped on that path |

## Parallel vs wait (for the Composer integrator)

Parallel now:
- Close/cancel the `53299D-PIN` worker (no work left).
- Start item 1a immediately (driver + its test; single owner of `appmain_init_ctor_98a90_slice.*`).
- Dispatch the `strcmp`→`strcasecmp` verification (ELF byte read of `0x20d80`/`0x213b0` callees, then fix `lookup_1fc80` default
  and `StrcmpLess` if confirmed). Small, independent, and a prerequisite for item 2.
- Item 2 implementation as soon as Luna's spec lands, in new files only.

Wait / serialize:
- 1b/1c after 1a (same function). Items 3, 4, 5 strictly after 1 (all edit the driver or remainder).
- Item 2 waits on Luna answers (a) and (b); do not start with a guessed registrar.

Integrator paste block for HANDOFF/TASKS: register `CPU-01-INIT-DRIVER-JOIN-01` (READY), keep `21620-LOOKUP-01` as
WAITING_FOR_DEPENDENCY (Luna spec), mark `53299D-PIN-01` DONE_DOC/redundant, add the strcasecmp correction as a
`STATIC_ANALYSIS_VERIFIED` fact. Run `python3 scripts/project_index.py` after adding this file.

## Evidence trail

- ELF `local/re/eboot.elf` (SHA in checkers `d0723afb…61d3`) read via `scripts/ps4_dynlink.py`:
  `0x21620..` = `554889e54157415641554154534883ec38`; `0x53299d` = `488d050450dc0231f6488b38e8024a5100`;
  `0x5329ae` = `488d055fb58902`; `0xf8f500` = `ff256a0b6700` → GOT `0x1600070`.
- Saved disassemblies (private, derived earlier, not re-derived this turn): `local/validation/appmain-agent/init_531d60.dis`,
  `local/validation/registry-analysis-agent/all.dis`.
- Repo reads: `HANDOFF.md` E1033–E1042, `ROADMAP.md` operating plan, E1035 inventory, `docs/TASKS.md:8–19`,
  driver/modules named inline. Observations versus inference are labelled per item.
