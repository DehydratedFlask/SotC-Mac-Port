# 0x20d80 executes its non-null path: 94 native bodies

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. No successor id is defined.

**Evidence:** E1447. Branch `cursor/init-hash-551140-6aa93e8`, base `310432a5`.

**State: green.** Init-lift untouched: **46,876 / 46,881 B**, **108,325 B** native. `0x2170`
(`0x5ba041`) and S4.2 geometry `0x7f8664..0x7f87d8` stay closed.

## 1. The decoded non-null contract

The 392-byte body at `0x20d80` was decoded end to end. For `rsi != 0` it is a case-insensitive
intrusive-set insert:

- `rsi` is the container: tree root `+0x18`, size `+0x28`, lock `+0x58`; the root sentinel is
  `container+0x8`.
- `rdi` is the new node; its key pointer is at `node+0x20`.
- `0x3b390(container+0x58, stack_guard)` locks; `0x3b3c0` unlocks.
- The tree walk calls `0xf8f500` at `0x20e1a` and `0x20e4c`; the GOT slot `0x1600070` resolves to
  NID `AV6ipCNa4Rw`, the already-audited **strcasecmp** import.
- An equal key takes the `jns` path and returns without inserting.
- Insertion calls `0x211d0(container, &new_node+0x20)`, which allocates 0x48 B, stores the key at
  `tree_node+0x20`, copies the value qwords, and returns `tree_node+0x40`.
- `0x20d80` writes the original node pointer at `tree_node+0x40`; the null branch at `0x20e7f`
  initializes the global BSS sentinel at `0x2dfce50` and is not used by the seeded runtime path.

## 2. Native model and observed effect

`native/core/guest_thunks.cpp` now has custom kind `concmd_registry_insert`:

- host mutex around the model, mirroring the `+0x58` lock;
- deterministic binary search tree ordered by `strcasecmp` on `node+0x20`;
- duplicate keys return without changing size, root or either node;
- insertion copies `node+0x28..0x38`, stores the original node at tree-node `+0x40`, updates
  `container+0x18` root and increments `container+0x28` size.

This is a contract-level native model of the decoded non-null path, not execution of the original
x86 instructions. The RB-tree rotations and the `0x211d0` allocator/fixup internals are not claimed;
the observable set membership, duplicate rejection, root/size/value effects are. The null branch
(`rsi == 0`) is decoded but not modelled, and the row effect says so.

The runtime seeds a zeroed container, inserts key `"Alpha"` (root), inserts `"Beta"` (non-empty
tree path), then inserts `"alpha"` (duplicate). Observed: **size 2**, root non-null, all three node
buffers byte-identical, exit 0, stderr 0 B. The existing `byte_setter=255` and
`previous_targets={total:15,called:14,ok:true}` effects still pass.

## 3. Row and counts

`0x20d80` is registered with:

- `direct_call_sites=3`: `0x21166` in `0x210b0`, `0x23770` and `0x238d9` in `0x235d0`;
- `indirect_call_sites=0`, `call_sites=3`, `rax_live_sites=[]`;
- body sha256 `f59869f13c36fe90ed647d1a642006ec48be8676d992175fd959a97346cc41fb`.

Surface **93 -> 94**; blocked **23 -> 22**. Per kind: 23 `void_noop`, 28 `const_u32`, 4 `zero_u32`,
3 `set_u8`, 2 `set_u32`, 31 `vtable_pair_ctor`, 1 `concmd_lazy_register`, 1
`concmd_registry_insert`, 1 `printf_string`.

The bounded guard remains **116 candidate rows** (94 + 22). The 570-row unbounded variant stays
recorded as rejected inflation; the 22 remaining blocked entries are the reference-audited stubs
(19 relocated slots, one inert-data reference, two without image/relocation references).

## 4. Gates

Measured on this state: build **0**; full CTest **706/706**; checker `native_thunks=94` with
mutation probes for row/total/hash/direct count/scan/dispatch/reference resolution/candidate guard;
runtime exit **0**, stderr **0 B**, `concmd_registry={size:2,ok:true}`, `byte_setter=255` and
`previous_targets={total:15,called:14,ok:true}`; `run_all_checkers.sh` **915 passed / 0 skipped /
0 failed**, exit **0**; project index **114,667**; publish-path `--all` exit **0**; attribution
audit OK.
