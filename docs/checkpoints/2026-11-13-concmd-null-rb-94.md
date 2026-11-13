# `0x20d80` null branch and RB rotations close the last live gap

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. No successor id is defined.

**Evidence:** E1448. Branch `cursor/init-hash-551140-6aa93e8`, base `a44f15a5`.

**Checkpoint:** local **2026-11-12 22:02 CST** (record date 2026-11-13), agent Buffy. Changed:
`native/core/guest_thunks.cpp`, `native/core/guest_thunks.hpp`, `native/app/sotc_init_window_main.cpp`,
`native/tests/test_guest_thunks.cpp`, `tools/lift/entry_closure.py`, `tools/lift/check_entry_closure.py`,
the regenerated `docs/entry_closure.json` / `docs/ENTRY_CLOSURE.md` / `docs/entry_map.json`, and the
checkpoint/handoff/roadmap/tasks/index/evidence records. Logs: `logs/build-rb2.log`,
`logs/ctest-rb.log`, `logs/check-rb.log`, `logs/sweep-rb.log`. Exact commands and exit results are
in section 5. Observation: decoded stores and the runtime JSON; inference: the RB fixup's internal
rotation sequence is the standard contract, not an instruction-by-instruction replay.

**State: green.** The top ordered blockers are the **22 reference-audited one-byte stubs**; none can
be registered without widening the candidate scan or inventing a call site. This pass therefore
takes the roadmap's fallback branch: it closes the two unmodelled paths inside the already
registered `0x20d80`. Registered/blocked counts remain **94 / 22**; the per-kind delta is **0**.

## 1. Null-container branch, from the decoded instructions

At `0x20e7f` the decoded body tests the one-shot guard at `0x2dfce60`. If unset it calls `0xf8f3d0`
(init), writes the self-referential sentinel pair at `0x2dfce50`/`0x2dfce58` with a `vmovdqu`, then
calls `0xf8f3e0` (guard completion). The append at `0x20ebe..0x20edf` then:

- loads the tail at `0x2dfce58`;
- writes that tail to `node+0x10`;
- writes the sentinel `0x2dfce50` to `node+0x8`;
- stores `node+0x8` back to `0x2dfce58`;
- writes `node+0x8` through the old tail link.

The branch never reads `node+0x20`, so it does not compare keys and does not reject duplicates.
`append_global` models that sentinel and those four stores. A second insertion leaves the first
node's `+0x8` holding the second node's `+0x8`, while the second node's `+0x10` holds the first
node's `+0x8`; the runtime asserts both exact links.

## 2. RB allocator/rotation contract

The non-null insert still calls `0x211d0` (0x48 B allocation, tree node returned at `tree_node+0x40`)
and `0x42d30` performs the fixup. Decoded `0x42d30` stores left at `+0x0`, right at `+0x8`, parent at
`+0x10`, and the color byte at `+0x18`; it recurses up the parent chain and rewrites the root when a
rotation crosses it. The native model now uses those same four tree-node fields and implements the
standard red-black insert fixup, counting each left/right rotation.

`Alpha`, `Beta`, `Gamma` is a right-right insertion, so the third insert forces one rotation.
The following case-insensitive `alpha` duplicate returns before insertion and changes neither size,
root, rotations nor any original node byte.

## 3. Observed runtime effect

```text
concmd_registry={size:3,rotations:1,global:2,null_ok:true,ok:true}
byte_setter=255
previous_targets={total:15,called:14,ok:true}
printf_string_ok=true
registered=94
```

Exit **0**, stderr **0 B**. The null scenario calls `0x20d80` twice with container `0` and key-less
nodes; both calls succeed, the global sentinel count reaches 2, and the exact `+0x8`/`+0x10` link
values match the decoded store order.

This remains a **contract-level native model**, not execution of the original x86 instructions. It
now covers the null global-list branch and the observable RB rotation contract; it does not claim
the allocator's internal ownership or every upstream RB variant.

## 4. Row and evidence

`0x20d80` keeps `direct_call_sites=3` (`0x21166`, `0x23770`, `0x238d9`), `indirect=0`, `call_sites=3`,
`rax_live_sites=[]`, and body SHA-256
`f59869f13c36fe90ed647d1a642006ec48be8676d992175fd959a97346cc41fb`.

The row now records `null_branch=0x20e7f`, `global_sentinel=0x2dfce50`, `global_tail=0x2dfce58`,
`global_guard=0x2dfce60`, and `tree_color_offset=0x18`. The generator validates the decoded
mnemonics at `0x20e7f`, `0x20e99`, `0x20ea0`, `0x20eb1`, `0x20ebe`, `0x20ec5`, `0x20ed8`, and
`0x20edf`, plus the two init calls at `0x20e90` and `0x20eb9`.

Surface remains **94** (23 noop, 28 const, 4 zero, 3 set_u8, 2 set_u32, 31 vtable-pair ctor,
1 ConCmd lazy, 1 ConCmd registry insert, 1 printf). Blocked remains **22** (19 relocated slots, one
inert image reference, two without image/relocation references). Guard remains **116** rows; the
570-row unbounded variant remains rejected inflation.

## 5. Gates

Measured on this state: build **0**; full CTest **706/706**; checker `native_thunks=94` with mutation
probes for row/total/hash/direct count/scan/dispatch/reference resolution/candidate guard; runtime
exit **0**, stderr **0 B**, `concmd_registry={size:3,rotations:1,global:2,null_ok:true,ok:true}`;
`run_all_checkers.sh` **915 passed / 0 skipped / 0 failed**; entry map **1,957 / 584 / 462**;
project index **114,667**; publish-path `--all` exit **0**; attribution OK.

**NOT RUN:** cold boot, audible game audio, save/reload after relaunch, Metal validation.

**Next exact action:** resolve one of the 22 reference-audited stubs through a real relocated-slot
call with caller-level `rax` liveness. Do not widen the scan or invent a site.
