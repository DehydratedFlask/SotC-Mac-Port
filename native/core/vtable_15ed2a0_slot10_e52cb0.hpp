// Vtable slot resolved by the vtable-closure sweep: `0x15ed2a0 + 0x10 -> 0xe52cb0`.
//
// Evidence:
//   * `0xe52d30` (inside this very body) is `call *0x10(%rax)` with `%rax` from the
//     RIP-relative table `0x15ed2a0`;
//   * the qword at `0x15ed2a0 + 0x10` in the pinned image is `0xe52cb0`;
//   * the dynamic relocation table has one type-8 row at `0x15ed2b0` with addend `0xe52cb0`;
//   * `0xe52cb0` is an FDE function start with length `0xfe`.
//
// Body (254 B) is a three-owner teardown:
//   *(void**)this = 0x15ed2b0;                     ; install the base vtable
//   if (this->entry_count > 0)                     ; +0x70 (i32), entries at +0x60
//       for (i = 0; i < count; ++i) {
//           record = entry_base + i*0x10;
//           if (record->flag_byte == 0) continue;  ; flag at record+0x8
//           if (record->pointer == null) continue; ; pointer at record+0x0
//           manager->vtable[0xd0](manager, record->pointer);   ; manager = *(this+0x8)
//       }
//   if (this->element_count > 0)                   ; +0x58 (i32), array at +0x48
//       for (i = 0; i < count; ++i)
//           if (array[i] != null) array[i]->vtable[0x10](array[i]);
//   if (this->elements_owned != 0) manager->vtable[0xc0](manager, this->elements);  ; +0x50/+0x48
//   if (this->entries_owned  != 0) manager->vtable[0xc0](manager, this->entries);   ; +0x68/+0x60
//   if (this->third_owned    != 0) manager->vtable[0xc0](manager, this->third);     ; +0x80/+0x78
//   jmp 0xe1ede0                                   ; base destructor
//
// Modelled: the vtable stamp, both teardown loops with the live count/array reloads, the three
// conditional buffer releases, and the base-destructor tail call as a hook. Not modelled: the
// manager, element and base-destructor bodies. Every read is unconditional except the documented
// flags, so a missing cell or a hook needed on the path is a visible failure.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0Rva = 0xe52cb0;
inline constexpr std::size_t kVtable15ed2a0Slot10_e52cb0Size = 0xfe;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0EndAddress = 0xe52dae;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0VtableAddress = 0x15ed2a0;
inline constexpr std::uint32_t kVtable15ed2a0Slot10_e52cb0SlotOffset = 0x10;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0RelocAddress = 0x15ed2b0;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0InstalledTable = 0x15ed2b0;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0ManagerOffset = 0x08;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0EntryCountOffset = 0x70;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0EntryBaseOffset = 0x60;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0EntryStride = 0x10;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0EntryFlagOffset = 0x08;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0ElementCountOffset = 0x58;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0ElementArrayOffset = 0x48;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0ElementsOwnedOffset = 0x50;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0EntriesOwnedOffset = 0x68;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0ThirdOwnedOffset = 0x80;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0ThirdBufferOffset = 0x78;
inline constexpr std::uint32_t kVtable15ed2a0Slot10_e52cb0EntryFreeSlot = 0xd0;
inline constexpr std::uint32_t kVtable15ed2a0Slot10_e52cb0BufferReleaseSlot = 0xc0;
inline constexpr std::uint32_t kVtable15ed2a0Slot10_e52cb0ElementDestroySlot = 0x10;
inline constexpr std::uint64_t kVtable15ed2a0Slot10_e52cb0BaseDtorRva = 0xe1ede0;

struct Vtable15ed2a0Slot10_e52cb0Cells {
    void* self = nullptr;                        // rdi at entry
    void** vtable_field = nullptr;               // this+0x00, receives 0x15ed2b0
    void* const* manager_field = nullptr;        // this+0x08
    std::int32_t* entry_count = nullptr;          // this+0x70
    std::uint8_t* const* entry_base_field = nullptr;  // this+0x60
    std::int32_t* element_count = nullptr;        // this+0x58
    void* const* const* elements_field = nullptr; // this+0x48
    const std::uint8_t* elements_owned = nullptr; // this+0x50
    const std::uint8_t* entries_owned = nullptr;  // this+0x68
    const std::uint8_t* third_owned = nullptr;    // this+0x80
    const void* const* third_field = nullptr;     // this+0x78
};

struct Vtable15ed2a0Slot10_e52cb0Hooks {
    // Manager vtable slot 0xd0: called with the manager and the entry pointer.
    void (*free_entry)(void* manager, void* entry) = nullptr;
    // Manager vtable slot 0xc0: called with the manager and the buffer.
    void (*release_buffer)(void* manager, const void* buffer) = nullptr;
    // Element vtable slot 0x10: called with the element.
    void (*destroy_element)(void* element) = nullptr;
    // Stands in for the `jmp 0xe1ede0` base destructor.
    void (*base_dtor_e1ede0)(void* self) = nullptr;
};

struct Vtable15ed2a0Slot10_e52cb0Result {
    bool ok = false;
    bool vtable_written = false;
    std::uint32_t entries_freed = 0;
    std::uint32_t elements_destroyed = 0;
    std::uint32_t buffers_released = 0;
    bool base_dtor_called = false;
};

[[nodiscard]] Vtable15ed2a0Slot10_e52cb0Result vtable_15ed2a0_slot10_e52cb0(
    const Vtable15ed2a0Slot10_e52cb0Cells& cells = {},
    const Vtable15ed2a0Slot10_e52cb0Hooks& hooks = {}) noexcept;

}  // namespace sotc
