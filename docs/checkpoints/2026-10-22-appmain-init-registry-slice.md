# Measured app-main Init→registry slice (G1 remains PARTIAL)

- Date/time: 2026-10-22 10:17 CDT (America/Chicago).
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9 (Drive).
- Task: `CPU-01-APPMAIN-INIT-REGISTRY-SLICE-01`; gate: **G1**; status: **PARTIAL** (not PASSED).
- Failed hypotheses/tries: 0.
- Tip: `8bca0ad` on `cursor/init-hash-551140-9dfc` (PR #4). Private manifest sha256 `d64beb03…`.

## Claim boundary (honest)

**Claimed:** a measured native ARM64 harness runs the app-main static edge
(vtable `0x15c8cf8` → Init `0x531d60`) into the already-lifted registry fragment
(`alloc 0x98` → fill `0x52a220` → store cell `0x2efd160`) through **tip**
`AllocatorFront` (+ FakeHeap). ELF pins and ctest/measured stdout recorded.

**Not claimed:** G1 PASSED; full Init body; pool/listener half of `0x52a220`;
PR #5 heap hierarchy as part of this tip’s binary; cold boot / audio / save / Metal.

## Allocator merge gap (cite-only)

PR #5 `cursor/heap-depth-5d26` tip `4d7e484` / pause `0510d35` carries Composite /
Bucket / HeapAllocator + bootstrap `0x13380` + lazy latch `0x2dfaaa0`
(**PR #5 evidence E399–E403**). Those commits are **not** ancestors of this Drive tip
(`git merge-base --is-ancestor` false). Drive consumes tip `AllocatorFront` only;
heap worktree untouched.

## Private evidence (gitignored)

Directory: `local/validation/appmain-init-registry-slice-20261022/`

Exact command:

```sh
python3 -I local/validation/appmain-init-registry-slice-20261022/capture.py
```

## Observed results

| Step | Result |
|---|---|
| ELF | `check_fappmain_init_registry_slice.py` OK — slot4 `0x15c8cf8`→`0x531d60`; callsite `0x25aa06` |
| Binary | `test_appmain_init_registry_slice` = Mach-O arm64 |
| CTest | `appmain_init_registry_slice` + `registry_init_531d60` PASS |
| Measured | entries=275; object_bytes=0x98; alloc_ops=3 via tip AllocatorFront+FakeHeap |
| Pairs | `check_ctor_slots` still **155** (slice not registered into `recovered_constructors`) |

## Unresolved edges (still block G1 PASSED)

1. Remainder of Init `0x531d60` beyond registry fragment
2. `0x52a220` pool/listener loop after `0x52a680`
3. PR #5 heap merge into Drive tip (when/if integrator asks)
4. idx **95** dormant; cliff **152+** demoted
5. Full app Run / CRT wiring of this slice
6. G5 cold boot / audio / real save — NOT RUN

## Gate status

- **G0:** PARTIAL
- **G1:** **PARTIAL** (E404 pack + this measured edge). **Not PASSED.**
- **G2–G6:** NOT RUN

## Next exact action

Close a named unresolved edge required for G1 PASSED (full Init callees that the
registry install depends on beyond the fragment, or merge PR #5 allocator depth when
integrator schedules it). Goal remains open.
