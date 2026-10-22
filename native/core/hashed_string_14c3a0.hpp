// Native recovery of original helpers 0x14c3a0, 0x14c4a0 and 0xf730 (ELF sha256 d0723afb...5861d3),
// task CPU-01. Names and field names are ours; the original has no symbols.
//
// Shapes observed in the machine code (offline analysis only):
//   SharedStr (0x20 bytes): {ptr @0, len @8, aux @0x10, ctl @0x18}. `ctl` is either null or the address
//     of an int64 counter that every copy increments with `lock inc qword [ctl]`. ptr/len are the
//     {ptr,len} pair consumed by 0x161f80; `aux` is only ever copied (its meaning is not recovered).
//   HashedStr (0x28 bytes): {u32 hash @0, 4 bytes never touched @4, SharedStr @8}.
//
//   0x14c3a0 (this=rdi, src=rsi): copy_hashed. Copies hash (dword), then 16 bytes @8, aux @0x18, ctl
//     @0x20, then `if (ctl) lock inc qword [ctl]`. The four padding bytes @4 are NOT copied.
//   0x14c4a0 (this=rdi, esi ignored/overwritten, src=rdx -> SharedStr): make_hashed. Copies the 0x20
//     bytes to this+8 (ctl incremented when non-null), then this->hash = fnv_span_161f80({ptr,len},
//     0x811c9dc5). The caller-provided esi is never read. dst and src must not overlap.
//   0xf730: function-local static empty SharedStr at 0x2dd9f50 (Itanium guard byte at 0x2dd9f70, i.e.
//     __cxa_guard_acquire/release, a C++ runtime protocol, not game behaviour): ptr = &0x2dd9f78 (a
//     zero byte 0x28 after the object), len = aux = ctl = 0. The original also registers the dtor
//     0xf7b0 with __cxa_atexit. EXPLICIT OMISSION: that dtor is `if (ctl) {release...}` and ctl is null
//     for this object (the object is never mutated by the code analysed), so at exit it executes only
//     `mov rax,[this+0x18]; test; je ret`. The native static is trivially destructible; nothing is
//     registered. Whether other code could mutate the original object is not analysed.
#pragma once
#include <cstddef>
#include <cstdint>

namespace sotc {

struct SharedStr {
    const std::uint8_t* data;   // +0x00
    std::int64_t size;          // +0x08
    std::int64_t aux;           // +0x10
    std::int64_t* ctl;          // +0x18  null, or 8-byte-aligned shared counter
};
static_assert(sizeof(SharedStr) == 0x20 && offsetof(SharedStr, data) == 0 && offsetof(SharedStr, size) == 8 &&
              offsetof(SharedStr, aux) == 0x10 && offsetof(SharedStr, ctl) == 0x18);

struct HashedStr {
    std::uint32_t hash;         // +0x00
    std::uint8_t gap04[4];      // +0x04 never read or written by these helpers
    SharedStr str;              // +0x08
};
static_assert(sizeof(HashedStr) == 0x28 && offsetof(HashedStr, hash) == 0 && offsetof(HashedStr, str) == 8);

// 0x14c3a0.
void copy_hashed_14c3a0(HashedStr& dst, const HashedStr& src) noexcept;

// 0x14c4a0.
void make_hashed_14c4a0(HashedStr& dst, const SharedStr& src) noexcept;

// 0xf730. First call builds the static (thread-safe compiler guard); every call returns the same object.
// `data` points 0x28 bytes after the object, at a zero byte, exactly as the original layout.
[[nodiscard]] const SharedStr& empty_string_f730() noexcept;

// Offset of the byte `empty_string_f730().data` designates relative to the object (original 0x2dd9f78 - 0x2dd9f50).
inline constexpr std::size_t kEmptyStringBufferOffsetF730 = 0x28;

}  // namespace sotc
