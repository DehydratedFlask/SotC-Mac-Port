# Pointer and packed-data schemas (CPU-02, partial) — 2026-10-19

Evidence labels: **OBS** observed in the exact ELF (sha256 d0723afb…5861d3) by byte-exact match; **SRC** claimed by
pinned reference source/comments only; **INF** inferred; **OPEN** unknown. Native types: `native/core/addr.{hpp,cpp}`.

## Address classes

| Class | Representation | Status |
|---|---|---|
| `Tagged31` game field | u32. bit31 set: heap-relative, address = `[heap_base] + (v & 0x7fffffff)`. bit31 clear: absolute 31-bit value; 0 is null | decode rule **OBS** (tie: `tools/lift/check_f66dec0.py`); meaning of "absolute" values (image base, e.g. 0x400000 per SRC probe.c:55) **OPEN** |
| Heap window | two 64-bit runtime cells: base @0x2f3b8c0, limit @0x2f08000 (bss, filled at runtime) | **OBS** (tie: `tools/lift/check_heap_window.py`): the allocator hands back a region R via `0x19ae0` (see below); `start=R[0x60]`, `end=start+R[0x58]` are copied into a Region struct at 0x2f3b8c8 (+0x30 start, +0x38 end, +0x40 name "Shadow B…"); then `base = [0x2f3b8f8 (= Region.start)] & 0xffffffff80000000` (init function 0x57d5a0) and `limit` cell 0x2f08000 = region end |
| Allocator region API `0x19ae0` | SysV `(rdi, rsi, edx, ecx)` → pointer to R or null | **OBS** (tie: `tools/lift/check_f19ae0.py`, native `allocator_region.*`): `rsi`=byte size → `R[+0x58]`; `edx`=alignment (ceil 64 KiB / later 1 MiB); `ecx`=flags (`1` = fast path when other args zeroed in the forwarder); `rdi`=preferred/context (0 at Shadow site). Forwards to `0x19e00` as `(rdi=r12, rsi=0, rdx=0, rcx=size, r8=0, r9=align, [rsp]=flags)`. Shadow call: `(0, 0x6400000, 0x20000000, 1)`. Carve-fit math lifted (`free_list_carve.*`); node/list/bitmap open |
| Host map (SRV-01 mem) | Darwin `mmap` RW anon + over-alloc align | **OBS** call sites (tie: `tools/lift/check_host_map_calls.py`) + native `host_map.*`: `ReserveVirtual` PLT callers `0x19891`/`0x19fb2` (align ≥ 1 MiB); `MapFlexible` @`0x198b1` (`prot=0x33`,`flags=0x10`). Native `map_region` covers `preferred_base==0` only; nonzero preferred → nullopt (no MAP_FIXED policy yet) |
| Free-list carve `0x1a800` | fit/align only → `CarveFit` | **OBS** (tie: `tools/lift/check_f1a800_carve.py`, native `free_list_carve.*`): page budget `req>>16`; lead=`align_up(start)-start`; lead==0 consumes whole free node; else need `lead+req≤free_size` and allocated size=`free_size-lead` (not trimmed). Node alloc/list/bitmap **not** lifted. MAP_FIXED skipped: `0x479c2` rdi is object context |
| Region split `0x1a8b6` | free→neu field surgery | **OBS** (tie: `tools/lift/check_f1a8b6_split.py`, native `region_split.*`): after `0x19660` pop, `neu.{size,start,cap}=free-lead`, copy flag_a, flag_b=0, +0x40=0, +0x48=~0; `free.size/cap=lead`. Full `0x19660` replenish via `0x19e00` OPEN |
| Region list `0x1a902` | intrusive links @+0x20 | **OBS** (tie: `tools/lift/check_f1a902_list.py`, native `region_list.*`): insert_before / unlink self-loop / head insert via `[arena+0x10]`; owner @+0x38; free-count dword `[arena+0x48]` inc on insert_before / dec on unlink; alloc-count `[arena+0x20]` on head insert. Page bitmap still OPEN (no `bt*` in `0x18000..0x22000`). Full `0x19660` replenish still OPEN |
| Retag rule | field fix-up: if untagged, nonzero, and `[base] + off < [limit]` (unsigned 64-bit) then set bit31 | **OBS** |
| Type dispatcher @0x66dec0 | decode object Tagged31; `type-=2`; if `type>12` done; jump table @0x1270354 (13 entries) | **OBS** (tie: `tools/lift/check_f66dec0_dispatch.py`); native `retag_field_offset` / `dispatch_retag`. Arms: type2→+0x0c; 3–6→+0x20; 7–11 nop; 12/14→+0x18; 13→+0x14 |
| 40-bit GPU descriptor address | u64 < 2^40 | **SRC** only (runtime_memory.c header: user range below 1 TiB, descriptors encode 40 bits). **No** 40-bit mask was found in the game code; `Gpu40` is a native guard type, not a recovered layout |
| Allocator page pointers | 48-bit-masked (`0xffffffff0000`) page index << 16 inside a custom heap manager near 0x19940 | **OBS** idiom, internal to the allocator; not a game-data format |
| Host pointers | never stored in 32-bit fields | native rule; `addr.hpp` offers no conversion |

