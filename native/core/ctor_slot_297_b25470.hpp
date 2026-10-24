// Native recovery of original constructor-table index 297 / RVA 0xb25470 (ELF sha256
// d0723afb...5861d3). The original is a 105-byte bulk initializer for the 400-record table at
// original .bss 0x332f650 (stride 0x118, extent [0x332f650, 0x334abd0)):
//
//   rbx = 0x110; r14 = &table;
//   loop:
//     rax = call 0x14d880                 ; guarded singleton getter (once-init)
//     mov [rbx+r14-8], rax                ; rec+0x108 = handle
//     mov byte [rbx+r14], 0               ; rec+0x110 = 0
//     call 0x14ccb0                       ; lock inc dword [handle+0x30] (retain)
//     mov byte [rbx+r14-0x110], 0         ; rec+0x000 = 0
//     rbx += 0x118; cmp rbx,0x1b690; jne loop
//   __cxa_atexit(0xb24be0, nullptr, 0x1604218)
//
// Only those three offsets are written; every other record byte is untouched (the native record type
// keeps them as explicit gaps, shared with core/registry_object_14c9a0). The exit destructor
// 0xb24be0 is a pinned, documented contract and is NOT executed natively:
//
//   0xb24be0 (89 B): rbx = 0x1b580; r14 = &table;
//   loop (records 399 -> 0):
//     if (rec+0x110 != 0) { call 0x14d2d0(rec+0x108); rec+0x110 = 0; }
//     call 0x14ccc0(rec+0x108); rec+0x108 = 0;
//     rbx -= 0x118; jne loop
//
// 0x14d2d0 (release dependency) and 0x14ccc0 (drop reference) are large non-leaf bodies that are
// not lifted, so registering an invented teardown would be dishonest. tools/lift's checker pins the
// destructor bytes, including the reverse order and every store offset.
//
// Missing contracts are visible, never success: a null acquire or retain leaves the table untouched
// and names the stage; a null handle stops before writing the record and keeps the exact prefix.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

#include "registry_object_14c9a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot297Rva = 0xb25470;
inline constexpr std::size_t kSlot297BodySize = 105;
inline constexpr std::size_t kSlot297Count = 400;
inline constexpr std::size_t kSlot297Stride = 0x118;
inline constexpr std::size_t kSlot297HandleOffset = 0x108;
inline constexpr std::size_t kSlot297FlagOffset = 0x110;
inline constexpr std::uint64_t kSlot297TableRva = 0x332f650;

inline constexpr std::uint64_t kSlot297AcquireRva = 0x14d880;   // guarded singleton getter
inline constexpr std::uint64_t kSlot297RetainRva = 0x14ccb0;    // lock inc dword [handle+0x30]
inline constexpr std::uint64_t kSlot297AtexitRva = 0xf8f390;    // __cxa_atexit PLT
inline constexpr std::uint64_t kSlot297DsoRva = 0x1604218;

// Registered exit destructor (pinned bytes; not executed natively).
inline constexpr std::uint64_t kSlot297DtorRva = 0xb24be0;
inline constexpr std::size_t kSlot297DtorSize = 0x59;
inline constexpr std::uint64_t kSlot297DtorReleaseRva = 0x14d2d0;
inline constexpr std::uint64_t kSlot297DtorDropRefRva = 0x14ccc0;

using Slot297Record = Slot118;   // 0x118 B, +0x108 handle, +0x110 flag (shared layout)
using Slot297Table = std::array<Slot297Record, kSlot297Count>;

static_assert(kSlot297Count == kSlotCountB25470);
static_assert(sizeof(Slot297Record) == kSlot297Stride);
static_assert(offsetof(Slot297Record, flag00) == 0x000);
static_assert(offsetof(Slot297Record, node) == kSlot297HandleOffset);
static_assert(offsetof(Slot297Record, flag110) == kSlot297FlagOffset);

struct Slot297Hooks {
    // 0x14d880: guarded once-init then the same owned object. Null => table untouched, "acquire".
    Object14c9a0* (*acquire_shared_14d880)() = nullptr;
    // 0x14ccb0: retain the handle. Null => table untouched, "retain".
    void (*retain_14ccb0)(void* handle) = nullptr;
};

// Hooks wired to the lifted primitives (getter_14d880 + add_ref_14ccb0). This is the production
// wiring; the explicit hook struct exists so tests and hosts can observe or substitute the stages.
[[nodiscard]] Slot297Hooks default_slot297_hooks() noexcept;

enum class Slot297Stop {
    none,           // every record completed
    acquire,        // acquire hook missing (nothing touched)
    retain,         // retain hook missing (nothing touched)
    acquire_null,   // acquire returned null; record i not touched
};

// Exact names used by the original analysis: "" / "acquire" / "retain" / "acquire(null)".
[[nodiscard]] const char* slot297_stop_name(Slot297Stop stop) noexcept;

struct Slot297Result {
    bool ok = false;
    std::size_t initialised = 0;         // records completed in original order
    Slot297Stop stopped_at = Slot297Stop::none;
    Object14c9a0* shared = nullptr;      // last handle stored
};

[[nodiscard]] Slot297Result init_slot297_b25470(Slot297Table& table,
                                                const Slot297Hooks& hooks = {}) noexcept;

}  // namespace sotc
