#include "init_531d60_post_base_a54920_532a62_u1.hpp"

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

Init531d60PostBaseA54920_532a62_U1Result init_531d60_post_base_a54920_532a62_u1(
    const Init531d60PostBaseA54920_532a62_U1Hooks& hooks) noexcept {
    Init531d60PostBaseA54920_532a62_U1Result out{};

    void* parent2 = const_cast<void*>(hooks.incoming_r13);

    ++out.lea_rip_count;  // lea rsi,[0x2efd9f8]

    alignas(32) SaveDataDescriptor desc2{};
    void* desc2_ptr = &desc2;

    desc2.field_30 = 0;
    desc2.self_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&desc2) + 0x20);
    std::memset(desc2.zero, 0, sizeof(desc2.zero));
    desc2.vtable = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseA54920_532a62_U1Vtable1Rva));
    desc2.field_48 = 0;

    if (hooks.call_a54920 != nullptr) {
        hooks.call_a54920(parent2,
                          reinterpret_cast<void*>(
                              static_cast<std::uintptr_t>(kInit531d60PostBaseA54920_532a62_U1Name2Rva)),
                          desc2_ptr);
    }
    ++out.call_a54920_count;

    out.a54920_hook_supplied = (hooks.call_a54920 != nullptr);
    out.ok = (out.call_a54920_count == 1) && (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
