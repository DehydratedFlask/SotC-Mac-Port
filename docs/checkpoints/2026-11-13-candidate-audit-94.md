# Candidate audit: none of the three helper RVAs is a 117th row

**Card:** `CPU-01-AOT-CFG-CALLABLE-01`, continued.

**Evidence:** E1449. Branch `cursor/init-hash-551140-6aa93e8`, base `33b26f9c`.

**Checkpoint:** local **2026-11-12 22:35 CST** (record date 2026-11-13), agent Buffy. This is a
guard-data audit, not a registration: no native code or generated row changed. Files changed are the
checkpoint/evidence/handoff/roadmap/tasks/index records only.

**State: green, 94 / 22 retained.** The previous pass exposed three unregistered bodies from the
registered `0x20d80` decode. This pass asks whether any is already a candidate in the bounded
**116-row** guard. None is. The required fallback is therefore a written per-candidate determination,
not a fabricated row and not a widened scan.

## 1. Guard census

The generated `docs/entry_closure.json` has:

```text
native_thunks.count = 94
native_thunks.blocked = 22
native_thunks.candidate_guard.candidate_rows = 116
native_thunks.candidate_guard.unbounded_one_byte_rows_rejected = 570
```

The 116 rows are exactly the 94 registered thunks plus the 22 blocked reference-audited stubs.
`0xf8f3d0`, `0xf8f3e0` and `0x42d30` occur in neither list. The generator's guard is deliberately
bounded to the campaign candidates; the 570-row unbounded one-byte population remains rejected
inflation.

## 2. Per-candidate determination

| RVA | Entry-map status | FDE owner / body | Guard status | Determination |
|---|---|---|---|---|
| `0xf8f3d0` | `kLookup21620GuardAcquire`, `kAppend16a380GuardAcquire` | **no FDE**; 6-byte `jmpq *GOT` PLT stub, followed by push/jmp resolver | not in 94 or 22 | **ineligible**: import/PLT thunk, not a candidate body; no FDE or direct-call liveness record |
| `0xf8f3e0` | `kLookup21620GuardRelease`, `kAppend16a380GuardRelease` | **no FDE**; 6-byte `jmpq *GOT` PLT stub, followed by push/jmp resolver | not in 94 or 22 | **ineligible**: import/PLT thunk, not a candidate body; no FDE or direct-call liveness record |
| `0x42d30` | not in entry map | FDE start, size **0x21c**; left `+0x0`, right `+0x8`, parent `+0x10`, color `+0x18` | not in 94 or 22 | **ineligible under the existing guard**: real body, but admitted only by adding a candidate row / widening the candidate class. Registering it would make the guard 117, which the task forbids |

`0x42d30` is not a phantom: `0x211d0` calls it directly at **`0x2138a`** with `rdi=tree_node`,
`rsi=parent`, `rdx=root` and `ecx=color`, immediately after the 0x48 B allocation at `0x2134a`.
That makes it a genuine contract helper, but it is not one of the 116 bounded candidates and has no
entry-map name. The direct call site is inside `0x211d0`, which is itself outside the guard; no
`0x20d80` row claims `0x42d30` as a direct target.

`0xf8f3d0`/`0xf8f3e0` are reached from `0x20d80` at `0x20e90`/`0x20eb9`, but the reached address is
a lazy PLT slot, not a function FDE. Their GOT slots are import guards, so treating them as native
bodies would invent an implementation behind an unresolved host import.

## 3. Runtime and frontier

The runtime is unchanged and still asserts:

```text
concmd_registry={size:3,rotations:1,global:2,null_ok:true,ok:true}
byte_setter=255
previous_targets={total:15,called:14,ok:true}
printf_string_ok=true
registered=94
```

Exit **0**, stderr **0 B**. No existing effect was weakened. The 22 stubs and the `0x9844b0`
printf row are untouched. The guard remains **116** bounded rows; the 570 unbounded variant remains
recorded as rejected.

## 4. Gates

Measured on this unchanged state: build **0**; full CTest **706/706**; checker `native_thunks=94`
with mutation probes for row/total/hash/direct count/scan/dispatch/reference resolution/candidate
guard; runtime exit **0**, stderr **0 B**; `run_all_checkers.sh` **915/0/0**; entry map
**1,957 / 584 / 462**; project index **114,667**; publish-path `--all` exit **0**; attribution OK.

**NOT RUN:** cold boot, audible game audio, save/reload after relaunch, Metal validation.

**Next exact action:** find a real call site and liveness proof for one of the 22 stubs, or a
candidate body that is already inside the 116-row guard. Do not add a candidate row.
