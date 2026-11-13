# Full-FDE direct-call scan: 91 -> 92 native bodies

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued. The roadmap still names no successor id.

**Evidence:** E1445. Branch `cursor/init-hash-551140-6aa93e8`, base `19f5e83c`.

**State: green.** Init-lift untouched: **46,876 / 46,881 B**, **108,325 B** native. `0x2170`
(`0x5ba041`) and S4.2 geometry `0x7f8664..0x7f87d8` stay closed.

## 1. Full-FDE scan coverage

The previous pass scanned only the **830 mapped function starts**. This pass scans every
`.eh_frame_hdr` range for direct `call rel32` candidates, then decodes the complete body of every
candidate caller and validates the instruction at the site before using it.

Measured coverage:

| Measure | Value |
| --- | ---: |
| FDE ranges scanned | **55,790** |
| Code bytes scanned | **15,839,273** |
| Raw `call rel32` candidates on the candidate targets | **3,002** |
| Candidate caller bodies decoded | **898** |
| Instructions decoded in those bodies | **231,649** |
| Validated direct call sites | **3,002** |
| Targets with validated sites | **19** |

The raw scan uses the ELF **image mapping**, not file offsets. An earlier attempt indexed the file
buffer directly; code file offsets differ from virtual addresses, so it found only 4 false
candidates and zero real calls. The image-mapped scan is the correct one.

The scanner is bounded to the campaign candidates: mapped starts whose FDE size is 1, 4 or 5 bytes
(stubs/setters), all resolved indirect targets, and `0x20d80`/`0x9844b0`. An initial unbounded
variant that scanned every one-byte FDE start produced **570** rows / **109** blocked by admitting
unrelated trivial stubs; that was rejected as inflation, not evidence.

## 2. One new registration

`0xa1cc0` is a 1-byte `retq` FDE start with no entry-map name. The full scan finds exactly one
direct caller:

- site `0x256c38` in function `0x2569d0` (`callq 0xa1cc0`);
- backward `rax` liveness over the complete `0x2569d0` body: **dead**;
- no other direct site and no resolved indirect target.

It is registered as `retail_void_stub` / `void_noop`, so the surface is **91 -> 92**:

| Kind | Before | After |
| --- | ---: | ---: |
| `void_noop` | 22 | **23** |
| `const_u32` | 28 | 28 |
| `zero_u32` | 4 | 4 |
| `set_u8` | 3 | 3 |
| `set_u32` | 2 | 2 |
| `vtable_pair_ctor` | 31 | 31 |
| `concmd_lazy_register` | 1 | 1 |
| **Total** | **91** | **92** |

## 3. Every remaining entry has a measured blocker

**24** entries stay unregistered. None is labelled merely "no call site":

- `0x20d80` — 3 direct sites (`0x21166` in `0x210b0`, `0x23770` and `0x238d9` in `0x235d0`), all
  `rax`-dead, 0 qword references. Blocker:
  `needs_live_manager_pointer_from_bss_0x2dfcbc8` (the 392 B body's non-null path).
- `0x9844b0` — `tail_jump_needs_unmodelled_callee_and_rodata_leaq`; 2 resolved indirect sites,
  0 direct sites, 2 image qword references.
- 22 one-byte stubs — **no validated direct call anywhere in the 55,790 FDEs** and no resolved
  indirect target. Each carries `image_qword_references` (the measured in-image pointer count):

| RVA | qword refs | RVA | qword refs |
| --- | ---: | --- | ---: |
| `0x207d0` | 2 | `0x829ac0` | 2 |
| `0x23370` | 2 | `0x829b20` | 2 |
| `0x235c0` | 2 | `0xa32490` | 2 |
| `0x25c90` | 4 | `0xa40e20` | 2 |
| `0x41900` | 1 | `0xa4f4c0` | 2 |
| `0x140ce0` | 2 | `0xa4feb0` | 2 |
| `0x16a5d0` | 2 | `0xa50270` | 2 |
| `0x2111d0` | 2 | `0xa62490` | 2 |
| `0x49cc50` | 2 | `0xa62850` | 2 |
| `0x49ce50` | 2 | `0xb0e240` | 2 |
| `0x5388e0` | 0 | `0xacc5b0` | 0 |

The exact reason string is
`no_direct_call_in_55790_fdes_no_resolved_indirect_target` (plus `_qword_refs_N` when N > 0), and
the JSON also carries `direct_call_sites`, `indirect_call_sites`, `call_sites` and
`image_qword_references`. No blocked row has a live `rax` at a validated site.

## 4. Checker, observed effects, gates

`check_entry_closure.py` re-derives the full-FDE scan and requires:

- `native_thunks.scan` to match the re-derived FDE/byte/candidate/caller/instruction totals exactly;
- every registered row's `direct_call_sites`, `indirect_call_sites`, `call_sites` and
  `rax_live_sites` against the scan plus the resolved indirect records;
- every blocked row's counts and `image_qword_references` against the ELF;
- all 92 body hashes, FDE starts/sizes, generated-vs-custom placement and the dispatch slot.

The mutation probe now also tampers a direct-call count and the scan summary; both are detected.
The external JSON tamper remains detected (checker exit 1) and is reverted.

The runtime and fixture invoke the new `0xa1cc0` body against a `0x3c` canary and assert it is
byte-identical; observed `full_scan_stub_ok=true`, with the existing `byte_setter=255` and
`previous_targets={total:15,called:14,ok:true}` unchanged. `sotc_init_window` exits 0 with empty
stderr and `ok=true`.

Measured gates: build **0**; full CTest **706/706**; checker `native_thunks=92`; entry map
**1,957 / 584 / 462**; `run_all_checkers.sh` **915 passed / 0 skipped / 0 failed**, exit **0**;
project index **114,667**; publish-path audit `--all` exit **0**; attribution audit OK. External
tamper of `direct_call_sites` (7 instead of 1) made the checker exit **1**, then the file was
restored byte-for-byte and the checker was green again.

**Not claimed:** cold boot, audible audio, real save/reload and Metal validation are **NOT RUN**.
