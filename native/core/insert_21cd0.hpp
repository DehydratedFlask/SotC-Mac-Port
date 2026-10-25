// Native lift of original function RVA `0x21cd0` — the B-insert body behind Init
// `0x1290`'s second call (E516). This is a *bump-allocator* vector insert, not a
// linked-list insert — a different container path than the `0x1beeb0` list-3 insert.
//
// ELF sha256 d0723afb…861d3. Body `0x21cd0..0x21d9c` = 204 B (0xCC), fn sha256
// 2fb96a55…53ef. Byte-verified against `local/re/eboot.elf`:
//
//   entry contract (from 0x1290 call at 0x1334):
//     rdi = owner   (*(0x2dfcbc8)),   rsi = &B (the second 0x20 B descriptor)
//   own frame: push rbp; mov rbp,rsp; push r15/r14/r12/rbx
//   own guard: read from cell 0x15fe518, two-level deref [r12]→[rax]
//
// Recovered effect order:
//   1. Build stack aggregate {code=0xf95df0, code=0xf95c06, int=0xea} at [rbp-0x40]
//      (assert metadata: function signature + source file path + exception code).
//   2. Call begin-thunk 0x3b390(rdi=owner+0x58, rsi=&aggregate)
//      (a GOT/PLT import stub: likely scoped-lock enter or exception guard; modelled
//       as a hook since the original can only abort, never meaningfully succeed).
//   3. Capacity check: owner[0x40] (bump pos) vs owner[0x48] (capacity end).
//      If pos >= end → grow path (step 3b).
//   3a. Fast path (room available):
//       - bump owner[0x40] += 0x20
//       - zero slot[0x00..0x10) (vxorps + vmovups 16 B)
//       - copy descriptor[0x08..0x18) → slot[0x08..0x18) (vmovdqu 16 B: invoke+offset)
//       - copy descriptor[0x00] → slot[0x00] (mov 8 B: ctx)
//       - copy descriptor[0x18] → slot[0x18] (mov 8 B: fn)
//       - if fn != 0: self-link slot[0x00] = &slot[0x00]
//   3b. Grow path (pos >= end):
//       - call 0x23210(rdi=owner+0x38, rsi=&B)
//         (realloc+copy old entries+insert new; modelled as a hook).
//   4. Call end-thunk 0x3b3c0(rdi=owner+0x58) (release/exit; modelled as hook).
//   5. Stack canary compare → __stack_chk_fail(0xf8f3a0) on mismatch.
//
// This is a DIFFERENT container than 0x1beeb0:
//   - 0x1beeb0: 0x60 B linked-list node, allocates via operator new(0x60),
//     push-front into list-3, incq count at +0x50.
//   - 0x21cd0:   0x20 B flat entry, bump-allocates in an existing buffer,
//     begin/end lock around insert, no count, no node vtable.
//
// NOT modelled: the begin/end thunks (0x3b390/0x3b3c0, GOT/PLT import stubs that
// can only abort — `int 0xea` is an exception/assert code, not a success path);
// the grow function 0x23210 (realloc+copy logic); the stack-canary fail path
// (read-only guard 0x15fe518); the two assert format strings (0xf95df0, 0xf95c06);
// the owner's allocation/construction (lives in 0x533100/0x2569d0, not orderable
// against Init statically — E522). G1 PARTIAL — fixture-level model only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInsert21cd0Rva = 0x21cd0;
inline constexpr std::size_t kInsert21cd0Size = 204;
inline constexpr std::uint64_t kInsert21cd0EndRva = 0x21d9c;
inline constexpr std::uint64_t kInsert21cd0BeginThunk = 0x3b390;
inline constexpr std::uint64_t kInsert21cd0EndThunk = 0x3b3c0;
inline constexpr std::uint64_t kInsert21cd0GrowFn = 0x23210;
inline constexpr std::uint64_t kInsert21cd0GuardCell = 0x15fe518;
inline constexpr std::uint64_t kInsert21cd0StackChkFail = 0xf8f3a0;
inline constexpr std::uint64_t kInsert21cd0Code1 = 0xf95df0;    // func sig string
inline constexpr std::uint64_t kInsert21cd0Code2 = 0xf95c06;    // src path string
inline constexpr std::uint32_t kInsert21cd0ExceptionCode = 0xea;
inline constexpr std::size_t kInsert21cd0EntrySize = 0x20;      // slot width (bump stride)
inline constexpr std::size_t kInsert21cd0LockOffset = 0x58;     // owner+0x58 → lock handle
inline constexpr std::size_t kInsert21cd0GrowArgOffset = 0x38;  // owner+0x38 → grow arg
inline constexpr std::size_t kInsert21cd0BumpOffset = 0x40;     // owner[0x40] = bump pos
inline constexpr std::size_t kInsert21cd0EndOffset = 0x48;      // owner[0x48] = capacity end
inline constexpr std::size_t kInsert21cd0CopySize = 0x20;       // total bytes copied

