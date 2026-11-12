# Native retail-ABI thunks and a CFG-resolved indirect frontier

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. `ROADMAP.md` defines no new id after this card;
its own next-gap sentence is "Native ABI thunk bodies are the next gap", and `docs/ENTRY_MAP.md`
item 3 still owns the remaining indirect frontier. This pass therefore closes both halves under the
same id instead of inventing a successor.

**Evidence:** E1442. Branch `cursor/init-hash-551140-6aa93e8`, base `426ffbb1`.

**State: green.** Init-lift untouched: body extent **46,876 / 46,881 B**, native total **108,325 B**;
`0x2170` (`0x5ba041`) stays a contract and the S4.2 geometry block `0x7f8664..0x7f87d8` stays gated
on `0x3288478`/`0x3288480`.

## 1. The callable map now has callable entries

`native/core/guest_thunks.{hpp,cpp}` registers two entries behind a real SysV function pointer
(`std::uint64_t (*)(rdi, rsi, rdx, rcx, r8, r9)`); `guest_thunk_invoke` calls through it. Nothing
here interprets guest code.

| RVA | Name | Body | Behaviour |
| --- | --- | --- | --- |
| `0x13d20` | `slot_1c_setter` | 4 B, sha256 `71ce06d0…35fbf3c` | exact `movl %esi,0x1c(%rdi); ret`; observed `+0x1c = 0x2a` after the call |
| `0x20d60` | `concmd_lazy_register` | 26 B, sha256 `f4463b3b…84b403` | flag test at `+0x18`, set once, then host hook for the `0x20d80` map insert |

`0x20d60`'s tail is not faked: `0x20d80` inserts into the manager map keyed by the live pointer in
BSS global `0x2dfcbc8`, which the host does not have. The thunk records the registration through
`GuestConCmdRegisterHook`; the observed predicate is flag `1`, hook `1`, second call a no-op. The 14
one-byte `ret` targets in the resolved frontier (`0xc4f0`, `0xc770`, `0x19f0`, `0x25aa0`, `0x2bdd0`,
`0x538520`, `0x538850`, `0x538200`, `0x5388c0`, `0x529150`, …) are deliberately **not** registered:
the retail stub leaves `rax` as the caller had it and a C thunk cannot reproduce that without
inventing a return value.

Observed runtime block (`sotc_init_window`, exit 0, stderr 0 B):

```
guest_thunks={registered:2,calls:3,setter_value:42,register_flag:1,hook_calls:1,ok:true}
```

The runtime exits 3 if either probe fails. The new fixture `test_guest_thunks` asserts the same
effects plus the unknown-RVA negative.

## 2. Indirect frontier from a real CFG

`tools/lift/entry_closure.py` no longer scans backwards inside one basic block. For each of the 105
frontier functions it:

1. splits the FDE range at branch targets and fallthroughs into basic blocks;
2. runs a forward dataflow to a fixed point over those blocks;
3. meets predecessors at every block entry — a register or stack-slot value survives only when
   every reaching path defined the same absolute value;
4. models `call` as clobbering the caller-saved registers while stack slots survive (a callee cannot
   write its caller's frame), and treats `push`/`pop`/`sub rsp`/`add rsp` as frame-delta changes;
5. resolves a site only when the meet proves an absolute base (or a stack spill) **and** the slot
   contents are an exact FDE start.

Result: **222 resolved / 277 blocked of 499 sites** (was 180/319). All 180 earlier resolutions
survive; 25 of the 109 old `stack_block_boundary` sites now resolve. Resolved kinds: 217
`vtable_slot`, 5 `register`.

| Blocked reason | Sites | Verbatim meaning |
| --- | ---: | --- |
| `base_stack_object` | 85 | base is a stack object whose vtable is built by a callee at run time |
| `base_addq_writes` | 42 | base is written by add/sub with an unproven operand (indexed vtable) |
| `base_object_load:…` | 138 | base is loaded from object memory (24 are `(%rdi)`, 13 `(%rbx)`, long tail) |
| `unresolved_base` | 9 | no reaching definition |
| `stack_slot_not_entry` | 1 | the spill does not hold an FDE start |
| `unreachable_block` | 2 | `0x885eab`, `0x885ece`; the CFG proves the block unreachable from entry |

68 distinct reason strings; every site keeps its function, site RVA, operand text, status and either
a target or that reason in `docs/entry_closure.json`.

The 85 `base_stack_object` sites are genuinely irreducible statically, not a scanner limit: e.g. at
`0xc3f7` (`call *(%rax)` in `0xc300`) the base is loaded from `-0x68(%rbp)`, a field of a caller
stack object written by the preceding `0xc3e6` callee through `rdi = &-0x88(%rbp)`. Without callee
semantics the vtable pointer does not exist in the file.

## 3. Real bugs found and fixed

- **`lea` was dereferenced.** `leaq 0x…(%rip), %rax` was evaluated as a load, so the vtable base
  became its first slot (`0x1594060` → `0x19a0`) and the whole frontier resolved 0/499.
- **Fallthrough used `last + 1`** instead of the next instruction address. Most blocks became
  unreachable and the site census fell from 499 to 175; the next-instruction fix restored it.
- **`testq`/`cmpq` invalidated their destination**, producing six `base_testq_writes` and losing
  bases. Flag-only mnemonics now leave the state untouched.
- **Unreachable-block sites could disappear.** The two sites in CFG-unreachable code are now
  recorded with `unreachable_block`, keeping resolved + blocked = 499 exactly.

## 4. Checker

`tools/lift/check_entry_closure.py` re-derives the table, every site and the two thunk body hashes
from the pinned ELF, asserts the `cfg_forward` tag, unique site records, resolved + blocked = sites,
and every blocked/resolved record's required field. Its in-memory mutation probe now detects a
tampered table row, a tampered total **and** a tampered thunk hash. An external tamper of
`docs/entry_closure.json` (thunk hash zeroed) was run: checker exit **1**; file restored; clean run
`OK check_entry_closure: entries=1957 … indirect=222/499 closure=141/608`.

## 5. Gates (measured)

- Full build: **exit 0**.
- Full CTest: **706/706** (new `guest_thunks` test).
- Checker sweep after regenerating the entry map: **915 passed / 0 skipped / 0 failed**, exit 0.
- `sotc_init_window`: exit **0**, stderr **0 B**, `ok=true`, `entry_map.indirect =
  {105,499,222,277}`, `guest_thunks.ok=true`.
- Entry map unchanged: 3,052 constants → **1,957 RVAs / 584 headers / 462 duplicates**.
- Project index regenerated; init totals unchanged.

Not claimed: no native game code runs; cold boot, audio, save/reload and Metal validation stay
**NOT RUN**; G1 **PARTIAL**.
