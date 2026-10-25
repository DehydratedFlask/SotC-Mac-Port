# Checkpoint — SharedStr resize/COW and terminate

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; integrator verification
- Task: `CPU-01-SHARED-STR-RESIZE-01`
- Gate: **FIXTURE_VERIFIED**

## Bodies

| function | span | sha256 |
|---|---|---|
| `0xf3c0` resize | `0xf3c0..0xf58b`, 459 B | `980c6911da8fad573162fd1d97f77b1dc20677bb7eedbee3e7e5fc8fe25227fb` |
| `0xf6b0` terminate | `0xf6b0..0xf6d0`, 32 B | `bfaf9b09a6f5f0345f8341cbc75da04f7ebe4dcf9ae0d697d86bbac82da25be1` |

Four paths:

1. `control == 0`: allocate `align4(newLen+4)` data and an 8-byte control, copy
   `min(size,newLen)`, `*control = 1`, capacity `alloc-1`.
2. `*control != 1`: allocate new data and control, copy `min`, `lock xadd -1` on the old control;
   free old data (tag 0) and old control (tag 1) only when previous == 1.
3. `*control == 1 && capacity >= newLen`: only `size = newLen`.
4. `*control == 1 && capacity < newLen`: `align16(newLen+0x10)` for `<= 0x80`,
   `align256(newLen+0x100)` for `<= 0x400`, else `align1024(newLen+0x400)`; copy old `size`, free
   old data (tag 0), keep the control block.

## Return correction

The audit table said paths 1/2 return true. The bytes show `xor r15d,r15d` at `0xf430`/`0xf50e` and
`mov r15b,1` only at `0xf576`: **only path 4 returns true**. Paths 1/2 still allocate; callers
observe that through the allocator hooks. The native API follows the bytes; `terminate` uses the
internal outcome enum so a path-1/2 resize is not mistaken for a rejection.

## Native

- `resize_shared_str_f3c0` and `terminate_shared_str_f6b0`, reusing `SharedStrCtorHooks`.
- Failure checks the original lacks: negative length, malformed holder, missing
  allocator/deallocator, failed allocation → false with the old holder intact.
- Path 2 allocates the replacement control before releasing the old reference, so a control
  failure cannot leave the holder pointing at freed memory (the original would crash).

## Fixture and checker

- All four paths, shared-control refcount, three size classes, `data[size] = 0`, and the failure
  preservation cases.
- Mutation: forcing path 3 to return true fails the test.
- `check_fshared_str_resize.py` pins both bodies and the three return paths.

## Limits

The guest allocator bodies and holder-field concurrency are not modelled. Path 1 does not free a
heap old buffer when `control == 0` (matches the original; normal input is the shared empty
static). G1 PARTIAL.
