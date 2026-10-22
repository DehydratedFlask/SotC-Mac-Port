// Native recovery of original ctor 0x14c9a0 (parent == nullptr path ONLY), its guarded-static getter
// 0x14d880 and the 400-iteration startup ctor 0xb25470 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Names and field names are ours; the original has no symbols; offline analysis only.
//
// 0x14c9a0 (this=rdi, parent=rsi), observed stores (everything else in the 0x100-byte object is untouched):
//   +0x00 = 0, +0x08 = this, +0x10 = this, +0x18 = 0, +0x20 = parent
//   xchg-stores of 0: dword +0x30, dword +0x40, qword +0x50 (seq_cst exchanges); +0x60 dword = 0, +0x68 qword = 0
//   +0x70 HashedStr  <- copy_hashed_14c3a0(static hashed empty string)   (hash dword, 16 B, aux, ctl)
//   +0x98 dword = 0xb;  +0xa8..+0xc7 = 32 zero bytes (one ymm store)
//   +0xc8 HashedStr  <- copy_hashed_14c3a0(static hashed empty string)
//   +0xf0 dword = 0, +0xf4 dword = 0xb, +0xf8 byte = 0
// The static hashed empty string (original object 0x2ed3640, guard byte 0x2ed3668, two compiler-inlined
// copies of the same guarded initialiser inside 0x14c9a0) is make_hashed_14c4a0(empty_string_f730()):
// hash 0x811c9dc5, ptr = the zero byte of the 0xf730 object, len = aux = ctl = 0.
//
// UNSUPPORTED CONTRACT: for parent != nullptr the original copies the parent's string at +0x30..
// (releasing the old one through 0xf590, a shared-string/allocator release with TLS and indirect vcalls)
// and then tail-jumps through the manager object's vtable slot (global 0x2ee2d00, +0x20). That path is
// NOT lifted. ctor_14c9a0 aborts the process with a diagnostic (always, not only in debug builds)
// BEFORE writing anything; it never silently models that path.
//
// 0x14d880 (no arguments): guarded static getter for the object 0x2ee3b10 through the pointer cell
// 0x2ee3b00 (guard byte 0x2ee3c10):
//     if (cell != null) return cell;
//     [guard] ctor_14c9a0(object, nullptr) ; [atexit(0x14cb70, object) -- OMITTED, see below] [/guard]
//     cell = &object ; lock inc dword [object+0x30] (inlined 0x14ccb0) ; return &object
// The counter is therefore incremented every time the cell is found null, including after the guard is
// already set. The returned rax is &object (or the cell contents on the early return).
// EXPLICIT OMISSION: the original registers the object destructor 0x14cb70 with __cxa_atexit. That
// destructor is NOT lifted: its transitive callees (0x14ccc0, 0x14d2d0, 0xf590/0x12af0 allocator
// internals) are non-leaf with indirect vcalls. The native object is trivially destructible and nothing
// is registered at exit, so exit-time release of any strings/vector elements later stored in the object
// is not modelled. (With the +0xa8 vector empty and the string ctl fields null as built here, the
// original destructor performs no release.) Likewise the exit-time destructor 0x95610 registered for
// the static hashed empty string is omitted (its ctl is null, so it would do nothing).
//
// 0xb25470 (ctor, 400 iterations, stride 0x118): for element i of the array at 0x332f650
//     p = getter_14d880(); elem.node(+0x108) = p; elem.flag110 = 0; add_ref_14ccb0(p); elem.flag00 = 0;
// then registers destructor 0xb24be0 with __cxa_atexit (OMITTED, same policy). Other bytes of each
// element are not written. Consumers of the array and of the node counter are not analysed. Not
// registered in startup_ctors.
#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

#include "hashed_string_14c3a0.hpp"

