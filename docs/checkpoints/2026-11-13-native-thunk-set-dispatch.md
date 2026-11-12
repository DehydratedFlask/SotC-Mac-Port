# The 15 resolved targets, 81 native bodies, and a dispatch that runs

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. The roadmap names no successor id; its own
next-gap sentence is "Native ABI thunk bodies are the next gap", and `docs/ENTRY_MAP.md` item 3
owns the remaining indirect frontier. This pass takes half (1) of that card seriously and adds the
CFG-resolved dispatch path it implies.

**Evidence:** E1443. Branch `cursor/init-hash-551140-6aa93e8`, base `fa0b058a`.

**State: green.** Init-lift untouched: body extent **46,876 / 46,881 B**, native total **108,325 B**;
`0x2170` (`0x5ba041`) and the S4.2 geometry block `0x7f8664..0x7f87d8` (gated on
`0x3288478`/`0x3288480`) remain the only contracts and are not re-opened.

## 1. All 15 previously resolved targets are accounted for

The prior pass declined the 14 one-byte `ret` targets as "a C thunk cannot leave `rax`
caller-owned". That was a blanket refusal, so this pass ran backward `rax` liveness over the
frontier CFG for each target: at every resolved call site it records the instruction's `live_out`
for `rax` and walks forward from the site to the block exit. The result is that **`rax` is dead at
every resolved call site of all 14 stubs** (including the 47/12/1 sites that a block-exit-only check
had wrongly reported as live). The stubs therefore have no return contract to preserve, and the
native body is an exact `void` no-op.

| RVA | Body | Resolved sites | `rax` live | Native state / observed effect |
| --- | --- | ---: | --- | --- |
| `0x19f0` | 1 B `retq` | 1 | none | `void_noop`; invoked, canary unchanged |
| `0x25aa0` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0x2bdd0` | 1 B `retq` | 1 | none | `void_noop`; invoked, canary unchanged |
| `0x20d60` | 26 B | n/a (custom) | n/a | `concmd_lazy_register`; dispatch flips `+0x18`, hook once |
| `0x528d90` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0x529150` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0x5296c0` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0xc2f0` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0x529a80` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0x538520` | 1 B `retq` | 6 | none | `void_noop`; invoked, canary unchanged |
| `0x538850` | 1 B `retq` | 3 | none | `void_noop`; invoked, canary unchanged |
| `0x538200` | 1 B `retq` | 2 | none | `void_noop`; invoked, canary unchanged |
| `0x5388c0` | 1 B `retq` | 1 | none | `void_noop`; invoked, canary unchanged |
| `0xc4f0` | 1 B `retq` | 138 | none | `void_noop`; invoked, canary unchanged |
| `0xc770` | 1 B `retq` | 35 | none | `void_noop`; invoked, canary unchanged |

The runtime obtains the 15 RVAs from the generated `kGuestResolvedTargets` array, invokes the 14
stubs through `guest_thunk_invoke` with a 0x40-byte `0xa5` canary object, and requires all 0x40
bytes unchanged after each call. `0x20d60` is asserted by the dispatch block below. Observed:
`previous_targets={total:15,called:14,ok:true}`. The fixture repeats the loop and checks the canary
per byte.

## 2. Registered native surface: 2 -> 81, every row re-derived

`entry_closure.py` now plans and emits the bodies itself. Registration is limited to an exact
complete FDE body matching one host-free pattern; nothing is registered from a name or a guess.

| Kind | Count | Exact body / precondition | Observed effect |
| --- | ---: | --- | --- |
| `void_noop` | 16 | 1 B `retq`; backward `rax` liveness proves every resolved caller ignores the result | none |
| `const_u32` | 28 | 6 B `movl $imm,%eax; retq` | returns the pinned immediate |
| `zero_u32` | 4 | 3 B `xorl %eax,%eax; retq` | returns 0 |
| `set_u32` | 1 | 4 B `movl %esi,0x1c(%rdi); retq`; `rax` dead at every site | object `+0x1c = 0x2a` |
| `vtable_pair_ctor` | 31 | 20 B; stores `rsi+0x10` at `rdi+0x10`, a RIP-pinned constant at `rdi+0x20`, returns `rsi+0x10` | qword copied and constant installed |
| `concmd_lazy_register` | 1 | 26 B custom; flag test/store reproduced, map insert is a host hook | flag 1, hook 1, second call no-op |

