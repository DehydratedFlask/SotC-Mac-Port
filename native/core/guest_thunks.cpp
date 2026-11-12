#include "core/guest_thunks.hpp"

namespace sotc {
namespace {

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

// 0x13d20: `movl %esi,0x1c(%rdi); ret` (4 B leaf).
std::uint64_t thunk_slot_1c_setter(std::uint64_t self, std::uint64_t value, std::uint64_t,
                                   std::uint64_t, std::uint64_t, std::uint64_t) {
    if (self == 0) return 0;
    *reinterpret_cast<std::uint32_t*>(self + 0x1c) = static_cast<std::uint32_t>(value);
    return 0;
}

constexpr GuestThunk kGuestThunks[] = {
    {0x20d60ULL, "concmd_lazy_register", thunk_concmd_lazy_register, 1},
    {0x13d20ULL, "slot_1c_setter", thunk_slot_1c_setter, 2},
};

}  // namespace

void guest_set_concmd_register_hook(GuestConCmdRegisterHook hook) noexcept {
    g_concmd_hook = hook;
}

const GuestThunk* guest_thunk_find(std::uint64_t rva) noexcept {
    for (const GuestThunk& thunk : kGuestThunks) {
        if (thunk.rva == rva) return &thunk;
    }
    return nullptr;
}

std::size_t guest_thunk_count() noexcept {
    return sizeof(kGuestThunks) / sizeof(kGuestThunks[0]);
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

}  // namespace sotc
