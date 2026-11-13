# Liveness fix, reference audit, and the 93rd body: the printf wrapper

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. No successor id is defined.

**Evidence:** E1446. Branch `cursor/init-hash-551140-6aa93e8`, base `6e321630`.

**State: green.** Init-lift untouched: **46,876 / 46,881 B**, **108,325 B** native. `0x2170`
(`0x5ba041`) and S4.2 geometry `0x7f8664..0x7f87d8` stay closed.

## 1. A real liveness bug, not a new blocker

`0x9844b0` looked live at indirect site `0x984e95`. The site is a `callq *%rbx`; the next
instruction (`0x984e97`) is `movq -0x178(%rbp),%rax`, so the printf return is overwritten before any
read. The liveness solver was indexing successor block **indices** into the address window:

```python
out = any(live_in[position[window[s]]] for s in successors[bi])   # wrong
out = any(live_in[position[blocks[s][0]]] for s in successors[bi])  # fixed
```

`window[s]` is the address at instruction position `s`, not the successor block's first address, so
an unrelated instruction could make `rax` live. After the fix the full registry was re-derived from
scratch; the 23 existing `void_noop` rows stayed dead, no row needed to be withdrawn, and
`0x9844b0` became the supported row described below. The checker now recomputes indirect-site
liveness independently instead of trusting only direct-site liveness.

## 2. What the 22 stub qword references actually are

The previous `image_qword_references` counted the whole ELF file, including `.rela`/section bytes
that are never mapped. The generator now scans only file-backed load segments and separately reads
RELATIVE relocation slots. This changes the evidence from a file-byte count to named image sites:

| Stub | Image site(s) | Resolution |
| --- | --- | --- |
| `0x207d0` | `0x1594d78` | relocated function-pointer/vtable slot |
| `0x23370` | `0x1594e90` | relocated function-pointer/vtable slot |
| `0x235c0` | `0x1594eb0` | relocated function-pointer/vtable slot |
| `0x25c90` | `0x1595118`, `0x24e49e1` | relocated slot plus inert unaligned bytes |
| `0x41900` | `0x29b2bef` | inert unaligned bytes; no relocation |
| `0x140ce0` | `0x15983e0` | relocated function-pointer/vtable slot |
| `0x16a5d0` | `0x1598798` | relocated function-pointer/vtable slot |
| `0x2111d0` | `0x159abd8` | relocated function-pointer/vtable slot |
| `0x49cc50` | `0x15bfd00` | relocated function-pointer/vtable slot |
| `0x49ce50` | `0x15bfd98` | relocated function-pointer/vtable slot |
| `0x5388e0` | none | no image or relocation reference |
| `0x829ac0` | `0x15ceae0` | relocated function-pointer/vtable slot |
| `0x829b20` | `0x15ceb00` | relocated function-pointer/vtable slot |
| `0xa32490` | `0x15d11a0` | relocated function-pointer/vtable slot |
| `0xa40e20` | `0x15d16e0` | relocated function-pointer/vtable slot |
| `0xa4f4c0` | `0x15d1b90` | relocated function-pointer/vtable slot |
| `0xa4feb0` | `0x15d1c48` | relocated function-pointer/vtable slot |
| `0xa50270` | `0x15d1ce0` | relocated function-pointer/vtable slot |
| `0xa62490` | `0x15d1fc0` | relocated function-pointer/vtable slot |
| `0xa62850` | `0x15d2098` | relocated function-pointer/vtable slot |
| `0xacc5b0` | none | no image or relocation reference |
| `0xb0e240` | `0x15d5978` | relocated function-pointer/vtable slot |

None of the relocated slots has a resolved indirect **call** site in the 55,790-FDE scan, so the
return cannot be proved unused at a caller. They stay blocked with the slot address, the image-site
list and `reference_resolution` in JSON. No hand-written rows were added.

## 3. `0x9844b0` is now the 93rd body

The body is exactly:

```text
0x9844b0  leaq 0x12b57f4(%rip),%rdi   # "%s"
0x9844b7  xorl %eax,%eax
0x9844b9  movq %rdx,%rsi
0x9844bc  jmp  0xf8fed0               # PLT slot 0x1600558
```

The GOT relocation resolves the NID to **`hcuQgD53UxM` = printf**; the format bytes at `0x12b57f4`
are `%s\0`; argument 3 (`rdx`) is forwarded as the string. Its two resolved indirect sites are
`0x98461a` in `0x9844d0` and `0x984e95` in `0x984700`; both ignore the printf return after the
liveness fix. It is registered as custom kind `printf_string`, with `direct_call_sites=0`,
`indirect_call_sites=2`, `call_sites=2`, `rax_live_sites=[]`.

The native body is a normal SysV function: a host hook observes the forwarded string and returns
its length; without a hook it calls `std::printf("%s", message)` and returns printf's count. The
runtime asserts `printf_string_ok=true`; the fixture asserts the same string and return.

## 4. `0x20d80` stays blocked with the named missing piece

Three direct sites remain: `0x21166` in `0x210b0`, and `0x23770`/`0x238d9` in `0x235d0`. All are
`rax`-dead, so return liveness is not the blocker. The non-null path needs:

- live container at BSS `0x2dfcbc8`, with tree root at `+0x18` and lock at `+0x58`;
- node inserter `0x211d0` and comparator `0xf8f500`, both unmodelled;
- `0x23612` clears the BSS slot to zero before the first call, so the pinned static init does not
  produce a non-null container for that path.

The actual 392-byte body was decoded, including the null branch at `0x20e7f` and the tree walk at
`0x20e10`. It is not relabelled; the JSON carries `missing_host_state`, `unmodelled_callees`, the
three direct sites and the corrected zero image-reference count.

## 5. Counts and gates

Surface **92 -> 93**; blocked **24 -> 23**. Per kind: 23 `void_noop`, 28 `const_u32`, 4 `zero_u32`,
3 `set_u8`, 2 `set_u32`, 31 `vtable_pair_ctor`, 1 `concmd_lazy_register`, 1 `printf_string`.
The bounded candidate guard stays at **116 candidate rows** (93 registered + 23 blocked); the
rejected unbounded variant remains recorded at **570 rows**.

Measured on this state: build **0**; full CTest **706/706**; checker `native_thunks=93` with
mutation and reference/guard tamper probes; `run_all_checkers.sh` **915 passed / 0 skipped /
0 failed**, exit **0**; project index **114,667**; publish-path audit `--all` exit **0**;
attribution audit OK; runtime exit **0**, stderr **0 B**, `ok=true`, `printf_string_ok=true`,
`byte_setter=255`, `previous_targets` unchanged. External `reference_resolution` tamper exits
**1** and is restored.