## Census (tools/lift/survey_packed_ptr.py; leads, not proofs)

Direct encodings in the code segment: `and r32,0x7fffffff` 2,939 sites, `or r32,0x80000000` 356, `shr r32,31` 547,
`shl r64,24` 180; busiest cluster 0x66e000–0x66f000 (tagged-field helpers). Many `& 0x7fffffff` sites are not
pointers (e.g. container capacity words with a flag in bit31 at +0x1c of a vector: seen in 0xe0e140). Do not read the
count as "number of pointer fields".

## Rules recorded for native code

* Null: untagged zero. A tagged zero offset decodes to the heap base (legal to decode; the retag rule never produces it).
* Width: offset is exactly 31 bits (0..0x7fffffff); the tag bit never participates in offsets.
* Window: `HeapWindow::from_region(start,size)` = {start & ~0x7fffffff, start+size}; native rejects regions that overflow or exceed the 2 GiB window (the original does not check).
* Overflow: original `base + off` wraps modulo 2^64; native `decode` returns nullopt and `tag_if_in_heap` refuses (documented difference).
* Alignment: no alignment rule was observed for Tagged31 fields; do not assume one. Page granularity 16 KiB is SRC for kernel mappings only.
* Serialization: never persist host pointers; persisted/asset fields keep the raw u32 and are decoded against an explicit `HeapWindow`.
* Static (untagged) values cannot become host pointers by arithmetic: the native image is not mapped at its console address.
  They must be resolved through an explicit table once their targets are recovered (OPEN).

## Open questions (next actions)

1. ~~Who writes the heap base/limit cells?~~ **Answered** (see table): region start/end from the allocator, base = start rounded down to a 2 GiB boundary. Still open: the region's actual runtime size/placement, and whether the one-sided `base+off < limit` test can tag offsets below the region start (it can by the observed code; reachable only if such values exist).
2. ~~Allocator region API args / R layout?~~ **Answered** (contract + host map preferred_base==0 + carve-fit math at `0x1a800`; see `free_list_carve.*`). List/free-count surgery lifted (`region_list.*`). Still open: node factory `0x19660`, page bitmap, used-byte high-water `@+0x80/+0x90`, full arena lifecycle. MAP_FIXED not adopted — nonzero rdi at `0x479c2` is object context.
3. What do untagged absolute values point at: image offset (+0x400000?) — a quick string-landing test was inconclusive (60% vs 27%, weak). Needs a data-structure-level proof (e.g. a known table with both forms).
4. Where are packed asset structures read (PSARC members use extra encodings, docs/ASSETS.md)? Their schemas come after loader recovery.
5. ~~13-arm dispatcher at 0x66dec0~~ **Lifted** (see table + `dispatch_retag`). Type names / full object layouts for those types still OPEN.
