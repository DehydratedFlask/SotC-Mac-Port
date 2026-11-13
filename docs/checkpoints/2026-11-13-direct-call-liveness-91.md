# Direct-call liveness: 81 -> 91 native bodies

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. The roadmap still names no successor id; its
next-gap sentence remains "Native ABI thunk bodies are the next gap".

**Evidence:** E1444. Branch `cursor/init-hash-551140-6aa93e8`, base `9b88fab8`.

**State: green.** Init-lift untouched: **46,876 / 46,881 B**, **108,325 B** native. `0x2170`
(`0x5ba041`) and the S4.2 geometry block `0x7f8664..0x7f87d8` (gated on `0x3288478`/`0x3288480`)
stay closed.

## 1. The gap was in the evidence scope, not the stubs

The previous pass proved `rax` dead at every **indirect** call site, but only decoded the first
128 bytes of each mapped function and only ran liveness over the 105-function indirect frontier.
The 23 still-blocked one-byte stubs and four setters were therefore blocked by *missing call-site
evidence*, not by a proven live return.

This pass decodes the **complete FDE body of all 830 mapped function starts** (150,453
instructions), collects every direct `call rel32` to a mapped start (the `LLVM objdump` operand
annotation `0x… <PT_LOAD#0+…>` is part of the operand string), and runs the same backward
`rax` liveness over the caller's full body. Direct call sites are merged with the existing
indirect sites before classification.

Measured: **13 direct call sites to 11 targets**; `rax` is dead after every one.

| Target | Direct sites | Outcome |
| --- | ---: | --- |
| `0x4a4a0` | 1 | `void_noop` registered |
| `0x56db70` | 1 | `void_noop` registered |
| `0x56dca0` | 1 | `void_noop` registered |
| `0x56fad0` | 1 | `void_noop` registered |
| `0x5b3cf0` | 1 | `void_noop` registered |
| `0x5b3d00` | 1 | `void_noop` registered |
| `0x147e10` | 1 | `set_u32` registered (`object+0x40`) |
| `0xa431e0` | 1 | `set_u8` registered (`object+0x51`) |
| `0xa52b70` | 1 | `set_u8` registered (`object+0x1`) |
| `0xa599e0` | 1 | `set_u8` registered (`object+0x29`) |
| `0x20d80` | 3 | **not registered**: 392 B body, non-null path needs the live manager cell; remains `needs_live_manager_pointer_from_bss_0x2dfcbc8` |

The direct-only rows all carry `direct_call_sites: 1`, `call_sites: 1`, `rax_live_sites: []`.
`0x13d20` remains the sole indirect-only setter (1 site, 0 direct).

## 2. Registered surface and blocked set

**91** rows: 22 `void_noop`, 28 `const_u32`, 4 `zero_u32`, 3 `set_u8`, 2 `set_u32`, 31
`vtable_pair_ctor`, 1 custom `concmd_lazy_register`. (81 + 6 stubs + 4 setters.)

**25** stay non-callable with exact RVA and reason:

- `no_mapped_call_site_to_prove_return_unused` — **23**:
  `0x207d0 0x23370 0x235c0 0x25c90 0x41900 0xa1cc0 0x140ce0 0x16a5d0 0x2111d0 0x49cc50
  0x49ce50 0x5388e0 0x829ac0 0x829b20 0xa32490 0xa40e20 0xa4f4c0 0xa4feb0 0xa50270 0xa62490
  0xa62850 0xacc5b0 0xb0e240`
- `0x20d80` — `needs_live_manager_pointer_from_bss_0x2dfcbc8`
- `0x9844b0` — `tail_jump_needs_unmodelled_callee_and_rodata_leaq`

No image symbol exists for any blocked RVA (stripped image), so `symbol` is `null`.

## 3. Observed effects

The fixture now invokes the direct-call byte setter `0xa431e0` with `rsi = 0x1ff` and a `0x5a`
canary, and asserts `+0x51 = 0xff` with `+0x50` and `+0x52` untouched. The runtime repeats that
probe and reports `byte_setter=255`.

Observed runtime (`sotc_init_window`, exit 0, stderr 0 B):

```
guest_thunks={registered:91,calls:3,setter_value:42,register_flag:1,hook_calls:1,ok:true,
dispatch:{site:0x28471,target:0x20d60,flag:1,hook_calls:1,ok:true},
previous_targets:{total:15,called:14,ok:true},byte_setter:255}
```

The 15 previously resolved targets remain accounted for by the same generated list and canary loop;
the dispatch behaviour (flag 1, one hook, second call no-op) is unchanged.

## 4. Checker and gates

`check_entry_closure.py` now independently re-derives direct-call evidence: it decodes the same 830
full FDE bodies, collects direct calls, recomputes `rax` liveness per caller, and requires every
`direct_call_sites` count and every `rax_live_sites` list to match. It still re-derives all 91 body
hashes, FDE starts/sizes, generated-vs-custom placement, 25 blocked RVAs and the dispatch slot. The
mutation probe adds a tampered direct-call count and detects it alongside the row, total, thunk hash
and dispatch-target mutations.

**Real bugs found and fixed in this pass:** the first direct-call scan matched the operand as
`^0x[0-9a-f]+$`, so every LLVM `0x… <PT_LOAD#0+…>` annotation was rejected and the scan reported
**0** callers; accepting the annotation made the 13 sites visible. The checker's row-count guard
was also still pinned at 81 and correctly failed until updated to 91.

Measured gates:

- build exit **0**; full CTest **706/706**.
- `check_entry_closure.py --mutate`: `native_thunks=91`, all mutations detected.
- `sotc_init_window`: exit **0**, stderr **0 B**, `ok=true`, `byte_setter=255`.
- entry map regenerated: **1,957 RVAs / 584 headers / 462 duplicates**.
- `run_all_checkers.sh`: **915 passed / 0 skipped / 0 failed**, exit **0**.
- Project index **114,667**; publish-path audit `--all` exit **0**; attribution audit OK.
- External JSON tamper of `direct_call_sites` (9 instead of 1) made the checker exit **1**; the file
  was restored byte-for-byte and the checker is green again.
- Init totals unchanged **46,876 / 108,325 B**. G1 **PARTIAL**.

**Not claimed:** cold boot, audio, save/reload after relaunch and Metal validation are **NOT RUN**.
`0x20d80` still has no standalone native body because its non-null manager path needs host state.
