// Resolved slot `vtable 0x1594dc0 + 0x10 -> 0x4b0` (113 B): an array teardown.
//
// Body:
//   *(u64*)this = 0x1594dc0 + 0x10            ; install the base vtable (0x1594dd0, not the vtable head)
//   for (element = *(u64*)(this+0x80); element != *(u64*)(this+0x88); element += 0x50) {
//       release_f590(element + 0x28);
//       release_f590(element + 0x8);
//   }
//   if (*(u64*)(this+0x80) != null) free_170c0(*(u64*)(this+0x80));   ; tag-2 free of the array
//   jmp 0x20b50(this)                                                ; post-pass tail call
//
// The two per-element releases are `0xf590`, the shared-str release used across the registry code; a
// host supplies them as hooks because that body is modelled elsewhere. The array free uses the native
// `free_170c0` (tag 2), so the allocation contract is the same one the other teardowns take.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1594dc0Slot10_4b0Rva = 0x4b0;
inline constexpr std::size_t kVtable1594dc0Slot10_4b0Size = 0x71;
inline constexpr std::uint64_t kVtable1594dc0Slot10_4b0EndAddress = 0x521;
inline constexpr std::uint64_t kVtable1594dc0Address = 0x1594dc0;
inline constexpr std::uint32_t kVtable1594dc0Slot10 = 0x10;
inline constexpr std::uint64_t kVtable1594dc0Slot10Reloc = 0x1594dd0;
inline constexpr std::uint64_t kVtable1594dc0Slot10InstalledTable = 0x1594dd0;
inline constexpr std::uint64_t kVtable1594dc0Slot10ArrayBegin = 0x80;
inline constexpr std::uint64_t kVtable1594dc0Slot10ArrayEnd = 0x88;
inline constexpr std::uint64_t kVtable1594dc0Slot10ElementStride = 0x50;
inline constexpr std::uint64_t kVtable1594dc0Slot10ElementFieldA = 0x28;
inline constexpr std::uint64_t kVtable1594dc0Slot10ElementFieldB = 0x08;
inline constexpr std::uint64_t kVtable1594dc0Slot10PostCallRva = 0x20b50;
inline constexpr std::uint64_t kSharedReleaseF590Rva = 0xf590;

struct Vtable1594dc0Slot10_4b0Cells {
    void* self = nullptr;
    void** vtable_field = nullptr;          // *this
    const void* const* array_begin = nullptr;  // this+0x80
    const void* const* array_end = nullptr;    // this+0x88
};

struct Vtable1594dc0Slot10_4b0Hooks {
    // `0xf590` on one element field.
    void (*release_element)(void* field) = nullptr;
    // The post-pass at `0x20b50`.
    void (*post_call)(void* self) = nullptr;
    // Allocation dispatch for the tag-2 free of the element array.
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable1594dc0Slot10_4b0Result {
    bool ok = false;
    void* vtable_written = nullptr;
    std::uint32_t elements = 0;
    std::uint32_t releases = 0;
    bool array_freed = false;
    std::uint32_t free_tag = 0;
    bool post_called = false;
};

[[nodiscard]] Vtable1594dc0Slot10_4b0Result vtable_1594dc0_slot10_4b0(
    const Vtable1594dc0Slot10_4b0Cells& cells = {},
    const Vtable1594dc0Slot10_4b0Hooks& hooks = {}) noexcept;

// `vtable 0x1594dc0 + 0x18 -> 0x22a60` (0x79 B): the same array teardown with a different ending.
// The 0x4b0 variant *tail-calls* the post-pass at `0x20b50`; this one **calls** it and then tail-calls
// the tag-1 free at `0x170b0`, so it also releases `this` itself.
inline constexpr std::uint64_t kVtable1594dc0Slot18_22a60Rva = 0x22a60;
inline constexpr std::size_t kVtable1594dc0Slot18_22a60Size = 0x79;
inline constexpr std::uint32_t kVtable1594dc0Slot18 = 0x18;
inline constexpr std::uint64_t kVtable1594dc0Slot18Reloc = 0x1594dd8;

struct Vtable1594dc0Slot18_22a60Result {
    bool ok = false;
    void* vtable_written = nullptr;
    std::uint32_t elements = 0;
    std::uint32_t releases = 0;
    bool array_freed = false;
    std::uint32_t array_free_tag = 0;
    bool post_called = false;
    bool self_freed = false;
    std::uint32_t self_free_tag = 0;
};

[[nodiscard]] Vtable1594dc0Slot18_22a60Result vtable_1594dc0_slot18_22a60(
    const Vtable1594dc0Slot10_4b0Cells& cells = {},
    const Vtable1594dc0Slot10_4b0Hooks& hooks = {}) noexcept;

}  // namespace sotc