**81** rows in total (16 + 28 + 4 + 1 + 31 + 1), generated as
`native/core/guest_thunk_table.inc` (sha256 of the include is pinned by the checker through exact
text comparison; every row's body sha256 is re-derived independently). The fixture exercises the
setter (`0x13d20`), a constant (`0xb890` returns 1), a constructor (`0xc2a0` copies
`0x1594280`), a zero entry, every one of the 15 targets, the custom body and an unknown RVA.

**35 entries keep their exact RVA and verbatim reason and are not registered.** 29 have no mapped
call site, so the return value cannot be proven unused:

`0x207d0 0x23370 0x235c0 0x25c90 0x41900 0x4a4a0 0xa1cc0 0x140ce0 0x16a5d0 0x2111d0 0x49cc50
0x49ce50 0x5388e0 0x56db70 0x56dca0 0x56fad0 0x5b3cf0 0x5b3d00 0x829ac0 0x829b20 0xa32490
0xa40e20 0xa4f4c0 0xa4feb0 0xa50270 0xa62490 0xa62850 0xacc5b0 0xb0e240`
(`no_mapped_call_site_to_prove_return_unused`).

Three byte setters have no liveness evidence:
`0xa431e0 0xa52b70 0xa599e0` (`byte_setter_return_value_caller_owned_no_liveness_evidence`).

`0x147e10` — `setter_return_value_caller_owned_no_call_site`.
`0x20d80` — `needs_live_manager_pointer_from_bss_0x2dfcbc8`.
`0x9844b0` — `tail_jump_needs_unmodelled_callee_and_rodata_leaq`.

No image symbol exists for these RVAs (the retail image is stripped), so `symbol` is `null` in
`docs/entry_closure.json` for every blocked row.

## 3. A CFG-resolved indirect site executes end to end

Site `0x28471` in function `0x27990` is `ff 50 20` (`callq *0x20(%rax)`), resolved by the CFG to
vtable base `0x1594dc0`, slot `0x20`, target `0x20d60` (record kind `vtable_slot`). The generated
include contains the host image of the 0x28-byte vtable read from the pinned ELF (sha256
`62d5d2776fa8b3c005533e61eef04eea0e796469b49ef183a3e2093f3c899aa7`). `guest_dispatch_slot`:

1. receives an object whose first qword is the host vtable image;
2. reads slot `0x20` from that image at run time;
3. looks the value up as a registered entry and calls its native body through the same SysV
   pointer as the direct probes;
4. asserts **the invocation result**: object `+0x18` becomes 1, the registration hook fires once,
   the second dispatch is a no-op.

It does not assert the resolution table's contents as a substitute for the call. Observed:
`dispatch={site:0x28471,target:0x20d60,flag:1,hook_calls:1,ok:true}`. A wrong-slot dispatch is
rejected by the fixture.

`native/core/guest_thunks.hpp` exposes `guest_thunk_count`, `guest_thunk_lookup`,
`guest_thunk_invoke`, `guest_dispatch_slot`, `guest_dispatch_site` and `guest_resolved_targets`;
the generated include carries the leaf table, `kGuestResolvedTargets`, `kGuestDispatchSite` and
`kGuestDispatchVtable`.

## 4. Checker, real bugs, gates

`check_entry_closure.py` now unpacks the generator's `(data, inc, thunk_inc)` triple, compares all
three artifacts byte-for-byte, and independently re-derives for every one of the 81 rows: FDE start,
exact FDE size, body sha256, generated-vs-custom placement and the C++ registry spelling. It
re-derives the 35 blocked RVAs as real starts (or PLT), requires a verbatim reason for each, and
re-derives the dispatch: the qword at `vtable+slot`, the 0x28-byte image hash, the resolved site
record (function/operand/target) and the instruction bytes `ff 50 20`.

The mutation probe is run and detected a tampered table row, a tampered indirect total, a tampered
thunk body hash and a tampered dispatch target. An external tamper of `docs/entry_closure.json`
was run (checker exit 1) and reverted.

**Real bugs found and fixed in this pass:**

- The dispatch instruction check compared the ModRM byte to `0x50 + slot/8`; for `ff 50 20` the
  ModRM is `0x50` and `0x20` is the displacement. The checker rejected a correct artifact. Fixed
  to require `ff 50 <slot>`.
- The blocked list contained `0x147e10` twice (once from the setter loop, once appended manually),
  inflating "36 blocked" by one. The checker's count guard surfaced it; the duplicate was removed
  and the true count is **35**.
- (Carried from the implementation step) the generated table had included the custom `0x20d60` row
  a second time, shadowing the custom body; custom rows are now excluded from the generated table
  and counted once in JSON.

The 15-entry `previous_targets` list is emitted by the same generator that plans the rows, so the
custom entry and the 14 stubs are registered before the runtime reads the list; the runtime fails
`ok` unless it observes all 15.

Measured gates:

- build exit **0**; full CTest **706/706**.
- `check_entry_closure.py --mutate` OK; checker re-derives `222/499` resolved sites and **81**
  native thunks.
- `sotc_init_window` exit **0**, stderr **0 B**, `ok=true`,
  `guest_thunks={registered:81,calls:3,setter_value:42,register_flag:1,hook_calls:1,ok:true,
  dispatch:{site:0x28471,target:0x20d60,flag:1,hook_calls:1,ok:true},
  previous_targets:{total:15,called:14,ok:true}}`.
- Entry map regenerated: **1,957 RVAs / 584 headers / 462 duplicates** (unchanged).
- `run_all_checkers.sh`: **915 passed / 0 skipped / 0 failed**, exit **0** (after the entry-map
  regeneration above).
- Project index **114,667**; publish-path audit `--all` exit **0**; attribution audit OK.
- Init totals unchanged **46,876 / 108,325 B**. G1 **PARTIAL**.

**Not claimed:** cold boot, audio, real save/reload after relaunch and Metal validation are
**NOT RUN**. No original RVA executes; the app still does not boot the game.