// 0x20 B entry stored into the vector. Matches the HandlerEntry1290 layout: the
// original copies descriptor[0x00](ctx) + descriptor[0x08..0x18)(invoke+offset) +
// descriptor[0x18](fn), zero-initialises first, then self-links +0x00 if fn != 0.
struct Entry21cd0 {
    std::uintptr_t ctx;       // +0x00  descriptor[0x00]; self-linked when fn != 0
    std::uintptr_t invoke;    // +0x08  descriptor[0x08]
    std::uintptr_t offset;    // +0x10  descriptor[0x10]
    std::uintptr_t fn;        // +0x18  descriptor[0x18]
};
static_assert(sizeof(Entry21cd0) == kInsert21cd0EntrySize);
static_assert(offsetof(Entry21cd0, ctx) == 0x00);
static_assert(offsetof(Entry21cd0, fn) == 0x18);

// Container model. The original owner is `*(0x2dfcbc8)`, allocated by the
// 0x533100/0x2569d0 path and NOT orderable against Init statically (E522). Only the
// fields the insert writes/reads at the known ELF offsets are modelled; the +0x00..+0x38
// prefix, +0x50 region, and +0x60.. tail are opaque.
//
// Offsets touched by 0x21cd0:
//   +0x38 — dereferenced by the grow function 0x23210 as its first argument
//   +0x40 — bump position (incremented by 0x20 on each insert)
//   +0x48 — capacity limit (compared against bump; jae → grow)
//   +0x58 — lock handle passed to the begin/end thunks
struct Container21cd0 {
    // +0x00..+0x38: opaque prefix (contains the grow function's vector control ptr at +0x38)
    std::uintptr_t _prefix[7];  // 0x00..0x38
    // +0x38: pointer dereferenced by grow 0x23210. In the original, this is likely a
    // pointer to a std::vector-like control block.
    std::uintptr_t grow_vector_ptr;
    // +0x40: bump position — in the original this is a raw pointer into the vector's
    // storage. Modelled here as a derived field (count * 0x20 offset into `data`).
    std::uintptr_t _bump;       // computed: reinterpret_cast<uintptr_t>(data + count)
    // +0x48: capacity end — raw pointer limit.
    std::uintptr_t _end;        // computed: reinterpret_cast<uintptr_t>(data + capacity)
    // +0x50..+0x58: opaque padding
    std::uintptr_t _pad50[2];
    // +0x58: lock handle passed to begin/end thunks
    std::uintptr_t lock_handle;

    // Native-only fields (not in the original ELF layout): the underlying storage.
    Entry21cd0* data = nullptr;
    std::size_t count = 0;
    std::size_t capacity = 0;

    // Synchronise the native fields into the ELF-offset fields.
    void sync_bump_end() {
        _bump = reinterpret_cast<std::uintptr_t>(data + count);
        _end  = reinterpret_cast<std::uintptr_t>(data + capacity);
    }
};
static_assert(sizeof(Container21cd0) >= 0x60,
              "Container21cd0 must cover at least the ELF offsets through +0x58");

// 0x20 B descriptor read by the insert. The original only reads [0x00], [0x08..0x18),
// and [0x18]; the +0x20..+0x50 tail is NOT read (no vcall/table access here).
struct Descriptor21cd0 {
    std::uintptr_t ctx;        // 0x00 → entry[0x00]
    std::uintptr_t invoke;     // 0x08 → entry[0x08]
    std::uintptr_t reserved;   // 0x10 → entry[0x10]
    std::uintptr_t fn;         // 0x18 → entry[0x18]; tested for self-link
};
static_assert(sizeof(Descriptor21cd0) == 0x20);

// Explict alloc seam for the vector storage. The grow function 0x23210 is NOT lifted;
// when the fast path is full, the caller must supply a grow hook that reallocates and
// copies. A null hook means the container is treated as fixed-capacity and the insert
// fails visibly when full.
struct Insert21cd0Hooks {
    // 0x3b390 begin thunk: called with (owner+0x58, &{code,code,int 0xea}).
    // The original can only abort (__cxa_throw or similar); modelled as an optional
    // observable hook. Null means skip the call (tests can observe no abort).
    void (*begin)(void* lock_handle, const void* aggregate) = nullptr;

    // 0x3b3c0 end thunk: called with (owner+0x58). Null means skip.
    void (*end)(void* lock_handle) = nullptr;

    // 0x23210 grow: called with (owner+0x38, &B) when the bump buffer is full.
    // Expected to reallocate `data`, update `capacity`, copy existing entries, and
    // insert the new entry. Returns true on success, false on allocation failure.
    // Null means fixed-capacity: insert fails when full.
    bool (*grow)(Container21cd0& container, const Descriptor21cd0& descriptor) = nullptr;
};

struct Insert21cd0Result {
    bool ok = false;               // false on allocation failure or null hook
    Entry21cd0* entry = nullptr;   // pointer to the newly inserted entry (in data[])
    std::size_t count = 0;         // container.count after insert
    bool self_linked = false;      // fn != 0
    bool begin_called = false;     // observable only
    bool end_called = false;
};

// Runs the recovered insert. `container` is the owner object with `data`/`count`/
// `capacity` already initialised (and `sync_bump_end()` already called). The begin/end
// hooks observe the lock handle at +0x58; the grow hook handles capacity expansion.
// Allocation failure or a full buffer without a grow hook is a visible failure.
// The original returns void; the result struct is native observability only.
[[nodiscard]] Insert21cd0Result insert_21cd0(Container21cd0& container,
                                             const Descriptor21cd0& descriptor,
                                             const Insert21cd0Hooks& hooks) noexcept;

}  // namespace sotc
