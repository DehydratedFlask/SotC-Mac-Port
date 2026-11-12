#include "core/guest_thunks.hpp"

#include <cstring>

namespace sotc {
namespace {

#include "core/guest_thunk_table.inc"

GuestConCmdRegisterHook g_concmd_hook = nullptr;

// 0x20d60: `cmpb $0,0x18(%rdi); je +1; ret` then flag store, manager reload, jmp 0x20d80.
std::uint64_t thunk_concmd_lazy_register(std::uint64_t self, std::uint64_t, std::uint64_t,
                                         std::uint64_t, std::uint64_t, std::uint64_t) {
    if (self == 0) return 0;
    auto* bytes = reinterpret_cast<std::uint8_t*>(self);
    if (bytes[0x18] != 0) return 0;
    bytes[0x18] = 1;
    if (g_concmd_hook != nullptr) g_concmd_hook(reinterpret_cast<void*>(self), nullptr);
    return 0;
}

constexpr GuestThunk kCustomThunks[] = {
    {0x20d60ULL, "concmd_lazy_register", thunk_concmd_lazy_register, 1},
};

}  // namespace

void guest_set_concmd_register_hook(GuestConCmdRegisterHook hook) noexcept {
    g_concmd_hook = hook;
}

const GuestThunk* guest_thunk_find(std::uint64_t rva) noexcept {
    for (const GuestThunk& thunk : kGuestLeafThunks) {
        if (thunk.rva == rva) return &thunk;
    }
    for (const GuestThunk& thunk : kCustomThunks) {
        if (thunk.rva == rva) return &thunk;
    }
    return nullptr;
}

std::size_t guest_thunk_count() noexcept {
    return sizeof(kGuestLeafThunks) / sizeof(kGuestLeafThunks[0]) +
           sizeof(kCustomThunks) / sizeof(kCustomThunks[0]);
}

bool guest_thunk_invoke(std::uint64_t rva, const std::uint64_t* args,
                        std::uint64_t* result) noexcept {
    const GuestThunk* thunk = guest_thunk_find(rva);
    if (thunk == nullptr) return false;
    std::uint64_t a[6] = {0, 0, 0, 0, 0, 0};
    for (std::uint8_t i = 0; i < thunk->arg_count && i < 6; ++i) {
        if (args != nullptr) a[i] = args[i];
    }
    const std::uint64_t value = thunk->fn(a[0], a[1], a[2], a[3], a[4], a[5]);
    if (result != nullptr) *result = value;
    return true;
}

const GuestDispatchSite& guest_dispatch_site() noexcept {
    return kGuestDispatchSite;
}

const std::uint64_t* guest_resolved_targets(std::size_t* count) noexcept {
    if (count != nullptr) *count = sizeof(kGuestResolvedTargets) / sizeof(kGuestResolvedTargets[0]);
    return kGuestResolvedTargets;
}

const std::uint8_t* guest_dispatch_vtable(std::size_t* size) noexcept {
    if (size != nullptr) *size = kGuestDispatchVtableSize;
    return kGuestDispatchVtable;
}

bool guest_dispatch_slot(const std::uint8_t* vtable, std::size_t vtable_size,
                         std::size_t slot_offset, const std::uint64_t* args,
                         std::uint64_t* result, std::uint64_t* target_rva) noexcept {
    if (vtable == nullptr || slot_offset + sizeof(std::uint64_t) > vtable_size) return false;
    std::uint64_t target = 0;
    std::memcpy(&target, vtable + slot_offset, sizeof(target));
    if (target_rva != nullptr) *target_rva = target;
    return guest_thunk_invoke(target, args, result);
}

}  // namespace sotc
