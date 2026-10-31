#include "init_531d60_post_base_a54920_u1.hpp"

#include <cstring>

namespace sotc {
namespace {

struct SaveDataDescriptor {
    std::uint8_t zero[0x20]{};
    void* self_ptr = nullptr;
    std::uint64_t gap_28 = 0;
    std::uint64_t field_30 = 0;
    std::uint64_t gap_38 = 0;
    void* vtable = nullptr;
    std::uint64_t field_48 = 0;
};
static_assert(sizeof(SaveDataDescriptor) == 0x50, "descriptor size must match 0x50 B");

}  // namespace

Init531d60PostBaseA54920U1Result init_531d60_post_base_a54920_u1(
    const Init531d60PostBaseA54920U1Hooks& hooks) noexcept {
    Init531d60PostBaseA54920U1Result out{};

    ++out.lea_rip_count;  // lea rax,[0x2dcdf14]
    ++out.lea_rip_count;  // lea rsi,[0x2efd9a0]

    void* parent = const_cast<void*>(hooks.incoming_r13);
    alignas(32) SaveDataDescriptor desc{};
    void* desc_ptr = &desc;

    ++out.lea_rip_count;  // lea rbx,[0x15c8d98]
    desc.field_30 = 0;
    desc.self_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&desc) + 0x20);
    std::memset(desc.zero, 0, sizeof(desc.zero));
    desc.vtable = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseA54920U1Vtable1Rva));
    desc.field_48 = 0;

    if (hooks.boss_flag_2dcdf14 != nullptr) {
        out.boss_flag_before = *hooks.boss_flag_2dcdf14;
        *hooks.boss_flag_2dcdf14 = hooks.incoming_r12b;
        out.boss_flag_after = *hooks.boss_flag_2dcdf14;
    }
    ++out.stamp_count;

    if (hooks.call_a54920 != nullptr) {
        hooks.call_a54920(parent,
                          reinterpret_cast<void*>(
                              static_cast<std::uintptr_t>(kInit531d60PostBaseA54920U1Name1Rva)),
                          desc_ptr);
    }
    ++out.call_a54920_count;

    out.a54920_hook_supplied = (hooks.call_a54920 != nullptr);
    out.ok = (out.call_a54920_count == 1) && (out.stamp_count == 1) &&
             (out.lea_rip_count == 3);
    return out;
}

}  // namespace sotc
