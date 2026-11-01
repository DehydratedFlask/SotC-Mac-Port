// Resolved vtable target `0xe21420` (reached from vtable `0x15e2028` slot `0x2a0`).
//
// Evidence: `0xe119be` is `call *0x2a0(%rax)` with `%rax` from the RIP-relative table `0x15e2028`;
// the qword at slot `0x2a0` is `0xe21420`; the dynamic relocation at `0x15e22c8` is type 8 with
// addend `0xe21420`; `0xe21420` is an FDE function start of length `0x2a9` (681 B).
//
// This is the largest resolved body: a name lookup with status codes and 19 indirect calls. Only its
// **decision contract** is modelled here; the terminal bodies it jumps to are hooks:
//
//   provider = *(ctx + 0x48)
//   obj      = provider->slot28(name)                  ; 0x1317064 is the name argument
//   if (obj == null)                      -> missing path, status 0x1b
//   if (obj->slot18() != 0x0e)            -> status 0x13
//   if (obj->slot20() != 1)               -> missing path, status 0x1b
//   if (name == null && provider->slot78() > 1) -> status 0x14
//   for (i = 0; i < provider->slot78(); ++i)
//       if (strcmp(provider->slot88(i), name) == 0) -> found path (0xe2167e, index i)
//   status 0x14
//
// NOT modelled: everything after the two jump targets `0xe214f7` (missing path: allocates a 0x48
// object, sets status 0x1b) and `0xe2167e` (found path), plus the rest of the 681-byte body. Those
// remain entry records; this model stops at the branch that selects them.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420Rva = 0xe21420;
inline constexpr std::size_t kVtable15e2028Slot2A0_e21420Size = 0x2a9;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420EndAddress = 0xe216c9;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420VtableAddress = 0x15e2028;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420SlotOffset = 0x2a0;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420RelocAddress = 0x15e22c8;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420ProviderOffset = 0x48;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420NameRva = 0x1317064;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420LookupSlot = 0x28;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420StateSlot = 0x18;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420KindSlot = 0x20;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420CountSlot = 0x78;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420NameSlot = 0x88;
inline constexpr std::uint32_t kVtable15e2028Slot2A0_e21420ExpectedState = 0x0e;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420ExpectedKind = 1;
inline constexpr std::int32_t kVtable15e2028Slot2A0_e21420StatusState = 0x13;
inline constexpr std::int32_t kVtable15e2028Slot2A0_e21420StatusScan = 0x14;
inline constexpr std::int32_t kVtable15e2028Slot2A0_e21420StatusMissing = 0x1b;
inline constexpr std::uint64_t kVtable15e2028Slot2A0_e21420StrcmpTarget = 0xf8f490;

enum class Vtable15e2028Slot2A0_e21420Path : std::uint8_t {
    none = 0,
    state_mismatch = 1,  // 0x13
    scan_exhausted = 2,  // 0x14
    missing = 3,         // 0x1b (jumps to 0xe214f7)
    found = 4,           // jumps to 0xe2167e
};

const char* vtable_15e2028_slot2a0_e21420_path_name(
    Vtable15e2028Slot2A0_e21420Path path) noexcept;

struct Vtable15e2028Slot2A0_e21420Cells {
    void* self = nullptr;
    void* provider = nullptr;      // *(ctx + 0x48)
    const char* name = nullptr;    // arg3 (rdx)
};

struct Vtable15e2028Slot2A0_e21420Hooks {
    // provider->slot28(name)
    void* (*provider_lookup)(void* provider, const char* name) = nullptr;
    // object->slot18()
    std::int32_t (*object_state)(void* object) = nullptr;
    // object->slot20()
    std::uint64_t (*object_kind)(void* object) = nullptr;
    // provider->slot78()
    std::int32_t (*provider_count)(void* provider) = nullptr;
    // provider->slot88(index)
    const char* (*provider_name)(void* provider, std::int32_t index) = nullptr;
    // Terminal hooks; the bodies behind 0xe214f7 and 0xe2167e are out of scope.
    void (*on_missing)(void* provider) = nullptr;
    void (*on_found)(void* provider, std::int32_t index) = nullptr;
};

struct Vtable15e2028Slot2A0_e21420Result {
    bool ok = false;
    Vtable15e2028Slot2A0_e21420Path path = Vtable15e2028Slot2A0_e21420Path::none;
    std::int32_t status = 0;
    std::int32_t found_index = -1;
    std::int32_t scanned = 0;
    bool missing_hook_called = false;
    bool found_hook_called = false;
};

[[nodiscard]] Vtable15e2028Slot2A0_e21420Result vtable_15e2028_slot2a0_e21420(
    const Vtable15e2028Slot2A0_e21420Cells& cells = {},
    const Vtable15e2028Slot2A0_e21420Hooks& hooks = {}) noexcept;

}  // namespace sotc
