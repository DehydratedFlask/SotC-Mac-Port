# 2026-10-22 Init 0x1e4c0 contract analysis (CPU-01-INIT-1E4C0-ANALYSIS-01)

Date: 2026-10-22 23:59 CDT. Agent: subagent/deepseek-v4-pro. Attempt 1 — success (0 failed tries).

## OBSERVATION

### 0x1e4c0 function (0x1e4c0..0x1e5ff, 0x140 B, sha256 1188f8517326e086907a871b96482be50005651deed17f9b55fb22c51c4afbc1)

**Signature**: `int config_bool_lookup(void* obj, const char* key, int index, int default_val)`

- Searches a flat array of 16-byte entries `{name_ptr, value_ptr}` at `obj->{+0x30, +0x38}` (begin/end iterators).
- Match: strcasecmp at PLT 0xf8f500 (NID `AV6ipCNa4Rw`, libc) on entry->name.
- Value parsing on matched entry's value string:
  - `"true"` (0xf95a1e) / `"yes"` (0xf95a23) → returns 1
  - `"false"` (0xf95a27) / `"no"` (0xf95a2d) → returns 0
  - else → strtol-like at PLT 0xf8f5d0 (NID `zlfEH8FmyUA`, libc) base 10 → `result != 0`
- No-match fallback: by-index path at `index * 16`, same value parsing; returns `default_val` if OOB.
- Returns via `mov eax, r15d` where r15d carries the boolean result (0/1).

### Call site at 0x53277f (0x30 B, sha256 af262b1a3f25fb7ed7e85d7843a925e4604dbfbae00bb678845f64b8ca3bc2e0)

```
0x53277f: xor esi, esi       ; key = NULL → by-index path
0x532781: xor edx, edx       ; index = 0
0x532783: mov ecx, 0x1       ; default = 1
0x532788: mov rdi, rax       ; obj = struct from 0x1fc80 result
0x53278e: call 0x1e4c0       ; bl = config_bool_lookup(obj, NULL, 0, 1)
0x532793: mov ebx, eax
0x532796: mov r12b, bl
0x532799: xor r12b, 1        ; !bl → set_init_flag_pair(!bl)
0x53279d: movzx edi, r12b
0x5327a1: call 0x4c4950
0x5327a6: mov r15, [r13]     ; r13=0x32f84f8 (BSS global ptr)
0x5327a9: test bl, bl
0x5327ab: je 0x5328e0        ; bl==0 → 0x5328e0
0x5327ad: jmp 0x532904        ; bl==1 → 0x532904
```

### 0x1fc80 function (0x1fc80..0x1fd0f, 0x90 B, sha256 b7fb9f259453bae7ebf9ce089156975fa93f87ea2dcf9a515644730930c49929)

**Signature**: `void* lookup_by_name(void* container, const char* name)`

- container = `{void** begin, void** end}`.
- Linear scan of pointer array (8-byte pointers), each pointing to a 0x98-byte struct whose name at +0x28 is compared via strcasecmp.
- Returns matching struct pointer or NULL.
- Called at 0x532775 as `lookup_by_name(&rsp20_obj[1], "nointro")`.

### r13 / 0x32f84f8

In BSS segment 2 (zero at load). Written at runtime by earlier Init. Its value is loaded into r15 for use in the branch-successor path (both 0x5328e0 and 0x532904).

### String "nointro" at 0x1014d69

Config key passed to 0x1fc80 — the game checks whether intro sequences should be skipped.

## INFERENCE

1. **bl semantics**: bl is PROVABLY 0 or 1. With ecx=1 (call-site default), every code path in 0x1e4c0 sets bl to either 0 or 1. It is a boolean config-value flag: 1 = truthy (intro skip enabled), 0 = falsy (intro play).
2. **Branch modelling**: The `test bl,bl; je 0x5328e0` branch CAN be modelled honestly. When `branch_outcome_modelled = true`: bl=1 → 0x532904 path, bl=0 → 0x5328e0 path.
3. **0x1fc80 contract**: Named struct lookup returning pointer or NULL. Consumed at 0x532788 where result becomes rdi to 0x1e4c0.
4. **0x32f84f8**: BSS slot, runtime-initialized pointer. Not statically resolvable; needs host stand-in.

## Rule for `branch_outcome_modelled`

May become `true` when host provides config-value stand-ins via 0x1fc80/0x1e4c0 hooks. 0x1fc80 must return non-NULL (opaque struct ptr), 0x1e4c0 must return 0 or 1.

## EVIDENCE rows to add

- **E520**: 0x1e4c0 body (0x140 B, sha 1188f851…): boolean config-value lookup; strings "true"/"yes"/"false"/"no"; strcasecmp + strtol-like PLT calls; returns 0/1 with ecx=1 default.
- **E521**: 0x1fc80 body (0x90 B, sha b7fb9f25…): named struct lookup at +0x28 name offset via strcasecmp; returns pointer or NULL; consumed by "nointro" key.
- **E522**: 0x53277f call-site block (0x30 B, sha af262b1a…): 0x1e4c0 call with ecx=1 + test bl,bl branch; 0x32f84f8 BSS pointer load; branch outcomes 0x5328e0/0x532904 pinned.

## Next exact action

Continue Init at 0x5327af (jmp 0x532904, the fallthrough when bl!=0), or model the 0x5328e0 branch target path, with `branch_outcome_modelled = false` until hook stand-ins exist.

## INDEX rows

```
E520 | 0x1e4c0 body | 0x140 B | boolean config-value lookup ("true"/"yes"/"false"/"no") | init_1e4c0_contract | 2026-10-22
E521 | 0x1fc80 body | 0x90 B | named struct lookup (strcasecmp at +0x28) | init_1fc80_contract | 2026-10-22
E522 | 0x53277f call-site | 0x30 B | 0x1e4c0 call + test bl,bl branch + 0x32f84f8 BSS load | init_post_base_continue_b_call | 2026-10-22
```