namespace sotc {

struct Object14c9a0 {
    std::uint64_t q00;              // +0x00 = 0
    Object14c9a0* self08;           // +0x08 = this
    Object14c9a0* self10;           // +0x10 = this
    std::uint64_t q18;              // +0x18 = 0
    Object14c9a0* parent;           // +0x20
    std::uint8_t gap28[0x08];       // +0x28 never written
    std::int32_t a30;               // +0x30 exchanged to 0 (also the add_ref counter of 0x14ccb0)
    std::uint8_t gap34[0x0c];
    std::int32_t a40;               // +0x40 exchanged to 0
    std::uint8_t gap44[0x0c];
    std::int64_t a50;               // +0x50 exchanged to 0
    std::uint8_t gap58[0x08];
    std::int32_t d60;               // +0x60 = 0
    std::uint8_t gap64[0x04];
    std::int64_t q68;               // +0x68 = 0
    HashedStr s70;                  // +0x70..+0x97
    std::int32_t d98;               // +0x98 = 0xb
    std::uint8_t gap9c[0x0c];
    std::uint8_t zero_a8[0x20];     // +0xa8..+0xc7 = 0
    HashedStr sc8;                  // +0xc8..+0xef
    std::int32_t df0;               // +0xf0 = 0
    std::int32_t df4;               // +0xf4 = 0xb
    std::uint8_t bf8;               // +0xf8 = 0
    std::uint8_t gapf9[0x07];
};
static_assert(sizeof(Object14c9a0) == 0x100 && alignof(Object14c9a0) == 8);
static_assert(offsetof(Object14c9a0, parent) == 0x20 && offsetof(Object14c9a0, a30) == 0x30 &&
              offsetof(Object14c9a0, a40) == 0x40 && offsetof(Object14c9a0, a50) == 0x50 &&
              offsetof(Object14c9a0, d60) == 0x60 && offsetof(Object14c9a0, q68) == 0x68 &&
              offsetof(Object14c9a0, s70) == 0x70 && offsetof(Object14c9a0, d98) == 0x98 &&
              offsetof(Object14c9a0, zero_a8) == 0xa8 && offsetof(Object14c9a0, sc8) == 0xc8 &&
              offsetof(Object14c9a0, df0) == 0xf0 && offsetof(Object14c9a0, df4) == 0xf4 &&
              offsetof(Object14c9a0, bf8) == 0xf8);

inline constexpr std::int32_t kInitialD98 = 0xb;   // also df4

// Static hashed empty string (original object 0x2ed3640). Thread-safe first-use construction.
[[nodiscard]] const HashedStr& static_empty_hashed_2ed3640() noexcept;

// 0x14c9a0, parent == nullptr only. `parent != nullptr` aborts the process (see file comment).
void ctor_14c9a0(Object14c9a0& self, Object14c9a0* parent) noexcept;

// The singleton object (original 0x2ee3b10; zero-initialised like BSS) and its pointer cell (0x2ee3b00).
[[nodiscard]] Object14c9a0& static_object_2ee3b10() noexcept;
[[nodiscard]] std::atomic<Object14c9a0*>& registry_cell_2ee3b00() noexcept;

// 0x14d880.
Object14c9a0* getter_14d880() noexcept;

// 0xb25470 element (original array 0x332f650, stride 0x118, 400 elements = 0x1b580 bytes).
struct Slot118 {
    std::uint8_t flag00;            // +0x000 = 0
    std::uint8_t gap01[0x107];      // never written
    Object14c9a0* node;             // +0x108 = getter_14d880()
    std::uint8_t flag110;           // +0x110 = 0
    std::uint8_t gap111[0x07];      // never written
};
static_assert(sizeof(Slot118) == 0x118 && offsetof(Slot118, node) == 0x108 && offsetof(Slot118, flag110) == 0x110);

inline constexpr std::size_t kSlotCountB25470 = 400;
void init_ctor_b25470(std::array<Slot118, kSlotCountB25470>& slots) noexcept;

}  // namespace sotc
